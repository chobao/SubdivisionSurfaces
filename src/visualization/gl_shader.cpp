//=============================================================================
// FILE: Shader.cpp
//
//                  Copyright (c) 2015 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//=============================================================================

#include "gl_shader.h"
#include "gl_utils.h"


#include <unistd.h>     // Need gettid()
#include <sys/syscall.h>

namespace GLRendering
{

static ShaderAttribute gDefaultAttributes[] =
{
    { kPosition, "position"},
    { kNormal, "normal"},
    { kColor, "color"},
    { kTexcoord0, "texcoord0"},
    { kTexcoord1, "texcoord1"}
};

bool IsSamplerType(unsigned int type)
{
    if( type == GL_SAMPLER_2D ||
        type == GL_SAMPLER_2D_ARRAY ||
        type == GL_SAMPLER_EXTERNAL_OES ||
        type == GL_SAMPLER_3D ||
        type == GL_SAMPLER_CUBE )
    {
        return true;
    }
    return false;
}

unsigned int Shader::gCurrentBoundShader = 0;


Shader::Shader()
    : mShaderId(0)
    , mVsId(0)
    , mFsId(0)
{
    mRefCount = 0;
    mUniformMap.Init(32);
}

bool Shader::Initialize(int numVertStrings, const char** pVertSrc, int numFragStrings, const char** pFragSrc, const char* pVertDbgName, const char* pFragDbgName)
{
    static char errMsg[4096];
    int result;

    bool fLogShaderInit = false;

    if (fLogShaderInit)
    {
        if (pVertDbgName && pFragDbgName)
            LOG("Loading Shader: %s <=> %s", pVertDbgName, pFragDbgName);
        else
            LOG("Loading Shader: Unknown <=> Unknown");
    }

    mVsId = GL(glCreateShader( GL_VERTEX_SHADER ));
    GL(glShaderSource(mVsId, numVertStrings, pVertSrc, 0));
    GL(glCompileShader( mVsId ));
    GL(glGetShaderiv( mVsId, GL_COMPILE_STATUS, &result ));
    if( result == GL_FALSE )
    {
        errMsg[0] = 0;
        GL(glGetShaderInfoLog( mVsId, sizeof(errMsg), 0, errMsg));
        if (pVertDbgName)
            LOG("%s : %s\n", pVertDbgName, errMsg);
        else
            LOG("Compile Error : %s\n", errMsg);
        return false;
    }

    mFsId = GL(glCreateShader( GL_FRAGMENT_SHADER ));
    GL(glShaderSource(mFsId, numFragStrings, pFragSrc, 0));
    GL(glCompileShader( mFsId ));
    GL(glGetShaderiv( mFsId, GL_COMPILE_STATUS, &result ));
    if( result == GL_FALSE )
    {
        errMsg[0] = 0;
        GL(glGetShaderInfoLog( mFsId, sizeof(errMsg), 0, errMsg));
        if (pFragDbgName)
            LOG("%s : %s\n", pFragDbgName, errMsg);
        else
            LOG("Compile Error : %s\n", errMsg);
        return false;
    }

    mShaderId = GL(glCreateProgram());
    GL(glAttachShader( mShaderId, mVsId ));
    GL(glAttachShader( mShaderId, mFsId ));

    for ( unsigned int i = 0; i < sizeof( gDefaultAttributes ) / sizeof( gDefaultAttributes[0] ); i++ )
	{
        if (fLogShaderInit) LOG("Shader %d: %s => %d", mShaderId, gDefaultAttributes[i].name, gDefaultAttributes[i].location);
        GL(glBindAttribLocation(mShaderId, gDefaultAttributes[i].location, gDefaultAttributes[i].name));
	}

    GL(glLinkProgram( mShaderId ));
    GL(glGetProgramiv( mShaderId, GL_LINK_STATUS, &result));
    if( result == GL_FALSE )
    {
        errMsg[0] = 0;
        GL(glGetProgramInfoLog( mShaderId, sizeof(errMsg), 0, errMsg));
        if (pVertDbgName && pFragDbgName)
            LOG("Link (%s,%s) : %s\n", pVertDbgName, pFragDbgName, errMsg);
        else
            LOG("Link Error : %s\n", errMsg);
        return false;
    }

    if (fLogShaderInit) LOG("    Shader (Handle = %d) Linked: %s <=> %s", mShaderId, pVertDbgName, pFragDbgName);

    int curTextureUnit = 0;
    int nActiveUniforms;
    GL(glGetProgramiv( mShaderId, GL_ACTIVE_UNIFORMS, &nActiveUniforms ));
    if (fLogShaderInit) LOG("    Shader (Handle = %d) has %d active uniforms", mShaderId, nActiveUniforms);

    GL( glUseProgram( mShaderId ) );

    char nameBuffer[MAX_UNIFORM_NAME_LENGTH];
    for( int i=0; i < nActiveUniforms; i++ )
    {
        int uniformNameLength, uniformSize;
        GLenum uniformType;
        GL(glGetActiveUniform( mShaderId, i, MAX_UNIFORM_NAME_LENGTH - 1, &uniformNameLength, &uniformSize, &uniformType, &nameBuffer[0]));
        nameBuffer[uniformNameLength] = 0;
        unsigned int location = GL(glGetUniformLocation( mShaderId, nameBuffer ));

        ShaderUniform uniform;
        uniform.location = location;
        uniform.type = uniformType;
        strncpy( uniform.name, nameBuffer, MAX_UNIFORM_NAME_LENGTH );

        if (fLogShaderInit)
        {
            switch (uniformType)
            {
            case GL_FLOAT_VEC2:             // 0x8B50
                LOG("    Uniform (%s) is a type GL_FLOAT_VEC2 => Bound to location %d", nameBuffer, location);
                break;
            case GL_FLOAT_VEC3:             // 0x8B51
                LOG("    Uniform (%s) is a type GL_FLOAT_VEC3 => Bound to location %d", nameBuffer, location);
                break;
            case GL_FLOAT_VEC4:             // 0x8B52
                LOG("    Uniform (%s) is a type GL_FLOAT_VEC4 => Bound to location %d", nameBuffer, location);
                break;
            case GL_FLOAT_MAT2:
                LOG("    Uniform (%s) is a type GL_FLOAT_MAT2 => Bound to location %d", nameBuffer, location);
                break;
            case GL_FLOAT_MAT4:             // 0x8B5C
                LOG("    Uniform (%s) is a type GL_FLOAT_MAT4 => Bound to location %d", nameBuffer, location);
                break;
            case GL_SAMPLER_2D:             // 0x8B5E
                LOG("    Uniform (%s) is a type GL_SAMPLER_2D => Bound to location %d", nameBuffer, location);
                break;
            case GL_SAMPLER_CUBE:           // 0x8B60
                LOG("    Uniform (%s) is a type GL_SAMPLER_CUBE => Bound to location %d", nameBuffer, location);
                break;
            case GL_SAMPLER_EXTERNAL_OES:   // 0x8D66
                LOG("    Uniform (%s) is a type GL_SAMPLER_EXTERNAL_OES => Bound to location %d", nameBuffer, location);
                break;
            case GL_SAMPLER_2D_ARRAY:       // 0x8DC1
                LOG("    Uniform (%s) is a type GL_SAMPLER_2D_ARRAY => Bound to location %d", nameBuffer, location);
                break;

            default:
                LOG("    Uniform (%s) is a type 0x%x => Bound to location %d", nameBuffer, uniformType, location);
                break;
            }

        }


        if (IsSamplerType(uniformType))
        {
            GL(glUniform1i( location, curTextureUnit));
            uniform.textureUnit = curTextureUnit++;
            if (fLogShaderInit) LOG("        Uniform (%s) is a texture in unit %d", nameBuffer, uniform.textureUnit);
        }
        else
        {
            uniform.textureUnit = 0;
            if (fLogShaderInit) LOG("        Uniform (%s) is NOT a texture in unit %d", nameBuffer, uniform.textureUnit);
        }

        if (fLogShaderInit) LOG("    Inserting (%s) into uniform map...", nameBuffer);
        mUniformMap.Insert( &nameBuffer[0], uniform );
    }

    GL( glUseProgram( 0 ) );

    // Always want this log message
    if (true || fLogShaderInit)
    {
        if (pVertDbgName && pFragDbgName)
            LOG("Loaded Shader (%d): %s <=> %s\n", mShaderId, pVertDbgName, pFragDbgName);
        else
            LOG("Loaded Shader (%d): Unknown <=> Unknown\n", mShaderId);
    }

    return true;
}

void Shader::Destroy()
{
    if (mShaderId != 0)
    {
        GL(glDeleteProgram(mShaderId));
    }

    if (mVsId != 0)
    {
        GL(glDeleteShader(mVsId));
    }

    if (mFsId != 0)
    {
        GL(glDeleteShader(mFsId));
    }


    mShaderId = 0;
    mVsId = 0;
    mFsId = 0;
}

void Shader::Bind()
{
    // Here is the problem: gCurrentBoundShader is global across all threads!
    // This means there are collisions between threads that are not really collisions.
    // Therefore we can't check this for the spacewarp branch :(
    // if (gCurrentBoundShader != 0)
    // {
    //     LOG("Warning! (ThreadID = 0x%08x) Shader (Handle = %d) is already bound while binding new shader (Handle = %d)", gettid(), gCurrentBoundShader, mShaderId);
    // }

    if (mRefCount != 0)
    {
        LOG("Warning! (ThreadID = ) Shader (Handle = %d) is being bound without being unbound. Bind count = %d",  mShaderId, mRefCount);
    }
    mRefCount = mRefCount + 1;

    gCurrentBoundShader = mShaderId;

    // LOG("Binding Shader (Handle = %d). Bind count = %d", mShaderId, mRefCount);
    GL( glUseProgram( mShaderId ) );
}

void Shader::Unbind()
{
    if (mRefCount == 0)
    {
        LOG("Warning! Shader (Handle = %d) is being unbound without being bound.", mShaderId);
    }
    else
    {
        mRefCount = mRefCount - 1;
    }

    gCurrentBoundShader = 0;

    // LOG("Unbinding Shader (Handle = %d). Bind count = %d", mShaderId, mRefCount);
    GL(glUseProgram(0));

    // Unbind all samplers
    UniformMap::Iterator uniformIter = mUniformMap.GetIterator();
    while (!uniformIter.End())
    {
        ShaderUniform oneEntry = uniformIter.Current();

        if (IsSamplerType(oneEntry.type))
        {
            // LOG("Uniform Iterator Entry:");
            // LOG("    name = %s", oneEntry.name);
            // LOG("    location = %d", oneEntry.location);
            // LOG("    type = %d", oneEntry.type);
            // LOG("    textureUnit = %d", oneEntry.textureUnit);
            GL(glBindSampler(oneEntry.textureUnit, 0));
        }

        uniformIter.Next();
    }
}

void Shader::SetUniformMat2(const char* name, glm::mat2& matrix)
{
    ShaderUniform  uniform;
    if( mUniformMap.Find( name, &uniform))
    {
        SetUniformMat2(uniform.location, matrix);
    }
}

void Shader::SetUniformMat2(int location, glm::mat2& matrix)
{
    GL(glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(matrix)));
}

void Shader::SetUniformMat2fv(const char* name, unsigned int count, float *pData)
{
    ShaderUniform  uniform;
    if (mUniformMap.Find(name, &uniform))
    {
        SetUniformMat2fv(uniform.location, count, pData);
    }
    //else
    //{
    //    LOG("Failed to set uniform %s on shader %d", name, mShaderId);
    //}
}

void Shader::SetUniformMat2fv(int location, unsigned int count, float *pData)
{
    GL(glUniformMatrix2fv(location, count, GL_FALSE, pData));
}

void Shader::SetUniformMat3(const char* name, glm::mat3& matrix)
{
    ShaderUniform  uniform;
    if (mUniformMap.Find(name, &uniform))
    {
        SetUniformMat3(uniform.location, matrix);
    }
}

void Shader::SetUniformMat3(int location, glm::mat3& matrix)
{
    GL(glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix)));
}

void Shader::SetUniformMat4(const char* name, glm::mat4& matrix)
{
    ShaderUniform  uniform;
    if( mUniformMap.Find( name, &uniform ) )
    {
        SetUniformMat4(uniform.location, matrix);
    }
    else
    {
       LOG("Failed to set uniform %s on shader %d\n", name, mShaderId);
    }
}

void Shader::SetUniformMat4(int location, glm::mat4& matrix)
{
    GL(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix)));
}

void Shader::SetUniformMat4fv(const char* name, unsigned int count, float *pData)
{
    ShaderUniform  uniform;
    if (mUniformMap.Find(name, &uniform))
    {
        SetUniformMat4fv(uniform.location, count, pData);
    }
    //else
    //{
    //    LOG("Failed to set uniform %s on shader %d", name, mShaderId);
    //}
}

void Shader::SetUniformMat4fv(int location, unsigned int count, float *pData)
{
    GL(glUniformMatrix4fv(location, count, GL_FALSE, pData));
}

void Shader::SetUniformVec4(const char* name, glm::vec4& vector)
{
    ShaderUniform uniform;
    if(mUniformMap.Find( name, &uniform))
    {
        SetUniformVec4(uniform.location, vector);
    }
    //else
    //{
    //    LOG("Failed to set uniform %s on shader %d", name, mShaderId);
    //}
}

void Shader::SetUniformVec4(int location, glm::vec4& vector)
{
    GL(glUniform4fv(location, 1, glm::value_ptr(vector)));
}

void Shader::SetUniformVec3(const char* name, glm::vec3& vector)
{
    ShaderUniform uniform;
    if (mUniformMap.Find(name, &uniform))
    {
        SetUniformVec3(uniform.location, vector);
    }
}

void Shader::SetUniformVec3(int location, glm::vec3& vector)
{
    GL(glUniform3fv(location, 1, glm::value_ptr(vector)));
}

void Shader::SetUniformVec2(const char* name, glm::vec2& vector)
{
    ShaderUniform uniform;
    if (mUniformMap.Find(name, &uniform))
    {
        SetUniformVec2(uniform.location, vector);
    }
}

void Shader::SetUniformVec2(int location, glm::vec2& vector)
{
    GL(glUniform2fv(location, 1, glm::value_ptr(vector)));
}

void Shader::SetUniform1ui(const char* name, unsigned int value)
{
    ShaderUniform uniform;
    if (mUniformMap.Find(name, &uniform))
    {
        SetUniform1ui(uniform.location, value);
    }    
}

void Shader::SetUniform1ui(int location, unsigned int value)
{
    GL(glUniform1ui(location, value));
}

void Shader::SetUniformSampler(const char* name, unsigned int samplerId, unsigned int samplerType, GLuint samplerObjId)
{
    bool fLogTextureInfo = false;

    ShaderUniform uniform;
    if(mUniformMap.Find( name, &uniform))
    {
        if (fLogTextureInfo)
        {
            switch (samplerType)
            {
            case GL_TEXTURE_2D:
                LOG("    %s: Texture Unit = %d; Sampler = %d; Type = GL_TEXTURE_2D", name, uniform.textureUnit, samplerId);
                break;

            case GL_TEXTURE_CUBE_MAP:
                LOG("    %s: Texture Unit = %d; Sampler = %d; Type = GL_TEXTURE_CUBE_MAP", name, uniform.textureUnit, samplerId);
                break;

            case GL_TEXTURE_EXTERNAL_OES:
                LOG("    %s: Texture Unit = %d; Sampler = %d; Type = GL_TEXTURE_EXTERNAL_OES", name, uniform.textureUnit, samplerId);
                break;

            default:
                LOG("    %s: Texture Unit = %d; Sampler = %d; Type = %d", name, uniform.textureUnit, samplerId, samplerType);
                break;
            }
        }

        // It turns out the driver does not handle samplers on image textures :(
        if (samplerObjId != 0 && samplerType != GL_TEXTURE_EXTERNAL_OES)
        {
            // LOG("Binding sampler %d to unit %d", samplerObjId, uniform.textureUnit);
            GL(glBindSampler(uniform.textureUnit, samplerObjId));
        }

        GL(glActiveTexture(GL_TEXTURE0 + uniform.textureUnit));
        GL(glBindTexture(samplerType, samplerId));
    }
    // else
    // {
    //     LOG("Failed to set uniform %s on shader %d", name, mShaderId);
    // }
}


}
