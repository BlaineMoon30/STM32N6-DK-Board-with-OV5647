
/**
  ******************************************************************************
  * @file    ov5647_reg.h
  * @brief   Header of ov5647_reg.c (OV5647 low-level register access & defines)
  ******************************************************************************
  */

#ifndef OV5647_REG_H
#define OV5647_REG_H

#include <cmsis_compiler.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---------- Generic context (same pattern as IMX335) ---------- */
typedef int32_t (*OV5647_Write_Func)(void *, uint16_t, uint8_t*, uint16_t);
typedef int32_t (*OV5647_Read_Func) (void *, uint16_t, uint8_t*, uint16_t);

typedef struct
{
  OV5647_Write_Func   WriteReg;
  OV5647_Read_Func    ReadReg;
  void               *handle;
} ov5647_ctx_t;

/* -------- Sensor ID / basic -------- */
#define OV5647_CHIP_ID_H              0x300A
#define OV5647_CHIP_ID_L              0x300B
#define OV5647_CHIP_ID_VAL_H          0x56
#define OV5647_CHIP_ID_VAL_L          0x47
#define OV5647_MODE_STANDBY         0x00
/* -------- Key registers -------- */
#define OV5647_REG_MODE_SELECT        0x0100  /* 0x00=standby, 0x01=stream */
#define OV5647_REG_SW_RESET           0x0103  /* 0x01=reset */
#define OV5647_REG_CHIP_ID_HIGH       0x300A
#define OV5647_REG_CHIP_ID_LOW        0x300B

/* PLL / format */
#define OV5647_REG_MIPI_CTRL00        0x4800
#define OV5647_REG_MIPI_TIMING        0x4837
#define OV5647_REG_DVP_MIPI_SC        0x3018
#define OV5647_REG_FORMAT             0x3034
#define OV5647_REG_PLL_SYS_DIV        0x3035
#define OV5647_REG_PLL_MULT           0x3036
#define OV5647_REG_PLL_ROOT_DIV       0x3037

#define OV5647_REG_TIMING_TC_REG20  0x3820 /* bit[1]:V flip */
#define OV5647_REG_TIMING_TC_REG21  0x3821 /* bit[1]:H mirror */

/* Timing */
#define OV5647_REG_HTS_H              0x380C
#define OV5647_REG_HTS_L              0x380D
#define OV5647_REG_VTS_H              0x380E
#define OV5647_REG_VTS_L              0x380F
#define OV5647_REG_X_START_H          0x3800
#define OV5647_REG_X_START_L          0x3801
#define OV5647_REG_Y_START_H          0x3802
#define OV5647_REG_Y_START_L          0x3803
#define OV5647_REG_X_END_H            0x3804
#define OV5647_REG_X_END_L            0x3805
#define OV5647_REG_Y_END_H            0x3806
#define OV5647_REG_Y_END_L            0x3807
#define OV5647_REG_X_OUT_H            0x3808
#define OV5647_REG_X_OUT_L            0x3809
#define OV5647_REG_Y_OUT_H            0x380A
#define OV5647_REG_Y_OUT_L            0x380B

/* Mirror/Flip (색필터 배열 정렬에 영향) */
#define OV5647_REG_TC_REG20           0x3820
#define OV5647_REG_TC_REG21           0x3821

/* Exposure/Gain */
#define OV5647_REG_AEC_AGC            0x3503
#define OV5647_REG_EXPOSURE_H         0x3500  /* [19:16] */
#define OV5647_REG_EXPOSURE_M         0x3501  /* [15:8]  */
#define OV5647_REG_EXPOSURE_L         0x3502  /* [7:4] fractional */
#define OV5647_REG_GAIN_H             0x350A
#define OV5647_REG_GAIN_L             0x350B

/* Test pattern */
#define OV5647_REG_TEST_PATTERN       0x503D

/* MIPI control bits (0x4800) */
#define OV5647_MIPI_HS_ONLY           (1u<<7)
#define OV5647_MIPI_LINE_SYNC_EN      (1u<<4)
#define OV5647_MIPI_IDLE_LP11         (1u<<2)
#define OV5647_MIPI_CLK_GATE          (1u<<5)
#define REG_TEST_PATTERN           0x503D  /* 0x80=enable color bar */
/* Resolutions */
#define OV5647_R2592_1944             (0u)
#define OV5647_R1920_1080             (1u)

/* PixelFormat (RAW10 RGGB) — 값만 전달용 */
#define OV5647_RAW_RGGB10             (0u)

/* Info/Cap 구조체는 기존 IMX335과 동일하게 매핑해서 사용 */
#define OV5647_NAME            "OV5647"
#define OV5647_BAYER_PATTERN   0 /* RGGB */
#define OV5647_COLOR_DEPTH     10
#define OV5647_WIDTH           1920
#define OV5647_HEIGHT          1080
#define OV5647_GAIN_MIN_MDB    0
#define OV5647_GAIN_MAX_MDB    4800
#define OV5647_EXPOSURE_MIN_US 50
#define OV5647_EXPOSURE_MAX_US 1000000

/* Low-level API */
int32_t ov5647_write_reg(ov5647_ctx_t *ctx, uint16_t reg, uint8_t *pdata, uint16_t length);
int32_t ov5647_read_reg (ov5647_ctx_t *ctx, uint16_t reg, uint8_t *pdata, uint16_t length);

#ifdef __cplusplus
}
#endif

#endif /* OV5647_REG_H */
