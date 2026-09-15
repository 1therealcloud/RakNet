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

#include "NetworkTypes.h"
#include <string.h>
#include <stdio.h>

#if defined(_WIN32)
// IP_DONTFRAGMENT is different between winsock 1 and winsock 2.  Therefore, Winsock2.h must be linked againt Ws2_32.lib
// winsock.h must be linked against WSock32.lib.  If these two are mixed up the flag won't work correctly
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

// Defaults to not in peer to peer mode for NetworkIDs.  This only sends the localSystemId portion in the BitStream class
// This is what you want for client/server, where the server assigns all NetworkIDs and it is unnecessary to transmit the full structure.
// For peer to peer, this will transmit the playerId of the system that created the object in addition to localSystemId.  This allows
// Any system to create unique ids locally.
// All systems must use the same value for this variable.
bool RAK_DLL_EXPORT NetworkID::peerToPeerMode=false;

bool PlayerID::operator==( const PlayerID& right ) const
{
	return binaryAddress == right.binaryAddress && port == right.port;
}

bool PlayerID::operator!=( const PlayerID& right ) const
{
	return binaryAddress != right.binaryAddress || port != right.port;
}

bool PlayerID::operator>( const PlayerID& right ) const
{
	return ( ( binaryAddress > right.binaryAddress ) || ( ( binaryAddress == right.binaryAddress ) && ( port > right.port ) ) );
}

bool PlayerID::operator<( const PlayerID& right ) const
{
	return ( ( binaryAddress < right.binaryAddress ) || ( ( binaryAddress == right.binaryAddress ) && ( port < right.port ) ) );
}
char *PlayerID::ToString(bool writePort) const
{
	static char str[22];
	in_addr in;
	in.s_addr = binaryAddress;
	strcpy(str, inet_ntoa( in ));
	if (writePort)
	{
		snprintf(str + strlen(str), sizeof(str) - strlen(str), ":%u", port);
	}
	
	return str;
}
void PlayerID::SetBinaryAddress(const char *str)
{
	binaryAddress=inet_addr(str);
}

NetworkID& NetworkID::operator = ( const NetworkID& input )
{
	playerId = input.playerId;
	localSystemId = input.localSystemId;
	return *this;
}

bool NetworkID::operator==( const NetworkID& right ) const
{
	if (NetworkID::peerToPeerMode)
		return playerId == right.playerId && localSystemId == right.localSystemId;
	else
		return localSystemId==right.localSystemId;
}

bool NetworkID::operator!=( const NetworkID& right ) const
{
	if (NetworkID::peerToPeerMode)
		return playerId != right.playerId || localSystemId != right.localSystemId;
	else
		return localSystemId!=right.localSystemId;
}

bool NetworkID::operator>( const NetworkID& right ) const
{
	if (NetworkID::peerToPeerMode)
		return ( ( playerId > right.playerId ) || ( ( playerId == right.playerId ) && ( localSystemId > right.localSystemId ) ) );
	else
		return localSystemId>right.localSystemId;
}

bool NetworkID::operator<( const NetworkID& right ) const
{
	if (NetworkID::peerToPeerMode)
		return ( ( playerId < right.playerId ) || ( ( playerId == right.playerId ) && ( localSystemId < right.localSystemId ) ) );
	else
		return localSystemId<right.localSystemId;
}

bool NetworkID::IsPeerToPeerMode(void)
{
	return peerToPeerMode;
}
void NetworkID::SetPeerToPeerMode(bool isPeerToPeer)
{
	peerToPeerMode=isPeerToPeer;
}
