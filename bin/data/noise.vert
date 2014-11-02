#version 410

layout (location = 0) in vec3 VertexPosition;
const float gNumGrid = 768;

uniform mat4 modelMatrix;
uniform mat4 MVP;

uniform vec2 offset0, offset1, offset2, noiseLightOffset;
uniform vec2 freq0, freq1, freq2, noiseLightFreq;

uniform vec2 amp0, amp1, amp2, noiseLightAmp;
uniform vec2 twistFreq, twistOffset;
uniform vec2 scale;
uniform vec2 randomCoord;

uniform float colorOffset;
uniform float noiseFactor;
uniform float distanceFactor;
uniform float twistFactor;
uniform float noiseToColorFactor;

uniform float sphereAmp, tubeAmp, ringAmp, planeAmp;

uniform vec3 cameraPos;
uniform sampler2DRect noiseTexture, sphereTexture, tubeTexture, ringTexture, planeTexture;

out float VertexColor;

float wrap(float target, float operand){
    if(operand == 0.0)
        return 0;
    while(target < 0)
        target += operand;
    while(target >= operand)
        target -= operand;
    return target;
}

float cosineInterpolation(float a, float b, float x){
    float f = (1.0 - cos(x * 3.14159265359)) * 0.5;
    return  a*(1-f) + b*f;
}

float cosineInterpolation2d(float v1, float v2, float v3, float v4, float x, float y){
    return cosineInterpolation(cosineInterpolation(v1, v2, x),
                               cosineInterpolation(v3, v4, x),
                               y);
}

float calc(vec2 freq, vec2 offset, vec2 amp){
    float vx = VertexPosition.x * (freq.x / gNumGrid) + offset.x;
    float vy = VertexPosition.y * (freq.y / gNumGrid) + offset.y;
    float ix = floor(vx);
    float iy = floor(vy);
    
    // interpolation factor
    float factorX = vx - ix;
    float factorY = vy - iy;
    
    ix = wrap(ix, gNumGrid);
    iy = wrap(iy, gNumGrid);
    float nx = wrap(ix+1, gNumGrid );
    float ny = wrap(iy+1, gNumGrid );
    
    float v1 = (texture(noiseTexture, vec2(ix, iy))[0] - 0.5) * 2.0 * amp.x;
    float v2 = (texture(noiseTexture, vec2(nx, iy))[0] - 0.5) * 2.0 * amp.x;
    float v3 = (texture(noiseTexture, vec2(ix, ny))[0] - 0.5) * 2.0 * amp.y;
    float v4 = (texture(noiseTexture, vec2(nx, ny))[0] - 0.5) * 2.0 * amp.y;
    
    // interpolate four values
    return cosineInterpolation2d(v1, v2, v3, v4, factorX, factorY );
}

void main(){
    
    // z value
    float value = calc(freq0, offset0, amp0);
    value += calc(freq1, offset1, amp1);
    value += calc(freq2, offset2, amp2);
    
    // brightness
    float brightness = calc(noiseLightFreq, noiseLightOffset, noiseLightAmp);
    
    // geometry
    vec4 sphere = texture(sphereTexture, vec2(VertexPosition.x, VertexPosition.y)) - 0.5;
    vec4 plane = texture(planeTexture, vec2(VertexPosition.x, VertexPosition.y)) - 0.5;
    vec4 tube = texture(tubeTexture, vec2(VertexPosition.x, VertexPosition.y)) - 0.5;
    vec4 ring = texture(ringTexture, vec2(VertexPosition.x, VertexPosition.y)) - 0.5;

    float x = (sphere[0] *  sphereAmp + plane[0] * planeAmp + tube[0] * tubeAmp + ring[0] * ringAmp);
    float y = (sphere[1] *  sphereAmp + plane[1] * planeAmp + tube[1] * tubeAmp + ring[1] * ringAmp);
    float z = (sphere[2] *  sphereAmp + plane[2] * planeAmp + tube[2] * tubeAmp + ring[2] * ringAmp);
    
    float angleX = ((y * twistFreq.x)  + twistOffset.x);
    float angleY = ((x * twistFreq.y)  + twistOffset.y);

    x = (x * cos(angleX) - y * sin(angleX));
    y = (x * sin(angleX) + y * cos(angleX));
    x = (x * cos(angleY) - y * sin(angleY));
    y = (x * sin(angleY) + y * cos(angleY));
    
    vec2 lookupCoord = vec2(wrap(VertexPosition.x + randomCoord.x, gNumGrid),  wrap(VertexPosition.y + randomCoord.y, gNumGrid));
    float noise = texture(noiseTexture, lookupCoord)[0];
    gl_Position =   MVP * vec4( x  * gNumGrid,  y * gNumGrid, z * gNumGrid + value + ((noise-0.5) * noiseFactor), 1.0);
    
    VertexColor = 1.0 - (noise * noiseToColorFactor);
    VertexColor -= distance(vec4(cameraPos,1.0), modelMatrix * vec4(VertexPosition, 1.0)) / distanceFactor;
    VertexColor += brightness + colorOffset;
}


