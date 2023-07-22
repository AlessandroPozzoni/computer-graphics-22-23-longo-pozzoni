#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
	float visible;
	float Ar;
} ubo;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inUV;

layout(location = 0) out vec2 outUV;

void main() {
	if(ubo.Ar >= 16.0f / 9.0f) {
		gl_Position = vec4(vec2((16.0f / 9.0f / ubo.Ar), 1.0f) * inPosition * ubo.visible, 0.5f, 1.0f);
		outUV = inUV;
	} else {
		gl_Position = vec4(/* vec2(1.0f, (9.0 / 16.0 * ubo.Ar)) * */ inPosition * ubo.visible, 0.5f, 1.0f);
		outUV = inUV + vec2((1.0f + inUV[0] * (-2.0f)) * (0.5f - 9.0f/16.0f * ubo.Ar / 2), 0.0f);
	}
}