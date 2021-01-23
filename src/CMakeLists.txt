




################################################################################
# Add sources
################################################################################
file(
  GLOB_RECURSE
  subdivisonsurface_files_header
  *.h
)
file(
  GLOB_RECURSE
  subdivisonsurface_files_source
  *.cpp
)
list(FILTER subdivisonsurface_files_header EXCLUDE REGEX "${PROJECT_SOURCE_DIR}/src/software/.*" )
list(FILTER subdivisonsurface_files_source EXCLUDE REGEX "${PROJECT_SOURCE_DIR}/src/software/.*" )

message(STATUS "subdivisonsurface_files_header:${subdivisonsurface_files_header}")
message(STATUS "subdivisonsurface_files_source:${subdivisonsurface_files_source}")


add_library(SubdivisionSurface ${subdivisonsurface_files_header} ${subdivisonsurface_files_source})
target_link_libraries(SubdivisionSurface 
                        LINK_PRIVATE
                        ${SubdivisionSurface_EXTERNAL_LIBRARIES})


################################################################################
# Add executable
################################################################################
add_executable(mainSubdivisionSurface software/main.cpp)
target_link_libraries(mainSubdivisionSurface  PRIVATE SubdivisionSurface)