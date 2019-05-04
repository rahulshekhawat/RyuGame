
#include <iostream>
#include "ApplicationBase.h"

int main()
{
	UApplicationBase App;

	try
	{
		App.Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
