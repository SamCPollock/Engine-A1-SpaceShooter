#include "World.h"
#include "Player.h"

#include "StateIdentifiers.h"
#include "TitleState.h"
#include "GameState.h"
#include "MenuState.h"
#include "PauseState.h"
//#include "SettingState.h"
#include "StateStack.h"

class StateStack;


class Game : public D3DApp
{
public:
	Game(HINSTANCE hInstance);
	Game(const Game& rhs) = delete;
	Game& operator=(const Game& rhs) = delete;
	~Game();

	virtual bool Initialize()override;

	void ClearFrameResources();
	void BuildFrameResources(int numRenderItems);

	void BuildMaterials();
private:
	virtual void OnResize()override;
	virtual void Update(const GameTimer& gt)override;
	virtual void Draw(const GameTimer& gt)override;


	void registerStates();

	virtual void OnMouseDown(WPARAM btnState, int x, int y)override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y)override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y)override;
	//virtual void OnKeyboardInput(WPARAM btnState)override;

	void UpdateCamera(const GameTimer& gt);
	void AnimateMaterials(const GameTimer& gt);
	void UpdateObjectCBs(const GameTimer& gt);
	void UpdateMaterialCBs(const GameTimer& gt);
	void UpdateMainPassCB(const GameTimer& gt);

	void LoadTextures();
	void BuildRootSignature();
	void BuildDescriptorHeaps();
	void BuildShadersAndInputLayout();
	void BuildShapeGeometry();
	void BuildPSOs();
	//void BuildFrameResources();


	void BuildRenderItems();
	void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems);

	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();

private:

	std::vector<std::unique_ptr<FrameResource>> mFrameResources;
	FrameResource* mCurrFrameResource = nullptr;
	int mCurrFrameResourceIndex = 0;

	UINT mCbvSrvDescriptorSize = 0;

	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;

	ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;

	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries;
	std::unordered_map<std::string, std::unique_ptr<Material>> mMaterials;

	std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;

	std::unordered_map<std::string, ComPtr<ID3DBlob>> mShaders;

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

	ComPtr<ID3D12PipelineState> mOpaquePSO = nullptr;

	//// List of all the render items.
	//std::vector<std::unique_ptr<RenderItem>> mAllRitems;

	// Render items divided by PSO.
	//std::vector<RenderItem*> mOpaqueRitems;

	PassConstants mMainPassCB;


	POINT mLastMousePos;
	Camera mCamera;
	//World mWorld;
	Player mPlayer;

	StateStack	mStateStack;

public:
	ID3D12GraphicsCommandList* getCmdList() { return mCommandList.Get(); }
	//std::vector<std::unique_ptr<RenderItem>>& getRenderItems() { return mAllRitems; }
	std::unordered_map<std::string, std::unique_ptr<Material>>& getMaterials() { return mMaterials; }
	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& getGeometries() { return mGeometries; }
	ID3D12DescriptorHeap* GetSrvSrvDescriptorHeap() { return mSrvDescriptorHeap.Get(); }
	UINT GetCbvSrvDescriptorSize() { return mCbvSrvDescriptorSize; }
	FrameResource* GetCurrFrameResource() { return mCurrFrameResource; }

};
