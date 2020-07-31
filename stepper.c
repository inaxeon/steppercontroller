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
#include <avr/io.h>

#include "stepper.h"
#include "iopins.h"
#include "util.h"

static uint8_t _g_step_phase;
static uint16_t _g_delay;

static void stepper_step(uint8_t phase);

void stepper_init(uint16_t delay)
{
    // IO_OUTPUT(ENA);
    // IO_OUTPUT(ENB);
    // IO_OUTPUT(DIRA);
    // IO_OUTPUT(DIRB);

    _g_step_phase = 3;
    _g_delay = delay;
}

void stepper_set_delay(uint16_t delay)
{
    _g_delay = delay;
}

void stepper_move_fixed_count(uint8_t dir, uint16_t steps)
{
    uint16_t i;

    IO_OUTPUT(ENA);
    IO_OUTPUT(ENB);
    IO_OUTPUT(DIRA);
    IO_OUTPUT(DIRB);

    IO_LOW(ENA);

    for (i = 0; i < steps; i++)
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

        stepper_step(_g_step_phase);
    }

    IO_HIGH(ENA);

    IO_INPUT(ENA);
    IO_INPUT(ENB);
    IO_INPUT(DIRA);
    IO_INPUT(DIRB);
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

    for (uint16_t i = 0; i < _g_delay; i++)
        _delay_ms(1);
}