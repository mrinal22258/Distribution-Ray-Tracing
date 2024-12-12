#version 330 core

in vec2 TexCoord;  // Input texture coordinates from the vertex shader
out vec4 FragColor;  // Output color of the fragment (pixel)

uniform sampler2D screenTexture;  // The texture applied to the screen

void main() {
    vec3 color = texture(screenTexture, TexCoord).rgb;  // Sample the color from the texture

    // Gamma correction (for better color representation on screens)
    color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);  // Output the final color with full opacity
}
