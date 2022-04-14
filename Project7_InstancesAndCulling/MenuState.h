#ifndef BOOK_MENUSTATE_HPP
#define BOOK_MENUSTATE_HPP

#include "State.h"
#include "SpriteNode.h"
#include "../Common/d3dApp.h"


class MenuState : public State
{
public:
	MenuState(StateStack* stack, Context* context);

	virtual void			draw();
	virtual bool			update(const GameTimer& gt);
	virtual bool			handleEvent(WPARAM btnState);

	void					updateOptionText();


private:
	enum OptionNames
	{
		Play,
		Setting,
		Exit,
	};


private:

	SpriteNode* mBackground;
	SpriteNode* mMenuPlay;
	SpriteNode* mMenuSetting;
	SpriteNode* mMenuQuit;
	SpriteNode* mMenuSelector;

	std::vector<SpriteNode*>	mOptions;
	std::size_t				mOptionIndex;

	// Inherited via State
	virtual void BuildScene() override;
};

#endif // BOOK_MENUSTATE_HPP
