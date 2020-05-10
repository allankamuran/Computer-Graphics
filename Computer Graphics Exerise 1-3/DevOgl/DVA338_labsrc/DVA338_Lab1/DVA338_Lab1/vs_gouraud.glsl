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

const vec3 diffuseColor = vec3(0.5, 0.0, 0.0);
const vec3 specColor = vec3(1.0, 1.0, 1.0);

in vec3 vPos;                          
in vec3 vNorm;                      
uniform mat4 P, MV, V;	
out vec4 color; 

void main() 
{
	vec3 lightning = {0.0, 0.0, 0.0}; // ambient

	gl_Position = P * MV * vec4(vPos, 1.0);  

	vec4 vertPos4 = MV * vec4(vPos, 1.0);
	vec3 vertPos = vec3(vertPos4) / vertPos4.w;
	vec3 normal = vec3(transpose(inverse(MV)) * vec4(vNorm, 0.0));

	for(int i = 0; i<numberOfLights; ++i)
	{
		vec3 viewPos = vec3(V * vec4(lights[i].position, 1.0));
		vec3 lightDir = normalize(viewPos - vertPos);
		vec3 reflectDir = reflect(-lightDir, normal);
		vec3 viewDir = normalize(-vertPos);

		float lambertian = max(dot(lightDir,normal), 0.0);
		vec3 specular = {0.0, 0.0, 0.0};
		vec3 diffuse = lights[i].diffuse * material.diffuse * lambertian;

		if(lambertian > 0.0) {
		   float specAngle = max(dot(reflectDir, viewDir), 0.0);
		   specular = lights[i].specular * material.specular * pow(specAngle, material.shininess);
		}
		lightning = lightning + diffuse + specular;
	}
    color = vec4(lightning, 1.0);
}


                                                                