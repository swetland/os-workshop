// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#pragma once

#include <stdint.h>

#define NB_PORT_QUERY  58571
#define NB_PORT_CTRL   58572

// echo -n netboot.magic | sha256sum | cut -c1-8
// commands are similar (netboot.cmd.name)
#define NB_MAGIC       0x1e5dae11U

// host to target commands
#define NB_CMD_QUERY   0x56a938e6U // arg=0, db=targetname.ascii
#define NB_CMD_WRITE   0x7b639621U // arg=addr, db=bytes
#define NB_CMD_READ    0x1c1d1ecfU // arg=addr, reply.db=bytes
#define NB_CMD_EXEC    0xbdb9a844U // arg=addr

// target to host responses
#define NB_CMD_STATUS  0x9e68e03bU // arg=status (0=OK)

// target to multicast messages
#define NB_CMD_SYSLOG  0x5a374503U // arg=0, db=logmsgs.ascii

#define NB_OK          0
#define NB_ERR_BADCMD  1
#define NB_ERR_PARAM   2

#define NB_MSG_MIN     16
#define NB_MSG_MAX     (16 + 1024)
#define NB_DATA_MAX    1024

typedef struct {
	uint32_t magic;
	uint32_t cmd;
	uint32_t seq;
	uint32_t arg;
	union {
		uint8_t db[1024];
		uint32_t dw[1024 / 4];
	};
} netboot_msg_t;

