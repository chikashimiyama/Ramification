#version 410

layout( location = 0 ) out vec4 outputColor;

in float VertexColor;

void main(){
    
    outputColor = vec4(vec3(1.0) * VertexColor, 1.0);
}