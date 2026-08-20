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

#include "RPCMap.h"
#include <string.h>

// SA-MP 0.3.7-R5 uses a fixed 256-entry table indexed directly by the
// numeric one-byte RPC identifier.

RPCMap::RPCMap()
{
    memset(rpcSet, 0, sizeof(rpcSet));
}

RPCMap::~RPCMap()
{
	Clear();
}

void RPCMap::Clear(void)
{
    for (unsigned i = 0; i < 256; ++i)
    {
        if (rpcSet[i])
            delete rpcSet[i];
        rpcSet[i] = 0;
    }
}

RPCNode *RPCMap::GetNodeFromIndex(RPCIndex index)
{
    return rpcSet[(unsigned char)index];
}

RPCNode *RPCMap::GetNodeFromFunctionName(int *uniqueIdentifier)
{
    if (uniqueIdentifier == 0)
        return 0;

    return rpcSet[*((unsigned char*)uniqueIdentifier)];
}

RPCIndex RPCMap::GetIndexFromFunctionName(int *uniqueIdentifier)
{
    if (uniqueIdentifier == 0)
        return (RPCIndex)UNDEFINED_RPC_INDEX;

    // R5 simply returns the low byte; it does not search the table here.
    return (RPCIndex)(*((unsigned char*)uniqueIdentifier));
}

void RPCMap::AddIdentifierWithFunction(int *uniqueIdentifier, void *functionPointer, bool isPointerToMember)
{
    if (uniqueIdentifier == 0 || functionPointer == 0)
        return;

    const unsigned char identifier = *((unsigned char*)uniqueIdentifier);

    RPCNode *node = new RPCNode;
    node->uniqueIdentifier = identifier;
    node->functionPointer = functionPointer;
    node->isPointerToMember = isPointerToMember;

    // R5 overwrites this slot directly.  It does not free/check a previous
    // registration first.
    rpcSet[identifier] = node;
}

void RPCMap::AddIdentifierAtIndex(RPCIndex insertionIndex)
{
    const unsigned char index = (unsigned char)insertionIndex;

    if (rpcSet[index])
        delete rpcSet[index];

    RPCNode *node = new RPCNode;
    node->uniqueIdentifier = index;
    node->functionPointer = 0;
    node->isPointerToMember = false;
    rpcSet[index] = node;
}

void RPCMap::RemoveNode(int *uniqueIdentifier)
{
    if (uniqueIdentifier == 0)
        return;

    const unsigned char index = *((unsigned char*)uniqueIdentifier);
    if (rpcSet[index])
    {
        delete rpcSet[index];
        rpcSet[index] = 0;
    }
}
