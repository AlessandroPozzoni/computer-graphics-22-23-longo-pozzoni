#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform GlobalUniformBufferObject {
	vec3 lightPos;
	vec3 lightDir;
	vec4 lightColor;
	vec3 AmbLightColor;
	vec3 eyePos;	
} gubo;

layout(set = 1, binding = 0) uniform UniformBufferObject {
	float amb;
	float gamma;
	vec3 sColor;
	mat4 mvpMat;
	mat4 mMat;
	mat4 nMat;
} ubo;

layout(set = 1, binding = 1) uniform sampler2D tex;

const float beta = 2.0f;
const float g = 3.0f;

void main() {
	vec3 N = normalize(fragNorm);				// surface normal
	vec3 V = normalize(gubo.eyePos - fragPos);	// viewer direction
	//vec3 L = normalize(gubo.lightDir);			// light direction

	vec3 albedo = texture(tex, fragUV).rgb;		// main color
	vec3 MD = albedo;
	vec3 MS = ubo.sColor;
	vec3 MA = albedo * ubo.amb;
	vec3 LA = gubo.AmbLightColor;
	
	// Write the shader here
	vec3 lightDir = vec3(0.0,1.0,0.0);
	vec3 lightColor = vec3(1.0,0.15,0.2);

	lightDir = normalize(gubo.lightPos - fragPos);
	lightColor = vec3(gubo.lightColor) * pow(g / length(gubo.lightPos - fragPos), beta);

	vec3 L = lightDir;

	vec3 Diff = MD * clamp(dot(L,N),0.0f,1.0f);
	vec3 Spec = vec3(pow(clamp(dot(N, normalize(L + V)),0.0,1.0), ubo.gamma));
	vec3 Ambient = LA * MA;

	vec3 ME = vec3(1.0f) - ubo.amb; // Texture2 se devono emettere
	
	outColor = vec4(clamp(clamp((Diff + Spec) * lightColor.rgb + Ambient,0.0,1.0) + ME, 0.0f, 1.0f), 1.0f);

	/*
	Cook-Torrance: I need h, N, rho, PI, F0, L, V, K

	vec3 h = normalize(L + V);
	float rho = 0.1; // as parameter of ubo
	float pi = 3.141595f;
	float F0 = 0.3f; // as parameter of ubo
	float K = 0.2f; // as parameter of ubo

	float D = pow(dot(h, N), (2 / pow(rho,2) - 2)) / (pi * pow(rho, 2));
	float F = F0 + (1.0f - F0) * pow((1.0f - clamp(dot(V, h), 0.0f, 1.0f)), 5);

	float g1 = 2 * dot(h, N) * dot(V, N) / dot(V, h);
	float g2 = 2 * dot(h, N) * dot(V, N) / dot(L, h);

	float G = min(1.0f, min(g1, g2));

	vec3 Diff = MD * clamp(dot(L, N), 0.0f, 1.0f);
	vec3 Spec = MS * (D * G * F) / (4 * clamp(dot(V, N), 0.0f, 1.0f));
	vec3 fr = K * Diff + (1.0f - K) * Spec;

	outColor = vec4(clamp((lightColor * fr + Ambient) + ME, 0.0f, 1.0f), 1.0f);

	*/

	
	/*outColor = vec4(
				clamp((MD * clamp(dot(L,N),0.0f,1.0f) +
					  MS * pow(clamp(dot(N, normalize(L + V)), 0.0f, 1.0f), ubo.gamma)) +
					  LA * MA,
				0.0f, 1.0f), 1.0f);	// output color

	outColor = vec4(clamp(Diff + Spec + Ambient,0.0,1.0), 1.0f);*/
}