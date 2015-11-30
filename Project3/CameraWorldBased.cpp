#include "CameraWorldBased.h"

#include "UserInputHandler.h"
#include "InputHandlers/CameraMouseZoomHandler.inl"
#include "InputHandlers/CameraRightClickMoveHandler.inl"

CameraWorldBased::CameraWorldBased(glm::i64vec2 const & p_pos, glm::i32vec2 p_pixelSize, glm::i64 p_worldPerPixel, glm::float32 p_exposure):
m_pos(p_pos),
m_perspective(1.0f),
m_worldPerPixel(p_worldPerPixel),
m_pixelSize(p_pixelSize),
m_exposure(p_exposure),
m_lookAt(nullptr)
{
  updateTransform();
  addInputHandler(std::shared_ptr<UserInputHandler>(new CameraMouseZoomHandler(this)));
  addInputHandler(std::shared_ptr<UserInputHandler>(new CameraRightClickMoveHandler(this)));
}

CameraWorldBased::~CameraWorldBased()
{
}

void CameraWorldBased::setHalfSize(glm::u64vec2 p_size)
{
  float aspectOld = float(double(m_pixelSize.x) / double(m_pixelSize.y));
  float aspectNew = float(double(p_size.x) / double(p_size.y));

  if (aspectOld > aspectNew)
  {
    m_worldPerPixel = 2 * p_size.y / m_pixelSize.y;
  }
  else
  {
    m_worldPerPixel = 2 * p_size.x / m_pixelSize.x;
  }

  Object::setHalfSize(p_size);
  updateTransform();
  updateAabb();
}

void CameraWorldBased::setWorldPerPixel(glm::i64 p_worldPerPixel)
{
  m_worldPerPixel = p_worldPerPixel;

  glm::u64vec2 halfSize;
  halfSize.x = (m_pixelSize.x / 2) * m_worldPerPixel;
  halfSize.y = (m_pixelSize.y / 2) * m_worldPerPixel;

  Object::setHalfSize(halfSize);
  updateTransform();
  updateAabb();
}

glm::i64 const & CameraWorldBased::getWorldPerPixel() const
{
  return m_worldPerPixel;
}

AABB const & CameraWorldBased::getAABB() const
{
  if(m_lookAt)
  {
    AABB * aabb = const_cast<AABB *>(&m_aabb);
    aabb->setCenter(m_lookAt->getPos());
    aabb->setSize(getHalfSize());
    return m_aabb;
  }
  return Object::getAABB();
}

void CameraWorldBased::updateTransform()
{
  if (m_worldPerPixel < 1)
  {
    m_worldPerPixel = 1;
  }
  m_perspective = glm::ortho(double(-m_pixelSize.x) /2, double(m_pixelSize.x)  /2, double(-m_pixelSize.y) /2, double(m_pixelSize.y) /2);
/*  glm::vec3 scale(1/double(m_worldPerPixel), 1/double(m_worldPerPixel), 0.0f);
  m_perspective *= glm::scale(glm::mat4(1.0f), scale);*/

  Object::setHalfSize(glm::i64vec2(m_pixelSize.x * m_worldPerPixel / 2, m_pixelSize.x * m_worldPerPixel / 2));
  Object::updateAabb();
}


glm::mat4 const & CameraWorldBased::getTransform() const
{
  return m_perspective;
}

glm::i64vec2 CameraWorldBased::screenToWorld(glm::i32vec2 const & p_screen)
{
  glm::i64vec2 pos = p_screen;

  pos -= glm::vec2(m_pixelSize.x/2, m_pixelSize.y/2);
  pos.y = -pos.y;

  pos *= m_worldPerPixel;
  pos += getPos();

  return pos;
}

void CameraWorldBased::setExposure(glm::float32 p_exposure)
{
  m_exposure = p_exposure;
}

glm::float32 const & CameraWorldBased::getExposure() const
{
  return m_exposure;
}

void CameraWorldBased::lookAt(Object * p_object)
{
  m_lookAt = p_object;
}

glm::int64 CameraWorldBased::getXPos() const
{
  if (m_lookAt)
  {
    return m_lookAt->getXPos();
  }
  return Object::getXPos();
}

glm::int64 CameraWorldBased::getYPos() const
{
  if (m_lookAt)
  {
    return m_lookAt->getYPos();
  }
  return Object::getYPos();
}

glm::i64vec2 const & CameraWorldBased::getPos() const
{
  if (m_lookAt)
  {
    return m_lookAt->getPos();
  }
  return Object::getPos();
}

