/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_exti.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_dma.h"
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
void MX_USART1_UART_Init(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DIO4_Pin LL_GPIO_PIN_13
#define DIO4_GPIO_Port GPIOC
#define PE_Pin LL_GPIO_PIN_14
#define PE_GPIO_Port GPIOC
#define LED2_Pin LL_GPIO_PIN_15
#define LED2_GPIO_Port GPIOC
#define LED1_Pin LL_GPIO_PIN_1
#define LED1_GPIO_Port GPIOA
#define REN_Pin LL_GPIO_PIN_2
#define REN_GPIO_Port GPIOA
#define IFC_Pin LL_GPIO_PIN_3
#define IFC_GPIO_Port GPIOA
#define NDAC_Pin LL_GPIO_PIN_4
#define NDAC_GPIO_Port GPIOA
#define NRFD_Pin LL_GPIO_PIN_5
#define NRFD_GPIO_Port GPIOA
#define DAV_Pin LL_GPIO_PIN_6
#define DAV_GPIO_Port GPIOA
#define EOI_Pin LL_GPIO_PIN_7
#define EOI_GPIO_Port GPIOA
#define ATN_Pin LL_GPIO_PIN_0
#define ATN_GPIO_Port GPIOB
#define SRQ_Pin LL_GPIO_PIN_1
#define SRQ_GPIO_Port GPIOB
#define SC_Pin LL_GPIO_PIN_2
#define SC_GPIO_Port GPIOB
#define DC_Pin LL_GPIO_PIN_13
#define DC_GPIO_Port GPIOB
#define TE_Pin LL_GPIO_PIN_8
#define TE_GPIO_Port GPIOA
#define DIO1_Pin LL_GPIO_PIN_15
#define DIO1_GPIO_Port GPIOA
#define DIO2_Pin LL_GPIO_PIN_3
#define DIO2_GPIO_Port GPIOB
#define DIO3_Pin LL_GPIO_PIN_4
#define DIO3_GPIO_Port GPIOB
#define DS18B20_Pin LL_GPIO_PIN_5
#define DS18B20_GPIO_Port GPIOB
#define DIO5_Pin LL_GPIO_PIN_6
#define DIO5_GPIO_Port GPIOB
#define DIO6_Pin LL_GPIO_PIN_7
#define DIO6_GPIO_Port GPIOB
#define DIO7_Pin LL_GPIO_PIN_8
#define DIO7_GPIO_Port GPIOB
#define DIO8_Pin LL_GPIO_PIN_9
#define DIO8_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
