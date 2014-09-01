# Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

file(GLOB_RECURSE sources_Init Init/*.cpp Init/*.h)

set(EPS_SRCS
  ${EPS_SRCS}
  ${sources_Init}
)

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/dep/happyhttp
  ${CMAKE_SOURCE_DIR}/dep/tinyxml2

  ${CMAKE_CURRENT_SOURCE_DIR}/Init

  ${OPENSSL_INCLUDE_DIR}
)

add_library(AutOgame
  ${EPS_SRCS}
)

target_link_libraries(AutOgame
  happyHTTP
  tinyXML2
)

#install(TARGETS FastServer DESTINATION "${CMAKE_INSTALL_PREFIX}")

