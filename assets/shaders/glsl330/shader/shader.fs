#version 330 core

in vec2 fragTexCoord;
in vec3 fragNormal;

const int MAX_LIGHTS = 16;

// raylib fills these by name: the material's texture and the DrawModel tint
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform vec3 lightDir[MAX_LIGHTS];
uniform vec3 lightColour[MAX_LIGHTS];
uniform int lightCount;
uniform vec3 ambientColour;

out vec4 finalColor;


void main() {
  vec3 N = normalize(fragNormal);
  vec3 light = ambientColour;

  for (int i = 0; i < MAX_LIGHTS; i++) {
    if (i >= lightCount) break;

    light += max(dot(N, lightDir[i]), 0.0) * lightColour[i];
  }

  vec4 albedo = texture(texture0, fragTexCoord);
  finalColor = vec4(albedo.rgb * colDiffuse.rgb * light, albedo.a * colDiffuse.a);
}
