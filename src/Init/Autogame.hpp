#ifndef _AUTOGAME_AUTOGAME_H_
#define _AUTOGAME_AUTOGAME_H_

#include "Common.hpp"

#include <map>

// ERROR CODES
#define LIBRARY_ERROR           0
#define LIBRARY_OK              1

#define ERROR_OGAME             0x0100
#define ERROR_LOGIN_ERROR       (ERROR_OGAME | 0x01)
#define ERROR_INCORRECT_LOGIN   (ERROR_OGAME | 0x02)

#define ERROR_EX_UNAUTHORIZED	0x01


namespace autogame
{
    enum STATE
    {
        UNAUTHORIZED,
        AUTHORIZED,
        LOGGED,
    };

    int Session(int session);
    int Init(const char* region, int server);
    int Login(const char* username, const char* password);
    BuildingsMap PlanetBuildings();
    int Update();
}

#endif
