/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __SOC_TMC_API_REG_ACCESS_H_INCLUDED__

#define __SOC_TMC_API_REG_ACCESS_H_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/TMC/tmc_api_general.h>






#define SOC_TMC_DEFAULT_INSTANCE   0xFF









#define SOC_TMC_FLD_NOF_BITS(fld)                                               \
  ((fld).msb - (fld).lsb + 1)
#define SOC_TMC_FLD_LSB(fld)                                                    \
  (fld).lsb
#define SOC_TMC_FLD_MASK(fld)                                                   \
  (uint32)((SOC_SAND_BIT((fld).msb) - SOC_SAND_BIT((fld).lsb)) + SOC_SAND_BIT((fld).msb))
#define SOC_TMC_FLD_SHIFT(fld)                                                  \
  (fld).lsb
#define SOC_TMC_FLD_MAX(fld)                                                    \
  (SOC_TMC_FLD_MASK(fld)>>SOC_TMC_FLD_SHIFT(fld))














#define SOC_TMC_FLD_DEFINE(fld_var,fld_offset,fld_size) \
{                                                     \
  fld_var.lsb = fld_offset;                           \
  fld_var.msb = (fld_offset) + (fld_size) - 1;            \
}





typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 base;
  
  uint16 step;

} SOC_TMC_REG_ADDR;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_REG_ADDR addr;
  
  uint8 msb;
  
  uint8 lsb;

} SOC_TMC_REG_FIELD;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_REG_ADDR addr;
  
  uint32 val;

} SOC_TMC_REG_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 expected_value;
  
  uint32 busy_wait_nof_iters;
  
  uint32 timer_nof_iters;
  
  uint32 timer_delay_msec;

} SOC_TMC_POLL_INFO;












uint32
  soc_tmcfield_from_reg_get(
    SOC_SAND_IN  uint32          *reg_buffer,
    SOC_SAND_IN  SOC_TMC_REG_FIELD     *field,
    SOC_SAND_OUT uint32          *fld_buffer
  );

void
  SOC_TMC_REG_ADDR_clear(
    SOC_SAND_OUT SOC_TMC_REG_ADDR *info
  );

void
  SOC_TMC_REG_FIELD_clear(
    SOC_SAND_OUT SOC_TMC_REG_FIELD *info
  );

void
  SOC_TMC_REG_INFO_clear(
    SOC_SAND_OUT SOC_TMC_REG_INFO *info
  );

void
  SOC_TMC_POLL_INFO_clear(
    SOC_SAND_OUT SOC_TMC_POLL_INFO *info
  );

#if SOC_TMC_DEBUG_IS_LVL1

void
  SOC_TMC_REG_ADDR_print(
    SOC_SAND_IN  SOC_TMC_REG_ADDR *info
  );

void
  SOC_TMC_REG_FIELD_print(
    SOC_SAND_IN  SOC_TMC_REG_FIELD *info
  );

void
  SOC_TMC_REG_INFO_print(
    SOC_SAND_IN  SOC_TMC_REG_INFO *info
  );

void
  SOC_TMC_POLL_INFO_print(
    SOC_SAND_IN  SOC_TMC_POLL_INFO *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
