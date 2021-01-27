cmake_minimum_required(VERSION 3.09)
project(SubdivisionSurface)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

################################################################################
# Find packages
################################################################################
find_package(Eigen3 REQUIRED)
if(EIGEN3_FOUND)
message(STATUS "Eigen include dirs: ${EIGEN3_INCLUDE_DIRS}")
endif()

find_package(OpenGL REQUIRED)
if(OPENGL_FOUND)
#message(STATUS "OpenGL include dirs:${OPENGL_INCLUDE_DIRS}")
message(STATUS "OpenGL libraries :${OPENGL_LIBRARIES}")
endif()

find_package(glfw3 REQUIRED)
find_package(GLUT REQUIRED)
if(GLUT_FOUND)
message(STATUS "GLUT libraries :${GLUT_LIBRARIES}")
endif()

find_package(GLEW REQUIRED)
if(GLEW_FOUND)
message(STATUS "GLEW libraries :${GLEW_LIBRARIES}")
endif()


set(SubdivisionSurface_EXTERNA_INCLUDEDIRS ${EIGEN3_INCLUDE_DIRS})
set(SubdivisionSurface_EXTERNAL_LIBRARIES ${OPENGL_LIBRARIES} glfw ${GLEW_LIBRARIES} ${GLUT_LIBRARIES})
include_directories(${SubdivisionSurface_EXTERNA_INCLUDEDIRS})
include_directories(src)
add_subdirectory(src)
