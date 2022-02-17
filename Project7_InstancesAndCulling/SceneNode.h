///***************************************************************************************
/// Scene Node,
/// Uses RenderItem from GBC Graphics, 2021. 
/// Used to in scene to set up heirarchy and handle updated, transform, and drawing.
/// Sam Pollock, 2022
///***************************************************************************************


#pragma once
#include "../Common/d3dApp.h"
#include "../Common/MathHelper.h"
#include "../Common/UploadBuffer.h"
#include "../Common/GeometryGenerator.h"
#include "../Common/Camera.h"
#include "FrameResource.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;



///*** FROM YEAR 2 GRAPHICS 
// Lightweight structure stores parameters to draw a shape.  This will
// vary from app-to-app.
struct RenderItem
{
	RenderItem() = default;

	// World matrix of the shape that describes the object's local space
	// relative to the world space, which defines the position, orientation,
	// and scale of the object in the world.
	XMFLOAT4X4 World = MathHelper::Identity4x4();

	XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();

	// Dirty flag indicating the object data has changed and we need to update the constant buffer.
	// Because we have an object cbuffer for each FrameResource, we have to apply the
	// update to each FrameResource.  Thus, when we modify obect data we should set 
	// NumFramesDirty = gNumFrameResources so that each frame resource gets the update.
	int NumFramesDirty = gNumFrameResources;

	// Index into GPU constant buffer corresponding to the ObjectCB for this render item.
	UINT ObjCBIndex = -1;

	Material* Mat = nullptr;
	MeshGeometry* Geo = nullptr;

	// Primitive topology.
	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// DrawIndexedInstanced parameters.
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	int BaseVertexLocation = 0;
};

class Game;

/// <summary>
/// Scenenode 
/// </summary>
class SceneNode
{
public:
	typedef std::unique_ptr<SceneNode> Ptr;


public:
	SceneNode(Game* game);								// Constuctor

	void attachChild(Ptr child);						// Attach another scenenode as child.
	Ptr detachChild(const SceneNode& node);				// Detach child

	void update(const GameTimer& gt);					// Update frames
	void draw() const;									// Draw/display
	void build();						

	XMFLOAT3 getWorldPosition() const;					// return world pos
	void setPosition(float x, float y, float z);		// set pos.
	XMFLOAT3 getWorldRotation() const;					// return world rotation
	void setWorldRotation(float x, float y, float z);	// set world rotation
	XMFLOAT3 getWorldScale() const;						// return scale
	void setScale(float x, float y, float z);			// sets scale

	XMFLOAT4X4	getWorldTransform() const;				// returns world transform matrix
	XMFLOAT4X4	getTransform() const;					// returns transform matrix 

	void move(float x, float y, float z);				// moves sceneNode.
private:
	virtual void updateCurrent(const GameTimer& gt);	// updates scenNode
	void updateChildren(const GameTimer& gt);			// updates all children

	virtual void drawCurrent() const;					// draws sceneNode
	void drawChildren() const;							// draws all children
	virtual void buildCurrent();						// builds sceneNOde
	void buildChildren();								// builds all children

protected:
	Game* game;
	RenderItem* renderer;
private:
	XMFLOAT3				mWorldPosition;
	XMFLOAT3				mWorldRotation;
	XMFLOAT3				mWorldScaling;
	std::vector<Ptr>		mChildren;
	SceneNode* mParent;
};

