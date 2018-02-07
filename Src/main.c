/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f0xx_hal.h"

/* USER CODE BEGIN Includes */
#include "EventRecorder.h"              // Keil.ARM Compiler::Compiler:Event Recorder
#include "stdio.h"
#include "dwm1000.h"
#include <string.h>
#include <inttypes.h>
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
uint8_t RxData[128];
uint8_t TxData[128];

uint64_t t1, t2, t3, t4, t5, t6;
uint8_t t1_8[5];
uint8_t t2_8[5];
uint8_t t3_8[5];
uint8_t t4_8[5];
uint8_t t5_8[5];
uint8_t t6_8[5];

double tof;

float distance;

// Boolean variables
uint8_t TxOk = 0;
uint8_t RxOk = 0;
uint8_t RxError = 0;

int state;

// UART Variables

int uartLen;
char uartBuffer[100];
char uartRx_indx, uartRx_data[2],uartRxBuffer[100], uartTransfer_cplt;
int uartPress_enter = 0;

// antenna calibration variables
int measure_counter = 0;
float moy_distance = 0;
float moy_tof = 0;
float sum_square = 0;
int old_antenna_delay = ANTENNA_DELAY;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */
	EventRecorderInitialize (EventRecordAll, 1);
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();

  /* USER CODE BEGIN 2 */
	_deviceHandle = &hspi1;					// Assign SPI handle

	/* initialisation of the DecaWave */
	HAL_Delay(10); //time for the DW to go from Wakeup to init and then IDLE
	DWM_Init();
	state = STATE_INIT;
	
	HAL_UART_Receive_IT(&huart1, (uint8_t *)uartRx_data, 1);
//	#ifdef UART_PLUGGED
//	__disable_irq();
//	uartLen = sprintf(uartBuffer, "Start \n");
//	HAL_UART_Transmit(&huart1, (uint8_t *)uartBuffer, uartLen, HAL_MAX_DELAY);
//	__enable_irq();
//	#endif
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
#ifdef MASTER_BOARD
		switch (state){
			case STATE_INIT :
					// IDLE to save Power
					DWM_Disable_Rx();
					HAL_GPIO_WritePin(GPIOC, LD3_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOC, LD4_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOC, LD5_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOC, LD6_Pin, GPIO_PIN_RESET);
					
					HAL_Delay(1); // 1msec between 2 measures
				
					//Send first data
					TxData[0] = MASTER_FIRST_MESSAGE;
					DWM_SendData(TxData, 1);
				
					//Change state to wait TX OK (polling)
					state = STATE_WAIT_FIRST_SEND;
			break;
			
			case STATE_WAIT_FIRST_SEND:
				if (TxOk){
					//get tx time (T1)
					DWM_ReadSPI_ext(TX_TIME, NO_SUB, t1_8, 5);
					state = STATE_WAIT_RESPONSE;
					TxOk = 0;
					HAL_GPIO_WritePin(GPIOC, LD5_Pin, GPIO_PIN_SET);
				}	
			break;
				
			case STATE_WAIT_RESPONSE:
				if (RxError){
					RxError = 0;
					state = STATE_INIT;
				}
				if (RxOk){
					// Read Rx buffer
					DWM_ReceiveData(RxData);
					// Check RxFrame
					if (RxData[0] == SLAVE_STANDARD_MESSAGE){
						//get rx time (t4)
						DWM_ReadSPI_ext(RX_TIME, NO_SUB, t4_8, 5);
						
						//Send second time
						//HAL_Delay(1);
						TxData[0] = MASTER_SECOND_MESSAGE;
						DWM_SendData(TxData, 1);
						state = STATE_WAIT_SECOND_SEND;
						HAL_GPIO_WritePin(GPIOC, LD6_Pin, GPIO_PIN_SET);
					}
					else {
						state = STATE_INIT;
					}
					RxOk = 0;
				}
			break;
				
			case STATE_WAIT_SECOND_SEND:
					if (TxOk){
					//get tx time (T5)
						DWM_ReadSPI_ext(TX_TIME, NO_SUB, t5_8, 5);
						state = STATE_GET_TIMES;
						TxOk = 0;
				}
			break;
			
			case STATE_GET_TIMES:
				if (RxError){
					state = STATE_INIT;
					RxError = 0;
				}
				if (RxOk){
					//Read Rx Buffer
					DWM_ReceiveData(RxData);
					
					HAL_GPIO_WritePin(GPIOC, LD4_Pin, GPIO_PIN_SET);
					for (int i=0;i<5;i++){
						t2_8[i] = RxData[i];
						t3_8[i] = RxData[i+5];
						t6_8[i] = RxData[i+10];
					}
					// Cast all times to uint64
					t1 = t2 = t3 = t4 = t5 = t6 = 0;
					for (int i=0;i<5;i++){
						t1 = (t1 << 8) | t1_8[4-i];
						t2 = (t2 << 8) | t2_8[4-i];
						t3 = (t3 << 8) | t3_8[4-i];
						t4 = (t4 << 8) | t4_8[4-i];
						t5 = (t5 << 8) | t5_8[4-i];
						t6 = (t6 << 8) | t6_8[4-i];
					}
					if (t6 < t2 || t5 < t1){
						state = STATE_INIT;
					}
					else{
						state = STATE_COMPUTE_DISTANCE;
						RxOk = 0;
					}
				}
			break;
				
			case STATE_COMPUTE_DISTANCE :{
				uint64_t TroundA = (t4-t1);
				uint64_t TreplyB = (t3-t2);
				uint64_t TroundB = (t6-t3);
				uint64_t TreplyA = (t5-t4);
				tof = (TroundA + TroundB) - (TreplyA + TreplyB);
				tof = tof /4;
//				#ifdef UART_PLUGGED  //Printing the treply IN DW UNIT
//				__disable_irq();
//				uartLen = sprintf(uartBuffer,"TrepB : %" PRIu64 "/ TrepA : %" PRIu64 "\n", TreplyB, TreplyA);
//				HAL_UART_Transmit(&huart1, (uint8_t *)uartBuffer, uartLen, HAL_MAX_DELAY);
//				__enable_irq();
//				#endif
				if (TreplyB > TroundA){
					tof = 0;
					distance = 0;
				}
				else{
					double distancepicosec = tof/(128*499.2);
					distance = distancepicosec * 299792458 * 0.000001;
				}
				if (distance < 100){
					// antenna tunning
					measure_counter++;
					float delta = distance - moy_distance;
					moy_distance = moy_distance + (delta/measure_counter);
					float delta2 = distance - moy_distance;
					sum_square = sum_square + delta*delta2;
					moy_tof = moy_tof + ((tof-moy_tof)/measure_counter);
					#ifdef UART_PLUGGED
					__disable_irq();
					uartLen = sprintf(uartBuffer,"%f\n",distance);
					//uartLen = sprintf(uartBuffer, "Distance = %f / Moyenne = %f / mesure %d / ant %d \r\n", distance, moy_distance, measure_counter, old_antenna_delay);
					HAL_UART_Transmit(&huart1, (uint8_t *)uartBuffer, uartLen, HAL_MAX_DELAY);
					__enable_irq();
					#endif
				}
				
				if (measure_counter >1000){
					float tof_theorique = (THEORETICAL_DISTANCE/(299702547 * 0.000001))*128*499.2;
					int ant_error = (moy_tof-tof_theorique);
					float variance = sum_square / (measure_counter -1 );
					#ifdef UART_PLUGGED
					__disable_irq();
					uartLen = sprintf(uartBuffer, "end antenna error : 0x%04X /distance : %f / var : %f\n", ant_error, moy_distance, variance);
					HAL_UART_Transmit(&huart1, (uint8_t *)uartBuffer, uartLen, HAL_MAX_DELAY);
					__enable_irq();
					#endif
					while (!uartPress_enter){
						HAL_GPIO_TogglePin(GPIOC, LD3_Pin);
						HAL_Delay(100);
					}
					measure_counter = 0;
					uartPress_enter = 0;
					moy_distance = 0;
					sum_square = 0;
					#ifdef UART_PLUGGED
					__disable_irq();
					uartLen = sprintf(uartBuffer, "Restart\r \n");
					HAL_UART_Transmit(&huart1, (uint8_t *)uartBuffer, uartLen, HAL_MAX_DELAY);
					__enable_irq();
					#endif
					state = STATE_INIT;
				}
				state = STATE_INIT;
			break;}
		}
#endif
		
#ifdef SLAVE_BOARD
		switch (state){
			case STATE_INIT :
				DWM_Enable_Rx();
				HAL_Delay(1);
				state = STATE_WAIT_RECEIVE;
				HAL_GPIO_WritePin(GPIOC, LD3_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOC, LD4_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOC, LD5_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOC, LD6_Pin, GPIO_PIN_RESET);
			break;
			
			case STATE_WAIT_RECEIVE :
				if (RxError){
					state = STATE_INIT;
					RxError = 0;
				}
				if (RxOk){
					// Read Rx buffer
					DWM_ReceiveData(RxData);				
					// Check RxFrame
					if (RxData[0] == MASTER_FIRST_MESSAGE){
						state = STATE_MESSAGE_1;
						HAL_GPIO_WritePin(GPIOC, LD5_Pin, GPIO_PIN_SET);
					}
					if (RxData[0] == MASTER_SECOND_MESSAGE){
						state = STATE_MESSAGE_2;
						HAL_GPIO_WritePin(GPIOC, LD4_Pin, GPIO_PIN_SET);
					}
					RxOk = 0;	
				}
			break;
				
			case STATE_MESSAGE_1:
				// get T2
				DWM_ReadSPI_ext(RX_TIME,NO_SUB, t2_8,5);
				//HAL_Delay(1);
				TxData[0] = SLAVE_STANDARD_MESSAGE;
				DWM_SendData(TxData, 1);
				state = STATE_SEND_RESPONSE;
			break;
			
			case STATE_SEND_RESPONSE :
				if (TxOk){
					//get tx time (T3)
					HAL_GPIO_WritePin(GPIOC, LD6_Pin, GPIO_PIN_SET);
					DWM_ReadSPI_ext(TX_TIME, NO_SUB, t3_8, 5);
					TxOk = 0;
					state = STATE_WAIT_RECEIVE;
				}
			break;
			
			case STATE_MESSAGE_2 :
				//get T6
				DWM_ReadSPI_ext(RX_TIME, NO_SUB, t6_8,5);
				state = STATE_SEND_TIMES;
			break;
			
			case STATE_SEND_TIMES :
				for (int i=0; i<5; i++){
					TxData[i] = t2_8[i];
					TxData[i+5] = t3_8[i];
					TxData[i+10] = t6_8[i];
				}
				DWM_SendData(TxData, 15);
				
				state = STATE_END_CYCLE;				
			break;
				
			case STATE_END_CYCLE :
				if (TxOk){
					TxOk = 0;
					state = STATE_INIT;
				}	
			break;		
		}
#endif
  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* SPI1 init function */
static void MX_SPI1_Init(void)
{

  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
     PA2   ------> TSC_G1_IO3
     PA3   ------> TSC_G1_IO4
     PA6   ------> TSC_G2_IO3
     PA7   ------> TSC_G2_IO4
     PB0   ------> TSC_G3_IO2
     PB1   ------> TSC_G3_IO3
     PB10   ------> I2C2_SCL
     PB11   ------> I2C2_SDA
     PB13   ------> SPI2_SCK
     PB14   ------> SPI2_MISO
     PB15   ------> SPI2_MOSI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, NCS_MEMS_SPI_Pin|EXT_RESET_Pin|LD3_Pin|LD6_Pin 
                          |LD4_Pin|LD5_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : NCS_MEMS_SPI_Pin EXT_RESET_Pin LD3_Pin LD6_Pin 
                           LD4_Pin LD5_Pin */
  GPIO_InitStruct.Pin = NCS_MEMS_SPI_Pin|EXT_RESET_Pin|LD3_Pin|LD6_Pin 
                          |LD4_Pin|LD5_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : MEMS_INT1_Pin MEMS_INT2_Pin */
  GPIO_InitStruct.Pin = MEMS_INT1_Pin|MEMS_INT2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PA2 PA3 PA6 PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF3_TSC;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF3_TSC;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : I2C2_SCL_Pin I2C2_SDA_Pin */
  GPIO_InitStruct.Pin = I2C2_SCL_Pin|I2C2_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF1_I2C2;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : SPI2_SCK_Pin SPI2_MISO_Pin SPI2_MOSI_Pin */
  GPIO_InitStruct.Pin = SPI2_SCK_Pin|SPI2_MISO_Pin|SPI2_MOSI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF0_SPI2;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : SPI_IRQ_Pin */
  GPIO_InitStruct.Pin = SPI_IRQ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(SPI_IRQ_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SPI1_CS_Pin */
  GPIO_InitStruct.Pin = SPI1_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SPI1_CS_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);

  HAL_NVIC_SetPriority(EXTI2_3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);

}

/* USER CODE BEGIN 4 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if (GPIO_Pin != SPI_IRQ_Pin){return;}
	uint8_t RxBuffer[4];
	uint32_t StatusRegister;
	uint8_t ack[4];
	memset(ack, 0, 4);	
	// Getting status Register
	DWM_ReadSPI_ext(SYS_STATUS, NO_SUB,  RxBuffer, 4);
	StatusRegister  = (RxBuffer[3] << 24) | (RxBuffer[2] << 16) | (RxBuffer[1] << 8) | RxBuffer[0];
	if (StatusRegister == 0xFFFFFFFF) {
		// Read again
		DWM_ReadSPI_ext(SYS_STATUS, NO_SUB,  RxBuffer, 4);
		StatusRegister  = (RxBuffer[3] << 24) | (RxBuffer[2] << 16) | (RxBuffer[1] << 8) | RxBuffer[0];
	}
	if (StatusRegister & TX_OK_MASK){
		TxOk = 1;
		setBit(ack, 4, TX_OK_BIT, 1);
	}
	// check if RX finished
	if (StatusRegister & RX_FINISHED_MASK){
		setBit(ack, 4, RX_FINISHED_BIT, 1);
		// check in no error in RX
		if (StatusRegister & RX_NO_ERROR_MASK){
			RxOk = 1;
			setBit(ack, 4, RX_NO_ERROR_BIT, 1);
		}
	}
	if ((StatusRegister & RX_ERROR_MASK) | (StatusRegister & RX_TIMEOUT_MASK)){
		RxError = 1;
		setBit(ack,4,12,1);
		setBit(ack,4,17,1);
	}
	// clear IRQ flags on DW
	DWM_WriteSPI_ext(SYS_STATUS ,NO_SUB, ack, 4);
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if (huart->Instance == USART1){  //current UART
		if (uartRx_data[0]==13){ //if received data is ascii 13 (enter)
			uartPress_enter = 1;
		}	
		HAL_UART_Receive_IT(&huart1, (uint8_t *)uartRx_data, 1);
	}
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
