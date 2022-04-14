#include "Ship.h"
#include "ShooterGame.h"

/// <summary>
/// Constructor, uses switch case to determine what type of ship to create. 
/// </summary>
/// <param name="type"></param>
/// <param name="game"></param>
Ship::Ship(Type type, State* state) : Entity(state)
, mType(type)
{
	switch (type)
	{
	case (Type::Eagle):
		mSprite = "Eagle";
		break;
	case (Type::Raptor):
		mSprite = "Raptor";
		break;
	default:
		mSprite = "Eagle";
		break;
	}
}
/// <summary>
/// GetCategory, returns an int representing the category of the ship (Either PlayerAircraft or EnemyAircraft, to determine response to commands) 
/// </summary>
/// <returns></returns>
unsigned int Ship::getCategory() const
{
	switch (mType)
	{
	case Type::Eagle:
		return Category::PlayerAircraft;

	default:
		return Category::EnemyAircraft;
	}
}
/// <summary>
/// Overrides virtual SceneNode function, draws self.
/// </summary>
void Ship::drawCurrent() const
{
	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	UINT matCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));

	auto objectCB = state->getContext()->game->GetCurrFrameResource()->ObjectCB->Resource();
	auto matCB = state->getContext()->game->GetCurrFrameResource()->MaterialCB->Resource();

	if (renderer != nullptr)
	{
		state->getContext()->game->getCmdList()->IASetVertexBuffers(0, 1, &renderer->Geo->VertexBufferView());
		state->getContext()->game->getCmdList()->IASetIndexBuffer(&renderer->Geo->IndexBufferView());
		state->getContext()->game->getCmdList()->IASetPrimitiveTopology(renderer->PrimitiveType);

		CD3DX12_GPU_DESCRIPTOR_HANDLE tex(state->getContext()->game->GetSrvSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
		tex.Offset(renderer->Mat->DiffuseSrvHeapIndex, state->getContext()->game->GetCbvSrvDescriptorSize());

		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + (UINT64)renderer->ObjCBIndex * objCBByteSize;
		D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + (UINT64)renderer->Mat->MatCBIndex * matCBByteSize;

		state->getContext()->game->getCmdList()->SetGraphicsRootDescriptorTable(0, tex);
		state->getContext()->game->getCmdList()->SetGraphicsRootConstantBufferView(1, objCBAddress);
		state->getContext()->game->getCmdList()->SetGraphicsRootConstantBufferView(3, matCBAddress);

		state->getContext()->game->getCmdList()->DrawIndexedInstanced(renderer->IndexCount, 1, renderer->StartIndexLocation, renderer->BaseVertexLocation, 0);
	}
}
/// <summary>
/// build current, gets the renderer and adds the object and builds it. 
/// </summary>
void Ship::buildCurrent()
{
	auto render = std::make_unique<RenderItem>();
	renderer = render.get();
	renderer->World = getTransform();
	renderer->ObjCBIndex = (UINT)state->mAllRitems.size();
	renderer->Mat = state->getContext()->game->getMaterials()[mSprite].get();
	renderer->Geo = state->getContext()->game->getGeometries()["boxGeo"].get();
	renderer->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	renderer->IndexCount = renderer->Geo->DrawArgs["box"].IndexCount;
	renderer->StartIndexLocation = renderer->Geo->DrawArgs["box"].StartIndexLocation;
	renderer->BaseVertexLocation = renderer->Geo->DrawArgs["box"].BaseVertexLocation;
	//mAircraftRitem = render.get();
	state->mAllRitems.push_back(std::move(render));
}
