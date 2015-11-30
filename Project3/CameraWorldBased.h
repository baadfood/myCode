#ifndef CameraWorldBased_h_
#define CameraWorldBased_h_

#include "CameraBase.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "AABB.h"

class CameraWorldBased : public CameraBase
{
public:
  CameraWorldBased(glm::i64vec2 const & p_pos, glm::i32vec2 p_pixelSize, glm::i64 p_worldPerPixel, glm::float32 p_exposure);
  virtual ~CameraWorldBased();

  virtual void setHalfSize(glm::u64vec2 p_size);
  virtual AABB const & getAABB() const;

  void setWorldPerPixel(glm::i64 p_worldPerPixel);
  glm::i64 const & getWorldPerPixel() const;

  void updateTransform();
  glm::mat4 const & getTransform() const;

  virtual glm::i64vec2 screenToWorld(glm::i32vec2 const & p_screen);

  void setExposure(glm::float32 p_exposure);

  glm::float32 const & getExposure() const;
  virtual void lookAt(Object * p_object);

  virtual glm::int64 getXPos() const;
  virtual glm::int64 getYPos() const;
  virtual glm::i64vec2 const & getPos() const;

protected:
private:
//  std::vector<std::shared_ptr<ObjectRenderer>> m_renderers;
  AABB m_aabb;
  glm::i64vec2 m_pos;
  glm::i32vec2 m_pixelSize;
  glm::i64 m_worldPerPixel;
  glm::float32 m_exposure;
  glm::mat4 m_perspective;

  Object * m_lookAt;
};

#endif
