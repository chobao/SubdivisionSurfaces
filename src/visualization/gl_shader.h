//=============================================================================
// FILE: svrShader.h
//
//                  Copyright (c) 2015 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//=============================================================================
#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "utils/hash_table.h"

#define MAX_UNIFORM_NAME_LENGTH 64

// These are no longer defined in GL3Ext.h
#ifndef GL_TEXTURE_EXTERNAL_OES
#define GL_TEXTURE_EXTERNAL_OES           0x8D65
#endif

#ifndef GL_SAMPLER_EXTERNAL_OES
#define GL_SAMPLER_EXTERNAL_OES           0x8D66
#endif

namespace GLRendering
{
    using CommonUtils::HashTable;
    using CommonUtils::DjB2Hash;

    enum AttributeLocation
    {
        kPosition = 0,
        kNormal = 1,
        kColor = 2,
        kTexcoord0 = 3,
        kTexcoord1 = 4
    };

    struct ShaderAttribute
    {
        AttributeLocation    location;
        const char*             name;
    };

    struct ShaderUniform
    {
        unsigned int location;
        GLenum       type;
        char         name[MAX_UNIFORM_NAME_LENGTH];
        unsigned int textureUnit;
    };

    class Shader
    {
    public:
        Shader();

        bool Initialize(int numVertStrings, const char** pVertSrc, int numFragStrings, const char** pFragSrc, const char* pVertDbgName = NULL, const char* pFragDbgName = NULL);
        void Destroy();
        void Bind();
        void Unbind();

        void SetUniformMat2(const char* name, glm::mat2& matrix);
        void SetUniformMat2(int location, glm::mat2& matrix);
        void SetUniformMat2fv(const char* name, unsigned int count, float *pData);
        void SetUniformMat2fv(int location, unsigned int count, float *pData);
        void SetUniformMat3(const char* name, glm::mat3& matrix);
        void SetUniformMat3(int location, glm::mat3& matrix);
        void SetUniformMat4(const char* name, glm::mat4& matrix);
        void SetUniformMat4(int location, glm::mat4& matrix);
        void SetUniformMat4fv(const char* name, unsigned int count, float *pData);
        void SetUniformMat4fv(int location, unsigned int count, float *pData);
        void SetUniformVec4(const char* name, glm::vec4& vector);
        void SetUniformVec4(int location, glm::vec4& vector);
        void SetUniformVec3(const char* name, glm::vec3& vector);
        void SetUniformVec3(int location, glm::vec3& vector);
        void SetUniformVec2(const char* name, glm::vec2& vector);
        void SetUniformVec2(int location, glm::vec2& vector);
        void SetUniform1ui(const char* name, unsigned int value);
        void SetUniform1ui(int location, unsigned int value);
        void SetUniformSampler(const char* name, unsigned int samplerId, unsigned int samplerType, GLuint samplerObjId);

        unsigned int GetShaderId() { return mShaderId;  }

    private:
        static unsigned int gCurrentBoundShader;

        typedef HashTable<unsigned int, ShaderUniform, DjB2Hash> UniformMap;
        typedef HashTable<unsigned int, unsigned int, DjB2Hash> SamplerMap;

        unsigned int    mRefCount;

        unsigned int    mShaderId;
        unsigned int    mVsId;
        unsigned int    mFsId;
        UniformMap      mUniformMap;
    };
}
