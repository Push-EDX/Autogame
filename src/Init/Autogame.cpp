#include "Autogame.hpp"
#include <Building.hpp>
#include <Utils.hpp>

#include <restclient-cpp/restclient.h>
#include <tinyxml2.h>
#include <tidy.h>
#include <buffio.h>

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
    int updateResources();

    struct Planet
    {
        // Name
        std::string name;

        // Min-Max temp
        int minTemp;
        int maxTemp;

        struct Coordinates
        {
            int galaxy;
            int system;
            int position;
        } coordinates;

        // Building levels
        std::map<int, Building> buildings;

        Planet(std::string nm)
        {
            name = nm;
            minTemp = 0;
            maxTemp = 0;
        }
    };

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
        std::vector<Planet> planets;
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

    int Update()
    {
        if (ao->state < LOGGED)
        {
            return LIBRARY_ERROR;
        }

        // Check overview
        updateOverview();
        updateResources();

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

        if (planet)
        {
            ao->currentPlanet = planet;

            if (body)
            {
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
            }

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
        }

        return LIBRARY_OK;
    }

    bool updateBuilding(tinyxml2::XMLElement* content, int building)
    {
        std::string cls = std::string("supply").append(std::to_string(building));
        tinyxml2::XMLElement* supply = findByClass(content, cls);

        if (supply)
        {
            supply = findByClass(supply, "level");
            if (supply)
            {
                // Delete the inner span (if any)
                if (supply->FirstChildElement())
                {
                    supply->DeleteChild(supply->FirstChildElement());
                }

                if (supply->GetText())
                {
                    std::string levelStr = supply->GetText();
                    int level = atoi(RestClient::trim(levelStr).c_str());
                    ao->currentPlanet->buildings[building].level = level;
                    ao->currentPlanet->buildings[building].costs = COST(building, level);
                    ao->currentPlanet->buildings[building].production = PRODUCTION(building, level, ao->currentPlanet->maxTemp);

                    printf("BUILDING %d: %d\t[%.1f %.1f %.1f]\t[%.1f %.1f %.1f]\n", building, level, ao->currentPlanet->buildings[building].costs.metal, ao->currentPlanet->buildings[building].costs.crystal, ao->currentPlanet->buildings[building].costs.deuterium, ao->currentPlanet->buildings[building].production.metal, ao->currentPlanet->buildings[building].production.crystal, ao->currentPlanet->buildings[building].production.deuterium);

                    return true;
                }
            }
        }

        return false;
    }

    int updateResources()
    {
        if (!ao->currentPlanet)
            return LIBRARY_ERROR;

        std::string url = ao->serverURL + "/game/index.php?page=resources";

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

        // Skip the first meta and get the one we want
        tinyxml2::XMLElement* body = doc.FirstChildElement("html");
        SAFE_CHILD(body, "body");
        
        tinyxml2::XMLElement* content = findByID(body, "content");
        if (content)
        {
            updateBuilding(content, METAL_FACTORY);
            updateBuilding(content, CRYSTAL_FACTORY);
            updateBuilding(content, DEUTERIUM_FACTORY);
            updateBuilding(content, SOLAR_PANT);
            updateBuilding(content, SOLAR_SATELLITE);
            updateBuilding(content, METAL_STORAGE);
            updateBuilding(content, CRYSTAL_STORAGE);
            updateBuilding(content, DEUTERIUM_STORAGE);
            updateBuilding(content, METAL_HIDEOUT);
            updateBuilding(content, CRYSTAL_HIDEOUT);
            updateBuilding(content, DEUTERIUM_HIDEOUT);
        }
    }
}
