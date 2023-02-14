## Boombaard gegenereerd CMakelists add_library bestand
## Bestand gegenereerd op : 2021-01-14 13:16:53.
## (c)2001-2020 Hogeschool van Arnhem en Nijmegen
## ir drs E.J Boks, opleiding Elektrotechniek/ESE
## $Id: CMakeLists.txt 4390 2022-02-24 14:44:20Z ewout $


set(LibName STM32F4xx_HAL_Driver)

# Het absolute pad voor dit bestand “CMakeLists.txt” is “/Users/ese/vakken/ti_cp/DigitaleSignaalbewerking/practicum/software/STM32Basis/HAL/gemeenschappelijk/Libraries/STM32F4xx_HAL_Driver/“ 
## Definieer de minimale cmake versie.
cmake_minimum_required(VERSION 3.10.4)


## GemPath is een gemeenschappelijk pad dat gevonden wordt onder de bron, inclusief en bibliotheek paden.
set(GemPath0 ${CMAKE_CURRENT_SOURCE_DIR}/Inc)
set(GemPath1 ${GemPath0}/Legacy)
set(GemPath2 ${CMAKE_CURRENT_SOURCE_DIR}/Src)
set(GemPath3 ${GemPath2}/Legacy)



## SourcePath is een gemeenschappelijk pad voor de bronbestanden.
set(SourcePath0 ${GemPath1})
set(SourcePath1 ${GemPath0})
set(SourcePath2 ${GemPath3})
set(SourcePath3 ${GemPath2})



## Dit gedeelte geeft de include directories voor de bronbestanden aan.
set(AllIncludeDirs ${SourcePath0} ${SourcePath1} )
include_directories(${AllIncludeDirs})


## Geef een lijst van alle bronbestanden met de juiste paden.
set(SRCS0 stm32f4xx_hal_can.c )
set(Addition0Path ${SourcePath2})
foreach(FN ${SRCS0})
	list(APPEND SRCS ${Addition0Path}/${FN})
endforeach(FN)

set(SRCS1 stm32f4xx_hal.c stm32f4xx_hal_adc.c stm32f4xx_hal_adc_ex.c stm32f4xx_hal_can.c stm32f4xx_hal_cec.c 
	stm32f4xx_hal_cortex.c stm32f4xx_hal_crc.c stm32f4xx_hal_cryp.c stm32f4xx_hal_cryp_ex.c stm32f4xx_hal_dac.c 
	stm32f4xx_hal_dac_ex.c stm32f4xx_hal_dcmi.c stm32f4xx_hal_dcmi_ex.c stm32f4xx_hal_dfsdm.c stm32f4xx_hal_dma.c 
	stm32f4xx_hal_dma2d.c stm32f4xx_hal_dma_ex.c stm32f4xx_hal_dsi.c stm32f4xx_hal_eth.c stm32f4xx_hal_exti.c 
	stm32f4xx_hal_flash.c stm32f4xx_hal_flash_ex.c stm32f4xx_hal_flash_ramfunc.c stm32f4xx_hal_fmpi2c.c stm32f4xx_hal_fmpi2c_ex.c 
	stm32f4xx_hal_fmpsmbus.c stm32f4xx_hal_gpio.c stm32f4xx_hal_hash.c stm32f4xx_hal_hash_ex.c stm32f4xx_hal_hcd.c 
	stm32f4xx_hal_i2c.c stm32f4xx_hal_i2c_ex.c stm32f4xx_hal_i2s.c stm32f4xx_hal_i2s_ex.c stm32f4xx_hal_irda.c 
	stm32f4xx_hal_iwdg.c stm32f4xx_hal_lptim.c stm32f4xx_hal_ltdc.c stm32f4xx_hal_ltdc_ex.c stm32f4xx_hal_mmc.c 
	stm32f4xx_hal_nand.c stm32f4xx_hal_nor.c stm32f4xx_hal_pccard.c stm32f4xx_hal_pcd.c stm32f4xx_hal_pcd_ex.c 
	stm32f4xx_hal_pwr.c stm32f4xx_hal_pwr_ex.c stm32f4xx_hal_qspi.c stm32f4xx_hal_rcc.c stm32f4xx_hal_rcc_ex.c 
	stm32f4xx_hal_rng.c stm32f4xx_hal_rtc.c stm32f4xx_hal_rtc_ex.c stm32f4xx_hal_sai.c stm32f4xx_hal_sai_ex.c 
	stm32f4xx_hal_sd.c stm32f4xx_hal_sdram.c stm32f4xx_hal_smartcard.c stm32f4xx_hal_smbus.c stm32f4xx_hal_spdifrx.c 
	stm32f4xx_hal_spi.c stm32f4xx_hal_sram.c stm32f4xx_hal_tim.c stm32f4xx_hal_tim_ex.c stm32f4xx_hal_uart.c 
	stm32f4xx_hal_usart.c stm32f4xx_hal_wwdg.c stm32f4xx_ll_adc.c stm32f4xx_ll_crc.c stm32f4xx_ll_dac.c 
	stm32f4xx_ll_dma.c stm32f4xx_ll_dma2d.c stm32f4xx_ll_exti.c stm32f4xx_ll_fmc.c stm32f4xx_ll_fmpi2c.c stm32f4xx_ll_fsmc.c 
	stm32f4xx_ll_gpio.c stm32f4xx_ll_i2c.c stm32f4xx_ll_lptim.c stm32f4xx_ll_pwr.c stm32f4xx_ll_rcc.c stm32f4xx_ll_rng.c 
	stm32f4xx_ll_rtc.c stm32f4xx_ll_sdmmc.c stm32f4xx_ll_spi.c stm32f4xx_ll_tim.c stm32f4xx_ll_usart.c stm32f4xx_ll_usb.c 
	stm32f4xx_ll_utils.c )
set(Addition1Path ${SourcePath3})
foreach(FN ${SRCS1})
	list(APPEND SRCS ${Addition1Path}/${FN})
endforeach(FN)


list(SORT SRCS)

add_library(${LibName} STATIC ${SRCS})
## This library can now be included in any build by using “add_subdirectory( the_path_to_this_lib )”.


