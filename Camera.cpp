#include "Camera.hpp"
#include "ICameraSceneNode.h"
#include <iostream>
using namespace kosu;
using namespace irr;

DefaultCamera::DefaultCamera() : moveSpeed_(10.0f), rotateSpeed_(15.0f), move_(0.0f, 0.0f){
	
}

void DefaultCamera::animateNode(scene::IDummyTransformationSceneNode *node, uint32_t timeMs){
	scene::ICameraSceneNode* camera = static_cast<scene::ICameraSceneNode*>(node);
	camera->updateAbsolutePosition();
	bool x = this->isEventReceiverEnabled();	

	move_ = core::vector2df(0.0f, 0.0f);
}

bool DefaultCamera::OnEvent(const SEvent &e){
	int x;
	x = 10;
	x = 20;
	std::cout << "FUCK\n";
	switch(e.EventType){
	case EET_KEY_INPUT_EVENT:
		if(!e.KeyInput.PressedDown) return false;

		switch(e.KeyInput.Key){
		case KEY_KEY_W:
			move_.Y = moveSpeed_;
			break;
		case KEY_KEY_S:
			move_.Y = -moveSpeed_;
			break;

		case KEY_KEY_A:
			move_.X = -moveSpeed_;
			break;

		case KEY_KEY_D:
			move_.X = moveSpeed_;
			break;

		}


		break;
	}
	return false;
}

scene::ISceneNodeAnimator* DefaultCamera::createClone(scene::IDummyTransformationSceneNode* node, scene::ISceneManager* newManager){
	return new DefaultCamera;
}