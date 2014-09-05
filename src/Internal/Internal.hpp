#ifndef _AUTOGAME_INTERNAL_H_
#define _AUTOGAME_INTERNAL_H_

#include "Autogame.hpp"
#include "Common.hpp"

#include <restclient-cpp/restclient.h>

namespace autogame
{
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

		// App State
		STATE state;

		// Constructor
		Autogame()
		{
			gameURL = std::string();
			serverURL = std::string();
			state = UNAUTHORIZED;
		}
	};

	extern Autogame* ao;
}

#endif
