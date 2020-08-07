/*
 *   File:   stepper.c
 *   Author: Matt
 *
 *   Created on 30 July 2018, 06:37
 * 
 *   This is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 2 of the License, or
 *   (at your option) any later version.
 *   This software is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *   You should have received a copy of the GNU General Public License
 *   along with this software.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "project.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "stepper.h"
#include "iopins.h"
#include "timer.h"
#include "util.h"

static uint8_t _g_step_phase;
static uint16_t _g_delay;
static uint16_t _g_pwm;
static uint8_t _g_last_dir;
static uint8_t _g_timer_reload;
static uint8_t _g_cont_step_dir;
static bool _g_contstep_running;

static void stepper_shift_phase(uint8_t dir);
static void stepper_step(uint8_t phase);
static void stepper_single_step(uint8_t phase);
static void stepper_update_duty(uint8_t duty);

#define ASSERT_ENABLE_A() if (_g_pwm < 255) TCCR2A |= _BV(COM2B1); else IO_HIGH(ENA)
#define ASSERT_ENABLE_B() if (_g_pwm < 255) TCCR2A |= _BV(COM2A1); else IO_HIGH(ENB)

#define DEASSERT_ENABLE_A() TCCR2A &= ~_BV(COM2B1); IO_LOW(ENA)
#define DEASSERT_ENABLE_B() TCCR2A &= ~_BV(COM2A1); IO_LOW(ENB)

ISR(TIMER0_OVF_vect)
{
    if (!_g_contstep_running)
    {
        timer0_stop();
        DEASSERT_ENABLE_A();
        DEASSERT_ENABLE_B();
    }

    stepper_shift_phase(_g_cont_step_dir);
    stepper_step(_g_step_phase);
    timer0_reload(_g_timer_reload);
}

void stepper_init(uint16_t delay, uint8_t pwm_duty)
{
    IO_OUTPUT(ENA);
    IO_OUTPUT(ENB);
    IO_OUTPUT(DIRA);
    IO_OUTPUT(DIRB);

    _g_step_phase = 3;
    
    _g_last_dir = STEP_FORWARD;
    _g_contstep_running = false;

    stepper_set_delay(delay);
    stepper_set_duty(pwm_duty);
    timer0_init();

    // set none-inverting mode
    TCCR2A |= _BV(WGM21) | _BV(WGM20);

    // set prescaler to 8 and starts PWM
    TCCR2B |= _BV(CS20);
}

void stepper_set_duty(uint8_t duty)
{
    _g_pwm = duty;

    if (_g_contstep_running)
        stepper_update_duty(_g_pwm);
}

static void stepper_update_duty(uint8_t duty)
{
    if (duty == 255)
    {
        IO_HIGH(ENA);
        IO_HIGH(ENB);

        TCCR2A &= ~_BV(COM2B1);
        TCCR2A &= ~_BV(COM2A1); 
    }
    else
    {
        IO_LOW(ENA);
        IO_LOW(ENB);

        TCCR2A |= _BV(COM2B1);
        TCCR2A |= _BV(COM2A1); 

        OCR2A = duty;
        OCR2B = duty;
    }
}

void stepper_set_delay(uint16_t delay)
{
    uint16_t cycles_per_ms = (255 * 100) / 164;
    uint16_t cycles_for_period = cycles_per_ms * delay;

    cycles_for_period /= 100;
    _g_timer_reload = (0x100 - (uint8_t)cycles_for_period);
    _g_delay = delay;
}

uint8_t stepper_get_phase(void)
{
    return _g_step_phase;
}

static void stepper_shift_phase(uint8_t dir)
{
    if (dir == STEP_FORWARD)
    {
        if (_g_step_phase == 3)
            _g_step_phase = 0;
        else
            _g_step_phase++;
    }
    
    if (dir == STEP_REVERSE)
    {
        if (_g_step_phase == 0)
            _g_step_phase = 3;
        else
            _g_step_phase--;
    }
}

bool stepper_move_fixed_count(uint8_t dir, uint16_t steps)
{
    if (_g_contstep_running)
        return false;

    stepper_update_duty(_g_pwm);

    ASSERT_ENABLE_A();
    ASSERT_ENABLE_B();

#ifdef _TWOSTEP_
    steps *= 2;
#endif

    for (uint16_t i = 0; i < steps; i++)
    {
        if (dir == _g_last_dir)
            stepper_shift_phase(dir);

        _g_last_dir = dir;

        stepper_step(_g_step_phase);

        for (uint16_t j = 0; j < _g_delay; j++)
            _delay_us(100);
    }

    DEASSERT_ENABLE_A();
    DEASSERT_ENABLE_B();

    return true;
}

bool stepper_move_single_step(uint8_t dir)
{
    if (_g_contstep_running)
        return false;

#ifdef _TWOSTEP_
    steps *= 2;
#endif

    stepper_shift_phase(dir);
    stepper_single_step(_g_step_phase);

    return true;
}

void stepper_start_continous(uint8_t dir)
{
    _g_cont_step_dir = dir;
    _g_contstep_running = true;

    stepper_update_duty(_g_pwm);

    ASSERT_ENABLE_A();
    ASSERT_ENABLE_B();

    if (dir == _g_last_dir)
        stepper_shift_phase(dir);

    _g_last_dir = dir;

    stepper_step(_g_step_phase);


    timer0_reload(_g_timer_reload);
    timer0_start();
}

void stepper_stop_continous(void)
{
    _g_contstep_running = false;
}

static void stepper_step(uint8_t phase)
{
    switch (phase)
    {
        case 0:
            IO_LOW(DIRA);
            IO_HIGH(DIRB);
            break;
        case 1:
            IO_HIGH(DIRA);
            IO_HIGH(DIRB);
            break;
        case 2:
            IO_HIGH(DIRA);
            IO_LOW(DIRB);
            break;
        case 3:
            IO_LOW(DIRA);
            IO_LOW(DIRB);
            break;
    }
}

static void stepper_single_step(uint8_t phase)
{
    switch (phase)
    {
        case 0:
            IO_HIGH(DIRA);
            IO_HIGH(ENA);
            break;
        case 1:
            IO_LOW(DIRB);
            IO_HIGH(ENB);
            break;
        case 2:
            IO_LOW(DIRA);
            IO_HIGH(ENA);
            break;
        case 3:
            IO_HIGH(DIRB);
            IO_HIGH(ENB);
            break;
    }

    _delay_ms(50);

    IO_LOW(ENA);
    IO_LOW(ENB);

    _delay_ms(15);
}