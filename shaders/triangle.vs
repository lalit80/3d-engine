#version 460 core 
layout (location = 0) in vec4 aPosition; 
uniform mat4 uMVPMatrix; 
void main(void)  { 
    gl_Position = uMVPMatrix * aPosition; 
}
