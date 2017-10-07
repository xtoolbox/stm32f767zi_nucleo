/**
  ******************************************************************************
  * @file    usbd_audio.h
  * @author  MCD Application Team
  * @version V2.4.2
  * @date    11-December-2015
  * @brief   header file for the usbd_audio.c file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_VIDEO_H
#define __USB_VIDEO_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include  "usbd_ioreq.h"

/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */
  
/** @defgroup USBD_VIDEO
  * @brief This file is the Header file for usbd_audio.c
  * @{
  */ 


/** @defgroup USBD_VIDEO_Exported_Defines
  * @{
  */ 
// input endpoint for video
#define VIDEO_IN_EP                                   0x81
// output endpoint for output data
#define CONTROL_OUT_EP                                0x02
#define CONTROL_IN_EP                                 0x82
#define USB_VIDEO_CONFIG_DESC_SIZ                     169
#define USB_VIDEO_HID_DESC_OFFSET                     153
#define USB_VIDEO_HID_DESC_SIZE                       9
#define VIDEO_INTERFACE_DESC_SIZE                     9
#define USB_VIDEO_DESC_SIZ                            0x09
#define VIDEO_STANDARD_ENDPOINT_DESC_SIZE             0x09
#define VIDEO_STREAMING_ENDPOINT_DESC_SIZE            0x07

#define VIDEO_REPORT_DESC_SIZ                         20

#define VIDEO_DESCRIPTOR_TYPE                         0x21
#define VIDEO_HID_REPORT_DESC                         0x22
#define VIDEO_HID_DESCRIPTOR_TYPE                     0x21

#define USB_DEVICE_CLASS_VIDEO                        0x01
#define VIDEO_SUBCLASS_VIDEOCONTROL                   0x01
#define VIDEO_SUBCLASS_VIDEOSTREAMING                 0x02
#define VIDEO_PROTOCOL_UNDEFINED                      0x00
#define VIDEO_STREAMING_GENERAL                       0x01
#define VIDEO_STREAMING_FORMAT_TYPE                   0x02

/* Audio Descriptor Types */
#define VIDEO_INTERFACE_DESCRIPTOR_TYPE               0x24
#define VIDEO_ENDPOINT_DESCRIPTOR_TYPE                0x25

/* Audio Control Interface Descriptor Subtypes */
#define VIDEO_CONTROL_HEADER                          0x01
#define VIDEO_CONTROL_INPUT_TERMINAL                  0x02
#define VIDEO_CONTROL_OUTPUT_TERMINAL                 0x03
#define VIDEO_CONTROL_FEATURE_UNIT                    0x06

#define VIDEO_INPUT_TERMINAL_DESC_SIZE                0x0C
#define VIDEO_OUTPUT_TERMINAL_DESC_SIZE               0x09
#define VIDEO_STREAMING_INTERFACE_DESC_SIZE           0x07


#define VIDEO_HID_REQ_SET_PROTOCOL          0x0B
#define VIDEO_HID_REQ_GET_PROTOCOL          0x03

#define VIDEO_HID_REQ_SET_IDLE              0x0A
#define VIDEO_HID_REQ_GET_IDLE              0x02

#define VIDEO_HID_REQ_SET_REPORT            0x09
#define VIDEO_HID_REQ_GET_REPORT            0x01



#define VIDEO_CONTROL_MUTE                            0x0001

#define VIDEO_FORMAT_TYPE_I                           0x01
#define VIDEO_FORMAT_TYPE_III                         0x03

#define VIDEO_ENDPOINT_GENERAL                        0x01

#define VIDEO_REQ_GET_CUR                             0x81
#define VIDEO_REQ_GET_MIN                             0x82
#define VIDEO_REQ_GET_MAX                             0x83
#define VIDEO_REQ_SET_CUR                             0x01

#define VIDEO_OUT_STREAMING_CTRL                      0x02

//#define CONTROL_OUT_PACKET                            0x40
//#define VIDEO_OUT_PACKET                              (uint32_t)(((USBD_VIDEO_FREQ * 2 * 2) /1000)) 
#define VIDEO_DEFAULT_VOLUME                          70
    
/* Number of sub-packets in the audio transfer buffer. You can modify this value but always make sure
  that it is an even number and higher than 3 */
#define VIDEO_OUT_PACKET_NUM                          80
/* Total size of the audio transfer buffer */
#define VIDEO_TOTAL_BUF_SIZE                          ((uint32_t)(VIDEO_OUT_PACKET * VIDEO_OUT_PACKET_NUM))
    
    /* Audio Commands enumeration */
typedef enum
{
  VIDEO_CMD_START = 1,
  VIDEO_CMD_PLAY,
  VIDEO_CMD_STOP,
}VIDEO_CMD_TypeDef;


typedef enum
{
  VIDEO_OFFSET_NONE = 0,
  VIDEO_OFFSET_HALF,
  VIDEO_OFFSET_FULL,  
  VIDEO_OFFSET_UNKNOWN,    
}
VIDEO_OffsetTypeDef;
/**
  * @}
  */ 


/** @defgroup USBD_CORE_Exported_TypesDefinitions
  * @{
  */
 typedef struct
{
   uint8_t cmd;   
   uint8_t data[USB_MAX_EP0_SIZE];  
   uint8_t len;  
   uint8_t unit;    
}
USBD_VIDEO_ControlTypeDef; 


#define VIDEO_FRAME_TYPE   0x02

typedef struct
{
  __IO uint32_t             alt_setting; 
  VIDEO_OffsetTypeDef       offset;
  uint8_t                    rd_enable;  
  uint16_t                   rd_ptr;  
  uint16_t                   wr_ptr;  
  USBD_VIDEO_ControlTypeDef control;
	
	// video related fields
	uint32_t                  video_interface_changed;
	uint32_t                  video_alt_setting;
	uint32_t                  total_length;
	uint32_t                  current_length;
	uint8_t                   video_frame_id;
	uint8_t                   video_buffer[VIDEO_BUFFER_SIZE]; // this buffer may used by the jpeg engine
	
	// hid related fields
  uint8_t              Report_buf[CONTROL_PACKET_SIZE];
  uint32_t             Protocol;   
  uint32_t             IdleState;  
  uint32_t             hid_alt_setting;
  uint32_t             IsReportAvailable; 
	uint8_t              control_buffer[CONTROL_PACKET_SIZE];
	
}
USBD_VIDEO_HandleTypeDef; 

#define  CT_MOUSE  0
#define  CT_KEY    1
typedef struct{
	uint32_t x;
	uint32_t y;
	uint32_t button;
	uint32_t is_press;
}mouse_data_t;

typedef struct{
	uint32_t key;
	uint32_t modifier;
	uint32_t padding;
	uint32_t is_press;
}key_data_t;

typedef struct
{
	uint32_t type;  // 0 mouse, 1 keyboard
	union{
		mouse_data_t mouse;
		key_data_t key;
	}data;
}control_t;

typedef struct
{
    int8_t  (*Init)         (uint32_t  AudioFreq, uint32_t Volume, uint32_t options);
    int8_t  (*DeInit)       (uint32_t options);
	  int8_t  (*ControlEvent) (control_t* ctrl);
	  int8_t  (*CameraOn)     (void);
		int8_t  (*CameraOff)    (void);
	  int8_t  (*FrameDone)    (void);
}USBD_VIDEO_ItfTypeDef;
/**
  * @}
  */ 



/** @defgroup USBD_CORE_Exported_Macros
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup USBD_CORE_Exported_Variables
  * @{
  */ 

extern USBD_ClassTypeDef  USBD_VIDEO;
#define USBD_VIDEO_CLASS    &USBD_VIDEO
/**
  * @}
  */ 

/** @defgroup USB_CORE_Exported_Functions
  * @{
  */ 
uint8_t  USBD_VIDEO_RegisterInterface  (USBD_HandleTypeDef   *pdev, 
                                        USBD_VIDEO_ItfTypeDef *fops);

void  USBD_VIDEO_Sync (USBD_HandleTypeDef *pdev, VIDEO_OffsetTypeDef offset);


uint8_t*  VIDEO_Get_Video_Buffer(USBD_HandleTypeDef *pdev);

int8_t  VIDEO_Start_Transmit_Video(USBD_HandleTypeDef *pdev, uint32_t length);

int8_t  VIDEO_Send_Video(USBD_HandleTypeDef *pdev, const uint8_t* data, uint32_t length);
/**
  * @}
  */ 

#ifdef __cplusplus
}
#endif

#endif  /* __USB_VIDEO_H */
/**
  * @}
  */ 

/**
  * @}
  */ 
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
