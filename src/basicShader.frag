#version 460 core

// Interpolated values from the vertex shader
in vec3 fNormal;
in vec3 fPosition;

// Output buffer
out vec4 oColor;

void main()
{
    // Affichage normale (exprimee dans l'espace camera)
    vec3 nNormal = normalize(fNormal);
    oColor = vec4(nNormal * 0.5 + 0.5, 1);
}