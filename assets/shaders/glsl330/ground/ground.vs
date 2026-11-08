#version 330 core

// raylib fills these by name
in vec3 vertexPosition;
in vec2 vertexTexCoord;

in mat4 instanceTransform;

uniform mat4 mvp;

out vec2 fragTexCoord;
out vec2 fragWorldXZ;

void main() {
  vec4 worldPos = instanceTransform * vec4(vertexPosition, 1.0);
  fragTexCoord  = vertexTexCoord;
  gl_Position   = mvp * worldPos;
  fragWorldXZ   = worldPos.xz;
}
