/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * Purpose:    Implements application interrupt lists for FE3200.
 */

/*************
 * INCLUDES  *
 */
#include <shared/bsl.h>

#include <soc/dfe/fe3200/fe3200_intr.h>

#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm_int/common/debug.h>

#include <appl/diag/system.h>
#include <appl/dcmn/interrupts/dcmn_intr.h>

/*************
 * DEFINES   *
 */
#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_APPL_INTR

/*************
 * TYPE DEFS *
 */
fe3200_interrupt_type fe3200_int_disable_on_init[] = {
    INVALIDr
};

fe3200_interrupt_type fe3200_int_active_on_init[] = {
    FE3200_INT_RTP_ERROR_ECC,
    FE3200_INT_RTP_DRHP_0_INT_REG,
    FE3200_INT_RTP_DRHP_1_INT_REG,
    FE3200_INT_RTP_DRHP_2_INT_REG,
    FE3200_INT_RTP_DRHP_3_INT_REG,
    FE3200_INT_RTP_CRH_0_INT_REG,
    FE3200_INT_RTP_CRH_1_INT_REG,
    FE3200_INT_RTP_CRH_2_INT_REG,
    FE3200_INT_RTP_CRH_3_INT_REG,
    FE3200_INT_RTP_GENERAL_INT_REG,
    FE3200_INT_DCL_ERROR_ECC,
    FE3200_INT_CCS_ERROR_ECC,
    FE3200_INT_DCM_ERROR_ECC,
    FE3200_INT_DCH_ERROR_ECC,
    FE3200_INT_FSRD_ERROR_ECC,
    FE3200_INT_FSRD_INT_REG_0,
    FE3200_INT_FSRD_INT_REG_1,
    FE3200_INT_FSRD_INT_REG_2,
    FE3200_INT_FMAC_ERROR_ECC,
    FE3200_INT_FMAC_INT_REG_1,
    FE3200_INT_FMAC_INT_REG_2,
    FE3200_INT_FMAC_INT_REG_3,
    FE3200_INT_FMAC_INT_REG_4,
    FE3200_INT_FMAC_INT_REG_5,
    FE3200_INT_FMAC_INT_REG_6,
    FE3200_INT_FMAC_INT_REG_7,
    FE3200_INT_FMAC_INT_REG_8,
    FE3200_INT_ECI_ERROR_ECC,
    FE3200_INT_CCS_ECC_ECC_1B_ERR_INT,
    FE3200_INT_CCS_ECC_ECC_2B_ERR_INT,
    FE3200_INT_DCH_ECC_ECC_1B_ERR_INT,
    FE3200_INT_DCH_ECC_ECC_2B_ERR_INT,
    FE3200_INT_DCH_ECC_PARITY_ERR_INT,
    FE3200_INT_DCL_ECC_ECC_1B_ERR_INT,
    FE3200_INT_DCL_ECC_ECC_2B_ERR_INT,
    FE3200_INT_DCL_ECC_PARITY_ERR_INT,
    FE3200_INT_DCM_ECC_ECC_1B_ERR_INT,
    FE3200_INT_DCM_ECC_ECC_2B_ERR_INT,
    FE3200_INT_DCM_ECC_PARITY_ERR_INT,
    FE3200_INT_ECI_ECC_ECC_1B_ERR_INT,
    FE3200_INT_ECI_ECC_ECC_2B_ERR_INT,
    FE3200_INT_ECI_ECC_PARITY_ERR_INT,
    FE3200_INT_FMAC_ECC_ECC_1B_ERR_INT,
    FE3200_INT_FMAC_ECC_ECC_2B_ERR_INT,
    FE3200_INT_FMAC_ECC_PARITY_ERR_INT,
    FE3200_INT_FSRD_ECC_ECC_1B_ERR_INT,
    FE3200_INT_FSRD_ECC_ECC_2B_ERR_INT,
    FE3200_INT_FSRD_ECC_PARITY_ERR_INT,
    FE3200_INT_RTP_ECC_ECC_1B_ERR_INT,
    FE3200_INT_RTP_ECC_ECC_2B_ERR_INT,
    FE3200_INT_RTP_ECC_PARITY_ERR_INT,
    FE3200_INT_DCMC_DCQ_0_LB_CNT_UNDERFLOW_P_0_INT,
    FE3200_INT_DCMC_DCQ_0_LB_CNT_UNDERFLOW_P_1_INT,
    FE3200_INT_DCMC_DCQ_0_LB_CNT_UNDERFLOW_P_2_INT,
    FE3200_INT_DCMC_DCQ_1_LB_CNT_UNDERFLOW_P_0_INT,
    FE3200_INT_DCMC_DCQ_1_LB_CNT_UNDERFLOW_P_1_INT,
    FE3200_INT_DCMC_DCQ_1_LB_CNT_UNDERFLOW_P_2_INT,
    FE3200_INT_DCMC_DCQ_2_LB_CNT_UNDERFLOW_P_0_INT,
    FE3200_INT_DCMC_DCQ_2_LB_CNT_UNDERFLOW_P_1_INT,
    FE3200_INT_DCMC_DCQ_2_LB_CNT_UNDERFLOW_P_2_INT,
    FE3200_INT_DCMC_DCQ_3_LB_CNT_UNDERFLOW_P_0_INT,
    FE3200_INT_DCMC_DCQ_3_LB_CNT_UNDERFLOW_P_1_INT,
    FE3200_INT_DCMC_DCQ_3_LB_CNT_UNDERFLOW_P_2_INT,
    INVALIDr
};

fe3200_interrupt_type fe3200_int_disable_print_on_init[] = {
    INVALIDr
};

/* } */
/*************
 * GLOBALS   *
 */
/* { */

/* These 2 arrays are not for use. They are junk
 * In case of getting compilation error that the size of one of them is zero or negative, 
 * it means that the value of _FFE3200_INT_LAST should be updated due to adding new items to fe3200_interrupt_type, 
 * or due to removing items from fe3200_interrupt_type in include/soc/dfe/fe3200/fe3200_intr.h. 
 */
char ___fe3200_interrupts_junk_array1[_FE3200_INT_LAST - FE3200_INT_LAST + 1];
char ___fe3200_interrupts_junk_array2[FE3200_INT_LAST - _FE3200_INT_LAST + 1];

/*************
 * FUNCTIONS *
 */
int
fe3200_interrupt_cmn_param_init(int unit, intr_common_params_t* common_params)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(common_params);

    common_params->int_disable_on_init = fe3200_int_disable_on_init;
    common_params->int_disable_print_on_init = fe3200_int_disable_print_on_init;
    common_params->int_active_on_init = fe3200_int_active_on_init;

exit:
    BCMDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME

