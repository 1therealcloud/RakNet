/// \file
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

#include "SimpleMutex.h"
#include <assert.h>

SimpleMutex::SimpleMutex()
{
#ifdef _WIN32
	InitializeCriticalSection(&criticalSection);
#else
	int error = pthread_mutex_init(&hMutex, 0);
	assert(error==0);
#endif
}

SimpleMutex::~SimpleMutex()
{
#ifdef _WIN32
	//	CloseHandle(hMutex);
	DeleteCriticalSection(&criticalSection);
#else
	pthread_mutex_destroy(&hMutex);
#endif
}

#ifdef _WIN32
#ifdef _DEBUG
#include <stdio.h>
#endif
#endif

void SimpleMutex::Lock(void)
{
#ifdef _WIN32
	EnterCriticalSection(&criticalSection);
#else
	int error = pthread_mutex_lock(&hMutex);
	assert(error==0);
#endif
}

void SimpleMutex::Unlock(void)
{
#ifdef _WIN32
	LeaveCriticalSection(&criticalSection);
#else
	int error = pthread_mutex_unlock(&hMutex);
	assert(error==0);
#endif
}
