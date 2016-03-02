set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})

message("CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}")

find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

set(Project3_SRCS
./CameraWorldBased.cpp
./Circle.cpp
./MtQuadTree.cpp
./PhysicsManager.cpp
./CameraBase.cpp
./ObjectRenderer.cpp
./Mesh.cpp
./Vertex.cpp
./LogicManager.cpp
./InputManager.cpp
./LightSource.cpp
./Asset.cpp
./Collision.cpp
./Transform2D.cpp
./Shape.cpp
./main.cpp
./Base.cpp
./Box.cpp
./SpatialTree.cpp
./ShapeCollisionDetector.cpp
./GameManager.cpp
./Object.cpp
./QuadTree.cpp
./ObjectShader.cpp
./RenderManager.cpp
./Display.cpp
./AABB.cpp
./obj_loader.cpp
./Camera.cpp
./GenericRenderer.cpp
./FpsManager.cpp
./Texture.cpp
./Transform.cpp
./utils.cpp
./Shader.cpp
)

set(Project3_HDRS
./Asset.h
./Manager.h
./Shader.h
./InputHandlers/Directions.h
./Shape.h
./obj_loader.h
./utils.h
./UserInputHandler.h
./QuadTree.h
./Circle.h
./SpinlockThreadpool.h
./AABB.h
./Camera.h
./ShapeCollisionHandler.h
./Display.h
./LightSource.h
./GameManager.h
./GenericRenderer.h
./Transform.h
./Box.h
./Texture.h
./SpatialTree.h
./InputManager.h
./GameState.h
./CameraWorldBased.h
./RenderManager.h
./Physics/Shape.h
./Physics/Collision/CircleToCircleCollision.h
./Physics/Collision/Collision.h
./Physics/Collision/CircleToPolygonCollision.h
./Physics/Contact/CircleContact.h
./Physics/Fixture.h
./Physics/ContactFilter.h
./Physics/Manifold.h
./Physics/Transform.h
./Physics/PhysCommon.h
./Physics/MassData.h
./Physics/Shapes/PolygonShape.h
./Physics/Shapes/CircleShape.h
./Physics/Contact.h
./Object.h
./MtQuadTree.h
./LogicManager.h
./cptl.h
./Transform2D.h
./Directions.h
./Base.h
./FpsManager.h
./Mesh.h
./Collision.h
./Vertex.h
./ObjectShader.h
./CameraBase.h
./ShapeCollisionDetector.h
./stb_image.h
./Spinlock.h
./ObjectRenderer.h
./PhysicsManager.h
)

set(Project3_INLS./InputHandlers/MoveOnKeyDownHandler.inl
./InputHandlers/CameraRightClickMoveHandler.inl
./InputHandlers/CameraMouseZoomHandler.inl
./InputHandlers/RightClickMoveHandler.inl
)

add_executable(Project3 ${Project3_SRCS} ${Project3_HDRS} ${Project3_INLS})

if(NOT GLM_INCLUDE_DIRS)
  find_path(GLM_INCLUDE_DIRS NAMES glm/glm.hpp)
endif()
if(NOT GLM_INCLUDE_DIRS)
  message(FATAL_ERROR "GLM_INCLUDE_DIRS not set or found \"${GLM_INCLUDE_DIRS}\"")
endif()
message("GLM_INCLUDE_DIRS \"${GLM_INCLUDE_DIRS}\"")
include_directories(${GLM_INCLUDE_DIRS} ${SDL2_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${GLEW_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR})

target_link_libraries(Project3 ${SDL2_LIBRARY} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES})
