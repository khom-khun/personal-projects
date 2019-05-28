#ifndef HPP_KOSU_CAMERA
#define HPP_KOSU_CAMERA

#include "ISceneNodeAnimatorCameraFPS.h"
#include "vector2d.h"
#include "position2d.h"
#include "SKeyMap.h"

namespace kosu{
	using namespace irr;
	class DefaultCamera : public scene::ISceneNodeAnimatorCameraFPS {
	public:
		DefaultCamera();

		virtual void animateNode(scene::IDummyTransformationSceneNode *node, uint32_t timeMs) override;
		virtual bool OnEvent(const SEvent &event) override;

		virtual float getMoveSpeed() const override;
		virtual void setMoveSpeed(float moveSpeed) override;

		virtual float getRotateSpeed() const override;
		virtual void setRotateSpeed(float rotateSpeed) override;

		virtual void setKeyMap(SKeyMap *map, uint32_t count) override;
		virtual void setKeyMap(const core::vector<SKeyMap> &keymap) override;
		virtual const core::vector<SKeyMap> &getKeyMap() const override;

		virtual void setVerticalMovement(bool allow) override;
		virtual void setInvertMouse(bool invert) override;
	
		virtual ISceneNodeAnimator* createClone(scene::IDummyTransformationSceneNode* node, scene::ISceneManager* newManager = 0) override;

	protected:
		core::vector<SKeyMap> keyMap_;

		float moveSpeed_;
		float rotateSpeed_;

		core::vector2df move_;

	};


}

#endif