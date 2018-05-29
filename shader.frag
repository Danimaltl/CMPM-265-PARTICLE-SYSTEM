uniform sampler2D texture;

void main()
{
    // lookup the pixel in the texture
    vec4 pixel;

	// lens distortion coefficient (between
    float k = -0.25;
       
    // cubic distortion value
    float kcube = 3;
       
       
    float r2 = (gl_TexCoord[0].x-0.5) * (gl_TexCoord[0].x-0.5) + (gl_TexCoord[0].y-0.5) * (gl_TexCoord[0].y-0.5);       
    float f = 0;
       
 
    //only compute the cubic distortion if necessary
    if( kcube == 0.0){
            f = 1 + r2 * k;
    }else{
            f = 1 + r2 * (k + kcube * sqrt(r2));
    };
       
    // get the right pixel for the current position
    float x = f*(gl_TexCoord[0].x-0.5)+0.5;
    float y = f*(gl_TexCoord[0].y-0.5)+0.5;
    vec3 inputDistord = tex2D(texture,vec2(x,y));
 
 
    pixel = vec4(inputDistord.r,inputDistord.g,inputDistord.b,1);

    // multiply it by the color
    gl_FragColor = gl_Color * pixel;
}