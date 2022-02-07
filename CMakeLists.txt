cmake_minimum_required(VERSION 3.20)
project(simple_ncurses_snake)

set(CMAKE_CXX_STANDARD 14)

add_executable(simple_ncurses_snake
        main.cpp
        cpoint.h
        screen.h
        screen.cpp
        snake.h
        snake.cpp
        winsys.h
        winsys.cpp
        )
target_link_libraries( simple_ncurses_snake curses )