/** \file src/bcm/dnx/spb/spb_otp.c
 *
 *
 *  This file contains implementation of functions for reading OCB (SRAM) information from OTP
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_DRAM

/** allow memregs.h include excplictly */
#ifdef _SOC_MEMREGS_H
#undef _SOC_MEMREGS_H
#endif
/*
 * Include files.
 * {
 */

#include <shared/shrextend/shrextend_debug.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/mcm/memregs.h>
#include <soc/cm.h>
#include <soc/cmic.h>
#include <soc/util.h>
#include <sal/appl/sal.h>
#include <soc/sand/sand_aux_access.h>
#include <soc/dnxc/dnxc_ha.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_spb.h>
#include <shared/utilex/utilex_bitstream.h>
#include <include/bcm_int/dnx/cmn/dnxcmn.h>
#include "spb_otp.h"

/*
 * defines
 * {
 */

#define DNX_SPB_OTP_BLOCK_ID                 (1984)
#define TOTAL_NOF_BITS_RING_INFO(unit)       (dnx_data_spb.ocb.otp_defective_bank_nof_bits_get(unit) + dnx_data_spb.ocb.otp_is_any_bank_defective_nof_bits_get(unit))
#define IS_ANY_BANK_DEFECTIVE_OFFSET(unit)   (dnx_data_spb.ocb.otp_defective_bank_nof_bits_get(unit))

/*
 * }
 */

/**
 * \brief - write to otp register
 *
 * \param [in] unit - unit number
 * \param [in] addr - address of the register
 * \param [in] val - value
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_spb_otp_reg_write(
    int unit,
    int addr,
    uint32 val)
{
    SHR_FUNC_INIT_VARS(unit);

    addr = addr << 8;
    SHR_IF_ERR_EXIT(soc_direct_reg_set(unit, DNX_SPB_OTP_BLOCK_ID, addr, 1, &val));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - read from otp register
 *
 * \param [in] unit - unit number
 * \param [in] addr - address of the register
 * \param [out] *val - read value
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_spb_otp_reg_read(
    int unit,
    uint32 addr,
    uint32 *val)
{
    SHR_FUNC_INIT_VARS(unit);

    addr = addr << 8;
    SHR_IF_ERR_EXIT(soc_direct_reg_get(unit, DNX_SPB_OTP_BLOCK_ID, addr, 1, val));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - read a single word from otp
 *
 * \param [in] unit - unit number
 * \param [in] core - core index
 * \param [in] addr - address of the OTP register to read
 * \param [out] status - status indication from otp.
 * \param [out] data - pointer to which data will be read from otp
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_spb_otp_mem_word_read(
    int unit,
    int core,
    uint32 addr,
    uint32 *status,
    uint32 *data)
{
    soc_timeout_t timeout;
    sal_usecs_t timeout_value = 100000;

    /** All address were taken from OTP regfile located in :
        $PERSONAL_PATH/verification/full_chip/env/regfile/otpc_sbus_regfile_base.sv */
    const int OTPC_MODE_REG_reg_ = 0x200;
    const int OTPC_CPUADDR_REG_reg_ = 0x228;
    const int OTPC_COMMAND_reg_ = 0x204;
    const int OTPC_CMD_START_reg_ = 0x208;
    const int OTPC_CPU_DATA0_reg_ = 0x210;
    const int OTPC_CPU_STATUS_reg_ = 0x20c;
    const int OTP_READ_CMD = 0x0;
    SHR_FUNC_INIT_VARS(unit);

    /** enable CPU access */
    SHR_IF_ERR_EXIT(dnx_spb_otp_reg_write(unit, OTPC_MODE_REG_reg_, 0x1));

    /** address */
    SHR_IF_ERR_EXIT(dnx_spb_otp_reg_write(unit, OTPC_CPUADDR_REG_reg_, addr));

    /** command  */

    SHR_IF_ERR_EXIT(dnx_spb_otp_reg_write(unit, OTPC_COMMAND_reg_, OTP_READ_CMD));

    /** start - command write enable  */
    SHR_IF_ERR_EXIT(dnx_spb_otp_reg_write(unit, OTPC_CMD_START_reg_, 0x1));

    /** read cpu status */
    soc_timeout_init(&timeout, timeout_value, 0);
    do
    {
        SHR_IF_ERR_EXIT(dnx_spb_otp_reg_read(unit, OTPC_CPU_STATUS_reg_, status));
        /** check done indication bit */
        if (((*status) & 0x2) != 0)
        {
            break;
        }
        if (soc_timeout_check(&timeout))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "ERROR: (read_otp_mem_word) READ operation not done. CPU status = 0x%x.\n",
                         (*status));
        }
    }
    while (TRUE);

    SHR_IF_ERR_EXIT(dnx_spb_otp_reg_read(unit, OTPC_CPU_DATA0_reg_, data));

    /** disable - command write disable  */
    SHR_IF_ERR_EXIT(dnx_spb_otp_reg_write(unit, OTPC_CMD_START_reg_, 0x0));

    /** disable CPU access */
    SHR_IF_ERR_EXIT(dnx_spb_otp_reg_write(unit, OTPC_MODE_REG_reg_, 0));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - check in OTP bank info if a ring has any defective bank in it
 *
 * \param [in]  unit - unit number
 * \param [in]  banks_info - defective banks info as read from OTP
 * \param [in]  core - core index
 * \param [in]  ring - ring in core index
 * \return
 *   int (TRUE / FALSE)
 *
 * \remark
 *   * None
 * \see
 *   * None
 */

int
dnx_spb_otp_is_any_bank_defective(
    int unit,
    uint32 *banks_info,
    int core,
    int ring)
{
    return SHR_IS_BITSET(banks_info,
                         (core * dnx_data_spb.ocb.nof_rings_get(unit) + ring) * TOTAL_NOF_BITS_RING_INFO(unit) +
                         IS_ANY_BANK_DEFECTIVE_OFFSET(unit));
}

/**
 * \brief - get defective bank from OTP defective bank info
 *
 * \param [in]  unit - unit number
 * \param [in]  banks_info - defective banks info as read from OTP
 * \param [in]  core - core index
 * \param [in]  ring - ring in core index
 * \return
 *   int (bank index relative to core)
 *
 * \remark
 *   * None
 * \see
 *   * None
 */

int
dnx_spb_otp_defective_bank_get(
    int unit,
    uint32 *banks_info,
    int core,
    int ring)
{
    uint32 defective_bank = 0;

    SHR_BITCOPY_RANGE(&defective_bank,
                      0,
                      banks_info,
                      (core * dnx_data_spb.ocb.nof_rings_get(unit) + ring) * TOTAL_NOF_BITS_RING_INFO(unit),
                      dnx_data_spb.ocb.otp_defective_bank_nof_bits_get(unit));

    return defective_bank;
}

/**
 * \brief - read defective banks info
 *
 * \param [in]  unit - unit number
 * \param [out] defective_banks - which (if any) banks are defective (per ring per core)
 * \return
 *   shr_error_e
 *
 * \remark
 *   * caller is responsible for initializing defective_banks array
 *   * in a ring with no defective bank, value in outparam array statys as is
 * \see
 *   * None
 */

shr_error_e
dnx_spb_otp_defective_banks_info_read(
    int unit,
    int defective_banks[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES][DNX_DATA_MAX_SPB_OCB_NOF_RINGS])
{
    const int SPB_OTP_DEFECTIVE_BANK_INFO_ADDR = 0x9;
    uint32 banks_info[1] = { 0 };
    uint32 status;
    int core;
    int ring;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_spb_otp_mem_word_read(unit, 0, SPB_OTP_DEFECTIVE_BANK_INFO_ADDR, &status, banks_info));

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        for (ring = 0; ring < dnx_data_spb.ocb.nof_rings_get(unit); ring++)
        {
            if (dnx_spb_otp_is_any_bank_defective(unit, banks_info, core, ring))
            {
                defective_banks[core][ring] = dnx_spb_otp_defective_bank_get(unit, banks_info, core, ring);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}
