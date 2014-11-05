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

    debug_info(F("[i] Start..."));
}

void loop()
{
#if USE_FOR_DEBUG
    // тестовый Blink
    if( is_Time(times.blink_time, 1000) ) {
        digitalWrite(led_pin, !digitalRead(led_pin));
        debug_info(F("[i] ======= BLINK ======="));
    }
#endif

#if 0
    setter.stp_x.set_dir(HIGH);
    setter.stp_x.make_steps(200, 1);

    //setter.stp_x.set_dir(LOW);
    setter.stp_x.make_steps(200, 1);
#endif

    setter.make();

}
