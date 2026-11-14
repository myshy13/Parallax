#version 100

// raylib fills these by name
attribute vec3 vertexPosition;
attribute vec2 vertexTexCoord;
attribute vec3 vertexNormal;

attribute mat4 instanceTransform;

uniform mat4 mvp;

varying vec2 fragTexCoord;
varying vec3 fragNormal;
varying highp vec3 fragPos; // world position; highp: mediump is too coarse at tens of metres

void main() {
  vec4 worldPos = instanceTransform * vec4(vertexPosition, 1.0);
  fragTexCoord  = vertexTexCoord;
  gl_Position   = mvp * worldPos;
  fragPos       = worldPos.xyz;

  // ES 1.00 has no mat3(mat4); world-space normal, only correct for uniform scale
  mat3 normalMatrix = mat3(instanceTransform[0].xyz, instanceTransform[1].xyz,
                           instanceTransform[2].xyz);
  fragNormal = normalMatrix * vertexNormal;
}
