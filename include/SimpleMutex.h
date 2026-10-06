/// \file
/// \brief \b [Internal] Encapsulates a mutex
///
/// This file is part of RakNet Copyright 2003 Kevin Jenkins.
///
/// Usage of RakNet is subject to the appropriate license agreement.
/// Creative Commons Licensees are subject to the
/// license found at
/// http://creativecommons.org/licenses/by-nc/2.5/
/// Single application licensees are subject to the license found at
/// http://www.rakkarsoft.com/SingleApplicationLicense.html
/// Custom license users are subject to the terms therein.
/// GPL license users are subject to the GNU General Public
/// License as published by the Free
/// Software Foundation; either version 2 of the License, or (at your
/// option) any later version.

#ifndef __SIMPLE_MUTEX_H
#define __SIMPLE_MUTEX_H

#include <mutex>

/// \brief An easy to use mutex.
/// 
/// I wrote this because the version that comes with Windows is too complicated and requires too much code to use.
/// @remark Previously I used this everywhere, and in fact for a year or two RakNet was totally threadsafe.  While doing profiling, I saw that this function was incredibly slow compared to the blazing performance of everything else, so switched to single producer / consumer everywhere.  Now the user thread of RakNet is not threadsafe, but it's 100X faster than before.
class SimpleMutex
{
public:

	// Locks the mutex.  Slow!
	void Lock(void) { m.lock(); }
	
	// Unlocks the mutex.
	void Unlock(void) { m.unlock(); }
private:
	// recursive, win CRITICAL_SECTION was too
	std::recursive_mutex m;
};

#endif
