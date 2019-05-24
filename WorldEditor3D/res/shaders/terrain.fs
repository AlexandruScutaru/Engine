#version 330 core

in vec2 TexCoords; 
in vec3 Normal;
in vec3 FragPos;
in float visibility;

out vec4 FragColor;

//types of light casters
struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct PointLight {    
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	vec3 att;
};
struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
	vec3 att;   
};

#define NR_POINT_LIGHTS 4

//texture uniforms
uniform sampler2D baseTexture;
uniform sampler2D rTexture;
uniform sampler2D gTexture;
uniform sampler2D bTexture;
uniform sampler2D blendMap;
//data
uniform vec3 viewPos;
uniform DirLight dirLight;
uniform SpotLight spotLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform bool flashlightOn;
uniform int pointLightsNum;
uniform float tiling_factor;
uniform vec3 fog_color;

//function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 color);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 color);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 color);


void main(){
	//blend the textures
	vec4 blendMapColor = texture(blendMap, TexCoords);
	float baseTextureAmount = 1 - (blendMapColor.r + blendMapColor.g + blendMapColor.b);
	vec2 tiledCoords = TexCoords * tiling_factor;
	vec4 baseTextureColor = texture(baseTexture, tiledCoords) * baseTextureAmount;
	vec4 rTextureColor = texture(rTexture, tiledCoords) * blendMapColor.r;
	vec4 gTextureColor = texture(gTexture, tiledCoords) * blendMapColor.g;
	vec4 bTextureColor = texture(bTexture, tiledCoords) * blendMapColor.b;
	vec4 finalColor = baseTextureColor + rTextureColor + gTextureColor + bTextureColor;
	
	// properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // phase 1: Directional lighting
    vec3 result = CalcDirLight(dirLight, norm, viewDir, finalColor.xyz);
    // phase 2: Point lights
	for(int i = 0; i < pointLightsNum; i++)
		result += CalcPointLight(pointLights[i], norm, FragPos, viewDir, finalColor.xyz);    

    // phase 3: Spot light
	if(flashlightOn)
		result += CalcSpotLight(spotLight, norm, FragPos, viewDir, finalColor.xyz);    
    
	FragColor = vec4(result, 1.0);

	FragColor = mix(vec4(fog_color, 1.0), FragColor, visibility);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 color){
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0);
    // combine results
    vec3 ambient  = light.ambient  * color;
    vec3 diffuse  = light.diffuse  * diff * color;
    vec3 specular = light.specular * spec * vec3(0.03, 0.03, 0.03);

	return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 color){
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.att.x + light.att.y * distance + light.att.z * (distance * distance));   
    // combine results
    vec3 ambient  = light.ambient  * color;
    vec3 diffuse  = light.diffuse  * diff * color;
    vec3 specular = light.specular * spec * vec3(0.03, 0.03, 0.03);
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 color){
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.att.x + light.att.y * distance + light.att.z * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * color;
    vec3 diffuse = light.diffuse * diff * color;
    vec3 specular = light.specular * spec * vec3(0.03, 0.03, 0.03);
    ambient  *= attenuation * intensity;
    diffuse  *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}
