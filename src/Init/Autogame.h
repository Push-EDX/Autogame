
// ERROR CODES
#define LIBRARY_ERROR		-1
#define LIBRARY_OK		1

#define ERROR_HTTP              0x01000
#define ERROR_HTTP_CODE         ERROR_HTTP // | ERROR_CODE

#define ERROR_EX_UNAUTHORIZED	0x01


namespace autogame
{
    enum STATE
    {
        UNAUTHORIZED,
        AUTHORIZED,
        LOGGING,
        LOGGED,
        
    };

    int Init(const char* region, int server);
    int Login(const char* username, const char* password);
}

