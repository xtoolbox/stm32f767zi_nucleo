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
#include "usbd_video.h"
/** @addtogroup STM32F7xx_HAL_Examples
  * @{
  */

/** @addtogroup JPEG_EncodingFromFLASH_DMA
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define   JPEG_SHOW_SRC   1
#define   JPEG_SHOW_DST   2
#define   JPEG_ENCODE     3
#define   JPEG_DECODE     4
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

extern JPEG_HandleTypeDef hjpeg;
extern USBD_HandleTypeDef hUsbDeviceFS;
uint32_t RGB_ImageAddress;

uint8_t  jpeg_data[32*1024];
jpeg_dest_t jout;

rt_mailbox_t  jpeg_mb = 0;
control_t     jpeg_control;

uint8_t*  show_addr = 0;
uint32_t  show_length = 0;
uint32_t  camera_on = 0;

static void show_data(void)
{
	uint32_t len = show_length;
	uint8_t* pos = show_addr;
	uint32_t i = 0;
	if(len <= 0) len = 16;
	if(len > 1024*20) len = 1024*20;
	for(i=0;i<len;++i){
		rt_kprintf("%02x ", *pos);
		pos++;
		if( (i&15) == 15){
			rt_kprintf("\n");
		}
	}
}

int8_t  VIDEO_Start_Transmit_Video(USBD_HandleTypeDef *pdev, uint32_t length);

static void jpeg_main(void *parameter)
{
  uint32_t JpegEncodeProcessing_End = 0;
	rt_uint32_t mb_value = 0;
  
#if (JPEG_RGB_FORMAT == JPEG_ARGB8888)
  RGB_ImageAddress = (uint32_t)Image_ARGB8888;

#elif(JPEG_RGB_FORMAT == JPEG_RGB888)
  RGB_ImageAddress = (uint32_t)Image_RGB888;
  
#elif(JPEG_RGB_FORMAT == JPEG_RGB565)
  RGB_ImageAddress = (uint32_t)IMAGE_ADDRESS;//Image_RGB565;
  
#endif /* JPEG_RGB_FORMAT */
	
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->CYCCNT = 0;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
	
	jpeg_mb = rt_mb_create("jpeg_mb", 16, 0);
	if(!jpeg_mb){
		rt_kprintf("rt_mb_create fail\n");
		while(1)rt_thread_delay(RT_TICK_PER_SECOND/2);
	}
	JPEG_InitColorTables();
	rt_kprintf("jpeg thread running...\n");
	//rt_mb_send(jpeg_mb, JPEG_DECODE);
	//rt_thread_delay(1000);
  while(1){
		rt_mb_recv(jpeg_mb, &mb_value, RT_WAITING_FOREVER);
		switch(mb_value){
			case JPEG_SHOW_SRC:
			case JPEG_SHOW_DST:
				show_data();
				break;
			case JPEG_ENCODE:
				rt_kprintf("start encode \n");
				jout.data = jpeg_data;
				jout.current_length = 0;
				jout.total_length = sizeof(jpeg_data);
				JPEG_Encode_DMA(&hjpeg, RGB_ImageAddress, RGB_IMAGE_HEIGHT * RGB_IMAGE_WIDTH *2, &jout);
				do{
					JPEG_EncodeInputHandler(&hjpeg);
					JpegEncodeProcessing_End = JPEG_EncodeOutputHandler(&hjpeg);
				}while(JpegEncodeProcessing_End == 0);
				rt_kprintf("encode done %d (%x)\n", jout.current_length, jout.current_length);
				
				if(camera_on){
					VIDEO_Send_Video(&hUsbDeviceFS, jout.data, jout.current_length);
				}
				break;
			case JPEG_DECODE:
				rt_kprintf("start decode not implemnet\n");
				break;
			case JPEG_CAMERA_ON:
				rt_kprintf("camera on\n");
				camera_on = 1;
			  rt_mb_send(jpeg_mb, JPEG_ENCODE);
				break;
			case JPEG_CAMERA_OFF:
				rt_kprintf("camera off\n");
				camera_on = 0;
				break;
			case JPEG_CONTROL:
				rt_kprintf("control x:%d, y:%d, btn:%x\n", jpeg_control.x, jpeg_control.y, jpeg_control.button);
				break;
			case JPEG_FRAME_DONE:
				if(camera_on){
					// TODO : we can update the image frame here
					VIDEO_Send_Video(&hUsbDeviceFS, jout.data, jout.current_length);
				}
				break;
		}
		
		//rt_thread_delay(RT_TICK_PER_SECOND/2);
	}
  /* Infinite loop */
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

long jpeg_src_mem(int start, int len)
{
	//int i = 0;
	uint8_t* pos = (uint8_t*)(IMAGE_ADDRESS) + start;
	if(len <= 0) len = 16;
	if(len > 1024*20) len = 1024*20;
	show_addr = pos;
	show_length = len;
	rt_mb_send(jpeg_mb, JPEG_SHOW_SRC);
	/*
	for(i=0;i<len;++i){
		rt_kprintf("%02x ", *pos);
		pos++;
		if( (i&15) == 15){
			rt_kprintf("\n");
		}
	}*/
	return len;
}

long jpeg_dst_mem(int start, int len)
{
	//int i = 0;
	uint8_t* pos = (uint8_t*)(jpeg_data) + start;
	if(len <= 0) len = 16;
	if(len > 1024*20) len = 1024*20;
	show_addr = pos;
	show_length = len;
	rt_mb_send(jpeg_mb, JPEG_SHOW_SRC);
	/*
	for(i=0;i<len;++i){
		rt_kprintf("%02x ", *pos);
		pos++;
		if( (i&15) == 15){
			rt_kprintf("\n");
		}
	}*/
	return jout.current_length;
}

long jpeg_encode(void)
{
	rt_mb_send(jpeg_mb, JPEG_ENCODE);
	return 1;
}

long jpeg_debug(void)
{
	return 1;
}
uint32_t xfer_length;
long jpeg_set_length(int len)
{
	if(len < 0) len = 1024;
	if(len > 1024*20) len = 1024*20;
	xfer_length = len;
	return xfer_length;
}

FINSH_FUNCTION_EXPORT(jpeg, say jpeg world);
FINSH_FUNCTION_EXPORT(jpeg_src_mem, show image memory);
FINSH_FUNCTION_EXPORT(jpeg_dst_mem, show dst memory);
FINSH_FUNCTION_EXPORT(jpeg_encode, jpeg encode);
FINSH_FUNCTION_EXPORT(jpeg_debug, jpeg debug);
FINSH_FUNCTION_EXPORT(jpeg_set_length, jpeg set length);

void OnError_Handler(void)
{
  while(1) { ; } /* Blocking on error */
}

INIT_APP_EXPORT(jpeg_init);


