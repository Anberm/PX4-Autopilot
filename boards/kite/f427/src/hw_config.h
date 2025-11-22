/*
 * hw_config.h
 *
 * Kite F427 Flight Controller Bootloader Configuration
 * Hardware: STM32F427VIT6
 */

#ifndef HW_CONFIG_H_
#define HW_CONFIG_H_

/****************************************************************************
 * Bootloader Configuration for Kite F427
 *
 * This file defines the hardware-specific configuration for the bootloader
 * running on the Kite F427 flight controller board.
 *
 * Key Parameters:
 * APP_LOAD_ADDRESS     0x08004000  - Application firmware starts at 16KB offset
 * BOOTLOADER_DELAY     5000        - Wait 5 seconds for bootloader requests
 * BOARD_TYPE           427         - Must match firmware.prototype board_id
 * BOARD_FLASH_SIZE     2MB (2048KB)
 * APP_RESERVATION_SIZE 16KB        - Reserved for bootloader
 *
 ****************************************************************************/

/* Boot device selection list */
#define USB0_DEV       0x01
#define SERIAL0_DEV    0x02
#define SERIAL1_DEV    0x04

/* USART Interface Configuration - Used by bootloader for serial break detection */
#define BOARD_PORT_USART_RX            GPIOD
#define BOARD_PIN_RX                   GPIO6    /* PD6: USART2 RX */
#define BOARD_PORT_USART_TX            GPIOD
#define BOARD_PIN_TX                   GPIO5    /* PD5: USART2 TX */
#define BOARD_USART                    2        /* USART2 */
#define BOARD_USART_CLOCK_REGISTER     RCC_APB1ENR
#define BOARD_USART_CLOCK_BIT          RCC_APB1ENR_USART2EN

/* GPIO macro for bootloader use */
#define MK_GPIO_INPUT(def) (((def) & (GPIO_PORT_MASK | GPIO_PIN_MASK | GPIO_MODE_MASK)) | GPIO_INPUT)

/* Application firmware load address (after 16KB bootloader) */
#define APP_LOAD_ADDRESS               0x08004000

/* Bootloader delay in milliseconds */
#define BOOTLOADER_DELAY               5000

/* Bootloader reservation size (must match script.ld) */
#define BOOTLOADER_RESERVATION_SIZE    (16 * 1024)

/* USB Interface Configuration */
#define INTERFACE_USB                  1
#define INTERFACE_USB_CONFIG           "/dev/ttyACM0"
#define BOARD_VBUS                     MK_GPIO_INPUT(GPIO_OTGFS_VBUS)

/* UART Interface Configuration - USART1 at 921600 baud */
#define INTERFACE_USART                1
#define INTERFACE_USART_CONFIG         "/dev/ttyS0,921600"

/* Bootloader delay address for custom delay from app firmware */
#define BOOT_DELAY_ADDRESS             0x000001a0

/* Board Type - Must match firmware.prototype board_id (427) */
#define BOARD_TYPE                     427

/* Flash size detection - STM32F427 has 2048KB flash */
#define _FLASH_KBYTES                  (*(uint32_t *)0x1FF1E880)

/* STM32F427 has 12 flash sectors total (16KB + 16KB + 16KB + 16KB + 64KB + 128KB x 7) */
#define BOARD_FLASH_SECTORS            (12)

/* Total flash size available */
#define BOARD_FLASH_SIZE               (_FLASH_KBYTES * 1024)

/* Application reservation size (first sector reserved for bootloader) */
#define APP_RESERVATION_SIZE           (16 * 1024)

/* Oscillator frequency in MHz */
#define OSC_FREQ                       24

/* LED Configuration */
/* GPIO_LED_RED: PB11 (OVERLOAD), GPIO_LED_GREEN: PB1 (ARMED_STATE), GPIO_LED_BLUE: PB3 (ARMED) */
#define BOARD_PIN_LED_ACTIVITY         GPIO_LED_BLUE   /* BLUE - Activity indicator */
#define BOARD_PIN_LED_BOOTLOADER       GPIO_LED_GREEN  /* GREEN - Bootloader indicator */
#define BOARD_LED_ON                   0                /* Open-drain: 0 = ON (low) */
#define BOARD_LED_OFF                  1                /* Open-drain: 1 = OFF (high-z) */

/* Disable serial break detection to prevent accidental bootloader entry */
#define SERIAL_BREAK_DETECT_DISABLED   1

/* Optional: Force bootloader mode via PWM pin (disabled by default) */
/*
 * #define BOARD_FORCE_BL_PIN_OUT       (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_SET|GPIO_PORTE|GPIO_PIN14)
 * #define BOARD_FORCE_BL_PIN_IN        (GPIO_INPUT|GPIO_PULLUP|GPIO_PORTE|GPIO_PIN11)
 *
 * #define BOARD_POWER_PIN_OUT          (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_SET|GPIO_PORTA|GPIO_PIN4)
 * #define BOARD_POWER_ON               1
 * #define BOARD_POWER_OFF              0
 */

/* Architecture Serial Number Maximum Length */
#if !defined(ARCH_SN_MAX_LENGTH)
# define ARCH_SN_MAX_LENGTH 12
#endif

/* Default App Reservation Size if not defined */
#if !defined(APP_RESERVATION_SIZE)
#  define APP_RESERVATION_SIZE 0
#endif

/* First flash sector to erase (after bootloader) */
#if !defined(BOARD_FIRST_FLASH_SECTOR_TO_ERASE)
#  define BOARD_FIRST_FLASH_SECTOR_TO_ERASE 1
#endif

/* USB Data Alignment */
#if !defined(USB_DATA_ALIGN)
# define USB_DATA_ALIGN
#endif

/* Boot Device Selection - Allow all devices */
#ifndef BOOT_DEVICES_SELECTION
#  define BOOT_DEVICES_SELECTION USB0_DEV|SERIAL0_DEV|SERIAL1_DEV
#endif

/* Boot Device Filter when powered by USB */
#ifndef BOOT_DEVICES_FILTER_ONUSB
#  define BOOT_DEVICES_FILTER_ONUSB USB0_DEV|SERIAL0_DEV|SERIAL1_DEV
#endif

/* Calculate maximum application firmware size */
#define APP_SIZE_MAX (BOARD_FLASH_SIZE - (BOOTLOADER_RESERVATION_SIZE + APP_RESERVATION_SIZE))

#endif /* HW_CONFIG_H_ */
