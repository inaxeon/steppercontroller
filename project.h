/*
 *   File:   project.h
 *   Author: Matt
 *
 *   Created on 11 May 2018, 11:51
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


#ifndef __PROJECT_H__
#define __PROJECT_H__

#define _USART1_

#define F_CPU      16000000

#define CONFIG_MAGIC        0x5343

#define CLRWDT() asm("wdr")

#define g_irq_disable cli
#define g_irq_enable sei

#define USART1_DDR         DDRD
#define USART1_TX          PD3
#define USART1_RX          PD2
#define USART1_XCK         PD5

#define SPI_DDR            DDRB
#define SPI_PORT           PORTB
#define SPI_MISO           PB3
#define SPI_MOSI           PB2
#define SPI_SCK            PB1

#define UART1_BAUD           9600

#define TIMEOUT_TICK_PER_SECOND  (100)
#define TIMEOUT_MS_PER_TICK      (1000 / TIMEOUT_TICK_PER_SECOND)

#define console_busy         usart1_busy
#define console_put          usart1_put
#define console_data_ready   usart1_data_ready
#define console_get          usart1_get
#define console_clear_oerr   usart1_clear_oerr

#endif /* __PROJECT_H__ */