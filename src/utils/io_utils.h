#ifndef MY_IO_UTILS_H
#define MY_IO_UTILS_H

#include "common_utils.h"
#include <fstream>
#include <Eigen/Core>

namespace CommonUtils {
    bool LoadObj(const std::string& file_path,
                std::vector<Eigen::Vector3d>& vertices, 
                std::vector<std::vector<index_t>>& polygons,
                float b_split_polygon = false);

    
    //Read content from file path and return pointer of the buffer
    //pFileName: the pointer of the file path string
    //pBufferSize: will write the the length of the content into it
    //return a pointer of the buffer which have the file content
    void *GetFileBuffer(const char *pFileName, int *pBufferSize);
}
#endif