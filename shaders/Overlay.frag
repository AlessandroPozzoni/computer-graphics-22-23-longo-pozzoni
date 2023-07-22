#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
	float visible;
	float Ar;
} ubo;

layout(location = 0) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D tex;

void main() {
	vec4 color = texture(tex, fragUV).rgba;
	color.a = color.a * ubo.visible;
	outColor = color.rgba;	// output color
}