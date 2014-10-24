//
// объект реализующий работу установщика
//

#ifndef _SETTER_H_
#define _SETTER_H_

#include "definitions.h"
#include "utils.h"

#include <LiquidCrystalExt.h>
#include <LiquidCrystalRus.h>

class CStepper
{
public:
    CStepper();
    CStepper(int stp_pin, int dir_pin, int ena_pin);
    ~CStepper();

    int init();
    int enable();
    void set_dir(int val);
    void step(unsigned long ms_delay);
    void make_steps(unsigned long count, unsigned long ms_delay);

    int stp_pin;
    int dir_pin;
    int ena_pin;
};

class CLamellaSetter
{
public:

    enum { STATE_MAINTENANCE = 0,   // обслуживание
           STATE_GOTOMAINTENANCE,   // перейти в режим обслуживания
           STATE_WAITING,       // ожидание клиента
           STATE_WORKING,       // ожидание действий клиента
           STATE_COOKING,       // приготовление
           STATE_CHANGE_GIVING, // выдача сдачи
           STATE_BYE,           // прощание
           STATE_ERROR };

    //CLamellaSetter( LiquidCrystalRus &lcd );
    CLamellaSetter();
    ~CLamellaSetter();

    int init();
    int enable();

public:
    // экран пользователя
    //LiquidCrystalRus &lcd;

    // текущее состояние автомата
    int state;

    CStepper stp_x;
    CStepper stp_y;

private:


};

#endif //#ifndef _SETTER_H_
