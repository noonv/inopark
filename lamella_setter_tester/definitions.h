//
// определения
//

#ifndef _DEFINITIONS_H_
#define _DEFINITIONS_H_

#include "Arduino.h"

// выводить в Serial отладочную информацию
#define SHOW_DEBUG_INFO  1

// для отладки
#define USE_FOR_DEBUG 1

// параметры экранчика
#define LCD_WIDTH  16
#define LCD_HEIGHT 2

//LiquidCrystalRus lcd(8, 9, 10, 11, 12, 13);

// пины управления шаговиками
#define STP_X_STP_PIN 7
#define STP_X_DIR_PIN 6
//#define STP_X_ENA_PIN 10

#define STP_Y_STP_PIN 5
#define STP_Y_DIR_PIN 4
//#define STP_Y_ENA_PIN 7

// пины концевиков
#define END_X_PIN   A0
#define END_Y_PIN   A1
#define HOME_Y_PIN  A2

// пины органов управления
#define BUTTON_PIN      A3
#define ENCODER1_PIN    A4
#define ENCODER2_PIN    A5

// для хранения счётчиков времени срабатывания процедур
struct Timings
{
    unsigned long blink_time;           // тестовый blink
    unsigned long loop_time;            // работа loop-а

    Timings() {
        blink_time = 0;
        loop_time = 0;
    }
};

#endif //#ifndef _DEFINITIONS_H_
