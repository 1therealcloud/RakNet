#if defined(_WIN32)
#include <windows.h> // Sleep
#else
#include <unistd.h> // usleep
#endif

void RakSleep(unsigned int ms)
{
#ifdef _WIN32
	Sleep(ms);
#else
	usleep(ms * 1000);
#endif
}
