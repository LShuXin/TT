cmake_minimum_required(VERSION 2.6)
PROJECT(TeamtalkServer)
SET(CMAKE_CONFIGURATION_TYPES "Debug;Release")

message("Generated with config types: ${CMAKE_CONFIGURATION_TYPES}")

add_subdirectory(slog)
add_subdirectory(base)
add_subdirectory(db_proxy_server)
add_subdirectory(file_server)
add_subdirectory(login_server)
add_subdirectory(msfs)
add_subdirectory(msg_server)
add_subdirectory(http_msg_server)
add_subdirectory(push_server)
add_subdirectory(route_server)
