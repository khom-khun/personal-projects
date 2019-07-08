#ifndef HPP_KOSU_CAMERA
#define HPP_KOSU_CAMERA

#include "ISceneNodeAnimator.h"
#include "vector2d.h"
#include "position2d.h"
#include "SKeyMap.h"

namespace kosu{
	using namespace irr;
	class DefaultCamera : public scene::ISceneNodeAnimator {
	public:
		DefaultCamera();

		virtual void animateNode(scene::IDummyTransformationSceneNode *node, uint32_t timeMs) override;
		virtual bool OnEvent(const SEvent &event) override;

		virtual ISceneNodeAnimator* createClone(scene::IDummyTransformationSceneNode* node, scene::ISceneManager* newManager = 0) override;

		virtual bool isEventReceiverEnabled() const override{
			return true;
		}
		
	protected:
		enum CAMERA_CONTROLS {
			MOVE_FORWARD = 0,
			MOVE_BACK,
			MOVE_LEFT,
			MOVE_RIGHT,

			TURN_LEFT,
			TURN_RIGHT,

			CAMERA_CONTROL_SIZE
		};
		bool keyMap_[CAMERA_CONTROL_SIZE];


		float moveSpeed_;
		float rotateSpeed_;
		int32_t lastTime_;
	};


}

#endif