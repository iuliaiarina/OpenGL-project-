#version 410 core

in vec3 fNormal;  // normala fragmentului
in vec4 fPosEye; // pozita sa in coordonate camera
in vec4 fPos; // pozita sa 
in vec2 fTexCoords; // coordonate textura
in vec4 fragPosLightSpace;  

out vec4 fColor;

//lighting + switches
uniform	vec3 lightPunct1;  // pozitie lumina punct
uniform	vec3 lightPunct1Color;  // culoare lumina punct
uniform	vec3 lightPunct2;  // pozitie lumina punct
uniform	vec3 lightPunct2Color;  // culoare lumina punct
uniform	vec3 lightPunct3;  // pozitie lumina punct
uniform	vec3 lightPunct3Color;  // culoare lumina punct

uniform	vec3 lightDir1;  // pozitie lumina directionale
uniform	vec3 lightColor1;  // culoare lumina directionale
uniform	vec3 lightDir2;  // pozitie lumina punctiforme
uniform	vec3 lightColor2;  // culoare lumina punctiforme

uniform int switch1;
uniform int switch2;
uniform int switchAuto;
uniform int flash;

//texture
uniform sampler2D diffuseTexture;  // textura difuza
uniform sampler2D specularTexture; //speculara
uniform sampler2D shadowMap;  // shadow mapul


vec3 ambient  =vec3(0.0f);
float ambientStrength = 0.2f;
vec3 diffuse =vec3(0.0f);
vec3 specular =vec3(0.0f);
float specularStrength = 0.5f;
float shininess = 32.0f;

float constant = 1.0f;
float linear = 0.22f;
float quadratic = 0.20f;

void computeLightPunct(vec3 lightPunct , vec3 lightColor)
{		
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	vec3 normalEye = normalize(fNormal);	
	vec3 lightDirN = normalize(lightPunct - fPos.xyz);
	vec3 viewDirN = normalize(cameraPosEye - fPos.xyz);
		

	//compute half vector
	vec3 halfVector = normalize(lightDirN + viewDirN);

	//compute distance to light
	float dist = length(lightPunct - fPos.xyz);
	//compute attenuation
	float att = 1.0f / (constant + linear * dist + quadratic * (dist * dist));

	//compute ambient light
	ambient +=  att * ambientStrength * lightColor;
	
	//compute diffuse light
	diffuse += att * max(dot(normalEye, lightDirN), 0.0f) * lightColor;

	//compute specular light
	float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), shininess);
	specular +=  att * specularStrength * specCoeff * lightColor;

}

void computeflash()
{		
	vec3 cameraPosEye = vec3(0.0f);
	vec3 lightColor = vec3(1.0f);
	vec3 normalEye = normalize(fNormal);	
	vec3 lightDirN1 = normalize(lightDir1);
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
	vec3 halfVector = normalize(lightDirN1 + viewDirN);
	ambient += ambientStrength * lightColor;
	diffuse += max(dot(normalEye, lightDirN1), 0.0f) * lightColor;
	float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), shininess);
	specular += specularStrength * specCoeff * lightColor;
}

void computeLightComponents()
{		
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal (normala in spatiu camera)
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction pentru lumina directionala
	vec3 lightDirN1 = normalize(lightDir1);
	//vec3 lightDirN = normalize(lightDir - fPosEye.xyz);
	
	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);

	//compute half vector  pt modelul Bling
	vec3 halfVector = normalize(lightDirN1 + viewDirN);

	//compute ambient light
	ambient = ambientStrength * lightColor1;
	
	//compute diffuse light
	diffuse = max(dot(normalEye, lightDirN1), 0.0f) * lightColor1;
	

	//compute specular light
	float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), shininess);
	specular = specularStrength * specCoeff * lightColor1;
}

void computeLightComponents2()
{		
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lightDir2);
	//vec3 lightDirN = normalize(lightDir2 - fPosEye.xyz);

	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
		

	//compute half vector
	vec3 halfVector = normalize(lightDirN + viewDirN);

	//compute distance to light
	//float dist = length(lightDir2 - fPosEye.xyz);
	//compute attenuation
	//float att = 1.0f / (constant + linear * dist + quadratic * (dist * dist));

	//compute ambient light
	ambient +=  ambientStrength * lightColor2;
	
	//compute diffuse light
	diffuse += max(dot(normalEye, lightDirN), 0.0f) * lightColor2;

	//compute specular light
	float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), shininess);
	specular += specularStrength * specCoeff * lightColor2;

}


float computeShadow()
{
	vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	normalizedCoords = normalizedCoords * 0.5 + 0.5;
	if (normalizedCoords.z > 1.0f)
		return 0.0f;
	float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
	float currentDepth = normalizedCoords.z;
	float bias = 0.005f;
	float shadow= (currentDepth-bias)> closestDepth ? 1.0f : 0.0f;

	return shadow;
}



float computeFog()
{
 float fogDensity = 0.01f; 
 float fragmentDistance = length(fPosEye);
 float fogFactor = exp(-pow(fragmentDistance*fogDensity, 2));
 float res = clamp(fogFactor, 0.0f, 1.0f);
 return res;
}

void main() 
{
	if(switch1==1){  
		computeLightComponents();
	}

	if(switch2==1){
		computeLightComponents2();
	}
	if(switchAuto == 1){
		computeLightPunct( lightPunct1 ,  lightPunct1Color);
	}
	if(flash >= 285 && flash <=300){
		computeflash();
		computeflash();
	}

	computeLightPunct( lightPunct2 ,  lightPunct2Color);
	computeLightPunct( lightPunct3 ,  lightPunct3Color);

	ambient *= texture(diffuseTexture, fTexCoords).rgb;
	diffuse *= texture(diffuseTexture, fTexCoords).rgb;
	specular *= texture(specularTexture, fTexCoords).rgb;

	float shadow = computeShadow();
	vec3 color = min((ambient + (1.0f - shadow)*diffuse) + (1.0f - shadow)*specular, 1.0f);
	vec4 auxColor= vec4(color,1.0f);

	vec4 colorFromTexture = texture(diffuseTexture, fTexCoords);
	if(colorFromTexture.a < 0.001) // alpha mai mic ca 0.1 => trebuie discard shader
		discard;
	
// altfel calculam ceata:
	float fogFactor=computeFog();
	vec4 fogColor=vec4(0.5f, 0.5f, 0.5f, 1.0f);
	fColor = fogColor*(1- fogFactor) + auxColor*fogFactor;
	//fColor= vec4(color,1.0f);
}
