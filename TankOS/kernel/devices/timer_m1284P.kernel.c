/*
 * timer_m1284P.kernel
 *
 * Created: 28.04.2012 11:29:04
 *  Author: Anton
 */ 

#include <kernel/kernel_init.h>
#include "timer_m1284P.h"

DEFINE_TIMER_PAIR_IMPL(Timer0)
DEFINE_TIMER_PAIR_IMPL(Timer1)
DEFINE_TIMER_PAIR_IMPL(Timer2)
DEFINE_TIMER_PAIR_IMPL(Timer3)

DEFINE_TIMER_IMPL(Timer0A)
DEFINE_TIMER_IMPL(Timer0B)
DEFINE_TIMER_IMPL(Timer1A)
DEFINE_TIMER_IMPL(Timer1B)
DEFINE_TIMER_IMPL(Timer2A)
DEFINE_TIMER_IMPL(Timer2B)
DEFINE_TIMER_IMPL(Timer3A)
DEFINE_TIMER_IMPL(Timer3B)

__attribute__ ((noinline)) void init_timer_m1284P() {
	INIT_TIMER_PAIR(Timer0, TIMER_NORMAL, TCCR0A, TCCR0B, TIMSK0)
	INIT_TIMER_PAIR(Timer1, TIMER_16bit, TCCR1A, TCCR1B, TIMSK1)
	INIT_TIMER_PAIR(Timer2, TIMER_ASYNCHRONOUS, TCCR2A, TCCR2B, TIMSK2)
	INIT_TIMER_PAIR(Timer3, TIMER_16bit, TCCR3A, TCCR3B, TIMSK3)

	INIT_TIMER(Timer0A, Timer0, OCR0A, TIMER_A, PinB3) // Pin 4
	INIT_TIMER(Timer0B, Timer0, OCR0B, TIMER_B, PinB4) // Pin 5
	INIT_TIMER(Timer1A, Timer1, OCR1A, TIMER_A, PinD5) // Pin 19
	INIT_TIMER(Timer1B, Timer1, OCR1B, TIMER_B, PinD4) // Pin 18
	INIT_TIMER(Timer2A, Timer2, OCR2A, TIMER_A, PinD7) // Pin 21
	INIT_TIMER(Timer2B, Timer2, OCR2B, TIMER_B, PinD6) // Pin 20
	INIT_TIMER(Timer3A, Timer3, OCR3A, TIMER_A, PinB6) // Pin 7
	INIT_TIMER(Timer3B, Timer3, OCR3B, TIMER_B, PinB7) // Pin 8
}
KERNEL_INIT(init_timer_m1284P)
