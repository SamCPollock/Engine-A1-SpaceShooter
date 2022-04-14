#ifndef MAINMENUSTATE_HPP
#define MAINMENUSTATE_HPP

#include "State.h"
#include "../Common/d3dApp.h"

class MainMenuState : public State
{
public:
	MainMenuState(StateStack* stack, Context* context);
	virtual void draw();
	virtual bool update(const GameTimer& gt);
	virtual bool handleEvent(WPARAM btnState);

private:
	virtual void BuildScene() override;
};

#endif 

