#ifndef __MOTOR_STEP__
#define __MOTOR_STEP__

#include <tank_os_common.h>
#include "port.h"
#include "motor.h"

DEFINE_HANDLE(StepMotor);
#define PinStepMotor 9

typedef enum {
    StepMotorNormal = 0,
    StepMotorInverseDir = _BV(0),
    StepMotorInverseEnable = _BV(1)
} StepMotorFlags;

typedef int32_t pos_t; // Position in steps

StepMotor newStepMotor(Pin step, Pin dir, Pin enable, pos_t stepsPerTurn, StepMotorFlags flags);
StepMotor destroyStepMotor(StepMotor motor);
BOOL stepMotorValid(StepMotor motor);

// If enable-Pin given in newStepMotor
void enableStepMotor(StepMotor motor);
void disableStepMotor(StepMotor motor);
BOOL stepMotorEnabled(StepMotor motor);

// Sets the maximum frequency for movements
BOOL stepMotorSetMaxFrequency(StepMotor motor, uint16_t stepsPerSecond); // Return FALSE if frequency is too high
uint16_t stepMotorGetMaxFrequency(StepMotor motor);

void stepMotorStep(StepMotor motor, pos_t numSteps);
void stepMotorRotate(StepMotor motor, MotorDirection dir);
void stepMotorStop(StepMotor motor);

pos_t stepMotorPosition(StepMotor motor);

// Only set once before creating first motor!
extern uint16_t motor_step_ticks_per_second;
void motor_step_tick();

#endif // __MOTOR_STEP__
