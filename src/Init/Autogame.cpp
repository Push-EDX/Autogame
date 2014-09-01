#include "Autogame.h"

#include <happyhttp.h>
#include <tinyxml2.h>
#include <string>
#include <sstream>

namespace autogame
{

    struct Autogame
    {
    public:
        // Char[23 (22 + 1)]
        char gameURL[23];

	    // Char[28 (27 + 1)] (2 state code)
        char serverURL[28];

        // Connection
        happyhttp::Connection* conn;

        // HTTP Buffer
        std::stringstream buffer;

        // APP State
        STATE state;

        bool pending;
        bool error;
	    int lastError;
    } ao;
    
    void OnError(int e)
    {
        if (ao.state > LOGGED)
        {
            ao.state = LOGGED;
        }
        
        ao.error = true;
        ao.lastError = e;
    }

    void OnBegin(const happyhttp::Response* r, void* userdata)
    {
        ao.buffer.clear();
        int status = r->getstatus();
        
        if (!(status >= 200 && status < 400))
        {
            OnError(ERROR_HTTP_CODE | status);
        }
    }

    void OnData(const happyhttp::Response* r, void* userdata, const unsigned char* data, int n)
    {
        ao.buffer.write(data, n);
    }

    void OnComplete(const happyhttp::Response* r, void* userdata)
    {
        ao.pending = false;
        
        if (!ao.error)
        {
            switch (ao.state)
            {
                case LOGGING:
                {
                    
                    break;
                }
            }
        }
    }

    bool POST(const char* path, const char* body)
    {
        ao.pending = false;

	    const char* headers[] = 
	    {
		    //"Connection", "close",
		    "Content-type", "application/x-www-form-urlencoded",
		    "Accept", "text/plain",
		    0
	    };

        try
        {
            ao.conn->setcallbacks(OnBegin, OnData, OnComplete, 0);
            ao.conn->request("POST", path, headers, (const unsigned char*)body, strlen(body));
        }
        catch (happyhttp::Wobbly& e)
        {
            OnError(0);
            return false;
        }

        ao.pending = true;
        return true;
    }

    int Init(const char* region, int server)
    {
        sprintf(ao.gameURL, "%s.ogame.gameforge.com\0", region);
        sprintf(ao.serverURL, "s%i-%s.ogame.gameforge.com\0", server, region);
    }

    int Login(const char* username, const char* password)
    {
        char* body = new char[strlen(username) + strlen(password) + 18]; // 5(login) 4(pass) 3(kid) 2(&) 3(=) 1(\0)
        sprintf(body, "kid=&login=%s&pass=%s\0", username, password);

        if (!ao.conn)
        {
            ao.conn = new happyhttp::Connection(ao.gameURL, 80);
        }

        bool result = POST("/main/login", body);
        delete[] body;
        
        if (result)
        {
            ao.state = LOGGING;
            return LIBRARY_OK;
        }

        return LIBRARY_ERROR;
    }

    int Update()
    {
        if (ao.pending && ao.conn)
        {
            if (ao.conn->outstanding())
            {
                ao.conn->pump();
                return LIBRARY_OK;
            }
        }

        return LIBRARY_ERROR;
    }
}

