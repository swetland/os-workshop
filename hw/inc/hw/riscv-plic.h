
// Platform Level Interrupt Controller

#pragma once

#define PLIC_PRIORITY		(0x000000)
#define PLIC_PENDING		(0x001000)
#define PLIC_MENABLE(id)	(0x002000 + 0x100 * (id))
#define PLIC_SENABLE(id)	(0x002080 + 0x100 * (id))
#define PLIC_MPRIORITY(id)	(0x200000 + 0x2000 * (id))
#define PLIC_SPRIORITY(id)	(0x201000 + 0x2000 * (id))
#define PLIC_MCLAIN(id)		(0x200004 + 0x2000 * (id))
#define PLIC_SCLAIM(id)		(0x201004 + 0x2000 * (id))
