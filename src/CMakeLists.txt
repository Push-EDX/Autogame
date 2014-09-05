# Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

file(GLOB_RECURSE include_files ../include/*.cpp ../include/*.hpp)
file(GLOB_RECURSE sources_Buildings Buildings/*.cpp Buildings/*.hpp)
file(GLOB_RECURSE sources_Internal Internal/*.cpp Internal/*.hpp)
file(GLOB_RECURSE sources_Process Process/*.cpp Process/*.hpp)
file(GLOB_RECURSE sources_Update Update/*.cpp Update/*.hpp)
file(GLOB_RECURSE sources_Utils Utils/*.cpp Utils/*.hpp)

set(EPS_SRCS
  ${EPS_SRCS}
  ${include_files}
  ${sources_Buildings}
  ${sources_Internal}
  ${sources_Process}
  ${sources_Update}
  ${sources_Utils}
)

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/dep/curl/include
  ${CMAKE_SOURCE_DIR}/dep/restclient-cpp/include
  ${CMAKE_SOURCE_DIR}/dep/tinyxml2
  ${CMAKE_SOURCE_DIR}/dep/libtidy/include
  ${CMAKE_SOURCE_DIR}/dep/rapidjson/include

  ${CMAKE_SOURCE_DIR}/include

  ${CMAKE_CURRENT_SOURCE_DIR}/Buildings
  ${CMAKE_CURRENT_SOURCE_DIR}/Internal
  ${CMAKE_CURRENT_SOURCE_DIR}/Process
  ${CMAKE_CURRENT_SOURCE_DIR}/Update
  ${CMAKE_CURRENT_SOURCE_DIR}/Utils
)

add_library(AutOgame STATIC 
  ${EPS_SRCS}
)

target_link_libraries(AutOgame
  restclient-cpp
  tinyxml2
  libtidy
)

#install(TARGETS FastServer DESTINATION "${CMAKE_INSTALL_PREFIX}")

