
#pragma once

// Litex CTRL Registers

#define LX_CTRL_RESET         0x000 // write to reset SoC
#define LX_CTRL_SCRATCH       0x004
#define LX_CTRL_BUS_ERRORS    0x008


// Litex UART Registers

#define LX_UART_TX            0x000 // write to transmit
#define LX_UART_RX            0x000 // read to receive
#define LX_UART_TXFULL        0x004 // 1 if TX full
#define LX_UART_RXEMPTY       0x008 // 1 if RX empty
#define LX_UART_EV_STATUS     0x00C // active events
#define LX_UART_EV_PENDING    0x010 // pending events (write to clear)
#define LX_UART_EV_ENABLE     0x014 // events that cause IRQs when pending
#define LX_UART_TXEMPTY       0x018 // 1 if TX empty
#define LX_UART_RXFULL        0x01C // 1 if RX full
#define LX_UART_MAX           0x100

#define LX_UART_EVb_TX        (1U << 0) // ready for tx write
#define LX_UART_EVb_RX        (1U << 1) // ready for rx read


// Litex Timer Registers

#define LX_TIMER_LOAD         0x000 // write to set value
#define LX_TIMER_RELOAD       0x004 // value becomes this on underflow
#define LX_TIMER_EN           0x008 // write 1 to start 0 to stop
#define LX_TIMER_UPDATE_VALUE 0x00C // write 1 to latch value for reading
#define LX_TIMER_VALUE        0x010 // ro: last latched value
#define LX_TIMER_EV_STATUS    0x014 // active events
#define LX_TIMER_EV_PENDING   0x018 // pending events (write to clear)
#define LX_TIMER_EV_ENABLE    0x01C // events that cause IRQs when pending
#define LX_TIMER_MAX          0x100

#define LX_TIMER_EVb_ZERO     (1U << 0) // value is zero


// Litex Video Framebuffer

#define LX_VFB_DMA_BASE      0x000
#define LX_VFB_DMA_LENGTH    0x004
#define LX_VFB_DMA_ENABLE    0x008
#define LX_VFB_DMA_DONE      0x00C
#define LX_VFB_DMA_LOOP      0x010
#define LX_VFB_DMA_OFFSET    0x014


// Litex Video Timing Generator

#define LX_VTG_ENABLE        0x000
#define LX_VTG_HRES          0x004
#define LX_VTG_HSYNC_START   0x008
#define LX_VTG_HSYNC_END     0x00C
#define LX_VTG_HSCAN         0x010
#define LX_VTG_VRES          0x004
#define LX_VTG_VSYNC_START   0x008
#define LX_VTG_VSYNC_END     0x00C
#define LX_VTG_VSCAN         0x010


// Litex Ethernet MAC

// WR is from MAC to SRAM (RX Packets)
#define LX_ETHMAC_WR_SLOT       0x000
#define LX_ETHMAC_WR_LEN        0x004
#define LX_ETHMAC_WR_ERR        0x008
#define LX_ETHMAC_WR_EV_STATUS  0x00C
#define LX_ETHMAC_WR_EV_PENDING 0x010
#define LX_ETHMAC_WR_EV_ENABLE  0x014

// RD is from SRAM to MAC (TX Packets)
#define LX_ETHMAC_RD_START      0x018
#define LX_ETHMAC_RD_READY      0x01C
#define LX_ETHMAC_RD_LEVEL      0x020
#define LX_ETHMAC_RD_SLOT       0x024
#define LX_ETHMAC_RD_LEN        0x028
#define LX_ETHMAC_RD_EV_STATUS  0x02C
#define LX_ETHMAC_RD_EV_PENDING 0x030
#define LX_ETHMAC_RD_EV_ENABLE  0x034

#define LX_ETHMAC_PRE_CRC       0x038
#define LX_ETHMAC_RX_PRE_ERR    0x03C
#define LX_ETHMAC_RX_CRC_ERR    0x040

#define LX_ETHMAC_EVb_WR 1
#define LX_ETHMAC_EVb_RD 1

// Litex Ethernet PHY

#define LX_ETHPHY_RESET      0x000

