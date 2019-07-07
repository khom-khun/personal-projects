#ifndef HPP_KOSU_GEOMETRY
#define HPP_KOSU_GEOMETRY
#include <irrlicht.h>

namespace kosu{
	using namespace irr;


#include "irr/irrpack.h"
	struct DefaultLayout {
		float v[3];
		uint8_t c[3];
		uint8_t padding[1];
	} PACK_STRUCT;
#include "irr/irrunpack.h"

	video::IGPUMeshBuffer *quad(video::IVideoDriver *driver,
		core::vector3df_SIMD tL = core::vector3df_SIMD(-0.5, 0.5, 0.0), core::vector3df_SIMD bR = core::vector3df_SIMD(0.5, -0.5, 0.0)
		){
		DefaultLayout vD[4];
		vD[0] = DefaultLayout{ {tL.x, tL.y, tL.z}, {255,0,0} };
		vD[1] = DefaultLayout{ {bR.x, tL.y, tL.z}, {0,255,0} };
		vD[2] = DefaultLayout{ {bR.x, bR.y, bR.z}, {0,0,255} };
		vD[3] = DefaultLayout{ {tL.x, bR.y, bR.z}, {0,255,255} };

		uint16_t iD[] = {
			0,1,2,	0,2,3
		};

		video::IGPUBuffer *vb = driver->createDeviceLocalGPUBufferOnDedMem(sizeof(vD));
		video::IGPUBuffer *ib = driver->createDeviceLocalGPUBufferOnDedMem(sizeof(iD));

		video::IGPUMeshDataFormatDesc *desc = driver->createGPUMeshDataFormatDesc();
		video::IGPUMeshBuffer *mb = new video::IGPUMeshBuffer;
		mb->setMeshDataAndFormat(desc);
		desc->drop();

		driver->updateBufferRangeViaStagingBuffer(vb, 0, sizeof(vD), vD);
		driver->updateBufferRangeViaStagingBuffer(ib, 0, sizeof(iD), iD);


		desc->setVertexAttrBuffer(vb, asset::EVAI_ATTR0, asset::EF_R32G32B32_SFLOAT, sizeof(DefaultLayout), 0);
		desc->setVertexAttrBuffer(vb, asset::EVAI_ATTR1, asset::EF_R8G8B8_UNORM, sizeof(DefaultLayout), 12);
		desc->setIndexBuffer(ib);

		mb->setIndexType(asset::EIT_16BIT);
		mb->setIndexCount(6);

		vb->drop();
		ib->drop();

		mb->grab();
		return mb;
	}

};


#endif