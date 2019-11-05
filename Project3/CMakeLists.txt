set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})

message("CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}")

find_package(SDL2 REQUIRED)

find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Boost REQUIRED)
#find_package(Zmq REQUIRED)

set(Project3_RES
./res/objectShader.vs
./res/texture.jpg
./res/monkey.obj
./res/sensor.png
./res/fighter.png
./res/basicShader.vs
./res/objectShader2.fs
./res/F5S4.png
./res/objectShader.fs
./res/F5S4_.png
./res/basicShader.fs
./res/objectShader2.vs
)
    
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
./Components/Component.cpp
./Components/Sensor.cpp
./Components/Engine.cpp
./AI/BasicAi.cpp
./AI/Ai.cpp
./AI/FlyByWireAi.cpp
./Physics/Collision/CollisionHandler.cpp
)

set(Project3_HDRS
./Asset.h
./Manager.h
./Shader.h
./InputHandlers/Directions.h
./DebugPrint.h
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
./Physics/Collision/CollisionHandler.h
./Physics/Collision/CircleToPolygonCollision.h
./Physics/Collision/PolygonToPolygonCollision.h
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
./Components/Component.h
./Components/Sensor.h
./Components/Engine.h
./AI/BasicAi.h
./AI/Ai.h
./AI/FlyByWireAi.h
)

set(Project3_INLS
./InputHandlers/MoveOnKeyDownHandler.inl
./InputHandlers/CameraRightClickMoveHandler.inl
./InputHandlers/CameraMouseZoomHandler.inl
./InputHandlers/RightClickMoveHandler.inl
./InputHandlers/FlyByWireAiController.inl
)

foreach(RES_FILE ${Project3_RES})

set(Project3_RESC ${Project3_RESC} "${CMAKE_CURRENT_BINARY_DIR}/${RES_FILE}")

include(CreateSourceGroupsForTarget.txt)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${RES_FILE}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${RES_FILE}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${RES_FILE} ${CMAKE_CURRENT_BINARY_DIR}/${RES_FILE})
    
    set_source_files_properties(
    ${CMAKE_CURRENT_BINARY_DIR}/${RES_FILE}
    PROPERTIES
    GENERATED TRUE
    HEADER_FILE_ONLY TRUE)
    
endforeach()

add_executable(Project3 ${Project3_SRCS} ${Project3_HDRS} ${Project3_INLS} ${Project3_RESC})

create_source_groups_for_target(Project3)



if(NOT GLM_INCLUDE_DIRS)
  find_path(GLM_INCLUDE_DIRS NAMES glm/glm.hpp HINTS "${CMAKE_SOURCE_DIR}/glm")
endif()
if(NOT GLM_INCLUDE_DIRS)
  message(FATAL_ERROR "GLM_INCLUDE_DIRS not set or found \"${GLM_INCLUDE_DIRS}\"")
endif()
message("GLM_INCLUDE_DIRS \"${GLM_INCLUDE_DIRS}\"")
include_directories(${GLM_INCLUDE_DIRS} ${SDL2_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${GLEW_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR} ${Boost_INCLUDE_DIR})

target_link_libraries(Project3 ${SDL2_LIBRARY} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES} ${ZMQ_LIB})

