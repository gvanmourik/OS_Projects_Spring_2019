
# include_directories("${PROJECT_SOURCE_DIR}/include")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_executable(sim02 main.cpp)
target_link_libraries(sim02 Threads::Threads)

install(TARGETS sim02 
		DESTINATION ${PROJECT_BINARY_DIR}/bin
		PERMISSIONS WORLD_EXECUTE)