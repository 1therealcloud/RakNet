/// \file
/// \brief \b [Internal] Generates and validates checksums
///
/// \note I didn't write this, but took it from http://www.flounder.com/checksum.htm
///

#ifndef __CHECKSUM_H
#define __CHECKSUM_H

#include <cstring>

/// Generates and validates checksums
class CheckSum
{

public:
	
 /// Default constructor
	
	CheckSum()
	{
		Clear();
	}
	
	void Clear()
	{
		sum = 0;
		r = 55665;
		c1 = 52845;
		c2 = 22719;
	}

	/****************************************************************************
	*        CheckSum::add
	* Inputs:
	*   unsigned int d: word to add
	* Result: void
	* 
	* Effect: 
	*   Adds the bytes of the unsigned int to the CheckSum
	****************************************************************************/

	void Add ( unsigned int value )
	{
		unsigned char bytes[4];
		std::memcpy(bytes, &value, sizeof(bytes));

		for ( unsigned int i = 0; i < sizeof( bytes ); i++ )
			Add ( bytes[ i ] );
	} // CheckSum::add(unsigned int)


	/****************************************************************************
	*       CheckSum::add
	* Inputs:
	*   unsigned short value:
	* Result: void
	* 
	* Effect: 
	*   Adds the bytes of the unsigned short value to the CheckSum
	****************************************************************************/

	void Add ( unsigned short value )
	{
		unsigned char bytes[2];
		std::memcpy(bytes, &value, sizeof(bytes));

		for ( unsigned int i = 0; i < sizeof( bytes ); i++ )
			Add ( bytes[ i ] );
	} // CheckSum::add(unsigned short)

	/****************************************************************************
	*       CheckSum::add
	* Inputs:
	*   LPunsigned char b: pointer to byte array
	*   unsigned int length: count
	* Result: void
	* 
	* Effect: 
	*   Adds the bytes to the CheckSum
	****************************************************************************/

	void Add ( unsigned char *b, unsigned int length )
	{
		for ( unsigned int i = 0; i < length; i++ )
			Add ( b[ i ] );
	} // CheckSum::add(LPunsigned char, unsigned int)

	/****************************************************************************
	*       CheckSum::add
	* Inputs:
	*   unsigned char value:
	* Result: void
	* 
	* Effect: 
	*   Adds the byte to the CheckSum
	****************************************************************************/

	void Add ( unsigned char value )
	{
		unsigned char cipher = (unsigned char)( value ^ ( r >> 8 ) );
		r = ( cipher + r ) * c1 + c2;
		sum += cipher;
	} // CheckSum::add(unsigned char)

	unsigned int Get ()
	{
		return sum;
	}
	
protected:
	unsigned short r;
	unsigned short c1;
	unsigned short c2;
	unsigned int sum;
};

#endif
