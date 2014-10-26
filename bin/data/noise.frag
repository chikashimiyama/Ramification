#version 410

layout( location = 0 ) out vec4 outputColor;

in float distFactor;
in vec4 noiseFactor;
in float value;

void main(){
    
    outputColor = vec4(vec3(1.0) * distFactor, 1.0);
}