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
  horizonColourLoc = GetShaderLocation(skyShader, "horizonColour");
  sunDirLoc = GetShaderLocation(skyShader, "sunDir");
  sunColourLoc = GetShaderLocation(skyShader, "sunColour");
  sunRadiusLoc = GetShaderLocation(skyShader, "sunRadius");
  viewPosLoc = GetShaderLocation(skyShader, "viewPos");
  cloudOffsetLoc = GetShaderLocation(skyShader, "cloudOffset");

  const float sunRadiusValue = SUN_RADIUS;
  SetShaderValue(skyShader, sunRadiusLoc, &sunRadiusValue,
                 SHADER_UNIFORM_FLOAT);
}

Sky::~Sky() {
  UnloadModel(dome);
  UnloadShader(skyShader);
}

void Sky::update(float dt, Color skyColour, Color horizonColour,
                 Vector3 sunDirection, Color sunColour) {
  cloudOffset = Vector2Add(cloudOffset, Vector2Scale(CLOUD_WIND, dt));

  Vector3 vecSkyColour = colourToVector(skyColour);
  Vector3 vecHorizonColour = colourToVector(horizonColour);
  Vector3 vecSunColour = colourToVector(sunColour);

  SetShaderValue(skyShader, skyColourLoc, &vecSkyColour, SHADER_UNIFORM_VEC3);
  SetShaderValue(skyShader, horizonColourLoc, &vecHorizonColour,
                 SHADER_UNIFORM_VEC3);
  SetShaderValue(skyShader, sunDirLoc, &sunDirection, SHADER_UNIFORM_VEC3);
  SetShaderValue(skyShader, sunColourLoc, &vecSunColour, SHADER_UNIFORM_VEC3);
  SetShaderValue(skyShader, cloudOffsetLoc, &cloudOffset, SHADER_UNIFORM_VEC2);
}

void Sky::draw(Vector3 viewpointPos) {
  rlDisableDepthMask();
  rlDisableBackfaceCulling();

  SetShaderValue(skyShader, viewPosLoc, &viewpointPos, SHADER_UNIFORM_VEC3);
  DrawModel(dome, viewpointPos, DOME_RADIUS, WHITE);

  rlEnableBackfaceCulling();
  rlEnableDepthMask();
}

void Sky::setCloudOrigin(Vector2 origin) { cloudOffset = origin; }
