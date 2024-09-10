# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Set the project name and the programming language used
project(MyProject LANGUAGES CXX)

# Add an executable target named 'main' built from the specified source files
add_executable(main src/main.cpp src/redis.cpp src/BloomFilter.cpp src/PapaDuck.cpp src/Utils.cpp
	src/Packet.cpp)

# Link the necessary libraries to the executable
target_link_libraries(main PRIVATE hiredis)


