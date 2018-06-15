#root/code/

#If you need to link to external libraries (e.g. box2D, OpenAL etc.),
#use 'target_link_directories()
#
#Example:
#
#target_link_directories( ${MOOSE_LIBRARY}
#	PRIVATE
#	${CMAKE_CURRENT_SOURCE_DIR} ${MOOSE_EXTERNAL_LIBS}/box2d/lib)
#
#For including headers, use 'target_include_directories()'
#Example:
#
#target_include_directories( ${MOOSE_LIBRARY}
#	PRIVATE
#	${CMAKE_CURRENT_SOURCE_DIR} ${MOOSE_EXTERNAL_HEADERS}/OpenAL/include)

link_directories( ${MOOSE_LIBRARY}
	${CMAKE_CURRENT_SOURCE_DIR} ${MOOSE_PATH_EXTERNAL_LIBS}/Assimp/lib/x64
	${CMAKE_CURRENT_SOURCE_DIR} ${MOOSE_PATH_EXTERNAL_LIBS}/freeglut/lib/x64
	${CMAKE_CURRENT_SOURCE_DIR} ${MOOSE_PATH_EXTERNAL_LIBS}/glew/lib/Release/x64
	${CMAKE_CURRENT_SOURCE_DIR} ${MOOSE_PATH_EXTERNAL_LIBS}/FreeImage/x64
)
target_link_libraries( ${MOOSE_LIBRARY}
	assimp
	freeglut
	glew32
	FreeImage
)
include_directories( ${MOOSE_LIBRARY}
	${CMAKE_CURRENT_SOURCE_DIR} ${MOOSE_PATH_EXTERNAL_HEADERS}/Assimp/include
	${CMAKE_CURRENT_SOURCE_DIR} ${MOOSE_PATH_EXTERNAL_HEADERS}/freeglut/include
	${CMAKE_CURRENT_SOURCE_DIR} ${MOOSE_PATH_EXTERNAL_HEADERS}/FreeImage/x64
	${CMAKE_CURRENT_SOURCE_DIR} ${MOOSE_PATH_EXTERNAL_HEADERS}/glew/include
	${CMAKE_CURRENT_SOURCE_DIR} ${MOOSE_PATH_EXTERNAL_HEADERS}/glm
	${CMAKE_CURRENT_SOURCE_DIR} ${MOOSE_PATH_EXTERNAL_HEADERS}/imgui/include)
	
FILE (GLOB IMGUI
	${CMAKE_CURRENT_SOURCE_DIR} ${MOOSE_PATH_EXTERNAL_HEADERS}/imgui/include/*.h
	${CMAKE_CURRENT_SOURCE_DIR} ${MOOSE_PATH_EXTERNAL_HEADERS}/imgui/include/*.cpp
)

#**********************************************************************************

#When adding new files to public API, add them here.
#
#USE the prefix ${CMAKE_CURRENT_LIST_DIR}
set (MOOSE_CORE_API
        ${CMAKE_CURRENT_LIST_DIR}/headers/Follower.h
        ${CMAKE_CURRENT_LIST_DIR}/headers/Geometry.h
        ${CMAKE_CURRENT_LIST_DIR}/headers/InputManager.h
        ${CMAKE_CURRENT_LIST_DIR}/headers/Matrix.h
        ${CMAKE_CURRENT_LIST_DIR}/headers/ModelManager.h
        ${CMAKE_CURRENT_LIST_DIR}/headers/PathFinding.h
        ${CMAKE_CURRENT_LIST_DIR}/headers/RenderFunctions.h
        ${CMAKE_CURRENT_LIST_DIR}/headers/Turret.h
        ${CMAKE_CURRENT_LIST_DIR}/headers/Vector.h
        ${CMAKE_CURRENT_LIST_DIR}/headers/Quadtree.h
        ${CMAKE_CURRENT_LIST_DIR}/headers/Octree.h
        ${CMAKE_CURRENT_LIST_DIR}/headers/Vehicle.h
        ${CMAKE_CURRENT_LIST_DIR}/headers/Scene.h
        ${CMAKE_CURRENT_LIST_DIR}/headers/Animation.h
        ${CMAKE_CURRENT_LIST_DIR}/headers/Entity.h
)

#Add files that are internal. (Internal headers and sources and also sources for
#public headers)
#USE the prefix ${CMAKE_CURRENT_LIST_DIR}
set (MOOSE_CORE_INTERNAL
		${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt #KEEP HERE. Ensures that CMakeLists.txt shows in IDE
        ${CMAKE_CURRENT_LIST_DIR}/source/Geometry.cpp
        ${CMAKE_CURRENT_LIST_DIR}/source/InputManager.cpp
        ${CMAKE_CURRENT_LIST_DIR}/source/ModelManager.cpp
        ${CMAKE_CURRENT_LIST_DIR}/source/RenderFunctions.cpp
        ${CMAKE_CURRENT_LIST_DIR}/source/Vector.cpp
        ${CMAKE_CURRENT_LIST_DIR}/source/Quadtree.cpp
        ${CMAKE_CURRENT_LIST_DIR}/source/Octree.cpp
        ${CMAKE_CURRENT_LIST_DIR}/source/Scene.cpp
        ${CMAKE_CURRENT_LIST_DIR}/source/Animation.cpp
        ${CMAKE_CURRENT_LIST_DIR}/source/Entity.cpp
)

#**********************************************************************************

#Create a folder structure to IDE
source_group ("Core" FILES 
		${MOOSE_CORE_INTERNAL} 
)
source_group ("Headers" FILES 
		${MOOSE_CORE_API}
)
source_group ("Imgui" FILES 
		${IMGUI} 
)

#Adds files to public and internal api files
list (APPEND MOOSE_PUBLIC_API_FILES ${MOOSE_CORE_API})
list (APPEND MOOSE_INTERNAL_FILES ${MOOSE_CORE_INTERNAL})
list (APPEND MOOSE_INTERNAL_FILES ${IMGUI})