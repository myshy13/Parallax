#version 100

precision mediump float;

varying highp vec2 fragWorldXZ; // highp: mediump is too coarse at tens of metres

const int MAX_LIGHTS = 4; // must match Lighting.hpp and shader.fs
const vec3 groundNormal = vec3(0.0, 1.0, 0.0); // the ground plane faces straight up

uniform vec4 colDiffuse;
uniform vec3 lightDir[MAX_LIGHTS];
uniform vec3 lightColour[MAX_LIGHTS];
uniform int lightCount;
uniform vec3 ambientColour;
uniform highp vec2 seedOffset; // highp: values up to ~100 are too coarse in mediump

const vec3 dustDark  = vec3(0.4, 0.2, 0.094);
const vec3 dustLight = vec3(0.749, 0.431, 0.286);

// ==== noise ==== //

const float noiseScale = 0.1; // noise units per metre
const int   octaves    = 4;

float hash(highp vec2 p) {
  return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float valueNoise(highp vec2 p) {
  highp vec2 i = floor(p);
  highp vec2 f = fract(p);
  vec2 u = f * f * (3.0 - 2.0 * f); // smoothstep weight, avoids grid creases

  float a = hash(i);
  float b = hash(i + vec2(1.0, 0.0));
  float c = hash(i + vec2(0.0, 1.0));
  float d = hash(i + vec2(1.0, 1.0));

  return mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
}

float fbm(highp vec2 p) {
  float sum       = 0.0;
  float amplitude = 0.5;

  for (int i = 0; i < octaves; i++) {
    sum       += amplitude * valueNoise(p);
    p         *= 2.0;
    amplitude *= 0.5;
  }

  return sum; // 0 to just under 1
}

void main() {
  vec3 light = ambientColour;

  // loop bound must be constant in ES 1.00, so skip unused lights instead
  for (int i = 0; i < MAX_LIGHTS; i++) {
    if (i >= lightCount) break;

    light += max(dot(groundNormal, lightDir[i]), 0.0) * lightColour[i];
  }

  vec3 colour  = mix(dustDark, dustLight, fbm(fragWorldXZ * noiseScale + seedOffset));
  gl_FragColor = vec4(colour * colDiffuse.rgb * light, colDiffuse.a);
}
