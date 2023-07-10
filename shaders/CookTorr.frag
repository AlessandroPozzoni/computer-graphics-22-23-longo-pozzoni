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
	float rho;
	float K;
	float F0;
    float g;
    float beta;
    float emit;
	vec3 sColor;
	mat4 mvpMat;
	mat4 mMat;
	mat4 nMat;
} ubo;

layout(set = 1, binding = 1) uniform sampler2D tex;
layout(set = 1, binding = 2) uniform sampler2D texEmit;

void main() {

	vec3 N = normalize(fragNorm);				// surface normal
	vec3 V = normalize(gubo.eyePos - fragPos);	// viewer direction
	vec3 L = normalize(gubo.lightDir);			// light direction

	vec4 albedo = texture(tex, fragUV).rgba;		// main color
    vec4 MD = albedo;
	vec3 MS = ubo.sColor;
	vec4 MA = albedo * vec4(vec3(ubo.amb), 1.0f);
	vec3 LA = gubo.AmbLightColor;

    float g = ubo.g;
    float beta = ubo.beta;

    vec3 lightDir = normalize(gubo.lightPos - fragPos);
	vec3 lightColor = vec3(gubo.lightColor) * pow(g / length(gubo.lightPos - fragPos), beta);
    L = lightDir;

    vec3 h = normalize(L + V);
	float rho = ubo.rho;
	float pi = 3.141595f;
	float F0 = ubo.F0;
	float K = ubo.K;

    float emit = ubo.emit;

    float D = pow(dot(h, N), (2 / pow(rho,2) - 2)) / (pi * pow(rho, 2));
	float F = F0 + (1.0f - F0) * pow((1.0f - clamp(dot(V, h), 0.0f, 1.0f)), 5);
	float g1 = 2 * dot(h, N) * dot(V, N) / dot(V, h);
	float g2 = 2 * dot(h, N) * dot(V, N) / dot(L, h);
	float G = min(1.0f, min(g1, g2));

    vec4 Diff = MD * vec4(vec3(clamp(dot(L, N), 0.0f, 1.0f)), 1.0f);
	vec4 Spec = vec4(vec3(MS * (D * G * F) / (4 * clamp(dot(V, N), 0.0f, 1.0f))), albedo.a);
	vec4 fr = K * Diff + (1.0f - K) * Spec;

	vec4 Ambient = vec4(LA, 1.0f) * MA;

    vec4 ME = texture(texEmit, fragUV).rgba * emit;

    if(dot(L, N) >= 0) {
	    outColor = clamp((vec4(lightColor, 1.0f) * fr + Ambient) + ME, 0.0f, 1.0f);
    } else {
        outColor = clamp(Ambient + ME, 0.0f, 1.0f);
    }

}