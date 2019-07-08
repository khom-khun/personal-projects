#include "Camera.hpp"
#include "ICameraSceneNode.h"
#include <iostream>
using namespace kosu;
using namespace irr;

DefaultCamera::DefaultCamera() : moveSpeed_(10.0f), rotateSpeed_(4.0f), lastTime_(0.0f){
	for(bool &v : keyMap_){ v = false; }
}

/*

                cos  -->   - 
	UNIT CIRCLE            O |
	                         ^
							 |
							sin

*/

void DefaultCamera::animateNode(scene::IDummyTransformationSceneNode *node, uint32_t timeMs){
	core::vector2df look_ = core::vector2df(0, 0);
	core::vector2df move_ = core::vector2df(0, 0);


	if (keyMap_[CAMERA_CONTROLS::MOVE_FORWARD])
		move_.Y += 1.0f;
	if (keyMap_[CAMERA_CONTROLS::MOVE_BACK])
		move_.Y -= 1.0f;
	if (keyMap_[CAMERA_CONTROLS::MOVE_LEFT])
		move_.X += 1.0f;
	if (keyMap_[CAMERA_CONTROLS::MOVE_RIGHT])
		move_.X -= 1.0f;
	if (keyMap_[CAMERA_CONTROLS::TURN_LEFT])
		look_.Y -= 1.0f;
	if (keyMap_[CAMERA_CONTROLS::TURN_RIGHT])
		look_.Y += 1.0f;

	scene::ICameraSceneNode* camera = static_cast<scene::ICameraSceneNode*>(node);
		
	float timeDiff = (float)(timeMs - lastTime_)/1000;
	lastTime_ = timeMs;

	core::vector3df pos = camera->getPosition();
	core::vector3df target = (camera->getTarget() - camera->getPosition());

	target = core::clamp(&target.X, core::vectorSIMDf(-1,-1,-1), core::vectorSIMDf(1,1,1)).getAsVector3df();

	core::matrix4x3 mat;
	mat.setRotationAxisRadians(core::degToRad(rotateSpeed_ * look_.Y), core::vector3df(0,1,0));
	mat.transformVect(&target.X);
	target.normalize();


	core::vector3df strafeDir = target;
	strafeDir = strafeDir.crossProduct(camera->getUpVector());


	pos += (target * move_.Y * timeDiff);
	pos += (strafeDir * move_.X * timeDiff);
	
	camera->setPosition(pos);
	camera->setTarget(target + camera->getPosition());


}

bool DefaultCamera::OnEvent(const SEvent &e){
	switch(e.EventType){
	case EET_KEY_INPUT_EVENT:
		if(e.KeyInput.PressedDown){
			switch(e.KeyInput.Key){
			case KEY_KEY_W:
				keyMap_[CAMERA_CONTROLS::MOVE_FORWARD] = true;
				return true;
			case KEY_KEY_S:
				keyMap_[CAMERA_CONTROLS::MOVE_BACK] = true;
				return true;
			case KEY_KEY_A:
				keyMap_[CAMERA_CONTROLS::MOVE_LEFT] = true;
				return true;
			case KEY_KEY_D:
				keyMap_[CAMERA_CONTROLS::MOVE_RIGHT] = true;
				return true;
			case KEY_LEFT:
				keyMap_[CAMERA_CONTROLS::TURN_LEFT] = true;
				return true;
			case KEY_RIGHT:
				keyMap_[CAMERA_CONTROLS::TURN_RIGHT] = true;
				return true;
			}
		}
		else {
			switch (e.KeyInput.Key) {
			case KEY_KEY_W:
				keyMap_[CAMERA_CONTROLS::MOVE_FORWARD] = false;
				return true;
			case KEY_KEY_S:
				keyMap_[CAMERA_CONTROLS::MOVE_BACK] = false;
				return true;
			case KEY_KEY_A:
				keyMap_[CAMERA_CONTROLS::MOVE_LEFT] = false;
				return true;
			case KEY_KEY_D:
				keyMap_[CAMERA_CONTROLS::MOVE_RIGHT] = false;
				return true;
			case KEY_LEFT:
				keyMap_[CAMERA_CONTROLS::TURN_LEFT] = false;
				return true;
			case KEY_RIGHT:
				keyMap_[CAMERA_CONTROLS::TURN_RIGHT] = false;
				return true;
			}
		}

		break;
	}
	return false;
}

scene::ISceneNodeAnimator* DefaultCamera::createClone(scene::IDummyTransformationSceneNode* node, scene::ISceneManager* newManager){
	return new DefaultCamera;
}