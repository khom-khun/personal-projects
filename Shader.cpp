#include "Shader.hpp"

using namespace kosu;
using namespace irr;

ShaderCallback::ShaderCallback(){}

void ShaderCallback::PostLink(video::IMaterialRendererServices*, 
							  const video::E_MATERIAL_TYPE&, 
							  const core::vector<video::SConstantLocationNamePair>& v){
	for(auto &e : v){
		if(e.name == "mvp") mvp = e;
	}

	

}


void ShaderCallback::OnSetConstants(video::IMaterialRendererServices *service, int32_t){
	if(mvp.name == "mvp")
		service->setShaderConstant(service->getVideoDriver()->getTransform(video::EPTS_PROJ_VIEW_WORLD).pointer(), mvp.location, mvp.type);
}
