#version 100

precision mediump float;

varying highp vec3 fragDirection;

uniform vec3 skyColour;
uniform vec3 sunDir;
uniform vec3 sunColour;
uniform float sunRadius;

const float feather = 0.01; // radians; must stay below sunRadius

void main() {
  vec3 dir = normalize(fragDirection);
  float d  = distance(dir, sunDir);

  float disc = 1.0 - smoothstep(sunRadius - feather, sunRadius, d);

  vec3 colour  = skyColour + sunColour * disc;
  gl_FragColor = vec4(colour, 1.0);
}
