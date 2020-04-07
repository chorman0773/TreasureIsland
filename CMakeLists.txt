cmake_minimum_required(VERSION 3.15)
project(TreasureIsland C)

set(CMAKE_C_STANDARD 11)

if(NOT DEFINED tigame_abi)
    set(tigame_abi 1)
endif()

if(WIN32)
    set(extload_os windows)
else()
    set(extload_os linux)
endif()

if(NOT (CMAKE_BUILD_TYPE EQUAL "Debug"))
add_definitions(-DTIGAME_NO_DEBUG)
endif()

configure_file(include/tigame/ComAbi.h.in include/tigame/ComAbi.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)
include_directories(include)
include_directories(libconinteract/include)
include_directories(random/include)
include_directories(util/collect/include util/io/include util/shadenbt/include util/time/include)

add_subdirectory(extensions)
add_subdirectory(libconinteract)
add_subdirectory(random)
add_subdirectory(util)

add_executable(TreasureIsland
        include/internal/ColoredWrite.h
        include/internal/ExtensionEntries.h
        include/internal/Game.h
        include/tigame/ComTypes.h
        include/tigame/Game.h
        include/tigame/LinkedList.h
        include/tigame/Map.h
        src/extload/${extload_os}/ExtensionLoader.c
        src/ColoredWrite.c
        src/Food.c
        src/Game.c
        src/Items.c
        src/Main.c
        src/Tiles.c)

target_link_libraries(TreasureIsland libconinteract random io collect shadenbt dl m)

install(TARGETS TreasureIsland DESTINATION .)
install(DIRECTORY include/tigame DESTINATION include PATTERN "*.h" EXCLUDE PATTERN "*.in")
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/tigame DESTINATION include)

if(NOT WIN32)
    target_link_options(TreasureIsland PUBLIC "LINKER:-rpath='$ORIGIN/../lib'")
endif()