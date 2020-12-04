
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_OAM

#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <src/bcm/dnx/oam/oam_internal.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>

#define ING_OAM_LIF_NULL  DBAL_ENUM_FVAL_OAM_ING_KEY_BASE_SELECT_NULL

#define ING_OAM_LIF       DBAL_ENUM_FVAL_OAM_ING_KEY_BASE_SELECT_OAM_LIF

#define ING_OAM_LIF_3     DBAL_ENUM_FVAL_OAM_ING_KEY_BASE_SELECT_OAM_LIF_3

#define ING_OAM_LIF_2     DBAL_ENUM_FVAL_OAM_ING_KEY_BASE_SELECT_OAM_LIF_2

#define ING_OAM_LIF_1     DBAL_ENUM_FVAL_OAM_ING_KEY_BASE_SELECT_OAM_LIF_1

#define EG_OAM_LIF_NULL  DBAL_ENUM_FVAL_OAM_EG_KEY_BASE_SELECT_NULL

#define EG_OAM_LIF       DBAL_ENUM_FVAL_OAM_EG_KEY_BASE_SELECT_OAM_LIF

#define EG_OAM_LIF_3     DBAL_ENUM_FVAL_OAM_EG_KEY_BASE_SELECT_OAM_LIF_3

#define EG_OAM_LIF_2     DBAL_ENUM_FVAL_OAM_EG_KEY_BASE_SELECT_OAM_LIF_2

#define EG_OAM_LIF_1     DBAL_ENUM_FVAL_OAM_EG_KEY_BASE_SELECT_OAM_LIF_1

#define PREFIX_HIGH   DBAL_ENUM_FVAL_OAM_KEY_PREFIX_HIGH

#define PREFIX_MID    DBAL_ENUM_FVAL_OAM_KEY_PREFIX_MID

#define PREFIX_LOW    DBAL_ENUM_FVAL_OAM_KEY_PREFIX_LOW

#define PREFIX_BFD    DBAL_ENUM_FVAL_OAM_KEY_PREFIX_BFD

typedef struct oam_key_select_ingress_table_entry_s
{

    int nof_valid_lm_lifs;

    int in_lif_valid_for_lm;

    int your_disc_valid;

    dbal_enum_value_field_oam_ing_key_base_select_e oam_key_base_select_1;
    dbal_enum_value_field_oam_ing_key_base_select_e oam_key_base_select_2;
    dbal_enum_value_field_oam_ing_key_base_select_e oam_key_base_select_3;

    int oam_key_prefix_1;
    int oam_key_prefix_2;
    int oam_key_prefix_3;
    int mp_profile_sel;
} oam_key_select_ingress_table_entry_t;

typedef struct oam_key_select_egress_table_entry_s
{

    int nof_valid_lm_lifs;

    int packet_is_oam;

    int oam_inject;

    int counter_pointer_valid;

    dbal_enum_value_field_oam_eg_key_base_select_e oam_key_base_select_1;
    dbal_enum_value_field_oam_eg_key_base_select_e oam_key_base_select_2;
    dbal_enum_value_field_oam_eg_key_base_select_e oam_key_base_select_3;

    int oam_key_prefix_1;
    int oam_key_prefix_2;
    int oam_key_prefix_3;
    int mp_profile_sel;
} oam_key_select_egress_table_entry_t;

typedef struct oam_key_select_wo_hlm_ingress_table_entry_s
{

    int nof_valid_lm_lifs;

    int in_lif_valid_for_lm;

    int your_disc_valid;

    dbal_enum_value_field_oam_ing_key_base_select_e oam_key_base_select_1;

    int oam_key_prefix_1;
    int mp_profile_sel;
} oam_key_select_wo_hlm_ingress_table_entry_t;

typedef struct oam_key_select_wo_hlm_egress_table_entry_s
{

    int nof_valid_lm_lifs;

    int packet_is_oam;

    int oam_inject;

    int counter_pointer_valid;

    dbal_enum_value_field_oam_eg_key_base_select_e oam_key_base_select_1;

    int oam_key_prefix_1;
    int mp_profile_sel;
} oam_key_select_wo_hlm_egress_table_entry_t;

#ifdef BCM_DNX2_SUPPORT

static const oam_key_select_ingress_table_entry_t oam_key_select_ingress_map[] = {

    {0, 0, 0, ING_OAM_LIF, ING_OAM_LIF_NULL, ING_OAM_LIF_NULL, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},

    {0, 0, 1, ING_OAM_LIF, ING_OAM_LIF_NULL, ING_OAM_LIF_NULL, PREFIX_BFD, PREFIX_HIGH, PREFIX_HIGH, 1},

    {1, 0, 0, ING_OAM_LIF, ING_OAM_LIF_1, ING_OAM_LIF_1, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},

    {1, 0, 1, ING_OAM_LIF, ING_OAM_LIF_1, ING_OAM_LIF_1, PREFIX_BFD, PREFIX_HIGH, PREFIX_MID, 1},

    {1, 1, 0, ING_OAM_LIF, ING_OAM_LIF, ING_OAM_LIF, PREFIX_HIGH, PREFIX_MID, PREFIX_LOW, 1},

    {2, 0, 0, ING_OAM_LIF, ING_OAM_LIF_1, ING_OAM_LIF_2, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},

    {2, 0, 1, ING_OAM_LIF, ING_OAM_LIF_1, ING_OAM_LIF_1, PREFIX_BFD, PREFIX_HIGH, PREFIX_HIGH, 1},

    {2, 1, 0, ING_OAM_LIF, ING_OAM_LIF_2, ING_OAM_LIF_3, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},

    {3, 0, 0, ING_OAM_LIF_1, ING_OAM_LIF_2, ING_OAM_LIF_3, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 0},

    {3, 0, 1, ING_OAM_LIF, ING_OAM_LIF_1, ING_OAM_LIF_2, PREFIX_BFD, PREFIX_HIGH, PREFIX_HIGH, 1},

    {3, 1, 0, ING_OAM_LIF, ING_OAM_LIF_2, ING_OAM_LIF_3, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
};

static const oam_key_select_egress_table_entry_t oam_key_select_egress_map[] = {

    {0, 0, 0, 0, EG_OAM_LIF, EG_OAM_LIF_NULL, EG_OAM_LIF_NULL, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {0, 0, 0, 1, EG_OAM_LIF, EG_OAM_LIF_NULL, EG_OAM_LIF_NULL, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {0, 0, 1, 0, EG_OAM_LIF, EG_OAM_LIF_NULL, EG_OAM_LIF_NULL, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {0, 0, 1, 1, EG_OAM_LIF, EG_OAM_LIF_NULL, EG_OAM_LIF_NULL, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {0, 1, 0, 0, EG_OAM_LIF, EG_OAM_LIF_NULL, EG_OAM_LIF_NULL, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {0, 1, 0, 1, EG_OAM_LIF, EG_OAM_LIF_NULL, EG_OAM_LIF_NULL, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {0, 1, 1, 0, EG_OAM_LIF, EG_OAM_LIF_NULL, EG_OAM_LIF_NULL, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {0, 1, 1, 1, EG_OAM_LIF, EG_OAM_LIF_NULL, EG_OAM_LIF_NULL, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},

    {1, 0, 0, 1, EG_OAM_LIF, EG_OAM_LIF_1, EG_OAM_LIF_1, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},

    {1, 0, 0, 0, EG_OAM_LIF_1, EG_OAM_LIF_1, EG_OAM_LIF_1, PREFIX_HIGH, PREFIX_MID, PREFIX_LOW, 1},
    {1, 0, 1, 0, EG_OAM_LIF, EG_OAM_LIF_1, EG_OAM_LIF_1, PREFIX_HIGH, PREFIX_MID, PREFIX_LOW, 1},
    {1, 0, 1, 1, EG_OAM_LIF, EG_OAM_LIF_1, EG_OAM_LIF_1, PREFIX_HIGH, PREFIX_MID, PREFIX_LOW, 1},
    {1, 1, 0, 0, EG_OAM_LIF, EG_OAM_LIF_1, EG_OAM_LIF_1, PREFIX_HIGH, PREFIX_MID, PREFIX_LOW, 1},
    {1, 1, 0, 1, EG_OAM_LIF, EG_OAM_LIF_1, EG_OAM_LIF_1, PREFIX_HIGH, PREFIX_MID, PREFIX_LOW, 1},
    {1, 1, 1, 0, EG_OAM_LIF_1, EG_OAM_LIF_1, EG_OAM_LIF_1, PREFIX_HIGH, PREFIX_MID, PREFIX_LOW, 1},
    {1, 1, 1, 1, EG_OAM_LIF_1, EG_OAM_LIF_1, EG_OAM_LIF_1, PREFIX_HIGH, PREFIX_MID, PREFIX_LOW, 1},

    {2, 0, 0, 0, EG_OAM_LIF, EG_OAM_LIF_1, EG_OAM_LIF_2, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {2, 0, 0, 1, EG_OAM_LIF, EG_OAM_LIF_1, EG_OAM_LIF_2, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {2, 0, 1, 0, EG_OAM_LIF, EG_OAM_LIF_1, EG_OAM_LIF_2, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {2, 0, 1, 1, EG_OAM_LIF, EG_OAM_LIF_1, EG_OAM_LIF_2, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {2, 1, 0, 0, EG_OAM_LIF, EG_OAM_LIF_1, EG_OAM_LIF_2, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {2, 1, 0, 1, EG_OAM_LIF, EG_OAM_LIF_1, EG_OAM_LIF_2, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {2, 1, 1, 0, EG_OAM_LIF_1, EG_OAM_LIF_2, EG_OAM_LIF_NULL, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {2, 1, 1, 1, EG_OAM_LIF_1, EG_OAM_LIF_2, EG_OAM_LIF_NULL, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},

    {3, 0, 0, 0, EG_OAM_LIF_1, EG_OAM_LIF_2, EG_OAM_LIF_3, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {3, 0, 0, 1, EG_OAM_LIF_1, EG_OAM_LIF_2, EG_OAM_LIF_3, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},

    {3, 0, 1, 0, EG_OAM_LIF, EG_OAM_LIF_1, EG_OAM_LIF_2, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {3, 0, 1, 1, EG_OAM_LIF, EG_OAM_LIF_1, EG_OAM_LIF_2, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {3, 1, 0, 0, EG_OAM_LIF, EG_OAM_LIF_1, EG_OAM_LIF_2, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {3, 1, 0, 1, EG_OAM_LIF, EG_OAM_LIF_1, EG_OAM_LIF_2, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {3, 1, 1, 0, EG_OAM_LIF_1, EG_OAM_LIF_2, EG_OAM_LIF_3, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {3, 1, 1, 1, EG_OAM_LIF_1, EG_OAM_LIF_2, EG_OAM_LIF_3, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
};
#endif

shr_error_e
dnx_oam_key_select_ingress_init(
    int unit)
{
    uint32 entry_handle_id;
    int entry = 0;
    int nof_ingress_key_select_entries = 0;

    int nof_valid_lm_lifs = 0;
    int in_lif_valid_for_lm = 0;
    int your_disc_valid = 0;

    int oam_key_prefix_1 = 0;
    dbal_enum_value_field_oam_ing_key_base_select_e oam_key_base_select_1 = ING_OAM_LIF_NULL;
#ifdef BCM_DNX2_SUPPORT
    int oam_key_prefix_2 = 0;
    int oam_key_prefix_3 = 0;
    dbal_enum_value_field_oam_ing_key_base_select_e oam_key_base_select_2 = ING_OAM_LIF_NULL;
    dbal_enum_value_field_oam_ing_key_base_select_e oam_key_base_select_3 = ING_OAM_LIF_NULL;
    uint8 oam_hlm_support = 0;
#endif
    int mp_profile_sel = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

#ifdef BCM_DNX2_SUPPORT
    oam_hlm_support = dnx_data_oam.general.feature_get(unit, dnx_data_oam_general_oam_hlm_support);
    if (oam_hlm_support == 1)
    {
        nof_ingress_key_select_entries =
            sizeof(oam_key_select_ingress_map) / sizeof(oam_key_select_ingress_table_entry_t);
    }
    else
#endif
    {
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_OAM_KEY_SELECT, &entry_handle_id));

    for (entry = 0; entry < nof_ingress_key_select_entries; entry++)
    {

#ifdef BCM_DNX2_SUPPORT
        if (oam_hlm_support == 1)
        {
            nof_valid_lm_lifs = oam_key_select_ingress_map[entry].nof_valid_lm_lifs;
            in_lif_valid_for_lm = oam_key_select_ingress_map[entry].in_lif_valid_for_lm;
            your_disc_valid = oam_key_select_ingress_map[entry].your_disc_valid;

            oam_key_base_select_1 = oam_key_select_ingress_map[entry].oam_key_base_select_1;
            oam_key_prefix_1 = oam_key_select_ingress_map[entry].oam_key_prefix_1;
            oam_key_base_select_2 = oam_key_select_ingress_map[entry].oam_key_base_select_2;
            oam_key_base_select_3 = oam_key_select_ingress_map[entry].oam_key_base_select_3;
            oam_key_prefix_2 = oam_key_select_ingress_map[entry].oam_key_prefix_2;
            oam_key_prefix_3 = oam_key_select_ingress_map[entry].oam_key_prefix_3;
            mp_profile_sel = oam_key_select_ingress_map[entry].mp_profile_sel;
        }
        else
#endif
        {
        }

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_YOUR_DISC_VALID, your_disc_valid);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_VALID_FOR_LM, in_lif_valid_for_lm);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_VALID_LM_LIFS, nof_valid_lm_lifs);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ING_KEY_BASE_SELECT_1, INST_SINGLE,
                                     oam_key_base_select_1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX_1, INST_SINGLE, oam_key_prefix_1);
#ifdef BCM_DNX2_SUPPORT
        if (oam_hlm_support == 1)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ING_KEY_BASE_SELECT_2, INST_SINGLE,
                                         oam_key_base_select_2);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ING_KEY_BASE_SELECT_3, INST_SINGLE,
                                         oam_key_base_select_3);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX_2, INST_SINGLE,
                                         oam_key_prefix_2);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX_3, INST_SINGLE,
                                         oam_key_prefix_3);
        }
#endif
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MP_PROFILE_SEL, INST_SINGLE, mp_profile_sel);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_INGRESS_OAM_KEY_SELECT, entry_handle_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_key_select_egress_init(
    int unit)
{
    uint32 entry_handle_id;
    int entry = 0;
    int nof_egress_key_select_entries = 0;

    int packet_is_oam = 0;
    int counter_pointer_valid = 0;
    int nof_valid_lm_lifs = 0;
    int oam_inject = 0;

    int mp_profile_sel = 0;
    dbal_enum_value_field_oam_eg_key_base_select_e oam_key_base_select_1 = EG_OAM_LIF_NULL;
#ifdef BCM_DNX2_SUPPORT
    int oam_key_prefix_1 = 0;
    int oam_key_prefix_2 = 0;
    int oam_key_prefix_3 = 0;
    dbal_enum_value_field_oam_eg_key_base_select_e oam_key_base_select_2 = EG_OAM_LIF_NULL;
    dbal_enum_value_field_oam_eg_key_base_select_e oam_key_base_select_3 = EG_OAM_LIF_NULL;
    uint8 oam_hlm_support = 0;
#endif

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

#ifdef BCM_DNX2_SUPPORT
    oam_hlm_support = dnx_data_oam.general.feature_get(unit, dnx_data_oam_general_oam_hlm_support);
    if (oam_hlm_support == 1)
    {
        nof_egress_key_select_entries = sizeof(oam_key_select_egress_map) / sizeof(oam_key_select_egress_table_entry_t);
    }
    else
#endif
    {
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_OAM_KEY_SELECT, &entry_handle_id));

    for (entry = 0; entry < nof_egress_key_select_entries; entry++)
    {

#ifdef BCM_DNX2_SUPPORT
        if (oam_hlm_support == 1)
        {
            oam_inject = oam_key_select_egress_map[entry].oam_inject;
            nof_valid_lm_lifs = oam_key_select_egress_map[entry].nof_valid_lm_lifs;
            counter_pointer_valid = oam_key_select_egress_map[entry].counter_pointer_valid;
            packet_is_oam = oam_key_select_egress_map[entry].packet_is_oam;

            oam_key_base_select_1 = oam_key_select_egress_map[entry].oam_key_base_select_1;
            oam_key_base_select_2 = oam_key_select_egress_map[entry].oam_key_base_select_2;
            oam_key_base_select_3 = oam_key_select_egress_map[entry].oam_key_base_select_3;
            oam_key_prefix_1 = oam_key_select_egress_map[entry].oam_key_prefix_1;
            oam_key_prefix_2 = oam_key_select_egress_map[entry].oam_key_prefix_2;
            oam_key_prefix_3 = oam_key_select_egress_map[entry].oam_key_prefix_3;
            mp_profile_sel = oam_key_select_egress_map[entry].mp_profile_sel;
        }
        else
#endif
        {
        }

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PACKET_IS_OAM, packet_is_oam);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER_PTR_VALID, counter_pointer_valid);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_VALID_LM_LIFS, nof_valid_lm_lifs);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_INJECT, oam_inject);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_EG_KEY_BASE_SELECT_1, INST_SINGLE,
                                     oam_key_base_select_1);
#ifdef BCM_DNX2_SUPPORT
        if (oam_hlm_support == 1)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_EG_KEY_BASE_SELECT_2, INST_SINGLE,
                                         oam_key_base_select_2);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_EG_KEY_BASE_SELECT_3, INST_SINGLE,
                                         oam_key_base_select_3);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX_1, INST_SINGLE,
                                         oam_key_prefix_1);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX_2, INST_SINGLE,
                                         oam_key_prefix_2);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX_3, INST_SINGLE,
                                         oam_key_prefix_3);
        }
#endif
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MP_PROFILE_SEL, INST_SINGLE, mp_profile_sel);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EGRESS_OAM_KEY_SELECT, entry_handle_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_key_select_ingress_get(
    int unit,
    int nof_valid_lm_lifs,
    int in_lif_valid_for_lm,
    int your_disc_valid,
    uint32 *key_pre_1,
    uint32 *key_pre_2,
    uint32 *key_pre_3,
    uint32 *key_base_1,
    uint32 *key_base_2,
    uint32 *key_base_3)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_OAM_KEY_SELECT, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_VALID_LM_LIFS, nof_valid_lm_lifs);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_VALID_FOR_LM, in_lif_valid_for_lm);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_YOUR_DISC_VALID, your_disc_valid);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX_1, INST_SINGLE, key_pre_1);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX_2, INST_SINGLE, key_pre_2);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX_3, INST_SINGLE, key_pre_3);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_ING_KEY_BASE_SELECT_1, INST_SINGLE, key_base_1);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_ING_KEY_BASE_SELECT_2, INST_SINGLE, key_base_2);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_ING_KEY_BASE_SELECT_3, INST_SINGLE, key_base_3);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_key_select_egress_get(
    int unit,
    int nof_valid_lm_lifs,
    int counter_ptr_valid,
    int oam_inject,
    int packet_is_oam,
    uint32 *key_pre_1,
    uint32 *key_pre_2,
    uint32 *key_pre_3,
    uint32 *key_base_1,
    uint32 *key_base_2,
    uint32 *key_base_3)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_OAM_KEY_SELECT, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_VALID_LM_LIFS, nof_valid_lm_lifs);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_INJECT, oam_inject);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER_PTR_VALID, counter_ptr_valid);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PACKET_IS_OAM, packet_is_oam);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX_1, INST_SINGLE, key_pre_1);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX_2, INST_SINGLE, key_pre_2);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX_3, INST_SINGLE, key_pre_3);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_EG_KEY_BASE_SELECT_1, INST_SINGLE, key_base_1);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_EG_KEY_BASE_SELECT_2, INST_SINGLE, key_base_2);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_EG_KEY_BASE_SELECT_3, INST_SINGLE, key_base_3);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
