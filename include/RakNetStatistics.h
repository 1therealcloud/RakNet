/// \file
/// \brief A structure that holds all statistical data returned by RakNet.
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


#ifndef __RAK_NET_STATISTICS_H
#define __RAK_NET_STATISTICS_H

#include "PacketPriority.h"
#include "Export.h"
#include "NetworkTypes.h"

/// \brief Network Statisics Usage 
///
/// Store Statistics information related to network usage 
#pragma pack(push, 1)
struct RAK_DLL_EXPORT RakNetStatisticsStruct
{
	unsigned messageSendBuffer[ NUMBER_OF_PRIORITIES ];
	unsigned messagesSent[ NUMBER_OF_PRIORITIES ];

	// Present in SA-MP 0.3.7-R5.  These 16 bytes sit between the two
	// 32-bit priority arrays and the 64-bit bit counters.
	unsigned char sampR5Pad20[16];

	unsigned long long messageDataBitsSent[ NUMBER_OF_PRIORITIES ];
	unsigned long long messageTotalBitsSent[ NUMBER_OF_PRIORITIES ];

	unsigned packetsContainingOnlyAcknowlegements;
	unsigned acknowlegementsSent;
	unsigned acknowlegementsPending;
	unsigned long long acknowlegementBitsSent;
	unsigned packetsContainingOnlyAcknowlegementsAndResends;
	unsigned messageResends;
	unsigned long long messageDataBitsResent;
	unsigned long long messagesTotalBitsResent;
	unsigned messagesOnResendQueue;
	unsigned numberOfUnsplitMessages;
	unsigned numberOfSplitMessages;
	unsigned totalSplits;
	unsigned packetsSent;
	unsigned long long encryptionBitsSent;
	unsigned long long totalBitsSent;
	unsigned sequencedMessagesOutOfOrder;
	unsigned sequencedMessagesInOrder;
	unsigned orderedMessagesOutOfOrder;
	unsigned orderedMessagesInOrder;
	unsigned packetsReceived;
	unsigned packetsWithBadCRCReceived;
	unsigned long long bitsReceived;
	unsigned long long bitsWithBadCRCReceived;
	unsigned acknowlegementsReceived;
	unsigned duplicateAcknowlegementsReceived;
	unsigned messagesReceived;
	unsigned invalidMessagesReceived;
	unsigned duplicateMessagesReceived;
	unsigned messagesWaitingForReassembly;
	unsigned internalOutputQueueSize;
	double bitsPerSecond;
	RakNetTime connectionStartTime;

	// R5-private statistics state.  Names are intentionally neutral until
	// their exact public meaning is needed; offsets are +0x110..+0x124.
	RakNetTime field_110;
	unsigned field_114;
	unsigned field_118;
	RakNetTime field_11C;
	unsigned field_120;
	unsigned field_124;

	RakNetStatisticsStruct operator +=(const RakNetStatisticsStruct& other)
	{
		unsigned i;
		for (i=0; i < NUMBER_OF_PRIORITIES; i++)
		{
			messageSendBuffer[i]+=other.messageSendBuffer[i];
			messagesSent[i]+=other.messagesSent[i];
			messageDataBitsSent[i]+=other.messageDataBitsSent[i];
			messageTotalBitsSent[i]+=other.messageTotalBitsSent[i];
		}

		packetsContainingOnlyAcknowlegements+=other.packetsContainingOnlyAcknowlegements;
		acknowlegementsSent+=other.acknowlegementsSent;
		acknowlegementsPending+=other.acknowlegementsPending;
		acknowlegementBitsSent+=other.acknowlegementBitsSent;
		packetsContainingOnlyAcknowlegementsAndResends+=other.packetsContainingOnlyAcknowlegementsAndResends;
		messageResends+=other.messageResends;
		messageDataBitsResent+=other.messageDataBitsResent;
		messagesTotalBitsResent+=other.messagesTotalBitsResent;
		messagesOnResendQueue+=other.messagesOnResendQueue;
		numberOfUnsplitMessages+=other.numberOfUnsplitMessages;
		numberOfSplitMessages+=other.numberOfSplitMessages;
		totalSplits+=other.totalSplits;
		packetsSent+=other.packetsSent;
		encryptionBitsSent+=other.encryptionBitsSent;
		totalBitsSent+=other.totalBitsSent;
		sequencedMessagesOutOfOrder+=other.sequencedMessagesOutOfOrder;
		sequencedMessagesInOrder+=other.sequencedMessagesInOrder;
		orderedMessagesOutOfOrder+=other.orderedMessagesOutOfOrder;
		orderedMessagesInOrder+=other.orderedMessagesInOrder;
		packetsReceived+=other.packetsReceived;
		packetsWithBadCRCReceived+=other.packetsWithBadCRCReceived;
		bitsReceived+=other.bitsReceived;
		bitsWithBadCRCReceived+=other.bitsWithBadCRCReceived;
		acknowlegementsReceived+=other.acknowlegementsReceived;
		duplicateAcknowlegementsReceived+=other.duplicateAcknowlegementsReceived;
		messagesReceived+=other.messagesReceived;
		invalidMessagesReceived+=other.invalidMessagesReceived;
		duplicateMessagesReceived+=other.duplicateMessagesReceived;
		messagesWaitingForReassembly+=other.messagesWaitingForReassembly;
		internalOutputQueueSize+=other.internalOutputQueueSize;

		return *this;
	}
};
#pragma pack(pop)

/// Verbosity level currently supports 0 (low), 1 (medium), 2 (high)
/// \param[in] s The Statistical information to format out
/// \param[in] buffer The buffer containing a formated report
/// \param[in] verbosityLevel 
/// 0 low
/// 1 medium 
/// 2 high 
void StatisticsToString( RakNetStatisticsStruct *s, char *buffer, int verbosityLevel );

#endif
