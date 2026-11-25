#include "sky.hpp"
#include "Lighting/light.hpp"
#include <raylib.h>
#include <rlgl.h>

Sky::Sky() {
  skyShader =
      LoadShader(TextFormat("assets/shaders/glsl%i/sky/sky.vs", GLSL_VERSION),
                 TextFormat("assets/shaders/glsl%i/sky/sky.fs", GLSL_VERSION));
  Mesh sphereMesh = GenMeshSphere(1.0f, 16, 16);
  dome = LoadModelFromMesh(sphereMesh);
  dome.materials[0].shader = skyShader;

  skyColourLoc = GetShaderLocation(skyShader, "skyColour");
  sunDirLoc = GetShaderLocation(skyShader, "sunDir");
  sunColourLoc = GetShaderLocation(skyShader, "sunColour");
  sunRadiusLoc = GetShaderLocation(skyShader, "sunRadius");

  const float sunRadiusValue = SUN_RADIUS;
  SetShaderValue(skyShader, sunRadiusLoc, &sunRadiusValue, SHADER_UNIFORM_FLOAT);
}

Sky::~Sky() {
  UnloadModel(dome);
  UnloadShader(skyShader);
}

void Sky::update(Color skyColour, Vector3 sunDirection, Color sunColour) {
  Vector3 vecSkyColour = colourToVector(skyColour);
  Vector3 vecSunColour = colourToVector(sunColour);

  SetShaderValue(skyShader, skyColourLoc, &vecSkyColour, SHADER_UNIFORM_VEC3);
  SetShaderValue(skyShader, sunDirLoc, &sunDirection, SHADER_UNIFORM_VEC3);
  SetShaderValue(skyShader, sunColourLoc, &vecSunColour, SHADER_UNIFORM_VEC3);
}

void Sky::draw(Vector3 viewpointPos) {
  rlDisableDepthMask();
  rlDisableBackfaceCulling();

  DrawModel(dome, viewpointPos, DOME_RADIUS, WHITE);

  rlEnableBackfaceCulling();
  rlEnableDepthMask();
}
