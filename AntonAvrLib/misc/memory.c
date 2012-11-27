
#include "memory.h"

// This is maintained by the malloc implementation.
// Points to the address on the heap, from where on there are no allocations.
extern char *__brkval;

uint16_t usedDynamicMemory() {
	uint16_t mainProcessStack = RAMEND - (uint16_t) __malloc_heap_end;
	uint16_t usedHeap = (uint16_t) __brkval - (uint16_t) __malloc_heap_start;
	return mainProcessStack + usedHeap;
}

uint16_t availableDynamicMemory() {
	return TotalDynamicMemory - usedDynamicMemory();
}

float usedDynamicMemoryF() {
	return usedDynamicMemory() / (float) TotalDynamicMemory;
}

float availableDynamicMemoryF() {
	return availableDynamicMemory() / (float) TotalDynamicMemory;
}
