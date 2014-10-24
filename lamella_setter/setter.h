//
// объект реализующий работу установщика
//

#ifndef _SETTER_H_
#define _SETTER_H_

#include "definitions.h"
#include "utils.h"

#include <LineDriver.h>
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
    int disable();
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
    enum { STATE_ERROR = -1,    //
           STATE_INIT = 0,      // поиск нуля
           STATE_WAITING,       // ожидание действий пользователя
           STATE_WORKING        // движение
         };

    CLamellaSetter(LiquidCrystalRus &lcd);
    ~CLamellaSetter();

    int init();
    int enable();
    int disable();

    int make();

public:
    // экран пользователя
    LiquidCrystalRus &lcd;

    // текущее состояние автомата
    int state;

    CStepper stp_x;
    CStepper stp_y;

private:


};

#endif //#ifndef _SETTER_H_
