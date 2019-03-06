
# include_directories("${PROJECT_SOURCE_DIR}/include")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_executable(sim01 main.cpp)
# target_link_libraries(sim01 SimLib)

install(TARGETS sim01 
		DESTINATION ${PROJECT_BINARY_DIR}/bin
		PERMISSIONS WORLD_EXECUTE)