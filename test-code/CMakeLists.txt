cmake_minimum_required(VERSION 3.10)

# set the project name
project(Test)

# add the executable
add_executable(Thd threadss.c)
add_executable(Frk forking.c)
add_executable(Ded dead.c)