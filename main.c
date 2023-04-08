/*
 *   File:   main.c
 *   Author: Matt
 *
 *   Created on 11 May 2018, 12:13
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
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "iopins.h"
#include "config.h"
#include "cmd.h"
#include "usart.h"
#include "timeout.h"
#include "util.h"
#include "timer.h"
#include "stepper.h"

#define F_CHANGE_DIR        0x01
#define F_UP                0x02
#define F_DOWN              0x04

typedef struct
{
    sys_config_t *config;
    uint8_t runflags;
    uint8_t last_portb;
    uint8_t last_portc;
    uint8_t last_portd;
} sys_runstate_t;

sys_config_t _g_cfg;
sys_runstate_t _g_rs;

FILE uart_str = FDEV_SETUP_STREAM(print_char, NULL, _FDEV_SETUP_RW);

static void io_init(void);
static void port_change_interrupt(void);
//static void test_1s(void *param);

ISR(PCINT0_vect)
{
    port_change_interrupt();
}

ISR(PCINT1_vect)
{
    port_change_interrupt();
}

ISR(PCINT2_vect)
{
    port_change_interrupt();
}

static void port_change_interrupt(void)
{
    if (!(_g_rs.last_portc & _BV(PC1)) && (PINC & _BV(PC1)))
    {
        _g_rs.runflags |= F_CHANGE_DIR;
        PCMSK1 &= ~_BV(PCINT9); // Disable interrupt
    }

    if (!(_g_rs.last_portb & _BV(PB0)) && (PINB & _BV(PB0)))
    {
        _g_rs.runflags |= F_UP;
        PCMSK0 &= ~_BV(PCINT0); // Disable interrupt
    }

    if (!(_g_rs.last_portd & _BV(PD5)) && (PIND & _BV(PD5)))
    {
        _g_rs.runflags |= F_DOWN;
        PCMSK2 &= ~_BV(PCINT21); // Disable interrupt
    }

    _g_rs.last_portb = PINB;
    _g_rs.last_portc = PINC;
    _g_rs.last_portd = PIND;
}

int main(void)
{
    sys_runstate_t *rs = &_g_rs;
    sys_config_t *config = &_g_cfg;
    rs->config = config;
    _g_rs.runflags = 0;

    io_init();
    g_irq_enable();

    usart1_open(USART_CONT_RX, (((F_CPU / UART1_BAUD) / 16) - 1)); // Console

    stdout = &uart_str;

    printf("\r\n");

    load_configuration(config);

    printf("Starting up...\r\n");

    CLRWDT();

    stepper_init(config->step_delay_01ms, config->pwm_duty);
    //timeout_init();

    //timeout_create(1000, true, true, &test_1s, (void *)rs);

    if (config->start_mode == 1)
        stepper_start_continous(STEP_FORWARD, 0);

    if (config->start_mode == 2)
        stepper_start_continous(STEP_REVERSE, 0);

    // Idle loop
    for (;;)
    {
        //timeout_check();
        cmd_process(config);
        CLRWDT();

        if (rs->runflags & F_CHANGE_DIR)
        {
            rs->runflags &= ~F_CHANGE_DIR;
            stepper_change_dir();
            _delay_ms(250);
            _delay_ms(250);
            PCIFR |= _BV(PCIE1);
            PCMSK1 |= _BV(PCINT9);
        }

        if (rs->runflags & F_UP)
        {
            rs->runflags &= ~F_UP;
            stepper_move_fixed_count(STEP_REVERSE, 3250);
            PCIFR |= _BV(PCIE0);
            PCMSK0 |= _BV(PCINT0);
        }

        if (rs->runflags & F_DOWN)
        {
            rs->runflags &= ~F_DOWN;
            stepper_move_fixed_count(STEP_FORWARD, 3250);
            PCIFR |= _BV(PCIE2);
            PCMSK2 |= _BV(PCINT21);
        }
    }
}


static void io_init(void)
{
#ifdef _LEONARDO_
    // Disable USB, because the bootloader has probably left it on
    USBCON &= ~_BV(USBE);
#endif

    _g_rs.last_portc = PINC;
    _g_rs.last_portb = PINB;
    _g_rs.last_portd = PIND;

    PCICR |= _BV(PCIE0);
    PCICR |= _BV(PCIE1);
    PCICR |= _BV(PCIE2);

    // Up button
    PCMSK0 |= _BV(PCINT0);
    // Down button
    PCMSK2 |= _BV(PCINT21);
    // Reverse
    PCMSK1 |= _BV(PCINT9);
}

// static void test_1s(void *param)
// {
//     printf("one second\r\n");
// }
