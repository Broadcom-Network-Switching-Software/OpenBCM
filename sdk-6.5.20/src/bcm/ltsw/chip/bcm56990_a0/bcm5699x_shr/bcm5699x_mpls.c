/*! \file bcm5699x_mpls.c
 *
 * BCM5699x MPLS subordinate driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/mpls_int.h>
#include <bcm_int/ltsw/mbcm/mpls.h>
#include <bcm_int/ltsw/xgs/types.h>

#include <bcmltd/chip/bcmltd_str.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_MPLS

static const char
*bos_label_action_str[bcmintLtswMplsTnlBosActionCnt] = {
                      [bcmintLtswMplsTnlBosActionInvalid]       = INVALIDs,
                      [bcmintLtswMplsTnlBosL3Iif]       = L3_IIFs,
                      [bcmintLtswMplsTnlBosSwapNhi]        = SWAP_NHIs,
                      [bcmintLtswMplsTnlBosL3Nhi]  = L3_NHIs,
                      [bcmintLtswMplsTnlBosL3Ecmp] = L3_ECMPs,
                      [bcmintLtswMplsTnlBosSwapEcmp] = SWAP_ECMPs,
                    };

static const char
*non_bos_label_action_str[bcmintLtswMplsTnlBosActionCnt] = {
                      [bcmintLtswMplsTnlBosActionInvalid]       = INVALIDs,
                      [bcmintLtswMplsTnlNonBosPop]       = POPs,
                      [bcmintLtswMplsTnlNonBosPhpNhi]        = PHP_NHIs,
                      [bcmintLtswMplsTnlNonBosSwapNhi]  = SWAP_NHIs,
                      [bcmintLtswMplsTnlNonBosSwapEcmp] = SWAP_ECMPs,
                      [bcmintLtswMplsTnlNonBosPhpEcmp] = PHP_ECMPs,
                    };

static const char
*exp_map_action_str[bcmintLtswMplsExpMapActionCnt] = {
                      [bcmintLtswMplsExpMapActionNone]       = NONEs,
                      [bcmintLtswMplsExpMapActionMapPriCng]       = MAP_PRI_CNGs,
                      [bcmintLtswMplsExpMapActionFixedPriMapCng]        = FIXED_PRI_MAP_CNGs,
                    };
/*!
 * \brief Get bos label action from sybmol.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol String.
 * \param [out] value label action.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bos_action_symbol_to_scalar(int unit, const char *symbol, uint64_t *value)
{
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i< bcmintLtswMplsTnlBosActionCnt; i++) {
        if (!sal_strcmp(symbol, bos_label_action_str[i])) {
            break;
        }
    }

    if (i >= bcmintLtswMplsTnlBosActionCnt) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *value = i;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get bos label action sybmol from value.
 *
 * \param [in] unit Unit number.
 * \param [in] value dest_type.
 * \param [out] symbol String.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bos_action_scalar_to_symbol(int unit, uint64_t value, const char **symbol)
{
    SHR_FUNC_ENTER(unit);

    if (value >= bcmintLtswMplsTnlBosActionCnt) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *symbol = bos_label_action_str[value];

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get non bos label action from sybmol.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol String.
 * \param [out] value label action.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
non_bos_action_symbol_to_scalar(int unit, const char *symbol, uint64_t *value)
{
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i< bcmintLtswMplsTnlNonBosActionCnt; i++) {
        if (!sal_strcmp(symbol, non_bos_label_action_str[i])) {
            break;
        }
    }

    if (i >= bcmintLtswMplsTnlNonBosActionCnt) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *value = i;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get non bos label action sybmol from value.
 *
 * \param [in] unit Unit number.
 * \param [in] value dest_type.
 * \param [out] symbol String.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
non_bos_action_scalar_to_symbol(int unit, uint64_t value, const char **symbol)
{
    SHR_FUNC_ENTER(unit);

    if (value >= bcmintLtswMplsTnlNonBosActionCnt) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *symbol = non_bos_label_action_str[value];

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get exp map action from sybmol.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol String.
 * \param [out] value label action.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
exp_map_action_symbol_to_scalar(int unit, const char *symbol, uint64_t *value)
{
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i< bcmintLtswMplsExpMapActionCnt; i++) {
        if (!sal_strcmp(symbol, exp_map_action_str[i])) {
            break;
        }
    }

    if (i >= bcmintLtswMplsExpMapActionCnt) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *value = i;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get exp map action sybmol from value.
 *
 * \param [in] unit Unit number.
 * \param [in] value dest_type.
 * \param [out] symbol String.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
exp_map_action_scalar_to_symbol(int unit, uint64_t value, const char **symbol)
{
    SHR_FUNC_ENTER(unit);

    if (value >= bcmintLtswMplsExpMapActionCnt) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *symbol = exp_map_action_str[value];

exit:
    SHR_FUNC_EXIT();
}

static const bcmint_mpls_fld_t
eflex_action_flds[BCMINT_LTSW_MPLS_FLD_EFA_FLD_CNT] = {
    [BCMINT_LTSW_MPLS_FLD_EFA_ID] = {
        TNL_MPLS_CTR_ING_EFLEX_ACTION_IDs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_EFA_ACTION] = {
        ACTIONs,
        NULL,
        NULL
    }
};

static const bcmint_mpls_fld_t
decap_trunk_flds[BCMINT_LTSW_MPLS_FLD_DECAP_FLD_CNT] = {
    [BCMINT_LTSW_MPLS_FLD_DECAP_LABEL] = {
        MPLS_LABELs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_DECAP_SOURCE] = {
        TRUNK_IDs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_DECAP_BOS_ACTIONS] = {
        BOS_ACTIONSs,
        bos_action_symbol_to_scalar,
        bos_action_scalar_to_symbol
    },
    [BCMINT_LTSW_MPLS_FLD_DECAP_NON_BOS_ACTIONS] = {
        NON_BOS_ACTIONSs,
        non_bos_action_symbol_to_scalar,
        non_bos_action_scalar_to_symbol
    },
    [BCMINT_LTSW_MPLS_FLD_DECAP_L3_IIF_ID] = {
        L3_IIF_IDs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_DECAP_NHOP_ID] = {
        NHOP_IDs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_DECAP_ECMP_ID] = {
        ECMP_IDs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_DECAP_USE_TTL_FROM_DECAP_HDR] = {
       USE_TTL_FROM_DECAP_HDRs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_DECAP_KEEP_PAYLOAD_DSCP] = {
        KEEP_PAYLOAD_DSCPs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_DECAP_TNL_EXP_MAP] = {
        TNL_EXP_TO_INNER_EXPs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_DECAP_EXP_MAP_ACTION] = {
        EXP_MAP_ACTIONs,
        exp_map_action_symbol_to_scalar,
        exp_map_action_scalar_to_symbol
    },
    [BCMINT_LTSW_MPLS_FLD_DECAP_EXP_TO_INT_PRI_ID] = {
        PHB_ING_MPLS_EXP_TO_INT_PRI_IDs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_DECAP_IPV6_PAYLOAD] = {
        IPV6_PAYLOADs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_DECAP_IPV4_PAYLOAD] = {
        IPV4_PAYLOADs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_DECAP_DROP] = {
        DROPs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_DECAP_INT_PRI] = {
        INT_PRIs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_DECAP_EXP_TO_IP_ECN_ID] = {
        ECN_MPLS_EXP_TO_IP_ECN_IDs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_DECAP_BFD] = {
        BFDs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_DECAP_CTR_EFLEX_OBJECT] = {
        CTR_ING_EFLEX_OBJECTs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_DECAP_CTR_EFLEX_ACTION] = {
        CTR_ING_EFLEX_ACTIONs,
        NULL,
        NULL
    },
};

static const bcmint_mpls_fld_t
decap_flds[BCMINT_LTSW_MPLS_FLD_DECAP_FLD_CNT] = {
    [BCMINT_LTSW_MPLS_FLD_DECAP_LABEL] = {
        MPLS_LABELs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_DECAP_SOURCE] = {
        MODPORTs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_DECAP_BOS_ACTIONS] = {
        BOS_ACTIONSs,
        bos_action_symbol_to_scalar,
        bos_action_scalar_to_symbol
    },
    [BCMINT_LTSW_MPLS_FLD_DECAP_NON_BOS_ACTIONS] = {
        NON_BOS_ACTIONSs,
        non_bos_action_symbol_to_scalar,
        non_bos_action_scalar_to_symbol
    },
    [BCMINT_LTSW_MPLS_FLD_DECAP_L3_IIF_ID] = {
        L3_IIF_IDs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_DECAP_NHOP_ID] = {
        NHOP_IDs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_DECAP_ECMP_ID] = {
        ECMP_IDs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_DECAP_USE_TTL_FROM_DECAP_HDR] = {
       USE_TTL_FROM_DECAP_HDRs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_DECAP_KEEP_PAYLOAD_DSCP] = {
        KEEP_PAYLOAD_DSCPs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_DECAP_TNL_EXP_MAP] = {
        TNL_EXP_TO_INNER_EXPs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_DECAP_EXP_MAP_ACTION] = {
        EXP_MAP_ACTIONs,
        exp_map_action_symbol_to_scalar,
        exp_map_action_scalar_to_symbol
    },
    [BCMINT_LTSW_MPLS_FLD_DECAP_EXP_TO_INT_PRI_ID] = {
        PHB_ING_MPLS_EXP_TO_INT_PRI_IDs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_DECAP_IPV6_PAYLOAD] = {
        IPV6_PAYLOADs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_DECAP_IPV4_PAYLOAD] = {
        IPV4_PAYLOADs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_DECAP_DROP] = {
        DROPs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_DECAP_INT_PRI] = {
        INT_PRIs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_DECAP_EXP_TO_IP_ECN_ID] = {
        ECN_MPLS_EXP_TO_IP_ECN_IDs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_DECAP_BFD] = {
        BFDs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_DECAP_CTR_EFLEX_OBJECT] = {
        CTR_ING_EFLEX_OBJECTs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_DECAP_CTR_EFLEX_ACTION] = {
        CTR_ING_EFLEX_ACTIONs,
        NULL,
        NULL
    },
};

static const bcmint_mpls_lt_t
mpls_lt[] = {
    [BCMINT_LTSW_MPLS_LT_EFLEX_ACTION] = {
        .name = TNL_MPLS_CTR_ING_EFLEX_ACTIONs,
        .fld_bmp =
            (1 << BCMINT_LTSW_MPLS_FLD_EFA_ID) |
            (1 << BCMINT_LTSW_MPLS_FLD_EFA_ACTION),
        .flds = eflex_action_flds
    },
    [BCMINT_LTSW_MPLS_LT_DECAP_TRUNK] = {
        .name = TNL_MPLS_DECAP_TRUNKs,
        .fld_bmp =
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_LABEL) |
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_SOURCE) |
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_BOS_ACTIONS) |
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_NON_BOS_ACTIONS) |
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_L3_IIF_ID) |
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_NHOP_ID) |
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_ECMP_ID) |
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_USE_TTL_FROM_DECAP_HDR) |
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_KEEP_PAYLOAD_DSCP) |
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_TNL_EXP_MAP) |
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_EXP_MAP_ACTION) |
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_EXP_TO_INT_PRI_ID) |
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_IPV6_PAYLOAD) |
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_IPV4_PAYLOAD) |
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_DROP) |
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_INT_PRI) |
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_EXP_TO_IP_ECN_ID) |
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_BFD) |
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_CTR_EFLEX_OBJECT) |
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_CTR_EFLEX_ACTION),
        .flds = decap_trunk_flds
    },
    [BCMINT_LTSW_MPLS_LT_DECAP] = {
        .name = TNL_MPLS_DECAPs,
        .fld_bmp =
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_LABEL) |
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_SOURCE) |
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_BOS_ACTIONS) |
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_NON_BOS_ACTIONS) |
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_L3_IIF_ID) |
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_NHOP_ID) |
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_ECMP_ID) |
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_USE_TTL_FROM_DECAP_HDR) |
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_KEEP_PAYLOAD_DSCP) |
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_TNL_EXP_MAP) |
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_EXP_MAP_ACTION) |
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_EXP_TO_INT_PRI_ID) |
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_IPV6_PAYLOAD) |
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_IPV4_PAYLOAD) |
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_DROP) |
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_INT_PRI) |
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_EXP_TO_IP_ECN_ID) |
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_BFD) |
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_CTR_EFLEX_OBJECT) |
            (1 << BCMINT_LTSW_MPLS_FLD_DECAP_CTR_EFLEX_ACTION),
        .flds = decap_flds
    }
};

/******************************************************************************
 * Private functions
 */

static int
bcm5699x_ltsw_mpls_lt_db_get(int unit,
                             bcmint_mpls_lt_db_t *lt_db)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lt_db, SHR_E_PARAM);

    lt_db->lt_bmp = (1 << BCMINT_LTSW_MPLS_LT_EFLEX_ACTION) |
                    (1 << BCMINT_LTSW_MPLS_LT_DECAP_TRUNK) |
                    (1 << BCMINT_LTSW_MPLS_LT_DECAP);

    lt_db->lts = mpls_lt;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Mpls sub driver functions for bcm5699x.
 */
static mbcm_ltsw_mpls_drv_t bcm5699x_mpls_sub_drv = {
    .mpls_lt_db_get = bcm5699x_ltsw_mpls_lt_db_get,
};

/******************************************************************************
 * Public functions
 */

int
bcm5699x_mpls_sub_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mpls_drv_set(unit, &bcm5699x_mpls_sub_drv));

exit:
    SHR_FUNC_EXIT();
}
