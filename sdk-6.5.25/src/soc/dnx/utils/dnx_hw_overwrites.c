
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *  All Rights Reserved.
 *
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TEMPLATEMNGR

#include <bcm/types.h>
#include <soc/drv.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_hw_overwrites.h>
#include <soc/dnxc/dnxc_adapter_reg_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/cmicx.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnxc/dnxc_ha.h>
#include <bcm_int/dnx/rx/rx.h>
#include <soc/dnx/swstate/auto_generated/access/l2_access.h>
#include <soc/dnx/swstate/auto_generated/access/switch_access.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>

#define FIELD_0_5_DEFAULT       61
#define FIELD_6_6_DEFAULT       1
#define FIELD_7_11_DEFAULT      DBAL_ENUM_FVAL_LAYER_TYPES_MPLS
#define FIELD_12_16_DEFAULT     DBAL_ENUM_FVAL_LAYER_TYPES_IPV4
#define FIELD_7_16_DEFAULT      ((FIELD_7_11_DEFAULT) | (FIELD_12_16_DEFAULT << 5))
#define FIELD_17_24_DEFAULT     0xFF
#define FIELD_25_27_DEFAULT     6
#define FIELD_28_49_DEFAULT     0
#define FIELD_50_51_DEFAULT     2
#define FIELD_52_69_DEFAULT     0
#define FIELD_70_72_DEFAULT     4
#define FIELD_73_80_DEFAULT     1
#define FIELD_81_82_DEFAULT     3
#define FIELD_83_85_DEFAULT     0
#define FIELD_86_88_DEFAULT     4
#define FIELD_89_92_DEFAULT     7
#define FIELD_93_94_DEFAULT     2
#define FIELD_95_97_DEFAULT     0
#define FIELD_98_99_DEFAULT     3
#define FIELD_100_102_DEFAULT   0

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

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    if (system_headers_mode == dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit))
    {
        reg_val = OAMP_REG_0127r;
        if (soc_is(unit, J2P_DEVICE) || soc_is(unit, J2X_DEVICE))
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
    }

    sal_memset(field_data, 0, sizeof(field_data));
    sal_memset(data, 0x0, sizeof(data));

    reg_val = OAMP_REG_0129r;
    if (soc_is(unit, J2P_DEVICE) || soc_is(unit, J2X_DEVICE))
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

    if (soc_is(unit, J2P_DEVICE) || soc_is(unit, J2X_DEVICE))
    {
        SHR_IF_ERR_EXIT(WRITE_OAMP_REG_012Ar(unit, data));
    }
    else
    {
        SHR_IF_ERR_EXIT(WRITE_OAMP_REG_0129r(unit, data));
    }

    if (!dnx_data_l2.feature.feature_get(unit, dnx_data_l2_feature_learn_payload_native_update_enable))
    {
        dbal_enum_value_field_vt5_context_id_e native_eth_contexts[] =
            { DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_0_VTAG_NWK_SCOPED,
            DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_1_VTAG_NWK_SCOPED,
            DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_2_VTAG_NWK_SCOPED,
            DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_0_VTAG_LIF_SCOPED,
            DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_1_VTAG_LIF_SCOPED,
            DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_2_VTAG_LIF_SCOPED
        };
        int ctx_idx;
        uint32 entry_handle_id;
        int nof_ctx = sizeof(native_eth_contexts) / sizeof(dbal_enum_value_field_vt5_context_id_e);

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_IRPP_VT5_CONTEXT_PROPERTIES, &entry_handle_id));
        for (ctx_idx = 0; ctx_idx < nof_ctx; ctx_idx++)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VT5_CONTEXT_ID, native_eth_contexts[ctx_idx]);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIF_LERAN_PAYLOAD_ENABLE, INST_SINGLE,
                                         FALSE);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_INGRESS_IRPP_VT5_CONTEXT_PROPERTIES, entry_handle_id));
        }
    }

#ifdef ADAPTER_SERVER_MODE
    if (!dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_egress_estimated_bta_btr_config))
    {
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ETPPA_RESERVED_SPARE_0r, REG_PORT_ANY, 0, 0));
    }
#endif

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
