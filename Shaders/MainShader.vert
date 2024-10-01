#version 330 core
		
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 in_color;

uniform mat4 projMatrix;

out vec4 u_Color;

void main() {
	vec4 product = projMatrix * position;
	if (product[3] != 0){
		product[0] = product[0] / product[3];
		product[1] = product[1] / product[3];
		product[2] = product[2] / product[3];
	}
	gl_Position = product, 0; 
	u_Color = in_color;
}
