#include "GenericRenderer.h"

#include "CameraWorldBased.h"
#include "ObjectShader.h"
#include "Mesh.h"
#include "utils.h"
#include "Shader.h"
#include "Asset.h"
#include "Components/Component.h"

#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glext.h>
  #define glVertexAttribDivisor glVertexAttribDivisorARB
#endif

#include <memory>

namespace mika
{
  template<typename T, typename... Args>
  std::unique_ptr<T> make_unique(Args&&... args)
  {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
  }
}
/*
void processRenderBatch(GenericRenderer::RenderBatch * p_batch, CameraWorldBased * p_camera)
{
  p_camera->updateTransform();
  if(p_batch->models.size() < p_batch->components.size())
  {
    p_batch->models.resize(p_batch->components.size());
  }

  glm::i64vec2 cameraPos = p_camera->getPos();
  glm::i64 worldPerPixel = p_camera->getWorldPerPixel();

  for(int index = 0;
  index < p_batch->components.size();
    index++)
  {
    Component * component = p_batch->components[index];
    object->updateTransform(cameraPos, worldPerPixel);
    p_batch->models[index] = object->getTransform();
  }
}
*/

GenericRenderer::GenericRenderer()
{
  glGenBuffers(GenericRenderer::eNumOfBuffers, m_buffers);

  m_cameraTransformUnifName = mika::getConstChar(std::string("camera"));
  m_positionUnifName = mika::getConstChar(std::string("light.position"));
  m_intensityUnifName = mika::getConstChar(std::string("light.intensities"));
  m_attenuationUnifName = mika::getConstChar(std::string("light.attenuation"));
  m_ambientCoefficientUnifName = mika::getConstChar(std::string("light.ambientCoefficient"));

  m_cameraPosUnifName = mika::getConstChar(std::string("cameraPosition"));
  m_cameraExposureUnifName = mika::getConstChar(std::string("exposure"));
  m_lightDistanceModifierUnifName = mika::getConstChar(std::string("lightDistanceModifier"));

  m_lightPos = glm::vec3(0, 0, -2*OBJTOWORLD);
  m_lightColor = glm::vec3(1, 1, 1);
  m_attenuation = 0.0;
  m_ambient = 0.1;

  m_cameraPos = glm::vec3(0, 0, -2);
  m_exposure = 10;
  m_lightDistanceModifier = 1.0/OBJTOWORLD;

}


GenericRenderer::~GenericRenderer()
{
  glDeleteBuffers(GenericRenderer::eNumOfBuffers, m_buffers);
}

void GenericRenderer::setShader(Shader * p_shader)
{
/*  if(m_shader != p_shader)
  {
    Shader::Attribute attribute;
    if(p_shader->getAttribute("model", attribute))
    {
      m_modelVb = attribute;
    }
  }*/
  m_modelVb.id = Shader::eModel;
  m_selfIlluminationVb.id = Shader::eSelfIlluminationColor;
  m_shader = p_shader;
}

void GenericRenderer::setCamera(CameraWorldBased * p_camera)
{
  m_camera = p_camera;
}

void GenericRenderer::prepareRenderData(std::vector<Object*> p_objects, GameState * p_state)
{
  if(p_objects.empty())
  {
    return;
  }

  std::vector<std::unique_ptr<std::future<void>>> tasks;
  for(auto iter = m_renderBatches.begin();
  iter != m_renderBatches.end();
    iter++)
  {
    iter->second->components.clear();
    iter->second->models.clear();
    iter->second->selfIlluminationColors.clear();
  }

  Asset * currentAsset = nullptr;
  m_camera->updateTransform();
  glm::i64vec2 cameraPos = m_camera->getPos();
  glm::i64 worldPerPixel = m_camera->getWorldPerPixel();
  
//  std::cout << "Camera Pos " << cameraPos.x << ", " << cameraPos.y << std::endl;
//  std::cout << "worldPerPixel " << worldPerPixel << std::endl;
  
  typedef decltype(m_renderBatches.begin()) RenderBatchIterator;
  RenderBatchIterator iter;
  for(int index = 0;
  index < p_objects.size();
    index++)
  {
    p_objects[index]->updateTransform(cameraPos, worldPerPixel);
    std::vector<Component*> const & components = p_objects[index]->getComponents();
    for(Component * component : components)
    {
      std::vector<Asset * > const & assets = component->getAssets();
      for(Asset * asset : assets)
      {
        if(asset != currentAsset)
        {
          iter = m_renderBatches.find(asset);
          if(iter == m_renderBatches.end())
          {
            std::shared_ptr<RenderBatch> batch(new RenderBatch);
            batch->asset = asset;
            iter = m_renderBatches.insert(std::make_pair<>(asset, batch)).first;
          }
        }
        iter->second->components.push_back(component);
        iter->second->models.push_back(component->getModel());
        iter->second->selfIlluminationColors.push_back(component->getSelfIllumination());
      }
    }
  }

/*7
  for(auto iter = m_renderBatches.begin();
  iter != m_renderBatches.end();
    iter++)
  {
    tasks.push_back(mika::make_unique<std::future<void>>(getThreadPool().push(std::bind(processRenderBatch, iter->second.get(), m_camera))));
  }

  getThreadPool().waitAndDoTasks();
  while(!tasks.empty())
  {
    tasks.back()->wait();
    tasks.pop_back();
  }
*/
  
}

void GenericRenderer::render()
{
  m_shader->bind();
  for(auto iter = m_renderBatches.begin();
      iter != m_renderBatches.end();
      iter++)
  {
    glUniformMatrix4fv(m_shader->getUniformLocation(m_cameraTransformUnifName), 1, 0, &(m_camera->getTransform()[0][0]));
    glUniform3fv(m_shader->getUniformLocation(m_positionUnifName), 1, &m_lightPos[0]);
    glUniform3fv(m_shader->getUniformLocation(m_intensityUnifName), 1, &m_lightColor[0]);
    glUniform1fv(m_shader->getUniformLocation(m_attenuationUnifName), 1, &m_attenuation);
    glUniform1fv(m_shader->getUniformLocation(m_ambientCoefficientUnifName), 1, &m_ambient);
    glUniform3fv(m_shader->getUniformLocation(m_cameraPosUnifName), 1, &m_cameraPos[0]);
    glUniform1fv(m_shader->getUniformLocation(m_cameraExposureUnifName), 1, &m_exposure);
    glUniform1fv(m_shader->getUniformLocation(m_lightDistanceModifierUnifName), 1, &m_lightDistanceModifier);

    iter->second->asset->bind(m_shader);

    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[GenericRenderer::eModel_Vb]);
    glBufferData(GL_ARRAY_BUFFER, iter->second->models.size() * sizeof(iter->second->models[0]), iter->second->models.data(), GL_DYNAMIC_DRAW);
    for(unsigned int i = 0; i < 4; i++)
    {
      glEnableVertexAttribArray(m_modelVb.id + i);
      glVertexAttribPointer(m_modelVb.id + i, 4, GL_FLOAT, GL_FALSE, sizeof(iter->second->models[0]), (const GLvoid*) (sizeof(GLfloat) * i * 4));
      glVertexAttribDivisor(m_modelVb.id + i, 1);
    }
    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[GenericRenderer::eSelfIllumination_Vb]);
    glBufferData(GL_ARRAY_BUFFER, iter->second->selfIlluminationColors.size() * sizeof(iter->second->selfIlluminationColors[0]), iter->second->selfIlluminationColors.data(), GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(m_selfIlluminationVb.id);
    glVertexAttribPointer(m_selfIlluminationVb.id, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribDivisor(m_selfIlluminationVb.id, 1);

    glDrawElementsInstancedBaseVertex(GL_TRIANGLES,
                                      iter->second->asset->getMesh()->getDrawCount(),
                                      GL_UNSIGNED_INT,
                                      0,
                                      iter->second->components.size(),
                                      0);

    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[GenericRenderer::eModel_Vb]);
    for(unsigned int i = 0; i < 4; i++)
    {
      glDisableVertexAttribArray(m_modelVb.id + i);
    }
    iter->second->asset->unbind();
    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[GenericRenderer::eSelfIllumination_Vb]);
    glDisableVertexAttribArray(m_selfIlluminationVb.id);
  }
  m_shader->unbind();
}

