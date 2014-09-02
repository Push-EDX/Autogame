#include "Autogame.hpp"

#include <restclient-cpp/restclient.h>
#include <htmlcxx/ParserDom.h>

#include <string>
#include <sstream>
#include <vector>
#include <iostream>

namespace htmlcxx
{
    namespace HTML
    {
        struct NodeEx
        {
            bool found;
            Node node;
            std::string text;

            long from;
        };

        std::string getText(tree<Node> dom, tree<Node>::iterator node)
        {
            std::string str = std::string();
            
            int n = node.number_of_children();
            if (n == 0)
                return str;

            tree_node_<Node>* it = node.node->first_child;

            while (n > 0)
            {
                tree<Node> child = it;
                Node node = (*child.begin());

                if (!node.isTag())
                    str.append(node.text());

                it = it->next_sibling;
                n--;
            }

            return str;
        }

        NodeEx findFirst(tree<Node> dom, std::string tagName)
        {
            tree<Node>::iterator it = dom.begin();
            tree<Node>::iterator end = dom.end();

            long i = 0;
            for (; it != end; ++it, ++i)
            {
                if (it->tagName().compare(tagName) == 0)
                {
                    return{ true, *it, getText(dom, it), i };
                }
            }

            return{ false, Node(), std::string(), 0 };
        }

        NodeEx findID(tree<Node> dom, std::string ID)
        {
            tree<Node>::iterator it = dom.begin();
            tree<Node>::iterator end = dom.end();

            long i = 0;
            for (; it != end; ++it, ++i)
            {
                if (it->isTag())
                {
                    it->parseAttributes();
                    Attribute attr = it->attribute("id");
                    
                    if (attr.first)
                    {
                        if (attr.second.compare(ID) == 0)
                        {
                            return { true, *it, getText(dom, it), i };
                        }
                    }
                }
            }

            return{ false, Node(), std::string(), 0 };
        }

        NodeEx findClass(tree<Node> dom, long from, std::string cls)
        {
            tree<Node>::iterator it = dom.begin();
            tree<Node>::iterator end = dom.end();

            std::advance(it, from);

            long i = from;
            for (; it != end; ++it, ++i)
            {
                if (it->isTag())
                {
                    it->parseAttributes();
                    Attribute attr = it->attribute("class");

                    if (attr.first)
                    {
                        if (attr.second.find(cls) == std::string::npos)
                            continue;

                        std::istringstream iss(attr.second);
                        std::string _cls;

                        while (std::getline(iss, _cls, ' '))
                        {
                            if (_cls.compare(cls) == 0)
                            {
                                return{ true, *it, getText(dom, it), i };
                            }
                        }
                    }
                }
            }

            return{ false, Node(), std::string(), 0 };
        }

        NodeEx findClass(tree<Node> dom, std::string cls)
        {
            return findClass(dom, 0, cls);
        }

    }
}

namespace autogame
{
    // Forward declarations
    void updateOverview();
    void updateResources();

    struct Building
    {
        std::string name;
        char level;

        struct Costs
        {
            long long metal;
            long long crystal;
            long long deuterium;
        } costs;
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

        // Building levels
        Building buildings[256];

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
            htmlcxx::HTML::ParserDom parser;
            tree<htmlcxx::HTML::Node> dom = parser.parseTree(r.body.c_str());

            htmlcxx::HTML::NodeEx node = htmlcxx::HTML::findFirst(dom, "script");
            if (!node.found)
                return LIBRARY_ERROR;

            // Get the URL
            std::string redirect = node.text;
            redirect = redirect.substr(redirect.find_first_of('=') + 1, -1);

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
            htmlcxx::HTML::ParserDom parser;
            tree<htmlcxx::HTML::Node> dom = parser.parseTree(r.body.c_str());

            htmlcxx::HTML::NodeEx node = htmlcxx::HTML::findFirst(dom, "meta");
            if (!node.found)
                return LIBRARY_ERROR;
            
            node.node.parseAttributes();
            htmlcxx::HTML::Attribute meta = node.node.attribute("content");
                    
            if (meta.first)
            {
                std::size_t url = meta.second.find("url=");
                if (url != std::string::npos)
                {
                    // Login data is incorrect
                    std::string content = meta.second.substr(url + 4);
                    if (content.substr(content.find_last_of('/')).compare("/loginError") == 0)
                    {
                        return ERROR_LOGIN_ERROR;
                    }

                    return LoginEx(content);
                }
            }
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

    void updateOverview()
    {
        std::string url = ao->serverURL + "/game/index.php?page=overview";

        RestClient::setCookies(ao->getCookies());
        RestClient::response r = RestClient::get(url);
        RestClient::setReferer(url);

        htmlcxx::HTML::ParserDom parser;
        tree<htmlcxx::HTML::Node> dom = parser.parseTree(r.body.c_str());
    }

    bool updateBuilding(tree<htmlcxx::HTML::Node> dom, long from, int building)
    {
        htmlcxx::HTML::NodeEx node = htmlcxx::HTML::findClass(dom, from, std::string("supply").append(std::to_string(building)));
        if (node.found)
        {
            node = htmlcxx::HTML::findClass(dom, node.from, "level");
            if (node.found)
            {
                //ao->buildings[building].level = atoi(RestClient::trim(node.text).c_str());
                return true;
            }
        }

        return false;
    }

    void updateResources()
    {
        std::string url = ao->serverURL + "/game/index.php?page=resources";

        RestClient::setCookies(ao->getCookies());
        RestClient::response r = RestClient::get(url);
        RestClient::setReferer(url);

        htmlcxx::HTML::ParserDom parser;
        tree<htmlcxx::HTML::Node> dom = parser.parseTree(r.body.c_str());

        htmlcxx::HTML::NodeEx content = htmlcxx::HTML::findClass(dom, "content");

        for (int i = 1; i <= 12; ++i)
        {
            updateBuilding(dom, content.from, i);
            printf("Supply%d: %d\n", i, ao->buildings[i].level);
        }
    }
}
