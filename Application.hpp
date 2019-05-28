#ifndef HPP_KOSU_APP
#define HPP_KOSU_APP

#include <functional>

#define _IRR_STATIC_LIB_
#include <irrlicht.h>


namespace kosu {


	class EventReader : public irr::IEventReceiver {
	public:
		EventReader();
		bool OnEvent(const irr::SEvent &e);
		bool quit;
	};


	class Application {
	public:
		

		Application();
		~Application();
		void run();
		void hookDraw(std::function<void(irr::video::IVideoDriver*)>);

		irr::IrrlichtDevice *device;
		irr::video::IVideoDriver *driver;
	private:
		EventReader e;
		std::function<void(irr::video::IVideoDriver*)> onDraw;

	};


}

#endif