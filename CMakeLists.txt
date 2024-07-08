cmake_minimum_required(VERSION 3.28)
project(buildyourownlisp C)

set(CMAKE_C_STANDARD 23)

add_executable(lisp_clone main.c mpc.h mpc.c
        parser.c
        parser.h
        eval.h
        eval.c)
