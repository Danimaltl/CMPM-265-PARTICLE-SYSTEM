uniform sampler2D texture;
uniform float time;
uniform vec2 resolution;

//RADIUS of our vignette, where 0.5 results in a circle fitting the screen
const float RADIUS = 0.1;

//softness of our vignette, between 0.0 and 1.0
const float SOFTNESS = 0.2;

float random (in vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))
                 * 43758.5453123);
}

// 2D Noise based on Morgan McGuire @morgan3d
// https://www.shadertoy.com/view/4dS3Wd
float noise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    // Smooth Interpolation

    // Cubic Hermine Curve.  Same as SmoothStep()
    vec2 u = f*f*(3.0-2.0*f);
    // u = smoothstep(0.,1.,f);

    // Mix 4 coorners porcentages
    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}

void main()
{
    // lookup the pixel in the texture
    vec4 pixel;

	// lens distortion coefficient (between
    float k = 40 * cos(time); //cos(time * (time/2) ) * 50 + 100;
       
    // cubic distortion value
    float kcube = -400 ;
       
       
    float r2 = (gl_TexCoord[0].x-0.5) * (gl_TexCoord[0].x-0.5) + (gl_TexCoord[0].y-0.5) * (gl_TexCoord[0].y-0.5);       
    float f = 0;
       
 
    //only compute the cubic distortion if necessary
    if( kcube == 0.0){
            f = 1 + r2 * k;
    }else{
            f = 1 + r2 * (k + kcube * sqrt(r2));
    };
    
	vec2 pos;
    // get the right pixel for the current position
    pos.x = f*(gl_TexCoord[0].x-0.5)+0.5;
    pos.y = f*(gl_TexCoord[0].y-0.5)+0.5;
    vec3 inputDistord = tex2D(texture,pos);
	
	vec2 position = (gl_FragCoord.xy / resolution.xy) - vec2(0.5);
	float len = length(position);

	float vignette = smoothstep(RADIUS, RADIUS-SOFTNESS, len);
 
    pixel = vec4(inputDistord.r,inputDistord.g,inputDistord.b,1);

	pixel.r = pixel.r + (noise(vec2(pos.x + time, pos.y + time)) - 0.5);
	pixel.g = pixel.g + (noise(vec2(pos.x + time/2, pos.y + time/2)) - 0.5);
	pixel.b = pixel.b + (noise(vec2(pos.x + time*2, pos.y + time*2)) - 0.5);

	pixel.rgb = mix(pixel.rgb, pixel.rgb * vignette, 0.5);

    // multiply it by the color
    gl_FragColor = gl_Color * pixel;
}