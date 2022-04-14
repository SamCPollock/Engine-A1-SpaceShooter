///***************************************************************************************
/// TitleState
/// State which shows a simple title and moves to gamestate on button press.
/// Sam Pollock, 2022
///***************************************************************************************



#ifndef BOOK_TITLESTATE_HPP
#define BOOK_TITLESTATE_HPP

#include "State.h"
#include "../Common/d3dApp.h"

class TitleState : public State
{
public:
	TitleState(StateStack* stack, Context* context);
	virtual void draw();
	virtual bool update(const GameTimer& gt);
	virtual bool handleEvent(WPARAM btnState);

private:
	virtual void BuildScene() override;
};

#endif // BOOK_TITLESTATE_HPP
