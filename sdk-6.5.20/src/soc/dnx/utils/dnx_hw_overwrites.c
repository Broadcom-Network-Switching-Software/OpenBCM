
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *  All Rights Reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif 
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TEMPLATEMNGR


#include <bcm/types.h>
#include <soc/drv.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_hw_overwrites.h>
#include <soc/dnx/adapter/adapter_reg_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/cmicx.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnxc/dnxc_ha.h>
#include <bcm_int/dnx/rx/rx.h>
#include <soc/dnx/swstate/auto_generated/access/l2_access.h>

#define FIELD_0_5_DEFAULT 61
#define FIELD_6_6_DEFAULT 1
#define FIELD_7_11_DEFAULT DBAL_ENUM_FVAL_LAYER_TYPES_MPLS
#define FIELD_12_16_DEFAULT DBAL_ENUM_FVAL_LAYER_TYPES_IPV4
#define FIELD_7_16_DEFAULT (FIELD_7_11_DEFAULT) | (FIELD_12_16_DEFAULT << 5)
#define FIELD_17_24_DEFAULT 0xFF
#define FIELD_25_27_DEFAULT 6
#define FIELD_28_49_DEFAULT 0
#define FIELD_50_51_DEFAULT 2
#define FIELD_52_69_DEFAULT 0
#define FIELD_70_72_DEFAULT 4
#define FIELD_73_80_DEFAULT 1
#define FIELD_81_82_DEFAULT 3
#define FIELD_83_85_DEFAULT 0
#define FIELD_86_88_DEFAULT 4
#define FIELD_89_92_DEFAULT 7
#define FIELD_93_94_DEFAULT 2
#define FIELD_95_97_DEFAULT 0
#define FIELD_98_99_DEFAULT 3
#define FIELD_100_102_DEFAULT 0
#define IPPF_REG_03A0_FIELD_17_17_DEFAULT 0
#define VTT_ST_TWO_LE_SHARED_PDS_STAGE_SELECTORS_DEFAULT 2
#define ITPP_ITPP_GENERAL_CFG_PD_CUD_INVALID_VALUE_DEFAULT 0x3ffffe
#define ITPPD_ITPP_GENERAL_CFG_PD_CUD_INVALID_VALUE_DEFAULT 0x3ffffe

shr_error_e
dnx_hw_overwrite_init(
    int unit)
{
    uint32 data[SOC_REG_ABOVE_64_MAX_SIZE_U32] = { 0 };
    uint32 field_data[SOC_REG_ABOVE_64_MAX_SIZE_U32];
    uint32 reg_val;
    uint64 reg64;
    int system_headers_mode;
    uint32 udh_size = 0;
    int ii = 0, field_0 = 0, field_1 = 0;
    uint32 hw_context_id1, hw_context_id2, hw_context_id3;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    if (system_headers_mode == dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit))
    {
        reg_val = OAMP_REG_0127r;
        if (soc_is(unit, J2P_DEVICE))
        {
            reg_val = OAMP_REG_0128r;
        }
        SHR_IF_ERR_EXIT(soc_reg_get(unit, reg_val, _SHR_CORE_ALL, 0, &reg64));

        soc_reg64_field32_set(unit, reg_val, &reg64, FIELD_5_5f, 0);

        udh_size = BITS2BYTES(dnx_data_field.udh.field_class_id_size_0_get(unit));
        udh_size += BITS2BYTES(dnx_data_field.udh.field_class_id_size_1_get(unit));
        udh_size += BITS2BYTES(dnx_data_field.udh.field_class_id_size_2_get(unit));
        udh_size += BITS2BYTES(dnx_data_field.udh.field_class_id_size_3_get(unit));
        soc_reg64_field32_set(unit, reg_val, &reg64, FIELD_49_55f, udh_size);

        soc_reg64_field32_set(unit, reg_val, &reg64, FIELD_8_8f, 1);
        soc_reg64_field32_set(unit, reg_val, &reg64, FIELD_47_48f, 3);

        SHR_IF_ERR_EXIT(soc_reg_set(unit, reg_val, _SHR_CORE_ALL, 0, reg64));

        SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_FWD_CONTEXT_ID,
                                                      DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___NOP_AND_POP,
                                                      &hw_context_id1));
        SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                        (unit, DBAL_FIELD_FWD_CONTEXT_ID, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___DO_NOT_EDIT,
                         &hw_context_id2));
        SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                        (unit, DBAL_FIELD_FWD_CONTEXT_ID, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___NOP_AND_POP_2,
                         &hw_context_id3));
        for (ii = 0; ii < 64; ii++)
        {
            SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_FORWARDING_CONTEXT_SELECTION_CAMm, -1, ii, data));
            field_0 = soc_mem_field32_get(unit, ETPPC_FORWARDING_CONTEXT_SELECTION_CAMm, data, PROGRAMf);
            field_1 = soc_mem_field32_get(unit, ETPPC_FORWARDING_CONTEXT_SELECTION_CAMm, data, PHP_PERFORMEDf);
            if ((((field_0 >> 1) == hw_context_id1) || ((field_0 >> 1) == hw_context_id2)
                 || ((field_0 >> 1) == hw_context_id3)) && (field_1 == 1))
            {
                soc_mem_field32_set(unit, ETPPC_FORWARDING_CONTEXT_SELECTION_CAMm, data, PHP_PERFORMEDf, 0);
                soc_mem_field32_set(unit, ETPPC_FORWARDING_CONTEXT_SELECTION_CAMm, data,
                                    soc_is(unit, J2P_DEVICE) ? TERMINATION_CONTEXT_PROFILEf : TERMINATION_CONTEXTf, 0);
                soc_mem_field32_set(unit, ETPPC_FORWARDING_CONTEXT_SELECTION_CAMm, data,
                                    soc_is(unit,
                                           J2P_DEVICE) ? TERMINATION_CONTEXT_PROFILE_MASKf : TERMINATION_CONTEXT_MASKf,
                                    0x3f);
                SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_FORWARDING_CONTEXT_SELECTION_CAMm, -1, ii, data));
            }
        }
    }

    {
        int ii = 0, field_0 = 0;
        uint32 hw_context_id;

        SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_TRAP_CONTEXT_ID,
                                                      DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___VISIBILITY,
                                                      &hw_context_id));

        for (ii = 0; ii < 48; ii++)
        {
            SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_TRAP_CONTEXT_SELECTION_CAMm, -1, ii, data));
            field_0 = soc_mem_field32_get(unit, ETPPB_TRAP_CONTEXT_SELECTION_CAMm, data, PROGRAMf);
            if ((field_0 >> 1) == hw_context_id)
            {
                soc_mem_field32_set(unit, ETPPB_TRAP_CONTEXT_SELECTION_CAMm, data, VALIDf, 0);
                SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_TRAP_CONTEXT_SELECTION_CAMm, -1, ii, data));
            }
        }
    }

    {
        int ii = 0, field_0 = 0;
        uint32 hw_context_id;

        SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_TRAP_CONTEXT_ID,
                                                      DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___CPU_TRAPPED_KEEP_ORIGINAL,
                                                      &hw_context_id));

        for (ii = 0; ii < 48; ii++)
        {
            SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_TRAP_CONTEXT_SELECTION_CAMm, -1, ii, data));
            field_0 = soc_mem_field32_get(unit, ETPPB_TRAP_CONTEXT_SELECTION_CAMm, data, PROGRAMf);
            if ((field_0 >> 1) == hw_context_id)
            {
                soc_mem_field32_set(unit, ETPPB_TRAP_CONTEXT_SELECTION_CAMm, data, VALIDf, 0);
                soc_mem_field32_set(unit, ETPPB_TRAP_CONTEXT_SELECTION_CAMm, data, FHEI_TYPE_MASKf, 0xf);
                soc_mem_field32_set(unit, ETPPB_TRAP_CONTEXT_SELECTION_CAMm, data, FHEI_TYPEf, 0);
                SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_TRAP_CONTEXT_SELECTION_CAMm, -1, ii, data));
            }
        }
    }

    {
        int ipv4uc_counter = 0;
        int ipv4mc_counter = 0;
        int ipv6uc_counter = 0;
        int ipv6mc_counter = 0;
        int mpls_counter = 0;
        uint32 context_field = 0;
        uint32 dummy_field = 0;
        uint32 layer_type_field = 0;
        int ii = 0, jj = 0;
        uint8 is_init;
        dbal_enum_value_field_fwd1_context_id_e ipv4uc_context_list[5] = {
            DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_GENERAL___NOP_CTX_MACT_SA_LKP,
            DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC_MACT_SA_LKP,
            DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_UC_MACT_SA_LKP,
            DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC_W_OPTIONS_MACT_SA_LKP,
            DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_UC_W_OPTIONS_MACT_SA_LKP
        };
        dbal_enum_value_field_fwd1_context_id_e ipv4mc_context_list[2] = {
            DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___MC_PRIVATE_PUBLIC_MACT_SA_LKP,
            DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___MC_PRIVATE_MACT_SA_LKP
        };
        dbal_enum_value_field_fwd1_context_id_e ipv6uc_context_list[3] = {
            DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_GENERAL___NOP_CTX_MACT_SA_LKP,
            DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___PRIVATE_PUBLIC_OPTIMIZED_UC_MACT_SA_LKP,
            DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___PRIVATE_UC_MACT_SA_LKP
        };
        dbal_enum_value_field_fwd1_context_id_e ipv6mc_context_list[2] = {
            DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___MC_PRIVATE_MACT_SA_LKP,
            DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___MC_PRIVATE_PUBLIC_MACT_SA_LKP
        };
        dbal_enum_value_field_fwd1_context_id_e mpls_context_list[1] = {
            DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_MPLS___FWD_MACT_SA_LKP
        };

        SHR_IF_ERR_EXIT(l2_db_context.is_init(unit, &is_init));
        if (!is_init)
        {
            l2_db_context.init(unit);
        }

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VT5_CONTEXT_SELECTION, &entry_handle_id));

        for (ii = 0; ii < 128; ii++)
        {
            int rv = 0;
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_VT5_CONTEXT_SELECTION, entry_handle_id));
            SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, ii));
            rv = dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS);
            if (rv == _SHR_E_NOT_FOUND)
            {
                continue;
            }
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_FWD1_CONTEXT_ID, 0, &context_field));
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_FORWARDING_DOMAIN_IS_ZERO, &dummy_field));
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE_CURRENT_LAYER, &layer_type_field));

            if (dummy_field == 1)
            {
                uint32 field_data1[SOC_REG_ABOVE_64_MAX_SIZE_U32];
                
                dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_FORWARDING_DOMAIN_IS_ZERO, 0, 0);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_VALID, 0, 1);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD1_CONTEXT_ID, 0, context_field);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NASID_VALID, 0, 1);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NASID, 0, 6);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));

                
                
                sal_memset(data, 0, sizeof(data));
                data[0] = 2;
                SHR_IF_ERR_EXIT(soc_mem_write(unit, IPPA_VTECS_TCAM_BANK_COMMANDm, -1, 2 * ii + 1, data));
                sal_memset(data, 0, sizeof(data));
                sal_memset(field_data, 0, sizeof(field_data));
                SHR_IF_ERR_EXIT(soc_mem_read(unit, IPPA_VTECS_TCAM_BANK_REPLYm, -1, 2 * ii + 1, data));
                soc_mem_field_get(unit, IPPA_VTECS_TCAM_BANK_REPLYm, data, VTECS_TCAM_CPU_REP_DOUTf, field_data);
                
                sal_memset(data, 0, sizeof(data));
                data[0] = 2;
                SHR_IF_ERR_EXIT(soc_mem_write(unit, IPPA_VTECS_TCAM_BANK_COMMANDm, -1, 2 * ii, data));
                sal_memset(data, 0, sizeof(data));
                sal_memset(field_data1, 0, sizeof(field_data1));
                SHR_IF_ERR_EXIT(soc_mem_read(unit, IPPA_VTECS_TCAM_BANK_REPLYm, -1, 2 * ii, data));
                soc_mem_field_get(unit, IPPA_VTECS_TCAM_BANK_REPLYm, data, VTECS_TCAM_CPU_REP_DOUTf, field_data1);
                
                sal_memset(data, 0, sizeof(data));
                soc_mem_field32_set(unit, IPPA_VTECS_TCAM_BANK_COMMANDm, data, VTECS_TCAM_CPU_CMD_WRf, 1);
                soc_mem_field32_set(unit, IPPA_VTECS_TCAM_BANK_COMMANDm, data, VTECS_TCAM_CPU_CMD_VALIDSf, 0);
                soc_mem_field_set(unit, IPPA_VTECS_TCAM_BANK_COMMANDm, data, VTECS_TCAM_CPU_CMD_KEYf, field_data1);
                soc_mem_field_set(unit, IPPA_VTECS_TCAM_BANK_COMMANDm, data, VTECS_TCAM_CPU_CMD_MASKf, field_data);
                SHR_IF_ERR_EXIT(soc_mem_write(unit, IPPA_VTECS_TCAM_BANK_COMMANDm, -1, 2 * ii, data));

                
                {
                    int add_to_list = 0;
                    for (jj = 0; jj < 5; jj++)
                    {
                        if (context_field == ipv4uc_context_list[jj])
                        {
                            if (ipv4uc_context_list[jj] == DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_GENERAL___NOP_CTX_MACT_SA_LKP)
                            {
                                if (layer_type_field == DBAL_ENUM_FVAL_LAYER_TYPES_IPV4)
                                {
                                    add_to_list = 1;
                                }
                            }
                            else
                            {
                                add_to_list = 1;
                            }
                        }
                        if (add_to_list)
                        {
                            SHR_IF_ERR_EXIT(l2_db_context.opportunistic_learning_ctx.
                                            ipv4uc.set(unit, ipv4uc_counter++, ii));
                            break;
                        }
                    }
                    SHR_IF_ERR_EXIT(l2_db_context.opportunistic_learning_ctx.ipv4uc_counter.set(unit, ipv4uc_counter));
                }
                
                {
                    for (jj = 0; jj < 2; jj++)
                    {
                        if (context_field == ipv4mc_context_list[jj])
                        {
                            SHR_IF_ERR_EXIT(l2_db_context.opportunistic_learning_ctx.
                                            ipv4mc.set(unit, ipv4mc_counter++, ii));
                            break;
                        }
                    }
                    SHR_IF_ERR_EXIT(l2_db_context.opportunistic_learning_ctx.ipv4mc_counter.set(unit, ipv4mc_counter));
                }
                
                {
                    int add_to_list = 0;
                    for (jj = 0; jj < 3; jj++)
                    {
                        if (context_field == ipv6uc_context_list[jj])
                        {
                            if (ipv6uc_context_list[jj] == DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_GENERAL___NOP_CTX_MACT_SA_LKP)
                            {
                                if (layer_type_field == DBAL_ENUM_FVAL_LAYER_TYPES_IPV6)
                                {
                                    add_to_list = 1;
                                }
                            }
                            else
                            {
                                add_to_list = 1;
                            }
                        }
                        if (add_to_list)
                        {
                            SHR_IF_ERR_EXIT(l2_db_context.opportunistic_learning_ctx.
                                            ipv6uc.set(unit, ipv6uc_counter++, ii));
                            break;
                        }
                    }
                    SHR_IF_ERR_EXIT(l2_db_context.opportunistic_learning_ctx.ipv6uc_counter.set(unit, ipv6uc_counter));
                }
                
                {
                    for (jj = 0; jj < 2; jj++)
                    {
                        if (context_field == ipv6mc_context_list[jj])
                        {
                            SHR_IF_ERR_EXIT(l2_db_context.opportunistic_learning_ctx.
                                            ipv6mc.set(unit, ipv6mc_counter++, ii));
                            break;
                        }
                    }
                    SHR_IF_ERR_EXIT(l2_db_context.opportunistic_learning_ctx.ipv6mc_counter.set(unit, ipv6mc_counter));
                }
                
                {
                    for (jj = 0; jj < 1; jj++)
                    {
                        if (context_field == mpls_context_list[jj])
                        {
                            SHR_IF_ERR_EXIT(l2_db_context.opportunistic_learning_ctx.
                                            mpls.set(unit, mpls_counter++, ii));
                            break;
                        }
                    }
                    SHR_IF_ERR_EXIT(l2_db_context.opportunistic_learning_ctx.mpls_counter.set(unit, mpls_counter));
                }
            }
        }
    }

    sal_memset(field_data, 0, sizeof(field_data));
    sal_memset(data, 0x0, sizeof(data));

    reg_val = OAMP_REG_0129r;
    if (soc_is(unit, J2P_DEVICE))
    {
        reg_val = OAMP_REG_012Ar;
    }
    field_data[0] = FIELD_0_5_DEFAULT;
    soc_reg_above_64_field_set(unit, reg_val, data, FIELD_0_5f, field_data);

    field_data[0] = FIELD_6_6_DEFAULT;
    soc_reg_above_64_field_set(unit, reg_val, data, FIELD_6_6f, field_data);

    field_data[0] = FIELD_7_16_DEFAULT;
    soc_reg_above_64_field_set(unit, reg_val, data, FIELD_7_16f, field_data);

    field_data[0] = FIELD_17_24_DEFAULT;
    soc_reg_above_64_field_set(unit, reg_val, data, FIELD_17_24f, field_data);

    field_data[0] = FIELD_25_27_DEFAULT;
    soc_reg_above_64_field_set(unit, reg_val, data, FIELD_25_27f, field_data);

    field_data[0] = FIELD_28_49_DEFAULT;
    soc_reg_above_64_field_set(unit, reg_val, data, FIELD_28_49f, field_data);

    field_data[0] = FIELD_50_51_DEFAULT;
    soc_reg_above_64_field_set(unit, reg_val, data, FIELD_50_51f, field_data);

    field_data[0] = FIELD_52_69_DEFAULT;
    soc_reg_above_64_field_set(unit, reg_val, data, FIELD_52_69f, field_data);

    field_data[0] = FIELD_70_72_DEFAULT;
    soc_reg_above_64_field_set(unit, reg_val, data, FIELD_70_72f, field_data);

    field_data[0] = FIELD_73_80_DEFAULT;
    soc_reg_above_64_field_set(unit, reg_val, data, FIELD_73_80f, field_data);

    field_data[0] = FIELD_81_82_DEFAULT;
    soc_reg_above_64_field_set(unit, reg_val, data, FIELD_81_82f, field_data);

    field_data[0] = FIELD_83_85_DEFAULT;
    soc_reg_above_64_field_set(unit, reg_val, data, FIELD_83_85f, field_data);

    field_data[0] = FIELD_86_88_DEFAULT;
    soc_reg_above_64_field_set(unit, reg_val, data, FIELD_86_88f, field_data);

    field_data[0] = FIELD_89_92_DEFAULT;
    soc_reg_above_64_field_set(unit, reg_val, data, FIELD_89_92f, field_data);

    field_data[0] = FIELD_93_94_DEFAULT;
    soc_reg_above_64_field_set(unit, reg_val, data, FIELD_93_94f, field_data);

    field_data[0] = FIELD_95_97_DEFAULT;
    soc_reg_above_64_field_set(unit, reg_val, data, FIELD_95_97f, field_data);

    field_data[0] = FIELD_98_99_DEFAULT;
    soc_reg_above_64_field_set(unit, reg_val, data, FIELD_98_99f, field_data);

    field_data[0] = FIELD_100_102_DEFAULT;
    soc_reg_above_64_field_set(unit, reg_val, data, FIELD_100_102f, field_data);

    if (soc_is(unit, J2P_DEVICE))
    {
        SHR_IF_ERR_EXIT(WRITE_OAMP_REG_012Ar(unit, data));
    }
    else
    {
        SHR_IF_ERR_EXIT(WRITE_OAMP_REG_0129r(unit, data));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
