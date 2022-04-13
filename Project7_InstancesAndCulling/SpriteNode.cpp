#include "SpriteNode.h"
#include "ShooterGame.h"

SpriteNode::SpriteNode(State* state, std::string spriteImage) : Entity(state)
, mSprite(spriteImage)
{
}

/// <summary>
/// Draws the current spriteNode
/// </summary>
void SpriteNode::drawCurrent() const
{
	renderer->World = getTransform();
	renderer->NumFramesDirty++;

	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	UINT matCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));

	auto objectCB = state->GetContext()->game->GetCurrFrameResource()->ObjectCB->Resource();
	auto matCB = state->GetContext()->game->GetCurrFrameResource()->MaterialCB->Resource();


	if (renderer != nullptr)
	{
		state->GetContext()->game->getCmdList()->IASetVertexBuffers(0, 1, &renderer->Geo->VertexBufferView());
		state->GetContext()->game->getCmdList()->IASetIndexBuffer(&renderer->Geo->IndexBufferView());
		state->GetContext()->game->getCmdList()->IASetPrimitiveTopology(renderer->PrimitiveType);

		CD3DX12_GPU_DESCRIPTOR_HANDLE tex(state->GetContext()->game->GetSrvSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
		tex.Offset(renderer->Mat->DiffuseSrvHeapIndex, state->GetContext()->game->mCbvSrvDescriptorSize);

		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + renderer->ObjCBIndex * objCBByteSize;
		D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + renderer->Mat->MatCBIndex * matCBByteSize;

		state->GetContext()->game->getCmdList()->SetGraphicsRootDescriptorTable(0, tex);
		state->GetContext()->game->getCmdList()->SetGraphicsRootConstantBufferView(1, objCBAddress);
		state->GetContext()->game->getCmdList()->SetGraphicsRootConstantBufferView(3, matCBAddress);

		state->GetContext()->game->getCmdList()->DrawIndexedInstanced(renderer->IndexCount, 1, renderer->StartIndexLocation, renderer->BaseVertexLocation, 0);
	}

}

/// <summary>
/// Builds the spriteNode. 
/// </summary>
void SpriteNode::buildCurrent()
{
	auto render = std::make_unique<RenderItem>();
	renderer = render.get();
	renderer->World = getTransform();
	XMStoreFloat4x4(&renderer->TexTransform, XMMatrixScaling(50.0f, 50.0f, 1.0f));
	renderer->ObjCBIndex = (UINT)state->mAllRitems.size();
	renderer->Mat = state->GetContext()->game->getMaterials()["Desert"].get();
	renderer->Geo = state->GetContext()->game->getGeometries()["boxGeo"].get();
	renderer->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	renderer->IndexCount = renderer->Geo->DrawArgs["box"].IndexCount;
	renderer->StartIndexLocation = renderer->Geo->DrawArgs["box"].StartIndexLocation;
	renderer->BaseVertexLocation = renderer->Geo->DrawArgs["box"].BaseVertexLocation;
	//mSpriteNodeRitem = render.get();
	state->GetContext()->game->getRenderItems().push_back(std::move(render));
}
