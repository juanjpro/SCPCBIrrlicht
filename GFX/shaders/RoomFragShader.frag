#version 110

uniform sampler2D Texture0; //base texture
uniform sampler2D Texture1; //lightmap
uniform sampler2D Texture2; //distortion map

void main (void)
{
	vec4 col3 = texture2D(Texture2, vec2(gl_TexCoord[0]));
	
	//interpolate with nearby pixels to reduce artifacts
	col3+=texture2D(Texture2, vec2(gl_TexCoord[0])+vec2(0.0009765625));
	col3+=texture2D(Texture2, vec2(gl_TexCoord[0])+vec2(-0.0009765625));
	col3+=texture2D(Texture2, vec2(gl_TexCoord[0])+vec2(0.0009765625,-0.0009765625));
	col3+=texture2D(Texture2, vec2(gl_TexCoord[0])+vec2(-0.0009765625,0.0009765625));
	col3/=vec4(vec3(5.0),1.0);
	
	vec4 col2 = texture2D(Texture1, vec2(gl_TexCoord[1])-(vec2(col3.r-0.5,col3.g-0.5)*vec2(0.03)));
	vec4 col = texture2D(Texture0, vec2(gl_TexCoord[0]));
	
	col *= col2;
    gl_FragColor = col;
}