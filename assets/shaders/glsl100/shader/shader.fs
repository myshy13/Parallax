#version 100

precision mediump float;

varying vec2 fragTexCoord;
varying vec3 fragNormal;

const int MAX_LIGHTS = 16;

// raylib fills these by name: the material's texture and the DrawModel tint
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform vec3 lightDir[MAX_LIGHTS];
uniform vec3 lightColour[MAX_LIGHTS];
uniform int lightCount;
uniform vec3 ambientColour;

void main() {
  vec3 N     = normalize(fragNormal);
  vec3 light = ambientColour;

  // loop bound must be constant in ES 1.00, so skip unused lights instead
  for (int i = 0; i < MAX_LIGHTS; i++) {
    if (i >= lightCount) break;

    light += max(dot(N, lightDir[i]), 0.0) * lightColour[i];
  }

  vec4 albedo   = texture2D(texture0, fragTexCoord);
  gl_FragColor  = vec4(albedo.rgb * colDiffuse.rgb * light, albedo.a * colDiffuse.a);
}
