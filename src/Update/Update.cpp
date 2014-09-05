#include "Autogame.hpp"
#include "Buildings.hpp"
#include "Utils.hpp"
#include "Internal.hpp"

#include <restclient-cpp/restclient.h>
#include <tinyxml2.h>
#include <tidy.h>
#include <buffio.h>
#include <rapidjson/document.h>

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <time.h>


#define SAFE_CHILD(x,y) if (!x) return LIBRARY_ERROR; else x = x->FirstChildElement(y)
#define SAFE_SIBLING(x,y) if (!x) return LIBRARY_ERROR; else x = x->NextSiblingElement(y)

namespace autogame
{
    // Forward declarations
    int updateOverview();
    int updateSupplies(Planet* planet);
	int updateResources(Planet* planet);
	int updateStation(Planet* planet);
	int updateResearch(Planet* planet);

    // Internal struct
    Autogame *ao = NULL;

    // Sessions holder
    std::vector<Autogame> sessions;

    int Init(const char* region, int server)
    {
		if (!ao)
		{
			return LIBRARY_ERROR;
		}

        char buffer[64] = {0};

        sprintf(buffer, "%s.ogame.gameforge.com\0", region);
        ao->gameURL = std::string(buffer);

        sprintf(buffer, "s%i-%s.ogame.gameforge.com\0", server, region);
        ao->serverURL = std::string(buffer);

        return LIBRARY_OK;
    }

    int Session(int session)
	{
		if (session == sessions.size())
		{
			sessions.push_back(Autogame());
		}
        else if (session > sessions.size())
        {
            return LIBRARY_ERROR;
        }

        ao = &sessions[session];

        return LIBRARY_OK;
    }

    // This login is done SERVER side
    int LoginEx(std::string url)
    {
        RestClient::response r = RestClient::get(url);
        RestClient::setReferer(url);

        if (r.code == 302)
        {
            tinyxml2::XMLDocument doc;
            tinyxml2::XMLError result = doc.Parse(html_xml(r.body).c_str());

            // Can not parse HTML
            if (result != tinyxml2::XML_NO_ERROR)
            {
                return LIBRARY_ERROR;
            }

            tinyxml2::XMLElement* script = doc.FirstChildElement("html");
            SAFE_CHILD(script, "head");
            SAFE_CHILD(script, "script");

            // Can not find <script> tag
            if (!script)
            {
                return LIBRARY_ERROR;
            }

            // Get the URL
            std::string redirect = script->GetText();
            std::size_t seperator = redirect.find_first_of('=');
            if (seperator == std::string::npos)
                return LIBRARY_ERROR;

            // Get the URL without quotes
            redirect = redirect.substr(seperator + 2, redirect.length() - seperator - 4);

            // Incorrect username/password
            if (redirect.find("/loginError") != std::string::npos)
            {
                return ERROR_INCORRECT_LOGIN;
            }

            std::size_t session = redirect.find("&PHPSESSID");
            if (session == std::string::npos)
            {
                return LIBRARY_ERROR;
            }

            for (RestClient::cookiesmap::iterator it = r.cookies.begin(); it != r.cookies.end(); ++it)
            {
                ao->cookies[it->first] = it->second;
            }

            ao->state = LOGGED;
            return LIBRARY_OK;
        }

        return LIBRARY_ERROR;
    }

    // This login is done GAME side
    int Login(const char* username, const char* password)
    {
		if (!ao)
		{
			return LIBRARY_ERROR;
		}

        //char* body = new char[strlen(username) + strlen(password) + 18]; // 5(login) 4(pass) 3(kid) 2(&) 3(=) 1(\0)
        char body[512] = { 0 };
        sprintf(body, "uni=%s&kid=&login=%s&pass=%s\0", ao->serverURL.c_str(), username, password);

        std::string url = ao->gameURL + std::string("/main/login");
        RestClient::response r = RestClient::post(url, "application/x-www-form-urlencoded", body);
        RestClient::setReferer(url);

        // Code: 303 redirect
        // Response: /loginError
        // OK:       serverURL/...
        if (r.code == 303)
        {
            tinyxml2::XMLDocument doc;
            tinyxml2::XMLError result = doc.Parse(html_xml(r.body).c_str());

            // Can not parse HTML
            if (result != tinyxml2::XML_NO_ERROR)
            {
                return LIBRARY_ERROR;
            }

            // Skip the first meta and get the one we want
            tinyxml2::XMLElement* meta = doc.FirstChildElement("html");
            SAFE_CHILD(meta, "head");
            SAFE_CHILD(meta, "meta");
            SAFE_SIBLING(meta, "meta");

            // Can not find <meta> tag
            if (!meta)
            {
                return LIBRARY_ERROR;
            }

            const char* attr = meta->Attribute("content");
            if (!attr)
            {
                return LIBRARY_ERROR;
            }

            std::string content(attr);
            std::size_t url = content.find("url=");
            if (url == std::string::npos)
            {
                return LIBRARY_ERROR;
            }

            // Login data is incorrect
            content = content.substr(url + 4);
            if (content.substr(content.find_last_of('/')).compare("/loginError") == 0)
            {
                return ERROR_LOGIN_ERROR;
            }

            return LoginEx(content);
        }

        return LIBRARY_ERROR;
    }

    int Update()
	{
		if (!ao)
		{
			return LIBRARY_ERROR;
		}

        if (ao->state < LOGGED)
        {
            return LIBRARY_ERROR;
        }

        // Check overview
        updateOverview();

		for (PlanetsVector::iterator it = ao->planets.begin(); it != ao->planets.end(); ++it)
        {
            updateResources(&*it);
			updateSupplies(&*it);
			updateStation(&*it);
			updateResearch(&*it);
        }

        return LIBRARY_OK;
    }

    int updateOverview()
    {
        std::string url = ao->serverURL + "/game/index.php?page=overview";

        RestClient::setCookies(ao->getCookies());
        RestClient::response r = RestClient::get(url);
        RestClient::setReferer(url);

        tinyxml2::XMLDocument doc;
        tinyxml2::XMLError result = doc.Parse(html_xml(r.body).c_str());

        // Can not parse HTML
        if (result != tinyxml2::XML_NO_ERROR)
        {
            return LIBRARY_ERROR;
        }

        // Get document
        tinyxml2::XMLElement* body = doc.FirstChildElement("html");

        // Get planet
        Planet* planet = NULL;

        if (body)
        {
            tinyxml2::XMLElement* nameElement = findByName(body, "ogame-planet-name");
            if (nameElement)
            {
                if (nameElement->Attribute("content"))
                {
                    std::string name(nameElement->Attribute("content"));

                    for (std::vector<Planet>::iterator it = ao->planets.begin(); it != ao->planets.end(); ++it)
                    {
                        if (it->name.compare(name) == 0)
                        {
                            planet = &*it;
                            break;
                        }
                    }

                    if (!planet)
                    {
                        planet = &*ao->planets.insert(ao->planets.begin(), Planet(name));
                    }
                }
            }
        }

        if (!planet ||!body)
            return LIBRARY_ERROR;

        // Get planet coordinates
        tinyxml2::XMLElement* coordinatesElement = findByName(body, "ogame-planet-coordinates");
        if (coordinatesElement)
        {
            if (coordinatesElement->Attribute("content"))
            {
                std::string coordinates(coordinatesElement->Attribute("content"));

                int cord[3] = { 0 };
                int n = 0;
                size_t pos = 0;
                std::string token;
                while ((pos = coordinates.find(":")) != std::string::npos) {
                    token = coordinates.substr(0, pos);
                    coordinates.erase(0, pos + 1/*delimiter.length()*/);
                    cord[n++] = atoi(token.c_str());
                }
                cord[2] = atoi(coordinates.c_str());

                planet->coordinates.galaxy = cord[0];
                planet->coordinates.system = cord[1];
                planet->coordinates.position = cord[2];
            }
        }

        // Get planet temperature
        tinyxml2::XMLElement* temperature = findByClass(body, "planetlink");
        if (temperature)
        {
            if (temperature->Attribute("title"))
            {
                std::string coordinates(temperature->Attribute("title"));

                int n = 0;
                int last = coordinates.length();
                size_t pos = 0;
                std::string token;
                while ((pos = coordinates.rfind(" ")) != std::string::npos) {
                    token = coordinates.substr(pos+1, last);
                    coordinates.erase(pos, last);
                    last = pos;

                    if (is_number(token))
                    {
                        if (n == 0)
                            planet->maxTemp = atoi(token.c_str());
                        else if (n == 1)
                            planet->minTemp = atoi(token.c_str());
                        ++n;
                    }
                }
            }
        }

        //printf("PLANET: %s [%d - %d]\n", planet->name.c_str(), planet->minTemp, planet->maxTemp);

        return LIBRARY_OK;
    }

	int updateResources(Planet* planet)
    {
        std::string url = ao->serverURL + "/game/index.php?page=fetchResources&ajax=1";

        RestClient::setCookies(ao->getCookies());
        RestClient::response r = RestClient::get(url);
        RestClient::setReferer(url);

        if (r.code != 200)
            return LIBRARY_ERROR;

        rapidjson::Document doc;
        doc.Parse(r.body.c_str());

		// Get current time
		planet->timestamp = time(0);

		// Save metal
        if (doc.HasMember("metal"))
        {
            if (doc["metal"].HasMember("resources"))
            {
                if (doc["metal"]["resources"].HasMember("actual"))
                {
                    planet->resources.metal = doc["metal"]["resources"]["actual"].GetInt();
                }
            }
        }

		// Save crystal
        if (doc.HasMember("crystal"))
        {
            if (doc["crystal"].HasMember("resources"))
            {
                if (doc["crystal"]["resources"].HasMember("actual"))
                {
                    planet->resources.crystal = doc["crystal"]["resources"]["actual"].GetInt();
                }
            }
        }

		// Save deuterium
        if (doc.HasMember("deuterium"))
        {
            if (doc["deuterium"].HasMember("resources"))
            {
                if (doc["deuterium"]["resources"].HasMember("actual"))
                {
                    planet->resources.deuterium = doc["deuterium"]["resources"]["actual"].GetInt();
                }
            }
        }

		// Update timestamp
		planet->resources.lastMetalUpdate = planet->timestamp;
		planet->resources.lastCrystalUpdate = planet->timestamp;
		planet->resources.lastDeuteriumUpdate = planet->timestamp;

        //printf("RESOURCES: %.1f %.1f %.1f\n", planet->resources.metal, planet->resources.crystal, planet->resources.deuterium);

        return LIBRARY_OK;
    }

    bool updateBuilding(Planet* planet, tinyxml2::XMLElement* content, int building, std::string type)
    {
        std::string cls = type.append(std::to_string(building));
        tinyxml2::XMLElement* supply = findByClass(content, cls);

        if (supply)
        {
			planet->buildings[building].identifier = building;
			planet->buildings[building].timestamp = planet->timestamp;

            tinyxml2::XMLElement* level = findByClass(supply, "level");
            if (level)
            {
                // Delete the inner span (if any)
                if (level->FirstChildElement())
                {
                    level->DeleteChild(level->FirstChildElement());
                }

                // If we have level
                if (level->GetText())
                {
                    std::string levelStr = level->GetText();
                    int level = atoi(RestClient::trim(levelStr).c_str());
                    planet->buildings[building].level = level;
                    planet->buildings[building].costs = COST(building, level);
                    planet->buildings[building].production = PRODUCTION(building, level, planet->maxTemp);

                    tinyxml2::XMLElement* time = findByClass(supply, "time");
                    if (time)
                    {
                        planet->buildings[building].upgrading = true;
                        planet->buildings[building].timeTotal = -1;
                        planet->buildings[building].timeLeft = -1;
                    }
                    //printf("BUILDING %d [%d]: %d\t[%.1f %.1f %.1f]\t[%.1f %.1f %.1f]\n", building, planet->buildings[building].upgrading, level, planet->buildings[building].costs.metal, planet->buildings[building].costs.crystal, planet->buildings[building].costs.deuterium, planet->buildings[building].production.metal, planet->buildings[building].production.crystal, planet->buildings[building].production.deuterium);

                    return true;
                }
            }
        }

        return false;
    }

    bool updateTiming(Planet* planet, std::string body, std::string type)
    {
        for (BuildingsMap::iterator it = planet->buildings.begin(); it != planet->buildings.end(); ++it)
        {
            int supplyID = it->first;
            Building& building = it->second;

            if (building.upgrading)
            {
                std::string ID = std::string("b_").append(type).append(std::to_string(supplyID));
                std::size_t seperator = body.find(ID);

                if (seperator != std::string::npos)
                {
                    seperator = body.find(ID, seperator + 1);
                    if (seperator != std::string::npos)
                    {
                        // Let's find commas
                        std::size_t firstComma = body.find_first_of(',', seperator + 1);
                        std::size_t secondComma = body.find_first_of(',', firstComma + 1);
                        std::size_t thirdComma = body.find_first_of(',', secondComma + 1);

                        std::string remaining = body.substr(firstComma + 1, secondComma - firstComma - 1);
                        std::string total = body.substr(secondComma + 1, thirdComma - secondComma - 1);

                        building.timeLeft = atoi(remaining.c_str());
                        building.timeTotal = atoi(total.c_str());
                    }
                }
            }
        }

        return true;
    }

	int updateSupplies(Planet* planet)
    {
        std::string url = ao->serverURL + "/game/index.php?page=resources";

        RestClient::setCookies(ao->getCookies());
        RestClient::response r = RestClient::get(url);
        RestClient::setReferer(url);

        if (r.code != 200)
            return LIBRARY_ERROR;

		// Update timestamp
		planet->timestamp = time(0);

		// Parse document
        tinyxml2::XMLDocument doc;
        tinyxml2::XMLError result = doc.Parse(html_xml(r.body).c_str());
        
        // Can not parse HTML
        if (result != tinyxml2::XML_NO_ERROR)
        {
            return LIBRARY_ERROR;
        }

        // Skip the first meta and get the one we want
        tinyxml2::XMLElement* body = doc.FirstChildElement("html");
        SAFE_CHILD(body, "body");
        
        tinyxml2::XMLElement* content = findByID(body, "content");
        if (content)
        {
            updateBuilding(planet, content, METAL_FACTORY, "supply");
            updateBuilding(planet, content, CRYSTAL_FACTORY, "supply");
            updateBuilding(planet, content, DEUTERIUM_FACTORY, "supply");
            updateBuilding(planet, content, SOLAR_PLANT, "supply");
            updateBuilding(planet, content, FUSION_PLANT, "supply");
            updateBuilding(planet, content, SOLAR_SATELLITE, "supply");
            updateBuilding(planet, content, METAL_STORAGE, "supply");
            updateBuilding(planet, content, CRYSTAL_STORAGE, "supply");
            updateBuilding(planet, content, DEUTERIUM_STORAGE, "supply");
            updateBuilding(planet, content, METAL_HIDEOUT, "supply");
            updateBuilding(planet, content, CRYSTAL_HIDEOUT, "supply");
            updateBuilding(planet, content, DEUTERIUM_HIDEOUT, "supply");
            updateTiming(planet, r.body, "supply");
        }

        return LIBRARY_OK;
    }

	int updateStation(Planet* planet)
    {
        std::string url = ao->serverURL + "/game/index.php?page=station";

        RestClient::setCookies(ao->getCookies());
        RestClient::response r = RestClient::get(url);
        RestClient::setReferer(url);

        if (r.code != 200)
			return LIBRARY_ERROR;

		// Update timestamp
		planet->timestamp = time(0);

		// Parse document
        tinyxml2::XMLDocument doc;
        tinyxml2::XMLError result = doc.Parse(html_xml(r.body).c_str());

        // Can not parse HTML
        if (result != tinyxml2::XML_NO_ERROR)
        {
            return LIBRARY_ERROR;
        }

        // Skip the first meta and get the one we want
        tinyxml2::XMLElement* body = doc.FirstChildElement("html");
        SAFE_CHILD(body, "body");

        tinyxml2::XMLElement* content = findByID(body, "content");
        if (content)
        {
            updateBuilding(planet, content, ROBOTS_FACTORY, "station");
            updateBuilding(planet, content, SHIPYARD, "station");
            updateBuilding(planet, content, LABORATORY, "station");
            updateBuilding(planet, content, ALLIANCE_DEPOT, "station");
            updateBuilding(planet, content, SILO, "station");
            updateBuilding(planet, content, NANITE_FACTORY, "station");
            updateBuilding(planet, content, TERRAFORMER, "station");
            updateTiming(planet, r.body, "station");
        }

        return LIBRARY_OK;
    }

	int updateResearch(Planet* planet)
    {
        std::string url = ao->serverURL + "/game/index.php?page=research";

        RestClient::setCookies(ao->getCookies());
        RestClient::response r = RestClient::get(url);
        RestClient::setReferer(url);

        if (r.code != 200)
			return LIBRARY_ERROR;

		// Update timestamp
		planet->timestamp = time(0);

		// Parse document
        tinyxml2::XMLDocument doc;
        tinyxml2::XMLError result = doc.Parse(html_xml(r.body).c_str());

        // Can not parse HTML
        if (result != tinyxml2::XML_NO_ERROR)
        {
            return LIBRARY_ERROR;
        }

        // Skip the first meta and get the one we want
        tinyxml2::XMLElement* body = doc.FirstChildElement("html");
        SAFE_CHILD(body, "body");

        tinyxml2::XMLElement* content = findByID(body, "content");
        if (content)
        {
            updateBuilding(planet, content, ENERGY_TECH, "research");
            updateBuilding(planet, content, LASER_TECH, "research");
            updateBuilding(planet, content, ION_TECH, "research");
            updateBuilding(planet, content, HYPERSPACE_TECH, "research");
            updateBuilding(planet, content, PLASMA_TECH, "research");
            updateBuilding(planet, content, COMBUSTION_TECH, "research");
            updateBuilding(planet, content, IMPULSE_TECH, "research");
            updateBuilding(planet, content, HYPERSPACE_DRIVE_TECH, "research");
            updateBuilding(planet, content, SPY_TECH, "research");
            updateBuilding(planet, content, COMPUTATIONAL_TECH, "research");
            updateBuilding(planet, content, ASTROPHYSICS_TECH, "research");
            updateBuilding(planet, content, INTERGALAXY_TECH, "research");
            updateBuilding(planet, content, GRAVITATIONAL_TECH, "research");
            updateBuilding(planet, content, MILITAR_TECH, "research");
            updateBuilding(planet, content, DEFENSE_TECH, "research");
            updateBuilding(planet, content, BLINDAGE_TECH, "research");
            updateTiming(planet, r.body, "research");
        }

        return LIBRARY_OK;
    }
}
