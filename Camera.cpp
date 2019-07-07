#include "Camera.hpp"
#include "ICameraSceneNode.h"
#include <iostream>
using namespace kosu;
using namespace irr;

DefaultCamera::DefaultCamera() : moveSpeed_(10.0f), rotateSpeed_(4.0f), move_(0.0f, 0.0f), look_(0.0f, 0.0f), lastTime_(0.0f){
	
}

/*

                cos  -->   - 
	UNIT CIRCLE            O |
	                         ^
							 |
							sin

*/

void DefaultCamera::animateNode(scene::IDummyTransformationSceneNode *node, uint32_t timeMs){
	scene::ICameraSceneNode* camera = static_cast<scene::ICameraSceneNode*>(node);
		
	float timeDiff = (float)(timeMs - lastTime_)/1000;
	lastTime_ = timeMs;
	
	if(timeDiff < 0 || timeDiff > 10000) timeDiff = lastFrameStep_;
	lastFrameStep_ = timeDiff;

	core::vector3df pos = camera->getPosition();
	core::vector3df target = (camera->getTarget() - camera->getPosition());

	target.set(
		core::clamp(target.X, -1.0f, 1.0f),
		core::clamp(target.Y, -1.0f, 1.0f),
		core::clamp(target.Z, -1.0f, 1.0f)
	);
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
			case KEY_LEFT:
				look_.Y = -1.0f;
				return true;
			case KEY_RIGHT:
				look_.Y = 1.0f;
				return true;
			case KEY_KEY_W:
				move_.Y = 1.0f;
				return true;
			case KEY_KEY_S:
				move_.Y = -1.0f;
				return true;

			case KEY_KEY_A:
				move_.X = 1.0f;
				return true;
			case KEY_KEY_D:
				move_.X = -1.0f;
				return true;
			}
		}
		else {
			switch (e.KeyInput.Key) {
			case KEY_LEFT:
			case KEY_RIGHT:
				look_.Y = 0.0f;
				return true;
			case KEY_KEY_W:
			case KEY_KEY_S:
				move_.Y = 0.0f;
				return true;
			case KEY_KEY_A:
			case KEY_KEY_D:
				move_.X = 0.0f;
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