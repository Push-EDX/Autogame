#ifndef _AUTOGAME_COMMON_H_
#define _AUTOGAME_COMMON_H_

#include <string>
#include <map>
#include <vector>

namespace autogame
{
    // BUILDINGS
    enum Buildings
    {
        METAL_FACTORY           = 1,
        CRYSTAL_FACTORY         = 2,
        DEUTERIUM_FACTORY       = 3,
        SOLAR_PLANT             = 4,
        FUSION_PLANT            = 12,
        ROBOTS_FACTORY          = 14,
        NANITE_FACTORY          = 15,
        SOLAR_SATELLITE         = 212,
        SHIPYARD                = 21,
        METAL_STORAGE           = 22,
        CRYSTAL_STORAGE         = 23,
        DEUTERIUM_STORAGE       = 24,
        METAL_HIDEOUT           = 25,
        CRYSTAL_HIDEOUT         = 26,
        DEUTERIUM_HIDEOUT       = 27,
        LABORATORY              = 31,
        TERRAFORMER             = 33,
        ALLIANCE_DEPOT          = 34,
        LUNAR_BASE              = 41,
        PHALANX_SENSOR          = 42,
        QUANTIC_JUMP            = 43,
        SILO                    = 44,
    };

    enum Technologies
    {
        SPY_TECH                = 106,
        COMPUTATIONAL_TECH      = 108,
        MILITAR_TECH            = 109,
        DEFENSE_TECH            = 110,
        BLINDAGE_TECH           = 111,
        ENERGY_TECH             = 113,
        HYPERSPACE_TECH         = 114,
        COMBUSTION_TECH         = 115,
        IMPULSE_TECH            = 117,
        HYPERSPACE_DRIVE_TECH   = 118,
        LASER_TECH              = 120,
        ION_TECH                = 121,
        PLASMA_TECH             = 122,
        INTERGALAXY_TECH        = 123,
        ASTROPHYSICS_TECH       = 124,
        GRAVITATIONAL_TECH      = 199,
    };

    struct Resources
    {
        double metal;
        double crystal;
        double deuterium;
    };

    struct Building;
    typedef std::map<int, Building> BuildingsMap;

    struct Building
    {
        std::string name;
        char level;
        bool available;
        bool upgrading;
        long timeTotal;
        long timeLeft;

        std::string fastBuild;

        Resources costs;
        Resources production;

        Building()
        {
            name = std::string();
            level = 0;
            available = false;
            upgrading = false;
            timeTotal = 0;
            timeLeft = 0;
        }
    };


    // Typedefs
    struct Planet;
    typedef std::vector<Planet> PlanetsVector;

    // Structs
    struct Planet
    {
        // Name
        std::string name;

        // Resources
        Resources resources;

        // Min-Max temp
        int minTemp;
        int maxTemp;

        struct Coordinates
        {
            int galaxy;
            int system;
            int position;
        } coordinates;

        // Building levels
        BuildingsMap buildings;

        Planet(std::string nm)
        {
            name = nm;
            minTemp = 0;
            maxTemp = 0;
        }
    };
}

#endif
