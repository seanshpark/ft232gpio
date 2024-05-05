#

set(SRCS
    src/ft232.cpp
    src/tm1637.cpp
)

add_library(ft232gpio STATIC ${SRCS})
target_include_directories(ft232gpio PUBLIC include)
target_include_directories(ft232gpio SYSTEM PUBLIC ${FTDI1_INCLUDE_DIRS})
target_link_libraries(ft232gpio PUBLIC ${FTDI1_LIBRARIES})

