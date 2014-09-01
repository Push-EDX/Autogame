#include "Autogame.hpp"

#include <restclient-cpp/restclient.h>
#include <tinyxml2.h>

#include <string>
#include <sstream>
#include <vector>

namespace autogame
{
    // Forward declarations
    void updateOverview();


    // Internal struct
    struct Autogame
    {
    public:
        // Base domains
        std::string gameURL;
        std::string serverURL;
        std::string sessionID;

        // App State
        STATE state;
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
            tinyxml2::XMLError result = doc.Parse(r.body.c_str());

            // Can not parse HTML
            if (result != tinyxml2::XML_NO_ERROR)
            {
                return LIBRARY_ERROR;
            }

            tinyxml2::XMLElement* script = doc.FirstChildElement("script");
            // Can not find <script> tag
            if (!script)
            {
                return LIBRARY_ERROR;
            }

            // Get the URL
            std::string redirect = script->GetText();
            redirect = redirect.substr(redirect.find_first_of("=") + 1, -1);

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

            ao->sessionID = redirect.substr(session + 1);
            /*
            for (RestClient::headermap::iterator it = r.headers.begin(); it != r.headers.end(); ++it)
            {
                printf("%s: %s\n", (*it).first.c_str(), (*it).second.c_str());
            }
            printf("\nBODY: %s\n\n", r.body.c_str());
            */

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
            // Ogame sends the first "<meta" not closed...
            // Ogame does not append an HTML closing tag...
            r.body.replace(r.body.find_first_of(">", r.body.find("meta")), 1, " />", 0, 3);
            r.body.append("</html>");

            tinyxml2::XMLDocument doc;
            tinyxml2::XMLError result = doc.Parse(r.body.c_str());

            // Can not parse HTML
            if (result != tinyxml2::XML_NO_ERROR)
            {
                return LIBRARY_ERROR;
            }

            tinyxml2::XMLElement* meta = doc.FirstChildElement("html")->FirstChildElement("head")->FirstChildElement("meta");
            // Can not find <meta> tag
            if (!meta)
            {
                return LIBRARY_ERROR;
            }

            std::string content(meta->Attribute("content"));
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
            
            for (RestClient::headermap::iterator it = r.headers.begin(); it != r.headers.end(); ++it)
            {
                printf("%s: %s\n", (*it).first.c_str(), (*it).second.c_str());
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

        return LIBRARY_OK;
    }

    void updateOverview()
    {
        std::string url = ao->serverURL + "/game/index.php?page=overview";
        printf("URL: %s\n", url.c_str());

        RestClient::setCookies("language=es;" + ao->sessionID);
        RestClient::response r = RestClient::get(url);
        RestClient::setReferer(url);

        tinyxml2::XMLDocument doc;
        tinyxml2::XMLError result = doc.Parse(r.body.c_str());

        printf("%d: %s\n\n", r.code, r.body.c_str());
    }
}
