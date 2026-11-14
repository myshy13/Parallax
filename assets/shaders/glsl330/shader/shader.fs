#version 330 core

in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragPos;

const int MAX_LIGHTS = 16;

// raylib fills these by name: the material's texture and the DrawModel tint
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform vec3 lightDir[MAX_LIGHTS];
uniform vec3 lightColour[MAX_LIGHTS];
uniform int lightCount;
uniform vec3 ambientColour;

uniform vec3 viewPos;

out vec4 finalColor;

const float shininess = 48.0;
const float specStrength = 0.4;

void main() {
  vec3 N = normalize(fragNormal);
  vec3 V = normalize(viewPos - fragPos);

  vec3 diffuse = ambientColour;
  vec3 specular = vec3(0.0);

  for (int i = 0; i < MAX_LIGHTS; i++) {
    if (i >= lightCount) break;

    float nDotL = max(dot(N, lightDir[i]), 0.0);
    diffuse += nDotL * lightColour[i];

    if (nDotL > 0.0) {
      vec3 H = normalize(lightDir[i] + V);
      specular += pow(max(dot(N, H), 0.0), shininess) * lightColour[i];
    }
  }

  vec4 albedo = texture(texture0, fragTexCoord);
  vec3 colour = albedo.rgb * colDiffuse.rgb * diffuse + specular * specStrength;
  finalColor  = vec4(colour, albedo.a * colDiffuse.a);
}
