#pragma once
#include "ObjectRenderer.h"
#include "Shader.h"

#include <vector>
#include <unordered_map>

class Component;
class ObjectShader;
class Texture;
class Mesh;
class CameraWorldBased;
struct LightSource;

class GenericRenderer: public ObjectRenderer
{
public:
  enum EBuffers
  {
    eModel_Vb,
    eSelfIllumination_Vb,
    eNumOfBuffers
  };

  GenericRenderer();
  virtual ~GenericRenderer();

  virtual void setShader(Shader * p_shader);

  virtual void setCamera(CameraWorldBased * p_camera);

  virtual void prepareRenderData(std::vector<Object*> p_objects, GameState * p_state);
  virtual void render();

  struct RenderBatch
  {
    Asset * asset;
    std::vector<glm::mat4> models;
    std::vector<Component*> components;
    std::vector<glm::f32vec3> selfIlluminationColors;
  };

protected:
private:

  char const * m_cameraTransformUnifName;
  char const * m_positionUnifName;
  char const * m_intensityUnifName;
  char const * m_attenuationUnifName;
  char const * m_ambientCoefficientUnifName;
  char const * m_cameraPosUnifName;
  char const * m_cameraExposureUnifName;
  char const * m_lightDistanceModifierUnifName;

  glm::vec3 m_lightPos;
  glm::vec3 m_lightColor;
  glm::float32 m_attenuation;
  glm::float32 m_ambient;

  glm::vec3 m_cameraPos;
  glm::float32 m_exposure;
  glm::float32 m_lightDistanceModifier;

  GLuint m_vao;
  GLuint m_buffers[eNumOfBuffers];
  Shader::Attribute m_modelVb;
  Shader::Attribute m_selfIlluminationVb;
  Shader * m_shader;
  CameraWorldBased * m_camera;
  std::unordered_map<Asset *, std::shared_ptr<RenderBatch> > m_renderBatches;

};

