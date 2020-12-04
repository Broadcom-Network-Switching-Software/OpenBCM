/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 *  $Copyright: (c) 2014 Broadcom Corporation All Rights Reserved.$
 * 
*/


#include <phymod/phymod.h>
#include "eagle_cfg_seq.h" 
#include <phymod/chip/bcmi_tsce_xgxs_defs.h>
#include "eagle_tsc_fields.h"
#include "eagle_tsc_dependencies.h"
#include "eagle_tsc_field_access.h"

/** Macro to execute a statement with automatic return of error LESTM(Local ESTM) to fix coverity error */
#define LESTM(statement) do {err_code_t __err = ERR_CODE_NONE; statement; if (__err) return(__err); } while(0)
/** Macro to invoke a function with automatic return of error */
#define LEFUN(fun) do {err_code_t __err = (fun); if (__err) return(__err); } while(0)



err_code_t eagle_uc_active_set(const phymod_access_t *pa, uint32_t enable)           /* set microcontroller active or not  */
{
    DIG_TOP_USER_CTL0r_t uc_active_reg;

    DIG_TOP_USER_CTL0r_CLR(uc_active_reg);
    DIG_TOP_USER_CTL0r_UC_ACTIVEf_SET(uc_active_reg, enable);
    MODIFY_DIG_TOP_USER_CTL0r(pa, uc_active_reg);
      
    return ERR_CODE_NONE;
}

err_code_t eagle_uc_active_get(const phymod_access_t *pa, uint32_t *enable)           /* get microcontroller active or not  */
{
    DIG_TOP_USER_CTL0r_t uc_active_reg;

    DIG_TOP_USER_CTL0r_CLR(uc_active_reg);
    READ_DIG_TOP_USER_CTL0r(pa, &uc_active_reg);
    *enable = DIG_TOP_USER_CTL0r_UC_ACTIVEf_GET(uc_active_reg);

    return ERR_CODE_NONE;
}

err_code_t eagle_uc_reset(const phymod_access_t *pa, uint32_t enable)           /* set dw8501 reset  */
{
    UC_COMMANDr_t command_reg;

    UC_COMMANDr_CLR(command_reg);
    UC_COMMANDr_MICRO_MDIO_DW8051_RESET_Nf_SET(command_reg, enable);
    MODIFY_UC_COMMANDr(pa, command_reg);
 
    return ERR_CODE_NONE;
}

err_code_t eagle_prbs_tx_inv_data_get(const phymod_access_t *pa, uint32_t *inv_data)
{
    TLB_TX_PRBS_GEN_CFGr_t tmp_reg;
    TLB_TX_PRBS_GEN_CFGr_CLR(tmp_reg);
    READ_TLB_TX_PRBS_GEN_CFGr(pa, &tmp_reg);
    *inv_data = TLB_TX_PRBS_GEN_CFGr_PRBS_GEN_INVf_GET(tmp_reg);

    return ERR_CODE_NONE;
}

err_code_t eagle_prbs_rx_inv_data_get(const phymod_access_t *pa, uint32_t *inv_data)
{
    TLB_RX_PRBS_CHK_CFGr_t tmp_reg;
    TLB_RX_PRBS_CHK_CFGr_CLR(tmp_reg);
    READ_TLB_RX_PRBS_CHK_CFGr(pa, &tmp_reg);
    *inv_data = TLB_RX_PRBS_CHK_CFGr_PRBS_CHK_INVf_GET(tmp_reg);
    return ERR_CODE_NONE;
}


err_code_t eagle_prbs_tx_poly_get(const phymod_access_t *pa, eagle_prbs_polynomial_type_t *prbs_poly)
{
    TLB_TX_PRBS_GEN_CFGr_t tmp_reg;
    TLB_TX_PRBS_GEN_CFGr_CLR(tmp_reg);
    READ_TLB_TX_PRBS_GEN_CFGr(pa, &tmp_reg);
    *prbs_poly = TLB_TX_PRBS_GEN_CFGr_PRBS_GEN_MODE_SELf_GET(tmp_reg);

    return ERR_CODE_NONE;
}

err_code_t eagle_prbs_rx_poly_get(const phymod_access_t *pa, eagle_prbs_polynomial_type_t *prbs_poly)
{
    TLB_RX_PRBS_CHK_CFGr_t tmp_reg;
    TLB_RX_PRBS_CHK_CFGr_CLR(tmp_reg);
    READ_TLB_RX_PRBS_CHK_CFGr(pa, &tmp_reg);
    *prbs_poly = TLB_RX_PRBS_CHK_CFGr_PRBS_CHK_MODE_SELf_GET(tmp_reg);
    return ERR_CODE_NONE;
}

err_code_t eagle_prbs_tx_enable_get(const phymod_access_t *pa, uint32_t *enable)
{
    TLB_TX_PRBS_GEN_CFGr_t tmp_reg;
    TLB_TX_PRBS_GEN_CFGr_CLR(tmp_reg);
    READ_TLB_TX_PRBS_GEN_CFGr(pa, &tmp_reg);
    *enable = TLB_TX_PRBS_GEN_CFGr_PRBS_GEN_ENf_GET(tmp_reg);

    return ERR_CODE_NONE;
}

err_code_t eagle_prbs_rx_enable_get(const phymod_access_t *pa, uint32_t *enable)
{
    TLB_RX_PRBS_CHK_CFGr_t tmp_reg;
    TLB_RX_PRBS_CHK_CFGr_CLR(tmp_reg);
    READ_TLB_RX_PRBS_CHK_CFGr(pa, &tmp_reg);
    *enable = TLB_RX_PRBS_CHK_CFGr_PRBS_CHK_ENf_GET(tmp_reg);
    return ERR_CODE_NONE;
}

err_code_t eagle_pll_mode_set(const phymod_access_t *pa, int pll_mode)   /* PLL divider set */
{
    PLL_CAL_CTL7r_t eagle_xgxs_ctl_7_reg;

    PLL_CAL_CTL7r_CLR(eagle_xgxs_ctl_7_reg);
    PLL_CAL_CTL7r_PLL_MODEf_SET(eagle_xgxs_ctl_7_reg, pll_mode);
    MODIFY_PLL_CAL_CTL7r(pa, eagle_xgxs_ctl_7_reg); 
    return ERR_CODE_NONE;
}


err_code_t eagle_core_soft_reset_release(const phymod_access_t *pa, uint32_t enable)   /* release the pmd core soft reset */
{
    DIG_TOP_USER_CTL0r_t reg;

    DIG_TOP_USER_CTL0r_CLR(reg);
    DIG_TOP_USER_CTL0r_CORE_DP_S_RSTBf_SET(reg, enable);
    MODIFY_DIG_TOP_USER_CTL0r(pa, reg);
    return ERR_CODE_NONE;
}

err_code_t eagle_lane_soft_reset_release(const phymod_access_t *pa, uint32_t enable)   /* release the pmd core soft reset */
{
    CKRST_LN_CLK_RST_N_PWRDWN_CTLr_t    reg_pwrdwn_ctrl;

    CKRST_LN_CLK_RST_N_PWRDWN_CTLr_CLR(reg_pwrdwn_ctrl);
    CKRST_LN_CLK_RST_N_PWRDWN_CTLr_LN_DP_S_RSTBf_SET(reg_pwrdwn_ctrl, enable);
    MODIFY_CKRST_LN_CLK_RST_N_PWRDWN_CTLr(pa,  reg_pwrdwn_ctrl);

    return ERR_CODE_NONE;
}

err_code_t eagle_pram_flop_set(const phymod_access_t *pa, int value)   /* release the pmd core soft reset */
{
    UC_COMMAND3r_t uc_reg;
    UC_COMMAND3r_CLR(uc_reg);
    UC_COMMAND3r_MICRO_PRAM_IF_FLOP_BYPASSf_SET(uc_reg, value);
    MODIFY_UC_COMMAND3r(pa, uc_reg);
    return ERR_CODE_NONE;
}


err_code_t eagle_pram_firmware_enable(const phymod_access_t *pa, int enable)   /* release the pmd core soft reset */
{
    UC_COMMAND3r_t uc_reg;
    /* UC_COMMAND4r_t reg; */
    UC_COMMAND3r_CLR(uc_reg);
    /* UC_COMMAND4r_CLR(reg); */
    if(enable == 1) {
    UC_COMMAND3r_MICRO_PRAM_IF_RSTBf_SET(uc_reg, 1);
    UC_COMMAND3r_MICRO_PRAM_IF_ENf_SET(uc_reg, 1);
    UC_COMMAND3r_MICRO_PRAM_IF_FLOP_BYPASSf_SET(uc_reg, 0);
    /*
    UC_COMMAND4r_MICRO_SYSTEM_CLK_ENf_SET(reg, 1);
    UC_COMMAND4r_MICRO_SYSTEM_RESET_Nf_SET(reg, 1);
    MODIFY_UC_COMMAND4r(pa, reg);
    */
    } else {
    UC_COMMAND3r_MICRO_PRAM_IF_RSTBf_SET(uc_reg, 0);
    UC_COMMAND3r_MICRO_PRAM_IF_ENf_SET(uc_reg, 0);
    UC_COMMAND3r_MICRO_PRAM_IF_FLOP_BYPASSf_SET(uc_reg, 1);
    }
    MODIFY_UC_COMMAND3r(pa, uc_reg);
    return ERR_CODE_NONE;
}

err_code_t eagle_pmd_force_signal_detect(const phymod_access_t *pa, int enable)   
{
    SIGDET_CTL1r_t reg;
    SIGDET_CTL1r_CLR(reg);
    SIGDET_CTL1r_SIGNAL_DETECT_FRCf_SET(reg, enable);
    SIGDET_CTL1r_SIGNAL_DETECT_FRC_VALf_SET(reg, enable);
    MODIFY_SIGDET_CTL1r(pa, reg);
    return ERR_CODE_NONE;
}

err_code_t eagle_pmd_loopback_get(const phymod_access_t *pa, uint32_t *enable)   
{
    TLB_RX_DIG_LPBK_CFGr_t reg;
    READ_TLB_RX_DIG_LPBK_CFGr(pa, &reg);
    *enable = TLB_RX_DIG_LPBK_CFGr_DIG_LPBK_ENf_GET(reg);
    return ERR_CODE_NONE;
}

err_code_t eagle_pmd_cl72_enable_get(const phymod_access_t *pa, uint32_t *enable)   
{
    CL72_TXBASE_R_PMD_CTLr_t reg;
    READ_CL72_TXBASE_R_PMD_CTLr(pa, &reg);
    *enable = CL72_TXBASE_R_PMD_CTLr_CL72_IEEE_TRAINING_ENABLEf_GET(reg);
    return ERR_CODE_NONE;
}

err_code_t eagle_pmd_cl72_receiver_status(const phymod_access_t *pa, uint32_t *status)   
{
    CL72_TXBASE_R_PMD_STSr_t reg;
    READ_CL72_TXBASE_R_PMD_STSr(pa, &reg);
    *status = CL72_TXBASE_R_PMD_STSr_CL72_IEEE_RECEIVER_STATUSf_GET(reg);
    return ERR_CODE_NONE;
}

err_code_t eagle_uc_init(const phymod_access_t *pa)
{
  uint8_t result;

  LEFUN(wrc_micro_system_clk_en(0x1));                         /* Enable clock to micro  */
  LEFUN(wrc_micro_system_reset_n(0x1));                        /* De-assert reset to micro */
  LEFUN(wrc_micro_system_reset_n(0x0));                        /* Assert reset to micro - Toggling micro reset*/
  LEFUN(wrc_micro_system_reset_n(0x1));                        /* De-assert reset to micro */

  eagle_pram_flop_set(pa, 0x0);
  LEFUN(wrc_micro_mdio_ram_access_mode(0x0));                  /* Select Program Memory access mode - Program RAM load when Serdes DW8051 in reset */

  LEFUN(wrc_micro_ram_address(0x0));                           /* RAM start address */
  LEFUN(wrc_micro_init_cmd(0x0));                              /* Clear initialization command */
  LEFUN(wrc_micro_init_cmd(0x1));                              /* Set initialization command */

  eagle_tsc_delay_us(4000);

  LESTM(result = !rdc_micro_init_done());
	if (result) {                                        /* Check if initialization done within 500us time interval */
		PHYMOD_DEBUG_ERROR(("ERR_CODE_MICRO_INIT_NOT_DONE\n"));
		return (ERR_CODE_MICRO_INIT_NOT_DONE);    /* Else issue error code */
	}
  LEFUN(wrc_micro_init_cmd(0x0));                              /* Clear initialization command */
  return ( ERR_CODE_NONE );

}


/***********************************************/
/*  Microcode Init into Program RAM Functions  */
/***********************************************/

  /* uCode Load through Register (MDIO) Interface [Return Val = Error_Code (0 = PASS)] */
err_code_t eagle_tsc_ucode_init( const phymod_access_t *pa ) {

    uint8_t result;

	LEFUN(wrc_micro_system_clk_en(0x1));                         /* Enable clock to micro  */
	LEFUN(wrc_micro_system_reset_n(0x1));                        /* De-assert reset to micro */
	LEFUN(wrc_micro_system_reset_n(0x0));                        /* Assert reset to micro - Toggling micro reset*/
	LEFUN(wrc_micro_system_reset_n(0x1));                        /* De-assert reset to micro */

	LEFUN(wrc_micro_mdio_ram_access_mode(0x0));                  /* Select Program Memory access mode - Program RAM load when Serdes DW8051 in reset */

	LEFUN(wrc_micro_ram_address(0x0));                           /* RAM start address */
	LEFUN(wrc_micro_init_cmd(0x0));                              /* Clear initialization command */
	LEFUN(wrc_micro_init_cmd(0x1));                              /* Set initialization command */

  /* coverity[check_return] */
	eagle_tsc_delay_us(500);
	LEFUN(wrc_micro_init_cmd(0x0));                              /* Set initialization command */

    LESTM(result = !rdc_micro_init_done());

	if (result) {                                        /* Check if initialization done within 500us time interval */
		PHYMOD_DEBUG_ERROR(("ERR_CODE_MICRO_INIT_NOT_DONE\n"));
		return (ERR_CODE_MICRO_INIT_NOT_DONE);    /* Else issue error code */
	}

	return (ERR_CODE_NONE);
}

err_code_t eagle_pmd_ln_h_rstb_pkill_override( const phymod_access_t *pa, uint16_t val) 
{
	CKRST_LN_RST_N_PWRDN_PIN_KILL_CTLr_t pin_ctl;

	/* 
	* Work around per Magesh/Justin
	* override input from PCS to allow uc_dsc_ready_for_cmd 
	* reg get written by UC
	*/ 
	CKRST_LN_RST_N_PWRDN_PIN_KILL_CTLr_CLR(pin_ctl);
	CKRST_LN_RST_N_PWRDN_PIN_KILL_CTLr_PMD_LN_H_RSTB_PKILLf_SET(pin_ctl, val);
	WRITE_CKRST_LN_RST_N_PWRDN_PIN_KILL_CTLr(pa, pin_ctl);
	return (ERR_CODE_NONE);
}

err_code_t eagle_tx_pi_control_get( const phymod_access_t *pa,  int16_t* value)
{
    TX_PI_CTL0r_t   tx_pi_reg;
    TX_PI_CTL1r_t   tx_pi_value_reg;

    READ_TX_PI_CTL0r(pa, &tx_pi_reg);
    READ_TX_PI_CTL1r(pa, &tx_pi_value_reg);
    if (TX_PI_CTL0r_TX_PI_ENf_GET(tx_pi_reg)) {
        *value = TX_PI_CTL1r_TX_PI_FREQ_OVERRIDE_VALf_GET(tx_pi_value_reg);
    } else {
        *value = 0;    
    }     

	return (ERR_CODE_NONE);
}



err_code_t eagle_tsc_identify(const phymod_access_t *pa, eagle_rev_id0_t *rev_id0, eagle_rev_id1_t *rev_id1)
{
  err_code_t __err;

  rev_id0->revid_rev_letter =0;
  rev_id0->revid_rev_number =0;
  rev_id0->revid_bonding    =0;
  rev_id0->revid_process    =0;
  rev_id0->revid_model      =0;

  rev_id1->revid_multiplicity =0;
  rev_id1->revid_mdio         =0;
  rev_id1->revid_micro        =0;
  rev_id1->revid_cl72         =0;
  rev_id1->revid_pir          =0;
  rev_id1->revid_llp          =0;
  rev_id1->revid_eee          =0;

  __err=ERR_CODE_NONE;

  rev_id0->revid_rev_letter =rdc_revid_rev_letter(); if(__err) return(__err);
  rev_id0->revid_rev_number =rdc_revid_rev_number(); if(__err) return(__err);
  rev_id0->revid_bonding    =rdc_revid_bonding();    if(__err) return(__err);
  rev_id0->revid_process    =rdc_revid_process();    if(__err) return(__err);
  rev_id0->revid_model      =rdc_revid_model();      if(__err) return(__err);

  rev_id1->revid_multiplicity =rdc_revid_multiplicity(); if(__err) return(__err);
  rev_id1->revid_mdio         =rdc_revid_mdio();         if(__err) return(__err);
  rev_id1->revid_micro        =rdc_revid_micro();        if(__err) return(__err);
  rev_id1->revid_cl72         =rdc_revid_cl72();         if(__err) return(__err);
  rev_id1->revid_pir          =rdc_revid_pir();          if(__err) return(__err);
  rev_id1->revid_llp          =rdc_revid_llp();          if(__err) return(__err);
  rev_id1->revid_eee          =rdc_revid_eee();          if(__err) return(__err);

  return ERR_CODE_NONE;
}


err_code_t eagle_tx_rx_polarity_set(const phymod_access_t *pa, uint32_t tx_pol, uint32_t rx_pol)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  __err = (uint32_t) wr_tx_pmd_dp_invert(tx_pol);
  if(__err) return(__err);
  __err = (uint32_t) wr_rx_pmd_dp_invert(rx_pol);
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t eagle_tx_rx_polarity_get(const phymod_access_t *pa, uint32_t *tx_pol, uint32_t *rx_pol)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *tx_pol = (uint32_t) rd_tx_pmd_dp_invert();
  if(__err) return(__err);
  *rx_pol = (uint32_t) rd_rx_pmd_dp_invert();
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t eagle_force_tx_set_rst (const phymod_access_t *pa, uint32_t rst)
{
  LEFUN(wr_afe_tx_reset_frc_val(rst));
  LEFUN(wr_afe_tx_reset_frc(0x1));
  return ERR_CODE_NONE;
}

err_code_t eagle_force_tx_get_rst (const phymod_access_t *pa, uint32_t *rst)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *rst=(uint32_t) rd_afe_tx_reset_frc_val();
  return ERR_CODE_NONE;
}

err_code_t eagle_force_rx_set_rst (const phymod_access_t *pa, uint32_t rst)
{
  LEFUN(wr_afe_rx_reset_frc_val(rst));
  LEFUN(wr_afe_rx_reset_frc(0x1));
  return ERR_CODE_NONE;
}

err_code_t eagle_force_rx_get_rst (const phymod_access_t *pa, uint32_t *rst)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *rst=(uint32_t) rd_afe_rx_reset_frc_val();
  return ERR_CODE_NONE;
}


err_code_t eagle_pll_mode_get(const phymod_access_t *pa, uint32_t *pll_mode)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *pll_mode=rdc_pll_mode();
  *pll_mode=rdc_ams_pll_fracn_sel();
  if(__err) return(__err);

  return ERR_CODE_NONE;
}


err_code_t eagle_osr_mode_set(const phymod_access_t *pa, int osr_mode)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  __err=wr_osr_mode_frc_val(osr_mode);
  if(__err) return(__err);
  __err=wr_osr_mode_frc(1);
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t eagle_osr_mode_get(const phymod_access_t *pa, int *osr_mode)
{
  int osr_forced;
  err_code_t __err;
  __err=ERR_CODE_NONE;
  osr_forced = rd_osr_mode_frc();
  if(osr_forced) {
    *osr_mode = rd_osr_mode_frc_val();
    if(__err) return(__err);
  } else {
    *osr_mode = rd_osr_mode_pin();
    if(__err) return(__err);
  }

  return ERR_CODE_NONE;
}

err_code_t eagle_tsc_dig_lpbk_get(const phymod_access_t *pa, uint32_t *lpbk)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *lpbk=rd_dig_lpbk_en();
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t eagle_tsc_rmt_lpbk_get(const phymod_access_t *pa, uint32_t *lpbk)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *lpbk=rd_rmt_lpbk_en();
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t eagle_core_soft_reset(const phymod_access_t *pa)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  __err=wrc_core_dp_s_rstb(1);
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

/* set powerdown for tx or rx */
/* tx_rx == 1 => disable (enable) power for Tx */
/* tx_rx != 0 => disable (enable) power for Rx */
/* pwrdn == 0 => enable power */
/* pwrdn == 1 => disable power */
err_code_t eagle_tsc_pwrdn_set(const phymod_access_t *pa, int tx_rx, int pwrdn)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  if(tx_rx) {
    __err = (uint32_t) wr_ln_tx_s_pwrdn(pwrdn);
  } else {
    __err = (uint32_t) wr_ln_rx_s_pwrdn(pwrdn);
  }
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t eagle_tsc_pwrdn_get(const phymod_access_t *pa, power_status_t *pwrdn)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  pwrdn->pll_pwrdn  = 0;
  pwrdn->tx_s_pwrdn = 0;
  pwrdn->rx_s_pwrdn = 0;
  pwrdn->pll_pwrdn  = rdc_afe_s_pll_pwrdn(); if(__err) return(__err);
  pwrdn->tx_s_pwrdn = rd_ln_tx_s_pwrdn();    if(__err) return(__err);
  pwrdn->rx_s_pwrdn = rd_ln_rx_s_pwrdn();    if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t eagle_pcs_lane_swap_tx(const phymod_access_t *pa, uint32_t tx_lane_map) {
  err_code_t __err;
  uint32_t lane_addr_0, lane_addr_1, lane_addr_2, lane_addr_3;

  __err=ERR_CODE_NONE;
  lane_addr_0 = (tx_lane_map >> 16) & 0xf;
  lane_addr_1 = (tx_lane_map >> 20) & 0xf;
  lane_addr_2 = (tx_lane_map >> 24) & 0xf;
  lane_addr_3 = (tx_lane_map >> 28) & 0xf;

  __err=wrc_lane_addr_0(lane_addr_0); if(__err) return(__err);
  __err=wrc_lane_addr_1(lane_addr_1); if(__err) return(__err);
  __err=wrc_lane_addr_2(lane_addr_2); if(__err) return(__err);
  __err=wrc_lane_addr_3(lane_addr_3); if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t eagle_pmd_lane_swap (const phymod_access_t *pa, uint32_t lane_map) {
  err_code_t __err;
  uint32_t lane_0, lane_1, lane_2, lane_3;

  __err=ERR_CODE_NONE;
  lane_0 = ((lane_map >> 0)  & 0x3);
  lane_1 = ((lane_map >> 4)  & 0x3);
  lane_2 = ((lane_map >> 8)  & 0x3);
  lane_3 = ((lane_map >> 12) & 0x3);

  __err=wrc_tx_lane_map_0(lane_0); if(__err) return(__err);
  __err=wrc_tx_lane_map_1(lane_1); if(__err) return(__err);
  __err=wrc_tx_lane_map_2(lane_2); if(__err) return(__err);
  __err=wrc_tx_lane_map_3(lane_3); if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t eagle_pmd_lane_swap_tx_get (const phymod_access_t *pa, uint32_t *lane_map) {
  err_code_t __err;
  uint32_t lane_0, lane_1, lane_2, lane_3;

  __err=ERR_CODE_NONE;

  lane_0=rdc_tx_lane_map_0(); if(__err) return(__err);
  lane_1=rdc_tx_lane_map_1(); if(__err) return(__err);
  lane_2=rdc_tx_lane_map_2(); if(__err) return(__err);
  lane_3=rdc_tx_lane_map_3(); if(__err) return(__err);

  *lane_map = ((lane_3 & 0x3) <<12)|((lane_2 & 0x3) <<8)|((lane_1 & 0x3) <<4)|((lane_0 & 0x3) <<0);

  return ERR_CODE_NONE;
}

err_code_t eagle_lane_hard_soft_reset_release(const phymod_access_t *pa, uint32_t enable)   /* release the pmd core soft reset */
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  __err=wr_ln_s_rstb(enable); if(__err) return(__err);

    return ERR_CODE_NONE;
}

err_code_t eagle_clause72_control(const phymod_access_t *pa, uint32_t cl_72_en)                /* CLAUSE_72_CONTROL */
{
  err_code_t __err;

  __err=ERR_CODE_NONE;
  if (cl_72_en) {
    __err=wr_cl72_ieee_training_enable(1);  if(__err) return(__err);
    __err=wr_cl72_ieee_restart_training(1); if(__err) return(__err);
    __err=wr_ln_dp_s_rstb(0);                  if(__err) return(__err);
    __err=wr_ln_dp_s_rstb(1);                  if(__err) return(__err);
  } else {
    __err=wr_cl72_ieee_training_enable(0);        if(__err) return(__err);
    __err=wr_cl72_ieee_restart_training(0); if(__err) return(__err);
    __err=wr_ln_dp_s_rstb(0);                  if(__err) return(__err);
    __err=wr_ln_dp_s_rstb(1);                  if(__err) return(__err);
  }
  return ERR_CODE_NONE;
}

err_code_t eagle_clause72_control_get(const phymod_access_t *pa, uint32_t *cl_72_en)                /* CLAUSE_72_CONTROL */
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *cl_72_en = rd_cl72_ieee_training_enable(); if(__err) return(__err);
  return ERR_CODE_NONE;
}

/**
@brief   Init the PMD
@param   pmd_touched If the PMD is already initialized
@returns The value ERR_CODE_NONE upon successful completion
@details Per core PMD resets (both datapath and entire core)
We only intend to use this function if the PMD has never been initialized.
*/
err_code_t eagle_pmd_reset_seq(const phymod_access_t *pa, int pmd_touched)
{
  if (pmd_touched == 0) {
    LEFUN(wrc_core_s_rstb(1));
    LEFUN(wrc_core_dp_s_rstb(1));
  }
    return (ERR_CODE_NONE);
}

/**
@brief   Enable the pll reset bit
@param   enable Controls whether to reset PLL
@returns The value ERR_CODE_NONE upon successful completion
@details
Resets the PLL
*/
err_code_t eagle_pll_reset_enable_set (const phymod_access_t *pa, int enable)
{
  LEFUN(wrc_afe_s_pll_reset_frc_val(enable));
  LEFUN(wrc_afe_s_pll_reset_frc(1));
    return (ERR_CODE_NONE);
}

/**
@brief   Read PLL range
*/
err_code_t eagle_tsc_read_pll_range (const phymod_access_t *pa, uint32_t *pll_range)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *pll_range = rdc_ams_pll_range();
  return (ERR_CODE_NONE);
}

/**
@brief   Reag signal detect
*/
err_code_t eagle_tsc_signal_detect (const phymod_access_t *pa, uint32_t *signal_detect)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *signal_detect = rd_signal_detect();
  return (ERR_CODE_NONE);
}


err_code_t eagle_tsc_ladder_setting_to_mV(const phymod_access_t *pa, int8_t y, int16_t* level) {

  /* *level = _ladder_setting_to_mV(y,0); */
   *level = (y*300)/127; 

  return(ERR_CODE_NONE);
}

err_code_t eagle_pll_config_get(const phymod_access_t *pa,enum  eagle_tsc_pll_enum *pll_mode)
{
  err_code_t __err;
  uint32_t fracn_sel;
  uint8_t fracn_ndiv;
  uint32_t pll_div;
  uint32_t pll_vco_div2;
  __err=ERR_CODE_NONE;

  pll_div=rdc_pll_mode();
  fracn_sel=rdc_ams_pll_fracn_sel();
  fracn_ndiv=rdc_ams_pll_fracn_ndiv_int();
  pll_vco_div2=rdc_ams_pll_vco_div2();

  if(fracn_sel == 1) {
     switch(fracn_ndiv) {
       case 0x52: *pll_mode = EAGLE_TSC_pll_div_82p5x;
                  break;
       case 0x57: *pll_mode = EAGLE_TSC_pll_div_87p5x;
                  break;
       case 0x49: if(pll_vco_div2 == 0x1)
                   *pll_mode = EAGLE_TSC_pll_div_36p8x;
                  else
                   *pll_mode = EAGLE_TSC_pll_div_73p6x;
                  break;
       case 0xc7: *pll_mode = EAGLE_TSC_pll_div_199p04x;
                  break;
       default: return ((ERR_CODE_INVALID_PLL_CFG));
                break;

    }
  } else {
     switch(pll_div){
       case 0x1: *pll_mode = EAGLE_TSC_pll_div_72x;
                 break;
       case 0xc: *pll_mode = EAGLE_TSC_pll_div_70x;
                 break;
       case 0xa: *pll_mode = EAGLE_TSC_pll_div_66x;
                 break;
       case 0x9: *pll_mode = EAGLE_TSC_pll_div_64x;
                 break;
       case 0x8: *pll_mode = EAGLE_TSC_pll_div_60x;
                 break;
       case 0x6: *pll_mode = EAGLE_TSC_pll_div_52x;
                 break;
       case 0x3: *pll_mode = EAGLE_TSC_pll_div_42x;
                 break;
       case 0x2: *pll_mode = EAGLE_TSC_pll_div_40x;
                 break;
       case 0xf: *pll_mode = EAGLE_TSC_pll_div_100x;
                 break;
       case 0xe: *pll_mode = EAGLE_TSC_pll_div_92x;
                 break;
       case 0xb: *pll_mode = EAGLE_TSC_pll_div_68x;
                 break;
       case 0x5: *pll_mode = EAGLE_TSC_pll_div_50x;
                 break;
       case 0x0: *pll_mode = EAGLE_TSC_pll_div_46x;
                 break;
       case 0xd: *pll_mode = EAGLE_TSC_pll_div_80x;
                 break;
       default: return ((ERR_CODE_INVALID_PLL_CFG));
                break;
     }
  }
  
  if(__err) return(__err);

  return ERR_CODE_NONE;
}


err_code_t eagle_tx_lane_control_set(const phymod_access_t *pa, uint32_t en)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;

  __err = wr_sdk_tx_disable(en); if(__err) return(__err);

  return ERR_CODE_NONE;
}


err_code_t eagle_tx_lane_control_get(const phymod_access_t *pa, uint32_t* en)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;

  *en = rd_sdk_tx_disable(); if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t eagle_rx_lane_control_set(const phymod_access_t *pa, uint32_t en)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;

  if(en) {
  __err = wr_signal_detect_frc(1); if(__err) return(__err);
  __err = wr_signal_detect_frc_val(0); if(__err) return(__err);
  } else {
  __err = wr_signal_detect_frc(0); if(__err) return(__err);
  __err = wr_signal_detect_frc_val(0); if(__err) return(__err);
  }

  return ERR_CODE_NONE;
}


err_code_t eagle_rx_lane_control_get(const phymod_access_t *pa, uint32_t* en)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;

  *en = rd_signal_detect_frc(); if(__err) return(__err);

  return ERR_CODE_NONE;
}


err_code_t eagle_tsc_get_vco (const phymod_phy_inf_config_t* config, uint32_t *vco_rate, enum eagle_tsc_pll_enum *new_pll_div, int16_t *new_os_mode) {
  *vco_rate = 0;
  *new_pll_div=0;
  *new_os_mode = 0;
  if(config->ref_clock == phymodRefClk156Mhz) {
    switch (config->data_rate) {

      case 1250 :   /* speed 1.25G ; pll_div= 40    */
        *new_pll_div = EAGLE_TSC_pll_div_40x; *new_os_mode = 5 /*= OS5 */; *vco_rate = 1250; /* vco_rate = */
        break;

      case 3125 :   /* speed 3.125G ; pll_div= 60    */
        *new_pll_div = EAGLE_TSC_pll_div_40x; *new_os_mode = 1 /*= OS2 */; *vco_rate = 6250; /* vco_rate = */
        break;

      case 5750 :   /* speed 5.75G ; fracn pll= 73.6    */
        *new_pll_div = EAGLE_TSC_pll_div_73p6x; *new_os_mode = 1 /*= OS2 */; *vco_rate = 11500; /* vco_rate = 24 */
        break;

      case 6250:   /* speed 6.25G: pll_div=40  */
        *new_pll_div = EAGLE_TSC_pll_div_40x;  *new_os_mode = 0 /*= OS2 */; *vco_rate = 6250; /* vco_rate=  */
        break;

      case 10312:   /* speed 10.3G; pll_div = 66  */
        *new_pll_div = EAGLE_TSC_pll_div_66x; *new_os_mode = 0 /*= OS1 */; *vco_rate = 10312; /* vco_rate = 19 */
        break;

      case 10937:   /* speed 10.937G ; pll_div= 70    */
        *new_pll_div = EAGLE_TSC_pll_div_70x; *new_os_mode = 0 /*= OS1 */; *vco_rate = 109375; /* vco_rate = */
        break;

      case 11250:   /* speed 11.25G ; pll_div= 72    */
        *new_pll_div = EAGLE_TSC_pll_div_72x; *new_os_mode = 0 /*= OS1 */; *vco_rate = 11250; /* vco_rate = */
        break;

      case 11500:   /* speed 11.5G ; fracn pll= 73.6    */
        *new_pll_div = EAGLE_TSC_pll_div_73p6x; *new_os_mode = 0 /*= OS1 */; *vco_rate = 11500; /* vco_rate = 24 */
        break;

      case 12500:   /* speed 12.5G ; pll_div = 80    */
        *new_pll_div = EAGLE_TSC_pll_div_80x; *new_os_mode = 0 /*= OS1 */; *vco_rate = 12500; /* vco_rate =  */
        break;

      default:
        PHYMOD_DEBUG_ERROR(("Unsupported speed :: %d :: at ref clk :: %d", (int)config->data_rate, config->ref_clock));
        return ERR_CODE_DIAG;
        break;
    }
  } else if(config->ref_clock == phymodRefClk125Mhz) {
    switch (config->data_rate) {

      case 1250 :   /* speed 1.25G ; pll_div= 50    */
        *new_pll_div = EAGLE_TSC_pll_div_50x; *new_os_mode = 5 /*= OS5 */; *vco_rate = 6250; /* vco_rate = */
        break;

      case 3125 :   /* speed 3.125G ; pll_div= 50    */
        *new_pll_div = EAGLE_TSC_pll_div_50x; *new_os_mode = 1 /*= OS2 */; *vco_rate = 6250; /* vco_rate = */
        break;

      case 5750 :   /* speed 5.75G ; pll_div= 46    */
        *new_pll_div = EAGLE_TSC_pll_div_46x; *new_os_mode = 0 /*= OS1 */; *vco_rate =5750; /* vco_rate =  */
        break;

      case 6250:   /* speed 6.25G: pll_div=50  */
        *new_pll_div = EAGLE_TSC_pll_div_50x;  *new_os_mode = 0 /*= OS1 */; *vco_rate = 6250; /* vco_rate= 3 */
        break;

      case 10312:   /* speed 10.3G; fracn pll = 82.5  */
        *new_pll_div = EAGLE_TSC_pll_div_82p5x; *new_os_mode = 0 /*= OS1 */; *vco_rate = 10312; /* vco_rate = 19 */
        break;

      case 11500:   /* speed 11.5G ; pll_div= 92    */
        *new_pll_div = EAGLE_TSC_pll_div_92x; *new_os_mode = 0 /*= OS1 */; *vco_rate = 11500; /* vco_rate = 24 */
        break;

      case 12500:   /* speed 12.5G ; pll_div= 100    */
        *new_pll_div = EAGLE_TSC_pll_div_100x; *new_os_mode = 0 /*= OS1 */; *vco_rate = 12500; /* vco_rate = */
        break;

      default:
        PHYMOD_DEBUG_ERROR(("Unsupported speed :: %d :: at ref clk :: %d", (int)config->data_rate, config->ref_clock));
        return ERR_CODE_DIAG;
        break;
    }
  } else {
    PHYMOD_DEBUG_ERROR(("Unsupported ref clk :: %d\n", config->ref_clock));
    return ERR_CODE_DIAG;
  }
  return(ERR_CODE_NONE);
}

err_code_t eagle_pmd_tx_disable_pin_dis_set(const phymod_access_t *pa, uint32_t enable) {
  err_code_t __err;
  __err=ERR_CODE_NONE;
  __err=wr_pmd_tx_disable_pkill(enable); if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t eagle_pmd_tx_disable_pin_dis_get(const phymod_access_t *pa, uint32_t *enable) {
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *enable = rd_pmd_tx_disable_pkill(); if(__err) return(__err);

  return ERR_CODE_NONE;
}

/* Get Enable/Disable Shared TX pattern generator */
err_code_t eagle_tsc_tx_shared_patt_gen_en_get( const phymod_access_t *pa, uint8_t *enable) {
  err_code_t __err;

  *enable = rd_patt_gen_en(); if(__err) return(__err);
  return ERR_CODE_NONE;
}

err_code_t eagle_tsc_config_shared_tx_pattern_idx_set( const phymod_access_t *pa, const uint32_t *pattern_len ) {
  err_code_t __err;
  uint8_t mode_sel;

  __err=ERR_CODE_NONE;


  if(*pattern_len==240) {
     mode_sel = 11;
  } else if (*pattern_len== 220 ) {
     mode_sel = 10;
  } else if (*pattern_len == 200) {
     mode_sel = 9;
  } else if (*pattern_len == 180) {
     mode_sel = 8;
  } else if (*pattern_len == 160) {
     mode_sel = 7;
  } else if (*pattern_len == 140) {
     mode_sel = 6;
  } else {
    mode_sel = 0;
    PHYMOD_DEBUG_ERROR(("Invalid length input!\n"));
    return ERR_CODE_DIAG;
  }

  if(mode_sel !=0){
    __err = wr_patt_gen_start_pos(mode_sel); 
    }
    if(__err) return(__err);
    
/*
  for (pattern_idx=0;pattern_idx<PATTERN_MAX_SIZE;pattern_idx++) {

      lsb = (uint16_t) (pattern[pattern_idx] & 0xffff);
      msb = (uint16_t) ((pattern[pattern_idx]>>16) & 0xffff);

      switch (pattern_idx) { 
        case 0:
          __err=wrc_patt_gen_seq_14(msb); if(__err) return(__err);
          __err=wrc_patt_gen_seq_13(lsb); if(__err) return(__err);
          break;
        case 1:
          __err=wrc_patt_gen_seq_12(msb); if(__err) return(__err);
          __err=wrc_patt_gen_seq_11(lsb); if(__err) return(__err);
          break;
        case 2:
          __err=wrc_patt_gen_seq_10(msb); if(__err) return(__err);
          __err=wrc_patt_gen_seq_9(lsb); if(__err) return(__err);
          break;
        case 3:
          __err=wrc_patt_gen_seq_8(msb); if(__err) return(__err);
          __err=wrc_patt_gen_seq_7(lsb); if(__err) return(__err);
          break;
        case 4:
          __err=wrc_patt_gen_seq_6(msb); if(__err) return(__err);
          __err=wrc_patt_gen_seq_5(lsb); if(__err) return(__err);
          break;
        case 5:
          __err=wrc_patt_gen_seq_4(msb); if(__err) return(__err);
          __err=wrc_patt_gen_seq_3(lsb); if(__err) return(__err);
          break;
        case 6:
          __err=wrc_patt_gen_seq_2(msb); if(__err) return(__err);
          __err=wrc_patt_gen_seq_1(lsb); if(__err) return(__err);
          break;
        case 7:
          __err=wrc_patt_gen_seq_0(msb); if(__err) return(__err);
          break;
   */     /* Its a dead default and cause coverity defect
        default:
              PHYMOD_DEBUG_ERROR(("Wrong index value : Should be between 0 and 7\n"));
          return ERR_CODE_DIAG;
          break;*/
    /*  }
  }*/
  return ERR_CODE_NONE;
}

err_code_t eagle_tsc_config_shared_tx_pattern_idx_get( const phymod_access_t *pa, uint32_t *pattern_len, uint32_t *pattern) {
  err_code_t __err;
  uint8_t mode_sel;
  uint32_t lsb = 0, msb = 0;
  int pattern_idx;

  __err=ERR_CODE_NONE; 

  mode_sel = rd_patt_gen_start_pos(); if(__err) return(__err);

  mode_sel = 12 - mode_sel;

  if(mode_sel == 6) {
    *pattern_len = 140;
  } else if (mode_sel == 5) {
    *pattern_len = 160;
  } else if (mode_sel == 4) {
    *pattern_len = 180;
  } else if (mode_sel == 3) {
    *pattern_len = 200;
  } else if (mode_sel == 2) {
    *pattern_len = 220;
  } else if (mode_sel == 1) {
    *pattern_len = 240;
  } else {
    *pattern_len = 0;
  }

  for (pattern_idx=0;pattern_idx<PATTERN_MAX_SIZE;pattern_idx++) {
      switch (pattern_idx) { 
        case 0:
          msb=rdc_patt_gen_seq_14(); if(__err) return(__err);
          lsb=rdc_patt_gen_seq_13(); if(__err) return(__err);
          break;
        case 1:
          msb=rdc_patt_gen_seq_12(); if(__err) return(__err);
          lsb=rdc_patt_gen_seq_11(); if(__err) return(__err);
          break;
        case 2:
          msb=rdc_patt_gen_seq_10(); if(__err) return(__err);
          lsb=rdc_patt_gen_seq_9(); if(__err) return(__err);
          break;
        case 3:
          msb=rdc_patt_gen_seq_8(); if(__err) return(__err);
          lsb=rdc_patt_gen_seq_7(); if(__err) return(__err);
          break;
        case 4:
          msb=rdc_patt_gen_seq_6(); if(__err) return(__err);
          lsb=rdc_patt_gen_seq_5(); if(__err) return(__err);
          break;
        case 5:
          msb=rdc_patt_gen_seq_4(); if(__err) return(__err);
          lsb=rdc_patt_gen_seq_3(); if(__err) return(__err);
          break;
        case 6:
          msb=rdc_patt_gen_seq_2(); if(__err) return(__err);
          lsb=rdc_patt_gen_seq_1(); if(__err) return(__err);
          break;
        case 7:
          msb=rdc_patt_gen_seq_0(); if(__err) return(__err);
          lsb=0;
          break;
        /* Its a dead default and cause coverity defect
        default:
              PHYMOD_DEBUG_ERROR(("Wrong index value : Should be between 0 and 7\n"));
          return ERR_CODE_DIAG;
          break;*/
      }

      pattern[pattern_idx] = (msb <<16) | lsb;
  }

  return ERR_CODE_NONE;
}

err_code_t eagle_tsc_rx_ppm(const phymod_access_t *pa, int16_t *rx_ppm)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *rx_ppm = rd_cdr_integ_reg() / 84;
  return (ERR_CODE_NONE);
}

