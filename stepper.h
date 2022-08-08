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

#define MAX_STEP_INTERVALS  160

void stepper_init(uint16_t delay, uint8_t pwm_duty);
void stepper_set_delay(uint16_t delay);
void stepper_set_duty(uint8_t duty);
bool stepper_move_fixed_count(uint8_t dir, uint16_t steps);
bool stepper_move_single_step(uint8_t dir);
void stepper_start_continous(uint8_t dir, uint16_t num_rotations);
void stepper_stop_continous(void);
void stepper_get_rotations(int16_t *rotations, uint8_t *fraction);
void stepper_change_dir(void);
uint8_t stepper_get_phase(void);

#endif /* __STEPPER_H__ */
