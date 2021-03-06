#include "FrameResource.h"

FrameResource::FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount)
{
	ThrowIfFailed(device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(CmdListAlloc.GetAddressOf())));

	//  FrameCB = std::make_unique<UploadBuffer<FrameConstants>>(device, 1, true);
	PassCB = std::make_unique<UploadBuffer<PassConstants>>(device, passCount, true);
	MaterialCB = std::make_unique<UploadBuffer<MaterialConstants>>(device, materialCount, true);
	ObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(device, objectCount, true);
}

FrameResource::~FrameResource()
{

}

//
//// FRAME RESOURCE 2
//#include "FrameResource.h"
//
//FrameResource::FrameResource(ID3D12Device* device, UINT passCount, UINT maxInstanceCount, UINT materialCount)
//{
//	ThrowIfFailed(device->CreateCommandAllocator(
//		D3D12_COMMAND_LIST_TYPE_DIRECT,
//		IID_PPV_ARGS(CmdListAlloc.GetAddressOf())));
//
//	PassCB = std::make_unique<UploadBuffer<PassConstants>>(device, passCount, true);
//	MaterialBuffer = std::make_unique<UploadBuffer<MaterialData>>(device, materialCount, false);
//	InstanceBuffer = std::make_unique<UploadBuffer<InstanceData>>(device, maxInstanceCount, false);
//}
//
//FrameResource::~FrameResource()
//{
//
//}
//
//
//
