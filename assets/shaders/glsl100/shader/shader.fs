#version 100

precision mediump float;

varying vec2 fragTexCoord;

// raylib fills these by name: the material's texture and the DrawModel tint
uniform sampler2D texture0;
uniform vec4 colDiffuse;

void main() {
  gl_FragColor = texture2D(texture0, fragTexCoord) * colDiffuse;
}
