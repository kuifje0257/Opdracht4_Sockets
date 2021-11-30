cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(Opdracht4)

link_libraries(libtlpi.a)
link_libraries(PJ_RPI)

add_executable(Client echo_Client.c)
add_executable(Server echo_Server.c)


