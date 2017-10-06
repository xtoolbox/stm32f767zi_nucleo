/**
  ******************************************************************************
  * @file           : usbd_conf.h
  * @version        : v1.0_Cube
  * @brief          : Header for usbd_conf file.
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_CONF__H__
#define __USBD_CONF__H__
#ifdef __cplusplus
 extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"
#include <rtthread.h>

/** @addtogroup USBD_OTG_DRIVER
  * @{
  */
  
/** @defgroup USBD_CONF
  * @brief usb otg low level driver configuration file
  * @{
  */ 

/** @defgroup USBD_CONF_Exported_Defines
  * @{
  */ 

/*---------- -----------*/
#define USBD_MAX_NUM_INTERFACES     3
/*---------- -----------*/
#define USBD_MAX_NUM_CONFIGURATION     1
/*---------- -----------*/
#define USBD_MAX_STR_DESC_SIZ     512
/*---------- -----------*/
#define USBD_SUPPORT_USER_STRING     0
/*---------- -----------*/
#define USBD_DEBUG_LEVEL     0
/*---------- -----------*/
#define USBD_LPM_ENABLED     1
/*---------- -----------*/
#define USBD_SELF_POWERED     1
/*---------- -----------*/
#define USBD_VIDEO_FREQ     22100

/****************************************/
/* #define for FS and HS identification */
#define DEVICE_FS 		0
#define DEVICE_HS 		1


#define WIDTH                                         320
#define HEIGHT                                        240
#define CAM_FPS                                       10
#define VIDEO_PACKET_SIZE                             (512)//(130+128) // max 368 bytes
#define VIDEO_XFER_SIZE                               (512) // we can modify the hal library to send mutilple ISO data packet

#define VIDEO_EP_TYPE                                 USBD_EP_TYPE_ISOC
#define VIEDO_EP_TYPE_DESC                            (USBD_EP_TYPE_ISOC | 0x00)

#define VIDEO_BUFFER_SIZE                             1024*30
#define CONTROL_PACKET_SIZE                           64
#define MIN_BIT_RATE                                  (20*1024*CAM_FPS)
#define MAX_BIT_RATE                                  (40*1024*CAM_FPS)
#define MAX_FRAME_SIZE                                (20*1024)

#define INTERVEL                                      (10000000ul/CAM_FPS)
#define MAKE_WORD(x)                                  (uint8_t)((x)&0xFF),(uint8_t)(((x)>>8)&0xFF)
#define MAKE_DWORD(x)                                 (uint8_t)((x)&0xFF),(uint8_t)(((x)>>8)&0xFF),(uint8_t)(((x)>>16)&0xFF),(uint8_t)(((x)>>24)&0xFF)


#define USB_DESC_TYPE_ASSOCIATION               0x0B
#define USB_DESC_TYPE_HID                       0x21


/** @defgroup USBD_Exported_Macros
  * @{
  */ 

 /* Memory management macros */   
#define USBD_malloc               rt_malloc
#define USBD_free                 rt_free
#define USBD_memset               memset
#define USBD_memcpy               memcpy

#define USBD_Delay   HAL_Delay
    
 /* DEBUG macros */  

#if (USBD_DEBUG_LEVEL > 0)
#define  USBD_UsrLog(...)   rt_kprintf(__VA_ARGS__);\
                            rt_kprintf("\n");
#else
#define USBD_UsrLog(...)   
#endif 
                            
                            
#if (USBD_DEBUG_LEVEL > 1)

#define  USBD_ErrLog(...)   rt_kprintf("ERROR: ") ;\
                            rt_kprintf(__VA_ARGS__);\
                            rt_kprintf("\n");
#else
#define USBD_ErrLog(...)   
#endif 
                            
                            
#if (USBD_DEBUG_LEVEL > 2)                         
#define  USBD_DbgLog(...)   printf("DEBUG : ") ;\
                            printf(__VA_ARGS__);\
                            printf("\n");
#else
#define USBD_DbgLog(...)                         
#endif
                            
/**
  * @}
  */ 
 
    
    
/**
  * @}
  */ 

/** @defgroup USBD_CONF_Exported_Types
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USBD_CONF_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USBD_CONF_Exported_Variables
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USBD_CONF_Exported_FunctionsPrototype
  * @{
  */ 
/**
  * @}
  */ 
#ifdef __cplusplus
}
#endif

#endif /*__USBD_CONF__H__*/

/**
  * @}
  */ 

/**
  * @}
  */ 
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

