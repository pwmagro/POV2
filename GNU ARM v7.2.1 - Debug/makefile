################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/POV/subdir.mk
-include src/subdir.mk
-include gecko_sdk_3.1.1/platform/service/udelay/src/subdir.mk
-include gecko_sdk_3.1.1/platform/service/system/src/subdir.mk
-include gecko_sdk_3.1.1/platform/service/device_init/src/subdir.mk
-include gecko_sdk_3.1.1/platform/emlib/src/subdir.mk
-include gecko_sdk_3.1.1/platform/emdrv/dmadrv/src/subdir.mk
-include gecko_sdk_3.1.1/platform/common/toolchain/src/subdir.mk
-include gecko_sdk_3.1.1/platform/Device/SiliconLabs/EFR32MG12P/Source/GCC/subdir.mk
-include gecko_sdk_3.1.1/platform/Device/SiliconLabs/EFR32MG12P/Source/subdir.mk
-include gecko_sdk_3.1.1/hardware/driver/mx25_flash_shutdown/src/sl_mx25_flash_shutdown_usart/subdir.mk
-include gecko_sdk_3.1.1/hardware/board/src/subdir.mk
-include autogen/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: POV2.axf

# Tool invocations
POV2.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -T "C:\Users\pwmag\SimplicityStudio\v5_workspace\POV2/autogen/linkerfile.ld" -u _printf_float -Xlinker --gc-sections -Xlinker -Map="POV2.map" -mfpu=fpv4-sp-d16 -mfloat-abi=softfp --specs=nano.specs -o POV2.axf -Wl,--start-group "./autogen/sl_board_default_init.o" "./autogen/sl_device_init_clocks.o" "./autogen/sl_event_handler.o" "./gecko_sdk_3.1.1/hardware/board/src/sl_board_control_gpio.o" "./gecko_sdk_3.1.1/hardware/board/src/sl_board_init.o" "./gecko_sdk_3.1.1/hardware/driver/mx25_flash_shutdown/src/sl_mx25_flash_shutdown_usart/sl_mx25_flash_shutdown.o" "./gecko_sdk_3.1.1/platform/Device/SiliconLabs/EFR32MG12P/Source/GCC/startup_efr32mg12p.o" "./gecko_sdk_3.1.1/platform/Device/SiliconLabs/EFR32MG12P/Source/system_efr32mg12p.o" "./gecko_sdk_3.1.1/platform/common/toolchain/src/sl_memory.o" "./gecko_sdk_3.1.1/platform/emdrv/dmadrv/src/dmadrv.o" "./gecko_sdk_3.1.1/platform/emlib/src/em_adc.o" "./gecko_sdk_3.1.1/platform/emlib/src/em_assert.o" "./gecko_sdk_3.1.1/platform/emlib/src/em_cmu.o" "./gecko_sdk_3.1.1/platform/emlib/src/em_core.o" "./gecko_sdk_3.1.1/platform/emlib/src/em_emu.o" "./gecko_sdk_3.1.1/platform/emlib/src/em_gpio.o" "./gecko_sdk_3.1.1/platform/emlib/src/em_i2c.o" "./gecko_sdk_3.1.1/platform/emlib/src/em_ldma.o" "./gecko_sdk_3.1.1/platform/emlib/src/em_letimer.o" "./gecko_sdk_3.1.1/platform/emlib/src/em_system.o" "./gecko_sdk_3.1.1/platform/emlib/src/em_timer.o" "./gecko_sdk_3.1.1/platform/emlib/src/em_usart.o" "./gecko_sdk_3.1.1/platform/service/device_init/src/sl_device_init_dcdc_s1.o" "./gecko_sdk_3.1.1/platform/service/device_init/src/sl_device_init_emu_s1.o" "./gecko_sdk_3.1.1/platform/service/device_init/src/sl_device_init_hfxo_s1.o" "./gecko_sdk_3.1.1/platform/service/device_init/src/sl_device_init_lfxo_s1.o" "./gecko_sdk_3.1.1/platform/service/device_init/src/sl_device_init_nvic.o" "./gecko_sdk_3.1.1/platform/service/system/src/sl_system_init.o" "./gecko_sdk_3.1.1/platform/service/system/src/sl_system_process_action.o" "./gecko_sdk_3.1.1/platform/service/udelay/src/sl_udelay.o" "./gecko_sdk_3.1.1/platform/service/udelay/src/sl_udelay_armv6m_gcc.o" "./main.o" "./src/HW_delay.o" "./src/SI7021.o" "./src/adc.o" "./src/app.o" "./src/battery.o" "./src/bmp280.o" "./src/cmu.o" "./src/gpio.o" "./src/i2c.o" "./src/letimer.o" "./src/scheduler.o" "./src/sleep_routines.o" "./src/timer.o" "./src/usart.o" "./src/ws2812b.o" "./src/POV/font.o" "./src/POV/pov.o" -lgcc -lc -lm -lnosys -Wl,--end-group -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: POV2.hex'
	arm-none-eabi-objcopy -O ihex "POV2.axf" "POV2.hex"
	@echo ' '

	@echo 'Building bin file: POV2.bin'
	arm-none-eabi-objcopy -O binary "POV2.axf" "POV2.bin"
	@echo ' '

	@echo 'Building s37 file: POV2.s37'
	arm-none-eabi-objcopy -O srec "POV2.axf" "POV2.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "POV2.axf" -A
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) POV2.axf
	-@echo ' '

.PHONY: all clean dependents

-include ../makefile.targets
