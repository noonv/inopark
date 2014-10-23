#if 1
//
// программа управления сервоприводом CD420
// для вакуумного ввода установки ионно-плазменного напыления иннопарка
//
//
// http://robocraft.ru
//

#include <LineDriver.h>
#include <LiquidCrystalExt.h>
#include <LiquidCrystalRus.h>
#include <TimerOne.h>

#define SHOW_DEBUG_INFO 0

const int set_but   = 2;    // джампер
const int test_but  = 3;    // повернутся на угол а1
const int start_but = 7;    // начать экпозицию
const int up_but    = 4;    // увеличить время
const int down_but  = 5;    // уменьшить время
const int DIRpin    = A0;   //
const int PULSEPin  = 6;    //
const int ENPin     = A1;   //
int exptime = 10000;        // время выдержки

// минимальный период пульсации
const unsigned long min_period = 15;

// постоянное вращение:
unsigned long period = 100; // период пульсации (мкс)
unsigned long period_delta = 5; // шаг изменения периода (мкс)

// угловое движение:
unsigned long angle_period = 100; //* период пульсации (мкс)
unsigned long angle_period_delta = 5; // шаг изменения периода (мкс)
const unsigned long angle_step_pulses = 25; // число импульсов для одного шага (при выставке А0)

long angle_number_of_pulses = 2000; //* число ипульсов для поворота на определённый угол
unsigned long angle_delay_ms = 0; //* задерка (мс)
const unsigned long angle_delay_ms_step = 100; // шаг задержки (мс)

LiquidCrystalRus lcd(8, 9, 10, 11, 12, 13);

enum {SETUP_SPEED=1, SETUP_ANGLE, WORKING_SPEED, WORKING_ANGLE};

int state = WORKING_ANGLE;

enum {SET_SPEED=1, SET_A0, SET_A1, SET_DELAY};

int angle_setup_state = SET_SPEED;
bool in_change_value_mode = false;

enum {SERVO_A0=1, SERVO_GOTO_A1, SERVO_A1, SERVO_DELAY, SERVO_GOTO_A0};
int servo_state = SERVO_A0;

// двигатель может находиться в двух положениях: текущее положение считается нулевым (А0)
enum {POS_A0=0, POS_A1};
int servo_angle_position = POS_A0;

void debug_info(const char *str)
{
#if SHOW_DEBUG_INFO
    Serial.println(str);
#endif
}

void debug_info(const __FlashStringHelper *ifsh)
{
#if SHOW_DEBUG_INFO
    Serial.println(ifsh);
#endif
}

int free_Ram ()
{
    extern int __heap_start, *__brkval;
    int v;
    return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void setup() 
{
#if SHOW_DEBUG_INFO
    Serial.begin(57600);
#endif

    lcd.begin(8, 2);
    pinMode(DIRpin, OUTPUT);
    pinMode(PULSEPin, OUTPUT);
    pinMode(ENPin, OUTPUT);

    pinMode(set_but, INPUT);
    pinMode(test_but, INPUT);
    pinMode(start_but, INPUT);
    pinMode(up_but, INPUT);
    pinMode(down_but, INPUT);

    digitalWrite(DIRpin, LOW);
    digitalWrite(PULSEPin, LOW);
    digitalWrite(ENPin, LOW);

    Timer1.initialize(100); //
    Timer1.attachInterrupt(make_pulse);
    Timer1.stop();

#if 0
    lcd.clear();
    lcd.print(F("Designed")); //приветствие
    lcd.setCursor(0,1);
    lcd.print(F("by"));
    delay(2000);
    lcd.clear();
    lcd.print(F("Robo")); //приветствие
    lcd.setCursor(0,1);
    lcd.print(F("   Craft"));
    delay(2000);
    lcd.clear();
    lcd.print(F("made in"));
    lcd.setCursor(0,1);
    lcd.print(F("Lab409"));
    delay(2000);
#endif
    lcd.clear();

    //читаем параметры из ЕЕПРОМа

    debug_info(F("[i] Start..."));
} 

bool in_rotation = false;

volatile int pulsPinState = 0;

void make_pulse() {
    // функция для перещёлкивания пина
    digitalWrite(PULSEPin, pulsPinState);
    pulsPinState = pulsPinState?0:1;
}

volatile unsigned long pulse_counter = 0;

void make_angle_pulse() {
    // функция для перещёлкивания пина
    if(pulse_counter > 0) {
        digitalWrite(PULSEPin, pulsPinState);
        pulsPinState = !pulsPinState;
        pulse_counter--;
    }
}

void rotation(unsigned long _period) {
    if (_period == 0) {
        debug_info(F("[i] Stop rotation!"));

        Timer1.stop(); // останавливаем таймер
        digitalWrite(ENPin, LOW); // тормозим мотор
    }
    else {
#if SHOW_DEBUG_INFO
        Serial.println(F("Start rotation: "));
        Serial.println(period);
#endif
        digitalWrite(ENPin, HIGH);
        Timer1.setPeriod( _period );
        Timer1.attachInterrupt(make_pulse);
        Timer1.resume();//Timer1.start();
    }
}

float calc_rotation_speed(unsigned long _period)
{
    return (float)( ((2.0*10000.0)/(float)_period)*0.75 );
}

void make_steps(unsigned long step_count, unsigned long _period)
{
    digitalWrite(ENPin, HIGH);
    pulse_counter = step_count;
    Timer1.setPeriod( _period );
    Timer1.attachInterrupt(make_angle_pulse);
    Timer1.resume(); //Timer1.start();
}

void angle_go_to_a0()
{
    if(servo_angle_position != POS_A0) {
        if(angle_number_of_pulses >= 0) {
            digitalWrite(DIRpin, LOW);
        }
        else {
            digitalWrite(DIRpin, HIGH);
        }
        make_steps( abs(angle_number_of_pulses), angle_period);

        servo_angle_position = POS_A0;
    }
}

void angle_go_to_a1()
{
    if(servo_angle_position != POS_A1) {
        if(angle_number_of_pulses >= 0) {
            digitalWrite(DIRpin, HIGH);
        }
        else {
            digitalWrite(DIRpin, LOW);
        }
        make_steps( abs(angle_number_of_pulses), angle_period);

        servo_angle_position = POS_A1;
    }
}

volatile long angle_delay_counter = 0;

void make_delay()
{
    // организуем задержку
    angle_delay_counter++;
}

void servo_set_direction()
{
    if(servo_angle_position == POS_A0) {
        if(angle_number_of_pulses >= 0) {
            digitalWrite(DIRpin, HIGH);
        }
        else {
            digitalWrite(DIRpin, LOW);
        }
    }
    else if(servo_angle_position == POS_A1) {
        if(angle_number_of_pulses >= 0) {
            digitalWrite(DIRpin, LOW);
        }
        else {
            digitalWrite(DIRpin, HIGH);
        }
    }
}

void angle_go()
{
    digitalWrite(ENPin, HIGH);

    servo_set_direction();

    pulse_counter = abs(angle_number_of_pulses);
    Timer1.setPeriod(angle_period);
    Timer1.attachInterrupt(make_angle_pulse);
    Timer1.resume(); //Timer1.start();
}

void angle_stop()
{
    debug_info(F("[i] Stop!"));

    Timer1.stop(); // останавливаем таймер
    digitalWrite(ENPin, LOW); // тормозим мотор
}

bool print_info = true;

int servo_counter = 0;

void loop() 
{
    switch (state) {
    case WORKING_SPEED:
        debug_info(F("[i] Speed..."));

        if(print_info) {
            lcd.clear();
            lcd.print(F("Стоп врщ"));
            lcd.setCursor(0,1);
            lcd.print( calc_rotation_speed(period) );
            print_info = false;
        }

        if (digitalRead(set_but) == 0) {
            // если установлен джампер set и мы находились в режиме WORKING_SPEED
            state = SETUP_SPEED; // переходим в настройку скорости
            rotation(0);
            print_info = true;
            return;
        }
        if ((digitalRead(up_but) == 0) && (digitalRead(down_but) == 0)) {
            // если нажаты обе кнопки
            state = WORKING_ANGLE;   // переходим в WORKING_ANGLE
            rotation(0);
            print_info = true;
            delay(500);
            return;
        }

        if (digitalRead(start_but) == 0) {
            if(!in_rotation) {
                // пишем "вращение", демонстрируем выбранную скорость

                lcd.clear();
                lcd.print(F("Вращаем"));
                lcd.setCursor(0,1);
                lcd.print( calc_rotation_speed(period) );

                // запускаем вращение с заданой скоростью
                rotation(period);
                in_rotation = true;
                delay(500);

                return;
            }
            else {
                // останавливаем вращение пишем "Стоп"

                rotation(0);
                in_rotation = false;

                lcd.clear();
                lcd.print(F("Стоп врщ"));
                lcd.setCursor(0,1);
                lcd.print( calc_rotation_speed(period) );

                delay(500);
                return;
            }
        }

        break;
        /////////////////////////////////////////////
    case SETUP_SPEED:
        debug_info(F("[i] Setup speed..."));

        if( digitalRead(set_but) == 1 ) {
            // если снят джампер set и мы находились в режиме WORKING_SPEED
            state = WORKING_SPEED; // переходим в настройку скорости
            rotation(0);
            print_info = true;
            return;
        }
        // настраиваем скорость кнопками up/down
        if( digitalRead(up_but) == 0 ) {
            period -= period_delta;
            print_info = true;
            delay(100);
        }
        if( digitalRead(down_but) == 0 ) {
            period += period_delta;
            print_info = true;
            delay(100);
        }

        // проверка на минимальное значение
        if(period < min_period) {
            period = min_period;
        }

#if SHOW_DEBUG_INFO
        Serial.print(F("period: "));
        Serial.println(period);
#endif

        if(print_info) {
            lcd.clear();
            lcd.print(F("Скор врщ"));
            lcd.setCursor(0,1);
            lcd.print( calc_rotation_speed(period) );
            delay(200);
            print_info = false;
        }

        break;
        /////////////////////////////////////////////
    case WORKING_ANGLE:
        debug_info(F("[i] Angle..."));

        if( print_info ) {
            lcd.clear();
            lcd.print(F("Стоп угл"));
            //lcd.setCursor(0,1);
            //lcd.print(F("Стоп угл"));
            print_info = false;
        }

        if( digitalRead(set_but) == 0 ) {
            // если установлен джампер set и мы находились в режиме WORKING_ANGLE
            state = SETUP_ANGLE; // переходим в настройку угла
            angle_setup_state = SET_SPEED;
            angle_stop();
            print_info = true;
            return;
        }
        if( (digitalRead(up_but) == 0) && (digitalRead(down_but) == 0) ) {
            // если нажаты обе кнопки
            state = WORKING_SPEED; // переходим в WORKING_SPEED
            rotation(0); // остановка движения
            print_info = true;
            delay(500);
            return;
        }
        if( digitalRead(start_but) == 0 ) {
            // запускаем отработку экспозиции

            if(!in_rotation) {
                // пишем "вращение", демонстрируем выбранную скорость

                lcd.clear();
                lcd.print(F("Идём угл"));
                //lcd.setCursor(0,1);
                //lcd.print(  );

                servo_counter = 0;
                servo_state = SERVO_A0;
                servo_angle_position = POS_A0;

                in_rotation = true;
                delay(500);

                return;
            }
            else {
                // останавливаем вращение пишем "Стоп"

                angle_stop();
                angle_go_to_a0();

                in_rotation = false;

                lcd.clear();
                lcd.print(F("Стоп угл"));
                //lcd.setCursor(0,1);
                //lcd.print(  );

                delay(500);
                return;
            }
        }

        if(in_rotation) {
            switch (servo_state) {
            case SERVO_A0:
                debug_info(F("[i] SERVO_A0"));

                servo_angle_position = POS_A0;

                Timer1.stop();

                servo_counter++;
                if(servo_counter >= 2) {
                    angle_stop();
                    in_rotation = false;

                    lcd.clear();
                    lcd.print(F("стоп угл"));
                    return;
                }

                angle_go();

                servo_state = SERVO_GOTO_A1;
                return;

                break;
            case SERVO_GOTO_A1:
                debug_info(F("[i] SERVO_GOTO_A1"));
                if(pulse_counter == 0) {
                    Timer1.stop();
                    servo_angle_position = POS_A1;
                    servo_state = SERVO_A1;
                    return;
                }
                break;
            case SERVO_A1:
                debug_info(F("[i] SERVO_A1"));

                servo_angle_position = POS_A1;

                angle_delay_counter = 0;

                // организуем задрежку
                Timer1.stop();
                Timer1.setPeriod(1000);
                Timer1.attachInterrupt(make_delay);
                Timer1.resume(); //Timer1.start();

                servo_state = SERVO_DELAY;
                return;

                break;
            case SERVO_DELAY:
                debug_info(F("[i] SERVO_DELAY"));

                if(angle_delay_counter >= angle_delay_ms) {
                    Timer1.stop();

                    // идём в а0
                    angle_go();

                    servo_state = SERVO_GOTO_A0;
                    return;
                }
                break;
            case SERVO_GOTO_A0:
                debug_info(F("[i] SERVO_GOTO_A0"));

                if(pulse_counter == 0) {
                    Timer1.stop();
                    servo_angle_position = POS_A0;
                    servo_state = SERVO_A0;
                    return;
                }
                break;
            default:
                break;
            }

#if SHOW_DEBUG_INFO
            Serial.print(F("angle_delay: "));
            Serial.println(angle_delay_counter);
#endif
        }

        break;
        ////////////////////////////////////////////
    case SETUP_ANGLE:
        debug_info(F("[i] Setup angle..."));

        if( digitalRead(set_but) == 1 ) {
            // если снят джампер set и мы находились в режиме WORKING_SPEED
            state = WORKING_ANGLE; // переходим в настройку скорости
            angle_stop();
            print_info = true;
            return;
        }

        // циклично перещёлкиваем кнопками up/down
        // * настройку скорости
        // * угол старта
        // * угол экспозиции
        // * время экспозиции
        // при нажатии на start_but переходим в режим настройки параметра кнопками up/down
        // при повторном нажатии выходим в режим выбора настроек, сохраняем (в ЕЕПРОМ)

        if( digitalRead(start_but) == 0 ) {
            in_change_value_mode = !in_change_value_mode;
            print_info = true;
            delay(300);
            return;
        }

        if(!in_change_value_mode) {
            if( digitalRead(up_but) == 0 ) {
                angle_setup_state++;
                print_info = true;
                delay(100);
            }
            if( digitalRead(down_but) == 0 ) {
                angle_setup_state--;
                print_info = true;
                delay(100);
            }
        }

        if( angle_setup_state < SET_SPEED ) {
            angle_setup_state = SET_DELAY;
        }
        if( angle_setup_state > SET_DELAY ) {
            angle_setup_state = SET_SPEED;
        }


        switch (angle_setup_state) {
        case SET_SPEED:
            debug_info(F("[i] Set angle speed..."));

            if(in_change_value_mode) {
                if( digitalRead(up_but) == 0 ) {
                    angle_period -= angle_period_delta;
                    print_info = true;
                    delay(100);
                }
                if( digitalRead(down_but) == 0 ) {
                    angle_period += angle_period_delta;
                    print_info = true;
                    delay(100);
                }

                // проверка на минимальное значение
                if(angle_period < min_period) {
                    angle_period = min_period;
                }

#if SHOW_DEBUG_INFO
                Serial.println(F("angle_period: "));
                Serial.println(angle_period);
#endif
            }

            if(print_info) {
                lcd.clear();
                if(in_change_value_mode)
                    lcd.print(F("Скорост*"));
                else
                    lcd.print(F("Скорость"));
                lcd.setCursor(0,1);
                lcd.print(calc_rotation_speed(angle_period));
                delay(200);
                print_info = false;
            }

            break;
        case SET_A0:
            debug_info(F("[i] Set angle a0..."));

            // идём в положение А0
            angle_go_to_a0();

            if(in_change_value_mode) {
                if( digitalRead(up_but) == 0 ) {

                    digitalWrite(DIRpin, HIGH);
                    make_steps(angle_step_pulses, angle_period);
                    servo_angle_position = POS_A0;
                    print_info = true;
                    delay(50);
                }
                if( digitalRead(down_but) == 0 ) {

                    digitalWrite(DIRpin, LOW);
                    make_steps(angle_step_pulses, angle_period);
                    servo_angle_position = POS_A0;
                    print_info = true;
                    delay(50);
                }

#if SHOW_DEBUG_INFO
                Serial.println(F("pulse_counter: "));
                Serial.println(pulse_counter);
#endif
            }

            if(print_info) {
                lcd.clear();
                if(in_change_value_mode)
                    lcd.print(F("Нач угл*"));
                else
                    lcd.print(F("Нач угол"));
                //lcd.setCursor(0,1);
                //lcd.print();
                delay(200);
                print_info = false;
            }

            break;
        case SET_A1:
            debug_info(F("[i] Set angle a1..."));

            if(in_change_value_mode) {

                // идём в положение А1
                angle_go_to_a1();

                if( digitalRead(up_but) == 0 ) {

                    angle_number_of_pulses += angle_step_pulses;
                    digitalWrite(DIRpin, HIGH);
                    make_steps(angle_step_pulses, angle_period);
                    servo_angle_position = POS_A1;
                    print_info = true;
                    delay(10);
                }
                if( digitalRead(down_but) == 0 ) {

                    angle_number_of_pulses -= angle_step_pulses;
                    digitalWrite(DIRpin, LOW);
                    make_steps(angle_step_pulses, angle_period);
                    servo_angle_position = POS_A1;
                    print_info = true;
                    delay(10);

                }

#if SHOW_DEBUG_INFO
                Serial.println(F("angle_number_of_pulses: "));
                Serial.println(angle_number_of_pulses);
#endif
            }
            else {
                // идём в положение А0
                angle_go_to_a0();
            }

            if(print_info) {
                lcd.clear();
                if(in_change_value_mode)
                    lcd.print(F("Кон угл*"));
                else
                    lcd.print(F("Кон угол"));
                lcd.setCursor(0,1);
                lcd.print(angle_number_of_pulses);
                delay(200);
                print_info = false;
            }

            break;
        case SET_DELAY:
            debug_info(F("[i] Set angle delay..."));

            if(in_change_value_mode) {
                if( digitalRead(up_but) == 0 ) {

                    angle_delay_ms += angle_delay_ms_step;

                    print_info = true;
                    delay(100);
                }
                if( digitalRead(down_but) == 0 ) {

                    angle_delay_ms -= angle_delay_ms_step;

                    print_info = true;
                    delay(100);
                }

#if SHOW_DEBUG_INFO
                Serial.println(F("angle_delay_ms: "));
                Serial.println(angle_delay_ms);
#endif
            }

            if(print_info) {
                lcd.clear();
                if(in_change_value_mode)
                    lcd.print(F("Задержк*"));
                else
                    lcd.print(F("Задержка"));
                lcd.setCursor(0,1);
                lcd.print(angle_delay_ms);
                delay(200);
                print_info = false;
            }

            break;
        default:
            break;
        }

        break;
        ///////////////////////////////////////////
    default:
        debug_info(F("[!] Unknown mode!"));
        break;

    }

#if 0 && SHOW_DEBUG_INFO
        Serial.print(F("[i][Free RAM] "));
        Serial.println(free_Ram());
#endif

    delay(10);
}
#endif


#if 0
//
// test
//

#include <TimerOne.h>

void setup()
{
    Serial.begin(57600);

    Timer1.initialize();

    pinMode(6, OUTPUT);
    Timer1.setPeriod(100);
    Timer1.attachInterrupt(make_pulse);
    Timer1.stop();
}

volatile int pulsPinState = 0;

void make_pulse() {
    // функция для перещёлкивания пина
    digitalWrite(6, pulsPinState);
    pulsPinState = pulsPinState?0:1;

    //digitalWrite(6, !digitalRead(6));
}

void loop()
{
    Serial.println("[i] Start...");
    Timer1.resume();
    //Timer1.start();
    delay(1000);
    Serial.println("[i] Stop...");
    Timer1.stop();
    delay(1000);
    // пока мы тут что-нибудь делаем, светодиоды мигают сами по себе
}
#endif
