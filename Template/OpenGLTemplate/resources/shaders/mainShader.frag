#version 400 core

in vec3 vColour;			// Interpolated colour using colour calculated in the vertex shader
in vec2 vTexCoord;			// Interpolated texture coordinate using texture coordinate from the vertex shader

out vec4 vOutputColour;		// The output colour

uniform sampler2D sampler0;  // The texture sampler
uniform samplerCube CubeMapTex;
uniform bool bUseTexture;    // A flag indicating if texture-mapping should be applied
uniform bool renderSkybox;
in vec3 worldPosition;
in vec3 worldNormal;

const vec3 fogColour = vec3(0.25, 0.25,0.25);

// moved light ambient outside of struct
//uniform vec3 La;

// Structure holding light information:  its position as well as ambient, diffuse, and specular colours
struct LightInfo
{
	vec4 position;
	vec3 La;
	vec3 Ld;
	vec3 Ls;
	vec3 direction;
	float exponent;
	float cutoff;
};

// Structure holding material information:  its ambient, diffuse, and specular colours, and shininess
struct MaterialInfo
{
	vec3 Ma;
	vec3 Md;
	vec3 Ms;
	float shininess;
};

// Lights and materials passed in as uniform variables from client programme
uniform LightInfo light1; 
uniform LightInfo light2;
uniform LightInfo light3;

uniform MaterialInfo material1; 
uniform float ambientBrightness;

in vec3 eyeNorm;
in vec4 eyePosition;
float fIntensity = 0;

// This function implements the Phong shading model
// The code is based on the OpenGL 4.0 Shading Language Cookbook, Chapter 2, pp. 62 - 63, with a few tweaks. 
// Please see Chapter 2 of the book for a detailed discussion.
/*
vec3 PhongModel(vec4 eyePosition, vec3 eyeNorm)
{
	vec3 s = normalize(vec3(light1.position - eyePosition));
	vec3 v = normalize(-eyePosition.xyz);
	vec3 r = reflect(-s, eyeNorm);
	vec3 n = eyeNorm;
	vec3 ambient = light1La * material1.Ma;
	float sDotN = max(dot(s, n), 0.0f);
	vec3 diffuse = light1.Ld * material1.Md * sDotN;
	vec3 specular = vec3(0.0f);
	float eps = 0.000001f; // add eps to shininess below -- pow not defined if second argument is 0 (as described in GLSL documentation)
	if (sDotN > 0.0f) 
		specular = light1.Ls * material1.Ms * pow(max(dot(r, v), 0.0f), material1.shininess + eps);
	

	return ambient + diffuse + specular;

}
*/
vec3 BlinnPhongSpotlightModel(LightInfo light, vec4 p, vec3 n)
{
	vec3 s = normalize(vec3(light.position - p));
	float angle = acos(dot(-s, light.direction));
	float cutoff = radians(clamp(light.cutoff, 0.0, 90.0));
	vec3 ambient = vec3(0.1,0.1,0.1);
	float diffuseIntensity = max(dot(s,n),0.0);
	float specularIntensity = 0;
	
	
	if (angle < cutoff) {
		float spotFactor = pow(dot(-s, light.direction), light.exponent);
		vec3 v = normalize(-p.xyz);
		vec3 h = normalize(v + s);	
		
		float sDotN = max(dot(s, n), 0.0);
		vec3 diffuse = light.Ld * material1.Md * sDotN;
		vec3 specular = vec3(0.0);
		ambient = light.La * material1.Ma;
		if (sDotN > 0.0)
		{
			specular = light.Ls * material1.Ms * pow(max(dot(h, n), 0.0), material1.shininess);
			specularIntensity = pow(max(dot(h,n),0.0), material1.shininess);
			fIntensity = specularIntensity + diffuseIntensity;
			return   ambient + spotFactor * (diffuse + specular);
		}
			
		
	} 
		fIntensity = specularIntensity + diffuseIntensity;
		return ambientBrightness * ambient;
}



// exponential fog shader is based on the materials from this website: http://in2gpu.com/2014/07/22/create-fog-shader/
float fog()
{
	
	float dist = length(eyePosition);
	float fogAmt = 1.0 /exp((dist * 0.008)*(dist * 0.008));
	fogAmt = clamp( fogAmt, 0.0, 1.0 );
	
	return fogAmt;

}

vec3 toon(vec3 c)
{
	float levels =4;
	return floor(c * levels)/levels;
	
}

void main()
{


	if (renderSkybox) {
		vOutputColour = texture(CubeMapTex, worldPosition);
		//vOutputColour = mix(vec4(fogColour,1), vOutputColour, 0.7);

	} else {
		vec3 lightColour = BlinnPhongSpotlightModel(light1, eyePosition, normalize(eyeNorm)) + BlinnPhongSpotlightModel(light2, eyePosition, normalize(eyeNorm)) + ambientBrightness* BlinnPhongSpotlightModel(light3, eyePosition, normalize(eyeNorm));
		lightColour = toon(lightColour);
		// Get the texel colour from the texture sampler
		vec4 vTexColour = texture(sampler0, vTexCoord);	

		if (bUseTexture)
		{
			vOutputColour = vTexColour*vec4( lightColour, 1.0f);	// Combine object colour and texture 
			
		}
		else
			vOutputColour = vec4( lightColour, 1.0f);	// Just use the colour instead

			
		}

		//vOutputColour = vec4(toon(vOutputColour.xyz),1);
		vOutputColour = mix(vec4(fogColour,1), vOutputColour, fog());
}
