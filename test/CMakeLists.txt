# Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

project(AutOGame-Test)

file(GLOB_RECURSE sources *.cpp *.hpp)

set(EPS_SRCS
  ${EPS_SRCS}
  ${sources}
)

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/include
)

add_executable(AutOGame-Test
  ${EPS_SRCS}
)

target_link_libraries(AutOGame-Test
  AutOgame
)

#install(TARGETS FastServer DESTINATION "${CMAKE_INSTALL_PREFIX}")

