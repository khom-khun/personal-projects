#ifndef HPP_KOSU_SHADER
#define HPP_KOSU_SHADER
#define _IRR_STATIC_LIB_
#include <irrlicht.h>

namespace kosu{
	class ShaderCallback : public irr::video::IShaderConstantSetCallBack {
	public:
		ShaderCallback();
		
		virtual void PostLink(irr::video::IMaterialRendererServices*, const irr::video::E_MATERIAL_TYPE&, const irr::core::vector<irr::video::SConstantLocationNamePair>&) override;
		virtual void OnSetConstants(irr::video::IMaterialRendererServices*, int32_t) override;
		virtual void OnUnsetMaterial() override{}
	private:
		irr::video::SConstantLocationNamePair mvp;
	};
}

#endif