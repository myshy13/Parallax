#version 330 core

in vec3 vertexPosition;

uniform mat4 mvp;

out vec3 fragDirection;

void main() {
  gl_Position = mvp * vec4(vertexPosition, 1.0);
  fragDirection = vertexPosition;
}
