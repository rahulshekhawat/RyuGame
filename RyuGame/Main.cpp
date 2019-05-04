
#include <iostream>
#include "RyuGame.h"

int main()
{
	FRyuGame Game;

	try
	{
		Game.Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
