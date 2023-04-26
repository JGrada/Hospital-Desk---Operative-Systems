cmake_minimum_required(VERSION 3.20)
project(TPSOCLION C)

set(CMAKE_C_STANDARD 99)

add_executable(TPSOCLION main.c medico.c medico.h balcao.c balcao.h cliente.c cliente.h)
