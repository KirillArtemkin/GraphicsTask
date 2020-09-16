#version 330 core
in vec4 FragPos;
out vec4 FragColor;

uniform vec3 lightPos;
uniform float far_plane;

void main()
{
    float lightDistance = length(FragPos.xyz- lightPos);
    

    lightDistance = lightDistance / far_plane;
    
        gl_FragDepth = lightDistance;
	FragColor = vec4(vec3(gl_FragDepth), 1.0);
}