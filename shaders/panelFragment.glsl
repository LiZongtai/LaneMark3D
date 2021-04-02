#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{
    vec4 texColor = texture(texture1, TexCoords);
    if(texColor.a < 0.1)
    discard;
    FragColor = min(texColor,vec4(1.0f, 1.0f, 1.0f, 0.7f));
}