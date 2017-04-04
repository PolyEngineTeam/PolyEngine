#define CATCH_CONFIG_RUNNER
#include <catch.hpp>

int main(int argc, char* const argv[])
{
	int result = Catch::Session().run(argc, argv);

// Wait for key press in visual studio
#if defined(_WIN32)
	system("pause");
#endif

	return result;
}