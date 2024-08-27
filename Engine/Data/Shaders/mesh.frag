#version 460

layout(std140) uniform LightSettings 
{
    vec3 viewPos;
    int numLights;
};

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 emission;
    float shininess;
};

struct LightUniform {
    vec3 position; 
    float intensity;
    vec4 color;
};

uniform sampler2D colorMap;

layout(std430) buffer LightBuffer
{
    LightUniform lights[];
};

layout(std430) buffer MaterialBuffer
{
    Material material;
};

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexCoord;
in vec4 vTestColor;

out vec4 fragColor;

vec3 CalcPointLight(LightUniform light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    // properties
    vec3 norm = normalize(vNormal);
    vec3 viewDir = normalize(viewPos - vPosition);
    vec4 color = texture(colorMap, vTexCoord);
    vec3 result = vec3(0.0);

    for(int i = 0; i < numLights; i++) {
        result += CalcPointLight(lights[i], norm, vPosition, viewDir);  
    }

    fragColor = vec4(result * color.rgb, 1.0);
    // DEBUG BONE WEIGHT
    //fragColor = vTestColor;
}

vec3 CalcPointLight(LightUniform light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));    
    // combine results
    vec3 ambient = light.color.xyz * material.diffuse.xyz;
    vec3 diffuse = light.color.xyz * diff * material.diffuse.xyz;
    vec3 specular = light.color.xyz * spec * material.specular.xyz;
    return attenuation * (ambient + diffuse + specular) + material.emission.xyz;
}