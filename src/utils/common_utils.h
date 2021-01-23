//
// Created by baochong on 2020/7.
// Modified in 2020/11.
//

#pragma once

#ifndef MY_COMMON_UTILS_H
#define MY_COMMON_UTILS_H
#include <cstdio> 
#include <cstdlib>
#include <iostream>
#include <vector>
#include <chrono>

namespace CommonUtils {
    typedef unsigned int uint;
    
    //(Note): `(b-a)` should be less than 32767(MAX_RAND),
    //       otherwise, the function only produces the number within `a` and `a+32767`
    inline int RandInt(int a, int b) {
        return rand()%(b-a+1)  + a;
    }

    struct Color {
        int r;
        int g;
        int b;
        Color(int rr = 0, int gg = 0, int bb = 0): r(rr), g(gg), b(bb){}
    };

    template<typename T>
    int Round(T x) { return int(x + 0.5);}

    inline void PrintSizetVector(const std::vector<size_t>& ids) {
        std::cout<<"("<<ids.size()<<"): ";
        for(int i = 0 ; i < ids.size() ; i++) {
            std::cout<<ids[i]<<" ";
        }
        std::cout<<"\n";
    }

    class Timer {
    public:
        Timer() {
            t0 = std::chrono::high_resolution_clock::now();
        }

        long long Elapsed() {
            std::chrono::milliseconds d = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - t0);
            return d.count();
        }
        std::chrono::time_point<std::chrono::high_resolution_clock> t0;
        //std::chrono::high_resolution_clock::time_point
    };
}

#endif

