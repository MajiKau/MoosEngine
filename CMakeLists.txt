#root/

#Minimum required version of CMake
cmake_minimum_required (VERSION 3.8)


#'project' is a Visual Studio solution. Solution has
#all the targets as Visual Studio projects.
#Set variable for application name
set (APPLICATION_NAME MoosE)
project (${APPLICATION_NAME})

set(CMAKE_BUILD_TYPE Release)

#Set variable for CMake's 'add_library'. This is used in 'root/src' 
#-CMakeLists.txt to define a name for library and in other sub-folders
#to add source files
set (MOOSE_LIBRARY MoosEngine)


#Current version (0.0.1) //TODO: Read this from separate file??
set (MOOSE_VERSION_MAJOR "0.")
set (MOOSE_VERSION_MINOR "0.")
set (MOOSE_VERSION_PATCH "1")

#Set name for release
set (RELEASE_NAME MoosE_${MOOSE_VERSION_MAJOR}${MOOSE_VERSION_MINOR}${MOOSE_VERSION_PATCH}
)
						

#Turn on the ability to use folders in Visual Studio's IDE or others supporting them.
set_property (GLOBAL PROPERTY USE_FOLDERS ON)

#Check that user's compiler supports C++11 standard
message ("testi")
include (CheckCXXCompilerFlag)
# Emscripten freezes on c++11 check
if (NOT EMSCRIPTEN)
	check_cxx_compiler_flag("-std=c++11" COMPILER_SUPPORTS_CXX11)
	if (COMPILER_SUPPORTS_CXX11)
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	else()
		message ("NO support for C++11, exiting" FATAL_ERROR)
	endif()
else()
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()




#Output information to console
message ("cxx Flags:" ${CMAKE_CXX_FLAGS})


#Add library
set (MOOSE_MAIN ${CMAKE_CURRENT_LIST_DIR}/code/source/main.cpp)
add_library (${MOOSE_LIBRARY} STATIC
	${MOOSE_MAIN}
)
	
#Tell CMake where our engine's headers are.
target_include_directories(${MOOSE_LIBRARY}
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/source/headers/
)
					
#Tell that we want our exe to go to 'bin' -folder
set_target_properties(${MOOSE_LIBRARY} 
	PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
)

#Set variable path to external libs and headers
set (MOOSE_PATH_EXTERNAL_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/include/)
set (MOOSE_PATH_EXTERNAL_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/)

#Add variables to hold public and internal api files
set (MOOSE_PUBLIC_API_FILES "")
set (MOOSE_INTERNAL_FILES "")

#Include sub-directories where more CMakeLists.txt exist.
#See
#https://crascit.com/2016/01/31/enhanced-source-file-handling-with-target_sources/
include (code/CMakeLists.txt)



#TODO: Move .cpp files from MOOSE_PUBLIC_API_FILES to MOOSE_INTERNAL_FILES,
#so that they don't get included when installing

#Target these sources to MoosE -project
target_sources(${MOOSE_LIBRARY} 
		PRIVATE
		${MOOSE_PUBLIC_API_FILES}
		PRIVATE
		${MOOSE_INTERNAL_FILES}	
)

#Add public headers to target for releases
set_target_properties ( ${MOOSE_LIBRARY} PROPERTIES PUBLIC_HEADER "${MOOSE_PUBLIC_API_FILES}")

#Installing

install(TARGETS ${MOOSE_LIBRARY}
		ARCHIVE DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/releases/${RELEASE_NAME}/lib
		LIBRARY DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/releases/${RELEASE_NAME}/lib
		RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/releases/${RELEASE_NAME}/lib
		PUBLIC_HEADER DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/releases/${RELEASE_NAME}/include
)


set (MOOSE_CREATE_TEST_PROJECT ON CACHE BOOL "Whether the test project should be generated")
set (TEST_PROJECT_EXE test_project)
if(${MOOSE_CREATE_TEST_PROJECT})
	add_executable(${TEST_PROJECT_EXE}
		${CMAKE_CURRENT_SOURCE_DIR}/code/source/main.cpp
	)	
	
	set(VS_STARTUP_PROJECT ${TEST_PROJECT_EXE})
	
	#Tell test_project where to find Routa -headers and lib
	target_include_directories(${TEST_PROJECT_EXE} PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}../code/headers/
		${CMAKE_CURRENT_SOURCE_DIR}../lib/		
	)
	
	#Set debugger working directory to be same as the location of this file
	set_target_properties(${TEST_PROJECT_EXE} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY
		${CMAKE_CURRENT_LIST_DIR})
	
	#Link Routa library to this executable
	target_link_libraries (${TEST_PROJECT_EXE}
		${MOOSE_LIBRARY}
	)
	
	#Tell that we want our exe to go to 'test_project/bin' -folder
	set_target_properties(${TEST_PROJECT_EXE} 
	PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/
)
endif()
