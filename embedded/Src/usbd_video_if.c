/**
  ******************************************************************************
  * @file           : usbd_video_if.c
  * @brief          : Generic media access Layer.
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
#include "usbd_video_if.h"
#include "rtthread.h"
#include "main.h"
#include "jpeg_driver.h"
/* USER CODE BEGIN INCLUDE */
/* USER CODE END INCLUDE */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */

/** @defgroup USBD_VIDEO 
  * @brief usbd core module
  * @{
  */ 

/** @defgroup USBD_VIDEO_Private_TypesDefinitions
  * @{
  */ 
/* USER CODE BEGIN PRIVATE_TYPES */
/* USER CODE END PRIVATE_TYPES */ 
/**
  * @}
  */ 

/** @defgroup USBD_VIDEO_Private_Defines
  * @{
  */ 
/* USER CODE BEGIN PRIVATE_DEFINES */
/* USER CODE END PRIVATE_DEFINES */
  
/**
  * @}
  */ 

/** @defgroup USBD_VIDEO_Private_Macros
  * @{
  */ 
/* USER CODE BEGIN PRIVATE_MACRO */
/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */ 

/** @defgroup USBD_VIDEO_IF_Private_Variables
  * @{
  */
/* USER CODE BEGIN PRIVATE_VARIABLES */
/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */ 
  
/** @defgroup USBD_VIDEO_IF_Exported_Variables
  * @{
  */ 
  extern USBD_HandleTypeDef hUsbDeviceFS;
/* USER CODE BEGIN EXPORTED_VARIABLES */
/* USER CODE END EXPORTED_VARIABLES */

/**
  * @}
  */ 
  
/** @defgroup USBD_VIDEO_Private_FunctionPrototypes
  * @{
  */
static int8_t  VIDEO_Init_FS         (uint32_t  AudioFreq, uint32_t Volume, uint32_t options);
static int8_t  VIDEO_DeInit_FS       (uint32_t options);
static int8_t  VIDEO_ControlEvent_FS (control_t* ctrl);
static int8_t  VIDEO_CameraOn_FS (void);
static int8_t  VIDEO_CameraOff_FS (void);
static int8_t  VIDEO_FrameDone_FS (void);

/* USER CODE BEGIN PRIVATE_FUNCTIONS_DECLARATION */
/* USER CODE END PRIVATE_FUNCTIONS_DECLARATION */

/**
  * @}
  */ 
  
USBD_VIDEO_ItfTypeDef USBD_VIDEO_fops_FS = 
{
  VIDEO_Init_FS,
  VIDEO_DeInit_FS,
	VIDEO_ControlEvent_FS,
	VIDEO_CameraOn_FS,
	VIDEO_CameraOff_FS,
	VIDEO_FrameDone_FS,
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  VIDEO_Init_FS
  *         Initializes the VIDEO media low layer over USB FS IP
  * @param  AudioFreq: Audio frequency used to play the audio stream.
  * @param  Volume: Initial volume level (from 0 (Mute) to 100 (Max))
  * @param  options: Reserved for future use 
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t VIDEO_Init_FS(uint32_t  AudioFreq, uint32_t Volume, uint32_t options)
{ 
  /* USER CODE BEGIN 0 */
  return (USBD_OK);
  /* USER CODE END 0 */
}

/**
  * @brief  VIDEO_DeInit_FS
  *         DeInitializes the VIDEO media low layer
  * @param  options: Reserved for future use
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t VIDEO_DeInit_FS(uint32_t options)
{
  /* USER CODE BEGIN 1 */ 
  return (USBD_OK);
  /* USER CODE END 1 */
}


int8_t  VIDEO_ControlEvent_FS (control_t* ctrl)
{
	// get new cursor x,y postion, and new button state
	memcpy(&jpeg_control, ctrl, sizeof(control_t));
	rt_mb_send(jpeg_mb, JPEG_CONTROL);
	return (USBD_OK);
}

int8_t  VIDEO_CameraOn_FS (void)
{
	GPIOB->BSRR = LD2_Pin;
	rt_mb_send(jpeg_mb, JPEG_CAMERA_ON);
	return (USBD_OK);
}

int8_t  VIDEO_CameraOff_FS (void)
{
	GPIOB->BSRR = LD2_Pin<<16;
	rt_mb_send(jpeg_mb, JPEG_CAMERA_OFF);
	return (USBD_OK);
}

int8_t  VIDEO_FrameDone_FS (void)
{
	rt_mb_send(jpeg_mb, JPEG_FRAME_DONE);
	return (USBD_OK);
}

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */
/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */

/**
  * @}
  */ 

/**
  * @}
  */  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

