//
// объект реализующий работу установщика
//

#include "setter.h"

CStepper::CStepper():
    stp_pin(-1), dir_pin(-1), ena_pin(-1)
{

}

CStepper::CStepper(int _stp_pin, int _dir_pin, int _ena_pin):
    stp_pin(_stp_pin), dir_pin(_dir_pin), ena_pin(_ena_pin)
{

}

CStepper::~CStepper()
{

}

int CStepper::init()
{
    pinMode(stp_pin, OUTPUT);
    pinMode(dir_pin, OUTPUT);
    if(ena_pin != -1) {
        pinMode(ena_pin, OUTPUT);
    }

    digitalWrite(stp_pin, LOW);
    digitalWrite(dir_pin, LOW);
    if(ena_pin != -1) {
        digitalWrite(ena_pin, HIGH);
    }

    return 0;
}

int CStepper::enable()
{
    if(ena_pin != -1) {
        digitalWrite(ena_pin, LOW);
    }
    return 0;
}

int CStepper::disable()
{
    if(ena_pin != -1) {
        digitalWrite(ena_pin, HIGH);
    }
    return 0;
}

void CStepper::set_dir(int val)
{
    digitalWrite(dir_pin, val);
}

void CStepper::step(unsigned long ms_delay)
{
    digitalWrite(stp_pin, HIGH);
    delay(ms_delay);
    digitalWrite(stp_pin, LOW);
    delay(ms_delay);
}

void CStepper::make_steps(unsigned long count, unsigned long ms_delay)
{
    for(unsigned long i=0; i<count; i++) {
        step(ms_delay);
    }
}



CLamellaSetter::CLamellaSetter(LiquidCrystalRus &_lcd):
    lcd(_lcd)
{
    state = STATE_INIT;
}

CLamellaSetter::~CLamellaSetter()
{

}

int CLamellaSetter::init()
{
    debug_info(F("[i] init..."));

    stp_x.stp_pin = STP_X_STP_PIN;
    stp_x.dir_pin = STP_X_DIR_PIN;
    //stp_x.ena_pin = STP_X_ENA_PIN;

    stp_y.stp_pin = STP_Y_STP_PIN;
    stp_y.dir_pin = STP_Y_DIR_PIN;
    //stp_y.ena_pin = STP_Y_ENA_PIN;

    stp_x.init();
    stp_y.init();

    return 0;
}

int CLamellaSetter::enable()
{
    stp_x.enable();
    stp_y.enable();
    return 0;
}

int CLamellaSetter::disable()
{
    stp_x.disable();
    stp_y.disable();
    return 0;
}

int CLamellaSetter::make()
{
#if SHOW_DEBUG_INFO
    Serial.print(F("[i] Make state: "));
    Serial.println(state);
#endif

    switch (state) {
    case STATE_ERROR:
        break;
    case STATE_INIT:
        break;
    case STATE_WAITING:
        break;
    default:
        break;
    }
    return 0;
}



