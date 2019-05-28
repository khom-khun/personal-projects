#include "Application.hpp"
#include <iostream>

using namespace kosu;
using namespace irr;

EventReader::EventReader() : quit(false){
}


bool EventReader::OnEvent(const SEvent& e){
	switch(e.EventType){
	case irr::EET_KEY_INPUT_EVENT:
		if(e.KeyInput.Key == KEY_KEY_Q)
			quit = true;
			return true;
		break;
	}
	return false;
}

Application::Application(){
	irr::SIrrlichtCreationParameters params;
	params.Bits = 24;
	params.ZBufferBits = 24;
	params.DriverType = video::EDT_OPENGL;
	params.WindowSize = core::dimension2d<uint32_t>(1280, 720);
	params.Fullscreen = false;
	params.Vsync = true;
	params.Doublebuffer = true;
	params.Stencilbuffer = false;
	params.AuxGLContexts = 16;
	params.EventReceiver = &e;


	device = createDeviceEx(params);
	device->setResizable(true);
	driver = device->getVideoDriver();
}

Application::~Application(){
	device->drop();
	
}

void Application::run(){
	

	while(!e.quit && device->run()){
		if(!device->isWindowActive()) continue;
		std::chrono::duration<double, std::ratio<1>> time = device->getTimer()->getTime();



		driver->beginScene(true, true, video::SColor(255, 255, 0, 255)); 
		device->getSceneManager()->drawAll();
	

		if(onDraw) onDraw(driver);
		driver->endScene();
		if(time.count() > 0.5){
			std::wostringstream str;
			str << L"GPU Mesh Demo - Irrlicht Engine [" << driver->getName() << "] FPS:" << (time.count() * 60);

			device->setWindowCaption(str.str().c_str());
			device->getTimer()->setTime(std::chrono::seconds(0));
		}
	}
}

void Application::hookDraw(std::function<void(video::IVideoDriver*)> fn){
	onDraw = fn;
}