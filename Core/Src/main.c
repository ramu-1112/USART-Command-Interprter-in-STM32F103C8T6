/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <string.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LED_PIN GPIO_PIN_13
#define LED_PORT GPIOC
#define UART_TIME_OUT 1000
#define CMD_SIZE 20
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

int idx=0;
char command[CMD_SIZE];
uint8_t temp[1];
uint8_t on_status[] = "\r\nLED=ON\r\n";
uint8_t off_status[] = "\r\nLED=OFF\r\n";
uint8_t help_command[] = "\r\nAvailable command\r\n"
						"LED ON: Turn on the LED\r\n"
						"LED OFF: Turn of the LED\r\n"
						"LED TOGGLE: Toggle the LED\r\n"
						"LED STATUS: Get the LED status\r\n";
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);

// Command Line Declaration
void Led_On(void);
void Led_Off(void);
void Led_Toggle(void);
void Led_Status(void);
void UART_Status(void);
void Help(void);
void Clear(void);

// Command managing
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void Select_Command(char buf[]);
void Print_Char(uint8_t byte);

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  HAL_UART_Receive_IT(&huart1, temp, 1);
  while (1)
  {

  }
}

void Led_On(){
	HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);
}

void Led_Off(){
	HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);
}

void Led_Toggle(){
	HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
}

void Led_Status(){
	uint16_t status = (GPIOC->ODR &= (1 << 13));
	if(status) HAL_UART_Transmit(&huart1, off_status, strlen(on_status), UART_TIME_OUT);
	else HAL_UART_Transmit(&huart1, on_status, strlen(off_status), UART_TIME_OUT);
	return;
}

void Help(){
	HAL_UART_Transmit(&huart1, help_command, strlen(help_command), UART_TIME_OUT);
	return;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	HAL_UART_Transmit(&huart1, temp, 1, UART_TIME_OUT);
	command[idx] = temp[0];
	idx++;
	if(temp[0] == '\r'){
		command[idx-1] = '\0';
		HAL_UART_Transmit(&huart1, command, strlen(command), UART_TIME_OUT);
		Select_Command(command);
		HAL_UART_Transmit(&huart1,"\r\n",2, UART_TIME_OUT);
		idx = 0;
	}
	if(idx >= CMD_SIZE-1) idx = 0;
	HAL_UART_Receive_IT(&huart1, temp, 1);
}
void Select_Command(char buf[]){
	int strcmp_res = strcmp(buf,"LED ON");
	if(strcmp_res==0)Led_On();
	else if(strcmp(buf, "LED OFF") == 0) Led_Off();
	else if(strcmp(buf, "LED TOGGLE") == 0) Led_Toggle();
	else if(strcmp(buf, "LED STATUS") == 0) Led_Status();
	else Help();
	return;
}

void Print_Char(uint8_t byte){
	HAL_UART_Transmit(&huart1, &byte, 1, UART_TIME_OUT);
	return;
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_USART1_UART_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
}
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line){}
#endif /* USE_FULL_ASSERT */
