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
	// Set buffer sizes according to object and material constants. 
	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	UINT matCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));


	auto objectCB = state->GetContext()->game->mCurrFrameResource->ObjectCB->Resource();
	auto matCB = state->GetContext()->game->mCurrFrameResource->MaterialCB->Resource();

	// Render the ship. 
	if (renderer != nullptr)
	{
		state->GetContext()->game->getCmdList()->IASetVertexBuffers(0, 1, &renderer->Geo->VertexBufferView());
		state->GetContext()->game->getCmdList()->IASetIndexBuffer(&renderer->Geo->IndexBufferView());
		state->GetContext()->game->getCmdList()->IASetPrimitiveTopology(renderer->PrimitiveType);

		CD3DX12_GPU_DESCRIPTOR_HANDLE tex(state->GetContext()->game->mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		tex.Offset(renderer->Mat->DiffuseSrvHeapIndex, state->GetContext()->game->mCbvSrvDescriptorSize);

		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + (UINT64)renderer->ObjCBIndex * objCBByteSize;
		D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + (UINT64)renderer->Mat->MatCBIndex * matCBByteSize;

		state->GetContext()->game->getCmdList()->SetGraphicsRootDescriptorTable(0, tex);
		state->GetContext()->game->getCmdList()->SetGraphicsRootConstantBufferView(1, objCBAddress);
		state->GetContext()->game->getCmdList()->SetGraphicsRootConstantBufferView(3, matCBAddress);

		state->GetContext()->game->getCmdList()->DrawIndexedInstanced(renderer->IndexCount, 1, renderer->StartIndexLocation, renderer->BaseVertexLocation, 0);
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
	renderer->Mat = state->GetContext()->game->getMaterials()[mSprite].get();
	renderer->Geo = state->GetContext()->game->getGeometries()["boxGeo"].get();
	renderer->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	renderer->IndexCount = renderer->Geo->DrawArgs["box"].IndexCount;
	renderer->StartIndexLocation = renderer->Geo->DrawArgs["box"].StartIndexLocation;
	renderer->BaseVertexLocation = renderer->Geo->DrawArgs["box"].BaseVertexLocation;
	//mShipRitem = render.get();
	state->mAllRitems.push_back(std::move(render));
}
