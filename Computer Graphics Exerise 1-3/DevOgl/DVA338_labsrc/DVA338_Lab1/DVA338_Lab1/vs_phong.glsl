#version 420 core

in vec3 vPos;                          
in vec3 vNorm;  
out vec4 color;                       
uniform mat4 P, MV;

out vec3 normalInterp;
out vec3 vertPos;      

void main(void)                         
{    
	gl_Position = P * MV * vec4(vPos, 1.0);  
	vec4 vertPos4 = MV * vec4(vPos, 1.0);      
	vertPos = vec3(vertPos4) / vertPos4.w;
	normalInterp = vec3(transpose(inverse(MV)) * vec4(vNorm, 0.0));  
} 