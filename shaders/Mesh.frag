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

layout(set = 0, binding = 1) uniform GlobalUniformBufferObject2 {
	vec3 lightPos;
	vec3 lightDir;
	vec4 lightColor;
	vec3 AmbLightColor;
	vec3 eyePos;	
} gubo2;

layout(set = 0, binding = 2) uniform GlobalUniformBufferObject3 {
	vec3 lightPos;
	vec3 lightDir;
	vec4 lightColor;
	vec3 AmbLightColor;
	vec3 eyePos;	
} gubo3;

layout(set = 1, binding = 0) uniform UniformBufferObject {
	float amb;
	float gamma;
	vec3 sColor;
	mat4 mvpMat;
	mat4 mMat;
	mat4 nMat;
} ubo;

layout(set = 1, binding = 1) uniform sampler2D tex;

const float beta = 5.0f;
const float g = 5.0f;

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

	float cosout = 0.90;
	float cosin  = 0.95;

	float clamping = clamp((dot(normalize(gubo.lightPos - fragPos), gubo.lightDir) - cosout) / (cosin - cosout), 0.0f, 1.0f);

	vec3 lightDir = normalize(gubo.lightPos - fragPos);
	vec3 lightColor = vec3(gubo.lightColor) * pow(g / length(gubo.lightPos - fragPos), beta) * clamping;

	vec3 L = lightDir;

	vec3 Diff = MD * clamp(dot(L,N),0.0f,1.0f);
	vec3 Spec = vec3(pow(clamp(dot(N, normalize(L + V)),0.0,1.0), ubo.gamma));
	vec3 Ambient = LA * MA;

	vec3 ME = vec3(0.0f);

	if(Ambient == vec3(0.0f)) {
		ME = vec3(1.0f);
	}



	vec3 N2 = normalize(fragNorm);				// surface normal
	vec3 V2 = normalize(gubo2.eyePos - fragPos);	// viewer direction
	//vec3 L2 = normalize(gubo2.lightDir);			// light direction

	vec3 LA2 = gubo2.AmbLightColor;
	
	// Write the shader here

	float clamping2 = clamp((dot(normalize(gubo2.lightPos - fragPos), gubo2.lightDir) - cosout) / (cosin - cosout), 0.0f, 1.0f);

	vec3 lightDir2 = normalize(gubo2.lightPos - fragPos);
	vec3 lightColor2 = vec3(gubo2.lightColor) * pow(g / length(gubo2.lightPos - fragPos), beta) * clamping2;

	vec3 L2 = lightDir2;

	vec3 Diff2 = MD * clamp(dot(L2,N2),0.0f,1.0f);
	vec3 Spec2 = vec3(pow(clamp(dot(N2, normalize(L2 + V2)),0.0,1.0), ubo.gamma));
	vec3 Ambient2 = LA2 * MA;
	


	vec3 N3 = normalize(fragNorm);				// surface normal
	vec3 V3 = normalize(gubo3.eyePos - fragPos);	// viewer direction
	//vec3 L2 = normalize(gubo3.lightDir);			// light direction

	vec3 LA3 = gubo3.AmbLightColor;
	
	// Write the shader here

	float clamping3 = clamp((dot(normalize(gubo3.lightPos - fragPos), gubo3.lightDir) - cosout) / (cosin - cosout), 0.0f, 1.0f);

	vec3 lightDir3 = normalize(gubo3.lightPos - fragPos);
	vec3 lightColor3 = vec3(gubo3.lightColor) * pow(g / length(gubo3.lightPos - fragPos), beta) * clamping3;

	vec3 L3 = lightDir3;

	vec3 Diff3 = MD * clamp(dot(L3,N3),0.0f,1.0f);
	vec3 Spec3 = vec3(pow(clamp(dot(N3, normalize(L3 + V3)),0.0,1.0), ubo.gamma));
	vec3 Ambient3 = LA3 * MA;
	
	outColor =
		vec4(clamp(clamp((Diff + Spec) * lightColor.rgb + Ambient,0.0,1.0) + ME, 0.0f, 1.0f), 1.0f) +
		vec4(clamp(clamp((Diff2 + Spec2) * lightColor2.rgb + Ambient2,0.0,1.0) + ME, 0.0f, 1.0f), 1.0f) +
		vec4(clamp(clamp((Diff3 + Spec3) * lightColor3.rgb + Ambient3,0.0,1.0) + ME, 0.0f, 1.0f), 1.0f);
	


}