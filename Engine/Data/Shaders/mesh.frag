#version 460

layout(std140) uniform MeshSettings 
{
    mat4 model;
    vec4 params; // x = hasTexture, y = hasBones, z = gTargetBone
};

#define TARGET_BONE params.z

layout(std140) uniform LightSettings 
{
    vec3 viewPos;
    int numLights;
};

struct Material 
{
    vec4 emission;
    vec4 params1; // x = shade_wrap, y = ambient_floor, z = spec_size, w = spec_strength
    vec4 params2; // x = shininess, y = toon_steps, z = toon_smoothness
};

#define SHADE_WRAP(m) m.params1.x
#define AMBIENT_FLOOR(m) m.params1.y
#define SPEC_SIZE(m) m.params1.z
#define SPEC_STRENGTH(m) m.params1.w
#define SHININESS(m) m.params2.x
#define TOON_STEPS(m) m.params2.y
#define TOON_SMOOTHNESS(m) m.params2.z

struct LightUniform 
{
    vec4 color;
    vec4 position; 
    vec4 params; // x = intensity, y = lightType
};

#define INTENSITY(l) l.params.x
#define LIGHT_TYPE(l) l.params.y

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

// Toon shading function - creates stepped lighting bands
float toonify(float value, float steps, float smoothness)
{
    float stepSize = 1.0 / steps;
    float stepped = floor(value / stepSize) * stepSize;
    float nextStep = stepped + stepSize;
    
    // Smooth transition at band edges instead of blending to gradient
    float edge = fract(value / stepSize);
    float smoothEdge = smoothstep(0.5 - smoothness, 0.5 + smoothness, edge);
    
    return mix(stepped, nextStep, smoothEdge);
}

vec3 CalcPointLight(LightUniform light, vec3 normal, vec3 color, vec3 viewDir)
{
    vec3 lightDir = normalize(vec3(light.position) - vPosition);
    // diffuse shading
    float NdotL = max(dot(normal, lightDir), 0.0);
    float diff = toonify(NdotL, TOON_STEPS(material), TOON_SMOOTHNESS(material));
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float specRaw = pow(max(dot(viewDir, reflectDir), 0.0), SHININESS(material));
    float spec = step(SPEC_SIZE(material), specRaw); // Hard edge specular
    // attenuation
    float distanceVar = length(vec3(light.position) - vPosition);
    float attenuation = 1.0 / (1.0 + 0.09 * distanceVar + 0.032 * (distanceVar * distanceVar));    
    // combine results
    vec3 ambient = color * AMBIENT_FLOOR(material);
    vec3 diffuse = color * diff * light.color.xyz;
    vec3 specular = light.color.xyz * spec * vec3(SPEC_STRENGTH(material));
    return (attenuation * (ambient + diffuse + specular) * INTENSITY(light)) + material.emission.xyz;
}

vec3 CalcDirectionalLight(LightUniform light, vec3 normal, vec3 color, vec3 viewDir)
{
    vec3 lightDir = normalize(vec3(light.position));

    // Diffuse - wrapped shadows
    float NdotL = dot(normal, lightDir);
    float wrappedDiff = (NdotL + SHADE_WRAP(material)) / (1.0 + SHADE_WRAP(material));
    wrappedDiff = clamp(wrappedDiff, 0.0, 1.0);
    
    // Apply toon banding
    float diff = toonify(wrappedDiff, TOON_STEPS(material), TOON_SMOOTHNESS(material));
    vec3 diffuse = diff * color * light.color.xyz;

    // Ambient
    vec3 ambient = AMBIENT_FLOOR(material) * color;
    
    // Specular
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float specRaw = pow(clamp(dot(normal, halfwayDir), 0.0, 1.0), SPEC_SIZE(material));
    float spec = smoothstep(0.5 - TOON_SMOOTHNESS(material), 0.5 + TOON_SMOOTHNESS(material), specRaw);
    vec3 specular = spec * SPEC_STRENGTH(material) * light.color.xyz;
        
    return ambient + diffuse + specular;
}

void main()
{
    // properties
    vec3 norm = normalize(vNormal);
    vec3 viewDir = normalize(viewPos - vPosition);
    vec3 color = texture(colorMap, vTexCoord).rgb;

    vec3 result = vec3(0.0);
    for(int i = 0; i < numLights; i++) {
        if (LIGHT_TYPE(lights[i]) == 0) {
            result += CalcPointLight(lights[i], norm, color, viewDir);  
        }
        else {
            result += CalcDirectionalLight(lights[i], norm, color, viewDir);  
        }
    }

    if (TARGET_BONE != -1) {
        fragColor = vBoneDebugColor;
    }
    else {
        fragColor = vec4(result, 1.0);
    }
    // Debug: Show UV color coords
    //fragColor = vec4(vTexCoord, 0.0, 1.0);  // Show UVs as colors
}