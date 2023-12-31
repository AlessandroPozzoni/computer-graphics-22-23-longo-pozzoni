#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

// Direct light + three spot lights

layout(set = 0, binding = 0) uniform GlobalUniformBufferObject {
    vec3 lightPos;
    vec3 lightDir;
    vec4 lightColor;
    vec3 AmbLightColor;
    vec3 eyePos;
} gubo;

layout(set = 0, binding = 1) uniform GlobalUniformBufferObject1 {
    vec3 lightPos;
    vec3 lightDir;
    vec4 lightColor;
    vec3 AmbLightColor;
    vec3 eyePos;
} gubo1;

layout(set = 0, binding = 2) uniform GlobalUniformBufferObject2 {
    vec3 lightPos;
    vec3 lightDir;
    vec4 lightColor;
    vec3 AmbLightColor;
    vec3 eyePos;
} gubo2;

layout(set = 0, binding = 3) uniform GlobalUniformBufferObject3 {
    vec3 lightPos;
    vec3 lightDir;
    vec4 lightColor;
    vec3 AmbLightColor;
    vec3 eyePos;
} gubo3;

// Uniform buffer object

layout(set = 1, binding = 0) uniform UniformBufferObject {
	float amb;
	float gamma;
	vec3 sColor;
	mat4 mvpMat;
	mat4 mMat;
	mat4 nMat;
} ubo;

// Texture

layout(set = 1, binding = 1) uniform sampler2D tex;

// Fixed parameters for spot light

const float beta = 5.0f;
const float g = 5.0f;

// Fixed parameters for spot light

const float cosout = 0.90;
const float cosin  = 0.95;

void main() {
	vec3 N = normalize(fragNorm);				// surface normal
	vec3 V1 = normalize(gubo1.eyePos - fragPos);	// viewer direction

	vec3 albedo = texture(tex, fragUV).rgb;		// main color
	vec3 MD = albedo;
	vec3 MS = ubo.sColor;
	vec3 MA = albedo * ubo.amb;
	vec3 LA1 = gubo1.AmbLightColor;
	
	// Spot light

	float clamping1 = clamp((dot(normalize(gubo1.lightPos - fragPos), gubo1.lightDir) - cosout) / (cosin - cosout), 0.0f, 1.0f);

	vec3 lightDir1 = normalize(gubo1.lightPos - fragPos);
	vec3 lightColor1 = vec3(gubo1.lightColor) * pow(g / length(gubo1.lightPos - fragPos), beta) * clamping1;

	vec3 L1 = lightDir1;

	// Blinn reflection

	vec3 Diff1 = MD * clamp(dot(L1,N),0.0f,1.0f);
	vec3 Spec1 = vec3(pow(clamp(dot(N, normalize(L1 + V1)),0.0,1.0), ubo.gamma));
	vec3 Ambient1 = LA1 * MA;

	// Handle emission

	vec3 ME = vec3(0.0f);

	if(Ambient1 == vec3(0.0f)) {
		ME = vec3(1.0f);
	}



	vec3 V2 = normalize(gubo2.eyePos - fragPos);

	vec3 LA2 = gubo2.AmbLightColor;

	float clamping2 = clamp((dot(normalize(gubo2.lightPos - fragPos), gubo2.lightDir) - cosout) / (cosin - cosout), 0.0f, 1.0f);

	vec3 lightDir2 = normalize(gubo2.lightPos - fragPos);
	vec3 lightColor2 = vec3(gubo2.lightColor) * pow(g / length(gubo2.lightPos - fragPos), beta) * clamping2;

	vec3 L2 = lightDir2;

	vec3 Diff2 = MD * clamp(dot(L2,N),0.0f,1.0f);
	vec3 Spec2 = vec3(pow(clamp(dot(N, normalize(L2 + V2)),0.0,1.0), ubo.gamma));
	vec3 Ambient2 = LA2 * MA;
	


	vec3 V3 = normalize(gubo3.eyePos - fragPos);

	vec3 LA3 = gubo3.AmbLightColor;

	float clamping3 = clamp((dot(normalize(gubo3.lightPos - fragPos), gubo3.lightDir) - cosout) / (cosin - cosout), 0.0f, 1.0f);

	vec3 lightDir3 = normalize(gubo3.lightPos - fragPos);
	vec3 lightColor3 = vec3(gubo3.lightColor) * pow(g / length(gubo3.lightPos - fragPos), beta) * clamping3;

	vec3 L3 = lightDir3;

	vec3 Diff3 = MD * clamp(dot(L3,N),0.0f,1.0f);
	vec3 Spec3 = vec3(pow(clamp(dot(N, normalize(L3 + V3)),0.0,1.0), ubo.gamma));
	vec3 Ambient3 = LA3 * MA;



	vec3 V = normalize(gubo.eyePos - fragPos);
	vec3 LA = gubo1.AmbLightColor;

	// Direct light

	vec3 lightDir = vec3(gubo.lightDir);
	vec3 lightColor = vec3(gubo.lightColor);

	vec3 L = lightDir;

	vec3 Diff = MD * clamp(dot(L, N), 0.0, 1.0);
	vec3 Spec = vec3(pow(clamp(dot(N, normalize(L + V)),0.0,1.0), ubo.gamma));
	vec3 Ambient = LA * MA;
	
	// Sum of the BRDF

	outColor =
		vec4(clamp((Diff + Spec) * lightColor.rgb + Ambient,0.0,1.0), 1.0f) +
		vec4(clamp(clamp((Diff1 + Spec1) * lightColor1.rgb + Ambient1,0.0,1.0) + ME, 0.0f, 1.0f), 1.0f) +
		vec4(clamp(clamp((Diff2 + Spec2) * lightColor2.rgb + Ambient2,0.0,1.0) + ME, 0.0f, 1.0f), 1.0f) +
		vec4(clamp(clamp((Diff3 + Spec3) * lightColor3.rgb + Ambient3,0.0,1.0) + ME, 0.0f, 1.0f), 1.0f);
	


}