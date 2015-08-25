#ifndef _TANK_IO_SERVICE_TWI_
#define _TANK_IO_SERVICE_TWI_

#include <twi/rpc/client_functions.h>
#include "tank_button.h"
#include "tank_led.h"

// inline array, indexed by below TankLedGroup enum values
#define TANK_LED_GROUPS ((LedGroup[]) { \
    whiteLeds, greenLeds, yellowLeds, redLeds, \
    middleLeds, leftLeds, rightLeds, \
    allLeds \
})

typedef enum {
    WHITE_LEDS, GREEN_LEDS, YELLOW_LEDS, RED_LEDS,
    MIDDLE_LEDS, LEFT_LEDS, RIGHT_LEDS,
    ALL_LEDS
} TankLedGroup;

typedef enum {
    TANK_IO_BUTTON_STATUS = 20,
    TANK_IO_SET_LEDS
} PROTOCOL_TANK_IO_SERVICE; // Start at 20

typedef struct ButtonStatusResult {
    uint8_t statusMask;
    uint8_t wasPressedMask;
    uint8_t wasReleasedMask;
} ButtonStatusResult;

typedef struct SetLedsParameters {
    uint16_t group; // enum TankLedGroup
    uint16_t mask;
} SetLedsParameters;

TWI_RPC_FUNCTION_NOARGS(tank_io_button_status, TANK_IO_BUTTON_STATUS, ButtonStatusResult)
TWI_RPC_FUNCTION_VOID(tank_io_set_leds, TANK_IO_SET_LEDS, SetLedsParameters)

#endif // _TANK_IO_SERVICE_TWI_
