#version 330 core

in vec3 vWorldPos;
in vec3 vWorldNormal;
in vec3 vModelColor;

out highp vec4 outColor;


//--------------------------------------------------------------------------------------
void main()
//--------------------------------------------------------------------------------------
{
    // **********************
    // Texture Value
    // **********************

     vec4 srcColor = vec4(vModelColor.xyz, 1.0);

    outColor = srcColor;
}

