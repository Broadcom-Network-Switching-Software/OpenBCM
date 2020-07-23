/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        field.c
 * Purpose:     Field Processor routines Specific to Helix5.
 */

#include <soc/defs.h>

#if defined (BCM_HELIX5_SUPPORT) && defined(BCM_FIELD_SUPPORT)

#include <bcm/error.h>
#include <bcm/field.h>
#include <shared/bsl.h>

#include <bcm_int/esw/field.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/helix5.h>
#include <bcm_int/esw/keygen_api.h>
#include <bcm_int/esw/flowtracker/ft_interface.h>
#if defined(BCM_FIREBOLT6_SUPPORT)
#include <bcm_int/esw/firebolt6.h>
#endif
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
#include "include/soc/esw/cancun.h"
#endif

/*
 * Typedef :
 *   _field_ft_group_qual_part_info_t
 * Purpose:
 *   Temporary structure used to convert keygen qualifiers and
 *   offsets information to field group qual_arr member field.
 */
typedef struct _field_ft_group_qual_part_info_s {
    int count;
    bcm_field_qset_t qset;
} _field_ft_group_qual_part_info_t;

/*
 * Function:
 *    _field_hx5_ft_stage_extractors_init
 *
 * Purpose:
 *    Initializes flowtracker stage extractors information.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    stage_fc   - (IN) Field stage control structure.
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_INTERNAL    - Invalid CAP Stage ID.
 *    BCM_E_NONE        - Success.
 */
STATIC int
_field_hx5_ft_stage_extractors_init(int unit,
                                    _field_stage_t *stage_fc)
{
    int rv = BCM_E_NONE;
    int mode = 0;
    bcmi_keygen_ext_cfg_db_t **ext_cfg_db_arr = NULL;

    /* Validate input params */
    if (stage_fc ==  NULL) {
        return BCM_E_PARAM;
    }

    if (stage_fc->ext_cfg_db_arr != NULL) {
        return BCM_E_NONE;
    }

    _FP_XGS3_ALLOC(ext_cfg_db_arr,
                   sizeof(bcmi_keygen_ext_cfg_db_t *) * BCMI_KEYGEN_MODE_COUNT,
                   " ft stage ext cfg db");

    if (ext_cfg_db_arr == NULL) {
        return BCM_E_MEMORY;
    }

    for (mode = BCMI_KEYGEN_MODE_SINGLE;
         mode < BCMI_KEYGEN_MODE_COUNT; mode++) {
        switch(mode) {
            case BCMI_KEYGEN_MODE_SINGLE:
            case BCMI_KEYGEN_MODE_DBLINTRA:
                _FP_XGS3_ALLOC(ext_cfg_db_arr[mode],
                                    sizeof(bcmi_keygen_ext_cfg_db_t),
                                    " ft ext db");
                if (ext_cfg_db_arr[mode] == NULL) {
                    rv = BCM_E_MEMORY;
                }
                if (BCM_SUCCESS(rv)) {
                    rv = _field_ft_keygen_profile_aab_extractors_db_create(unit,
                            ext_cfg_db_arr[mode], mode);
                }
                break;
            default:
                continue;
        }
        if (BCM_FAILURE(rv)) {
            break;
        }
    }

    if (BCM_FAILURE(rv)) {
        if (ext_cfg_db_arr != NULL) {
            for (mode = BCMI_KEYGEN_MODE_SINGLE;
                 mode < BCMI_KEYGEN_MODE_COUNT; mode++) {
                if (ext_cfg_db_arr[mode] != NULL) {
                    (void) bcmi_keygen_ext_cfg_db_free(unit,
                                                ext_cfg_db_arr[mode]);
                    ext_cfg_db_arr[mode] = NULL;
                }
            }
            sal_free(ext_cfg_db_arr);
        }
    } else {
        stage_fc->ext_cfg_db_arr = ext_cfg_db_arr;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_hx5_ft_session_keygen_profiles_init
 *
 * Purpose:
 *    Initializes flowtracker stage session keygen profiles.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    stage_fc   - (IN) Field stage control structure.
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_INTERNAL    - Invalid CAP Stage ID.
 *    BCM_E_NONE        - Success.
 */
STATIC int
_field_hx5_ft_session_keygen_profiles_init(int unit,
                                           _field_stage_t *stage_fc)
{
    int rv = BCM_E_NONE;
    soc_mem_t mem[5] = {INVALIDm};
    int entry_words[5] = {0};
    int min_idx[5] = {0};
    int max_idx[5] = {0};
    int idx_min, idx_max;
    int user_entry_profiles;
    _field_ft_info_t *ft_info = NULL;

    /* Valid only for Flowtracker */
    if (stage_fc->stage_id != _BCM_FIELD_STAGE_FLOWTRACKER) {
        return rv;
    }

    ft_info = stage_fc->ft_info;

    user_entry_profiles = soc_property_get(unit,
                              spn_FLOWTRACKER_NUM_UNIQUE_USER_ENTRY_KEYS, 0);
    if ((user_entry_profiles < 0) || (user_entry_profiles > 128)) {
        user_entry_profiles = 128;
    }

    /* Session Key keygen profile */
    soc_profile_mem_t_init(&(ft_info->session_key_profile));

    mem[0] = BSK_SESSION_KEY_LTS_MUX_CTRL_0m;
    entry_words[0] = BYTES2WORDS(sizeof(bsk_session_key_lts_mux_ctrl_0_entry_t));
    mem[1] = BSK_SESSION_KEY_LTS_MASK_0m;
    entry_words[1] = BYTES2WORDS(sizeof(bsk_session_key_lts_mask_0_entry_t));
    mem[2] = BSK_SESSION_KEY_LTS_MUX_CTRL_1m;
    entry_words[2] = BYTES2WORDS(sizeof(bsk_session_key_lts_mux_ctrl_1_entry_t));
    mem[3] = BSK_SESSION_KEY_LTS_MASK_1m;
    entry_words[3] = BYTES2WORDS(sizeof(bsk_session_key_lts_mask_1_entry_t));
    mem[4] = BSK_SESSION_KEY_LTS_MUX_CTRL_2_PLUS_MASKm;
    entry_words[4] = BYTES2WORDS(sizeof(bsk_session_key_lts_mux_ctrl_2_plus_mask_entry_t));

    idx_min = soc_mem_index_min(unit, mem[0]);
    idx_max = (soc_mem_index_max(unit, mem[0]) - user_entry_profiles);

    min_idx[0] = min_idx[1] = min_idx[2] = min_idx[3] = min_idx[4] = idx_min;
    max_idx[0] = max_idx[1] = max_idx[2] = max_idx[3] = max_idx[4] = idx_max;

    if (idx_min <= idx_max) {
        rv = soc_profile_mem_index_create(unit, mem,
                entry_words, min_idx, max_idx,
                NULL, 5, &(ft_info->session_key_profile));
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_FP,
                    (BSL_META_U(unit,
                                "Flowtracker FP Error: Session key"
                                " Keygen profile - Failed.\n")));
            return (rv);
        }
    }

    /* Session Key keygen profile for user installed flows. */
    if (user_entry_profiles > 0) {
        soc_profile_mem_t_init(&(ft_info->session_key_user_profile));

        idx_min = (soc_mem_index_max(unit, mem[0]) - user_entry_profiles + 1);
        idx_max = (soc_mem_index_max(unit, mem[0]));

        min_idx[0] = min_idx[1] = min_idx[2] = min_idx[3] = min_idx[4] = idx_min;
        max_idx[0] = max_idx[1] = max_idx[2] = max_idx[3] = max_idx[4] = idx_max;

        if (idx_min <= idx_max) {
            rv = soc_profile_mem_index_create(unit, mem,
                    entry_words, min_idx, max_idx,
                    NULL, 5, &(ft_info->session_key_user_profile));
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_FP,
                        (BSL_META_U(unit,
                                    "Flowtracker FP Error: Session key Keygen"
                                    " profile for user flows - Failed.\n")));
                return (rv);
            }
        }
    }


    /* Session Data keygen profile */
    soc_profile_mem_t_init(&(ft_info->session_data_profile));
    mem[0] = BSK_SESSION_DATA_LTS_MUX_CTRL_0m;
    entry_words[0] = BYTES2WORDS(sizeof(bsk_session_data_lts_mux_ctrl_0_entry_t));
    mem[1] = BSK_SESSION_DATA_LTS_MASK_0m;
    entry_words[1] = BYTES2WORDS(sizeof(bsk_session_data_lts_mask_0_entry_t));
    mem[2] = BSK_SESSION_DATA_LTS_MUX_CTRL_1_PLUS_MASKm;
    entry_words[2] = BYTES2WORDS(sizeof(bsk_session_data_lts_mux_ctrl_1_plus_mask_entry_t));

    rv = soc_profile_mem_create(unit, mem,
                entry_words, 3, &(ft_info->session_data_profile));
    if (BCM_FAILURE(rv)) {
        soc_profile_mem_destroy(unit, &(ft_info->session_key_profile));
        LOG_ERROR(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "Flowtracker FP Error: Session Data"
                              " Keygen profile - Failed.\n")));
        return (rv);
    }

    /* Alu data keygen profile */
    soc_profile_mem_t_init(&(ft_info->alu_data_profile));
    mem[0] = BSK_ALU_DATA_LTS_MUX_CTRL_PLUS_MASKm;
    entry_words[0] = BYTES2WORDS(sizeof(bsk_alu_data_lts_mux_ctrl_plus_mask_entry_t));

    rv = soc_profile_mem_create(unit, mem,
                entry_words, 1, &(ft_info->alu_data_profile));
    if (BCM_FAILURE(rv)) {
        soc_profile_mem_destroy(unit, &(ft_info->session_data_profile));
        soc_profile_mem_destroy(unit, &(ft_info->session_key_profile));
        LOG_ERROR(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "Flowtracker FP Error: Alu Data "
                              "Keygen profile - Failed.\n")));
        return (rv);
    }

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {

        /* Aggregate Session Data keygen profile */
        soc_profile_mem_t_init(&(ft_info->agg_session_data_profile));
        mem[0] = BSK_SESSION_DATA2_LTS_MUX_CTRL_0m;
        entry_words[0] = BYTES2WORDS(sizeof(bsk_session_data2_lts_mux_ctrl_0_entry_t));
        mem[1] = BSK_SESSION_DATA2_LTS_MASK_0m;
        entry_words[1] = BYTES2WORDS(sizeof(bsk_session_data2_lts_mask_0_entry_t));
        mem[2] = BSK_SESSION_DATA2_LTS_MUX_CTRL_1_PLUS_MASKm;
        entry_words[2] = BYTES2WORDS(sizeof(bsk_session_data2_lts_mux_ctrl_1_plus_mask_entry_t));

        rv = soc_profile_mem_create(unit, mem,
                entry_words, 3, &(ft_info->agg_session_data_profile));
        if (BCM_FAILURE(rv)) {
            soc_profile_mem_destroy(unit, &(ft_info->alu_data_profile));
            soc_profile_mem_destroy(unit, &(ft_info->session_data_profile));
            soc_profile_mem_destroy(unit, &(ft_info->session_key_profile));
            LOG_ERROR(BSL_LS_BCM_FP,
                    (BSL_META_U(unit,
                                "Flowtracker FP Error: Aggregate Session Data"
                                " Keygen profile - Failed.\n")));
            return (rv);
        }

        /* Aggregate Alu data keygen profile */
        soc_profile_mem_t_init(&(ft_info->agg_alu_data_profile));
        mem[0] = BSK_ALU_DATA2_LTS_MUX_CTRL_PLUS_MASKm;
        entry_words[0] = BYTES2WORDS(sizeof(bsk_alu_data2_lts_mux_ctrl_plus_mask_entry_t));

        rv = soc_profile_mem_create(unit, mem,
                entry_words, 1, &(ft_info->agg_alu_data_profile));
        if (BCM_FAILURE(rv)) {
            soc_profile_mem_destroy(unit, &(ft_info->agg_session_data_profile));
            soc_profile_mem_destroy(unit, &(ft_info->alu_data_profile));
            soc_profile_mem_destroy(unit, &(ft_info->session_data_profile));
            soc_profile_mem_destroy(unit, &(ft_info->session_key_profile));
            LOG_ERROR(BSL_LS_BCM_FP,
                    (BSL_META_U(unit,
                                "Flowtracker FP Error: Aggregate Alu Data "
                                "Keygen profile - Failed.\n")));
            return (rv);
        }
    }
#endif

    return rv;
}

/*
 * Function:
 *    _field_hx5_ft_stage_ftinfo_init
 *
 * Purpose:
 *    Initializes strucutures and databases for flowtracker core module
 *    which is maintained/used by flowtracker stage.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    stage_fc   - (IN) Field stage control structure.
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_hx5_ft_stage_ftinfo_init(int unit,
                                _field_stage_t *stage_fc)
{
    int rv = BCM_E_NONE;
    _field_ft_info_t *ft_info = NULL;

    if (stage_fc == NULL) {
        return BCM_E_PARAM;
    }

    _FP_XGS3_ALLOC(ft_info, sizeof (_field_ft_info_t), "FT core info");
    if (ft_info == NULL) {
        return BCM_E_MEMORY;
    }

    stage_fc->ft_info = ft_info;

    /* Initialize session keygen profiles */
    rv = _field_hx5_ft_session_keygen_profiles_init(unit, stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    /* Tracking param-flowcheck qualifier map db init */
    rv = _field_ft_tracking_param_qual_map_db_init(unit,
                        ft_info, stage_fc->qual_cfg_info_db);
    BCM_IF_ERROR_RETURN(rv);

    /* Flow key Qualifiers cfg */
    _FP_XGS3_ALLOC(ft_info->flow_key_qual_cfg,
            sizeof(bcmi_keygen_qual_cfg_info_db_t),
            "flowtracker qualifiers");
    if (NULL == stage_fc->qual_cfg_info_db) {
        return BCM_E_MEMORY;
    }
    rv = _field_hx5_ft_stage_quals_map_init(unit,
                        ft_info->flow_key_qual_cfg);
    BCM_IF_ERROR_RETURN(rv);


    /* Flow key Extractors cfg is same as stage extractors cfg */
    ft_info->flow_key_ext_cfg_arr = stage_fc->ext_cfg_db_arr;

    /* Flow data Qualifiers cfg */
    _FP_XGS3_ALLOC(ft_info->flow_data_qual_cfg,
            sizeof(bcmi_keygen_qual_cfg_info_db_t),
            "flowtracker qualifiers");
    if (NULL == stage_fc->qual_cfg_info_db) {
        return BCM_E_MEMORY;
    }
    rv = _field_hx5_ft_stage_quals_map_init(unit,
                        ft_info->flow_data_qual_cfg);
    BCM_IF_ERROR_RETURN(rv);

    /* Flow data Extractors cfg is profile AB - 256 bits */
    _FP_XGS3_ALLOC(ft_info->flow_data_ext_cfg,
                    sizeof(bcmi_keygen_ext_cfg_db_t),
                    "flow data extr cfg");
    if (ft_info->flow_data_ext_cfg == NULL) {
        return BCM_E_MEMORY;
    }
    rv = _field_ft_keygen_profile_ab_extractors_db_create(unit,
                ft_info->flow_data_ext_cfg, BCMI_KEYGEN_MODE_SINGLE);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_hx5_ft_stage_ftinfo_deinit
 *
 * Purpose:
 *    De-initializes strucutures and databases for flowtracker core module
 *    which is maintained/used by flowtracker stage.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    stage_fc   - (IN) Stage field control structure.
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_hx5_ft_stage_ftinfo_deinit(int unit,
                                  _field_stage_t *stage_fc)
{
    _field_ft_info_t *ft_info  =NULL;

    if (stage_fc == NULL) {
        return BCM_E_NONE;
    }

    if (stage_fc->ft_info == NULL) {
        return BCM_E_NONE;
    }
    ft_info = stage_fc->ft_info;

    /* destroy session keygen profiles */
    soc_profile_mem_destroy(unit, &(ft_info->session_key_profile));
    soc_profile_mem_destroy(unit, &(ft_info->session_data_profile));
    soc_profile_mem_destroy(unit, &(ft_info->alu_data_profile));

    if (ft_info->flow_key_qual_cfg  != NULL) {
        sal_free(ft_info->flow_key_qual_cfg);
        ft_info->flow_key_qual_cfg = NULL;
    }
    ft_info->flow_key_ext_cfg_arr = NULL;
    if (ft_info->flow_data_qual_cfg != NULL) {
        sal_free(ft_info->flow_data_qual_cfg);
        ft_info->flow_data_qual_cfg = NULL;
    }
    /* Flow data Extractors cfg */
    if (ft_info->flow_data_ext_cfg != NULL) {
        bcmi_keygen_ext_cfg_db_free(unit, ft_info->flow_data_ext_cfg);
        ft_info->flow_data_ext_cfg = NULL;
    }

    (void) _field_ft_tracking_param_qual_map_db_cleanup(unit, ft_info);

    sal_free(stage_fc->ft_info);
    stage_fc->ft_info = NULL;

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_hx5_ft_stage_quals_map_init
 *
 * Purpose:
 *    Initializes flowtracker stage qualifiers ingress field bus mapping.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    db         - (IN/OUT) Qualifier db
 *
 * Returns:
 *    BCM_E_xxx
 */
int
_field_hx5_ft_stage_quals_map_init(int unit,
        bcmi_keygen_qual_cfg_info_db_t *db)
{
    uint32 pp_ports = 0;
    bcmi_keygen_qual_flags_bmp_t qual_flags;

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    int rv = 0;
    soc_cancun_ceh_field_info_t ceh_info;
#endif /* BCM_FLOWTRACKER_V3_SUPPORT */

    BCMI_KEYGEN_FUNC_ENTER(unit);
    BCMI_KEYGEN_QUAL_CFG_DECL;

    sal_memset(&qual_flags, 0, sizeof(bcmi_keygen_qual_flags_bmp_t));

    /* Dummy Qualifiers */
    /* Stage */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyStage, qual_flags);

    /* Stage Flowtracker */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyStageIngressFlowtracker, qual_flags);

    /* Underlay Packet fields - PARSER1_FIELD_BUS */
    /* Source MAC Address */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 0, 0, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 1, 16, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(2, BCMI_KEYGEN_EXT_SECTION_L1E16, 2, 32, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcMac, qual_flags);

    /* Destination MAC Address */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 3, 48, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 4, 64, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(2, BCMI_KEYGEN_EXT_SECTION_L1E16, 5, 80, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstMac, qual_flags);

    /* Outer Vlan */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 10, 160, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyOuterVlan, qual_flags);

    /* Outer Vlan Identifier */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 10, 160, 12);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyOuterVlanId, qual_flags);

    /* Outer Vlan CFI */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 10, 172, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyOuterVlanCfi, qual_flags);

    /* Outer Vlan Priority */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 10, 173, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyOuterVlanPri, qual_flags);

    /* Inner Vlan */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 11, 176, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInnerVlan, qual_flags);

    /* Inner Vlan Identifier */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 11, 176, 12);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInnerVlanId, qual_flags);

    /* Inner Vlan CFI */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 11, 188, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInnerVlanCfi, qual_flags);

    /* Inner Vlan Priority */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 11, 189, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInnerVlanPri, qual_flags);

    /* Outer Ethertype */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 13, 208, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyEtherType, qual_flags);

    /* IPv4 Source Address */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 14, 224, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 15, 240, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcIp, qual_flags);

    /* IPv4 IP Protocol */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 17, 272, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIpProtocol, qual_flags);

    /* IPv4 TTL */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 17, 280, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyTtl, qual_flags);

    /* IPv4 Total Length */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 20, 320, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIp4Length, qual_flags);

    /* IPv4 TOS / DSCP */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 21, 336, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyTos, qual_flags);

    /* IPv6 Source Address */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 14, 224, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 15, 240, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(2, BCMI_KEYGEN_EXT_SECTION_L1E16, 16, 256, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(3, BCMI_KEYGEN_EXT_SECTION_L1E16, 17, 272, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(4, BCMI_KEYGEN_EXT_SECTION_L1E16, 18, 288, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(5, BCMI_KEYGEN_EXT_SECTION_L1E16, 19, 304, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(6, BCMI_KEYGEN_EXT_SECTION_L1E16, 20, 320, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(7, BCMI_KEYGEN_EXT_SECTION_L1E16, 21, 336, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcIp6, qual_flags);

    /* IPv6 LSB 64-bit Source Address */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 14, 224, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 15, 240, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(2, BCMI_KEYGEN_EXT_SECTION_L1E16, 16, 256, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(3, BCMI_KEYGEN_EXT_SECTION_L1E16, 17, 272, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcIp6Low, qual_flags);

    /* IPv6 MSB 64-bit Source Address */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 18, 288, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 19, 304, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(2, BCMI_KEYGEN_EXT_SECTION_L1E16, 20, 320, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(3, BCMI_KEYGEN_EXT_SECTION_L1E16, 21, 336, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcIp6High, qual_flags);

    /* IPv4 Destination Address */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 22, 352, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 23, 368, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstIp, qual_flags);

    /* IPv6 Flow Label */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 22, 352, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 23, 368, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIp6FlowLabel, qual_flags);

    /* IPv6 Traffic class */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 23, 372, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIp6PktTrafficClass, qual_flags);

    /* IPv6 Destination Address */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 24, 384, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 25, 400, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(2, BCMI_KEYGEN_EXT_SECTION_L1E16, 26, 416, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(3, BCMI_KEYGEN_EXT_SECTION_L1E16, 27, 432, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(4, BCMI_KEYGEN_EXT_SECTION_L1E16, 28, 448, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(5, BCMI_KEYGEN_EXT_SECTION_L1E16, 29, 464, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(6, BCMI_KEYGEN_EXT_SECTION_L1E16, 30, 480, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(7, BCMI_KEYGEN_EXT_SECTION_L1E16, 31, 496, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstIp6, qual_flags);

    /* IPv6 LSB 64-bit Destination Address */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 24, 384, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 25, 400, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(2, BCMI_KEYGEN_EXT_SECTION_L1E16, 26, 416, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(3, BCMI_KEYGEN_EXT_SECTION_L1E16, 27, 432, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstIp6Low, qual_flags);

    /* IPv6 MSB 64-bit Destination Address */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 28, 448, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 29, 464, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(2, BCMI_KEYGEN_EXT_SECTION_L1E16, 30, 480, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(3, BCMI_KEYGEN_EXT_SECTION_L1E16, 31, 496, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstIp6High, qual_flags);

    /* IPv6 Hop limit */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 32, 512, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIp6PktHopLimit, qual_flags);

    /* IPv6 Next Header */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 32, 520, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIp6PktNextHeader, qual_flags);

    /* IPv6 Payload Length */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 33, 528, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIp6Length, qual_flags);

    /* L4 Destination Port */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 34, 544, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL4DstPort, qual_flags);

    /* L4 Source Port */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 35, 560, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL4SrcPort, qual_flags);

    /* TCP Window Size */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 36, 576, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyTcpWindowSize, qual_flags);

    /* TCP Control Flags */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 37, 592, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyTcpControl, qual_flags);

    /* VxLan Reserved 56-63 bits */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 38, 608, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVxlanHeaderBits56_63, qual_flags);

    /* VxLan Identifier */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 38, 616, 8);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 39, 624, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVxlanNetworkId, qual_flags);

    /* VxLan Reserved 8-31 bits */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 40, 640, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 41, 656, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVxlanHeaderBits8_31, qual_flags);

    /* VxLan Flags */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 41, 664, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVxlanFlags, qual_flags);

    /* Overlay packet fields - PARSER2_FIELD_BUS */
    /* Tunneled Payload / Inner IPv4 Source Address */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 78, 1248, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 79, 1264, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInnerSrcIp, qual_flags);

    /* Tunneled Payload / Inner IPv4 Ip Protocol */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 81, 1296, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInnerIpProtocol, qual_flags);

    /* Inner IPv4 TTL */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 81, 1304, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInnerTtl, qual_flags);

    /* Tunnel Payload / Inner IPv4 Total Length */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 84, 1344, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyTunnelPayloadIp4Length, qual_flags);

    /* Inner IPv4 TOS / DSCP */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 85, 1360, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInnerTos, qual_flags);

    /* Inner IPv6 Source Address */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 78, 1248, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 79, 1264, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(2, BCMI_KEYGEN_EXT_SECTION_L1E16, 80, 1280, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(3, BCMI_KEYGEN_EXT_SECTION_L1E16, 81, 1296, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(4, BCMI_KEYGEN_EXT_SECTION_L1E16, 82, 1312, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(5, BCMI_KEYGEN_EXT_SECTION_L1E16, 83, 1328, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(6, BCMI_KEYGEN_EXT_SECTION_L1E16, 84, 1344, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(7, BCMI_KEYGEN_EXT_SECTION_L1E16, 85, 1360, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInnerSrcIp6, qual_flags);

    /* Inner IPv6 MSB 64-bit Source Address */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 82, 1312, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 83, 1328, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(2, BCMI_KEYGEN_EXT_SECTION_L1E16, 84, 1344, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(3, BCMI_KEYGEN_EXT_SECTION_L1E16, 85, 1360, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInnerSrcIp6High, qual_flags);

    /* Inner IPv4 Destination Address */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 86, 1376, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 87, 1392, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInnerDstIp, qual_flags);

    /* Inner IPv6 Flow Label */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 86, 1376, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 87, 1392, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInnerIp6FlowLabel, qual_flags);

    /* Inner IPv6 Traffic class */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 87, 1396, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInnerIp6PktTrafficClass, qual_flags);

    /* Inner IPv6 Destination Address */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 88, 1408, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 89, 1424, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(2, BCMI_KEYGEN_EXT_SECTION_L1E16, 90, 1440, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(3, BCMI_KEYGEN_EXT_SECTION_L1E16, 91, 1456, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(4, BCMI_KEYGEN_EXT_SECTION_L1E16, 92, 1472, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(5, BCMI_KEYGEN_EXT_SECTION_L1E16, 93, 1488, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(6, BCMI_KEYGEN_EXT_SECTION_L1E16, 94, 1504, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(7, BCMI_KEYGEN_EXT_SECTION_L1E16, 95, 1520, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInnerDstIp6, qual_flags);

    /* Inner IPv6 MSB 64-bit Destination Address */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 92, 1472, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 93, 1488, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(2, BCMI_KEYGEN_EXT_SECTION_L1E16, 94, 1504, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(3, BCMI_KEYGEN_EXT_SECTION_L1E16, 95, 1520, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInnerDstIp6High, qual_flags);

    /* Inner IPv6 Hop limit */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 96, 1536, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInnerIp6PktHopLimit, qual_flags);

    /* Inner IPv6 Next Header */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 96, 1544, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInnerIp6PktNextHeader, qual_flags);

    /* Tunnel Payload / Inner IPv6 Payload Length */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 97, 1552, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyTunnelPayloadIp6Length, qual_flags);

    /* Inner L4 Destination Port */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 98, 1568, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInnerL4DstPort, qual_flags);

    /* Inner L4 Source Port */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 99, 1584, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInnerL4SrcPort, qual_flags);

    /* Inner TCP Control Flags */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 101, 1616, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInnerTcpControl, qual_flags);

    /* OBJECT BUS */
    /* Module Header SGPP - Module Id + Port */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 129, 2064, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyHiGigSrcModPortGport, qual_flags);

    /* Module Header SGPP - Module Id */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 129, 2072, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyHiGigSrcModuleGport, qual_flags);

    /* Module Header SGPP - Port */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 129, 2064, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyHiGigSrcPortGport, qual_flags);

    /* SGLP Module Id + Port */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 130, 2080, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcModPortGport, qual_flags);

    /* SGLP Module Id */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 130, 2088, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcModuleGport, qual_flags);

    /*INGRESS_PP_PORT */
    pp_ports = 7;
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 133, 2128, pp_ports);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInPort, qual_flags);

    /* Source Trunk Map class ID */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 135, 2160, 12);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassPort, qual_flags);

    /* VFP_CLASSID HI */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 136, 2176, 10);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcClassField, qual_flags);

    /* VFP_CLASSID LO */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 137, 2192, 10);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassField, qual_flags);

    /* Forwarding VPN/VFI */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 138, 2208, 14);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVpn, qual_flags);

    /* Forwarding Vlan */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 138, 2208, 12);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyForwardingVlanId, qual_flags);

    /* Forwarding Type */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 138, 2222, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyForwardingType, qual_flags);

    /* Virtual Routing forwarding object */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 139, 2224, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVrf, qual_flags);

    /* Incoming L3 Interface */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 140, 2240, 13);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL3Ingress, qual_flags);

    /* Higig Dst Module + Port */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 143, 2288, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyHiGigDstModPortGport, qual_flags);

    /* Opaque Object 1 */
    /* Opaque Object 2 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 145, 2320, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyOpaqueObject2, qual_flags);

    /* Source VP Class Id */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 153, 2448, 12);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassVPort, qual_flags);

    /* L3_IIF Class ID */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 154, 2464, 12);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassL3, qual_flags);

    /* Tunnel Class for Flowtracker usage */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 157, 2512, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 158, 2528, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyTunnelId, qual_flags);

    /* Opaque Object 3 */
    /* Opaque Object 4 */

    /* HVE1_DOS_ATTACK_RESULTS_BUS */
    /* DOS Attack Events */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 173, 2768, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 174, 2784, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDosAttackEvents, qual_flags);

    /* HVE2_DOS_ATTACK_RESULTS_BUS */
    /* Inner DOS Attack Events */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 175, 2800, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 176, 2816, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInnerDosAttackEvents, qual_flags);

    /* PARSER1_HVE_BUS */
    /* Vlan Format */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 178, 2849, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVlanFormat, qual_flags);

    /* L2 Format */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 178, 2857, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL2Format, qual_flags);

    /* L2/L3 Unicast/Multicast/Broadcast */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 179, 2866, 2);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 179, 2876, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyPktDstAddrType, qual_flags);

    /* Outer Priority tagged */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 179, 2873, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyOuterPriTaggedPkt, qual_flags);

    /* IP with TTL=0 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 179, 2878, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIpTtlZeroPkt, qual_flags);

    /* IP Flags */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 181, 2899, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIpFlags, qual_flags);

    /* L4 Valid */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 181, 2908, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL4Ports, qual_flags);

    /* Inner Tpid */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 182, 2928, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInnerTpid, qual_flags);

    /* Outer Tpid */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 183, 2942, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyOuterTpid, qual_flags);

    /* PARSER2_HVE_BUS */
    /* Inner L2/L3 Unicast/Multicast/Broadcast */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 193, 3090, 2);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 193, 3099, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyPktInnerDstAddrType, qual_flags);

    /* Inner L4 Valid */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 195, 3132, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInnerL4Ports, qual_flags);

#if defined(BCM_APPL_SIGNATURE_SUPPORT)
    if (soc_feature(unit, soc_feature_appl_signature_support)) {
        /* Appliation signature */
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 169, 2712, 6);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyApplSignatureId, qual_flags);

        /* Tunnel Appliation signature */
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 172, 2760, 6);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInnerApplSignatureId, qual_flags);
    }
#endif

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        /* Gbp Source Id */
        rv = soc_cancun_ceh_obj_field_get(unit, "OPAQUE_1", "GBP_SID", &ceh_info);
        if (SOC_SUCCESS(rv)) {
            BCMI_KEYGEN_QUAL_CFG_INIT(unit);
            BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 144, 2304, 16);
            BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyGbpSrcId, qual_flags);
        }
        COMPILER_REFERENCE(ceh_info);
    }
#endif

exit:

    BCMI_KEYGEN_FUNC_EXIT();
}

/*
 * Function:
 *    _field_hx5_ft_stage_udf_quals_init
 *
 * Purpose:
 *    Initializes flowtracker stage udf qualifiers.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    db         - (IN/OUT) Qualifier db
 *
 * Returns:
 *    BCM_E_xxx
 */
STATIC int
_field_hx5_ft_stage_udf_quals_init(int unit,
        bcmi_keygen_qual_cfg_info_db_t *db)
{
    bcmi_keygen_qual_flags_bmp_t qual_flags;

    BCMI_KEYGEN_FUNC_ENTER(unit);
    BCMI_KEYGEN_QUAL_CFG_DECL;

    sal_memset(&qual_flags, 0, sizeof(bcmi_keygen_qual_flags_bmp_t));

    /* UDF chunk 0 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 49, 784, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData0, qual_flags);

    /* UDF chunk 1 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 48, 768, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData1, qual_flags);

    /* UDF chunk 2 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 47, 752, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData2, qual_flags);

    /* UDF chunk 3 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 46, 736, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData3, qual_flags);

    /* UDF chunk 4 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 45, 720, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData4, qual_flags);

    /* UDF chunk 5 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 44, 704, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData5, qual_flags);

    /* UDF chunk 6 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 43, 688, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData6, qual_flags);

    /* UDF chunk 7 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 42, 672, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData7, qual_flags);

    /* UDF chunk 8 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 60, 944, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData8, qual_flags);

    /* UDF chunk 9 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 58, 928, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData9, qual_flags);

    /* UDF chunk 10 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 56, 896, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData10, qual_flags);

    /* UDF chunk 11 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 54, 872, 8);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 55, 880, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData11, qual_flags);

    /* UDF chunk 12 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 53, 848, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData12, qual_flags);

    /* UDF chunk 13 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 52, 832, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData13, qual_flags);

    /* UDF chunk 14 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 51, 816, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData14, qual_flags);

    /* UDF chunk 15 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 50, 800, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData15, qual_flags);

exit:

    BCMI_KEYGEN_FUNC_EXIT();
}

/*
 * Function:
 *    _field_hx5_ft_stage_quals_ibus_map_init
 *
 * Purpose:
 *    Initializes flowtracker stage qualifiers ingress field bus mapping.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    stage_fc   - (IN) Stage field control structure.
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_INTERNAL    - Invalid CAP Stage ID.
 *    BCM_E_NONE        - Success.
 */
STATIC int
_field_hx5_ft_stage_quals_ibus_map_init(int unit,
                             _field_stage_t *stage_fc)
{
    int rv = BCM_E_NONE;
    bcmi_keygen_qual_cfg_info_db_t *db = NULL;

    /* validate input params */
    if (stage_fc == NULL) {
        return BCM_E_PARAM;
    }

    _FP_XGS3_ALLOC(stage_fc->qual_cfg_info_db,
            sizeof(bcmi_keygen_qual_cfg_info_db_t),
            "flowtracker qualifiers");
    if (NULL == stage_fc->qual_cfg_info_db) {
        return (BCM_E_MEMORY);
    }

    db = stage_fc->qual_cfg_info_db;

    rv = _field_hx5_ft_stage_quals_map_init(unit, db);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    rv = _field_hx5_ft_stage_udf_quals_init(unit, db);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    return BCM_E_NONE;

cleanup:

    if (db != NULL) {
        sal_free(db);
        stage_fc->qual_cfg_info_db = NULL;
    }

   return rv;
}

/*
 * Function:
 *    _bcm_field_hx5_ft_tcam_policy_mem_get
 *
 * Purpose:
 *    Get tcam+policy combo memory
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    idx        - (IN) Slice number
 *    tcam_mem   - (OUT) tcam table
 *    policy_mem - (OUT) policy table
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_bcm_field_hx5_ft_tcam_policy_mem_get(int unit,
                                      int slice_num,
                                      soc_mem_t *tcam_mem,
                                      soc_mem_t *policy_mem)
{
    if (slice_num == 0) {
        *tcam_mem = BSK_FTFP_TCAMm;
        *policy_mem = BSK_FTFP_POLICYm;
    } else {
        *tcam_mem = BSK_FTFP2_TCAMm;
        *policy_mem = BSK_FTFP2_POLICYm;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_hx5_ft_entry_qual_tcam_set
 *
 * Purpose:
 *    Sets tcam key and mask information to buffer
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    f_ent      - (IN) Field entry structure.
 *    tcam_mem   - (IN) tcam hw table.
 *    tcam_idx   - (IN) tcam hw index
 *    entry_buf  - (IN/OUT) Entry buffer.
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_hx5_ft_entry_qual_tcam_set(int unit,
                                  _field_entry_t *f_ent,
                                  soc_mem_t tcam_mem,
                                  int tcam_idx,
                                  uint32 *entry_buf)
{
    _field_tcam_t *tcam = NULL;

    if ((f_ent == NULL) || (entry_buf == NULL))   {
        return BCM_E_PARAM;
    }

    tcam = &f_ent->tcam;
    if (tcam == NULL) {
        return (BCM_E_INTERNAL);
    }

    soc_mem_field_set(unit, tcam_mem, entry_buf, KEYf, tcam->key);
    soc_mem_field_set(unit, tcam_mem, entry_buf, MASKf, tcam->mask);
    soc_mem_field32_set(unit, tcam_mem, entry_buf, VALIDf, 3);

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_hx5_ft_entry_policy_set
 *
 * Purpose:
 *    Sets policy data information to buffer
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    f_ent      - (IN) Field entry structure.
 *    policy_mem - (IN) Policy hw table.
 *    e_buf      - (IN/OUT) Entry buffer.
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_INTERNAL    - Invalid CAP Stage ID.
 *    BCM_E_NONE        - Success.
 */
STATIC int
_field_hx5_ft_entry_policy_set(int unit,
                               _field_entry_t *f_ent,
                               soc_mem_t policy_mem,
                               uint32 *e_buf)
{
    int rv = BCM_E_NONE;
    _field_action_t *fa = NULL;
    uint32 value = 0;
    uint32 do_not_ft_def = 1;
    uint32 action_ft = 0, action_ft_set = FALSE;
    uint32 action_new_learn = 0, action_new_learn_set = FALSE;
    bcm_flowtracker_group_t ft_group_id = -1;
    bcmi_ft_group_ftfp_data_t ftfp_data;
    bcmi_ft_session_profiles_t *profiles = NULL;
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    bcm_flowtracker_group_t aft_group_id = -1;
    uint32 action_aggregate_class_set = FALSE;
    uint32 action_aggregate_class = 0;
    uint32 action_aft_index_set = FALSE;
    uint32 action_aft_index = 0;
#endif

    if ((f_ent ==NULL) || (policy_mem == INVALIDm) || (e_buf == NULL)) {
        return (BCM_E_PARAM);
    }

    /* policy install is needed only for Entry part 0 */
    if (f_ent->flags & _FP_ENTRY_SECOND_HALF) {
        return BCM_E_NONE;
    }

    /* Loop through all actions */
    for (fa = f_ent->actions;
           (fa != NULL) && (fa->flags & _FP_ACTION_VALID);
           fa = fa->next) {
        switch(fa->action) {
            case bcmFieldActionFlowtrackerGroupId:
                ft_group_id = fa->param[0];
                break;
            case bcmFieldActionFlowtrackerEnable:
                action_ft_set = TRUE;
                action_ft = fa->param[0];
                break;
            case bcmFieldActionFlowtrackerNewLearnEnable:
                action_new_learn_set = TRUE;
                action_new_learn = fa->param[0];
                break;
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
            case bcmFieldActionFlowtrackerAggregateIngressGroupId:
                aft_group_id = fa->param[0];
                break;
            case bcmFieldActionFlowtrackerAggregateClass:
                action_aggregate_class_set = TRUE;
                action_aggregate_class = fa->param[0];
                break;
            case bcmFieldActionFlowtrackerAggregateIngressFlowIndex:
                action_aft_index_set = TRUE;
                action_aft_index = fa->param[1];
                break;
#endif
            default:
                return (BCM_E_INTERNAL);
        }
    }

    if (ft_group_id != -1) {
        /* Read all relevent information from FT module */
        BCM_IF_ERROR_RETURN(bcmi_ft_group_ftfp_data_get(unit,
                        ft_group_id, &ftfp_data));
        profiles = &ftfp_data.profiles;

        /* Set Group Id */
        if(SOC_MEM_FIELD_VALID(unit, policy_mem, GROUP_IDf)) {
            value = BCMI_FT_GROUP_INDEX_GET(ft_group_id);
            soc_mem_field_set(unit, policy_mem, (void *) e_buf,
                    GROUP_IDf, &(value));
        }

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
        if (SOC_MEM_FIELD_VALID(unit, policy_mem, UFLOW_FIELDS_VALIDf)) {
            value = 1;
            soc_mem_field_set(unit, policy_mem, (void *) e_buf,
                    UFLOW_FIELDS_VALIDf, &(value));
        }
#endif

        /* Set bi-directional flow */
        if (SOC_MEM_FIELD_VALID(unit, policy_mem, BIDIRECTIONAL_FLOWf)) {
            value = 0;
            if (ftfp_data.direction == bcmFlowtrackerFlowDirectionBidirectional) {
                value = 1;
            }
            BCM_IF_ERROR_RETURN(_field_hx5_ft_normalize_controls_set(unit,
                    ft_group_id, policy_mem, value, e_buf));
            soc_mem_field_set(unit, policy_mem, (void *) e_buf,
                BIDIRECTIONAL_FLOWf, &(value));
        }

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
        /* Set Direction Control Type */
        if (SOC_MEM_FIELD_VALID(unit, policy_mem, KMAP_CONTROL_1_2_PROFILE_IDXf)) {
            if (ftfp_data.dir_ctrl_type ==
                    bcmFlowtrackerGroupControlFlowDirection) {
                value = 0;
            } else if (ftfp_data.dir_ctrl_type ==
                    bcmFlowtrackerGroupControlUdpFlowDirection) {
                value = 1;
            }
            soc_mem_field_set(unit, policy_mem, (void *) e_buf,
                    KMAP_CONTROL_1_2_PROFILE_IDXf, &(value));
        }
#endif

        /* Set Do not FT */
        if (SOC_MEM_FIELD_VALID(unit, policy_mem, DO_NOT_FTf)) {
            if (action_ft_set == TRUE) {
                value = (action_ft) ? 0 : 1;
            } else {
                value = (ftfp_data.flowtrack) ? 0 : 1;
            }
            soc_mem_field_set(unit, policy_mem, (void *) e_buf,
                    DO_NOT_FTf, &(value));
        }

        /* Set New Learn Disable */
        if (SOC_MEM_FIELD_VALID(unit, policy_mem, LEARN_DISABLEf)) {
            if (action_new_learn_set == TRUE) {
                value = (action_new_learn) ? 0 : 1;
            } else {
                value = (ftfp_data.new_learn) ? 0: 1;
            }
            soc_mem_field_set(unit, policy_mem, (void *) e_buf,
                    LEARN_DISABLEf, &(value));
        }

        /* Set Session Key Profile index */
        if ((SOC_MEM_FIELD_VALID(unit,
                policy_mem, SESSION_KEY_LTS_PROFILEf)) &&
            (profiles->session_key_profile_idx != 0)) {
            value = profiles->session_key_profile_idx - 1;
            soc_mem_field_set(unit, policy_mem, (void *) e_buf,
                    SESSION_KEY_LTS_PROFILEf, &(value));
        }

        /* Set Session Data Profile index */
        if ((SOC_MEM_FIELD_VALID(unit,
                policy_mem, SESSION_DATA_LTS_PROFILEf)) &&
            (profiles->session_data_profile_idx != 0)) {
            value = profiles->session_data_profile_idx - 1;
            soc_mem_field_set(unit, policy_mem, (void *) e_buf,
                    SESSION_DATA_LTS_PROFILEf, &(value));
        }

        /* Set Alu Data Profile index */
        if ((SOC_MEM_FIELD_VALID(unit,
                policy_mem, ALU_DATA_LTS_PROFILEf)) &&
            (profiles->alu_data_profile_idx != 0)) {
            value = profiles->alu_data_profile_idx - 1;
            soc_mem_field_set(unit, policy_mem, (void *) e_buf,
                    ALU_DATA_LTS_PROFILEf, &(value));
        }

        /* Set Key Type */
        if (SOC_MEM_FIELD_VALID(unit, policy_mem, SESSION_KEY_TYPEf)) {
            /* Set this value to Profile_id of Session_key */
            value = ftfp_data.session_key_type;
            soc_mem_field_set(unit, policy_mem, (void *) e_buf,
                    SESSION_KEY_TYPEf, &(value));
        }

        /* Session Key Mode */
        if (SOC_MEM_FIELD_VALID(unit, policy_mem, SESSION_KEY_MODEf)) {
            value =
                (ftfp_data.session_key_mode == bcmiFtGroupModeDouble) ? 1 : 0;

            soc_mem_field_set(unit, policy_mem, (void *) e_buf,
                    SESSION_KEY_MODEf, &(value));
        }
    } else {
        /* Disable Micro Group Fields */
        /* Group is not associated, set do not Ft */
        if (SOC_MEM_FIELD_VALID(unit, policy_mem, DO_NOT_FTf)) {
            soc_mem_field_set(unit, policy_mem, (void *) e_buf,
                    DO_NOT_FTf, &do_not_ft_def);
        }
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
        if (soc_feature(unit, soc_feature_field_flowtracker_v3_support)) {
            if (SOC_MEM_FIELD_VALID(unit, policy_mem, UFLOW_FIELDS_VALIDf)) {
                value = 0;
                soc_mem_field_set(unit, policy_mem, (void *) e_buf,
                        UFLOW_FIELDS_VALIDf, &(value));
            }
        }
#endif
    }

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    /*
     * Both AIFT Group and AIID must be present to install
     * aggregate fields.
     */
    if (((aft_group_id != -1) && (action_aft_index_set == FALSE)) ||
            ((aft_group_id == -1) && (action_aft_index_set == TRUE))) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit, "FP(unit %d)"
                        " Error: Action for aggregate Group or FlowIndex"
                        " is missing\n"), unit));
        return BCM_E_CONFIG;
    }

    if (aft_group_id != -1) {
        /* Read all relevent information from FT module */
        BCM_IF_ERROR_RETURN(bcmi_ft_group_ftfp_data_get(unit,
                    aft_group_id, &ftfp_data));
        profiles = &ftfp_data.profiles;

        /* Set Group Id */
        if(SOC_MEM_FIELD_VALID(unit, policy_mem, AIGIDf)) {
            value = BCMI_FT_GROUP_INDEX_GET(aft_group_id);
            soc_mem_field_set(unit, policy_mem, (void *) e_buf,
                    AIGIDf, &(value));
        }

        /* Enable Aggregate Group Fields */
        if (SOC_MEM_FIELD_VALID(unit, policy_mem, AGG_FIELDS_VALIDf)) {
            value = 1;
            soc_mem_field_set(unit, policy_mem, (void *) e_buf,
                    AGG_FIELDS_VALIDf, &(value));
        }

        /* Set Aggregate Session Data Profile */
        if(SOC_MEM_FIELD_VALID(unit, policy_mem,
                    AGG_SESSION_DATA_LTS_PROFILEf) &&
                (profiles->session_data_profile_idx != 0)) {
            value = profiles-> session_data_profile_idx - 1;
            soc_mem_field_set(unit, policy_mem, (void *) e_buf,
                    AGG_SESSION_DATA_LTS_PROFILEf, &(value));
        }

        /* Set Aggregate Alu Data Profile */
        if(SOC_MEM_FIELD_VALID(unit, policy_mem,
                    AGG_ALU_DATA_LTS_PROFILEf) &&
                (profiles->alu_data_profile_idx != 0)) {
            value = profiles-> alu_data_profile_idx - 1;
            soc_mem_field_set(unit, policy_mem, (void *) e_buf,
                    AGG_ALU_DATA_LTS_PROFILEf, &(value));
        }

        /* Set Aggregate Flow Index */
        if(SOC_MEM_FIELD_VALID(unit, policy_mem, AIIDf)) {
            value = action_aft_index;
            soc_mem_field_set(unit, policy_mem, (void *) e_buf,
                    AIIDf, &(value));
        }
   }

    /* Set Aggregate Class */
    if (SOC_MEM_FIELD_VALID(unit, policy_mem, ACIDf)) {
        value = 0;
        if (action_aggregate_class_set == TRUE) {
            value = action_aggregate_class;
        }
        soc_mem_field_set(unit, policy_mem, (void *) e_buf,
                ACIDf, &(value));
    }

    if (soc_feature(unit, soc_feature_field_flowtracker_v3_support)) {
        if (SOC_MEM_FIELD_VALID(unit, policy_mem, AGG_FIELDS_VALIDf)) {
            value = 0;
            if ((aft_group_id != -1) ||
                    (action_aggregate_class_set == TRUE)) {
                value = 1;
            }
            soc_mem_field_set(unit, policy_mem, (void *) e_buf,
                    AGG_FIELDS_VALIDf, &(value));
        }
    }
#endif

    /* Reset DIRTY flag */
    for (fa = f_ent->actions;
           (fa != NULL) && (fa->flags & _FP_ACTION_VALID);
           fa = fa->next) {
        fa->flags &= (~_FP_ACTION_DIRTY);
    }

    return rv;
}

/*
 * Function:
 *    _field_hx5_ft_tcam_policy_init
 *
 * Purpose:
 *    Fill Tcam/Policy memories with default entries.
 *    Default entries are required to handle broadscan port.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    stage_fc   - (IN) Field stage control structure.
 *
 * Returns:
 *    BCM_E_PARAM       - Wrong Input params
 *    BCM_E_NONE        - Success.
 */
int
_field_hx5_ft_tcam_policy_init(int unit,
                               _field_stage_t *stage_fc)
{
    int rv = 0;
    int idx = 0;
    uint32 do_not_ft_def = 1;
    uint32 broadscan_port = 0;
    uint32 e_buf[SOC_MAX_MEM_FIELD_WORDS] = {0};
    uint32 policy_buf[SOC_MAX_MEM_FIELD_WORDS] = {0};
    uint32 key_key_buf[SOC_MAX_MEM_FIELD_WORDS] = {0};
    uint32 mask_key_buf[SOC_MAX_MEM_FIELD_WORDS] = {0};
    _field_slice_t *fs = NULL;
    soc_mem_t tcam_mem = INVALIDf;
    soc_mem_t policy_mem = INVALIDf;

    /* If user wants to flowtrack ipfix packets coming from FAE Port */
    if (!soc_feature(unit, soc_feature_bscan_fae_port_flow_exclude)) {
        return BCM_E_NONE;
    }

    /* Get FAE Port */
    broadscan_port = FAE_PORT(unit);

    /* Disable Ingress Filtering */
    rv = bcm_esw_port_control_set(unit, broadscan_port,
            bcmPortControlFilterIngress, FALSE);
    BCM_IF_ERROR_RETURN(rv);

    for (idx = 0; idx < stage_fc->tcam_slices; idx++) {

        /* Field Slice */
        fs = stage_fc->slices[0] + idx;

        /* Decrease tcam_sz by reserved entries */
        /* 1 for Single-wide, 2 for Double-wide and 1 Reserved */
        stage_fc->tcam_sz -= _BCM_FIELD_HX5_INGRESS_FT_RESERVED_ENTRIES;

        /* Move tcam start Index after reserved entries */
        fs->start_tcam_idx += _BCM_FIELD_HX5_INGRESS_FT_RESERVED_ENTRIES;
        /* Reduce number of Free Entry in the slice */
        fs->free_count -= _BCM_FIELD_HX5_INGRESS_FT_RESERVED_ENTRIES;
        /* Reduce number of entries in the slice */
        fs->entry_count -= _BCM_FIELD_HX5_INGRESS_FT_RESERVED_ENTRIES;

        /*
         * Modify Combo table to disard traffic coming from broadscan port.
         * Single-Wide default entry
         *
         * mod BSK_FTFP_COMBO_TCAM_POLICY 0 1 VALID=3 MASK_KEY=0x7F KEY_KEY=0x47 \
         *                                    MASK_MODE=1 KEY_MODE=0 DO_NOT_FT=1
         *
         * Double-Wide default entry
         * mod BSK_FTFP_COMBO_TCAM_POLICY 2 1 VALID=3 MASK_MODE=1 KEY_MODE=1 \
         MASK_KEY=0x7F KEY_KEY=0x47 DO_NOT_FT=1
         * mod BSK_FTFP_COMBO_TCAM_POLICY 3 1 VALID=3 MASK_MODE=1 KEY_MODE=1
         */

        rv = _bcm_field_hx5_ft_tcam_policy_mem_get(unit, idx, &tcam_mem, &policy_mem);
        BCM_IF_ERROR_RETURN(rv);

        /* Init Tcam+Policy at Index=0 */
        sal_memset(e_buf, 0, sizeof(uint32) * SOC_MAX_MEM_FIELD_WORDS);
        sal_memset(policy_buf, 0, sizeof(uint32) * SOC_MAX_MEM_FIELD_WORDS);

        /* Set fields in policy */
        soc_mem_field32_set(unit, policy_mem,
                (void *) policy_buf, DO_NOT_FTf, do_not_ft_def);

        /* Set fields in Tcam */
        key_key_buf[0] = broadscan_port;
        mask_key_buf[0]=0x7f;
        soc_mem_field_set(unit, tcam_mem,
                e_buf, KEY_KEYf, &key_key_buf[0]);
        soc_mem_field_set(unit, tcam_mem,
                e_buf, MASK_KEYf, &mask_key_buf[0]);
        soc_mem_field32_set(unit, tcam_mem,
                (void *) e_buf, KEY_MODEf, 0);
        soc_mem_field32_set(unit, tcam_mem,
                (void *) e_buf, MASK_MODEf, 1);
        soc_mem_field32_set(unit, tcam_mem,
                (void *) e_buf, VALIDf, 3);

        /* Write to hardware */
        rv = soc_mem_write(unit, policy_mem, MEM_BLOCK_ALL, 0, policy_buf);
        BCM_IF_ERROR_RETURN(rv);
        rv = soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL, 0, e_buf);
        BCM_IF_ERROR_RETURN(rv);

        /* Init Tcam+Policy at Index=2 */
        sal_memset(e_buf, 0, sizeof(uint32) * SOC_MAX_MEM_FIELD_WORDS);
        sal_memset(policy_buf, 0, sizeof(uint32) * SOC_MAX_MEM_FIELD_WORDS);

        /* Set fields in policy */
        soc_mem_field32_set(unit, policy_mem,
                (void *) policy_buf, DO_NOT_FTf, do_not_ft_def);

        /* Set fields in Tcam */
        key_key_buf[0] = broadscan_port;
        mask_key_buf[0]=0x7f;
        soc_mem_field_set(unit, tcam_mem,
                (void *) e_buf, KEY_KEYf, &key_key_buf[0]);
        soc_mem_field_set(unit, tcam_mem,
                (void *) e_buf, MASK_KEYf, &mask_key_buf[0]);
        soc_mem_field32_set(unit, tcam_mem,
                (void *) e_buf, KEY_MODEf, 1);
        soc_mem_field32_set(unit, tcam_mem,
                (void *) e_buf, MASK_MODEf, 1);
        soc_mem_field32_set(unit, tcam_mem,
                (void *) e_buf, VALIDf, 3);

        /* Write to hardware */
        rv = soc_mem_write(unit, policy_mem, MEM_BLOCK_ALL, 2, policy_buf);
        BCM_IF_ERROR_RETURN(rv);
        rv = soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL, 2, e_buf);
        BCM_IF_ERROR_RETURN(rv);

        /* Init Tcam+Policy at Index=3 */
        sal_memset(e_buf, 0, sizeof(uint32) * SOC_MAX_MEM_FIELD_WORDS);

        /* Set fields in Tcam */
        soc_mem_field32_set(unit, tcam_mem,
                (void *) e_buf, KEY_MODEf, 1);
        soc_mem_field32_set(unit, tcam_mem,
                (void *) e_buf, MASK_MODEf, 1);
        soc_mem_field32_set(unit, tcam_mem,
                (void *) e_buf, VALIDf, 3);

        /* Write to hardware */
        rv = soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL, 3, e_buf);
        BCM_IF_ERROR_RETURN(rv);
    }
    return rv;
}

/*
 * Function:
 *    _field_hx5_ftstage_init
 *
 * Purpose:
 *    Initializes flowtracker stage related information.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    fc         - (IN) Field control structure.
 *    stage_fc   - (IN) Field stage control structure.
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_INTERNAL    - Invalid CAP Stage ID.
 *    BCM_E_NONE        - Success.
 */
int
_field_hx5_ftstage_init(int unit,
                        _field_control_t *fc,
                        _field_stage_t *stage_fc)
{
    int rv = BCM_E_NONE;

    /* Input parameter Check */
    if ((fc == NULL) || (stage_fc == NULL)) {
        return (BCM_E_PARAM);
    }

    /* Applicable only for flowtracker stage */
    if (stage_fc->stage_id != _BCM_FIELD_STAGE_FLOWTRACKER) {
        return (BCM_E_NONE);
    }

    /* Initialize stage's extractor configuration */
    rv = _field_hx5_ft_stage_extractors_init(unit, stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Error: _field_hx5_ft_stage_extractors_init=%d\n"),
            unit, rv));
        return (rv);
    }

    /* Initializing supported Qset */
    sal_memset(&stage_fc->_field_supported_qset, 0, sizeof(bcm_field_qset_t));

    /* Initialize stage Preselector information for Lt slices and entries */
    rv = _bcm_field_th_stage_preselector_init(unit, fc, stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
           "FP(unit %d) Error: _bcm_field_th_stage_preselector_init=%d"
           "\n"), unit, rv));
        return (rv);
    }

    /* Qualifiers ingress field bus map */
    rv = _field_hx5_ft_stage_quals_ibus_map_init(unit, stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
           "FP(unit %d) Error: _field_hx5_ft_stage_quals_ibus_map_init=%d"
           "\n"), unit, rv));
        return (rv);
    }

    /* Init information used by FT core module */
    rv = _field_hx5_ft_stage_ftinfo_init(unit, stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
           "FP(unit %d) Error: _field_hx5_ft_stage_ftinfo_init=%d"
           "\n"), unit, rv));
        return (rv);
    }

    return (rv);
}

/*
 * Function:
 *    _field_hx5_ftstage_deinit
 *
 * Purpose:
 *    De-initializes flowtracker stage.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    stage_fc   - (IN) Stage field control structure.
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_INTERNAL    - Invalid CAP Stage ID.
 *    BCM_E_NONE        - Success.
 */
int
_field_hx5_ftstage_deinit(int unit,
                          _field_stage_t *stage_fc)
{
    /* Input parameter check. */
    if (NULL == stage_fc) {
        return (BCM_E_NONE);
    }

    if (stage_fc->stage_id != _BCM_FIELD_STAGE_FLOWTRACKER) {
        return BCM_E_NONE;
    }

    /* De-init ft related info */
    (void) _field_hx5_ft_stage_ftinfo_deinit(unit, stage_fc);

    /* Rest of allocations freed in common code */

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_hx5_ft_slice_validate
 *
 * Purpose:
 *    Validates if field group can be allowed to create in given slice.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    stage_fc   - (IN) Stage field control structure.
 *    fg         - (IN) Field group control structure.
 *    slice_id   - (IN) Slice number
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_INTERNAL    - Invalid CAP Stage ID.
 *    BCM_E_NONE        - Success.
 */
int
_field_hx5_ft_slice_validate(int unit,
                             _field_stage_t *stage_fc,
                             _field_group_t *fg,
                             int slice_id)
{
    int rv = BCM_E_NONE;

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_field_flowtracker_v3_support) &&
            (fg->flags & _FP_GROUP_SELECT_SECONDARY_SLICE)) {
        if (slice_id != 1) {
            return BCM_E_CONFIG;
        }
    }
#endif

    /* Validate lt slice */
    rv = _field_th_group_lt_slice_validate(unit, stage_fc, fg, slice_id, NULL);

    return rv;
}

/*
 * Function:
 *    _field_hx5_ft_group_key_type_clear
 *
 * Purpose:
 *    Clears group key type assignment in field group in flowtracker stage.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    stage_fc   - (IN) Stage field control structure.
 *    fg         - (IN) Field group control structure.
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_INTERNAL    - Invalid CAP Stage ID.
 *    BCM_E_NONE        - Success.
 */
int
_field_hx5_ft_group_keytype_clear(int unit,
                                   _field_stage_t *stage_fc,
                                   _field_group_t *fg)
{
    int keytype = 0;
    int slice_num = 0;
    _field_ft_info_t *ft_info = NULL;

    if ((stage_fc == NULL) || (fg == NULL)) {
        return BCM_E_PARAM;;
    }

    if (fg->stage_id != _BCM_FIELD_STAGE_FLOWTRACKER) {
        return (BCM_E_NONE);
    }

    if (stage_fc->ft_info == NULL) {
        return BCM_E_INTERNAL;
    }

    ft_info = stage_fc->ft_info;
    slice_num = fg->slices->slice_number;

    keytype = (fg->lt_id >> _FP_GROUP_FT_KEYTYPE_SHIFT) &
        (_FP_GROUP_FT_KEYTYPE_MASK);
    SHR_BITCLR(ft_info->key_type_bmp[slice_num], keytype);

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_hx5_ft_group_keytype_assign
 *
 * Purpose:
 *    Group key type to field group in flowtracker stage.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    stage_fc   - (IN) Stage field control structure.
 *    fg         - (IN) Field group control structure.
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_INTERNAL    - Invalid CAP Stage ID.
 *    BCM_E_NONE        - Success.
 */
int
_field_hx5_ft_group_keytype_assign(int unit,
                                   _field_stage_t *stage_fc,
                                   _field_group_t *fg)
{
    int keytype = 0;
    int double_wide = 0;
    int slice_num = 0;
    _field_ft_info_t *ft_info = NULL;

    /* Validate input params */
    if ((stage_fc == NULL) || (fg == NULL)) {
        return BCM_E_PARAM;
    }

    /* Applicable only in flowtracker stage. */
    if (fg->stage_id != _BCM_FIELD_STAGE_FLOWTRACKER) {
        return (BCM_E_NONE);
    }

    ft_info = stage_fc->ft_info;
    if (ft_info == NULL) {
        return BCM_E_INTERNAL;
    }
    slice_num = fg->slices->slice_number;

    /* Unique value for each group */
    for (keytype = 0;
         keytype < _BCM_FIELD_FT_GROUP_KEY_TYPE_MAX;
         keytype++) {
        if (!SHR_BITGET(stage_fc->ft_info->key_type_bmp[slice_num], keytype)) {
            SHR_BITSET(stage_fc->ft_info->key_type_bmp[slice_num], keytype);
            break;
        }
    }
    if (keytype == _BCM_FIELD_FT_GROUP_KEY_TYPE_MAX) {
        return BCM_E_RESOURCE;
    }

    if (fg->flags & _FP_GROUP_DW_DEPTH_EXPANDED) {
        double_wide = 1;
    }

    fg->lt_id = (keytype << _FP_GROUP_FT_KEYTYPE_SHIFT) | double_wide;

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_hx5_ft_group_keytype_bmp_recover
 *
 * Purpose:
 *    Recover keytype bitmap in flowtracker stage.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    fg         - (IN) Field group control structure.
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_INTERNAL    - Invalid CAP Stage ID.
 *    BCM_E_NONE        - Success.
 */
int
_field_hx5_ft_group_keytype_bmp_recover(int unit,
                                        _field_stage_t *stage_fc,
                                        _field_group_t *fg)
{
    int keytype = 0;
    int slice_num = 0;

    /* Validate input params */
    if ((stage_fc == NULL) || (fg == NULL)) {
        return BCM_E_PARAM;
    }

    /* Applicable only in flowtracker stage. */
    if (fg->stage_id != _BCM_FIELD_STAGE_FLOWTRACKER) {
        return (BCM_E_NONE);
    }

    slice_num = fg->slices->slice_number;
    keytype = fg->lt_id >> _FP_GROUP_FT_KEYTYPE_SHIFT;
    keytype &= _FP_GROUP_FT_KEYTYPE_MASK;

    SHR_BITSET(stage_fc->ft_info->key_type_bmp[slice_num], keytype);

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_hx5_ft_entries_free_get
 *
 * Purpose:
 *    Return number of free entries in slice for given group.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    fs         - (IN) field slice structure.
 *    fg         - (IN) Field group control structure.
 *    free_cnt   - (OUT) Out argument for number of free entries.
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_INTERNAL    - Invalid CAP Stage ID.
 *    BCM_E_NONE        - Success.
 */
int
_field_hx5_ft_entries_free_get(int unit,
                               _field_slice_t *fs,
                               _field_group_t *fg,
                               int *free_cnt)
{
    /* Input Paramter Check. */
    if ((fg == NULL) || (fs == NULL) || (free_cnt == NULL)) {
        return (BCM_E_PARAM);
    }

    /* Initialize free entry count. */
    *free_cnt = 0;

    /* Calculate free entry count. */
    if (fg->flags & _FP_GROUP_DW_DEPTH_EXPANDED) {
        *free_cnt = fs->free_count >> 1;
    } else {
        *free_cnt = fs->free_count;
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *    _bcm_field_hx5_ft_entry_keytype_set
 *
 * Purpose:
 *    Qualify entry in flowtracker stage with group keytype
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    f_ent      - (IN) Field entry structure.
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_INTERNAL    - Invalid CAP Stage ID.
 *    BCM_E_NONE        - Success.
 */
int
_bcm_field_hx5_ft_entry_keytype_set(int unit,
                                    _field_entry_t *f_ent)
{
    int rv = BCM_E_NONE;
    uint32 lt_id_mask = 0xFF;
    _bcm_field_qual_offset_t q_offset;

    if (f_ent == NULL) {
        return BCM_E_PARAM;
    }

    if (f_ent->group->lt_id == -1) {
        return BCM_E_INTERNAL;
    }

    sal_memset(&q_offset, 0,
            sizeof (_bcm_field_qual_offset_t));

    q_offset.field = KEYf;
    q_offset.num_offsets = 1;
    q_offset.offset[0] = 0;
    q_offset.width[0] = 8;
    q_offset.qual_width = 8;

    BCM_IF_ERROR_RETURN(_bcm_field_qual_value_set(unit, &q_offset,
            f_ent, (uint32 *)&f_ent->group->lt_id, &lt_id_mask));

    return rv;
}

/*
 * Function:
 *    _field_hx5_ft_entry_policy_mem_install
 *
 * Purpose:
 *    Installs policy hw table for entry.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    f_ent      - (IN) field entry structure.
 *    policy_mem - (IN) policy hw table
 *    tcam_idx   - (IN) Index in policy table
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_INTERNAL    - Invalid CAP Stage ID.
 *    BCM_E_NONE        - Success.
 */
int
_field_hx5_ft_entry_policy_mem_install(int unit,
                                       _field_entry_t *f_ent,
                                       soc_mem_t policy_mem,
                                       int tcam_idx)
{
    int rv = BCM_E_NONE;
    uint32 e_buf[SOC_MAX_MEM_FIELD_WORDS] = {0};

    /* Validate input params */
    if ((f_ent == NULL) || (policy_mem == INVALIDm) || (tcam_idx == -1)) {
        return BCM_E_PARAM;
    }

    /* Read policy memory */
    rv = soc_mem_read(unit, policy_mem, MEM_BLOCK_ANY, tcam_idx, e_buf);
    BCM_IF_ERROR_RETURN(rv);

    /* Set policy buffer */
    rv = _field_hx5_ft_entry_policy_set(unit, f_ent, policy_mem, e_buf);
    BCM_IF_ERROR_RETURN(rv);

    /* Write to hardware */
    rv = soc_mem_write(unit, policy_mem, MEM_BLOCK_ALL, tcam_idx, e_buf);
    return rv;

}

/*
 * Function:
 *    _field_ft_entry_tcam_policy_install
 *
 * Purpose:
 *    Installs tcam + policy hw tables for entry.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    f_ent      - (IN) field entry structure.
 *    tcam_idx   - (IN) Index in tcam table
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_INTERNAL    - Invalid CAP Stage ID.
 *    BCM_E_NONE        - Success.
 */
int
_field_hx5_ft_entry_tcam_policy_install(int unit,
                                        _field_entry_t *f_ent,
                                        int tcam_idx)
{
    int rv = BCM_E_NONE;
    soc_mem_t tcam_mem = INVALIDm;
    soc_mem_t policy_mem = INVALIDm;
    uint32 entry_buf[SOC_MAX_MEM_FIELD_WORDS] = {0};

    if (f_ent == NULL) {
        return (BCM_E_PARAM);
    }

    /* Get the combo TCAM+Policy */
    rv = _bcm_field_th_tcam_policy_mem_get(unit, f_ent,
            &tcam_mem, &policy_mem);
    BCM_IF_ERROR_RETURN(rv);

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    /* Convert tcam_idx to hw index */
    rv = _bcm_field_fb6_entry_sw_to_hw_tcam_idx(unit,
            f_ent->fs, tcam_idx, &tcam_idx);
    BCM_IF_ERROR_RETURN(rv);
#endif

    if ((tcam_idx < soc_mem_index_min(unit, tcam_mem)) ||
        (tcam_idx > soc_mem_index_max(unit, tcam_mem))) {
        return (BCM_E_PARAM);
    }

    rv = soc_mem_read(unit, tcam_mem, MEM_BLOCK_ANY,
                tcam_idx, entry_buf);
    BCM_IF_ERROR_RETURN(rv);

    /* Set Policy data buffer */
    rv = _field_hx5_ft_entry_policy_mem_install(unit, f_ent, policy_mem, tcam_idx);
    if (BCM_FAILURE(rv))  {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META(
                "Failed to Install Policy Table for Entry[%d]"
                " tcam_idx:[%d].\n\r"),
                f_ent->eid, tcam_idx));
        return rv;
    }

    /* Set Qualifier for TCAM Table */
    rv = _field_hx5_ft_entry_qual_tcam_set(unit, f_ent, tcam_mem,
                    tcam_idx, entry_buf);

    /* Write to h/w Table */
    if (BCM_SUCCESS(rv)) {
        rv = soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL, tcam_idx, entry_buf);
    }

    if (BCM_SUCCESS(rv)) {

        /* Increment the HW Slice entry count */
        f_ent->fs->hw_ent_count++;

        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "==> %s(): Entry[%d] Installed - mem:%d tcam_idx:%d slice:%d\n\r"),
            __func__, f_ent->eid, tcam_mem, tcam_idx, f_ent->fs->slice_number));
    } else {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META(
                 "Failed to Install the TCAM Entry[%d] tcam_idx:[%d].\n\r"),
                 f_ent->eid, tcam_idx));
    }

    return rv;
}

/*
 * Function:
 *    _bcm_field_hx5_ft_entry_move
 *
 * Purpose:
 *    Move tcam entry to new index
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    f_ent      - (IN) field entry structure.
 *    parts_count- (IN) Number of parts of entry.
 *    sw_cam_idx_old- (IN) Old tcam Indices of entry
 *    sw_tcam_idx_new- (IN) New tcam Indices of entry
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_INTERNAL    - Invalid CAP Stage ID.
 *    BCM_E_NONE        - Success.
 */
int
_bcm_field_hx5_ft_entry_move(int unit,
                             _field_entry_t *f_ent,
                             int parts_count,
                             int *sw_tcam_idx_old,
                             int *sw_tcam_idx_new)
{
    int rv = BCM_E_NONE;
    int part = 0;
    int is_backup_entry = 0;
    soc_mem_t tcam_mem = INVALIDm;
    soc_mem_t policy_mem = INVALIDm;
    int new_slice_numb = 0;
    int new_slice_idx = 0;
    int tcam_idx_min = 0, tcam_idx_max = 0;
    int hw_tcam_idx_old[_FP_MAX_ENTRY_WIDTH];
    int hw_tcam_idx_new[_FP_MAX_ENTRY_WIDTH];
    uint32 entry_buf[SOC_MAX_MEM_FIELD_WORDS] = {0};
    _field_slice_t *fs = NULL;
    _field_stage_t *stage_fc = NULL;

    if ((f_ent == NULL) || (sw_tcam_idx_old == NULL) || (sw_tcam_idx_new == NULL)) {
        return (BCM_E_PARAM);
    }

    rv = _field_stage_control_get(unit, f_ent->group->stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    /* Get entry tcam and actions. */
    rv = _bcm_field_th_tcam_policy_mem_get(unit, f_ent,
            &tcam_mem, &policy_mem);
    BCM_IF_ERROR_RETURN(rv);

    /* Sanity check for index */
    tcam_idx_max = soc_mem_index_max(unit, tcam_mem);
    tcam_idx_min = soc_mem_index_min(unit, tcam_mem);

    for (part = 0; part < parts_count; part++) {

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
        rv = _bcm_field_fb6_entry_sw_to_hw_tcam_idx(unit,
                f_ent->fs, sw_tcam_idx_old[part], &hw_tcam_idx_old[part]);
        BCM_IF_ERROR_RETURN(rv);

        rv = _bcm_field_fb6_entry_sw_to_hw_tcam_idx(unit,
                f_ent->fs, sw_tcam_idx_new[part], &hw_tcam_idx_new[part]);
        BCM_IF_ERROR_RETURN(rv);
#else
        hw_tcam_idx_old[part] = sw_tcam_idx_old[part];
        hw_tcam_idx_new[part] = sw_tcam_idx_new[part];
#endif
        if ((hw_tcam_idx_old[part] > tcam_idx_max) ||
            (hw_tcam_idx_old[part] < tcam_idx_min) ||
            (hw_tcam_idx_new[part] > tcam_idx_max) ||
            (hw_tcam_idx_new[part] < tcam_idx_min)) {
            return (BCM_E_PARAM);
        }
    }

    if ((NULL != f_ent->ent_copy) &&
            (f_ent->ent_copy->eid ==  _FP_INTERNAL_RESERVED_ID)) {
        is_backup_entry = 1;
    }

    /* Write to destination */
    /* In HR4 due to hw atomicity write feature for BSK_FTFP_TCAM
     * memory even index should be written first followed by odd index.
     * In FTFP double wide entry case first entry part has even index
     * and second part has odd endex. Hence looping through first
     * entry part to end. Controlled by soc feature
     * soc_feature_flowtracker_ver_2_ftfp_tcam_atomic_hw_write.
     */
    if (soc_feature(unit,
                soc_feature_flowtracker_ver_2_ftfp_tcam_atomic_hw_write)) {
        for (part = 0; part < parts_count; part++) {
            if (part == 0) {
                sal_memset(entry_buf, 0,
                        sizeof(uint32) * SOC_MAX_MEM_FIELD_WORDS);

                rv = soc_mem_read(unit, policy_mem, MEM_BLOCK_ANY,
                        hw_tcam_idx_old[part], entry_buf);
                BCM_IF_ERROR_RETURN(rv);

                /* Write duplicate  tcam entry to new tcam index. */
                rv = soc_mem_write(unit, policy_mem, MEM_BLOCK_ALL,
                        hw_tcam_idx_new[part], entry_buf);
                BCM_IF_ERROR_RETURN(rv);
            }

            sal_memset(entry_buf, 0,
                    sizeof(uint32) * SOC_MAX_MEM_FIELD_WORDS);

            rv = soc_mem_read(unit, tcam_mem, MEM_BLOCK_ANY,
                    hw_tcam_idx_old[part], entry_buf);
            BCM_IF_ERROR_RETURN(rv);

            /* Write duplicate  tcam entry to new tcam index. */
            rv = soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL,
                    hw_tcam_idx_new[part], entry_buf);
            BCM_IF_ERROR_RETURN(rv);
        }

    } else {
        for (part = parts_count - 1; part >= 0; part--) {
            if (part == 0) {
                sal_memset(entry_buf, 0,
                    sizeof(uint32) * SOC_MAX_MEM_FIELD_WORDS);

                rv = soc_mem_read(unit, policy_mem, MEM_BLOCK_ANY,
                        hw_tcam_idx_old[part], entry_buf);
                BCM_IF_ERROR_RETURN(rv);

                /* Write duplicate  tcam entry to new tcam index. */
                rv = soc_mem_write(unit, policy_mem, MEM_BLOCK_ALL,
                        hw_tcam_idx_new[part], entry_buf);
                BCM_IF_ERROR_RETURN(rv);
            }

            sal_memset(entry_buf, 0,
                sizeof(uint32) * SOC_MAX_MEM_FIELD_WORDS);

            rv = soc_mem_read(unit, tcam_mem, MEM_BLOCK_ANY,
                    hw_tcam_idx_old[part], entry_buf);
            BCM_IF_ERROR_RETURN(rv);

            /* Write duplicate  tcam entry to new tcam index. */
            rv = soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL,
                    hw_tcam_idx_new[part], entry_buf);
            BCM_IF_ERROR_RETURN(rv);
        }
    }

    for (part = 0; part < parts_count; part++) {
        /* Calculate entry new slice & offset in the slice. */
        rv = _bcm_field_tcam_idx_to_slice_offset(unit, stage_fc,
                f_ent->group->instance,
                sw_tcam_idx_new[part],
                &new_slice_numb,
                &new_slice_idx);
        BCM_IF_ERROR_RETURN(rv);
        fs = &stage_fc->slices[f_ent->group->instance][new_slice_numb];
        fs->hw_ent_count++;

       /*
        * fp_entry_move is invoked even for backup entry create
        * to copy TCAM and FP_POLICY_TABLE tables. In this case
        * TCAM and FP_POLICY_TABLEs should not be erased for
        * original entry.
        */
       if (1 != is_backup_entry) {
           /*
            * Clear old location
            */
           rv = _bcm_field_th_tcam_policy_clear(unit, f_ent,
                   sw_tcam_idx_old[part]);
           BCM_IF_ERROR_RETURN(rv);
       }
    }

    return rv;
}

/*
 * Function:
 *    _bcm_field_hx5_ft_tcam_policy_clear
 *
 * Purpose:
 *    Clears hw table at given index.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    f_ent      - (IN) field entry structure.
 *    tcam_idx   - (IN) tcam index.
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_INTERNAL    - Invalid CAP Stage ID.
 *    BCM_E_NONE        - Success.
 */
int
_bcm_field_hx5_ft_tcam_policy_clear(int unit,
                                    _field_entry_t *f_ent,
                                    int tcam_idx)
{
    int rv = BCM_E_NONE;
    soc_mem_t tcam_mem = INVALIDm;
    soc_mem_t policy_mem = INVALIDm;
    uint32 null_buf[SOC_MAX_MEM_FIELD_WORDS] = {0};

    rv = _bcm_field_th_tcam_policy_mem_get(unit, f_ent,
            &tcam_mem, &policy_mem);
    BCM_IF_ERROR_RETURN(rv);

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    rv = _bcm_field_fb6_entry_sw_to_hw_tcam_idx(unit,
            f_ent->fs, tcam_idx, &tcam_idx);
    BCM_IF_ERROR_RETURN(rv);
#endif

    if (tcam_idx > soc_mem_index_max(unit, tcam_mem)) {
        return BCM_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, tcam_mem,
                MEM_BLOCK_ALL, tcam_idx, null_buf));

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, policy_mem,
                MEM_BLOCK_ALL, tcam_idx, null_buf));
    return rv;
}

/*
 * Function:
 *    _field_hx5_ft_stage_entry_enable_set
 *
 * Purpose:
 *     Enable/Disable an entry from the hardware tables.
 *
 * Parameters:
 *     unit        - (IN) BCM device number
 *     f_ent       - (IN) Reference to Entry structure
 *     enable_flag - (IN) Flag to enable or disable
 *
 * Returns:
 *     BCM_E_XXX
 *
 * Notes:
 *     This does not destroy the entry, nor deallocate any related resources;
 *     it only enables/disables a rule from hardware table using VALIDf of the
 *     corresponding hardware entry. To deallocate the memory used by the entry
 *     call bcm_field_entry_destroy.
 */
int
_field_hx5_ft_stage_entry_enable_set(int unit,
                                     _field_entry_t *f_ent,
                                     int enable_flag)
{
    int rv = BCM_E_NONE;
    int part = 0, parts_count = 0;
    _field_group_t *fg = NULL;

    fg = f_ent->group;

    /* Get number of parts */
    rv = _bcm_field_entry_tcam_parts_count (unit, fg->stage_id,
                                            fg->flags, &parts_count);
    for (part = 0; part < parts_count; part++) {
        rv = _field_th_stage_entry_enable_set(unit, &f_ent[part], enable_flag);
    }

    return rv;
}

/*
 * Function:
 *    _bcm_field_hx5_ft_action_params_check
 *
 * Purpose:
 *    Verifies action to be associated with entry.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    f_ent      - (IN) field entry structure.
 *    fa         - (IN) field action structure.
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_INTERNAL    - Invalid CAP Stage ID.
 *    BCM_E_NONE        - Success.
 */
int
_bcm_field_hx5_ft_action_params_check(int unit,
                                     _field_entry_t *f_ent,
                                     _field_action_t *fa)
{
    int rv = BCM_E_NONE;
    bcm_flowtracker_group_t id = -1;
    _field_action_t *fa_iter = NULL;
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    int flow_index;
#endif

    if ((f_ent->group->stage_id != _BCM_FIELD_STAGE_FLOWTRACKER) &&
            (f_ent->group->stage_id != _BCM_FIELD_STAGE_AMFTFP)  &&
            (f_ent->group->stage_id != _BCM_FIELD_STAGE_AEFTFP)) {
        return BCM_E_NONE;
    }

    fa_iter = f_ent->actions;
    while(fa_iter != NULL) {
        if (fa_iter->flags & _FP_ACTION_VALID) {
            if ((fa->action == bcmFieldActionFlowtrackerNewLearnEnable) &&
                    (fa_iter->action == bcmFieldActionFlowtrackerGroupId)) {
                id = fa_iter->param[0];
                if (BCMI_FT_GROUP(unit, id)->flags
                        & BCMI_FT_GROUP_INFO_F_USER_ENTRIES_ONLY) {
                    return BCM_E_CONFIG;
                }
            } else if ((fa->action == bcmFieldActionFlowtrackerGroupId) &&
                    (fa_iter->action == bcmFieldActionFlowtrackerNewLearnEnable)) {
                id = fa->param[0];
                if (BCMI_FT_GROUP(unit, id)->flags
                        & BCMI_FT_GROUP_INFO_F_USER_ENTRIES_ONLY) {
                    return BCM_E_CONFIG;
                }
            }
        }

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
        if (soc_feature(unit, soc_feature_field_flowtracker_v3_support)) {
            if ((fa->action ==
                        bcmFieldActionFlowtrackerAggregateIngressFlowIndex) &&
                    (fa_iter->action ==
                     bcmFieldActionFlowtrackerAggregateIngressGroupId)) {

                if (fa->param[0] != fa_iter->param[0]) {
                    return BCM_E_CONFIG;
                }
            } else if ((fa->action ==
                        bcmFieldActionFlowtrackerAggregateIngressGroupId) &&
                    (fa_iter->action ==
                     bcmFieldActionFlowtrackerAggregateIngressFlowIndex)) {
                if (fa->param[0] != fa_iter->param[0]) {
                    return BCM_E_CONFIG;
                }
            }
            if ((fa->action ==
                        bcmFieldActionFlowtrackerAggregateMmuFlowIndex) &&
                    (fa_iter->action ==
                     bcmFieldActionFlowtrackerAggregateMmuGroupId)) {

                if (fa->param[0] != fa_iter->param[0]) {
                    return BCM_E_CONFIG;
                }
            } else if ((fa->action ==
                        bcmFieldActionFlowtrackerAggregateMmuGroupId) &&
                    (fa_iter->action ==
                     bcmFieldActionFlowtrackerAggregateMmuFlowIndex)) {
                if (fa->param[0] != fa_iter->param[0]) {
                    return BCM_E_CONFIG;
                }
            }

            if ((fa->action ==
                        bcmFieldActionFlowtrackerAggregateEgressFlowIndex) &&
                    (fa_iter->action ==
                     bcmFieldActionFlowtrackerAggregateEgressGroupId)) {

                if (fa->param[0] != fa_iter->param[0]) {
                    return BCM_E_CONFIG;
                }
            } else if ((fa->action ==
                        bcmFieldActionFlowtrackerAggregateEgressGroupId) &&
                    (fa_iter->action ==
                     bcmFieldActionFlowtrackerAggregateEgressFlowIndex)) {
                if (fa->param[0] != fa_iter->param[0]) {
                    return BCM_E_CONFIG;
                }
            }
        }
#endif /* BCM_FLOWTRACKER_V3_SUPPORT */
        fa_iter = fa_iter->next;
    }

    switch(fa->action) {
        case bcmFieldActionFlowtrackerGroupId:
            id = fa->param[0];

            /* Check FT module compliance */
            rv = bcmi_esw_ft_group_entry_add_check(unit,
                     bcmFlowtrackerGroupTypeNone, id);
            break;

        case bcmFieldActionFlowtrackerEnable:
        case bcmFieldActionFlowtrackerNewLearnEnable:
            if (fa->param[0] > 1) {
                LOG_ERROR(BSL_LS_BCM_FP,
                        (BSL_META_U(unit, "FP(unit %d) Error: Incorrect"
                                    " argument passed.\r\n"), unit));
                rv = BCM_E_PARAM;
            }
            break;
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
        case bcmFieldActionFlowtrackerAggregateIngressGroupId:
            id = fa->param[0];

            /* Check FT module compliance */
            rv = bcmi_esw_ft_group_entry_add_check(unit,
                    bcmFlowtrackerGroupTypeAggregateIngress, id);
            break;

        case bcmFieldActionFlowtrackerAggregateMmuGroupId:
            id = fa->param[0];

            /* Check FT module compliance */
            rv = bcmi_esw_ft_group_entry_add_check(unit,
                    bcmFlowtrackerGroupTypeAggregateMmu, id);
            break;

        case bcmFieldActionFlowtrackerAggregateEgressGroupId:
            id = fa->param[0];

            /* Check FT module compliance */
            rv = bcmi_esw_ft_group_entry_add_check(unit,
                    bcmFlowtrackerGroupTypeAggregateEgress, id);
            break;

        case bcmFieldActionFlowtrackerAggregateClass:

            /* Check FT module compliance */
            rv = bcmi_esw_ftv3_aggregate_class_entry_add_check(unit,
                        fa->param[0]);
            break;
        case bcmFieldActionFlowtrackerAggregateIngressFlowIndex:
            id = fa->param[0];
            flow_index = fa->param[1];

            /* Check FT module compliance */
            rv = bcmi_esw_ftv3_user_entry_entry_add_check(unit,
                        bcmFlowtrackerGroupTypeAggregateIngress,
                        id, flow_index);
            break;

        case bcmFieldActionFlowtrackerAggregateMmuFlowIndex:
            id = fa->param[0];
            flow_index = fa->param[1];

            /* Check FT module compliance */
            rv = bcmi_esw_ftv3_user_entry_entry_add_check(unit,
                        bcmFlowtrackerGroupTypeAggregateMmu,
                        id, flow_index);
            break;

        case bcmFieldActionFlowtrackerAggregateEgressFlowIndex:
            id = fa->param[0];
            flow_index = fa->param[1];

            /* Check FT module compliance */
            rv = bcmi_esw_ftv3_user_entry_entry_add_check(unit,
                        bcmFlowtrackerGroupTypeAggregateEgress,
                        id, flow_index);
            break;

#endif /* BCM_FLOWTRACKER_V3_SUPPPORT */
        default:
            break;
    }

    return rv;
}

/*
 * Function:
 *    _bcm_field_hx5_ft_group_aset_update
 *
 * Purpose:
 *    Updates aset of field group.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    fg         - (IN) field group structure.
 *    aset       - (IN) New action set structure.
 *
 * Returns:
 *    BCM_E_PARAM       - Wrong input argument.
 *    BCM_E_UNAVAIL     - Incoming Action is not supported.
 *    BCM_E_NONE        - Success.
 */
int
_bcm_field_hx5_ft_group_aset_update(int unit,
                                    _field_group_t *fg,
                                    bcm_field_aset_t *aset)
{
    int a_idx = 0;
    int aset_diff = FALSE;
    _bcm_field_aset_t aset_modified;

    if ((fg == NULL) || (aset == NULL)) {
        return BCM_E_PARAM;
    }

    /* Copy original aset of field group */
    sal_memcpy(&aset_modified, &fg->aset, sizeof(_bcm_field_aset_t));

    /* Set new actions to aset */
    for (a_idx = 0; a_idx < bcmFieldActionCount; a_idx++) {
        if (BCM_FIELD_ASET_TEST(*aset, a_idx)) {
            if (!_field_stage_action_support_check(unit, fg, a_idx)) {
                LOG_ERROR(BSL_LS_BCM_FP,
                        (BSL_META_U(unit, "FP(unit %d) Error: action =%s is"
                                    " not supported for given group stage.\n"),
                         unit, _field_action_name(a_idx)));
                return BCM_E_UNAVAIL;
            }
            if (!BCM_FIELD_ASET_TEST(fg->aset, a_idx)) {
                BCM_FIELD_ASET_ADD(aset_modified, a_idx);
                aset_diff = TRUE;
            }
        }
    }

    /* Copy modified aset to field group aset */
    if (aset_diff == TRUE) {
        sal_memcpy(&fg->aset, &aset_modified, sizeof(_bcm_field_aset_t));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_field_hx5_ft_group_hw_alloc
 *
 * Purpose:
 *    If flowtracker group id is associated with the entry,
 *    flowtracker APIs are called to install SESSION_KEY/DATA,
 *    flowtracker ALUs and other hardware tables.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    f_ent      - (IN) Field entry structure.
 *    fa         - (IN) Field action structure.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_hx5_ft_group_hw_alloc(int unit,
                                _field_entry_t *f_ent,
                                _field_action_t *fa)
{
    int rv = BCM_E_NONE;
    int num_data_info = 0;
    int ref_count;
    bcm_flowtracker_group_t id;

    fa->old_index = fa->hw_index;
    id = fa->param[0];

    /* Get reference count */
    BCM_IF_ERROR_RETURN(
            bcmi_esw_ft_group_param_retrieve(unit, id,
                bcmiFtGroupParamFtfpEntryNum, &ref_count));
    if (ref_count == 0) {
        /* Check Ft Group tracking params extraction */
        rv = bcmi_esw_ft_group_extraction_alu_info_get(unit,
                id, 0, 0, NULL, &num_data_info);
        if ((rv != BCM_E_NONE) || (num_data_info == 0)) {
            return BCM_E_CONFIG;
        }
    }
    /* Increment reference count */
    if (BCM_SUCCESS(rv)) {
        rv = bcmi_esw_ft_group_param_update(unit, id,
                bcmiFtGroupParamFtfpEntryNum,
                bcmiFtGroupParamsOprInc, 1);
    }
    fa->hw_index = id;

    return (rv);
}

/*
 * Function:
 *    _bcm_field_hx5_ft_actions_hw_alloc
 *
 * Purpose:
 *    If flowtracker group id is associated with the entry,
 *    flowtracker APIs are called to install SESSION_KEY/DATA,
 *    flowtracker ALUs and other hardware tables.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    f_ent      - (In) Field entry structure.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_hx5_ft_actions_hw_alloc(int unit,
                                _field_entry_t *f_ent)
{
    int rv = BCM_E_NONE;
    _field_action_t *fa = NULL;

    if ((f_ent->group->stage_id != _BCM_FIELD_STAGE_FLOWTRACKER) &&
            (f_ent->group->stage_id != _BCM_FIELD_STAGE_AMFTFP)  &&
            (f_ent->group->stage_id != _BCM_FIELD_STAGE_AEFTFP)) {
        return BCM_E_NONE;
    }

     for (fa = f_ent->actions;
            (fa != NULL) && (fa->flags & _FP_ACTION_VALID);
            fa = fa->next) {
         if (!(fa->flags & _FP_ACTION_DIRTY)) {
             continue;
         }

        switch(fa->action) {
            case bcmFieldActionFlowtrackerGroupId:
                rv = _bcm_field_hx5_ft_group_hw_alloc(unit, f_ent, fa);
                break;
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
            case bcmFieldActionFlowtrackerAggregateIngressGroupId:
            case bcmFieldActionFlowtrackerAggregateMmuGroupId:
            case bcmFieldActionFlowtrackerAggregateEgressGroupId:
                if (soc_feature(unit,
                            soc_feature_field_flowtracker_v3_support)) {
                    rv = _bcm_field_hx5_ft_group_hw_alloc(unit, f_ent, fa);
                }
                break;
            case bcmFieldActionFlowtrackerAggregateIngressFlowIndex:
            case bcmFieldActionFlowtrackerAggregateMmuFlowIndex:
            case bcmFieldActionFlowtrackerAggregateEgressFlowIndex:
                if (soc_feature(unit,
                            soc_feature_field_flowtracker_v3_support)) {
                    rv = _bcm_field_fb6_ft_user_entry_hw_alloc(unit,
                            f_ent, fa);
                }
                break;
            case bcmFieldActionFlowtrackerAggregateClass:
                if (soc_feature(unit,
                            soc_feature_field_flowtracker_v3_support)) {
                    rv = _bcm_field_fb6_ft_aggregate_class_hw_alloc(unit,
                            f_ent, fa);
                }
                break;
#endif
            default:
                rv = BCM_E_NONE;
                break;
        }

        BCM_IF_ERROR_RETURN(rv);
    }

    return rv;
}

/*
 * Function:
 *    _bcm_field_hx5_ft_group_hw_free
 *
 * Purpose:
 *    If flowtracker group id is dis-associated from the entry,
 *    flowtracker APIs are called to clear SESSION_KEY/DATA,
 *    flowtracker ALUs and other hardware tables.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    f_ent      - (IN) Field entry structure.
 *    fa         - (IN) Field action structure.
 *    flags      - (IN) Flags
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_hx5_ft_group_hw_free(int unit,
                            _field_entry_t *f_ent,
                            _field_action_t *fa,
                            int flags)
{
    int rv = BCM_E_NONE;
    bcm_flowtracker_group_t id;

    if ((flags & _FP_ACTION_OLD_RESOURCE_FREE) &&
            (fa->old_index != _FP_INVALID_INDEX)) {
        id = fa->old_index;
        /* Decrement reference count */
        BCM_IF_ERROR_RETURN(
                bcmi_esw_ft_group_param_update(unit,
                    id, bcmiFtGroupParamFtfpEntryNum,
                    bcmiFtGroupParamsOprDec, 1));
        fa->old_index = _FP_INVALID_INDEX;
    }
    if ((flags & _FP_ACTION_RESOURCE_FREE) &&
            (fa->hw_index != _FP_INVALID_INDEX)) {
        id = fa->hw_index;
        /* Decrement reference count */
        BCM_IF_ERROR_RETURN(
                bcmi_esw_ft_group_param_update(unit, id,
                    bcmiFtGroupParamFtfpEntryNum,
                    bcmiFtGroupParamsOprDec, 1));
        if (!(flags & _BCM_FIELD_ACTION_REF_STALE)) {
            fa->hw_index = _FP_INVALID_INDEX;
        }
    }
    if (flags & _BCM_FIELD_ACTION_REF_STALE) {
        fa->flags |= _BCM_FIELD_ACTION_REF_STALE;
    }
    return rv;
}

/*
 * Function:
 *    _bcm_field_hx5_ft_actions_hw_free
 *
 * Purpose:
 *    If flowtracker group id is dis-associated from the entry,
 *    flowtracker APIs are called to clear SESSION_KEY/DATA,
 *    flowtracker ALUs and other hardware tables.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    f_ent      - (In) Field entry structure.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_hx5_ft_actions_hw_free(int unit,
                                _field_entry_t *f_ent,
                                int flags)
{
    int rv = BCM_E_NONE;
    _field_action_t *fa = NULL;

    if ((f_ent->group->stage_id != _BCM_FIELD_STAGE_FLOWTRACKER) &&
            (f_ent->group->stage_id != _BCM_FIELD_STAGE_AMFTFP)  &&
            (f_ent->group->stage_id != _BCM_FIELD_STAGE_AEFTFP)) {
        return BCM_E_NONE;
    }


    for (fa = f_ent->actions; NULL != fa; fa = fa->next) {
        if (fa->flags & _BCM_FIELD_ACTION_REF_STALE) {
            /* Field Action is Stale */
            continue;
        }

        switch(fa->action) {
            case bcmFieldActionFlowtrackerGroupId:
                rv = _bcm_field_hx5_ft_group_hw_free(unit,
                        f_ent, fa, flags);
                break;
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
            case bcmFieldActionFlowtrackerAggregateIngressGroupId:
            case bcmFieldActionFlowtrackerAggregateMmuGroupId:
            case bcmFieldActionFlowtrackerAggregateEgressGroupId:
                rv = _bcm_field_hx5_ft_group_hw_free(unit,
                        f_ent, fa, flags);
                break;
            case bcmFieldActionFlowtrackerAggregateIngressFlowIndex:
            case bcmFieldActionFlowtrackerAggregateMmuFlowIndex:
            case bcmFieldActionFlowtrackerAggregateEgressFlowIndex:
                rv = _bcm_field_fb6_ft_user_entry_hw_free(unit,
                        f_ent, fa, flags);
                break;
            case bcmFieldActionFlowtrackerAggregateClass:
                rv = _bcm_field_fb6_ft_aggregate_class_hw_free(unit,
                        f_ent, fa, flags);
                break;
#endif
            default:
                rv = BCM_E_NONE;
                break;
        }

        BCM_IF_ERROR_RETURN(rv);
     }

    return rv;
}

/*
 * Function:
 *    _bcm_field_hx5_hint_dosattack_event_flags
 *
 * Purpose:
 *    Update qual_info for dos attack events when hint
 *    is provided.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    hint_node  - (IN) hint structure.
 *    qual_info  - (IN/OUT) Qualifier information for extraction.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_hx5_dosattack_event_qual_bitmap(int unit,
                    _field_hint_t *hint_node,
                    bcmi_keygen_qual_info_t *qual_info)
{
    uint32 event_flags[1];
    uint32 event_bit = 0, bit_pos = 0;
    soc_field_t field = INVALIDf;
    soc_format_t fmt = INVALIDfmt;
    soc_field_info_t *field_info = NULL;

    if ((qual_info == NULL) || (hint_node == NULL)) {
        return BCM_E_PARAM;
    }

    event_flags[0] = hint_node->hint->dosattack_event_flags;

    SHR_BIT_ITER(event_flags, 32, event_bit) {
        field = INVALIDf;
        switch((1 << event_bit)) {
            case BCM_FIELD_DOSATTACK_MACSA_DA_SAME:
                field = MACSA_EQ_MACDAf;
                break;
            case BCM_FIELD_DOSATTACK_IPFRAG_OFFSET:
                field = IP_FRAG_OFFSET_VALUE_OF_1f;
                break;
            case BCM_FIELD_DOSATTACK_V6_TYPE0_ROUTING_HDR:
                field = TYPE_ZERO_ROUTING_HDR_PRESENTf;
                break;
            case BCM_FIELD_DOSATTACK_V6_NON_LAST_FRAG_LT_1280:
                field = V6_NON_LAST_FRAGMENT_SIZE_TOO_SMALLf;
                break;
            case BCM_FIELD_DOSATTACK_FIRST_TCPIP_FRAG_SMALL:
                field = TCP_HDR_TOO_SMALLf;
                break;
            case BCM_FIELD_DOSATTACK_TCP_OFFSET:
                field = TCP_FRAG_OFFSET_EQ_ONEf;
                break;
            case BCM_FIELD_DOSATTACK_SYN_FRAG:
                field = TCP_FLAGS_SYN_EQ_ONE_ACK_EQ_ZERO_AND_SRC_PORT_LT_1024f;
                break;
            case BCM_FIELD_DOSATTACK_FLAGZERO_SEQZERO:
                field = TCP_ALL_FLAGS_EQ_ZERO_AND_SEQNUM_EQ_ZEROf;
                break;
            case BCM_FIELD_DOSATTACK_TCPFLAGS_FUP:
                field = TCP_FLAGS_FIN_URG_AND_PSH_EQ_ONE_AND_SEQNUM_EQ_ZEROf;
                break;
            case BCM_FIELD_DOSATTACK_TCPFLAGS_SF:
                field = TCP_FLAGS_FIN_AND_SYN_EQ_ONEf;
                break;
            case BCM_FIELD_DOSATTACK_TCPPORTS_EQUAL:
                field = TCP_SPORT_EQ_DPORTf;
                break;
            case BCM_FIELD_DOSATTACK_UDPPORTS_EQUAL:
                field = UDP_SPORT_EQ_DPORTf;
                break;
            case BCM_FIELD_DOSATTACK_ICMP:
                field = ICMP_ECHO_PKT_SIZE_TOO_BIGf;
                break;
            case BCM_FIELD_DOSATTACK_ICMPFRAGMENTS:
                field = ICMP_FRAGMENTED_PKTf;
                break;
            case BCM_FIELD_DOSATTACK_TCPFLAGS_SR:
                field = TCP_SYN_RSTf;
                break;
            case BCM_FIELD_DOSATTACK_TCPFLAGS_SYN_FIRSTFRAG:
                field = TCP_SYN_FIRST_FRAGMENTf;
                break;
            case BCM_FIELD_DOSATTACK_TCPFLAGS_FIN_NOACK:
                field = TCP_FIN_NO_ACKf;
                break;
            case BCM_FIELD_DOSATTACK_TCPFLAGS_RST_NOACK:
                field = TCP_RST_NO_ACKf;
                break;
            case BCM_FIELD_DOSATTACK_TCPFLAGS_NOSFRA:
                field = TCP_NO_SYN_FIN_RST_ACKf;
                break;
            case BCM_FIELD_DOSATTACK_TCP_DSTPORT_ZERO:
                field = TCP_DPORT_ZEROf;
                break;
            case BCM_FIELD_DOSATTACK_TCP_SRCPORT_ZERO:
                field = TCP_DPORT_ZEROf;
                break;
            case BCM_FIELD_DOSATTACK_TCP_DST_IP_BCAST:
                field = TCP_IP_BROADCAST_CHECKf;
                break;
            case BCM_FIELD_DOSATTACK_TCPFLAGS_ZERO:
                field = TCP_FLAG_ZEROf;
                break;
            case BCM_FIELD_DOSATTACK_TCPFLAGS_RESVD_NONZERO:
                field = TCP_RESERVED_NON_ZEROf;
                break;
            case BCM_FIELD_DOSATTACK_TCPFLAGS_ACK:
                field = TCP_ACK_ACKNUMBER_ZEROf;
                break;
            case BCM_FIELD_DOSATTACK_TCPFLAGS_URG_NOURGPTR:
                field = TCP_URG_NO_URG_POINTERf;
                break;
            case BCM_FIELD_DOSATTACK_TCPFLAGS_NOURG_URGPTR:
                field = TCP_NO_URG_URG_POINTERf;
                break;
            case BCM_FIELD_DOSATTACK_TCPFLAGS_SYNOPTION:
                field = TCP_SYN_WITH_DATA_PAYLOADf;
                break;
            default:
                return BCM_E_PARAM;
        }

        fmt = HVE_DOS_ATTACK_RESULTS_FORMATfmt;
        if (soc_format_field_valid(unit, fmt, field) == FALSE) {
            return BCM_E_PARAM;
        }
        field_info = soc_format_fieldinfo_get(unit, fmt, field);
        bit_pos = field_info->bp;

        SHR_BITSET(qual_info->bitmap, bit_pos);
    }
    qual_info->partial = TRUE;

    return BCM_E_NONE;
}

/******************* Extraction *****************************/
/*
 * Function:
 *    _field_offset_part_get
 *
 * Purpose:
 *    Returns group part number for given offset in keygen context.
 *
 * Parameters:
 *    offset_arr - (IN) Offset array in keygen context.
 *    index      - (IN) Index in offset array.
 *    part       - (OUT) pointer to part.
 *
 * Returns:
 *    BCM_E_INTERNAL    - Incorrect arguments.
 *    BCM_E_NONE        - Success.
 */
STATIC int
_field_offset_part_get(bcmi_keygen_qual_offset_t *offset_arr,
            uint8 index, int *part)
{
    int rv = BCM_E_NONE;

    if ((offset_arr == NULL) || (part == NULL)) {
        return BCM_E_PARAM;
    }

    if (index < BCMI_KEYGEN_QUAL_OFFSETS_MAX) {
        *part = offset_arr->offset[index] / 180;
    } else {
        rv = BCM_E_INTERNAL;
    }

    return rv;
}

/*
 * Function:
 *    _field_hx5_ft_qual_offset_split_req
 *
 * Purpose:
 *    Checks if spliting is required in given offset_info
 *    at given offset.
 *
 * Parameters:
 *    uint       - (IN) Device Unit.
 *    offset_arr - (IN) Offset array in keygen context.
 *    offset     - (IN) Offset to verify
 *    split_pos  - (OUT) Split bit position
 *
 * Returns:
 *    TRUE       - Splitting is required.
 *    FALSE      - Splitting is not required.
 */
STATIC int
_field_hx5_ft_qual_offset_split_req(int unit,
        bcmi_keygen_qual_offset_t *offset_arr,
        int offset,
        int *split_pos)
{
    int q_offset = 0;
    int width = 0;

    q_offset = offset_arr->offset[offset];
    width = offset_arr->width[offset];

    if ((q_offset < 180) && (q_offset + width) > 180) {
        if (split_pos != NULL) {
            *split_pos = 180;
        }
        return TRUE;
    }

    return FALSE;
}

/*
 * Function:
 *    _field_hx5_ft_qual_offset_arr_split
 *
 * Purpose:
 *    Split offset info info 2 parts if group_part boundary
 *    lies in-between the qualifier offsets.
 *
 * Parameters:
 *    uint       - (IN) Unit
 *    split_idx  - (IN) Qualifier offset which requires splitting.
 *    qual_offset_info - (IN/OUT) qualifier offset info
 *
 * Returns:
 *    BCM_E_XXX
 *
 * Notes:
 *    In Flowtracker, keygen types A/A/B are 128 bits in size. But,
 *    double-wide group mode is 360 bits with 180 bits in either
 *    parts. Due to this unsymmetry in keygen type bit size and
 *    group parts size, it can happen that 180 bit part boundary
 *    lies in-between offset info in at max 1 qualifier. So, it
 *    is required to split that offset_arr into 2 parts for
 *    correct population of group_qual info.
 */
STATIC int
_field_hx5_ft_qual_offset_arr_split(int unit,
            int split_idx,
            _field_ft_qual_ext_offset_info_t *qual_ext_offset_info)
{
    int rv = BCM_E_NONE;
    int width = 0;
    int offset = 0;
    int q_offset = 0;
    int ext_num = 0;
    int split_offset = 0;
    int num_offsets = 0;
    int split_pos = 0;
    _field_ft_qual_sel_t *sel_offset_arr = NULL;
    bcmi_keygen_qual_offset_t *offset_arr = NULL;

    offset_arr = qual_ext_offset_info->offset_arr;
    sel_offset_arr = qual_ext_offset_info->sel_offset_arr;

    /* Search for split offset */
    for (offset = 0; offset < BCMI_KEYGEN_QUAL_OFFSETS_MAX; offset++) {
        if (!offset_arr[split_idx].width[offset]) {
            continue;
        }
        if (_field_hx5_ft_qual_offset_split_req(unit,
                    &offset_arr[split_idx], offset, &split_pos) == TRUE) {
            split_offset = offset;
            width = offset_arr[split_idx].width[offset];
            q_offset = offset_arr[split_idx].offset[offset];
            ext_num = sel_offset_arr[split_idx].ext_num[offset];
            break;
        }
        num_offsets++;
        if (num_offsets == offset_arr[split_idx].num_offsets) {
            break;
        }
    }

    /* Split */
    if (num_offsets < offset_arr[split_idx].num_offsets) {
        if (((split_offset + 1) >= BCMI_KEYGEN_QUAL_OFFSETS_MAX) ||
            (offset_arr[split_idx].width[BCMI_KEYGEN_QUAL_OFFSETS_MAX - 1])) {
            /* Cannot split, not enough free indices */
            return BCM_E_INTERNAL;
        }
        for (offset = (BCMI_KEYGEN_QUAL_OFFSETS_MAX - 1);
            offset > split_offset; offset--) {
            offset_arr[split_idx].offset[offset] =
                offset_arr[split_idx].offset[offset - 1];
            offset_arr[split_idx].width[offset] =
                offset_arr[split_idx].width[offset - 1];
            sel_offset_arr[split_idx].ext_num[offset] =
                sel_offset_arr[split_idx].ext_num[offset - 1];
        }

        width = (q_offset + width) - split_pos;
        offset_arr[split_idx].offset[split_offset] = q_offset;
        offset_arr[split_idx].width[split_offset] = split_pos - q_offset;
        sel_offset_arr[split_idx].ext_num[split_offset] = ext_num;
        offset_arr[split_idx].offset[split_offset + 1] = split_pos;
        offset_arr[split_idx].width[split_offset + 1] = width;
        sel_offset_arr[split_idx].ext_num[split_offset + 1] = ext_num;

        offset_arr[split_idx].num_offsets += 1;
        sel_offset_arr[split_idx].num_offsets += 1;
    }

    return rv;
}

/*
 * Function:
 *    _bcm_field_hx5_ft_keygen_extractors_mask_set
 *
 * Purpose:
 *    Extractor mask memory are calculated based on
 *    extractors codes and qualifier offsets/width.
 *
 * Parameters:
 *    uint       - (IN) Unit
 *    qual_ext_offset_info - (IN) qualifier offset info
 *    ext_codes  - (IN/OUT) Extractors code.
 *
 * Returns:
 *    BCM_E_XXX
 *
 * Notes:
 *    In flowtracker, keygen type A has 128 bits of mask. This mask
 *    needs to be calculated based onn extractor codes and qualifier
 *    offset/width pair.
 *    Shift_offsets are first calculated to get correct shifted
 *    offsets of qualifier offsets.
 */
int
_bcm_field_hx5_ft_keygen_extractors_mask_set(int unit,
        _field_ft_qual_ext_offset_info_t *qual_ext_offset_info,
        _field_ft_keygen_ext_codes_t *ext_codes)
{
    int rv = BCM_E_NONE;
    int qual_idx = 0;
    int q_offset = 0;
    int q_width = 0;
    int ext_num = 0;
    int num_offsets = 0;
    int offset = 0;
    int shift_offsets_arr[30] = {0};

    (void) _field_hx5_ft_keygen_ext_shift_offsets_get(unit,
            ext_codes, &shift_offsets_arr[0]);

    while(qual_idx < qual_ext_offset_info->size) {
        num_offsets = 0;
        for (offset = 0; offset < BCMI_KEYGEN_QUAL_OFFSETS_MAX; offset++) {
            if (!qual_ext_offset_info->offset_arr[qual_idx].width[offset]) {
                continue;
            }

            q_offset = qual_ext_offset_info->offset_arr[qual_idx].offset[offset];
            q_width = qual_ext_offset_info->offset_arr[qual_idx].width[offset];
            ext_num = qual_ext_offset_info->sel_offset_arr[qual_idx].ext_num[offset];

            BCM_IF_ERROR_RETURN(
                _bcm_field_ft_keygen_profile_extractors_mask_set(unit,
                        q_offset, q_width, ext_num, shift_offsets_arr,
                        0, NULL, ext_codes));

            num_offsets++;
            if (num_offsets ==
                    qual_ext_offset_info->offset_arr[qual_idx].num_offsets) {
                break;
            }
        }
        qual_idx++;
    }

    return rv;
}

/*
 * Function:
 *    _field_hx5_ft_qual_offset_ext_shift_apply
 *
 * Purpose:
 *    Updates offset values in qual_offset_info based on extractors
 *    shift logic and group parts.
 *
 * Parameters:
 *    uint       - (IN) Unit.
 *    split_allowed - (IN) Check if we need to do split
 *    ext_codes  - (IN) Extractors code.
 *    qual_offset_info - (IN/OUT) qualifier offset info
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_field_hx5_ft_qual_offset_ext_shift_apply(int unit,
        int split_allowed,
        _field_ft_keygen_ext_codes_t *ext_codes,
        _field_ft_qual_ext_offset_info_t *qual_ext_offset_info)
{
    int idx = 0;
    int rv = BCM_E_NONE;
    int offset = 0;
    int q_offset = 0;
    int shift_idx = 0;
    int num_offsets = 0;
    int offset_arr_split = FALSE;
    int shift_offsets_arr[30] = {0};

    (void) _field_hx5_ft_keygen_ext_shift_offsets_get(unit,
            ext_codes, &shift_offsets_arr[0]);

    idx = 0;
    while (idx < qual_ext_offset_info->size) {
        num_offsets = 0;
        offset_arr_split = FALSE;
        for (offset = 0; offset < BCMI_KEYGEN_QUAL_OFFSETS_MAX; offset++) {
            if (!qual_ext_offset_info->offset_arr[idx].width[offset]) {
                continue;
            }
            q_offset = qual_ext_offset_info->offset_arr[idx].offset[offset];
            shift_idx = q_offset / 16;
            q_offset = (q_offset % 16) + shift_offsets_arr[shift_idx];
            qual_ext_offset_info->offset_arr[idx].offset[offset] = q_offset;

            if (_field_hx5_ft_qual_offset_split_req(unit, &qual_ext_offset_info->
                        offset_arr[idx], offset, NULL) == TRUE) {
                offset_arr_split = TRUE;
            }
            num_offsets++;
            if (num_offsets ==
                    qual_ext_offset_info->offset_arr[idx].num_offsets) {
                break;
            }
        }
        if ((split_allowed) && (offset_arr_split)) {
            BCM_IF_ERROR_RETURN(_field_hx5_ft_qual_offset_arr_split(unit,
                        idx, qual_ext_offset_info));
        }
        idx++;
    }

    return rv;
}

/*
 * Function:
 *    _field_hx5_ft_group_keygen_ext_codes_set
 *
 * Purpose:
 *   Extract flowtracker stage field group keygen extractor codes.
 *
 * Parameters:
 *    unit       - (IN) BCM device number
 *    fg         - (IN) Field group structure.
 *    keygen_oper- (IN) Kegen operational structure.
 *
 * Returns:
 *    BCM_E_MEMEORY     - Failed to allocate memory.
 *    BCM_E_PARAM       - Input params is wrong.
 *    BCM_E_NONE        - Success.
 */
STATIC int
_field_hx5_ft_group_keygen_ext_codes_set(int unit,
                                        _field_group_t *fg,
                                        bcmi_keygen_oper_t *keygen_oper)
{
    int rv = BCM_E_NONE;
    int keygen_profile;
    bcmi_keygen_ext_ctrl_sel_info_t *ext_ctrl_sel_info;
    uint16 ext_ctrl_sel_info_count;
    _field_ft_keygen_ext_codes_t *ext_codes = NULL;

    /* Validate input params. */
    if ((fg == NULL) || (keygen_oper == NULL)) {
        return BCM_E_PARAM;
    }

    if (fg->ft_ext_codes == NULL) {
        /* Allocate memory */
        _FP_XGS3_ALLOC(ext_codes, sizeof(_field_ft_keygen_ext_codes_t),
                "field group ext codes");
        if (ext_codes == NULL) {
            return BCM_E_MEMORY;
        }
        (void) _bcm_field_ft_keygen_extractors_init(ext_codes);
        keygen_profile = _BCM_FIELD_FT_KEYGEN_PROFILE_AAB;
    } else {
        ext_codes = fg->ft_ext_codes;
        keygen_profile = ext_codes->keygen_profile;
    }

    ext_ctrl_sel_info_count = keygen_oper->ext_ctrl_sel_info_count;
    ext_ctrl_sel_info = keygen_oper->ext_ctrl_sel_info;

    rv = _bcm_field_hx5_ft_keygen_extractors_set(unit, keygen_profile,
                ext_ctrl_sel_info_count, ext_ctrl_sel_info, ext_codes);

    if (BCM_SUCCESS(rv)) {
        fg->ft_ext_codes = ext_codes;
    } else {
        fg->ft_ext_codes = NULL;
        sal_free(ext_codes);
    }

    return rv;
}

/*
 * Function:
 *    _field_hx5_ft_qual_offset_info_merge
 *
 * Purpose:
 *    Create single offset_arr/qid_arr from output of Keygen_oper
 *
 * Parameters:
 *    uint       - (IN) Unit.
 *    keygen_oper - (IN) Output from Keygen algorithm.
 *    keygen_parts_count - (IN) Number of Parts in Keygen output.
 *    qual_offset_info - (OUT) qualifier offset info
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_field_hx5_ft_qual_offset_info_merge(int unit,
                                  bcmi_keygen_oper_t *keygen_oper,
                                  int keygen_parts_count,
                                  _field_ft_qual_ext_offset_info_t *qual_ext_offset_info)
{
    int rv = BCM_E_NONE;
    int q_idx = 0, idx = 0;
    int keygen_part = 0;
    int offset = 0;
    int sel_offset = 0;
    int new_qual_idx = 0;
    uint16 num_offsets = 0;
    bcm_field_qset_t qset;
    bcmi_keygen_qual_offset_info_t *info = NULL;

    new_qual_idx = 0;
    BCM_FIELD_QSET_INIT(qset);
    /* Loop through all keygen parts */
    for (keygen_part = 0; keygen_part < keygen_parts_count; keygen_part++) {
        info = &(keygen_oper->qual_offset_info[keygen_part]);
        sel_offset = 160 * keygen_part;

        /* Loop through qual info in the keygen part */
        for (idx = 0; idx < info->size; idx++, q_idx++) {
            /* Check if qual is also allocated, use that index */
            if (BCM_FIELD_QSET_TEST(qset, info->qid_arr[idx])) {
                for (q_idx = 0; q_idx < new_qual_idx; q_idx++) {
                    if (qual_ext_offset_info->qid_arr[q_idx] ==
                            info->qid_arr[idx]) {
                        break;
                    }
                }
                if (q_idx == new_qual_idx) {
                    return BCM_E_INTERNAL;
                }
            } else {
                /* Assign new index */
                q_idx = new_qual_idx;
                new_qual_idx++;
            }
            num_offsets = 0;
            for (offset = 0; offset < BCMI_KEYGEN_QUAL_OFFSETS_MAX; offset++) {
                if (!info->offset_arr[idx].width[offset]) {
                    continue;
                }
                if (qual_ext_offset_info->offset_arr[q_idx].width[offset]) {
                    return (BCM_E_INTERNAL);
                }
                qual_ext_offset_info->offset_arr[q_idx].offset[offset] =
                    info->offset_arr[idx].offset[offset] + sel_offset;
                qual_ext_offset_info->offset_arr[q_idx].width[offset] =
                    info->offset_arr[idx].width[offset];
                qual_ext_offset_info->sel_offset_arr[q_idx].ext_num[offset] =
                    (info->offset_arr[idx].offset[offset] + sel_offset) / 16;
                num_offsets++;
                if (num_offsets == info->offset_arr[idx].num_offsets) {
                    break;
                }
            }
            qual_ext_offset_info->qid_arr[q_idx] = info->qid_arr[idx];
            qual_ext_offset_info->offset_arr[q_idx].num_offsets += num_offsets;
            qual_ext_offset_info->sel_offset_arr[q_idx].num_offsets += num_offsets;
            /* Add qual to Qset */
            BCM_FIELD_QSET_ADD(qset, info->qid_arr[idx]);
        }
    }
    qual_ext_offset_info->size = new_qual_idx;

    return rv;
}

/*
 * Function:
 *    _field_hx5_ft_qual_ext_offset_info_free
 *
 * Purpose:
 *    Free qual_ext_offset_info
 *
 * Parameters:
 *    uint       - (IN) Unit.
 *    qual_ext_offset_info - (OUT) qualifier ext offset info
 *
 * Returns:
 *    None
 * Notes:
 *    Double pointes is passed as input. So, memory for
 *    qual_ext_offset_info is also freed.
 */
void
_field_hx5_ft_qual_ext_offset_info_free(int unit,
            _field_ft_qual_ext_offset_info_t **qual_ext_offset_info)
{
    _field_ft_qual_ext_offset_info_t *temp_info = NULL;

    if (*qual_ext_offset_info == NULL) {
        return;
    }

    temp_info = *qual_ext_offset_info;

    if (temp_info->qid_arr != NULL) {
        sal_free(temp_info->qid_arr);
        temp_info->qid_arr = NULL;
    }
    if (temp_info->offset_arr != NULL) {
        sal_free(temp_info->offset_arr);
        temp_info->offset_arr = NULL;
    }
    if (temp_info->sel_offset_arr != NULL) {
        sal_free(temp_info->sel_offset_arr);
        temp_info->sel_offset_arr = NULL;
    }

    sal_free(temp_info);
    *qual_ext_offset_info = NULL;
}

/*
 * Function:
 *    _field_hx5_ft_qual_ext_offset_info_get
 *
 * Purpose:
 *    Create a temporary structure to work on keygen_oper
 *
 * Parameters:
 *    uint       - (IN) Unit.
 *    keygen_parts_count - (IN) Number of Parts in Keygen output.
 *    keygen_oper - (IN) Output from Keygen algorithm.
 *    qual_ext_offset_info - (OUT) qualifier ext offset info
 *
 * Returns:
 *    BCM_E_XXX
 * Notes:
 *    Double pointer is passed as input. So, memory for
 *    qual_ext_offset_info is also allocated.
 */
int
_field_hx5_ft_qual_ext_offset_info_get(int unit,
                                int keygen_parts_count,
                                bcmi_keygen_oper_t *keygen_oper,
                                _field_ft_qual_ext_offset_info_t **qual_ext_offset_info)
{
    int rv = BCM_E_NONE;
    int keygen_part = 0;
    int qual_offset_info_count = 0;
    _field_ft_qual_ext_offset_info_t *temp_info = NULL;
    bcmi_keygen_qual_offset_info_t *qual_offset_info = NULL;

    if ((keygen_oper == NULL) ||
        (keygen_oper->qual_offset_info == NULL) ||
        (keygen_oper->ext_ctrl_sel_info == NULL)) {
        return BCM_E_PARAM;
    }

    /* Get number of qualifiers offset info */
    for (keygen_part = 0; keygen_part < keygen_parts_count; keygen_part++) {
        qual_offset_info = &(keygen_oper->qual_offset_info[keygen_part]);
        qual_offset_info_count += qual_offset_info->size;
    }

    _FP_XGS3_ALLOC(temp_info,
            sizeof(_field_ft_qual_ext_offset_info_t),
            " Qual Ext offset info");
    if (temp_info == NULL) {
        return BCM_E_MEMORY;
    }

    /* Allocate joint memory for all qual offset infos. */
    temp_info->size = qual_offset_info_count;
    _FP_XGS3_ALLOC(temp_info->qid_arr,
            qual_offset_info_count * sizeof(bcm_field_qualify_t),
            "Group Qualifier Info");
    _FP_XGS3_ALLOC(temp_info->offset_arr,
            qual_offset_info_count * sizeof(bcmi_keygen_qual_offset_t),
            "Group Qualifier Offset Info");
    _FP_XGS3_ALLOC(temp_info->sel_offset_arr,
            qual_offset_info_count * sizeof(_field_ft_qual_sel_t),
            "Group Qualifier Offset Info");

    if ((temp_info->qid_arr == NULL) ||
            (temp_info->offset_arr == NULL) ||
            (temp_info->sel_offset_arr == NULL)) {
        rv = BCM_E_MEMORY;
    }

    if (BCM_SUCCESS(rv)) {
        *qual_ext_offset_info = temp_info;

        /* Merge all keygen_parts to joint memory. */
        rv = _field_hx5_ft_qual_offset_info_merge(unit,
            keygen_oper, keygen_parts_count, *qual_ext_offset_info);
    }

    if (BCM_FAILURE(rv)) {
        if (temp_info != NULL) {
            *qual_ext_offset_info = temp_info;
            (void) _field_hx5_ft_qual_ext_offset_info_free(unit,
                                qual_ext_offset_info);
        }
        *qual_ext_offset_info = NULL;
    }

    return rv;
}

/*
 * Function:
 *    _field_hx5_ft_group_qual_offset_info_make
 *
 * Purpose:
 *    Create group Qual info for given group part
 *
 * Parameters:
 *    uint       - (IN) Unit.
 *    fg         - (IN/OUT) Structure to field group.
 *    qual_ext_offset_info - (IN) qualifier offset info.
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_hx5_ft_group_qual_offset_info_make(int unit,
            _field_group_t *fg,
            _field_ft_qual_ext_offset_info_t *qual_ext_offset_info)
{
    int rv = BCM_E_NONE;
    int idx = 0;
    int part = 0;
    int info_idx = 0;
    int offset = 0;
    int parts_count = 0;
    int qual_offset_info_count = 0;
    uint16 qual = 0;
    uint16 num_offsets = 0;
    uint8 part_offset_found[2];
    uint8 new_idx_allocated[2];
    bcm_field_qualify_t *qid_arr = NULL;
    _bcm_field_group_qual_t *fg_qual_arr = NULL;
    bcmi_keygen_qual_offset_t *offset_arr = NULL;
    _field_ft_group_qual_part_info_t grp_part_info[2];

    /* Validate input params. */
    if ((fg == NULL) || (qual_ext_offset_info == NULL)) {
        return BCM_E_INTERNAL;
    }

    qid_arr = qual_ext_offset_info->qid_arr;
    offset_arr = qual_ext_offset_info->offset_arr;
    qual_offset_info_count = qual_ext_offset_info->size;

    /* Get number of entry parts. */
    BCM_IF_ERROR_RETURN(_bcm_field_entry_tcam_parts_count(unit,
                fg->stage_id, fg->flags, &parts_count));

    for (part = 0; part < parts_count; part++) {
        grp_part_info[part].count = 0;
        BCM_FIELD_QSET_INIT(grp_part_info[part].qset);
    }

    /* Step 1: Loop through all info and get count in multiple group parts */
    for (info_idx = 0; info_idx < qual_offset_info_count; info_idx++) {
        qual = qid_arr[info_idx];
        num_offsets = 0;
        part_offset_found[0] = part_offset_found[1] = FALSE;
        for (offset = 0; offset < BCMI_KEYGEN_QUAL_OFFSETS_MAX; offset++) {
            if (offset_arr[info_idx].width[offset] == 0) {
                continue;
            }
            rv = _field_offset_part_get(&offset_arr[info_idx], offset, &part);
            if ((rv != BCM_E_NONE) || (part >= parts_count)) {
                rv = BCM_E_INTERNAL;
                goto cleanup;
            }
            part_offset_found[part] = TRUE;
            num_offsets++;
            if (num_offsets == offset_arr[info_idx].num_offsets) {
                break;
            }
        }
        for (part = 0; part < parts_count; part++) {
            if (part_offset_found[part] == TRUE) {
                if (!BCM_FIELD_QSET_TEST(grp_part_info[part].qset, qual)) {
                    grp_part_info[part].count++;
                    BCM_FIELD_QSET_ADD(grp_part_info[part].qset, qual);
                }
            }
        }
    }

    /* Step 2: Allocate memory to each group parts */
    for (part = 0; part < parts_count; part++) {
        fg_qual_arr = &(fg->qual_arr[0][part]);
        fg_qual_arr->size = grp_part_info[part].count;

        if (fg_qual_arr->size != 0) {
            _FP_XGS3_ALLOC(fg_qual_arr->qid_arr,
                    fg_qual_arr->size * sizeof(uint16),
                    "Group Qualifier Array");
            _FP_XGS3_ALLOC(fg_qual_arr->offset_arr,
                    fg_qual_arr->size * sizeof(_bcm_field_qual_offset_t),
                    "Group Qualifier Offset Info");
            if ((fg_qual_arr->qid_arr == NULL) ||
                    (fg_qual_arr->offset_arr == NULL)) {
                rv = BCM_E_MEMORY;
                goto cleanup;
            }
        } else {
            fg_qual_arr->qid_arr = NULL;
            fg_qual_arr->offset_arr = NULL;
        }
        grp_part_info[part].count = 0;
        BCM_FIELD_QSET_INIT(grp_part_info[part].qset);
    }

    /* Step 3: Fill up group qual parts */
    for (info_idx = 0; info_idx < qual_offset_info_count; info_idx++) {
        qual = qid_arr[info_idx];
        num_offsets = 0;
        new_idx_allocated[0] = new_idx_allocated[1] = FALSE;
        for (offset = 0; offset < BCMI_KEYGEN_QUAL_OFFSETS_MAX; offset++) {
            if (offset_arr[info_idx].width[offset] == 0) {
                continue;
            }
            (void) _field_offset_part_get(&offset_arr[info_idx], offset, &part);
            fg_qual_arr = &(fg->qual_arr[0][part]);
            if (new_idx_allocated[part] == TRUE) {
                idx = grp_part_info[part].count;
            } else {
                if (BCM_FIELD_QSET_TEST(grp_part_info[part].qset, qual)) {
                    for (idx = (grp_part_info[part].count - 1); idx >= 0; idx--) {
                        if (fg_qual_arr->qid_arr[idx] == qual) {
                            break;
                        }
                    }
                    if (idx < 0) {
                        rv = BCM_E_INTERNAL;
                        goto cleanup;
                    }
                } else {
                    new_idx_allocated[part] = TRUE;
                    idx = grp_part_info[part].count;
                    BCM_FIELD_QSET_ADD(grp_part_info[part].qset, qual);
                }
            }
            if ((idx >= fg_qual_arr->size) ||
                (fg_qual_arr->offset_arr[idx].width[offset] != 0)) {
                rv = BCM_E_INTERNAL;
                goto cleanup;
            }
            fg_qual_arr->qid_arr[idx] = qual;
            fg_qual_arr->offset_arr[idx].offset[offset] =
                offset_arr[info_idx].offset[offset];
            fg_qual_arr->offset_arr[idx].width[offset] =
                offset_arr[info_idx].width[offset];
            fg_qual_arr->offset_arr[idx].field = KEYf;
            if (fg_qual_arr->offset_arr[idx].num_offsets <= offset) {
                fg_qual_arr->offset_arr[idx].num_offsets = (offset + 1);
            }

            num_offsets++;
            if (num_offsets == offset_arr[info_idx].num_offsets) {
                break;
            }
        }
        for (part = 0; part < parts_count; part++) {
            if (new_idx_allocated[part] == TRUE) {
                grp_part_info[part].count++;
            }
        }
    }

cleanup:

    if(BCM_FAILURE(rv)) {
        for (part = 0; part < parts_count; part++) {
            fg_qual_arr = &(fg->qual_arr[0][part]);
            if (fg_qual_arr->qid_arr != NULL) {
                sal_free(fg_qual_arr->qid_arr);
                fg_qual_arr->qid_arr = NULL;
            }
            if (fg_qual_arr->offset_arr != NULL) {
                sal_free(fg_qual_arr->offset_arr);
                fg_qual_arr->offset_arr = NULL;
            }
        }
    }

    return (rv);
}

/*
 * Function:
 *    _field_hx5_ft_group_keygen_oper_insert
 *
 * Purpose:
 *    Convert and save output of keygen algorithm.
 *
 * Parameters:
 *    unit       - (IN) Unit.
 *    fg         - (IN) Field Group structure.
 *    keygen_parts_count - (IN) Number of keygen parts
 *    keygen_oper - (IN) Output from Keygen algorithm.
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_hx5_ft_group_keygen_oper_insert(int unit,
                                       _field_group_t *fg,
                                       int keygen_parts_count,
                                       bcmi_keygen_oper_t *keygen_oper)
{
    int rv = BCM_E_NONE;
    _field_ft_qual_ext_offset_info_t *qual_ext_offset_info = NULL;

    if ((fg == NULL) || (keygen_oper == NULL)) {
        return BCM_E_INTERNAL;
    }

    /* Verify if atleast one qualifier is extracted */
    if (keygen_oper->ext_ctrl_sel_info_count == 0) {
        return BCM_E_NONE;
    }

    /* Dump Keygen Oper Qual Offsets and Extractors codes */
    LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                            "fp_group=%d Keygen Oper Info \n"),fg->gid));
    (void) _bcm_field_keygen_qual_offset_info_dump(unit,
                keygen_parts_count, keygen_oper);

    rv = _field_hx5_ft_group_keygen_ext_codes_set(unit, fg, keygen_oper);
    BCM_IF_ERROR_RETURN(rv);

    rv = _field_hx5_ft_qual_ext_offset_info_get(unit, keygen_parts_count,
                keygen_oper, &qual_ext_offset_info);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Update Mask of extractors */
    rv = _bcm_field_hx5_ft_keygen_extractors_mask_set(unit,
                    qual_ext_offset_info, fg->ft_ext_codes);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /*
     * Apply shifts on qualifier offsets due to hw extractors shift logic.
     * This will also split offsets to multiple group parts, if required.
     */
    rv = _field_hx5_ft_qual_offset_ext_shift_apply(unit,
                    1, fg->ft_ext_codes, qual_ext_offset_info);
    if (rv != BCM_E_NONE) {
        goto cleanup;
    }
    /* Save qualifiers per group part */
    rv = _field_hx5_ft_group_qual_offset_info_make(unit,
                    fg, qual_ext_offset_info);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Dump Hardware extractor information */
    (void) _bcm_field_hx5_ft_keygen_hw_extractors_dump(unit, fg->ft_ext_codes);

cleanup:

    if (qual_ext_offset_info != NULL) {
        (void) _field_hx5_ft_qual_ext_offset_info_free(unit,
                    &qual_ext_offset_info);
    }

    return rv;
}

/*
 * Function:
 *    _field_hx5_ft_keygen_oper_qual_add
 *
 * Purpose:
 *    Add skipped qualifier Inport and its extractor codes to keygen_oper
 *    info. Before calling keygen algorithm, qualifier InPort and corres-
 *    ponding extractors are skipped from keygen algo processing as it is
 *    required to be extracted with same extractors for all groups. Once
 *    keygen_oper is formed qual InPort is added back to it for generating
 *    final qual_arr info for the field group. Qual InPort is added to
 *    part-0 of keygen output.
 *
 * Parameters:
 *    unit       - (IN) Unit.
 *    keygen_cfg - (IN) Keygen config
 *    keygen_oper- (IN/OUT) Output from Keygen algorithm.
 *
 * Returns:
 *    BCM_E_XXX
 */

STATIC int
_field_hx5_ft_keygen_oper_qual_add(int unit,
                                   bcmi_keygen_cfg_t *keygen_cfg,
                                   bcmi_keygen_oper_t *keygen_oper)
{
    uint16 idx = 0;
    uint16 offset = 0, num_offsets = 0;
    uint16 ext_ctrl_info_count = 0, skipped_sz = 0;
    uint16 qual_offset_info_size = 0;
    uint16 qual_offset_info_alloc = FALSE;
    bcm_field_qualify_t *qid_arr = NULL;
    bcmi_keygen_qual_offset_t *offset_arr = NULL;
    bcmi_keygen_qual_offset_info_t *qual_offset_info = NULL;
    bcmi_keygen_ext_ctrl_sel_info_t *ext_ctrl_sel_info = NULL;

    if ((keygen_cfg == NULL) || (keygen_oper == NULL)) {
        return BCM_E_PARAM;
    }

    /* No qualifiers/extractors were skipped */
    if (keygen_cfg->ext_ctrl_sel_info_count == 0) {
        return BCM_E_NONE;
    }

    /* Get total qual info size at part-0 of keygen_oper */
    qual_offset_info = &(keygen_oper->qual_offset_info[0]);
    if (qual_offset_info != NULL) {
        qual_offset_info_size = qual_offset_info->size;
    }
    qual_offset_info_size++;

    /* Get total extractors */
    ext_ctrl_info_count = keygen_oper->ext_ctrl_sel_info_count;
    ext_ctrl_info_count += keygen_cfg->ext_ctrl_sel_info_count;

    /* Allocate memories */
    if (qual_offset_info == NULL) {
        qual_offset_info_alloc = TRUE;
        _FP_XGS3_ALLOC(qual_offset_info,
            keygen_cfg->num_parts * sizeof(bcmi_keygen_qual_offset_info_t),
            "Qual Offset Info");
    }
    _FP_XGS3_ALLOC(qid_arr,
            qual_offset_info_size * sizeof(bcm_field_qualify_t),
            "Keygen oper qid arr");
    _FP_XGS3_ALLOC(offset_arr,
            qual_offset_info_size * sizeof(bcmi_keygen_qual_offset_t),
            "Keygen oper offset arr");
    _FP_XGS3_ALLOC(ext_ctrl_sel_info,
            ext_ctrl_info_count * sizeof(bcmi_keygen_ext_ctrl_sel_info_t),
            "Ext Ctrl Offset Info");
    if ((qual_offset_info == NULL) || (qid_arr == NULL) ||
        (offset_arr == NULL) || (ext_ctrl_sel_info == NULL)) {
        if ((qual_offset_info != NULL) && (qual_offset_info_alloc == TRUE)) {
            sal_free(qual_offset_info);
        }
        if (qid_arr != NULL) {
            sal_free(qid_arr);
        }
        if (offset_arr != NULL) {
            sal_free(offset_arr);
        }
        if (ext_ctrl_sel_info != NULL) {
            sal_free(ext_ctrl_sel_info);
        }
        return BCM_E_MEMORY;
    }
    /* Add InPort Qual info */
    qid_arr[0] = bcmFieldQualifyInPort;
    offset_arr[0].num_offsets = 1;
    offset_arr[0].offset[0] = 0;
    offset_arr[0].width[0] = 7;
    for (idx = 0; idx < qual_offset_info->size; idx++) {
        num_offsets = 0;
        for (offset = 0; offset < BCMI_KEYGEN_QUAL_OFFSETS_MAX; offset++) {
            if (qual_offset_info->offset_arr[idx].width[offset] == 0) {
                continue;
            }
            num_offsets++;
            offset_arr[idx + 1].offset[offset] =
                qual_offset_info->offset_arr[idx].offset[offset];
            offset_arr[idx + 1].width[offset] =
                qual_offset_info->offset_arr[idx].width[offset];
            if (num_offsets == qual_offset_info->offset_arr[idx].num_offsets) {
                break;
            }
        }
        offset_arr[idx + 1].num_offsets = num_offsets;
        qid_arr[idx + 1] = qual_offset_info->qid_arr[idx];
    }

    /* Copy ext ctrl sel info */
    skipped_sz = keygen_cfg->ext_ctrl_sel_info_count;
    sal_memcpy(&ext_ctrl_sel_info[0], keygen_cfg->ext_ctrl_sel_info,
            skipped_sz * sizeof(bcmi_keygen_ext_ctrl_sel_info_t));
    if (keygen_oper->ext_ctrl_sel_info_count != 0) {
        sal_memcpy(&ext_ctrl_sel_info[skipped_sz],
                keygen_oper->ext_ctrl_sel_info,
                keygen_oper->ext_ctrl_sel_info_count *
                sizeof(bcmi_keygen_ext_ctrl_sel_info_t));
    }

    /* Free modified members in keygen_oper */
    if (keygen_oper->ext_ctrl_sel_info != NULL) {
        sal_free(keygen_oper->ext_ctrl_sel_info);
        keygen_oper->ext_ctrl_sel_info = NULL;
    }
    if (qual_offset_info_alloc == FALSE) {
        if (keygen_oper->qual_offset_info[0].qid_arr != NULL) {
            sal_free(keygen_oper->qual_offset_info[0].qid_arr);
            keygen_oper->qual_offset_info[0].qid_arr = NULL;
        }
        if (keygen_oper->qual_offset_info[0].offset_arr != NULL) {
            sal_free(keygen_oper->qual_offset_info[0].offset_arr);
            keygen_oper->qual_offset_info[0].offset_arr = NULL;
        }
    }

    /* Update keygen_oper with modified members */
    keygen_oper->ext_ctrl_sel_info_count = ext_ctrl_info_count;
    keygen_oper->ext_ctrl_sel_info = ext_ctrl_sel_info;
    if (qual_offset_info_alloc == TRUE) {
        keygen_oper->qual_offset_info = qual_offset_info;
    }
    keygen_oper->qual_offset_info[0].qid_arr = qid_arr;
    keygen_oper->qual_offset_info[0].offset_arr = offset_arr;
    keygen_oper->qual_offset_info[0].size = qual_offset_info_size;

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_hx5_ft_ext_code_assign
 *
 * Purpose:
 *    Calculate extractors values and qualifiers offsets in
 *    final key.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    fsm_ptr    - (IN) Field group creation state machine.
 *    stage_fc   - (IN) Field stage control structure.
 *
 * Returns:
 *    BCM_E_XXX
 */
int _field_hx5_ft_ext_code_assign(int unit,
                                  int selcode_clear,
                                  _field_group_add_fsm_t *fsm_ptr)
{
    int rv = BCM_E_NONE;
    uint16 rkey_size = 0;
    uint16 skipped_size = 0;
    uint16 qual_info_count = 0;
    uint16 keygen_parts_count = 0;
    uint16 qual_inport_excluded = FALSE;
    bcm_field_qset_t qset_req;
    bcmi_keygen_cfg_t keygen_cfg;
    bcmi_keygen_oper_t keygen_oper;
    bcmi_keygen_mode_t keygen_mode;
    bcmi_keygen_mode_t keygen_ext_mode;
    _field_group_t *fg = NULL;
    _field_stage_t *stage_fc = NULL;
    bcmi_keygen_qual_info_t *qual_info_arr = NULL;

    /* Input parameters check. */
    if (NULL == fsm_ptr) {
        return (BCM_E_PARAM);
    }

    fg = fsm_ptr->fg;
    stage_fc = fsm_ptr->stage_fc;

    sal_memset(&keygen_cfg, 0, sizeof(bcmi_keygen_cfg_t));
    sal_memset(&keygen_oper, 0, sizeof(bcmi_keygen_oper_t));
    sal_memcpy(&qset_req, &(fg->qset), sizeof(bcm_field_qset_t));

    /* Need to remove Qual InPort from the qset */
    if ((BCM_FIELD_QSET_TEST(qset_req, bcmFieldQualifyInPort) &&
        (soc_feature(unit, soc_feature_bscan_fae_port_flow_exclude)))) {
        qual_inport_excluded = TRUE;
        BCM_FIELD_QSET_REMOVE(qset_req, bcmFieldQualifyInPort);
    }

    /* Remove dummy qualifiers. */
    BCM_FIELD_QSET_REMOVE(qset_req, bcmFieldQualifyStage);
    BCM_FIELD_QSET_REMOVE(qset_req, bcmFieldQualifyStageIngressFlowtracker);

    /* Get Group Qualifiers info array. */
    rv = _field_th_group_qual_info_get(unit, stage_fc, fg,
                &(qset_req), &qual_info_arr, &qual_info_count);
    BCM_IF_ERROR_RETURN(rv);

    if (fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE &&
        !(fg->flags & _FP_GROUP_DW_DEPTH_EXPANDED)) {
        keygen_mode = BCMI_KEYGEN_MODE_SINGLE;
        keygen_ext_mode = BCMI_KEYGEN_MODE_SINGLE;
        keygen_parts_count = 2;
    } else if (fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE &&
               fg->flags & _FP_GROUP_DW_DEPTH_EXPANDED) {
        keygen_mode = BCMI_KEYGEN_MODE_DBLINTRA;
        keygen_ext_mode = BCMI_KEYGEN_MODE_DBLINTRA;
        keygen_parts_count = 3;
    } else {
        rv = BCM_E_INTERNAL;
        keygen_parts_count = 0;
        goto cleanup;
    }

    keygen_cfg.qual_info_arr = qual_info_arr;
    keygen_cfg.qual_info_count = qual_info_count;
    keygen_cfg.qual_cfg_info_db = stage_fc->qual_cfg_info_db;
    keygen_cfg.ext_cfg_db = stage_fc->ext_cfg_db_arr[keygen_ext_mode];
    keygen_cfg.mode = keygen_mode;
    keygen_cfg.num_parts = keygen_parts_count;

    if (fsm_ptr->flags & _FP_GROUP_CONFIG_VALIDATE) {
        keygen_cfg.flags = BCMI_KEYGEN_CFG_FLAGS_VALIDATE_ONLY;
    }
    if ((fg->ft_ext_codes != NULL) &&
            (fsm_ptr->flags & _FP_GROUP_QSET_UPDATE)) {
        /* Create keygen ext codes to exclude already extracted qualifiers. */
        keygen_cfg.flags |= BCMI_KEYGEN_CFG_FLAGS_QSET_UPDATE;
        rv = _field_hx5_ft_keygen_ext_ctrl_info_get(unit,
                fg->ft_ext_codes, &keygen_cfg.ext_ctrl_sel_info,
                &keygen_cfg.ext_ctrl_sel_info_count);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
    } else {
        if (soc_feature(unit, soc_feature_bscan_fae_port_flow_exclude) &&
            (qual_inport_excluded == TRUE)) {
            /* Create keygen ext codes to skip extractors for InPort */
            keygen_cfg.flags |= BCMI_KEYGEN_CFG_FLAGS_QSET_UPDATE;
            rv = _field_hx5_ft_keygen_ext_ctrl_info_skip(unit,
                     &keygen_cfg, &skipped_size);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }
    }

    if (!(fsm_ptr->flags & _FP_GROUP_QSET_UPDATE)) {
        rv = bcmi_keygen_keysize_get(unit, &keygen_cfg, &rkey_size);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        if (((keygen_mode == BCMI_KEYGEN_MODE_SINGLE)
                    && ((rkey_size + skipped_size) > 180)) ||
                ((keygen_mode == BCMI_KEYGEN_MODE_DBLINTRA)
                 && ((rkey_size + skipped_size) > 360))) {
            rv = BCM_E_CONFIG;
            goto cleanup;
        }
    }

    rv = bcmi_keygen_cfg_process(unit, &keygen_cfg, &keygen_oper);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Update keygen_oper with InPort Qual Info */
    if (soc_feature(unit, soc_feature_bscan_fae_port_flow_exclude) &&
        (qual_inport_excluded == TRUE)) {
        rv = _field_hx5_ft_keygen_oper_qual_add(unit,
                    &keygen_cfg, &keygen_oper);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
    }

    if ((fsm_ptr->flags & _FP_GROUP_CONFIG_VALIDATE) == 0) {
        rv = _field_hx5_ft_group_keygen_oper_insert(unit,
                    fsm_ptr->fg, keygen_parts_count, &keygen_oper);
    }

cleanup:

    (void)bcmi_keygen_oper_free(unit, keygen_parts_count, &keygen_oper);
    if (qual_info_arr != NULL) {
        sal_free(qual_info_arr);
    }
    if (keygen_cfg.ext_ctrl_sel_info != NULL) {
        sal_free(keygen_cfg.ext_ctrl_sel_info);
        keygen_cfg.ext_ctrl_sel_info = NULL;
    }

    return rv;
}

/*
 * Function:
 *    _bcm_field_hx5_ft_group_update
 *
 * Purpose:
 *    Extract group ext codes again with new qualifiers and update
 *    qualifier info per part.
 *
 * Parameters:
 *    unit       - (IN) Unit number
 *    group      - (IN) Group Id
 *    qset       - (IN) New qualifiers qset
 *
 * Returns:
 *    BCM_E_NONE        - Success.
 */
int
_bcm_field_hx5_ft_group_update(int unit,
        bcm_field_group_t group, bcm_field_qset_t *qset)
{
    int rv = BCM_E_NONE; /* Return status.  */
    int idx = 0;         /* Index Variable. */
    int pidx = 0;        /* Index Variable. */
    int qid_index = 0;   /* Index Variable. */
    int qid;             /* Qualifier ID.   */
    int parts_count = 0; /* number of slices allocated for the group. */
    uint8 part;          /* Group Partition. */
    uint8 has_ft_ext_codes = FALSE;      /* If original field group has extractors */
    uint16 *qid_array[_FP_MAX_ENTRY_WIDTH] = { NULL };
                                            /* Array of qualifiers.    */
    uint16 size = 0;                        /* Total number of qualifiers in
                                               each partition of the group. */
    _field_group_t *fg = NULL;           /* Field group operational Structure. */
    _field_group_t *temp_fg = NULL;      /* Temp field group operational structure. */
    _field_stage_t *stage_fc = NULL;     /* Field Stage operational structure. */
    bcm_field_qset_t temp_qset;          /* Temporary qset information. */
    _bcm_field_qual_offset_t  *offset_array[_FP_MAX_ENTRY_WIDTH] = { NULL };
                                         /* Array of qualifiers offset information. */
    _field_group_add_fsm_t fsm_ptr;

    BCM_IF_ERROR_RETURN(_field_group_get(unit, group, &fg));
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit,
                _BCM_FIELD_STAGE_FLOWTRACKER, &stage_fc));

    sal_memcpy(&temp_qset, qset, sizeof(bcm_field_qset_t));

    for (qid = 0; qid < _bcmFieldQualifyCount; qid++) {
        if (BCM_FIELD_QSET_TEST(fg->qset, qid)) {
            BCM_FIELD_QSET_REMOVE(temp_qset, qid);
        }
    }

    _FP_XGS3_ALLOC(temp_fg, sizeof(_field_group_t), "Temp group for update");
    if (NULL == temp_fg) {
        return BCM_E_MEMORY;
    }

    temp_fg->gid = fg->gid;
    temp_fg->flags = fg->flags;
    sal_memcpy(&temp_fg->qset, &temp_qset, sizeof(bcm_field_qset_t));
    sal_memset(&fsm_ptr, 0, sizeof(_field_group_add_fsm_t));

    fsm_ptr.fg = temp_fg;
    fsm_ptr.stage_fc = stage_fc;
    fsm_ptr.flags = _FP_GROUP_QSET_UPDATE;

    /* Allocate memory for ext codes */
    if (fg->ft_ext_codes != NULL) {
        _FP_XGS3_ALLOC(temp_fg->ft_ext_codes,
                sizeof(_field_ft_keygen_ext_codes_t),
                "field group ext codes");
        if (temp_fg->ft_ext_codes == NULL) {
            sal_free(temp_fg);
            return BCM_E_MEMORY;
        }

        has_ft_ext_codes = TRUE;
        sal_memcpy(temp_fg->ft_ext_codes, fg->ft_ext_codes,
                sizeof(_field_ft_keygen_ext_codes_t));
    }
    rv = _field_hx5_ft_ext_code_assign(unit, 0, &fsm_ptr);

    if (BCM_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Verb: Processing group QSET.\n"), unit));
        goto cleanup;
    }

    for (part = 0; part < _FP_MAX_ENTRY_WIDTH; part++)  {
        /* If no new qualifier is extracted in the part, skip the part. */
        size = temp_fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part].size;
        if (size == 0) {
            continue;
        }
        size += fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part].size;
        if (size != 0) {
             _FP_XGS3_ALLOC(qid_array[part], size * sizeof(uint16), "Qid Array");
             if (NULL == qid_array[part]) {
                rv = BCM_E_MEMORY;
                goto cleanup;
             }
             _FP_XGS3_ALLOC(offset_array[part], size * sizeof(_bcm_field_qual_offset_t),
                                                        "Qualifier Offset Array");
             if (NULL == offset_array[part]) {
                rv = BCM_E_MEMORY;
                goto cleanup;
             }

             qid_index = 0;
             for (idx = 0; idx < fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part].size; idx++) {

                 sal_memcpy(&qid_array[part][qid_index],
                        &(fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part].qid_arr[idx]),
                        sizeof(uint16));

                 sal_memcpy(&offset_array[part][qid_index],
                            &(fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part].offset_arr[idx]),
                            sizeof(_bcm_field_qual_offset_t));

                 qid_index++;

             }

             fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part].size = size;

             /* Copy qualifier array and offset array of new
              * qualifiers in this partition.
              */
             size = temp_fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part].size;
             sal_memcpy(&qid_array[part][qid_index],
                        temp_fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part].qid_arr,
                        size * sizeof(uint16));

             sal_memcpy(&offset_array[part][qid_index],
                        temp_fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part].offset_arr,
                        size * sizeof(_bcm_field_qual_offset_t));

             if (NULL != fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part].qid_arr) {
                 sal_free(fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part].qid_arr);
             }

             if (NULL != fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part].offset_arr) {
                 sal_free(fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part].offset_arr);
             }

             if (NULL != temp_fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part].qid_arr) {
                 sal_free(temp_fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part].qid_arr);
             }

             if (NULL != temp_fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part].offset_arr) {
                 sal_free(temp_fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part].offset_arr);
             }

             fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part].qid_arr = qid_array[part];
             fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part].offset_arr = offset_array[part];
        }
    }

    rv = _bcm_field_th_entry_tcam_parts_count(unit, fg->flags, &parts_count);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    for (part = 0; part < _FP_MAX_ENTRY_WIDTH; part++)  {
         offset_array[part] = NULL;
         qid_array[part] = NULL;
    }

    if (has_ft_ext_codes == TRUE) {
        sal_memcpy(fg->ft_ext_codes, temp_fg->ft_ext_codes,
                      (sizeof(_field_ft_keygen_ext_codes_t)));
    } else {
        fg->ft_ext_codes = temp_fg->ft_ext_codes;
    }

    /* Install entry parts in hardware. Loop through all presels */
    for (pidx = 0; pidx < _FP_PRESEL_ENTRIES_MAX_PER_GROUP; pidx++) {
        if (fg->presel_ent_arr[pidx] != NULL) {
            rv = _field_hx5_ft_group_keygen_install(unit,
                    fg->presel_ent_arr[pidx]);
        }
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
    }

    for (qid = 0; qid < _bcmFieldQualifyCount; qid++) {
        if (BCM_FIELD_QSET_TEST(*qset, qid)) {
            BCM_FIELD_QSET_ADD(fg->qset, qid);
        }
    }

cleanup:
    for (part = 0; part < _FP_MAX_ENTRY_WIDTH; part++) {
         if (NULL != qid_array[part]) {
             sal_free(qid_array[part]);
             qid_array[part] = NULL;
         }

         if(NULL != offset_array[part]) {
             sal_free(offset_array[part]);
             offset_array[part] = NULL;
         }
    }

    if (NULL != temp_fg) {
        if ((temp_fg->ft_ext_codes != NULL) &&
            (has_ft_ext_codes == TRUE)) {
            sal_free(temp_fg->ft_ext_codes);
        }
        sal_free(temp_fg);
    }

    return (rv);
}

/****** Extraction ends ***************************************/

/*
 * Function:
 *    _bcm_field_keygen_qual_offset_info_dump
 *
 * Purpose:
 *    Dump the output of keygen for qualifiers and extractor
 *    codes.
 *
 * Parameters:
 *    unit       - (IN) Unit number
 *    keygen_parts_count - (IN) Keygen parts
 *    keygen_oper - (IN) Keygen operation info
 *
 * Returns:
 *    BCM_E_NONE        - Success.
 */
int
_bcm_field_keygen_qual_offset_info_dump(int unit,
                                 int keygen_parts_count,
                                 bcmi_keygen_oper_t *keygen_oper)
{
    int idx = 0;
    int part = 0;
    int offset = 0;
    int num_offsets = 0;
    bcm_field_qualify_t qid;
    bcmi_keygen_qual_offset_info_t *q_info = NULL;
    bcmi_keygen_ext_ctrl_sel_info_t *e_info = NULL;

    if (keygen_oper == NULL) {
        return BCM_E_NONE;
    }
    if (keygen_oper->ext_ctrl_sel_info_count == 0) {
        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                        " Extraction info not available\n\r")));
        return BCM_E_NONE;
    }

    /* Qualifiers offsets and widths */
    for (part = 0; part < keygen_parts_count; part++) {
        q_info = &(keygen_oper->qual_offset_info[part]);

        for (idx = 0; idx < q_info->size; idx++) {
            qid = q_info->qid_arr[idx];
            LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                            "qi[p=%d][%d] %s ->"), part, idx,
                        _field_qual_name(qid)));
            num_offsets = 0;
            for (offset = 0; offset < BCMI_KEYGEN_QUAL_OFFSETS_MAX; offset++) {
                if (!q_info->offset_arr[idx].width[offset]) {
                    continue;
                }
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                " {[%d] offset:%d, width:%d}"),
                            offset, q_info->offset_arr[idx].offset[offset],
                            q_info->offset_arr[idx].width[offset]));
                num_offsets++;
                if (num_offsets == q_info->offset_arr[idx].num_offsets) {
                    break;
                }
                if (((num_offsets + 1) % 3) == 0) {
                    LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit, "\n\r\t\t")));
                }
            }
            LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit, "\n\r")));
        }
    }

    /* Extractors */
    for (idx = 0; idx < keygen_oper->ext_ctrl_sel_info_count; idx++) {
        e_info = &keygen_oper->ext_ctrl_sel_info[idx];
        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                        "ei[p=%d][%d] -> ext_Id=0x%x ctrl_sel=%d "),
                    e_info->part, idx, e_info->ext_num, e_info->ctrl_sel));
        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,"val=%d l=%d g=%d\n\r"),
                    e_info->ctrl_sel_val, e_info->level, e_info->gran));
    }

    return (BCM_E_NONE);
}

#endif

