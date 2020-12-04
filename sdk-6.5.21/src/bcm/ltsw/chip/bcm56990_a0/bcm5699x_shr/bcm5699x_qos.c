/*! \file bcm5699x_qos.c
 *
 * BCM5699x QoS subordinate driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/mbcm/qos.h>
#include <bcm_int/ltsw/xgs/qos.h>
#include <bcm_int/ltsw/qos_int.h>

#include <bcmltd/chip/bcmltd_str.h>
#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_QOS

/******************************************************************************
 * Private functions
 */

static const bcmint_qos_fld_t phb_egr_l2_adapt_int_pri_to_otag_flds[] = {
    [BCMINT_LTSW_QOS_FLD_PHB_EGR_L2_ADAPT_INT_PRI_TO_OTAG_ID] = {
        PHB_EGR_L2_ADAPT_INT_PRI_TO_OTAG_IDs
    },
    [BCMINT_LTSW_QOS_FLD_PHB_EGR_L2_ADAPT_INT_PRI_TO_OTAG_INT_PRI] = {
        INT_PRIs
    },
    [BCMINT_LTSW_QOS_FLD_PHB_EGR_L2_ADAPT_INT_PRI_TO_OTAG_CNG] = {
        CNGs
    },
    [BCMINT_LTSW_QOS_FLD_PHB_EGR_L2_ADAPT_INT_PRI_TO_OTAG_PRI] = {
        PRIs
    },
    [BCMINT_LTSW_QOS_FLD_PHB_EGR_L2_ADAPT_INT_PRI_TO_OTAG_CFI] = {
        CFIs
    },
};

static const bcmint_qos_lt_t bcm5699x_qos_lt[] = {
    [BCMINT_LTSW_QOS_LT_PHB_EGR_L2_ADAPT_INT_PRI_TO_OTAG] = {
        .name = PHB_EGR_L2_ADAPT_INT_PRI_TO_OTAGs,
        .fld_bmp =
            (1 << BCMINT_LTSW_QOS_FLD_PHB_EGR_L2_ADAPT_INT_PRI_TO_OTAG_ID) |
            (1 << BCMINT_LTSW_QOS_FLD_PHB_EGR_L2_ADAPT_INT_PRI_TO_OTAG_INT_PRI) |
            (1 << BCMINT_LTSW_QOS_FLD_PHB_EGR_L2_ADAPT_INT_PRI_TO_OTAG_CNG) |
            (1 << BCMINT_LTSW_QOS_FLD_PHB_EGR_L2_ADAPT_INT_PRI_TO_OTAG_PRI) |
            (1 << BCMINT_LTSW_QOS_FLD_PHB_EGR_L2_ADAPT_INT_PRI_TO_OTAG_CFI),
        .flds = phb_egr_l2_adapt_int_pri_to_otag_flds
    }
};

static int
bcm5699x_ltsw_qos_lt_db_get(int unit, bcmint_qos_lt_db_t *lt_db)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lt_db, SHR_E_MEMORY);

    lt_db->lt_bmp = (1 << BCMINT_LTSW_QOS_LT_PHB_EGR_L2_ADAPT_INT_PRI_TO_OTAG);

    lt_db->lts = bcm5699x_qos_lt;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief QoS sub driver functions for bcm5699x.
 */
static mbcm_ltsw_qos_drv_t bcm5699x_qos_sub_drv = {
    .qos_lt_db_get = bcm5699x_ltsw_qos_lt_db_get,
};

/******************************************************************************
 * Public functions
 */

int
bcm5699x_qos_sub_drv_attach(int unit, void *hdl)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_qos_drv_set(unit, &bcm5699x_qos_sub_drv));

exit:
    SHR_FUNC_EXIT();
}
