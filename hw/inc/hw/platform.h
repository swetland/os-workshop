
#pragma once

#include <hw/riscv.h>
#include <hw/vexriscv.h>

// Memory Map
#define DRAM_BASE          0x40000000
#define FRAMEBUFFER_BASE   0x40C00000 // default fb dma base
#define DRAM_SIZE          (32*1024*1024)
#define BOOTLOADER_SIZE    0x00008000
#define MMIO_BASE          0xF0000000

// MMIO Devices
#define CTRL_BASE          0xF0000000
#define IDENT_BASE         0xF0000800
#define LEDS_BASE          0xF0001000
#define TIMER0_BASE        0xF0002000
#define UART0_BASE         0xF0002800
#define VFB_BASE           0xF0003000
#define VFB_VTG_BASE       0xF0003800
#define SPIFLASH_CORE_BASE 0xF0004000
#define SPICLASH_PHY_BASE  0xF0004800
#define SPISDCARD_BASE     0xF0005000

// Peripheral Interrupt Numbers
#define TIMER0_IRQn 0
#define UART0_IRQn  1

// Peripheral Interrupt Bits
#define TIMER0_IRQb (1U << TIMER0_IRQn)
#define UART0_IRQb (1U << UART0_IRQn)

#ifndef __ASSEMBLY__

#include <stdint.h>

static inline void io_wr32(uint32_t a, uint32_t v) {
	*((volatile uint32_t*) a) = v;
}
static inline uint32_t io_rd32(uint32_t a) {
	return *((volatile uint32_t*) a);
}

static inline void ext_irq_enable(uint32_t bit) {
	csr_set(CSR_S_INTC_ENABLE, bit);
}
static inline void ext_irq_disable(uint32_t bit) {
	csr_clr(CSR_S_INTC_ENABLE, bit);
}
static inline int ext_irq_pending(uint32_t bit) {
	return !!(csr_read(CSR_S_INTC_PENDING) & bit);
}

#endif
