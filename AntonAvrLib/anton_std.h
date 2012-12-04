/*
 * anton_std.h
 *
 * -- Some default includes and defines
 * 
 * Created: 08.02.2012 23:34:38
 *  Author: Anton
 */ 

#ifndef ANTON_STD_H_
#define ANTON_STD_H_

#include <stdint.h>
#include <stdlib.h>

#ifdef AVR
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#else
#define _BV(a) (1 << a)
#include "fake_registers.h"
#endif

typedef enum {
	FALSE = 0,
	TRUE = 1
} BOOL;

typedef uint8_t byte;

asm ("__RAMPZ__ = 0x3b");

#define ZERO_STRUCT(variableName, structName)				\
	uint8_t *___tmpStructContent = variableName;			\
	for (int __i = 0; __i < sizeof(structName); __i++) {	\
		___tmpStructContent[i] = 0;							\
	}

#define __CONCAT__(A, B) A##B

// macro to define anonymous pointer-types with compiler-checked type-safety.
// This whole struct can be cast to a void-pointer.
#define DEFINE_HANDLE(name)	\
	typedef struct name##__ { void *pointer; } name;

#define Invalid(Type) ((Type) { NULL })
#define ConstantInvalid(Type) { NULL }
#define IsValid(object) (object.pointer != NULL)
#define Cast(OtherType, object) ((OtherType) { object.pointer })
#define As(Type, ptr) ((Type) { ptr })
#define Equal(object1, object2) (object1.pointer == object2.pointer)

#define LOBYTE(x)        (uint8_t)((uint16_t)x)
#define HIBYTE(x)        (uint8_t)(((uint16_t)x)>>8)
#define MAKE_WORD(hi,lo) (((hi) * 0x100) + lo)
#define AS_WORD(b) MAKE_WORD(b, 0)

#define enable_interrupts() sei()
#define disable_interrupts() cli()

#define delay(x) _delay_ms(x)

// This can be used to ignore unused variables in test-modules
// #pragma GCC diagnostic ignored "-Wunused-variable"

#endif /* ANTON_STD_H_ */
