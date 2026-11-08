#version 100

// raylib fills these by name
attribute vec3 vertexPosition;
attribute vec2 vertexTexCoord;

attribute mat4 instanceTransform;

uniform mat4 mvp;

varying vec2 fragTexCoord;
varying highp vec2 fragWorldXZ; // highp: mediump is too coarse at tens of metres

void main() {
  vec4 worldPos = instanceTransform * vec4(vertexPosition, 1.0);
  fragTexCoord  = vertexTexCoord;
  gl_Position   = mvp * worldPos;
  fragWorldXZ   = worldPos.xz;
}
