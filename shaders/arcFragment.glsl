#version 330 core
out vec4 FragColor;

uniform float fragIter;

void main()
{
    // linearly interpolate between both textures (80% container, 20% awesomeface)
    FragColor =vec4(1.0f-0.05f*fragIter, 0.0f, 0.0f, 0.4f);
}