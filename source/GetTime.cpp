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

#include "GetTime.h"
#if defined(_WIN32)
#include <windows.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif

static bool initialized=false;
#ifdef _WIN32
static LARGE_INTEGER yo;
#else
static timeval initialTime;
#endif

RakNetTime RakNet::GetTime( void )
{
	if ( initialized == false )
	{
#ifdef _WIN32
		QueryPerformanceFrequency( &yo );
#else
		gettimeofday( &initialTime, 0 );
#endif
		initialized = true;
	}
	
#ifdef _WIN32
	LARGE_INTEGER PerfVal;
	
	QueryPerformanceCounter( &PerfVal );
	
	return (RakNetTime)(PerfVal.QuadPart*1000 / yo.QuadPart);
#else
	struct timeval tp;
	gettimeofday( &tp, 0 );
	
	// Seconds to ms and microseconds to ms
	return ( tp.tv_sec - initialTime.tv_sec ) * 1000 + ( tp.tv_usec - initialTime.tv_usec ) / 1000;
	
#endif
}


RakNetTimeNS RakNet::GetTimeNS( void )
{
	if ( initialized == false )
	{
#ifdef _WIN32
		QueryPerformanceFrequency( &yo );
#else
		gettimeofday( &initialTime, 0 );
#endif
		initialized = true;
	}

#ifdef _WIN32
	LARGE_INTEGER PerfVal;

	QueryPerformanceCounter( &PerfVal );

	__int64 quotient, remainder;
	quotient=((PerfVal.QuadPart*1000) / yo.QuadPart);
	remainder=((PerfVal.QuadPart*1000) % yo.QuadPart);
	return quotient*1000 + (remainder*1000 / yo.QuadPart);

#else
	struct timeval tp;
	gettimeofday( &tp, 0 );

	return ( tp.tv_sec - initialTime.tv_sec ) * (RakNetTimeNS) 1000000 + ( tp.tv_usec - initialTime.tv_usec );

#endif
}
