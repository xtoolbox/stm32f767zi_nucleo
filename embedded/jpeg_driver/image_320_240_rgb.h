/**
  ******************************************************************************
  * @file    JPEG/JPEG_EncodingFromFLASH_DMA/Inc/image_320_240_rgb.h
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    30-December-2016
  * @brief   Header for RGB 320x240 images definitions .
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __IMAGE_320x240_RGB_H
#define __IMAGE_320x240_RGB_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "jpeg_utils.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define RGB_IMAGE_WIDTH  ((uint32_t)320)  
#define RGB_IMAGE_HEIGHT ((uint32_t)240)


#if (JPEG_RGB_FORMAT == JPEG_ARGB8888)
  #define RGB_IMAGE_SIZE  ((uint32_t)RGB_IMAGE_HEIGHT * RGB_IMAGE_WIDTH)
  extern const uint32_t Image_ARGB8888[RGB_IMAGE_SIZE];

#elif(JPEG_RGB_FORMAT == JPEG_RGB888)
  #define RGB_IMAGE_SIZE  ((uint32_t)RGB_IMAGE_HEIGHT * RGB_IMAGE_WIDTH * 3/2)
  extern const uint32_t Image_RGB888[RGB_IMAGE_SIZE];

#elif(JPEG_RGB_FORMAT == JPEG_RGB565)
  #define RGB_IMAGE_SIZE  ((uint32_t)RGB_IMAGE_HEIGHT * RGB_IMAGE_WIDTH /2)
  extern const uint32_t Image_RGB565[RGB_IMAGE_SIZE];

#else
  #error "unknown JPEG_RGB_FORMAT "
#endif /* JPEG_RGB_FORMAT */
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */


#ifdef __cplusplus
}
#endif

#endif /* __IMAGE_320x240_RGB_H */
/*************************** End of file ****************************/
