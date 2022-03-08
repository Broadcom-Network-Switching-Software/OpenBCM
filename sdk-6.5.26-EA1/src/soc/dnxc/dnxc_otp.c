
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOC_OTP
#define SOC_DNXC_OTP_REG_ADDR_SHIFT         (8)

#ifdef _SOC_MEMREGS_H
#undef _SOC_MEMREGS_H
#endif

#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/types.h>
#include <soc/cmic.h>
#include <include/soc/dnxc/dnxc_otp.h>
#include <soc/drv.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/utilex/utilex_integer_arithmetic.h>

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_device.h>
#endif

#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_device.h>
#endif

typedef struct
{
    uint32 mode;
    uint32 cpu_addr;
    uint32 cmd;
    uint32 cmd_start;
    uint32 cpu_status;
    uint32 cpu_data_0;
    uint32 cpu_data_1;
    uint32 cpu_data_2;
} dnxc_data_device_otp_reg_addresses_t;

typedef struct
{
    int block_id;
    int is_supported;
    int size;
} dnxc_data_device_otp_otp_instance_attributes_t;

static shr_error_e
soc_dnxc_otp_reg_write(
    int unit,
    int addr,
    uint32 block_id,
    uint32 val)
{
    SHR_FUNC_INIT_VARS(unit);

    addr = addr << SOC_DNXC_OTP_REG_ADDR_SHIFT;
    SHR_IF_ERR_EXIT(soc_direct_reg_set(unit, block_id, addr, 1, &val));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
soc_dnxc_otp_reg_read(
    int unit,
    uint32 addr,
    uint32 block_id,
    uint32 *val)
{
    SHR_FUNC_INIT_VARS(unit);

    addr = addr << SOC_DNXC_OTP_REG_ADDR_SHIFT;
    SHR_IF_ERR_EXIT(soc_direct_reg_get(unit, block_id, addr, 1, val));

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxc_otp_nof_words_in_row_get(
    int unit)
{
    int nof_rows = -1;
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        nof_rows = dnx_data_device.otp.nof_words_in_row_get(unit);
    }
#endif
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        nof_rows = dnxf_data_device.otp.nof_words_in_row_get(unit);
    }
#endif

    return nof_rows;
}

static sal_usecs_t
soc_dnxc_otp_read_timeout_get(
    int unit)
{
    sal_usecs_t timeout = 0;
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        timeout = dnx_data_device.otp.read_timeout_get(unit);
    }
#endif
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        timeout = dnxf_data_device.otp.read_timeout_get(unit);
    }
#endif

    return timeout;
}

static void
soc_dnxc_otp_reg_addresses_get(
    int unit,
    dnxc_data_device_otp_reg_addresses_t * reg_addr)
{
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        reg_addr->mode = dnx_data_device.otp.reg_addresses_get(unit)->mode;
        reg_addr->cpu_addr = dnx_data_device.otp.reg_addresses_get(unit)->cpu_addr;
        reg_addr->cmd = dnx_data_device.otp.reg_addresses_get(unit)->cmd;
        reg_addr->cmd_start = dnx_data_device.otp.reg_addresses_get(unit)->cmd_start;
        reg_addr->cpu_status = dnx_data_device.otp.reg_addresses_get(unit)->cpu_status;
        reg_addr->cpu_data_0 = dnx_data_device.otp.reg_addresses_get(unit)->cpu_data_0;
        reg_addr->cpu_data_1 = dnx_data_device.otp.reg_addresses_get(unit)->cpu_data_1;
        reg_addr->cpu_data_2 = dnx_data_device.otp.reg_addresses_get(unit)->cpu_data_2;
    }
#endif
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        reg_addr->mode = dnxf_data_device.otp.reg_addresses_get(unit)->mode;
        reg_addr->cpu_addr = dnxf_data_device.otp.reg_addresses_get(unit)->cpu_addr;
        reg_addr->cmd = dnxf_data_device.otp.reg_addresses_get(unit)->cmd;
        reg_addr->cmd_start = dnxf_data_device.otp.reg_addresses_get(unit)->cmd_start;
        reg_addr->cpu_status = dnxf_data_device.otp.reg_addresses_get(unit)->cpu_status;
        reg_addr->cpu_data_0 = dnxf_data_device.otp.reg_addresses_get(unit)->cpu_data_0;
        reg_addr->cpu_data_1 = dnxf_data_device.otp.reg_addresses_get(unit)->cpu_data_1;
        reg_addr->cpu_data_2 = dnxf_data_device.otp.reg_addresses_get(unit)->cpu_data_2;
    }
#endif
}

static void
soc_dnxc_otp_instance_attributes_get(
    int unit,
    int otp_instance,
    dnxc_data_device_otp_otp_instance_attributes_t * instance_attr)
{
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        instance_attr->block_id = dnx_data_device.otp.otp_instance_attributes_get(unit, otp_instance)->block_id;
        instance_attr->is_supported = dnx_data_device.otp.otp_instance_attributes_get(unit, otp_instance)->is_supported;
        instance_attr->size = dnx_data_device.otp.otp_instance_attributes_get(unit, otp_instance)->size;
    }
#endif
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        instance_attr->block_id = dnxf_data_device.otp.otp_instance_attributes_get(unit, otp_instance)->block_id;
        instance_attr->is_supported =
            dnxf_data_device.otp.otp_instance_attributes_get(unit, otp_instance)->is_supported;
        instance_attr->size = dnxf_data_device.otp.otp_instance_attributes_get(unit, otp_instance)->size;
    }
#endif
}

static shr_error_e
soc_dnxc_otp_row_read(
    int unit,
    uint32 otp_instance,
    uint32 row_addr,
    uint32 *data)
{
    dnxc_data_device_otp_reg_addresses_t reg_addr = { 0 };
    dnxc_data_device_otp_otp_instance_attributes_t otp_instance_attr = { 0 };
    int words_to_read = soc_dnxc_otp_nof_words_in_row_get(unit);
    const sal_usecs_t timeout_value = soc_dnxc_otp_read_timeout_get(unit);
    const int OTP_READ_CMD = 0x0;
    soc_timeout_t timeout;
    uint32 status;
    SHR_FUNC_INIT_VARS(unit);

    soc_dnxc_otp_reg_addresses_get(unit, &reg_addr);
    soc_dnxc_otp_instance_attributes_get(unit, otp_instance, &otp_instance_attr);

    SHR_IF_ERR_EXIT(soc_dnxc_otp_reg_write(unit, reg_addr.mode, otp_instance_attr.block_id, 0x1));

    SHR_IF_ERR_EXIT(soc_dnxc_otp_reg_write(unit, reg_addr.cpu_addr, otp_instance_attr.block_id, row_addr));

    SHR_IF_ERR_EXIT(soc_dnxc_otp_reg_write(unit, reg_addr.cmd, otp_instance_attr.block_id, OTP_READ_CMD));

    SHR_IF_ERR_EXIT(soc_dnxc_otp_reg_write(unit, reg_addr.cmd_start, otp_instance_attr.block_id, 0x1));

    soc_timeout_init(&timeout, timeout_value, 0);
    do
    {
        SHR_IF_ERR_EXIT(soc_dnxc_otp_reg_read(unit, reg_addr.cpu_status, otp_instance_attr.block_id, &status));

        if (((status) & 0x2) != 0)
        {
            break;
        }
        if (soc_timeout_check(&timeout))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "ERROR: failed to read from OTP. READ operation not done. CPU status = 0x%x.\n", (status));
        }
    }
    while (TRUE);

    SHR_IF_ERR_EXIT(soc_dnxc_otp_reg_read(unit, reg_addr.cpu_data_0, otp_instance_attr.block_id, data));
    if (words_to_read > 1)
    {
        SHR_IF_ERR_EXIT(soc_dnxc_otp_reg_read(unit, reg_addr.cpu_data_1, otp_instance_attr.block_id, data + 1));
    }

    SHR_IF_ERR_EXIT(soc_dnxc_otp_reg_write(unit, reg_addr.cmd_start, otp_instance_attr.block_id, 0x0));

    SHR_IF_ERR_EXIT(soc_dnxc_otp_reg_write(unit, reg_addr.mode, otp_instance_attr.block_id, 0));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
soc_dnxc_otp_read_verify(
    int unit,
    uint32 start_bit_addr,
    int otp_instance)
{
    dnxc_data_device_otp_otp_instance_attributes_t otp_instance_attr = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    if (otp_instance >= SOC_DNXC_OTP_INSTANCE_COUNT)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                     "ERROR: %d is an illegal OTP instance ID. Only values from soc_dnxc_otp_instances_e are allowed.\n",
                     otp_instance);
    }

    soc_dnxc_otp_instance_attributes_get(unit, otp_instance, &otp_instance_attr);
    if (!otp_instance_attr.is_supported)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "ERROR: OTP instance %d does not exist in current device.\n", otp_instance);
    }

    if ((otp_instance_attr.size != -1) && (start_bit_addr >= otp_instance_attr.size))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "ERROR: requested address (0x%x) exceeds OTP size (0x%x).\n",
                     start_bit_addr, otp_instance_attr.size);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_otp_read(
    int unit,
    soc_dnxc_otp_instances_e otp_instance,
    uint32 start_bit_addr,
    uint32 nof_bits,
    uint32 flags,
    SHR_BITDCL * output_buffer)
{
    const int row_size_words = soc_dnxc_otp_nof_words_in_row_get(unit);
    const int row_size_bits = row_size_words * SHR_BITWID;
    const int start_row = UTILEX_DIV_ROUND_DOWN(start_bit_addr, row_size_bits);
    const int end_row = UTILEX_DIV_ROUND_UP(start_bit_addr + nof_bits, row_size_bits);
    const int buffer_size = SHR_BITALLOCSIZE(nof_bits) + 1;
    SHR_BITDCL *temp_buffer = NULL;
    SHR_BITDCL *temp_buffer_runner = NULL;
    int row_idx;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(soc_dnxc_otp_read_verify(unit, start_bit_addr, otp_instance));
    SHR_ALLOC_ERR_EXIT(temp_buffer, buffer_size, "memory to store data from OTP", "%s%s%s", "data from OTP", EMPTY,
                       EMPTY);
    temp_buffer_runner = temp_buffer;
    sal_memset(temp_buffer, 0, buffer_size);

    for (row_idx = start_row; row_idx < end_row; row_idx += 1, temp_buffer_runner += row_size_words)
    {
        SHR_IF_ERR_EXIT(soc_dnxc_otp_row_read(unit, otp_instance, row_idx, temp_buffer_runner));
    }

    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                    (temp_buffer, start_bit_addr % row_size_bits, nof_bits, output_buffer));

exit:
    SHR_FREE(temp_buffer);
    SHR_FUNC_EXIT;
}
