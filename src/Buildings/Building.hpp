#ifndef _AUTOGAME_BUILDINGS_H_
#define _AUTOGAME_BUILDINGS_H_

#include <string>

namespace autogame
{
    // BUILDINGS
    enum Buildings
    {
        METAL_FACTORY = 1,
        CRYSTAL_FACTORY = 2,
        DEUTERIUM_FACTORY = 3,
        SOLAR_PANT = 4,
        FUSION_PANT = 12,
        SOLAR_SATELLITE = 212,
        METAL_STORAGE = 22,
        CRYSTAL_STORAGE = 23,
        DEUTERIUM_STORAGE = 24,
        METAL_HIDEOUT = 25,
        CRYSTAL_HIDEOUT = 26,
        DEUTERIUM_HIDEOUT = 27
    };

    struct Resources
    {
        double metal;
        double crystal;
        double deuterium;
    };

    struct Building
    {
        std::string name;
        char level;

        Resources costs;
        Resources production;
    };

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

        case SOLAR_PANT:
            return{ 75, 30, 0 };

        case FUSION_PANT:
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

        default:
            return{ 0, 0, 0 };
        }
    }

    inline static Resources _COST(int x, int y, Resources z = { 0 })
    {
        switch (x)
        {
        case METAL_FACTORY:
            return{ 60 * pow(1.5, y), 15 * pow(1.5, y), 0 };

        case CRYSTAL_FACTORY:
            return{ 48 * pow(1.6, y), 24 * pow(1.6, y), 0 };

        case DEUTERIUM_FACTORY:
            return{ 225 * pow(1.5, y), 75 * pow(1.5, y), 0 };

        case SOLAR_PANT:
            return{ 75 * pow(1.5, y), 30 * pow(1.5, y), 0 };

        case FUSION_PANT:
            return{ 900 * pow(1.8, y), 360 * pow(1.8, y), 0 };

        default:
            return{ z.metal * pow(2, y), z.crystal * pow(2, y), z.deuterium * pow(2, y) };
        }
    }

    inline static Resources TOTAL_COST(int x, int y, Resources z = { 0 })
    {
        switch (x)
        {
        case METAL_FACTORY:
            return{ 60 * (1 - pow(1.5, y)) / (-0.5), 15 * (1 - pow(1.5, y)) / (-0.5), 0 };

        case CRYSTAL_FACTORY:
            return{ 48 * (1 - pow(1.6, y)) / (-0.6), 24 * (1 - pow(1.6, y)) / (-0.6), 0 };

        case DEUTERIUM_FACTORY:
            return{ 225 * (1 - pow(1.5, y)) / (-0.5), 75 * (1 - pow(1.5, y)) / (-0.5), 0 };

        case SOLAR_PANT:
            return{ 75 * (1 - pow(1.5, y)) / (-0.5), 30 * (1 - pow(1.5, y)) / (-0.5), 0 };

        case FUSION_PANT:
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
}

#endif
