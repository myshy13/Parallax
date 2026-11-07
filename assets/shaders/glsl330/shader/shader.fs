#version 330 core

in vec2 fragTexCoord;

// raylib fills these by name: the material's texture and the DrawModel tint
uniform sampler2D texture0;
uniform vec4 colDiffuse;

out vec4 finalColor;

void main() {
  finalColor = texture(texture0, fragTexCoord) * colDiffuse;
}
