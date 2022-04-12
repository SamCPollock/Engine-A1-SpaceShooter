#include "Ship.h"
#include "ShooterGame.h"

/// <summary>
/// Constructor, uses switch case to determine what type of ship to create. 
/// </summary>
/// <param name="type"></param>
/// <param name="game"></param>
Ship::Ship(Type type, Game* game) : Entity(game)
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


	auto objectCB = game->mCurrFrameResource->ObjectCB->Resource();
	auto matCB = game->mCurrFrameResource->MaterialCB->Resource();

	// Render the ship. 
	if (mShipRitem != nullptr)
	{
		game->getCmdList()->IASetVertexBuffers(0, 1, &mShipRitem->Geo->VertexBufferView());
		game->getCmdList()->IASetIndexBuffer(&mShipRitem->Geo->IndexBufferView());
		game->getCmdList()->IASetPrimitiveTopology(mShipRitem->PrimitiveType);

		CD3DX12_GPU_DESCRIPTOR_HANDLE tex(game->mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		tex.Offset(mShipRitem->Mat->DiffuseSrvHeapIndex, game->mCbvSrvDescriptorSize);

		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + (UINT64)mShipRitem->ObjCBIndex * objCBByteSize;
		D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + (UINT64)mShipRitem->Mat->MatCBIndex * matCBByteSize;

		game->getCmdList()->SetGraphicsRootDescriptorTable(0, tex);
		game->getCmdList()->SetGraphicsRootConstantBufferView(1, objCBAddress);
		game->getCmdList()->SetGraphicsRootConstantBufferView(3, matCBAddress);

		game->getCmdList()->DrawIndexedInstanced(mShipRitem->IndexCount, 1, mShipRitem->StartIndexLocation, mShipRitem->BaseVertexLocation, 0);
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
	renderer->ObjCBIndex = (UINT)game->getRenderItems().size();
	renderer->Mat = game->getMaterials()[mSprite].get();
	renderer->Geo = game->getGeometries()["boxGeo"].get();
	renderer->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	renderer->IndexCount = renderer->Geo->DrawArgs["box"].IndexCount;
	renderer->StartIndexLocation = renderer->Geo->DrawArgs["box"].StartIndexLocation;
	renderer->BaseVertexLocation = renderer->Geo->DrawArgs["box"].BaseVertexLocation;
	mShipRitem = render.get();
	game->getRenderItems().push_back(std::move(render));
}
