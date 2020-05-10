#version 420 core

struct lightSource{
	vec3 position;
	vec3 diffuse;
	vec3 specular;
};

uniform int numberOfLights;
uniform lightSource lights[3];

vec3 lightPos = vec3(1.0, 1.0, 1.0);

in vec3 normal;
in vec3 vertPos;
out vec4 fColor;

void main()
{
	float intensity;
	vec3 lightDir;
	vec4 color;

	for(int i; i<numberOfLights; ++i)
	{
		lightDir = normalize(lights[i].position - vertPos);
		intensity = dot(lightDir, normal);

		if (intensity > 0.95)
			color += vec4(1.0,0.5,0.5,1.0);
		else if (intensity > 0.5)
			color += vec4(0.6,0.3,0.3,1.0);
		else if (intensity > 0.25)
			color += vec4(0.4,0.2,0.2,1.0);
		else
			color += vec4(0.2,0.1,0.1,1.0);																					
	}
	fColor = color;
}