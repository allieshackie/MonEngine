#version 460

layout(std140) uniform MeshSettings 
{
    mat4 model;
    vec4 params; // x = hasTexture, y = hasBones, z = gTargetBone
};

layout(std140) uniform LightSettings 
{
    vec3 viewPos;
    int numLights;
};

struct Material 
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 emission;
    float shininess;
};

struct LightUniform 
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec3 position; 
    float intensity;
    vec4 lightType;
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
in vec4 vBoneDebugColor;

out vec4 fragColor;

vec3 CalcPointLight(LightUniform light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcDirectionalLight(LightUniform light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    // properties
    vec3 norm = normalize(vNormal);
    vec3 viewDir = normalize(viewPos - vPosition);
    vec3 color = texture(colorMap, vTexCoord).rgb;

    vec3 result = vec3(0.0);
    for(int i = 0; i < numLights; i++) {
        if (lights[i].lightType[0] == 0) {
            result += CalcPointLight(lights[i], norm, vPosition, viewDir);  
        }
        else {
            result += CalcDirectionalLight(lights[i], norm, vPosition, viewDir);  
        }
    }

    if (params.z != -1) {
        fragColor = vBoneDebugColor;
    }
    else {
        fragColor = vec4(result * color, 1.0);
    }
    // Debug: Show UV color coords
    //fragColor = vec4(vTexCoord, 0.0, 1.0);  // Show UVs as colors
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
    vec3 ambient = light.ambient.xyz * material.diffuse.xyz;
    vec3 diffuse = light.diffuse.xyz * diff * material.diffuse.xyz;
    vec3 specular = light.specular.xyz * spec * material.specular.xyz;
    return (attenuation * (ambient + diffuse + specular) * light.intensity) + material.emission.xyz;
}

vec3 CalcDirectionalLight(LightUniform light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    // ambient
    vec3 ambient = light.ambient.xyz * material.diffuse.xyz;
  	
    // diffuse 
    vec3 lightDir = normalize(-light.position);  
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse.xyz * diff * material.diffuse.xyz;
    
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular.xyz * spec * material.specular.xyz;
        
    return (ambient + diffuse + specular) * light.intensity;
}