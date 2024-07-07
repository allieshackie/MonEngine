#version 460

layout(std140) uniform Settings 
{
    mat4 model;
    mat4 view;
    mat4 projection;
    mat4 textureClip;
    vec3 lightPos; 
    float padding1; // Padding for alignment
    vec3 viewPos; 
    float padding2; // Padding for alignment
    vec3 lightColor;
    float padding3; // Padding for alignment
};

uniform sampler2D colorMap;

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexCoord;

out vec4 fragColor;

// Commented values with K represent material properties.  Need to implement refraction to get full material

void main()
{
    // ambient
    vec3 matAmbient = vec3(1.0, 1.0, 1.0); // Ka
    vec3 ambient = matAmbient * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(lightPos - vPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 matDiffuse = vec3(0.8, 0.8, 0.8); // Kd
    vec3 diffuse = lightColor * (diff * matDiffuse);
    
    // specular
    vec3 matSpecular = vec3(0.5, 0.5, 0.5); // Ks
    vec3 viewDir = normalize(viewPos - vPosition);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 360); // 360 is material shininess
    vec3 specular = matSpecular * spec * lightColor;  
    vec3 emission = vec3(0.0, 0.0, 0.0); // Ke

    vec4 color = texture(colorMap, vTexCoord);
    vec3 result = (ambient + diffuse + specular + emission) * color.rgb;
    fragColor = vec4(result, 1.0);
}