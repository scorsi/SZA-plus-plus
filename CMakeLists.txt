cmake_minimum_required(VERSION 3.9)
project(sza_plus_plus)

set(CMAKE_CXX_STANDARD 17)

add_executable(sza_plus_plus
        api/conf.h
        api/http.h
        api/module.h
        api/net.h main.cpp api/pp/conf.hpp api/pp/module.hpp api/pp/http.hpp api/pp/net.hpp)
