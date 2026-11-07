#version 100
#extension GL_OES_standard_derivatives : enable

precision mediump float;

varying highp vec2 fragWorldXZ; // highp: mediump is too coarse at tens of metres

uniform vec4 colDiffuse;

const float spacing      = 1.0;
const float lineWidth    = 0.008; // in cells; the width up close
const float linePixels   = 0.05;   // minimum on-screen width, in pixels
const vec3  groundColour = vec3(0.9);
const vec3  lineColour   = vec3(0.3);

void main() {
  highp vec2 gridPos = fragWorldXZ / spacing; // highp for the same reason as fragWorldXZ
  vec2 cell          = fract(gridPos);
  vec2 d             = min(cell, 1.0 - cell);
  vec2 pixelSize     = fwidth(gridPos);
  vec2 width         = max(vec2(lineWidth), pixelSize * linePixels);
  vec2 onLine        = step(d, width);

  vec3 colour  = mix(groundColour, lineColour, max(onLine.x, onLine.y));
  gl_FragColor = vec4(colour, 1.0) * colDiffuse;
}
