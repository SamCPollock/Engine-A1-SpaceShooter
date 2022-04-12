#include "ShooterGame.h"

#include "State.h"

#include "TitleState.h"
#include "MenuState.h"
#include "GameState.h"
#include "PauseState.h"


const int gNumFrameResources = 3;

/// <summary>
/// constructor, sets up the D3DApp instance 
/// Sets the world instance to this game. 
/// </summary>
/// <param name="hInstance"></param>
Game::Game(HINSTANCE hInstance)
	: D3DApp(hInstance)
	, mWorld(this)	// Maybe don't need, doing state stack instead? 
	, mPlayer()
	, mStateStack(State::Context(this, mPlayer))
{
}

/// <summary>
/// Destructor, tidies up. 
/// </summary>
Game::~Game()
{
	if (md3dDevice != nullptr)
		FlushCommandQueue();
}


/// <summary>
/// Initialize the game, 
/// sets the window caption
/// sets the initial camera position
/// Goes through initialization procedure (load textures, build the following: root signature, descriptor heaps 
/// shaders and input layout, shape geometry, materials, render items, frame resources, and PSOs. 
/// </summary>
/// <returns></returns>
bool Game::Initialize()
{
	if (!D3DApp::Initialize())
		return false;

	mMainWndCaption = L"Pollock-Assignment3";

	mCamera.SetPosition(0.0f, 20.0f, -6.0f);
	mCamera.Pitch(7.5f);

	// Reset the command list to prep for initialization commands.
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

	// Get the increment size of a descriptor in this heap type.  This is hardware specific, 
	// so we have to query this information.
	mCbvSrvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	LoadTextures();
	BuildRootSignature();
	BuildDescriptorHeaps();
	BuildShadersAndInputLayout();
	BuildShapeGeometry();
	BuildMaterials();
	BuildRenderItems();
	BuildFrameResources();
	BuildPSOs();

	RegisterStates();
	mStateStack.pushState(States::Game);

	// Execute the initialization commands.
	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until initialization is complete.
	FlushCommandQueue();

	return true;
}

void Game::Update(const GameTimer& gt)
{
	//OnKeyboardInput(gt);

	///**** MIGHT NOT NEED THESE DUE TO mStateStack handling them.
	//ProcessInput();
	//mWorld.update(gt);



	mStateStack.Update(gt);
	mStateStack.handleRealTimeInput();

	if (mStateStack.isEmpty())
	{
		PostQuitMessage(0);
	}

	UpdateCamera(gt);

	// Cycle through the circular frame resource array.
	mCurrFrameResourceIndex = (mCurrFrameResourceIndex + 1) % gNumFrameResources;
	mCurrFrameResource = mFrameResources[mCurrFrameResourceIndex].get();

	// Has the GPU finished processing the commands of the current frame resource?
	// If not, wait until the GPU has completed commands up to this fence point.
	if (mCurrFrameResource->Fence != 0 && mFence->GetCompletedValue() < mCurrFrameResource->Fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(mFence->SetEventOnCompletion(mCurrFrameResource->Fence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	//AnimateMaterials(gt);
	UpdateObjectCBs(gt);
	UpdateMaterialCBs(gt);
	UpdateMainPassCB(gt);
}

/// <summary>
/// Draw function, goes through game, does the standard stuff to draw the world. 
/// </summary>
/// <param name="gt"></param>
void Game::Draw(const GameTimer& gt)
{
	auto cmdListAlloc = mCurrFrameResource->CmdListAlloc;

	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on the GPU.
	ThrowIfFailed(cmdListAlloc->Reset());

	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.
	ThrowIfFailed(mCommandList->Reset(cmdListAlloc.Get(), mOpaquePSO.Get()));

	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	// Indicate a state transition on the resource usage.
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// Clear the back buffer and depth buffer.
	mCommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
	mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Specify the buffers we are going to render to.
	mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

	ID3D12DescriptorHeap* descriptorHeaps[] = { mSrvDescriptorHeap.Get() };
	mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	mCommandList->SetGraphicsRootSignature(mRootSignature.Get());

	auto passCB = mCurrFrameResource->PassCB->Resource();
	mCommandList->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress());

	mStateStack.Draw();
	//mWorld.draw();
	//DrawRenderItems(mCommandList.Get(), mOpaqueRitems);

	// Indicate a state transition on the resource usage.
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	// Done recording commands.
	ThrowIfFailed(mCommandList->Close());

	// Add the command list to the queue for execution.
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Swap the back and front buffers
	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

	// Advance the fence value to mark commands up to this fence point.
	mCurrFrameResource->Fence = ++mCurrentFence;

	// Add an instruction to the command queue to set a new fence point. 
	// Because we are on the GPU timeline, the new fence point won't be 
	// set until the GPU finishes processing all the commands prior to this Signal().
	mCommandQueue->Signal(mFence.Get(), mCurrentFence);
}

void Game::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void Game::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void Game::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));

		mCamera.Pitch(dy);
		mCamera.RotateY(dx);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		// Make each pixel correspond to 0.2 unit in the scene.
		float dx = 0.05f * static_cast<float>(x - mLastMousePos.x);
		float dy = 0.05f * static_cast<float>(y - mLastMousePos.y);

		//To Do
		// Update the camera radius based on input.
		//mRadius += dx - dy;

		// Restrict the radius.
		//mRadius = MathHelper::Clamp(mRadius, 5.0f, 150.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

/// <summary>
/// ProcessInput, handles each event and input from the player's command queue. 
/// </summary>
//void Game::ProcessInput()
//{
//	InputCommandQueue& commands = mWorld.getCommandQueue();
//	mPlayer.handleEvent(commands);
//	mPlayer.handleRealtimeInput(commands);
//}

/// <summary>
/// Takes keyboard input, uses it to move the camera. 
/// </summary>
/// <param name="gt"></param>
//void Game::OnKeyboardInput(const GameTimer& gt)
//{
//	const float dt = gt.DeltaTime();
//
//	mCamera.GetLook();
//	float tmin = 0;
//	float buffer = 0.5;
//	XMFLOAT3  oppositef3(-1, -1, -1);
//	XMVECTOR opposite = XMLoadFloat3(&oppositef3);
//
//	if (GetAsyncKeyState('W') & 0x8000)
//	{
//		bool hit = false;
//
//		if (!hit)
//		{
//			mCamera.Walk(10.0f * dt);
//
//		}
//	}
//
//	if (GetAsyncKeyState('S') & 0x8000)
//	{
//		bool hit = false;
//		if (!hit)
//		{
//			mCamera.Walk(-10.0f * dt);
//		}
//
//	}
//	if (GetAsyncKeyState('A') & 0x8000)
//	{
//		bool hit = false;
//		if (!hit)
//		{
//			mCamera.Strafe(-10.0f * dt);
//		}
//
//
//	}
//	if (GetAsyncKeyState('D') & 0x8000)
//	{
//		bool hit = false;
//		if (!hit)
//		{
//			mCamera.Strafe(10.0f * dt);
//		}
//	}
//
//
//	mCamera.UpdateViewMatrix();
//}

void Game::UpdateCamera(const GameTimer& gt)
{
	// Convert Spherical to Cartesian coordinates.
	//mEyePos.x = mRadius * sinf(mPhi) * cosf(mTheta);
	//mEyePos.z = mRadius * sinf(mPhi) * sinf(mTheta);
	//mEyePos.y = mRadius * cosf(mPhi);

	//// Build the view matrix.
	//XMVECTOR pos = XMVectorSet(mEyePos.x, mEyePos.y, mEyePos.z, 1.0f);
	//XMVECTOR target = XMVectorZero();
	//XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	//XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	//XMStoreFloat4x4(&mView, view);

	mCamera.UpdateViewMatrix(); // Need this to update the camera when ProcessKeyboardInput is not getting called every update.

}
//
//void Game::AnimateMaterials(const GameTimer& gt)
//{
//
//}

/// <summary>
/// Updates the object CBs
/// </summary>
/// <param name="gt"></param>
void Game::UpdateObjectCBs(const GameTimer& gt)
{
	auto currObjectCB = mCurrFrameResource->ObjectCB.get();
	for (auto& e : mAllRitems)
	{
		// Only update the cbuffer data if the constants have changed.  
		// This needs to be tracked per frame resource.
		if (e->NumFramesDirty > 0)
		{
			XMMATRIX world = XMLoadFloat4x4(&e->World);
			XMMATRIX texTransform = XMLoadFloat4x4(&e->TexTransform);

			ObjectConstants objConstants;
			XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));
			XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(texTransform));

			currObjectCB->CopyData(e->ObjCBIndex, objConstants);

			// Next FrameResource need to be updated too.
			e->NumFramesDirty--;
		}
	}
}

/// <summary>
/// Update material C buffers
/// </summary>
/// <param name="gt"></param>
void Game::UpdateMaterialCBs(const GameTimer& gt)
{
	auto currMaterialCB = mCurrFrameResource->MaterialCB.get();
	for (auto& e : mMaterials)
	{
		// Only update the cbuffer data if the constants have changed.  If the cbuffer
		// data changes, it needs to be updated for each FrameResource.
		Material* mat = e.second.get();
		if (mat->NumFramesDirty > 0)
		{
			XMMATRIX matTransform = XMLoadFloat4x4(&mat->MatTransform);

			MaterialConstants matConstants;
			matConstants.DiffuseAlbedo = mat->DiffuseAlbedo;
			matConstants.FresnelR0 = mat->FresnelR0;
			matConstants.Roughness = mat->Roughness;
			XMStoreFloat4x4(&matConstants.MatTransform, XMMatrixTranspose(matTransform));

			currMaterialCB->CopyData(mat->MatCBIndex, matConstants);

			// Next FrameResource need to be updated too.
			mat->NumFramesDirty--;
		}
	}
}

/// <summary>
/// updates main pass buffers 
/// </summary>
/// <param name="gt"></param>
void Game::UpdateMainPassCB(const GameTimer& gt)
{
	XMMATRIX view = mCamera.GetView();
	XMMATRIX proj = mCamera.GetProj();

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
	XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

	XMStoreFloat4x4(&mMainPassCB.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&mMainPassCB.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&mMainPassCB.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&mMainPassCB.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&mMainPassCB.ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&mMainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
	mMainPassCB.EyePosW = mCamera.GetPosition3f();
	mMainPassCB.RenderTargetSize = XMFLOAT2((float)mClientWidth, (float)mClientHeight);
	mMainPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / mClientWidth, 1.0f / mClientHeight);
	mMainPassCB.NearZ = 1.0f;
	mMainPassCB.FarZ = 1000.0f;
	mMainPassCB.TotalTime = gt.TotalTime();
	mMainPassCB.DeltaTime = gt.DeltaTime();
	mMainPassCB.AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };
	mMainPassCB.Lights[0].Direction = { 0.57735f, -0.57735f, 0.57735f };
	mMainPassCB.Lights[0].Strength = { 0.6f, 0.6f, 0.6f };
	mMainPassCB.Lights[1].Direction = { -0.57735f, -0.57735f, 0.57735f };
	mMainPassCB.Lights[1].Strength = { 0.3f, 0.3f, 0.3f };
	mMainPassCB.Lights[2].Direction = { 0.0f, -0.707f, -0.707f };
	mMainPassCB.Lights[2].Strength = { 0.15f, 0.15f, 0.15f };

	auto currPassCB = mCurrFrameResource->PassCB.get();
	currPassCB->CopyData(0, mMainPassCB);
}


/// <summary>
/// loads the textures from the textures folder. 
/// </summary>
void Game::LoadTextures()
{
	////Eagle
	//auto EagleTex = std::make_unique<Texture>();
	//EagleTex->Name = "EagleTex";
	//EagleTex->Filename = L"../Textures/checkboard.dds";
	//ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
	//	mCommandList.Get(), EagleTex->Filename.c_str(),
	//	EagleTex->Resource, EagleTex->UploadHeap));

	//mTextures[EagleTex->Name] = std::move(EagleTex);

	////Raptor
	//auto RaptorTex = std::make_unique<Texture>();
	//RaptorTex->Name = "RaptorTex";
	//RaptorTex->Filename = L"../Textures/bricks2.dds";
	//ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
	//	mCommandList.Get(), RaptorTex->Filename.c_str(),
	//	RaptorTex->Resource, RaptorTex->UploadHeap));

	//mTextures[RaptorTex->Name] = std::move(RaptorTex);

	////Desert
	//auto DesertTex = std::make_unique<Texture>();
	//DesertTex->Name = "DesertTex";
	//DesertTex->Filename = L"../Textures/bricks3.dds";
	//ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
	//	mCommandList.Get(), DesertTex->Filename.c_str(),
	//	DesertTex->Resource, DesertTex->UploadHeap));

	//mTextures[DesertTex->Name] = std::move(DesertTex);

	//// TitleBackground
	//auto TitleBackgroundTex = std::make_unique<Texture>();
	//TitleBackgroundTex->Name = "TitleBackgroundTex";
	//TitleBackgroundTex->Filename = L"../Textures/bricks3.dds";
	//ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
	//	mCommandList.Get(), TitleBackgroundTex->Filename.c_str(),
	//	TitleBackgroundTex->Resource, TitleBackgroundTex->UploadHeap));

	//mTextures[TitleBackgroundTex->Name] = std::move(TitleBackgroundTex);

	//// TitleText
	//auto TitleBackgroundTex = std::make_unique<Texture>();
	//TitleBackgroundTex->Name = "TitleBackgroundTex";
	//TitleBackgroundTex->Filename = L"../Textures/bricks2.dds";
	//ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
	//	mCommandList.Get(), TitleBackgroundTex->Filename.c_str(),
	//	TitleBackgroundTex->Resource, TitleBackgroundTex->UploadHeap));

	//mTextures[TitleBackgroundTex->Name] = std::move(TitleBackgroundTex);


	CreateTextures("EagleTex", L"../Textures/checkboard.dds");
	CreateTextures("RaptorTex", L"../Textures/bricks2.dds");
	CreateTextures("DesertTex", L"../Textures/bricks3.dds");
	CreateTextures("TitleBackgroundTex", L"../Textures/bricks3.dds");
	CreateTextures("TitleTextTex", L"../Textures/bricks3.dds");
	CreateTextures("MenuBackgroundTex", L"../Textures/bricks3.dds");
	CreateTextures("MenuInstructionsTex", L"../Textures/bricks3.dds");
	CreateTextures("PauseOverlayTex", L"../Textures/bricks3.dds");


}



void Game::CreateTextures(std::string name, std::wstring path)
{
	auto texture = std::make_unique<Texture>();
	texture->Name = name;
	texture->Filename = path;
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), texture->Filename.c_str(),
		texture->Resource, texture->UploadHeap));

	//Save textures name for srvDescriptorHeap
	mTexturesName.push_back(name);

	//Store texture in map
	mTextures[texture->Name] = std::move(texture);

}

/// <summary>
/// Builds the root signature. 
/// </summary>
void Game::BuildRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE texTable;
	texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	// Root parameter can be a table, root descriptor or root constants.
	CD3DX12_ROOT_PARAMETER slotRootParameter[4];

	// Perfomance TIP: Order from most frequent to least frequent.
	slotRootParameter[0].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);
	slotRootParameter[1].InitAsConstantBufferView(0);
	slotRootParameter[2].InitAsConstantBufferView(1);
	slotRootParameter[3].InitAsConstantBufferView(2);

	auto staticSamplers = GetStaticSamplers();

	// A root signature is an array of root parameters.
	//The Init function of the CD3DX12_ROOT_SIGNATURE_DESC class has two parameters that allow you to
		//define an array of so - called static samplers your application can use.
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(4, slotRootParameter,
		(UINT)staticSamplers.size(), staticSamplers.data(),  //6 samplers!
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	ThrowIfFailed(md3dDevice->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(mRootSignature.GetAddressOf())));
}




/// <summary>
/// Builds the descriptor heap
/// Once a texture resource is created, we need to create an SRV descriptor to it which we
/// can set to a root signature parameter slot for use by the shader programs.
/// </summary>
void Game::BuildDescriptorHeaps()
{
	//
	// Create the SRV heap.
	//
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 3;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&mSrvDescriptorHeap)));

	//
	// Fill out the heap with actual descriptors.
	//
	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(mSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	auto EagleTex = mTextures["EagleTex"]->Resource;
	auto RaptorTex = mTextures["RaptorTex"]->Resource;
	auto DesertTex = mTextures["DesertTex"]->Resource;
	
	auto TitleBackgroundTex = mTextures["TitleBackgroundTex"]->Resource;
	auto TitleTextTex = mTextures["TitleTextTex"]->Resource;
	auto MenuBackgroundTex = mTextures["MenuBackgroundTex"]->Resource;
	auto MenuInstructionsTex = mTextures["MenuInstructionsTex"]->Resource;
	auto PauseOverlayTex = mTextures["PauseOverlayTex"]->Resource;


	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

	//This mapping enables the shader resource view (SRV) to choose how memory gets routed to the 4 return components in a shader after a memory fetch.
	//When a texture is sampled in a shader, it will return a vector of the texture data at the specified texture coordinates.
	//This field provides a way to reorder the vector components returned when sampling the texture.
	//D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING  will not reorder the components and just return the data in the order it is stored in the texture resource.
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	srvDesc.Format = EagleTex->GetDesc().Format;

	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	//The number of mipmap levels to view, starting at MostDetailedMip.This field, along with MostDetailedMip allows us to
	//specify a subrange of mipmap levels to view.You can specify - 1 to indicate to view
	//all mipmap levels from MostDetailedMip down to the last mipmap level.

	srvDesc.Texture2D.MipLevels = EagleTex->GetDesc().MipLevels;

	//Specifies the minimum mipmap level that can be accessed. 0.0 means all the mipmap levels can be accessed.
	//Specifying 3.0 means mipmap levels 3.0 to MipCount - 1 can be accessed.
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	md3dDevice->CreateShaderResourceView(EagleTex.Get(), &srvDesc, hDescriptor);

	//Raptor Descriptor
	hDescriptor.Offset(1, mCbvSrvDescriptorSize);
	srvDesc.Format = RaptorTex->GetDesc().Format;
	md3dDevice->CreateShaderResourceView(RaptorTex.Get(), &srvDesc, hDescriptor);

	//Desert Descriptor
	hDescriptor.Offset(1, mCbvSrvDescriptorSize);
	srvDesc.Format = DesertTex->GetDesc().Format;
	md3dDevice->CreateShaderResourceView(DesertTex.Get(), &srvDesc, hDescriptor);



	////Title Background Descriptor
	//hDescriptor.Offset(1, mCbvSrvDescriptorSize);
	//srvDesc.Format = DesertTex->GetDesc().Format;
	//md3dDevice->CreateShaderResourceView(TitleBackgroundTex.Get(), &srvDesc, hDescriptor);

	////TitleText Descriptor
	//hDescriptor.Offset(1, mCbvSrvDescriptorSize);
	//srvDesc.Format = DesertTex->GetDesc().Format;
	//md3dDevice->CreateShaderResourceView(TitleTextTex.Get(), &srvDesc, hDescriptor);

	////Menu Background Descriptor
	//hDescriptor.Offset(1, mCbvSrvDescriptorSize);
	//srvDesc.Format = DesertTex->GetDesc().Format;
	//md3dDevice->CreateShaderResourceView(MenuBackgroundTex.Get(), &srvDesc, hDescriptor);

	////Menu Instructions Descriptor
	//hDescriptor.Offset(1, mCbvSrvDescriptorSize);
	//srvDesc.Format = DesertTex->GetDesc().Format;
	//md3dDevice->CreateShaderResourceView(MenuInstructionsTex.Get(), &srvDesc, hDescriptor);

	////Pause Overlay Descriptor
	//hDescriptor.Offset(1, mCbvSrvDescriptorSize);
	//srvDesc.Format = DesertTex->GetDesc().Format;
	//md3dDevice->CreateShaderResourceView(PauseOverlayTex.Get(), &srvDesc, hDescriptor);

}

void Game::BuildShadersAndInputLayout()
{
	mShaders["standardVS"] = d3dUtil::CompileShader(L"Shaders\\Default.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["opaquePS"] = d3dUtil::CompileShader(L"Shaders\\Default.hlsl", nullptr, "PS", "ps_5_1");

	mInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		//The texture coordinates determine what part of the texture gets mapped on the triangles.
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};
}


/// <summary>
/// Builds the shape geometry boxes. 
/// </summary>
void Game::BuildShapeGeometry()
{
	GeometryGenerator geoGen;
	//GeometryGenerator::MeshData box = geoGen.CreateQuad(0.0f, 0.0f, 20.0f, 10.0f, 10.0f);
	GeometryGenerator::MeshData box = geoGen.CreateBox(2, -1, -1, 1);
	SubmeshGeometry boxSubmesh;
	boxSubmesh.IndexCount = (UINT)box.Indices32.size();
	boxSubmesh.StartIndexLocation = 0;
	boxSubmesh.BaseVertexLocation = 0;


	std::vector<Vertex> vertices(box.Vertices.size());

	for (size_t i = 0; i < box.Vertices.size(); ++i)
	{
		vertices[i].Pos = box.Vertices[i].Position;
		vertices[i].Normal = box.Vertices[i].Normal;
		vertices[i].TexC = box.Vertices[i].TexC;
	}

	std::vector<std::uint16_t> indices = box.GetIndices16();

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = "boxGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
		mCommandList.Get(), vertices.data(), vbByteSize, geo->VertexBufferUploader);

	geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
		mCommandList.Get(), indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Vertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	geo->DrawArgs["box"] = boxSubmesh;

	mGeometries[geo->Name] = std::move(geo);
}

/// <summary>
///  Builds pipeline state objects
/// </summary>
void Game::BuildPSOs()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePsoDesc;

	//
	// PSO for opaque objects.
	//
	ZeroMemory(&opaquePsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	opaquePsoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
	opaquePsoDesc.pRootSignature = mRootSignature.Get();
	opaquePsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaders["standardVS"]->GetBufferPointer()),
		mShaders["standardVS"]->GetBufferSize()
	};
	opaquePsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaders["opaquePS"]->GetBufferPointer()),
		mShaders["opaquePS"]->GetBufferSize()
	};
	opaquePsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	opaquePsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	opaquePsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	opaquePsoDesc.SampleMask = UINT_MAX;
	opaquePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	opaquePsoDesc.NumRenderTargets = 1;
	opaquePsoDesc.RTVFormats[0] = mBackBufferFormat;
	opaquePsoDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	opaquePsoDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	opaquePsoDesc.DSVFormat = mDepthStencilFormat;
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&opaquePsoDesc, IID_PPV_ARGS(&mOpaquePSO)));
}

/// <summary>
/// Builds frame resources. 
/// </summary>
void Game::BuildFrameResources()
{
	for (int i = 0; i < gNumFrameResources; ++i)
	{
		mFrameResources.push_back(std::make_unique<FrameResource>(md3dDevice.Get(),
			1, (UINT)mAllRitems.size(), (UINT)mMaterials.size()));
	}
}



void Game::BuildFrameResources(int renderItemCount)
{
	for (int i = 0; i < gNumFrameResources; i++)
	{
		mFrameResources.push_back(std::make_unique<FrameResource>(md3dDevice.Get(),
			1, (UINT)renderItemCount, (UINT)mMaterials.size()));
	}
}

void Game::ResetFrameResources()
{
	mFrameResources.clear();
}

/// <summary>
/// Builds materials using the textures. 
/// </summary>
void Game::BuildMaterials()
{
	auto Eagle = std::make_unique<Material>();
	Eagle->Name = "Eagle";
	Eagle->MatCBIndex = 0;
	Eagle->DiffuseSrvHeapIndex = 0;
	Eagle->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Eagle->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	Eagle->Roughness = 0.2f;

	mMaterials["Eagle"] = std::move(Eagle);

	auto Raptor = std::make_unique<Material>();
	Raptor->Name = "Raptor";
	Raptor->MatCBIndex = 1;
	Raptor->DiffuseSrvHeapIndex = 1;
	Raptor->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Raptor->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	Raptor->Roughness = 0.2f;

	mMaterials["Raptor"] = std::move(Raptor);

	auto Desert = std::make_unique<Material>();
	Desert->Name = "Desert";
	Desert->MatCBIndex = 2;
	Desert->DiffuseSrvHeapIndex = 2;
	Desert->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Desert->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	Desert->Roughness = 0.2f;

	mMaterials["Desert"] = std::move(Desert);

	//CreateMaterials("Desert", XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.05f, 0.05f, 0.05f), 0.2f);


	auto TitleBackgroundMat = std::make_unique<Material>();
	TitleBackgroundMat->Name = "TitleBackgroundMat";
	TitleBackgroundMat->MatCBIndex = 2;
	TitleBackgroundMat->DiffuseSrvHeapIndex = 2;
	TitleBackgroundMat->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	TitleBackgroundMat->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	TitleBackgroundMat->Roughness = 0.2f;
	mMaterials["TitleBackgroundMat"] = std::move(TitleBackgroundMat);

	auto TitleTextMat = std::make_unique<Material>();
	TitleTextMat->Name = "TitleTextMat";
	TitleTextMat->MatCBIndex = 2;
	TitleTextMat->DiffuseSrvHeapIndex = 2;
	TitleTextMat->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	TitleTextMat->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	TitleTextMat->Roughness = 0.2f;
	mMaterials["TitleTextMat"] = std::move(TitleTextMat);


	/*CreateMaterials("TitleBackgroundMat", XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.05f, 0.05f, 0.05f), 0.2f);
	CreateMaterials("TitleTextMat", XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.05f, 0.05f, 0.05f), 0.2f);
	CreateMaterials("MenuBackgroundMat", XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.05f, 0.05f, 0.05f), 0.2f);
	CreateMaterials("MenuInstructionsMat", XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.05f, 0.05f, 0.05f), 0.2f);
	CreateMaterials("PauseOverlayMat", XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.05f, 0.05f, 0.05f), 0.2f);*/
}

void Game::CreateMaterials(std::string name, XMFLOAT4 diffuseAlbedo, XMFLOAT3 fresnelR0, float roughness)
{
	auto material = std::make_unique<Material>();
	material->Name = name;
	material->MatCBIndex = mMatCBIdx++;
	material->DiffuseSrvHeapIndex = mDiffuseSrvHeapIdx++;
	material->DiffuseAlbedo = diffuseAlbedo;
	material->FresnelR0 = fresnelR0;
	material->Roughness = roughness;

	mMaterials[name] = std::move(material);
}

void Game::RegisterStates()
{
	mStateStack.registerState<TitleState>(States::Title);
	mStateStack.registerState<MenuState>(States::Menu);
	mStateStack.registerState<GameState>(States::Game);
	mStateStack.registerState<PauseState>(States::Pause);
}

void Game::OnKeyDown(WPARAM btnState)
{
	mStateStack.HandleEvent(btnState);
}


/// <summary>
/// Builds render items (calls the buildScene function from the world)
/// </summary>
void Game::BuildRenderItems()
{
	mWorld.buildScene();

	// All the render items are opaque.
	for (auto& e : mAllRitems)
		mOpaqueRitems.push_back(e.get());
}

/// <summary>
///  Draws each render item.
/// </summary>
/// <param name="cmdList"></param>
/// <param name="ritems"></param>
void Game::DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems)
{
	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	UINT matCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));

	auto objectCB = mCurrFrameResource->ObjectCB->Resource();
	auto matCB = mCurrFrameResource->MaterialCB->Resource();

	// For each render item...
	for (size_t i = 0; i < ritems.size(); ++i)
	{
		auto ri = ritems[i];

		cmdList->IASetVertexBuffers(0, 1, &ri->Geo->VertexBufferView());
		cmdList->IASetIndexBuffer(&ri->Geo->IndexBufferView());
		cmdList->IASetPrimitiveTopology(ri->PrimitiveType);

		CD3DX12_GPU_DESCRIPTOR_HANDLE tex(mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		tex.Offset(ri->Mat->DiffuseSrvHeapIndex, mCbvSrvDescriptorSize);

		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + ri->ObjCBIndex * objCBByteSize;
		D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + ri->Mat->MatCBIndex * matCBByteSize;

		cmdList->SetGraphicsRootDescriptorTable(0, tex);
		cmdList->SetGraphicsRootConstantBufferView(1, objCBAddress);
		cmdList->SetGraphicsRootConstantBufferView(3, matCBAddress);

		cmdList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
	}
}


std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> Game::GetStaticSamplers()
{
	// Applications usually only need a handful of samplers.  So just define them all up front
	// and keep them available as part of the root signature.  

	const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
		0, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
		1, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
		2, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
		3, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
		4, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
		0.0f,                             // mipLODBias
		8);                               // maxAnisotropy

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
		5, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
		0.0f,                              // mipLODBias
		8);                                // maxAnisotropy

	return {
		pointWrap, pointClamp,
		linearWrap, linearClamp,
		anisotropicWrap, anisotropicClamp };
}
