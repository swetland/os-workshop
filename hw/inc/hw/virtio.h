#pragma once

#include <stdint.h>

#define VIO_MagicValue		0x000 // R
#define VIO_Version		0x004 // R
#define VIO_DeviceID		0x008 // R
#define VIO_VendorID		0x00C // R
#define VIO_DeviceFeatures	0x010 // R
#define VIO_DeviceFeaturesSel	0x014 // W
#define VIO_DriverFeatures	0x020 // W
#define VIO_DriverFeaturesSel	0x024 // W
#define VIO_QueueSel		0x030 // W   Selects Queue Number for VIO_Queue*
#define VIO_QueueNumMax		0x034 // R
#define VIO_QueueNum		0x038 // W
#define VIO_QueueReady		0x044 // RW
#define VIO_QueueNotify		0x050 // W
#define VIO_InterruptStatus	0x060 // R
#define VIO_InterruptAck	0x064 // W
#define VIO_DeviceStatus	0x070 // RW
#define VIO_QueueDescLow	0x080 // W   Address of Queue's Descriptor Area
#define VIO_QueueDescHigh	0x084 // W
#define VIO_QueueDriverLow	0x090 // W   Address of Queue's Driver Area
#define VIO_QueueDriverHigh	0x094 // W
#define VIO_QueueDeviceLow	0x0A0 // W   Address of Queue's Device Area
#define VIO_QueueDeviceHigh	0x0A4 // W
#define VIO_ConfigGeneration	0x0FC // R
#define VIO_Config		0x100 // RW  Device-Specific Config Space


#define VIO_MAGIC 0x74726976

// for VIO_Interrupt{Status,Ack}
#define VIO_INT_USED_BUFFER     (1U << 0)
#define VIO_INT_CONFIG_CHANGED  (1U << 1)
