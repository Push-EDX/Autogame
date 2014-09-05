#include <Buildings.hpp>
#include <Common.hpp>

namespace autogame
{
	void AutoResources::update(long timestamp, Building building, int planetMaxTemp)
	{
		Resources res = PRODUCTION(building.identifier, building.level, planetMaxTemp);

		if (res.metal != 0)
		{
			updateMetal(timestamp - lastMetalUpdate, res.metal);
			lastMetalUpdate = timestamp;
		}
		else if (res.crystal != 0)
		{
			updateCrystal(timestamp - lastCrystalUpdate, res.crystal);
			lastCrystalUpdate = timestamp;
		}
		else if (res.deuterium != 0)
		{
			updateDeuterium(timestamp - lastDeuteriumUpdate, res.deuterium);
			lastDeuteriumUpdate = timestamp;
		}
	}
}
