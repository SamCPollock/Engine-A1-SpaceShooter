#include "Command.h"


/// <summary>
/// Command constructer, sets up action and category for the commands.
/// Categories default to 'None'.
/// </summary>
Command::Command()
	: action()
	, category(Category::None)
{
}


