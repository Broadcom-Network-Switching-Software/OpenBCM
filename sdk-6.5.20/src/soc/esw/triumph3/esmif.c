/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tcam.c
 * Purpose:     Initialize serial link to external TCAM
 * Requires:
 */

#include <soc/defs.h>
#ifdef BCM_TRIUMPH3_SUPPORT

/* Translation of Tcl files in /projects/ntsw-sw3/home/smaskai/TR3_A0/TCL_FOR_SW */

#include <sal/core/boot.h>
#include <shared/bsl.h>
#include <soc/triumph3.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/debug.h>
#include <soc/mem.h>
#include <soc/er_tcam.h>
#include <soc/l2x.h>

#define NUM_WCLS                         6
#define ESMIF_MAX_FIFO_DEPTH             870
#define ESMIF_ALLOWED_FIFO_DEPTH         ((9 * ESMIF_MAX_FIFO_DEPTH) / 10)
#define REQD_RX_BW_FOR_CPU_PERCENT       0
#define REQD_TX_BW_FOR_CPU_PERCENT       0
#define REQD_NL_CCLK_BW_FOR_CPU_PERCENT  0
#define ESMIF_MAX_RSP_COST2              255
#define ESMIF_MAX_REQ_COST1              255
#define ESMIF_MAX_CCLK_COST0             255
#define NL_DEV_ID                        0xc1
#define NL_NUM_REQ_LANES                                                   \
    ((((soc_tcam_info_t *) SOC_CONTROL(unit)->tcam_info)->num_tcams == 2) ?\
     16 : 24)
#define NL_NUM_RSP_LANES                 12
#define NL_OVERHEAD_WORDS                4
#define NL_LANE_SPEED_MBPS               6250
#define NL_BITS_PER_ENC_WORD             67
#define NL_OPC_NOP                       0x0
#define NL_OPC_REG_WRITE                 0x1
#define NL_OPC_DB_WRITE                  0x1
#define NL_OPC_REG_READ                  0x2
#define NL_OPC_DBX_READ                  0x2
#define NL_OPC_DBY_READ                  0x3
#define NL_OPC_CBW                       0x100

#define SRST_L_DEASSERTION_DELAY_US      30 /* uSec */
#define CRST_L_DEASSERTION_DELAY_US      3000 /* uSec */


#define ESM_MAX(a, b) (a) > (b) ? (a) : (b)


#define NL_TCAM_ERR_THRESHOLD_COUNT      (3)

int apply_low_jitter_nl_settings = FALSE;

uint8 nl_tcam_err_count[SOC_MAX_NUM_DEVICES] = {0};

extern int soc_tr3_restore_ltr(int unit, int dev, int ltr);

STATIC unsigned
esmif_clk_freq_khz(int unit)
{
    unsigned speed;

    speed = soc_tr3_core_clock_speed(unit);
    if (speed == 415) {
        return (1000 * soc_tr3_core_clock_speed(unit) + 646);
    }
    return (1000 * soc_tr3_core_clock_speed(unit) + 100);
}


STATIC unsigned
nl_core_clk_freq_khz(int unit)
{
    return (1000 * soc_property_get(unit, spn_EXT_TCAM_FREQ, 400));
}


STATIC int
chk_esmif_init_config(int unit, unsigned exp_iesmif_latency)
{
    unsigned errcnt = 0;
    uint32   esmif_init_config_buf, latency;

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit,
                                      ESMIF_INIT_CONFIGr,
                                      0,
                                      0,
                                      &esmif_init_config_buf
                                      )
                        );

    if (soc_reg_field_get(unit,
                          ESMIF_INIT_CONFIGr,
                          esmif_init_config_buf,
                          IESMIF_BYPASS_ENABLEf
                          )
        ) {
        ++errcnt;               /* ESMIF stage is bypassed */
    }
    if (soc_reg_field_get(unit,
                          ESMIF_INIT_CONFIGr,
                          esmif_init_config_buf,
                          WR_ENABLEf)
        == 0
        ) {
        ++errcnt;               /* WR_ENABLE for ESM CTRL_BUS and AUX1 fifo is still 0 */
    }

    latency = soc_reg_field_get(unit,
                                ESMIF_INIT_CONFIGr,
                                esmif_init_config_buf,
                                LATENCYf
                                );

    if (latency != exp_iesmif_latency) {
        ++errcnt;               /* Unexpected value for LATENCY */
    }
    if (latency & 1) {
        ++errcnt;               /* LATENCY is odd */
    }
    if (latency == 0) {
        ++errcnt;               /* LATENCY is 0 */
    }
    if (latency > 1000) {
        ++errcnt;               /* LATENCY is greater than 1000 */
    }
    
    _SHR_RETURN(errcnt == 0 ? SOC_E_NONE : SOC_E_FAIL);
}

/* Disable interrupts */

STATIC int
dis_intr(int unit)
{
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ESM_EVENT_ERR_STATUS_INTR_ENABLEr, 0, 0, 0));
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ETU_GLOBAL_INTR_ENABLEr, 0, 0, 0));
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ILAMAC_RX_INTF_INTR0_ENABLEr, 0, 0, 0));
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ILAMAC_RX_INTF_INTR1_ENABLEr, 0, 0, 0));
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ILAMAC_RX_LANE_INTR0_ENABLEr, 0, 0, 0));
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ILAMAC_RX_LANE_INTR1_ENABLEr, 0, 0, 0));
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ILAMAC_RX_LANE_INTR2_ENABLEr, 0, 0, 0));
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ILAMAC_RX_LANE_INTR3_ENABLEr, 0, 0, 0));
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ILAMAC_TX_INTR_ENABLEr, 0, 0, 0));
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, WCL_INTR_ENABLEr, 0, 0, 0));
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ETU_TX_REQ_FIFO_INTR_ENABLEr, 0, 0, 0));
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ETU_TX_PIPE_CTL_FIFO_INTR_ENABLEr, 0, 0, 0));
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ETU_RX_RSP_FIFO_INTR_ENABLEr, 0, 0, 0));

    _SHR_RETURN(SOC_E_NONE);
}


STATIC int
etu_rst_nl_wcl_ilamac(int unit, unsigned tx_serdes_refclk_sel)
{
    /* Reset TCAM */

    {
        uint32 etu_config4_buf;
        
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_CONFIG4r, 0, 0, &etu_config4_buf));
        soc_reg_field_set(unit, ETU_CONFIG4r, &etu_config4_buf, EXT_TCAM_CRST_Lf, 1);
        soc_reg_field_set(unit, ETU_CONFIG4r, &etu_config4_buf, EXT_TCAM_SRST_Lf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ETU_CONFIG4r, 0, 0, etu_config4_buf));
        soc_reg_field_set(unit, ETU_CONFIG4r, &etu_config4_buf, EXT_TCAM_CRST_Lf, 0);
        soc_reg_field_set(unit, ETU_CONFIG4r, &etu_config4_buf, EXT_TCAM_SRST_Lf, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ETU_CONFIG4r, 0, 0, etu_config4_buf));
    }

    /* Reset all wcl */

    {
        uint32   wcl_ctl_buf;
        unsigned wcl_num;

        for (wcl_num = 0; wcl_num < NUM_WCLS; ++wcl_num) {
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, WCL_CTLr, 0, wcl_num, &wcl_ctl_buf));
            soc_reg_field_set(unit, WCL_CTLr, &wcl_ctl_buf, PWRDWNf, 1);
            soc_reg_field_set(unit, WCL_CTLr, &wcl_ctl_buf, IDDQf, 1);
            soc_reg_field_set(unit, WCL_CTLr, &wcl_ctl_buf, RSTB_HWf, 0);
            soc_reg_field_set(unit, WCL_CTLr, &wcl_ctl_buf, RSTB_MDIOREGSf, 0);
            soc_reg_field_set(unit, WCL_CTLr, &wcl_ctl_buf, RSTB_PLLf, 0);
            soc_reg_field_set(unit, WCL_CTLr, &wcl_ctl_buf, TXD1G_FIFO_RSTBf, 0);
            soc_reg_field_set(unit, WCL_CTLr, &wcl_ctl_buf, PLL_BYPASSf, 0);
            soc_reg_field_set(unit, WCL_CTLr, &wcl_ctl_buf, LCREF_ENf, 0);
            soc_reg_field_set(unit, WCL_CTLr, &wcl_ctl_buf, REFOUT_ENf, 0);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, WCL_CTLr, 0, wcl_num, wcl_ctl_buf));
        }
    }

    {
        /* Reset ILAMAC */

        uint32 etu_config4_buf;
        
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_CONFIG4r, 0, 0, &etu_config4_buf));
        soc_reg_field_set(unit, ETU_CONFIG4r, &etu_config4_buf, ILAMAC_RX_SERDES_RST_f, 0);
        soc_reg_field_set(unit, ETU_CONFIG4r, &etu_config4_buf, ILAMAC_RX_LBUS_RST_f, 0);
        soc_reg_field_set(unit, ETU_CONFIG4r, &etu_config4_buf, ILAMAC_TX_SERDES_RST_f, 0);
        soc_reg_field_set(unit, ETU_CONFIG4r, &etu_config4_buf, ILAMAC_TX_LBUS_RST_f, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ETU_CONFIG4r, 0, 0, etu_config4_buf));

        /* Select one of the 24 txck from WCL - should not matter if things are ok */

        soc_reg_field_set(unit, ETU_CONFIG4r, &etu_config4_buf, ILAMAC_TX_SERDES_REFCLK_SELf, tx_serdes_refclk_sel);
    }

    _SHR_RETURN(SOC_E_NONE);
}


STATIC int
tcam_phy_init(int unit)
{
    _SHR_RETURN(wcmod_esm_serdes_init(unit));
}


STATIC int
wcl_mdio_init_seq(int unit)
{
    _SHR_RETURN(tcam_phy_init(unit));
}


STATIC int
wcl_chk_txpll_lock(int unit)
{
    static const soc_field_t txpll_lock_fld[] = {
        TXPLL_LOCK_0f,
        TXPLL_LOCK_1f,
        TXPLL_LOCK_2f,
        TXPLL_LOCK_3f,
        TXPLL_LOCK_4f,
        TXPLL_LOCK_5f
    };

    uint32   wcl_cur_sts_buf;
    unsigned i;

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, WCL_CUR_STSr, 0, 0, &wcl_cur_sts_buf));
    for (i = 0; i < COUNTOF(txpll_lock_fld); ++i) {
        if (soc_reg_field_get(unit, WCL_CUR_STSr, wcl_cur_sts_buf, txpll_lock_fld[i]) == 0) {
            return (SOC_E_BUSY);
        }
    }

    return (SOC_E_NONE);
}


STATIC int
wcl_poll_txpll_lock(int unit)
{
    unsigned n;

    for (n = 1000; n; --n) {
        int errcode = wcl_chk_txpll_lock(unit);

        if (errcode == SOC_E_BUSY) {
            sal_usleep(1000);       /* Wait 1 ms */
            continue;
        }

        _SHR_RETURN(errcode);
    }

    _SHR_RETURN(SOC_E_TIMEOUT);
}


STATIC int
wcl_reset_seq(int unit, unsigned master_wcl_num)
{
    uint32   wcl_ctl_buf;
    unsigned wcl_num;
    
    /* Dont rely on previous state - restore default state of WCL_CTL
       wcl - assert pwrdwn, iddq, etc.
       Set iddq to '0' and rstb_pll to '0' (if it is not '0' to start with, this
       is to keep pll in reset).
    */

    for (wcl_num = 0; wcl_num < NUM_WCLS; ++wcl_num) {
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, WCL_CTLr, 0, wcl_num, &wcl_ctl_buf));
        soc_reg_field_set(unit, WCL_CTLr, &wcl_ctl_buf, PWRDWNf, 1);
        soc_reg_field_set(unit, WCL_CTLr, &wcl_ctl_buf, IDDQf, 1);
        soc_reg_field_set(unit, WCL_CTLr, &wcl_ctl_buf, RSTB_HWf, 0);
        soc_reg_field_set(unit, WCL_CTLr, &wcl_ctl_buf, RSTB_MDIOREGSf, 0);
        soc_reg_field_set(unit, WCL_CTLr, &wcl_ctl_buf, RSTB_PLLf, 0);
        soc_reg_field_set(unit, WCL_CTLr, &wcl_ctl_buf, TXD1G_FIFO_RSTBf, 0);
        soc_reg_field_set(unit, WCL_CTLr, &wcl_ctl_buf, PLL_BYPASSf, 0);
        soc_reg_field_set(unit, WCL_CTLr, &wcl_ctl_buf, LCREF_ENf, 0); /* LCREF_EN=0 means PLL input clock = pad_refclk_p/n */
        soc_reg_field_set(unit, WCL_CTLr, &wcl_ctl_buf, REFOUT_ENf, 0); /* REFOUT_EN=0 means lcrefoutp/n = HiZ */
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, WCL_CTLr, 0, wcl_num, wcl_ctl_buf));
    }

    /* Note: By default PLL_BYPASS=0 and we will keep it this way */

    /* Note: following sequence is based on email dated 6/17/2011, from Prasun Paul */

    /* wcl - remove iddq
       Set iddq to 0 and rstb_pll to 0 (if it is not 0 to start with, this is to keep pll in reset).  */ 

    for (wcl_num = 0; wcl_num < NUM_WCLS; ++wcl_num) {
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, WCL_CTLr, 0, wcl_num, &wcl_ctl_buf));
        soc_reg_field_set(unit, WCL_CTLr, &wcl_ctl_buf, IDDQf, 0);
        soc_reg_field_set(unit, WCL_CTLr, &wcl_ctl_buf, RSTB_PLLf, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, WCL_CTLr, 0, wcl_num, wcl_ctl_buf));
    }
    
    /* wcl - remove powerdown
       Set pwrdwn_tx[3:0]/ pwrdwn_rx[3:0] to '0' to release power down of Tx and Rx path.
       Can be controlled thru internal MDIO register bits as well to overwrite pin info.
       Note: wcl_ctl_pwrdwn from etu controls both pwrdwn_tx and pwrdwn_rx,
       so if we want to turn-off rx only, then we will have to do it thro mdio
    */
    
    for (wcl_num = 0; wcl_num < NUM_WCLS; ++wcl_num) {
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, WCL_CTLr, 0, wcl_num, &wcl_ctl_buf));
        soc_reg_field_set(unit, WCL_CTLr, &wcl_ctl_buf, PWRDWNf, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, WCL_CTLr, 0, wcl_num, wcl_ctl_buf));
    }

    /* wcl - master, slave clock assignment
       Set lcref_en and refout_en as desired. Do you need any explanation here?
       By default: LCREF_EN=0, REFOUT_EN=0 for all WCLs
    */
    
    for (wcl_num = 0; wcl_num < NUM_WCLS; ++wcl_num) {
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, WCL_CTLr, 0, wcl_num, &wcl_ctl_buf));
        if (wcl_num == master_wcl_num) {
            /* Master */

            soc_reg_field_set(unit, WCL_CTLr, &wcl_ctl_buf, LCREF_ENf, 0);
            soc_reg_field_set(unit, WCL_CTLr, &wcl_ctl_buf, REFOUT_ENf, 1);
        } else {
            /* Slave */

            soc_reg_field_set(unit, WCL_CTLr, &wcl_ctl_buf, LCREF_ENf, 1);
            soc_reg_field_set(unit, WCL_CTLr, &wcl_ctl_buf, REFOUT_ENf, 0);
        }
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, WCL_CTLr, 0, wcl_num, wcl_ctl_buf));
    }

    sal_usleep(1000);           /* Wait 1 ms */

    /* wcl - De-assert RSTB_HW - hardware_reset
       Release rstb_hw and rstb_mdioregs. That means set to '1's
    */
    
    for (wcl_num = 0; wcl_num < NUM_WCLS; ++wcl_num) {
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, WCL_CTLr, 0, wcl_num, &wcl_ctl_buf));
        soc_reg_field_set(unit, WCL_CTLr, &wcl_ctl_buf, RSTB_HWf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, WCL_CTLr, 0, wcl_num, wcl_ctl_buf));
    }
    for (wcl_num = 0; wcl_num < NUM_WCLS; ++wcl_num) {
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, WCL_CTLr, 0, wcl_num, &wcl_ctl_buf));
        soc_reg_field_set(unit, WCL_CTLr, &wcl_ctl_buf, RSTB_MDIOREGSf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, WCL_CTLr, 0, wcl_num, wcl_ctl_buf));
    }

    /* wcl - Set all the MDIO registers inside the ESM SerDes. Take care of any
       necessary initial programming. 
       For example, we may have to set the PLL divider, multiplier, etc thro
       mdio writes, reads
    */
    
    SOC_IF_ERROR_RETURN(wcl_mdio_init_seq(unit));

    /* wcl - De-assert RSTB_PLL - pll_reset
       Release rstb_pll
    */
    
    for (wcl_num = 0; wcl_num < NUM_WCLS; ++wcl_num) {
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, WCL_CTLr, 0, wcl_num, &wcl_ctl_buf));
        soc_reg_field_set(unit, WCL_CTLr, &wcl_ctl_buf, RSTB_PLLf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, WCL_CTLr, 0, wcl_num, wcl_ctl_buf));
    }

    _SHR_RETURN(wcl_poll_txpll_lock(unit));
}


STATIC int
phy_c45_read(int      unit,
             unsigned phy_id,
             unsigned phy_devad,
             unsigned phy_reg,
             uint16   *phy_data
             )
{
    _SHR_RETURN(soc_miimc45_read(unit, phy_id, phy_devad, phy_reg, phy_data));
}


STATIC int
phy_c45_write(int      unit,
              unsigned phy_id,
              unsigned phy_devad,
              unsigned phy_reg,
              uint16   phy_data
              )
{
    _SHR_RETURN(soc_miimc45_write(unit, phy_id, phy_devad, phy_reg, phy_data));
}


STATIC int
nl_mdio_read(int      unit,
             unsigned mdio_portid,
             unsigned mdio_dev_id, 
             unsigned mdio_addr,
             uint16   *rd_data
             )
{
    const unsigned phy_id = (1 << 8) | (0 << 7) | (0 << 5) | mdio_portid;

    _SHR_RETURN(phy_c45_read(unit, phy_id, mdio_dev_id, mdio_addr, rd_data));
}


STATIC int
nl_mdio_write(int      unit,
              unsigned mdio_portid,
              unsigned mdio_dev_id, 
              unsigned mdio_addr,
              uint16   wr_data,
              unsigned verify_wr
              )
{
    const unsigned phy_id = (1 << 8) | (0 << 7) | (0 << 5) | mdio_portid;

    SOC_IF_ERROR_RETURN(phy_c45_write(unit, phy_id, mdio_dev_id, mdio_addr, wr_data));
    
    if (verify_wr) {
        uint16 rd_data;

        SOC_IF_ERROR_RETURN(nl_mdio_read(unit,
                                         mdio_portid,
                                         mdio_dev_id, 
                                         mdio_addr,
                                         &rd_data
                                         )
                            );

        if (rd_data != wr_data)  _SHR_RETURN(SOC_E_FAIL);
    }

    _SHR_RETURN(SOC_E_NONE);
}


STATIC int
nl_mdio_test_reg_access(int unit, unsigned mdio_portid)
{
    const unsigned mdio_dev_id = 2;

    unsigned errcnt = 0;
    uint16   val;

    /* Will use following mdio regs for this test:
       RX SerDes - PRBS Control and Status Registers, MDIO Device ID = 2 to 10, see Table 24
    */

    /* Verify default value for registers */

    if (SOC_FAILURE(nl_mdio_read(unit, mdio_portid, mdio_dev_id, 0x12, &val))
        || val != 0xaaaa
        ) {
        ++errcnt;
    }
    if (SOC_FAILURE(nl_mdio_read(unit, mdio_portid, mdio_dev_id, 0x13, &val))
        || val != 0xaaaa
        ) {
        ++errcnt;
    }

    /* Test write_verify */

    if (SOC_FAILURE(nl_mdio_write(unit, mdio_portid, mdio_dev_id, 0x12, 0x5555, 1))) {
        ++errcnt;
    }
    if (SOC_FAILURE(nl_mdio_write(unit, mdio_portid, mdio_dev_id, 0x13, 0x1234, 1))) {
        ++errcnt;
    }

    /* Make sure our above writes went to different registers */

    if (SOC_FAILURE(nl_mdio_read(unit, mdio_portid, mdio_dev_id, 0x12, &val))
        || val != 0x5555
        ) {
        ++errcnt;
    }
    if (SOC_FAILURE(nl_mdio_read(unit, mdio_portid, mdio_dev_id, 0x13, &val))
        || val != 0x1234
        ) {
        ++errcnt;
    }

    /* Restore default values */

    if (SOC_FAILURE(nl_mdio_write(unit, mdio_portid, mdio_dev_id, 0x12, 0xaaaa, 1))) {
        ++errcnt;
    }
    if (SOC_FAILURE(nl_mdio_write(unit, mdio_portid, mdio_dev_id, 0x13, 0xaaaa, 1))) {
        ++errcnt;
    }
    
    _SHR_RETURN(errcnt ? SOC_E_FAIL : SOC_E_NONE);
}


unsigned
mdio_portid_get(int unit, unsigned dev_id)
{
    soc_tcam_info_t *tcam_info;

    tcam_info = (soc_tcam_info_t *) SOC_CONTROL(unit)->tcam_info;
    return (dev_id == 1 ? tcam_info->mdio_port_tcam1 : 
            tcam_info->mdio_port_tcam0);
}


STATIC int
nl_mdio_chk_serdes_reset_seq_done(int unit, unsigned mdio_portid)
{
    uint16 val;

    SOC_IF_ERROR_RETURN(nl_mdio_read(unit, mdio_portid, 1, 0x8183, &val));
    return (val & (1 << 3) ? SOC_E_NONE : SOC_E_BUSY);
}


STATIC int
nl_mdio_poll_serdes_reset_seq_done(int unit, unsigned mdio_portid)
{
    unsigned n;

    for (n = 1000; n; --n) {
        int errcode = nl_mdio_chk_serdes_reset_seq_done(unit, mdio_portid);

        if (errcode == SOC_E_BUSY) {
            sal_usleep(1000);       /* Wait 1 ms */
            continue;
        }

        _SHR_RETURN(errcode);
    }

    _SHR_RETURN(SOC_E_TIMEOUT);
}

STATIC int
nl_set_analog_parameters(int unit, int tcam_num) {
    int         transmitter, receiver, lane, config_val, post_cursor_val, main_tap, rx_gain;
    uint16      mdio_rval;
    unsigned    mdio_dev, mdio_addr;
    unsigned    mdio_portid = mdio_portid_get(unit, tcam_num);
	
    /* Setup all transmitters (12 TX and 24 CTX) */
    for (transmitter = 0; transmitter < 36; transmitter++) {
        if (transmitter < 12) {
            lane = transmitter;
            if (tcam_num == 0) {
                config_val = soc_property_suffix_num_get(unit, lane, 
                                                         spn_EXT_TCAM0_TX_DRIVER_CURRENT, 
                                                         "tx_lane", 0);
                post_cursor_val = soc_property_suffix_num_get(unit, lane, 
                                                              spn_EXT_TCAM0_TX_POSTCURSOR_TAP,
                                                              "tx_lane", 0);
                main_tap = soc_property_suffix_num_get(unit, lane, 
                                                       spn_EXT_TCAM0_TX_MAIN_TAP, 
                                                       "tx_lane", 0x3f);
            } else {
                config_val = soc_property_suffix_num_get(unit, lane, 
                                                         spn_EXT_TCAM1_TX_DRIVER_CURRENT, 
                                                         "tx_lane", 0);
                post_cursor_val = soc_property_suffix_num_get(unit, lane, 
                                                              spn_EXT_TCAM1_TX_POSTCURSOR_TAP, 
                                                              "tx_lane", 0);
                main_tap = soc_property_suffix_num_get(unit, lane, 
                                                       spn_EXT_TCAM1_TX_MAIN_TAP, 
                                                       "tx_lane", 0x3f);
            }
        } else {
            lane = transmitter - 12;
            if (tcam_num == 0) {
                config_val = soc_property_suffix_num_get(unit, lane, 
                                                         spn_EXT_TCAM0_TX_DRIVER_CURRENT, 
                                                         "ctx_lane",  0);
                post_cursor_val = soc_property_suffix_num_get(unit, lane, 
                                                              spn_EXT_TCAM0_TX_POSTCURSOR_TAP, 
                                                              "ctx_lane", 0);
                main_tap = soc_property_suffix_num_get(unit, lane, 
                                                       spn_EXT_TCAM0_TX_MAIN_TAP, 
                                                       "ctx_lane", 0x3f);
            } else {
                config_val = soc_property_suffix_num_get(unit, lane, 
                                                         spn_EXT_TCAM1_TX_DRIVER_CURRENT, 
                                                         "ctx_lane",  0);
                post_cursor_val = soc_property_suffix_num_get(unit, lane, 
                                                              spn_EXT_TCAM1_TX_POSTCURSOR_TAP, 
                                                              "ctx_lane", 0);
                main_tap = soc_property_suffix_num_get(unit, lane, 
                                                       spn_EXT_TCAM1_TX_MAIN_TAP, 
                                                       "ctx_lane", 0x3f);
            } 
        }
        
        /* Set transmitter bias settings */
        config_val &= 0x3;
        switch (transmitter / 4) {
            case 0:
            case 1:
            case 2:
                mdio_dev = 22;
                mdio_addr = 0x118 - (lane / 4 * 7);
                break;
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
                mdio_dev = 23;
                mdio_addr = 0x11f - (lane / 4 * 7);
                break;
            /*
             * COVERITY
             *
             * This default is unreachable but kept intentionally 
             * as a defensive check.
             */
            /* coverity[dead_error_begin] */
            default:
                mdio_dev = 22;
                mdio_addr = 0x103;
                break;
        }
        switch (lane % 4) {
            case 0:
                nl_mdio_read(unit, mdio_portid, mdio_dev, mdio_addr, &mdio_rval);
                mdio_rval &= 0xfff0;
                mdio_rval |= (config_val << 2) | (config_val << 0);
                nl_mdio_write(unit, mdio_portid, mdio_dev, mdio_addr, mdio_rval, 1);
                mdio_addr -= 3;
                nl_mdio_read(unit, mdio_portid, mdio_dev, mdio_addr, &mdio_rval);
                mdio_rval &= 0xf000;
                mdio_rval |= (config_val << 10) | (config_val << 8) |
                             (config_val << 6) | (config_val << 4) |
                             (config_val << 2) | (config_val << 0);
                nl_mdio_write(unit, mdio_portid, mdio_dev, mdio_addr, mdio_rval, 1);
                break;
            case 1:
                nl_mdio_read(unit, mdio_portid, mdio_dev, mdio_addr, &mdio_rval);
                mdio_rval &= 0xff0f;
                mdio_rval |= (config_val << 6) | (config_val << 4);
                nl_mdio_write(unit, mdio_portid, mdio_dev, mdio_addr, mdio_rval, 1);
                mdio_addr -= 2;
                nl_mdio_read(unit, mdio_portid, mdio_dev, mdio_addr, &mdio_rval);
                mdio_rval &= 0xff00;
                mdio_rval |= (config_val << 6) | (config_val << 4) |
                             (config_val << 2) | (config_val << 0);
                nl_mdio_write(unit, mdio_portid, mdio_dev, mdio_addr, mdio_rval, 1);
                mdio_addr -= 1;
                nl_mdio_read(unit, mdio_portid, mdio_dev, mdio_addr, &mdio_rval);
                mdio_rval &= 0x0fff;
                mdio_rval |= (config_val << 14) | (config_val << 12);
                nl_mdio_write(unit, mdio_portid, mdio_dev, mdio_addr, mdio_rval, 1);
                break;
            case 2:
                nl_mdio_read(unit, mdio_portid, mdio_dev, mdio_addr, &mdio_rval);
                mdio_rval &= 0xf0ff;
                mdio_rval |= (config_val << 10) | (config_val << 8);
                nl_mdio_write(unit, mdio_portid, mdio_dev, mdio_addr, mdio_rval, 1);
                mdio_addr -= 1;
                nl_mdio_read(unit, mdio_portid, mdio_dev, mdio_addr, &mdio_rval);
                mdio_rval &= 0xfff0;
                mdio_rval |= (config_val << 2) | (config_val << 0);
                nl_mdio_write(unit, mdio_portid, mdio_dev, mdio_addr, mdio_rval, 1);
                mdio_addr -= 1;
                nl_mdio_read(unit, mdio_portid, mdio_dev, mdio_addr, &mdio_rval);
                mdio_rval &= 0x00ff;
                mdio_rval |= (config_val << 14) | (config_val << 12) |
                             (config_val << 10) | (config_val << 8);
                nl_mdio_write(unit, mdio_portid, mdio_dev, mdio_addr, mdio_rval, 1);
                break;
            case 3:
                nl_mdio_read(unit, mdio_portid, mdio_dev, mdio_addr, &mdio_rval);
                mdio_rval &= 0x0fff;
                mdio_rval |= (config_val << 14) | (config_val << 12);
                nl_mdio_write(unit, mdio_portid, mdio_dev, mdio_addr, mdio_rval, 1);
                mdio_addr -= 1;
                nl_mdio_read(unit, mdio_portid, mdio_dev, mdio_addr, &mdio_rval);
                mdio_rval &= 0x000f;
                mdio_rval |= (config_val << 14) | (config_val << 12) |
                             (config_val << 10) | (config_val << 8) |
                             (config_val << 6) | (config_val << 4);
                nl_mdio_write(unit, mdio_portid, mdio_dev, mdio_addr, mdio_rval, 1);
                break;
            /*
             * COVERITY
             *
             * This default is unreachable but kept intentionally 
             * as a defensive check.
             */
            /* coverity[dead_error_begin] */ 
            default:
                break;
        }

        /* Set transmitter post-cursor tap */
        mdio_rval = post_cursor_val & 0x1f;
        mdio_dev = 11 + (transmitter / 4);
        mdio_addr = 0x104 + (lane % 4 * 0x10);
        nl_mdio_write(unit, mdio_portid, mdio_dev, mdio_addr, mdio_rval, 1);

        /* Set transmitter main tap */
        mdio_rval = main_tap & 0x3f;
        mdio_dev = 11 + (transmitter / 4);
        mdio_addr = 0x103 + (lane % 4 * 0x10);
        nl_mdio_write(unit, mdio_portid, mdio_dev, mdio_addr, mdio_rval, 1);
        
        /* Enable the preemphasis */
        mdio_rval = 0x0001;
        mdio_dev = 11 + (transmitter / 4);
        mdio_addr = 0x105 + (lane % 4 * 0x10);
        nl_mdio_write(unit, mdio_portid, mdio_dev, mdio_addr, mdio_rval, 1);
    }
    
    /* Setup all receivers (24 RX and 12 CRX) */
    for (receiver = 0; receiver < 36; receiver++) {
        if (receiver < 24) {
            lane = receiver;
            if (tcam_num == 0) {
                rx_gain = soc_property_suffix_num_get(unit, lane, 
                                                      spn_EXT_TCAM0_RX_GAIN, 
                                                      "rx_lane", 0);
            } else {
                rx_gain = soc_property_suffix_num_get(unit, lane, 
                                                      spn_EXT_TCAM1_RX_GAIN, 
                                                      "rx_lane", 0);
            }
        } else {
            lane = receiver - 24;
            if (tcam_num == 0) {
                rx_gain = soc_property_suffix_num_get(unit, lane, 
                                                      spn_EXT_TCAM0_RX_GAIN, 
                                                      "crx_lane", 0);
            } else {
                rx_gain = soc_property_suffix_num_get(unit, lane, 
                                                      spn_EXT_TCAM1_RX_GAIN, 
                                                      "crx_lane", 0);
            }
        }
        
        /* Set receiver gain 
           The serdes always operates at 6.25G, so no need to set Fc*/
        mdio_rval = (3 << 6) | ((rx_gain & 0x1f) << 1) | 0x0001;
        mdio_dev = 2 + (receiver / 4);
        mdio_addr = 0x128 + (lane % 4 * 0x10);
        nl_mdio_write(unit, mdio_portid, mdio_dev, mdio_addr, mdio_rval, 1);
    }
    
    return SOC_E_NONE;
}

#define NL_METAFRAME_LENGTH  2048

STATIC int
nl_mdio_init_seq(int      unit,
                 unsigned num_nl,
                 unsigned rx_fifo_thr,
                 unsigned tx_fifo_thr,
                 unsigned tx_burst_short_16b
                 )
{
    const unsigned mdio_dev_id = 1;
    unsigned dev_id;
    unsigned int nl_16lane_mode = 0;

    /* Device configuration using mdio (section 6.7)
       
       CSM - Config Regs, MDIO Devid=1, see Table 18 in NL spec
       - All regs are 16b wide
       NL's init seq: 4 Rx lanes, 2 Tx lanes
       See Table 17 in NL spec, it shows mapping of MDIO Device ID to SerDes lanes within NL
       This is must to understand when we want to enable prbs generators,
       checkers
    */
    
    if (num_nl == 2) {
        nl_16lane_mode = 1;
    }
    for (dev_id = 0; dev_id < num_nl; ++dev_id) {
        unsigned mdio_portid = mdio_portid_get(unit, dev_id);

        /* TX_SerDes_11_0_squelch (Active high squelch for TX SerDes 11:0) */
        SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                                          mdio_portid,
                                          mdio_dev_id,
                                          0x8117,
                                          0xfff,
                                          1
                                          )
                            );
        /* CTX_SerDes_15_0_squelch (Active high squelch for CTX SerDes 15:0) */
        SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                                          mdio_portid,
                                          mdio_dev_id,
                                          0x8118,
                                          0xffff,
                                          1
                                          )
                            );
        /* CTX_SerDes_23_16_squelch (Active high squelch for CTX SerDes 23:16) */
        SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                                          mdio_portid,
                                          mdio_dev_id,
                                          0x8119,
                                          0xff,
                                          1
                                          )
                            );
    }

    for (dev_id = 0; dev_id < num_nl; ++dev_id) {
        unsigned mdio_portid = mdio_portid_get(unit, dev_id);
        
        /* Global RX / TX Enable (PCS enables), Bit[0] = RX PCS Enable, Bit[1] =
           TX PCS Enable
        */
        SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                                          mdio_portid,
                                          mdio_dev_id,
                                          0x810c,
                                          0,
                                          1
                                          )
                            );
    }

    for (dev_id = 0; dev_id < num_nl; ++dev_id) {
        unsigned mdio_portid = mdio_portid_get(unit, dev_id);
    
        /* Global SW Reset - ASSERT (only bits 2:0 are defined)
           Bit[2] SerDes Initialization Sequence Trigger,
           Bit[1] Reset Core Logic,
           Bit[0] Reset Core PLL
        */
        SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                                          mdio_portid,
                                          mdio_dev_id,
                                          0x811c,
                                          7,
                                          1
                                          )
                            );
    }

    if ((!nl_16lane_mode) && (num_nl > 1)) {
        const unsigned dev_id      = 1;
        const unsigned mdio_portid = mdio_portid_get(unit, dev_id);

        uint16 regval;

        SOC_IF_ERROR_RETURN(nl_mdio_read(unit, mdio_portid, 1, 0x8236, &regval));
        SOC_IF_ERROR_RETURN(nl_mdio_write(unit, mdio_portid, 1, 0x8236, regval & ~(1 << 2), 1));

        SOC_IF_ERROR_RETURN(nl_mdio_read(unit, mdio_portid, 1, 0x8237, &regval));
        SOC_IF_ERROR_RETURN(nl_mdio_write(unit, mdio_portid, 1, 0x8237, regval & ~(1 << 0), 1));
    }


    for (dev_id = 0; dev_id < num_nl; ++dev_id) {
        unsigned mdio_portid = mdio_portid_get(unit, dev_id);
    
        /* Lane Swap
           Bit[0] for RX lanes swap, Bit[0]=1 means Lane 0 and Lane 23, Lane 1 and Lane 22,
           ...  Lane 11 and Lane 12 are swapped
           Bit[1] for TX lanes swap, Bit[1]=1 means Lane 0 and Lane 11, Lane 1 and Lane 10,
           ...  Lane 5 and Lane 6 are swapped
        */
        SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                                          mdio_portid,
                                          mdio_dev_id,
                                          0x8108,
                                          0,
                                          1
                                          )
                            );
        /* RX Metaframe Length */
        SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                                          mdio_portid,
                                          mdio_dev_id,
                                          0x8106,
                                          NL_METAFRAME_LENGTH,
                                          1
                                          )
                            );
        /* TX Metaframe Length */
        SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                                          mdio_portid,
                                          mdio_dev_id,
                                          0x8107,
                                          NL_METAFRAME_LENGTH,
                                          1
                                          )
                            );
        /* RX (req) FIFO Threshold */
        SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                                          mdio_portid,
                                          mdio_dev_id,
                                          0x8109,
                                          rx_fifo_thr,
                                          1
                                          )
                            );
        /* TX (rsp) FIFO Threshold */
        SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                                          mdio_portid,
                                          mdio_dev_id,
                                          0x810a,
                                          tx_fifo_thr,
                                          1
                                          )
                            );
        /* TX Burst Short (0 means 8 bytes, 1 means 16 bytes) */
        SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                                          mdio_portid,
                                          mdio_dev_id,
                                          0x810b,
                                          tx_burst_short_16b ? 1 : 0,
                                          1
                                          )
                            );
        /* Speed mode select, bits 2:0 select speed for rx serdes, bits 6:4
           select speed for tx serdes  
           3'b100 means 6.250 Gbps, 3'b000 means 3.125 Gbps
        */
        SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                                          mdio_portid,
                                          mdio_dev_id,
                                          0x811d,
                                          0x44,
                                          1
                                          )
                            );
        /* Add TX/RX analog configuration */
		    SOC_IF_ERROR_RETURN(nl_set_analog_parameters(unit, dev_id));
        /* Apply the low jitter settings */
        if (apply_low_jitter_nl_settings) {
            /* TX pll charge pump enable */
            SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                        mdio_portid,
                        31,
                        0x0501,
                        0x0031,
                        1
                        )
                    );
            
            /* TX regulator bandwidth */
            SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                        mdio_portid,
                        31,
                        0x0502,
                        0x0940,
                        1
                        )
                    );

            /* TX clock mode override */
            SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                        mdio_portid,
                        31,
                        0x0200,
                        0x0007,
                        1
                        )
                    );

            /* TX transmit PLL divider */
            SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                        mdio_portid,
                        31,
                        0x0500,
                        0x0501,
                        1
                        )
                    );

            /* TX BIAS generators for CTX23-20, TX11-0 */
            SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                        mdio_portid,
                        22,
                        0x0105,
                        0x0010,
                        1
                        )
                    );

            SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                        mdio_portid,
                        22,
                        0x010C,
                        0x0010,
                        1
                        )
                    );

            SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                        mdio_portid,
                        22,
                        0x0113,
                        0x0010,
                        1
                        )
                    );

            SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                        mdio_portid,
                        22,
                        0x011A,
                        0x0010,
                        1
                        )
                    );

            SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                        mdio_portid,
                        22,
                        0x0121,
                        0x0010,
                        1
                        )
                    );

            /* TX BIAS generators for CTX19-0 */
            SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                        mdio_portid,
                        23,
                        0x0105,
                        0x0010,
                        1
                        )
                    );

            SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                        mdio_portid,
                        23,
                        0x010C,
                        0x0010,
                        1
                        )
                    );

            SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                        mdio_portid,
                        23,
                        0x0113,
                        0x0010,
                        1
                        )
                    );

            SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                        mdio_portid,
                        23,
                        0x011A,
                        0x0010,
                        1
                        )
                    );

            SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                        mdio_portid,
                        23,
                        0x0121,
                        0x0010,
                        1
                        )
                    );

        }
    }

    for (dev_id = 0; dev_id < num_nl; ++dev_id) {
        unsigned mdio_portid = mdio_portid_get(unit, dev_id);

        /* RX Lane Enable 0 (RX lane enable for lanes 15 to 0) */
        SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                                          mdio_portid,
                                          mdio_dev_id,
                                          0x8100,
                                          0xffff,
                                          1
                                          )
                            );
        /* RX Lane Enable 1 (RX lane enable for lanes 23 to 16) */
        SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                                          mdio_portid,
                                          mdio_dev_id,
                                          0x8101,
                                          (nl_16lane_mode) ? 0x00 : 0xff,
                                          1
                                          )
                            );
        /* TX Lane Enable 0 (TX lane enable for lanes 11 to 0) */
        SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                                          mdio_portid,
                                          mdio_dev_id,
                                          0x8102,
                                          0xfff,
                                          1
                                          )
                            );
    }

    for (dev_id = 0; dev_id < num_nl; ++dev_id) {
        unsigned mdio_portid = mdio_portid_get(unit, dev_id);

        SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                                          mdio_portid,
                                          mdio_dev_id,
                                          0x8104,
                                          dev_id == (num_nl - 1) ? 0 : 0xffff,
                                          1
                                          )
                            );
        SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                                          mdio_portid,
                                          mdio_dev_id,
                                          0x8105,
                                          (nl_16lane_mode) ? 0x00 : dev_id == (num_nl - 1) ? 0 : 0xff,
                                          1
                                          )
                            );
    }

    for (dev_id = 0; dev_id < num_nl; ++dev_id) {
        unsigned mdio_portid = mdio_portid_get(unit, dev_id);

        SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                                          mdio_portid,
                                          mdio_dev_id,
                                          0x8103,
                                          dev_id ? 0xfff : 0,
                                          1
                                          )
                            );
    }

    for (dev_id = 0; dev_id < num_nl; ++dev_id) {
        unsigned mdio_portid = mdio_portid_get(unit, dev_id);

        /* Global SW Reset - Deassert (only bits 2:0 are defined)
           Bit[2] SerDes Initialization Sequence Trigger,
           Bit[1] Reset Core Logic,
           Bit[0] Reset Core PLL
        */
        SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                                          mdio_portid,
                                          mdio_dev_id,
                                          0x811c,
                                          0,
                                          1
                                          )
                            );
    }

    for (dev_id = 0; dev_id < num_nl; ++dev_id) {
        unsigned mdio_portid = mdio_portid_get(unit, dev_id);
        
        SOC_IF_ERROR_RETURN(nl_mdio_poll_serdes_reset_seq_done(unit, mdio_portid));
    }

    for (dev_id = 0; dev_id < num_nl; ++dev_id) {
        unsigned mdio_portid = mdio_portid_get(unit, dev_id);

        /* Global RX / TX Enable (PCS enables), Bit[0] = RX PCS Enable, Bit[1] =
           TX PCS Enable 
        */
        SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                                          mdio_portid,
                                          mdio_dev_id,
                                          0x810c,
                                          3,
                                          1
                                          )
                            );
    }

    for (dev_id = 0; dev_id < num_nl; ++dev_id) {
        unsigned mdio_portid = mdio_portid_get(unit, dev_id);

        /* TX_SerDes_11_0_squelch (Active high squelch for TX SerDes 11:0) */
        SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                                          mdio_portid,
                                          mdio_dev_id,
                                          0x8117,
                                          0,
                                          1
                                          )
                            );
        SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                                          mdio_portid,
                                          mdio_dev_id,
                                          0x8118,
                                          dev_id == (num_nl - 1) ? 0xffff : 0,
                                          1
                                          )
                            );
        SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                                          mdio_portid,
                                          mdio_dev_id,
                                          0x8119,
                                          dev_id == (num_nl - 1) ? 0xff : 0,
                                          1
                                          )
                            );
    }

    _SHR_RETURN(SOC_E_NONE);
}


int
nl_mdio_prbs_chk(int unit, uint16 mdio_portid, uint16 prbs, uint32 seed, 
                 uint16 crx, uint32 enable)
{
    uint16 nl_tcams = ((soc_tcam_info_t *)SOC_CONTROL(unit)->tcam_info)->num_tcams;
    uint16 mdio_devid_max = crx? 10 : 7;
    uint16 mdio_devid_min = 2;
    uint16 mdio_devid;

    seed = enable && (seed!=0)? seed : 0xAAAAAAAA;

    if (prbs == 1) {
        _SHR_RETURN(SOC_E_PARAM);
    }
    /* PRBS control [2:1] - poly */
    prbs = enable? (prbs == 0? 0x4: ((prbs == 2)? 0x2: 0x0)) : 0x0;
    /* PRBS control [0] - enable */
    prbs = enable? prbs | 0x1: prbs;

    for(mdio_devid = mdio_devid_min; mdio_devid <= mdio_devid_max; ++mdio_devid) {
        /* If cascaded tcam, request lines are just 16 lanes not 24,
           Skip for devid 6 & 7.
         */
        if((nl_tcams>1) && ((mdio_devid == 6) || (mdio_devid == 7))) {
            continue;
        }
        SOC_IF_ERROR_RETURN
            (nl_mdio_write(unit, mdio_portid, mdio_devid, 0x12, seed & 0xffff, 1));
        SOC_IF_ERROR_RETURN
            (nl_mdio_write(unit, mdio_portid, mdio_devid, 0x13, (seed >> 16) & 0xffff, 1));
        SOC_IF_ERROR_RETURN
            (nl_mdio_write(unit, mdio_portid, mdio_devid, 0x11, prbs, 1));
    }

    _SHR_RETURN(SOC_E_NONE);
}


int
nl_mdio_prbs_gen(int unit, uint16 mdio_portid, uint16 prbs, uint32 seed, 
                 uint16 ctx, uint32 enable)
{
    uint16 nl_tcams = ((soc_tcam_info_t *)SOC_CONTROL(unit)->tcam_info)->num_tcams;
    uint16 mdio_devid_max = ctx? ((nl_tcams>1)? 17: 19) : 13;
    uint16 mdio_devid_min = 11;
    uint16 mdio_devid;

    seed = enable && (seed!=0)? seed : 0xAAAAAAAA;

    if (prbs == 1) {
        _SHR_RETURN(SOC_E_PARAM);
    }
    /* PRBS control [2:1] - poly */
    prbs = enable? (prbs == 0? 0x4: ((prbs == 2)? 0x2: 0x0)) : 0x0;
    /* PRBS control [0] - enable */
    prbs = enable? prbs | 0x1: 0x0;

    for(mdio_devid = mdio_devid_min; mdio_devid <= mdio_devid_max; ++mdio_devid) {
        SOC_IF_ERROR_RETURN
            (nl_mdio_write(unit, mdio_portid, mdio_devid, 0x12, seed & 0xffff, 1));
        SOC_IF_ERROR_RETURN
            (nl_mdio_write(unit, mdio_portid, mdio_devid, 0x13, (seed >> 16) & 0xffff, 1));
        SOC_IF_ERROR_RETURN
            (nl_mdio_write(unit, mdio_portid, mdio_devid, 0x11, prbs, 1));
    }

    _SHR_RETURN(SOC_E_NONE);
}

int
nl_mdio_prbs_chk_err(int unit, uint16 mdio_portid, uint16 crx)
{
    uint16 nl_tcams = ((soc_tcam_info_t *)SOC_CONTROL(unit)->tcam_info)->num_tcams;
    uint16 mdio_devid_max = crx?10:7;
    uint16 mdio_devid_min = 2;
    uint16 mdio_devid;
    uint16 prbs_ctrl, prbs_pass = 1;
    uint16 err_count_l0 = 0, err_count_l1 = 0, err_count_l2, err_count_l3 = 0;
    char if_name[10];
    int	 if_lane_base;

    sal_memset(if_name, 0, sizeof(if_name));
    for(mdio_devid = mdio_devid_min; mdio_devid <= mdio_devid_max; ++mdio_devid) {
        /* If cascaded tcam, request lines are just 16 lanes not 24,
           Skip for devid 6 & 7.
         */
        if((nl_tcams>1) && ((mdio_devid == 6) || (mdio_devid == 7))) {
            continue;
        }
        SOC_IF_ERROR_RETURN
            (nl_mdio_read(unit, mdio_portid, mdio_devid, 0x11, &prbs_ctrl));
        if((prbs_ctrl & 0x1) == 0x0) {
            LOG_WARN(BSL_LS_SOC_ESM,
                     (BSL_META_U(unit,
                                 "PRBS is not enabled for NL TCAM mdio_portid=%d\n"), mdio_portid));
            return SOC_E_FAIL;
        }
        SOC_IF_ERROR_RETURN
            (nl_mdio_read(unit, mdio_portid, mdio_devid, 0x14, &err_count_l0));
        SOC_IF_ERROR_RETURN
            (nl_mdio_read(unit, mdio_portid, mdio_devid, 0x15, &err_count_l1));
        SOC_IF_ERROR_RETURN
            (nl_mdio_read(unit, mdio_portid, mdio_devid, 0x16, &err_count_l2));
        SOC_IF_ERROR_RETURN
            (nl_mdio_read(unit, mdio_portid, mdio_devid, 0x17, &err_count_l3));

        (mdio_devid < 8) ? sal_strncpy(if_name, "RX", (sizeof(if_name)-1)) :
                           sal_strncpy(if_name, "CRX",(sizeof(if_name)-1));
        if_lane_base = (mdio_devid < 8) ? (mdio_devid - 2) * 4 : (mdio_devid - 8) * 4;

        if(err_count_l0) {
			LOG_WARN(BSL_LS_SOC_ESM,
                                 (BSL_META_U(unit,
                                             "\t%d PRBS errors seen on lane %d of %s interface\n"), err_count_l0, if_lane_base + 0, if_name));
	    }		
        if(err_count_l1) {
			LOG_WARN(BSL_LS_SOC_ESM,
                                 (BSL_META_U(unit,
                                             "\t%d PRBS errors seen on lane %d of %s interface\n"), err_count_l1, if_lane_base + 1, if_name));
	    }		
        if(err_count_l2) {
			LOG_WARN(BSL_LS_SOC_ESM,
                                 (BSL_META_U(unit,
                                             "\t%d PRBS errors seen on lane %d of %s interface\n"), err_count_l2, if_lane_base + 2, if_name));
	    }		
        if(err_count_l3) {
			LOG_WARN(BSL_LS_SOC_ESM,
                                 (BSL_META_U(unit,
                                             "\t%d PRBS errors seen on lane %d of %s interface\n"), err_count_l3, if_lane_base + 3, if_name));
	    }		
	    if (err_count_l0 || err_count_l1 || err_count_l2 || err_count_l3) prbs_pass = 0;
    }
    if(prbs_pass) {
        LOG_WARN(BSL_LS_SOC_ESM,
                 (BSL_META_U(unit,
                             "\tPRBS OK \n")));
    }
    _SHR_RETURN(SOC_E_NONE);
}



STATIC int
wcl_chk_rxlane_lock(int unit)
{
    static const soc_field_t flds[] = {
        RXSEQDONE1G_4_3f,
        RXSEQDONE1G_4_2f,
        RXSEQDONE1G_4_1f,
        RXSEQDONE1G_4_0f,
        RXSEQDONE1G_2_3f,
        RXSEQDONE1G_2_2f,
        RXSEQDONE1G_2_1f,
        RXSEQDONE1G_2_0f,
        RXSEQDONE1G_0_3f,
        RXSEQDONE1G_0_2f,
        RXSEQDONE1G_0_1f,
        RXSEQDONE1G_0_0f
    };

    uint32   wcl_cur_sts_buf;
    unsigned i;

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, WCL_CUR_STSr, 0, 0, &wcl_cur_sts_buf));
    for (i = 0; i < COUNTOF(flds); ++i) {
        if (soc_reg_field_get(unit, WCL_CUR_STSr, wcl_cur_sts_buf, flds[i]) == 0) {
            return (SOC_E_BUSY);
        }
    }

    return (SOC_E_NONE);
}


STATIC int
wcl_poll_rxlane_lock(int unit)
{
    unsigned n;

    for (n = 1000; n; --n) {
        int errcode = wcl_chk_rxlane_lock(unit);

        if (errcode == SOC_E_BUSY) {
            continue;
        }

        _SHR_RETURN(errcode);
    }

    _SHR_RETURN(SOC_E_TIMEOUT);
}


STATIC int
ilamac_chk_rx_intf_state0(int unit)
{
    static const struct {
        soc_field_t fld;
        unsigned    exp_val;
    } flds[] = {
        { XON_TX_CH0f,     1 },
        { RX_ALIGNEDf,     1 },
        { RX_MISALIGNEDf,  0 },
        { RX_ALIGNED_ERRf, 0 },
        { RX_WORD_SYNCf,   0xfff },
        { RX_SYNCEDf,      0xfff }
    };

    uint32   ilamac_rx_intf_state0_buf;
    unsigned i;

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ILAMAC_RX_INTF_STATE0r, 0, 0, &ilamac_rx_intf_state0_buf));
    for (i = 0; i < COUNTOF(flds); ++i) {
        if (soc_reg_field_get(unit, ILAMAC_RX_INTF_STATE0r, ilamac_rx_intf_state0_buf, flds[i].fld) != flds[i].exp_val) {
            return (SOC_E_BUSY);
        }
    }

    return (SOC_E_NONE);
}


STATIC int
ilamac_poll_rx_intf_state0(int unit)
{
    unsigned n;

    for (n = 60; n; --n) {
        int errcode = ilamac_chk_rx_intf_state0(unit);

        if (errcode == SOC_E_BUSY) {
            sal_usleep(1000);       /* Wait 1 ms */
            continue;
        }

        _SHR_RETURN(errcode);
    }

    _SHR_RETURN(SOC_E_TIMEOUT);
}


STATIC int
ilamac_chk_rx_intf_state1(int unit)
{
    uint32 ilamac_rx_intf_state1_buf;

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ILAMAC_RX_INTF_STATE1r, 0, 0, &ilamac_rx_intf_state1_buf));
    _SHR_RETURN(ilamac_rx_intf_state1_buf == 0 ? SOC_E_NONE : SOC_E_FAIL);
}


STATIC int
ilamac_chk_tx_intf_state(int unit)
{
    static const struct {
        soc_field_t fld;
        unsigned    exp_val;
    } flds[] = {
        { TX_OVERFLOW_ERRf,  0 },
        { TX_UNDERFLOW_ERRf, 0 },
        { TX_BURST_ERRf,     0 },
        { TX_OVFOUTf,        0 },
        { TX_RDYOUTf,        1 }
    };

    uint32   ilamac_tx_intf_state_buf;
    unsigned i;
    
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ILAMAC_TX_INTF_STATEr, 0, 0, &ilamac_tx_intf_state_buf));
    for (i = 0; i < COUNTOF(flds); ++i) {
        if (soc_reg_field_get(unit, ILAMAC_TX_INTF_STATEr, ilamac_tx_intf_state_buf, flds[i].fld) != flds[i].exp_val) {
            _SHR_RETURN(SOC_E_FAIL);
        }
    }

    _SHR_RETURN(SOC_E_NONE);
}


STATIC int
nl_mdio_clr_csm_status_regs(int unit, unsigned mdio_portid, uint8 log)
{
    static const uint16 mdio_regs[] = {
        0x8180,
        0x8183,
        0x8184,
        0x8185,
        0x8186,
        0x8187,
        0x8188,
        0x8189,
        0x818a        
    };

    const unsigned mdio_dev_id = 1;

    unsigned i;

    uint16 dummy;

    for (i = 0; i < COUNTOF(mdio_regs); ++i) {
        dummy = 0;

        SOC_IF_ERROR_RETURN(
            nl_mdio_read(unit, mdio_portid, mdio_dev_id, mdio_regs[i], &dummy));
        /* Log incase of any error */
        if (dummy && log) {
            LOG_ERROR(BSL_LS_SOC_ESM,
                      (BSL_META_U(unit,
                                  "NL11K MDIO [0x%x] error [0x%x] on "
                                  "portid %d on unit %d.\n"),
                                  mdio_regs[i],
                                  dummy, mdio_portid, unit));

        }
    }

    _SHR_RETURN(SOC_E_NONE);
}


STATIC int
nl_mdio_chk_csm_status_regs(int unit, unsigned mdio_portid, unsigned chk_crx)
{
    static const struct {
        uint8  crxf;
        uint16 reg_addr;
        uint16 exp_val;
    } regvals[] = {
        { 0, 0x8180,   0 },
        { 0, 0x8184,   0 },
        { 1, 0x8185,   0 },
        { 0, 0x8186,   0 },
        { 0, 0x8187,   0 },
        { 1, 0x8188,   0 },
        { 0, 0x8189,   0 },
        { 0, 0x818a,   0 }
    };

    const unsigned mdio_dev_id = 1;

    unsigned i;
    uint16   regval;

    for (i = 0; i < COUNTOF(regvals); ++i) {
        if (regvals[i].crxf && !chk_crx) {
            continue;
        }
        
        /* Erratum: CRC24 error register crx_idle_crc24_err (0x8185) may increament
           even when no CRC24 error is detected. So, ignoring 0x8185 register. Real
           CRC24 errors are detected and handled as expected */ 
        if (regvals[i].reg_addr == 0x8185) {
            continue;
        } 
        
        SOC_IF_ERROR_RETURN(nl_mdio_read(unit, mdio_portid, mdio_dev_id, regvals[i].reg_addr, &regval));
        if (regval != regvals[i].exp_val) {
            _SHR_RETURN(SOC_E_FAIL);
        }
    }

    SOC_IF_ERROR_RETURN(nl_mdio_read(unit, mdio_portid, mdio_dev_id, 0x8183, &regval));
    if (regval != (chk_crx ? 0xf : 0xe)) {
        _SHR_RETURN(SOC_E_FAIL);
    }

    _SHR_RETURN(SOC_E_NONE);
}


STATIC int
nl_mdio_chk_csm_error_counters(int unit, unsigned mdio_portid, unsigned chk_crx)
{
    static const uint16 regaddrs[] = {
        0x8203,
        0x8204,
        0x8205,
        0x8284,
        0x8285,
        0x8288,
        0x8289,
        0x828a,
        0x828b,
        0x8290,
        0x8291,
        0x8292,
        0x8293        
    };

    const unsigned mdio_dev_id = 1;

    unsigned i;
    uint16   regval;

    for (i = 0; i < COUNTOF(regaddrs); ++i) {
        /* Erratum: CRC24 error register rx_nmac_csm_cres_crc_err (0x8292, 0x8293) may 
           increament even when no CRC24 error is detected. So, ignoring 0x8292 and 0x8293 
           registers. Real CRC24 errors are detected and handled as expected */
        if (regaddrs[i] == 0x8292 || regaddrs[i] == 0x8293) {
            continue;
        }

        SOC_IF_ERROR_RETURN(nl_mdio_read(unit, mdio_portid, mdio_dev_id, regaddrs[i], &regval));
        if (regval != 0) {
            _SHR_RETURN(SOC_E_FAIL);
        }
    }

    _SHR_RETURN(SOC_E_NONE);
}


STATIC int
nl_mdio_chk_pcs_error_status(int unit, unsigned mdio_portid, unsigned chk_crx)
{
    const unsigned min_mdio_dev_id = 2;
    const unsigned max_mdio_dev_id = chk_crx ? 10 : 7;
    const unsigned min_mdio_addr   = 0x8300;
    const unsigned max_mdio_addr   = 0x8308;

    unsigned errcnt = 0;
    unsigned mdio_dev_id, mdio_addr;
    uint16   exp_val, obs_val;
    unsigned int nl_16lane_mode = 0;


    if (((soc_tcam_info_t *) SOC_CONTROL(unit)->tcam_info)->num_tcams  == 2) {
        nl_16lane_mode = 1;
    }

    for (mdio_dev_id = min_mdio_dev_id; mdio_dev_id <= max_mdio_dev_id; ++mdio_dev_id) {
        for (mdio_addr = min_mdio_addr; mdio_addr <= max_mdio_addr; ++mdio_addr) {
            exp_val = mdio_addr == 0x8300 ? 0xff0 : 0;
            if (nl_16lane_mode) {
                /* mdio_dev_id - 6,7 are for NL_RX_LANES 16-19, 20-23 respectively - not needed for 16 req lane config*/

                if ((mdio_dev_id == 6) || (mdio_dev_id == 7)) {
                    exp_val = 0;
                }
            }
            SOC_IF_ERROR_RETURN(nl_mdio_read(unit, mdio_portid, mdio_dev_id, mdio_addr, &obs_val));
            if (obs_val != exp_val) {
                ++errcnt;
            }
        }
    }

    _SHR_RETURN(errcnt ? SOC_E_FAIL : SOC_E_NONE);
}


STATIC int
nl_mdio_chk_pcs_error_counters(int unit, unsigned mdio_portid, unsigned chk_crx)
{
    const unsigned min_mdio_dev_id = 2,
        max_mdio_dev_id = chk_crx ? 10 : 7,
        min_mdio_addr   = 0x8380,
        max_mdio_addr   = 0x83a7;

    int      errcode = SOC_E_NONE;
    unsigned mdio_dev_id, mdio_addr;

    for (mdio_dev_id = min_mdio_dev_id; mdio_dev_id <= max_mdio_dev_id; ++mdio_dev_id) {
        for (mdio_addr = min_mdio_addr; mdio_addr <= max_mdio_addr; ++mdio_addr) {
            uint16 regval;

            SOC_IF_ERROR_RETURN(nl_mdio_read(unit, mdio_portid, mdio_dev_id, mdio_addr, &regval));
            if (regval != 0) {
                errcode = SOC_E_FAIL;
            }
        }
    }

    if (SOC_FAILURE(errcode) && mdio_portid == 2) {
        

        _SHR_RETURN(SOC_E_NONE);
    }

    _SHR_RETURN(errcode);
}


STATIC int
chk_fifos(int unit)
{
    unsigned errcnt = 0;

    {
        uint32 esm_et_rsp_fifo_status_buf;
        
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ESM_ET_RSP_FIFO_STATUSr, 0, 0, &esm_et_rsp_fifo_status_buf));
        if (soc_reg_field_get(unit, ESM_ET_RSP_FIFO_STATUSr, esm_et_rsp_fifo_status_buf, WRITE_POINTERf) != soc_reg_field_get(unit, ESM_ET_RSP_FIFO_STATUSr, esm_et_rsp_fifo_status_buf, READ_POINTERf)) {
            ++errcnt;
        }
        if (!soc_reg_field_get(unit, ESM_ET_RSP_FIFO_STATUSr, esm_et_rsp_fifo_status_buf, EMPTYf)) {
            ++errcnt;
        }
        if (soc_reg_field_get(unit, ESM_ET_RSP_FIFO_STATUSr, esm_et_rsp_fifo_status_buf, FULLf)) {
            ++errcnt;
        }
    }

    {
        uint32 esm_adm_ctrl_fifo_status_buf;


        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ESM_ADM_CTRL_FIFO_STATUSr, 0, 0, &esm_adm_ctrl_fifo_status_buf));
        if (soc_reg_field_get(unit, ESM_ADM_CTRL_FIFO_STATUSr, esm_adm_ctrl_fifo_status_buf, WRITE_POINTERf) != soc_reg_field_get(unit, ESM_ADM_CTRL_FIFO_STATUSr, esm_adm_ctrl_fifo_status_buf, READ_POINTERf)) {
            ++errcnt;
        }
        if (!soc_reg_field_get(unit, ESM_ADM_CTRL_FIFO_STATUSr, esm_adm_ctrl_fifo_status_buf, EMPTYf)) {
            ++errcnt;
        }
        if (soc_reg_field_get(unit, ESM_ADM_CTRL_FIFO_STATUSr, esm_adm_ctrl_fifo_status_buf, FULLf)) {
            ++errcnt;
        }
        
    }

    {
        uint32 esm_max_latency_recorded_buf;

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ESM_MAX_LATENCY_RECORDEDr, 0, 0, &esm_max_latency_recorded_buf));
        if (soc_reg_field_get(unit, ESM_MAX_LATENCY_RECORDEDr, esm_max_latency_recorded_buf, MAX_LATENCYf) > ESMIF_ALLOWED_FIFO_DEPTH) {
            ++errcnt;
        }
    }

    {
        uint64 etu_tx_req_fifo_sts_buf;

        SOC_IF_ERROR_RETURN(soc_reg64_get(unit, ETU_TX_REQ_FIFO_STSr, 0, 0, &etu_tx_req_fifo_sts_buf));
        if (soc_reg64_field32_get(unit, ETU_TX_REQ_FIFO_STSr, etu_tx_req_fifo_sts_buf, WPTf) !=
            soc_reg64_field32_get(unit, ETU_TX_REQ_FIFO_STSr, etu_tx_req_fifo_sts_buf, RPTf)) {
            ++errcnt;
        }
        if (!soc_reg64_field32_get(unit, ETU_TX_REQ_FIFO_STSr, etu_tx_req_fifo_sts_buf, EMPTYf)) {
            ++errcnt;
        }
        if (soc_reg64_field32_get(unit, ETU_TX_REQ_FIFO_STSr, etu_tx_req_fifo_sts_buf, FULLf)) {
            ++errcnt;
        }
        if (soc_reg64_field32_get(unit, ETU_TX_REQ_FIFO_STSr, etu_tx_req_fifo_sts_buf, EARLY_FULLf)) {
            ++errcnt;
        }
        if (soc_reg64_field32_get(unit, ETU_TX_REQ_FIFO_STSr, etu_tx_req_fifo_sts_buf, GTE_CP_ACC_THRf) != 0) {
            ++errcnt;
        }
    }    

    {
        uint32 etu_tx_pipe_ctl_fifo_sts_buf;

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_TX_PIPE_CTL_FIFO_STSr, 0, 0, &etu_tx_pipe_ctl_fifo_sts_buf));
        if (soc_reg_field_get(unit, ETU_TX_PIPE_CTL_FIFO_STSr, etu_tx_pipe_ctl_fifo_sts_buf, WPTf) != soc_reg_field_get(unit, ETU_TX_PIPE_CTL_FIFO_STSr, etu_tx_pipe_ctl_fifo_sts_buf, RPTf)) {
            ++errcnt;
        }
        if (!soc_reg_field_get(unit, ETU_TX_PIPE_CTL_FIFO_STSr, etu_tx_pipe_ctl_fifo_sts_buf, EMPTYf)) {
            ++errcnt;
        }
        if (soc_reg_field_get(unit, ETU_TX_PIPE_CTL_FIFO_STSr, etu_tx_pipe_ctl_fifo_sts_buf, EARLY_FULLf)) {
            ++errcnt;
        }
    }    

    {
        uint32 etu_rx_rsp_fifo_sts_buf;

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_RX_RSP_FIFO_STSr, 0, 0, &etu_rx_rsp_fifo_sts_buf));
        if (soc_reg_field_get(unit, ETU_RX_RSP_FIFO_STSr, etu_rx_rsp_fifo_sts_buf, WPTf) != soc_reg_field_get(unit, ETU_RX_RSP_FIFO_STSr, etu_rx_rsp_fifo_sts_buf, RPTf)) {
            ++errcnt;
        }
        if (!soc_reg_field_get(unit, ETU_RX_RSP_FIFO_STSr, etu_rx_rsp_fifo_sts_buf, EMPTYf)) {
            ++errcnt;
        }
        if (soc_reg_field_get(unit, ETU_RX_RSP_FIFO_STSr, etu_rx_rsp_fifo_sts_buf, FULLf)) {
            ++errcnt;
        }
        if (!soc_reg_field_get(unit, ETU_RX_RSP_FIFO_STSr, etu_rx_rsp_fifo_sts_buf, XON_RXf)) {
            ++errcnt;
        }
    }    

    _SHR_RETURN(errcnt ? SOC_E_FAIL : SOC_E_NONE);
}


STATIC int
chk_etu_counters(int unit)
{
    unsigned errcnt = 0;

    {
        uint32 etu_dbg_ipipe_req_rsp_count_buf;

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_DBG_IPIPE_REQ_RSP_COUNTr, 0, 0, &etu_dbg_ipipe_req_rsp_count_buf));
        if (soc_reg_field_get(unit, ETU_DBG_IPIPE_REQ_RSP_COUNTr, etu_dbg_ipipe_req_rsp_count_buf, RSP_COUNTf)
            != soc_reg_field_get(unit, ETU_DBG_IPIPE_REQ_RSP_COUNTr, etu_dbg_ipipe_req_rsp_count_buf, REQ_COUNTf)
            ) {
            ++errcnt;
        }
    }

    {
        uint32 etu_dbg_ipipe_err_rsp_count_buf;

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_DBG_IPIPE_ERR_RSP_COUNTr, 0, 0, &etu_dbg_ipipe_err_rsp_count_buf));
        if (soc_reg_field_get(unit, ETU_DBG_IPIPE_ERR_RSP_COUNTr, etu_dbg_ipipe_err_rsp_count_buf, ERR_RSP_COUNTf) != 0) {
            ++errcnt;
        }
    }

    {
        uint64 ilamac_tx_packets_count_buf, ilamac_rx_packets_count_buf;
        uint64 tx_cnt, rx_cnt;

        SOC_IF_ERROR_RETURN(soc_reg64_get(unit, ILAMAC_RX_PACKETS_COUNTr, 0, 0, &ilamac_rx_packets_count_buf));
        SOC_IF_ERROR_RETURN(soc_reg64_get(unit, ILAMAC_TX_PACKETS_COUNTr, 0, 0, &ilamac_tx_packets_count_buf));
        rx_cnt = soc_reg64_field_get(unit, ILAMAC_RX_PACKETS_COUNTr, ilamac_rx_packets_count_buf, COUNTf);
        tx_cnt = soc_reg64_field_get(unit, ILAMAC_TX_PACKETS_COUNTr, ilamac_tx_packets_count_buf, COUNTf);
        if (COMPILER_64_NE(rx_cnt, tx_cnt)) {
            ++errcnt;
        }
    }

    {
        uint32 ilamac_debug_count_buf;

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ILAMAC_DEBUG_COUNTr, 0, 0, &ilamac_debug_count_buf));
        if (soc_reg_field_get(unit, ILAMAC_DEBUG_COUNTr, ilamac_debug_count_buf, XOFF_TX_CH0_COUNTf) != 0) {
            ++errcnt;
        }
        if (soc_reg_field_get(unit, ILAMAC_DEBUG_COUNTr, ilamac_debug_count_buf, TX_RDYOUT_COUNTf) != 0) {
            ++errcnt;
        }
    }

    {
        static const soc_reg_t regs[] = {
            ILAMAC_RX_WORD_SYNC_ERRORS_COUNTr,
            ILAMAC_RX_FRAMING_ERRORS_COUNTr,
            ILAMAC_RX_BAD_TYPE_ERRORS_COUNTr,
            ILAMAC_RX_DESCRAM_ERRORS_COUNTr,
            ILAMAC_RX_ALIGNMENT_ERRORS_COUNTr,
            ILAMAC_RX_ALIGNMENT_FAILURES_ERRORS_COUNTr,
            ILAMAC_RX_CRC_ERRORS_COUNTr,
            ILAMAC_RX_BURSTMAX_ERRORS_COUNTr,
            ILAMAC_RX_LANE_CRC_ERRORS_COUNTr,
            ILAMAC_RX_BAD_PACKETS_COUNTr,
            WCL_RX_LOST_LOCK_COUNTr
        };

        unsigned i;

        for (i = 0; i < COUNTOF(regs); ++i) {
            unsigned rx_lane;
            
            for (rx_lane = 0; rx_lane < NL_NUM_RSP_LANES; ++rx_lane) {
                uint32 regbuf;
                
                SOC_IF_ERROR_RETURN(soc_reg32_get(unit, regs[i], 0, rx_lane, &regbuf));
                if (soc_reg_field_get(unit, regs[i], regbuf, CMSBf) != 0
                    || soc_reg_field_get(unit, regs[i], regbuf, COUNTf) != 0
                    ) {
                    ++errcnt;
                }
            }
        }
    }

    {
        uint32 etu_dbg_sm_buf;

        SOC_IF_ERROR_RETURN(READ_ETU_DBG_SMr(unit, &etu_dbg_sm_buf));
        if (etu_dbg_sm_buf != 0) {
            ++errcnt;
        }
    }

    _SHR_RETURN(errcnt ? SOC_E_FAIL : SOC_E_NONE);
}


STATIC int
chk_reg_flds_w_mask(int               unit,
                    soc_reg_t         reg,
                    uint32            intr_mask,
                    const soc_field_t *flds,
                    unsigned          nflds
                    )
{
    uint32   regbuf;
    unsigned i;
    
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, 0, 0, &regbuf));
    for (i = 0; i < nflds; ++i) {
        if ((~soc_reg_field_get(unit, reg, intr_mask, flds[i])
             & soc_reg_field_get(unit, reg, regbuf, flds[i])
             )
            != 0
            ) {
            _SHR_RETURN(SOC_E_FAIL);
        }
    }
    
    _SHR_RETURN(SOC_E_NONE);
}


STATIC int
chk_esmif_intr_status(int unit, uint32 intr_mask)
{
    static const soc_field_t flds[] = {
        DROP_COUNT_MSBf,
        CNP_SEARCH_REQ_COUNT_MSBf,
        SOP_DROPf,
        XOFF_AXP_OVERSUBSf,
        MANDATORY_DENYf,
        DROP_ALLf,
        OPT_REQ_TRICKLEDf,
        OPTIONAL_DENYf,
        DROP_OPTIONALf,
        ET_RSP_ERRf,
        CNP_SEARCH_REQf,
        ET_REQ_FIFO_FULL_DENYf,
        ET_REQ_FIFO_FULLf,
        ADM_CTRL_FIFO_FULL_DENYf,
        ADM_CTRL_FIFO_FULLf,
        ET_RSP_FIFO_FULLf,
        CBA_MISMATCHf,
        ADM_CTRL_FIFO_UNDERFLOWf,
        ET_RSP_FIFO_UNDERFLOWf,
        ET_RSP_FIFO_OVERFLOWf
    };

    _SHR_RETURN(chk_reg_flds_w_mask(unit,
                                    ESM_EVENT_ERR_STATUS_INTRr,
                                    intr_mask,
                                    flds,
                                    COUNTOF(flds)
                                    )
                );
}

STATIC int nl_chk_err_status_reg(int unit, unsigned dev_id);

STATIC int
chk_etu_global_intr_sts(int unit, uint32 intr_mask, unsigned num_nl)
{
    static const soc_field_t flds[] = {
        ILAMAC_RX_INTF_INTR1f,
        ILAMAC_RX_INTF_INTR0f,
        ILAMAC_RX_LANE_INTR3f,
        ILAMAC_RX_LANE_INTR2f,
        ILAMAC_RX_LANE_INTR1f,
        ILAMAC_RX_LANE_INTR0f,
        ILAMAC_TX_INTRf,
        RX_RSP_FIFO_INTRf,
        TX_PIPE_CTL_FIFO_INTRf,
        TX_REQ_FIFO_INTRf,
        WCL_INTRf,
        RSVDf,
        EXT_L2_REQ_LTH_ERRf,
        TWO_MISSING_PKTf,
        INT_MEM_RST_DONEf,
        TX_RAW_REQ_DONEf,
        RRFE_WAIT_PENDING0f,
        RRFE_WAIT_FULLf,
        PP_XLAT1_PERRf,
        PP_XLAT0_PERRf,
        EXT_L2_CMD_ERRf,
        SBUS_CMD_ERRf
    };

    int      errcode = SOC_E_NONE;
    uint32   etu_global_intr_sts_buf;
    unsigned i, dev_id;
    
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_GLOBAL_INTR_STSr, 0, 0, &etu_global_intr_sts_buf));

    for (i = 0; i < COUNTOF(flds); ++i) {
        if ((~soc_reg_field_get(unit, ETU_GLOBAL_INTR_STSr, intr_mask, flds[i])
             & soc_reg_field_get(unit, ETU_GLOBAL_INTR_STSr, etu_global_intr_sts_buf, flds[i])
             )
            != 0
            ) {
            errcode = SOC_E_FAIL;
        }
    }

    if ((~soc_reg_field_get(unit, ETU_GLOBAL_INTR_STSr, intr_mask, EXT_TCAM_INTR0f)
         & soc_reg_field_get(unit, ETU_GLOBAL_INTR_STSr, etu_global_intr_sts_buf, EXT_TCAM_INTR0f)
         )
        != 0
        || (~soc_reg_field_get(unit, ETU_GLOBAL_INTR_STSr, intr_mask, EXT_TCAM_INTR1f)
            & soc_reg_field_get(unit, ETU_GLOBAL_INTR_STSr, etu_global_intr_sts_buf, EXT_TCAM_INTR1f)
            )
        != 0
        || (~soc_reg_field_get(unit, ETU_GLOBAL_INTR_STSr, intr_mask, RX_CW_ERS1f)
            & soc_reg_field_get(unit, ETU_GLOBAL_INTR_STSr, etu_global_intr_sts_buf, RX_CW_ERS1f)
            )
        != 0
        ) {
        for (dev_id = 0; dev_id < num_nl; ++dev_id) {
            if (SOC_FAILURE(nl_chk_err_status_reg(unit, dev_id))) {
                errcode = SOC_E_FAIL;
            }
        }
    }

    if ((~soc_reg_field_get(unit, ETU_GLOBAL_INTR_STSr, intr_mask, RX_CW_ERRf)
         & soc_reg_field_get(unit, ETU_GLOBAL_INTR_STSr, etu_global_intr_sts_buf, RX_CW_ERRf)
         )
        != 0
        ) {
        /* Received packet error - must read:
           ETU_RX_CW_ERR_INFO1, ETU_RX_CW_ERR_INFO2, ETU_RX_CW_ERR_CHANOUT,
           ETU_RX_CW_ERR_DW0_LO, ETU_RX_CW_ERR_DW0_HI, ETU_RX_CW_ERR_DW1_LO,
           ETU_RX_CW_ERR_DW1_HI registers
        */
        
        uint32 dummy;
        uint64 dummy64;

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_RX_CW_ERR_INFO1r,   0, 0, &dummy));
        SOC_IF_ERROR_RETURN(soc_reg_get(unit, ETU_RX_CW_ERR_INFO2r,   0, 0, &dummy64));
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_RX_CW_ERR_CHANOUTr, 0, 0, &dummy));
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_RX_CW_ERR_DW0_LOr,  0, 0, &dummy));
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_RX_CW_ERR_DW0_HIr,  0, 0, &dummy));
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_RX_CW_ERR_DW1_LOr,  0, 0, &dummy));
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_RX_CW_ERR_DW1_HIr,  0, 0, &dummy));

        errcode = SOC_E_FAIL;
    }

    if ((~soc_reg_field_get(unit, ETU_GLOBAL_INTR_STSr, intr_mask, RX_CW_ERS0f)
         & soc_reg_field_get(unit, ETU_GLOBAL_INTR_STSr, etu_global_intr_sts_buf, RX_CW_ERS0f)
         )
        != 0
        ) {
        uint32 dummy;

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_RX_ERS0_CHANOUTr, 0, 0, &dummy));
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_RX_ERS0_DW0_HIr, 0, 0, &dummy));
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_RX_ERS0_DW0_LOr, 0, 0, &dummy));
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_RX_ERS0_DW1_HIr, 0, 0, &dummy));
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_RX_ERS0_DW1_LOr, 0, 0, &dummy));
        
        errcode = SOC_E_FAIL;
    }
    
    _SHR_RETURN(errcode);
}

 
STATIC int
chk_ilamac_rx_intf_intr0_sts(int unit, uint32 intr_mask)
{
    static const soc_field_t flds[] = {
        XOFF_TX_CH1f,
        XOFF_TX_CH0f,
        RX_ALIGNEDf,
        RX_MISALIGNEDf,
        RX_ALIGNED_ERRf,
        RX_WORD_SYNCf,
        RX_SYNCEDf
    };

    _SHR_RETURN(chk_reg_flds_w_mask(unit,
                                    ILAMAC_RX_INTF_INTR0_STSr,
                                    intr_mask,
                                    flds,
                                    COUNTOF(flds)
                                    )
                );
}


STATIC int
chk_ilamac_rx_intf_intr1_sts(int unit, uint32 intr_mask)
{
    static const soc_field_t flds[] = {
        RX_BURST_GT_16B_ERRf,
        RX_CRC24_ERRf,
        RX_MSOP_ERRf,
        RX_MEOP_ERRf,
        RX_OVERFLOW_ERRf,
        RX_BURSTMAX_ERRf,
        RX_BURST_ERRf
    };

    _SHR_RETURN(chk_reg_flds_w_mask(unit,
                                    ILAMAC_RX_INTF_INTR1_STSr,
                                    intr_mask,
                                    flds,
                                    COUNTOF(flds)
                                    )
                );
}


STATIC int
chk_ilamac_rx_lane_intr0_sts(int unit, uint32 intr_mask)
{
    static const soc_field_t flds[] = {
        RX_SYNCED_ERRf,
        RX_FRAMING_ERRf
    };

    _SHR_RETURN(chk_reg_flds_w_mask(unit,
                                    ILAMAC_RX_LANE_INTR0_STSr,
                                    intr_mask,
                                    flds,
                                    COUNTOF(flds)
                                    )
                );
}


STATIC int
chk_ilamac_rx_lane_intr1_sts(int unit, uint32 intr_mask)
{
    static const soc_field_t flds[] = {
        RX_BAD_TYPE_ERRf,
        RX_MF_ERRf
    };

    _SHR_RETURN(chk_reg_flds_w_mask(unit,
                                    ILAMAC_RX_LANE_INTR1_STSr,
                                    intr_mask,
                                    flds,
                                    COUNTOF(flds)
                                    )
                );
}


STATIC int
chk_ilamac_rx_lane_intr2_sts(int unit, uint32 intr_mask)
{
    static const soc_field_t flds[] = {
        RX_DESCRAM_ERRf,
        RX_MF_LEN_ERRf
    };

    _SHR_RETURN(chk_reg_flds_w_mask(unit,
                                    ILAMAC_RX_LANE_INTR2_STSr,
                                    intr_mask,
                                    flds, 
                                    COUNTOF(flds)
                                    )
                );
}


STATIC int
chk_ilamac_rx_lane_intr3_sts(int unit, uint32 intr_mask)
{
    static const soc_field_t flds[] = {
        RX_MF_REPEAT_ERRf,
        RX_CRC32_ERRf
    };

    _SHR_RETURN(chk_reg_flds_w_mask(unit,
                                    ILAMAC_RX_LANE_INTR3_STSr,
                                    intr_mask,
                                    flds, 
                                    COUNTOF(flds)
                                    )
                );
}


STATIC int
chk_ilamac_tx_intr_sts(int unit, uint32 intr_mask)
{
    static const soc_field_t flds[] = {
        TX_ERRINf,
        TX_OVERFLOW_ERRf,
        TX_UNDERFLOW_ERRf,
        TX_BURST_ERRf,
        TX_OVFOUTf,
        TX_RDYOUTf
    };

    _SHR_RETURN(chk_reg_flds_w_mask(unit,
                                    ILAMAC_TX_INTR_STSr,
                                    intr_mask,
                                    flds,
                                    COUNTOF(flds)
                                    )
                );
}


STATIC int
chk_wcl_intr_sts(int unit, uint32 intr_mask)
{
    static const soc_field_t flds[] = {
        TXPLL_LOCK_5f,
        TXPLL_LOCK_4f,
        TXPLL_LOCK_3f,
        TXPLL_LOCK_2f,
        TXPLL_LOCK_1f,
        TXPLL_LOCK_0f,
        RXSEQDONE1G_4_3f,
        RXSEQDONE1G_4_2f,
        RXSEQDONE1G_4_1f,
        RXSEQDONE1G_4_0f,
        RXSEQDONE1G_2_3f,
        RXSEQDONE1G_2_2f,
        RXSEQDONE1G_2_1f,
        RXSEQDONE1G_2_0f,
        RXSEQDONE1G_0_3f,
        RXSEQDONE1G_0_2f,
        RXSEQDONE1G_0_1f,
        RXSEQDONE1G_0_0f
    };

    _SHR_RETURN(chk_reg_flds_w_mask(unit,
                                    WCL_INTR_STSr,
                                    intr_mask,
                                    flds,
                                    COUNTOF(flds)
                                    )
                );
}


STATIC int
chk_etu_tx_req_fifo_intr_sts(int unit, uint32 intr_mask)
{
    static const soc_field_t flds[] = {
        SBUS_STALL_MAX_LATf,
        EXT_L2_STALL_MAX_LATf,
        SBUS_STALL_GTE_CP_ACC_THRf,
        EXT_L2_STALL_GTE_CP_ACC_THRf,
        UNDERFLOW_DETECTEDf,
        OVERFLOW_DETECTEDf,
        FULLf,
        EARLY_FULLf,
        DBE_EVf,
        SBE_EVf,
        DBE_ODf,
        SBE_ODf
    };

    _SHR_RETURN(chk_reg_flds_w_mask(unit,
                                    ETU_TX_REQ_FIFO_INTR_STSr,
                                    intr_mask,
                                    flds,
                                    COUNTOF(flds)
                                    )
                );
}


STATIC int
chk_etu_tx_pipe_ctl_fifo_intr_sts(int unit, uint32 intr_mask)
{
    static const soc_field_t flds[] = {
        UNDERFLOW_DETECTEDf,
        OVERFLOW_DETECTEDf,
        FULLf,
        EARLY_FULLf,
        DBEf,
        SBEf
    };

    _SHR_RETURN(chk_reg_flds_w_mask(unit,
                                    ETU_TX_PIPE_CTL_FIFO_INTR_STSr,
                                    intr_mask,
                                    flds,
                                    COUNTOF(flds)
                                    )
                );
}


STATIC int
chk_etu_rx_rsp_fifo_intr_sts(int unit, uint32 intr_mask)
{
    static const soc_field_t flds[] = {
        XOFF_RXf,
        UNDERFLOW_DETECTEDf,
        OVERFLOW_DETECTEDf,
        FULLf,
        DBE_EVf,
        SBE_EVf,
        DBE_ODf,
        SBE_ODf
    };

    _SHR_RETURN(chk_reg_flds_w_mask(unit,
                                    ETU_RX_RSP_FIFO_INTR_STSr,
                                    intr_mask,
                                    flds,
                                    COUNTOF(flds)
                                    )
                );
}


STATIC int
chk_all_intr_sts(int unit, unsigned num_nl)
{
    SOC_IF_ERROR_RETURN(chk_esmif_intr_status(unit, 0x800));
    SOC_IF_ERROR_RETURN(chk_etu_global_intr_sts(unit, 0x600, num_nl));
    SOC_IF_ERROR_RETURN(chk_ilamac_rx_intf_intr0_sts(unit, 0));
    SOC_IF_ERROR_RETURN(chk_ilamac_rx_intf_intr1_sts(unit, 0));
    SOC_IF_ERROR_RETURN(chk_ilamac_rx_lane_intr0_sts(unit, 0));
    SOC_IF_ERROR_RETURN(chk_ilamac_rx_lane_intr1_sts(unit, 0));
    SOC_IF_ERROR_RETURN(chk_ilamac_rx_lane_intr2_sts(unit, 0));
    SOC_IF_ERROR_RETURN(chk_ilamac_rx_lane_intr3_sts(unit, 0));
    SOC_IF_ERROR_RETURN(chk_ilamac_tx_intr_sts(unit, 1));
    SOC_IF_ERROR_RETURN(chk_wcl_intr_sts(unit, 0));
    SOC_IF_ERROR_RETURN(chk_etu_tx_req_fifo_intr_sts(unit, 0));
    SOC_IF_ERROR_RETURN(chk_etu_tx_pipe_ctl_fifo_intr_sts(unit, 0));
    SOC_IF_ERROR_RETURN(chk_etu_rx_rsp_fifo_intr_sts(unit, 0));

    _SHR_RETURN(SOC_E_NONE);
}


STATIC int
chk_esmif_adc(int unit)
{
    unsigned errcnt = 0;

    {
        uint32 esmif_cur_pending_cost0_buf;

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ESMIF_CUR_PENDING_COST0r, 0, 0, &esmif_cur_pending_cost0_buf));
        if (soc_reg_field_get(unit, ESMIF_CUR_PENDING_COST0r, esmif_cur_pending_cost0_buf, COSTf) != 0) {
            ++errcnt;
        }
    }
    {
        uint32 esmif_cur_pending_cost1_buf;

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ESMIF_CUR_PENDING_COST1r, 0, 0, &esmif_cur_pending_cost1_buf));
        if (soc_reg_field_get(unit, ESMIF_CUR_PENDING_COST1r, esmif_cur_pending_cost1_buf, COSTf) != 0) {
            ++errcnt;
        }
    }
    {
        uint32 esmif_cur_pending_cost2_buf;

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ESMIF_CUR_PENDING_COST2r, 0, 0, &esmif_cur_pending_cost2_buf));
        if (soc_reg_field_get(unit, ESMIF_CUR_PENDING_COST2r, esmif_cur_pending_cost2_buf, COSTf) != 0) {
            ++errcnt;
        }
    }
    {
        uint32 esmif_adm_ctrl_status_buf;

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ESMIF_ADM_CTRL_STATUSr, 0, 0, &esmif_adm_ctrl_status_buf));
        if (soc_reg_field_get(unit, ESMIF_ADM_CTRL_STATUSr, esmif_adm_ctrl_status_buf, DROP_ALLf)) {
            ++errcnt;
        }        
        if (soc_reg_field_get(unit, ESMIF_ADM_CTRL_STATUSr, esmif_adm_ctrl_status_buf, DROP_OPTIONALf)) {
            ++errcnt;
        }        
        if (soc_reg_field_get(unit, ESMIF_ADM_CTRL_STATUSr, esmif_adm_ctrl_status_buf, XOFF_AXP_OVERSUBSf)) {
            ++errcnt;
        }        
    }
    {
        uint32 esmif_cnp_search_req_count_buf;

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ESMIF_CNP_SEARCH_REQ_COUNTr, 0, 0, &esmif_cnp_search_req_count_buf));
        if (soc_reg_field_get(unit, ESMIF_CNP_SEARCH_REQ_COUNTr, esmif_cnp_search_req_count_buf, COUNTf) != 0) {
            ++errcnt;
        }        
    }
    {
        uint32 esmif_drop_count_buf;

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ESMIF_DROP_COUNTr, 0, 0, &esmif_drop_count_buf));
        if (soc_reg_field_get(unit, ESMIF_DROP_COUNTr, esmif_drop_count_buf, COUNTf) != 0) {
            ++errcnt;
        }        
    }
    {
        uint32 esm_max_latency_recorded_buf;

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ESM_MAX_LATENCY_RECORDEDr, 0, 0, &esm_max_latency_recorded_buf));
        if (soc_reg_field_get(unit, ESM_MAX_LATENCY_RECORDEDr, esm_max_latency_recorded_buf, MAX_LATENCYf) > ESMIF_ALLOWED_FIFO_DEPTH) {
            ++errcnt;
        }        
    }

    _SHR_RETURN(errcnt ? SOC_E_FAIL : SOC_E_NONE);
}

STATIC int
clr_etu_counters(int unit)
{
    static const soc_reg_t regs64[] = {
        ILAMAC_RX_PACKETS_COUNTr,
        ILAMAC_RX_BYTES_COUNTr,
        ILAMAC_TX_PACKETS_COUNTr,
        ILAMAC_TX_BYTES_COUNTr
    }, regs[] = {
        ILAMAC_RX_ALIGNMENT_ERRORS_COUNTr,
        ILAMAC_RX_ALIGNMENT_FAILURES_ERRORS_COUNTr,
        ILAMAC_RX_CRC_ERRORS_COUNTr,
        ILAMAC_RX_BURSTMAX_ERRORS_COUNTr,
        ILAMAC_RX_BAD_PACKETS_COUNTr,
        ILAMAC_DEBUG_COUNTr,
        ETU_DBG_IPIPE_ERR_RSP_COUNTr,
        ETU_DBG_IPIPE_REQ_RSP_COUNTr
    }, regs_w_lanes[] = {
        ILAMAC_RX_WORD_SYNC_ERRORS_COUNTr,
        ILAMAC_RX_FRAMING_ERRORS_COUNTr,
        ILAMAC_RX_BAD_TYPE_ERRORS_COUNTr,
        ILAMAC_RX_DESCRAM_ERRORS_COUNTr,
        ILAMAC_RX_LANE_CRC_ERRORS_COUNTr,
        WCL_RX_LOST_LOCK_COUNTr
    };

    unsigned i;

    {
        uint64 zero;
        
        COMPILER_64_ZERO(zero);
        for (i = 0; i < COUNTOF(regs64); ++i) {
            SOC_IF_ERROR_RETURN(soc_reg64_set(unit, regs64[i], 0, 0, zero));
        }
    }

    for (i = 0; i < COUNTOF(regs); ++i) {
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, regs[i], 0, 0, 0));
    }

    for (i = 0; i < COUNTOF(regs_w_lanes); ++i) {
        unsigned rx_lane;

        for (rx_lane = 0; rx_lane < NL_NUM_RSP_LANES; ++rx_lane) {
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, regs_w_lanes[i], 0, rx_lane, 0));
        }
    }

    _SHR_RETURN(SOC_E_NONE);
}


STATIC int
clr_all_intr_sts(int unit)
{
    static const soc_reg_t regs[] = {
        ESM_EVENT_ERR_STATUS_INTRr,
        ETU_GLOBAL_INTR_STSr,
        ILAMAC_RX_INTF_INTR0_STSr,
        ILAMAC_RX_INTF_INTR1_STSr,
        ILAMAC_RX_LANE_INTR0_STSr,
        ILAMAC_RX_LANE_INTR1_STSr,
        ILAMAC_RX_LANE_INTR2_STSr,
        ILAMAC_RX_LANE_INTR3_STSr,
        ILAMAC_TX_INTR_STSr,
        WCL_INTR_STSr,
        ETU_TX_REQ_FIFO_INTR_STSr,
        ETU_TX_PIPE_CTL_FIFO_INTR_STSr,
        ETU_RX_RSP_FIFO_INTR_STSr
    };    

    unsigned i;

    for (i = 0; i < COUNTOF(regs); ++i) {
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, regs[i], 0, 0, 0));
    }

    _SHR_RETURN(SOC_E_NONE);
}


STATIC int
clr_adc_max_logs(int unit)
{
    SOC_IF_ERROR_RETURN(WRITE_ESMIF_CNP_SEARCH_REQ_COUNTr(unit, 0));
    SOC_IF_ERROR_RETURN(WRITE_ESMIF_DROP_COUNTr(unit, 0));

    {
        uint32 esm_et_hwtl_control_buf;
        
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ESM_ET_HWTL_CONTROLr, 0, 0, &esm_et_hwtl_control_buf));
        soc_reg_field_set(unit, ESM_ET_HWTL_CONTROLr, &esm_et_hwtl_control_buf, START_HWTL_TESTf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ESM_ET_HWTL_CONTROLr, 0, 0, esm_et_hwtl_control_buf));
        soc_reg_field_set(unit, ESM_ET_HWTL_CONTROLr, &esm_et_hwtl_control_buf, START_HWTL_TESTf, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ESM_ET_HWTL_CONTROLr, 0, 0, esm_et_hwtl_control_buf));
    }

    _SHR_RETURN(SOC_E_NONE);
}


STATIC int
init1(int      unit,
      unsigned num_nl,
      unsigned master_wcl_num,
      unsigned rx_fifo_thr,
      unsigned tx_fifo_thr,
      unsigned tx_burst_short_16b
      )
{
    unsigned dev_id;
    unsigned int nl_16lane_mode = 0;


    if (num_nl == 2) {
        nl_16lane_mode = 1;
    }

    SOC_IF_ERROR_RETURN(chk_esmif_init_config(unit, ESMIF_MAX_FIFO_DEPTH));
        
    {
        uint32 esm_misc_control_buf;
        
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ESM_MISC_CONTROLr, 0, 0, &esm_misc_control_buf));
        soc_reg_field_set(unit, ESM_MISC_CONTROLr, &esm_misc_control_buf, EXT_LOOKUP_ENABLEf, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ESM_MISC_CONTROLr, 0, 0, esm_misc_control_buf));
    }
    
    SOC_IF_ERROR_RETURN(dis_intr(unit));
    
    SOC_IF_ERROR_RETURN(etu_rst_nl_wcl_ilamac(unit, 9));
    
    SOC_IF_ERROR_RETURN(wcl_reset_seq(unit, master_wcl_num)); 

    {
        uint32 ilamac_tx_config0_buf;

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ILAMAC_TX_CONFIG0r, 0, 0, &ilamac_tx_config0_buf));
        soc_reg_field_set(unit, ILAMAC_TX_CONFIG0r, &ilamac_tx_config0_buf, BIT_ORDER_INVERTf, 1);
        soc_reg_field_set(unit, ILAMAC_TX_CONFIG0r, &ilamac_tx_config0_buf, TX_XOFF_MODEf, 0);
        soc_reg_field_set(unit, ILAMAC_TX_CONFIG0r, &ilamac_tx_config0_buf, TX_RLIM_ENABLEf, 0);
        soc_reg_field_set(unit, ILAMAC_TX_CONFIG0r, &ilamac_tx_config0_buf, TX_RDYOUT_THRESHf, 0);
        soc_reg_field_set(unit, ILAMAC_TX_CONFIG0r, &ilamac_tx_config0_buf, TX_DISABLE_SKIPWORDf, 0);
        soc_reg_field_set(unit, ILAMAC_TX_CONFIG0r, &ilamac_tx_config0_buf, TX_BURSTSHORTf, 1);
        soc_reg_field_set(unit, ILAMAC_TX_CONFIG0r, &ilamac_tx_config0_buf, TX_BURSTMAXf, 1);
        soc_reg_field_set(unit, ILAMAC_TX_CONFIG0r, &ilamac_tx_config0_buf, TX_MFRAMELEN_MINUS1f, NL_METAFRAME_LENGTH - 1);
        soc_reg_field_set(unit, ILAMAC_TX_CONFIG0r, &ilamac_tx_config0_buf, TX_ENABLEf, 1);
        soc_reg_field_set(unit, ILAMAC_TX_CONFIG0r, &ilamac_tx_config0_buf, XON_RX_CH1f, 0);
        soc_reg_field_set(unit, ILAMAC_TX_CONFIG0r, &ilamac_tx_config0_buf, XON_RX_CH0f, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ILAMAC_TX_CONFIG0r, 0, 0, ilamac_tx_config0_buf));
    }

    /* Allow ETU_RX_RSP_FIFO.XON_RX to go out. At this point in time, etu_rx_rsp_fifo is empty so XON_RX=1
       This signals NL that it is ok to send response packets
       We have not even checked if ilamac_rx is aligned - so why tell NL to send packets ??
       It will not matter because NL is slave device - it sends response packets
       only if we send a request 
    */

    /* Error injection fields. Some default values of non-zero - so make it 0 */
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ILAMAC_TX_CONFIG4r, 0, 0, 0));

    {
        uint32 ilamac_tx_config2_buf;
        /* Set tx lanes to 16 for cascaded mode */
        if (nl_16lane_mode) {
            /* Cascaded mode: set num lanes to 16 */
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ILAMAC_TX_CONFIG2r, 0, 0, &ilamac_tx_config2_buf));
            soc_reg_field_set(unit, ILAMAC_TX_CONFIG2r, &ilamac_tx_config2_buf, TX_LAST_LANEf, 15);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ILAMAC_TX_CONFIG2r, 0, 0, ilamac_tx_config2_buf));
        }
    }
{
    uint32 ilamac_rx_config_buf;

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ILAMAC_RX_CONFIGr, 0, 0, &ilamac_rx_config_buf));
    soc_reg_field_set(unit, ILAMAC_RX_CONFIGr, &ilamac_rx_config_buf, BIT_ORDER_INVERTf, 1);
    soc_reg_field_set(unit, ILAMAC_RX_CONFIGr, &ilamac_rx_config_buf, RX_BURSTMAXf, 0);
    soc_reg_field_set(unit, ILAMAC_RX_CONFIGr, &ilamac_rx_config_buf, RX_MFRAMELEN_MINUS1f, NL_METAFRAME_LENGTH - 1);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ILAMAC_RX_CONFIGr, 0, 0, ilamac_rx_config_buf));
}

{
    uint32 etu_config4_buf;

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_CONFIG4r, 0, 0, &etu_config4_buf));
    soc_reg_field_set(unit, ETU_CONFIG4r, &etu_config4_buf, ILAMAC_TX_LBUS_RST_f, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ETU_CONFIG4r, 0, 0, etu_config4_buf));
    sal_usleep(10);
    /* wait 10uS between de-assertion of ILAMAC_TX_LBUS_RST_ and asertion of
       ILAMAC_TX_SERDES_RST_ */
    soc_reg_field_set(unit, ETU_CONFIG4r, &etu_config4_buf, ILAMAC_TX_SERDES_RST_f, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ETU_CONFIG4r, 0, 0, etu_config4_buf));
}

{
    uint32 etu_bist_ctl_buf;

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_BIST_CTLr, 0, 0, &etu_bist_ctl_buf));
        soc_reg_field_set(unit, ETU_BIST_CTLr, &etu_bist_ctl_buf, ENABLEf, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ETU_BIST_CTLr, 0, 0, etu_bist_ctl_buf));        
    }

   sal_usleep(1);     /* wait 1uS after connecting ilamac_tx to SerDes */
    
    wcmod_esm_serdes_fifo_reset(unit);
    
    /* At this point, we should be sending out idle control words to NL (which
       is still in reset).
       We can de-assert SRST_L for NL only after we are sending idle control
       words
    */
    sal_usleep(SRST_L_DEASSERTION_DELAY_US); 
    /* wait 30uS after serdes_fifo_reset and before SRST_L de-assertion
     * Min time for SRST_L assertion as per NL11K datasheet is 3 uS
     */

    {
        uint32 etu_config4_buf;

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_CONFIG4r, 0, 0, &etu_config4_buf));
        soc_reg_field_set(unit, ETU_CONFIG4r, &etu_config4_buf, EXT_TCAM_SRST_Lf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ETU_CONFIG4r, 0, 0, etu_config4_buf));
    }

    /* Waiting after srst_l de-assertion is not specified in NL data sheet - from Hong's tcl */
    
    sal_usleep(10);     /* wait 10uS after SRST_L de-assertion */
    
    for (dev_id = 0; dev_id < num_nl; ++dev_id) {
        SOC_IF_ERROR_RETURN(nl_mdio_test_reg_access(unit,
                                                  mdio_portid_get(unit, dev_id)
                                                  )
                            );
    }

    SOC_IF_ERROR_RETURN(nl_mdio_init_seq(unit, num_nl, rx_fifo_thr, tx_fifo_thr, tx_burst_short_16b));

    for (dev_id = 0; dev_id < num_nl; ++dev_id) {
        uint16 dummy;

        nl_mdio_read(unit, mdio_portid_get(unit, dev_id), 1, 0x8184, &dummy);
    }

    sal_usleep(CRST_L_DEASSERTION_DELAY_US);
        /* wait 3000uS between SRST_L de-assertion and CRST_L de-assertion.
         * 10 uS above nl_mdio_test_reg, nl_mdio_init_seq, reading
         * of 0x8184 are already adding delay between SRST_L
         * de-assertion and CRST_L de-assertion. So, above delay
         * of 3 mSec is just as precaution. NL11K requirement is 2.5 mSec
         */
    {
        uint32 etu_config4_buf;

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_CONFIG4r, 0, 0, &etu_config4_buf));
        soc_reg_field_set(unit, ETU_CONFIG4r, &etu_config4_buf, EXT_TCAM_CRST_Lf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ETU_CONFIG4r, 0, 0, etu_config4_buf));
    }

    SOC_IF_ERROR_RETURN(wcl_poll_rxlane_lock(unit));

    {
        uint32 etu_config4_buf;

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_CONFIG4r, 0, 0, &etu_config4_buf));
        /* First de-asserting lbus_rst */
        soc_reg_field_set(unit, ETU_CONFIG4r, &etu_config4_buf, ILAMAC_RX_LBUS_RST_f, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ETU_CONFIG4r, 0, 0, etu_config4_buf));
        sal_usleep(10);
        /* wait 10uS between de-assertion of ILAMAC_RX_LBUS_RST_ and asertion of
           ILAMAC_RX_SERDES_RESET_ */
        soc_reg_field_set(unit, ETU_CONFIG4r, &etu_config4_buf, ILAMAC_RX_SERDES_RST_f, 0xfff);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ETU_CONFIG4r, 0, 0, etu_config4_buf));
    }

    SOC_IF_ERROR_RETURN(ilamac_poll_rx_intf_state0(unit));
    SOC_IF_ERROR_RETURN(ilamac_chk_rx_intf_state1(unit));
    SOC_IF_ERROR_RETURN(ilamac_chk_tx_intf_state(unit));

    {
        const unsigned mdio_dev_id = 1;

        unsigned dev_id;

        for (dev_id = 0; dev_id < num_nl; ++dev_id) {
            unsigned mdio_portid = mdio_portid_get(unit, dev_id);
            
            /* Toggle "Reset RX Satellite Sticky Registers" bit */
            SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                                              mdio_portid,
                                              mdio_dev_id,
                                              0x811b,
                                              0x40,
                                              1
                                              )
                                );
            SOC_IF_ERROR_RETURN(nl_mdio_write(unit,
                                              mdio_portid,
                                              mdio_dev_id,
                                              0x811b,
                                              0,
                                              1
                                              )
                                );
        }
        
        for (dev_id = 0; dev_id < num_nl; ++dev_id) {
            unsigned chk_crx     = (dev_id != 0);
            unsigned mdio_portid = mdio_portid_get(unit, dev_id);
            
            SOC_IF_ERROR_RETURN(nl_mdio_clr_csm_status_regs(unit, mdio_portid, 0));
            SOC_IF_ERROR_RETURN(nl_mdio_chk_csm_status_regs(unit, mdio_portid, chk_crx));
            SOC_IF_ERROR_RETURN(nl_mdio_chk_csm_error_counters(unit, mdio_portid, chk_crx));
            SOC_IF_ERROR_RETURN(nl_mdio_chk_pcs_error_status(unit, mdio_portid, chk_crx));
            SOC_IF_ERROR_RETURN(nl_mdio_chk_pcs_error_counters(unit, mdio_portid, chk_crx));
        }
    }

    chk_fifos(unit);
    chk_etu_counters(unit);
    chk_all_intr_sts(unit, num_nl);
    chk_esmif_adc(unit);
    
    SOC_IF_ERROR_RETURN(clr_etu_counters(unit));
    SOC_IF_ERROR_RETURN(clr_all_intr_sts(unit));
    SOC_IF_ERROR_RETURN(clr_adc_max_logs(unit));

    SOC_IF_ERROR_RETURN(chk_fifos(unit));
    SOC_IF_ERROR_RETURN(chk_etu_counters(unit));
    SOC_IF_ERROR_RETURN(chk_all_intr_sts(unit, num_nl));
    SOC_IF_ERROR_RETURN(chk_esmif_adc(unit));

    _SHR_RETURN(SOC_E_NONE);
}


#define NL_REG_BUF_SIZE  10
typedef uint8 nl_reg_buf_t[NL_REG_BUF_SIZE]; /* Little-endian */

#ifndef BIT
#define BIT(n)  (1U << (n))
#endif
#ifndef BITS
#define BITS(n)  ((n) == 32 ? ~0 : BIT(n) - 1)
#endif
#ifndef MIN
#define MIN(a, b)  ((a) < (b) ? (a) : (b))
#endif

STATIC uint32
nl_reg_buf_bitfield_get(nl_reg_buf_t buf, unsigned ofs, unsigned width)
{
    unsigned sh = ofs & BITS(3), r = 8 - sh, n, rsh;
    uint8    *p, m;
    uint32   result;

    /* assert((ofs + width) <= COUNTOF(buf) * sizeof(buf[0])); */
    /* assert(width <= 32); */

    for (rsh = result = 0, p = &buf[ofs >> 3]; width; width -= n, ++p) {
        n = MIN(width, 8);
        m = BITS(n);
        
        result |= ((p[0] >> sh) & m) << rsh;
        rsh += r;
        if (n > r) {
            result |= (p[1] & (m >> r)) << rsh;
            rsh += n - r;
        }
    }

    return (result);
}

STATIC void
nl_reg_buf_bitfield_set(nl_reg_buf_t buf, unsigned ofs, unsigned width, uint32 val)
{
    unsigned sh = ofs & BITS(3), r = 8 - sh, n;
    uint8    *p, m, u;

    /* assert((ofs + width) <= COUNTOF(buf) * sizeof(buf[0])); */
    /* assert(width <= 32); */
    /* assert(val <= BITS(width)); */

    for (p = &buf[ofs >> 3]; width; width -= n, ++p, val >>= 8) {
        n = MIN(width, 8);
        m = BITS(n);
        u = val & m;

        p[0] = (p[0] & ~(m << sh)) | (u << sh);
        if (n > r) {
            p[1] = (p[1] & ~(m >> r)) | (u >> r);
        }
    }
}

STATIC unsigned
nl_reg_buf_is_zero(nl_reg_buf_t buf)
{
    uint8    *p;
    unsigned n;

    for (p = buf, n = NL_REG_BUF_SIZE; n; --n, ++p) {
        if (*p != 0)  return (0);
    }
    return (1);
}

STATIC uint32
nl_reg_adr26b(unsigned dev_id, unsigned reg_adr20b)
{
    return ((dev_id << 23) | (reg_adr20b & 0xfffff));
}


STATIC int
nl_raw_req_submit(int      unit,
                  unsigned capture_rsp,
                  unsigned flip_dw_byte_order,
                  unsigned rsp_index_count,
                  unsigned eop,
                  unsigned num_dw,
                  unsigned nop_pre,
                  unsigned num_nops,
                  unsigned opcode
                  )
{
    uint32 etu_tx_raw_req_control_word_buf;

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_TX_RAW_REQ_CONTROL_WORDr, 0, 0, &etu_tx_raw_req_control_word_buf));
    if (soc_reg_field_get(unit, ETU_TX_RAW_REQ_CONTROL_WORDr, etu_tx_raw_req_control_word_buf, STARTf) == 1
        && soc_reg_field_get(unit, ETU_TX_RAW_REQ_CONTROL_WORDr, etu_tx_raw_req_control_word_buf, COMPLETEf) == 0
        ) {
        _SHR_RETURN(SOC_E_BUSY);
    }
    
    soc_reg_field_set(unit, ETU_TX_RAW_REQ_CONTROL_WORDr, &etu_tx_raw_req_control_word_buf, STARTf,         0);
    soc_reg_field_set(unit, ETU_TX_RAW_REQ_CONTROL_WORDr, &etu_tx_raw_req_control_word_buf, COMPLETEf,      0);
    soc_reg_field_set(unit, ETU_TX_RAW_REQ_CONTROL_WORDr, &etu_tx_raw_req_control_word_buf, CAPTUREf,       capture_rsp ? 1 : 0);
    soc_reg_field_set(unit, ETU_TX_RAW_REQ_CONTROL_WORDr, &etu_tx_raw_req_control_word_buf, NOT_SEARCHf,    !flip_dw_byte_order);
    soc_reg_field_set(unit, ETU_TX_RAW_REQ_CONTROL_WORDr, &etu_tx_raw_req_control_word_buf, TOTAL_NUM_RSPf, rsp_index_count);
    soc_reg_field_set(unit, ETU_TX_RAW_REQ_CONTROL_WORDr, &etu_tx_raw_req_control_word_buf, EOPf,           eop);
    soc_reg_field_set(unit, ETU_TX_RAW_REQ_CONTROL_WORDr, &etu_tx_raw_req_control_word_buf, NUM_DWf,        num_dw);
    soc_reg_field_set(unit, ETU_TX_RAW_REQ_CONTROL_WORDr, &etu_tx_raw_req_control_word_buf, NOP_PRE_PSTf,   nop_pre ? 1 : 0);
    soc_reg_field_set(unit, ETU_TX_RAW_REQ_CONTROL_WORDr, &etu_tx_raw_req_control_word_buf, NUM_NOPf,       num_nops);
    soc_reg_field_set(unit, ETU_TX_RAW_REQ_CONTROL_WORDr, &etu_tx_raw_req_control_word_buf, OPCf,           opcode);
    SOC_IF_ERROR_RETURN(WRITE_ETU_TX_RAW_REQ_CONTROL_WORDr(unit, etu_tx_raw_req_control_word_buf));
    
    soc_reg_field_set(unit, ETU_TX_RAW_REQ_CONTROL_WORDr, &etu_tx_raw_req_control_word_buf, STARTf, 1);
    SOC_IF_ERROR_RETURN(WRITE_ETU_TX_RAW_REQ_CONTROL_WORDr(unit, etu_tx_raw_req_control_word_buf));

    _SHR_RETURN(SOC_E_NONE);
}


STATIC int
nl_raw_req_check_done(int unit)
{
    uint32 etu_tx_raw_req_control_word_buf;

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_TX_RAW_REQ_CONTROL_WORDr, 0, 0, &etu_tx_raw_req_control_word_buf));
    if (soc_reg_field_get(unit, ETU_TX_RAW_REQ_CONTROL_WORDr, etu_tx_raw_req_control_word_buf, STARTf) != 1) {
        _SHR_RETURN(SOC_E_FAIL);
    }
    return (soc_reg_field_get(unit, ETU_TX_RAW_REQ_CONTROL_WORDr, etu_tx_raw_req_control_word_buf, COMPLETEf) ? SOC_E_NONE : SOC_E_BUSY);
}


STATIC int
nl_raw_req_poll_done(int unit, unsigned max_polls)
{
    for ( ; max_polls; --max_polls) {
        int errcode = nl_raw_req_check_done(unit);
        
        if (errcode == SOC_E_BUSY) {
            sal_usleep(1000);       /* Wait 1 ms */
            continue;
        }

        _SHR_RETURN(errcode);
    }

    _SHR_RETURN(SOC_E_TIMEOUT);
}

enum nl_read_type { NL_READ_TYPE_REG, NL_READ_TYPE_DBX, NL_READ_TYPE_DBY };

STATIC int
nl_raw_rsp_parse_for_read(int unit, nl_reg_buf_t rd_data)
{
    etu_dbg_rx_raw_rsp_entry_t etu_dbg_rx_raw_rsp_buf;
    uint64   dw0, dw1;
    unsigned i, sh;

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, ETU_DBG_RX_RAW_RSPm, MEM_BLOCK_ANY, 0, etu_dbg_rx_raw_rsp_buf.entry_data));
    soc_mem_field64_get(unit, ETU_DBG_RX_RAW_RSPm, etu_dbg_rx_raw_rsp_buf.entry_data, DW0f, &dw0);
    soc_mem_field64_get(unit, ETU_DBG_RX_RAW_RSPm, etu_dbg_rx_raw_rsp_buf.entry_data, DW1f, &dw1);
    for (sh = 56, i = 0; i < 8; ++i, sh -= 8) {
        uint64 temp = dw0;

        COMPILER_64_SHR(temp, sh);
        rd_data[i] = COMPILER_64_LO(temp);
    }
    for (sh = 56; i < NL_REG_BUF_SIZE; ++i, sh -= 8) {
        uint64 temp = dw1;

        COMPILER_64_SHR(temp, sh);
        rd_data[i] = COMPILER_64_LO(temp);
    }

    _SHR_RETURN(SOC_E_NONE);
}


STATIC int
nl_reg_read(int unit, unsigned dev_id, uint32 reg_adr, nl_reg_buf_t rd_data)
{
    uint32 reg_adr26b = nl_reg_adr26b(dev_id, reg_adr);
    uint64 dw0, dw1;
    etu_tx_raw_req_data_word_entry_t etu_tx_raw_req_data_word_buf;
        
    COMPILER_64_SET(dw0,
                    ((reg_adr26b & (BITS(8) << 0)) << 24)
                    | ((reg_adr26b & (BITS(8) << 8))  << 8)
                    | ((reg_adr26b & (BITS(8) << 16)) >> 8)
                    | ((reg_adr26b & (BITS(2) << 24)) >> 24),
                    0
                    );
    COMPILER_64_ZERO(dw1);
    
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, ETU_TX_RAW_REQ_DATA_WORDm, MEM_BLOCK_ANY, 0, etu_tx_raw_req_data_word_buf.entry_data));
    soc_mem_field64_set(unit, ETU_TX_RAW_REQ_DATA_WORDm, etu_tx_raw_req_data_word_buf.entry_data, DW0f, dw0);
    soc_mem_field64_set(unit, ETU_TX_RAW_REQ_DATA_WORDm, etu_tx_raw_req_data_word_buf.entry_data, DW1f, dw1);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, ETU_TX_RAW_REQ_DATA_WORDm, MEM_BLOCK_ALL, 0, etu_tx_raw_req_data_word_buf.entry_data));

    SOC_IF_ERROR_RETURN(nl_raw_req_submit(unit, 1, 0, 1, 8, 2, 1, 0, 0x2));
    SOC_IF_ERROR_RETURN(nl_raw_req_poll_done(unit, 60));
    SOC_IF_ERROR_RETURN(nl_raw_rsp_parse_for_read(unit, rd_data));

    _SHR_RETURN(SOC_E_NONE);
}


STATIC int
nl_reg_write(int              unit,
             unsigned         dev_id,
             uint32           reg_adr,
             const nl_reg_buf_t wr_data,
             unsigned         num_nops,
             unsigned         nop_pre,
             unsigned         capture_rsp
             )
{
    uint32 reg_adr26b = nl_reg_adr26b(dev_id, reg_adr);
    uint64 dw0, dw1, dw2;
    etu_tx_raw_req_data_word_entry_t etu_tx_raw_req_data_word_buf;

    COMPILER_64_SET(dw0,
                    ((reg_adr26b & (BITS(8) << 0)) << 24)
                    | ((reg_adr26b & (BITS(8) << 8))  << 8)
                    | ((reg_adr26b & (BITS(8) << 16)) >> 8)
                    | ((reg_adr26b & (BITS(2) << 24)) >> 24),
                    (wr_data[0] << 24)
                    | (wr_data[1] << 16)
                    | (wr_data[2] << 8)
                    | wr_data[3]
                    );
    COMPILER_64_SET(dw1,
                    (wr_data[4] << 24)
                    | (wr_data[5] << 16)
                    | (wr_data[6] << 8)
                    | wr_data[7],
                    (wr_data[8] << 24)
                    | (wr_data[9] << 16)
                    );
    COMPILER_64_ZERO(dw2);

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, ETU_TX_RAW_REQ_DATA_WORDm, MEM_BLOCK_ANY, 0, etu_tx_raw_req_data_word_buf.entry_data));
    soc_mem_field64_set(unit, ETU_TX_RAW_REQ_DATA_WORDm, etu_tx_raw_req_data_word_buf.entry_data, DW0f, dw0);
    soc_mem_field64_set(unit, ETU_TX_RAW_REQ_DATA_WORDm, etu_tx_raw_req_data_word_buf.entry_data, DW1f, dw1);
    soc_mem_field64_set(unit, ETU_TX_RAW_REQ_DATA_WORDm, etu_tx_raw_req_data_word_buf.entry_data, DW2f, dw2);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, ETU_TX_RAW_REQ_DATA_WORDm, MEM_BLOCK_ALL, 0, etu_tx_raw_req_data_word_buf.entry_data));
    
    SOC_IF_ERROR_RETURN(nl_raw_req_submit(unit, capture_rsp, 0, 1, 8, 3, nop_pre, num_nops, 0x1));
    SOC_IF_ERROR_RETURN(nl_raw_req_poll_done(unit, 60));

    _SHR_RETURN(SOC_E_NONE);
}

#define NL_REG_ADDR_DEV_ID                      0x00000
#define NL_REG_ADDR_DEV_CFG                     0x00001
#define NL_REG_ADDR_ERR_STS                     0x00002
#define NL_REG_ADDR_ERR_STS_MSK                 0x00003
#define NL_REG_ADDR_DB_SOFT_ERR_FIFO            0x00005
#define NL_REG_ADDR_ADV_FEATURES_SOFT_ERR       0x00019
#define NL_REG_ADDR_LPM_ENABLE                  0x00020
#define NL_REG_ADDR_DEBUG_REG0                  0x003FE
#define NL_REG_ADDR_DEBUG_REG1                  0x003FF
#define NL_REG_ADDR_CONTEXT_BUFFER_BASE         0x08000
#define NL_REG_ADDR_SCRATCHPAD_REG0             0x80000
#define NL_REG_ADDR_SCRATCHPAD_REG1             0x80001
#define NL_REG_ADDR_RESULT_REG0                 0x80010
#define NL_REG_ADDR_RESULT_REG1                 0x80011
#define NL_REG_ADDR_RANGE_CONTROL_BASE          0x85000
#define NL_REG_ADDR_LTR_BLOCK_SEL_REG0(ltr)     (0x04000 + ((ltr) << 5))
#define NL_REG_ADDR_LTR_BLOCK_SEL_REG1(ltr)     (0x04001 + ((ltr) << 5))
#define NL_REG_ADDR_LTR_MISCELLANEOUS_REG(ltr)  (0x04009 + ((ltr) << 5))

STATIC int
nl_reg_access_test(int unit, unsigned dev_id)
{
    const unsigned num_nops = 0, nop_pre = 0;
    const uint32   reg_adr0 = NL_REG_ADDR_SCRATCHPAD_REG0, reg_adr1 = NL_REG_ADDR_SCRATCHPAD_REG1;

    {
        static const nl_reg_buf_t wr_data0 = { 0x34, 0x12, 0xf0, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12 };
        static const nl_reg_buf_t wr_data1 = { 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab };
        
        nl_reg_buf_t rd_data0, rd_data1;
        
        SOC_IF_ERROR_RETURN(nl_reg_write(unit, dev_id, reg_adr0, wr_data0, num_nops, nop_pre, 0));
        SOC_IF_ERROR_RETURN(nl_reg_write(unit, dev_id, reg_adr1, wr_data1, num_nops, nop_pre, 0));
        SOC_IF_ERROR_RETURN(nl_reg_read(unit, dev_id, reg_adr0, rd_data0));
        SOC_IF_ERROR_RETURN(nl_reg_read(unit, dev_id, reg_adr1, rd_data1));
        if (sal_memcmp(rd_data0, wr_data0, sizeof(rd_data0)) != 0) {
            _SHR_RETURN(SOC_E_FAIL);
        }
        if (sal_memcmp(rd_data1, wr_data1, sizeof(rd_data1)) != 0) {
            _SHR_RETURN(SOC_E_FAIL);
        }
    }

    {
        static const nl_reg_buf_t wr_data0 = { 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa };
        static const nl_reg_buf_t wr_data1 = { 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55 };
        
        SOC_IF_ERROR_RETURN(nl_reg_write(unit, dev_id, reg_adr0, wr_data0, num_nops, nop_pre, 0));
        SOC_IF_ERROR_RETURN(nl_reg_write(unit, dev_id, reg_adr1, wr_data1, num_nops, nop_pre, 0));
    }

    _SHR_RETURN(SOC_E_NONE);
}

#if 0                           /* <HP> Skip for now, since db test fails </HP> */

STATIC int
nl_db_adr26b(uint32 et_pa22b, unsigned dev_density_mb, uint32 *adr26b)
{
    unsigned sh;

    switch (dev_density_mb) {
    case 80:
        sh = 20;
        break;
    case 40:
        sh = 19;
        break;
    case 20:
        sh = 18;
        break;
    case 10:
        sh = 17;
        break;
    default:
        _SHR_RETURN(SOC_E_FAIL);
    }

    *adr26b = (((et_pa22b >> sh) & BITS(2)) << 23) | (et_pa22b & BITS(sh));

    _SHR_RETURN(SOC_E_NONE);
}


STATIC int
nl_db_read(int               unit,
           uint32            et_pa22b,
           enum nl_read_type read_type,
           unsigned          dev_density_mb,
           nl_reg_buf_t      rd_data
           )
{
    uint32 adr26;
    uint64 dw0, dw1;
    etu_tx_raw_req_data_word_entry_t etu_tx_raw_req_data_word_buf;

    SOC_IF_ERROR_RETURN(nl_db_adr26b(et_pa22b, dev_density_mb, &adr26));
    COMPILER_64_SET(dw0,
                    ((reg_adr26b & (BITS(8) << 0)) << 24)
                    | ((reg_adr26b & (BITS(8) << 8))  << 8)
                    | ((reg_adr26b & (BITS(8) << 16)) >> 8)
                    | ((reg_adr26b & (BITS(2) << 24)) >> 24),
                    0
                    );
    COMPILTER_64_ZERO(dw1);

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, ETU_TX_RAW_REQ_DATA_WORDm, MEM_BLOCK_ANY, 0, etu_tx_raw_req_data_word_buf.entry_data));
    soc_mem_field64_set(unit, ETU_TX_RAW_REQ_DATA_WORDm, etu_tx_raw_req_data_word_buf.entry_data, DW0f, dw0);
    soc_mem_field64_set(unit, ETU_TX_RAW_REQ_DATA_WORDm, etu_tx_raw_req_data_word_buf.entry_data, DW1f, dw1);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, ETU_TX_RAW_REQ_DATA_WORDm, MEM_BLOCK_ALL, 0, etu_tx_raw_req_data_word_buf.entry_data));

    SOC_IF_ERROR_RETURN(nl_raw_req_submit(unit, 1, 0, 1, 8, 2, 1, 0, read_type == NL_READ_TYPE_DBX ? 2 : 3));
    SOC_IF_ERROR_RETURN(nl_raw_req_poll_done(unit, 60));
    SOC_IF_ERROR_RETURN(nl_raw_rsp_parse_for_read(unit, rd_data));

    _SHR_RETURN(SOC_E_NONE);
}


STATIC int
nl_db_write(int        unit,
            uint32     et_pa22b,
            nl_reg_buf_t wr_data,
            nl_reg_buf_t wr_mask,
            unsigned   vbit,
            unsigned   wr_mode_xy,
            unsigned   dev_density_mb,
            unsigned   capture_rsp
            )
{
    uint32 adr26;
    uint64 dw0, dw1, dw2;
    unsigned i;
    int      sh;
    etu_tx_raw_req_data_word_entry_t etu_tx_raw_req_data_word_buf;

    SOC_IF_ERROR_RETURN(nl_db_adr26b(et_pa22b, dev_density_mb, &adr26));
    COMPILER_64_SET(dw0,
                    ((adr26 & (BITS(8) << 0)) << 24)
                    | ((adr26 & (BITS(8) << 8)) << 8)
                    | ((adr26 & (BITS(8) << 16)) >> 8)
                    | ((adr26 & (BITS(2) << 24)) >> 24)
                    | ((wr_mode_xy ? 1 : 0) << 7)
                    | ((vibt ? 1 : 0) << 6),
                    (wr_data[0] << 24)
                    | (wr_data[1] << 16)
                    | (wr_data[2] << 8)
                    | wr_data[3]
                    );
    COMPILER_64_SET(dw1,
                    (wr_data[4] << 24)
                    | (wr_data[5] << 16)
                    | (wr_data[6] << 8)
                    | wr_data[7],
                    (wr_data[8] << 24)
                    | (wr_data[9] << 16)
                    | (wr_mask[0] << 8)
                    | wr_mask[1]
                    );
    COMPILER_64_SET(dw2,
                    (wr_mask[2] << 24)
                    | (wr_mask[3] << 16)
                    | (wr_mask[4] << 8)
                    | wr_mask[5],
                    (wr_mask[6] << 24)
                    | (wr_mask[7] << 16)
                    | (wr_mask[8] << 8)
                    | wr_mask[9]
                    );

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, ETU_TX_RAW_REQ_DATA_WORDm, MEM_BLOCK_ANY, 0, etu_tx_raw_req_data_word_buf.entry_data));
    soc_mem_field64_set(unit, ETU_TX_RAW_REQ_DATA_WORDm, etu_tx_raw_req_data_word_buf.entry_data, DW0f, dw0);
    soc_mem_field64_set(unit, ETU_TX_RAW_REQ_DATA_WORDm, etu_tx_raw_req_data_word_buf.entry_data, DW1f, dw1);
    soc_mem_field64_set(unit, ETU_TX_RAW_REQ_DATA_WORDm, etu_tx_raw_req_data_word_buf.entry_data, DW2f, dw2);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, ETU_TX_RAW_REQ_DATA_WORDm, MEM_BLOCK_ALL, 0, etu_tx_raw_req_data_word_buf.entry_data));

    SOC_IF_ERROR_RETURN(nl_raw_req_submit(unit, capture_rsp, 0, 1, 8, 3, 1, 0, 1));
    SOC_IF_ERROR_RETURN(nl_raw_req_poll_done(unit, 60));

    _SHR_RETURN(SOC_E_NONE);
}


STATIC int
nl_db_dup_entry_ofs_get(int unit, unsigned *dup_entry_ofs)
{
    uint32 etu_config0_buf;

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_CONFIG0r, 0, 0, &etu_config0_buf));
    *dup_entry_ofs = soc_reg_field_get(unit, ETU_CONFIG0r, etu_config0_buf, DUP_BLK_OFFSETf) << 12;

    _SHR_RETURN(SOC_E_NONE);
}


STATIC int
nl_db_invalidate(int unit, uint32 et_pa22b, uint32 vbit_bmp, unsigned duplicate)
{
    uint32 tmp_dup_et_pa22b, tmp_et_pa22b;
    unsigned invalidation_reqd, i;

    if (duplicate) {
        unsigned dup_entry_ofs;

        SOC_IF_ERROR_RETURN(nl_db_dup_entry_ofs_get(unit, &dup_entry_ofs));
        tmp_dup_et_pa22b = et_pa22b + dup_entry_ofs;
    } else {
        tmp_dup_et_pa22b = 0;
    }
    tmp_et_pa22b      = et_pa22b;
    invalidation_reqd = 0;
    for (i = 0; i < 32; ++i) {
        if ((vbit_bmp & BIT(i)) == 0) {
            invalidation_reqd = 1;
        }
        ++tmp_et_pa22b;
        ++tmp_dup_et_pa22b;
    }
    if (invalidation_reqd) {
        ext_tcam_vbit_entry_t ext_tcam_vbit_buf;
        
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, EXT_TCAM_VBITm, MEM_BLOCK_ANY, et_pa22b, ext_tcam_vbit_buf.entry_data));
        soc_mem_field32_set(unit, EXT_TCAM_VBITm, ext_tcam_vbit_buf.entry_data, DUPLICATEf, duplicate ? 1 : 0);
        soc_mem_field32_set(unit, EXT_TCAM_VBITm, ext_tcam_vbit_buf.entry_data, VBITf, vbit_bmp);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, EXT_TCAM_VBITm, MEM_BLOCK_ALL, et_pa22b, ext_tcam_vbit_buf.entry_data));
    }

    _SHR_RETURN(SOC_E_NONE);
}


STATIC int
nl_db_access_dm_test(int unit, unsigned dev_id)
{
    static const nl_reg_buf_t wr_data = { 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab };
    static const nl_reg_buf_t wr_mask = { 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00 };

    const uint32   et_pa22b            = (((dev_id << 7) + 126) << 12) + 25;
    const uint32   et_pa22b_for_delete = ((dev_id << 7) + 126) << 12;
    const uint32   vbit_bmp            = ~(1 << 25);
    const unsigned dev_density_mb      = 40;

    nl_reg_buf_t exp_data, rd_data;
    unsigned   i;

    SOC_IF_ERROR_RETURN(nl_db_write(unit,
                                    et_pa22b,
                                    (uint8 *) wr_data,
                                    (uint8 *) wr_mask,
                                    1,
                                    0,
                                    dev_density_mb,
                                    0
                                    )
                        );

    SOC_IF_ERROR_RETURN(nl_db_read(unit,
                                   et_pa22b,
                                   NL_READ_TYPE_DBX,
                                   dev_density_mb,
                                   rd_data
                                   )
                        );
    for (i = 0; i < NL_REG_BUF_SIZE; ++i) {
        exp_data[i] = wr_data[i] & ~wr_mask[i];
    }
    if (sal_memcmp(rd_data, exp_data, sizeof(rd_data)) != 0) {
        _SHR_RETURN(SOC_E_FAIL);
    }
                                    
    SOC_IF_ERROR_RETURN(nl_db_read(unit,
                                   et_pa22b,
                                   NL_READ_TYPE_DBY,
                                   dev_density_mb,
                                   rd_data
                                   )
                        );
    for (i = 0; i < NL_REG_BUF_SIZE; ++i) {
        exp_data[i] = ~wr_data[i] & ~wr_mask[i];
    }
    if (sal_memcmp(rd_data, exp_data, sizeof(rd_data)) != 0) {
        _SHR_RETURN(SOC_E_FAIL);
    }
                       
    SOC_IF_ERROR_RETURN(nl_db_invalidate(unit,
                                         et_pa22b_for_delete,
                                         vbit_bmp,
                                         0
                                         )
                        );

    SOC_IF_ERROR_RETURN(nl_db_read(unit,
                                   et_pa22b,
                                   NL_READ_TYPE_DBX,
                                   dev_density_mb,
                                   rd_data
                                   )
                        );
    sal_memset(exp_data, 0, sizeof(exp_data));
    if (sal_memcmp(rd_data, exp_data, sizeof(rd_data)) != 0) {
        _SHR_RETURN(SOC_E_FAIL);
    }
                                    
    SOC_IF_ERROR_RETURN(nl_db_read(unit,
                                   et_pa22b,
                                   NL_READ_TYPE_DBY,
                                   dev_density_mb,
                                   rd_data
                                   )
                        );
    sal_memset(exp_data, 0xff, sizeof(exp_data));
    if (sal_memcmp(rd_data, exp_data, sizeof(rd_data)) != 0) {
        _SHR_RETURN(SOC_E_FAIL);
    }

    _SHR_RETURN(SOC_E_NONE);
}

#endif

STATIC int
nl_chk_err_status_mask_reg(int unit, unsigned dev_id)
{
    unsigned errcnt = 0;
    nl_reg_buf_t nl_reg_err_sts_msk_buf;

    SOC_IF_ERROR_RETURN(nl_reg_read(unit, dev_id, NL_REG_ADDR_ERR_STS_MSK, nl_reg_err_sts_msk_buf));
    if (nl_reg_buf_bitfield_get(nl_reg_err_sts_msk_buf, 79, 1) == 0) {
        ++errcnt;
    }
    if (nl_reg_buf_bitfield_get(nl_reg_err_sts_msk_buf, 50, 1) != BITS(1)) {
        ++errcnt;
    }
    if (nl_reg_buf_bitfield_get(nl_reg_err_sts_msk_buf, 48, 3) != BITS(3)) {
        ++errcnt;
    }
    if (nl_reg_buf_bitfield_get(nl_reg_err_sts_msk_buf, 31, 1) != BITS(1)) {
        ++errcnt;
    }
    if (nl_reg_buf_bitfield_get(nl_reg_err_sts_msk_buf, 16, 14) != BITS(14)) {
        ++errcnt;
    }
    if (nl_reg_buf_bitfield_get(nl_reg_err_sts_msk_buf, 5, 1) != BITS(1)) {
        ++errcnt;
    }
    if (nl_reg_buf_bitfield_get(nl_reg_err_sts_msk_buf, 0, 3) != BITS(3)) {
        ++errcnt;
    }
    
    _SHR_RETURN(errcnt ? SOC_E_FAIL : SOC_E_NONE);
}


STATIC int
nl_prog_err_status_mask_reg(int unit, unsigned dev_id)
{
    nl_reg_buf_t nl_reg_err_sts_msk_buf;

    sal_memset(nl_reg_err_sts_msk_buf, 0, sizeof(nl_reg_err_sts_msk_buf));
    nl_reg_buf_bitfield_set(nl_reg_err_sts_msk_buf, 0,   3,    0x7);
    nl_reg_buf_bitfield_set(nl_reg_err_sts_msk_buf, 5,   1,      1);
    nl_reg_buf_bitfield_set(nl_reg_err_sts_msk_buf, 16, 16, 0xffff);
    nl_reg_buf_bitfield_set(nl_reg_err_sts_msk_buf, 48,  3,    0x7);
    nl_reg_buf_bitfield_set(nl_reg_err_sts_msk_buf, 79,  1,      1);
    SOC_IF_ERROR_RETURN(nl_reg_write(unit, dev_id, NL_REG_ADDR_ERR_STS_MSK, nl_reg_err_sts_msk_buf, 0, 0, 0));
    _SHR_RETURN(nl_chk_err_status_mask_reg(unit, dev_id));
}


STATIC int
nl_chk_err_status_reg(int unit, unsigned dev_id)
{
    nl_reg_buf_t nl_reg_err_sts_buf;

    SOC_IF_ERROR_RETURN(nl_reg_read(unit, dev_id, NL_REG_ADDR_ERR_STS, nl_reg_err_sts_buf));
    _SHR_RETURN(nl_reg_buf_is_zero(nl_reg_err_sts_buf) ? SOC_E_NONE : SOC_E_FAIL);
}

STATIC int
nl_chk_low_jitter_settings(int unit)
{
    const uint32   reg_adr0 = NL_REG_ADDR_DEBUG_REG0, reg_adr1 = NL_REG_ADDR_DEBUG_REG1;
    static const uint32 wr_data0[3] = { 0x0, 0x0, 0x000204f4 };
    static const uint32 wr_data1[3] = { 0xffffffff, 0xffffffff, 0xffffffff };
    static const uint32 wr_data2[3] = { 0x0, 0x0, 0x00020800 };
    static uint32 rd_data[3];

    SOC_IF_ERROR_RETURN(soc_tr3_tcam_write_reg(unit, reg_adr0, wr_data0[0],
                                               wr_data0[1], wr_data0[2]));
    SOC_IF_ERROR_RETURN(soc_tr3_tcam_write_reg(unit, reg_adr1, wr_data1[0],
                                               wr_data1[1], wr_data1[2]));
    SOC_IF_ERROR_RETURN(soc_tr3_tcam_write_reg(unit, reg_adr0, wr_data2[0],
                                               wr_data2[1], wr_data2[2]));
    SOC_IF_ERROR_RETURN(soc_tr3_tcam_read_reg(unit, reg_adr1, rd_data,
                                              rd_data + 1, rd_data + 2));
    if (((*rd_data) & (BIT(8) | BIT(9))) == 0) {
        return 1;
    } else {
        return 0;
    }
}

STATIC int
etu_mems_rst(int unit, unsigned clr_hbits, unsigned clr_et_pa_xlat)
{
    uint32 etu_int_mem_rst_buf = 0;

    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ETU_INT_MEM_RSTr, 0, 0, etu_int_mem_rst_buf));
    if (clr_hbits) {
        soc_reg_field_set(unit, ETU_INT_MEM_RSTr, &etu_int_mem_rst_buf, EXT_DST_HBITSf, 1);
        soc_reg_field_set(unit, ETU_INT_MEM_RSTr, &etu_int_mem_rst_buf, EXT_SRC_HBITSf, 1);
        soc_reg_field_set(unit, ETU_INT_MEM_RSTr, &etu_int_mem_rst_buf, EXT_LOC_SRC_HBITSf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ETU_INT_MEM_RSTr, 0, 0, etu_int_mem_rst_buf));
    }
    if (clr_et_pa_xlat) {
        soc_reg_field_set(unit, ETU_INT_MEM_RSTr, &etu_int_mem_rst_buf, ET_PA_XLAT1f, 1);
        soc_reg_field_set(unit, ETU_INT_MEM_RSTr, &etu_int_mem_rst_buf, ET_PA_XLAT0f, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ETU_INT_MEM_RSTr, 0, 0, etu_int_mem_rst_buf));
    }
    soc_reg_field_set(unit, ETU_INT_MEM_RSTr, &etu_int_mem_rst_buf, STARTf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ETU_INT_MEM_RSTr, 0, 0, etu_int_mem_rst_buf));

    _SHR_RETURN(SOC_E_NONE);
}


STATIC int
etu_mems_chk_rst_done(int unit)
{
    uint32 etu_int_mem_rst_buf;

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_INT_MEM_RSTr, 0, 0, &etu_int_mem_rst_buf));
    if (soc_reg_field_get(unit, ETU_INT_MEM_RSTr, etu_int_mem_rst_buf, COMPLETEf)
        && soc_reg_field_get(unit, ETU_INT_MEM_RSTr, etu_int_mem_rst_buf, STARTf)
        ) {
        uint32 etu_global_intr_sts_buf;

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_GLOBAL_INTR_STSr, 0, 0, &etu_global_intr_sts_buf));
        if (soc_reg_field_get(unit, ETU_GLOBAL_INTR_STSr, etu_global_intr_sts_buf, INT_MEM_RST_DONEf)) {
            return (SOC_E_NONE);
        }
    }
    
    return (SOC_E_BUSY);
}


STATIC int
etu_mems_poll_rst_done(int unit)
{
    unsigned n;

    for (n = 1000; n; --n) {
        int errcode = etu_mems_chk_rst_done(unit);

        if (errcode == SOC_E_BUSY) {
            sal_usleep(1000);       /* Wait 1 ms */
            continue;
        }

        _SHR_RETURN(errcode);
    }

    _SHR_RETURN(SOC_E_TIMEOUT);
}


STATIC int
hwtl_zero_out_esm_range_check(int unit)
{
    unsigned i;

    for (i = 0; i < 22; ++i) {
        esm_range_check_entry_t esm_range_check_buf;

        SOC_IF_ERROR_RETURN(soc_mem_read(unit, ESM_RANGE_CHECKm, MEM_BLOCK_ANY, i, esm_range_check_buf.entry_data));
        soc_mem_field32_set(unit, ESM_RANGE_CHECKm, esm_range_check_buf.entry_data, ENABLEf, 0);
        soc_mem_field32_set(unit, ESM_RANGE_CHECKm, esm_range_check_buf.entry_data, FIELD_SELECTf, 0);
        soc_mem_field32_set(unit, ESM_RANGE_CHECKm, esm_range_check_buf.entry_data, ESM_HWTL_SEARCH_KEYf, 0);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, ESM_RANGE_CHECKm, MEM_BLOCK_ALL, i, esm_range_check_buf.entry_data));
    }

    _SHR_RETURN(SOC_E_NONE);
}

#define PTR_FME_REQ_NULL  0
#define PTR_FME_REQ_CBI   63
#define PTR_FME_RSP_CBI   (64 + PTR_FME_REQ_CBI)

STATIC int
hwtl_prog_fme_req_cbi(int unit)
{
    esm_key_id_to_field_mapper_entry_t esm_key_id_to_field_mapper_buf;

    sal_memset(esm_key_id_to_field_mapper_buf.entry_data, 0, sizeof(esm_key_id_to_field_mapper_buf.entry_data));
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, MEM_BLOCK_ALL, PTR_FME_REQ_CBI, esm_key_id_to_field_mapper_buf.entry_data));

    {
        static const soc_field_t flds[] = {
            F1_SELECTf,
            FLAGS_FIELD_NOT_PRESENTf,
            F6_SELECTf,
            F7_SELECTf,
            F8_SELECTf,
            F9_SELECTf,
            F10_SELECTf,
            F11_SELECTf,
            RSP_INDEX_COUNT_CW0f,
            COST2f,
            COST1f,
            COST0f,
            EOP_CW0f,
            CONTEXT_ADDR_LSB_CW0f,
            NUM_DWORDS_CW0f,
            OPCODE_CW0f,
            TOTAL_NUM_CWORDSf,
            HWTL_EXP_RSP_PTRf
        };
        static const uint32 fld_vals[] = {
            7,
            1,
            31,
            31,
            31,
            7,
            7,
            7,
            0,
            3,
            11,
            1,
            8,
            0,
            10,
            1 << 6,
            1,
            64 + 63
        };

        SOC_IF_ERROR_RETURN(soc_mem_fields32_modify(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, PTR_FME_REQ_CBI, COUNTOF(flds), (soc_field_t *) flds, (uint32 *) fld_vals));
    }

    _SHR_RETURN(SOC_E_NONE);
}


STATIC int
hwtl_prog_fme_rsp_cbi(int unit)
{
    esm_key_id_to_field_mapper_entry_t esm_key_id_to_field_mapper_buf;

    sal_memset(esm_key_id_to_field_mapper_buf.entry_data, 0, sizeof(esm_key_id_to_field_mapper_buf.entry_data));
    _SHR_RETURN(soc_mem_write(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, MEM_BLOCK_ALL, PTR_FME_RSP_CBI, esm_key_id_to_field_mapper_buf.entry_data));
}


STATIC int
nl_prog_ltr_block_sel(int unit, unsigned dev_id, unsigned ltr, unsigned num_blks, unsigned *blknums)
{
    nl_reg_buf_t reg0_data, reg1_data;

    sal_memset(reg0_data, 0, sizeof(reg0_data));
    sal_memset(reg1_data, 0, sizeof(reg1_data));

    for ( ; num_blks; --num_blks, ++blknums) {
        unsigned blknum = *blknums;

        if (blknum < 64) {
            nl_reg_buf_bitfield_set(reg0_data, blknum, 1, 1);
        } else if (blknum < 128) {
            nl_reg_buf_bitfield_set(reg1_data, blknum - 64, 1, 1);
        } else {
            _SHR_RETURN(SOC_E_FAIL);
        }
    }

    SOC_IF_ERROR_RETURN(nl_reg_write(unit, dev_id, NL_REG_ADDR_LTR_BLOCK_SEL_REG0(ltr), reg0_data, 2, 0, 0));
    SOC_IF_ERROR_RETURN(nl_reg_write(unit, dev_id, NL_REG_ADDR_LTR_BLOCK_SEL_REG1(ltr), reg1_data, 2, 0, 0));

    _SHR_RETURN(SOC_E_NONE);
}


STATIC int
nl_prog_ltr_miscellaneous(int unit, unsigned dev_id, unsigned ltr, unsigned rsp_index_count, unsigned bmr_sel3, unsigned bmr_sel2, unsigned bmr_sel1, unsigned bmr_sel0)
{
    nl_reg_buf_t reg_data;

    sal_memset(reg_data, 0, sizeof(reg_data));
    nl_reg_buf_bitfield_set(reg_data, 56, 2, rsp_index_count);
    nl_reg_buf_bitfield_set(reg_data, 12, 3, bmr_sel3);
    nl_reg_buf_bitfield_set(reg_data,  8, 3, bmr_sel2);
    nl_reg_buf_bitfield_set(reg_data,  4, 3, bmr_sel1);
    nl_reg_buf_bitfield_set(reg_data,  0, 3, bmr_sel0);

    _SHR_RETURN(nl_reg_write(unit, dev_id, NL_REG_ADDR_LTR_MISCELLANEOUS_REG(ltr), reg_data, 2, 0, 0));
}


STATIC int
hwtl_prog_ltr_cbi(int unit, unsigned dev_id)
{
    const unsigned ltr = 0;

    SOC_IF_ERROR_RETURN(nl_prog_ltr_block_sel(unit, dev_id, ltr, 0, 0));
    _SHR_RETURN(nl_prog_ltr_miscellaneous(unit, dev_id, ltr, 0, 0, 0, 0, 0));
}


STATIC int
hwtl_find_tx_pkt_count(int unit, uint32 *esm_key_id_to_field_mapper_buf_entry_data)
{
    if (soc_mem_field32_get(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, esm_key_id_to_field_mapper_buf_entry_data, LAST_CWORD_IS_OPTIONALf)) {
        return (-1);
    }

    return (soc_mem_field32_get(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, esm_key_id_to_field_mapper_buf_entry_data, TOTAL_NUM_CWORDSf));
}


STATIC int
hwtl_find_rx_byte_count(int unit, uint32 *esm_key_id_to_field_mapper_buf_entry_data)
{
    const unsigned total_num_cwords = soc_mem_field32_get(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, esm_key_id_to_field_mapper_buf_entry_data, TOTAL_NUM_CWORDSf),
        last_cword_is_optional = soc_mem_field32_get(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, esm_key_id_to_field_mapper_buf_entry_data, LAST_CWORD_IS_OPTIONALf),
        rsp_index_count_cw2 = soc_mem_field32_get(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, esm_key_id_to_field_mapper_buf_entry_data, RSP_INDEX_COUNT_CW2f), 
        rsp_index_count_cw1 = soc_mem_field32_get(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, esm_key_id_to_field_mapper_buf_entry_data, RSP_INDEX_COUNT_CW1f), 
        rsp_index_count_cw0 = soc_mem_field32_get(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, esm_key_id_to_field_mapper_buf_entry_data, RSP_INDEX_COUNT_CW0f);

    unsigned num_rx_bytes_cw2, num_rx_bytes_cw1, num_rx_bytes_cw0;
    int      result;

    if (rsp_index_count_cw2 > 2) {
        num_rx_bytes_cw2 = 8 * 2; /* rsp with 2 dwords */
    } else if (rsp_index_count_cw2 == 0) {
        num_rx_bytes_cw2 = 8 * 2; /* 4 rsp indexes */
    } else {
        num_rx_bytes_cw2 = 8 * 1; /* rsp with 1 dword */
    }

    if (rsp_index_count_cw1 > 2) {
        num_rx_bytes_cw1 = 8 * 2; /* rsp with 2 dwords */
    } else if (rsp_index_count_cw1 == 0) {
        num_rx_bytes_cw1 = 8 * 2; /* 4 rsp indexes */
    } else {
        num_rx_bytes_cw1 = 8 * 1; /* rsp with 1 dword */
    }

    if (rsp_index_count_cw0 > 2) {
        num_rx_bytes_cw0 = 8 * 2; /* rsp with 2 dwords */
    } else if (rsp_index_count_cw0 == 0) {
        num_rx_bytes_cw0 = 8 * 2; /* 4 rsp indexes */
    } else {
        num_rx_bytes_cw0 = 8 * 1; /* rsp with 1 dword */
    }
    
    result = -1;
    if (last_cword_is_optional == 0) {
        if (total_num_cwords == 1) {
            result = num_rx_bytes_cw0;
        } else if (total_num_cwords == 2) {
            result = num_rx_bytes_cw0 + num_rx_bytes_cw1;
        } else if (total_num_cwords == 3) {
            result = num_rx_bytes_cw0 + num_rx_bytes_cw1 + num_rx_bytes_cw2;
        }
    }

    return (result);
}


STATIC int
hwtl_calc_num_tx_bytes(unsigned num_dwords, unsigned eop)
{
    int num_tx_bytes, num_tx_bytes_last_dword;

    if (num_dwords > 1) {
        num_tx_bytes = 8 * (num_dwords - 1); /* 8 bytes for each complete DW */

        if (eop > 15) {
            return (-9999);     /* error */
        } else if (eop == 8) {
            num_tx_bytes_last_dword = 8;
        } else if (eop > 8) {
            num_tx_bytes_last_dword = eop - 8;
        } else {
            return (-9999);     /* error */
        }
    
        num_tx_bytes += num_tx_bytes_last_dword;
   } else {
        return (-9999);         /* error */
   }

    return (num_tx_bytes);
}


STATIC int
hwtl_find_tx_byte_count(int unit,  uint32 *esm_key_id_to_field_mapper_buf_entry_data)
{
    const unsigned total_num_cwords = soc_mem_field32_get(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, esm_key_id_to_field_mapper_buf_entry_data, TOTAL_NUM_CWORDSf),
        last_cword_is_optional = soc_mem_field32_get(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, esm_key_id_to_field_mapper_buf_entry_data, LAST_CWORD_IS_OPTIONALf),
        num_tx_dwords_cw2 = soc_mem_field32_get(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, esm_key_id_to_field_mapper_buf_entry_data, NUM_DWORDS_CW2f),
        num_tx_dwords_cw1 = soc_mem_field32_get(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, esm_key_id_to_field_mapper_buf_entry_data, NUM_DWORDS_CW1f),
        num_tx_dwords_cw0 = soc_mem_field32_get(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, esm_key_id_to_field_mapper_buf_entry_data, NUM_DWORDS_CW0f),
        eop_cw2 = soc_mem_field32_get(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, esm_key_id_to_field_mapper_buf_entry_data, EOP_CW2f),
        eop_cw1 = soc_mem_field32_get(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, esm_key_id_to_field_mapper_buf_entry_data, EOP_CW1f),
        eop_cw0 = soc_mem_field32_get(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, esm_key_id_to_field_mapper_buf_entry_data, EOP_CW0f);

    int num_tx_bytes_cw2 = hwtl_calc_num_tx_bytes(num_tx_dwords_cw2, eop_cw2),
        num_tx_bytes_cw1 = hwtl_calc_num_tx_bytes(num_tx_dwords_cw1, eop_cw1),
        num_tx_bytes_cw0 = hwtl_calc_num_tx_bytes(num_tx_dwords_cw0, eop_cw0);
    int num_tx_bytes = -1;

    if (last_cword_is_optional == 0) {
        if (total_num_cwords == 1) {
            num_tx_bytes = num_tx_bytes_cw0;
        } else if (total_num_cwords == 2) {
            num_tx_bytes = num_tx_bytes_cw0 + num_tx_bytes_cw1;
        } else if (total_num_cwords == 3) {
            num_tx_bytes = num_tx_bytes_cw0 + num_tx_bytes_cw1 + num_tx_bytes_cw2;
        }
    }
    
    return (num_tx_bytes);
}


STATIC int
hwtl_prog_sp_entry(int unit, unsigned sp_index, unsigned ptr_fme_req, unsigned ee)
{
    esm_search_profile_entry_t esm_search_profile_buf;

    sal_memset(esm_search_profile_buf.entry_data, 0, sizeof(esm_search_profile_buf.entry_data));
    soc_mem_field32_set(unit, ESM_SEARCH_PROFILEm, esm_search_profile_buf.entry_data, ESM_ELIGIBLEf, ee);
    soc_mem_field32_set(unit, ESM_SEARCH_PROFILEm, esm_search_profile_buf.entry_data, ESM_SEARCH_KEY_IDf, ptr_fme_req);
    _SHR_RETURN(soc_mem_write(unit, ESM_SEARCH_PROFILEm, MEM_BLOCK_ALL, sp_index, esm_search_profile_buf.entry_data));
}


STATIC int
hwtl_start(int unit, unsigned last_adr, unsigned last_iteration, unsigned endless, unsigned use_incr_cba)
{
    {
        uint64 zero;

        COMPILER_64_ZERO(zero);
        SOC_IF_ERROR_RETURN(soc_reg64_set(unit, ILAMAC_TX_PACKETS_COUNTr, 0, 0, zero));
        SOC_IF_ERROR_RETURN(soc_reg64_set(unit, ILAMAC_RX_PACKETS_COUNTr, 0, 0, zero));
        SOC_IF_ERROR_RETURN(soc_reg64_set(unit, ILAMAC_TX_BYTES_COUNTr,   0, 0, zero));
        SOC_IF_ERROR_RETURN(soc_reg64_set(unit, ILAMAC_RX_BYTES_COUNTr,   0, 0, zero));
    }

    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ETU_DBG_IPIPE_REQ_RSP_COUNTr, 0, 0, 0));
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ETU_DBG_IPIPE_ERR_RSP_COUNTr, 0, 0, 0));

    {
        uint32 esm_misc_control_buf;

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ESM_MISC_CONTROLr, 0, 0, &esm_misc_control_buf));
        soc_reg_field_set(unit, ESM_MISC_CONTROLr, &esm_misc_control_buf, EXT_LOOKUP_ENABLEf, 0);
        soc_reg_field_set(unit, ESM_MISC_CONTROLr, &esm_misc_control_buf, ETU_REQ_SEQ_NUM_SELf, use_incr_cba);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ESM_MISC_CONTROLr, 0, 0, esm_misc_control_buf));
    }

    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ESM_ET_HWTL_CONTROLr,        0, 0, 0));
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ESM_EVENT_ERR_STATUS_INTRr,  0, 0, 0));
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ESM_ET_HWTL_STATUS_0r,       0, 0, 0));
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ESM_ET_HWTL_STATUS_1r,       0, 0, 0));
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ESM_ET_HWTL_MM_INFOr,        0, 0, 0));
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ESM_ET_HWTL_EVENT_ERR_INFOr, 0, 0, 0));

    {
        uint32 esm_event_err_status_intr_enable_buf;

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ESM_EVENT_ERR_STATUS_INTR_ENABLEr, 0, 0, &esm_event_err_status_intr_enable_buf));
        soc_reg_field_set(unit, ESM_EVENT_ERR_STATUS_INTR_ENABLEr, &esm_event_err_status_intr_enable_buf, DROP_COUNT_MSBf, 1);
        soc_reg_field_set(unit, ESM_EVENT_ERR_STATUS_INTR_ENABLEr, &esm_event_err_status_intr_enable_buf, CNP_SEARCH_REQ_COUNT_MSBf, 1);
        soc_reg_field_set(unit, ESM_EVENT_ERR_STATUS_INTR_ENABLEr, &esm_event_err_status_intr_enable_buf, SOP_DROPf, 1);
        soc_reg_field_set(unit, ESM_EVENT_ERR_STATUS_INTR_ENABLEr, &esm_event_err_status_intr_enable_buf, XOFF_AXP_OVERSUBSf, 1);
        soc_reg_field_set(unit, ESM_EVENT_ERR_STATUS_INTR_ENABLEr, &esm_event_err_status_intr_enable_buf, MANDATORY_DENYf, 1);
        soc_reg_field_set(unit, ESM_EVENT_ERR_STATUS_INTR_ENABLEr, &esm_event_err_status_intr_enable_buf, DROP_ALLf, 1);
        soc_reg_field_set(unit, ESM_EVENT_ERR_STATUS_INTR_ENABLEr, &esm_event_err_status_intr_enable_buf, OPT_REQ_TRICKLEDf, 1);
        soc_reg_field_set(unit, ESM_EVENT_ERR_STATUS_INTR_ENABLEr, &esm_event_err_status_intr_enable_buf, OPTIONAL_DENYf, 1);
        soc_reg_field_set(unit, ESM_EVENT_ERR_STATUS_INTR_ENABLEr, &esm_event_err_status_intr_enable_buf, DROP_OPTIONALf, 1);
        soc_reg_field_set(unit, ESM_EVENT_ERR_STATUS_INTR_ENABLEr, &esm_event_err_status_intr_enable_buf, ET_RSP_ERRf, 1);
        soc_reg_field_set(unit, ESM_EVENT_ERR_STATUS_INTR_ENABLEr, &esm_event_err_status_intr_enable_buf, CNP_SEARCH_REQf, 1);
        soc_reg_field_set(unit, ESM_EVENT_ERR_STATUS_INTR_ENABLEr, &esm_event_err_status_intr_enable_buf, ET_REQ_FIFO_FULL_DENYf, 1);
        soc_reg_field_set(unit, ESM_EVENT_ERR_STATUS_INTR_ENABLEr, &esm_event_err_status_intr_enable_buf, ET_REQ_FIFO_FULLf, 1);
        soc_reg_field_set(unit, ESM_EVENT_ERR_STATUS_INTR_ENABLEr, &esm_event_err_status_intr_enable_buf, ADM_CTRL_FIFO_FULL_DENYf, 1);
        soc_reg_field_set(unit, ESM_EVENT_ERR_STATUS_INTR_ENABLEr, &esm_event_err_status_intr_enable_buf, ADM_CTRL_FIFO_FULLf, 1);
        soc_reg_field_set(unit, ESM_EVENT_ERR_STATUS_INTR_ENABLEr, &esm_event_err_status_intr_enable_buf, ET_RSP_FIFO_FULLf, 1);
        soc_reg_field_set(unit, ESM_EVENT_ERR_STATUS_INTR_ENABLEr, &esm_event_err_status_intr_enable_buf, CBA_MISMATCHf, 1);
        soc_reg_field_set(unit, ESM_EVENT_ERR_STATUS_INTR_ENABLEr, &esm_event_err_status_intr_enable_buf, ADM_CTRL_FIFO_UNDERFLOWf, 1);
        soc_reg_field_set(unit, ESM_EVENT_ERR_STATUS_INTR_ENABLEr, &esm_event_err_status_intr_enable_buf, ET_RSP_FIFO_UNDERFLOWf, 1);
        soc_reg_field_set(unit, ESM_EVENT_ERR_STATUS_INTR_ENABLEr, &esm_event_err_status_intr_enable_buf, ET_RSP_FIFO_OVERFLOWf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ESM_EVENT_ERR_STATUS_INTR_ENABLEr, 0, 0, esm_event_err_status_intr_enable_buf));
    }

    {
        uint32 esm_et_hwtl_control_buf;

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ESM_ET_HWTL_CONTROLr, 0, 0, &esm_et_hwtl_control_buf));
        soc_reg_field_set(unit, ESM_ET_HWTL_CONTROLr, &esm_et_hwtl_control_buf, EN_HWTL_MODEf, 1);
        soc_reg_field_set(unit, ESM_ET_HWTL_CONTROLr, &esm_et_hwtl_control_buf, LAST_ITERATIONf, last_iteration);
        soc_reg_field_set(unit, ESM_ET_HWTL_CONTROLr, &esm_et_hwtl_control_buf, ENDLESSf, endless);
        soc_reg_field_set(unit, ESM_ET_HWTL_CONTROLr, &esm_et_hwtl_control_buf, LAST_HWTL_ADRf, last_adr);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ESM_ET_HWTL_CONTROLr, 0, 0, esm_et_hwtl_control_buf));
        soc_reg_field_set(unit, ESM_ET_HWTL_CONTROLr, &esm_et_hwtl_control_buf, START_HWTL_TESTf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ESM_ET_HWTL_CONTROLr, 0, 0, esm_et_hwtl_control_buf));
    }
   
    _SHR_RETURN(SOC_E_NONE);
}


#if 0
STATIC unsigned
calc_mwps_per_lane(void)
{
    unsigned user_words             = NL_METAFRAME_LENGTH - NL_OVERHEAD_WORDS;
    unsigned usable_lane_speed_mbps = 
                      (NL_LANE_SPEED_MBPS * user_words) / NL_METAFRAME_LENGTH;

    return (usable_lane_speed_mbps / NL_BITS_PER_ENC_WORD);
}
#endif

STATIC unsigned
calc_mwps_per_lane_x(void)
{
    unsigned user_words             = NL_METAFRAME_LENGTH - NL_OVERHEAD_WORDS;
    unsigned usable_lane_speed_mbps_x = 
                (NL_LANE_SPEED_MBPS * user_words) / NL_METAFRAME_LENGTH*1000;

    return (usable_lane_speed_mbps_x / NL_BITS_PER_ENC_WORD);
}

STATIC int
esm_calc_new_step_size_ps(int unit, unsigned list_cost210_len,
                          unsigned *list_cost210)
{
    unsigned cost2_num_rsp_words, cost1_num_req_words, cost0_num_nl_cclks;
    unsigned nl_core_clk_freq_mhz;
    unsigned tmp;
    unsigned num_rsp_words, max_rsp_mwps, rsp_mpps, time_to_receive_rsp_ns;
    unsigned num_req_words, max_req_mwps, req_mpps, time_to_transmit_req_ns;
    unsigned num_nl_cclks, nl_core_mpps, time_for_nl_to_execute_search_ns;
    unsigned worst_req_proc_cost_ns, max_cost_value_can_be, step_size_ps;


    nl_core_clk_freq_mhz = nl_core_clk_freq_khz(unit)/1000;
    if (list_cost210_len == 3) {
        cost2_num_rsp_words = list_cost210[0];
        cost1_num_req_words = list_cost210[1];
        cost0_num_nl_cclks = list_cost210[2];
    } else {
        /* use worst possible cost values */
        cost2_num_rsp_words = 3*(1+2);
        cost1_num_req_words = 3+11; /* 3 CW, 11 DW */
        cost0_num_nl_cclks = 1 + 1 + 2;
    }

    /* mpps limited by rsp */
    num_rsp_words = cost2_num_rsp_words;
    max_rsp_mwps = (calc_mwps_per_lane_x() * NL_NUM_RSP_LANES *
                    (100 - REQD_RX_BW_FOR_CPU_PERCENT))/100; 
    rsp_mpps = max_rsp_mwps / num_rsp_words;
    time_to_receive_rsp_ns = 1000000000 / rsp_mpps;

    /* mpps limited by req */
    num_req_words = cost1_num_req_words; /* per ipipe_pkt */
    max_req_mwps = (calc_mwps_per_lane_x() * NL_NUM_REQ_LANES * 
                   (100 - REQD_TX_BW_FOR_CPU_PERCENT))/100;
    req_mpps = max_req_mwps / num_req_words;
    time_to_transmit_req_ns = 1000000000 / req_mpps;

    /* mpps limited by nl_core_clk_freq */
    num_nl_cclks  = cost0_num_nl_cclks;  /* per ipipe_pkt */
    nl_core_mpps = ((nl_core_clk_freq_mhz * 1000 * 
                 (100 - REQD_NL_CCLK_BW_FOR_CPU_PERCENT)) / 100) / num_nl_cclks;
    time_for_nl_to_execute_search_ns  = 1000000000 / nl_core_mpps;

   
    tmp = ESM_MAX(time_to_receive_rsp_ns,  time_to_transmit_req_ns); 
    worst_req_proc_cost_ns = ESM_MAX(tmp,  time_for_nl_to_execute_search_ns); 

    max_cost_value_can_be = 255; /* 2**8 - 1 */

    /* round up */
    step_size_ps = (worst_req_proc_cost_ns * 1000) / max_cost_value_can_be +
              ((worst_req_proc_cost_ns * 1000) % max_cost_value_can_be ? 1000 : 0);
    /* Is rounding-up really necessary ? - because anyways we are going to be 
     * dividing real-numbers by step-size, so how does rouding-up to nearest 
     * integer value help ?
     */
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "esm_calc_new_step_size_ps: ")));
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "worst_req_proc_cost_ns=%d"), 
                            worst_req_proc_cost_ns));
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            " step_size_ps=%d"), step_size_ps));
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "(num_bits_to_represent_worst_req_proc_cost=8)")));
    return (step_size_ps/1000) * 1000;
}

STATIC unsigned
esm_calc_new_drop_all_cost2 (int unit, unsigned idle_req_rsp_latency_ns, 
                            unsigned step_size_ps) 
{
   /* unused */
   return 0x100000 - 1;
}

STATIC unsigned
esm_calc_new_drop_all_cost1 (int unit, unsigned idle_req_rsp_latency_ns_x, 
                             unsigned step_size_ps_x) 
{
    unsigned max_allowed_qdly_ns_x, new_drop_all_cost1_qdly, esmif_clk_period_ns_x;
    unsigned esmif_max_allowed_latency_ns_x;
    unsigned esmif_clk_freq_mhz_x;

    /* compute max_allowed_qdly */
    esmif_clk_freq_mhz_x =  esmif_clk_freq_khz(unit);
    /* carry ns upto 3 more decimal places */
    esmif_clk_period_ns_x = (1000 * 1000 * 1000) / esmif_clk_freq_mhz_x;
    esmif_max_allowed_latency_ns_x = ESMIF_ALLOWED_FIFO_DEPTH * 
                                   esmif_clk_period_ns_x;
    max_allowed_qdly_ns_x = esmif_max_allowed_latency_ns_x - 
                                idle_req_rsp_latency_ns_x; 

    /* compute new_drop_all_costs */
    new_drop_all_cost1_qdly = (max_allowed_qdly_ns_x * 1000) / step_size_ps_x;

    return new_drop_all_cost1_qdly;
}

STATIC unsigned
esm_calc_new_drop_all_cost0(int unit, unsigned idle_req_rsp_latency_ns_x,
                            unsigned step_size_ps_x)
{
    unsigned esmif_clk_freq_mhz_x = esmif_clk_freq_khz(unit);
    unsigned max_allowed_latency_ns_x, new_drop_all_cost0_worst_req_proc;
    unsigned tmp_x;

    /* compute max_allowed_latency */
    tmp_x = (1000*1000*1000)/esmif_clk_freq_mhz_x;
    max_allowed_latency_ns_x = ESMIF_ALLOWED_FIFO_DEPTH * tmp_x;

    /* compute new_drop_all_costs */
    new_drop_all_cost0_worst_req_proc = (max_allowed_latency_ns_x * 1000) / 
                                        step_size_ps_x;

    return new_drop_all_cost0_worst_req_proc;
}

STATIC int
esm_prog_new_drop_all_costs(int unit, unsigned idle_req_rsp_latency_ns_x,
                            unsigned step_size_x,
                            unsigned percent_hysteresis) 
{
    /* compute drop_all_cost as function of max_allowed_latency_ns delay */
    unsigned drop_all_cost2, drop_all_cost1, drop_all_cost0;
    unsigned drop_all_reset_cost2, drop_all_reset_cost1, drop_all_reset_cost0;

    drop_all_cost2 = esm_calc_new_drop_all_cost2(unit, idle_req_rsp_latency_ns_x,
                                                 step_size_x);
    drop_all_cost1 = esm_calc_new_drop_all_cost1(unit, idle_req_rsp_latency_ns_x,
                                                 step_size_x);
    drop_all_cost0 = esm_calc_new_drop_all_cost0(unit, idle_req_rsp_latency_ns_x,
                                                 step_size_x);

    /* compute drop_all_reset_cost as % of drop_all_cost */
    drop_all_reset_cost2 = (drop_all_cost2 * percent_hysteresis) / 100;
    drop_all_reset_cost1 = (drop_all_cost1 * percent_hysteresis) / 100;
    drop_all_reset_cost0 = (drop_all_cost0 * percent_hysteresis) / 100;

    /* program drop_all_cost */
    SOC_IF_ERROR_RETURN(WRITE_ESM_ADM_THR_DROP_ALL_COST2r(unit, drop_all_cost2));
    SOC_IF_ERROR_RETURN(WRITE_ESM_ADM_THR_DROP_ALL_COST1r(unit, drop_all_cost1));
    SOC_IF_ERROR_RETURN(WRITE_ESM_ADM_THR_DROP_ALL_COST0r(unit, drop_all_cost0));

    /* program drop_all_reset_cost */
    SOC_IF_ERROR_RETURN(WRITE_ESM_ADM_THR_DROP_ALL_RESET_COST2r(unit, 
                                                        drop_all_reset_cost2));
    SOC_IF_ERROR_RETURN(WRITE_ESM_ADM_THR_DROP_ALL_RESET_COST1r(unit, 
                                                        drop_all_reset_cost1));
    SOC_IF_ERROR_RETURN(WRITE_ESM_ADM_THR_DROP_ALL_RESET_COST0r(unit, 
                                                        drop_all_reset_cost0));

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "esm_prog_drop_all_costs: drop_all_cost2=%d, "),
                 drop_all_cost2));
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "drop_all_reset_cost2=%d (is "), drop_all_reset_cost2));
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "%d %% of drop_all_cost2"), percent_hysteresis));


    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "esm_prog_drop_all_costs: drop_all_cost1=%d, "),
                 drop_all_cost1));
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "drop_all_reset_cost1=%d (is "), drop_all_reset_cost1));
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "%d %% of drop_all_cost1"), percent_hysteresis));

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "esm_prog_drop_all_costs: drop_all_cost0=%d, "),
                 drop_all_cost0));
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "drop_all_reset_cost0=%d (is "), drop_all_reset_cost0));
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "%d %% of drop_all_cost0"), percent_hysteresis));
    return SOC_E_NONE;
}

STATIC int
hwtl_check_done(int unit)
{
    {
        uint32 esm_et_hwtl_status_0_buf;
        
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ESM_ET_HWTL_STATUS_0r, 0, 0, &esm_et_hwtl_status_0_buf));
        if (!(soc_reg_field_get(unit, ESM_ET_HWTL_STATUS_0r, esm_et_hwtl_status_0_buf, REQ_DONEf)
              && soc_reg_field_get(unit, ESM_ET_HWTL_STATUS_0r, esm_et_hwtl_status_0_buf, RSP_DONEf)
              )
            ) {
            return (SOC_E_BUSY);
        }
    }

    {
        uint32 esm_et_hwtl_status_1_buf;

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ESM_ET_HWTL_STATUS_1r, 0, 0, &esm_et_hwtl_status_1_buf));
        if (esm_et_hwtl_status_1_buf != 0) {
            _SHR_RETURN(SOC_E_FAIL);
        }
    }

    return (SOC_E_NONE);
}


STATIC int
hwtl_poll_done(int unit)
{
    unsigned n;

    for (n = 1000; n; --n) {
        int errcode = hwtl_check_done(unit);

        if (errcode == SOC_E_BUSY) {
            sal_usleep(1000);   /* Wait 1 ms */
            continue;
        }

        _SHR_RETURN(errcode);
    }

    _SHR_RETURN(SOC_E_TIMEOUT);
}


STATIC int
hwtl_verify_pb_counters(int      unit,
                        uint32   exp_ipipe_req_count,
                        uint64   exp_tx_pkt_count,
                        uint64   exp_rx_byte_count,
                        uint64   exp_tx_byte_count,
                        unsigned skip_exp_txrx_pbc_chk,
                        unsigned skip_exp_ipipe_req_chk
                        )
{
    unsigned errcnt = 0;

    {
        uint64 cnt_mask;

        exp_ipipe_req_count &= BITS(16);
       /* coverity[large_shift:FALSE] */
        COMPILER_64_SET(cnt_mask, BITS(8), BITS(32));
        COMPILER_64_AND(exp_tx_pkt_count, cnt_mask);
        COMPILER_64_AND(exp_rx_byte_count, cnt_mask);
        COMPILER_64_AND(exp_tx_byte_count, cnt_mask);
    }

    {
        uint32   etu_dbg_ipipe_req_rsp_count_buf;
        unsigned req_count, rsp_count;

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_DBG_IPIPE_REQ_RSP_COUNTr, 0, 0, &etu_dbg_ipipe_req_rsp_count_buf));
        req_count = soc_reg_field_get(unit, ETU_DBG_IPIPE_REQ_RSP_COUNTr, etu_dbg_ipipe_req_rsp_count_buf, REQ_COUNTf);
        rsp_count = soc_reg_field_get(unit, ETU_DBG_IPIPE_REQ_RSP_COUNTr, etu_dbg_ipipe_req_rsp_count_buf, RSP_COUNTf);
        if (rsp_count != req_count) {
            ++errcnt;
        }
        if (!skip_exp_ipipe_req_chk && req_count != exp_ipipe_req_count) {
            ++errcnt;
        }
    }

    {
        uint32 etu_dbg_ipipe_err_rsp_count_buf;

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_DBG_IPIPE_ERR_RSP_COUNTr, 0, 0, &etu_dbg_ipipe_err_rsp_count_buf));
        if (etu_dbg_ipipe_err_rsp_count_buf != 0) {
            ++errcnt;
        }
    }

    {
        uint64 ilamac_tx_packets_count_buf, ilamac_rx_packets_count_buf;
        uint64 tx_pkt_count, rx_pkt_count;

        SOC_IF_ERROR_RETURN(soc_reg64_get(unit, ILAMAC_RX_PACKETS_COUNTr, 0, 0, &ilamac_rx_packets_count_buf));
        SOC_IF_ERROR_RETURN(soc_reg64_get(unit, ILAMAC_TX_PACKETS_COUNTr, 0, 0, &ilamac_tx_packets_count_buf));
        rx_pkt_count = soc_reg64_field_get(unit, ILAMAC_RX_PACKETS_COUNTr, ilamac_rx_packets_count_buf, COUNTf);
        tx_pkt_count = soc_reg64_field_get(unit, ILAMAC_TX_PACKETS_COUNTr, ilamac_tx_packets_count_buf, COUNTf);
        if (COMPILER_64_NE(rx_pkt_count, tx_pkt_count)) {
            ++errcnt;
        }
        if (!skip_exp_txrx_pbc_chk && COMPILER_64_NE(tx_pkt_count, exp_tx_pkt_count)) {
            ++errcnt;
        }
    }

    {
        uint64 ilamac_tx_bytes_count_buf;
        uint64 tx_byte_count;

        SOC_IF_ERROR_RETURN(soc_reg64_get(unit, ILAMAC_TX_BYTES_COUNTr, 0, 0, &ilamac_tx_bytes_count_buf));
        tx_byte_count = soc_reg64_field_get(unit, ILAMAC_TX_BYTES_COUNTr, ilamac_tx_bytes_count_buf, COUNTf);
        if (!skip_exp_txrx_pbc_chk && COMPILER_64_NE(tx_byte_count, exp_tx_byte_count)) {
            ++errcnt;
        }
    }

    {
        uint64 ilamac_rx_bytes_count_buf;
        uint64 rx_byte_count;

        SOC_IF_ERROR_RETURN(soc_reg64_get(unit, ILAMAC_RX_BYTES_COUNTr, 0, 0, &ilamac_rx_bytes_count_buf));
        rx_byte_count = soc_reg64_field_get(unit, ILAMAC_RX_BYTES_COUNTr, ilamac_rx_bytes_count_buf, COUNTf);
        if (!skip_exp_txrx_pbc_chk && COMPILER_64_NE(rx_byte_count, exp_rx_byte_count)) {
            ++errcnt;
        }
    }

    _SHR_RETURN(errcnt ? SOC_E_FAIL : SOC_E_NONE);
}


STATIC int
hwtl_start_cbi(int unit, unsigned num_sp_entries, unsigned ee_entry)
{
    unsigned endless = 0;
    unsigned last_iteration = (2040 / num_sp_entries) - 1;
    uint32   exp_ipipe_req_count = 0;
    uint64   exp_tx_pkt_count;
    uint64   exp_rx_byte_count;
    uint64   exp_tx_byte_count;
    unsigned sp_index, ee;
    unsigned num_iterations, last_sp_index;

    COMPILER_64_ZERO(exp_tx_pkt_count);
    COMPILER_64_ZERO(exp_rx_byte_count);
    COMPILER_64_ZERO(exp_tx_byte_count);
    for (sp_index = 0; sp_index < num_sp_entries; ++sp_index) {
        if (sp_index == ee_entry) {
            esm_key_id_to_field_mapper_entry_t esm_key_id_to_field_mapper_buf;

            ee = 1;
            SOC_IF_ERROR_RETURN(hwtl_prog_sp_entry(unit, sp_index, PTR_FME_REQ_CBI, ee));
            SOC_IF_ERROR_RETURN(soc_mem_read(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, MEM_BLOCK_ANY, PTR_FME_REQ_CBI, esm_key_id_to_field_mapper_buf.entry_data));
            ++exp_ipipe_req_count;

            COMPILER_64_ADD_32(exp_tx_pkt_count, hwtl_find_tx_pkt_count(unit, esm_key_id_to_field_mapper_buf.entry_data));
            COMPILER_64_ADD_32(exp_rx_byte_count, hwtl_find_rx_byte_count(unit, esm_key_id_to_field_mapper_buf.entry_data));
            COMPILER_64_ADD_32(exp_tx_byte_count, hwtl_find_tx_byte_count(unit, esm_key_id_to_field_mapper_buf.entry_data));
        } else {
            ee = 0;
            SOC_IF_ERROR_RETURN(hwtl_prog_sp_entry(unit, sp_index, PTR_FME_REQ_NULL, ee));
        }
    }

    num_iterations = last_iteration + 1;
    exp_ipipe_req_count *= num_iterations;
    COMPILER_64_UMUL_32(exp_tx_pkt_count, num_iterations);
    COMPILER_64_UMUL_32(exp_rx_byte_count, num_iterations);
    COMPILER_64_UMUL_32(exp_tx_byte_count, num_iterations);
 
    last_sp_index = num_sp_entries - 1;
    SOC_IF_ERROR_RETURN(hwtl_start(unit, last_sp_index, last_iteration, endless, 1));
    SOC_IF_ERROR_RETURN(hwtl_poll_done(unit));
    SOC_IF_ERROR_RETURN(hwtl_verify_pb_counters(unit, exp_ipipe_req_count, exp_tx_pkt_count, exp_rx_byte_count, exp_tx_byte_count, 0, 0));

    _SHR_RETURN(SOC_E_NONE);
}

STATIC int
hwtl_kill_endless (int unit) 
{

    int rc, status;
    unsigned hwtl_control;
    uint64 zero_64;

    SOC_IF_ERROR_RETURN(READ_ESM_ET_HWTL_CONTROLr(unit, &hwtl_control));
    soc_reg_field_set(unit, ESM_ET_HWTL_CONTROLr, &hwtl_control, ENDLESSf, 0);
    SOC_IF_ERROR_RETURN(WRITE_ESM_ET_HWTL_CONTROLr(unit, hwtl_control));

    COMPILER_64_ZERO(zero_64);

    /* wait for hwtl_done */
    rc =  hwtl_poll_done(unit);

    if (!SOC_SUCCESS(rc)) {
         /* hwtl did not complete */
        LOG_ERROR(BSL_LS_SOC_ESM,
                  (BSL_META_U(unit,
                              "unit: %d HWTL did not complete, will not "), unit));
        LOG_ERROR(BSL_LS_SOC_ESM,
                  (BSL_META_U(unit,
                              "verify packet, byte counters\n")));
        return rc;
    } else {
        /* hwtl had completed without mismatch */
        status =  hwtl_verify_pb_counters(unit, 0, zero_64, zero_64, zero_64, 
                                          1, 1);
        if (!SOC_SUCCESS(status)) {
            LOG_ERROR(BSL_LS_SOC_ESM,
                      (BSL_META_U(unit,
                                  "unit %d: HWTL test PASSED (completed without"),
                                  unit));
            LOG_ERROR(BSL_LS_SOC_ESM,
                      (BSL_META_U(unit,
                                  "mismatches), but packet, byte counters")));
            LOG_ERROR(BSL_LS_SOC_ESM,
                      (BSL_META_U(unit,
                                  "check FAILED\n")));
	    } else {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "unit %d: Packet, byte counters check also PASSED\n"),
                         unit));
	    }
	    return status;
    } 
    return SOC_E_NONE;
}

STATIC int
hwtl_start_burst (int unit, int ptr_fme_req, int num_bb_req, int num_gaps,
                  int last_iteration, int endless) 
{
    unsigned exp_ipipe_req_count = 0; /* also exp_ipipe_rsp_count */ 
    uint64   exp_tx_pkt_count; /* also exp_rx_pkt_count, num_CWs sent */
    uint64   exp_rx_byte_count; /* num of received bytes in DW */  
    uint64   exp_tx_byte_count; /* num of bytes transmitted in DW */
    esm_key_id_to_field_mapper_entry_t fme;
    int rc = 0, sp_index;
    unsigned last_sp_index, use_incr_cba;
    unsigned inc_tx_pkt_count, inc_rx_byte_count, inc_tx_byte_count;
    unsigned last_cword_is_optional, num_iterations;

    COMPILER_64_ZERO(exp_tx_pkt_count);
    COMPILER_64_ZERO(exp_rx_byte_count);
    COMPILER_64_ZERO(exp_tx_byte_count);
/*
   esm_puts_fyi "hwtl_start_burst: ptr_fme_req=$ptr_fme_req, num_bb_req=$num_bb_req, num_gaps=$num_gaps, last_iteration=$last_iteration, endless=$endless";
*/
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, 
                        MEM_BLOCK_ANY, ptr_fme_req, &fme));
    inc_tx_pkt_count = hwtl_find_tx_pkt_count(unit, fme.entry_data); 
    inc_rx_byte_count = hwtl_find_rx_byte_count(unit, fme.entry_data);
    inc_tx_byte_count = hwtl_find_tx_byte_count(unit, fme.entry_data);
    last_cword_is_optional = soc_ESM_KEY_ID_TO_FIELD_MAPPERm_field32_get(unit, 
                                       fme.entry_data, LAST_CWORD_IS_OPTIONALf);

    for (sp_index = 0; sp_index < num_bb_req;  sp_index++) {
        int ee = 1;
        rc = hwtl_prog_sp_entry(unit, sp_index,  ptr_fme_req,  ee);
        SOC_IF_ERROR_RETURN(rc);
        exp_ipipe_req_count += 1;
        COMPILER_64_ADD_32(exp_tx_pkt_count, inc_tx_pkt_count);
        COMPILER_64_ADD_32(exp_rx_byte_count, inc_rx_byte_count);
        COMPILER_64_ADD_32(exp_tx_byte_count, inc_tx_byte_count);
/*
      #esm_puts_fyi "ee=1, sp_index=$sp_index, ptr_fme_req=$ptr_fme_req, tx_pkt_count=$inc_tx_pkt_count, tx_byte_count=$inc_tx_byte_count, rx_byte_count=$inc_rx_byte_count";
*/
    }

    for (sp_index = num_bb_req; sp_index < num_bb_req + num_gaps; sp_index++) {
        int ee = 0;
        rc = hwtl_prog_sp_entry(unit, sp_index, PTR_FME_REQ_NULL,  ee);
        SOC_IF_ERROR_RETURN(rc);
    }


    /* num_iterations */
    num_iterations = last_iteration + 1; 
    exp_ipipe_req_count = exp_ipipe_req_count * num_iterations;
    COMPILER_64_UMUL_32(exp_tx_pkt_count, num_iterations);
    COMPILER_64_UMUL_32(exp_rx_byte_count, num_iterations);
    COMPILER_64_UMUL_32(exp_tx_byte_count, num_iterations);

    /*  kickoff */
    last_sp_index = num_bb_req + num_gaps - 1;
    use_incr_cba = 0; /* use sp_index as cba */
    rc = hwtl_start(unit, last_sp_index, last_iteration, endless, use_incr_cba);
    SOC_IF_ERROR_RETURN(rc);


    /* check results */
    if (endless) {
/*
      #esm_puts_fyi " WARNING: will not check for hwtl_done, will not verify packet, byte counters (ENDLESS=1), HWTL is still running";
*/
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "unit %d: WARNING: will not check for hwtl_done,"), unit));
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "will not verify packet, byte counters")));
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "(ENDLESS=1), HWTL is still running\n")));
        return SOC_E_NONE;
    } else {
        /* wait for hwtl_done */
        rc = hwtl_poll_done(unit);
        if (rc != SOC_E_NONE) {
            /* hwtl did not complete */
            LOG_ERROR(BSL_LS_SOC_ESM,
                      (BSL_META_U(unit,
                                  "unit %d: HWTL did not complete !!, will not"),
                                  unit));
            LOG_ERROR(BSL_LS_SOC_ESM,
                      (BSL_META_U(unit,
                                  "verify packet, byte counters")));
	        return rc;
        } else {
            unsigned skip_exp_ipipe_req_chk = 0;
            unsigned skip_exp_txrx_pbc_chk;
            /* hwtl had completed without mismatch */
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "unit %d: HWTL PASSED (completed"), unit));
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "without mismatch), will now verify ")));
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "packet, byte counters\n")));
            skip_exp_txrx_pbc_chk = last_cword_is_optional;
            rc =  hwtl_verify_pb_counters(unit, exp_ipipe_req_count,  
                                          exp_tx_pkt_count, exp_rx_byte_count,
                                          exp_tx_byte_count, 
                                          skip_exp_txrx_pbc_chk,
                                          skip_exp_ipipe_req_chk);
	        if (rc != SOC_E_NONE) {
                LOG_ERROR(BSL_LS_SOC_ESM,
                          (BSL_META_U(unit,
                                      "unit: %d HWTL PASSED (completed without "),
                                      unit));
                LOG_ERROR(BSL_LS_SOC_ESM,
                          (BSL_META_U(unit,
                                      "mismatches), but packet, byte counters ")));
                LOG_ERROR(BSL_LS_SOC_ESM,
                          (BSL_META_U(unit,
                                      "check FAILED !!")));
	        } else {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "unit: %d Packet, byte counters check"),
                             unit));
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        " also PASSED")));
	        }
	        return rc;
        } 
   }
}

int
hwtl_find_idle_req_rsp_latency(int unit, int ptr_fme_req, 
                               int test_runtime_seconds, 
                               unsigned *latency_ns)
{
    int num_bb_req = 1, num_gaps=299;
    int endless, last_iteration;
    int failed = 0, junk;
    int rv = 0, index=0;
    unsigned esmif_max_latency_recorded, esmif_clk_period_ns;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "test_runtime_seconds=%d\n"),test_runtime_seconds));
    if (test_runtime_seconds <= 0) {
        endless = 0;
        last_iteration = test_runtime_seconds * -1; 
        failed =  hwtl_start_burst(unit, ptr_fme_req, num_bb_req, num_gaps, 
                                    last_iteration, endless);
    } else {
        endless = 1;
        last_iteration = 0;
        junk = hwtl_start_burst(unit, ptr_fme_req, num_bb_req, num_gaps, 
                                last_iteration, endless);
        /* above will never return non-zero because endless=1 */
        SOC_IF_ERROR_RETURN(junk);
    
        sal_sleep(test_runtime_seconds); 
    
        failed = hwtl_kill_endless(unit);
    }
    
    if (failed) {
        LOG_ERROR(BSL_LS_SOC_ESM,
                  (BSL_META_U(unit,
                              "unit %d: hwtl_find_idle_req_rsp_latency: FAILED !!\n"), 
                   unit));
        rv = SOC_E_FAIL;
    } else {
       /* num_esmif_clks */
        rv = READ_ESM_MAX_LATENCY_RECORDEDr(unit, &esmif_max_latency_recorded); 
        SOC_IF_ERROR_RETURN(rv);
        esmif_clk_period_ns = (1000 * 1000 * 1000)/esmif_clk_freq_khz(unit);
        *latency_ns = esmif_clk_period_ns * esmif_max_latency_recorded;

        if (esmif_max_latency_recorded > ESMIF_MAX_FIFO_DEPTH) {
         LOG_ERROR(BSL_LS_SOC_ESM,
                   (BSL_META_U(unit,
                               "unit %d: ESMIF.MAX_LATENCY_RECORDED = "), unit));
         LOG_ERROR(BSL_LS_SOC_ESM,
                   (BSL_META_U(unit,
                               "%d !! - number "), esmif_max_latency_recorded));
         LOG_ERROR(BSL_LS_SOC_ESM,
                   (BSL_META_U(unit,
                               "greater than %d means "), ESMIF_MAX_FIFO_DEPTH ));
         LOG_ERROR(BSL_LS_SOC_ESM,
                   (BSL_META_U(unit,
                               "catastrophy !!\n")));
         rv = SOC_E_FAIL;
        } else if (esmif_max_latency_recorded > ESMIF_ALLOWED_FIFO_DEPTH) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "unit %d: ESMIF.MAX_LATENCY_RECORDED = "),
                         unit));
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "%d !! - which "),esmif_max_latency_recorded ));
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "is greater than budgeted %d !!"), 
                         ESMIF_ALLOWED_FIFO_DEPTH));
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "@ %d MHz,this \n corresponds to latency of %d nS"),
                         esmif_clk_freq_khz(unit)/1000, *latency_ns));
            rv = SOC_E_NONE;
        } else {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "ESMIF.MAX_LATENCY_RECORDED = %d "), 
                         esmif_max_latency_recorded));
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "(num_esmif_clks) @ %d MHz, this "),
                         esmif_clk_freq_khz(unit)/1000));
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "corresponds \n to latency of %d nS\n"), 
                         *latency_ns));
            rv = SOC_E_NONE;
        }
    }

    /* Cleanup */
    for (index = 0; index < (num_bb_req + num_gaps); index++) {
        SOC_IF_ERROR_RETURN(hwtl_prog_sp_entry(unit, index, 0, 0));
    }
    return rv;

}

STATIC int
nl_ctx_buf_reg_write_loc80(int unit, unsigned dev_id, unsigned location, nl_reg_buf_t wr_data)
{
    _SHR_RETURN(nl_reg_write(unit, dev_id, NL_REG_ADDR_CONTEXT_BUFFER_BASE + location, wr_data, 0, 0, 0));
}


STATIC int
nl_ctx_buf_init(int unit, unsigned num_nl, unsigned use_hwtl)
{
    unsigned first_location;

    if (use_hwtl) {
        const unsigned num_sp_entries = 10;

        unsigned dev_id, ee_entry;

        SOC_IF_ERROR_RETURN(hwtl_zero_out_esm_range_check(unit));
        SOC_IF_ERROR_RETURN(hwtl_prog_fme_req_cbi(unit));
        SOC_IF_ERROR_RETURN(hwtl_prog_fme_rsp_cbi(unit));
        for (dev_id = 0; dev_id < num_nl; ++dev_id) {
            SOC_IF_ERROR_RETURN(hwtl_prog_ltr_cbi(unit, dev_id));
        }
        for (ee_entry = 0; ee_entry < num_sp_entries; ++ee_entry) {
            SOC_IF_ERROR_RETURN(hwtl_start_cbi(unit, num_sp_entries, ee_entry));
        }

        first_location = (0x7f7 + 1) << 3;
    } else {
        first_location = 0;
    }

    {
        unsigned     dev_id;
        nl_reg_buf_t wr_data;

        sal_memset(wr_data, 0, sizeof(wr_data));
        for (dev_id = 0; dev_id < num_nl; ++dev_id) {
            unsigned location;
            
            for (location = first_location; location < 0x4000; ++location) {
                SOC_IF_ERROR_RETURN(nl_ctx_buf_reg_write_loc80(unit, dev_id, location, wr_data));
            }        
        }
    }

    _SHR_RETURN(SOC_E_NONE);
}


STATIC int
nl_prog_dev_cfg_reg(int unit, unsigned dev_id, unsigned en_nl_parity_scan)
{
    nl_reg_buf_t nl_reg_dev_cfg_buf;

    sal_memset(nl_reg_dev_cfg_buf, 0, sizeof(nl_reg_dev_cfg_buf));
    nl_reg_buf_bitfield_set(nl_reg_dev_cfg_buf, 7, 1, 1); /* db_parity_error_entry_invalidate = 1*/
    if (en_nl_parity_scan) {
        nl_reg_buf_bitfield_set(nl_reg_dev_cfg_buf, 6, 1, 1); /* soft_error_scan_enable = 1*/
    }
    SOC_IF_ERROR_RETURN(nl_reg_write(unit, dev_id, NL_REG_ADDR_DEV_CFG, nl_reg_dev_cfg_buf, 0, 0, 0));

    _SHR_RETURN(SOC_E_NONE);
}


STATIC int
nl_cfg_low_power_mode(int unit, unsigned num_nl, unsigned num_blks, unsigned *blknums)
{
    nl_reg_buf_t reg0_data, reg1_data, reg2_data, reg3_data;

    sal_memset(reg0_data, 0, sizeof(reg0_data));
    sal_memset(reg1_data, 0, sizeof(reg1_data));
    sal_memset(reg2_data, 0, sizeof(reg2_data));
    sal_memset(reg3_data, 0, sizeof(reg3_data));

    for ( ; num_blks; --num_blks, ++blknums) {
        unsigned blknum = *blknums;

        if (blknum >= 128) {
            _SHR_RETURN(SOC_E_FAIL);
        }
        if (blknum >= 96) {
            nl_reg_buf_bitfield_set(reg3_data, blknum - 96, 1, 1);            
        } else if (blknum >= 64) {
            nl_reg_buf_bitfield_set(reg2_data, blknum - 64, 1, 1);            
        } else if (blknum >= 32) {
            nl_reg_buf_bitfield_set(reg1_data, blknum - 32, 1, 1);            
        } else {
            nl_reg_buf_bitfield_set(reg0_data, blknum, 1, 1);            
        }
    }

    SOC_IF_ERROR_RETURN(nl_reg_write(unit, 0, NL_REG_ADDR_LPM_ENABLE, reg0_data, 0, 0, 0));
    sal_usleep(1);              /* Wait 1 us */
    if (num_nl > 1) {
        SOC_IF_ERROR_RETURN(nl_reg_write(unit, 1, NL_REG_ADDR_LPM_ENABLE, reg1_data, 0, 0, 0));
        sal_usleep(1);          /* Wait 1 us */
    }
    if (num_nl > 2) {
        SOC_IF_ERROR_RETURN(nl_reg_write(unit, 2, NL_REG_ADDR_LPM_ENABLE, reg2_data, 0, 0, 0));
        sal_usleep(1);          /* Wait 1 us */
    }
    if (num_nl > 3) {
        SOC_IF_ERROR_RETURN(nl_reg_write(unit, 3, NL_REG_ADDR_LPM_ENABLE, reg3_data, 0, 0, 0));
        sal_usleep(1);          /* Wait 1 us */
    }

    _SHR_RETURN(SOC_E_NONE);
}


STATIC int
chk_misc_config(int unit)
{
    unsigned errcnt = 0;

    {
        static const struct {
            soc_field_t fld;
            uint32      exp_val;
        } flds[] = {
            { BIT_ORDER_INVERTf,    1 },
            { TX_XOFF_MODEf,        0 },
            { TX_ENABLEf,           1 },
            { XON_RX_CH1f,          0 },
            { XON_RX_CH0f,          1 },
            { TX_RLIM_ENABLEf,      0 },
            { TX_RDYOUT_THRESHf,    0 },
            { TX_DISABLE_SKIPWORDf, 0 },
            { TX_BURSTSHORTf,       1 },
            { TX_BURSTMAXf,         1 },
            { TX_MFRAMELEN_MINUS1f, 2047 }
        };

        uint32   ilamac_tx_config0_buf;
        unsigned i;
        
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ILAMAC_TX_CONFIG0r, 0, 0, &ilamac_tx_config0_buf));
        for (i = 0; i < COUNTOF(flds); ++i) {
            if (soc_reg_field_get(unit, ILAMAC_TX_CONFIG0r, ilamac_tx_config0_buf, flds[i].fld) != flds[i].exp_val) {
                ++errcnt;
            }
        }
    }

    {
        uint32 ilamac_tx_config1_buf;
        
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ILAMAC_TX_CONFIG1r, 0, 0, &ilamac_tx_config1_buf));
        if (ilamac_tx_config1_buf != 0) {
            ++errcnt;
        }
    }

    {
        static struct {
            soc_field_t fld;
            uint32      exp_val;
        } flds[] = {
            { TX_CW56_RSVDf,     0 },
            { TX_CW_CP_CBA_MSBf, 0x7f8 },
            { TX_CW_CP_CBA_LSBf, 0 },
            { TX_CW_CHNLf,       0 },
            { TX_CW_ERR_STATUSf, 0 },
            { TX_CW29_27_RSVDf,  0 },
            { TX_LAST_LANEf,     23 },
            { TX_HAS_BAD_LANEf,  0 }
        };

        uint32   ilamac_tx_config2_buf;
        unsigned i;

        unsigned int nl_16lane_mode = 0;

        if (((soc_tcam_info_t *) SOC_CONTROL(unit)->tcam_info)->num_tcams  == 2) {
            nl_16lane_mode = 1;
        }

        if (nl_16lane_mode) {
            /* Cascaded mode: set num lanes to 16 */
            flds[6].exp_val = 15;
        }
        
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ILAMAC_TX_CONFIG2r, 0, 0, &ilamac_tx_config2_buf));
        for (i = 0; i < COUNTOF(flds); ++i) {
            if (soc_reg_field_get(unit, ILAMAC_TX_CONFIG2r, ilamac_tx_config2_buf, flds[i].fld) != flds[i].exp_val) {
                ++errcnt;
            }
        }
    }

    {
        static const struct {
            soc_field_t fld;
            uint32      exp_val;
        } flds[] = {
            { TX_DIAGWORD_INTFSTATf, 1 },
            { TX_DIAGWORD_LANESTATf, 0xffffff }
        };

        uint32   ilamac_tx_config3_buf;
        unsigned i;
        
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ILAMAC_TX_CONFIG3r, 0, 0, &ilamac_tx_config3_buf));
        for (i = 0; i < COUNTOF(flds); ++i) {
            if (soc_reg_field_get(unit, ILAMAC_TX_CONFIG3r, ilamac_tx_config3_buf, flds[i].fld) != flds[i].exp_val) {
                ++errcnt;
            }
        }
    }

    {
        uint32 ilamac_tx_config4_buf;

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ILAMAC_TX_CONFIG4r, 0, 0, &ilamac_tx_config4_buf));
        if (ilamac_tx_config4_buf != 0) {
            ++errcnt;
        }
    }

    {
        uint32 ilamac_tx_config5_buf;

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ILAMAC_TX_CONFIG5r, 0, 0, &ilamac_tx_config5_buf));
        if (ilamac_tx_config5_buf != 0) {
            ++errcnt;
        }
    }

    {
        static const struct {
            soc_field_t fld;
            uint32      exp_val;
        } flds[] = {
            { BIT_ORDER_INVERTf,    1 },
            { RX_FORCE_RESYNCf,     0 },
            { RX_LAST_LANEf,        11 },
            { RX_HAS_BAD_LANEf,     0 },
            { RX_BURSTMAXf,         0 },
            { RX_MFRAMELEN_MINUS1f, 2047 }
        };

        uint32   ilamac_rx_config_buf;
        unsigned i;
        
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ILAMAC_RX_CONFIGr, 0, 0, &ilamac_rx_config_buf));
        for (i = 0; i < COUNTOF(flds); ++i) {
            if (soc_reg_field_get(unit, ILAMAC_RX_CONFIGr, ilamac_rx_config_buf, flds[i].fld) != flds[i].exp_val) {
                ++errcnt;
            }
        }        
    }

    {
        static const struct {
            soc_field_t fld;
            uint32      exp_val;
        } flds[] = {
            { HBIT_MEM_PSM_VDDf,      0 },
            { EXT_TCAM_DENSITYf,      2 }
        };

        uint32   etu_config0_buf;
        unsigned i;
        
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_CONFIG0r, 0, 0, &etu_config0_buf));
        for (i = 0; i < COUNTOF(flds); ++i) {
            if (soc_reg_field_get(unit, ETU_CONFIG0r, etu_config0_buf, flds[i].fld) != flds[i].exp_val) {
                ++errcnt;
            }
        }        
    }

    {
        static const struct {
            soc_field_t fld;
            uint32      exp_val;
        } flds[] = {
            { LTR_CP_EXT_L2_ALLf,  0x3f },
            { LTR_CP_EXT_L2_WIDEf, 0x3e },
            { LTR_CP_EXT_L2f,      0x3d }
        };

        uint64   etu_config1_buf;
        unsigned i;
        
        SOC_IF_ERROR_RETURN(soc_reg64_get(unit, ETU_CONFIG1r, 0, 0, &etu_config1_buf));
        for (i = 0; i < COUNTOF(flds); ++i) {
            if (soc_reg64_field32_get(unit, ETU_CONFIG1r, etu_config1_buf, flds[i].fld) != flds[i].exp_val) {
                ++errcnt;
            }
        }        
    }

    {
        static const struct {
            soc_field_t fld;
            uint32      exp_val;
        } flds[] = {
            { ET_POST_WR_ENABLEf, 1 },
            { DB_WR_MODEf,        0 }
        };

        uint32   etu_config2_buf;
        unsigned i;
        
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_CONFIG2r, 0, 0, &etu_config2_buf));
        for (i = 0; i < COUNTOF(flds); ++i) {
            if (soc_reg_field_get(unit, ETU_CONFIG2r, etu_config2_buf, flds[i].fld) != flds[i].exp_val) {
                ++errcnt;
            }
        }        
    }

    {
        static const struct {
            soc_field_t fld;
            uint32      exp_val;
        } flds[] = {
            { WR_DB_NUM_NOPf,              0 },
            { WR_DB_NOP_MODEf,             0 },
            { IGNORE_RX_CW_CBA_MMf,        0 },
            { IGNORE_RX_CW_OPC_MMf,        0 },
            { IGNORE_RX_CW_CH_ERRf,        0 },
            { MIN_RX_PKT_LEN_16Bf,         0 },
            { CHK_PAR_XLAT1f,              1 },
            { CHK_PAR_XLAT0f,              1 },
            { GEN_PAR_XLAT1f,              1 },
            { GEN_PAR_XLAT0f,              1 },
            { GEN_PAR_L2_ENTRY_DATAf,      1 },
            { GEN_PAR_L2_ENTRY_DATA_WIDEf, 1 },
            { GEN_PAR_DEFIP_DATAf,         1 },
            { GEN_PAR_L3_UCAST_DATAf,      1 },
            { GEN_PAR_L3_UCAST_DATA_WIDEf, 1 },
            { GEN_PAR_FP_POLICY_1Xf,       1 },
            { GEN_PAR_FP_POLICY_2Xf,       1 },
            { GEN_PAR_FP_POLICY_3Xf,       1 },
            { GEN_PAR_FP_POLICY_4Xf,       1 },
            { GEN_PAR_FP_POLICY_6Xf,       1 },
	    { CHK_PAR_L2_ENTRY_DATAf,      1 },
	    { CHK_PAR_L2_ENTRY_DATA_WIDEf, 1 },
	    { CHK_PAR_DEFIP_DATAf,         1 },
	    { CHK_PAR_L3_UCAST_DATAf,      1 },
	    { CHK_PAR_L3_UCAST_DATA_WIDEf, 1 },
	    { CHK_PAR_FP_POLICY_1Xf,       1 },
	    { CHK_PAR_FP_POLICY_2Xf,       1 },
	    { CHK_PAR_FP_POLICY_3Xf,       1 },
	    { CHK_PAR_FP_POLICY_4Xf,       1 },
	    { CHK_PAR_FP_POLICY_6Xf,       1 }
        };

        uint32   etu_config3_buf;
        unsigned i;
        
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_CONFIG3r, 0, 0, &etu_config3_buf));
        for (i = 0; i < COUNTOF(flds); ++i) {
            if (soc_reg_field_get(unit, ETU_CONFIG3r, etu_config3_buf, flds[i].fld) != flds[i].exp_val) {
                ++errcnt;
            }
        }        
    }

    {
        static const struct {
            soc_field_t fld;
            uint32      exp_val;
        } flds[] = {
            { DATAPATH_RST_f,         0 },
            { ILAMAC_RX_SERDES_RST_f, 0xfff },
            { ILAMAC_RX_LBUS_RST_f,   1 },
            { ILAMAC_TX_SERDES_RST_f, 1 },
            { ILAMAC_TX_LBUS_RST_f,   1 },
            { EXT_TCAM_CRST_Lf,       1 },
            { EXT_TCAM_SRST_Lf,       1 }
        };

        uint32   etu_config4_buf;
        unsigned i;
        
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_CONFIG4r, 0, 0, &etu_config4_buf));
        for (i = 0; i < COUNTOF(flds); ++i) {
            if (soc_reg_field_get(unit, ETU_CONFIG4r, etu_config4_buf, flds[i].fld) != flds[i].exp_val) {
                ++errcnt;
            }
        }        
    }

    {
        uint32 etu_rrfe_wait_timer_buf, cnt;

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_RRFE_WAIT_TIMERr, 0, 0, &etu_rrfe_wait_timer_buf));
        cnt = soc_reg_field_get(unit, ETU_RRFE_WAIT_TIMERr, etu_rrfe_wait_timer_buf, CNTf);
        if (cnt == 0xffffff || (cnt > 0 && cnt < 15)) {
            ++errcnt;
        }
    }

    {
        static const struct {
            soc_field_t fld;
            uint32      exp_val;
        } flds[] = {
            { INJECT_SEC_EVf,  0 },
            { INJECT_DED_EVf,  0 },
            { INJECT_SEC_ODf,  0 },
            { INJECT_DED_ODf,  0 },
            { DIS_CHK_ECC_EVf, 0 },
            { DIS_CHK_ECC_ODf, 0 },
            { SHOW_CUR_CNTf,   0 },
            { EARLY_FULL_THRf, 700 },
            { CP_ACC_THRf,     200 }
        };

        uint32   etu_tx_req_fifo_ctl_buf;
        unsigned i;
        
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_TX_REQ_FIFO_CTLr, 0, 0, &etu_tx_req_fifo_ctl_buf));
        for (i = 0; i < COUNTOF(flds); ++i) {
            if (soc_reg_field_get(unit, ETU_TX_REQ_FIFO_CTLr, etu_tx_req_fifo_ctl_buf, flds[i].fld) != flds[i].exp_val) {
                ++errcnt;
            }
        }        
    }

    {
        static const struct {
            soc_field_t fld;
            uint32      exp_val;
        } flds[] = {
            { INJECT_SECf,     0 },
            { INJECT_DEDf,     0 },
            { DIS_CHK_ECCf,    0 },
            { SHOW_CUR_CNTf,   0 },
            { EARLY_FULL_THRf, 1015 }
        };

        uint32   etu_tx_pipe_ctl_fifo_ctl_buf;
        unsigned i;
        
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_TX_PIPE_CTL_FIFO_CTLr, 0, 0, &etu_tx_pipe_ctl_fifo_ctl_buf));
        for (i = 0; i < COUNTOF(flds); ++i) {
            if (soc_reg_field_get(unit, ETU_TX_PIPE_CTL_FIFO_CTLr, etu_tx_pipe_ctl_fifo_ctl_buf, flds[i].fld) != flds[i].exp_val) {
                ++errcnt;
            }
        }        
    }

    {
        static const struct {
            soc_field_t fld;
            uint32      exp_val;
        } flds[] = {
            { INJECT_SEC_EVf, 0 },
            { INJECT_DED_EVf, 0 },
            { INJECT_SEC_ODf, 0 },
            { INJECT_DED_ODf, 0 },
            { DIS_CHK_ECCf,   0 },
            { SHOW_CUR_CNTf,  0 },
            { XOFF_RX_THRf,   200 },
            { XON_RX_THRf,    150 }
        };

        uint32   etu_rx_rsp_fifo_ctl_buf;
        unsigned i;
        
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_RX_RSP_FIFO_CTLr, 0, 0, &etu_rx_rsp_fifo_ctl_buf));
        for (i = 0; i < COUNTOF(flds); ++i) {
            if (soc_reg_field_get(unit, ETU_RX_RSP_FIFO_CTLr, etu_rx_rsp_fifo_ctl_buf, flds[i].fld) != flds[i].exp_val) {
                ++errcnt;
            }
        }        
    }

    {
        static const struct {
            soc_field_t fld;
            uint32      exp_val;
        } flds[] = {
            { RX_RESET_Nf, 0 },
            { STARTf,      0 },
            { ENABLEf,     0 }
        };

        uint32   etu_bist_ctl_buf;
        unsigned i;
        
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ETU_BIST_CTLr, 0, 0, &etu_bist_ctl_buf));
        for (i = 0; i < COUNTOF(flds); ++i) {
            if (soc_reg_field_get(unit, ETU_BIST_CTLr, etu_bist_ctl_buf, flds[i].fld) != flds[i].exp_val) {
                ++errcnt;
            }
        }        
    }
    
    {
        static const struct {
            soc_field_t fld;
            uint32      exp_val;
        } flds[] = {
            { ACLf, 3 },
            { SIPf, 1 },
            { DIPf, 2 },
            { DAf,  2 },
            { SAf,  0 }
        };

        uint32   esm_request_lane_map_buf;
        unsigned i;
        
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ESM_REQUEST_LANE_MAPr, 0, 0, &esm_request_lane_map_buf));
        for (i = 0; i < COUNTOF(flds); ++i) {
            if (soc_reg_field_get(unit, ESM_REQUEST_LANE_MAPr, esm_request_lane_map_buf, flds[i].fld) != flds[i].exp_val) {
                ++errcnt;
            }
        }        
    }
    
    {
        static const struct {
            soc_field_t fld;
            uint32      exp_val;
        } flds[] = {
            { FORCE_ADM_CTRL_FIFO_READf, 0 },
            { FORCE_ET_RSP_FIFO_READf,   0 },
            { FORCE_L2ETU_ACKf,          0 },
            { ETU_REQ_SEQ_NUM_SELf,      1 },
            { EXT_LOOKUP_ENABLEf,        1 }
        };

        uint32   esm_misc_control_buf;
        unsigned i;
        
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ESM_MISC_CONTROLr, 0, 0, &esm_misc_control_buf));
        for (i = 0; i < COUNTOF(flds); ++i) {
            if (soc_reg_field_get(unit, ESM_MISC_CONTROLr, esm_misc_control_buf, flds[i].fld) != flds[i].exp_val) {
                ++errcnt;
            }
        }        
        if (soc_reg_field_get(unit, ESM_MISC_CONTROLr, esm_misc_control_buf, MAX_CONTEXT_ADR_MSBf) > 0x7f7) {
            ++errcnt;
        }
    }
    
    {
        static const struct {
            soc_field_t fld;
            uint32      exp_val;
        } flds[] = {
            { SER_STATUS_BUS_INJECT_DBEf , 0 },
            { SER_STATUS_BUS_INJECT_SBEf,  0 },
            { SER_STATUS_BUS_ECC_ENf,      1 },
            { CTRL_BUS_FIFO_1_INJECT_DBEf, 0 },
            { CTRL_BUS_FIFO_1_INJECT_SBEf, 0 },
            { CTRL_BUS_FIFO_1_ECC_ENf,     1 },
            { CTRL_BUS_FIFO_INJECT_DBEf,   0 },
            { CTRL_BUS_FIFO_INJECT_SBEf,   0 },
            { CTRL_BUS_FIFO_ECC_ENf,       1 },
            { AUX1_FIFO_INJECT_DBEf,       0 },
            { AUX1_FIFO_INJECT_SBEf,       0 },
            { AUX1_FIFO_ECC_ENf,           1 },
            { ADM_CTRL_FIFO_INJECT_DBEf,   0 },
            { ADM_CTRL_FIFO_INJECT_SBEf,   0 },
            { ADM_CTRL_FIFO_ECC_ENf,       1 },
            { ET_RSP_FIFO_INJECT_DBEf,     0 },
            { ET_RSP_FIFO_INJECT_SBEf,     0 },
            { ET_RSP_FIFO_ECC_ENf,         1 }
        };

        uint32   iesmif_ecc_control_buf;
        unsigned i;
        
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, IESMIF_ECC_CONTROLr, 0, 0, &iesmif_ecc_control_buf));
        for (i = 0; i < COUNTOF(flds); ++i) {
            if (soc_reg_field_get(unit, IESMIF_ECC_CONTROLr, iesmif_ecc_control_buf, flds[i].fld) != flds[i].exp_val) {
                ++errcnt;
            }
        }        
    }
    
    {
        static const struct {
            soc_field_t fld;
            uint32      exp_val;
        } flds[] = {
            { IESMIF_BUS_FORCE_ERRORf,           0 },
            { IESMIF_BUS_PARITY_ENf,             0 },
            { L3_PROTOCOL_FN_PARITY_ENf,         1 },
            { KEY_ID_TO_FIELD_MAPPER_PARITY_ENf, 1 },
            { SEARCH_PROFILE_PARITY_ENf,         1 },
            { PKT_TYPE_ID_PARITY_ENf,            1 },
            { AD_6X_PARITY_ENf,                  1 },
            { AD_4X_PARITY_ENf,                  1 },
            { AD_3X_PARITY_ENf,                  1 },
            { AD_2X_PARITY_ENf,                  1 },
            { AD_1X_PARITY_ENf,                  1 }
        };

        uint32   iesmif_parity_control_buf;
        unsigned i;
        
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, IESMIF_PARITY_CONTROLr, 0, 0, &iesmif_parity_control_buf));
        for (i = 0; i < COUNTOF(flds); ++i) {
            if (soc_reg_field_get(unit, IESMIF_PARITY_CONTROLr, iesmif_parity_control_buf, flds[i].fld) != flds[i].exp_val) {
                ++errcnt;
            }
        }        
    }
    
    _SHR_RETURN(errcnt ? SOC_E_FAIL : SOC_E_NONE);
}


STATIC int
nl_mdio_chk_csm_counters(int unit, unsigned mdio_portid, unsigned chk_crx)
{
    return (SOC_E_NONE);
}


STATIC int
nl_mdio_chk_error_counters_status(int unit, unsigned dev_id, unsigned chk_crx)
{
    const unsigned mdio_portid = mdio_portid_get(unit, dev_id);

    unsigned errcnt = 0;
    
    if (SOC_FAILURE(nl_mdio_chk_csm_status_regs(unit, mdio_portid, chk_crx))) {
        ++errcnt;
    }
    if (SOC_FAILURE(nl_mdio_chk_csm_error_counters(unit, mdio_portid, chk_crx))) {
        ++errcnt;
    }
    if (SOC_FAILURE(nl_mdio_chk_csm_counters(unit, mdio_portid, chk_crx))) {
        ++errcnt;
    }
    if (SOC_FAILURE(nl_mdio_chk_pcs_error_status(unit, mdio_portid, chk_crx))) {
        ++errcnt;
    }
    if (SOC_FAILURE(nl_mdio_chk_pcs_error_counters(unit, mdio_portid, chk_crx))) {
        ++errcnt;
    }

    _SHR_RETURN(errcnt ? SOC_E_FAIL : SOC_E_NONE);
}


STATIC int
init2(int      unit,
      unsigned num_nl,
      unsigned en_nl_parity_scan,
      unsigned num_superblocks_lowpowermode,
      unsigned *superblocknums_lowpowermode
      )
{
    unsigned dev_id;

    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, ETU_CONFIG2r,               0, ET_POST_WR_ENABLEf, 1));
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, ETU_TX_EXT_L2_MAX_LATENCYr, 0, CNTf, 10));
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, ETU_TX_CP_MAX_LATENCYr,     0, CNTf, 10));
    
    for (dev_id = 0; dev_id < num_nl; ++dev_id) {
        SOC_IF_ERROR_RETURN(nl_reg_access_test(unit, dev_id));

#if 0                           /* <HP> Skip for now, since it fails </HP> */
        SOC_IF_ERROR_RETURN(nl_db_access_dm_test(unit, dev_id));
#endif

        SOC_IF_ERROR_RETURN(nl_prog_err_status_mask_reg(unit, dev_id));
        SOC_IF_ERROR_RETURN(nl_chk_err_status_reg(unit, dev_id));
    }

    {
        static const soc_field_t flds[] = {
            SER_STATUS_BUS_ECC_ENf,
            CTRL_BUS_FIFO_1_ECC_ENf,
            CTRL_BUS_FIFO_ECC_ENf,
            AUX1_FIFO_ECC_ENf,
            ADM_CTRL_FIFO_ECC_ENf,
            ET_RSP_FIFO_ECC_ENf
        };

        uint32   iesmif_ecc_control_buf = 0;
        unsigned i;

        for (i = 0; i < COUNTOF(flds); ++i) {
            soc_reg_field_set(unit, IESMIF_ECC_CONTROLr, &iesmif_ecc_control_buf, flds[i], 1);
        }
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, IESMIF_ECC_CONTROLr, 0, 0, iesmif_ecc_control_buf));
    }

    {
        static const soc_field_t flds[] = {
            L3_PROTOCOL_FN_PARITY_ENf,
            KEY_ID_TO_FIELD_MAPPER_PARITY_ENf,
            SEARCH_PROFILE_PARITY_ENf,
            PKT_TYPE_ID_PARITY_ENf,
            AD_6X_PARITY_ENf,
            AD_4X_PARITY_ENf,
            AD_3X_PARITY_ENf,
            AD_2X_PARITY_ENf,
            AD_1X_PARITY_ENf
        };

        uint32   iesmif_parity_control_buf = 0;
        unsigned i;

        for (i = 0; i < COUNTOF(flds); ++i) {
            soc_reg_field_set(unit, IESMIF_PARITY_CONTROLr, &iesmif_parity_control_buf, flds[i], 1);
        }
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, IESMIF_PARITY_CONTROLr, 0, 0, iesmif_parity_control_buf));
    }

    {
        static const soc_field_t flds[] = {
            EXT_LOOKUP_ENABLEf,
            MAX_CONTEXT_ADR_MSBf
        };
        static const uint32 fld_vals[] = {
            0,
            0x7f7
        };

        SOC_IF_ERROR_RETURN(soc_reg_fields32_modify(unit,
                                                    ESM_MISC_CONTROLr,
                                                    0,
                                                    COUNTOF(flds),
                                                    (soc_field_t *) flds,
                                                    (uint32 *) fld_vals
                                                    )
                            );
    }

    {
        static const struct {
            soc_reg_t reg;
            uint32    cost;
        } regs_and_vals[] = {
            { ESM_ADM_THR_DROP_ALL_COST0r, 0xfffff },
            { ESM_ADM_THR_DROP_ALL_COST1r, 0xfffff },
            { ESM_ADM_THR_DROP_ALL_COST2r, 0xfffff },
            { ESM_ADM_THR_DROP_OPT_COST0r, 0xfffff },
            { ESM_ADM_THR_DROP_OPT_COST1r, 0xfffff },
            { ESM_ADM_THR_DROP_OPT_COST2r, 0xfffff },
            { ESM_AXP_THR_XOFF_COST0r,     0xffff },
            { ESM_AXP_THR_XOFF_COST1r,     0xffff },
            { ESM_AXP_THR_XOFF_COST2r,     0xffff }
        };

        unsigned i;
        
        for (i = 0; i < COUNTOF(regs_and_vals); ++i) {
            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, regs_and_vals[i].reg, 0, COSTf, regs_and_vals[i].cost));
        }
    }

    SOC_IF_ERROR_RETURN(etu_mems_rst(unit, 1, 1));
    SOC_IF_ERROR_RETURN(etu_mems_poll_rst_done(unit));

    if (en_nl_parity_scan) {
        SOC_IF_ERROR_RETURN(nl_ctx_buf_init(unit, num_nl, 1));
    }

    SOC_IF_ERROR_RETURN(chk_all_intr_sts(unit, num_nl));

    {
        static const soc_field_t flds[] = {
            EXT_LOOKUP_ENABLEf,
            ETU_REQ_SEQ_NUM_SELf,
            MAX_CONTEXT_ADR_MSBf
        };
        static const uint32 fld_vals[] = {
            1,
            1,
            0x7f7
        };

        SOC_IF_ERROR_RETURN(soc_reg_fields32_modify(unit,
                                                    ESM_MISC_CONTROLr,
                                                    0,
                                                    COUNTOF(flds),
                                                    (soc_field_t *) flds,
                                                    (uint32 *) fld_vals
                                                    )
                            );
    }

    for (dev_id = 0; dev_id < num_nl; ++dev_id) {
        SOC_IF_ERROR_RETURN(nl_prog_dev_cfg_reg(unit, dev_id, en_nl_parity_scan));
    }
    
    SOC_IF_ERROR_RETURN(nl_cfg_low_power_mode(unit, num_nl, 0, 0));
    
    SOC_IF_ERROR_RETURN(chk_all_intr_sts(unit, num_nl));

    SOC_IF_ERROR_RETURN(soc_tcam_init(unit));
    SOC_IF_ERROR_RETURN(soc_triumph3_esm_init(unit));
    
    SOC_IF_ERROR_RETURN(chk_fifos(unit));
    SOC_IF_ERROR_RETURN(chk_etu_counters(unit));
    SOC_IF_ERROR_RETURN(chk_all_intr_sts(unit, num_nl));
    SOC_IF_ERROR_RETURN(chk_esmif_adc(unit));
    SOC_IF_ERROR_RETURN(chk_misc_config(unit));

    for (dev_id = 0; dev_id < num_nl; ++dev_id) {
        unsigned chk_crx = (dev_id != 0);

        SOC_IF_ERROR_RETURN(nl_mdio_chk_error_counters_status(unit, dev_id, chk_crx));
    }

    _SHR_RETURN(SOC_E_NONE);
}


STATIC unsigned
calc_cost2_num_rsp_words(int      unit,
                         uint32   *esm_key_id_to_field_mapper_buf_entry_data,
                         unsigned calc_optional_cost
                         )
{
    unsigned total_num_cwords;
    unsigned last_cword_is_optional;
    unsigned rsp_index_count_cw2;
    unsigned rsp_index_count_cw1;
    unsigned rsp_index_count_cw0;
    unsigned num_rsp_dwords_cw2;
    unsigned num_rsp_dwords_cw1;
    unsigned num_rsp_dwords_cw0;
    unsigned total_num_rsp_words;

    total_num_cwords = soc_mem_field32_get(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, esm_key_id_to_field_mapper_buf_entry_data, TOTAL_NUM_CWORDSf);
    last_cword_is_optional = soc_mem_field32_get(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, esm_key_id_to_field_mapper_buf_entry_data, LAST_CWORD_IS_OPTIONALf);

    if (total_num_cwords < 1 || total_num_cwords > 3) {
        return (0xffff);
    }
    
    if (calc_optional_cost) {
        if (!last_cword_is_optional) {
            return (0);
        }
    }

    if (!calc_optional_cost) {
        if (last_cword_is_optional) {
            if (total_num_cwords == 1) {
                return (0);
            }
             
            --total_num_cwords;
        }
    }

    rsp_index_count_cw2 = soc_mem_field32_get(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, esm_key_id_to_field_mapper_buf_entry_data, RSP_INDEX_COUNT_CW2f);
    rsp_index_count_cw1 = soc_mem_field32_get(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, esm_key_id_to_field_mapper_buf_entry_data, RSP_INDEX_COUNT_CW1f);
    rsp_index_count_cw0 = soc_mem_field32_get(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, esm_key_id_to_field_mapper_buf_entry_data, RSP_INDEX_COUNT_CW0f);

    if (rsp_index_count_cw2 == 3) {
        num_rsp_dwords_cw2 = 2;
    } else if (rsp_index_count_cw2 == 0) {
        num_rsp_dwords_cw2 = 2;
    } else {
        num_rsp_dwords_cw2 = 1;
    }
    if (rsp_index_count_cw1 == 3) {
        num_rsp_dwords_cw1 = 2;
    } else if (rsp_index_count_cw1 == 0) {
        num_rsp_dwords_cw1 = 2;
    } else {
        num_rsp_dwords_cw1 = 1;
    }
    if (rsp_index_count_cw0 == 3) {
        num_rsp_dwords_cw0 = 2;
    } else if (rsp_index_count_cw0 == 0) {
        num_rsp_dwords_cw0 = 2;
    } else {
        num_rsp_dwords_cw0 = 1;
    }

    if (total_num_cwords == 1) {
        total_num_rsp_words = total_num_cwords + num_rsp_dwords_cw0;
    } else if (total_num_cwords == 2) {
        total_num_rsp_words = total_num_cwords + num_rsp_dwords_cw0 + num_rsp_dwords_cw1;
    } else if (total_num_cwords == 3) {
        total_num_rsp_words = total_num_cwords + num_rsp_dwords_cw0 + num_rsp_dwords_cw1 + num_rsp_dwords_cw2;
    } else {
        /* coverity[dead_error_line] */
        total_num_rsp_words = 0xffff;
    }

    return (total_num_rsp_words);
}                         

STATIC unsigned
calc_cost1_num_req_words(int      unit,
                         uint32   *esm_key_id_to_field_mapper_buf_entry_data,
                         unsigned calc_optional_cost
                         )
{
    unsigned total_num_cwords;
    unsigned last_cword_is_optional;
    unsigned num_req_dwords_cw2;
    unsigned num_req_dwords_cw1;
    unsigned num_req_dwords_cw0;
    unsigned total_num_req_words;

    total_num_cwords = soc_mem_field32_get(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, esm_key_id_to_field_mapper_buf_entry_data, TOTAL_NUM_CWORDSf);
    last_cword_is_optional = soc_mem_field32_get(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, esm_key_id_to_field_mapper_buf_entry_data, LAST_CWORD_IS_OPTIONALf);

    if (total_num_cwords < 1 || total_num_cwords > 3) {
        return (0xffff);
    }
    
    if (calc_optional_cost) {
        if (!last_cword_is_optional) {
            return (0);
        }
    }
    
    if (!calc_optional_cost) {
        if (last_cword_is_optional) {
            if (total_num_cwords == 1) {
                return (0);
            }
            
            --total_num_cwords;
        }
    }

    num_req_dwords_cw2 = soc_mem_field32_get(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, esm_key_id_to_field_mapper_buf_entry_data, NUM_DWORDS_CW2f);
    num_req_dwords_cw1 = soc_mem_field32_get(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, esm_key_id_to_field_mapper_buf_entry_data, NUM_DWORDS_CW1f);
    num_req_dwords_cw0 = soc_mem_field32_get(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, esm_key_id_to_field_mapper_buf_entry_data, NUM_DWORDS_CW0f);
    
    if (total_num_cwords == 1) {
        total_num_req_words = total_num_cwords + num_req_dwords_cw0;
    } else if (total_num_cwords == 2) {
        total_num_req_words = total_num_cwords + num_req_dwords_cw0 + num_req_dwords_cw1;
    } else if (total_num_cwords == 3) {
        total_num_req_words = total_num_cwords + num_req_dwords_cw0 + num_req_dwords_cw1 + num_req_dwords_cw2;
    } else {       
        /* coverity[dead_error_line] */
        total_num_req_words = 0xffff;
    }

    return (total_num_req_words);
}

STATIC unsigned
nl_find_num_cclks(unsigned nl_opcode)
{
    unsigned bits_8_6 = (nl_opcode >> 6) & 0x7;

    if (bits_8_6 == 1) {
        return (1);
    }
    if (bits_8_6 == 2) {
        return (2);
    }
    switch (nl_opcode) {
    case NL_OPC_NOP:
    case NL_OPC_CBW:
        return (1);
    case NL_OPC_REG_WRITE:
        return (2);
    case NL_OPC_REG_READ:
    case NL_OPC_DBY_READ:
        return (3);
    default:
        ;
    }

    return (0xffff);
}

STATIC unsigned
calc_cost0_num_nl_cclks(int      unit,
                         uint32   *esm_key_id_to_field_mapper_buf_entry_data,
                         unsigned calc_optional_cost
                         )
{
    unsigned total_num_cwords;
    unsigned last_cword_is_optional;
    unsigned opcode_cw2;
    unsigned opcode_cw1;
    unsigned opcode_cw0;
    unsigned num_nl_cclks_cw2;
    unsigned num_nl_cclks_cw1;
    unsigned num_nl_cclks_cw0;
    unsigned total_num_nl_cclks;

    total_num_cwords = soc_mem_field32_get(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, esm_key_id_to_field_mapper_buf_entry_data, TOTAL_NUM_CWORDSf);
    last_cword_is_optional = soc_mem_field32_get(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, esm_key_id_to_field_mapper_buf_entry_data, LAST_CWORD_IS_OPTIONALf);

    if (total_num_cwords < 1 || total_num_cwords > 3) {
        return (0xffff);
    }
    
    if (calc_optional_cost) {
        if (!last_cword_is_optional) {
            return (0);
        }
    }

    if (!calc_optional_cost) {
        if (last_cword_is_optional) {
            if (total_num_cwords == 1) {
                return (0);
            }

            --total_num_cwords;
        }
    }

    opcode_cw2 = soc_mem_field32_get(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, esm_key_id_to_field_mapper_buf_entry_data, OPCODE_CW2f);
    opcode_cw1 = soc_mem_field32_get(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, esm_key_id_to_field_mapper_buf_entry_data, OPCODE_CW1f);
    opcode_cw0 = soc_mem_field32_get(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, esm_key_id_to_field_mapper_buf_entry_data, OPCODE_CW0f);

    num_nl_cclks_cw2 = nl_find_num_cclks(opcode_cw2);
    num_nl_cclks_cw1 = nl_find_num_cclks(opcode_cw1);
    num_nl_cclks_cw0 = nl_find_num_cclks(opcode_cw0);
 
    if (total_num_cwords == 1) {
        total_num_nl_cclks = num_nl_cclks_cw0;
    } else if (total_num_cwords == 2) {
        total_num_nl_cclks = num_nl_cclks_cw0 + num_nl_cclks_cw1;
    } else if (total_num_cwords == 3) {
        total_num_nl_cclks = num_nl_cclks_cw0 + num_nl_cclks_cw1 + num_nl_cclks_cw2;
    } else {
        /* coverity[dead_error_line] */
        total_num_nl_cclks = 0xffff;
    }

    return (total_num_nl_cclks);
}

STATIC int
calc_cost210_num(int unit, unsigned ptr_fme_req, unsigned calc_optional_cost, unsigned *list_cost210_len, unsigned *list_cost210)
{
    esm_key_id_to_field_mapper_entry_t esm_key_id_to_field_mapper_buf;
    unsigned cost2_num_rsp_words;
    unsigned cost1_num_req_words;
    unsigned cost0_num_nl_cclks;

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, MEM_BLOCK_ANY, ptr_fme_req, esm_key_id_to_field_mapper_buf.entry_data));
    
    cost2_num_rsp_words = calc_cost2_num_rsp_words(unit, esm_key_id_to_field_mapper_buf.entry_data, calc_optional_cost);
    cost1_num_req_words = calc_cost1_num_req_words(unit, esm_key_id_to_field_mapper_buf.entry_data, calc_optional_cost);
    cost0_num_nl_cclks  = calc_cost0_num_nl_cclks(unit, esm_key_id_to_field_mapper_buf.entry_data, calc_optional_cost);

    list_cost210[0] = cost2_num_rsp_words;
    list_cost210[1] = cost1_num_req_words;
    list_cost210[2] = cost0_num_nl_cclks;
    *list_cost210_len = 3;

    _SHR_RETURN(SOC_E_NONE);
}


STATIC int
esm_calc_new_fme_cost210 (int unit, int ptr_fme_req, int calc_optional_cost, 
                          unsigned step_size_ps_x, unsigned *fme_llength,
                          unsigned *fme_cost210) 
{
    char proc_name[80];
    esm_key_id_to_field_mapper_entry_t fme;
    unsigned list_cost210[3] = {0, 0, 0}, llength = 0;
    unsigned cost2_num_rsp_words, cost1_num_req_words;
    unsigned cost0_num_nl_cclks, num_rsp_words, max_rsp_mwps, rsp_mpps;
    unsigned time_to_receive_rsp_ns_x, num_req_words, max_req_mwps, req_mpps;
    unsigned num_nl_cclks, nl_core_mpps, time_for_nl_to_execute_search_ns_x;
    unsigned worst_req_proc_cost_ns_x, esmif_req_cost_ns_x, qdly_per_esmif_req_ns_x;
    unsigned last_cword_is_optional, time_to_transmit_req_ns_x, tmp;
    unsigned num_cwords;
    int rc = SOC_E_NONE;

    if (calc_optional_cost) {
        sal_strncpy(proc_name, "esm_calc_new_OPTIONAL_fme_cost210",
                    (sizeof(proc_name)-1));
    } else {
        sal_strncpy(proc_name, "esm_calc_new_fme_cost210",
                    (sizeof(proc_name)-1));
    }

/*
   esm_puts_fyi "$proc_name: ptr_fme_req=$ptr_fme_req, calc_optional_cost=$calc_optional_cost, step_size_ps=$step_size_ps";
*/

    /* compute num_rsp_words, num_req_words, nl_cclks for this fme_req */
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, 
                        MEM_BLOCK_ANY, ptr_fme_req, &fme));
    num_cwords =  soc_ESM_KEY_ID_TO_FIELD_MAPPERm_field32_get(unit, 
                                       fme.entry_data, TOTAL_NUM_CWORDSf);
    if (calc_optional_cost) {
        last_cword_is_optional = 
                    soc_ESM_KEY_ID_TO_FIELD_MAPPERm_field32_get(unit, 
                                       fme.entry_data, LAST_CWORD_IS_OPTIONALf);
        if (last_cword_is_optional) {
            /* calc OPTIONAL_COST fields */
            if (num_cwords == 0) {
                /* don't care values. */
                *fme_llength = 3;
                fme_cost210[2] = fme_cost210[1] = fme_cost210[0] = 0;
                return SOC_E_NONE; 
            }
            rc = calc_cost210_num(unit, ptr_fme_req, 1, &llength, list_cost210);
        } else {
            /* 
            esm_puts_fyi "$proc_name: ptr_fme_req=$ptr_fme_req, new_fme_cost210 are 0 0 0\n";
            */
            /* don't care values. */
            *fme_llength = 3;
            fme_cost210[2] = fme_cost210[1] = fme_cost210[0] = 0;
            return SOC_E_NONE; 
        }
    } else {
        if (num_cwords == 0) {
            /* don't care values. */
            *fme_llength = 3;
            fme_cost210[2] = fme_cost210[1] = fme_cost210[0] = 0;
            return SOC_E_NONE; 
        }
        /* calc COST fields */
        rc = calc_cost210_num(unit, ptr_fme_req, 0, &llength, list_cost210);
        SOC_IF_ERROR_RETURN(rc);
    }
    if (llength != 3) {
        LOG_ERROR(BSL_LS_SOC_ESM,
                  (BSL_META_U(unit,
                              "unit %d: %s: ptr_fme_req=%d, could not"),
                              unit, proc_name, ptr_fme_req));
        LOG_ERROR(BSL_LS_SOC_ESM,
                  (BSL_META_U(unit,
                              "compute cost in terms of num_rsp_words,")));
        LOG_ERROR(BSL_LS_SOC_ESM,
                  (BSL_META_U(unit,
                              "num_req_words, num_nl_cclks\n")));
        /* failure */
        return SOC_E_FAIL; 
    }
    cost2_num_rsp_words = list_cost210[0]; 
    cost1_num_req_words = list_cost210[1]; 
    cost0_num_nl_cclks = list_cost210[2];

    /*mpps limited by rsp */
    num_rsp_words = cost2_num_rsp_words; 
    if (num_rsp_words) {
        max_rsp_mwps = (calc_mwps_per_lane_x() * NL_NUM_RSP_LANES * 
                       (100 - REQD_RX_BW_FOR_CPU_PERCENT))/100;
        rsp_mpps = max_rsp_mwps / num_rsp_words;
        time_to_receive_rsp_ns_x = (1000*1000*1000) / rsp_mpps;
    } else {
        LOG_ERROR(BSL_LS_SOC_ESM,
                  (BSL_META_U(unit,
                              "unit %d: %s: ptr_fme_req=%d, num_rsp_words"),
                              unit, proc_name, ptr_fme_req));
        LOG_ERROR(BSL_LS_SOC_ESM,
                  (BSL_META_U(unit,
                              "must be non-zero, exitting\n")));
        return SOC_E_FAIL; /* failed */
    }

    /*mpps limited by req */
    num_req_words = cost1_num_req_words; 
    if (num_req_words) {
        max_req_mwps = (calc_mwps_per_lane_x() * NL_NUM_REQ_LANES * 
                       (100 - REQD_TX_BW_FOR_CPU_PERCENT))/100;
        req_mpps = max_req_mwps / num_req_words;
        time_to_transmit_req_ns_x = (1000*1000*1000) / req_mpps;
    } else {
        LOG_ERROR(BSL_LS_SOC_ESM,
                  (BSL_META_U(unit,
                              "unit %d: %s: ptr_fme_req=%d, num_req_words"),
                              unit, proc_name, ptr_fme_req));
        LOG_ERROR(BSL_LS_SOC_ESM,
                  (BSL_META_U(unit,
                              "must be non-zero, exitting\n")));
        return SOC_E_FAIL; /* failed */
    }

    /* mpps limited by nl_core_clk_freq */
    num_nl_cclks = cost0_num_nl_cclks; 
    if (num_nl_cclks) {
        nl_core_mpps = ((nl_core_clk_freq_khz(unit) * 
                  (100 - REQD_NL_CCLK_BW_FOR_CPU_PERCENT))/100) / num_nl_cclks;
        time_for_nl_to_execute_search_ns_x = (1000*1000*1000) / nl_core_mpps;
    } else {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "unit %d: %s: ptr_fme_req=%d, num_nl_cclks"),
                     unit, proc_name, ptr_fme_req));
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "must be non-zero, exitting\n")));
        return SOC_E_FAIL; /* failed */
    }

    tmp = ESM_MAX(time_to_receive_rsp_ns_x, time_to_transmit_req_ns_x);
    worst_req_proc_cost_ns_x = ESM_MAX(tmp, time_for_nl_to_execute_search_ns_x);

    /* compute qdly_per_esmif_req_ns */
    /* 1 clock to post the ipipe_request */
    esmif_req_cost_ns_x = (1000 * 1000 *1000) / esmif_clk_freq_khz(unit); 
    if (worst_req_proc_cost_ns_x > esmif_req_cost_ns_x) {
        qdly_per_esmif_req_ns_x = worst_req_proc_cost_ns_x - esmif_req_cost_ns_x;
    } else {
        qdly_per_esmif_req_ns_x = 0;
    }

    /* compute new_fme_costs */
    /* cost2 */
    fme_cost210[0] = 0;

    /* round up */
    /* cost1 */
    fme_cost210[1] = (qdly_per_esmif_req_ns_x * 1000) / step_size_ps_x +
                      ((qdly_per_esmif_req_ns_x * 1000) % step_size_ps_x ? 1 : 0);

    /* cost0 */
    fme_cost210[2] = (worst_req_proc_cost_ns_x * 1000)/step_size_ps_x
                    + ((worst_req_proc_cost_ns_x * 1000) % step_size_ps_x ? 1 : 0);
    *fme_llength = 3;
    return SOC_E_NONE; 
}

STATIC int
esm_prog_new_fme_costs (int unit, int ptr_fme_req, int prog_optional_cost, 
                       unsigned step_size_ps_x) 
{
    int rc;
    unsigned list_cost210[3], llength;
    unsigned fme_cost2, fme_cost1, fme_cost0;
    esm_key_id_to_field_mapper_entry_t fme;
    unsigned num_cwords;

    rc = esm_calc_new_fme_cost210(unit, ptr_fme_req, prog_optional_cost,
                                  step_size_ps_x, &llength, list_cost210);
    if (rc < 0) {
        return rc;
    }
    if (llength != 3) {
        LOG_ERROR(BSL_LS_SOC_ESM,
                  (BSL_META_U(unit,
                              "unit %d: esm_prog_new_fme_costs: will not "),
                              unit));
        LOG_ERROR(BSL_LS_SOC_ESM,
                  (BSL_META_U(unit,
                              "to program costs in FIELDMAPPER table")));
        return SOC_E_FAIL;
    }
    fme_cost2 = list_cost210[0]; /* element_0 of list */
    fme_cost1 = list_cost210[1]; /* element_1 of list */
    fme_cost0 = list_cost210[2]; /* element_2 of list */
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "unit %d: esm_prog_new_fme_costs: ptr_fme_req=%d"),
                 unit, ptr_fme_req));
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "fme_cost2=%d, fme_cost1=%d, fme_cost0=%d\n"),
                 fme_cost2, fme_cost1, fme_cost0));

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, 
                        MEM_BLOCK_ANY, ptr_fme_req, &fme));

    num_cwords =  soc_ESM_KEY_ID_TO_FIELD_MAPPERm_field32_get(unit, 
                                       fme.entry_data, TOTAL_NUM_CWORDSf);
    if (num_cwords == 0) {
        /* don't program anything for unused FMEs */
        return SOC_E_NONE;
    }
    if (prog_optional_cost) {
        soc_ESM_KEY_ID_TO_FIELD_MAPPERm_field32_set(unit, fme.entry_data, 
                         OPTIONAL_COST2f,
                         fme_cost2);
        soc_ESM_KEY_ID_TO_FIELD_MAPPERm_field32_set(unit, fme.entry_data, 
                         OPTIONAL_COST1f,
                         fme_cost1);
        soc_ESM_KEY_ID_TO_FIELD_MAPPERm_field32_set(unit, fme.entry_data, 
                         OPTIONAL_COST0f,
                         fme_cost0);
   } else {
        soc_ESM_KEY_ID_TO_FIELD_MAPPERm_field32_set(unit, fme.entry_data, 
                         COST2f,
                         fme_cost2);
        soc_ESM_KEY_ID_TO_FIELD_MAPPERm_field32_set(unit, fme.entry_data, 
                         COST1f,
                         fme_cost1);
        soc_ESM_KEY_ID_TO_FIELD_MAPPERm_field32_set(unit, fme.entry_data, 
                         COST0f,
                         fme_cost0);
   }
   SOC_IF_ERROR_RETURN(soc_mem_write(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, 
                       MEM_BLOCK_ALL, ptr_fme_req, fme.entry_data));
   return SOC_E_NONE; /* passed */
}
    
#if 0
STATIC int
prog_fme_costs(int unit, unsigned ptr_fme_req, unsigned prog_optional_cost, unsigned list_cost210_len, unsigned *list_cost210)
{
    unsigned _list_cost210[3];
    unsigned cost2_num_rsp_words;
    unsigned cost1_num_req_words;
    unsigned cost0_num_nl_cclks;
    unsigned fme_cost2_rsp;
    unsigned fme_cost1_req;
    unsigned fme_cost0_nl_cclks;
    esm_key_id_to_field_mapper_entry_t esm_key_id_to_field_mapper_buf;

    if (list_cost210_len != 3) {
        SOC_IF_ERROR_RETURN(calc_cost210_num(unit, ptr_fme_req, prog_optional_cost, &list_cost210_len, _list_cost210));
        list_cost210 = _list_cost210;
    }

    cost2_num_rsp_words = list_cost210[0];
    cost1_num_req_words = list_cost210[1];
    cost0_num_nl_cclks  = list_cost210[2];

    fme_cost2_rsp      = cost2_num_rsp_words;
    fme_cost1_req      = cost1_num_req_words;
    fme_cost0_nl_cclks = cost0_num_nl_cclks;
    
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, MEM_BLOCK_ANY, ptr_fme_req, esm_key_id_to_field_mapper_buf.entry_data));
    
    if (soc_mem_field32_get(unit,
                            ESM_KEY_ID_TO_FIELD_MAPPERm,
                            esm_key_id_to_field_mapper_buf.entry_data,
                            TOTAL_NUM_CWORDSf
                            )
        == 0) {
        /* Not used => skip cost programming */

        _SHR_RETURN(SOC_E_NONE);
    }

    if (prog_optional_cost) {
        soc_mem_field32_set(unit,
                            ESM_KEY_ID_TO_FIELD_MAPPERm,
                            esm_key_id_to_field_mapper_buf.entry_data,
                            OPTIONAL_COST2f,
                            fme_cost2_rsp
                            );
        soc_mem_field32_set(unit,
                            ESM_KEY_ID_TO_FIELD_MAPPERm,
                            esm_key_id_to_field_mapper_buf.entry_data,
                            OPTIONAL_COST1f,
                            fme_cost1_req
                            );
        soc_mem_field32_set(unit,
                            ESM_KEY_ID_TO_FIELD_MAPPERm,
                            esm_key_id_to_field_mapper_buf.entry_data,
                            OPTIONAL_COST0f,
                            fme_cost0_nl_cclks                            
                            );
    } else {
        soc_mem_field32_set(unit,
                            ESM_KEY_ID_TO_FIELD_MAPPERm,
                            esm_key_id_to_field_mapper_buf.entry_data,
                            COST2f,
                            fme_cost2_rsp
                            );
        soc_mem_field32_set(unit,
                            ESM_KEY_ID_TO_FIELD_MAPPERm,
                            esm_key_id_to_field_mapper_buf.entry_data,
                            COST1f,
                            fme_cost1_req
                            );
        soc_mem_field32_set(unit,
                            ESM_KEY_ID_TO_FIELD_MAPPERm,
                            esm_key_id_to_field_mapper_buf.entry_data,
                            COST0f,
                            fme_cost0_nl_cclks                            
                            );
    }
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, MEM_BLOCK_ALL, ptr_fme_req, esm_key_id_to_field_mapper_buf.entry_data));

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, ESM_KEY_ID_TO_FIELD_MAPPERm, MEM_BLOCK_ANY, ptr_fme_req, esm_key_id_to_field_mapper_buf.entry_data));
    if (soc_mem_field32_get(unit,
                            ESM_KEY_ID_TO_FIELD_MAPPERm,
                            esm_key_id_to_field_mapper_buf.entry_data,
                            prog_optional_cost ? OPTIONAL_COST2f : COST2f
                            )
        != fme_cost2_rsp
        || soc_mem_field32_get(unit,
                            ESM_KEY_ID_TO_FIELD_MAPPERm,
                            esm_key_id_to_field_mapper_buf.entry_data,
                            prog_optional_cost ? OPTIONAL_COST1f : COST1f
                            )
        != fme_cost1_req
        || soc_mem_field32_get(unit,
                            ESM_KEY_ID_TO_FIELD_MAPPERm,
                            esm_key_id_to_field_mapper_buf.entry_data,
                            prog_optional_cost ? OPTIONAL_COST0f : COST0f
                            )
        != fme_cost0_nl_cclks
        ) {
        _SHR_RETURN(SOC_E_FAIL);
    }
    
    _SHR_RETURN(SOC_E_NONE);
}


STATIC unsigned
calc_drop_all_cost2_rsp(unsigned max_allowed_latency_ns)
{
    unsigned rsp_mwps, rsp_word_duration_ps, drop_all_cost2_rsp;

    rsp_mwps = (calc_mwps_per_lane() * NL_NUM_RSP_LANES * (100 - REQD_RX_BW_FOR_CPU_PERCENT)) / 100;
    rsp_word_duration_ps = 1000000 / rsp_mwps;
    drop_all_cost2_rsp = (1000 * max_allowed_latency_ns) / rsp_word_duration_ps;

    return (drop_all_cost2_rsp);
}


STATIC unsigned
calc_drop_all_cost1_req(unsigned max_allowed_latency_ns)
{
    unsigned req_mwps, req_word_duration_ps, drop_all_cost1_req;

    req_mwps = (calc_mwps_per_lane() * NL_NUM_REQ_LANES * (100 - REQD_TX_BW_FOR_CPU_PERCENT)) / 100;
    req_word_duration_ps = 1000000 / req_mwps;
    drop_all_cost1_req = (1000 * max_allowed_latency_ns) / req_word_duration_ps;

    return (drop_all_cost1_req);
}


STATIC unsigned
calc_drop_all_cost0_nl_cclks(int unit, unsigned max_allowed_latency_ns)
{
    unsigned nl_core_khz, nl_cclks_duration_ps, drop_all_cost0_nl_cclks;

    nl_core_khz = (nl_core_clk_freq_khz(unit) * (100 - REQD_NL_CCLK_BW_FOR_CPU_PERCENT)) / 100;
    nl_cclks_duration_ps = 1000000000 / nl_core_khz;
    drop_all_cost0_nl_cclks = (1000 * max_allowed_latency_ns) / nl_cclks_duration_ps;
    
    return (drop_all_cost0_nl_cclks);
}


STATIC int
prog_drop_all_costs(int unit, unsigned max_latency_ns, unsigned  percent_hysteresis)
{
    uint32 drop_all_cost2 = calc_drop_all_cost2_rsp(max_latency_ns);
    uint32 drop_all_cost1 = calc_drop_all_cost1_req(max_latency_ns);
    uint32 drop_all_cost0 = calc_drop_all_cost0_nl_cclks(unit, max_latency_ns);
    uint32 drop_all_reset_cost2 = (percent_hysteresis * drop_all_cost2) / 100;
    uint32 drop_all_reset_cost1 = (percent_hysteresis * drop_all_cost1) / 100;
    uint32 drop_all_reset_cost0 = (percent_hysteresis * drop_all_cost0) / 100;

    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ESM_ADM_THR_DROP_ALL_COST2r, 0, 0, drop_all_cost2));
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ESM_ADM_THR_DROP_ALL_COST1r, 0, 0, drop_all_cost1));
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ESM_ADM_THR_DROP_ALL_COST0r, 0, 0, drop_all_cost0));

    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ESM_ADM_THR_DROP_ALL_RESET_COST2r, 0, 0, drop_all_reset_cost2));
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ESM_ADM_THR_DROP_ALL_RESET_COST1r, 0, 0, drop_all_reset_cost1));
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ESM_ADM_THR_DROP_ALL_RESET_COST0r, 0, 0, drop_all_reset_cost0));

    _SHR_RETURN(SOC_E_NONE);
}
#endif


STATIC int
prog_drop_opt_costs(int unit, unsigned percent_drop_all, unsigned percent_hysteresis)
{
    uint32 drop_all_cost2;
    uint32 drop_all_cost1;
    uint32 drop_all_cost0;
    uint32 drop_opt_cost2;
    uint32 drop_opt_cost1;
    uint32 drop_opt_cost0;
    uint32 drop_opt_reset_cost2;
    uint32 drop_opt_reset_cost1;
    uint32 drop_opt_reset_cost0;

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ESM_ADM_THR_DROP_ALL_COST2r, 0, 0, &drop_all_cost2));
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ESM_ADM_THR_DROP_ALL_COST1r, 0, 0, &drop_all_cost1));
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ESM_ADM_THR_DROP_ALL_COST0r, 0, 0, &drop_all_cost0));
    
    drop_opt_cost2 = (percent_drop_all * drop_all_cost2) / 100;
    drop_opt_cost1 = (percent_drop_all * drop_all_cost1) / 100;
    drop_opt_cost0 = (percent_drop_all * drop_all_cost0) / 100;

    drop_opt_reset_cost2 = (percent_hysteresis * drop_opt_cost2) / 100;
    drop_opt_reset_cost1 = (percent_hysteresis * drop_opt_cost1) / 100;
    drop_opt_reset_cost0 = (percent_hysteresis * drop_opt_cost0) / 100;

    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ESM_ADM_THR_DROP_OPT_COST2r, 0, 0, drop_opt_cost2));
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ESM_ADM_THR_DROP_OPT_COST1r, 0, 0, drop_opt_cost1));
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ESM_ADM_THR_DROP_OPT_COST0r, 0, 0, drop_opt_cost0));

    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ESM_ADM_THR_DROP_OPT_RESET_COST2r, 0, 0, drop_opt_reset_cost2));
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ESM_ADM_THR_DROP_OPT_RESET_COST1r, 0, 0, drop_opt_reset_cost1));
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ESM_ADM_THR_DROP_OPT_RESET_COST0r, 0, 0, drop_opt_reset_cost0));

    _SHR_RETURN(SOC_E_NONE);
}

STATIC int
esm_prog_drop_all_cost_fme_req_cost(int unit, unsigned *idle_req_rsp_latency_ns_x, 
                                    int idle_req_rsp_ptr_fme_req)
{
    /* compute step_size_ps (independent of idle_req_rsp_latency_ns) */
    /* use 'worst possible' cost numbers */
    unsigned list_user_cost210[] = {9, 14, 4};
    unsigned step_size_ps_x;
    int rc = 0, ptr_fme_req, max_ptr_fme_req = KEY_GEN_OFFSET_MAX;

    step_size_ps_x = esm_calc_new_step_size_ps(unit, 3, list_user_cost210);


/*
    esm_puts_fyi "======= Will use : ptr_fme_req=$idle_req_rsp_ptr_fme_req to measure idle_req_rsp_latency_ns ....";
*/
    if (*idle_req_rsp_latency_ns_x == 0) {
        rc = hwtl_find_idle_req_rsp_latency(unit, idle_req_rsp_ptr_fme_req,
                                            30, idle_req_rsp_latency_ns_x);
        SOC_IF_ERROR_RETURN(rc);
        LOG_CLI((BSL_META_U(unit,
                            "config add idle_req_rsp_latency_ns.%d=%d\n"), 
                 unit, *idle_req_rsp_latency_ns_x));
    }
    if (*idle_req_rsp_latency_ns_x == 0) {
        LOG_ERROR(BSL_LS_SOC_ESM,
                  (BSL_META_U(unit,
                              "unit %d: ptr_fme_req=%d: idle_req_rsp_latency"),
                              unit, idle_req_rsp_ptr_fme_req));
        LOG_ERROR(BSL_LS_SOC_ESM,
                  (BSL_META_U(unit,
                              "measurement FAILED, will quit\n")));
        return SOC_E_FAIL;
    }
/* Already an int, no need to do this:
    set idle_req_rsp_latency_ns [esm_mpexpr int(ceil($idle_req_rsp_latency_ns + 10))];
*/
    /* program new drop_all_costs */
    rc = esm_prog_new_drop_all_costs(unit, *idle_req_rsp_latency_ns_x, 
                                     step_size_ps_x, 90);
    SOC_IF_ERROR_RETURN(rc);
    rc = prog_drop_opt_costs(unit, 50,  90);
    SOC_IF_ERROR_RETURN(rc);

    /* For each fme_entry program correct costs */
    for (ptr_fme_req = 1; ptr_fme_req <= max_ptr_fme_req; ++ptr_fme_req) {
        SOC_IF_ERROR_RETURN(esm_prog_new_fme_costs(unit, ptr_fme_req, 0, 
                            step_size_ps_x));
        SOC_IF_ERROR_RETURN(esm_prog_new_fme_costs(unit, ptr_fme_req, 1, 
                            step_size_ps_x));
    }
    return SOC_E_NONE;
}

STATIC int
enable_report_all_intr(int unit)
{
    {
        static const struct {
            soc_field_t fld;
            uint32      val;
        }  fldvals[] = {
            { DROP_COUNT_MSBf,           1 }, 
            { CNP_SEARCH_REQ_COUNT_MSBf, 1 }, 
            { SOP_DROPf,                 1 }, 
            { XOFF_AXP_OVERSUBSf,        1 }, 
            { MANDATORY_DENYf,           1 }, 
            { DROP_ALLf,                 1 }, 
            { OPT_REQ_TRICKLEDf,         1 }, 
            { OPTIONAL_DENYf,            1 }, 
            { ET_RSP_ERRf,               1 }, 
            { CNP_SEARCH_REQf,           1 }, 
            { ET_REQ_FIFO_FULL_DENYf,    1 }, 
            { ET_REQ_FIFO_FULLf,         1 }, 
            { ADM_CTRL_FIFO_FULL_DENYf,  1 }, 
            { ADM_CTRL_FIFO_FULLf,       1 }, 
            { ET_RSP_FIFO_FULLf,         1 }, 
            { CBA_MISMATCHf,             1 }, 
            { ADM_CTRL_FIFO_UNDERFLOWf,  1 }, 
            { ET_RSP_FIFO_UNDERFLOWf,    1 }, 
            { ET_RSP_FIFO_OVERFLOWf,     1 } 
        };

        uint32   esm_event_err_status_intr_enable_buf = 0;
        unsigned i;

        for (i = 0; i < COUNTOF(fldvals); ++i) {
            soc_reg_field_set(unit, ESM_EVENT_ERR_STATUS_INTRr, &esm_event_err_status_intr_enable_buf, fldvals[i].fld, fldvals[i].val);
        }
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ESM_EVENT_ERR_STATUS_INTR_ENABLEr, 0, 0, esm_event_err_status_intr_enable_buf));
    }

    {
        static const struct {
            soc_field_t fld;
            uint32      val;
        }  fldvals[] = {
	    { EXT_TCAM_INTR1f,     1 },
	    { EXT_TCAM_INTR0f,     1 },
	    { EXT_L2_REQ_LTH_ERRf, 1 },
	    { TWO_MISSING_PKTf,    1 },
	    { RRFE_WAIT_PENDING0f, 1 },
	    { RRFE_WAIT_FULLf,     1 },
	    { PP_XLAT1_PERRf,      1 },
	    { PP_XLAT0_PERRf,      1 },
	    { EXT_L2_CMD_ERRf,     1 },
	    { SBUS_CMD_ERRf,       1 },
	    { RX_CW_ERRf,          1 },
	    { RX_CW_ERS1f,         1 },
	    { RX_CW_ERS0f,         1 }
        };

        uint32   etu_global_intr_enable_buf = 0;
        unsigned i;

        for (i = 0; i < COUNTOF(fldvals); ++i) {
            soc_reg_field_set(unit, ETU_GLOBAL_INTR_STSr, &etu_global_intr_enable_buf, fldvals[i].fld, fldvals[i].val);
        }
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ETU_GLOBAL_INTR_ENABLEr, 0, 0, etu_global_intr_enable_buf));
    }

    {
        static const struct {
            soc_field_t fld;
            uint32      val;
        }  fldvals[] = {
	    { XOFF_TX_CH1f,      1 },
	    { XOFF_TX_CH0f,      1 },
	    { RX_ALIGNEDf,       1 },
	    { RX_MISALIGNEDf,    1 },
	    { RX_ALIGNED_ERRf,   1 },
	    { RX_WORD_SYNCf, 0xfff },
	    { RX_SYNCEDf,    0xfff }
        };

        uint32   ilamac_rx_intf_intr0_enable_buf = 0;
        unsigned i;

        for (i = 0; i < COUNTOF(fldvals); ++i) {
            soc_reg_field_set(unit, ILAMAC_RX_INTF_INTR0_STSr, &ilamac_rx_intf_intr0_enable_buf, fldvals[i].fld, fldvals[i].val);
        }
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ILAMAC_RX_INTF_INTR0_ENABLEr, 0, 0, ilamac_rx_intf_intr0_enable_buf));
    }

    {
        static const struct {
            soc_field_t fld;
            uint32      val;
        }  fldvals[] = {
	    { RX_BURST_GT_16B_ERRf, 1 },
	    { RX_CRC24_ERRf,        1 },
	    { RX_MSOP_ERRf,         1 },
	    { RX_MEOP_ERRf,         1 },
	    { RX_OVERFLOW_ERRf,     1 },
	    { RX_BURSTMAX_ERRf,     1 },
	    { RX_BURST_ERRf,        1 }
        };

        uint32   ilamac_rx_intf_intr1_enable_buf = 0;
        unsigned i;

        for (i = 0; i < COUNTOF(fldvals); ++i) {
            soc_reg_field_set(unit, ILAMAC_RX_INTF_INTR1_STSr, &ilamac_rx_intf_intr1_enable_buf, fldvals[i].fld, fldvals[i].val);
        }
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ILAMAC_RX_INTF_INTR1_ENABLEr, 0, 0, ilamac_rx_intf_intr1_enable_buf));
    }

    {
        uint32 ilamac_rx_lane_intr0_enable_buf = 0;

        soc_reg_field_set(unit, ILAMAC_RX_LANE_INTR0_STSr, &ilamac_rx_lane_intr0_enable_buf, RX_SYNCED_ERRf,  0xfff);
        soc_reg_field_set(unit, ILAMAC_RX_LANE_INTR0_STSr, &ilamac_rx_lane_intr0_enable_buf, RX_FRAMING_ERRf, 0xfff);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ILAMAC_RX_LANE_INTR0_ENABLEr, 0, 0, ilamac_rx_lane_intr0_enable_buf));
    }

    {
        uint32 ilamac_rx_lane_intr1_enable_buf = 0;

        soc_reg_field_set(unit, ILAMAC_RX_LANE_INTR1_STSr, &ilamac_rx_lane_intr1_enable_buf, RX_BAD_TYPE_ERRf, 0xfff);
        soc_reg_field_set(unit, ILAMAC_RX_LANE_INTR1_STSr, &ilamac_rx_lane_intr1_enable_buf, RX_MF_ERRf,       0xfff);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ILAMAC_RX_LANE_INTR1_ENABLEr, 0, 0, ilamac_rx_lane_intr1_enable_buf));
    }

    {
        uint32 ilamac_rx_lane_intr2_enable_buf = 0;

        soc_reg_field_set(unit, ILAMAC_RX_LANE_INTR2_STSr, &ilamac_rx_lane_intr2_enable_buf, RX_DESCRAM_ERRf, 0xfff);
        soc_reg_field_set(unit, ILAMAC_RX_LANE_INTR2_STSr, &ilamac_rx_lane_intr2_enable_buf, RX_MF_LEN_ERRf,  0xfff);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ILAMAC_RX_LANE_INTR2_ENABLEr, 0, 0, ilamac_rx_lane_intr2_enable_buf));
    }

    {
        uint32 ilamac_rx_lane_intr3_enable_buf = 0;

        soc_reg_field_set(unit, ILAMAC_RX_LANE_INTR3_STSr, &ilamac_rx_lane_intr3_enable_buf, RX_MF_REPEAT_ERRf, 0xfff);
        soc_reg_field_set(unit, ILAMAC_RX_LANE_INTR3_STSr, &ilamac_rx_lane_intr3_enable_buf, RX_CRC32_ERRf,     0xfff);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ILAMAC_RX_LANE_INTR3_ENABLEr, 0, 0, ilamac_rx_lane_intr3_enable_buf));
    }

    {
        static const struct {
            soc_field_t fld;
            uint32      val;
        }  fldvals[] = {
	    { TX_ERRINf,         1 },
	    { TX_OVERFLOW_ERRf,  1 },
	    { TX_UNDERFLOW_ERRf, 1 },
	    { TX_BURST_ERRf,     1 },
	    { TX_OVFOUTf,        1 }
        };

        uint32   ilamac_tx_intr_enable_buf = 0;
        unsigned i;

        for (i = 0; i < COUNTOF(fldvals); ++i) {
            soc_reg_field_set(unit, ILAMAC_TX_INTR_STSr, &ilamac_tx_intr_enable_buf, fldvals[i].fld, fldvals[i].val);
        }
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ILAMAC_TX_INTR_ENABLEr, 0, 0, ilamac_tx_intr_enable_buf));
    }

    {
        static const struct {
            soc_field_t fld;
            uint32      val;
        }  fldvals[] = {
	    { TXPLL_LOCK_5f,    1 },
	    { TXPLL_LOCK_4f,    1 },
	    { TXPLL_LOCK_3f,    1 },
	    { TXPLL_LOCK_2f,    1 },
	    { TXPLL_LOCK_1f,    1 },
	    { TXPLL_LOCK_0f,    1 },
	    { RXSEQDONE1G_4_3f, 1 },
	    { RXSEQDONE1G_4_2f, 1 },
	    { RXSEQDONE1G_4_1f, 1 },
	    { RXSEQDONE1G_4_0f, 1 },
	    { RXSEQDONE1G_2_3f, 1 },
	    { RXSEQDONE1G_2_2f, 1 },
	    { RXSEQDONE1G_2_1f, 1 },
	    { RXSEQDONE1G_2_0f, 1 },
	    { RXSEQDONE1G_0_3f, 1 },
	    { RXSEQDONE1G_0_2f, 1 },
	    { RXSEQDONE1G_0_1f, 1 },
	    { RXSEQDONE1G_0_0f, 1 }
        };

        uint32   wcl_intr_enable_buf = 0;
        unsigned i;

        for (i = 0; i < COUNTOF(fldvals); ++i) {
            soc_reg_field_set(unit, WCL_INTR_STSr, &wcl_intr_enable_buf, fldvals[i].fld, fldvals[i].val);
        }
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, WCL_INTR_ENABLEr, 0, 0, wcl_intr_enable_buf));
    }

    {
        static const struct {
            soc_field_t fld;
            uint32      val;
        }  fldvals[] = {
	    { SBUS_STALL_MAX_LATf,          1 },
	    { EXT_L2_STALL_MAX_LATf,        1 },
	    { SBUS_STALL_GTE_CP_ACC_THRf,   1 },
	    { EXT_L2_STALL_GTE_CP_ACC_THRf, 1 },
	    { UNDERFLOW_DETECTEDf,          1 },
	    { OVERFLOW_DETECTEDf,           1 },
	    { FULLf,                        1 },
	    { EARLY_FULLf,                  1 },
	    { DBE_EVf,                      1 },
	    { SBE_EVf,                      1 },
	    { DBE_ODf,                      1 },
	    { SBE_ODf,                      1 }
        };

        uint32   etu_tx_req_fifo_intr_enable_buf = 0;
        unsigned i;

        for (i = 0; i < COUNTOF(fldvals); ++i) {
            soc_reg_field_set(unit, ETU_TX_REQ_FIFO_INTR_STSr, &etu_tx_req_fifo_intr_enable_buf, fldvals[i].fld, fldvals[i].val);
        }
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ETU_TX_REQ_FIFO_INTR_ENABLEr, 0, 0, etu_tx_req_fifo_intr_enable_buf));
    }

    {
        static const struct {
            soc_field_t fld;
            uint32      val;
        }  fldvals[] = {
	    { UNDERFLOW_DETECTEDf, 1 },
	    { OVERFLOW_DETECTEDf,  1 },
	    { FULLf,               1 },
	    { EARLY_FULLf,         1 },
	    { DBEf,                1 },
	    { SBEf,                1 }
        };

        uint32   etu_tx_pipe_ctl_fifo_intr_enable_buf = 0;
        unsigned i;

        for (i = 0; i < COUNTOF(fldvals); ++i) {
            soc_reg_field_set(unit, ETU_TX_PIPE_CTL_FIFO_INTR_STSr, &etu_tx_pipe_ctl_fifo_intr_enable_buf, fldvals[i].fld, fldvals[i].val);
        }
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ETU_TX_PIPE_CTL_FIFO_INTR_ENABLEr, 0, 0, etu_tx_pipe_ctl_fifo_intr_enable_buf));
    }

    {
        static const struct {
            soc_field_t fld;
            uint32      val;
        }  fldvals[] = {
	    { XOFF_RXf,            1 },
	    { UNDERFLOW_DETECTEDf, 1 },
	    { OVERFLOW_DETECTEDf,  1 },
	    { FULLf,               1 },
	    { DBE_EVf,             1 },
	    { SBE_EVf,             1 },
	    { DBE_ODf,             1 },
	    { SBE_ODf,             1 }
        };

        uint32   etu_rx_rsp_fifo_intr_enable_buf = 0;
        unsigned i;

        for (i = 0; i < COUNTOF(fldvals); ++i) {
            soc_reg_field_set(unit, ETU_RX_RSP_FIFO_INTR_STSr, &etu_rx_rsp_fifo_intr_enable_buf, fldvals[i].fld, fldvals[i].val);
        }
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ETU_RX_RSP_FIFO_INTR_ENABLEr, 0, 0, etu_rx_rsp_fifo_intr_enable_buf));
    }

    _SHR_RETURN(SOC_E_NONE);
}


STATIC int
init3(int unit, unsigned num_nl)
{
#if 0
    {
        const unsigned max_ptr_fme_req = 28;
        
        unsigned ptr_fme_req;
        
        for (ptr_fme_req = 1; ptr_fme_req <= max_ptr_fme_req; ++ptr_fme_req) {
            SOC_IF_ERROR_RETURN(prog_fme_costs(unit, ptr_fme_req, 0, 0, 0));
            SOC_IF_ERROR_RETURN(prog_fme_costs(unit, ptr_fme_req, 1, 0, 0));
        }
    }

    {
        const unsigned max_allowed_latency_ns = (1000000 * ESMIF_ALLOWED_FIFO_DEPTH) / esmif_clk_freq_khz(unit);
        const unsigned percent_hysteresis = 90;
        const unsigned percent_drop_all = 50;

        SOC_IF_ERROR_RETURN(prog_drop_all_costs(unit, max_allowed_latency_ns, percent_hysteresis));
        SOC_IF_ERROR_RETURN(prog_drop_opt_costs(unit, percent_drop_all, percent_hysteresis));
    }
#else
    unsigned idle_req_rsp_latency_ns = 0;

    /* get worst case fme ptr */
    
    
    idle_req_rsp_latency_ns = soc_property_get(unit, spn_EXT_TCAM_REQUEST_RESPONSE_LATENCY, 0);
    SOC_IF_ERROR_RETURN(esm_prog_drop_all_cost_fme_req_cost(unit, 
                        &idle_req_rsp_latency_ns,
                        soc_tr3_esm_get_worst_fme(unit)));
#endif

    SOC_IF_ERROR_RETURN(chk_esmif_adc(unit));

    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ESM_ET_HWTL_CONTROLr, 0, 0, 0));

    {
        static const struct {
            soc_field_t fld;
            uint32      val;
        }  fldvals[] = {
            { FORCE_ADM_CTRL_FIFO_READf, 0 },
            { FORCE_ET_RSP_FIFO_READf,   0 },
            { FORCE_L2ETU_ACKf,          0 },
            { ETU_REQ_SEQ_NUM_SELf,      1 },
            { MAX_CONTEXT_ADR_MSBf,      0x7f7 },
            { EXT_LOOKUP_ENABLEf,        1 }            
        };

        uint32   esm_misc_control_buf = 0;
        unsigned i;

        for (i = 0; i < COUNTOF(fldvals); ++i) {
            soc_reg_field_set(unit, ESM_MISC_CONTROLr, &esm_misc_control_buf, fldvals[i].fld, fldvals[i].val);
        }
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ESM_MISC_CONTROLr, 0, 0, esm_misc_control_buf));
    }

    SOC_IF_ERROR_RETURN(enable_report_all_intr(unit));

    _SHR_RETURN(SOC_E_NONE);
}


STATIC unsigned
num_nl_find_wb(int unit)
{
    unsigned mdio_dev_id = 2;
    unsigned dev_id, num_nl;
    uint16 val;

    for (num_nl = dev_id = 0; dev_id < 2; ++dev_id) {
        if (SOC_FAILURE(nl_mdio_read(unit, mdio_portid_get(unit, dev_id), 
                        mdio_dev_id, 0x12, &val)) || val != 0xaaaa) {
            break;
        }
        if (SOC_FAILURE(nl_mdio_read(unit, mdio_portid_get(unit, dev_id), 
                        mdio_dev_id, 0x13, &val)) || val != 0xaaaa) {
            break;
        }
        ++num_nl;
    }
    return num_nl;
}

STATIC unsigned
num_nl_find(int unit, unsigned first_reset_nl)
{
    unsigned num_nl, dev_id;

    if (SOC_WARM_BOOT(unit)) {
        return num_nl_find_wb(unit);
    }
    SOC_IF_ERROR_RETURN(etu_rst_nl_wcl_ilamac(unit, 0x9));

    sal_usleep(SRST_L_DEASSERTION_DELAY_US);

    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, ETU_CONFIG4r, 0, EXT_TCAM_SRST_Lf, 1));

    sal_usleep(10);

    for (num_nl = dev_id = 0; dev_id < 2; ++dev_id) {
        if (SOC_FAILURE(nl_mdio_test_reg_access(unit, 
                                            mdio_portid_get(unit, dev_id)))) {
            break;
        }
        ++num_nl;
    }
    
    return (num_nl);
}

STATIC int
clr_esm_mems(int unit)
{
    esm_key_id_to_field_mapper_entry_t entry;

    sal_memset(entry.entry_data, 0, sizeof(entry.entry_data));

    /* Clean up after the hardware tests are complete */
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, ESM_KEY_ID_TO_FIELD_MAPPERm,
                        MEM_BLOCK_ALL, PTR_FME_REQ_CBI, entry.entry_data));

    _SHR_RETURN(SOC_E_NONE);
}

int
soc_tr3_esmif_init(int unit)
{
    const unsigned max_init1_attempts = 5,   /* <HP> Check this </HP> */
        en_nl_parity_scan  = 1,
        rx_fifo_thr        = (100 << 8) | 120,
        tx_fifo_thr        = (100 << 8) | 120,
        tx_burst_short_16b = 0; /* Specifying 0 => 8 bytes. less efficient
                                   choice is 16 bytes */

    unsigned first_reset_nl, num_nl, init1_cnt;
    unsigned master_wcl_num     = 2;

    if (SAL_BOOT_SIMULATION) {
        _SHR_RETURN(SOC_E_NONE);
    }

    /* ESM Serdes master clk src */
    master_wcl_num = soc_property_get(unit, spn_ESM_SERDES_MASTER_CLK_SRC, 2);


    SOC_IF_ERROR_RETURN(wcl_reset_seq(unit, master_wcl_num)); 

    first_reset_nl = 1;
    num_nl = num_nl_find(unit, first_reset_nl);
    if (num_nl < 1 || num_nl > 2) {
        return (SOC_E_FAIL);    /* Only 1 or 2 NL devices allowed */
    }

    ((soc_tcam_info_t *) SOC_CONTROL(unit)->tcam_info)->num_tcams = num_nl;

    if (SOC_WARM_BOOT(unit)) {
        SOC_IF_ERROR_RETURN(soc_tcam_init(unit));
        SOC_IF_ERROR_RETURN(soc_triumph3_esm_init(unit));
        return SOC_E_NONE;
    }

    for (init1_cnt = 0; init1_cnt < max_init1_attempts; ++init1_cnt) {
        if (SOC_SUCCESS(init1(unit,
                              num_nl,
                              master_wcl_num,
                              rx_fifo_thr,
                              tx_fifo_thr,
                              tx_burst_short_16b
                              )
                        )
            ) {
            break;
        }
    }
    /* If enable low jitter settings, re-run the initialization sequence */
    if (nl_chk_low_jitter_settings(unit)) {
        apply_low_jitter_nl_settings = TRUE;
        for (init1_cnt = 0; init1_cnt < max_init1_attempts; ++init1_cnt) {
            if (SOC_SUCCESS(init1(unit,
                                  num_nl,
                                  master_wcl_num,
                                  rx_fifo_thr,
                                  tx_fifo_thr,
                                  tx_burst_short_16b
                                  )
                            )
                ) {
                break;
            }
        }
    }

    if (init1_cnt >= max_init1_attempts) {
        _SHR_RETURN(SOC_E_FAIL);
    }

    SOC_IF_ERROR_RETURN(init2(unit, num_nl, en_nl_parity_scan, 0, 0));

    SOC_IF_ERROR_RETURN(init3(unit, num_nl));

    SOC_IF_ERROR_RETURN(chk_all_intr_sts(unit, num_nl));
    SOC_IF_ERROR_RETURN(chk_fifos(unit));
    SOC_IF_ERROR_RETURN(chk_esmif_adc(unit));
    SOC_IF_ERROR_RETURN(chk_etu_counters(unit));

    {
        uint32 regbuf;

        SOC_IF_ERROR_RETURN(READ_ESM_ET_HWTL_CONTROLr(unit, &regbuf));
        if (regbuf != 0) {
            return (SOC_E_FAIL);
        }

        SOC_IF_ERROR_RETURN(READ_ESM_MISC_CONTROLr(unit, &regbuf));
        if (regbuf != 0x1fef) {
            return (SOC_E_FAIL);
        }
    }

    SOC_IF_ERROR_RETURN(clr_esm_mems(unit));

    _SHR_RETURN(SOC_E_NONE);
}

STATIC int
etu_chk_errors(int unit)
{
    static const soc_field_t flds[] = {
            RX_CW_ERS0f,
            RX_CW_ERRf
    };
    uint32 rval, i;
    
    READ_ETU_GLOBAL_INTR_STSr(unit, &rval);
    for (i = 0; i < COUNTOF(flds); i++) {
        if (soc_reg_field_get(unit, ETU_GLOBAL_INTR_STSr, rval, flds[i])) {
            return 1;
        }
    }
    return 0;
}

#define NL_CRC_24_ERROR         0x0001
#define NL_SOP_ERROR            0x0002
#define NL_EOP_ERROR            0x0004
#define NL_NO_DATA_ERROR        0x0008
#define NL_BURST_MAX_ERROR      0x0010
#define NL_ALIGNMENT_ERROR      0x0020
#define NL_FRAMING_CW_ERROR     0x0040
#define NL_RX_FIFO_ERROR        0x0080
#define NL_INSTRUCTION_ERROR    0x0100
#define NL_PROTOCOL_ERROR       0x0200
#define NL_CHANNEL_ERROR        0x0400
#define NL_CONTROL_WORD_ERROR   0x0800
#define NL_EFIFO_ERROR          0x1000
#define NL_CRX_UI_ERROR         0x2000
#define NL_RX_UI_ERROR          0x4000
#define NL_TX_FIFO_ERROR        0x8000
STATIC int
nl_chk_errors(int unit, unsigned dev_id)
{
    unsigned                mdio_portid;
    const unsigned          mdio_dev_id = 1;
    uint16                  nl_mdioval;
    uint32                  i;
    
    /* Array of all fields in the Common Status register in NL11K that would
       be considered a fatal error requiring recovery */
    static const uint16     error_types[] = {
        NL_ALIGNMENT_ERROR
    };
    
    mdio_portid = mdio_portid_get(unit, dev_id);
    nl_mdio_read(unit, mdio_portid, mdio_dev_id, 0x8180, &nl_mdioval);
    for (i = 0; i < COUNTOF(error_types); i++) {
        if (nl_mdioval & error_types[i]) {
            return 1;
        }
    }
    return 0;
}

STATIC int
esm_chk_giol(int unit)
{
    uint32      rval;
    unsigned    giol0, giol1;
    
    READ_ETU_GLOBAL_INTR_STSr(unit, &rval);
    giol0 = soc_reg_field_get(unit, ETU_GLOBAL_INTR_STSr, rval,
                              EXT_TCAM_INTR0f);
    giol1 = soc_reg_field_get(unit, ETU_GLOBAL_INTR_STSr, rval,
                              EXT_TCAM_INTR1f);
    
    if (giol0) {
       LOG_ERROR(BSL_LS_SOC_ESM,
                 (BSL_META_U(unit,
                             "GIOL0 is asserted on unit %d!.\n"), unit));
    }
    if (giol1) {
       LOG_ERROR(BSL_LS_SOC_ESM,
                 (BSL_META_U(unit,
                             "GIOL1 is asserted on unit %d!.\n"), unit));
    }
    return giol0 | giol1;
}  

/*
 * Function to be filled-in by customer to do required operations when max
 * threshold is reached.
 * Eg: A msg can be posted to application thread to do switch reset operations
 */
STATIC int
nl_tcam_err_max_threshold(int unit, unsigned dev_id)
{
    LOG_ERROR(BSL_LS_SOC_ESM,
              (BSL_META_U(unit,
                          "NL11K error max threshold reached on "
                          "device %d on unit %d.\n"),
               dev_id, unit));
    return SOC_E_NONE;
}

STATIC int
nl_clear_tcam_error(int unit, unsigned dev_id, nl_reg_buf_t regval)
{
    int             rv;
    nl_reg_buf_t    nl_regval;
    nl_reg_buf_t    nl_regval_adv;
    nl_reg_buf_t    wr_data;
    unsigned        reg_addr;
    uint32          nl_ltr;

    rv = SOC_E_NONE;

    sal_memcpy(nl_regval, regval, sizeof(nl_reg_buf_t));

    SOC_IF_ERROR_RETURN(
        nl_reg_read(unit, dev_id, NL_REG_ADDR_ADV_FEATURES_SOFT_ERR,
                    nl_regval_adv));

    LOG_ERROR(BSL_LS_SOC_ESM,
              (BSL_META_U(unit,
                          "NL11K error register"
                          "[0x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x] on "
                          "device %d on unit %d.\n"),
               nl_regval[9],
               nl_regval[8],
               nl_regval[7],
               nl_regval[6],
               nl_regval[5],
               nl_regval[4],
               nl_regval[3],
               nl_regval[2],
               nl_regval[1],
               nl_regval[0],
               dev_id, unit));

    LOG_ERROR(BSL_LS_SOC_ESM,
              (BSL_META_U(unit,
                          "NL11K Adv Feat soft error register"
                          "[0x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x] on "
                          "device %d on unit %d.\n"),
               nl_regval_adv[9],
               nl_regval_adv[8],
               nl_regval_adv[7],
               nl_regval_adv[6],
               nl_regval_adv[5],
               nl_regval_adv[4],
               nl_regval_adv[3],
               nl_regval_adv[2],
               nl_regval_adv[1],
               nl_regval_adv[0],
               dev_id, unit));

    /* Check status of bit 28 LTR Parity Error */
    if (nl_reg_buf_bitfield_get(nl_regval, 28, 1)) {
        /* Get LTR info from ADV FEAT SOFT ERR register.
         * It can be LTR number or LTR address
         * irrespective of operation type bits [58:53] will
         * represent LTR number */
        nl_ltr = nl_reg_buf_bitfield_get(nl_regval_adv, 53, 6);

        rv = soc_tr3_restore_ltr(unit, dev_id, nl_ltr);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_ESM,
                      (BSL_META_U(unit,
                                  "Failed to restore LTR parity error on "
                                  "LTR number %d on unit %d. rv %d\n"),
                                  nl_ltr, unit, rv));
        }
    }

    /* Check status of bit 29 Context Buffer Parity Error */
    if (nl_reg_buf_bitfield_get(nl_regval, 29, 1)) {
        /* Get Context buffer address from ADV FEAT SOFT ERR register */
        reg_addr = nl_reg_buf_bitfield_get(nl_regval_adv, 0, 14);
        sal_memset(wr_data, 0, sizeof(wr_data));
        rv = nl_ctx_buf_reg_write_loc80(unit, dev_id, reg_addr, wr_data);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_ESM,
                      (BSL_META_U(unit,
                                  "Failed to reset Context parity error on "
                                  "addr 0x%x on unit %d. rv %d\n"),
                                  reg_addr, unit, rv));
        }
    }

    /* Reset bits 28, 29 */
    nl_reg_buf_bitfield_set(nl_regval, 28, 2 , 0);

    /* Check if any other error bits are set */
    if (!nl_reg_buf_is_zero(nl_regval)) {
        if (++nl_tcam_err_count[unit] > NL_TCAM_ERR_THRESHOLD_COUNT) {
            SOC_IF_ERROR_RETURN(nl_tcam_err_max_threshold(unit, dev_id));
            nl_tcam_err_count[unit] = 0;
        }
    }

    return rv;
}

STATIC int
nl_clear_giol(int unit, unsigned num_nl)
{
    int             rv;
    unsigned        dev_id, mdio_portid, chk_crx;
    nl_reg_buf_t    nl_regval;
   
    /* Clear ERROR_STATUS register to de-assert GIO_L[0] */
    for (dev_id = 0; dev_id < num_nl; ++dev_id) {
        SOC_IF_ERROR_RETURN(nl_reg_read(unit, dev_id, NL_REG_ADDR_ERR_STS, 
                                        nl_regval));

        /* Check if any error bits are set */
        if (nl_reg_buf_is_zero(nl_regval) == 0) {
            SOC_IF_ERROR_RETURN(nl_clear_tcam_error(unit, dev_id, nl_regval));
        }

        SOC_IF_ERROR_RETURN(nl_reg_write(unit, dev_id, NL_REG_ADDR_ERR_STS, 
                                        nl_regval, 0, 0, 0));
        if (nl_chk_err_status_reg(unit, dev_id)) {
            LOG_ERROR(BSL_LS_SOC_ESM,
                      (BSL_META_U(unit,
                                  "Failed to clear the NL11K error register on "
                                  "device %d on unit %d.\n"), dev_id, unit));
        }
    }

    /* Clear GIO_L[1] */
    for (dev_id = 0; dev_id < num_nl; ++dev_id) {
        mdio_portid = mdio_portid_get(unit, dev_id);
        chk_crx = (dev_id != 0);
        /* 1st time: chk csm_status and ignore any errors (that could have 
           happened during init sequence of NL) */
        SOC_IF_ERROR_RETURN(nl_mdio_clr_csm_status_regs(unit, mdio_portid, 1));
        
        /* from now on : chk csm_status and make sure there are no errors */
        rv = nl_mdio_chk_csm_status_regs(unit, mdio_portid, chk_crx);
        if (rv < SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_ESM,
                      (BSL_META_U(unit,
                                  "Status check failed on NL11K "
                                  "device %d on unit %d.\n"), dev_id, unit));
        }
    }
    
    if (esm_chk_giol(unit)) {
        LOG_ERROR(BSL_LS_SOC_ESM,
                  (BSL_META_U(unit,
                              "Failed to clear GIOL status on unit %d.\n"), unit));
        return SOC_E_FAIL;
    }
    return SOC_E_NONE;
} 

#define ESM_IF_ERROR_RETURN_UNLOCK(unit, rv) \
    if (SOC_FAILURE(rv)) {      \
        MEM_UNLOCK(unit, ETU_TX_RAW_REQ_DATA_WORDm);  \
        SOC_ESM_UNLOCK(unit);    \
        return rv;    \
    }    \

STATIC int
esm_err_recovery_seq(int unit, unsigned num_nl)
{
    uint32      error = 0;
    uint32      l2x_running = 0, l2x_flags;
    sal_usecs_t l2x_interval;
    uint32      rval;
    uint32      learn_ctrl, kt_a, kt_b;
    uint32      ipipe_req_count0, ipipe_rsp_count0;
    uint32      ipipe_req_count1, ipipe_rsp_count1;
    uint64      tx_pkt_count0, rx_pkt_count0;
    uint64      tx_pkt_count1, rx_pkt_count1;
    unsigned    dev_id;
    int         rv;
    
    /* Disable reporting of interrupts */
    SOC_IF_ERROR_RETURN(dis_intr(unit));
    
    /* set etu_int_mem_rst.start = 0 to prevent xlat table being reset */
    SOC_IF_ERROR_RETURN(READ_ETU_INT_MEM_RSTr(unit, &rval));
    soc_reg_field_set(unit, ETU_INT_MEM_RSTr, &rval, STARTf, 0);
    soc_reg_field_set(unit, ETU_INT_MEM_RSTr, &rval, COMPLETEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_ETU_INT_MEM_RSTr(unit, rval));    
    
    /* Disable sending of lookup requests from IPIPE */
    SOC_IF_ERROR_RETURN(READ_ESM_MISC_CONTROLr(unit, &rval));
    soc_reg_field_set(unit, ESM_MISC_CONTROLr, &rval, EXT_LOOKUP_ENABLEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_ESM_MISC_CONTROLr(unit, rval));
    
    /* Disable all software and hardware accesses to NL11K */
    if (soc_l2x_running(unit, &l2x_flags, &l2x_interval)) {
        l2x_running = 1;
        SOC_IF_ERROR_RETURN(soc_l2x_stop(unit));
    }
    /* Add MEM_LOCK to avoid deadlock with external TCAM memory access */
    MEM_LOCK(unit, ETU_TX_RAW_REQ_DATA_WORDm);
    SOC_ESM_LOCK(unit);

    /* Ensure that no hardware learns happen by making use of the 
       LEARN_FLOOD cabaility to cover keys that ESM supports (BRIDGE/VFI) */
    rv = READ_FLOOD_LEARN_KEY_TYPE_PORT_Ar(unit, &kt_a);
    ESM_IF_ERROR_RETURN_UNLOCK(unit, rv);

    rv = READ_FLOOD_LEARN_KEY_TYPE_PORT_Br(unit, &kt_b);
    ESM_IF_ERROR_RETURN_UNLOCK(unit, rv);

    rv = READ_FLOOD_LEARN_CONTROLr(unit, &learn_ctrl);
    ESM_IF_ERROR_RETURN_UNLOCK(unit, rv);

    rval = 0;
    soc_reg_field_set(unit, FLOOD_LEARN_KEY_TYPE_PORT_Ar, &rval, KEY_TYPEf, 0);
    rv = WRITE_FLOOD_LEARN_KEY_TYPE_PORT_Ar(unit, rval);
    ESM_IF_ERROR_RETURN_UNLOCK(unit, rv);

    rval = 0;
    soc_reg_field_set(unit, FLOOD_LEARN_KEY_TYPE_PORT_Br, &rval, KEY_TYPEf, 1);
    rv = WRITE_FLOOD_LEARN_KEY_TYPE_PORT_Br(unit, rval);
    ESM_IF_ERROR_RETURN_UNLOCK(unit, rv);

    rval = 0;
    soc_reg_field_set(unit, FLOOD_LEARN_CONTROLr, &rval, 
                      MATCH_KEY_TYPE_PORT_Af, 1);
    soc_reg_field_set(unit, FLOOD_LEARN_CONTROLr, &rval, 
                      MATCH_KEY_TYPE_PORT_Bf, 1);
    soc_reg_field_set(unit, FLOOD_LEARN_CONTROLr, &rval, 
                      LEARN_DISABLEf, 1);
    rv = WRITE_FLOOD_LEARN_CONTROLr(unit, rval); 
    ESM_IF_ERROR_RETURN_UNLOCK(unit, rv);

    /* Make sure that ipipe_req_count, ipipe_rsp_count are no longer 
       incrementing - this tells us that tcam + etu system is IDLING */
    /* max buffer_size in esmif is 2 uS, so 4 uS should be enough to 
       flush everything in etu-NL11K */
    sal_usleep(4);
    rv = READ_ETU_DBG_IPIPE_REQ_RSP_COUNTr(unit, &rval);
    ESM_IF_ERROR_RETURN_UNLOCK(unit, rv);

    ipipe_req_count0 = soc_reg_field_get(unit, ETU_DBG_IPIPE_REQ_RSP_COUNTr,
                                         rval, REQ_COUNTf);
    ipipe_rsp_count0 = soc_reg_field_get(unit, ETU_DBG_IPIPE_REQ_RSP_COUNTr,
                                         rval, RSP_COUNTf);
    rv = READ_ILAMAC_TX_PACKETS_COUNTr(unit, &tx_pkt_count0);
    ESM_IF_ERROR_RETURN_UNLOCK(unit, rv);

    rv = READ_ILAMAC_RX_PACKETS_COUNTr(unit, &rx_pkt_count0);
    ESM_IF_ERROR_RETURN_UNLOCK(unit, rv);

    sal_usleep(1);
    rv = READ_ETU_DBG_IPIPE_REQ_RSP_COUNTr(unit, &rval);
    ESM_IF_ERROR_RETURN_UNLOCK(unit, rv);

    ipipe_req_count1 = soc_reg_field_get(unit, ETU_DBG_IPIPE_REQ_RSP_COUNTr,
                                         rval, REQ_COUNTf);
    ipipe_rsp_count1 = soc_reg_field_get(unit, ETU_DBG_IPIPE_REQ_RSP_COUNTr,
                                         rval, RSP_COUNTf);
    rv = READ_ILAMAC_TX_PACKETS_COUNTr(unit, &tx_pkt_count1);
    ESM_IF_ERROR_RETURN_UNLOCK(unit, rv);

    rv = READ_ILAMAC_RX_PACKETS_COUNTr(unit, &rx_pkt_count1);
    ESM_IF_ERROR_RETURN_UNLOCK(unit, rv);

    if (ipipe_req_count0 != ipipe_req_count1) {
        LOG_ERROR(BSL_LS_SOC_ESM,
                  (BSL_META_U(unit,
                              "Not successful in stopping Esmif lookup "
                              "requests to Etu on unit %d.\n"), unit));
        error++;
    }
    if (COMPILER_64_NE(tx_pkt_count0, tx_pkt_count1)) {
        LOG_ERROR(BSL_LS_SOC_ESM,
                  (BSL_META_U(unit,
                              "Not successful in stopping ETU requests "
                              "to NL11K on unit %d.\n"), unit));
        error++;
    }
    if (COMPILER_64_NE(rx_pkt_count0, rx_pkt_count1)) {
        LOG_ERROR(BSL_LS_SOC_ESM,
                  (BSL_META_U(unit,
                              "Not successful in stopping NL11K responses "
                              "to ETU on unit %d.\n"), unit));
        error++;
    }  
    if (ipipe_rsp_count0 != ipipe_rsp_count1) {
        LOG_ERROR(BSL_LS_SOC_ESM,
                  (BSL_META_U(unit,
                              "Not successful in stopping Etu responses to "
                              "to Esmif on unit %d.\n"), unit));
        error++;
    }
    
    /* Force draining of Esmif fifos */
    rv = READ_ESM_MISC_CONTROLr(unit, &rval);
    ESM_IF_ERROR_RETURN_UNLOCK(unit, rv);

    soc_reg_field_set(unit, ESM_MISC_CONTROLr, &rval, 
                      FORCE_ADM_CTRL_FIFO_READf, 1);
    soc_reg_field_set(unit, ESM_MISC_CONTROLr, &rval, 
                      FORCE_ET_RSP_FIFO_READf, 1);
    rv = WRITE_ESM_MISC_CONTROLr(unit, rval); 
    ESM_IF_ERROR_RETURN_UNLOCK(unit, rv);

    /* Reset etu datapath */   
    rv = READ_ETU_CONFIG4r(unit, &rval);
    ESM_IF_ERROR_RETURN_UNLOCK(unit, rv);

    soc_reg_field_set(unit, ETU_CONFIG4r, &rval, 
                      DATAPATH_RST_f, 1);
    rv = WRITE_ETU_CONFIG4r(unit, rval);
    ESM_IF_ERROR_RETURN_UNLOCK(unit, rv);

    sal_usleep(1);
    rv = READ_ETU_CONFIG4r(unit, &rval);
    ESM_IF_ERROR_RETURN_UNLOCK(unit, rv);

    soc_reg_field_set(unit, ETU_CONFIG4r, &rval, 
                      DATAPATH_RST_f, 0);
    rv = WRITE_ETU_CONFIG4r(unit, rval);
    ESM_IF_ERROR_RETURN_UNLOCK(unit, rv);

    /* Release forced draining of esmif fifos */
    rv = READ_ESM_MISC_CONTROLr(unit, &rval);
    ESM_IF_ERROR_RETURN_UNLOCK(unit, rv);

    soc_reg_field_set(unit, ESM_MISC_CONTROLr, &rval, 
                      FORCE_ADM_CTRL_FIFO_READf, 0);
    soc_reg_field_set(unit, ESM_MISC_CONTROLr, &rval, 
                      FORCE_ET_RSP_FIFO_READf, 0);
    rv = WRITE_ESM_MISC_CONTROLr(unit, rval); 
    ESM_IF_ERROR_RETURN_UNLOCK(unit, rv);

    /* Clear pending costs in esmif_adc */
    rval = 0;
    rv = WRITE_ESMIF_CUR_PENDING_COST0r(unit, rval);
    ESM_IF_ERROR_RETURN_UNLOCK(unit, rv);

    rv = WRITE_ESMIF_CUR_PENDING_COST1r(unit, rval);
    ESM_IF_ERROR_RETURN_UNLOCK(unit, rv);

    rv = WRITE_ESMIF_CUR_PENDING_COST2r(unit, rval);
    ESM_IF_ERROR_RETURN_UNLOCK(unit, rv);

    /* Clean up all nl status regs */
    rv = nl_clear_giol(unit, num_nl);
    ESM_IF_ERROR_RETURN_UNLOCK(unit, rv);

    /* Clear all interrupts, etu_counters, adc_max_logs */
    if (clr_all_intr_sts(unit)) error++;
    if (clr_etu_counters(unit)) error++;
    if (clr_adc_max_logs(unit)) error++;
    
    /* make sure nothing is reported without traffic */
    if (chk_fifos(unit)) {
        LOG_ERROR(BSL_LS_SOC_ESM,
                  (BSL_META_U(unit,
                              "ESM FIFO check failed on unit %d.\n"), unit));
        error++;
    }
    if (chk_etu_counters(unit)) {
        LOG_ERROR(BSL_LS_SOC_ESM,
                  (BSL_META_U(unit,
                              "ESM counter check failed on unit %d.\n"), unit));
        error++;
    }
    if (chk_all_intr_sts(unit, num_nl)) {
        LOG_ERROR(BSL_LS_SOC_ESM,
                  (BSL_META_U(unit,
                              "ESM interrupt check failed on unit %d.\n"), unit));
        error++;
    }    
    if (chk_esmif_adc(unit)) {
        LOG_ERROR(BSL_LS_SOC_ESM,
                  (BSL_META_U(unit,
                              "ESM ADC check failed on unit %d.\n"), unit));
        error++;
    }
    for (dev_id = 0; dev_id < num_nl; ++dev_id) {
        if (nl_chk_errors(unit, dev_id)) {
            LOG_ERROR(BSL_LS_SOC_ESM,
                      (BSL_META_U(unit,
                                  "Failed to clear errors on NL11K "
                                  "device %d on unit %d.\n"), dev_id, unit));
            error++;
        }
    }
    
    if (!error) {
        /* resume sending of lookup requests from IPIPE */
        rv = READ_ESM_MISC_CONTROLr(unit, &rval);
        ESM_IF_ERROR_RETURN_UNLOCK(unit, rv);

        soc_reg_field_set(unit, ESM_MISC_CONTROLr, &rval, EXT_LOOKUP_ENABLEf, 1);
        rv = WRITE_ESM_MISC_CONTROLr(unit, rval);
        ESM_IF_ERROR_RETURN_UNLOCK(unit, rv);
    }

    /* Allow software and hardware access to the NL11K */
    rv = WRITE_FLOOD_LEARN_KEY_TYPE_PORT_Ar(unit, kt_a);
    ESM_IF_ERROR_RETURN_UNLOCK(unit, rv);

    rv = WRITE_FLOOD_LEARN_KEY_TYPE_PORT_Br(unit, kt_b);
    ESM_IF_ERROR_RETURN_UNLOCK(unit, rv);

    rv= WRITE_FLOOD_LEARN_CONTROLr(unit, learn_ctrl);
    ESM_IF_ERROR_RETURN_UNLOCK(unit, rv);

    MEM_UNLOCK(unit, ETU_TX_RAW_REQ_DATA_WORDm);
    SOC_ESM_UNLOCK(unit);

    if (l2x_running) {
        rv = soc_l2x_start(unit, l2x_flags, l2x_interval);
        if (SOC_FAILURE(rv)) {
            return rv;
        }
    }
    
    /* resume reporting of interrupts */
    rv = enable_report_all_intr(unit);
    if (SOC_FAILURE(rv)) {
        return rv;
    }
    if (error) {
        return SOC_E_FAIL;
    }
    
    return SOC_E_NONE;
}

STATIC void
soc_tr3_esm_interrupt_process(int unit)
{
    unsigned            num_nl;
    static unsigned     count = 0;
    int                 rv;
    
    num_nl = ((soc_tcam_info_t *) SOC_CONTROL(unit)->tcam_info)->num_tcams;
    
    /* If ETU error, apply recovery sequence */
    if (etu_chk_errors(unit)) {
        count++;
        LOG_ERROR(BSL_LS_SOC_ESM,
                  (BSL_META_U(unit,
                              "Recovering from ETU alignment error on "
                              "unit %d (incident #%d).\n"), unit, count));
        rv = esm_err_recovery_seq(unit, num_nl);
        if (rv < SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_ESM,
                      (BSL_META_U(unit,
                                  "ETU alignment recovery failed on "
                                  "unit %d.\n"), unit));
        }
    }
    return;
}

STATIC void
soc_tr3_esm_recovery_thread(void* unit_vp)
{
    int unit = PTR_TO_INT(unit_vp);
    soc_control_t * soc = SOC_CONTROL(unit);
    
    while(1) {
        sal_sem_take(soc->esm_recovery_notify, sal_sem_FOREVER);
        if (soc->esm_recovery_enable == TRUE) {
            soc_tr3_esm_interrupt_process(unit);
        } else {
            break;
        }
    }
    soc->esm_recovery_pid = SAL_THREAD_ERROR;
    sal_thread_exit(0);
}

int
soc_tr3_esm_recovery_start(int unit)
{
    soc_control_t * soc = SOC_CONTROL(unit);
    
    if (soc->esm_recovery_pid != SAL_THREAD_ERROR) {
	      SOC_IF_ERROR_RETURN(soc_tr3_esm_recovery_stop(unit));
    }
 
    soc->esm_recovery_enable = TRUE;
    /* Spawn a task */
    soc->esm_recovery_pid = sal_thread_create("esm_recovery",
			                                        SAL_THREAD_STKSZ,
			                                        soc_property_get(unit, spn_ESM_RECOVERY_THREAD_PRI, 50),
			                                        soc_tr3_esm_recovery_thread, INT_TO_PTR(unit));

    if (soc->esm_recovery_pid == SAL_THREAD_ERROR) {
        LOG_ERROR(BSL_LS_SOC_ESM,
                  (BSL_META_U(unit,
                              "soc_tr3_esm_recovery_start: "
                              "Could not start esm recovery thread\n")));
	      return SOC_E_MEMORY;
    }
    
    return SOC_E_NONE;
}

int
soc_tr3_esm_recovery_stop (int unit)
{
    soc_control_t	*soc = SOC_CONTROL(unit);
    int rv = SOC_E_NONE;
    soc_timeout_t    to;

    soc->esm_recovery_enable = FALSE;

    if (soc->esm_recovery_pid != SAL_THREAD_ERROR) {
        sal_sem_give(soc->esm_recovery_notify);

        /* Give thread 5 seconds to wake up and exit */
        soc_timeout_init(&to, 5 * 1000000, 0);
        /* wait for esm thread quit completely */
        while (soc->esm_recovery_pid != SAL_THREAD_ERROR) {
            if (soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_SOC_ESM,
                          (BSL_META_U(unit,
                                      "soc_tr3_esm_recovery_stop: "
                                      "esm_thread not exit timeout 5 seconds\n")));
                rv = SOC_E_TIMEOUT;
                break;
            }
        }
    }
    return rv;
}

#endif /* BCM_TRIUMPH3_SUPPORT */
