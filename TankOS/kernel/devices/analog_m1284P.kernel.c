/*
 * analog_m1284P.kernel
 *
 * Created: 28.04.2012 11:29:04
 *  Author: Anton
 */

#include <kernel_init.h>
#include "analog_m1284P.h"
#include "port_m1284P.h"

#define INIT_ANALOG_INPUT_M1284P(number)	\
	INIT_ANALOG_INPUT(Analog##number, PinA##number, number)

DEFINE_ANALOG_INPUT_IMPL(Analog0)
DEFINE_ANALOG_INPUT_IMPL(Analog1)
DEFINE_ANALOG_INPUT_IMPL(Analog2)
DEFINE_ANALOG_INPUT_IMPL(Analog3)
DEFINE_ANALOG_INPUT_IMPL(Analog4)
DEFINE_ANALOG_INPUT_IMPL(Analog5)
DEFINE_ANALOG_INPUT_IMPL(Analog6)
DEFINE_ANALOG_INPUT_IMPL(Analog7)

void init_analog_m1284P() {
	INIT_ANALOG_INPUT_M1284P(0)
	INIT_ANALOG_INPUT_M1284P(1)
	INIT_ANALOG_INPUT_M1284P(2)
	INIT_ANALOG_INPUT_M1284P(3)
	INIT_ANALOG_INPUT_M1284P(4)
	INIT_ANALOG_INPUT_M1284P(5)
	INIT_ANALOG_INPUT_M1284P(6)
	INIT_ANALOG_INPUT_M1284P(7)
}
KERNEL_INIT(init_analog_m1284P)
