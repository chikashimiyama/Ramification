#version 410

layout (location = 0) in vec4 VertexPosition;
uniform mat4 modelViewProjectionMatrix;
void main(){
    gl_Position = modelViewProjectionMatrix * VertexPosition;
}