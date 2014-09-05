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

namespace autogame
{
	int processBuildings(Planet* planet);

	int Process()
	{
		if (!ao)
		{
			return LIBRARY_ERROR;
		}

		if (ao->state < LOGGED)
		{
			return LIBRARY_ERROR;
		}

		for (PlanetsVector::iterator it = ao->planets.begin(); it != ao->planets.end(); ++it)
		{
			processBuildings(&*it);
		}

		return LIBRARY_OK;
	}

	int processBuildings(Planet* planet)
	{
		for (BuildingsMap::iterator it = planet->buildings.begin(); it != planet->buildings.end(); ++it)
		{
			int supplyID = it->first;
			Building& building = it->second;

			// Update building availability
			building.available = CAN_BUILD(planet->buildings, supplyID);

			// Save timestamp
			long timestamp = time(0);

			// We must check for level upgradings
			if (building.upgrading)
			{
				// Elapsed time since last update
				long elapsed = timestamp - building.timestamp;

				// Is it finished?
				if (elapsed >= building.timeLeft)
				{
					// Flag end
					building.upgrading = false;
					building.timeLeft = 0;

					// Update planet resources pre-upgrade
					long preUpgradeTime = timestamp - (elapsed - building.timeLeft);
					planet->resources.update(preUpgradeTime, building, planet->maxTemp);

					// Increment level
					building.level += 1;
				}
				else
				{
					// Update time left
					building.timeLeft -= elapsed;
				}

				// Save timestamp
				building.timestamp = timestamp;
			}

			// Update resources (if any)
			planet->resources.update(timestamp, building, planet->maxTemp);
		}

		return LIBRARY_OK;
	}
}
