# Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
project(libtidy)

file(GLOB_RECURSE sources src/*.c src/*.h)
file(GLOB_RECURSE inc include/*.h)

set(EPS_SRCS
  ${EPS_SRCS}
  ${sources}
  ${inc}
)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_library(libtidy STATIC 
  ${EPS_SRCS}
)
