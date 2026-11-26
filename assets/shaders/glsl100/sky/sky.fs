#version 100

precision mediump float;

varying highp vec3 fragDirection;

uniform vec3 skyColour;     // colour straight overhead
uniform vec3 horizonColour; // paler colour at the horizon
uniform vec3 sunDir;
uniform vec3 sunColour;
uniform float sunRadius;

uniform highp vec3 viewPos;     // highp: cloud coordinates get large
uniform highp vec2 cloudOffset;

const float feather = 0.05;
const float glowWidth = 0.2;      // chord distance; how far the halo reaches
const float glowStrength = 0.35;
const float gradientCurve = 0.4;  // below 1 keeps the horizon colour thin
const float bandHeight = 0.12;    // how tall the sunset band is
const float bandStrength = 0.6;
const float sunLowHeight = 0.35;  // sun height where the sunset band is gone

// Cheaper than glsl330 on purpose: one layer, 3 octaves, one shadow tap.
const float bias = 0.03;          // higher than glsl330 so sin() args stay small
const float coverage = 0.4;
const float softness = 0.15;

const float cloudScale = 0.01;
const float cloudHeight = 150.0;

const float tapStep = 0.15;
const float absorb = 3.0;

const float horizonFadeEnd = 0.3; // dir.y where clouds are fully visible; hides aliasing
const float edgeWidth = 0.3;      // density range above coverage that counts as a thin edge
const float rimWidth = 0.2;       // chord distance; how far the silver rim reaches from the sun
const float rimStrength = 0.3;
const float litSun = 0.4;         // how much of the sun colour lit cloud takes; mix keeps it under 1

const float nearDrift = 0.6;
const vec2 nearLayerShift = vec2(53.0, 17.0);

// ==== noise ==== //
const int octaves    = 3;
const int lowOctaves = 2;

highp float hash(highp vec2 p) {
  return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

highp float valueNoise(highp vec2 p) {
  highp vec2 i = floor(p);
  highp vec2 f = fract(p);
  highp vec2 u = f * f * (3.0 - 2.0 * f); // smoothstep weight, avoids grid creases

  highp float a = hash(i);
  highp float b = hash(i + vec2(1.0, 0.0));
  highp float c = hash(i + vec2(0.0, 1.0));
  highp float d = hash(i + vec2(1.0, 1.0));

  return mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
}

highp float fbm(highp vec2 p) {
  highp float sum = 0.0;
  float amplitude = 0.5;

  for (int i = 0; i < octaves; i++) {
    sum       += amplitude * valueNoise(p);
    p         *= 2.0;
    amplitude *= 0.5;
  }

  return sum; // 0 to just under 1
}

highp float fbmLow(highp vec2 p) {
  highp float sum = 0.0;
  float amplitude = 0.5;

  for (int i = 0; i < lowOctaves; i++) {
    sum       += amplitude * valueNoise(p);
    p         *= 2.0;
    amplitude *= 0.5;
  }

  return sum;
}

// Returns the cloud colour in rgb and how opaque it is in a.
vec4 cloudLayer(vec3 dir, float height, float scale, highp vec2 offset) {
  float d = distance(dir, sunDir);

  // Where this view ray meets the cloud layer, in metres, then in noise units.
  highp vec2 hit = viewPos.xz + dir.xz * ((height - viewPos.y) / max(dir.y, bias));
  highp vec2 cloudUV = hit * scale + offset;
  float density = fbm(cloudUV);
  float cloud   = smoothstep(coverage, coverage + softness, density);

  vec2 toSun = normalize(sunDir.xz + vec2(1e-4, 0.0)) * tapStep;

  float thickness = smoothstep(coverage, coverage + edgeWidth, density);
  float rim = (1.0 - thickness) * exp(-d / rimWidth);
  float horizonFade = smoothstep(0.0, horizonFadeEnd, dir.y);

  // One tap toward the sun: cloud in the way means this pixel is in shadow.
  float depth = max(fbmLow(cloudUV + toSun) - coverage, 0.0);
  float lit   = exp(-depth * absorb);

  vec3 litColour    = mix(horizonColour, sunColour, litSun);
  vec3 shadowColour = horizonColour * 0.45;

  vec3 cloudColour = mix(shadowColour, litColour, lit);
  cloudColour     += sunColour * rim * rimStrength;
  return vec4(cloudColour, cloud * horizonFade);
}

void main() {
  vec3 dir = normalize(fragDirection);
  float d  = distance(dir, sunDir);

  // Hide the part of the sun that is below the horizon.
  float aboveHorizon = smoothstep(-0.03, 0.0, dir.y);
  float disc = (1.0 - smoothstep(sunRadius - feather, sunRadius, d)) * aboveHorizon;
  float glow = exp(-d / glowWidth) * glowStrength;

  float up  = clamp(dir.y, 0.0, 1.0);
  vec3 base = mix(horizonColour, skyColour, pow(up, gradientCurve));

  // Sunset band: near the horizon, on the side the sun is on, when it is low.
  vec3 sunFlat = normalize(vec3(sunDir.x, 0.0, sunDir.z));
  float facing = pow(max(dot(dir, sunFlat), 0.0), 3.0);
  float band   = exp(-abs(dir.y) / bandHeight);
  float low    = 1.0 - smoothstep(0.0, sunLowHeight, sunDir.y);

  vec3 colour = base + sunColour * (disc + glow + facing * band * low * bandStrength);

  if (dir.y > 0.0) {
    vec4 nearLayer = cloudLayer(dir, cloudHeight, cloudScale, cloudOffset * nearDrift + nearLayerShift);
    colour         = mix(colour, nearLayer.rgb, nearLayer.a);
  }

  gl_FragColor = vec4(colour, 1.0);
}
