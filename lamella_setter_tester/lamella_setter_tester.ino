//
// Тестировщик выставки ламелей (управление двумя шаговыми двигателями)
//
// http://robocraft.ru
//

#include "setter.h"

Timings times;

#if USE_FOR_DEBUG
int led_pin = 13;
#endif


CLamellaSetter setter;

void setup()
{
#if SHOW_DEBUG_INFO
    Serial.begin(57600);
#endif

#if USE_FOR_DEBUG
    pinMode(led_pin, OUTPUT);
#endif

    setter.init();

    pinMode(STATE_SWITCH_PIN, INPUT);
    pinMode(STP_X_BUTTON_UP, INPUT);
    pinMode(STP_X_BUTTON_DOWN, INPUT);
    pinMode(STP_Y_BUTTON_UP, INPUT);
    pinMode(STP_X_BUTTON_DOWN, INPUT);

    debug_info(F("[i] Start..."));
}

int stepper_delay = 1;
int stepper_count = 1;

void loop()
{
#if 0 && USE_FOR_DEBUG
    // тестовый Blink
    if( is_Time(times.blink_time, 1000) ) {
        digitalWrite(led_pin, !digitalRead(led_pin));
        debug_info(F("[i] ======= BLINK ======="));
    }
#endif

#if 1
    setter.state = STATE_SLOW;
    if( debounce_digitalRead(STATE_SWITCH_PIN) == HIGH) {
        setter.state = STATE_FAST;
        digitalWrite(led_pin, HIGH);
        stepper_count = 67;
        debug_info(F("[i] FAST"));
    }
    else {
        digitalWrite(led_pin, LOW);
        stepper_count = 5;
        debug_info(F("[i] SLOW"));
    }

#if 1
    if( debounce_digitalRead(STP_X_BUTTON_UP) == HIGH) {
        while( debounce_digitalRead(STP_X_BUTTON_UP) != LOW) delay(1);
        setter.stp_x.set_dir(HIGH);
        setter.stp_x.make_steps(stepper_count, 1);
        debug_info(F("[i] X UP"));
    }
    else if(debounce_digitalRead(STP_X_BUTTON_DOWN) == HIGH) {
        while( debounce_digitalRead(STP_X_BUTTON_DOWN) != LOW) delay(1);

        setter.stp_x.set_dir(LOW);
        setter.stp_x.make_steps(stepper_count, 1);
        debug_info(F("[i] X DOWN"));
    }
    else {
        debug_info(F("[i] X NA"));
    }

    if( debounce_digitalRead(STP_Y_BUTTON_UP) == HIGH) {
        while( debounce_digitalRead(STP_Y_BUTTON_UP) != LOW) delay(1);
        setter.stp_y.set_dir(HIGH);
        setter.stp_y.make_steps(stepper_count, 1);
        debug_info(F("[i] Y UP"));
    }
    else if(debounce_digitalRead(STP_Y_BUTTON_DOWN) == HIGH) {
        while( debounce_digitalRead(STP_Y_BUTTON_DOWN) != LOW) delay(1);
        setter.stp_y.set_dir(LOW);
        setter.stp_y.make_steps(stepper_count, 1);
        debug_info(F("[i] Y DOWN"));
    }
    else {
        debug_info(F("[i] Y NA"));
    }
#endif

    //delay(200);
#endif

#if 0
    setter.stp_x.set_dir(HIGH);
    setter.stp_x.make_steps(200, 1);
    delay(1000);

    setter.stp_x.set_dir(LOW);
    setter.stp_x.make_steps(200, 1);

    delay(1000);
#endif

    //setter.make();

}
