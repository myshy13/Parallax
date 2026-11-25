#version 330 core

in vec3 fragDirection;

uniform vec3 skyColour;
uniform vec3 sunDir;
uniform vec3 sunColour;
uniform float sunRadius;

out vec4 finalColor;

const float feather = 0.05;

void main() {
  vec3 dir = normalize(fragDirection);
  float d = distance(dir, sunDir);

  float disc = 1.0 - smoothstep(sunRadius - feather, sunRadius, d);

  vec3 colour = skyColour + sunColour * disc;
  finalColor = vec4(colour, 1.0);
}
