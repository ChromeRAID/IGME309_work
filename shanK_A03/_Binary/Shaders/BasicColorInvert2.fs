#version 330

in vec3 Color;

uniform vec3 SolidColor = vec3(-1,-1,-1);

out vec4 Fragment;

uniform bool Complimentary;

void main()
{
	vec3 output = Color;
	
	if(SolidColor.r != -1.0 && SolidColor.g != -1.0 && SolidColor.b != -1.0)
		output = SolidColor;

	if(Complimentary){
	output = vec3(1.0f, 1.0f, 1.0f) - output;

}	
	Fragment = vec4(output, 1.0f);	
	return;
}