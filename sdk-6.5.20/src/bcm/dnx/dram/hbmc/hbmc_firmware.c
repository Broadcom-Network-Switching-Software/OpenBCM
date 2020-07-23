

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_DRAM



#include <sal/appl/io.h>
#include <shared/util.h>
#include <shared/dbx/dbx_file.h>
#include <soc/util.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/dram/hbmc/hbmc_firmware.h>
#include "hbmc_dbal_access.h"






#define DNX_HBMC_FIRMWARE_FILE_MAX_SIZE 384


#define DNX_HBMC_FIRMWARE_STATUS_OPERATION_DONE              (dnx_data_dram.firmware.operation_status_get(unit)->done)
#define DNX_HBMC_FIRMWARE_STATUS_OPERATION_ACTIVE            (dnx_data_dram.firmware.operation_status_get(unit)->active)
#define DNX_HBMC_FIRMWARE_STATUS_OPERATION_ERRORS_DETECTED   (dnx_data_dram.firmware.operation_status_get(unit)->errors_detected)

#define DNX_HBMC_FIRMWARE_INTERRUPT_SUCCESS (1)

#define DNX_HBMC_FIRMWARE_INTERRUPT_DATA_COMPOSE(operation, channel)   (operation | ((channel == DNX_HBMC_FIRMWARE_ALL_CHANNELS) ? 0 : (channel << 8)))






typedef enum
{
    
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_OPERATION_STATUS = 0x00,
    
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_GLOBAL_ERROR_CODE = 0x01,
    
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_CHANNEL_0_ERROR_CODE = 0x02,
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_CHANNEL_1_ERROR_CODE = 0x03,
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_CHANNEL_2_ERROR_CODE = 0x04,
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_CHANNEL_3_ERROR_CODE = 0x05,
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_CHANNEL_4_ERROR_CODE = 0x06,
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_CHANNEL_5_ERROR_CODE = 0x07,
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_CHANNEL_6_ERROR_CODE = 0x08,
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_CHANNEL_7_ERROR_CODE = 0x09,
    
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_CHANNEL_0_LAST_OPERATION = 0x0a,
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_CHANNEL_1_LAST_OPERATION = 0x0b,
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_CHANNEL_2_LAST_OPERATION = 0x0c,
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_CHANNEL_3_LAST_OPERATION = 0x0d,
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_CHANNEL_4_LAST_OPERATION = 0x0e,
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_CHANNEL_5_LAST_OPERATION = 0x0f,
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_CHANNEL_6_LAST_OPERATION = 0x10,
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_CHANNEL_7_LAST_OPERATION = 0x11,
    
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_HBM_SPARE_0 = 0x12,
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_HBM_SPARE_1 = 0x13,
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_HBM_SPARE_2 = 0x14,
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_HBM_SPARE_3 = 0x15,
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_HBM_SPARE_4 = 0x16,
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_HBM_SPARE_5 = 0x17,
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_HBM_SPARE_6 = 0x18,
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_HBM_SPARE_7 = 0x19,
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_HBM_SPARE_8 = 0x1a,
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_HBM_SPARE_9 = 0x1b,
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_HBM_SPARE_10 = 0x1c,
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_HBM_SPARE_11 = 0x1d,
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_HBM_SPARE_12 = 0x1e,
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_HBM_SPARE_13 = 0x1f,
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_HBM_SPARE_14 = 0x20,
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_HBM_SPARE_15 = 0x21,
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_HBM_SPARE_16 = 0x22,
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_HBM_SPARE_17 = 0x23,
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_HBM_SPARE_18 = 0x24,
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_HBM_SPARE_19 = 0x25,
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_HBM_SPARE_20 = 0x26,
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_HBM_SPARE_21 = 0x27,
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_HBM_SPARE_22 = 0x28,
    DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_HBM_SPARE_23 = 0x29
} dnx_hbmc_firmware_operation_result_type_e;


typedef struct dnx_hbmc_firmware_operation_result_error_code_s
{
    
    uint32 global_error_code;
    
    uint32 channel_error_code[DNX_DATA_MAX_DRAM_GENERAL_INFO_NOF_CHANNELS];
    
    uint32 channel_last_operation[DNX_DATA_MAX_DRAM_GENERAL_INFO_NOF_CHANNELS];
} dnx_hbmc_firmware_operation_result_error_code_t;




static shr_error_e dnx_hbmc_firmware_operation_result_error_code_get(
    int unit,
    int hbmc_index,
    int channel_begin,
    int channel_end,
    dnx_hbmc_firmware_operation_result_error_code_t * error_code);


static shr_error_e
dnx_hbmc_firmware_interrupt_access(
    int unit,
    int hbmc_index,
    uint32 interrupt_code,
    uint32 interrupt_data,
    uint32 *interrupt_result)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HBMC_PHY_FIRMWARE_INTERRUPT_ACCESS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbmc_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INTERRUPT_CODE, INST_SINGLE, interrupt_code);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INTERRUPT_DATA, INST_SINGLE, interrupt_data);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_HBMC_PHY_FIRMWARE_INTERRUPT_ACCESS, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbmc_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INTERRUPT_ASSERT, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_HBMC_PHY_FIRMWARE_INTERRUPT_ACCESS, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbmc_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INTERRUPT_ASSERT, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_HBMC_PHY_FIRMWARE_INTERRUPT_ACCESS, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbmc_index);
    SHR_IF_ERR_EXIT(dnxcmn_polling
                    (unit, DNXCMN_TIMEOUT, DNXCMN_MIN_POLLS, entry_handle_id, DBAL_FIELD_INTERRUPT_BUSY, 0));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_HBMC_PHY_FIRMWARE_INTERRUPT_ACCESS, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbmc_index);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INTERRUPT_RESULT, INST_SINGLE, interrupt_result);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_firmware_info_get(
    int unit,
    int hbmc_index,
    dnx_hbmc_firmware_info_t * firmware_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_hbmc_firmware_interrupt_access
                    (unit, hbmc_index, DNX_HBMC_FIRMWARE_INTERRUPT_CODE_REVISION, 0, &firmware_info->revision));
    SHR_IF_ERR_EXIT(dnx_hbmc_firmware_interrupt_access
                    (unit, hbmc_index, DNX_HBMC_FIRMWARE_INTERRUPT_CODE_BUILD_ID, 0, &firmware_info->build_id));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_firmware_launch_operation(
    int unit,
    int hbmc_index,
    int hbmc_channel,
    dnx_hbmc_firmware_operation_e operation)
{
    uint32 interrupt_result = 0;
    dnx_hbmc_firmware_interrupt_code_e interrupt_code;
    uint32 interrupt_data;
    uint32 operation_status;
    uint32 nof_channels = dnx_data_dram.general_info.nof_channels_get(unit);
    uint32 channel_begin;
    uint32 channel_end;
    soc_timeout_t to;
    SHR_FUNC_INIT_VARS(unit);

    
    if (hbmc_channel == DNX_HBMC_FIRMWARE_ALL_CHANNELS)
    {
        interrupt_code = DNX_HBMC_FIRMWARE_INTERRUPT_CODE_LAUNCH_OPERATION_ON_ALL_CHANNELS;
        channel_begin = 0;
        channel_end = nof_channels - 1;
    }
    else
    {
        interrupt_code = DNX_HBMC_FIRMWARE_INTERRUPT_CODE_LAUNCH_OPERATION_ON_A_SINGLE_CHANNEL;
        channel_begin = hbmc_channel;
        channel_end = hbmc_channel;
    }
    
    interrupt_data = DNX_HBMC_FIRMWARE_INTERRUPT_DATA_COMPOSE(operation, hbmc_channel);

    
    SHR_IF_ERR_EXIT(dnx_hbmc_firmware_interrupt_access
                    (unit, hbmc_index, interrupt_code, interrupt_data, &interrupt_result));
#ifndef PLISIM
    if (interrupt_result != DNX_HBMC_FIRMWARE_INTERRUPT_SUCCESS)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "HBMC%d firmware operation %d failed to start: invalid return code %u\n", hbmc_index,
                     operation, interrupt_result);
    }
#endif

    
    soc_timeout_init(&to, 5000000, 1000);
    do
    {
        SHR_IF_ERR_EXIT(dnx_hbmc_firmware_interrupt_access
                        (unit, hbmc_index, DNX_HBMC_FIRMWARE_INTERRUPT_CODE_GET_OPERATION_RESULT,
                         DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_OPERATION_STATUS, &operation_status));

        if (soc_timeout_check(&to))
        {
            SHR_ERR_EXIT(_SHR_E_TIMEOUT, "TIMEOUT when polling for operation %d to complete\n", operation);
        }
    }
    while (operation_status == DNX_HBMC_FIRMWARE_STATUS_OPERATION_ACTIVE);

    
    if (operation_status == DNX_HBMC_FIRMWARE_STATUS_OPERATION_ERRORS_DETECTED)
    {
        dnx_hbmc_firmware_operation_result_error_code_t error_code;
        uint32 i_channel;
        dnx_hbmc_firmware_operation_result_hbm_spare_t hbm_spare;
        uint32 i_spare;
        uint32 nof_hbm_spares = dnx_data_dram.firmware.nof_hbm_spare_data_results_get(unit);

        
        SHR_IF_ERR_EXIT(dnx_hbmc_firmware_operation_result_error_code_get
                        (unit, hbmc_index, channel_begin, channel_end, &error_code));
        if (error_code.global_error_code)
        {
            
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "HBMC%d firmware operation result: global_error_code=0x%x:%s\n"), hbmc_index,
                       error_code.global_error_code, dnx_data_dram.firmware.operation_result_error_code_get(unit,
                                                                                                            error_code.global_error_code)->description));
        }

        for (i_channel = channel_begin; i_channel <= channel_end; ++i_channel)
        {
            if (error_code.channel_error_code[i_channel])
            {
                
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U
                           (unit,
                            "HBMC%d firmware operation result: channel=%u, error_code=0x%x:%s, operation=0x%x:%s\n"),
                           hbmc_index, i_channel, error_code.channel_error_code[i_channel],
                           dnx_data_dram.firmware.operation_result_error_code_get(unit,
                                                                                  error_code.channel_error_code
                                                                                  [i_channel])->description,
                           error_code.channel_last_operation[i_channel],
                           dnx_data_dram.firmware.operation_result_last_operation_get(unit,
                                                                                      error_code.channel_last_operation
                                                                                      [i_channel])->description));
            }
        }

        
        SHR_IF_ERR_EXIT(dnx_hbmc_firmware_operation_result_spare_get(unit, hbmc_index, nof_hbm_spares, &hbm_spare));
        for (i_spare = 0; i_spare < nof_hbm_spares; ++i_spare)
        {
            if (hbm_spare.hbm_spare_data[i_spare])
            {
                
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "HBMC%d firmware operation result: hbm_spare_%u=0x%x\n"), hbmc_index,
                           i_spare, hbm_spare.hbm_spare_data[i_spare]));
            }
        }

        SHR_ERR_EXIT(_SHR_E_FAIL, "HBMC%d firmware operation 0x%x resulted with error\n", hbmc_index, operation);
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_hbmc_firmware_operation_result_error_code_get(
    int unit,
    int hbmc_index,
    int channel_begin,
    int channel_end,
    dnx_hbmc_firmware_operation_result_error_code_t * error_code)
{
    uint32 i_channel;
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_hbmc_firmware_interrupt_access
                    (unit, hbmc_index, DNX_HBMC_FIRMWARE_INTERRUPT_CODE_GET_OPERATION_RESULT,
                     DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_GLOBAL_ERROR_CODE, &error_code->global_error_code));

    
    for (i_channel = channel_begin; i_channel <= channel_end; ++i_channel)
    {
            
        SHR_IF_ERR_EXIT(dnx_hbmc_firmware_interrupt_access
                        (unit, hbmc_index, DNX_HBMC_FIRMWARE_INTERRUPT_CODE_GET_OPERATION_RESULT,
                         DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_CHANNEL_0_ERROR_CODE + i_channel,
                         &error_code->channel_error_code[i_channel]));

            
        SHR_IF_ERR_EXIT(dnx_hbmc_firmware_interrupt_access
                        (unit, hbmc_index, DNX_HBMC_FIRMWARE_INTERRUPT_CODE_GET_OPERATION_RESULT,
                         DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_CHANNEL_0_LAST_OPERATION + i_channel,
                         &error_code->channel_last_operation[i_channel]));
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_firmware_operation_result_spare_get(
    int unit,
    int hbmc_index,
    uint32 nof_hbm_spares,
    dnx_hbmc_firmware_operation_result_hbm_spare_t * hbm_spare)
{
    uint32 i_spare;
    SHR_FUNC_INIT_VARS(unit);

    SHR_MAX_VERIFY(nof_hbm_spares, dnx_data_dram.firmware.nof_hbm_spare_data_results_get(unit), _SHR_E_PARAM,
                   "Number of required HBM spare data results is too high (%d). Max is %d\n.", nof_hbm_spares,
                   dnx_data_dram.firmware.nof_hbm_spare_data_results_get(unit));

    
    for (i_spare = 0; i_spare < nof_hbm_spares; ++i_spare)
    {
        SHR_IF_ERR_EXIT(dnx_hbmc_firmware_interrupt_access
                        (unit, hbmc_index, DNX_HBMC_FIRMWARE_INTERRUPT_CODE_GET_OPERATION_RESULT,
                         DNX_HBMC_FIRMWARE_OPERATION_RESULT_TYPE_HBM_SPARE_0 + i_spare,
                         &hbm_spare->hbm_spare_data[i_spare]));
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_firmware_param_set(
    int unit,
    int hbmc_index,
    dnx_hbmc_firmware_param_e param,
    uint32 value)
{
    uint32 interrupt_result;
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_hbmc_firmware_interrupt_access
                    (unit, hbmc_index, DNX_HBMC_FIRMWARE_INTERRUPT_CODE_SET_PARAMETER, param, &interrupt_result));
#ifndef PLISIM
    if (interrupt_result != DNX_HBMC_FIRMWARE_INTERRUPT_SUCCESS)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "HBM%d firmware param offset %x set failed\n", hbmc_index, param);
    }
#endif

    
    SHR_IF_ERR_EXIT(dnx_hbmc_firmware_interrupt_access
                    (unit, hbmc_index, DNX_HBMC_FIRMWARE_INTERRUPT_CODE_SET_PARAMETER_VALUE, value, &interrupt_result));
#ifndef PLISIM
    if (interrupt_result != DNX_HBMC_FIRMWARE_INTERRUPT_SUCCESS)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "HBMC%d firmware param value %x set failed\n", hbmc_index, value);
    }
#endif

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_firmware_param_get(
    int unit,
    int hbmc_index,
    dnx_hbmc_firmware_param_e param,
    uint32 *value)
{
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_hbmc_firmware_interrupt_access
                    (unit, hbmc_index, DNX_HBMC_FIRMWARE_INTERRUPT_CODE_GET_PARAMETER_VALUE, param, value));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_hbmc_firmware_sbus_clock_divider_set(
    int unit,
    int hbmc_index,
    uint32 divider)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (!utilex_is_power_of_2(divider))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ERROR HBMC%d: sbus clock divider must be power of 2, but got %d.\n", hbmc_index,
                     divider);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HBMC_PHY_FIRMWARE_SBUS_CONTROLLER_ACCESS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbmc_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CLOCK_DIVIDER, INST_SINGLE,
                                 utilex_log2_round_down(divider));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static int
dnx_hbmc_read_firmware_line_from_rom(
    int unit,
    FILE * fp,
    uint32 *line)
{
    int ascii_hex_digit;

    while ((ascii_hex_digit = sal_fgetc(fp)) != EOF)
    {
        
        if (ascii_hex_digit == '\n')
        {
            break;
        }

        *line <<= 4;
        
        *line |= _shr_xdigit2i(ascii_hex_digit);
    }

    return (ascii_hex_digit == EOF) ? FALSE : TRUE;
}


static shr_error_e
dnx_hbmc_firmware_upload(
    int unit,
    int hbmc_index)
{
    uint32 entry_handle_id;
    FILE *fp = NULL;
    char *rel_file_path = dnx_data_dram.firmware.rom_get(unit)->filename;
    char file_path[DNX_HBMC_FIRMWARE_FILE_MAX_SIZE];
    int valid = 1;
    uint32 line_number = 0;
    dnx_hbmc_firmware_info_t firmware_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "%s(hbmc%d): Uploading Sbus Master ROM via SBus\n"), FUNCTION_NAME(), hbmc_index));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HBMC_PHY_FIRMWARE_SNAP_ACCESS, &entry_handle_id));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_HBMC_PHY_FIRMWARE_SNAP_ACCESS, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbmc_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SINGLE_STEP_EN, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_HBMC_PHY_FIRMWARE_SNAP_ACCESS, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbmc_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SBUS_CNTL_GATE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SNAP_RESET, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SNAP_ENABLE, INST_SINGLE, FALSE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_HBMC_PHY_FIRMWARE_SNAP_ACCESS, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbmc_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SNAP_RESET, INST_SINGLE, FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IMEM_CNTL_EN, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_HBMC_PHY_FIRMWARE_SNAP_ACCESS, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbmc_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SINGLE_STEP_EN, INST_SINGLE, FALSE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_HBMC_PHY_FIRMWARE_SNAP_ACCESS, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbmc_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IMEM_WRITE_EN, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(dbx_file_get_file_path(unit, rel_file_path, CONF_OPEN_PER_DEVICE, file_path));
    if ((fp = sal_fopen(file_path, "r")) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ERROR: HBMC%d firmware file %s was not found.\n", hbmc_index, file_path);
    }

    while (valid)
    {
        int line_index = 0;
        uint32 buf_data1 = 0;
        uint32 buf_data2 = 0;
        uint32 buf_data3 = 0;

        
        for (line_index = 0; line_index <= 7; ++line_index)
        {
            uint32 rom_line = 0;

            
            valid = dnx_hbmc_read_firmware_line_from_rom(unit, fp, &rom_line);

            if (valid)
            {
                switch (line_index)
                {
                    case 0:
                    {
                        buf_data1 = rom_line;
                        break;
                    }
                    case 1:
                    {
                        buf_data1 |= (rom_line << 12);
                        break;
                    }
                    case 2:
                    {
                        buf_data1 |= ((rom_line & 0xff) << 24);
                        buf_data2 = (rom_line >> 8);
                        break;
                    }
                    case 3:
                    {
                        buf_data2 |= (rom_line << 4);
                        break;
                    }
                    case 4:
                    {
                        buf_data2 |= (rom_line << 16);
                        break;
                    }
                    case 5:
                    {
                        buf_data2 |= ((rom_line & 0xf) << 28);
                        buf_data3 = (rom_line >> 4);
                        break;
                    }
                    case 6:
                    {
                        buf_data3 |= (rom_line << 8);
                        break;
                    }
                    case 7:
                    {
                        buf_data3 |= (rom_line << 20);
                        break;
                    }
                }
            }
            else
            {
                break;
            }
        }

        
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U
                     (unit, "%s(hbmc%d): Spico upload line_number:0x%04x data1:0x%08x data2:0x%08x data3:0x%08x\n"),
                     FUNCTION_NAME(), hbmc_index, line_number, buf_data1, buf_data2, buf_data3));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_HBMC_PHY_FIRMWARE_SNAP_ACCESS, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbmc_index);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IMEM_BURST_DATA, INST_SINGLE, buf_data1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_HBMC_PHY_FIRMWARE_SNAP_ACCESS, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbmc_index);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IMEM_BURST_DATA, INST_SINGLE, buf_data2);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_HBMC_PHY_FIRMWARE_SNAP_ACCESS, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbmc_index);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IMEM_BURST_DATA, INST_SINGLE, buf_data3);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        line_number += 8;
    }

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_HBMC_PHY_FIRMWARE_SNAP_ACCESS, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbmc_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IMEM_CNTL_EN, INST_SINGLE, FALSE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_HBMC_PHY_FIRMWARE_SNAP_ACCESS, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbmc_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IMEM_ECC_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DMEM_ECC_ENABLE, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_HBMC_PHY_FIRMWARE_SNAP_ACCESS, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbmc_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SNAP_ENABLE, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "%s(hbmc%d): Waiting for Spico to initialize\n"), FUNCTION_NAME(), hbmc_index));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_HBMC_PHY_FIRMWARE_SNAP_ACCESS, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbmc_index);
    SHR_IF_ERR_EXIT(dnxcmn_polling
                    (unit, 1000000, 500, entry_handle_id, DBAL_FIELD_SNAP_STATE,
                     dnx_data_dram.firmware.snap_state_init_done_get(unit)));
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(hbmc%d): Spico initialized\n"), FUNCTION_NAME(), hbmc_index));

    
    SHR_IF_ERR_EXIT(dnx_hbmc_firmware_info_get(unit, hbmc_index, &firmware_info));
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "%s(hbmc%d): firmware revision 0x%x build id 0x%x\n"), FUNCTION_NAME(), hbmc_index,
                 firmware_info.revision, firmware_info.build_id));

exit:
    if (fp)
    {
        sal_fclose(fp);
    }
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_firmware_phy_init(
    int unit,
    int hbmc_index)
{
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_hbmc_firmware_sbus_clock_divider_set
                    (unit, hbmc_index, dnx_data_dram.firmware.sbus_clock_divider_get(unit)));

    
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "%s(hbmc%d): take phy control out of reset\n"), FUNCTION_NAME(), hbmc_index));
    SHR_IF_ERR_EXIT(dnx_hbmc_dbal_access_phy_control_out_of_reset_config(unit, hbmc_index));

    
    SHR_IF_ERR_EXIT(dnx_hbmc_firmware_upload(unit, hbmc_index));

    
    SHR_IF_ERR_EXIT(dnx_hbmc_firmware_param_set(unit, hbmc_index, DNX_HBMC_FIRMWARE_PARAM_HBM_DIV_MODE, 0));

    
    SHR_IF_ERR_EXIT(dnx_hbmc_firmware_param_set
                    (unit, hbmc_index, DNX_HBMC_FIRMWARE_PARAM_HBM_FREQ,
                     (dnx_data_dram.general_info.frequency_get(unit) * 2)));

    
    SHR_IF_ERR_EXIT(dnx_hbmc_firmware_param_set
                    (unit, hbmc_index, DNX_HBMC_FIRMWARE_PARAM_HBM_BYPASS_REPAIR_ON_RESET, 1));

    

    
    SHR_IF_ERR_EXIT(dnx_hbmc_firmware_param_set
                    (unit, hbmc_index, DNX_HBMC_FIRMWARE_PARAM_HBM_PARITY_LATENCY,
                     dnx_data_dram.general_info.actual_parity_latency_get(unit)));

    
    SHR_IF_ERR_EXIT(dnx_hbmc_firmware_param_set
                    (unit, hbmc_index, DNX_HBMC_FIRMWARE_PARAM_HBM_MODE_REGISTER0,
                     dnx_data_dram.general_info.mr_defaults_get(unit, 0)->value));
    SHR_IF_ERR_EXIT(dnx_hbmc_firmware_param_set
                    (unit, hbmc_index, DNX_HBMC_FIRMWARE_PARAM_HBM_MODE_REGISTER1,
                     dnx_data_dram.general_info.mr_defaults_get(unit, 1)->value));
    SHR_IF_ERR_EXIT(dnx_hbmc_firmware_param_set
                    (unit, hbmc_index, DNX_HBMC_FIRMWARE_PARAM_HBM_MODE_REGISTER2,
                     dnx_data_dram.general_info.mr_defaults_get(unit, 2)->value));
    SHR_IF_ERR_EXIT(dnx_hbmc_firmware_param_set
                    (unit, hbmc_index, DNX_HBMC_FIRMWARE_PARAM_HBM_MODE_REGISTER3,
                     dnx_data_dram.general_info.mr_defaults_get(unit, 3)->value));
    SHR_IF_ERR_EXIT(dnx_hbmc_firmware_param_set
                    (unit, hbmc_index, DNX_HBMC_FIRMWARE_PARAM_HBM_MODE_REGISTER4,
                     dnx_data_dram.general_info.mr_defaults_get(unit, 4)->value));
    SHR_IF_ERR_EXIT(dnx_hbmc_firmware_param_set
                    (unit, hbmc_index, DNX_HBMC_FIRMWARE_PARAM_HBM_MODE_REGISTER5,
                     dnx_data_dram.general_info.mr_defaults_get(unit, 5)->value));
    SHR_IF_ERR_EXIT(dnx_hbmc_firmware_param_set
                    (unit, hbmc_index, DNX_HBMC_FIRMWARE_PARAM_HBM_MODE_REGISTER6,
                     dnx_data_dram.general_info.mr_defaults_get(unit, 6)->value));
    SHR_IF_ERR_EXIT(dnx_hbmc_firmware_param_set
                    (unit, hbmc_index, DNX_HBMC_FIRMWARE_PARAM_HBM_MODE_REGISTER7,
                     dnx_data_dram.general_info.mr_defaults_get(unit, 7)->value));
    SHR_IF_ERR_EXIT(dnx_hbmc_firmware_param_set
                    (unit, hbmc_index, DNX_HBMC_FIRMWARE_PARAM_HBM_MODE_REGISTER8,
                     dnx_data_dram.general_info.mr_defaults_get(unit, 8)->value));
    SHR_IF_ERR_EXIT(dnx_hbmc_firmware_param_set
                    (unit, hbmc_index, DNX_HBMC_FIRMWARE_PARAM_HBM_MODE_REGISTER15,
                     dnx_data_dram.general_info.mr_defaults_get(unit, 15)->value));

    
    SHR_IF_ERR_EXIT(dnx_hbmc_firmware_launch_operation
                    (unit, hbmc_index, DNX_HBMC_FIRMWARE_ALL_CHANNELS, DNX_HBMC_FIRMWARE_OPERATION_POWER_ON_FLOW));

exit:
    SHR_FUNC_EXIT;
}
