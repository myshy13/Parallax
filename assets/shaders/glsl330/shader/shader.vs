#version 330 core

// raylib fills these by name
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;

in mat4 instanceTransform;

uniform mat4 mvp;

out vec2 fragTexCoord;
out vec3 fragNormal;
out vec3 fragPos;

void main() {
  fragTexCoord = vertexTexCoord;
  gl_Position = mvp * instanceTransform * vec4(vertexPosition, 1.0);

  // world-space normal; only correct for uniform scale (else use the inverse transpose)
  fragNormal = mat3(instanceTransform) * vertexNormal;
  vec4 worldPos = instanceTransform * vec4(vertexPosition, 1.0);
  fragPos = worldPos.xyz;
}
