/*
 * STSPIN820 - Stepper Motor Driver 
 */
#include "STSPIN820.h"

/*
 * Microstepping resolution truth table (Page 13 of STSPIN820 pdf)
 * 0bMS3,MS2,MS1 for 1,2,4,8,16,32,128,256 microsteps
 */
const uint8_t STSPIN820::MS_TABLE[] = {0b000, 0b001, 0b010, 0b011, 0b100, 0b101, 0b110, 0b111};

/*
 * Basic connection: only DIR, STEP are connected.
 * Microstepping controls should be hardwired.
 */
STSPIN820::STSPIN820(short steps, short dir_pin, short step_pin)
:BasicStepperDriver(steps, dir_pin, step_pin)
{}

STSPIN820::STSPIN820(short steps, short dir_pin, short step_pin, short enable_pin)
:BasicStepperDriver(steps, dir_pin, step_pin, enable_pin)
{}

/*
 * Fully wired.
 * All the necessary control pins for STSPIN820 are connected.
 */
STSPIN820::STSPIN820(short steps, short dir_pin, short step_pin, short modes1_pin, short modes2_pin, short modes3_pin)
:BasicStepperDriver(steps, dir_pin, step_pin),
    modes1_pin(modes1_pin), modes2_pin(modes2_pin), modes3_pin(modes3_pin)
{}

STSPIN820::STSPIN820(short steps, short dir_pin, short step_pin, short enable_pin, short modes1_pin, short modes2_pin, short modes3_pin)
:BasicStepperDriver(steps, dir_pin, step_pin, enable_pin),
modes1_pin(modes1_pin), modes2_pin(modes2_pin), modes3_pin(modes3_pin)
{}

void STSPIN820::begin(float rpm, short microsteps){
    BasicStepperDriver::begin(rpm, microsteps);

    if (!IS_CONNECTED(modes1_pin) || !IS_CONNECTED(modes2_pin) || !IS_CONNECTED(modes3_pin)){
        return;
    }

    pinMode(modes1_pin, OUTPUT);
    pinMode(modes2_pin, OUTPUT);
    pinMode(modes3_pin, OUTPUT);
}

/*
 * Set microstepping mode (1:divisor)
 * Allowed ranges for STSPIN820 are 1:1 to 1:256
 * If the control pins are not connected, we recalculate the timing only
 */
short STSPIN820::setMicrostep(short microsteps){
    BasicStepperDriver::setMicrostep(microsteps);

    if (!IS_CONNECTED(modes1_pin) || !IS_CONNECTED(modes2_pin) || !IS_CONNECTED(modes3_pin)){
        return this->microsteps;
    }

    const uint8_t* ms_table = getMicrostepTable();
    size_t ms_table_size = getMicrostepTableSize();

    unsigned short i = 0;
    while (i < ms_table_size){
        if (this->microsteps & (1<<i)){
            uint8_t mask = ms_table[i];
            digitalWrite(modes3_pin, mask & 4);
            digitalWrite(modes2_pin, mask & 2);
            digitalWrite(modes1_pin, mask & 1);
            break;
        }
        i++;
    }
    return this->microsteps;
}

const uint8_t* STSPIN820::getMicrostepTable(){
    return STSPIN820::MS_TABLE;
}

size_t STSPIN820::getMicrostepTableSize(){
    return sizeof(STSPIN820::MS_TABLE);
}

short STSPIN820::getMaxMicrostep(){
    return STSPIN820::MAX_MICROSTEP;
}
