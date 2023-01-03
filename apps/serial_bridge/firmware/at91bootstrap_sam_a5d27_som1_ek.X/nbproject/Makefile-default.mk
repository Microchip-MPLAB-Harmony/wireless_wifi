#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile).
#
_/_=\\ShExtension=.batDevice=ATSAMA5D27CD1G
ProjectDir="D:\WiFi_Projects\WINC\csp_apps_sam_a5d2\apps\uart\uart_ring_buffer_interrupt\firmware\at91bootstrap_sam_a5d27_som1_ek.X"
ProjectName=at91bootstrap
ConfName=default
TOOLCHAIN_pic-as=C:\Program Files\Microchip\xc8\v2.31\pic-as\bin
TOOLCHAIN_XC32=C:\Program Files\Microchip\xc32\v4.00\bin
TOOLCHAIN_AVRASM=C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avrassembler
TOOLCHAIN_ARM=c:\Program Files (x86)\Atmel\Studio\7.0\toolchain\arm\arm-gnu-toolchain\bin
TOOLCHAIN_XC8=C:\Program Files\Microchip\xc8\v2.31\bin
TOOLCHAIN_AVR=C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin
#
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
ifeq (,$(findstring nbproject/Makefile-local-default.mk,$(MAKEFILES)))
MAKEFILES+=nbproject/Makefile-local-default.mk
endif
endif
endif
.build-conf:  ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-default.mk image

# ------------------------------------------------------------------------------------
# Rules for buildStep: build and debug
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
image: D:/WiFi_Projects/WINC/csp_apps_sam_a5d2/apps/uart/uart_ring_buffer_interrupt/firmware/at91bootstrap_sam_a5d27_som1_ek.X/binaries/at91bootstrap.elf nbproject/Makefile-default.mk 
else
image: D:/WiFi_Projects/WINC/csp_apps_sam_a5d2/apps/uart/uart_ring_buffer_interrupt/firmware/at91bootstrap_sam_a5d27_som1_ek.X/binaries/at91bootstrap-stripped.elf nbproject/Makefile-default.mk 
endif

.PHONY: D:/WiFi_Projects/WINC/csp_apps_sam_a5d2/apps/uart/uart_ring_buffer_interrupt/firmware/at91bootstrap_sam_a5d27_som1_ek.X/binaries/at91bootstrap.elf
D:/WiFi_Projects/WINC/csp_apps_sam_a5d2/apps/uart/uart_ring_buffer_interrupt/firmware/at91bootstrap_sam_a5d27_som1_ek.X/binaries/at91bootstrap.elf: 
	cd . && make CROSS_COMPILE=arm-none-eabi-

.PHONY: D:/WiFi_Projects/WINC/csp_apps_sam_a5d2/apps/uart/uart_ring_buffer_interrupt/firmware/at91bootstrap_sam_a5d27_som1_ek.X/binaries/at91bootstrap-stripped.elf
D:/WiFi_Projects/WINC/csp_apps_sam_a5d2/apps/uart/uart_ring_buffer_interrupt/firmware/at91bootstrap_sam_a5d27_som1_ek.X/binaries/at91bootstrap-stripped.elf: 
	cd . && make CROSS_COMPILE=arm-none-eabi-


# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	cd . && make mplabclean CROSS_COMPILE=arm-none-eabi-

