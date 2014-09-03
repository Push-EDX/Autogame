#include "Autogame.hpp"
#include "Buildings.hpp"
#include "Utils.hpp"

#include <restclient-cpp/restclient.h>
#include <tinyxml2.h>
#include <tidy.h>
#include <buffio.h>
#include <rapidjson/document.h>

#include <string>
#include <sstream>
#include <vector>
#include <iostream>


#define SAFE_CHILD(x,y) if (!x) return LIBRARY_ERROR; else x = x->FirstChildElement(y)
#define SAFE_SIBLING(x,y) if (!x) return LIBRARY_ERROR; else x = x->NextSiblingElement(y)

namespace autogame
{
    // Forward declarations
    int updateOverview();
    int updateSupplies();
    int updateResources();
    int updateAvailability();
    int updateStation();
    int updateResearch();

    // Internal struct
    struct Autogame
    {
    public:
        // Base domains
        std::string gameURL;
        std::string serverURL;

        // Cookies
        RestClient::cookiesmap cookies;
        std::string getCookies()
        {
            std::string cookiesStr = std::string();

            for (RestClient::cookiesmap::iterator it = cookies.begin(); it != cookies.end(); ++it)
                cookiesStr.append(it->first).append("=").append(it->second.value).append(";");

            return cookiesStr;
        }

        // Planets
        PlanetsVector planets;
        Planet* currentPlanet;

        // App State
        STATE state;

        // Constructor
        Autogame()
        {
            gameURL = std::string();
            serverURL = std::string();
            currentPlanet = NULL;
            state = UNAUTHORIZED;
        }
    } *ao;

    // Sessions holder
    std::vector<Autogame*> sessions;

    int Init(const char* region, int server)
    {
        ao = new Autogame();
        sessions.push_back(ao);

        char buffer[64] = {0};

        sprintf(buffer, "%s.ogame.gameforge.com\0", region);
        ao->gameURL = std::string(buffer);

        sprintf(buffer, "s%i-%s.ogame.gameforge.com\0", server, region);
        ao->serverURL = std::string(buffer);

        return LIBRARY_OK;
    }

    int Session(int session)
    {
        if (session > sessions.size())
        {
            return LIBRARY_ERROR;
        }

        ao = sessions[session];
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

    BuildingsMap PlanetBuildings()
    {
        return ao->currentPlanet->buildings;
    }

    int Update()
    {
        if (ao->state < LOGGED)
        {
            return LIBRARY_ERROR;
        }

        // Check overview
        updateOverview();

        if (ao->currentPlanet)
        {
            updateResources();
            updateSupplies();
            updateStation();
            updateResearch();
            updateAvailability();
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

        ao->currentPlanet = planet;
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

                ao->currentPlanet->coordinates.galaxy = cord[0];
                ao->currentPlanet->coordinates.system = cord[1];
                ao->currentPlanet->coordinates.position = cord[2];
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

        printf("PLANET: %s [%d - %d]\n", planet->name.c_str(), planet->minTemp, planet->maxTemp);

        return LIBRARY_OK;
    }

    int updateResources()
    {
        std::string url = ao->serverURL + "/game/index.php?page=fetchResources&ajax=1";

        RestClient::setCookies(ao->getCookies());
        RestClient::response r = RestClient::get(url);
        RestClient::setReferer(url);

        if (r.code != 200)
            return LIBRARY_ERROR;

        rapidjson::Document doc;
        doc.Parse(r.body.c_str());

        if (doc.HasMember("metal"))
        {
            if (doc["metal"].HasMember("resources"))
            {
                if (doc["metal"]["resources"].HasMember("actual"))
                {
                    ao->currentPlanet->resources.metal = doc["metal"]["resources"]["actual"].GetInt();
                }
            }
        }

        if (doc.HasMember("crystal"))
        {
            if (doc["crystal"].HasMember("resources"))
            {
                if (doc["crystal"]["resources"].HasMember("actual"))
                {
                    ao->currentPlanet->resources.crystal = doc["crystal"]["resources"]["actual"].GetInt();
                }
            }
        }

        if (doc.HasMember("deuterium"))
        {
            if (doc["deuterium"].HasMember("resources"))
            {
                if (doc["deuterium"]["resources"].HasMember("actual"))
                {
                    ao->currentPlanet->resources.deuterium = doc["deuterium"]["resources"]["actual"].GetInt();
                }
            }
        }

        printf("RESOURCES: %.1f %.1f %.1f\n", ao->currentPlanet->resources.metal, ao->currentPlanet->resources.crystal, ao->currentPlanet->resources.deuterium);

        return LIBRARY_OK;
    }

    bool updateBuilding(tinyxml2::XMLElement* content, int building, std::string type)
    {
        std::string cls = type.append(std::to_string(building));
        tinyxml2::XMLElement* supply = findByClass(content, cls);

        if (supply)
        {
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
                    ao->currentPlanet->buildings[building].level = level;
                    ao->currentPlanet->buildings[building].costs = COST(building, level);
                    ao->currentPlanet->buildings[building].production = PRODUCTION(building, level, ao->currentPlanet->maxTemp);

                    tinyxml2::XMLElement* time = findByClass(supply, "time");
                    if (time)
                    {
                        ao->currentPlanet->buildings[building].upgrading = true;
                        ao->currentPlanet->buildings[building].timeTotal = -1;
                        ao->currentPlanet->buildings[building].timeLeft = -1;
                    }
                    printf("BUILDING %d [%d]: %d\t[%.1f %.1f %.1f]\t[%.1f %.1f %.1f]\n", building, ao->currentPlanet->buildings[building].upgrading, level, ao->currentPlanet->buildings[building].costs.metal, ao->currentPlanet->buildings[building].costs.crystal, ao->currentPlanet->buildings[building].costs.deuterium, ao->currentPlanet->buildings[building].production.metal, ao->currentPlanet->buildings[building].production.crystal, ao->currentPlanet->buildings[building].production.deuterium);

                    return true;
                }
            }
        }

        return false;
    }

    bool updateTiming(std::string body, std::string type)
    {
        for (BuildingsMap::iterator it = ao->currentPlanet->buildings.begin(); it != ao->currentPlanet->buildings.end(); ++it)
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

    int updateSupplies()
    {
        std::string url = ao->serverURL + "/game/index.php?page=resources";

        RestClient::setCookies(ao->getCookies());
        RestClient::response r = RestClient::get(url);
        RestClient::setReferer(url);

        if (r.code != 200)
            return LIBRARY_ERROR;

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
            updateBuilding(content, METAL_FACTORY, "supply");
            updateBuilding(content, CRYSTAL_FACTORY, "supply");
            updateBuilding(content, DEUTERIUM_FACTORY, "supply");
            updateBuilding(content, SOLAR_PLANT, "supply");
            updateBuilding(content, FUSION_PLANT, "supply");
            updateBuilding(content, SOLAR_SATELLITE, "supply");
            updateBuilding(content, METAL_STORAGE, "supply");
            updateBuilding(content, CRYSTAL_STORAGE, "supply");
            updateBuilding(content, DEUTERIUM_STORAGE, "supply");
            updateBuilding(content, METAL_HIDEOUT, "supply");
            updateBuilding(content, CRYSTAL_HIDEOUT, "supply");
            updateBuilding(content, DEUTERIUM_HIDEOUT, "supply");
            updateTiming(r.body, "supply");
        }

        return LIBRARY_OK;
    }

    int updateStation()
    {
        std::string url = ao->serverURL + "/game/index.php?page=station";

        RestClient::setCookies(ao->getCookies());
        RestClient::response r = RestClient::get(url);
        RestClient::setReferer(url);

        if (r.code != 200)
            return LIBRARY_ERROR;

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
            updateBuilding(content, ROBOTS_FACTORY, "station");
            updateBuilding(content, SHIPYARD, "station");
            updateBuilding(content, LABORATORY, "station");
            updateBuilding(content, ALLIANCE_DEPOT, "station");
            updateBuilding(content, SILO, "station");
            updateBuilding(content, NANITE_FACTORY, "station");
            updateBuilding(content, TERRAFORMER, "station");
            updateTiming(r.body, "station");
        }

        return LIBRARY_OK;
    }

    int updateResearch()
    {
        std::string url = ao->serverURL + "/game/index.php?page=research";

        RestClient::setCookies(ao->getCookies());
        RestClient::response r = RestClient::get(url);
        RestClient::setReferer(url);

        if (r.code != 200)
            return LIBRARY_ERROR;

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
            updateBuilding(content, ENERGY_TECH, "research");
            updateBuilding(content, LASER_TECH, "research");
            updateBuilding(content, ION_TECH, "research");
            updateBuilding(content, HYPERSPACE_TECH, "research");
            updateBuilding(content, PLASMA_TECH, "research");
            updateBuilding(content, COMBUSTION_TECH, "research");
            updateBuilding(content, IMPULSE_TECH, "research");
            updateBuilding(content, HYPERSPACE_DRIVE_TECH, "research");
            updateBuilding(content, SPY_TECH, "research");
            updateBuilding(content, COMPUTATIONAL_TECH, "research");
            updateBuilding(content, ASTROPHYSICS_TECH, "research");
            updateBuilding(content, INTERGALAXY_TECH, "research");
            updateBuilding(content, GRAVITATIONAL_TECH, "research");
            updateBuilding(content, MILITAR_TECH, "research");
            updateBuilding(content, DEFENSE_TECH, "research");
            updateBuilding(content, BLINDAGE_TECH, "research");
            updateTiming(r.body, "research");
        }

        return LIBRARY_OK;
    }

    int updateAvailability()
    {
        for (BuildingsMap::iterator it = ao->currentPlanet->buildings.begin(); it != ao->currentPlanet->buildings.end(); ++it)
        {
            int supplyID = it->first;
            Building& building = it->second;

            building.available = CAN_BUILD(ao->currentPlanet->buildings, supplyID);
        }

        return LIBRARY_OK;
    }
}
