#pragma once
#include "SceneNode.h"

class Entity :
	public SceneNode
{
public:
	Entity(Game* game);										// Constructor with reference to game. 
	void setVelocity(XMFLOAT3 velocity);					// Sets velocity using a float3 (Vector3 like)
	void setVelocity(float vx, float vy, float vz);			// Sets velocity using 3 floats (seperated) 
	XMFLOAT3 getVelocity() const;							// Return velocity as float3
	
	virtual	void updateCurrent(const GameTimer& gt);		// Updates stuff (Moves according to velocity, renders stuff)

public:
	XMFLOAT3 mVelocity;
};

