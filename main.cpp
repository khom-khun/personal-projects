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
layout(location = 1) in vec4 color;   

uniform mat4 mvp;

out vec4 vColor;   
        
void main(){
    gl_Position = mvp * pos;
    vColor = color;
}

)";

const char* fS = R"(
#version 430 core

in vec4 vColor;

layout(location = 0) out vec4 color;

void main(){
    color = vColor;
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

	cam->setPosition({0,0,-1});
	cam->setTarget({0,0,0});
	cam->setNearValue(0.01f);
	cam->setFarValue(10.0f);
	app.device->getSceneManager()->setActiveCamera(cam);

	

	video::IGPUMeshBuffer *mesh = kosu::quad(app.driver);


	video::SGPUMaterial mat;
	
	kosu::ShaderCallback *cb = new kosu::ShaderCallback;

	mat.BackfaceCulling = false;
	mat.MaterialType = (video::E_MATERIAL_TYPE)app.driver->getGPUProgrammingServices()
		->addHighLevelShaderMaterial(vS, nullptr, nullptr, nullptr, fS, 3, video::EMT_SOLID, cb);
	cb->drop();


	app.hookDraw([&](video::IVideoDriver *driver) {
		driver->setTransform(video::E4X3TS_WORLD, core::matrix4x3());
		driver->setMaterial(mat);
		driver->drawMeshBuffer(mesh);
	});
	app.run();

	//cleanup session
	mesh->drop();

	return 0;
}
