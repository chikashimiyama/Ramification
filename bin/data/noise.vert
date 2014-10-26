#version 410

layout (location = 0) in vec3 VertexPosition;

uniform mat4 modelViewProjectionMatrix;
uniform vec3 cameraPos;
uniform sampler2DRect tex0;

out float dist;
out vec4 noiseFactor;

void main(){
    
    gl_Position =  vec4(VertexPosition.x-512, VertexPosition.y-384, VertexPosition.z, 1.0) * modelViewProjectionMatrix;
    dist = distance(cameraPos, VertexPosition);

    noiseFactor = texture(tex0, vec2(VertexPosition));
}