/**
  ******************************************************************************
  * File Name          : main.hpp
  * Description        : This file contains the common defines of the application
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
  /* Includes ------------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define NCS_MEMS_SPI_Pin GPIO_PIN_0
#define NCS_MEMS_SPI_GPIO_Port GPIOC
#define MEMS_INT1_Pin GPIO_PIN_1
#define MEMS_INT1_GPIO_Port GPIOC
#define MEMS_INT1_EXTI_IRQn EXTI0_1_IRQn
#define MEMS_INT2_Pin GPIO_PIN_2
#define MEMS_INT2_GPIO_Port GPIOC
#define MEMS_INT2_EXTI_IRQn EXTI2_3_IRQn
#define B1_Pin GPIO_PIN_0
#define B1_GPIO_Port GPIOA
#define B1_EXTI_IRQn EXTI0_1_IRQn
#define EXT_RESET_Pin GPIO_PIN_5
#define EXT_RESET_GPIO_Port GPIOC
#define I2C2_SCL_Pin GPIO_PIN_10
#define I2C2_SCL_GPIO_Port GPIOB
#define I2C2_SDA_Pin GPIO_PIN_11
#define I2C2_SDA_GPIO_Port GPIOB
#define SPI2_SCK_Pin GPIO_PIN_13
#define SPI2_SCK_GPIO_Port GPIOB
#define SPI2_MISO_Pin GPIO_PIN_14
#define SPI2_MISO_GPIO_Port GPIOB
#define SPI2_MOSI_Pin GPIO_PIN_15
#define SPI2_MOSI_GPIO_Port GPIOB
#define LD3_Pin GPIO_PIN_6
#define LD3_GPIO_Port GPIOC
#define LD6_Pin GPIO_PIN_7
#define LD6_GPIO_Port GPIOC
#define LD4_Pin GPIO_PIN_8
#define LD4_GPIO_Port GPIOC
#define LD5_Pin GPIO_PIN_9
#define LD5_GPIO_Port GPIOC
#define USBF4_DM_Pin GPIO_PIN_11
#define USBF4_DM_GPIO_Port GPIOA
#define USBF4_DP_Pin GPIO_PIN_12
#define USBF4_DP_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define SPI_IRQ_Pin GPIO_PIN_3
#define SPI_IRQ_GPIO_Port GPIOB
#define SPI_IRQ_EXTI_IRQn EXTI2_3_IRQn
#define SPI1_CS_Pin GPIO_PIN_6
#define SPI1_CS_GPIO_Port GPIOB

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */
// board switching
//#define SLAVE1_BOARD
//#define SLAVE2_BOARD
//#define SLAVE3_BOARD

#define MASTER_BOARD

#define UART_PLUGGED

// Beacon positions
#define BEACONPOS1X 		0
#define BEACONPOS1Y			0
#define BEACONPOS2X 		0
#define BEACONPOS2Y			2000
#define BEACONPOS3X 		2800
#define BEACONPOS3Y			1000

#define STARTPOSITIONX	1400
#define STARTPOSITIONY	1600

/*
//Optimal
float beacon1[2] = {-50,-50};
float beacon2[2] = {3050,-50};
float beacon3[2] = {1500,2050};
*/

// Antenna calibration					
#define ANTENNA_DELAY 					0x8000 //Offset for error at -50cm  //0x8066 // precis 10cm 
#define THEORETICAL_DISTANCE		5.1

#define ADRESS_AND_PAN					0x000000FF

#ifdef SLAVE1_BOARD
#define SLAVE_BOARD
#define MASTER_FIRST_MESSAGE				0x11
#define MASTER_SECOND_MESSAGE				0x21
#define SLAVE_STANDARD_MESSAGE			0x1A
#endif

#ifdef SLAVE2_BOARD
#define SLAVE_BOARD
#define MASTER_FIRST_MESSAGE				0x12
#define MASTER_SECOND_MESSAGE				0x22
#define SLAVE_STANDARD_MESSAGE			0x2A
#endif

#ifdef SLAVE3_BOARD
#define SLAVE_BOARD
#define MASTER_FIRST_MESSAGE				0x13
#define MASTER_SECOND_MESSAGE				0x23
#define SLAVE_STANDARD_MESSAGE			0x3A
#endif

#define TX_OK_MASK							0x00000080 // TX OK
#define RX_FINISHED_MASK				0x00002000 // RX FINISHED
#define RX_NO_ERROR_MASK				0x00004000 // RX NO ERROR
#define RX_ERROR_MASK						0x00001000 // RX ERROR
#define RX_TIMEOUT_MASK					0x00020000 // RX TIMEOUT

// State Machine Master
#define STATE_INIT							1
#define STATE_WAIT_FIRST_SEND		2
#define STATE_WAIT_RESPONSE			3
#define STATE_WAIT_SECOND_SEND	4
#define STATE_GET_TIMES					5
#define STATE_COMPUTE_DISTANCE	6
#define END_STATE								7

// State Machine Slave
#define STATE_INIT							1
#define STATE_WAIT_RECEIVE			2
#define STATE_MESSAGE_1					3
#define STATE_MESSAGE_2					4
#define STATE_SEND_TIMES				5
#define STATE_END_CYCLE					6
#define STATE_SEND_RESPONSE			7


/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

#endif /* __MAIN_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
