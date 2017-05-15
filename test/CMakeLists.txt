PROJECT(bitcask)

SET(CMAKE_C_FLAGS "-std=c99 -g")
INCLUDE_DIRECTORIES(${CURRENT_SOURCE_DIR}../src)

#ADD_EXECUTABLE(hash_test hash_test.c)
#ADD_EXECUTABLE(dir_test dir_test.c)

ADD_EXECUTABLE(bitcask_test bitcask_test.c ../src/bitcask.c)




