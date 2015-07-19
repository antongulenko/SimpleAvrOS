#ifndef NATIVE_SIMULATION_H_
#define NATIVE_SIMULATION_H_

#include <stdint.h>

// Resets all variables
void init_native_simulation();

// This is included when we do a native build.
// Instead of preprocessor-macros for hardware addresses, some registers
// are simply represented as global data.

// This also contains some definitions to make code compile for the native env.

// == Types
#define _BV(a) (1 << a)
typedef uint8_t REGISTER;

// == stdio.h
// This is not usable, only to make the code compile somehow.
#define FDEV_SETUP_STREAM(a,b,c) {0}

// == Memory
// TODO use correct values x86 platforms?
extern uint16_t DYNAMIC_MEMORY_START;
extern uint16_t ALLOCATED_HEAP_END;
extern uint16_t MALLOC_END;
extern uint16_t MALLOC_START;
#define RAMSTART 0x100
#define RAMEND 0xffff

// == delay
// This allows adding some code to each delay_ms call.
// Intended for testing, to keep track of delay_ms calls.
void delay_ms_hook(uint32_t ms);
void delay_us_hook(uint32_t us);

static inline void delay_ms(uint32_t ms) {
	delay_ms_hook(ms);
}
static inline void delay_us(uint32_t us) {
	delay_us_hook(us);
}

// == External interrupts
extern REGISTER PCMSK0, PCMSK1, PCMSK2, PCMSK3;
extern REGISTER PCICR;

uint8_t _interrupts_enabled; // Can be used by tests
void sei(); // Enable external interrupts
void cli(); // Disable external interrupts

// == Atomic
// TODO -- this should be testable somehow.
#define ATOMIC_BLOCK(ARG)
#define ATOMIC_RESTORESTATE
#define ATOMIC_FORCEON

// == Sleep
#define sleep_enable()
#define set_sleep_mode(SleepMode)
#define SLEEP_MODE_IDLE
#define sleep_cpu()

// == WDT
#define WDRF 0
extern REGISTER MCUSR;
#define wdt_enable(mode)
#define WDTO_15MS
#define wdt_disable()

// == Reset Condition
#define PORF 1
#define BORF 2

// == Hardware reset
extern uint8_t hardware_reset_triggered;
void HARDWARE_RESET();

// == TWI
#define TWIE 0
#define TWEN 1
#define TWSTA 2
#define TWSTO 3
#define TWEA 4
#define TWINT 5

extern REGISTER TWCR, TWDR, TWBR, TWSR, TWAR, TWAMR;

enum {
	TW_START,
	TW_REP_START,
	TW_MR_ARB_LOST,
	TW_MT_DATA_ACK,
	TW_MT_SLA_ACK,
	TW_MT_SLA_NACK,
	TW_MT_DATA_NACK,
	TW_MR_SLA_ACK,
	TW_MR_DATA_ACK,
	TW_MR_SLA_NACK,
	TW_MR_DATA_NACK,
	TW_ST_SLA_ACK,
	TW_ST_ARB_LOST_SLA_ACK,
	TW_ST_DATA_ACK,
	TW_ST_LAST_DATA,
	TW_ST_DATA_NACK,
	TW_SR_SLA_ACK,
	TW_SR_ARB_LOST_SLA_ACK,
	TW_SR_GCALL_ACK,
	TW_SR_ARB_LOST_GCALL_ACK,
	TW_SR_DATA_ACK,
	TW_SR_GCALL_DATA_ACK,
	TW_SR_STOP,
	TW_SR_DATA_NACK,
	TW_SR_GCALL_DATA_NACK,
	TW_NO_INFO,
	TW_BUS_ERROR,
    TW_MT_ARB_LOST = TW_MR_ARB_LOST, // They are the same in avr-libc
};

// eeprom
#define EEMEM
uint16_t eeprom_read_word(uint16_t *addr);
void eeprom_update_word(uint16_t *addr, uint16_t value);

#endif // NATIVE_SIMULATION_H_
