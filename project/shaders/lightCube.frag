#version 410 core

out vec4 fColor;

uniform bool isNIght;

void main() 
{    
	fColor = vec4(1.0f);
	if (isNIght) fColor = vec4(0.3f,0.3f,0.3f,1.0f);
}
