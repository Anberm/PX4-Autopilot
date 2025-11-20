############################################################################
#
#   Copyright (c) 2015 PX4 Development Team. All rights reserved.
#
# ... existing code ...
#
############################################################################

if("${PX4_BOARD_LABEL}" STREQUAL "bootloader")
	add_library(drivers_board
		bootloader_main.c
		usb.c
	)
	target_link_libraries(drivers_board
		PRIVATE
			nuttx_arch
			nuttx_drivers
			bootloader
	)
	target_include_directories(drivers_board PRIVATE ${PX4_SOURCE_DIR}/platforms/nuttx/src/bootloader/common)
else()
	add_library(drivers_board
		can.c
		i2c.cpp
		init.c
		led.c
		spi.cpp
		timer_config.cpp
		usb.c
	)
	target_link_libraries(drivers_board
		PRIVATE
			arch_spi
			drivers__led # drv_led_start
			nuttx_arch # sdio
			nuttx_drivers # sdio
			px4_layer
	)
endif()
