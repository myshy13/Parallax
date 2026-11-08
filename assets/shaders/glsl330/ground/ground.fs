#version 330 core

in vec2 fragWorldXZ;

uniform vec4 colDiffuse;
out vec4 finalColor;

const float spacing     = 1.0;
const float lineWidth   = 0.008; // in cells; the width up close
const float linePixels  = 0.05;   // minimum on-screen width, in pixels
const vec3 groundColour = vec3(0.9);
const vec3 lineColour   = vec3(0.3);

void main() {
  vec2 gridPos   = fragWorldXZ / spacing;
  vec2 pixelSize = fwidth(gridPos);
  vec2 width     = max(vec2(lineWidth), pixelSize * linePixels);

  vec2 cell   = fract(gridPos);
  vec2 d      = min(cell, 1.0 - cell);
  vec2 onLine = step(d, width);

  vec3 colour = mix(groundColour, lineColour, max(onLine.x, onLine.y));
  finalColor  = vec4(colour, 1.0) * colDiffuse;
}
