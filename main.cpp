#define _IRR_STATIC_LIB_
#include <irrlicht.h>
#include "../source/Irrlicht/COpenGLExtensionHandler.h"
#include "../source/Irrlicht/COpenGLBuffer.h"
#include "../source/Irrlicht/COpenGLDriver.h"

#include "Application.hpp"
#include "Geom.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "CSceneNodeAnimatorCameraFPS.h"

using namespace irr;


const char *vS = R"(
#version 430 core
layout(location = 0) in vec4 pos;
layout(location = 1) in vec2 texCoord;   


layout(location = 0) out vec2 tCoord;
layout(location = 1) out vec4 posLightSpace;


uniform mat4 mvp;
uniform mat4 lightmvp;

        
void main(){
    gl_Position = mvp * pos;
    tCoord = texCoord;
	posLightSpace = lightmvp * pos;
}

)";

const char* fS = R"(
#version 430 core


layout(location = 0) in vec2 tCoord;
layout(location = 1) in vec4 posLightSpace;

layout(location = 0) out vec4 color;

layout(binding = 0) uniform sampler2D tex0;
layout(binding = 1) uniform sampler2D shadowMap;

bool range(float val){
	return val >= 0.0 && val <= 1.0;
}

void main(){

	vec3 projCoords = posLightSpace.xyz / posLightSpace.w;
	projCoords = projCoords	* 0.5 + 0.5;

	float closeDepth = texture(shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
		
	float shadowDepth = texture2D(shadowMap, projCoords.xy).x;
	float shadow = currentDepth - shadowDepth;
	


    color = (1.0 - shadow) * texture(tex0, tCoord);
}
)";

const char* vSSM = R"(
#version 430 core
layout(location = 0) in vec4 pos;
layout(location = 1) in vec2 texCoord;   

uniform mat4 lightmvp;

out vec2 tCoord;
        
void main(){
    gl_Position = lightmvp * pos;
    tCoord = texCoord;
}

)";

const char* fSSM = R"(
#version 430 core

in vec2 tCoord;

layout(location = 0) out vec4 color;

layout(binding = 0) uniform sampler2D tex0;

void main(){
    color = texture(tex0, tCoord);
}

)";

core::matrix4SIMD lightMat;





class SimpleCallBack : public video::IShaderConstantSetCallBack
{
public:
	SimpleCallBack(){}

	virtual void PostLink(video::IMaterialRendererServices* services, const video::E_MATERIAL_TYPE& materialType, const core::vector<video::SConstantLocationNamePair>& v)
	{
		uniforms = v;
	}	

	virtual void OnSetConstants(video::IMaterialRendererServices* services, int32_t userData)
	{
		for(auto &mvp : uniforms){
			if (mvp.name == "mvp")
				services->setShaderConstant(services->getVideoDriver()->getTransform(video::EPTS_PROJ_VIEW_WORLD).pointer(), mvp.location, mvp.type);
			if (mvp.name == "lightmvp")
				services->setShaderConstant(&lightMat, mvp.location, mvp.type);
		}

	
	}

	virtual void OnUnsetMaterial() {}
private:
	core::vector<video::SConstantLocationNamePair> uniforms;


};







int main(){
	
	kosu::Application app;

	if (app.device == 0)
		return 1; 


	app.driver->setTextureCreationFlag(video::ETCF_ALWAYS_32_BIT, true);
	app.driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);


	app.driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, true);


	scene::ICameraSceneNode *cam = app.device->getSceneManager()->addCameraSceneNode();
	scene::ISceneNodeAnimator *anim = new kosu::DefaultCamera;
	cam->addAnimator(anim);
	anim->drop();

	cam->setPosition({0,0.5,-1.0});
	cam->setTarget({0,0.5,0});
	cam->setNearValue(0.1f);
	cam->setFarValue(100.0f);
	app.device->getSceneManager()->setActiveCamera(app.device->getSceneManager()->addCameraSceneNodeFPS());
	
	asset::IAssetLoader::SAssetLoadParams lparams;
	asset::ICPUTexture* cpuTex = static_cast<asset::ICPUTexture*>(app.device->getAssetManager().getAsset("../../media/wall.jpg", lparams));
	video::ITexture* tex = app.driver->getGPUObjectsFromAssets(&cpuTex, (&cpuTex) + 1).front();
	
	
	
	video::IGPUMeshBuffer *mesh = kosu::quad(app.driver, core::vector3df_SIMD(-1, 1, 1), core::vector3df_SIMD(1, 1, -1));
	video::IGPUMeshBuffer *mesh2 = kosu::quad(app.driver, core::vector3df_SIMD(-3, -0.1, 3), core::vector3df_SIMD(3, -0.1, -3));

	video::SGPUMaterial mat, mat2;
	
	{
	SimpleCallBack *cb = new SimpleCallBack;

	mat.BackfaceCulling = false;
	mat.MaterialType = (video::E_MATERIAL_TYPE)app.driver->getGPUProgrammingServices()
		->addHighLevelShaderMaterial(vS, nullptr, nullptr, nullptr, fS, 3, video::EMT_SOLID, cb);
	cb->drop();
	
	mat.setTexture(0, tex);
	}

	{
		SimpleCallBack* cb = new SimpleCallBack;

		mat2.BackfaceCulling = false;
		mat2.MaterialType = (video::E_MATERIAL_TYPE)app.driver->getGPUProgrammingServices()
			->addHighLevelShaderMaterial(vSSM, nullptr, nullptr, nullptr, fSSM, 3, video::EMT_SOLID, cb);
		cb->drop();

		mat2.setTexture(0, tex);
	}


	video::IFrameBuffer *fbo = app.driver->addFrameBuffer();
	uint32_t depthTexSize[] = { 1024u, 1024u, 1u };
	auto test = core::dimension2d<uint32_t>(1280, 720);
	video::ITexture *depth = app.driver->createGPUTexture(video::ITexture::ETT_2D, depthTexSize, 1u, asset::EF_D32_SFLOAT);
	


	{
		video::STextureSamplingParams params;
		params.UseMipmaps = 0;
		params.MaxFilter = params.MinFilter = video::ETFT_LINEAR_NO_MIP;
		params.TextureWrapU = params.TextureWrapV = video::ETC_CLAMP_TO_EDGE;
		const_cast<video::COpenGLDriver::SAuxContext*>(reinterpret_cast<video::COpenGLDriver*>(app.driver)->getThreadContext())->setActiveTexture(0, depth, params);
	}


	fbo->attach(video::EFAP_DEPTH_ATTACHMENT, depth);
	mat.setTexture(1, depth);
	//core::matrix4SIMD::buildProjectionMatrixPerspectiveFovRH(0.5f * core::PI, (1280.0 / 720.0), 0.1, 100)
	lightMat = core::concatenateBFollowedByA(core::matrix4SIMD::buildProjectionMatrixOrthoRH(20, 20, 1, 7.5),
		core::matrix3x4SIMD::buildCameraLookAtMatrixLH(core::vectorSIMDf(-2.0f, 4.0f, -1.0f), core::vectorSIMDf(0.0f, 0.0f, 0.0f), core::vectorSIMDf(0.0f, 1.0f, 0.0f))
	);


	auto draw = [&](video::IVideoDriver *driver){
		driver->drawMeshBuffer(mesh);
		driver->drawMeshBuffer(mesh2);
	
	};

	app.hookDraw([&](video::IVideoDriver *driver) {
		
		

		driver->setRenderTarget(fbo);
		driver->clearZBuffer();
		driver->setMaterial(mat2);
		draw(driver);

		driver->setRenderTarget(0);
		driver->setMaterial(mat);
		draw(driver);
		
	});
	app.run();

	//cleanup session
	mesh->drop();
	mesh2->drop();
	return 0;
}
