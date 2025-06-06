#version 330 core

layout(location = 0) in vec2 aPos;  // Vertex position
layout(location = 1) in vec2 aTexCoord;  // Texture coordinates

out vec2 TexCoord;  // Pass texture coordinates to the fragment shader

void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);  // Transform position to clip space
    TexCoord = aTexCoord;  // Pass texture coordinates to the next shader stage
}

