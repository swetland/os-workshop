
#pragma once

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
