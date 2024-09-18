# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Set the project name and the programming language used
project(MyProject LANGUAGES C)

# Add an executable target named 'main' built from the specified source files
add_executable(publish_custom src/publish_custom.c src/redis.c)

add_executable(redis_consumer src/redis_consumer.c src/redis.c)

add_executable(delete_stream src/delete_stream.c src/redis.c)

add_executable(redis_stream src/redis_stream.c src/redis.c)

# Link the necessary libraries to each executable
target_link_libraries(publish_custom PRIVATE hiredis)
target_link_libraries(redis_consumer PRIVATE hiredis)
target_link_libraries(delete_stream PRIVATE hiredis)
target_link_libraries(redis_stream PRIVATE hiredis)
