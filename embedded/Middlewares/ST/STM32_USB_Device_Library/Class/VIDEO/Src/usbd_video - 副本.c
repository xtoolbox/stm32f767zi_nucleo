/**
  ******************************************************************************
  * @file    usbd_video.c
  * @author  MCD Application Team
  * @version V2.4.2
  * @date    11-December-2015
  * @brief   This file provides the Audio core functions.
  *
  * @verbatim
  *      
  *          
  * @note     In HS mode and when the DMA is used, all variables and data structures
  *           dealing with the DMA during the transaction process should be 32-bit aligned.
  *           
  *      
  *  @endverbatim
  *
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

/* Includes ------------------------------------------------------------------*/
#include "usbd_video.h"
#include "usbd_desc.h"
#include "usbd_ctlreq.h"
#include "stm32f7xx_hal.h"
#include "rtthread.h"

/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */


/** @defgroup USBD_VIDEO 
  * @brief usbd core module
  * @{
  */ 

/** @defgroup USBD_VIDEO_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBD_VIDEO_Private_Defines
  * @{
  */ 
typedef struct 
{
	uint32_t start;
	uint32_t stop;
	uint32_t frm_idx;
	uint32_t id;
}debug_t;

#define  DECLARE_DBG(name)\
debug_t  name##_buffer[64];\
uint32_t name##_index = 0;
#define  INIT_DBG(name)\
memset(name##_buffer,0,sizeof(name##_buffer));\
name##_index = 0;
#define  DBG_ENTER(name)\
void dbg_enter_##name(void){\
	USB_OTG_GlobalTypeDef *USBx = USB_OTG_FS;\
	if(debug_enable && name##_index<64){\
		name##_buffer[name##_index].start = DWT->CYCCNT;\
		name##_buffer[name##_index].frm_idx = (USBx_DEVICE->DSTS & USB_OTG_DSTS_FNSOF_Msk) >> USB_OTG_DSTS_FNSOF_Pos;\
		name##_buffer[name##_index].id = (USBx_INEP(1)->DIEPCTL & USB_OTG_DIEPCTL_EONUM_DPID) ? 1:0;\
	}\
}
#define DBG_LEAVE(name)\
void dbg_leave_##name(void){\
	if(debug_enable && name##_index<64){\
		name##_buffer[name##_index].stop = DWT->CYCCNT;\
		\
		name##_index++;\
	}\
}

DECLARE_DBG(sof)
DECLARE_DBG(xfer_done)
DECLARE_DBG(otg_int)
DECLARE_DBG(fifo_empty)
DECLARE_DBG(fill_data)
DECLARE_DBG(xfer_frame)

uint32_t debug_enable = 0;

void dbg_start(void){
	debug_enable = 1;
  INIT_DBG(sof)
  INIT_DBG(xfer_done)
  INIT_DBG(otg_int)
  INIT_DBG(fifo_empty)
  INIT_DBG(fill_data)
  INIT_DBG(xfer_frame)
}
void dbg_stop(void){
	debug_enable = 0;
}
  DBG_ENTER(sof)
  DBG_ENTER(xfer_done)
  DBG_ENTER(otg_int)
  DBG_ENTER(fifo_empty)
  DBG_ENTER(fill_data)
  DBG_ENTER(xfer_frame)

  DBG_LEAVE(sof)
  DBG_LEAVE(xfer_done)
  DBG_LEAVE(otg_int)
  DBG_LEAVE(fifo_empty)
  DBG_LEAVE(fill_data)
  DBG_LEAVE(xfer_frame)

#define  DBG_INFO(name,field)\
do{\
	rt_kprintf("   {  name = \"" #name "\", \n");\
	for(i=0;i<name##_index;++i){\
	rt_kprintf("      {start = 0x%x, stop = 0x%x, desc = \"%d\"}, \n", name##_buffer[i].start, name##_buffer[i].stop, name##_buffer[i].field);\
  }\
	rt_kprintf("   }, \n");\
}while(0)

void print_debug_info(void)
{
	uint32_t i;
	rt_kprintf("{\n");
	DBG_INFO(sof, frm_idx);
	DBG_INFO(xfer_done, id);
	DBG_INFO(otg_int, frm_idx);
	DBG_INFO(xfer_frame, frm_idx);
	DBG_INFO(fill_data, id);
	//rt_kprintf("   {  name = \"sof\", \n");
	//for(i=0;i<sof_index;++i){
	//rt_kprintf("      {start = 0x%x, stop = 0x%x, desc = \"%d\"}, \n", sof_buffer[i].start, sof_buffer[i].stop, sof_buffer[i].frm_idx);
  //}
	//rt_kprintf("   }, \n");
	
	
	
	rt_kprintf("}\n");
}
/**
  * @}
  */ 


/** @defgroup USBD_VIDEO_Private_Macros
  * @{
  */ 
#define VIDEO_SAMPLE_FREQ(frq)      (uint8_t)(frq), (uint8_t)((frq >> 8)), (uint8_t)((frq >> 16))

#define VIDEO_PACKET_SZE(frq)          (uint8_t)(((frq * 2 * 2)/1000) & 0xFF), \
                                       (uint8_t)((((frq * 2 * 2)/1000) >> 8) & 0xFF)
                                         
/**
  * @}
  */ 




/** @defgroup USBD_VIDEO_Private_FunctionPrototypes
  * @{
  */


static uint8_t  USBD_VIDEO_Init (USBD_HandleTypeDef *pdev, 
                               uint8_t cfgidx);

static uint8_t  USBD_VIDEO_DeInit (USBD_HandleTypeDef *pdev, 
                                 uint8_t cfgidx);

static uint8_t  USBD_VIDEO_Setup (USBD_HandleTypeDef *pdev, 
                                USBD_SetupReqTypedef *req);

static uint8_t  *USBD_VIDEO_GetCfgDesc (uint16_t *length);

static uint8_t  *USBD_VIDEO_GetDeviceQualifierDesc (uint16_t *length);

static uint8_t  USBD_VIDEO_DataIn (USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t  USBD_VIDEO_DataOut (USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t  USBD_VIDEO_EP0_RxReady (USBD_HandleTypeDef *pdev);

static uint8_t  USBD_VIDEO_EP0_TxReady (USBD_HandleTypeDef *pdev);

static uint8_t  USBD_VIDEO_SOF (USBD_HandleTypeDef *pdev);

static uint8_t  USBD_VIDEO_IsoINIncomplete (USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t  USBD_VIDEO_IsoOutIncomplete (USBD_HandleTypeDef *pdev, uint8_t epnum);

static void VIDEO_REQ_GetCurrent(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);

static void VIDEO_REQ_SetCurrent(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);

/**
  * @}
  */ 

/** @defgroup USBD_VIDEO_Private_Variables
  * @{
  */ 

USBD_ClassTypeDef  USBD_VIDEO = 
{
  USBD_VIDEO_Init,
  USBD_VIDEO_DeInit,
  USBD_VIDEO_Setup,
  USBD_VIDEO_EP0_TxReady,  
  USBD_VIDEO_EP0_RxReady,
  USBD_VIDEO_DataIn,
  USBD_VIDEO_DataOut,
  USBD_VIDEO_SOF,
  USBD_VIDEO_IsoINIncomplete,
  USBD_VIDEO_IsoOutIncomplete,      
  USBD_VIDEO_GetCfgDesc,
  USBD_VIDEO_GetCfgDesc, 
  USBD_VIDEO_GetCfgDesc,
  USBD_VIDEO_GetDeviceQualifierDesc,
};

/* USB VIDEO device Configuration Descriptor */
__ALIGN_BEGIN static const uint8_t USBD_VIDEO_CfgDesc[USB_VIDEO_CONFIG_DESC_SIZ] __ALIGN_END =
{
    /* Configuration Descriptor */
    0x09,                                /* bLength */
    USB_DESC_TYPE_CONFIGURATION,   /* bDescriptorType */
    LOBYTE(USB_VIDEO_CONFIG_DESC_SIZ),           /* wTotalLength  0x90 bytes*/
    HIBYTE(USB_VIDEO_CONFIG_DESC_SIZ),    
    0x03,                                 /* bNumInterfaces */
    0x01,                                 /* bConfigurationValue */
    0x00,                                 /* iConfiguration */
    0x80,                                 /* bmAttributes  BUS Powred, no remote wakeup*/
    0xFA,                                 /* bMaxPower = 500 mA*/
    /* 09 byte, total size 9*/

    /* Standard Video Interface Collection IAD */
    0x08,                                 /* bLength */
    USB_DESC_TYPE_ASSOCIATION,      /* bDescriptorType */
    0x00,                                 /* bFirstInterface: Interface number of the VideoControl interface that is associated with this function*/
    0x02,                                 /* Number of contiguous Video interfaces that are associated with this function */
    0x0E,                                 /* bFunction Class: CC_VIDEO*/
    0x03,                                 /* bFunction sub Class: SC_VIDEO_INTERFACE_COLLECTION */
    0x00,                                 /* bFunction protocol : PC_PROTOCOL_UNDEFINED*/
    0x02,                                 /* iFunction */
    /* 08 bytes, total size 17*/
    
    /* Standard VC Interface Descriptor */
    0x09,                                 /* bLength */
    0x04,                                 /* bDescriptorType */
    0x00,                                 /* bInterfaceNumber */
    0x00,                                 /* bAlternateSetting */
    0x00,                                 /* bNumEndpoints */
    0x0e,                                 /* bInterfaceClass : CC_VIDEO */
    0x01,                                 /* bInterfaceSubClass : SC_VIDEOCONTROL */
    0x00,                                 /* bInterfaceProtocol : PC_PROTOCOL_UNDEFINED */
    0x02,                                 /* iInterface */
    /* 09 bytes, total size 26*/
    
    /* Class-specific VC Interface Descriptor */
    0x0d,                                 /* bLength */
    0x24,                                 /* bDescriptorType : CS_INTERFACE */
    0x01,                                 /* bDescriptorSubType : VC_HEADER subtype */
    0x00,                                 /* bcdUVC : Revision of class specification that this device is based upon. For this example, the device complies with Video Class specification version 1.0 */
    0x01,
    0x1e,                                 /* wTotalLength : Total size of class-specific descriptors*/
    0x00,
    0x80, 0x8d, 0x5b, 0x00,               /* dwClockFrequency : 0x005b8d80 -> 6,000,000 == 6MHz*/
    0x01,                                 /* bInCollection : Number of streaming interfaces. */
    0x01,                                 /* baInterfaceNr(1) : VideoStreaming interface 1 belongs to this VideoControl interface.*/
    /* 13 Bytes, totoal size 39 */
    
    /* Input Terminal Descriptor (Composite) */
    0x08,                                 /* bLength */
    0x24,                                 /* bDescriptorType : CS_INTERFACE */
    0x02,                                 /* bDescriptorSubType : VC_INPUT_TERMINAL subtype */
    0x02,                                 /* bTerminalID: ID of this input terminal */
    0x01, 0x04,                           /* wTerminalType: 0x0401 COMPOSITE_CONNECTOR type. This terminal is the composite connector. */
    0x00,                                 /* bAssocTerminal: No association */
    0x00,                                 /* iTerminal: Unused*/
    /* 8 Bytes, totoal size 47 */
  
    /* Output Terminal Descriptor */
    0x09,                                 /* bLength */
    0x24,                                 /* bDescriptorType : CS_INTERFACE */
    0x03,                                 /* bDescriptorSubType : VC_OUTPUT_TERMINAL subtype */
    0x03,                                 /* bTerminalID: ID of this output terminal */
    0x01, 0x01,                           /* wTerminalType: 0x0101 TT_STREAMING type. This terminal is a USB streaming terminal. */
    0x00,                                 /* bAssocTerminal: No association */
    0x02,                                 /* bSourceID: The input pin of this unit is connected to the output pin of unit 2. */
    0x00,                                 /* iTerminal: Unused*/
    /* 9 bytes, total size 56 */

    /* Standard VS Interface Descriptor*/
    0x09,                                 /* bLength */
    0x04,                                 /* bDescriptorType : INTERFACE */
    0x01,                                 /* bInterfaceNumber */
    0x00,                                 /* bAlternateSetting */
    0x00,                                 /* bNumEndpoints : 0 endpoints – no bandwidth used*/
    0x0e,                                 /* bInterfaceClass : CC_VIDEO */
    0x02,                                 /* bInterfaceSubClass : SC_VIDEOSTREAMING */
    0x00,                                 /* bInterfaceProtocol : PC_PROTOCOL_UNDEFINED */
    0x00,                                 /* iInterface : unused */
    /* 9 bytes, total size 65 */
    
    /* Class-specific VS Header Descriptor (Input) */
    0x0e,                                 /* bLength */
    0x24,                                 /* bDescriptorType : CS_INTERFACE */
    0x01,                                 /* bDescriptorSubType : VC_HEADER subtype */
    0x01,                                 /* bNumFormats : One format descriptor follows. */
    0x3f, 0x00,                           /* wTotalLength : Total size of class-specific descriptors*/
    0x81,                                 /* bEndpointAddress : 0x81 */
    0x00,                                 /* bmInfo : No dynamic format change supported. */
    0x03,                                 /* bTerminalLink : This VideoStreaming interface supplies terminal ID 3 (Output Terminal). */
    0x00,                                 /* bStillCaptureMethod : Device supports still image capture method 0. */
    0x00,                                 /* bTriggerSupport : Hardware trigger supported for still image capture */
    0x00,                                 /* bTriggerUsage : Hardware trigger should initiate a still image capture. */
    0x01,                                 /* bControlSize : Size of the bmaControls field */
    0x00,                                 /* bmaControls : No VideoStreaming specific controls are supported.*/
    /* 14 Bytes, totoal size 79 */
    
    /* Class-specific VS Format Descriptor  */   
    0x0b,                                 /* bLength */
    0x24,                                 /* bDescriptorType : CS_INTERFACE */
    0x06,                                 /* bDescriptorSubType : VS_FORMAT_MJPEG subtype */
    0x01,                                 /* bFormatIndex : First (and only) format descriptor */
    0x01,                                 /* bNumFrameDescriptors : One frame descriptor for this format follows. */
    0x01,                                 /* bmFlags : Uses fixed size samples.. */
    0x01,                                 /* bDefaultFrameIndex : Default frame index is 1. */
    0x00,                                 /* bAspectRatioX : Non-interlaced stream – not required. */
    0x00,                                 /* bAspectRatioY : Non-interlaced stream – not required. */
    0x00,                                 /* bmInterlaceFlags : Non-interlaced stream */
    0x00,                                 /* bCopyProtect : No restrictions imposed on the duplication of this video stream. */
    /* 11 bytes, total size 90 */
    
    /* Class-specific VS Frame Descriptor */
    0x26,                                 /* bLength */
    0x24,                                 /* bDescriptorType : CS_INTERFACE */
    0x07,                                 /* bDescriptorSubType : VS_FRAME_MJPEG */
    0x01,                                 /* bFrameIndex : First (and only) frame descriptor */
    0x02,                                 /* bmCapabilities : Still images using capture method 0 are supported at this frame setting.D1: Fixed frame-rate. */
    MAKE_WORD(WIDTH),                     /* wWidth : Width of frame is 176 pixels. */
    MAKE_WORD(HEIGHT),                    /* wHeight : Height of frame is 144 pixels. */
    MAKE_DWORD(MIN_BIT_RATE),             /* dwMinBitRate : Min bit rate in bits/s  */
    MAKE_DWORD(MAX_BIT_RATE),             /* dwMaxBitRate : Max bit rate in bits/s  */
    MAKE_DWORD(MAX_FRAME_SIZE),           /* dwMaxVideoFrameBufSize : Maximum video or still frame size, in bytes. */
    MAKE_DWORD(INTERVEL),                 /* dwDefaultFrameInterval : 1,000,000 * 100ns -> 10 FPS */
    0x00,                                 /* bFrameIntervalType : Continuous frame interval */
    MAKE_DWORD(INTERVEL),                 /* dwMinFrameInterval : 1,000,000 ns  *100ns -> 10 FPS */
    MAKE_DWORD(INTERVEL),                 /* dwMaxFrameInterval : 1,000,000 ns *100ns -> 10 FPS */
    0x00, 0x00, 0x00, 0x00,               /* dwFrameIntervalStep : No frame interval step supported. */
    /* 38 bytes, total size 128 */

    /* Standard VS Interface Descriptor */
    0x09,                       /*bLength: Interface Descriptor size*/
    USB_DESC_TYPE_INTERFACE,    /*bDescriptorType: Interface descriptor type*/
    0x01,                       /*bInterfaceNumber: Number of Interface*/
    0x01,                       /*bAlternateSetting: Alternate setting*/
    0x01,                       /*bNumEndpoints*/
    0x0e,                       /*bInterfaceClass :*/
    0x02,                       /*bInterfaceSubClass :*/
    0x00,                       /*nInterfaceProtocol :*/
    0x00,                       /*iInterface: Index of string descriptor*/
    /* 9 bytes, total size 137 */
    
    /* Standard VS Isochronous Video Data Endpoint Descriptor */
    0x07,                       /*bLength: Endpoint Descriptor size*/
    USB_DESC_TYPE_ENDPOINT,     /*bDescriptorType:*/
    VIDEO_IN_EP,                /*bEndpointAddress: Endpoint Address (OUT)*/
    VIEDO_EP_TYPE_DESC,         /*bmAttributes: iso endpoint*/
    MAKE_WORD(VIDEO_PACKET_SIZE),
    0x01,
    /* 7 bytes, total size 144 */
    
    // Add a control data output descriptor
    /************** Descriptor of Joystick Mouse interface ****************/
    /* 09 */
    0x09,         /*bLength: Interface Descriptor size*/
    USB_DESC_TYPE_INTERFACE,/*bDescriptorType: Interface descriptor type*/
    0x02,         /*bInterfaceNumber: Number of Interface*/
    0x00,         /*bAlternateSetting: Alternate setting*/
    0x01,         /*bNumEndpoints*/
    0x03,         /*bInterfaceClass: HID*/
    0x00,         /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
    0x00,         /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
    0,            /*iInterface: Index of string descriptor*/
    /******************** Descriptor of Joystick Mouse HID ********************/
    /* 9 bytes, total size 153 */
    0x09,         /*bLength: HID Descriptor size*/
    USB_DESC_TYPE_HID, /*bDescriptorType: HID*/
    0x00,         /*bcdHID: HID Class Spec release number*/
    0x01,
    0x00,         /*bCountryCode: Hardware target country*/
    0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
    0x22,         /*bDescriptorType*/
    VIDEO_REPORT_DESC_SIZ,/*wItemLength: Total length of Report descriptor*/
    0x00,
    /******************** Descriptor of Joystick Mouse endpoint ********************/
    /* 9 bytes, total size 162 */
    0x07,          /*bLength: Endpoint Descriptor size*/
    USB_DESC_TYPE_ENDPOINT, /*bDescriptorType:*/

    CONTROL_OUT_EP,/*bEndpointAddress: Endpoint Address (OUT)*/
    0x03,          /*bmAttributes: Interrupt endpoint*/
    CONTROL_PACKET_SIZE,          /*wMaxPacketSize: 4 Byte max */
    0x00,
    0x01,          /*bInterval: Polling Interval (32 ms)*/
		/* 7 bytes, total size 169 */
};

/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t USBD_VIDEO_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END=
{
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00,
  0x02,
  0x00,
  0x00,
  0x00,
  0x40,
  0x01,
  0x00,
};

__ALIGN_BEGIN static const uint8_t VIDEO_ReportDescriptor[VIDEO_REPORT_DESC_SIZ] __ALIGN_END =
  {
    0x05, 0x8c,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x01,                    // USAGE (Pointer)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x09, 0x01,                    //   USAGE (Pointer)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x95, CONTROL_PACKET_SIZE,                    //   REPORT_COUNT (CONTROL_PACKET_SIZE)
    0x91, 0x02,                    //   OUTPUT (Data,Var,Abs)
    0xc0,                          // END_COLLECTION
  }; /* Keyboard_ReportDescriptor */
	
/**
  * @}
  */ 

/** @defgroup USBD_VIDEO_Private_Functions
  * @{
  */ 

/**
  * @brief  USBD_VIDEO_Init
  *         Initialize the VIDEO interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
USBD_VIDEO_HandleTypeDef video_class_handle;
static uint8_t  USBD_VIDEO_Init (USBD_HandleTypeDef *pdev, 
                               uint8_t cfgidx)
{
  USBD_VIDEO_HandleTypeDef   *hvideo;
	USB_OTG_GlobalTypeDef *USBx = USB_OTG_FS;
  
  /* Open EP OUT */
  USBD_LL_OpenEP(pdev,
                 VIDEO_IN_EP,
                 VIDEO_EP_TYPE,
                 VIDEO_PACKET_SIZE);
	
	USBD_LL_OpenEP(pdev,
                 CONTROL_OUT_EP,
                 USBD_EP_TYPE_INTR,
                 CONTROL_PACKET_SIZE);
	
  /* Allocate Audio structure */
  pdev->pClassData = &video_class_handle;//USBD_malloc(sizeof (USBD_VIDEO_HandleTypeDef));
  
  if(pdev->pClassData == NULL)
  {
    return USBD_FAIL; 
  }
  else
  {
    hvideo = (USBD_VIDEO_HandleTypeDef*) pdev->pClassData;
    hvideo->alt_setting = 0;
    hvideo->offset = VIDEO_OFFSET_UNKNOWN;
    hvideo->wr_ptr = 0; 
    hvideo->rd_ptr = 0;  
    hvideo->rd_enable = 0;
		// payload header, must toggle every frame
		hvideo->video_interface_changed = 0;
		hvideo->video_alt_setting = 0;
		hvideo->current_length = 0;
		hvideo->total_length = 0;
		hvideo->video_frame_id = 0x00;
    
    /* Initialize the Audio output Hardware layer */
    if (((USBD_VIDEO_ItfTypeDef *)pdev->pUserData)->Init(USBD_VIDEO_FREQ, VIDEO_DEFAULT_VOLUME, 0) != USBD_OK)
    {
      return USBD_FAIL;
    }
    
    /* Prepare Out endpoint to receive control packet */ 
    USBD_LL_PrepareReceive(pdev,
                           CONTROL_OUT_EP,
                           hvideo->control_buffer,
                           CONTROL_PACKET_SIZE);
  }
  return USBD_OK;
}

/**
  * @brief  USBD_VIDEO_Init
  *         DeInitialize the VIDEO layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_VIDEO_DeInit (USBD_HandleTypeDef *pdev, 
                                 uint8_t cfgidx)
{
  
  /* Open EP OUT */
  USBD_LL_CloseEP(pdev,
              VIDEO_IN_EP);
	USBD_LL_CloseEP(pdev,
              CONTROL_OUT_EP);

  /* DeInit  physical Interface components */
  if(pdev->pClassData != NULL)
  {
   ((USBD_VIDEO_ItfTypeDef *)pdev->pUserData)->DeInit(0);
    USBD_free(pdev->pClassData);
    pdev->pClassData = NULL;
  }
  
  return USBD_OK;
}

uint32_t MUTE_DATA = 0;


typedef struct  _VideoControl{
  uint8_t    bmHint[2];                      // 0x02
  uint8_t    bFormatIndex[1];                // 0x03
  uint8_t    bFrameIndex[1];                 // 0x04
  uint8_t    dwFrameInterval[4];             // 0x08
  uint8_t    wKeyFrameRate[2];               // 0x0A
  uint8_t    wPFrameRate[2];                 // 0x0C
  uint8_t    wCompQuality[2];                // 0x0E
  uint8_t    wCompWindowSize[2];             // 0x10
  uint8_t    wDelay[2];                      // 0x12
  uint8_t    dwMaxVideoFrameSize[4];         // 0x16
  uint8_t    dwMaxPayloadTransferSize[4];    // 0x1A
  uint8_t    dwClockFrequency[4];            // 0x1C
  uint8_t    bmFramingInfo[1];
  uint8_t    bPreferedVersion[1];
  uint8_t    bMinVersion[1];
  uint8_t    bMaxVersion[1];
  
}VideoControl;

VideoControl    videoCommitControl = 
{
  {0x01,0x00},                      // bmHint
  {0x01},                           // bFormatIndex
  {0x01},                           // bFrameIndex
  {MAKE_DWORD(INTERVEL),},          // dwFrameInterval
  {0x00,0x00,},                     // wKeyFrameRate
  {0x00,0x00,},                     // wPFrameRate
  {0x00,0x00,},                     // wCompQuality
  {0x00,0x00,},                     // wCompWindowSize
  {0x00,0x00},                      // wDelay
  {MAKE_DWORD(MAX_FRAME_SIZE)},    // dwMaxVideoFrameSize
  {0x00, 0x00, 0x00, 0x00},         // dwMaxPayloadTransferSize
  {0x00, 0x00, 0x00, 0x00},         // dwClockFrequency
  {0x00},                           // bmFramingInfo
  {0x00},                           // bPreferedVersion
  {0x00},                           // bMinVersion
  {0x00},                           // bMaxVersion
};

VideoControl    videoProbeControl = 
{
  {0x01,0x00},                      // bmHint
  {0x01},                           // bFormatIndex
  {0x01},                           // bFrameIndex
  {MAKE_DWORD(INTERVEL),},          // dwFrameInterval
  {0x00,0x00,},                     // wKeyFrameRate
  {0x00,0x00,},                     // wPFrameRate
  {0x00,0x00,},                     // wCompQuality
  {0x00,0x00,},                     // wCompWindowSize
  {0x00,0x00},                      // wDelay
  {MAKE_DWORD(MAX_FRAME_SIZE)},    // dwMaxVideoFrameSize
  {0x00, 0x00, 0x00, 0x00},         // dwMaxPayloadTransferSize
  {0x00, 0x00, 0x00, 0x00},         // dwClockFrequency
  {0x00},                           // bmFramingInfo
  {0x00},                           // bPreferedVersion
  {0x00},                           // bMinVersion
  {0x00},                           // bMaxVersion
};

/**
  * @brief  USBD_VIDEO_Setup
  *         Handle the VIDEO specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t  USBD_VIDEO_Setup (USBD_HandleTypeDef *pdev, 
                                USBD_SetupReqTypedef *req)
{
  USBD_VIDEO_HandleTypeDef   *hvideo;
  uint16_t len;
  uint8_t *pbuf;
  uint8_t ret = USBD_OK;
	uint8_t interface = req->wIndex & 0xff;
  hvideo = (USBD_VIDEO_HandleTypeDef*) pdev->pClassData;
  
  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
  case USB_REQ_TYPE_CLASS :  
	  if(interface == 2 ){
			// the hid interace 
			switch (req->bRequest)
			{
			case VIDEO_HID_REQ_SET_PROTOCOL:
				hvideo->Protocol = (uint8_t)(req->wValue);
				break;
				
			case VIDEO_HID_REQ_GET_PROTOCOL:
				USBD_CtlSendData (pdev, 
													(uint8_t *)&hvideo->Protocol,
													1);    
				break;
				
			case VIDEO_HID_REQ_SET_IDLE:
				hvideo->IdleState = (uint8_t)(req->wValue >> 8);
				break;
				
			case VIDEO_HID_REQ_GET_IDLE:
				USBD_CtlSendData (pdev, 
													(uint8_t *)&hvideo->IdleState,
													1);        
				break;      
			
			case VIDEO_HID_REQ_SET_REPORT:
				hvideo->IsReportAvailable = 1;
				USBD_CtlPrepareRx (pdev, hvideo->Report_buf, (uint8_t)(req->wLength));
				
				break;
			default:
				USBD_CtlError (pdev, req);
				return USBD_FAIL; 
			}
			break;
		}
	  // otherwise video request
    switch (req->bRequest)
    {
    case VIDEO_REQ_GET_CUR:
	  case VIDEO_REQ_GET_MIN:
	  case VIDEO_REQ_GET_MAX:
			if(interface == 1){
				if(req->wValue == 0x0100){
					USBD_CtlSendData (pdev, 
                    (uint8_t*)&videoProbeControl,
                    req->wLength);
				}else if(req->wValue == 0x0200){
					USBD_CtlSendData (pdev, 
                    (uint8_t*)&videoCommitControl,
                    req->wLength);
				}
				break;
			}
			// if interface not 1, will fall in next branch
      //VIDEO_REQ_GetCurrent(pdev, req);
      //break;
      
    case VIDEO_REQ_SET_CUR:
			if(1){
				if(req->wValue == 0x0100){
					  if (req->wLength){
							USBD_CtlPrepareRx (pdev,
                       (uint8_t*)&videoProbeControl,                             
                       req->wLength);
						}
				}else if(req->wValue == 0x0200){
					  if (req->wLength){
							USBD_CtlPrepareRx (pdev,
                       (uint8_t*)&videoCommitControl,                             
                       req->wLength);
						}
				}
				break;
			}
      //VIDEO_REQ_SetCurrent(pdev, req);   
      //break;
      
    default:
      USBD_CtlError (pdev, req);
      ret = USBD_FAIL; 
    }
    break;
    
  case USB_REQ_TYPE_STANDARD:
    switch (req->bRequest)
    {
    case USB_REQ_GET_DESCRIPTOR:
		  if(interface == 2){
				if( (req->wValue >> 8) == VIDEO_HID_REPORT_DESC){
					pbuf = (uint8_t*)VIDEO_ReportDescriptor;
					len = MIN(VIDEO_REPORT_DESC_SIZ , req->wLength);
					USBD_CtlSendData (pdev, 
														pbuf,
														len);
					break;
				}else if( (req->wValue >> 8) == VIDEO_HID_DESCRIPTOR_TYPE){
					pbuf = (uint8_t*)USBD_VIDEO_CfgDesc + USB_VIDEO_HID_DESC_OFFSET;
					len = MIN(9 , req->wLength);
					USBD_CtlSendData (pdev,
														pbuf,
														len);
					break;
				}
			}
      if( (req->wValue >> 8) == VIDEO_DESCRIPTOR_TYPE)
      {
        pbuf = (uint8_t*)USBD_VIDEO_CfgDesc + 18;
        len = MIN(USB_VIDEO_DESC_SIZ , req->wLength);
        
        
        USBD_CtlSendData (pdev, 
                          pbuf,
                          len);
      }
      break;
      
    case USB_REQ_GET_INTERFACE :
			if(interface == 2){
				USBD_CtlSendData (pdev,
                        (uint8_t *)&(hvideo->hid_alt_setting),
                        1);
			}else{
				USBD_CtlSendData (pdev,
                        (uint8_t *)&(hvideo->video_alt_setting),
                        1);
			}
      break;
      
    case USB_REQ_SET_INTERFACE :
      if ((uint8_t)(req->wValue) <= USBD_MAX_NUM_INTERFACES)
      {
				if(interface == 2){
					hvideo->hid_alt_setting = (uint8_t)(req->wValue);
				}else{
					hvideo->video_alt_setting = (uint8_t)(req->wValue);
					//hvideo->video_interface_changed = 1;
					if(hvideo->video_alt_setting){
						((USBD_VIDEO_ItfTypeDef *)pdev->pUserData)->CameraOn(); 
					}else{
						((USBD_VIDEO_ItfTypeDef *)pdev->pUserData)->CameraOff(); 
					}
				}
      }
      else
      {
        /* Call the error management function (command will be nacked */
        USBD_CtlError (pdev, req);
      }
      break;      
      
    default:
      USBD_CtlError (pdev, req);
      ret = USBD_FAIL;     
    }
  }
  return ret;
}


/**
  * @brief  USBD_VIDEO_GetCfgDesc 
  *         return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *USBD_VIDEO_GetCfgDesc (uint16_t *length)
{
  *length = sizeof (USBD_VIDEO_CfgDesc);
  return (uint8_t*)USBD_VIDEO_CfgDesc;
}



/**
  * @brief  USBD_VIDEO_DataIn
  *         handle data IN Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
const uint8_t xxxx[] = {
	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
	
	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
	
	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
	
	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
	
	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
	
	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
	
	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
	
	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
	
};


/**** ISO transmit produre
1. prepare buffer
2. enable FIFO empty interrupt
3. enalbe end point in sof interrupt, the packet will transfer at next frame
4. fill FIFO in FIFO empty interrupt
5. re-enable end point is tranfer complete interrupt
6. 

 **************************/



#define   VIDEO_IN_EP_NUM    (VIDEO_IN_EP & 0x7F)
uint32_t new_data_ready = 0;

HAL_StatusTypeDef PCD_WriteEmptyTxFifo(PCD_HandleTypeDef *hpcd, uint32_t epnum);
void fast_ep1_in(PCD_HandleTypeDef *hpcd);
static uint32_t xfer_last_len = 1;
USBD_StatusTypeDef  USBD_ISO_Transmit_start (USBD_HandleTypeDef *pdev, 
                                      uint8_t  *pbuf,
                                      uint16_t  len)
{
	USB_OTG_EPTypeDef *ep;
	USB_OTG_GlobalTypeDef *USBx = ((PCD_HandleTypeDef*)pdev->pData)->Instance;
  ep = &((PCD_HandleTypeDef*)pdev->pData)->IN_ep[VIDEO_IN_EP & 0x7F];
  
  /*setup and start the Xfer */
  ep->xfer_buff = pbuf;  
  ep->xfer_len = len;
  ep->xfer_count = 0;
  ep->is_in = 1;
  ep->num = VIDEO_IN_EP & 0x7F;	
	xfer_last_len = 1;
	new_data_ready = 1; // data send in SOF
	//fast_ep1_in((PCD_HandleTypeDef*)pdev->pData);
	return 0;
	
    if (1 || ep->xfer_len == 0)
    {
      USBx_INEP(VIDEO_IN_EP_NUM)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_PKTCNT); 
      USBx_INEP(VIDEO_IN_EP_NUM)->DIEPTSIZ |= (USB_OTG_DIEPTSIZ_PKTCNT & (1 << 19)) ;
      USBx_INEP(VIDEO_IN_EP_NUM)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_XFRSIZ); 
			
		  if ((USBx_DEVICE->DSTS & ( 1 << 8 )) == 0)
      {
        USBx_INEP(VIDEO_IN_EP_NUM)->DIEPCTL |= USB_OTG_DIEPCTL_SODDFRM;
      }
      else
      {
        USBx_INEP(VIDEO_IN_EP_NUM)->DIEPCTL |= USB_OTG_DIEPCTL_SD0PID_SEVNFRM;
      }
			
			USBx_INEP(VIDEO_IN_EP_NUM)->DIEPCTL |= (USB_OTG_DIEPCTL_CNAK | USB_OTG_DIEPCTL_EPENA);
			xfer_last_len = 1;
			return 0;
    }
    else
    {
      /* Program the transfer size and packet count
      * as follows: xfersize = N * maxpacket +
      * short_packet pktcnt = N + (short_packet
      * exist ? 1 : 0)
      */
			//if(len > ep->maxpacket) len = ep->maxpacket;
      USBx_INEP(VIDEO_IN_EP_NUM)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_XFRSIZ);
      USBx_INEP(VIDEO_IN_EP_NUM)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_PKTCNT); 
      USBx_INEP(VIDEO_IN_EP_NUM)->DIEPTSIZ |= (USB_OTG_DIEPTSIZ_PKTCNT & (((len + ep->maxpacket -1)/ ep->maxpacket) << 19)) ;
      USBx_INEP(VIDEO_IN_EP_NUM)->DIEPTSIZ |= (USB_OTG_DIEPTSIZ_XFRSIZ & len); 
      
        USBx_INEP(VIDEO_IN_EP_NUM)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_MULCNT); 
        USBx_INEP(VIDEO_IN_EP_NUM)->DIEPTSIZ |= (USB_OTG_DIEPTSIZ_MULCNT & (1 << 29)); 
    }
		
		// enable the fifo
		USBx_DEVICE->DIEPEMPMSK |= 1 << VIDEO_IN_EP_NUM;
		
		PCD_WriteEmptyTxFifo(((PCD_HandleTypeDef*)pdev->pData), VIDEO_IN_EP_NUM);
		
		//new_data_ready = 1; // start transfer in the SOF interrupt
		return 0;
}

extern USBD_HandleTypeDef hUsbDeviceFS;
void fast_ep1_in(PCD_HandleTypeDef *hpcd)
{
	USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;
	USB_OTG_EPTypeDef *ep;
	dbg_enter_xfer_done();
  ep = &hpcd->IN_ep[VIDEO_IN_EP & 0x7F];
	uint32_t len = ep->xfer_len - ep->xfer_count;
	if (len > ep->maxpacket)
	{
		len = ep->maxpacket;
	}
			
	if (len == 0)
   {
      USBx_INEP(VIDEO_IN_EP_NUM)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_PKTCNT); 
      USBx_INEP(VIDEO_IN_EP_NUM)->DIEPTSIZ |= (USB_OTG_DIEPTSIZ_PKTCNT & (1 << 19)) ;
      USBx_INEP(VIDEO_IN_EP_NUM)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_XFRSIZ); 
		 if(xfer_last_len != ep->maxpacket){
			//xfer_last_len = 0;
		 }
    }
    else
    {
      /* Program the transfer size and packet count
      * as follows: xfersize = N * maxpacket +
      * short_packet pktcnt = N + (short_packet
      * exist ? 1 : 0)
      */
			//if(len > ep->maxpacket) len = ep->maxpacket;
      USBx_INEP(VIDEO_IN_EP_NUM)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_XFRSIZ);
      USBx_INEP(VIDEO_IN_EP_NUM)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_PKTCNT); 
      USBx_INEP(VIDEO_IN_EP_NUM)->DIEPTSIZ |= (USB_OTG_DIEPTSIZ_PKTCNT & (((len + ep->maxpacket -1)/ ep->maxpacket) << 19)) ;
      USBx_INEP(VIDEO_IN_EP_NUM)->DIEPTSIZ |= (USB_OTG_DIEPTSIZ_XFRSIZ & len); 
      
        USBx_INEP(VIDEO_IN_EP_NUM)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_MULCNT); 
        USBx_INEP(VIDEO_IN_EP_NUM)->DIEPTSIZ |= (USB_OTG_DIEPTSIZ_MULCNT & (1 << 29)); 
    }
		  if ((USBx_DEVICE->DSTS & ( 1 << 8 )) == 0)
      {
        USBx_INEP(VIDEO_IN_EP_NUM)->DIEPCTL |= USB_OTG_DIEPCTL_SODDFRM;
      }
      else
      {
        USBx_INEP(VIDEO_IN_EP_NUM)->DIEPCTL |= USB_OTG_DIEPCTL_SD0PID_SEVNFRM;
      }

			if(xfer_last_len > 0){
				USBx_INEP(VIDEO_IN_EP_NUM)->DIEPCTL |= (USB_OTG_DIEPCTL_CNAK | USB_OTG_DIEPCTL_EPENA);
			}else{
				dbg_leave_xfer_frame();
				dbg_leave_xfer_done();
				dbg_stop();
				//((USBD_VIDEO_ItfTypeDef *)hUsbDeviceFS.pUserData)->FrameDone(); 
			}
			xfer_last_len = len;
			
	if(len > 0){
		uint32_t len32b;
		len32b = (len + 3) / 4;
	 
		if  ( (USBx_INEP(VIDEO_IN_EP_NUM)->DTXFSTS & USB_OTG_DTXFSTS_INEPTFSAV) < len32b)
		{
			while(1); // may not reach here
		}
		{ 
			dbg_enter_fill_data();
			USB_WritePacket(USBx, ep->xfer_buff, VIDEO_IN_EP_NUM, len, hpcd->Init.dma_enable); 
			//USB_WritePacket(USBx, (uint8_t*)xxxx, VIDEO_IN_EP_NUM, len, hpcd->Init.dma_enable); 
			dbg_leave_fill_data();
			
			ep->xfer_buff  += len;
			ep->xfer_count += len;
		}
	}
	dbg_leave_xfer_done();
}

void even_odd_toggle(PCD_HandleTypeDef *hpcd)
{
	USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;  
	if ((USBx_DEVICE->DSTS & ( 1 << 8 )) == 0)
		{
			USBx_INEP(VIDEO_IN_EP_NUM)->DIEPCTL |= USB_OTG_DIEPCTL_SODDFRM;
		}
		else
      {
        USBx_INEP(VIDEO_IN_EP_NUM)->DIEPCTL |= USB_OTG_DIEPCTL_SD0PID_SEVNFRM;
			}
}

USBD_StatusTypeDef  USBD_ISO_Transmit_sof (USBD_HandleTypeDef *pdev)
{
	USB_OTG_GlobalTypeDef *USBx = ((PCD_HandleTypeDef*)pdev->pData)->Instance;
	if(new_data_ready){
		
		fast_ep1_in((PCD_HandleTypeDef*)pdev->pData);
		/*
		  if ((USBx_DEVICE->DSTS & ( 1 << 8 )) == 0)
      {
        USBx_INEP(VIDEO_IN_EP_NUM)->DIEPCTL |= USB_OTG_DIEPCTL_SODDFRM;
      }
      else
      {
        USBx_INEP(VIDEO_IN_EP_NUM)->DIEPCTL |= USB_OTG_DIEPCTL_SD0PID_SEVNFRM;
      }
			
			
		USBx_INEP(VIDEO_IN_EP_NUM)->DIEPCTL |= (USB_OTG_DIEPCTL_CNAK | USB_OTG_DIEPCTL_EPENA);
	  */
		new_data_ready = 0;
	}
	return 0;
}

USBD_StatusTypeDef  USBD_ISO_Transmit_data_in (USBD_HandleTypeDef *pdev)
{
	// may be there is no need to toggle the odd/even bit
	USB_OTG_EPTypeDef *ep;
	USB_OTG_GlobalTypeDef *USBx = ((PCD_HandleTypeDef*)pdev->pData)->Instance;
	uint32_t len;
  ep = &((PCD_HandleTypeDef*)pdev->pData)->IN_ep[VIDEO_IN_EP & 0x7F];
	if(ep->xfer_count >= ep->xfer_len){
		// transfer complete
		return 0;
	}
	len = ep->xfer_len - ep->xfer_count;
	if (len > ep->maxpacket)
  {
    len = ep->maxpacket;
  }
	USBx_INEP(VIDEO_IN_EP_NUM)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_XFRSIZ);
	USBx_INEP(VIDEO_IN_EP_NUM)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_PKTCNT); 
	USBx_INEP(VIDEO_IN_EP_NUM)->DIEPTSIZ |= (USB_OTG_DIEPTSIZ_PKTCNT & (((len + ep->maxpacket -1)/ ep->maxpacket) << 19)) ;
	USBx_INEP(VIDEO_IN_EP_NUM)->DIEPTSIZ |= (USB_OTG_DIEPTSIZ_XFRSIZ & len); 
      
	USBx_INEP(VIDEO_IN_EP_NUM)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_MULCNT); 
	USBx_INEP(VIDEO_IN_EP_NUM)->DIEPTSIZ |= (USB_OTG_DIEPTSIZ_MULCNT & (1 << 29)); 
	
	
	if ((USBx_DEVICE->DSTS & ( 1 << 8 )) == 0)
  {
    USBx_INEP(VIDEO_IN_EP_NUM)->DIEPCTL |= USB_OTG_DIEPCTL_SODDFRM;
  }
  else
  {
    USBx_INEP(VIDEO_IN_EP_NUM)->DIEPCTL |= USB_OTG_DIEPCTL_SD0PID_SEVNFRM;
  }
	USBx_INEP(VIDEO_IN_EP_NUM)->DIEPCTL |= (USB_OTG_DIEPCTL_CNAK | USB_OTG_DIEPCTL_EPENA);
	return 0;
}



USBD_StatusTypeDef  USBD_FAST_Transmit (USBD_HandleTypeDef *pdev, 
                                      uint8_t  *pbuf,
                                      uint16_t  len)
{
	USB_OTG_EPTypeDef *ep;
	USB_OTG_GlobalTypeDef *USBx = ((PCD_HandleTypeDef*)pdev->pData)->Instance;
  ep = &((PCD_HandleTypeDef*)pdev->pData)->IN_ep[VIDEO_IN_EP & 0x7F];
  
  /*setup and start the Xfer */
  ep->xfer_buff = pbuf;  
  ep->xfer_len = len;
  ep->xfer_count = 0;
  ep->is_in = 1;
  ep->num = VIDEO_IN_EP & 0x7F;	
	
  
  
  /* IN endpoint */
  if (ep->is_in == 1)
  {
    /* Zero Length Packet? */
    if (ep->xfer_len == 0)
    {
      USBx_INEP(VIDEO_IN_EP_NUM)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_PKTCNT); 
      USBx_INEP(VIDEO_IN_EP_NUM)->DIEPTSIZ |= (USB_OTG_DIEPTSIZ_PKTCNT & (1 << 19)) ;
      USBx_INEP(VIDEO_IN_EP_NUM)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_XFRSIZ); 
    }
    else
    {
      /* Program the transfer size and packet count
      * as follows: xfersize = N * maxpacket +
      * short_packet pktcnt = N + (short_packet
      * exist ? 1 : 0)
      */
      USBx_INEP(VIDEO_IN_EP_NUM)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_XFRSIZ);
      USBx_INEP(VIDEO_IN_EP_NUM)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_PKTCNT); 
      USBx_INEP(VIDEO_IN_EP_NUM)->DIEPTSIZ |= (USB_OTG_DIEPTSIZ_PKTCNT & (((ep->xfer_len + ep->maxpacket -1)/ ep->maxpacket) << 19)) ;
      USBx_INEP(VIDEO_IN_EP_NUM)->DIEPTSIZ |= (USB_OTG_DIEPTSIZ_XFRSIZ & ep->xfer_len); 
      
        USBx_INEP(VIDEO_IN_EP_NUM)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_MULCNT); 
        USBx_INEP(VIDEO_IN_EP_NUM)->DIEPTSIZ |= (USB_OTG_DIEPTSIZ_MULCNT & (1 << 29)); 
    }

		// prepare packet for next frame, so if this frame is odd, next will be even
		  if ((USBx_DEVICE->DSTS & ( 1 << 8 )) == 0)
      {
        USBx_INEP(VIDEO_IN_EP_NUM)->DIEPCTL |= USB_OTG_DIEPCTL_SODDFRM;
      }
      else
      {
        USBx_INEP(VIDEO_IN_EP_NUM)->DIEPCTL |= USB_OTG_DIEPCTL_SD0PID_SEVNFRM;
      }
    
    /* EP enable, IN data in FIFO */
		//USBx_DEVICE->DIEPEMPMSK |= 1 << VIDEO_IN_EP_NUM;
    USBx_INEP(ep->num)->DIEPCTL |= (USB_OTG_DIEPCTL_CNAK | USB_OTG_DIEPCTL_EPENA);
    USB_WritePacket(USBx, ep->xfer_buff, ep->num, ep->xfer_len, 0);     
  }
	return 0;
}


extern uint32_t  camera_on;
static uint8_t  USBD_VIDEO_DataIn (USBD_HandleTypeDef *pdev, 
                              uint8_t epnum)
{

	static uint32_t last_len = 0;
	//USBD_ISO_Transmit_data_in(pdev);
  /* Only OUT data are processed */
	USBD_VIDEO_HandleTypeDef   *hvideo;
  hvideo = (USBD_VIDEO_HandleTypeDef*) pdev->pClassData;
	if((epnum & 0x7f) == (VIDEO_IN_EP & 0x7f) && camera_on ){
		if(hvideo->current_length < hvideo->total_length){
			uint32_t len = hvideo->total_length - hvideo->current_length;
			if(len > VIDEO_PACKET_SIZE) len = VIDEO_PACKET_SIZE;

			USBD_FAST_Transmit(pdev, hvideo->video_buffer+hvideo->current_length, len);
			hvideo->current_length += len;

		}else if(hvideo->current_length >= hvideo->total_length){
			if(last_len){
				last_len = 0;
				// zero length data
				//USBD_LL_Transmit(pdev, VIDEO_IN_EP, hvideo->video_buffer, 0);
				USBD_FAST_Transmit(pdev, hvideo->video_buffer, 0);
				//hvideo->current_length = 0;
				
			}else{
				((USBD_VIDEO_ItfTypeDef *)pdev->pUserData)->FrameDone(); 
			}
			// TODO: shall we send a zero length packet???
			//USBD_LL_Transmit(pdev, VIDEO_IN_EP, hviedo->video_buffer, 0);
			// video transfer complete
			
			
		}
	}else{
		// there only video input ep, reach here means something wrong
		// while(1);
	}
  return USBD_OK;
}

uint32_t xfer_length = 13840;
int8_t  VIDEO_Start_Transmit_Video(USBD_HandleTypeDef *pdev, uint32_t length)
{
	USBD_VIDEO_HandleTypeDef   *hvideo;
  hvideo = (USBD_VIDEO_HandleTypeDef*) pdev->pClassData;
	if(length > VIDEO_BUFFER_SIZE){
		return -1;
	}
	hvideo->total_length = length;
	hvideo->current_length = 0;
	// toggle the payload header
	hvideo->video_frame_id ^= 0x01;
	USBD_LL_FlushEP(pdev, VIDEO_IN_EP);
	new_data_ready = 1;
	return 0;
}


int8_t  VIDEO_Send_Video(USBD_HandleTypeDef *pdev, const uint8_t* data, uint32_t length)
{
	USBD_VIDEO_HandleTypeDef   *hvideo;
	uint32_t tx_len = 0;
	uint8_t* tx_buf;
	hvideo = (USBD_VIDEO_HandleTypeDef*) pdev->pClassData;
	tx_buf = hvideo->video_buffer;
	if(length > VIDEO_BUFFER_SIZE){
		return -1;
	}
	// each frame contains a frame type and frame ID
	while(tx_len < length){
		
		tx_buf[0] = VIDEO_FRAME_TYPE;
		tx_buf[1] = hvideo->video_frame_id;
		memcpy(tx_buf+2, data, (VIDEO_XFER_SIZE-2));
		tx_len += (VIDEO_XFER_SIZE-2);
		data += (VIDEO_XFER_SIZE-2);
		tx_buf += VIDEO_XFER_SIZE;
	}
	{
		uint32_t act_len = tx_buf - hvideo->video_buffer;
		uint32_t frame_cnt = (length + VIDEO_XFER_SIZE - 3) / (VIDEO_XFER_SIZE - 2);
		length = length + frame_cnt*2;
		if(act_len != length){
			//while(1);
		}
	}
	VIDEO_Start_Transmit_Video(pdev, tx_buf - hvideo->video_buffer);
	
	return 0;
}


/**
  * @brief  USBD_VIDEO_EP0_RxReady
  *         handle EP0 Rx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t  USBD_VIDEO_EP0_RxReady (USBD_HandleTypeDef *pdev)
{
  USBD_VIDEO_HandleTypeDef   *hvideo;
  hvideo = (USBD_VIDEO_HandleTypeDef*) pdev->pClassData;
  
	if (hvideo->IsReportAvailable == 1)
  {
		// TODO: parse control data, in hvideo->Report_buf[0]
    //hvideo->Report_buf[0];
    hvideo->IsReportAvailable = 0;
  }
	/*
	if(hvideo->video_interface_changed){
		if(hvideo->video_alt_setting){
			((USBD_VIDEO_ItfTypeDef *)pdev->pUserData)->CameraOn(); 
		}else{
			((USBD_VIDEO_ItfTypeDef *)pdev->pUserData)->CameraOff(); 
		}
		hvideo->video_interface_changed = 0;
	}
	*/

  return USBD_OK;
}
/**
  * @brief  USBD_VIDEO_EP0_TxReady
  *         handle EP0 TRx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t  USBD_VIDEO_EP0_TxReady (USBD_HandleTypeDef *pdev)
{
  /* Only OUT control data are processed */
  return USBD_OK;
}
/**
  * @brief  USBD_VIDEO_SOF
  *         handle SOF event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t  USBD_VIDEO_SOF (USBD_HandleTypeDef *pdev)
{
	if(new_data_ready){
		new_data_ready = 0;
		USBD_VIDEO_DataIn(pdev, VIDEO_IN_EP);
	}
  return USBD_OK;
}

/**
  * @brief  USBD_VIDEO_SOF
  *         handle SOF event
  * @param  pdev: device instance
  * @retval status
  */
void  USBD_VIDEO_Sync (USBD_HandleTypeDef *pdev, VIDEO_OffsetTypeDef offset)
{
  USBD_VIDEO_HandleTypeDef   *hv;
  hv = (USBD_VIDEO_HandleTypeDef*) pdev->pClassData;
	(void)hv;
}

/**
  * @brief  USBD_VIDEO_IsoINIncomplete
  *         handle data ISO IN Incomplete event
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_VIDEO_IsoINIncomplete (USBD_HandleTypeDef *pdev, uint8_t epnum)
{

  return USBD_OK;
}
/**
  * @brief  USBD_VIDEO_IsoOutIncomplete
  *         handle data ISO OUT Incomplete event
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_VIDEO_IsoOutIncomplete (USBD_HandleTypeDef *pdev, uint8_t epnum)
{

  return USBD_OK;
}
/**
  * @brief  USBD_VIDEO_DataOut
  *         handle data OUT Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_VIDEO_DataOut (USBD_HandleTypeDef *pdev, 
                              uint8_t epnum)
{
  USBD_VIDEO_HandleTypeDef   *hvideo;
  hvideo = (USBD_VIDEO_HandleTypeDef*) pdev->pClassData;
	
	if (epnum == CONTROL_OUT_EP){
		// TODO: parse control data, 
		control_t* ctrl = (control_t*)hvideo->control_buffer;
		((USBD_VIDEO_ItfTypeDef *)pdev->pUserData)->ControlEvent(ctrl); 
		
    USBD_LL_PrepareReceive(pdev,
                           CONTROL_OUT_EP,
                           &hvideo->control_buffer[0],
                           CONTROL_PACKET_SIZE);  
	}
  return USBD_OK;
}

/**
* @brief  DeviceQualifierDescriptor 
*         return Device Qualifier descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
static uint8_t  *USBD_VIDEO_GetDeviceQualifierDesc (uint16_t *length)
{
  *length = sizeof (USBD_VIDEO_DeviceQualifierDesc);
  return USBD_VIDEO_DeviceQualifierDesc;
}

/**
* @brief  USBD_VIDEO_RegisterInterface
* @param  fops: Audio interface callback
* @retval status
*/
uint8_t  USBD_VIDEO_RegisterInterface  (USBD_HandleTypeDef   *pdev, 
                                        USBD_VIDEO_ItfTypeDef *fops)
{
  if(fops != NULL)
  {
    pdev->pUserData= fops;
  }
  return 0;
}
/**
  * @}
  */ 


/**
  * @}
  */ 


/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
