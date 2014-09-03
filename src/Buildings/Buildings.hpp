#ifndef _AUTOGAME_BUILDINGS_H_
#define _AUTOGAME_BUILDINGS_H_

#include "Common.hpp"

#include <string>
#include <map>

namespace autogame
{
    inline static Resources DEFAULT_COST(int x)
    {
        switch (x)
        {
        case METAL_FACTORY:
            return{ 60, 15, 0 };

        case CRYSTAL_FACTORY:
            return{ 48, 24, 0 };

        case DEUTERIUM_FACTORY:
            return{ 225, 75, 0 };

        case SOLAR_PLANT:
            return{ 75, 30, 0 };

        case FUSION_PLANT:
            return{ 900, 360, 0 };

        case SOLAR_SATELLITE:
            return{ 0, 2000, 500 };

        case METAL_STORAGE:
            return{ 1000, 0, 0 };

        case CRYSTAL_STORAGE:
            return{ 1000, 500, 0 };
            
        case DEUTERIUM_STORAGE:
            return{ 1000, 1000, 0 };

        case METAL_HIDEOUT:
            return{ 2645, 0, 0 };

        case CRYSTAL_HIDEOUT:
            return{ 2645, 1322, 0 };
            
        case DEUTERIUM_HIDEOUT:
            return{ 2645, 2645, 0 };

        case ALLIANCE_DEPOT:
            return{ 20000, 40000, 0 };

        case QUANTIC_JUMP:
            return{ 2000000, 4000000, 2000000 };

        case LUNAR_BASE:
            return{ 20000, 40000, 20000 };

        case SILO:
            return{ 20000, 20000, 1000 };

        case NANITE_FACTORY:
            return{ 1000000, 500000, 100000 };

        case LABORATORY:
            return{ 200, 400, 200 };

        case ROBOTS_FACTORY:
            return{ 400, 120, 200 };

        case PHALANX_SENSOR:
            return{ 20000, 40000, 20000 };

        case SHIPYARD:
            return{ 400, 200, 100 };

        case TERRAFORMER:
            return{ 0, 50000, 100000 };

        case ENERGY_TECH:
            return{0, 800, 400};

        case LASER_TECH:
            return{ 200, 100, 0 };

        case ION_TECH:
            return{ 1000, 300, 100 };

        case HYPERSPACE_TECH:
            return{ 0, 4000, 2000 };

        case PLASMA_TECH:
            return{ 2000, 4000, 1000 };

        case COMBUSTION_TECH:
            return{ 400, 0, 600 };

        case IMPULSE_TECH:
            return{ 2000, 4000, 600 };

        case HYPERSPACE_DRIVE_TECH:
            return{ 10000, 20000, 6000 };

        case SPY_TECH:
            return{ 200, 1000, 200 };

        case COMPUTATIONAL_TECH:
            return{ 0, 400, 600 };

        case ASTROPHYSICS_TECH:
            return{ 4000, 8000, 4000 };

        case INTERGALAXY_TECH:
            return{ 240000, 400000, 160000 };

        case GRAVITATIONAL_TECH:
            return{ 0, 0, 0 };

        case MILITAR_TECH:
            return{ 800, 200, 0 };

        case DEFENSE_TECH:
            return{ 200, 600, 0 };

        case BLINDAGE_TECH:
            return{ 1000, 0, 0 };

        default:
            return{ 0, 0, 0 };
        }
    }

    inline static Resources _COST(int x, int y, Resources z)
    {
        switch (x)
        {
        case METAL_FACTORY:
            return{ 60 * pow(1.5, y), 15 * pow(1.5, y), 0 };

        case CRYSTAL_FACTORY:
            return{ 48 * pow(1.6, y), 24 * pow(1.6, y), 0 };

        case DEUTERIUM_FACTORY:
            return{ 225 * pow(1.5, y), 75 * pow(1.5, y), 0 };

        case SOLAR_PLANT:
            return{ 75 * pow(1.5, y), 30 * pow(1.5, y), 0 };

        case FUSION_PLANT:
            return{ 900 * pow(1.8, y), 360 * pow(1.8, y), 0 };

        default:
            return{ z.metal * pow(2, y), z.crystal * pow(2, y), z.deuterium * pow(2, y) };
        }
    }

    inline static Resources TOTAL_COST(int x, int y, Resources z)
    {
        switch (x)
        {
        case METAL_FACTORY:
            return{ 60 * (1 - pow(1.5, y)) / (-0.5), 15 * (1 - pow(1.5, y)) / (-0.5), 0 };

        case CRYSTAL_FACTORY:
            return{ 48 * (1 - pow(1.6, y)) / (-0.6), 24 * (1 - pow(1.6, y)) / (-0.6), 0 };

        case DEUTERIUM_FACTORY:
            return{ 225 * (1 - pow(1.5, y)) / (-0.5), 75 * (1 - pow(1.5, y)) / (-0.5), 0 };

        case SOLAR_PLANT:
            return{ 75 * (1 - pow(1.5, y)) / (-0.5), 30 * (1 - pow(1.5, y)) / (-0.5), 0 };

        case FUSION_PLANT:
            return{ 900 * (1 - pow(1.8, y)) / (-0.8), 360 * (1 - pow(1.8, y)) / (-0.8), 0 };

        default:
            return{ z.metal * (1 - pow(2, y)) / (-1), z.crystal * (1 - pow(2, y)) / (-1), z.deuterium * (1 - pow(2, y)) / (-1) };
        }
    }

    inline static Resources COST(int x, int y)
    {
        Resources w;
        if (y != 0)
            w = _COST(x, y, DEFAULT_COST(x));
        else
            w = DEFAULT_COST(x);

        return{ floor(w.metal), floor(w.crystal), floor(w.deuterium) };
    }

    inline static Resources PRODUCTION(int x, int y, int z)
    {
        switch (x)
        {
        case METAL_FACTORY:
            return{ round(30 * y * pow(1.1, y)) + 30, 0, 0 };

        case CRYSTAL_FACTORY:
            return{ 0, round(20 * y * pow(1.1, y)) + 15, 0 };

        case DEUTERIUM_FACTORY:
            return{ 0, 0, round(10 * y * pow(1.1, y) * (1.44 - 0.004 * z)) };

        default:
            return{ 0, 0, 0 };
        }
    }

    static bool CAN_BUILD(BuildingsMap b, int x)
    {
        switch (x)
        {
        case FUSION_PLANT:
            return b[DEUTERIUM_FACTORY].level >= 5 && b[ENERGY_TECH].level >= 3;

        case NANITE_FACTORY:
            return b[ROBOTS_FACTORY].level >= 10 && b[COMPUTATIONAL_TECH].level >= 10;

        case SHIPYARD:
            return b[ROBOTS_FACTORY].level >= 2;

        case METAL_HIDEOUT:
            return b[METAL_STORAGE].level >= 2;

        case CRYSTAL_HIDEOUT:
            return b[CRYSTAL_STORAGE].level >= 2;

        case DEUTERIUM_HIDEOUT:
            return b[DEUTERIUM_STORAGE].level >= 2;

        case TERRAFORMER:
            return b[NANITE_FACTORY].level >= 1 && b[ENERGY_TECH].level >= 12;

        case PHALANX_SENSOR:
            return b[LUNAR_BASE].level >= 1;

        case QUANTIC_JUMP:
            return b[LUNAR_BASE].level >= 1 && b[HYPERSPACE_TECH].level >= 7;

        case SILO:
            return b[SHIPYARD].level >= 1;

        case SPY_TECH:
            return b[LABORATORY].level >= 3;

        case COMPUTATIONAL_TECH:
            return b[LABORATORY].level >= 1;

        case MILITAR_TECH:
            return b[LABORATORY].level >= 4;

        case DEFENSE_TECH:
            return b[ENERGY_TECH].level >= 3 && b[LABORATORY].level >= 6;

        case BLINDAGE_TECH:
            return b[LABORATORY].level >= 2;

        case ENERGY_TECH:
            return b[LABORATORY].level >= 1;

        case HYPERSPACE_TECH:
            return b[LABORATORY].level >= 7 && b[ENERGY_TECH].level >= 5 && b[DEFENSE_TECH].level >= 5;

        case COMBUSTION_TECH:
            return b[LABORATORY].level >= 1 && b[ENERGY_TECH].level >= 1;

        case IMPULSE_TECH:
            return b[LABORATORY].level >= 2 && b[ENERGY_TECH].level >= 2;

        case HYPERSPACE_DRIVE_TECH:
            return b[LABORATORY].level >= 7 && b[HYPERSPACE_TECH].level >= 3;

        case LASER_TECH:
            return b[LABORATORY].level >= 1 && b[ENERGY_TECH].level >= 2;

        case ION_TECH:
            return b[LABORATORY].level >= 4 && b[LASER_TECH].level >= 5 && b[ENERGY_TECH].level >= 4;

        case PLASMA_TECH:
            return b[LABORATORY].level >= 4 && b[ENERGY_TECH].level >= 8 && b[LASER_TECH].level >= 10 && b[ION_TECH].level >= 5;

        case INTERGALAXY_TECH:
            return b[LABORATORY].level >= 10 && b[HYPERSPACE_TECH].level >= 8 && b[COMPUTATIONAL_TECH].level >= 8;

        case ASTROPHYSICS_TECH:
            return b[LABORATORY].level >= 3 && b[SPY_TECH].level >= 4 && b[IMPULSE_TECH].level >= 3;

        case GRAVITATIONAL_TECH:
            return b[LABORATORY].level >= 12;

        default:
            return true;
        }
    }
}

#endif
