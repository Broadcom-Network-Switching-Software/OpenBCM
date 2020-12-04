/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       tsc_functions.c
 */

#include "tsc_functions.h"
#include "bcm_utils.h" /* for print only */

extern int _bcm_tsc_reg_read(int unit, phy_ctrl_t *pc, int mode, int tsc_port, int pmd, uint32 reg, uint16 *value);
extern int _bcm_tsc_reg_write(int unit, phy_ctrl_t *pc, int mode, int tsc_port, int pmd, uint32 reg, uint16 value, uint16 mask);


uint8 bcm_pe_tsc_get_lane(phy_ctrl_t *pa) {
    if (pa->lane_mask == 0x1) {
        return ( 0 );
    } else if (pa->lane_mask == 0x2) {
        return ( 1 );
    } else if (pa->lane_mask == 0x4) {
        return ( 2 );
    } else if (pa->lane_mask == 0x8) {
        return ( 3 );
    } else {
        return ( 0 );
    }
}

int bcm_pe_pmd_rdt_reg(phy_ctrl_t *pa, uint16 address, uint16 *val)
{
    int tsc_port;
    int mode;

    /* bits 23-19 of tsc register address ln:1,2,3,4 or sys:9,a,b,c */
    tsc_port = (pa->tsc_sys_port << 3) + pa->phy_id + 1;

    mode = bcm_pe_tsc_get_lane(pa);

    SOC_IF_ERROR_RETURN
        (_bcm_tsc_reg_read(pa->unit, pa, mode, tsc_port, 1, (uint32)address, val));

    return ( 0 );
}

int bcm_pe_pmd_wr_reg(phy_ctrl_t *pa, uint16 address, uint16 val)
{
    int tsc_port;
    int mode;

    /* bits 23-19 of tsc register address ln:1,2,3,4 or sys:9,a,b,c */
    tsc_port = (pa->tsc_sys_port << 3) + pa->phy_id + 1;

    mode = bcm_pe_tsc_get_lane(pa);

    SOC_IF_ERROR_RETURN
        (_bcm_tsc_reg_write(pa->unit, pa, mode, tsc_port, 1, (uint32)address, val, 0));

    return  SOC_E_NONE; 
}

int bcm_pe_pmd_mwr_reg(phy_ctrl_t *pa, uint16 addr, uint16 mask, uint8 lsb, uint16 val)
{
    phy_ctrl_t pa_copy;
    int tsc_port;
    int mode, i;

    bcm_memcpy(&pa_copy, pa, sizeof(phy_ctrl_t));
    for (i=1; i <= 0x8; i = i << 1) {
        if ( i & pa->lane_mask ) {
            pa_copy.lane_mask = i;

            /* bits 23-19 of tsc register address ln:1,2,3,4 or sys:9,a,b,c */
            tsc_port = (pa_copy.tsc_sys_port << 3) + pa_copy.phy_id + 1;

            mode = bcm_pe_tsc_get_lane(&pa_copy);

            SOC_IF_ERROR_RETURN
                (_bcm_tsc_reg_write(pa_copy.unit, &pa_copy, mode, tsc_port, 1, (uint32)addr, (val << lsb), (mask ? ~mask : 0)));
/*
            BCM_LOG_CLI("%s(): lane_mask 0x%x; phy_devad 0x%x; addr 0x%x; data 0x%x; mask 0x%x\n", __FUNCTION__,
                        pa_copy.lane_mask, pa_copy.phy_devad, addr, (val << lsb), mask);
*/
        }
    }

    return  SOC_E_NONE; 
}

uint16 _bcm_pe_pmd_rde_reg (phy_ctrl_t *pa, uint16 addr, int *err_code_p)
{
    uint16 data;

    EPFUN(bcm_pe_pmd_rdt_reg (pa, addr, &data));
    return data;
}


uint16 _bcm_pe_pmd_rde_field (phy_ctrl_t *pa, uint16 addr, uint8 shift_left, uint8 shift_right, int *err_code_p)
{
    uint16 data;

    EPSTM(data = _bcm_pe_pmd_rde_reg (pa, addr, err_code_p));

    data <<= shift_left;                     /* Move the sign bit to the left most [shift_left      = (15-msb)] */
    data >>= shift_right;                    /* Right shift entire field to bit 0  [shift_right = (15-msb+lsb)] */

    return data;
}

int16 _bcm_pe_pmd_rde_field_signed (phy_ctrl_t *pa, uint16 addr, uint8 shift_left, uint8 shift_right, int *err_code_p)
{
    int16 data;

    EPSTM(data = (int16)_bcm_pe_pmd_rde_reg (pa, addr, err_code_p));      /* convert it to signed */

    data <<= shift_left;                                                    /* Move the sign bit to the left most    [shift_left  = (15-msb)] */
    data >>= shift_right;                                                   /* Move to the right with sign extension [shift_right = (15-msb+lsb)] */

    return data;
}

uint8 _bcm_pe_pmd_rde_field_byte (phy_ctrl_t *pa, uint16 addr, uint8 shift_left, uint8 shift_right, int *err_code_p)
{
    return (uint8)_bcm_pe_pmd_rde_field (pa, addr, shift_left, shift_right, err_code_p);
}


int8 _bcm_pe_pmd_rde_field_signed_byte (phy_ctrl_t *pa, uint16 addr, uint8 shift_left, uint8 shift_right, int *err_code_p)
{
    return (int8)_bcm_pe_pmd_rde_field_signed (pa, addr, shift_left, shift_right, err_code_p);
}

int _bcm_pe_pmd_mwr_reg_byte (phy_ctrl_t *pa, uint16 addr, uint16 mask, uint8 lsb, uint8 val)
{
    EFUN(bcm_pe_pmd_mwr_reg (pa, addr, mask, lsb, (uint16)val));

    return SOC_E_NONE;
}

/* Micro RAM Word Read */
uint16 bcm_pe_rdw_uc_ram (phy_ctrl_t *pa, int *err_code_p, uint16 addr)
{
    uint16 rddata = 0;

    if (!err_code_p)
        return 0;
    *err_code_p = SOC_E_NONE;
    EPFUN(wrc_micro_autoinc_rdaddr_en(0));
    EPFUN(wrc_micro_ra_rddatasize(0x1));            /* Select 16bit read datasize */
    EPFUN(wrc_micro_ra_rdaddr_msw(0x2000));         /* Upper 16bits of RAM address to be read */
    EPFUN(wrc_micro_ra_rdaddr_lsw(addr));           /* Lower 16bits of RAM address to be read */
    EPSTM(rddata = rdc_micro_ra_rddata_lsw());      /* 16bit read data */
    return rddata;
}

/* Micro RAM Byte Read */
uint8 bcm_pe_rdb_uc_ram (phy_ctrl_t *pa, int *err_code_p, uint16 addr)
{
    uint8 rddata = 0;

    if (!err_code_p)
        return 0;
    *err_code_p = SOC_E_NONE;
    EPFUN(wrc_micro_autoinc_rdaddr_en(0));
    EPFUN(wrc_micro_ra_rddatasize(0x0));                    /* Select 8bit read datasize */
    EPFUN(wrc_micro_ra_rdaddr_msw(0x2000));                 /* Upper 16bits of RAM address to be read */
    EPFUN(wrc_micro_ra_rdaddr_lsw(addr));                   /* Lower 16bits of RAM address to be read */
    EPSTM(rddata = (uint8)rdc_micro_ra_rddata_lsw());     /* 16bit read data */
    return rddata;
}

uint8 bcm_pe_rdbc_uc_var (phy_ctrl_t *pa, int *err_code_p, uint8 addr)
{
    uint8 rddata;

    if (!err_code_p)
        return 0;

    EPSTM(rddata = bcm_pe_rdb_uc_ram (pa, err_code_p, (CORE_VAR_RAM_BASE + addr)));              /* Use Micro register interface for reading RAM */

    return rddata;
}

/* Micro RAM Lane Byte Read */
uint8 bcm_pe_rdbl_uc_var (phy_ctrl_t *pa, int *err_code_p, uint16 addr)
{
    uint8 rddata;

    if (!err_code_p)
        return 0;

    EPSTM(rddata = bcm_pe_rdw_uc_ram (pa, err_code_p, (LANE_VAR_RAM_BASE + addr + (bcm_pe_tsc_get_lane (pa ) * LANE_VAR_RAM_SIZE))));

    return rddata;
}

/* Micro RAM Lane Byte Signed Read */
int8 bcm_pe_rdbls_uc_var (phy_ctrl_t *pa, int *err_code_p, uint16 addr)
{
    return (int8)bcm_pe_rdbl_uc_var (pa, err_code_p, addr);
}

/* Micro RAM Core Word Read */
uint16 bcm_pe_rdwc_uc_var (phy_ctrl_t *pa, int *err_code_p, uint8 addr)
{

    uint16 rddata;

    if (!err_code_p)
        return 0;
    if (addr % 2 != 0) {
        *err_code_p = SOC_E_PARAM;
        return 0;
    }

    EPSTM(rddata = bcm_pe_rdw_uc_ram (pa, err_code_p, (CORE_VAR_RAM_BASE + addr)));          /* Use Micro register interface for reading RAM */

    return rddata;
}

int bcm_pe_poll_uc_dsc_ready_for_cmd_equals_1 (phy_ctrl_t *pa, uint32 timeout_ms)
{
    uint16 rddata;
    uint8 rddata8;
    uint16 loop;

    /* read quickly for 4 tries */
    for (loop = 0; loop < 100; loop++) {
        EFUN(bcm_pe_pmd_rdt_reg (pa, DSC_A_DSC_UC_CTRL, &rddata));
        if (rddata & 0x0080) {          /* bit 7 is uc_dsc_ready_for_cmd */
            if (rddata & 0x0040) {  /* bit 6 is uc_dsc_error_found   */
                BCM_LOG_ERR("ERROR : DSC command returned error (after cmd) !\n");
                return SOC_E_FAIL;
            }
            return SOC_E_NONE;
        }
        if (loop > 10)
            bcm_udelay(10 * timeout_ms);
    }

    BCM_LOG_ERR("ERROR : DSC ready for command is not working, applying workaround and getting debug info !\n");
    ESTM(rddata8 = rd_uc_dsc_ready_for_cmd());
    BCM_LOG_ERR("rd_uc_dsc_ready_for_cmd = %x\n", rddata8);
#if 0
    /* Print some Debug Info */
    DISP(rd_uc_dsc_supp_info());
    DISP(rd_uc_dsc_gp_uc_req());
    DISP(rd_uc_dsc_data());
    DISP(rd_dsc_state());
#endif
    ESTM(rddata = rdcv_status_byte());
    BCM_LOG_ERR("Uc Core Status Byte = %x\n", rddata);

    /* artifically terminate the command to re-enable the command interface */
    EFUN(wr_uc_dsc_ready_for_cmd(0x1));

    return SOC_E_TIMEOUT;
}

int bcm_pe_pmd_uc_cmd_return_immediate (phy_ctrl_t *pa, uint8 cmd, uint8 supp_info)
{
    int err_code;
    uint16 cmddata;

    err_code = bcm_pe_poll_uc_dsc_ready_for_cmd_equals_1 (pa, 1); /* Poll for uc_dsc_ready_for_cmd = 1 to indicate phy8806x_tsc ready for command */
    if (err_code) {
        return err_code;
    }
    cmddata = (((uint16)supp_info) << 8) | (uint16)cmd;         /* Combine writes to single write instead of 4 RMW */

    EFUN(bcm_pe_pmd_wr_reg (pa, DSC_A_DSC_UC_CTRL, cmddata));       /* This address is same for Eagle, and all Merlin */

    return SOC_E_NONE;
}

int bcm_pe_pmd_uc_cmd (phy_ctrl_t *pa, uint8 cmd, uint8 supp_info, uint32 timeout_ms)
{
    uint8 uc_dsc_error_found;

    EFUN(bcm_pe_pmd_uc_cmd_return_immediate (pa, cmd, supp_info));          /* Invoke phy8806x_tsc_uc_cmd and return control immediately */

    EFUN(bcm_pe_poll_uc_dsc_ready_for_cmd_equals_1 (pa, timeout_ms));       /* Poll for uc_dsc_ready_for_cmd = 1 to indicate phy8806x_tsc ready for command */

    ESTM(uc_dsc_error_found = rd_uc_dsc_error_found());
    if (uc_dsc_error_found) {
        return SOC_E_FAIL;
    }

    return SOC_E_NONE;
}


/* Micro RAM Lane Word Read */
uint16 bcm_pe_rdwl_uc_var (phy_ctrl_t *pa, int *err_code_p, uint16 addr)
{
    uint16 rddata;

    if (!err_code_p)
        return 0;

    if (addr % 2 != 0) {                            /* Validate even address */
        *err_code_p = SOC_E_PARAM;
        return 0;
    }

    EPSTM(rddata = bcm_pe_rdw_uc_ram (pa, err_code_p, (LANE_VAR_RAM_BASE + addr + (bcm_pe_tsc_get_lane(pa) * LANE_VAR_RAM_SIZE))));

    return rddata;
}

/* Micro RAM Lane Word Signed Read */
int16 bcm_pe_rdwls_uc_var (phy_ctrl_t *pa, int *err_code_p, uint16 addr)
{
    return (int16)bcm_pe_rdwl_uc_var (pa, err_code_p, addr);
}

int bcm_pe_pmd_uc_diag_cmd (phy_ctrl_t *pa, uint8 control, uint32 timeout_ms)
{
    return bcm_pe_pmd_uc_cmd (pa, CMD_DIAG_EN, (uint8)control, timeout_ms);
}

int bcm_pe_pmd_uc_control (phy_ctrl_t *pa, uint8 control, uint32 timeout_ms)
{
    return bcm_pe_pmd_uc_cmd (pa, CMD_UC_CTRL, (uint8)control, timeout_ms);
}

int bcm_pe_set_pll(uint8 pll_index)
{
    return 0;
}

uint8 bcm_pe_get_pll(void)
{
    return 0;
}

uint8 bcm_pe_get_core(void)
{
    return(0);
}

int _bcm_pe_pll_lock_status (phy_ctrl_t *pa, uint8 *pll_lock, uint8 *pll_lock_chg)
{
    uint16 rddata;

    EFUN(bcm_pe_pmd_rdt_reg (pa, PLL_STATUS_ADDR, &rddata));

    *pll_lock     = ((rddata >> 8) & 0x1);
    *pll_lock_chg = (rddata & 0x1);

    return SOC_E_NONE;
}

int _bcm_pe_pmd_lock_status (phy_ctrl_t *pa, uint8 *pmd_lock, uint8 *pmd_lock_chg)
{

    uint16 rddata;

    EFUN(bcm_pe_pmd_rdt_reg (pa, PMD_LOCK_STATUS_ADDR, &rddata));

    *pmd_lock     = (rddata & 0x1);
    *pmd_lock_chg = ((rddata >> 1) & 0x1);

    return SOC_E_NONE;
}

/* PRBS Checker Lock State */
int bcm_pe_prbs_chk_lock_state (phy_ctrl_t *pa, uint8 *chk_lock)
{
    if (!chk_lock)
        return SOC_E_PARAM;

    ESTM(*chk_lock = rd_prbs_chk_lock());              /* PRBS Checker Lock Indication 1 = Locked, 0 = Out of Lock */
    return SOC_E_NONE;
}

/* PRBS Error Count and Lock Lost (bit 31 in lock lost) */
int bcm_pe_prbs_err_count_ll (phy_ctrl_t *pa, uint32 *prbs_err_cnt)
{
    uint16 rddata;

    if (!prbs_err_cnt)
        return SOC_E_PARAM;
    EFUN(bcm_pe_pmd_rdt_reg (pa, TLB_RX_PRBS_CHK_ERR_CNT_MSB_STATUS, &rddata));
    *prbs_err_cnt = ((uint32)rddata) << 16;
    ESTM(*prbs_err_cnt = (*prbs_err_cnt | rd_prbs_chk_err_cnt_lsb()));
    return SOC_E_NONE;
}

/*******************************/
/*  Stop/Resume RX Adaptation  */
/*******************************/
int bcm_pe_stop_rx_adaptation (phy_ctrl_t *pa, uint8 enable)
{

    if (enable)
        return bcm_pe_pmd_uc_control (pa, CMD_UC_CTRL_STOP_GRACEFULLY, 100);
    else
        return bcm_pe_pmd_uc_control (pa, CMD_UC_CTRL_RESUME, 50);
}

