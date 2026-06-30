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
        Game.cpp
        rendering/GameRenderer.cpp
        rendering/gui/UIManager.cpp
        rendering/gui/UIButton.cpp
        rendering/gui/UIState.cpp
        roads/RoadBuilder.cpp
        roads/Segment.cpp
        roads/Road.cpp
        roads/Road.h
)

target_link_libraries(Traffix PRIVATE sfml-graphics sfml-window sfml-system)

add_custom_command(TARGET Traffix POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:sfml-graphics>"
        "$<TARGET_FILE:sfml-window>"
        "$<TARGET_FILE:sfml-system>"
        "$<TARGET_FILE_DIR:Traffix>"
)
