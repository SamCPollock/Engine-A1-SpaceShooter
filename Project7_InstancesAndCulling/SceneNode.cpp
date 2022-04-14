#include "SceneNode.h"
#include "ShooterGame.h"
#include "Command.h"
/// <summary>
/// Constructor. 
/// Takes the game referencen
/// sets the world poistion, scaling, and rotation to defaults. 
/// </summary>
/// <param name="game"></param>
SceneNode::SceneNode(State* state)
	: mChildren()
	, mParent(nullptr)
	, state(state)
{
	mWorldPosition = XMFLOAT3(0, 0, 0);
	mWorldScaling = XMFLOAT3(1, 1, 1);
	mWorldRotation = XMFLOAT3(0, 0, 0);
}
/// <summary>
/// Attach a scenenode to this scenenode as a child. 
/// </summary>
/// <param name="child"></param>
void SceneNode::attachChild(Ptr child)
{
	child->mParent = this;
	mChildren.push_back(std::move(child));
}
/// <summary>
/// Detach child node. 
/// </summary>
/// <param name="node"></param>
/// <returns></returns>
SceneNode::Ptr SceneNode::detachChild(const SceneNode& node)
{
	auto found = std::find_if(mChildren.begin(), mChildren.end(), [&](Ptr& p) { return p.get() == &node; });
	assert(found != mChildren.end());

	Ptr result = std::move(*found);
	result->mParent = nullptr;
	mChildren.erase(found);
	return result;
}
/// <summary>
/// Update according to gametime. 
/// Does so by calling updateCurrent and updateChildren. 
/// </summary>
/// <param name="gt"></param>
void SceneNode::update(const GameTimer& gt)
{
	updateCurrent(gt);
	updateChildren(gt);
}

/// @note the "current" functions are virtual and empty in the base class, but are overwritten by inherited classes. 

/// <summary>
/// Update Current, empty. updateChildren used instead. 
/// </summary>
/// <param name="gt"></param>
void SceneNode::updateCurrent(const GameTimer& gt)
{
	// Do nothing by default
}
/// <summary>
/// Loops through children of sceneNode and calls the update function in each. 
/// </summary>
/// <param name="gt"></param>
void SceneNode::updateChildren(const GameTimer& gt)
{
	for (Ptr& child : mChildren)
	{
		child->update(gt);
	}
}
/// <summary>
/// Calls drawcurrent (does nothing) and drawChildren. 
/// </summary>
void SceneNode::draw() const
{
	drawCurrent();
	drawChildren();
}
/// <summary>
/// Empty by default, drawing is done in drawChildren.
/// </summary>
void SceneNode::drawCurrent() const
{
	//Empty for now
}
/// <summary>
/// Loops through children to call the draw function on each. 
/// </summary>
void SceneNode::drawChildren() const
{
	for (const Ptr& child : mChildren)
	{
		child->draw();
	}
}

/// <summary>
/// Calls buildCurrent and buildChildren. 
/// </summary>
void SceneNode::build()
{
	buildCurrent();
	buildChildren();
}

/// <summary>
/// Does nothing, building is done in buildChildren. 
/// </summary>
void SceneNode::buildCurrent()
{
	//Empty for now
}

/// <summary>
/// Loops through each child, calling the build function for each. 
/// </summary>
void SceneNode::buildChildren()
{
	for (const Ptr& child : mChildren)
	{
		child->build();
	}
}
/// <summary>
/// Returns world position of sceneNOde as an XMFLOAT3 
/// </summary>
/// <returns></returns>
XMFLOAT3 SceneNode::getWorldPosition() const
{
	return mWorldPosition;
}
/// <summary>
/// Takes 3 floats to set the worldPostiion of scenNode. 
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="z"></param>
void SceneNode::setPosition(float x, float y, float z)
{
	mWorldPosition = XMFLOAT3(x, y, z);
}
/// <summary>
/// Returns rotaion as an XMFLOAT3 
/// </summary>
/// <returns></returns>
XMFLOAT3 SceneNode::getWorldRotation() const
{
	return mWorldRotation;
}
/// <summary>
/// Sets rotation using 3 floats. 
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="z"></param>
void SceneNode::setWorldRotation(float x, float y, float z)
{
	mWorldRotation = XMFLOAT3(x, y, z);
}

/// <summary>
/// returns scale as an XMFLOAT3 
/// </summary>
/// <returns></returns>
XMFLOAT3 SceneNode::getWorldScale() const
{
	return mWorldScaling;
}
/// <summary>
/// Sets scales using 3 floats. 
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="z"></param>
void SceneNode::setScale(float x, float y, float z)
{
	mWorldScaling = XMFLOAT3(x, y, z);
}
/// <summary>
/// Returns transform of scnenode. 
/// Returned as a 4x4 matrix, 
/// </summary>
/// <returns></returns>
XMFLOAT4X4 SceneNode::getWorldTransform() const
{
	XMFLOAT4X4 transform = MathHelper::Identity4x4();
	XMMATRIX T = XMLoadFloat4x4(&transform);

	for (const SceneNode* node = this; node != nullptr; node = node->mParent)
	{
		XMMATRIX Tp = XMLoadFloat4x4(&node->getTransform());
		T = Tp * T;
	}
	XMStoreFloat4x4(&transform, T);

	return transform;
}
/// <summary>
/// Returns transform as an XMFLOAT4x4. 
/// Gets the scaling, rotation, and position and packages them into a 4x4. 
/// </summary>
/// <returns></returns>
XMFLOAT4X4 SceneNode::getTransform() const
{
	XMFLOAT4X4 transform;
	XMStoreFloat4x4(&transform, XMMatrixScaling(mWorldScaling.x, mWorldScaling.y, mWorldScaling.z) *
		XMMatrixRotationX(mWorldRotation.x) *
		XMMatrixRotationY(mWorldRotation.y) *
		XMMatrixRotationZ(mWorldRotation.z) *
		XMMatrixTranslation(mWorldPosition.x, mWorldPosition.y, mWorldPosition.z));
	return transform;
}

/// <summary>
/// OnCommand, goes through self and children to perform command actions. 
/// </summary>
/// <param name="command"></param>
/// <param name="gt"></param>
void SceneNode::onCommand(const Command& command, const GameTimer& gt)
{
	// Command current node, if category matches
	if (command.category & getCategory())
		command.action(*this, gt);

	// Command children
	for (Ptr& child : mChildren)
		child->onCommand(command, gt);
}
/// <summary>
/// Returns the category of the game object (Returns 'Scene' by default
/// </summary>
/// <returns></returns>
unsigned int SceneNode::getCategory() const
{
	return Category::Scene;
}

/// <summary>
/// Takes 3 floats to move the sceneNode's worldPostiion.
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="z"></param>
void SceneNode::move(float x, float y, float z)
{
	mWorldPosition.x += x;
	mWorldPosition.y += y;
	mWorldPosition.z += z;
}
/// <summary>
/// Clears all children
/// </summary>
void SceneNode::ClearChildren()
{
	mChildren.clear();
}
