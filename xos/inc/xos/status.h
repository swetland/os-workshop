// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#pragma once

//-------------------------------------------------------------
// Success
#define XOS_OK                  0x0000

//-------------------------------------------------------------
// Timeout Expired
#define XOS_TIMEOUT             0x0001

// Not Ready Yet, Try Later (eg, no data to read from socket)
#define XOS_WAIT                0x0002

// Far Endpoint No Longer Available
#define XOS_PEER_CLOSED         0x0003

//-------------------------------------------------------------
#define XOS_ERROR               0x0080

//-------------------------------------------------------------
// IO Error
#define XOS_ERR_IO              0x0081

// Cannot locate requested entity
#define XOS_ERR_NOT_FOUND       0x0082

// Cannot do this operation on this object
#define XOS_ERR_NOT_SUPPORTED   0x0083

// Out of memory, resources, etc
#define XOS_ERR_NOT_AVAILABLE   0x0084

// Already Exists, Already Bound, Already Mapped, etc
#define XOS_ERR_CONFLICT        0x0085

// Caller lacks permission for this operation
#define XOS_ERR_PERMISSION      0x0086

//-------------------------------------------------------------
// Invalid Argument: Call Specific
#define XOS_ERR_BAD_PARAM       0x0040

// Invalid Argument: Bad Handle
#define XOS_ERR_BAD_HANDLE      0x0041

// Inavlid Argument: Wrong Type of Handle
#define XOS_ERR_BAD_TYPE        0x0042

// Invalid Argument: Bogus Pointer 
#define XOS_ERR_BAD_POINTER     0x0043

// Invalid Argument: Out of Range
#define XOS_ERR_BAD_RANGE       0x0044

// Invalid Argument: Buffer Too Small
#define XOS_ERR_BAD_BUFFER      0x0045

// Invalid System Call Number
#define XOS_ERR_BAD_SYSCALL     0x0046

// Action Impossible
#define XOS_ERR_BAD_STATE       0x0047
