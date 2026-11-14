#version 100

precision mediump float;

varying vec2 fragTexCoord;
varying vec3 fragNormal;
varying vec3 fragView; // surface -> camera, from the vertex shader

const int MAX_LIGHTS = 4; // must match Lighting.hpp and ground.fs
const float shininess = 48.0;   // higher = tighter highlight
const float specStrength = 0.4; // how bright the highlight is

// raylib fills these by name: the material's texture and the DrawModel tint
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform vec3 lightDir[MAX_LIGHTS];
uniform vec3 lightColour[MAX_LIGHTS];
uniform int lightCount;
uniform vec3 ambientColour;

void main() {
  vec3 N = normalize(fragNormal);
  vec3 V = normalize(fragView);

  vec3 diffuse  = ambientColour;
  vec3 specular = vec3(0.0);

  // loop bound must be constant in ES 1.00, so skip unused lights instead
  for (int i = 0; i < MAX_LIGHTS; i++) {
    if (i >= lightCount) break;

    float nDotL = max(dot(N, lightDir[i]), 0.0);
    diffuse += nDotL * lightColour[i];

    // no highlight on the unlit side, and clamp before pow (undefined for negatives)
    if (nDotL > 0.0) {
      vec3 H = normalize(lightDir[i] + V);
      specular += pow(max(dot(N, H), 0.0), shininess) * lightColour[i];
    }
  }

  // the highlight is the light's colour, not tinted by the paint
  vec4 albedo  = texture2D(texture0, fragTexCoord);
  vec3 colour  = albedo.rgb * colDiffuse.rgb * diffuse + specular * specStrength;
  gl_FragColor = vec4(colour, albedo.a * colDiffuse.a);
}
