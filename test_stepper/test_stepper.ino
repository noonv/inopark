//
// тестирование шаговика
//
// http://robocraft.ru
//

#define SHOW_DEBUG_INFO 1

const int PULSEPin  = 13;
const int DIRpin    = 12;
const int ENPin     = 11;

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

void setup()
{
#if SHOW_DEBUG_INFO
    Serial.begin(57600);
#endif

    pinMode(DIRpin, OUTPUT);
    pinMode(PULSEPin, OUTPUT);
    pinMode(ENPin, OUTPUT);

    digitalWrite(DIRpin, LOW);
    digitalWrite(PULSEPin, LOW);
    digitalWrite(ENPin, LOW);

    debug_info(F("[i] Start..."));

    digitalWrite(DIRpin, HIGH);
    digitalWrite(ENPin, LOW);
}

void loop()
{
    digitalWrite(DIRpin, HIGH);
    for(int i=0; i<200; i++) {
        digitalWrite(PULSEPin, HIGH);
        delay(1);
        digitalWrite(PULSEPin, LOW);
        delay(1);
    }
    digitalWrite(DIRpin, LOW);
    for(int i=0; i<200; i++) {
        digitalWrite(PULSEPin, HIGH);
        delay(1);
        digitalWrite(PULSEPin, LOW);
        delay(1);
    }
}
