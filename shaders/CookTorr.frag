#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

// Direct light + spot lights

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
	float rho;
	float K;
	float F0;
    float g;
    float beta;
    float emit;
	float baseColor;
	vec3 sColor;
	mat4 mvpMat;
	mat4 mMat;
	mat4 nMat;
} ubo;

// Texture

layout(set = 1, binding = 1) uniform sampler2D tex;

// Fixed parameters for spot light

const float cosout = 0.65;
const float cosin  = 0.80;

const float pi = 3.141595f;

vec4 getColorWith(vec3 guboeyePos, vec3 gubolightDir, vec3 guboAmbLightColor, vec3 gubolightPos, vec4 gubolightColor) {
	vec3 N = normalize(fragNorm);				// surface normal
	vec3 V = normalize(guboeyePos - fragPos);	// viewer direction
	vec3 L = normalize(gubolightDir);			// light direction

	vec4 albedo = texture(tex, fragUV).rgba;		// main color
    
	// Changing color logic

	vec4 orange = {1.0f, 0.474509803922f, 0.290196078431f, 1.0f};
	vec4 purple = {0.580392156863f, 0.0f, 0.309803921569f, 1.0f};
	vec4 blue = {0.286274509804f, 0.196078431373f, 0.541176470588f, 1.0f};
	vec4 yellow = {1.0f, 0.807843137255f, 0.556862745098f, 1.0f};
	vec4 black = {0.0f, 0.0f, 0.0f, 1.0f};

	if (ubo.baseColor == 0.0f)
		albedo = texture(tex, fragUV).rgba;		// for elements with texture
	else if (ubo.baseColor == -1.0f)
		albedo = black.rgba;					//black elements (cameras)
	else if (ubo.baseColor == 1.0f)
		albedo = vec4(1.0f).rgba;				//basic white
	else if (ubo.baseColor == 2.0f)
		albedo = orange.rgba;					//orange skin
	else if (ubo.baseColor == 3.0f)
		albedo = purple.rgba;					//purple skin
	else if (ubo.baseColor == 4.0f) 
		albedo = blue.rgba;						//blue skin
	else
		albedo = yellow.rgba;					//yellow skin

	if(albedo.a < 1.0f) {
		albedo.a = 0.0f;
	}

	// BRDF

	vec4 MD = albedo;
	vec3 MS = ubo.sColor;
	vec4 MA = albedo * vec4(vec3(ubo.amb), 1.0f);
	vec3 LA = guboAmbLightColor;

	// Spot light

    float g = ubo.g;
    float beta = ubo.beta;

	float clamping = clamp((dot(normalize(gubolightPos - fragPos), gubolightDir) - cosout) / (cosin - cosout), 0.0f, 1.0f);

	vec3 lightDir = normalize(gubolightPos - fragPos);
	vec3 lightColor = vec3(gubolightColor) * pow(g / length(gubolightPos - fragPos), beta) * clamping;

    L = lightDir;

	// Cook Torrance

    vec3 h = normalize(L + V);
	float rho = ubo.rho;
	float F0 = ubo.F0;
	float K = ubo.K;

    float D = pow(dot(h, N), (2 / pow(rho,2) - 2)) / (pi * pow(rho, 2));
	float F = F0 + (1.0f - F0) * pow((1.0f - clamp(dot(V, h), 0.0f, 1.0f)), 5);
	float g1 = 2 * dot(h, N) * dot(V, N) / dot(V, h);
	float g2 = 2 * dot(h, N) * dot(V, N) / dot(L, h);
	float G = min(1.0f, min(g1, g2));

    vec4 Diff = MD * vec4(vec3(clamp(dot(L, N), 0.0f, 1.0f)), 1.0f);
	vec4 Spec = vec4(vec3(MS * (D * G * F) / (4 * clamp(dot(V, N), 0.0f, 1.0f))), albedo.a);
	vec4 fr = K * Diff + (1.0f - K) * Spec;

	vec4 Ambient = vec4(LA, 1.0f) * MA;

	vec4 color = vec4(0.0f);

	if (dot(L, N) >= 0) {
		color = clamp(vec4(lightColor, 1.0f) * fr + Ambient, 0.0f, 1.0f);
	} else {
        color = clamp(Ambient, 0.0f, 1.0f);
    }

	return color;
}

void main() {

	// Handle emit

    float emit = ubo.emit;

    vec4 ME = texture(tex, fragUV).rgba * emit;

	// Sum of BRDFs

	outColor = clamp(
			getColorWith(gubo1.eyePos, gubo1.lightDir, gubo1.AmbLightColor, gubo1.lightPos, gubo1.lightColor) +
			getColorWith(gubo2.eyePos, gubo2.lightDir, gubo2.AmbLightColor, gubo2.lightPos, gubo2.lightColor) +
			getColorWith(gubo3.eyePos, gubo3.lightDir, gubo3.AmbLightColor, gubo3.lightPos, gubo3.lightColor) +
			ME, 0.0f, 1.0f);

}