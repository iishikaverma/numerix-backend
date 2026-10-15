cmake_minimum_required(VERSION 3.20)

project(NumerixBackend)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Crow CONFIG REQUIRED)

include_directories(include)

add_executable(numerix

    src/main.cpp
    src/apis/api.cpp

    src/algorithms/bisection.cpp
    src/algorithms/secant.cpp
    src/algorithms/newton_raphson.cpp
    src/algorithms/lagrange.cpp
    src/algorithms/newton_divided_difference.cpp
    src/utils/expression_parser.cpp
)

target_link_libraries(numerix PRIVATE
    Crow::Crow
    ws2_32
    mswsock
)