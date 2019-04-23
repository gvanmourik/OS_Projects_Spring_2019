
# include_directories("${PROJECT_SOURCE_DIR}/include")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_executable(${exe_name} main.cpp)
target_link_libraries(${exe_name} Threads::Threads)

install(TARGETS ${exe_name} 
		DESTINATION ${PROJECT_BINARY_DIR}/bin
		PERMISSIONS WORLD_EXECUTE)