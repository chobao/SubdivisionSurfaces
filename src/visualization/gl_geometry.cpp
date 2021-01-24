#include "gl_geometry.h"

namespace GLRendering{
    Geometry::Geometry()
        : vb_id_(0)
        , ib_id_(0)
        , vao_id_(0)
        , vertex_count_(0)
        , index_count_(0)
    {

    }

    void Geometry::Initialize(ProgramAttribute* pAttribs, int nAttribs,
                                unsigned int* pIndices, int nIndices,
                                const void* pVertexData, int bufferSize, int nVertices)
    {
        //Create the VBO
        GL(glGenBuffers( 1, &vb_id_));
        GL(glBindBuffer( GL_ARRAY_BUFFER, vb_id_ ));
        GL(glBufferData( GL_ARRAY_BUFFER, bufferSize, pVertexData, GL_STATIC_DRAW));
        GL(glBindBuffer( GL_ARRAY_BUFFER, 0));

        //Create the Index Buffer
        GL(glGenBuffers( 1, &ib_id_));
        GL(glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ib_id_ ));
        GL(glBufferData( GL_ELEMENT_ARRAY_BUFFER, nIndices * sizeof(unsigned int), pIndices, GL_STATIC_DRAW));
        GL(glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0));

        //Create the VAO
        GL(glGenVertexArrays( 1, &vao_id_ ));

        GL(glBindVertexArray( vao_id_ ));

        GL(glBindBuffer( GL_ARRAY_BUFFER, vb_id_ ));

        for ( int i = 0; i < nAttribs; i++ )
        {
            GL(glEnableVertexAttribArray( pAttribs[i].index ));
            GL(glVertexAttribPointer(pAttribs[i].index, pAttribs[i].size,
                pAttribs[i].type, pAttribs[i].normalized,
                pAttribs[i].stride, (void*)(unsigned long long)(pAttribs[i].offset)));
        }

        GL(glBindBuffer(GL_ARRAY_BUFFER, 0));

        GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib_id_));

        GL(glBindVertexArray( 0 ));

        vertex_count_ = nVertices;
        index_count_ = nIndices;
    }

    void Geometry::Update(const void* pVertexData, int bufferSize, int nVertices)
    {
        GL(glBindBuffer(GL_ARRAY_BUFFER, vb_id_));
        GL(glBufferData(GL_ARRAY_BUFFER, bufferSize, pVertexData, GL_STATIC_DRAW));
        GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    }

    void Geometry::Destroy()
    {
        GL(glDeleteBuffers( 1, &ib_id_ ));
        GL(glDeleteBuffers( 1, &vb_id_ ));

        vb_id_ = 0;
        ib_id_ = 0;
        vao_id_ = 0;
        vertex_count_ = 0;
        index_count_ = 0;
    }

    void Geometry::Submit(GLenum mode)
    {
        if(mode == GL_LINES || mode == GL_LINE_LOOP) {
            GL(glLineWidth(10));
        }
        GL( glBindVertexArray( vao_id_ ) );
        GL( glDrawElements(mode, index_count_, GL_UNSIGNED_INT, NULL) );
    //    GL( glDrawElements(GL_TRIANGLES, index_count_, GL_UNSIGNED_INT, NULL) );
        GL( glBindVertexArray( 0 ) );
    }

    void Geometry::Submit(ProgramAttribute* pAttribs, int nAttribs)
    {
        GL(glBindBuffer(GL_ARRAY_BUFFER, vb_id_));

        for (int i = 0; i < nAttribs; i++)
        {
            GL(glEnableVertexAttribArray(pAttribs[i].index));
            GL(glVertexAttribPointer(pAttribs[i].index, pAttribs[i].size,
                pAttribs[i].type, pAttribs[i].normalized,
                pAttribs[i].stride, (void*)(unsigned long long)(pAttribs[i].offset)));
        }

        GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib_id_));

        GL(glDrawElements(GL_TRIANGLES, index_count_, GL_UNSIGNED_INT, NULL));


        for (int i = 0; i < nAttribs; i++)
        {
            GL(glDisableVertexAttribArray(pAttribs[i].index));
        }

        GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
        GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

    }
}