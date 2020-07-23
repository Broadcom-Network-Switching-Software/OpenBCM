/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_BCM_INTR

/* 
 *  include  
 */ 
#include <sal/core/time.h>
#include <sal/core/dpc.h>

#include <soc/intr.h>
#include <soc/drv.h>
#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/fe1600/fe1600_interrupts.h>
#include <soc/dfe/fe1600/fe1600_port.h>

#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm_int/dfe_dispatch.h>
#include <bcm_int/common/debug.h>

#include <appl/diag/system.h>

#include <appl/dfe/interrupts/interrupt_handler.h>
#include <appl/dfe/interrupts/interrupt_handler_cb_func.h>
#include <appl/dfe/interrupts/interrupt_handler_corr_act_func.h>

#include <appl/dcmn/interrupts/interrupt_handler.h>

fe1600_interrupt_type fe1600_int_disable_on_init[] = {
    FE1600_INT_LAST /*always last*/
};

fe1600_interrupt_type fe1600_int_active_on_init[] = {
    FE1600_INT_DCH_DCHDESCCNTOP,
    FE1600_INT_DCH_DCHDESCCNTOS,
    FE1600_INT_DCH_OUTOFSYNCINTP,
    FE1600_INT_DCH_OUTOFSYNCINTS,
/*
 *  Interrupts that are Vectors pointing to other Interrupt Register.
 */
    FE1600_INT_DCH_INTREG1INT,
    FE1600_INT_DCH_INTREG2INT,
    FE1600_INT_RTP_DRHP0INTREG,
    FE1600_INT_RTP_DRHP1INTREG,
    FE1600_INT_RTP_DRHP2INTREG,
    FE1600_INT_RTP_DRHP3INTREG,
    FE1600_INT_RTP_DRHS0INTREG,
    FE1600_INT_RTP_DRHS1INTREG,
    FE1600_INT_RTP_DRHS2INTREG,
    FE1600_INT_RTP_DRHS3INTREG,
    FE1600_INT_RTP_CRH0INTREG,
    FE1600_INT_RTP_CRH1INTREG,
    FE1600_INT_RTP_GENERALINTREG,
    FE1600_INT_RTP_ECC1BERRINTREG,
    FE1600_INT_RTP_ECC2BERRINTREG,
    FE1600_INT_RTP_PARERRINTREG,
    FE1600_INT_MAC_INTREG1,
    FE1600_INT_MAC_INTREG2,
    FE1600_INT_MAC_INTREG3,
    FE1600_INT_MAC_INTREG4,
    FE1600_INT_MAC_INTREG5,
    FE1600_INT_MAC_INTREG6,
    FE1600_INT_MAC_INTREG7,
    FE1600_INT_MAC_INTREG8,
    FE1600_INT_MAC_INTREG9,
    FE1600_INT_SRD_INTREG0,
    FE1600_INT_SRD_INTREG1,
    FE1600_INT_SRD_INTREG2,
    FE1600_INT_SRD_INTREG3,
/*
 * Memory Error Interrupts
 */
    FE1600_INT_DCH_ECC_1BERRINT,
    FE1600_INT_DCH_ECC_2BERRINT,
    FE1600_INT_DCH_PARITYERRINT,
    FE1600_INT_DCL_ECC_1BERRINT,
    FE1600_INT_DCL_ECC_2BERRINT,
    FE1600_INT_DCL_PARITYERRINT,
    FE1600_INT_DCMA_ECC_1BERRINT,
    FE1600_INT_DCMA_ECC_2BERRINT,
    FE1600_INT_DCMA_PARITYERRINT,
    FE1600_INT_DCMB_ECC_1BERRINT,
    FE1600_INT_DCMB_ECC_2BERRINT,
    FE1600_INT_DCMB_PARITYERRINT,
    FE1600_INT_CCS_ECC_1BERRINT,
    FE1600_INT_CCS_ECC_2BERRINT,
    FE1600_INT_CCS_PARITYERRINT,
    FE1600_INT_RTP_ECC1BERRINTREG,
    FE1600_INT_RTP_ECC2BERRINTREG,
    FE1600_INT_RTP_PARERRINTREG,
    FE1600_INT_RTP_ECC_1BERRINT,
    FE1600_INT_RTP_ECC_2BERRINT,
    FE1600_INT_RTP_PARITYERRINT,
    FE1600_INT_MAC_ECC_1BERRINT,
    FE1600_INT_MAC_ECC_2BERRINT,
    FE1600_INT_LAST /*always last*/
};

fe1600_interrupt_type fe1600_int_disable_print_on_init[] = {
    FE1600_INT_LAST
};

int
fe1600_interrupt_handler_init_cmn_param(int unit, interrupt_common_params_t* common_params)
{
    int nof_interrupts;
    int rc;

    SOC_INIT_FUNC_DEFS;

    SOC_NULL_CHECK(common_params);
 
    /*get number of interrupts */
    rc = soc_fe1600_nof_interrupts(unit, &nof_interrupts);
    _SOC_IF_ERR_EXIT(rc);
  
    common_params->nof_interrupts = nof_interrupts;
    common_params->interrupt_add_interrupt_handler_init = fe1600_interrupt_add_interrupt_handler_init;
    common_params->interrupt_handle_block_instance = soc_dfe_nof_block_instances;
    common_params->int_disable_on_init = fe1600_int_disable_on_init;
    common_params->int_active_on_init = fe1600_int_active_on_init;
    common_params->int_disable_print_on_init = fe1600_int_disable_print_on_init;
    common_params->cached_mem = NULL;

exit:
    SOC_FUNC_RETURN;

}

#undef _ERR_MSG_MODULE_NAME


