cmake_minimum_required(VERSION 3.10)
project(tweets_generator C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic")
add_executable(tweets_generator
        tweets_generator.c
        markov_chain.c
        linked_list.c)