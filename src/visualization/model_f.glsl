#version 330 core

in vec3 vWorldPos;
in vec3 vWorldNormal;
in vec3 vModelColor;

uniform vec3 modelColor;
uniform vec3 eyePos;
uniform vec3 lightPos;

out highp vec4 outColor;

vec4 GetSchlickFresnel(vec3 F0, vec3 Vector1, vec3 Vector2)
{
    float CosTheta = max(0.0, dot(Vector1, Vector2));
    float PowerTerm = pow(1.0 - CosTheta, 5.0);

    vec3 NewColor = F0 + (vec3(1.0, 1.0, 1.0) - F0) * PowerTerm;

    return vec4(NewColor.rgb, 1.0);
}


//--------------------------------------------------------------------------------------
void main()
//--------------------------------------------------------------------------------------
{

    vec4 srcColor = vec4(modelColor.xyz, 1.0);
    vec3 FinalNorm = normalize(vWorldNormal);

    vec3 Light = normalize(lightPos.xyz - vWorldPos.xyz);
    vec3 View = normalize(eyePos.xyz - vWorldPos.xyz);
    vec3 Half = normalize(Light + View);

    //diffuse light
    float DiffuseAmount = dot(FinalNorm, Light.xyz);
    DiffuseAmount = clamp(DiffuseAmount, 0.1, 1.0);

    vec4 Schlick_LdotH = GetSchlickFresnel(vec3(0.04, 0.04, 0.04), Light, Half);

    float AlphaP = 128.0;
    float MicroFacet = max(0.0, dot(FinalNorm, Half));
    MicroFacet = max(0.1, pow(MicroFacet, AlphaP));

    vec4 SpecColor = ((AlphaP + 2.0) / 8.0) * MicroFacet * Schlick_LdotH;


    vec4 lightColor = vec4(1.0, 1.0, 1.0, 1.0);
    vec4 FinalColor = vec4(srcColor.xyz, 1.0) + vec4(lightColor.xyz * SpecColor.xyz, 1.0);
    FinalColor = DiffuseAmount * FinalColor * lightColor;

    outColor = DiffuseAmount * srcColor;

}

