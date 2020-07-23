/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/






#include <shared/bsl.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/SAND_FM/sand_mem_access.h>
#include <soc/dpp/SAND/SAND_FM/sand_trigger.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/TMC/tmc_api_reg_access.h>
#include <soc/dpp/TMC/tmc_api_general.h>

























uint32
  soc_tmcfield_from_reg_get(
    SOC_SAND_IN  uint32                                *reg_buffer,
    SOC_SAND_IN  SOC_TMC_REG_FIELD                           *field,
    SOC_SAND_OUT uint32                                *fld_buffer
  )
{

  uint32
    out_buff = 0,
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  SOC_SAND_CHECK_NULL_INPUT(reg_buffer);
  SOC_SAND_CHECK_NULL_INPUT(field);
  SOC_SAND_CHECK_NULL_INPUT(fld_buffer);

  res = soc_sand_bitstream_get_any_field(
    reg_buffer,
    SOC_TMC_FLD_LSB(*field),
    SOC_TMC_FLD_NOF_BITS(*field),
    &out_buff
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *fld_buffer = out_buff;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_tmcfield_from_reg_get",0,0);
}


uint32
  soc_tmcfield_from_reg_set(
    SOC_SAND_IN  uint32                                *fld_buffer,
    SOC_SAND_IN  SOC_TMC_REG_FIELD                           *field,
    SOC_SAND_OUT uint32                                *reg_buffer
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  SOC_SAND_CHECK_NULL_INPUT(fld_buffer);
  SOC_SAND_CHECK_NULL_INPUT(field);
  SOC_SAND_CHECK_NULL_INPUT(reg_buffer);

  res = soc_sand_bitstream_set_any_field(
    fld_buffer,
    SOC_TMC_FLD_LSB(*field),
    SOC_TMC_FLD_NOF_BITS(*field),
    reg_buffer
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_tmcfield_from_reg_get",0,0);
}

void
  SOC_TMC_REG_ADDR_clear(
    SOC_SAND_OUT SOC_TMC_REG_ADDR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_REG_ADDR));
  info->base = 0;
  info->step = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_REG_FIELD_clear(
    SOC_SAND_OUT SOC_TMC_REG_FIELD *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_REG_FIELD));
  SOC_TMC_REG_ADDR_clear(&(info->addr));
  info->msb = 0;
  info->lsb = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_REG_INFO_clear(
    SOC_SAND_OUT SOC_TMC_REG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_REG_INFO));
  SOC_TMC_REG_ADDR_clear(&(info->addr));
  info->val = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_POLL_INFO_clear(
    SOC_SAND_OUT SOC_TMC_POLL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_POLL_INFO));
  info->expected_value = 0;
  info->busy_wait_nof_iters = 0;
  info->timer_nof_iters = 0;
  info->timer_delay_msec = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_TMC_DEBUG_IS_LVL1

void
  SOC_TMC_REG_ADDR_print(
    SOC_SAND_IN  SOC_TMC_REG_ADDR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "base: %u[Bytes]\n\r"),info->base));
  LOG_CLI((BSL_META_U(unit,
                      "step: %u[Bytes]\n\r"),info->step));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_REG_FIELD_print(
    SOC_SAND_IN  SOC_TMC_REG_FIELD *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "addr:")));
  SOC_TMC_REG_ADDR_print(&(info->addr));
  LOG_CLI((BSL_META_U(unit,
                      "msb: %u\n\r"), info->msb));
  LOG_CLI((BSL_META_U(unit,
                      "lsb: %u\n\r"), info->lsb));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_REG_INFO_print(
    SOC_SAND_IN  SOC_TMC_REG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "addr:")));
  SOC_TMC_REG_ADDR_print(&(info->addr));
  LOG_CLI((BSL_META_U(unit,
                      "val: %u\n\r"),info->val));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_POLL_INFO_print(
    SOC_SAND_IN  SOC_TMC_POLL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "expected_value: %u\n\r"),info->expected_value));
  LOG_CLI((BSL_META_U(unit,
                      "busy_wait_nof_iters: %u\n\r"),info->busy_wait_nof_iters));
  LOG_CLI((BSL_META_U(unit,
                      "timer_nof_iters: %u\n\r"),info->timer_nof_iters));
  LOG_CLI((BSL_META_U(unit,
                      "timer_delay_msec: %u\n\r"),info->timer_delay_msec));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

