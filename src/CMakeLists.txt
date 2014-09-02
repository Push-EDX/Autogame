# Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

file(GLOB_RECURSE sources_Init Init/*.cpp Init/*.hpp)

set(EPS_SRCS
  ${EPS_SRCS}
  ${sources_Init}
)

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/dep/curl/include
  ${CMAKE_SOURCE_DIR}/dep/restclient-cpp/include
  ${CMAKE_SOURCE_DIR}/dep/htmlcxx/html
  ${CMAKE_SOURCE_DIR}/dep/libtidy/include

  ${CMAKE_CURRENT_SOURCE_DIR}/Init
)

add_library(AutOgame STATIC 
  ${EPS_SRCS}
)

target_link_libraries(AutOgame
  restclient-cpp
  htmlcxx
  libtidy
)

#install(TARGETS FastServer DESTINATION "${CMAKE_INSTALL_PREFIX}")

