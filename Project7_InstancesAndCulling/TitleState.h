#ifndef BOOK_TITLESTATE_HPP
#define BOOK_TITLESTATE_HPP

#include "State.h"
#include "../Common/d3dApp.h"

class TitleState : public State
{
public:
	TitleState(StateStack* stack, Context* context);
	virtual void		draw();
	virtual bool		update(const GameTimer& gt);
	virtual bool		handleEvent(WPARAM btnState);

private:
	/*SpriteNode* mBackground;
	SpriteNode* mPrompt;*/



	// Inherited via State
	virtual void BuildScene() override;
};

#endif // BOOK_TITLESTATE_HPP
