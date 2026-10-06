#ifndef __RAK_SLEEP_H
#define __RAK_SLEEP_H

#include <thread>
#include <chrono>

inline void RakSleep( unsigned int ms )
{
	std::this_thread::sleep_for( std::chrono::milliseconds( ms ) );
}

#endif
