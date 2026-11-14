#version 330 core

// raylib fills these by name
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;

in mat4 instanceTransform;

uniform mat4 mvp;
uniform vec3 viewPos; // camera position in world space

out vec2 fragTexCoord;
out vec3 fragNormal;
out vec3 fragView; // surface -> camera

void main() {
  vec4 worldPos = instanceTransform * vec4(vertexPosition, 1.0);
  fragTexCoord  = vertexTexCoord;
  gl_Position   = mvp * worldPos;
  fragView      = viewPos - worldPos.xyz;

  // world-space normal; only correct for uniform scale (else use the inverse transpose)
  fragNormal = mat3(instanceTransform) * vertexNormal;
}
