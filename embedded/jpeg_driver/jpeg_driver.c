/**
  ******************************************************************************
  * @file    JPEG/JPEG_EncodingFromFLASH_DMA/Src/main.c
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    30-December-2016
  * @brief   This sample code shows how to use the HW JPEG to Encode an RGB image 
  *          from FLASH with DMA method.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
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
#include "jpeg_driver.h"
#include "image_320_240_rgb.h"
#include <rtthread.h>
#include "finsh.h"
/** @addtogroup STM32F7xx_HAL_Examples
  * @{
  */

/** @addtogroup JPEG_EncodingFromFLASH_DMA
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

JPEG_HandleTypeDef    JPEG_Handle;
uint32_t RGB_ImageAddress;

uint8_t  jpeg_data[16];

static void jpeg_main(void *parameter)
{
  uint32_t JpegEncodeProcessing_End = 0;
  
#if (JPEG_RGB_FORMAT == JPEG_ARGB8888)
  RGB_ImageAddress = (uint32_t)Image_ARGB8888;

#elif(JPEG_RGB_FORMAT == JPEG_RGB888)
  RGB_ImageAddress = (uint32_t)Image_RGB888;
  
#elif(JPEG_RGB_FORMAT == JPEG_RGB565)
  RGB_ImageAddress = (uint32_t)Image_RGB565;
  
#endif /* JPEG_RGB_FORMAT */
  
	rt_kprintf("jpeg thread running...\n");
  while(1){
		rt_thread_delay(RT_TICK_PER_SECOND/2);
	}
	
  /*##-1- JPEG Initialization ################################################*/   
  /* Init The JPEG Color Look Up Tables used for YCbCr to RGB conversion   */ 
  JPEG_InitColorTables();
	
	
	JPEG_Encode_DMA(&JPEG_Handle, RGB_ImageAddress, RGB_IMAGE_SIZE, jpeg_data);
	
	do{
		JPEG_EncodeInputHandler(&JPEG_Handle);
		JpegEncodeProcessing_End = JPEG_EncodeOutputHandler(&JPEG_Handle);
	}while(JpegEncodeProcessing_End == 0);
  
  /* Infinite loop */
  while (1)
  {
  }
}

/**
  * @brief  Get the images sizes from BMP header.
  * @param  pInfo : pointer to the Info structure
  * @retval None
  */
void RGB_GetInfo(JPEG_ConfTypeDef *pInfo)
{
  /* Read Images Sizes */
  pInfo->ImageWidth         = RGB_IMAGE_WIDTH;
  pInfo->ImageHeight        = RGB_IMAGE_HEIGHT;

  /* Jpeg Encoding Setting to be setted by users */
  pInfo->ChromaSubsampling  = JPEG_CHROMA_SAMPLING;
  pInfo->ColorSpace         = JPEG_COLOR_SPACE;
  pInfo->ImageQuality       = JPEG_IMAGE_QUALITY;
    
  /*Check if Image Sizes meets the requirements */
  if (((pInfo->ImageWidth % 8) != 0 ) || ((pInfo->ImageHeight % 8) != 0 ) || \
      (((pInfo->ImageWidth % 16) != 0 ) && (pInfo->ColorSpace == JPEG_YCBCR_COLORSPACE) && (pInfo->ChromaSubsampling != JPEG_444_SUBSAMPLING)) || \
      (((pInfo->ImageHeight % 16) != 0 ) && (pInfo->ColorSpace == JPEG_YCBCR_COLORSPACE) && (pInfo->ChromaSubsampling == JPEG_420_SUBSAMPLING)))
  {
		while(1);
  }
  
}

int jpeg_init(void)
{
    rt_thread_t tid;

    tid = rt_thread_create("jpeg",
                           jpeg_main, RT_NULL,
                           512, 13, 5);

    if (tid != RT_NULL)
        rt_thread_startup(tid);

    return 0;
}
//INIT_APP_EXPORT(jpeg_init);

long jpeg(void)
{
    rt_kprintf("jpeg RT-Thread!\n");

    return 0;
}

FINSH_FUNCTION_EXPORT(jpeg, say jpeg world);


void OnError_Handler(void)
{
  while(1) { ; } /* Blocking on error */
}

INIT_APP_EXPORT(jpeg_init);


