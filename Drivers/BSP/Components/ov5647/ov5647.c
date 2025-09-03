
/**
  ******************************************************************************
  * @file    ov5647.c
  * @brief   OV5647 camera driver (ported to match IMX335 driver interface)
  ******************************************************************************
  */

/**
  ******************************************************************************
  * @file    ov5647.c
  * @brief   OV5647 camera driver (ported to match IMX335 driver interface)
  ******************************************************************************
  */

#include "ov5647.h"
#include <string.h>
#include <stdint.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/* 25 MHz XCLK (module on-board XO) — used only for reference notes.
   In this driver we reconstruct pclk from (HTS*VTS*fps_nominal) after loading a mode. */
#define OV5647_XCLK_HZ   (25000000UL)

/* ---- Small helper struct for register tables ---- */
struct regval { uint16_t addr; uint8_t val; };

/* ---- Timing cache (current mode) ----
   We keep HTS/VTS and nominal fps to reconstruct pclk = HTS * VTS * fps.
   This provides a coherent base for exposure/fps calculations without reprogramming PLL. */
static uint16_t s_hts = 0;      /* LINE_LEN_PCK (0x380C/0x380D) */
static uint16_t s_vts = 0;      /* FRAME_LEN_LINES (0x380E/0x380F) */
static uint32_t s_pclk = 0;     /* pixel clock (Hz). s_pclk = s_hts * s_vts * s_fps */
static int32_t  s_fps  = 30;    /* nominal fps of current mode */

/* ---- Private helpers ---- */
static int32_t OV5647_WriteTable(OV5647_Object_t *pObj, const struct regval *regs, uint32_t size)
{
  for (uint32_t i = 0; i < size; ++i)
  {
    if (ov5647_write_reg(&pObj->Ctx, regs[i].addr, (uint8_t*)&regs[i].val, 1) != 0)
      return OV5647_ERROR;
  }
  return OV5647_OK;
}

static void ov5647_update_timing_cache(OV5647_Object_t *pObj, int32_t fps_nominal)
{

}

static const struct regval ov5647_1920x1080_regs[] =
{

    {0x0100, 0x00},  /* Stream Off */
    {0x0103, 0x01},  /* SW reset  */

    {0x3034, 0x1A},  /* RAW10 */
    {0x3035, 0x21},  /* PLL: sys divider  */
    {0x3036, 0x62},  /* PLL multiplier */
    {0x303C, 0x11},
    {0x3106, 0xF5},

    {0x3820, 0x46},
    {0x3821, 0x06},

    {0x3827, 0xEC},
    {0x370C, 0x03},
    {0x3612, 0x5B},
    {0x3618, 0x04},

    {0x5000, 0x06},
    {0x5002, 0x41},
    {0x5003, 0x08},
    {0x5A00, 0x08},

    {0x3000, 0x00},
    {0x3001, 0x00},
    {0x3002, 0x00},
    {0x3016, 0x08},
    {0x3017, 0xE0},
    {0x3018, 0x44},  /* MIPI Enable + 2-lane */

    {0x301C, 0xF8},
    {0x301D, 0xF0},

    {0x3A18, 0x00},
    {0x3A19, 0xF8},
    {0x3C01, 0x80},
    {0x3B07, 0x0C},

    {0x380C, 0x09}, {0x380D, 0x70},  /* HTS = 0x0970 = 2416 */
    {0x380E, 0x04}, {0x380F, 0x50},  /* VTS = 0x0450 = 1104 */

    {0x3814, 0x11},
    {0x3815, 0x11},

    {0x3708, 0x64},
    {0x3709, 0x12},

    {0x3808, 0x07}, {0x3809, 0x80},  /* X_OUT = 1920 */
    {0x380A, 0x04}, {0x380B, 0x38},  /* Y_OUT = 1080 */

    {0x3800, 0x01}, {0x3801, 0x5C},  /* X start */
    {0x3802, 0x01}, {0x3803, 0xB2},  /* Y start */
    {0x3804, 0x08}, {0x3805, 0xE3},  /* X end */
    {0x3806, 0x05}, {0x3807, 0xF1},  /* Y end */

    {0x3811, 0x04},
    {0x3813, 0x02},

    {0x3630, 0x2E},
    {0x3632, 0xE2},
    {0x3633, 0x23},
    {0x3634, 0x44},
    {0x3636, 0x06},
    {0x3620, 0x64},
    {0x3621, 0xE0},
    {0x3600, 0x37},
    {0x3704, 0xA0},
    {0x3703, 0x5A},
    {0x3715, 0x78},
    {0x3717, 0x01},
    {0x3731, 0x02},
    {0x370B, 0x60},
    {0x3705, 0x1A},
    {0x3F05, 0x02},
    {0x3F06, 0x10},
    {0x3F01, 0x0A},

    {0x3A08, 0x01},
    {0x3A09, 0x4B},
    {0x3A0A, 0x01},
    {0x3A0B, 0x13},
    {0x3A0D, 0x04},
    {0x3A0E, 0x03},
    {0x3A0F, 0x58},
    {0x3A10, 0x50},
    {0x3A1B, 0x58},
    {0x3A1E, 0x50},
    {0x3A11, 0x60},
    {0x3A1F, 0x28},

    {0x4001, 0x02},
    {0x4004, 0x04},
    {0x4000, 0x09},

    {0x4837, 0x19},
    {0x4800, 0x34},

    {0x3503, 0x00},

    {0x3500, 0x00},  /* exposure[19:16] */
    {0x3501, 0x40},  /* exposure[15:8]  */
    {0x3502, 0x00},  /* exposure[7:4]   */
    {0x350A, 0x00},  /* gain[9:8]       */
    {0x350B, 0x10},  /* gain[7:0]       */

};
/* Test pattern helpers */
static const struct regval test_pattern_enable_bar[] = {
  {OV5647_REG_TEST_PATTERN, 0x80 | (0x0 << 2)} /* enable, color bar type 0 */
};
static const struct regval test_pattern_disable[] = {
  {OV5647_REG_TEST_PATTERN, 0x00}
};

/* ---- Public driver table ---- */
OV5647_CAMERA_Drv_t OV5647_CAMERA_Driver =
{
  .Init            = OV5647_Init,
  .DeInit          = OV5647_DeInit,
  .ReadID          = OV5647_ReadID,
  .GetCapabilities = OV5647_GetCapabilities,
  .SetLightMode    = NULL,
  .SetColorEffect  = NULL,
  .SetBrightness   = NULL,
  .SetSaturation   = NULL,
  .SetContrast     = NULL,
  .SetHueDegree    = NULL,
  .MirrorFlipConfig= OV5647_MirrorFlipConfig,
  .ZoomConfig      = NULL,
  .SetResolution   = NULL,
  .GetResolution   = NULL,
  .SetPixelFormat  = NULL,
  .GetPixelFormat  = NULL,
  .NightModeConfig = NULL,
  .SetFrequency    = NULL,
  .SetGain         = OV5647_SetGain,
  .SetExposure     = OV5647_SetExposure,
  .SetExposureMode = NULL,
  .GetSensorInfo   = OV5647_GetSensorInfo,
  .SetTestPattern  = OV5647_SetTestPattern
};

/* ---- IO registration (same pattern as IMX335) ---- */
static int32_t OV5647_ReadRegWrap(void *handle, uint16_t Reg, uint8_t* pData, uint16_t Length)
{
  OV5647_Object_t *pObj = (OV5647_Object_t *)handle;
  return pObj->IO.ReadReg(pObj->IO.Address, Reg, pData, Length);
}
static int32_t OV5647_WriteRegWrap(void *handle, uint16_t Reg, uint8_t* pData, uint16_t Length)
{
  OV5647_Object_t *pObj = (OV5647_Object_t *)handle;
  return pObj->IO.WriteReg(pObj->IO.Address, Reg, pData, Length);
}

int32_t OV5647_RegisterBusIO(OV5647_Object_t *pObj, OV5647_IO_t *pIO)
{
  if (!pObj) return OV5647_ERROR;
  pObj->IO.Init      = pIO->Init;
  pObj->IO.DeInit    = pIO->DeInit;
  pObj->IO.Address   = pIO->Address;
  pObj->IO.WriteReg  = pIO->WriteReg;
  pObj->IO.ReadReg   = pIO->ReadReg;
  pObj->IO.GetTick   = pIO->GetTick;

  pObj->Ctx.ReadReg  = OV5647_ReadRegWrap;
  pObj->Ctx.WriteReg = OV5647_WriteRegWrap;
  pObj->Ctx.handle   = pObj;

  if (pObj->IO.Init) return pObj->IO.Init();
  return OV5647_ERROR;
}


#define OV5647_VERIFY_RETRIES   3
#define OV5647_VERIFY_DELAY_MS  20
#define OV5647_RESET_DELAY_MS   5


static const uint16_t ov5647_verify_skip[] = {
  0x0103,
};

static int ov5647_is_verify_skipped(uint16_t addr)
{
  for (size_t i=0; i<sizeof(ov5647_verify_skip)/sizeof(ov5647_verify_skip[0]); ++i)
    if (ov5647_verify_skip[i] == addr) return 1;
  return 0;
}


static int32_t ov5647_write_verify(OV5647_Object_t *pObj, uint16_t addr, uint8_t val)
{
  uint8_t rd = 0x00;
  //for (int t=0; t<OV5647_VERIFY_RETRIES; ++t)
  {
	  printf("[OV5647] Write reg 0x%04X=0x%02X (verify skipped)\r\n", addr, val);
    if (ov5647_write_reg(&pObj->Ctx, addr, &val, 1) != 0)
      return OV5647_ERROR;

    if (addr == 0x0103) {
      HAL_Delay(OV5647_RESET_DELAY_MS);
      return OV5647_OK;
    }

    if (ov5647_is_verify_skipped(addr))
      return OV5647_OK;

    if (ov5647_read_reg(&pObj->Ctx, addr, &rd, 1) != 0)
      return OV5647_ERROR;

    printf("[OV5647] Read  reg 0x%04X=0x%02X (verify skipped)\r\n", addr, rd);

    HAL_Delay(OV5647_VERIFY_DELAY_MS);
  }
  /* 마지막 읽은 값과 기대값을 로그로 남기고 실패 처리(원하면 printf/COM 로그 사용) */
#if USE_COM_LOG
  printf("OV5647 verify fail: reg=0x%04X write=0x%02X read=0x%02X\r\n", addr, val, rd);
#endif
  return OV5647_OK;
}

static int32_t OV5647_WriteTable_Verify(OV5647_Object_t *pObj,
                                        const struct regval *regs, uint32_t size)
{
  for (uint32_t i=0; i<size; ++i)
  {
    int32_t st = ov5647_write_verify(pObj, regs[i].addr, regs[i].val);
    if (st != OV5647_OK) return st;

    if (regs[i].addr == 0x3036) { HAL_Delay(3); }
  }
  return OV5647_OK;
}



/* ---- Init / DeInit ---- */
int32_t OV5647_Init(OV5647_Object_t *pObj, uint32_t Resolution, uint32_t PixelFormat)
{
  (void)PixelFormat;
  if (pObj->IsInitialized) return OV5647_OK;

  /* Check Chip ID */
  uint8_t idh=0, idl=0;
  if (ov5647_read_reg(&pObj->Ctx, OV5647_REG_CHIP_ID_HIGH, &idh, 1) != 0) return OV5647_ERROR;
  if (ov5647_read_reg(&pObj->Ctx, OV5647_REG_CHIP_ID_LOW,  &idl, 1) != 0) return OV5647_ERROR;
  if (idh != 0x56 || idl != 0x47) return OV5647_ERROR;

  /* Load mode (tables already include Stream On at the end) */
  switch (Resolution)
  {

    case OV5647_R1920_1080:

#if 1
        if (OV5647_WriteTable(pObj, ov5647_1920x1080_regs, ARRAY_SIZE(ov5647_1920x1080_regs)) != OV5647_OK)
          return OV5647_ERROR;
#else
    	if (OV5647_WriteTable_Verify(pObj,
    	      ov5647_1920x1080_regs,
    	      ARRAY_SIZE(ov5647_1920x1080_regs)) != OV5647_OK)
    	{
    	  return OV5647_ERROR;
    	}

    	//while(1);
#endif
      break;

    default:
      return OV5647_ERROR;
  }

  /* Do NOT re-write Mode Select here (tables already turned streaming ON) */

  pObj->IsInitialized = 1U;
  return OV5647_OK;
}

int32_t OV5647_DeInit(OV5647_Object_t *pObj)
{
  if (pObj->IsInitialized)
  {
    uint8_t mode = OV5647_MODE_STANDBY;
    ov5647_write_reg(&pObj->Ctx, OV5647_REG_MODE_SELECT, &mode, 1);
    pObj->IsInitialized = 0;
  }
  return OV5647_OK;
}

/* ---- ID / Capabilities / Info ---- */
int32_t OV5647_ReadID(OV5647_Object_t *pObj, uint32_t *Id)
{
  uint8_t idh=0, idl=0;
  if (ov5647_read_reg(&pObj->Ctx, OV5647_REG_CHIP_ID_HIGH, &idh, 1) != 0) return OV5647_ERROR;
  if (ov5647_read_reg(&pObj->Ctx, OV5647_REG_CHIP_ID_LOW,  &idl, 1) != 0) return OV5647_ERROR;
  *Id = ((uint32_t)idh << 8) | idl;
  return OV5647_OK;
}

int32_t OV5647_GetCapabilities(OV5647_Object_t *pObj, OV5647_Capabilities_t *Capabilities)
{
  (void)pObj;
  if (!Capabilities) return OV5647_ERROR;
  Capabilities->Config_Brightness    = 0;
  Capabilities->Config_Contrast      = 0;
  Capabilities->Config_HueDegree     = 0;
  Capabilities->Config_Gain          = 1;
  Capabilities->Config_Exposure      = 1;
  Capabilities->Config_ExposureMode  = 0;
  Capabilities->Config_LightMode     = 0;
  Capabilities->Config_MirrorFlip    = 1;
  Capabilities->Config_NightMode     = 0;
  Capabilities->Config_Resolution    = 0;
  Capabilities->Config_Saturation    = 0;
  Capabilities->Config_SpecialEffect = 0;
  Capabilities->Config_Zoom          = 0;
  Capabilities->Config_SensorInfo    = 1;
  Capabilities->Config_TestPattern   = 1;
  Capabilities->Config_ColorEffect   = 0;
  return OV5647_OK;
}

int32_t OV5647_GetSensorInfo(OV5647_Object_t *pObj, OV5647_SensorInfo_t *Info)
{
  (void)pObj;
  if (!Info) return OV5647_ERROR;
  const char *name = OV5647_NAME;
  if (sizeof(Info->name) < strlen(name)+1) return OV5647_ERROR;
  strcpy(Info->name, name);
  Info->bayer_pattern = OV5647_BAYER_PATTERN;
  Info->color_depth   = OV5647_COLOR_DEPTH;
  Info->width         = OV5647_WIDTH;
  Info->height        = OV5647_HEIGHT;
  Info->gain_min      = OV5647_GAIN_MIN_MDB;
  Info->gain_max      = OV5647_GAIN_MAX_MDB;
  Info->exposure_min  = OV5647_EXPOSURE_MIN_US;
  Info->exposure_max  = OV5647_EXPOSURE_MAX_US;
  return OV5647_OK;
}

/* ---- Controls ---- */
int32_t OV5647_SetGain(OV5647_Object_t *pObj, int32_t gain_mdb)
{
  /* Bring-up approximation: map 0..48 dB -> 0x10..0xF8 linearly. */
  if (gain_mdb < OV5647_GAIN_MIN_MDB) gain_mdb = OV5647_GAIN_MIN_MDB;
  if (gain_mdb > OV5647_GAIN_MAX_MDB) gain_mdb = OV5647_GAIN_MAX_MDB;

  uint16_t code = 0x10 + (uint16_t)(( (int64_t)gain_mdb * (int64_t)(0xF8 - 0x10) ) /
                                    (int64_t)(OV5647_GAIN_MAX_MDB ? OV5647_GAIN_MAX_MDB : 1));

  uint8_t h = (code >> 8) & 0xFF;
  uint8_t l = code & 0xFF;

  if (ov5647_write_reg(&pObj->Ctx, OV5647_REG_GAIN_H, &h, 1) != 0) return OV5647_ERROR;
  if (ov5647_write_reg(&pObj->Ctx, OV5647_REG_GAIN_L, &l, 1) != 0) return OV5647_ERROR;
  return OV5647_OK;
}

int32_t OV5647_SetExposure(OV5647_Object_t *pObj, int32_t exposure_us)
{
  if (exposure_us < (int32_t)OV5647_EXPOSURE_MIN_US) exposure_us = OV5647_EXPOSURE_MIN_US;

  /* Refresh timing cache if needed */
  if (s_hts == 0 || s_vts == 0 || s_pclk == 0) ov5647_update_timing_cache(pObj, 30);

  /* lines = exposure_us * pclk / (HTS * 1e6) */
  uint64_t num   = (uint64_t)exposure_us * (uint64_t)s_pclk;
  uint64_t denom = (uint64_t)s_hts * 1000000ULL;
  uint32_t lines = (uint32_t)((num + (denom/2ULL)) / denom);
  if (lines == 0) lines = 1;

  /* Ensure exposure lines fit into VTS with a margin */
  const uint32_t margin = 8;
  if (s_vts != 0 && lines > (uint32_t)(s_vts - margin))
    lines = (uint32_t)(s_vts - margin);

  /* OV5647 exposure format: [19:16]=H[3:0], [15:8]=M, [7:4]=L[7:4] (4 LSB are fractional) */
  uint8_t h = (lines >> 12) & 0x0F;
  uint8_t m = (lines >> 4)  & 0xFF;
  uint8_t l = (lines << 4)  & 0xF0;

  if (ov5647_write_reg(&pObj->Ctx, OV5647_REG_EXPOSURE_H, &h, 1) != 0) return OV5647_ERROR;
  if (ov5647_write_reg(&pObj->Ctx, OV5647_REG_EXPOSURE_M, &m, 1) != 0) return OV5647_ERROR;
  if (ov5647_write_reg(&pObj->Ctx, OV5647_REG_EXPOSURE_L, &l, 1) != 0) return OV5647_ERROR;

  return OV5647_OK;
}

int32_t OV5647_SetFramerate(OV5647_Object_t *pObj, int32_t fps_target)
{

}

int32_t OV5647_MirrorFlipConfig(OV5647_Object_t *pObj, uint32_t Config)
{

}

int32_t OV5647_SetTestPattern(OV5647_Object_t *pObj, int32_t mode)
{
  if (mode >= 0) /* enable simple color bars regardless of mode index */
    return OV5647_WriteTable(pObj, test_pattern_enable_bar, ARRAY_SIZE(test_pattern_enable_bar));
  else
    return OV5647_WriteTable(pObj, test_pattern_disable, ARRAY_SIZE(test_pattern_disable));
}
