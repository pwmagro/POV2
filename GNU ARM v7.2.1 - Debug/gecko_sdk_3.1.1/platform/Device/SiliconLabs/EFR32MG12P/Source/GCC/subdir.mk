################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/gecko_sdk_suite/v3.1/platform/Device/SiliconLabs/EFR32MG12P/Source/GCC/startup_efr32mg12p.c 

OBJS += \
./gecko_sdk_3.1.1/platform/Device/SiliconLabs/EFR32MG12P/Source/GCC/startup_efr32mg12p.o 

C_DEPS += \
./gecko_sdk_3.1.1/platform/Device/SiliconLabs/EFR32MG12P/Source/GCC/startup_efr32mg12p.d 


# Each subdirectory must supply rules for building sources it contributes
gecko_sdk_3.1.1/platform/Device/SiliconLabs/EFR32MG12P/Source/GCC/startup_efr32mg12p.o: C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/gecko_sdk_suite/v3.1/platform/Device/SiliconLabs/EFR32MG12P/Source/GCC/startup_efr32mg12p.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFR32MG12P332F1024GL125=1' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Users\pwmag\SimplicityStudio\v5_workspace\POV2\hdr" -I"C:\Users\pwmag\SimplicityStudio\v5_workspace\POV2\autogen" -I"C:\Users\pwmag\SimplicityStudio\v5_workspace\POV2\config" -I"C:\Users\pwmag\SimplicityStudio\v5_workspace\POV2" -I"C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/gecko_sdk_suite/v3.1//platform/common/toolchain/inc" -I"C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/gecko_sdk_suite/v3.1//platform/emlib/inc" -I"C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/gecko_sdk_suite/v3.1//hardware/board/inc" -I"C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/gecko_sdk_suite/v3.1//hardware/driver/mx25_flash_shutdown/inc/sl_mx25_flash_shutdown_usart" -I"C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/gecko_sdk_suite/v3.1//platform/service/device_init/inc" -I"C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/gecko_sdk_suite/v3.1//platform/common/inc" -I"C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/gecko_sdk_suite/v3.1//platform/Device/SiliconLabs/EFR32MG12P/Include" -I"C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/gecko_sdk_suite/v3.1//platform/service/system/inc" -I"C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/gecko_sdk_suite/v3.1//platform/emdrv/dmadrv/inc" -I"C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/gecko_sdk_suite/v3.1//app/common/util/app_assert" -I"C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/gecko_sdk_suite/v3.1//platform/CMSIS/Include" -I"C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/gecko_sdk_suite/v3.1//platform/emdrv/common/inc" -I"C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/gecko_sdk_suite/v3.1//platform/service/udelay/inc" -O0 -Wall -Wextra -fno-builtin -fomit-frame-pointer -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"gecko_sdk_3.1.1/platform/Device/SiliconLabs/EFR32MG12P/Source/GCC/startup_efr32mg12p.d" -MT"gecko_sdk_3.1.1/platform/Device/SiliconLabs/EFR32MG12P/Source/GCC/startup_efr32mg12p.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


