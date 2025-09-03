
/**
  ******************************************************************************
  * @file    ov5647.h
  * @brief   Public driver interface for OV5647 (mirrors IMX335 camera API shape)
  ******************************************************************************
  */

#ifndef OV5647_H
#define OV5647_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include "ov5647_reg.h"

/* --------- IO bus glue (kept isomorphic to IMX335 style) --------- */
typedef int32_t (*OV5647_Init_Func)    (void);
typedef int32_t (*OV5647_DeInit_Func)  (void);
typedef int32_t (*OV5647_GetTick_Func) (void);
typedef int32_t (*OV5647_WriteReg_Func)(uint16_t, uint16_t, uint8_t*, uint16_t);
typedef int32_t (*OV5647_ReadReg_Func) (uint16_t, uint16_t, uint8_t*, uint16_t);

typedef struct
{
  OV5647_Init_Func          Init;
  OV5647_DeInit_Func        DeInit;
  uint16_t                  Address;
  OV5647_WriteReg_Func      WriteReg;
  OV5647_ReadReg_Func       ReadReg;
  OV5647_GetTick_Func       GetTick;
} OV5647_IO_t;

typedef struct
{
  OV5647_IO_t    IO;
  ov5647_ctx_t   Ctx;
  uint8_t        IsInitialized;
} OV5647_Object_t;

#define OV5647_SENSOR_INFO_MAX_LENGTH (32U)

typedef struct
{
  char     name[OV5647_SENSOR_INFO_MAX_LENGTH];
  uint8_t  bayer_pattern;
  uint8_t  color_depth;
  uint32_t width;
  uint32_t height;
  uint32_t gain_min;
  uint32_t gain_max;
  uint32_t exposure_min;
  uint32_t exposure_max;
} OV5647_SensorInfo_t;

typedef struct
{
  uint32_t Config_Resolution;
  uint32_t Config_LightMode;
  uint32_t Config_SpecialEffect;
  uint32_t Config_Brightness;
  uint32_t Config_Saturation;
  uint32_t Config_Contrast;
  uint32_t Config_HueDegree;
  uint32_t Config_Gain;
  uint32_t Config_Exposure;
  uint32_t Config_MirrorFlip;
  uint32_t Config_Zoom;
  uint32_t Config_NightMode;
  uint32_t Config_ExposureMode;
  uint32_t Config_SensorInfo;
  uint32_t Config_TestPattern;
  uint32_t Config_ColorEffect;
} OV5647_Capabilities_t;

/* Return codes */
#define OV5647_OK     (0)
#define OV5647_ERROR  (-1)

/* Camera features (align naming with IMX335 headers so higher layers plug-in) */
#define OV5647_R1920_1080          7U      /* 1920x1080 */
#define OV5647_R2592_1944          6U      /* 2592x1944 */
#define OV5647_RAW_RGGB10          10U     /* RAW10 */

/* Mirror/Flip config (reusing same mask values as IMX335 for drop-in) */
#define OV5647_MIRROR_FLIP_NONE    0x00U
#define OV5647_FLIP                0x01U
#define OV5647_MIRROR              0x02U
#define OV5647_MIRROR_FLIP         0x03U

/* Public driver vtable (same shape as IMX335) */
typedef struct
{
  int32_t  (*Init              )(OV5647_Object_t*, uint32_t, uint32_t);
  int32_t  (*DeInit            )(OV5647_Object_t*);
  int32_t  (*ReadID            )(OV5647_Object_t*, uint32_t*);
  int32_t  (*GetCapabilities   )(OV5647_Object_t*, OV5647_Capabilities_t*);
  int32_t  (*SetLightMode      )(OV5647_Object_t*, uint32_t);
  int32_t  (*SetColorEffect    )(OV5647_Object_t*, uint32_t);
  int32_t  (*SetBrightness     )(OV5647_Object_t*, int32_t);
  int32_t  (*SetSaturation     )(OV5647_Object_t*, int32_t);
  int32_t  (*SetContrast       )(OV5647_Object_t*, int32_t);
  int32_t  (*SetHueDegree      )(OV5647_Object_t*, int32_t);
  int32_t  (*MirrorFlipConfig  )(OV5647_Object_t*, uint32_t);
  int32_t  (*ZoomConfig        )(OV5647_Object_t*, uint32_t);
  int32_t  (*SetResolution     )(OV5647_Object_t*, uint32_t);
  int32_t  (*GetResolution     )(OV5647_Object_t*, uint32_t*);
  int32_t  (*SetPixelFormat    )(OV5647_Object_t*, uint32_t);
  int32_t  (*GetPixelFormat    )(OV5647_Object_t*, uint32_t*);
  int32_t  (*NightModeConfig   )(OV5647_Object_t*, uint32_t);
  int32_t  (*SetFrequency      )(OV5647_Object_t*, int32_t);
  int32_t  (*SetGain           )(OV5647_Object_t*, int32_t);
  int32_t  (*SetExposure       )(OV5647_Object_t*, int32_t);
  int32_t  (*SetExposureMode   )(OV5647_Object_t*, int32_t);
  int32_t  (*GetSensorInfo     )(OV5647_Object_t*, OV5647_SensorInfo_t*);
  int32_t  (*SetTestPattern    )(OV5647_Object_t*, int32_t);
} OV5647_CAMERA_Drv_t;

/* API exported by ov5647.c */
int32_t OV5647_RegisterBusIO(OV5647_Object_t *pObj, OV5647_IO_t *pIO);
int32_t OV5647_Init(OV5647_Object_t *pObj, uint32_t Resolution, uint32_t PixelFormat);
int32_t OV5647_DeInit(OV5647_Object_t *pObj);
int32_t OV5647_ReadID(OV5647_Object_t *pObj, uint32_t *Id);
int32_t OV5647_GetCapabilities(OV5647_Object_t *pObj, OV5647_Capabilities_t *Capabilities);
int32_t OV5647_SetGain(OV5647_Object_t *pObj, int32_t gain_mdb);
int32_t OV5647_SetExposure(OV5647_Object_t *pObj, int32_t exposure_us);
int32_t OV5647_SetFramerate(OV5647_Object_t *pObj, int32_t fps);
int32_t OV5647_MirrorFlipConfig(OV5647_Object_t *pObj, uint32_t Config);
int32_t OV5647_GetSensorInfo(OV5647_Object_t *pObj, OV5647_SensorInfo_t *Info);
int32_t OV5647_SetTestPattern(OV5647_Object_t *pObj, int32_t mode);

/* Global vtable instance */
extern OV5647_CAMERA_Drv_t OV5647_CAMERA_Driver;

#ifdef __cplusplus
}
#endif
#endif /* OV5647_H */
