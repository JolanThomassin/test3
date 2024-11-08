#version 460 core

// Uniform for the transformation
layout(location = 0) uniform mat4 uMVMatrix;
layout(location = 1) uniform mat4 uProjMatrix;
layout(location = 2) uniform mat3 uNormalMVMatrix;

// Vertices inputs
layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec3 vNormal;

// Output for the fragment shader (interpolation)
out vec3 fNormal;
out vec3 fPosition;

void main()
{
     // Multiplication model view matrix
     // position maintenant exprimee dans l'espace camera
     vec4 vEyeCoord = uMVMatrix * vPosition;
     
     // Multiplication par la matrice de projection
     // point maintenant exprimee dans NDC
     gl_Position = uProjMatrix * vEyeCoord;

     // Passage de ces variables au fragment shader
     fPosition = vEyeCoord.xyz;
     fNormal = uNormalMVMatrix*vNormal; 
}

