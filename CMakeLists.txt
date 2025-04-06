cmake_minimum_required(VERSION 3.30)
project(Traffix)

set(CMAKE_CXX_STANDARD 17)

IF(WIN32)
    set(SFML_DIR "C:/SFML-2.6.2/lib/cmake/SFML")
ELSEIF(UNIX)
    # Do linux specific includes
ENDIF(WIN32)

find_package(SFML 2.6.2 REQUIRED COMPONENTS graphics window system)

add_executable(Traffix
        main.cpp
        GameRenderer.cpp
        Game.cpp
)

target_link_libraries(Traffix PRIVATE sfml-graphics sfml-window sfml-system)
