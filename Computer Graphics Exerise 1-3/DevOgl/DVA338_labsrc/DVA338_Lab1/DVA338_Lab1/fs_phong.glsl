#version 420 core       	

struct Material
{
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};

struct lightSource{
	vec3 position;
	vec3 diffuse;
	vec3 specular;
};

uniform int numberOfLights;
uniform lightSource lights[3];

uniform Material material;
uniform mat4 V;

in vec3 normalInterp;
in vec3 vertPos;
out vec4 fColor;

void main() 
{
	vec3 lightning = {0.0, 0.0, 0.0}; // ambient

    vec3 normal = normalize(normalInterp);
	vec3 viewDir = normalize(-vertPos);
	vec3 reflectDir;

	for(int i = 0; i < numberOfLights; ++i)
	{
		vec3 viewPos = vec3(V * vec4(lights[i].position, 1.0));
		vec3 lightDir = normalize(viewPos - vertPos);
		 reflectDir = reflect(-lightDir, normal);

		float lambertian = max(dot(lightDir,normal), 0.0);
		vec3 specular = {0.0, 0.0, 0.0};

		vec3 diffuse = lights[i].diffuse * material.diffuse * lambertian; 

		if(lambertian > 0.0) {
			float specAngle = max(dot(reflectDir, viewDir), 0.0);
			specular = lights[i].specular * material.specular * pow(specAngle, material.shininess);
		}
		lightning = lightning + diffuse + specular;
	}
	if(lightning.x < 1.0f || lightning.y < 1.0f || lightning.z < 1.0f)
		fColor = vec4(normal, 1.0);

    fColor = vec4(lightning, 1.0);
}
                 