# Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

if( CMAKE_COMPILER_IS_GNUCXX )
  add_definitions(--no-warnings)
elseif( MSVC )
  add_definitions(/W0)
endif()

add_subdirectory(curl)
add_subdirectory(restclient-cpp)
add_subdirectory(tinyxml2)
add_subdirectory(libtidy)

