#version 100

attribute vec3 vertexPosition;

uniform mat4 mvp;

varying highp vec3 fragDirection; // highp: a mediump direction is too coarse

void main() {
  gl_Position   = mvp * vec4(vertexPosition, 1.0);
  fragDirection = vertexPosition;
}
