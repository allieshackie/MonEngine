#version 460

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 emission;
    float shininess;
};

struct LightUniform {
    vec4 position; 
    vec4 color;
    float intensity;
};

uniform vec3 viewPos;
uniform sampler2D colorMap;

layout(std430) buffer LightBuffer
{
    LightUniform light;
};

layout(std430) buffer MaterialBuffer
{
    Material material;
};

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexCoord;

out vec4 fragColor;

void main()
{
    // ambient
    vec3 ambient = material.ambient.xyz * light.color.xyz;
  	
    // diffuse 
    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(light.position.xyz - vPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.color.xyz * (diff * material.diffuse.xyz);
    
    // specular
    vec3 viewDir = normalize(viewPos - vPosition);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = material.specular.xyz * spec * light.color.xyz;  

    vec4 color = texture(colorMap, vTexCoord);
    vec3 result = (ambient + diffuse + specular + material.emission.xyz) * color.rgb;
    fragColor = vec4(result, 1.0);
}