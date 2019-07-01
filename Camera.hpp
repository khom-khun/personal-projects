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

	protected:
		core::vector<SKeyMap> keyMap_;

		float moveSpeed_;
		float rotateSpeed_;

		core::vector2df move_;

	};


}

#endif