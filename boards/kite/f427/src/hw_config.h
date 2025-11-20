/****************************************************************************
 *
 *   Copyright (C) 2021 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#pragma once

/* STM32F4 compatibility - use AHB2 for OTG FS reset on F4 series */
#ifndef RCC_AHB1RSTR_OTGFSRST
#  define RCC_AHB1RSTR_OTGFSRST  RCC_AHB2RSTR_OTGFSRST
#endif

#undef STM32_RCC_AHB1RSTR
#define STM32_RCC_AHB1RSTR     STM32_RCC_AHB2RSTR

#define MK_GPIO_INPUT(def) (((def) & (GPIO_PORT_MASK | GPIO_PIN_MASK | GPIO_MODE_MASK)) | (GPIO_INPUT))

/****************************************************************************
 * Bootloader Configuration for Kite F427 v1
 *
 * STM32F427 Flight Controller Board
 * - 168 MHz ARM Cortex-M4 with FPU
 * - 1 MB Flash, 192 KB SRAM
 * - USB 2.0 Full-Speed interface
 * - CAN bus support
 *
 ****************************************************************************/

/* Boot device selection list */
#define USB0_DEV       0x01
#define SERIAL0_DEV    0x02
#define SERIAL1_DEV    0x04

/* Bootloader Configuration */
#define BOOTLOADER_RESERVATION_SIZE    (128 * 1024)
#define APP_LOAD_ADDRESS               0x08020000
#define BOOTLOADER_DELAY               5000
#define INTERFACE_USB                  1
#define INTERFACE_USB_CONFIG           "/dev/ttyACM0"

#define INTERFACE_USART                1
#define INTERFACE_USART_CONFIG         "/dev/ttyS0,115200"
#define BOOT_DELAY_ADDRESS             0x000001a0

/* Hardware Configuration */
#define BOARD_TYPE                     2021      /* board_id: (7 << 8) + 229 */
#define _FLASH_KBYTES                  (*(uint32_t *)0x1FFF7A22)  /* STM32F427 flash size register */
#define BOARD_FLASH_SECTORS            (11)      /* Last sector index for STM32F4 (0-11) */
#define BOARD_FLASH_SIZE               (_FLASH_KBYTES * 1024)
#define APP_SIZE_MAX                   (BOARD_FLASH_SIZE - (BOOTLOADER_RESERVATION_SIZE + APP_RESERVATION_SIZE))

#define OSC_FREQ                       8

/* ========== USB Configuration (OTG FS) ========== */
#define INTERFACE_USB                  1
#define USBDEVICESTRING                "PX4 BL KiteF427"
#define USBPRODUCTID                   0x0022
#define BOARD_USB_VBUS_SENSE_DISABLED  0
/* USB VBUS sensing on PA9 - configured in board_config.h */

/* ========== UART Configuration (for bootloader UART channel) ========== */
/* UART1 verified from schematic: TX=PB6, RX=PB7 */
#define INTERFACE_USART                1
#define BOARD_USART                    USART1
#define BOARD_USART_CLOCK_REGISTER     RCC_APB2ENR
#define BOARD_USART_CLOCK_BIT          RCC_APB2ENR_USART1EN

/* ========== LED Configuration - using GPIO outputs ========== */
/* LED configuration verified from schematic DXF */
/* LED_Activity (D7): PA4, LED_Bootloader (D14): PA2 */
#define BOARD_PIN_LED_ACTIVITY         (GPIO_OUTPUT|GPIO_OPENDRAIN|GPIO_SPEED_50MHz|GPIO_OUTPUT_SET|GPIO_PORTA|GPIO_PIN4)
#define BOARD_PIN_LED_BOOTLOADER       (GPIO_OUTPUT|GPIO_OPENDRAIN|GPIO_SPEED_50MHz|GPIO_OUTPUT_SET|GPIO_PORTA|GPIO_PIN2)
/* LED active-low logic (0=ON, 1=OFF) */
#define BOARD_LED_ON                   0
#define BOARD_LED_OFF                  1

/* Bootloader Features */
#define SERIAL_BREAK_DETECT_DISABLED   1

#if !defined(ARCH_SN_MAX_LENGTH)
# define ARCH_SN_MAX_LENGTH 12
#endif

#if !defined(APP_RESERVATION_SIZE)
#  define APP_RESERVATION_SIZE 0
#endif

#if !defined(BOARD_FIRST_FLASH_SECTOR_TO_ERASE)
#  define BOARD_FIRST_FLASH_SECTOR_TO_ERASE 1
#endif

#if !defined(USB_DATA_ALIGN)
# define USB_DATA_ALIGN
#endif

#ifndef BOOT_DEVICES_SELECTION
#  define BOOT_DEVICES_SELECTION USB0_DEV|SERIAL0_DEV|SERIAL1_DEV
#endif

#ifndef BOOT_DEVICES_FILTER_ONUSB
#  define BOOT_DEVICES_FILTER_ONUSB USB0_DEV|SERIAL0_DEV|SERIAL1_DEV
#endif
