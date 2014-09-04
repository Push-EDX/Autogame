project(restclient-cpp)

file(GLOB_RECURSE INCFILES include/*.h)
file(GLOB_RECURSE SRCFILES source/*.cpp)

set(REST_SRCS
  ${INCFILES}
  ${SRCFILES}
)

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/dep/curl/include
  
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_definitions(-DCURL_STATICLIB)
add_library(restclient-cpp STATIC ${REST_SRCS})

target_link_libraries(restclient-cpp
  libcurl
  ws2_32
)
