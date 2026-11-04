#include <cstdlib>
#include <raylib.h>

inline Mesh GenMeshGridLines(int slices, float spacing) {
  Mesh mesh = {0};

  // Each slice adds 2 lines (one parallel to X, one parallel to Z)
  // Plus 2 final bounding lines. Total lines = (slices + 1) * 2
  int lineCount = (slices + 1) * 2;
  mesh.vertexCount = lineCount * 2; // 2 vertices per line

  // Allocate memory for vertices (3 floats per vertex: x, y, z)
  mesh.vertices = (float *)RL_MALLOC(mesh.vertexCount * 3 * sizeof(float));

  float halfLength = (slices * spacing) / 2.0f;
  int index = 0;

  for (int i = 0; i <= slices; i++) {
    float offset = -halfLength + (i * spacing);

    // --- Line 1: Parallel to Z axis ---
    mesh.vertices[index++] = offset;      // Start X
    mesh.vertices[index++] = 0.0f;        // Start Y
    mesh.vertices[index++] = -halfLength; // Start Z

    mesh.vertices[index++] = offset;     // End X
    mesh.vertices[index++] = 0.0f;       // End Y
    mesh.vertices[index++] = halfLength; // End Z

    // --- Line 2: Parallel to X axis ---
    mesh.vertices[index++] = -halfLength; // Start X
    mesh.vertices[index++] = 0.0f;        // Start Y
    mesh.vertices[index++] = offset;      // Start Z

    mesh.vertices[index++] = halfLength; // End X
    mesh.vertices[index++] = 0.0f;       // End Y
    mesh.vertices[index++] = offset;     // End Z
  }

  // Upload mesh vertex data to GPU VRAM
  UploadMesh(&mesh, false);

  return mesh;
}