// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#include <hw/debug.h>
#include <hw/riscv.h>
#include <hw/context.h>
#include <hw/intrinsics.h>
#include <hw/platform.h>
#include <hw/litex.h>
#include <gfx/gfx.h>
#include <string.h>

// (extremely simple) memory management
// ------------------------------------

#define MEMORY_TOP (DRAM_BASE + DRAM_SIZE)

static uint8_t* next_stack = (void*) (MEMORY_TOP - 4096);
static uint8_t* next_heap = (void*) (DRAM_BASE + 1024*1024);

void* alloc_stack(void) {
	void* s = next_stack;
	next_stack -= 4096;
	memset(next_stack, 0xee, 4096);
	return s;
}

void* alloc(unsigned sz) {
	sz = (sz + 31) & (~31);
	uint8_t* ptr = next_heap;
	memset(ptr, 0, sz);
	next_heap += sz;
	return ptr;
}

// (cooperative) thread library
// ----------------------------

#define THREAD_MAGIC 0x5c95bec3

typedef struct thread {
	uint32_t magic;
	uint32_t id;
	struct thread *next;
	struct thread *prev;
	cframe_t ctxt;
} thread_t;

static uint32_t next_thread_id = 1;

thread_t thread0 = {
	.magic = THREAD_MAGIC,
	.id = 0,
	.next = &thread0,
	.prev = &thread0,
};

void thread_init(void) {
	threadptr_set(&thread0);
};

void thread_exit(int status);

thread_t* thread_create(int (*fn)(void*), void* arg) {
	irq_disable();

	thread_t *current = threadptr_get();

	thread_t *t = alloc(sizeof(thread_t));
	t->magic = THREAD_MAGIC;
	t->id = next_thread_id++;
	t->next = current;
	t->prev = current->prev;
	t->prev->next = t;
	t->next->prev = t;

	// setup threadptr and stackptr
	t->ctxt.tp = (uintptr_t) t;
	t->ctxt.sp = (uintptr_t) alloc_stack();
	t->ctxt.pc = (uintptr_t) trap_exit;

	// setup a trap frame to allow us to "return"
	// into the new thread the first time we switch to it
	t->ctxt.sp -= (32 * 4);
	tframe_t *frame = (void*) t->ctxt.sp;
	memset(frame, 0, sizeof(*frame));
	frame->ra = (uintptr_t) thread_exit;
	frame->a0 = (uintptr_t) arg;
	frame->pc = (uintptr_t) fn;

	xprintf("[ created thread %p id=%u sp=%08x pc=%08x ]\n",
		t, t->id, t->ctxt.sp, frame->pc);

	irq_enable();
	return t;
}

void thread_exit(int status) {
	// disable interrupts to protect thread list manipulation
	irq_disable();

	// make sure privilege and interrupt status are correct
	// for when sret is executed after the context switch
	csr_set(CSR_SSTATUS, SSTATUS_SPP | SSTATUS_SPIE);

	thread_t *current = threadptr_get();
	thread_t *next = current->next;
	thread_t *prev = current->prev;

	xprintf("[ thread %p id=%u exited status=%d ]\n", current, current->id, status);

	if (next == current) {
		xprintf("[ all threads have exited ]\n");
		for (;;) ;
	}

	// remove current thread from list
	next->prev = prev;
	prev->next = next;
	current->prev = 0;
	current->next = 0;

	// switch to next thread
	context_switch(&current->ctxt, &next->ctxt);

	xprintf("[ error: zombie thread ]\n");
	for (;;) ;
}

// multithreaded mandelbrot demo
// -----------------------------

uint16_t colors[12] = {
	0x1111, 0x2222, 0x3333, 0x4444, 0x5555, 0x6666,
	0x7777, 0x8888, 0x9999, 0xAAAA, 0xBBBB, 0xCCCC,
};

int render(gfx_surface_t* _gs,
		uint32_t px0, uint32_t py0,
		uint32_t px1, uint32_t py1) {
	gfx_surface_t gs;
	memcpy(&gs, _gs, sizeof(gs));

	for (int py = py0; py < py1; py++) {
		int y0 = 650 - (1300 * py) / gs.height;
		for (int px = px0; px < px1; px++) {
			int x0 = -1250 + (1750 * px) / gs.width;
			int x = 0, y = 0;
			for (int i = 0; i < 500; i++) {
				int x2 = x * x / 1000;
				int y2 = y * y / 1000;
				if ((x2 + y2) > 4000) {
					gs.fgcolor = colors[(i > 11) ? 11 : i];
					gfx_plot(&gs, px, py);
					goto done;
				}
				y = 2 * x * y / 1000 + y0;
				x = x2 - y2 + x0;
			}
			gs.fgcolor = 0;
			gfx_plot(&gs, px, py);
		done:
			;
		}
	}

	return 2;
}

int t1(void* arg) {
	gfx_surface_t *gs = arg;
	gfx_fill(gs, 0, 0, 320, 240, gfx_color(gs, C_WHITE));
	gfx_puts(gs, 0, 240-16, "Thread One");
	return render(gs, 0, 0, 320, 240);
}
int t2(void* arg) {
	gfx_surface_t *gs = arg;
	gfx_fill(gs, 320, 240, 640, 480, gfx_color(gs, C_GRAY75));
	gfx_puts(gs, 320, 480-16, "Thread Two");
	return render(gs, 320, 240, 640, 480);
}
int t3(void* arg) {
	gfx_surface_t *gs = arg;
	gfx_fill(gs, 320, 0, 640, 240, gfx_color(gs, C_GRAY25));
	gfx_puts(gs, 320, 240-16, "Thread Three");
	return render(gs, 320, 0, 640, 240);
}
int t4(void* arg) {
	gfx_surface_t *gs = arg;
	gfx_fill(gs, 0, 240, 320, 480, gfx_color(gs, C_GRAY50));
	gfx_puts(gs, 0, 480-16, "Thread Four");
	return render(gs, 0, 240, 320, 480);
}

// program start
// -------------

#define timer_rd(a) io_rd32(TIMER0_BASE + LX_TIMER_ ## a)
#define timer_wr(a,v) io_wr32(TIMER0_BASE + LX_TIMER_ ## a, v)

void timer_init(void) {
	// enable timer0 irq
	csr_set(CSR_S_INTC_ENABLE, TIMER0_IRQb);

	// enable external interrupts
	csr_set(CSR_SIE, INTb_SVC_EXTERN);

	// program timer for 10ms repeating
	timer_wr(EN, 0);
	timer_wr(EV_PENDING, LX_TIMER_EVb_ZERO);
	timer_wr(LOAD, 0);
	timer_wr(RELOAD, 50000000/100);
	timer_wr(EN, 1);
	timer_wr(EV_ENABLE, LX_TIMER_EVb_ZERO);
}

void interrupt_handler(void) {
	if (timer_rd(EV_PENDING)) {
		// acknowledge timer irq
		timer_wr(EV_PENDING, LX_TIMER_EVb_ZERO);
		// switch to next thread
		thread_t* current = threadptr_get();
		context_switch(&current->ctxt, &current->next->ctxt);
	}
}

void exception_handler(eframe_t *ef) {
	xprintf("\n** SUPERVISOR EXCEPTION **\n");
	xprint_s_exception(ef);
	xprintf("\nHALT\n");
	for (;;) ;
}

void start(void) {
	xprintf("Example 11 - Threads Preemptive\n\n");

	csr_write(CSR_STVEC, (uintptr_t) trap_entry);

	// setup the main thread (running now)
	thread_init();

	// setup graphics
	gfx_surface_t *gs = alloc(sizeof(*gs));
	gfx_init_display(gs);
	memset((void*) gs->pixels, 0, gs->width * gs->height * 2);

	// launch four worker threads
	thread_create(t1, gs);
	thread_create(t2, gs);
	thread_create(t3, gs);
	thread_create(t4, gs);

	timer_init();

	// exit the main thread
	// this will context switch to another thread
	// and enable interrupts to allow multitasking
	thread_exit(0);
}

