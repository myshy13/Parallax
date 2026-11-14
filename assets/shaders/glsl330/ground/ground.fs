#version 330 core

in vec2 fragWorldXZ;

const int MAX_LIGHTS = 16; // must match Lighting.hpp and shader.fs
const vec3 groundNormal = vec3(0.0, 1.0, 0.0);

const float spacing     = 1.0;
const float lineWidth   = 0.008; // in cells; the width up close
const float linePixels  = 0.05;   // minimum on-screen width, in pixels
const vec3 groundColour = vec3(0.9);
const vec3 lineColour   = vec3(0.3);

uniform vec4 colDiffuse;
uniform vec3 lightDir[MAX_LIGHTS];
uniform vec3 lightColour[MAX_LIGHTS];
uniform int lightCount;
uniform vec3 ambientColour;

out vec4 finalColor;

void main() {
  vec2 gridPos   = fragWorldXZ / spacing;
  vec2 pixelSize = fwidth(gridPos);
  vec2 width     = max(vec2(lineWidth), pixelSize * linePixels);

  vec2 cell   = fract(gridPos);
  vec2 d      = min(cell, 1.0 - cell);
  vec2 onLine = step(d, width);

  vec3 light = ambientColour;

  for (int i = 0; i < MAX_LIGHTS; i++) {
    if (i >= lightCount) break;

    light += max(dot(groundNormal, lightDir[i]), 0.0) * lightColour[i];
  }

  vec3 colour = mix(groundColour, lineColour, max(onLine.x, onLine.y));
  finalColor  = vec4(colour * colDiffuse.rgb * light, colDiffuse.a);
}
