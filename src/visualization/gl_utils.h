#pragma once
#include <iostream>
#include <glm/glm.hpp>
namespace GLRendering {
    #define GL(func) func

    #define LOG(...) printf(__VA_ARGS__)

     inline void Output(const std::string name, const glm::mat4& matrix) {
        for (int i = 0; i < 4; i++) {
            printf("DEBUG(BCHO): %s (%d), %f, %f, %f, %f\n", name.c_str(), i,
                 matrix[i][0], matrix[i][1], matrix[i][2], matrix[i][3]);
        }
    }

}