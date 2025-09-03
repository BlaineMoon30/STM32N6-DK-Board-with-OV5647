
/**
  ******************************************************************************
  * @file    ov5647.c
  * @brief   OV5647 camera driver (ported to match IMX335 driver interface)
  ******************************************************************************
  */

/* ov5647_reg.c — LOW LEVEL ONLY */
#include "ov5647_reg.h"

int32_t ov5647_read_reg(ov5647_ctx_t *ctx, uint16_t reg, uint8_t *pdata, uint16_t length)
{
  return ctx->ReadReg(ctx->handle, reg, pdata, length);
}

int32_t ov5647_write_reg(ov5647_ctx_t *ctx, uint16_t reg, uint8_t *pdata, uint16_t length)
{
  return ctx->WriteReg(ctx->handle, reg, pdata, length);
}

