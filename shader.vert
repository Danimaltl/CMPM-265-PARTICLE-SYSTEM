uniform float dt;

uniform float size = 25;

void main()
{
	unsigned int index = gl_Color.x + gl_Color.y * 0x100 + gl_Color.z * 0x10000 + gl_Color.w * 0x1000000;

	// transform the vertex position
	vec4 scaled = gl_Vertex;
	scaled.x = (gl_Vertex.x * cos(dt)) - (gl_Vertex.y * sin(dt));
	scaled.y = (gl_Vertex.x * sin(dt)) + (gl_Vertex.y * cos(dt));
    gl_Position = gl_ModelViewProjectionMatrix * scaled;

    // transform the texture coordinates
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;

	

    // forward the vertex color
    gl_FrontColor = gl_Color;
}