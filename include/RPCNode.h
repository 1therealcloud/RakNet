/// \file
/// \brief \b [Internal] Holds information related to a RPC
///
/// \ingroup RAKNET_RPC
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

#ifndef __RPC_NODE
#define __RPC_NODE

#include "NetworkTypes.h"
#include "Export.h"

class RakPeerInterface;

/// \defgroup RAKNET_RPC Remote Procedure Call Subsystem
/// \brief A system to call C or object member procedures on remote systems.

/// SA-MP 0.3.7-R5 stores this node packed as:
/// byte rpcId, pointer function, byte isMember (6 bytes in the 32-bit client).
#pragma pack(push, 1)
struct RAK_DLL_EXPORT RPCNode
{
    unsigned char uniqueIdentifier;

    union
    {
        void ( *staticFunctionPointer ) ( RPCParameters *rpcParms );
#if defined(__GNUC__)
        void (*memberFunctionPointer)(void* _this, RPCParameters *rpcParms);
#else
        void (__cdecl *memberFunctionPointer)(void* _this, RPCParameters *rpcParms);
#endif
        void *functionPointer;
    };

    bool isPointerToMember;
};
#pragma pack(pop)

#endif
