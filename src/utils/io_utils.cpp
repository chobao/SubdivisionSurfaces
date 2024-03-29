#include "io_utils.h"
//#define DEBUG_INPUT

namespace CommonUtils {
bool LoadObj(const std::string& file_path,
                std::vector<Eigen::Vector3d>& vertices, 
                std::vector<std::vector<index_t>>& polygons,
                float b_split_polygon) {
        std::ifstream file(file_path);
        if (!file.is_open()) {
            std::cerr << "fail to open file " + file_path << std::endl;
            return false;
        }
        std::string type;
        int face_cnt = 0, vertex_cnt = 0;


        while (file >> type) {
            if (type == "v") {
                // vertex
                Eigen::Vector3d vt;
                file >> vt(0) >> vt(1) >> vt(2);
                vertices.push_back(vt);
            } else if (type == "f") {
                std::string token;
                std::vector<index_t> index_v;
                std::string line;
                getline(file, line);
                std::stringstream ss(line);
                while(getline(ss, token, ' '))
                {
                    if (token.size() == 0 || token[0] == '\r' || !(token[0] >= '0'&&token[0]<='9') ) continue;
    #ifdef DEBUG_INPUT
                    std::cout<<"DEBUG(BCHO): LoadObj(): token:"<<token<<"("<<std::stoi(token) - 1<<")"<<std::endl;
    #endif
                    index_v.push_back(std::stoi(token) - 1);
                }

                if (index_v.size() > 2) {
                    if(b_split_polygon) {
                        const index_t id0 = index_v[0];
                        for(int j = 1 ,sz = index_v.size() - 1; j < sz ;j++) {
                            //push vertex
                            polygons.emplace_back(std::vector<index_t>{id0, index_v[j], index_v[j+1]});
                            vertex_cnt += 3;
                            face_cnt++;
                        }
                    }
                    else {
                        polygons.push_back(index_v);
                        vertex_cnt += index_v.size();
                        face_cnt++;
                    }
                    
                }
            }
        }
        file.close();
    
        std::cout << "Model has " << face_cnt << " faces and " << vertex_cnt << " vertexes"
            << std::endl;
        return true;
    }

    void *GetFileBuffer(const char *pFileName, int *pBufferSize)
    {

        printf("Opening File: %s\n", pFileName);
        FILE *fp = fopen(pFileName, "rb");
        if (!fp) {
            printf("Unable to open file: %s\n", pFileName);
            return NULL;
        }

        fseek(fp, 0, SEEK_END);
        *pBufferSize = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        char *pRetBuff = new char[*pBufferSize + 1];
        size_t tmp = fread(pRetBuff, 1, *pBufferSize, fp);
        pRetBuff[*pBufferSize] = 0;

        fclose(fp);

        return pRetBuff;
    }

    
}