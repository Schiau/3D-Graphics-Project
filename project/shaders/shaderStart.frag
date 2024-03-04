#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec4 fPosEye;
in vec2 fTexCoords;
in vec4 fragPosLightSpace;

out vec4 fColor;

//lighting
uniform	vec3 lightDir;
uniform	vec3 lightColor;

uniform bool isLamp0;
uniform vec3 lightPoint0;
uniform bool isLamp1;
uniform vec3 lightPoint1;
uniform bool isLamp2;
uniform vec3 lightPoint2;
uniform bool isLamp3;
uniform vec3 lightPoint3;

//texture
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

uniform sampler2D shadowMap;

//control
uniform bool isShadow;
uniform bool isDirLight;
uniform bool isPointLight;
uniform bool isFog;
uniform bool isThor;

vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;

float constant = 1.0f;
float linear = 0.027f;
float quadratic = 0.0028f;

void computeDirLightComponents()
{		
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lightDir);
	
	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
		
	//compute ambient light
	ambient = ambientStrength * lightColor;
	
	//compute diffuse light
	diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	//compute specular light
	vec3 reflection = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
	specular = specularStrength * specCoeff * lightColor;
}

float computeShadow()
{
	vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	normalizedCoords = normalizedCoords * 0.5 + 0.5;
	float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
	float currentDepth = normalizedCoords.z;
	float bias = 0.005f;
	float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;
	return shadow;
}

void computePointLightComponents(vec3 lightPosition) {
    float dist = length(lightPosition - fPosition);
    float att = 1.0f / (constant + linear * dist + quadratic * (dist * dist));

    diffuse += att * diffuse * lightColor;
    ambient += att * ambient * lightColor;
    specular += att * specular * lightColor;
}

float computeFog()
{
	float fogDensity = 0.01f;
	float fragmentDistance = length(fPosEye);
	float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

	return clamp(fogFactor, 0.0f, 1.0f);
}

void main() 
{
	computeDirLightComponents();
	float shadow = 0.0f;
	
	if (isShadow)	shadow = computeShadow();
	if (isPointLight){
	if (!isLamp0) computePointLightComponents(lightPoint0);
	if (!isLamp1) computePointLightComponents(lightPoint1);
	if (!isLamp2) computePointLightComponents(lightPoint2);
	if (!isLamp3) computePointLightComponents(lightPoint3);
	}
	
	ambient *= texture(diffuseTexture, fTexCoords).rgb;
	diffuse *= texture(diffuseTexture, fTexCoords).rgb;
	specular *= texture(specularTexture, fTexCoords).rgb;
	
	if(isThor) specular = vec3(0.6f, 0.6f, 0.6f);
	vec3 color = min((1.0f - shadow)*texture(diffuseTexture, fTexCoords).rgb, 1.0f);
	if (isDirLight)	color = min((ambient + (1.0f - shadow)*diffuse) + (1.0f - shadow)*specular, 1.0f);
	
	float fogFactor = 1.0f;
	vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
	if(isFog) fogFactor = computeFog();
	fColor = fogColor*(1-fogFactor)+(vec4(color, 1.0f))*fogFactor;
}
