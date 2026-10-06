/// \file
/// \brief \b [Internal] Random number generator
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

#ifndef __RAND_H
#define __RAND_H 

#include <random>

// one engine per thread, so no data races (old one was a plain global)
// shared engine, seed it with seedMT (default seed is fixed, like the old 4357)
inline std::mt19937 &RakRng( void )
{
	static std::mt19937 rng;
	return rng;
}

 /// Initialise seed for Random Generator
 /// \param[in] seed The seed value for the random number generator.
inline void seedMT( unsigned int seed )
{
    RakRng().seed( seed );
}

/// Gets a random unsigned int
/// \return an integer random value.
inline unsigned int randomMT( void )
{
    return RakRng()();
}

/// Gets a random float
/// \return 0 to 1.0f, inclusive
inline float frandomMT( void )
{
	return ( float ) ( ( double ) randomMT() / 4294967296.0 );
}

#endif
