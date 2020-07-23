
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    diag_sand_dsig.c
 * Purpose:    Routines for handling debug and internal signals
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_KBP_DIAG

#if defined(INCLUDE_KBP)

#include <sal/appl/sal.h>
#include <sal/compiler.h>

#include <soc/dnx/kbp/kbp_common.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/dbal_string_apis.h>
#include <soc/sand/sand_signals.h>
/** allow drv.h include excplictly */
#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
#include <soc/drv.h>
#include <bcm/switch.h>

#include <bcm/types.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <bcm_int/dnx/kbp/kbp_connectivity.h>
#include <bcm_int/dnx/kbp/kbp_rop.h>
#include <bcm_int/dnx/kbp/kbp_recover.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/field/field_group.h>
#include <shared/swstate/sw_state_workarounds.h>

#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_signals.h>
#include <appl/diag/dnx/diag_dnx_utils.h>

#include "diag_dnx_kbp.h"

/*
 * Globals
 * {
 */
extern sal_field_type_e dbal_field_type_to_sal_field_type[];

extern shr_error_e dbal_fields_print_type_get(
    int unit,
    dbal_fields_e field_id,
    dbal_field_print_type_e * field_print_type);

extern shr_error_e kbp_mngr_opcode_printable_entry_result_parsing(
    int unit,
    uint32 flags,
    uint8 opcode_id,
    uint32 *res_sig_value,
    int res_sig_size,
    uint8 *kbp_entry_print_num,
    kbp_printable_entry_t * entry_print_info);

extern generic_kbp_app_data_t *Kbp_app_data[SOC_MAX_NUM_DEVICES];

/*
 * }
 */

/*
 * Defines
 * {
 */

/*
 * This macro is used to print a diagnostic header in the following format
 * "HEADER TEXT"
 * ==============
 */
#define DIAG_DNX_KBP_HEADER_DUMP(header, name)                         \
{                                                                     \
int iter = 0, iter_num = sal_strlen(header)+sal_strlen(name);       \
LOG_CLI((BSL_META("\n%s %s\n"), header, name));                     \
for(iter=0;iter<iter_num+1;iter++)                                  \
{                                                                   \
  LOG_CLI((BSL_META("=")));                                         \
}                                                                   \
LOG_CLI((BSL_META("\n\n")));                                        \
}

/*
 * helper macros for performing the setting the segment value according to the field type & parameters
 */
#define KBP_MASTER_KEY_SEGMENT_SET(__unit, __byte_offset, __segment_bytes, __field_type, __sand_arg, __master_key)    \
do                                                                                                                    \
{                                                                                                                     \
    if (__field_type == SAL_FIELD_TYPE_MAC)                                                                           \
    {                                                                                                                 \
        sal_memcpy(&__master_key[__byte_offset], &SH_SAND_ARG_MAC_DATA(__sand_arg), __segment_bytes);                 \
    }                                                                                                                 \
    else if (__field_type == SAL_FIELD_TYPE_IP6)                                                                      \
    {                                                                                                                 \
        sal_memcpy(&__master_key[__byte_offset], &SH_SAND_ARG_IP6_DATA(__sand_arg), __segment_bytes);                 \
    }                                                                                                                 \
    else                                                                                                              \
    {                                                                                                                 \
        SHR_IF_ERR_EXIT(dnx_kbp_buffer_dbal_to_kbp(__unit, (__segment_bytes << 3),                                    \
                        SH_SAND_ARG_ARRAY_DATA(__sand_arg), &__master_key[__byte_offset]));                           \
    }                                                                                                                 \
}while(0)

#define DNX_KBP_DIAG_CMD_VERIFY(__unit)                                                         \
do                                                                                              \
{                                                                                               \
    dbal_enum_value_field_kbp_device_status_e kbp_mngr_status;                                  \
    if (SAL_BOOT_PLISIM)                                                                        \
    {                                                                                           \
        LOG_CLI((BSL_META("NOT Supported, This CMD is valid only while using KBP HW\n")));      \
        SHR_EXIT();                                                                             \
    }                                                                                           \
    if (!dnx_kbp_device_enabled(__unit))                                                        \
    {                                                                                           \
        LOG_CLI((BSL_META("KBP was not enabled for the device.\n")));                           \
        SHR_EXIT();                                                                             \
    }                                                                                           \
    SHR_IF_ERR_EXIT(kbp_mngr_status_get(__unit, &kbp_mngr_status));                             \
    if (kbp_mngr_status != DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED)                             \
    {                                                                                           \
        LOG_CLI((BSL_META("KBP device is not locked\n")));                                      \
        SHR_EXIT();                                                                             \
    }                                                                                           \
}while(0)

/** \brief Long string max length */
#define DNX_KBP_DIAG_MAX_STRING_LENGTH          1024

#define DNX_KBP_DIAG_MAX_NOF_DB                 (64)

#define DNX_KBP_DIAG_PRINTABLE_KEY_SEGMENT_SIZE 96

sh_sand_enum_t dnx_kbp_diag_opcode_dump_enum_table[] = {
    {"ALL", DNX_KBP_MAX_NOF_OPCODES, "All opcodes"},
    {NULL}
};

/*
 * }
 */

/**
 * \brief - callback to check if KBP interface is enabled.
 */
static shr_error_e
dnx_kbp_diag_interface_is_enabled(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    if (SAL_BOOT_PLISIM)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    }
    else
    {   /** one of supported KBP devices must be defined */
        char *kbp_device_type = soc_property_get_str(unit, "ext_tcam_dev_type");
        if ((kbp_device_type == NULL) ||
            (sal_strcasecmp(kbp_device_type, "BCM52311") && sal_strcasecmp(kbp_device_type, "BCM52321")))
        {
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        }
        else
        {   /** if kbp_blackhole defined disable command */
            char *kbp_blackhole = soc_property_get_str(unit, "custom_feature_kbp_blackhole");
            if ((kbp_blackhole != NULL) && sal_strcasecmp(kbp_blackhole, "0"))
            {
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            }
        }
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief - callback to check if not OP2 KBP interface is enabled.
 */
static shr_error_e
dnx_kbp_diag_interface_is_not_op2_enabled(
    int unit,
    rhlist_t * test_list)
{
    char *kbp_device_type = NULL;
    SHR_FUNC_INIT_VARS(unit);

    kbp_device_type = soc_property_get_str(unit, "ext_tcam_dev_type");

    if (SHR_FAILURE(dnx_kbp_diag_interface_is_enabled(unit, test_list)) ||
        (kbp_device_type == NULL) || !sal_strcasecmp(kbp_device_type, "BCM52321"))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    }

    SHR_FUNC_EXIT;
}

#define KBP_COMPATIBILITY_MESSAGE "Command is not available when KBP is not connected or KBP type is incorrect"

static shr_error_e
dnx_kbp_diag_device_print_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *file_path = NULL;
    FILE *kbp_file;

    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_kbp_device_enabled(unit))
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "KBP was not enabled for the device.\n");
    }

    SH_SAND_GET_STR("PATH", file_path);

    if (sal_strncmp(file_path, "stdout", sal_strlen(file_path)))
    {
        kbp_file = fopen(file_path, "w");
    }
    else
    {
        kbp_file = stdout;
    }

    DNX_KBP_TRY(kbp_device_print_html(Kbp_app_data[unit]->device_p[DNX_KBP_INST_MASTER], kbp_file));

    if (sal_strncmp(file_path, "stdout", sal_strlen(file_path)))
    {
        fclose(kbp_file);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_diag_device_dump_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int info = 0;
    char *file_path = NULL;
    FILE *kbp_file;

    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_kbp_device_enabled(unit))
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "KBP was not enabled for the device.\n");
    }

    SH_SAND_GET_STR("PATH", file_path);
    SH_SAND_GET_INT32("INFo", info);

    if (sal_strncmp(file_path, "stdout", sal_strlen(file_path)))
    {
        kbp_file = fopen(file_path, "w");
    }
    else
    {
        kbp_file = stdout;
    }

    DNX_KBP_TRY(kbp_device_dump(Kbp_app_data[unit]->device_p[DNX_KBP_INST_MASTER], info, kbp_file));

    if (sal_strncmp(file_path, "stdout", sal_strlen(file_path)))
    {
        fclose(kbp_file);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_diag_device_swstate_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *file_path = NULL;
    FILE *kbp_file;

    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_kbp_device_enabled(unit))
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "KBP was not enabled for the device.\n");
    }

    SH_SAND_GET_STR("PATH", file_path);

    if (sal_strncmp(file_path, "stdout", sal_strlen(file_path)))
    {
        kbp_file = fopen(file_path, "w");
    }
    else
    {
        kbp_file = stdout;
    }

    DNX_KBP_TRY(kbp_device_print_sw_state(Kbp_app_data[unit]->device_p[DNX_KBP_INST_MASTER], kbp_file));
    sal_fprintf(kbp_file, "\n");

    if (sal_strncmp(file_path, "stdout", sal_strlen(file_path)))
    {
        fclose(kbp_file);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_diag_device_last_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int info = 0;
    char *file_path = NULL;
    FILE *kbp_file;

    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_kbp_device_enabled(unit))
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "KBP was not enabled for the device.\n");
    }

    SH_SAND_GET_STR("PATH", file_path);
    SH_SAND_GET_INT32("INFo", info);

    if (sal_strncmp(file_path, "stdout", sal_strlen(file_path)))
    {
        kbp_file = fopen(file_path, "w");
    }
    else
    {
        kbp_file = stdout;
    }

    DNX_KBP_TRY(kbp_device_read_last_result(Kbp_app_data[unit]->device_p[DNX_KBP_INST_MASTER], info, kbp_file));

    if (sal_strncmp(file_path, "stdout", sal_strlen(file_path)))
    {
        fclose(kbp_file);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_diag_sync_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_kbp_device_enabled(unit))
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "KBP was not enabled for the device.\n");
    }

    bsl_printf("Performing KBP device sync\n");
    if (bcm_switch_control_set(unit, bcmSwitchExternalTcamSync, 0) != _SHR_E_NONE)
    {
        bsl_printf("KBP device sync failed\n");
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_diag_sdk_ver_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    const char *ver_str = NULL;

    SHR_FUNC_INIT_VARS(unit);

    ver_str = kbp_device_get_sdk_version();

    if (ver_str != NULL)
    {
        bsl_printf("%s\n", ver_str);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error getting KBPSDK version\n");
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_diag_printable_master_key_build(
    int unit,
    uint32 entry_handle_id,
    char master_key[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY * DNX_KBP_DIAG_PRINTABLE_KEY_SEGMENT_SIZE],
    uint8 build_overlay_fields)
{
    int index;
    uint8 opcode_id;
    uint32 nof_segments;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(master_key, 0x0,
               (sizeof(char) * DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY * DNX_KBP_DIAG_PRINTABLE_KEY_SEGMENT_SIZE));

    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_get(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, &opcode_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                    (unit, entry_handle_id, DBAL_FIELD_NOF_VALID_SEGMENTS, INST_SINGLE, &nof_segments));

    for (index = 0; index < nof_segments; index++)
    {
        uint32 segment_type, segment_bytes, overlay_offset_bytes;
        char segment_name[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES] = { 0 };
        char master_key_segment[DNX_KBP_DIAG_PRINTABLE_KEY_SEGMENT_SIZE] = { 0 };
        char segment_type_string[DBAL_MAX_STRING_LENGTH] = { 0 };
        int meaningful_segment_type_string_offset = sal_strlen("KBP_KEY_FIELD_TYPE_");

        SHR_IF_ERR_EXIT(kbp_mngr_master_key_segment_get
                        (unit, opcode_id, index, &segment_type, &segment_bytes, segment_name, &overlay_offset_bytes));

        SHR_IF_ERR_EXIT(dnx_dbal_fields_string_form_hw_value_get
                        (unit, "SEGMENT_TYPE", segment_type, segment_type_string));

        if (!build_overlay_fields && !DNX_KBP_KEY_IS_OVERLAY_FIELD(overlay_offset_bytes))
        {
            sal_snprintf(master_key_segment, (DNX_KBP_DIAG_PRINTABLE_KEY_SEGMENT_SIZE - 1), "<%s %d %s> ", segment_name,
                         segment_bytes, (segment_type_string + meaningful_segment_type_string_offset));

            sal_strncat(master_key, master_key_segment,
                        (DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY * DNX_KBP_DIAG_PRINTABLE_KEY_SEGMENT_SIZE - 1));
        }
        else if (build_overlay_fields && DNX_KBP_KEY_IS_OVERLAY_FIELD(overlay_offset_bytes))
        {
            sal_snprintf(master_key_segment, (DNX_KBP_DIAG_PRINTABLE_KEY_SEGMENT_SIZE - 1), "<%s %d %s %d> ",
                         segment_name, segment_bytes, (segment_type_string + meaningful_segment_type_string_offset),
                         overlay_offset_bytes);

            sal_strncat(master_key, master_key_segment,
                        (DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY * DNX_KBP_DIAG_PRINTABLE_KEY_SEGMENT_SIZE - 1));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_diag_printable_db_key_build(
    int unit,
    uint32 entry_handle_id,
    char db_key[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP * DNX_KBP_DIAG_PRINTABLE_KEY_SEGMENT_SIZE])
{
    int index;
    uint8 db_id;
    uint32 nof_segments;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(db_key, 0x0,
               (sizeof(char) * DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP * DNX_KBP_DIAG_PRINTABLE_KEY_SEGMENT_SIZE));

    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_get(unit, entry_handle_id, DBAL_FIELD_DB_ID, &db_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                    (unit, entry_handle_id, DBAL_FIELD_NOF_VALID_SEGMENTS, INST_SINGLE, &nof_segments));

    for (index = 0; index < nof_segments; index++)
    {
        uint32 segment_type, segment_bytes;
        char segment_name[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES] = { 0 };
        char db_key_segment[DNX_KBP_DIAG_PRINTABLE_KEY_SEGMENT_SIZE] = { 0 };
        char segment_type_string[DBAL_MAX_STRING_LENGTH] = { 0 };
        int meaningful_segment_type_string_offset = sal_strlen("KBP_KEY_FIELD_TYPE_");

        SHR_IF_ERR_EXIT(kbp_mngr_db_key_segment_get(unit, db_id, index, &segment_type, &segment_bytes, segment_name));

        SHR_IF_ERR_EXIT(dnx_dbal_fields_string_form_hw_value_get
                        (unit, "SEGMENT_TYPE", segment_type, segment_type_string));

        sal_snprintf(db_key_segment, (DNX_KBP_DIAG_PRINTABLE_KEY_SEGMENT_SIZE - 1), "<%s %d %s> ", segment_name,
                     segment_bytes, (segment_type_string + meaningful_segment_type_string_offset));

        sal_strncat(db_key, db_key_segment,
                    (DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP * DNX_KBP_DIAG_PRINTABLE_KEY_SEGMENT_SIZE - 1));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_diag_opcode_dump_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int index;
    uint32 entry_handle_id;
    uint32 db_entry_handle_id;
    uint32 lkp_entry_handle_id;
    uint32 res_entry_handle_id;
    uint32 ctx_entry_handle_id;
    uint32 lookup_bmp = 0;
    uint32 result_bmp = 0;
    uint32 opcode_id;
    uint8 is_valid;
    uint8 print_one;
    int is_end;
    char opcode_name[DBAL_MAX_STRING_LENGTH] = { 0 };
    char master_key[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY * DNX_KBP_DIAG_PRINTABLE_KEY_SEGMENT_SIZE] = { 0 };
    uint32 total_result_bits = 0;
    dbal_enum_value_field_kbp_device_status_e kbp_mngr_status;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("OPcode", opcode_id);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_DB_INFO, &db_entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_LOOKUP_INFO, &lkp_entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_RESULT_INFO, &res_entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_MAPPING, &ctx_entry_handle_id));
    {
        if (opcode_id < DNX_KBP_MAX_NOF_OPCODES)
        {
            /** Print the given opcode */
            dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                            (unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, &is_valid));

            if (!is_valid)
            {
                bsl_printf("Opcode %d is not in use\n", opcode_id);
                SHR_EXIT();
            }

            print_one = TRUE;
            is_end = FALSE;
        }
        else if (opcode_id == DNX_KBP_MAX_NOF_OPCODES)
        {
            /** Initialize iterator to iterate over all opcodes */
            is_valid = TRUE;
            SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
            SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr8_rule_add
                            (unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, DBAL_CONDITION_EQUAL_TO, &is_valid,
                             &is_valid));
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

            print_one = FALSE;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Opcode %d is out of range\n", opcode_id);
        }
    }
    while (!is_end)
    {
        int context_is_end = TRUE;
        uint8 contexts_attached = FALSE;

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, &opcode_id));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                        (unit, entry_handle_id, DBAL_FIELD_OPCODE_NAME, INST_SINGLE, (uint8 *) opcode_name));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_LOOKUP_BITMAP, INST_SINGLE, &lookup_bmp));

        PRT_TITLE_SET("KBP Opcode %d (%s)", opcode_id, opcode_name);
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Key fields");
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));
        if (kbp_mngr_status != DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED)
        {
            PRT_INFO_ADD("WARNING: KBP device is not locked");
        }

        SHR_IF_ERR_EXIT(dnx_kbp_diag_printable_master_key_build(unit, entry_handle_id, master_key, FALSE));
        PRT_CELL_SET("Master key fields %s", master_key);

        SHR_IF_ERR_EXIT(dnx_kbp_diag_printable_master_key_build(unit, entry_handle_id, master_key, TRUE));
        if (sal_strncmp
            (master_key, "", DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY * DNX_KBP_DIAG_PRINTABLE_KEY_SEGMENT_SIZE))
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("Overlay fields %s", master_key);
        }

        PRT_COMMITX;

        PRT_TITLE_SET("Contexts");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FWD context");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "ACL context");

        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, ctx_entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, ctx_entry_handle_id, &context_is_end));

        while (!context_is_end)
        {
            uint32 mapped_opcode_id = -1;

            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, ctx_entry_handle_id, DBAL_FIELD_KBP_OPCODE_ID, INST_SINGLE, &mapped_opcode_id));

            if (opcode_id == mapped_opcode_id)
            {
                uint32 context_hw_id, fwd_context_enum;

                PRT_ROW_ADD(PRT_ROW_SEP_NONE);

                /** FWD context is retrieved as enum and needs to be converted to hw value */
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                (unit, ctx_entry_handle_id, DBAL_FIELD_FWD2_CONTEXT_ID, &fwd_context_enum));
                SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                                (unit, DBAL_FIELD_FWD2_CONTEXT_ID, fwd_context_enum, &context_hw_id));
                PRT_CELL_SET("%s (%d)",
                             dbal_enum_id_type_to_string(unit, DBAL_FIELD_TYPE_DEF_FWD2_CONTEXT_ID, fwd_context_enum),
                             context_hw_id);

                /** ACL context */
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, ctx_entry_handle_id, DBAL_FIELD_ACL_CONTEXT,
                                                                      &context_hw_id));
                PRT_CELL_SET("%d", context_hw_id);

                contexts_attached = TRUE;
            }

            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, ctx_entry_handle_id, &context_is_end));
        }

        if (!contexts_attached)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("None");
            PRT_CELL_SET("None");
        }

        PRT_COMMITX;

        PRT_TITLE_SET("Lookup info");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Search");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Result");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "DB ID");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "DB Name");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Type");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Key");

        for (index = 0; index < DNX_KBP_MAX_NOF_LOOKUPS; index++)
        {
            uint8 db_id;
            uint8 result_id;
            uint8 lookup_type;
            uint32 table_id;
            char db_key[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP * DNX_KBP_DIAG_PRINTABLE_KEY_SEGMENT_SIZE] = { 0 };
            char lookup_type_string[DBAL_MAX_STRING_LENGTH] = { 0 };
            int meaningful_lookup_type_string_offset = sal_strlen("KBP_LOOKUP_TYPE_");

            dbal_entry_key_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
            dbal_entry_key_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_ID, (uint8) index);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, lkp_entry_handle_id, DBAL_GET_ALL_FIELDS));

            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                            (unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_DB, INST_SINGLE, &db_id));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                            (unit, lkp_entry_handle_id, DBAL_FIELD_RESULT_ID, INST_SINGLE, &result_id));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                            (unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_TYPE, INST_SINGLE, &lookup_type));

            SHR_IF_ERR_EXIT(dnx_dbal_fields_string_form_hw_value_get
                            (unit, "LOOKUP_TYPE", lookup_type, lookup_type_string));

            PRT_ROW_ADD(PRT_ROW_SEP_NONE);

            if (utilex_bitstream_test_bit(&lookup_bmp, index))
            {
                dbal_entry_key_field8_set(unit, db_entry_handle_id, DBAL_FIELD_DB_ID, db_id);
                SHR_IF_ERR_EXIT(dbal_entry_get(unit, db_entry_handle_id, DBAL_GET_ALL_FIELDS));
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, db_entry_handle_id, DBAL_FIELD_TABLE_ID, INST_SINGLE, &table_id));

                SHR_IF_ERR_EXIT(dnx_kbp_diag_printable_db_key_build(unit, db_entry_handle_id, db_key));

                PRT_CELL_SET("%d", index); /** Search */
                PRT_CELL_SET("%d", result_id); /** Result */
                PRT_CELL_SET("%d", db_id); /** DB ID */
                PRT_CELL_SET("%s", dbal_logical_table_to_string(unit, table_id)); /** DB Name */
                PRT_CELL_SET("%s", (lookup_type_string + meaningful_lookup_type_string_offset)); /** Type */
                PRT_CELL_SET("%s", db_key); /** Key */
            }
            else
            {
                PRT_CELL_SET("%d", index); /** Search */
                PRT_CELL_SET("N/A"); /** Result */
                PRT_CELL_SET("N/A"); /** DB ID */
                PRT_CELL_SET(""); /** DB Name */
                PRT_CELL_SET("%s", (lookup_type_string + meaningful_lookup_type_string_offset)); /** Type */
                PRT_CELL_SET(""); /** Key */
            }
        }

        PRT_COMMITX;

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_RESULT_BITMAP, INST_SINGLE, &result_bmp));

        SHR_IF_ERR_EXIT(kbp_mngr_opcode_total_result_size_get(unit, opcode_id, &total_result_bits));
        PRT_TITLE_SET("Total result length %d", BITS2BYTES(total_result_bits) + 1); /** +1 including the hit indication */

        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Result");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Length");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Offset");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Padding");

        for (index = 0; index < DNX_KBP_MAX_NOF_RESULTS; index++)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%d", index); /** Result */

            if (utilex_bitstream_test_bit(&result_bmp, index))
            {
                uint8 size;
                uint8 offset;
                uint8 padding;

                dbal_entry_key_field8_set(unit, res_entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
                dbal_entry_key_field8_set(unit, res_entry_handle_id, DBAL_FIELD_RESULT_ID, (uint8) index);
                SHR_IF_ERR_EXIT(dbal_entry_get(unit, res_entry_handle_id, DBAL_GET_ALL_FIELDS));

                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                                (unit, res_entry_handle_id, DBAL_FIELD_RESULT_SIZE, INST_SINGLE, &size));
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                                (unit, res_entry_handle_id, DBAL_FIELD_RESULT_OFFSET, INST_SINGLE, &offset));
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                                (unit, res_entry_handle_id, DBAL_FIELD_RESULT_PADDING, INST_SINGLE, &padding));

                PRT_CELL_SET("%d", size); /** Length */
                PRT_CELL_SET("%d", offset); /** Offset */
                PRT_CELL_SET("%d", padding); /** Padding */

            }
            else
            {
                PRT_CELL_SET("N/A"); /** Length */
                PRT_CELL_SET("N/A"); /** Offset */
                PRT_CELL_SET("N/A"); /** Padding */
            }
        }

        PRT_COMMITX;

        if (print_one)
        {
            /** Print only one opcode. Stop here */
            break;
        }

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_diag_opcode_list_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 entry_handle_id;
    uint8 is_valid = TRUE;
    int is_end;
    dbal_enum_value_field_kbp_device_status_e kbp_mngr_status;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr8_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, DBAL_CONDITION_EQUAL_TO, &is_valid,
                     &is_valid));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    PRT_TITLE_SET("KBP Opcode list");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Name");

    SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));
    if (kbp_mngr_status != DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED)
    {
        PRT_INFO_ADD("WARNING: KBP device is not locked");
    }

    while (!is_end)
    {
        uint8 opcode_id = 0;
        char opcode_name[DBAL_MAX_STRING_LENGTH] = { 0 };

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_get(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, &opcode_id));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                        (unit, entry_handle_id, DBAL_FIELD_OPCODE_NAME, INST_SINGLE, (uint8 *) opcode_name));

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%d", opcode_id);
        PRT_CELL_SET("%s", opcode_name);

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

    PRT_COMMITX;

exit:
    DBAL_FUNC_FREE_VARS;
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - CMD for getting KBP DB info
 */
static shr_error_e
dnx_kbp_diag_db_dump_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 db_entry_handle_id;
    uint32 db_id;
    uint32 key_width;
    uint32 res_width;
    uint32 optimized_width;
    uint32 clone_db;
    uint32 type;
    uint32 table_id;
    uint32 meta_prio;
    uint8 is_valid;
    uint8 counters_enable;
    uint8 db_has_zero_size_ad;

    int is_end;
    char db_key[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP * DNX_KBP_DIAG_PRINTABLE_KEY_SEGMENT_SIZE] = { 0 };
    dbal_enum_value_field_kbp_device_status_e kbp_mngr_status;
    uint32 num_of_entries;
    uint32 estimated_capacity;
    uint32 algo_mode;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_DB_INFO, &db_entry_handle_id));

    /** Initialize iterator to iterate over all db's */
    is_valid = TRUE;
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, db_entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr8_rule_add
                    (unit, db_entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, DBAL_CONDITION_EQUAL_TO, &is_valid,
                     &is_valid));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, db_entry_handle_id, &is_end));

    PRT_TITLE_SET("KBP DB DUMP");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Key Width");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Result Width (regular)");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Result Width (optimized)");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Result 0 size");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Clone From");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Type");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Table Id");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Counters");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Algo modes");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Meta Prio");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "DB key");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Num Entries");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Estimated Capacity");

    SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));
    if (kbp_mngr_status != DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED)
    {
        PRT_INFO_ADD("WARNING: KBP device is not locked");
    }
    while (!is_end)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, db_entry_handle_id, DBAL_FIELD_DB_ID, &db_id));

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, db_entry_handle_id, DBAL_FIELD_KEY_WIDTH, INST_SINGLE, &key_width));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, db_entry_handle_id, DBAL_FIELD_RESULT_WIDTH, INST_SINGLE, &res_width));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, db_entry_handle_id, DBAL_FIELD_OPTIMIZED_WIDTH, INST_SINGLE, &optimized_width));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, db_entry_handle_id, DBAL_FIELD_CLONE_DB, INST_SINGLE, &clone_db));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, db_entry_handle_id, DBAL_FIELD_DB_TYPE, INST_SINGLE, &type));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, db_entry_handle_id, DBAL_FIELD_TABLE_ID, INST_SINGLE, &table_id));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                        (unit, db_entry_handle_id, DBAL_FIELD_ENTRY_COUNTERS_EN, INST_SINGLE, &counters_enable));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, db_entry_handle_id, DBAL_FIELD_META_PRIORITY, INST_SINGLE, &meta_prio));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, db_entry_handle_id, DBAL_FIELD_ALGO_MODE, INST_SINGLE, &algo_mode));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                        (unit, db_entry_handle_id, DBAL_FIELD_DB_HAS_ZERO_SIZE_AD, INST_SINGLE, &db_has_zero_size_ad));

        SHR_IF_ERR_EXIT(kbp_mngr_db_capacity_get(unit, db_id, &num_of_entries, &estimated_capacity));

        SHR_IF_ERR_EXIT(dnx_kbp_diag_printable_db_key_build(unit, db_entry_handle_id, db_key));
        PRT_CELL_SET("%d", db_id);
        PRT_CELL_SET("%s", dbal_logical_table_to_string(unit, table_id));
        PRT_CELL_SET("%d", key_width);
        if (res_width)
        {
            PRT_CELL_SET("%d", res_width);
        }
        else
        {
            PRT_CELL_SET("%s", "N/A");
        }
        if (optimized_width)
        {
            PRT_CELL_SET("%d", optimized_width);
        }
        else
        {
            PRT_CELL_SET("%s", "N/A");
        }
        PRT_CELL_SET("%s", db_has_zero_size_ad ? "Yes" : "No");
        PRT_CELL_SET("%d", clone_db);
        PRT_CELL_SET("%s", dbal_table_type_to_string(unit, type));
        PRT_CELL_SET("%s", dbal_logical_table_to_string(unit, table_id));
        PRT_CELL_SET("%s", (counters_enable ? "ENABLED" : "DIASBLED"));
        PRT_CELL_SET("%s", dbal_enum_id_type_to_string(unit, DBAL_FIELD_TYPE_DEF_ALGO_MODE, algo_mode));
        PRT_CELL_SET("%d", meta_prio);
        PRT_CELL_SET("%s", db_key);
        /** Number of entries and capacity are available only after KBP is configured and locked */
        if (kbp_mngr_status == DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED)
        {
            SHR_IF_ERR_EXIT(kbp_mngr_db_capacity_get(unit, db_id, &num_of_entries, &estimated_capacity));
            PRT_CELL_SET("%d", num_of_entries);
            PRT_CELL_SET("%d", estimated_capacity);
        }
        else
        {
            PRT_CELL_SET("%s", "N/A");
            PRT_CELL_SET("%s", "N/A");
        }

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, db_entry_handle_id, &is_end));
    }
    PRT_COMMITX;

exit:
    DBAL_FUNC_FREE_VARS;
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static void
dnx_kbp_diag_rop_last_payload_to_string(
    dnx_kbp_rop_packet_t * rop_packet,
    char payload_string[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES * 2 + 1])
{
    int index;

    sal_memset(payload_string, 0x0, (DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES * 2 + 1));
    for (index = 0; index < rop_packet->payload_len; index++)
    {
        char tmp_byte[3] = { 0 };
        sal_snprintf(tmp_byte, 3, "%02X", rop_packet->payload[index]);
        sal_strncat(payload_string, tmp_byte, (DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES * 2 + 1));
    }
}

/**
 * \brief
 * defualt unknown opcode string
 */
static char *unknown_opcode_string = "unknown_opcode";

CONST char *
dnx_kbp_diag_managment_opcode_string(
    int unit,
    uint8 opcode)
{
    switch (opcode)
    {
        case DNX_KBP_CPU_WR_LUT_OPCODE:
            return "WR_LUT";
            break;
        case DNX_KBP_CPU_RD_LUT_OPCODE:
            return "RD_LUT";
            break;
        case DNX_KBP_CPU_PIOWR_OPCODE:
            return "PIOWR";
            break;
        case DNX_KBP_CPU_PIORDX_OPCODE:
            return "PIORDX";
            break;
        case DNX_KBP_CPU_PIORDY_OPCODE:
            return "PIORDY";
            break;
        case DNX_KBP_CPU_ERR_OPCODE:
            return "ERR";
            break;
        case DNX_KBP_CPU_BLK_COPY_OPCODE:
            return "BLK_COPY";
            break;
        case DNX_KBP_CPU_BLK_MOVE_OPCODE:
            return "BLK_MOVE";
            break;
        case DNX_KBP_CPU_BLK_CLR_OPCODE:
            return "BLK_CLR";
            break;
        case DNX_KBP_CPU_BLK_EV_OPCODE:
            return "BLK_EV";
            break;
        case DNX_KBP_CPU_CTX_BUFF_WRITE_OPCODE:
            return "CTX_BUFF_WRITE";
            break;
        case DNX_KBP_CPU_PAD_OPCODE:
            return "PAD";
            break;
        default:
            return unknown_opcode_string;
    }
}

/**
 * \brief
 * Translate KBP forward opcode to string,
 * this function is for opcode>1 and opcode<200 strings
 */
shr_error_e
dnx_kbp_diag_opcode_id_to_string(
    int unit,
    uint8 opcode,
    char *opcode_str)
{
    uint32 entry_handle_id;
    uint8 is_valid = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id));
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    if (opcode_str)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                        (unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, &is_valid));

        if (!is_valid)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "opcode (%d) not valid\n", opcode);
        }

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                        (unit, entry_handle_id, DBAL_FIELD_OPCODE_NAME, INST_SINGLE, (uint8 *) opcode_str));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Translate KBP opcode to string
 */
shr_error_e
dnx_kbp_diag_opcode_string(
    int unit,
    uint8 opcode,
    char *opcode_str)
{
    SHR_FUNC_INIT_VARS(unit);
    {
        if (opcode == DNX_KBP_CPU_PAD_OPCODE)
        {
            sal_snprintf(opcode_str, DBAL_MAX_STRING_LENGTH, "INVALID");
        }
        else if (opcode < DNX_KBP_CPU_MANAGMENT_OPCODE_FIRST)
        {
            SHR_IF_ERR_EXIT(dnx_kbp_diag_opcode_id_to_string(unit, opcode, opcode_str));
        }
        else
        {
            sal_snprintf(opcode_str, DBAL_MAX_STRING_LENGTH, "%s", dnx_kbp_diag_managment_opcode_string(unit, opcode));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_diag_rop_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char opcode_str[DBAL_MAX_STRING_LENGTH];
    int core;
    uint8 request = 0;
    uint8 response = 0;
    dnx_kbp_rop_packet_t rop_packet = { 0 };
    char payload_string[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES * 2 + 1] = { 0 };

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (!dnx_kbp_device_enabled(unit))
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "KBP was not enabled for the device.\n");
    }

    SH_SAND_GET_BOOL("ReQueST", request);
    SH_SAND_GET_BOOL("ReSPoNSe", response);

#if defined(ADAPTER_SERVER_MODE)
    LOG_CLI((BSL_META("ROP diag is not supported in adapter\n")));
    SHR_EXIT();
#endif

    if (!request && !response)
    {
        /** Print both by default */
        request = TRUE;
        response = TRUE;
    }

    PRT_TITLE_SET("ROP packet diagnostic");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Type");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Core");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Opcode");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Seq num");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Length");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Payload");

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        if (request)
        {
            SHR_IF_ERR_EXIT(dnx_kbp_rop_last_request_get(unit, core, DNX_KBP_ROP_GET_DNX_SIDE, &rop_packet));
            dnx_kbp_diag_rop_last_payload_to_string(&rop_packet, payload_string);
            SHR_IF_ERR_EXIT(dnx_kbp_diag_opcode_string(unit, rop_packet.opcode, opcode_str));

            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", "Request from DNX");
            PRT_CELL_SET("%d", core);
            PRT_CELL_SET("%s", opcode_str);
            PRT_CELL_SET("%d", rop_packet.sequence_num);
            PRT_CELL_SET("%d", rop_packet.payload_len);
            PRT_CELL_SET("%s", payload_string);
        }

        if (response)
        {
            SHR_IF_ERR_EXIT(dnx_kbp_rop_last_response_get(unit, core, DNX_KBP_ROP_GET_KBP_SIDE, &rop_packet));
            dnx_kbp_diag_rop_last_payload_to_string(&rop_packet, payload_string);
            SHR_IF_ERR_EXIT(dnx_kbp_diag_opcode_string(unit, rop_packet.opcode, opcode_str));

            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", "Response from KBP");
            PRT_CELL_SET("%d", core);
            PRT_CELL_SET("%s", opcode_str);
            PRT_CELL_SET("%d", rop_packet.sequence_num);
            PRT_CELL_SET("%d", rop_packet.payload_len);
            PRT_CELL_SET("%s", payload_string);
        }
    }

    PRT_COMMITX;

exit:
    DBAL_FUNC_FREE_VARS;
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - CMD for ROP write
 */
static shr_error_e
dnx_kbp_diag_rop_write_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 *data, *mask, addr, nbo_addr;
    int core, count, i, core_index;
    dnx_kbp_rop_write_t wr_data;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("core", core);
    SH_SAND_GET_UINT32("address", addr);
    SH_SAND_GET_ARRAY32("data", data);
    SH_SAND_GET_ARRAY32("mask", mask);
    SH_SAND_GET_INT32("count", count);

    nbo_addr = soc_htonl(addr);
    sal_memcpy(wr_data.addr, &nbo_addr, sizeof(uint32));

#if defined(BE_HOST)
    dnx_kbp_buffer_endians_fix(unit, data);
#endif

    DNX_KBP_ROP_REVERSE_DATA(((uint8 *) data), wr_data.data, NLM_DATA_WIDTH_BYTES);
    sal_memcpy(wr_data.mask, (uint8 *) mask, NLM_DATA_WIDTH_BYTES);

    for (i = 0; i < count; ++i)
    {
        DNXCMN_CORES_ITER(unit, core, core_index)
        {
            SHR_IF_ERR_EXIT(dnx_kbp_rop_write(unit, core_index, &wr_data));
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - CMD for ROP read
 */
static shr_error_e
dnx_kbp_diag_rop_read_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 addr, nbo_addr;
    int core, i, core_index;
    dnx_kbp_rop_read_t rd_data;
    NlmDnxReadMode datatype;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("core", core);
    SH_SAND_GET_UINT32("address", addr);
    SH_SAND_GET_ENUM("datatype", datatype);

    nbo_addr = soc_htonl(addr);
    sal_memcpy(rd_data.addr, &nbo_addr, sizeof(uint32));
    rd_data.dataType = datatype;

    DNXCMN_CORES_ITER(unit, core, core_index)
    {
        SHR_IF_ERR_EXIT(dnx_kbp_rop_read(unit, core_index, &rd_data));
        cli_out("ROP Read for Core %d:\n", core_index);
        cli_out("    Read Status: 0x%02x \n", rd_data.data[0]);
        cli_out("    Read Data:   0x");
        for (i = 1; i < NLM_DATA_WIDTH_BYTES + 1; ++i)
        {
            cli_out("%02x ", rd_data.data[i]);
        }
        cli_out("\n");
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - CMD for ROP test
 */
static shr_error_e
dnx_kbp_diag_rop_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    int core, count, i, iter, core_index;
    int kbp_mdio_id, kbp_rop_retrans;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("core", core);
    SH_SAND_GET_INT32("count", count);

    kbp_mdio_id = dnx_data_elk.connectivity.mdio_get(unit, 0)->mdio_id;
    kbp_rop_retrans = dnx_data_elk.recovery.iteration_get(unit);

    DNXCMN_CORES_ITER(unit, core, core_index)
    {
        for (i = 0; i < count; i++)
        {
            SHR_IF_ERR_CONT(dnx_kbp_rop_scratch_write_read(unit, core_index));

            if (SHR_FUNC_ERR())
            {
                SHR_SET_CURRENT_ERR(_SHR_E_NONE);
                for (iter = 0; iter < kbp_rop_retrans; iter++)
                {
                    SHR_IF_ERR_EXIT(dnx_kbp_recover_run_recovery_sequence(unit, core_index, kbp_mdio_id));

                    SHR_IF_ERR_CONT(dnx_kbp_rop_scratch_write_read(unit, core_index));

                    if (!SHR_FUNC_ERR())
                    {
                        /*
                         * KBP Recovery succeeded
                         */
                        break;
                    }
                }
                /*
                 * KBP Recovery failed
                 */
                if (iter == kbp_rop_retrans)
                {
                    SHR_ERR_EXIT(_SHR_E_NONE, "ROP stability test failed on core %d\n", core_index);
                }
            }
        }

        cli_out("ROP stability test PASSED on core %d\n", core_index);
    }

    cli_out("ROP stability test was completed successfully \n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - CMD for dumpping KBP status registers
 */
static shr_error_e
dnx_kbp_diag_status_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *file_path = NULL;
    FILE *kbp_file;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("PATH", file_path);

    if (sal_strncmp(file_path, "stdout", sal_strlen(file_path)) != 0)
    {
        kbp_file = fopen(file_path, "w");
    }
    else
    {
        kbp_file = stdout;
    }

    DNX_KBP_TRY(kbp_device_interface_print_regs
                (Kbp_app_data[unit]->device_type, Kbp_app_data[unit]->flags, &Kbp_app_data[unit]->device_config[0],
                 kbp_file));

    if (sal_strncmp(file_path, "stdout", sal_strlen(file_path)) != 0)
    {
        fclose(kbp_file);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * This function is used for dynamic parameters translation for
 * dnx_kbp_diag_opcode_list_cmd diagnostic, it enable the insertion of dynamic master key fields
 * for each unique master key structure
 */
static shr_error_e
dnx_kbp_diag_search_cmd_field_get(
    int unit,
    char *keyword,
    sal_field_type_e * type_p,
    uint32 *id_p,
    void **ext_ptr_p)
{
    dbal_fields_e field_id = DBAL_NOF_FIELDS;
    dbal_field_print_type_e field_type;

    SHR_FUNC_INIT_VARS(unit);

    if (type_p != NULL)
    {
        shr_error_e rv;
        rv = dbal_field_string_to_id_no_error(unit, keyword, &field_id);
        if (rv == _SHR_E_NONE)
        {
            SHR_IF_ERR_EXIT(dbal_fields_print_type_get(unit, field_id, &field_type));
            *type_p = dbal_field_type_to_sal_field_type[field_type];
        }
        else
        {
            *type_p = SAL_FIELD_TYPE_UINT32;
        }
    }
    if (id_p != NULL)
    {
        *id_p = field_id;
    }
    if (ext_ptr_p != NULL)
    {
        *ext_ptr_p = NULL;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * This function print the available master key segment fields for a given opcode
 */
shr_error_e
dnx_kbp_diag_search_print_master_key_fields(
    int unit,
    uint32 entry_handle_id,
    char master_key[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY * DNX_KBP_DIAG_PRINTABLE_KEY_SEGMENT_SIZE])
{
    int index;
    uint8 opcode_id;
    uint32 nof_segments;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(master_key, 0x0,
               (sizeof(char) * DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY * DNX_KBP_DIAG_PRINTABLE_KEY_SEGMENT_SIZE));

    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_get(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, &opcode_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                    (unit, entry_handle_id, DBAL_FIELD_NOF_VALID_SEGMENTS, INST_SINGLE, &nof_segments));

    for (index = 0; index < nof_segments; index++)
    {
        uint32 segment_type, segment_bytes, overlay_offset_bytes;
        char segment_name[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES] = { 0 };
        char master_key_segment[DNX_KBP_DIAG_PRINTABLE_KEY_SEGMENT_SIZE] = { 0 };
        char segment_type_string[DBAL_MAX_STRING_LENGTH] = { 0 };

        SHR_IF_ERR_EXIT(kbp_mngr_master_key_segment_get
                        (unit, opcode_id, index, &segment_type, &segment_bytes, segment_name, &overlay_offset_bytes));

        if ((sal_strncmp(segment_name, "ZERO_PADDING", sal_strlen("ZERO_PADDING")) == 0) ||
            (DNX_KBP_KEY_IS_OVERLAY_FIELD(overlay_offset_bytes)))
        {
            continue;
        }

        SHR_IF_ERR_EXIT(dnx_dbal_fields_string_form_hw_value_get
                        (unit, "SEGMENT_TYPE", segment_type, segment_type_string));

        sal_snprintf(master_key_segment, (DNX_KBP_DIAG_PRINTABLE_KEY_SEGMENT_SIZE - 1), "%s=<value> ", segment_name);

        sal_strncat(master_key, master_key_segment,
                    (DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY * DNX_KBP_DIAG_PRINTABLE_KEY_SEGMENT_SIZE - 1));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - CMD for performing search on KBP lookups
 * This diagnostic build the requested search master key according to the requested opcode
 * then it exercises the instruction and searches the hardware/model. The real
 */
static shr_error_e
dnx_kbp_diag_search_cmds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    struct kbp_search_result search_rslt;
    uint8 master_key[80] = { 0 };
    uint32 opcode_id;
    char opcode_name[DBAL_MAX_STRING_LENGTH] = { 0 };
    uint32 entry_handle_id;
    uint32 nof_master_key_segments;
    uint8 is_valid;
    uint8 seg_index;
    kbp_instruction_handles_t inst_handle;
    sh_sand_arg_t *sand_arg;
    char *arg_name;
    uint32 segment_bytes;
    uint8 index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    DNX_KBP_DIAG_CMD_VERIFY(unit);

    SH_SAND_GET_UINT32("OPcode", opcode_id);

    if (opcode_id == DNX_KBP_MAX_NOF_OPCODES)
    {
        DIAG_DNX_KBP_HEADER_DUMP("Example: kbp search Opcode=<opcode_id>", "\0");
        SHR_IF_ERR_EXIT(dnx_kbp_diag_opcode_list_cmd(unit, NULL, NULL));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id));
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                    (unit, entry_handle_id, DBAL_FIELD_OPCODE_NAME, INST_SINGLE, (uint8 *) opcode_name));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, &is_valid));

    if (!is_valid)
    {
        bsl_printf("Opcode %d is not in use\n", opcode_id);
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_NOF_VALID_SEGMENTS, INST_SINGLE, &nof_master_key_segments));

    seg_index = 0;
    SH_SAND_GET_ITERATOR(sand_arg)
    {
        uint8 byte_offset = 0;
        sal_field_type_e field_type;

        field_type = SH_SAND_GET_TYPE(sand_arg);
        arg_name = SH_SAND_GET_NAME(sand_arg);

        for (index = 0; index < nof_master_key_segments; index++)
        {
            uint32 segment_type, overlay_offset_bytes;
            char segment_name[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES] = { 0 };
            char segment_type_string[DBAL_MAX_STRING_LENGTH] = { 0 };

            SHR_IF_ERR_EXIT(kbp_mngr_master_key_segment_get
                            (unit, opcode_id, index, &segment_type, &segment_bytes, segment_name,
                             &overlay_offset_bytes));

            SHR_IF_ERR_EXIT(dnx_dbal_fields_string_form_hw_value_get
                            (unit, "SEGMENT_TYPE", segment_type, segment_type_string));

            if (DNX_KBP_KEY_IS_OVERLAY_FIELD(overlay_offset_bytes) ||
                (sal_strncmp(segment_name, arg_name, sal_strlen(arg_name)) == 0))
            {
                KBP_MASTER_KEY_SEGMENT_SET(unit, byte_offset, segment_bytes, field_type, sand_arg, master_key);
            }
            else
            {
                byte_offset += segment_bytes;
            }
        }
        seg_index++;
    }

    if (seg_index == 0)
    {
        char master_key_string[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY * DNX_KBP_DIAG_PRINTABLE_KEY_SEGMENT_SIZE];
        DIAG_DNX_KBP_HEADER_DUMP("USAGE EXAMPLE: ", "\0");
        LOG_CLI((BSL_META("kbp search OPCODE=%d "), opcode_id));
        SHR_IF_ERR_EXIT(dnx_kbp_diag_search_print_master_key_fields(unit, entry_handle_id, master_key_string));
        LOG_CLI((BSL_META("%s"), master_key_string));
        LOG_CLI((BSL_META("\n")));
        SHR_EXIT();
    }

    sal_memset(&search_rslt, 0, sizeof(struct kbp_search_result));

    SHR_IF_ERR_EXIT(KBP_ACCESS.instruction_info.get(unit, opcode_id, &inst_handle));

    SHR_IF_ERR_EXIT(kbp_instruction_search(inst_handle.inst_p[0], master_key, 0, &search_rslt));

    LOG_CLI((BSL_META("kbp search result for opcode %s is HITBIT[%d,%d,%d,%d,%d,%d,%d,%d]\n"),
             opcode_name, search_rslt.hit_or_miss[0], search_rslt.hit_or_miss[1], search_rslt.hit_or_miss[2],
             search_rslt.hit_or_miss[3], search_rslt.hit_or_miss[4], search_rslt.hit_or_miss[5],
             search_rslt.hit_or_miss[6], search_rslt.hit_or_miss[7]));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - CMD for performing counters read for a Field Group on KBP
 */
static shr_error_e
dnx_kbp_diag_db_counters_cmds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int *ext_acl_db;
    uint32 db_entry_handle_id;
    dbal_tables_e table_id, db_table_id;
    char *db_name;
    int is_complete;
    int is_end;
    kbp_db_handles_t kbp_db_handles;
    uint8 is_valid;
    uint8 counters_enable;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    DNX_KBP_DIAG_CMD_VERIFY(unit);

    SH_SAND_GET_STR("DB", db_name);

    if (ISEMPTY(db_name))
    {
        LOG_CLI((BSL_META("db name must exists: kbp db CouNTer DB=<DB_name>\n")));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dbal_logical_table_string_to_id(unit, db_name, &table_id));

    SHR_IF_ERR_EXIT(dbal_tables_table_access_info_get
                    (unit, table_id, DBAL_ACCESS_METHOD_KBP, (void *) &kbp_db_handles));
    ext_acl_db = (int *) (kbp_db_handles.db_p);

    /** check if DB is valid and has counters enabled */

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_DB_INFO, &db_entry_handle_id));

    /** Initialize iterator to iterate over all db's */
    is_valid = TRUE;
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, db_entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr8_rule_add
                    (unit, db_entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, DBAL_CONDITION_EQUAL_TO, &is_valid,
                     &is_valid));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, db_entry_handle_id, &is_end));

    while (!is_end)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, db_entry_handle_id, DBAL_FIELD_TABLE_ID, INST_SINGLE, &db_table_id));
        if (table_id == db_table_id)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                            (unit, db_entry_handle_id, DBAL_FIELD_ENTRY_COUNTERS_EN, INST_SINGLE, &counters_enable));
            if (!counters_enable)
            {
                SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "ACL counters are not enabled for DB [%s] %s%s", db_name, EMPTY, EMPTY);
            }
        }
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, db_entry_handle_id, &is_end));
    }

    /** Initiate counters read */
    DNX_KBP_TRY(kbp_db_counter_read_initiate((struct kbp_db *) (ext_acl_db)));

    do
    {
        DNX_KBP_TRY(kbp_db_is_counter_read_complete((struct kbp_db *) (ext_acl_db), &is_complete));
    }
    while (is_complete == 0);

#if defined(LONGS_ARE_64BITS) || defined (COMPILER_HAS_LONGLONG)
/* { */
    {
        /*
         * This code is intended to run only when compiler has 64 bits variables.
         * See the reason in file compiler.h
         */
        uint32 entry_handle_id;
        /** dump results */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
        PRT_TITLE_SET("KBP ACL counters dump for DB: %s", dbal_logical_table_to_string(unit, table_id));
        PRT_COLUMN_ADD("ACCESS ID");
        PRT_COLUMN_ADD("Counters Value");

        /*
         * Use iterator to print all ACL counters
         */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

        while (!is_end)
        {
            uint32 entry_access_id;
            kbp_entry_t *kbp_entry;
            uint64_t value;

            SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_get(unit, entry_handle_id, &entry_access_id));
            kbp_entry = INT_TO_PTR(entry_access_id);
            DNX_KBP_TRY(kbp_entry_get_counter_value((struct kbp_db *) (ext_acl_db), kbp_entry, &value));
            if (value > 0)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("%d", entry_access_id);
                PRT_CELL_SET("%ju", value);
            }

            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
        PRT_INFO_ADD("Note: ACL counters are clear on read, entries with 0 hits are not displayed");
        PRT_INFO_SET_MODE(PRT_ROW_SEP_EQUAL_BEFORE);
        PRT_COMMITX;
    }
/* } */
#else
/* { */
    LOG_CLI_EX("\r\n"
               "This compiler does not have 'long long' or has 'long' as 64 bits so counters can not be displayed!!%s%s%s%s\r\n\n",
               EMPTY, EMPTY, EMPTY, EMPTY);
/* } */
#endif
exit:
    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - CMD for configuring and enabling KBP prbs
 */
static shr_error_e
dnx_kbp_diag_prbs_set_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int poly, lane_is_present = 0;
    uint32 lane_bmp;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_ENUM("polynomial", poly);
    SH_SAND_IS_PRESENT("lane", lane_is_present);

    if (lane_is_present)
    {
        /*
         * Per Lane PRBS
         */
        SH_SAND_GET_UINT32("lane", lane_bmp);

        cli_out("Enabling PRBS, PRBS poly %d\n", poly);
        DNX_KBP_TRY(kbp_device_interface_per_lane_prbs
                    (Kbp_app_data[unit]->device_type, Kbp_app_data[unit]->flags, &Kbp_app_data[unit]->device_config[0],
                     poly, 1, lane_bmp));
        cli_out("Clearing PRBS counters, please ignore!\n");
        DNX_KBP_TRY(kbp_device_interface_per_lane_prbs_print
                    (Kbp_app_data[unit]->device_type, Kbp_app_data[unit]->flags, &Kbp_app_data[unit]->device_config[0],
                     lane_bmp));
    }
    else
    {
        cli_out("Enabling PRBS, PRBS poly %d\n", poly);
        DNX_KBP_TRY(kbp_device_interface_prbs
                    (Kbp_app_data[unit]->device_type, Kbp_app_data[unit]->flags, &Kbp_app_data[unit]->device_config[0],
                     poly, 1));
        cli_out("Clearing PRBS counters, please ignore!\n");
        DNX_KBP_TRY(kbp_device_interface_prbs_print
                    (Kbp_app_data[unit]->device_type, Kbp_app_data[unit]->flags,
                     &Kbp_app_data[unit]->device_config[0]));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - CMD for getting KBP prbs result
 */
static shr_error_e
dnx_kbp_diag_prbs_get_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int lane_is_present = 0;
    uint32 lane_bmp;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_IS_PRESENT("lane", lane_is_present);

    if (lane_is_present)
    {
        /*
         * Per Lane PRBS
         */
        SH_SAND_GET_UINT32("lane", lane_bmp);
        DNX_KBP_TRY(kbp_device_interface_per_lane_prbs_print
                    (Kbp_app_data[unit]->device_type, Kbp_app_data[unit]->flags, &Kbp_app_data[unit]->device_config[0],
                     lane_bmp));
    }
    else
    {
        DNX_KBP_TRY(kbp_device_interface_prbs_print
                    (Kbp_app_data[unit]->device_type, Kbp_app_data[unit]->flags,
                     &Kbp_app_data[unit]->device_config[0]));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - CMD for disabling KBP prbs
 */
static shr_error_e
dnx_kbp_diag_prbs_clear_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int poly, lane_is_present = 0;
    uint32 lane_bmp;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_ENUM("polynomial", poly);

    SH_SAND_IS_PRESENT("lane", lane_is_present);

    cli_out("Disabling PRBS\n");

    if (lane_is_present)
    {
        /*
         * Per Lane PRBS
         */
        SH_SAND_GET_UINT32("lane", lane_bmp);
        DNX_KBP_TRY(kbp_device_interface_per_lane_prbs
                    (Kbp_app_data[unit]->device_type, Kbp_app_data[unit]->flags, &Kbp_app_data[unit]->device_config[0],
                     poly, 0, lane_bmp));
    }
    else
    {
        DNX_KBP_TRY(kbp_device_interface_prbs
                    (Kbp_app_data[unit]->device_type, Kbp_app_data[unit]->flags, &Kbp_app_data[unit]->device_config[0],
                     poly, 0));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - CMD for dumpping KBP eyescan for each lane
 */
static shr_error_e
dnx_kbp_diag_eyescan_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_KBP_TRY(kbp_device_interface_eyescan_print
                (Kbp_app_data[unit]->device_type, Kbp_app_data[unit]->flags, &Kbp_app_data[unit]->device_config[0]));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - CMD for dumpping KBP interface info
 */
static shr_error_e
dnx_kbp_diag_intf_info_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int lane_is_present = 0;
    uint32 lane_bmp;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_IS_PRESENT("lane", lane_is_present);

    if (lane_is_present)
    {
        SH_SAND_GET_UINT32("lane", lane_bmp);

        DNX_KBP_TRY(kbp_device_interface_per_lane_diag_print
                    (Kbp_app_data[unit]->device_type, Kbp_app_data[unit]->flags, &Kbp_app_data[unit]->device_config[0],
                     lane_bmp));
    }
    else
    {
        DNX_KBP_TRY(kbp_device_interface_diag_print
                    (Kbp_app_data[unit]->device_type, Kbp_app_data[unit]->flags,
                     &Kbp_app_data[unit]->device_config[0]));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - CMD for getting KBP result from FWD2 to PMF1,
 * the function prints the parsed payload and related opcode
 */
static shr_error_e
dnx_kbp_diag_vis_result_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int core, core_id;
    uint32 fwd_context;
    uint32 acl_context;
    uint32 *res_sig_value = NULL;
    int res_sig_size;
    signal_output_t *signal_output = NULL;
    kbp_printable_entry_t *kbp_entry_print_info = NULL;
    uint8 kbp_entry_print_num = 0;
    uint8 opcode_id;
    uint8 kbp_entry_index, result_index;
    char opcode_name[DBAL_MAX_STRING_LENGTH];
    char res_signal_str[DNX_KBP_DIAG_MAX_STRING_LENGTH];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (!dnx_kbp_device_enabled(unit))
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "KBP was not enabled for the device.\n");
    }

    kbp_entry_print_info = sal_alloc(DNX_KBP_MAX_NOF_RESULTS * sizeof(kbp_printable_entry_t), "kbp_entry_print_info");
    if (kbp_entry_print_info == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Unable to allocate memory for kbp_entry_print_info\n");
    }

    sal_memset(kbp_entry_print_info, 0x0, DNX_KBP_MAX_NOF_RESULTS * sizeof(kbp_printable_entry_t));

    SH_SAND_GET_INT32("Core", core);

    DNXCMN_CORES_ITER(unit, core, core_id)
    {
        DNX_PP_BLOCK_IS_READY_CONT(core_id, DNX_PP_BLOCK_IRPP, TRUE);
        /*
         * get context signal sig get from=FWD2 show=asic name=context
         */
        SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core_id, "IRPP", "FWD2", "", "Context", &fwd_context, 1));

        /*
         * get acl_context signal sig get from=FWD2 show=asic name=acl_context
         */
        SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core_id, "IRPP", "FWD2", "", "ACL_Context", &acl_context, 1));

        /*
         * read opcode from table
         */
        SHR_IF_ERR_EXIT_EXCEPT_IF(kbp_mngr_context_to_opcode_translate(unit, fwd_context, acl_context, &opcode_id),
                                  _SHR_E_NOT_FOUND);

        if (opcode_id != 255)
        {
            int word_idx, char_count;
            char *res_string = &res_signal_str[0];
            int meaningful_lookup_type_string_offset = sal_strlen("KBP_LOOKUP_TYPE_");

            SHR_IF_ERR_EXIT(dnx_kbp_diag_opcode_string(unit, opcode_id, opcode_name));

            SHR_IF_ERR_EXIT(sand_signal_output_find(unit, core_id, 0, "IRPP", "FWD2", "IPMF1", "Elk_Lkp_Payload",
                                                    &signal_output));
            res_sig_size = signal_output->size;
            res_sig_value = signal_output->value;

            /** Print Res as buffer */
            char_count = sal_snprintf(res_string, DNX_KBP_DIAG_MAX_STRING_LENGTH, "RES(%db)=0x", res_sig_size);
            res_string += char_count;
            for (word_idx = BITS2WORDS(res_sig_size) - 1; word_idx >= 0; word_idx--)
            {
                if (word_idx == (BITS2WORDS(res_sig_size) - 1))
                {
                    int print_width = (3 + (res_sig_size % 32)) / 4;

                    if (print_width == 0)
                    {
                        print_width = 8;
                    }
                    char_count = sal_snprintf(res_string,
                                              DNX_KBP_DIAG_MAX_STRING_LENGTH, "%0*x", print_width,
                                              res_sig_value[word_idx]);
                    res_string += char_count;
                }
                else
                {
                    char_count =
                        sal_snprintf(res_string, DNX_KBP_DIAG_MAX_STRING_LENGTH, "%08x", res_sig_value[word_idx]);
                    res_string += char_count;
                }
            }
            SHR_IF_ERR_EXIT(kbp_mngr_opcode_printable_entry_result_parsing
                            (unit, 0, opcode_id, res_sig_value, res_sig_size, &kbp_entry_print_num,
                             kbp_entry_print_info));

            PRT_TITLE_SET("KBP VIS packet diagnostic Core (%d) Opcode (%s)", core_id, opcode_name);
            PRT_INFO_ADD("Result value(%s)", res_signal_str);
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Lookup Id");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Type");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "DBAL Table");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Result values");
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            for (kbp_entry_index = 0; kbp_entry_index < kbp_entry_print_num; kbp_entry_index++)
            {
                kbp_printable_entry_t *kbp_entry = &kbp_entry_print_info[kbp_entry_index];

                res_string = &res_signal_str[0];

                for (result_index = 0; result_index < kbp_entry->entry_print_info.nof_res_fields; result_index++)
                {
                    char_count = sal_snprintf(res_string, DNX_KBP_DIAG_MAX_STRING_LENGTH, "%s ",
                                              kbp_entry->entry_print_info.res_fields_info[result_index].field_name);
                    res_string += char_count;
                    char_count = sal_snprintf(res_string, DNX_KBP_DIAG_MAX_STRING_LENGTH, "%s\n",
                                              kbp_entry->entry_print_info.
                                              res_fields_info[result_index].field_print_value);
                    res_string += char_count;
                }
                PRT_CELL_SET("%d", kbp_entry_print_info[kbp_entry_index].lookup_id);

                PRT_CELL_SET("%s",
                             (kbp_entry_print_info[kbp_entry_index].lookup_type_str +
                              meaningful_lookup_type_string_offset));

                PRT_CELL_SET("%s", dbal_logical_table_to_string(unit, kbp_entry_print_info[kbp_entry_index].table_id));

                PRT_CELL_SET("%s", res_signal_str);
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            }

            PRT_COMMITX;
        }
    }

exit:
    sand_signal_output_free(signal_output);
    SHR_FREE(kbp_entry_print_info);
    DBAL_FUNC_FREE_VARS;
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */

static sh_sand_man_t dnx_kbp_diag_device_man = {
    .brief = "KBP device diagnostic commands"
};
static sh_sand_man_t dnx_kbp_diag_device_print_man = {
    .brief = "Prints device information",
    .full = "Prints device information in HTML format to the specified file.\n"
             "If option \"path\" is not present, the output will be printed on the screen.\n"
             "Option \"info\" is not used with this diagnostic and will be ignored.",
    .synopsis = "path=<file_path>",
    .examples = "path=kbp_device_print.log info=0"
};
static sh_sand_man_t dnx_kbp_diag_device_dump_man = {
   .brief = "Dumps the current state of the s/w",
   .full = "Dumps the current state of the s/w in the XML Format to the specified file.\n"
            "If option \"path\" is not present, the output will be printed on the screen.\n"
            "If option \"info\"=1 the entries will be dumped as well (default=0).",
   .synopsis = "path=<file_path>",
   .examples = "path=kbp_device_dump.log info=0"
};
static sh_sand_man_t dnx_kbp_diag_device_swstate_man = {
   .brief = "Prints the current sw state",
   .full = "Prints the current sw state in encoded format to the specified file.\n"
            "If option \"path\" is not present, the output will be printed on the screen.\n"
            "Option \"info\" is not used with this diagnostic and will be ignored.",
   .synopsis = "path=<file_path>",
   .examples = "path=kbp_device_swstate.log info=0"
};
static sh_sand_man_t dnx_kbp_diag_device_last_man = {
   .brief = "Dumps Last result register data",
   .full = "Dumps Last result register data in readable format to the specified file.\n"
            "If option \"path\" is not present, the output will be printed on the screen.\n"
            "If option \"info\"=1 will dump the raw register data (default=0).",
   .synopsis = "path=<file_path>",
   .examples = "path=kbp_device_last.log info=0"
};

static sh_sand_man_t dnx_kbp_diag_sync_man = {
   .brief = "Performs KBP device sync",
   .full = "Performs KBP device sync in the following steps:\n"
            " 1. Applies the field configurations\n"
            " 2. Applies the KBP configurations\n"
            " 3. Applies all configurations to KBP HW"
};

static sh_sand_man_t dnx_kbp_diag_sdk_ver_man = {
    .brief = "Prints the KBPSDK version"
};

static sh_sand_man_t dnx_kbp_diag_opcode_man = {
    .brief = "KBP opcode diagnostic commands"
};

static sh_sand_man_t dnx_kbp_diag_db_man = {
    .brief = "KBP DB diagnostic commands"
};

static sh_sand_man_t dnx_kbp_diag_opcode_dump_man = {
   .brief = "Dumps all relevant information for a given opcode name or all for all opcodes",
   .synopsis = "opcode=<opcode_id | ALL>",
   .examples = "opcode=1"
};

static sh_sand_man_t dnx_kbp_diag_db_dump_man = {
   .brief = "Dumps all relevant information for a given db"
};

static sh_sand_man_t dnx_kbp_diag_opcode_list_man = {
   .brief = "Lists all valid opcodes"
};

static sh_sand_man_t dnx_kbp_diag_rop_man = {
    .brief = "ROP diagnostic commands",
    .full = "Print last ROP request packet from DNX and/or response packet from KBP",
    .synopsis = "ReQueST=0/1 ReSPoNSe=0/1",
    .examples = "ReQueST=1 ReSPoNSe=1"
};

static sh_sand_man_t dnx_kbp_diag_vis_result_man = {
    .brief = "KBP visibility result diagnostic command",
    .full = "Dumps the parsed KBP result from FWD2 to IPMF1 and its opcode"
};

static sh_sand_man_t dnx_kbp_diag_rop_write_man = {
    .brief    = "KBP ROP write operation",
    .full     = "KBP ROP write operation. The data and mask length should be 10 bytes. "
                "If the given data or mask length is less than 10 bytes, it will be padded "
                "to 10 bytes with 0. If the given data or mask length is more than 10 bytes, "
                "only the first 10 bytes should be valid.",
    .synopsis = "address=<addr> [core=<core>] [data=<data>] [mask=<mask>] [count=<count>]",
    .examples = "address=0xe data=0x11223344556677889900",
    .compatibility = KBP_COMPATIBILITY_MESSAGE
};
static sh_sand_man_t dnx_kbp_diag_rop_read_man = {
    .brief    = "KBP ROP Read operation",
    .full     = "KBP ROP Read operation",
    .synopsis = "address=<addr> [core=<core>]",
    .examples = "address=0xe core=0",
    .compatibility = KBP_COMPATIBILITY_MESSAGE
};
static sh_sand_man_t dnx_kbp_diag_rop_test_man = {
    .brief    = "KBP ROP Test operation",
    .full     = "KBP ROP Test operation",
    .synopsis = "[core=<core>] [count=<count>]",
    .examples = "core=0 count=5000",
    .compatibility = KBP_COMPATIBILITY_MESSAGE
};
static sh_sand_man_t dnx_kbp_diag_status_man = {
    .brief    = "Dump KBP status registers",
    .full     = "Dump KBP status registers. If option \"path\" is not present, "
                "the output will be printed on the screen.",
    .synopsis = "[path=<file_path>]",
    .compatibility = KBP_COMPATIBILITY_MESSAGE
};

static sh_sand_man_t dnx_kbp_diag_search_man = {
    .brief    = "Perform a search in KBP DB",
    .full     = "Perform a search in KBP DB, Exercises the instruction and search the hardware lookup"
                "for every valid opcode the requested master key segments are requested and a proper "
                "master key is built and used for the performed search",
    .compatibility = KBP_COMPATIBILITY_MESSAGE
};

static sh_sand_man_t dnx_kbp_diag_db_counters_man = {
    .brief    = "Read ACL counters for a DB",
    .full     = "Read ACL counters for a DB, ACL counters are clear on read",
    .compatibility = KBP_COMPATIBILITY_MESSAGE
};

static sh_sand_man_t dnx_kbp_diag_prbs_set_man = {
    .brief    = "Enable and configure KBP PRBS",
    .full     = "Enable and configure KBP PRBS. "
                "Support Per lane PRBS. Please specify the lane bitmp for "
                "option \"lane\" if per lane PRBS is required.",
    .synopsis = "[poly=<poly>][lane=<lane_bitmap>]",
    .examples = "poly=5",
};
static sh_sand_man_t dnx_kbp_diag_prbs_get_man = {
    .brief    = "Get KBP PRBS status",
    .full     = "Get KBP PRBS status. "
                "If per lane PRBS is enabled, please also specify "
                "the lane bitmap for get command.",
    .synopsis = "[lane=<lane_bitmap>]",
};
static sh_sand_man_t dnx_kbp_diag_prbs_clear_man = {
    .brief    = "Disable KBP PRBS",
    .full     = "Disable KBP PRBS. "
                "If per lane prbs is enabled, please also specify "
                "the lane bitmap for clear command",
    .synopsis = "[poly=<poly>][lane=<lane_bitmap>]",
    .examples = "poly=5",
};
static sh_sand_man_t dnx_kbp_diag_prbs_man = {
    .brief = "KBP PRBS",
    .compatibility = KBP_COMPATIBILITY_MESSAGE
};
static sh_sand_man_t dnx_kbp_diag_eyescan_man = {
    .brief    = "KBP eyescan",
    .full     = "KBP eyescan for all the active lanes",
    .compatibility = KBP_COMPATIBILITY_MESSAGE
};
static sh_sand_man_t dnx_kbp_diag_intf_info_man = {
    .brief    = "Dump KBP interface info.",
    .full     = "Dump KBP interface info.",
    .synopsis = "[lane=<lane_bitmap>]",
};
static sh_sand_man_t dnx_kbp_diag_intf_man = {
    .brief = "KBP Interface Diag",
    .compatibility = KBP_COMPATIBILITY_MESSAGE
};

static sh_sand_man_t dnx_kbp_diag_vis_man = {
    .brief = "KBP vis Interface Diag",
    .compatibility = KBP_COMPATIBILITY_MESSAGE
};

sh_sand_man_t sh_dnx_kbp_man = {
    .brief = "KBP diagnostic commands",
};

static sh_sand_option_t dnx_kbp_diag_device_options[] = {
    {"PATH", SAL_FIELD_TYPE_STR, "File path to the output file", "stdout"},
    {"INFo", SAL_FIELD_TYPE_INT32, "Additional options for the device diagnostics", "0"},
    {NULL}
};
static sh_sand_option_t dnx_kbp_diag_opcode_dump_options[] = {
    {"OPcode", SAL_FIELD_TYPE_UINT32, "ID of the opcode", "ALL", (void*)dnx_kbp_diag_opcode_dump_enum_table},
    {NULL}
};

static sh_sand_option_t dnx_kbp_diag_rop_options[] = {
    {"ReQueST", SAL_FIELD_TYPE_BOOL, "Request from DNX", "FALSE"},
    {"ReSPoNSe", SAL_FIELD_TYPE_BOOL, "Response from KBP", "FALSE"},
    {NULL}
};

static sh_sand_option_t dnx_kbp_diag_rop_write_options[] = {
    {"ADdRess", SAL_FIELD_TYPE_UINT32, "Address", NULL},
    {"DaTa", SAL_FIELD_TYPE_ARRAY32, "Date", "0"},
    {"MAsK", SAL_FIELD_TYPE_ARRAY32, "Mask", "0"},
    {"CouNT", SAL_FIELD_TYPE_INT32, "Number of iterations for rop write", "1"},
    {NULL}
};

static sh_sand_enum_t dnx_kbp_diag_rop_read_data_type_enum_table[] = {
    {"0",     NLM_DNX_READ_MODE_DATA_X, "Data X"},
    {"1",     NLM_DNX_READ_MODE_DATA_Y, "Data Y"},
    {NULL}
};
static sh_sand_option_t dnx_kbp_diag_rop_read_options[] = {
    {"ADdRess", SAL_FIELD_TYPE_UINT32, "Address", NULL},
    {"DaTaTyPe", SAL_FIELD_TYPE_ENUM, "Data Type", "0", (void *) dnx_kbp_diag_rop_read_data_type_enum_table},
    {NULL}
};
static sh_sand_option_t dnx_kbp_diag_rop_test_options[] = {
    {"CouNT", SAL_FIELD_TYPE_INT32, "Number of iterations for rop test", "10000"},
    {NULL}
};
static sh_sand_option_t dnx_kbp_diag_status_options[] = {
    {"PATH", SAL_FIELD_TYPE_STR, "File path for the output file", "stdout"},
    {NULL}
};

static sh_sand_option_t dnx_kbp_diag_search_options[] = {
    {"OPcode", SAL_FIELD_TYPE_UINT32, "ID of the opcode", "ALL", (void*)dnx_kbp_diag_opcode_dump_enum_table},
    {NULL}
};

static sh_sand_option_t dnx_kbp_diag_db_counters_options[] = {
    {"DB", SAL_FIELD_TYPE_STR, "DB Name",""},
    {NULL}
};

static sh_sand_enum_t dnx_kbp_diag_prbs_poly_enum_table[] = {
    {"X7_X6_1", KBP_PRBS_7,  "PRBs polynomial X7_X6_1"},
    {"X9_X5_1", KBP_PRBS_9,  "PRBs polynomial X9_X5_1"},
    {"X11_X9_1", KBP_PRBS_11, "PRBs polynomial X11_X9_1" },
    {"X15_X14_1", KBP_PRBS_15, "PRBs polynomial X15_X14_1"},
    {"X23_X18_1", KBP_PRBS_23, "PRBs polynomial X23_X18_1"},
    {"X31_X28_1", KBP_PRBS_31, "PRBs polynomial X31_X28_1"},
    {"X58_X31_1", KBP_PRBS_58, "PRBs polynomial X58_X31_1"},
    {"0", KBP_PRBS_7,  "PRBs polynomial X7_X6_1"},
    {"1", KBP_PRBS_9,  "PRBs polynomial X9_X5_1"},
    {"2", KBP_PRBS_11, "PRBs polynomial X11_X9_1" },
    {"3", KBP_PRBS_15, "PRBs polynomial X15_X14_1"},
    {"4", KBP_PRBS_23, "PRBs polynomial X23_X18_1"},
    {"5", KBP_PRBS_31, "PRBs polynomial X31_X28_1"},
    {"6", KBP_PRBS_58, "PRBs polynomial X58_X31_1"},
    {NULL}
};
static sh_sand_option_t dnx_kbp_diag_prbs_set_options[] = {
    {"POLYnomial", SAL_FIELD_TYPE_ENUM, "PRBS poly", "5", (void *) dnx_kbp_diag_prbs_poly_enum_table},
    {"Lane", SAL_FIELD_TYPE_UINT32, "Lane Bitmap", "1"},
    {NULL}
};
static sh_sand_option_t dnx_kbp_diag_prbs_get_options[] = {
    {"Lane", SAL_FIELD_TYPE_UINT32, "Lane Bitmap", "1"},
    {NULL}
};
static sh_sand_option_t dnx_kbp_diag_prbs_clear_options[] = {
    {"POLYnomial", SAL_FIELD_TYPE_ENUM, "PRBS poly", "5", (void *) dnx_kbp_diag_prbs_poly_enum_table},
    {"Lane", SAL_FIELD_TYPE_UINT32, "Lane Bitmap", "1"},
    {NULL}
};
static sh_sand_option_t dnx_kbp_diag_intf_info_options[] = {
    {"Lane", SAL_FIELD_TYPE_UINT32, "Lane Bitmap", "1"},
    {NULL}
};

static sh_sand_cmd_t dnx_kbp_diag_device_cmds[] = {
    {"PRinT",   dnx_kbp_diag_device_print_cmd,   NULL,   dnx_kbp_diag_device_options,      &dnx_kbp_diag_device_print_man},
    {"DuMP",    dnx_kbp_diag_device_dump_cmd,    NULL,   dnx_kbp_diag_device_options,      &dnx_kbp_diag_device_dump_man},
    {"SWSTate", dnx_kbp_diag_device_swstate_cmd, NULL,   dnx_kbp_diag_device_options,      &dnx_kbp_diag_device_swstate_man},
    {"LAST",    dnx_kbp_diag_device_last_cmd,    NULL,   dnx_kbp_diag_device_options,      &dnx_kbp_diag_device_last_man, NULL, NULL, SH_CMD_CONDITIONAL, dnx_kbp_diag_interface_is_enabled},
    {NULL}
};

static sh_sand_cmd_t dnx_kbp_diag_opcode_cmds[] = {
    {"DuMP",    dnx_kbp_diag_opcode_dump_cmd,    NULL,   dnx_kbp_diag_opcode_dump_options, &dnx_kbp_diag_opcode_dump_man},
    {"LIST",    dnx_kbp_diag_opcode_list_cmd,    NULL,   NULL,                             &dnx_kbp_diag_opcode_list_man},
    {NULL}
};

static sh_sand_cmd_t dnx_kbp_diag_db_cmds[] = {
    {"DuMP"        , dnx_kbp_diag_db_dump_cmd,     NULL, NULL,                             &dnx_kbp_diag_db_dump_man},
    {"COUnter" , dnx_kbp_diag_db_counters_cmds, NULL, dnx_kbp_diag_db_counters_options, &dnx_kbp_diag_db_counters_man,NULL, NULL, SH_CMD_CONDITIONAL, dnx_kbp_diag_interface_is_enabled},
    {NULL}
};

static sh_sand_cmd_t dnx_kbp_diag_vis_cmds[] = {
    {"RESult",  dnx_kbp_diag_vis_result_cmd,     NULL,   NULL,                             &dnx_kbp_diag_vis_result_man},
    {NULL}
};

static sh_sand_cmd_t dnx_kbp_diag_prbs_cmds[] = {
    {"set",     dnx_kbp_diag_prbs_set_cmd,   NULL,   dnx_kbp_diag_prbs_set_options,   &dnx_kbp_diag_prbs_set_man},
    {"get",     dnx_kbp_diag_prbs_get_cmd,   NULL,   dnx_kbp_diag_prbs_get_options,   &dnx_kbp_diag_prbs_get_man},
    {"clear",   dnx_kbp_diag_prbs_clear_cmd, NULL,   dnx_kbp_diag_prbs_clear_options, &dnx_kbp_diag_prbs_clear_man},
    {NULL}
};
static sh_sand_cmd_t dnx_kbp_diag_intf_cmds[] = {
    {"info",    dnx_kbp_diag_intf_info_cmd,  NULL,   dnx_kbp_diag_intf_info_options,   &dnx_kbp_diag_intf_info_man},
    {NULL}
};
sh_sand_cmd_t sh_dnx_kbp_cmds[] = {
    {"DeViCe",     NULL,                       dnx_kbp_diag_device_cmds, NULL,                            &dnx_kbp_diag_device_man},
    {"VERSion",    dnx_kbp_diag_sdk_ver_cmd,   NULL,                     NULL,                            &dnx_kbp_diag_sdk_ver_man},
    {"OPcode",     NULL,                       dnx_kbp_diag_opcode_cmds, NULL,                            &dnx_kbp_diag_opcode_man, NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {"DB",         NULL,                       dnx_kbp_diag_db_cmds,     NULL,                            &dnx_kbp_diag_db_man, NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {"ROP",        dnx_kbp_diag_rop_cmd,       NULL,                     dnx_kbp_diag_rop_options,        &dnx_kbp_diag_rop_man},
    {"ROP_WRite",  dnx_kbp_diag_rop_write_cmd, NULL,                     dnx_kbp_diag_rop_write_options,  &dnx_kbp_diag_rop_write_man, NULL, NULL, SH_CMD_CONDITIONAL, dnx_kbp_diag_interface_is_not_op2_enabled},
    {"ROP_ReaD",   dnx_kbp_diag_rop_read_cmd,  NULL,                     dnx_kbp_diag_rop_read_options,   &dnx_kbp_diag_rop_read_man, NULL, NULL, SH_CMD_CONDITIONAL, dnx_kbp_diag_interface_is_not_op2_enabled},
    {"ROP_TeST",   dnx_kbp_diag_rop_test_cmd,  NULL,                     dnx_kbp_diag_rop_test_options,   &dnx_kbp_diag_rop_test_man, NULL, NULL, SH_CMD_CONDITIONAL, dnx_kbp_diag_interface_is_not_op2_enabled},
    {"StaTuS",     dnx_kbp_diag_status_cmd,    NULL,                     dnx_kbp_diag_status_options,     &dnx_kbp_diag_status_man, NULL, NULL, SH_CMD_CONDITIONAL, dnx_kbp_diag_interface_is_enabled},
    {"PRBS",       NULL,                       dnx_kbp_diag_prbs_cmds,   NULL,                            &dnx_kbp_diag_prbs_man, NULL, NULL, SH_CMD_CONDITIONAL, dnx_kbp_diag_interface_is_enabled},
    {"EYEScan",    dnx_kbp_diag_eyescan_cmd,   NULL,                     NULL,                            &dnx_kbp_diag_eyescan_man, NULL, NULL, SH_CMD_CONDITIONAL, dnx_kbp_diag_interface_is_enabled},
    {"INTerFace",  NULL,                       dnx_kbp_diag_intf_cmds,   NULL,                            &dnx_kbp_diag_intf_man, NULL, NULL, SH_CMD_CONDITIONAL, dnx_kbp_diag_interface_is_enabled},
    {"VISibility", NULL,                       dnx_kbp_diag_vis_cmds,    NULL,                            &dnx_kbp_diag_vis_man,  NULL, NULL, SH_CMD_CONDITIONAL, dnx_kbp_diag_interface_is_enabled},
    {"Search",     dnx_kbp_diag_search_cmds,   NULL,                     dnx_kbp_diag_search_options,     &dnx_kbp_diag_search_man, dnx_kbp_diag_search_cmd_field_get, NULL, SH_CMD_CONDITIONAL, dnx_kbp_diag_interface_is_enabled},
    {"SYNC",       dnx_kbp_diag_sync_cmd,      NULL,                     NULL,                            &dnx_kbp_diag_sync_man},
    {NULL}
};

/* *INDENT-ON* */

/*
 * }
 */
#else

/** This is DUMMY code. It is only for compilers that do not accept empty files and is never to be used. */
/* *INDENT-OFF* */
void dnx_kbp_diag_dummy_empty_function_to_make_compiler_happy() { return; }
/* *INDENT-ON* */

#endif /** defined(INCLUDE_KBP) */
