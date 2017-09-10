/**
  ******************************************************************************
  * File Name          : stm32f7xx_hal_msp.c
  * Description        : This file provides code for the MSP Initialization 
  *                      and de-Initialization codes.
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"

extern DMA_HandleTypeDef hdma_jpeg_in;
extern DMA_HandleTypeDef hdma_jpeg_out;

extern void _Error_Handler(char *, int);
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* System interrupt init*/
  /* MemoryManagement_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
  /* BusFault_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
  /* UsageFault_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
  /* SVCall_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SVCall_IRQn, 0, 0);
  /* DebugMonitor_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DebugMonitor_IRQn, 0, 0);
  /* PendSV_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(PendSV_IRQn, 0, 0);
  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

void HAL_JPEG_MspInit(JPEG_HandleTypeDef* hjpeg)
{

  if(hjpeg->Instance==JPEG)
  {
  /* USER CODE BEGIN JPEG_MspInit 0 */

  /* USER CODE END JPEG_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_JPEG_CLK_ENABLE();
  
    /* JPEG DMA Init */
    /* Enable DMA clock */
  __DMA2_CLK_ENABLE();
		
    /* JPEG_IN Init */
    hdma_jpeg_in.Instance = DMA2_Stream0;
    hdma_jpeg_in.Init.Channel = DMA_CHANNEL_9;
    hdma_jpeg_in.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_jpeg_in.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_jpeg_in.Init.MemInc = DMA_MINC_ENABLE;
    hdma_jpeg_in.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_jpeg_in.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_jpeg_in.Init.Mode = DMA_NORMAL;
    hdma_jpeg_in.Init.Priority = DMA_PRIORITY_LOW;
    hdma_jpeg_in.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_jpeg_in.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_jpeg_in.Init.MemBurst = DMA_MBURST_INC4;
    hdma_jpeg_in.Init.PeriphBurst = DMA_PBURST_INC4;
    if (HAL_DMA_Init(&hdma_jpeg_in) != HAL_OK)
    {
      _Error_Handler(__FILE__, __LINE__);
    }

    __HAL_LINKDMA(hjpeg,hdmain,hdma_jpeg_in);

    /* JPEG interrupt Init */
    HAL_NVIC_SetPriority(JPEG_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(JPEG_IRQn);
  /* USER CODE BEGIN JPEG_MspInit 1 */

		
		
  /* Output DMA */
  /* Set the parameters to be configured */ 
  hdma_jpeg_out.Init.Channel = DMA_CHANNEL_9;
  hdma_jpeg_out.Init.Direction = DMA_PERIPH_TO_MEMORY;
  hdma_jpeg_out.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_jpeg_out.Init.MemInc = DMA_MINC_ENABLE;
  hdma_jpeg_out.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  hdma_jpeg_out.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
  hdma_jpeg_out.Init.Mode = DMA_NORMAL;
  hdma_jpeg_out.Init.Priority = DMA_PRIORITY_VERY_HIGH;
  hdma_jpeg_out.Init.FIFOMode = DMA_FIFOMODE_ENABLE;         
  hdma_jpeg_out.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
  hdma_jpeg_out.Init.MemBurst = DMA_MBURST_INC4;
  hdma_jpeg_out.Init.PeriphBurst = DMA_PBURST_INC4;

  
  hdma_jpeg_out.Instance = DMA2_Stream1;
  /* DeInitialize the DMA Stream */
  HAL_DMA_DeInit(&hdma_jpeg_out);  
  /* Initialize the DMA stream */
  HAL_DMA_Init(&hdma_jpeg_out);

  /* Associate the DMA handle */
  __HAL_LINKDMA(hjpeg, hdmaout, hdma_jpeg_out);
  
	HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0x07, 0x0F);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);    
	
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0x07, 0x0F);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);  


  /* USER CODE END JPEG_MspInit 1 */
  }

}

void HAL_JPEG_MspDeInit(JPEG_HandleTypeDef* hjpeg)
{

  if(hjpeg->Instance==JPEG)
  {
  /* USER CODE BEGIN JPEG_MspDeInit 0 */

  /* USER CODE END JPEG_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_JPEG_CLK_DISABLE();

    /* JPEG DMA DeInit */
    HAL_DMA_DeInit(hjpeg->hdmain);

    /* JPEG interrupt DeInit */
    HAL_NVIC_DisableIRQ(JPEG_IRQn);
  /* USER CODE BEGIN JPEG_MspDeInit 1 */

  /* USER CODE END JPEG_MspDeInit 1 */
  }

}

void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(huart->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspInit 0 */

  /* USER CODE END USART3_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();
  
    /**USART3 GPIO Configuration    
    PD8     ------> USART3_TX
    PD9     ------> USART3_RX 
    */
    GPIO_InitStruct.Pin = STLK_RX_Pin|STLK_TX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* USART3 interrupt Init */
    HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspInit 1 */

  /* USER CODE END USART3_MspInit 1 */
  }

}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{

  if(huart->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspDeInit 0 */

  /* USER CODE END USART3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();
  
    /**USART3 GPIO Configuration    
    PD8     ------> USART3_TX
    PD9     ------> USART3_RX 
    */
    HAL_GPIO_DeInit(GPIOD, STLK_RX_Pin|STLK_TX_Pin);

    /* USART3 interrupt DeInit */
    HAL_NVIC_DisableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspDeInit 1 */

  /* USER CODE END USART3_MspDeInit 1 */
  }

}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
