/**
  ******************************************************************************
  * @file    JPEG/JPEG_EncodingFromFLASH_DMA/Src/encode_dma.c
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    30-December-2016
  * @brief   This file provides routines for JPEG Encoding from memory with 
  *          DMA method.
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
#include "encode_dma.h"
#include "string.h"
#include "image_320_240_rgb.h"
/** @addtogroup STM32F7xx_HAL_Examples
  * @{
  */

/** @addtogroup JPEG_EncodingFromFLASH_DMA
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  uint8_t State;  
  uint8_t *DataBuffer;
  uint32_t DataBufferSize;

}JPEG_Data_BufferTypeDef;

/* Private define ------------------------------------------------------------*/
#if (JPEG_RGB_FORMAT == JPEG_ARGB8888)
#define BYTES_PER_PIXEL    4
#elif (JPEG_RGB_FORMAT == JPEG_RGB888)
#define BYTES_PER_PIXEL    3
#elif (JPEG_RGB_FORMAT == JPEG_RGB565)
#define BYTES_PER_PIXEL    2
#endif

#define CHUNK_SIZE_IN   ((uint32_t)(MAX_INPUT_WIDTH * BYTES_PER_PIXEL * MAX_INPUT_LINES)) 
#define CHUNK_SIZE_OUT  ((uint32_t) (4096*10))

#define JPEG_BUFFER_EMPTY       0
#define JPEG_BUFFER_FULL        1

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
JPEG_RGBToYCbCr_Convert_Function pRGBToYCbCr_Convert_Function;

uint8_t MCU_Data_IntBuffer0[CHUNK_SIZE_IN];
//uint8_t MCU_Data_InBuffer1[CHUNK_SIZE_IN];

uint8_t JPEG_Data_OutBuffer0[CHUNK_SIZE_OUT];
//uint8_t JPEG_Data_OutBuffer1[CHUNK_SIZE_OUT];

JPEG_Data_BufferTypeDef Jpeg_OUT_BufferTab = {JPEG_BUFFER_EMPTY , JPEG_Data_OutBuffer0 , 0};

JPEG_Data_BufferTypeDef Jpeg_IN_BufferTab = {JPEG_BUFFER_EMPTY , MCU_Data_IntBuffer0, 0};

uint32_t MCU_TotalNb                = 0;
uint32_t MCU_BlockIndex             = 0;
uint32_t Jpeg_HWEncodingEnd         = 0;


__IO uint32_t Output_Is_Paused      = 0;
__IO uint32_t Input_Is_Paused       = 0;

JPEG_ConfTypeDef Conf;

uint32_t RGB_InputImageIndex;
uint32_t RGB_InputImageSize_Bytes;
uint32_t RGB_InputImageAddress;

jpeg_dest_t* pJpegFile = 0;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



uint32_t  JPEG_Init(JPEG_HandleTypeDef *hjpeg)
{
  RGB_GetInfo(&Conf);
  MCU_TotalNb                = 0;
  JPEG_GetEncodeColorConvertFunc(&Conf, &pRGBToYCbCr_Convert_Function, &MCU_TotalNb);
  HAL_JPEG_ConfigEncoding(hjpeg, &Conf);
  return 0;
}

uint32_t  JPEG_Start_DMA(JPEG_HandleTypeDef *hjpeg)
{
  // I don't known why the DMA not work well, so I use a polling method
  //HAL_JPEG_Encode_DMA(hjpeg ,Jpeg_IN_BufferTab.DataBuffer ,Jpeg_IN_BufferTab.DataBufferSize ,Jpeg_OUT_BufferTab.DataBuffer ,CHUNK_SIZE_OUT);
  HAL_JPEG_Encode(hjpeg, Jpeg_IN_BufferTab.DataBuffer, Jpeg_IN_BufferTab.DataBufferSize, Jpeg_OUT_BufferTab.DataBuffer, sizeof(JPEG_Data_OutBuffer0), 0xffff);
  //HAL_JPEG_EncodeCpltCallback(hjpeg);
  return 0;
}
#define  YUV420_MCU_SIZE    (64*4+64+64)  // 4Y + 1U + 1V
#define  RGB_W              RGB_IMAGE_WIDTH
#define  RGB_H              RGB_IMAGE_HEIGHT
#define  H_SCALE            16
#define  V_SCALE            16
#define  MCN_CNT            ((RGB_IMAGE_WIDTH/H_SCALE) * (RGB_IMAGE_HEIGHT/V_SCALE))
void RGB565_to_YUV420(const uint16_t* in, uint8_t* out);
/**
  * @brief  Encode_DMA
  * @param hjpeg: JPEG handle pointer
  * @param  FileName    : jpg file path for decode.
  * @param  DestAddress : ARGB destination Frame Buffer Address.
  * @retval None
  */
uint32_t  JPEG_ConvertRGB_to_YUV(JPEG_HandleTypeDef *hjpeg, uint32_t RGBImageBufferAddress, uint32_t RGBImageSize_Bytes, jpeg_dest_t *jpgfile)
{
  uint32_t DataBufferSize = 0;
  
  pJpegFile = jpgfile;
  jpgfile->current_length = 0;

  /* Reset all Global variables */
  MCU_TotalNb                = 0;
  MCU_BlockIndex             = 0;
  Jpeg_HWEncodingEnd         = 0;
  Output_Is_Paused           = 0;
  Input_Is_Paused            = 0;

  /* Get RGB Info */
  //RGB_GetInfo(&Conf);
  //JPEG_GetEncodeColorConvertFunc(&Conf, &pRGBToYCbCr_Convert_Function, &MCU_TotalNb);

  /* Clear Output Buffer */
  Jpeg_OUT_BufferTab.DataBufferSize = 0;
  Jpeg_OUT_BufferTab.State = JPEG_BUFFER_EMPTY; 

  /* Fill input Buffers */  
  RGB_InputImageIndex = 0;
  RGB_InputImageAddress = RGBImageBufferAddress;
  RGB_InputImageSize_Bytes = RGBImageSize_Bytes;
  DataBufferSize= Conf.ImageWidth * MAX_INPUT_LINES * BYTES_PER_PIXEL;
  
  MCU_TotalNb = MCN_CNT;
  RGB565_to_YUV420((const uint16_t*)RGB_InputImageAddress, Jpeg_IN_BufferTab.DataBuffer);
  

  if(RGB_InputImageIndex < RGB_InputImageSize_Bytes)
  {
    /* Pre-Processing */
    //MCU_BlockIndex += pRGBToYCbCr_Convert_Function((uint8_t *)(RGB_InputImageAddress + RGB_InputImageIndex), Jpeg_IN_BufferTab.DataBuffer, 0, DataBufferSize,(uint32_t*)(&Jpeg_IN_BufferTab.DataBufferSize));
    Jpeg_IN_BufferTab.DataBufferSize = DataBufferSize;
    MCU_BlockIndex += MCU_TotalNb;
    Jpeg_IN_BufferTab.State = JPEG_BUFFER_FULL;
    
    RGB_InputImageIndex += DataBufferSize;
  }
  return 0;
}


#include "rgb565_to_yuv_table.c.tab"

void RGB565_to_YUV420(const uint16_t* in, uint8_t* out)
{
  int x,y;
  while((RGB_W % H_SCALE) || (RGB_H % V_SCALE));
  
  for(y=0;y<RGB_H;y+=2,in+= RGB_W*2){
    uint32_t l_mcu = (y&(~15)) *  (YUV420_MCU_SIZE/16) * (RGB_W/H_SCALE);
    uint8_t* cur_mcu = l_mcu + out;
    int y_off = (y&15)*8 + (y&8)*8;
    int u_off = 64*4 + (y&15) * 4;
    int v_off = 64*4+64 + (y&15) * 4;
    
    for(x=0;x<RGB_W;x+=H_SCALE){
      cur_mcu[y_off+0] = rgb565_2_y[in[x+0]];
      cur_mcu[y_off+1] = rgb565_2_y[in[x+1]];
      cur_mcu[y_off+2] = rgb565_2_y[in[x+2]];
      cur_mcu[y_off+3] = rgb565_2_y[in[x+3]];
      cur_mcu[y_off+4] = rgb565_2_y[in[x+4]];
      cur_mcu[y_off+5] = rgb565_2_y[in[x+5]];
      cur_mcu[y_off+6] = rgb565_2_y[in[x+6]];
      cur_mcu[y_off+7] = rgb565_2_y[in[x+7]];
      
      cur_mcu[y_off+0+64] = rgb565_2_y[in[x+8 ]];
      cur_mcu[y_off+1+64] = rgb565_2_y[in[x+9 ]];
      cur_mcu[y_off+2+64] = rgb565_2_y[in[x+10]];
      cur_mcu[y_off+3+64] = rgb565_2_y[in[x+11]];
      cur_mcu[y_off+4+64] = rgb565_2_y[in[x+12]];
      cur_mcu[y_off+5+64] = rgb565_2_y[in[x+13]];
      cur_mcu[y_off+6+64] = rgb565_2_y[in[x+14]];
      cur_mcu[y_off+7+64] = rgb565_2_y[in[x+15]];
      
      cur_mcu[y_off+8]  = rgb565_2_y[in[x+0+RGB_W]];
      cur_mcu[y_off+9]  = rgb565_2_y[in[x+1+RGB_W]];
      cur_mcu[y_off+10] = rgb565_2_y[in[x+2+RGB_W]];
      cur_mcu[y_off+11] = rgb565_2_y[in[x+3+RGB_W]];
      cur_mcu[y_off+12] = rgb565_2_y[in[x+4+RGB_W]];
      cur_mcu[y_off+13] = rgb565_2_y[in[x+5+RGB_W]];
      cur_mcu[y_off+14] = rgb565_2_y[in[x+6+RGB_W]];
      cur_mcu[y_off+15] = rgb565_2_y[in[x+7+RGB_W]];
      
      cur_mcu[y_off+8 +64] = rgb565_2_y[in[x+ 8+RGB_W]];
      cur_mcu[y_off+9 +64] = rgb565_2_y[in[x+ 9+RGB_W]];
      cur_mcu[y_off+10+64] = rgb565_2_y[in[x+10+RGB_W]];
      cur_mcu[y_off+11+64] = rgb565_2_y[in[x+11+RGB_W]];
      cur_mcu[y_off+12+64] = rgb565_2_y[in[x+12+RGB_W]];
      cur_mcu[y_off+13+64] = rgb565_2_y[in[x+13+RGB_W]];
      cur_mcu[y_off+14+64] = rgb565_2_y[in[x+14+RGB_W]];
      cur_mcu[y_off+15+64] = rgb565_2_y[in[x+15+RGB_W]];
      
      cur_mcu[u_off+0] = rgb565_2_u[in[x+0 ]];
      cur_mcu[u_off+1] = rgb565_2_u[in[x+2 ]];
      cur_mcu[u_off+2] = rgb565_2_u[in[x+4 ]];
      cur_mcu[u_off+3] = rgb565_2_u[in[x+6 ]];
      cur_mcu[u_off+4] = rgb565_2_u[in[x+8 ]];
      cur_mcu[u_off+5] = rgb565_2_u[in[x+10]];
      cur_mcu[u_off+6] = rgb565_2_u[in[x+12]];
      cur_mcu[u_off+7] = rgb565_2_u[in[x+14]];
      
      cur_mcu[v_off+0] = rgb565_2_v[in[x+0 ]];
      cur_mcu[v_off+1] = rgb565_2_v[in[x+2 ]];
      cur_mcu[v_off+2] = rgb565_2_v[in[x+4 ]];
      cur_mcu[v_off+3] = rgb565_2_v[in[x+6 ]];
      cur_mcu[v_off+4] = rgb565_2_v[in[x+8 ]];
      cur_mcu[v_off+5] = rgb565_2_v[in[x+10]];
      cur_mcu[v_off+6] = rgb565_2_v[in[x+12]];
      cur_mcu[v_off+7] = rgb565_2_v[in[x+14]];
      
      cur_mcu += YUV420_MCU_SIZE;
    }    
  }
  
}


uint32_t JPEG_Encode_DMA(JPEG_HandleTypeDef *hjpeg, uint32_t RGBImageBufferAddress, uint32_t RGBImageSize_Bytes, jpeg_dest_t *jpgfile)
{
  uint32_t DataBufferSize = 0;
  
  pJpegFile = jpgfile;
  jpgfile->current_length = 0;

  /* Reset all Global variables */
  MCU_TotalNb                = 0;
  MCU_BlockIndex             = 0;
  Jpeg_HWEncodingEnd         = 0;
  Output_Is_Paused           = 0;
  Input_Is_Paused            = 0;

  /* Get RGB Info */
  RGB_GetInfo(&Conf);
  JPEG_GetEncodeColorConvertFunc(&Conf, &pRGBToYCbCr_Convert_Function, &MCU_TotalNb);

  /* Clear Output Buffer */
  Jpeg_OUT_BufferTab.DataBufferSize = 0;
  Jpeg_OUT_BufferTab.State = JPEG_BUFFER_EMPTY; 

  /* Fill input Buffers */  
  RGB_InputImageIndex = 0;
  RGB_InputImageAddress = RGBImageBufferAddress;
  RGB_InputImageSize_Bytes = RGBImageSize_Bytes;
  DataBufferSize= Conf.ImageWidth * MAX_INPUT_LINES * BYTES_PER_PIXEL;

  if(RGB_InputImageIndex < RGB_InputImageSize_Bytes)
  {
    /* Pre-Processing */
    MCU_BlockIndex += pRGBToYCbCr_Convert_Function((uint8_t *)(RGB_InputImageAddress + RGB_InputImageIndex), Jpeg_IN_BufferTab.DataBuffer, 0, DataBufferSize,(uint32_t*)(&Jpeg_IN_BufferTab.DataBufferSize));
    Jpeg_IN_BufferTab.State = JPEG_BUFFER_FULL;
    
    RGB_InputImageIndex += DataBufferSize;
  }

  /* Fill Encoding Params */
  HAL_JPEG_ConfigEncoding(hjpeg, &Conf);

  /* Start JPEG encoding with DMA method */
  HAL_JPEG_Encode_DMA(hjpeg ,Jpeg_IN_BufferTab.DataBuffer ,Jpeg_IN_BufferTab.DataBufferSize ,Jpeg_OUT_BufferTab.DataBuffer ,CHUNK_SIZE_OUT);

  return 0;
}

/**
  * @brief JPEG Ouput Data BackGround processing .
  * @param hjpeg: JPEG handle pointer
  * @retval 1 : if JPEG processing has finiched, 0 : if JPEG processing still ongoing
  */
uint32_t JPEG_EncodeOutputHandler(JPEG_HandleTypeDef *hjpeg)
{
  uint32_t bytesWritefile = 0;

  if(Jpeg_OUT_BufferTab.State == JPEG_BUFFER_FULL)
  {  
    //f_write (pJpegFile, Jpeg_OUT_BufferTab.DataBuffer ,Jpeg_OUT_BufferTab.DataBufferSize, (UINT*)(&bytesWritefile)) ;
    if(!pJpegFile){
      OnError_Handler();
    }
    if(pJpegFile->current_length + Jpeg_OUT_BufferTab.DataBufferSize <= pJpegFile->total_length){
      memcpy(pJpegFile->data + pJpegFile->current_length, Jpeg_OUT_BufferTab.DataBuffer, Jpeg_OUT_BufferTab.DataBufferSize);
      pJpegFile->current_length += Jpeg_OUT_BufferTab.DataBufferSize;
    }else{
      OnError_Handler();
    }
    
    Jpeg_OUT_BufferTab.State = JPEG_BUFFER_EMPTY;
    Jpeg_OUT_BufferTab.DataBufferSize = 0;
    
    if(Jpeg_HWEncodingEnd != 0)
    {
      return 1;
    }
    else if((Output_Is_Paused == 1) && (Jpeg_OUT_BufferTab.State == JPEG_BUFFER_EMPTY))
    {
      Output_Is_Paused = 0;
      HAL_JPEG_Resume(hjpeg, JPEG_PAUSE_RESUME_OUTPUT);            
    }    
  }


  return 0;  
}

/**
  * @brief JPEG Input Data BackGround Preprocessing .
  * @param hjpeg: JPEG handle pointer
  * @retval None
  */
void JPEG_EncodeInputHandler(JPEG_HandleTypeDef *hjpeg)
{
  uint32_t DataBufferSize = Conf.ImageWidth * MAX_INPUT_LINES * BYTES_PER_PIXEL;

  if((Jpeg_IN_BufferTab.State == JPEG_BUFFER_EMPTY) && (MCU_BlockIndex <= MCU_TotalNb))  
  {
    /* Read and reorder lines from RGB input and fill data buffer */
    if(RGB_InputImageIndex < RGB_InputImageSize_Bytes)
    {
      /* Pre-Processing */
      MCU_BlockIndex += pRGBToYCbCr_Convert_Function((uint8_t *)(RGB_InputImageAddress + RGB_InputImageIndex), Jpeg_IN_BufferTab.DataBuffer, 0, DataBufferSize, (uint32_t*)(&Jpeg_IN_BufferTab.DataBufferSize));
      Jpeg_IN_BufferTab.State = JPEG_BUFFER_FULL;
      RGB_InputImageIndex += DataBufferSize;

      if(Input_Is_Paused == 1)
      {
        Input_Is_Paused = 0;
        HAL_JPEG_ConfigInputBuffer(hjpeg,Jpeg_IN_BufferTab.DataBuffer, Jpeg_IN_BufferTab.DataBufferSize);    
  
        HAL_JPEG_Resume(hjpeg, JPEG_PAUSE_RESUME_INPUT); 
      }
    }
    else
    {
      MCU_BlockIndex++;
    }
    rt_kprintf("we may never enter here\n");
  }
}

/**
  * @brief JPEG Get Data callback
  * @param hjpeg: JPEG handle pointer
  * @param NbEncodedData: Number of encoded (consummed) bytes from input buffer
  * @retval None
  */
void HAL_JPEG_GetDataCallback(JPEG_HandleTypeDef *hjpeg, uint32_t NbEncodedData)
{
  if(NbEncodedData == Jpeg_IN_BufferTab.DataBufferSize)
  {  
    Jpeg_IN_BufferTab.State = JPEG_BUFFER_EMPTY;
    Jpeg_IN_BufferTab.DataBufferSize = 0;

    HAL_JPEG_Pause(hjpeg, JPEG_PAUSE_RESUME_INPUT);
    Input_Is_Paused = 1;
  }
  else
  {
    HAL_JPEG_ConfigInputBuffer(hjpeg,Jpeg_IN_BufferTab.DataBuffer + NbEncodedData, Jpeg_IN_BufferTab.DataBufferSize - NbEncodedData);      
  }
}

/**
  * @brief JPEG Data Ready callback
  * @param hjpeg: JPEG handle pointer
  * @param pDataOut: pointer to the output data buffer
  * @param OutDataLength: length of output buffer in bytes
  * @retval None
  */
void HAL_JPEG_DataReadyCallback (JPEG_HandleTypeDef *hjpeg, uint8_t *pDataOut, uint32_t OutDataLength)
{
  Jpeg_OUT_BufferTab.State = JPEG_BUFFER_FULL;
  Jpeg_OUT_BufferTab.DataBufferSize = OutDataLength;

  HAL_JPEG_Pause(hjpeg, JPEG_PAUSE_RESUME_OUTPUT);
  Output_Is_Paused = 1;

  HAL_JPEG_ConfigOutputBuffer(hjpeg, Jpeg_OUT_BufferTab.DataBuffer, CHUNK_SIZE_OUT); 
}

/**
  * @brief  JPEG Error callback
  * @param hjpeg: JPEG handle pointer
  * @retval None
  */
void HAL_JPEG_ErrorCallback(JPEG_HandleTypeDef *hjpeg)
{
  OnError_Handler();
}

/*
  * @brief JPEG Decode complete callback
  * @param hjpeg: JPEG handle pointer
  * @retval None
  */
void HAL_JPEG_EncodeCpltCallback(JPEG_HandleTypeDef *hjpeg)
{    
  Jpeg_HWEncodingEnd = 1; 
  JPEG_EncodeOutputHandler(hjpeg);
  JPEG_On_DMA_Done(hjpeg);
}

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
