/*
 * stm32f401xe_RCC.h
 *
 *  Created on: Jan 13, 2022
 *      Author: ROJEK
 */

#ifndef MYDRIVERS_INC_STM32F401XE_RCC_H_
#define MYDRIVERS_INC_STM32F401XE_RCC_H_

#include "stm32f401xe.h"






// If we are using PLL as main clock source of system clock then we need to calculate it with equation :
// SYSCLK = PLLSource / M  * N	/ P (this value is then going on AHB and ABP prescalers
// Clock for OTG FS, SDIO and RNG = PLLSource /M *N /Q (straight to those peripherals)
//
typedef struct
{
  uint32_t PLLState;   /*!< The new state of the PLL.
                            This parameter can be a value of @ref RCC_PLL_Config                      */

  uint8_t PLLSource;   /*!  @PLLSource - HSI or HSE  */

  uint8_t PLLM;       /*!< PLLM: Division factor for PLL VCO input clock. Range 0 - 63  */

  uint16_t PLLN;       /*!< PLLN: Multiplication factor for PLL VCO output clock.
                            This parameter must be a number between Min_Data = 50 and Max_Data = 432
                            except for STM32F411xE devices where the Min_Data = 192 */

  uint8_t PLLP;       /*!< @PLLP PLLP: Division factor for main system clock (SYSCLK).*/

  uint8_t PLLQ;       /*!< PLLQ: Division factor for OTG FS, SDIO and RNG clocks.
                            This parameter must be a number between Min_Data = 2 and Max_Data = 15    */

}RCC_PLLInitTypeDef;

typedef struct
{
  uint32_t OscillatorType;       /*!< The oscillators to be configured.
                                      This parameter can be a value of @ref RCC_Oscillator_Type                   */

  uint32_t HSEState;             /*!< The new state of the HSE.
                                      This parameter can be a value of @ref RCC_HSE_Config                        */

  uint32_t LSEState;             /*!< The new state of the LSE.
                                      This parameter can be a value of @ref RCC_LSE_Config                        */

  uint32_t HSIState;             /*!< The new state of the HSI.
                                      This parameter can be a value of @ref RCC_HSI_Config                        */

  uint32_t HSICalibrationValue;  /*!< The HSI calibration trimming value (default is RCC_HSICALIBRATION_DEFAULT).
                                       This parameter must be a number between Min_Data = 0x00 and Max_Data = 0x1F */

  uint32_t LSIState;             /*!< The new state of the LSI.
                                      This parameter can be a value of @ref RCC_LSI_Config                        */

  RCC_PLLInitTypeDef PLL;        /*!< PLL structure parameters                                                    */
}RCC_OscInitTypeDef;

/*
 Main PLL(PLL) and audio PLL (PLLI2S) entry clock source
 @PLLSource
 */
#define RCC_PLL_SOURCE_HSI		0U
#define RCC_PLL_SOURCE_HSE		1U

/*
 PLLP: Main PLL (PLL) division factor for main system clock
 @PLLP
 */
#define RCC_PLLP_DIV2			0U
#define RCC_PLLP_DIV4			1U
#define RCC_PLLP_DIV6			2U
#define RCC_PLLP_DIV8			3U

/*
System clock switch
Set and cleared by software to select the system clock source.
Set by hardware to force the HSI selection when leaving the Stop or Standby mode or in
case of failure of the HSE oscillator used directly or indirectly as the system clock.
*/
#define RCC_SW_HSI				0U
#define RCC_SW_HSE				1U
#define RCC_SW_PLL				2U

/*
AHB prescaler
Set and cleared by software to control AHB clock division factor
 */
#define RCC_HPRE_DIV2			0U
#define RCC_HPRE_DIV4			1U
#define RCC_HPRE_DIV8			2U
#define RCC_HPRE_DIV16			3U
#define RCC_HPRE_DIV64			4U
#define RCC_HPRE_DIV128			5U
#define RCC_HPRE_DIV256			6U
#define RCC_HPRE_DIV512			7U

/*
APB Low speed prescaler (APB1)
Set and cleared by software to control APB low-speed clock division factor
 */

#define RCC_ABP1_PRESCALER_DIV2		0U
#define RCC_ABP1_PRESCALER_DIV4		1U
#define RCC_ABP1_PRESCALER_DIV8		2U
#define RCC_ABP1_PRESCALER_DIV16	3U

/*
APB high-speed prescaler (APB2)
Set and cleared by software to control APB high-speed clock division factor
 */

#define RCC_ABP2_PRESCALER_DIV2		0U
#define RCC_ABP2_PRESCALER_DIV4		1U
#define RCC_ABP2_PRESCALER_DIV8		2U
#define RCC_ABP2_PRESCALER_DIV16	3U

#endif /* MYDRIVERS_INC_STM32F401XE_RCC_H_ */
