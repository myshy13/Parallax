#version 100

// raylib fills these by name
attribute vec3 vertexPosition;
attribute vec2 vertexTexCoord;
attribute vec3 vertexNormal;

attribute mat4 instanceTransform;

uniform mat4 mvp;
uniform vec3 viewPos; // camera position in world space (highp by default in a vertex shader)

varying vec2 fragTexCoord;
varying vec3 fragNormal;
varying vec3 fragView; // surface -> camera; computed here so the fragment shader needs no highp

void main() {
  vec4 worldPos = instanceTransform * vec4(vertexPosition, 1.0);
  fragTexCoord  = vertexTexCoord;
  gl_Position   = mvp * worldPos;
  fragView      = viewPos - worldPos.xyz;

  // ES 1.00 has no mat3(mat4); world-space normal, only correct for uniform scale
  mat3 normalMatrix = mat3(instanceTransform[0].xyz, instanceTransform[1].xyz,
                           instanceTransform[2].xyz);
  fragNormal = normalMatrix * vertexNormal;
}
