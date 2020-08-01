/*
 *   File:   stepper.h
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

#ifndef __STEPPER_H__
#define __STEPPER_H__

#define STEP_FORWARD    0
#define STEP_REVERSE    1

#define ENA_DDR   IO3_DDR
#define ENA_PORT  IO3_PORT
#define ENA       IO3

#define ENB_DDR   IO11_DDR
#define ENB_PORT  IO11_PORT
#define ENB       IO11

#define DIRA_DDR  IO12_DDR
#define DIRA_PORT IO12_PORT
#define DIRA      IO12

#define DIRB_DDR  IO13_DDR
#define DIRB_PORT IO13_PORT
#define DIRB      IO13

void stepper_init(uint16_t delay);
void stepper_set_delay(uint16_t delay);
void stepper_move_fixed_count(uint8_t dir, uint16_t steps);
uint8_t stepper_get_phase(void);

#endif /* __STEPPER_H__ */
