#version 330 core

in vec3 position;
in vec3 normal;
in vec3 color;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

out vec3 vWorldPos;
out vec3 vWorldNormal;
out vec3 vModelColor;

void main()
{
    gl_Position = projectionMatrix * (viewMatrix * (modelMatrix * vec4(position, 1.0)));
    vWorldPos = (modelMatrix * vec4(position.xyz, 1.0)).xyz;
    // Only rotate the rest of these!
    vWorldNormal = (modelMatrix * vec4(normal.xyz, 0.0)).xyz;
    vModelColor = color;

}
