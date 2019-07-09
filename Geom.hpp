#ifndef HPP_KOSU_GEOMETRY
#define HPP_KOSU_GEOMETRY
#include <irrlicht.h>
#include <irr/asset/CImageData.h>

namespace kosu{
	using namespace irr;


#include "irr/irrpack.h"
	struct DefaultColorLayout {
		float v[3];
		uint8_t c[3];
		uint8_t padding[1];
	} PACK_STRUCT;
#include "irr/irrunpack.h"

#include "irr/irrpack.h"
	struct DefaultTextureLayout {
		float v[3];
		uint8_t t[2];
		uint8_t padding[2];
	} PACK_STRUCT;
#include "irr/irrunpack.h"

#include "irr/irrpack.h"
	struct DefaultTerrainLayout {
		float v[4];
	} PACK_STRUCT;
#include "irr/irrunpack.h"

	video::IGPUMeshBuffer *quad(video::IVideoDriver *driver,
		core::vector3df_SIMD tL = core::vector3df_SIMD(-0.5, 0.5, 0.0), core::vector3df_SIMD bR = core::vector3df_SIMD(0.5, -0.5, 0.0)
		){
		DefaultTextureLayout vD[4];
		vD[0] = DefaultTextureLayout{ {tL.x, tL.y, tL.z}, {255,0} };
		vD[1] = DefaultTextureLayout{ {bR.x, tL.y, tL.z}, {255,255} };
		vD[2] = DefaultTextureLayout{ {bR.x, bR.y, bR.z}, {0,255} };
		vD[3] = DefaultTextureLayout{ {tL.x, bR.y, bR.z}, {0,0} };

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
		

		desc->setVertexAttrBuffer(vb, asset::EVAI_ATTR0, asset::EF_R32G32B32_SFLOAT, sizeof(DefaultTextureLayout), 0);
		desc->setVertexAttrBuffer(vb, asset::EVAI_ATTR1, asset::EF_R8G8_UNORM, sizeof(DefaultTextureLayout), 12);
		desc->setIndexBuffer(ib);

		mb->setIndexType(asset::EIT_16BIT);
		mb->setIndexCount(6);

		vb->drop();
		ib->drop();

		mb->grab();
		return mb;
	}


	std::pair<video::IGPUMeshBuffer*, video::ITextureBufferObject*> terrain(video::IVideoDriver *driver, asset::ICPUTexture *tex){
		asset::CImageData *imgData = tex->getRanges()[0];
		uint32_t sizePerPixel = imgData->getImageDataSizeInBytes()/imgData->getImageDataSizeInPixels();

		core::vector2di size;
		size.X = imgData->getSize().X;
		size.Y = imgData->getSize().Y;
		

		core::vector<float> heightData;
		
		core::vector<DefaultTerrainLayout> vertexData;
		core::vector<int32_t> indexData;
		for(int i = 0; i < imgData->getImageDataSizeInPixels(); ++i){
			core::vector3d<uint8_t> &vec = reinterpret_cast<core::vector3d<uint8_t>*>(imgData->getData())[i * sizePerPixel];
			heightData.push_back(((vec.X + vec.Y + vec.Z) / 3) / 255.0f);
		}

		video::IGPUBuffer *buf = driver->createDeviceLocalGPUBufferOnDedMem(heightData.size());
		driver->updateBufferRangeViaStagingBuffer(buf, 0, heightData.size(), heightData.data());
		video::ITextureBufferObject *tbo = driver->addTextureBufferObject(buf, video::ITextureBufferObject::ETBOF_R32F);

		//Generate VertexData
		for(int x = 0; x < size.X; ++x){
			for(int y = 0; y < size.Y; ++y){
				vertexData.push_back({(float)x, 0.f, (float)y, (float)(x % 2)});
			}
		}

		//Generate IndexData
		for(int i = 0; i < imgData->getImageDataSizeInPixels(); ++i){
			if(i % (size.X - 1) == 0 || i >= (size.X * size.Y) - (size.Y + 1)) continue;
			indexData.insert(indexData.end(), {i, i + 1, i + 1 + size.X,   
											   i, i + 1 + size.X, i + size.X});
		}

		video::IGPUBuffer* vb = driver->createDeviceLocalGPUBufferOnDedMem(vertexData.size() * sizeof(DefaultTerrainLayout));
		video::IGPUBuffer* ib = driver->createDeviceLocalGPUBufferOnDedMem(indexData.size() * sizeof(int32_t));

		video::IGPUMeshDataFormatDesc* desc = driver->createGPUMeshDataFormatDesc();
		video::IGPUMeshBuffer* mb = new video::IGPUMeshBuffer;
		mb->setMeshDataAndFormat(desc);
		desc->drop();

		driver->updateBufferRangeViaStagingBuffer(vb, 0, vertexData.size() * sizeof(DefaultTerrainLayout), vertexData.data());
		driver->updateBufferRangeViaStagingBuffer(ib, 0, indexData.size() * sizeof(int32_t), indexData.data());


		desc->setVertexAttrBuffer(vb, asset::EVAI_ATTR0, asset::EF_R32G32B32A32_SFLOAT, sizeof(DefaultTerrainLayout), 0);
		desc->setIndexBuffer(ib);

		mb->setIndexType(asset::EIT_32BIT);
		mb->setIndexCount(indexData.size());

		vb->drop();
		ib->drop();

		
		mb->grab();
		tbo->grab();
		return {mb, tbo};

	}

};


#endif