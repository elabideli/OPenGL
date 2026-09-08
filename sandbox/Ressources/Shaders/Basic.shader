#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texcoord;

out vec2 v_TexCoord;

void main()
{
	gl_Position = position;
	v_TexCoord = texcoord;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

uniform sampler2D u_Texture;

void main()
{
	vec4 texcolor = texture(u_Texture, v_TexCoord);
	color = texcolor;
};