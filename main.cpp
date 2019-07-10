#define _IRR_STATIC_LIB_
#include <irrlicht.h>
#include "../source/Irrlicht/COpenGLExtensionHandler.h"
#include "../source/Irrlicht/COpenGLBuffer.h"
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

uniform mat4 mvp;

out vec2 tCoord;
        
void main(){
    gl_Position = mvp * pos;
    tCoord = texCoord;
}

)";

const char* fS = R"(
#version 430 core

in vec2 tCoord;

layout(location = 0) out vec4 color;

layout(binding = 0) uniform sampler2D tex0;

void main(){
    color = texture(tex0, tCoord);
}

)";

const char* vST = R"(
#version 430 core
layout(location = 0) in vec4 pos;

layout(binding = 0) uniform samplerBuffer tex0;

out float clr;

uniform mat4 mvp;
void main(){
	vec4 data = texelFetch(tex0, int(pos.w));
	clr = data.x;

    gl_Position = mvp * (vec4(pos.xyz, 1.0) + vec4(0, data.x * 3, 0, 0));
}

)";

const char* fST = R"(
#version 430 core
layout(location = 0) out vec4 color;
in float clr;

void main(){
	color = vec4(clr,clr,clr,1.0);
}

)";

int main(){
	
	kosu::Application app;

	if (app.device == 0)
		return 1; 

	scene::ICameraSceneNode *cam = app.device->getSceneManager()->addCameraSceneNode();
	scene::ISceneNodeAnimator *anim = new kosu::DefaultCamera;
	cam->addAnimator(anim);
	anim->drop();

	cam->setPosition({0,10,-1});
	cam->setTarget({0,5,0});
	cam->setNearValue(0.01f);
	cam->setFarValue(10000.0f);
	app.device->getSceneManager()->setActiveCamera(cam);

	asset::IAssetLoader::SAssetLoadParams lparams;
	asset::ICPUTexture* cpuTex = static_cast<asset::ICPUTexture*>(app.device->getAssetManager().getAsset("../../media/wall.jpg", lparams));
	video::ITexture* tex = app.driver->getGPUObjectsFromAssets(&cpuTex, (&cpuTex) + 1).front();
	

	std::pair<video::IGPUMeshBuffer*, video::ITextureBufferObject*> terrain =  kosu::terrain(app.driver, static_cast<asset::ICPUTexture*>(app.device->getAssetManager().getAsset("../../media/heightmap.jpg", lparams)));

	video::IGPUMeshBuffer *mesh = kosu::quad(app.driver, core::vector3df_SIMD(-0.5, 1, 0), core::vector3df_SIMD(0.5, 0, 0));


	video::SGPUMaterial mat;
	
	{
	kosu::ShaderCallback *cb = new kosu::ShaderCallback;

	mat.BackfaceCulling = false;
	mat.MaterialType = (video::E_MATERIAL_TYPE)app.driver->getGPUProgrammingServices()
		->addHighLevelShaderMaterial(vS, nullptr, nullptr, nullptr, fS, 3, video::EMT_SOLID, cb);
	cb->drop();
	
	mat.setTexture(0, tex);
	}

	video::SGPUMaterial mat2;

	{
	kosu::ShaderCallback* cb = new kosu::ShaderCallback;
	//mat2.Wireframe = true;
	mat2.BackfaceCulling = false;
	mat2.MaterialType = (video::E_MATERIAL_TYPE)app.driver->getGPUProgrammingServices()
		->addHighLevelShaderMaterial(vST, nullptr, nullptr, nullptr, fST, 3, video::EMT_SOLID, cb);
	cb->drop();
	
	mat2.setTexture(0, terrain.second);
	}


	app.hookDraw([&](video::IVideoDriver *driver) {
		driver->setTransform(video::E4X3TS_WORLD, core::matrix4x3());
		driver->setMaterial(mat);
		driver->drawMeshBuffer(mesh);

		driver->setMaterial(mat2);
		driver->drawMeshBuffer(terrain.first);

	});
	app.run();

	//cleanup session
	mesh->drop();

	return 0;
}
