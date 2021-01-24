
#pragma once
#include <GL/glew.h>
#include <GL/gl.h>
#include "gl_utils.h"

namespace GLRendering {

    struct ProgramAttribute
    {
        unsigned int    index;  //location layout in vertex shader
        int             size;   //number of elements in input array
        unsigned int    type;   //type of input element
        bool            normalized;  //whether the input element is normalized
        int             stride;  // the stride between 2 neighbour attribute
		int				offset;  // the offset betwwen first element and beginning
    };

    class Geometry {
        public:
        Geometry();

        void Initialize(ProgramAttribute* pAttribs, int nAttribs,
                        unsigned int* pIndices, int nIndices,
                        const void* pVertexData, int bufferSize, int nVertices);

        void Update(const void* pVertexData, int bufferSize, int nVertices);

        void Destroy();
        void Submit(GLenum mode);
        void Submit(ProgramAttribute* pAttribs, int nAttribs);

        static void CreateFromObjFile(const char* pObjFilePath, Geometry** pOutGeometry, int& outNumGeometry);

        unsigned int GetVbId() { return vb_id_; }
        unsigned int GetIbId() { return ib_id_; }
        unsigned int GetVaoId() { return vao_id_; }
        int GetVertexCount() { return vertex_count_; }
        int GetIndexCount() { return index_count_; }

    private:
        unsigned int    vb_id_;
        unsigned int    ib_id_;
        unsigned int    vao_id_;
        int             vertex_count_;
        int             index_count_;
    };

}