#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "board_config.h"
#include "hw_config.h"
#include "bl.h"
#include <nvic.h>
#include <lib/systick.h>
#include <stm32_gpio.h>

/* Define MCU identification masks for STM32F4 */
#define DEVID_MASK  0x0fff
#define REVID_MASK  0xffff0000
#define STM32_UNKNOWN 0

static void board_init(void);

#define BOOT_RTC_SIGNATURE          0xb007b007
#define BOOT_RTC_REG                MMIO32(RTC_BASE + 0x50)

static bool usb_connected = false;

static uint32_t board_get_rtc_signature(void)
{
	stm32_pwr_initbkp(true);
	uint32_t result = getreg32(STM32_RTC_BK0R);
	stm32_pwr_initbkp(false);
	return result;
}

static void board_set_rtc_signature(uint32_t sig)
{
	stm32_pwr_initbkp(true);
	putreg32(sig, STM32_RTC_BK0R);
	stm32_pwr_initbkp(false);
}

static bool board_test_force_pin(void)
{
#if defined(BOARD_FORCE_BL_PIN_IN) && defined(BOARD_FORCE_BL_PIN_OUT)
	volatile unsigned samples = 0;
	volatile unsigned vote = 0;

	for (volatile unsigned cycles = 0; cycles < 10; cycles++) {
		px4_arch_gpiowrite(BOARD_FORCE_BL_PIN_OUT, 1);

		for (unsigned count = 0; count < 20; count++) {
			if (px4_arch_gpioread(BOARD_FORCE_BL_PIN_IN) != 0) {
				vote++;
			}
			samples++;
		}

		px4_arch_gpiowrite(BOARD_FORCE_BL_PIN_OUT, 0);

		for (unsigned count = 0; count < 20; count++) {
			if (px4_arch_gpioread(BOARD_FORCE_BL_PIN_IN) == 0) {
				vote++;
			}
			samples++;
		}
	}

	if ((vote * 100) > (samples * 90)) {
		return true;
	}

#endif
#if defined(BOARD_FORCE_BL_PIN)
	volatile unsigned samples = 0;
	volatile unsigned vote = 0;

	for (samples = 0; samples < 200; samples++) {
		if ((px4_arch_gpioread(BOARD_FORCE_BL_PIN) ? 1 : 0) == BOARD_FORCE_BL_STATE) {
			vote++;
		}
	}

	if ((vote * 100) > (samples * 90)) {
		return true;
	}

#endif
	return false;
}

uint32_t board_get_devices(void)
{
	uint32_t devices = BOOT_DEVICES_SELECTION;

	if (usb_connected) {
		devices &= BOOT_DEVICES_FILTER_ONUSB;
	}

	return devices;
}

static void board_init(void)
{
	board_info.fw_size = APP_SIZE_MAX;

#if defined(BOARD_POWER_PIN_OUT)
	px4_arch_configgpio(BOARD_POWER_PIN_OUT);
	px4_arch_gpiowrite(BOARD_POWER_PIN_OUT, BOARD_POWER_ON);
#endif

#if INTERFACE_USB
#if !defined(BOARD_USB_VBUS_SENSE_DISABLED)
#  if defined(USE_VBUS_PULL_DOWN)
	px4_arch_configgpio((GPIO_OTGFS_VBUS & GPIO_PUPD_MASK) | GPIO_PULLDOWN);
#  else
	px4_arch_configgpio((GPIO_OTGFS_VBUS & GPIO_PUPD_MASK) | GPIO_FLOAT);
#  endif
#endif
#endif

#if defined(BOARD_FORCE_BL_PIN_IN) && defined(BOARD_FORCE_BL_PIN_OUT)
	px4_arch_configgpio(BOARD_FORCE_BL_PIN_IN);
	px4_arch_configgpio(BOARD_FORCE_BL_PIN_OUT);
#endif

#if defined(BOARD_FORCE_BL_PIN)
	px4_arch_configgpio(BOARD_FORCE_BL_PIN);
#endif

#if defined(BOARD_PIN_LED_ACTIVITY)
	px4_arch_configgpio(BOARD_PIN_LED_ACTIVITY);
#endif
#if defined(BOARD_PIN_LED_BOOTLOADER)
	px4_arch_configgpio(BOARD_PIN_LED_BOOTLOADER);
#endif
}

void board_deinit(void)
{
#if INTERFACE_USB
	px4_arch_configgpio(MK_GPIO_INPUT(GPIO_OTGFS_VBUS));
	putreg32(RCC_AHB1RSTR_OTGFSRST, STM32_RCC_AHB1RSTR);
#endif

#if defined(BOARD_FORCE_BL_PIN_IN) && defined(BOARD_FORCE_BL_PIN_OUT)
	px4_arch_configgpio(MK_GPIO_INPUT(BOARD_FORCE_BL_PIN_IN));
	px4_arch_configgpio(MK_GPIO_INPUT(BOARD_FORCE_BL_PIN_OUT));
#endif

#if defined(BOARD_FORCE_BL_PIN)
	px4_arch_configgpio(MK_GPIO_INPUT(BOARD_FORCE_BL_PIN));
#endif

#if defined(BOARD_POWER_PIN_OUT) && defined(BOARD_POWER_PIN_RELEASE)
	px4_arch_configgpio(MK_GPIO_INPUT(BOARD_POWER_PIN_OUT));
#endif

#if defined(BOARD_PIN_LED_ACTIVITY)
	px4_arch_configgpio(MK_GPIO_INPUT(BOARD_PIN_LED_ACTIVITY));
#endif
#if defined(BOARD_PIN_LED_BOOTLOADER)
	px4_arch_configgpio(MK_GPIO_INPUT(BOARD_PIN_LED_BOOTLOADER));
#endif

	putreg32(0, STM32_RCC_AHB1RSTR);
	putreg32(0, STM32_RCC_AHB1ENR);
}

inline void arch_systic_init(void)
{
	/* (re)start the timer system */
	systick_set_clocksource(CLKSOURCE_PROCESOR);
	systick_set_reload(board_info.systick_mhz * 1000);  /* 1ms tick, magic number */
	systick_interrupt_enable();
	systick_counter_enable();
}

inline void arch_systic_deinit(void)
{
	/* kill the systick interrupt */
	systick_interrupt_disable();
	systick_counter_disable();
	systick_set_clocksource(CLKSOURCE_EXTERNAL);
	systick_set_reload(0);  /* 1ms tick, magic number */
}

static inline void clock_init(void)
{
	// Done by NuttX
}

void clock_deinit(void)
{
	uint32_t regval;

	regval = getreg32(STM32_RCC_CR);
	regval |= RCC_CR_HSION;
	putreg32(regval, STM32_RCC_CR);

	while ((getreg32(STM32_RCC_CR) & RCC_CR_HSIRDY) == 0);

	putreg32(0, STM32_RCC_CFGR);

	// For STM32F4: Stop HSE, CSS, PLL
	regval  = getreg32(STM32_RCC_CR);
	regval  &= ~(RCC_CR_HSEON | RCC_CR_PLLON);
	putreg32(regval, STM32_RCC_CR);

#ifdef STM32_RCC_PLLCFGR
	putreg32(0x00000000, STM32_RCC_PLLCFGR);
#else
	/* STM32F4 doesn't have PLLCFGR register */
#endif

	regval  = getreg32(STM32_RCC_CR);
	regval  &= ~(RCC_CR_HSEBYP);
	putreg32(regval, STM32_RCC_CR);
}

void arch_flash_lock(void)
{
	stm32_flash_lock();
}

void arch_flash_unlock(void)
{
	stm32_flash_unlock();
}

ssize_t arch_flash_write(uintptr_t address, const void *buffer, size_t buflen)
{
	return up_progmem_write(address, buffer, buflen);
}

inline void arch_setvtor(const uint32_t *address)
{
	putreg32((uint32_t)address, NVIC_VECTAB);
}

uint32_t flash_func_sector_size(unsigned sector)
{
	if (sector <= BOARD_FLASH_SECTORS) {
		return 128 * 1024;
	}

	return 0;
}

void flash_func_erase_sector(unsigned sector, bool force)
{
	if (sector > BOARD_FLASH_SECTORS || (int)sector < BOARD_FIRST_FLASH_SECTOR_TO_ERASE) {
		return;
	}

	if (force || (up_progmem_ispageerased(sector) != 0)) {
		up_progmem_eraseblock(sector);
	}
}

void flash_func_write_word(uintptr_t address, uint32_t word)
{
	address += APP_LOAD_ADDRESS;
	up_progmem_write(address, (const void *)&word, sizeof(word));
}

uint32_t flash_func_read_word(uintptr_t address)
{
	if (address & 3) {
		return 0;
	}

	return *(uint32_t *)(address + APP_LOAD_ADDRESS);
}

uint32_t flash_func_read_otp(uintptr_t address)
{
	return 0;
}

uint32_t get_mcu_id(void)
{
	return *(uint32_t *)STM32_DEBUGMCU_BASE;
}

int get_mcu_desc(int max, uint8_t *revstr)
{
	/* Simple MCU description for STM32F427 */
	const char *desc = "STM32F427";
	uint8_t *strp = revstr;
	uint8_t *endp = &revstr[max - 1];

	while (strp < endp && *desc) {
		*strp++ = *desc++;
	}

	return strp - revstr;
}

int check_silicon(void)
{
	return 0;
}

uint32_t flash_func_read_sn(uintptr_t address)
{
	return *(uint32_t *)(address + STM32_SYSMEM_UID);
}

void led_on(unsigned led)
{
	switch (led) {
	case LED_ACTIVITY:
#if defined(BOARD_PIN_LED_ACTIVITY)
		px4_arch_gpiowrite(BOARD_PIN_LED_ACTIVITY, BOARD_LED_ON);
#endif
		break;

	case LED_BOOTLOADER:
#if defined(BOARD_PIN_LED_BOOTLOADER)
		px4_arch_gpiowrite(BOARD_PIN_LED_BOOTLOADER, BOARD_LED_ON);
#endif
		break;
	}
}

void led_off(unsigned led)
{
	switch (led) {
	case LED_ACTIVITY:
#if defined(BOARD_PIN_LED_ACTIVITY)
		px4_arch_gpiowrite(BOARD_PIN_LED_ACTIVITY, BOARD_LED_OFF);
#endif
		break;

	case LED_BOOTLOADER:
#if defined(BOARD_PIN_LED_BOOTLOADER)
		px4_arch_gpiowrite(BOARD_PIN_LED_BOOTLOADER, BOARD_LED_OFF);
#endif
		break;
	}
}

void led_toggle(unsigned led)
{
	switch (led) {
	case LED_ACTIVITY:
#if defined(BOARD_PIN_LED_ACTIVITY)
		px4_arch_gpiowrite(BOARD_PIN_LED_ACTIVITY, px4_arch_gpioread(BOARD_PIN_LED_ACTIVITY) ^ 1);
#endif
		break;

	case LED_BOOTLOADER:
#if defined(BOARD_PIN_LED_BOOTLOADER)
		px4_arch_gpiowrite(BOARD_PIN_LED_BOOTLOADER, px4_arch_gpioread(BOARD_PIN_LED_BOOTLOADER) ^ 1);
#endif
		break;
	}
}

#ifndef SCB_CPACR
# define SCB_CPACR (*((volatile uint32_t *) (((0xE000E000UL) + 0x0D00UL) + 0x088)))
#endif

void arch_do_jump(const uint32_t *app_base)
{
	uint32_t stacktop = app_base[0];
	uint32_t entrypoint = app_base[1];

	asm volatile(
		"msr msp, %0  \n"
		"bx %1  \n"
		: : "r"(stacktop), "r"(entrypoint) :);

	for (;;) ;
}

int bootloader_main(void)
{
	bool try_boot = true;
	unsigned timeout = BOOTLOADER_DELAY;

	/* Enable the FPU before we hit any FP instructions */
	SCB_CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2));

#if defined(BOARD_POWER_PIN_OUT)
	if (board_get_rtc_signature() == POWER_DOWN_RTC_SIGNATURE) {
		board_set_rtc_signature(0);
		while (1);
	}
#endif

	board_init();
	clock_init();

	if (board_get_rtc_signature() == BOOT_RTC_SIGNATURE) {
		try_boot = false;
		timeout = 0;
		board_set_rtc_signature(0);
	}

#ifdef BOOT_DELAY_ADDRESS
	{
		uint32_t sig1 = flash_func_read_word(BOOT_DELAY_ADDRESS);
		uint32_t sig2 = flash_func_read_word(BOOT_DELAY_ADDRESS + 4);

		if (sig2 == BOOT_DELAY_SIGNATURE2 &&
		    (sig1 & 0xFFFFFF00) == (BOOT_DELAY_SIGNATURE1 & 0xFFFFFF00)) {
			unsigned boot_delay = sig1 & 0xFF;

			if (boot_delay <= BOOT_DELAY_MAX) {
				try_boot = false;

				if (timeout < boot_delay * 1000) {
					timeout = boot_delay * 1000;
				}
			}
		}
	}
#endif

	if (board_test_force_pin()) {
		try_boot = false;
	}

#if INTERFACE_USB
#if defined(BOARD_VBUS)
	if (px4_arch_gpioread(BOARD_VBUS) != 0) {
		usb_connected = true;
		try_boot = false;
	}
#else
	try_boot = false;
#endif
#endif

	if (try_boot) {
#ifdef BOARD_BOOT_FAIL_DETECT
		board_set_rtc_signature(BOOT_RTC_SIGNATURE);
#endif
		jump_to_app();
		board_set_rtc_signature(BOOT_RTC_SIGNATURE);
		timeout = 0;
	}

#if INTERFACE_USART
#ifdef INTERFACE_USART_CONFIG
	cinit(INTERFACE_USART_CONFIG, USART);
#endif
#endif
#if INTERFACE_USB
#ifdef INTERFACE_USB_CONFIG
	cinit(INTERFACE_USB_CONFIG, USB);
#endif
#endif

	while (1) {
		bootloader(timeout);

		if (board_test_force_pin()) {
			continue;
		}

#ifdef BOARD_BOOT_FAIL_DETECT
		board_set_rtc_signature(BOOT_RTC_SIGNATURE);
#endif

		jump_to_app();
		timeout = 0;
	}
}
