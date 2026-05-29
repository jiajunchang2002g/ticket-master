cmake_minimum_required(VERSION 3.16)

project(TicketSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include headers
include_directories(include)

# Core library
add_library(ticket_system
    src/TicketSystem.cpp
)

target_include_directories(ticket_system PUBLIC include)

# Main executable (optional demo)
add_executable(demo
    src/main.cpp
)

target_link_libraries(demo PRIVATE ticket_system)

# Tests executable
add_executable(test_ticket_system
    tests/test_ticket_system.cpp
)

target_link_libraries(test_ticket_system PRIVATE ticket_system)

# Enable testing
enable_testing()
add_test(NAME TicketSystemTests COMMAND test_ticket_system)
