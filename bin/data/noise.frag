#version 410

layout( location = 0 ) out vec4 outputColor;

in float dist;
in vec4 noiseFactor;

void main(){
    
    outputColor = vec4(vec3(noiseFactor) *  (dist / 3000.0), 1.0);
}