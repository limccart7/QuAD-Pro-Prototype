# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Set the project name and the programming language used
project(MyProject LANGUAGES C)

# Add an executable target named 'main' built from the specified source files
add_executable(transmit_ex src/transmit.c src/helpFunctions.c src/sx1262.c)

# Link the necessary libraries to the executable
target_link_libraries(transmit_ex PRIVATE lgpio )

