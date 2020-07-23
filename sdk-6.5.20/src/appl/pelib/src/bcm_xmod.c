/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        bcm_xmod.c
 * Purpose:     Support Broadcom BCP8806X external PHY.
 */

/*
 *   This module implements an NTSW SDK Phy driver for the BCM8806X PHY.
 *  
 *   LAYERING.
 *
 *   This driver is built on top of the PHYMOD library, which is a PHY
 *   driver library that can operate on a family of PHYs, including
 *   BCM8806X.  PHYMOD can be built in a standalone enviroment and be
 *   provided independently from the SDK.  PHYMOD APIs consist of
 *   "core" APIs and "phy" APIs.
 *
 *
 *   KEY IDEAS AND MAIN FUNCTIONS
 *
 *   Some key ideas in this architecture are:
 *
 *   o A PHMOD "phy" consists of one more more lanes, all residing in a single
 *     core.  An SDK "phy" is a logical port, which can consist of one or
 *     more lanes in a single core, OR, can consist of multiples lanes in
 *     more than one core.
 *   o PHYMOD code is "stateless" in the sense that all calls to PHYMOD
 *     APIs are fully parameterized and no per-phy state is held by a PHYMOD
 *     driver.
 *   o PHYMOD APIs do not map 1-1 with SDK .pd_control_set or .pd_control_get
 *     APIs (nor ".pd_xx" calls, nor to .phy_tsce_per_lane_control_set and
 *     .phy_tsce_per_lane_control_get APIs.
 *
 *   The purpose of this code, then, is to provide the necessary translations
 *   between the SDK interfaces and the PHYMOD interfaces.  This includes:
 * 
 *   o Looping over the cores in a logical PHY in order to operate on the
 *     supporting PHMOD PHYs
 *   o Determining the configuration data for the phy, based on programmed
 *     defaults and SOC properties.
 *   o Managing the allocation and freeing of phy data structures required for
 *     working storage.  Locating these structures on procedure invocation.
 *   o Mapping SDK API concepts to PHYMOD APIs.  In some cases, local state is
 *     required in this module in order to present the legacy API.
 *
 * 
 *   MAIN DATA STRUCTURES
 * 
 *   phy_ctrl_t
 *   The PHY control structure defines the SDK notion of a PHY
 *   (existing) structure owned by SDK phy driver modules.  In order
 *   to support PHYMOD PHYs, one addition was made to this structure
 *   (soc_phymod_ctrl_t phymod_ctrl;)
 */

#include "types.h"
#include "error.h"
#include "portphy.h"
#include "bcm_utils.h"
#include "bcm_phy.h"
#include "bcm_xmod_pe_api.h"

#define FLAGS(_pc) ((_pc)->flags)
#define TO(_pc) ((_pc)->to)
#define FIRMWARE(_pc) ((_pc)->firmware)
#define FIRMWARE_LEN(_pc) ((_pc)->firmware_len)
#if 0
#define DL_DIVIDEND(_pc) ((_pc)->dl_dividend)
#define DL_DIVISOR(_pc) ((_pc)->dl_divisor)
extern int
soc_cmic_rate_param_get(int unit, int *dividend, int *divisor);
extern int
soc_cmic_rate_param_set(int unit, int dividend, int divisor);
#endif

/* PHY flag definitions */
#define PHY8806X_FWDLM 0x1 /* firmware download master */
#define PHY8806X_BMP0  0x2 /* boot master 0 */
#define PHY8806X_BMP1  0x4 /* boot master 1 */
#define PHY8806X_CHM   0x8 /* chip master */

#define WRITE_BLK_SIZE 4

#ifdef BE_HOST
#define HOST2LE32(_x) (BCMSWAP32((_x)))
#define LE2HOST32(_x) (BCMSWAP32((_x)))
#else
#define HOST2LE32(_x) (_x)
#define LE2HOST32(_x) (_x)
#endif

#define MDIO_FW_DOWNLOAD_TIMEOUT   35000000  /* 35000 ms - 35 sec */
#if 0
#define MDIO_XMOD_RESPONSE_TIMEOUT 20000     /* 20 ms */
#else
#define MDIO_XMOD_RESPONSE_TIMEOUT 20000000     /* 5000 ms - 5 sec */
#endif
#define PMD_TEST_FIRMWARE          1

sal_mutex_t sbus_lock;
sal_mutex_t axi_lock;
sal_mutex_t xmod_lock;


extern int bcm_reg_read32(int unit, int module, uint32 addr, uint32 *value);
extern int bcm_reg_write32(int unit, int module, uint32 addr, uint32 value);

int bcm_device_init_leds(int unit, int module);
int bcm_sbus_cmd(int unit,phy_ctrl_t *pc, uint32 reg, uint32 cmd, int ring,  uint32 arg[]);
int _bcm_pmd_enable(int unit, phy_ctrl_t *pc, int sys);
int _bcm_pmd_disable(int unit, phy_ctrl_t *pc, int sys);
int _bcm_pmd_start(int unit, phy_ctrl_t *pc, int sys);
int bcm_firmware_set(int unit, int module, int offset, uint8 *data, int len);
int _bcm_write_to_arm(int unit, phy_ctrl_t *pc, uint32 addr, uint8 *data,int len);
int _bcm_axi_write(int unit, phy_ctrl_t *pc, uint32 addr, uint32 data[], int size);
int _bcm_axi_read(int unit, phy_ctrl_t *pc, uint32 addr, uint32 data[], int size);
int bcm_tsc_reg_read_test_base(int unit, int module, int sys, int pmd, uint32 reg, uint16 *value);
int bcm_tsc_reg_write_test_base(int unit, int module, int sys, int pmd, uint32 reg, uint16 value, uint16 mask);
int bcm_tsc_crc(int unit, int module, int sys, int pmd);
int bcm_xmod_command_send(int unit, phy_ctrl_t *pc, int mode_opcode,  uint32 arg[], int arg_size);
int bcm_xmod_result_recv(int unit, phy_ctrl_t *pc, uint32 result[], int result_size);
int bcm_xmod_command_ack(int unit, phy_ctrl_t *pc);


#define IND_LANE_MODE(_pc) (((_pc)->phy_mode == PHYCTRL_DUAL_LANE_PORT) || ((_pc)->phy_mode == PHYCTRL_ONE_LANE_PORT))  
#define PORT_ADDRESS(_base_addr, _sys, _mig) ( ((_base_addr) & ~(0x1f << 19)) |  ((((_sys) << 3) + ((_mig) << 2)  + 1) << 19) )

/*
 * Function:
 *      bcm_init_t
 * Purpose:
 *      initializes the phy_ctrl_t per port structure 
 *      must be called for each line side port
 * Parameters:
 *      unit - BCM unit number (0).
 *      module - module number. (0,1,2,3)
 * Returns:     
 *      SOC_E_NONE
 */
int
bcm_init_t(int unit, int module)
{
    phy_ctrl_t *pc;

    BCM_LOG_DEBUG("%s() enter unit %d module %d\n", __func__, unit, module);

    if ( (module<0) || (module>=4) ) {
        BCM_LOG_ERR("%s() ERROR.  module (%d) out of range, must be 0 or 1\n", __func__, module);
        return -1;
    }

	pc = EXT_PHY_SW_STATE(unit, module);
    pc->unit = 0;
    pc->module = module;
    pc->phy_id = PHY0_DEV_ID+module;
    if (module==0) {
        pc->flags = PHY8806X_FWDLM | PHY8806X_BMP0 | PHY8806X_CHM;
    } else {
        pc->flags = 0;
    }
    pc->phy_devad = PHY_DEVAD;
    pc->lane_mask = 1<<module;
    pc->to.expire = 0;
    pc->to.usec = 0;
    pc->to.min_polls = 0;
    pc->to.polls = 0;
    pc->to.exp_delay = 0;
    pc->firmware = NULL;
    pc->firmware_len = 0;
    pc->dl_dividend = 0;
    pc->dl_divisor = 0;

    return 0;
}


/*
 * Function:
 *      bcm_initialize
 * Purpose:
 *      initializes the phy_ctrl_t per port structures
 * Parameters:
 *      NA
 * Returns:     
 *      SOC_E_NONE
 */
int
bcm_initialize(void)
{
    int mod;

    BCM_LOG_DEBUG("%s() Initializing bcm structures\n", __func__);

    SBUS_LOCK_INIT;
    AXI_LOCK_INIT;
    XMOD_LOCK_INIT;

    for (mod=0; mod<4; mod++) {
        bcm_init_t(0, mod);
    }

    bcm_device_init_leds(0, 0);

    return 0;
}


/*
 * Function:
 *      bcm_device_init_leds
 * Purpose:
 *      sets the LEDs to Open Drain
 * Parameters:
 *      unit - BCM unit number (0).
 *      module - module number. (0,1)
 * Returns:     
 *      SOC_E_NONE
 */
int
bcm_device_init_leds(int unit, int module)
{
    phy_ctrl_t *pc;
	uint32 topmisctrl2;
	uint32 topmisctrl3;
    int rc = SOC_E_NONE;

    BCM_LOG_DEBUG("%s() enter unit %d module %d\n", __func__, unit, module);

    pc = EXT_PHY_SW_STATE(unit, module);

	SOC_IF_ERROR_RETURN
            (READ_TOP_MISC_CONTROL2_REG(uint, pc, &topmisctrl2));
	BCM_LOG_DEBUG("%s() TOP_MISC_CONTROL2(0x%x) read: 0x%x\n", __func__, montreal2_TOP_MISC_CONTROL2r, topmisctrl2);
	SOC_IF_ERROR_RETURN
            (READ_TOP_MISC_CONTROL3_REG(uint, pc, &topmisctrl3));
	BCM_LOG_DEBUG("%s() TOP_MISC_CONTROL3(0x%x) read: 0x%x\n", __func__, montreal2_TOP_MISC_CONTROL3r, topmisctrl3);

    /* set mig0 LED to reverse polarity & open drain mode */
    topmisctrl3 |= ((1<<14) | (1<<6));

	BCM_LOG_DEBUG("%s() TOP_MISC_CONTROL3(0x%x) write: 0x%x\n", __func__, montreal2_TOP_MISC_CONTROL3r, topmisctrl3);
	SOC_IF_ERROR_RETURN
			(WRITE_TOP_MISC_CONTROL3_REG(unit, pc, topmisctrl3));

	return rc;
}


int
bcm_sbus_mutex_init(void)
{
    BCM_LOG_CLI("%s() enter\n", __func__);
    /* Allocate synchronization structures */
    sbus_lock = bcm_mutex_create("sbus_lock");
    if (sbus_lock == NULL) {
        BCM_LOG_ERR("%s() Error: failed to create SBUS Mutex\n", __func__);
        return -1;
    }
    return 0;
}

void
bcm_sbus_mutex_cleanup(void)
{
    if (sbus_lock != NULL) {
        bcm_mutex_destroy(sbus_lock);
        sbus_lock = NULL;
    }
}

int
bcm_axi_mutex_init(void)
{
    BCM_LOG_CLI("%s() enter\n", __func__);
    /* Allocate synchronization structures */
    axi_lock = bcm_mutex_create("axi_lock");
    if (axi_lock == NULL) {
        BCM_LOG_ERR("%s() Error: failed to create AXI Mutex\n", __func__);
        return -1;
    }
    return 0;
}

void
bcm_axi_mutex_cleanup(void)
{
    if (axi_lock != NULL) {
        bcm_mutex_destroy(axi_lock);
        axi_lock = NULL;
    }
}

int
bcm_xmod_mutex_init(void)
{
    BCM_LOG_CLI("%s() enter\n", __func__);
    /* Allocate synchronization structures */
    xmod_lock = bcm_mutex_create("xmod_lock");
    if (xmod_lock == NULL) {
        BCM_LOG_ERR("%s() Error: failed to create XMOD Mutex\n", __func__);
        return -1;
    }
    return 0;
}

void
bcm_xmod_mutex_cleanup(void)
{
    if (xmod_lock != NULL) {
        bcm_mutex_destroy(xmod_lock);
        xmod_lock = NULL;
    }
}

/*
 * Function:
 *      bcm_device_rev_id
 * Purpose:
 *      gets the toronto device rev id
 * Parameters:
 *      unit - BCM unit number (0).
 *      module - module number. (0,1)
 *      rev_id - toronto device rev id
 * Returns:     
 *      SOC_E_NONE
 */
int
bcm_device_rev_id(int unit, int module, int *rev_id)
{
    phy_ctrl_t *pc;
	uint16	id0, id1;
    int rc = SOC_E_NONE;

    BCM_LOG_DEBUG("%s() enter unit %d module %d\n", __func__, unit, module);

    pc = EXT_PHY_SW_STATE(unit, module);

	SOC_IF_ERROR_RETURN
			(READ_PHY8806X_DEV1_ID0r(unit, pc, &id0));
	SOC_IF_ERROR_RETURN
			(READ_PHY8806X_DEV1_ID1r(unit, pc, &id1));

	BCM_LOG_CLI("%s() id0 0x%x; id1 0x%x\n", __func__, id0, id1);

	*rev_id = id1&0x0f;

	return rc;
}


/*
 * Function:
 *      bcm_device_die_temp
 * Purpose:
 *      gets the toronto die temp
 * Parameters:
 *      unit - BCM unit number (0).
 *      module - module number. (0,1)
 *      temp - toronto die temp
 * Returns:     
 *      SOC_E_NONE
 */
int
bcm_device_die_temp(int unit, int module, int *temp)
{
    phy_ctrl_t *pc;
	uint32 pvtctrl0, pvtctrl1, tctrl0;
    uint32 pvtresult0, pvtresult1, pvtresult2, tresult;
    int max_cur = -100;
    int fval, cur;

    BCM_LOG_DEBUG("%s() enter unit %d module %d\n", __func__, unit, module);

    pc = EXT_PHY_SW_STATE(unit, module);

	SOC_IF_ERROR_RETURN
			(READ_PHY8806X_TOP_PVTMON_CTRL_0_REG(unit, pc, &pvtctrl0));
	SOC_IF_ERROR_RETURN
			(READ_PHY8806X_TOP_PVTMON_CTRL_1_REG(unit, pc, &pvtctrl1));
	SOC_IF_ERROR_RETURN
			(READ_PHY8806X_TOP_TMON_CTRL_0_REG(unit, pc, &tctrl0));

	SOC_IF_ERROR_RETURN
			(READ_PHY8806X_TOP_PVTMON_RESULT_0_REG(unit, pc, &pvtresult0));
	SOC_IF_ERROR_RETURN
			(READ_PHY8806X_TOP_PVTMON_RESULT_1_REG(unit, pc, &pvtresult1));
	SOC_IF_ERROR_RETURN
			(READ_PHY8806X_TOP_PVTMON_RESULT_2_REG(unit, pc, &pvtresult2));
	SOC_IF_ERROR_RETURN
			(READ_PHY8806X_TOP_TMON_RESULT_REG(unit, pc, &tresult));

	BCM_LOG_DEBUG("%s() pvtctrl0 0x%x; pvtctrl1 0x%x; tctrl0 0x%x\n",
            __func__, pvtctrl0, pvtctrl1, tctrl0);
	BCM_LOG_DEBUG("%s() pvtresults0 0x%x; pvtresults1 0x%x; pvtresults2 0x%x; tresult 0x%x\n",
            __func__, pvtresult0, pvtresult1, pvtresult2, tresult);

    /* reset pvtmon */
	SOC_IF_ERROR_RETURN
			(READ_PHY8806X_TOP_PVTMON_CTRL_1_REG(unit, pc, &pvtctrl1));
    pvtctrl1 &= ~(1<<31);
	SOC_IF_ERROR_RETURN
			(WRITE_PHY8806X_TOP_PVTMON_CTRL_1_REG(unit, pc, pvtctrl1));
    /* pull out of reset */
    pvtctrl1 |= (1<<31);
	SOC_IF_ERROR_RETURN
			(WRITE_PHY8806X_TOP_PVTMON_CTRL_1_REG(unit, pc, pvtctrl1));

	SOC_IF_ERROR_RETURN
			(READ_PHY8806X_TOP_PVTMON_CTRL_1_REG(unit, pc, &pvtctrl1));
	BCM_LOG_DEBUG("%s() pvtctrl1 0x%x\n", __func__, pvtctrl1);

    bcm_udelay(1000);

	SOC_IF_ERROR_RETURN
			(READ_PHY8806X_TOP_PVTMON_RESULT_0_REG(unit, pc, &pvtresult0));
	SOC_IF_ERROR_RETURN
			(READ_PHY8806X_TOP_PVTMON_RESULT_1_REG(unit, pc, &pvtresult1));
	SOC_IF_ERROR_RETURN
			(READ_PHY8806X_TOP_PVTMON_RESULT_2_REG(unit, pc, &pvtresult2));
	BCM_LOG_DEBUG("%s() pvtresults0 0x%x; pvtresults1 0x%x; pvtresults2 0x%x\n",
            __func__, pvtresult0, pvtresult1, pvtresult2);

    /* search for max */
    fval = pvtresult0 & 0x3ff;
    cur = (41004000 - (48705 * fval)) / 100000;
	BCM_LOG_DEBUG("%s() tresult0-cur %d\n", __func__, cur);
    if (cur > max_cur)
        max_cur = cur;

    fval = pvtresult1 & 0x3ff;
    cur = (41004000 - (48705 * fval)) / 100000;
	BCM_LOG_DEBUG("%s() tresult1-cur %d\n", __func__, cur);
    if (cur > max_cur)
        max_cur = cur;

    fval = pvtresult2 & 0x3ff;
    cur = (41004000 - (48705 * fval)) / 100000;
	BCM_LOG_DEBUG("%s() tresult2-cur %d\n", __func__, cur);
    if (cur > max_cur)
        max_cur = cur;

    *temp = max_cur;

	return SOC_E_NONE;
}


int
bcm_axi_read_reg(int unit, phy_ctrl_t *pc, uint32 addr, uint32 *data)
{
    int         rc;
    uint32      temp32;

    rc = _bcm_axi_read(unit, pc, addr, &temp32, 1);
    if (rc == SOC_E_NONE) {
        *data = LE2HOST32(temp32);
    }

    return rc;
}

int
bcm_axi_write_reg(int unit, phy_ctrl_t *pc, uint32 addr, uint32 data)
{
    int         rc;
    uint32      temp32;
    
    temp32 = HOST2LE32(data);
    rc = _bcm_axi_write(unit, pc, addr, &temp32, 1);

    return rc;
}

/*
 * Function:
 *      bcm_reset_cpu
 * Purpose:
 *      bring R5 out of reset
 * Parameters:
 *      unit - BCM unit number.
 *      module - module number (0,1,2,3).
 * Returns:
 *      SOC_E_NONE
 */
int
bcm_reset_cpu(int unit, int module)
{
    phy_ctrl_t  *pc;
    int         rc = SOC_E_NONE;
    uint32      temp32;

    BCM_LOG_CLI("%s() bring CPU out of reset\n", __func__);

    pc = EXT_PHY_SW_STATE(unit, module);

    /* a.)  bring all mHosts out of reset */
    SOC_IF_ERROR_RETURN
        (READ_TOP_SOFT_RESET_REG(unit, pc, &temp32));
    BCM_LOG_DEBUG("%s() TOP_SOFT_RESET_REG(0x%x) read 0x%x\n", __func__, montreal2_TOP_SOFT_RESET_REGr, temp32);
    temp32 = 0;
    SOC_IF_ERROR_RETURN
        (WRITE_TOP_SOFT_RESET_REG(unit, pc, temp32));
    BCM_LOG_DEBUG("%s() TOP_SOFT_RESET_REG(0x%x) write 0x%x\n", __func__, montreal2_TOP_SOFT_RESET_REGr, temp32);
    temp32 &= ~(0xf << 6);
    temp32 |= (0x3 << 30) | 0x3f;
    SOC_IF_ERROR_RETURN
        (WRITE_TOP_SOFT_RESET_REG(unit, pc, temp32));
    BCM_LOG_DEBUG("%s() TOP_SOFT_RESET_REG(0x%x) write 0x%x\n", __func__, montreal2_TOP_SOFT_RESET_REGr, temp32);

    /* MHOST 0 CR5 RST CTRL */
    bcm_axi_read_reg(unit, pc, montreal2_RTS_MHOST_0_CR5_RST_CTRLr, &temp32);
    BCM_LOG_DEBUG("%s() montreal2_RTS_MHOST_0_CR5_RST_CTRLr(0x%x) read 0x%x\n", __func__, montreal2_RTS_MHOST_0_CR5_RST_CTRLr, temp32);
    temp32 = 0x1f;
    bcm_axi_write_reg(unit, pc, montreal2_RTS_MHOST_0_CR5_RST_CTRLr, temp32);
    BCM_LOG_DEBUG("%s() montreal2_RTS_MHOST_0_CR5_RST_CTRLr(0x%x) write 0x%x\n", __func__, montreal2_RTS_MHOST_0_CR5_RST_CTRLr, temp32);

    /* MHOST 1 CR5 RST CTRL */
    bcm_axi_read_reg(unit, pc, montreal2_RTS_MHOST_1_CR5_RST_CTRLr, &temp32);
    BCM_LOG_DEBUG("%s() montreal2_RTS_MHOST_1_CR5_RST_CTRLr(0x%x) read 0x%x\n", __func__, montreal2_RTS_MHOST_1_CR5_RST_CTRLr, temp32);
    temp32 = 0x1f;
    bcm_axi_write_reg(unit, pc, montreal2_RTS_MHOST_1_CR5_RST_CTRLr, temp32);
    BCM_LOG_DEBUG("%s() montreal2_RTS_MHOST_1_CR5_RST_CTRLr(0x%x) write 0x%x\n", __func__, montreal2_RTS_MHOST_1_CR5_RST_CTRLr, temp32);

    /* MHOST 2 CR5 RST CTRL */
    bcm_axi_read_reg(unit, pc, montreal2_RTS_MHOST_2_CR5_RST_CTRLr, &temp32);
    BCM_LOG_DEBUG("%s() montreal2_RTS_MHOST_2_CR5_RST_CTRLr(0x%x) read 0x%x\n", __func__, montreal2_RTS_MHOST_2_CR5_RST_CTRLr, temp32);
    temp32 = 0x1f;
    bcm_axi_write_reg(unit, pc, montreal2_RTS_MHOST_2_CR5_RST_CTRLr, temp32);
    BCM_LOG_DEBUG("%s() montreal2_RTS_MHOST_2_CR5_RST_CTRLr(0x%x) write 0x%x\n", __func__, montreal2_RTS_MHOST_2_CR5_RST_CTRLr, temp32);

    /* MHOST 3 CR5 RST CTRL */
    bcm_axi_read_reg(unit, pc, montreal2_RTS_MHOST_3_CR5_RST_CTRLr, &temp32);
    BCM_LOG_DEBUG("%s() montreal2_RTS_MHOST_3_CR5_RST_CTRLr(0x%x) read 0x%x\n", __func__, montreal2_RTS_MHOST_3_CR5_RST_CTRLr, temp32);
    temp32 = 0x1f;
    bcm_axi_write_reg(unit, pc, montreal2_RTS_MHOST_3_CR5_RST_CTRLr, temp32);
    BCM_LOG_DEBUG("%s() montreal2_RTS_MHOST_3_CR5_RST_CTRLr(0x%x) write 0x%x\n", __func__, montreal2_RTS_MHOST_3_CR5_RST_CTRLr, temp32);

    SOC_IF_ERROR_RETURN
        (READ_TOP_SOFT_RESET_REG_2(unit, pc, &temp32));
    BCM_LOG_DEBUG("READ_TOP_SOFT_RESET_REG_2(0x%x) 0x%x\n", montreal2_TOP_SOFT_RESET_REG_2r, temp32);
        temp32 |= 0x115;
    SOC_IF_ERROR_RETURN
        (WRITE_TOP_SOFT_RESET_REG_2(unit, pc, temp32));
    BCM_LOG_DEBUG("WRITE_TOP_SOFT_RESET_REG_2(0x%x) 0x%x\n", montreal2_TOP_SOFT_RESET_REG_2r, temp32);

    return rc;
}


/*
 * Function:
 *      bcm_start_cpu
 * Purpose:
 *      bring R5 out of reset
 * Parameters:
 *      unit - BCM unit number.
 *      module - module number (0,1,2,3).
 * Returns:
 *      SOC_E_NONE
 */
int
bcm_start_cpu(int unit, int module)
{
    phy_ctrl_t  *pc;
    int         rc = SOC_E_NONE;
    uint32      temp32;

    BCM_LOG_CLI("%s() Start CPU %d\n", __func__, module);

    pc = EXT_PHY_SW_STATE(unit, module);

    if ( FLAGS(pc) & (PHY8806X_BMP0 | PHY8806X_BMP1) ) {
        /* BMP module, CPU already started */
        BCM_LOG_DEBUG("%s() CPU %d already started\n", __func__, module);
        return rc;
    }

    /* Start all non BMPs */
    if ( !( FLAGS(pc) & (PHY8806X_BMP0 | PHY8806X_BMP1)) ) {

        SOC_IF_ERROR_RETURN
            (READ_TOP_SOFT_RESET_REG(unit, pc, &temp32));
            temp32 |= 1 << ((pc->phy_id & 0x7) + 6);
        SOC_IF_ERROR_RETURN
            (WRITE_TOP_SOFT_RESET_REG(unit, pc, temp32));

        SOC_IF_ERROR_RETURN
            (READ_TOP_RTS_MISC_CTRL_REG(unit, pc, &temp32));
            temp32 &= ~(0x3 << ((pc->phy_id & 0x7) << 1));
            temp32 |= (0x2 << ((pc->phy_id & 0x7) << 1));
        SOC_IF_ERROR_RETURN
            (WRITE_TOP_RTS_MISC_CTRL_REG(unit, pc, temp32));

        SOC_IF_ERROR_RETURN
            (READ_TOP_SOFT_RESET_REG(unit, pc, &temp32));
            temp32 &= ~(1 << ((pc->phy_id & 0x7) + 6));
        SOC_IF_ERROR_RETURN
            (WRITE_TOP_SOFT_RESET_REG(unit, pc, temp32));

    }

    return rc;
}


/*
 * Function:
 *      bcm_firmware_download
 * Purpose:
 *      Download the FW
 * Parameters:
 *      unit - BCM unit number (0).
 *      module - module number. (0)
 * Requirements:
 *      Must have called bcm_initialize() first.
 *      Must load phy_ctrl_t.firmware
 *      Must load phy_ctrl_t.firmware_len
 *      ONLY CALL ONCE with the parameters specified above.
 * Returns:     
 *      SOC_E_NONE
 */
int
bcm_firmware_download(int unit, int module, void *data, int len)
{
    phy_ctrl_t *pc;
    int rc = SOC_E_NONE;
    uint32 temp32;

    BCM_LOG_DEBUG("%s() enter unit %d module %d data 0x%p len %d\n", __func__, unit, module, data, len);

    pc = EXT_PHY_SW_STATE(unit, module);

    /* load file into pc struct */
    FIRMWARE(pc) = data;
    FIRMWARE_LEN(pc) = len;
 
    BCM_LOG_CLI("%s() Firmware 0x%p Firmware_len %d\n", __func__, FIRMWARE(pc), FIRMWARE_LEN(pc));
    
    /* display top soft reset reg */
    SOC_IF_ERROR_RETURN
        (READ_TOP_SOFT_RESET_REG(unit, pc, &temp32));
    BCM_LOG_DEBUG("%s() TOP_SOFT_RESET_REG() 0x%x\n", __func__, temp32);

    /* download file in steps */

    /* setup */
    rc = bcm_firmware_set(unit, module, PHYCTRL_UCODE_BCST_SETUP, NULL, 0);
    if (rc) {
        BCM_LOG_ERR("%s() PHYCTRL_UCODE_BCST_SETUP FAILED\n", __func__);
        return rc;
    }

    /* uC setup */
    rc = bcm_firmware_set(unit, module, PHYCTRL_UCODE_BCST_uC_SETUP, NULL, 0);
    if (rc) {
        BCM_LOG_ERR("%s() PHYCTRL_UCODE_BCST_uC_SETUP FAILED\n", __func__);
        return rc;
    }

    /* enable */
    rc = bcm_firmware_set(unit, module, PHYCTRL_UCODE_BCST_ENABLE, NULL, 0);
    if (rc) {
        BCM_LOG_ERR("%s() PHYCTRL_UCODE_BCST_ENABLE FAILED\n", __func__);
        return rc;
    }

    /* download */
    rc = bcm_firmware_set(unit, module, PHYCTRL_UCODE_BCST_LOAD, NULL, 0);
    if (rc) {
        BCM_LOG_ERR("%s() PHYCTRL_UCODE_BCST_LOAD FAILED\n", __func__);
        return rc;
    }

    /* end */
    rc = bcm_firmware_set(unit, module, PHYCTRL_UCODE_BCST_END, NULL, 0);
    if (rc) {
        BCM_LOG_ERR("%s() PHYCTRL_UCODE_BCST_END FAILED\n", __func__);
        return rc;
    }

    /* display top soft reset reg */
    SOC_IF_ERROR_RETURN
        (READ_TOP_SOFT_RESET_REG(unit, pc, &temp32));
    BCM_LOG_DEBUG("%s() TOP_SOFT_RESET_REG() 0x%x\n", __func__, temp32);

    /* release cpus reset */
    bcm_start_cpu(unit, 1);
    bcm_start_cpu(unit, 2);
/*     bcm_start_cpu(unit, 3); */

    return rc;
}


/*
 * Function:
 *      bcm_firmware_set
 * Purpose:
 *      xx
 * Parameters:
 *      unit - BCM unit number.
 *      module - module number.
 * Returns:     
 *      SOC_E_NONE
 */
int
bcm_firmware_set(int unit, int module, int offset, uint8 *data, int len)
{
    phy_ctrl_t *pc;
#if 0
    int dividend, divisor
#endif
    int rv = SOC_E_FAIL;
    uint32 temp32;
    uint16 temp16;

    /* BCM_LOG_DEBUG("%s() enter unit %d module %d\n", __func__, unit, module); */

    pc = EXT_PHY_SW_STATE(unit, module);

    if (data) {
        return SOC_E_FAIL;
    }

    switch (offset) {
        case PHYCTRL_UCODE_BCST_SETUP:
            BCM_LOG_DEBUG("%s() PHYCTRL_UCODE_BCST_SETUP\n", __func__);
            /* Turn on broadcast mode */

            if ( FLAGS(pc) & PHY8806X_BMP0 ) {
                /* a.)  bring all mHosts out of reset */
                SOC_IF_ERROR_RETURN
                    (READ_TOP_SOFT_RESET_REG(unit, pc, &temp32));
                    temp32 &= ~(0xf << 6);
                    temp32 |= (0x3 << 30) | 0x3f;
                SOC_IF_ERROR_RETURN
                    (WRITE_TOP_SOFT_RESET_REG(unit, pc, temp32));
                /* b.) make sure that PLLs are locked */


                /* c.) place all mHosts in reset */
                SOC_IF_ERROR_RETURN
                    (READ_TOP_SOFT_RESET_REG(unit, pc, &temp32));
                    temp32 |= (0xf << 6);
                SOC_IF_ERROR_RETURN
                    (WRITE_TOP_SOFT_RESET_REG(unit, pc, temp32));

                /* d.) e.) combined together */
                SOC_IF_ERROR_RETURN
                    (READ_TOP_RTS_MISC_CTRL_REG(unit, pc, &temp32));
                    temp32 &= ~0xff;
                    temp32 |= (0x3 << ((pc->phy_id & 0x3) << 1));
                SOC_IF_ERROR_RETURN
                    (WRITE_TOP_RTS_MISC_CTRL_REG(unit, pc, temp32));

                /* f.)  bring all mHosts out of reset */
                SOC_IF_ERROR_RETURN
                    (READ_TOP_SOFT_RESET_REG(unit, pc, &temp32));
                    temp32 &= ~(0xf << 6);
                SOC_IF_ERROR_RETURN
                    (WRITE_TOP_SOFT_RESET_REG(unit, pc, temp32));

                /* g.) */
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY8806X_AXI_CONTROL_F0_REG(unit, pc, (1U << 9), (1U << 9)));

                SOC_IF_ERROR_RETURN
                    (WRITE_PHY8806X_AXI_ADDR_UPPER_F0_REG(unit, pc, 0x0026 + (pc->phy_id & 0x3) * 0x10));
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY8806X_AXI_ADDR_LOWER_F0_REG(unit, pc, 0x0000));

                SOC_IF_ERROR_RETURN
                    (_bcm_pmd_enable(unit, pc, 0)); /* line */
                SOC_IF_ERROR_RETURN
                   (_bcm_pmd_enable(unit, pc, 1)); /* system */

            } else if ( FLAGS(pc) & PHY8806X_BMP1 ) {
                /* a.) bring all mHosts out of reset */
                SOC_IF_ERROR_RETURN
                    (READ_TOP_SOFT_RESET_REG(unit, pc, &temp32));
                    temp32 &= ~(0xf << 10);
                    temp32 |= (0x3 << 30) | 0x3f;
                SOC_IF_ERROR_RETURN
                    (WRITE_TOP_SOFT_RESET_REG(unit, pc, temp32));
                /* b.) make sure that PLLs are locked */


                /* c.) place all mHosts in reset */
                SOC_IF_ERROR_RETURN
                    (READ_TOP_SOFT_RESET_REG(unit, pc, &temp32));
                    temp32 |= (0xf << 10);
                SOC_IF_ERROR_RETURN
                    (WRITE_TOP_SOFT_RESET_REG(unit, pc, temp32));

                /* d.) e.) combined together */
                SOC_IF_ERROR_RETURN
                    (READ_TOP_RTS_MISC_CTRL_REG(unit, pc, &temp32));
                    temp32 &= ~0xff00;
                    temp32 |= (0x3 << (((pc->phy_id & 0x3) << 1) + 8));
                SOC_IF_ERROR_RETURN
                    (WRITE_TOP_RTS_MISC_CTRL_REG(unit, pc, temp32));

                /* f.) bring all mHosts out of reset */
                SOC_IF_ERROR_RETURN
                    (READ_TOP_SOFT_RESET_REG(unit, pc, &temp32));
                    temp32 &= ~(0xf << 10);
                SOC_IF_ERROR_RETURN
                    (WRITE_TOP_SOFT_RESET_REG(unit, pc, temp32));

                /* g.) */
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY8806X_AXI_CONTROL_F1_REG(unit, pc, (1U << 9), (1U << 9)));

                SOC_IF_ERROR_RETURN
                    (WRITE_PHY8806X_AXI_ADDR_UPPER_F1_REG(unit, pc, 0x0026 + (pc->phy_id & 0x3) * 0x10));
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY8806X_AXI_ADDR_LOWER_F1_REG(unit, pc, 0x0000));

                SOC_IF_ERROR_RETURN
                    (_bcm_pmd_enable(unit, pc, 0)); /* line */
                SOC_IF_ERROR_RETURN
                    (_bcm_pmd_enable(unit, pc, 1)); /* system */

            }
            break;

        case PHYCTRL_UCODE_BCST_uC_SETUP:
            BCM_LOG_DEBUG("%s() PHYCTRL_UCODE_BCST_uC_SETUP\n", __func__);
            break;

        case PHYCTRL_UCODE_BCST_ENABLE:
            BCM_LOG_DEBUG("%s() PHYCTRL_UCODE_BCST_ENABLE\n", __func__);

            /* h.) */
            if ( FLAGS(pc) & PHY8806X_FWDLM ) {
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY8806X_MISC_CONTROL_REG(unit, pc, 0, (1U << 14)));
            }

            if ( FLAGS(pc) & PHY8806X_CHM) {
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY8806X_MISC_CONTROL_REG(unit, pc, (1U << 15), (1U << 15)));
            }

            break;

        case PHYCTRL_UCODE_BCST_LOAD:
            BCM_LOG_DEBUG("%s() PHYCTRL_UCODE_BCST_LOAD\n", __func__);
#if 0
            if (DL_DIVIDEND(pc) && DL_DIVISOR(pc)) {
                 rv = soc_cmic_rate_param_get(unit, &dividend, &divisor);
                if (SOC_SUCCESS(rv)) {
                    rv = soc_cmic_rate_param_set(unit, DL_DIVIDEND(pc), DL_DIVISOR(pc));
                }
            }
#endif

            if ( FLAGS(pc) & PHY8806X_FWDLM ) {
               SOC_IF_ERROR_RETURN
                   (_bcm_write_to_arm(unit, pc, 0, FIRMWARE(pc), FIRMWARE_LEN(pc)));
            }
#if 0
            if (SOC_SUCCESS(rv)) {
                /* rv contains the return value of previous soc_cmic_rate_param_set() */
                SOC_IF_ERROR_RETURN
                    (soc_cmic_rate_param_set(unit, dividend, divisor));
            }
#endif
            if ( FLAGS(pc) & (PHY8806X_BMP0 | PHY8806X_BMP1) ) {
                bcm_timeout_init(&TO(pc), MDIO_FW_DOWNLOAD_TIMEOUT, 0);
            }
            break;

        case PHYCTRL_UCODE_BCST_END:
            BCM_LOG_DEBUG("%s() PHYCTRL_UCODE_BCST_END\n", __func__);
            /* Turn off broadcast mode by reading the following reg. */
            if ( FLAGS(pc) & PHY8806X_CHM ) {
                SOC_IF_ERROR_RETURN
                    (READ_PHY8806X_MISC_CONTROL_REG(unit, pc, &temp16));
            }

            if ( FLAGS(pc) & PHY8806X_FWDLM ) {
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY8806X_MISC_CONTROL_REG(unit, pc, (1U << 14), (1U << 14)));
            }

            if ( FLAGS(pc) & (PHY8806X_BMP0 | PHY8806X_BMP1) ) {
                SOC_IF_ERROR_RETURN
                   (_bcm_pmd_disable(unit, pc, 0)); /* line */
                SOC_IF_ERROR_RETURN
                   (_bcm_pmd_disable(unit, pc, 1)); /* system */

                temp16 = 0;
                do {
                    rv = READ_PHY8806X_FW_TO_SW_MESSAGE_REG(unit, pc, &temp16);
                    if (temp16 | SOC_FAILURE(rv)) {
                        break;
                    }
                } while (!bcm_timeout_check(&TO(pc)));

                BCM_LOG_DEBUG("PHY8806X_FW_TO_SW_MESSAGE_REG S : u=%d p=%d  status=%04x"
                            " !.\n", unit, pc->module, temp16);
                if ((temp16 & 0x5000) != 0x5000) {
                    BCM_LOG_ERR("PHY8806X_FW_TO_SW_MESSAGE_REG failed: u=%d p=%d  status=%04x"
                            " !.\n", unit, pc->module, temp16);
                    /* return SOC_E_FAIL; */
                }
            }

            if ( FLAGS(pc) & PHY8806X_BMP0 ) {
                BCM_LOG_CLI("%s() Start Bootmaster CPU\n", __func__);
                /* c.) place all mHosts in reset */
                SOC_IF_ERROR_RETURN
                    (READ_TOP_SOFT_RESET_REG(unit, pc, &temp32));
                    temp32 |= (0xf << 6);
                SOC_IF_ERROR_RETURN
                    (WRITE_TOP_SOFT_RESET_REG(unit, pc, temp32));

                /* d.) e.) combined together */
                SOC_IF_ERROR_RETURN
                    (READ_TOP_RTS_MISC_CTRL_REG(unit, pc, &temp32));
                    temp32 &= ~0xff;
                    temp32 |= (0x2 << ((pc->phy_id & 0x3) << 1));
                SOC_IF_ERROR_RETURN
                    (WRITE_TOP_RTS_MISC_CTRL_REG(unit, pc, temp32));

                /* f.) Release all mHosts from reset, Only BMP will run. Others halted. */
                SOC_IF_ERROR_RETURN
                    (READ_TOP_SOFT_RESET_REG(unit, pc, &temp32));
                    temp32 &= ~(0xf << 6);
                SOC_IF_ERROR_RETURN
                    (WRITE_TOP_SOFT_RESET_REG(unit, pc, temp32));

                /* not sure if necessary but what the heck */
                SOC_IF_ERROR_RETURN
                    (READ_TOP_SOFT_RESET_REG_2(unit, pc, &temp32));
                BCM_LOG_DEBUG("READ_TOP_SOFT_RESET_REG_2 0x%x\n", temp32);
                    temp32 |= 0x115;
                SOC_IF_ERROR_RETURN
                    (WRITE_TOP_SOFT_RESET_REG_2(unit, pc, temp32));
                BCM_LOG_DEBUG("WRITE_TOP_SOFT_RESET_REG_2 0x%x\n", temp32);

            } else if ( FLAGS(pc) & PHY8806X_BMP1 ) {
                /* c.) place all mHosts in reset */
                SOC_IF_ERROR_RETURN
                    (READ_TOP_SOFT_RESET_REG(unit, pc, &temp32));
                    temp32 |= (0xf << 10);
                SOC_IF_ERROR_RETURN
                    (WRITE_TOP_SOFT_RESET_REG(unit, pc, temp32));

                /* d.) e.) combined together */
                SOC_IF_ERROR_RETURN
                    (READ_TOP_RTS_MISC_CTRL_REG(unit, pc, &temp32));
                    temp32 &= ~0xff00;
                    temp32 |= (0x2 << (((pc->phy_id & 0x3) << 1) + 8));
                SOC_IF_ERROR_RETURN
                    (WRITE_TOP_RTS_MISC_CTRL_REG(unit, pc, temp32));

                /* f.) Release all mHosts from reset, Only BMP will run. Others halted. */
                SOC_IF_ERROR_RETURN
                    (READ_TOP_SOFT_RESET_REG(unit, pc, &temp32));
                    temp32 &= ~(0xf << 10);
                SOC_IF_ERROR_RETURN
                    (WRITE_TOP_SOFT_RESET_REG(unit, pc, temp32));
            }

            if ( FLAGS(pc) & (PHY8806X_BMP0 | PHY8806X_BMP1) ) {
                BCM_LOG_CLI("%s() Start PMDs\n", __func__);
                /* Start PMDs */
                SOC_IF_ERROR_RETURN
                    (_bcm_pmd_start(unit, pc, 0)); /* line */
                SOC_IF_ERROR_RETURN
                    (_bcm_pmd_start(unit, pc, 1)); /* system */

/* #ifdef PMD_TEST_FIRMWARE */
#if 1
                bcm_udelay(1000000);
                /* line */
                bcm_tsc_reg_read_test_base(unit, module, 0, 1, 0xd150, &temp16);
                BCM_LOG_CLI("%s() tsc read: u=%d m=%d Line pmd=1 reg=0xd150 val=x%x\n", __func__, unit, module, temp16);
                /* sys */
                bcm_tsc_reg_read_test_base(unit, module, 1, 1, 0xd150, &temp16);
                BCM_LOG_CLI("%s() tsc read: u=%d m=%d System pmd=1 reg=0xd150 val=x%x\n", __func__, unit, module, temp16);
                /* cmd & status */
                bcm_tsc_reg_read_test_base(unit, module, 0, 1, 0xd03d, &temp16);
                BCM_LOG_CLI("%s() tsc read: u=%d m=%d Line pmd=1 reg=0xd03d val=x%x\n", __func__, unit, module, temp16);
                bcm_tsc_reg_read_test_base(unit, module, 1, 1, 0xd03d, &temp16);
                BCM_LOG_CLI("%s() tsc read: u=%d m=%d System pmd=1 reg=0xd03d val=x%x\n", __func__, unit, module, temp16);

#ifdef PEMODE_BCM_BUILD
                bcm_tsc_crc(unit, module, 0, 1);
                bcm_tsc_crc(unit, module, 1, 1);
#endif /*PEMODE_BCM_BUILD*/
#endif
             }
            break;

        default:
            return SOC_E_PARAM;
            break;
    }

    return SOC_E_NONE;
}


/***********************************************************************
 *
 * PASS-THROUGH NOTIFY ROUTINES
 */

/* Utility functions */

int
_bcm_write_to_arm(int unit, phy_ctrl_t *pc, uint32 addr, uint8 *data, int len)
{
    int n_writes, i;
	int org_devad;

#ifdef PHY8806X_MEASSURE_DOWNLOAD_TIME
    sal_usecs_t t;
    t = sal_time_usecs();
#endif

    n_writes = (len + WRITE_BLK_SIZE -1) / WRITE_BLK_SIZE;
    BCM_LOG_CLI("%s() Write to arm n_writes=%d\n", __func__, n_writes);

	/* save original devad */
	org_devad = pc->phy_devad;
	/* set devad for download */
	pc->phy_devad = (1U << 3);

    for ( i = 0; i < n_writes; i++ ) {

        /* address frames are ignored in the download mode */
        SOC_IF_ERROR_RETURN
            (PHY8806X_REG_WRITE(unit, pc,
				(((uint16)data[0])<<8)|data[1], (((uint16)data[2])<<8)|data[3] ));

        data += WRITE_BLK_SIZE;
    }

	/* restore devad */
	pc->phy_devad = org_devad;

#ifdef PHY8806X_MEASSURE_DOWNLOAD_TIME
    BCM_LOG_CLI("%s() u=%d m=%d raw download took %u us\n", __func__, unit, pc->module, SAL_USECS_SUB(sal_time_usecs(), t));
#endif

    return (SOC_E_NONE);
}


/* _bcm_toplvl_sbus_{read|write}() need to be optimized when the data only transactions are
   implemented in the CMIC MDIO code.
 */
 
int
_bcm_toplvl_sbus_write(int unit, phy_ctrl_t *pc, uint32 reg, uint32 arg[], int size)
{
    uint32 cmd;
    int     rv;

    if ((size < 1) || (size > 4)) {
        return (SOC_E_PARAM);
    }

   cmd = (SBUS_WRITE_REGISTER_CMD_MSG << 26) |
         ((montreal2_BLKTYPE_TOP & 0x7f) << 19) |
         ((4 * size) << 7);

    SBUS_LOCK;
    rv = bcm_sbus_cmd(unit, pc, reg, cmd, 2, arg);
    SBUS_UNLOCK;

    return rv;
}


int
_bcm_toplvl_sbus_read(int unit, phy_ctrl_t *pc, uint32 reg, uint32 arg[], int size)
{
    uint32 cmd;
    int     rv;

    if ((size < 1) || (size > 4)) {
        return (SOC_E_PARAM);
    }

    cmd = (SBUS_READ_REGISTER_CMD_MSG << 26) |
          ((montreal2_BLKTYPE_TOP & 0x7f) << 19) |
          ((4 * size) << 7);

    SBUS_LOCK;
    rv = bcm_sbus_cmd(unit, pc, reg, cmd, 2, arg);
    SBUS_UNLOCK;

    return rv;
}


int
_bcm_fwdl_test(int unit, int module)
{
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, module);

    SOC_IF_ERROR_RETURN
        (_bcm_write_to_arm(unit, pc, 0, FIRMWARE(pc), FIRMWARE_LEN(pc)));

    return (SOC_E_NONE);
}


/* _bcm_axi_{read|write}() need to be optimized when the data only transactions are
   implemented in the CMIC MDIO code.
 */
int
_bcm_axi_write(int unit, phy_ctrl_t *pc, uint32 addr, uint32 data[], int size)
{
    int    count, rv, i;
    uint16 status;
    uint32 temp32;

    if ( addr & 0x3 ) {
        BCM_LOG_ERR("_bcm_axi_write failed: u=%d p=%d "
                    "addr=%08x - unaligned !.\n", unit, pc->module, addr);
        return (SOC_E_FAIL);
    }

    AXI_LOCK;
    SOC_IF_ERROR_PRINT
        (WRITE_PHY8806X_AXI_ADDR_UPPER_FX_REG(unit, pc, (addr >> 16) & 0xffff));
    SOC_IF_ERROR_PRINT
        (WRITE_PHY8806X_AXI_ADDR_LOWER_FX_REG(unit, pc, addr & 0xffff));

    i = 0;
    while (size-- > 0) {
        /* 
           When the current destination address is 8 byte aligned write to  AXI_DATA_BYTES_1_2_FX_REG and 
           AXI_DATA_BYTES_3_4_FX_REG else write to AXI_DATA_BYTES_5_6_FX_REG and AXI_DATA_BYTES_7_8_FX_REG.
         */
        /* The following preserves the byte order. */
        temp32 = HOST2LE32(data[i]);

        if ( (addr + (i << 2)) & 0x7 ) {
            SOC_IF_ERROR_PRINT
                (WRITE_PHY8806X_AXI_DATA_BYTES_5_4_FX_REG(unit, pc, temp32 & 0xffff));
            SOC_IF_ERROR_PRINT
                (WRITE_PHY8806X_AXI_DATA_BYTES_7_6_FX_REG(unit, pc, (temp32 >> 16) & 0xffff));
        } else {
            SOC_IF_ERROR_PRINT
                (WRITE_PHY8806X_AXI_DATA_BYTES_1_0_FX_REG(unit, pc, temp32 & 0xffff));
            SOC_IF_ERROR_PRINT
                (WRITE_PHY8806X_AXI_DATA_BYTES_3_2_FX_REG(unit, pc, (temp32 >> 16) & 0xffff));
        }
        i++;

        SOC_IF_ERROR_PRINT
            (WRITE_PHY8806X_AXI_CONTROL_FX_REG(unit, pc, (5U << 13))); /* go */

        count = 0;

        do {
            rv = READ_PHY8806X_AXI_STATUS_FX_REG(unit, pc, &status);
            if (((status & (1U << 15) ) != 0) ||
                SOC_FAILURE(rv)) {
                break;
            }
        } while (count++ < AXI_MAX_POLLS);

        if (count >= AXI_MAX_POLLS) {
            BCM_LOG_ERR("_bcm_axi_write failed: u=%d p=%d  status=%04x "
                        "AXI_MAX_POLLS reached !.\n", unit, pc->module, status);
            AXI_UNLOCK;
            return (SOC_E_FAIL);
        }

        if ((status & (1U << 9)) != 0) {
            BCM_LOG_ERR("_bcm_axi_write failed: u=%d p=%d "
                        "addr=%04x status=%04x\n", unit, pc->module, addr, status);
            AXI_UNLOCK;
            return (SOC_E_FAIL);
        }
    }

    AXI_UNLOCK;
    return (SOC_E_NONE);
}


int
_bcm_axi_read(int unit, phy_ctrl_t *pc, uint32 addr, uint32 data[], int size)
{
    int    count, rv, i;
    uint16 status;
    uint16 temp0, temp1;
    uint32 temp32;

    if ( addr & 0x3 ) {
        BCM_LOG_ERR("_bcm_axi_read failed: u=%d p=%d "
                    "addr=%08x - unaligned !.\n", unit, pc->module, addr);
        return (SOC_E_FAIL);
    }

    AXI_LOCK;
    SOC_IF_ERROR_PRINT
        (WRITE_PHY8806X_AXI_ADDR_UPPER_FX_REG(unit, pc, (addr >> 16) & 0xffff));
    SOC_IF_ERROR_PRINT
        (WRITE_PHY8806X_AXI_ADDR_LOWER_FX_REG(unit, pc, addr & 0xffff));

    i = 0;
    while (size-- > 0) {
        SOC_IF_ERROR_PRINT
            (WRITE_PHY8806X_AXI_CONTROL_FX_REG(unit, pc, (7U << 13))); /* go */

        count = 0;

        do {
            rv = READ_PHY8806X_AXI_STATUS_FX_REG(unit, pc, &status);
            if (((status & (1U << 15)) != 0) ||
                SOC_FAILURE(rv)) {
                break;
            }
        } while (count++ < AXI_MAX_POLLS);

        if (count >= AXI_MAX_POLLS) {
            BCM_LOG_ERR("_bcm_axi_read failed: u=%d p=%d status=%04x"
                        "AXI_MAX_POLLS reached !.\n", unit, pc->module, status);
            AXI_UNLOCK;
            return (SOC_E_FAIL);
        }

        if ((status & (1U << 9)) != 0) {
            BCM_LOG_ERR("_bcm_axi_read failed: u=%d p=%d "
                        "addr=%08x status=%04x\n", unit, pc->module, addr, status);
            AXI_UNLOCK;
            return (SOC_E_FAIL);
        }

        /* 
           When the current source address is 8 byte aligned read from AXI_DATA_BYTES_1_2_FX_REG and 
           AXI_DATA_BYTES_3_4_FX_REG else read from AXI_DATA_BYTES_5_6_FX_REG and AXI_DATA_BYTES_7_8_FX_REG.
         */
        if ( (addr + (i << 2)) & 0x7 ) {
            SOC_IF_ERROR_PRINT
                (READ_PHY8806X_AXI_DATA_BYTES_5_4_FX_REG(unit, pc, &temp0));
            SOC_IF_ERROR_PRINT
                (READ_PHY8806X_AXI_DATA_BYTES_7_6_FX_REG(unit, pc, &temp1));
        } else {
            SOC_IF_ERROR_PRINT
                (READ_PHY8806X_AXI_DATA_BYTES_1_0_FX_REG(unit, pc, &temp0));
            SOC_IF_ERROR_PRINT
                (READ_PHY8806X_AXI_DATA_BYTES_3_2_FX_REG(unit, pc, &temp1));
        }
        temp32 = (((uint32) temp1) << 16) | temp0;

        /* The following preserves the byte order. */
        data[i] = LE2HOST32(temp32);

        i++;
    }

    AXI_UNLOCK;
    return (SOC_E_NONE);

}


int
bcm_sbus_cmd(int unit, phy_ctrl_t *pc, uint32 reg, uint32 cmd, int ring, uint32 arg[])
{
    int    count, rv, write, size;
    uint16 status, temp0, temp1;

    size = ((cmd >> 7) & 0x7f) >> 2;

    if ((size < 1) || (size > 4)) {
        return (SOC_E_PARAM);
    }


    if (ring > 0x2) {
        BCM_LOG_ERR("bcm_sbus_cmd: u=%d p=%d  : invalid ring !\n", unit, pc->module);
        return (SOC_E_PARAM);
    }

    ring &= 0x3;

    /* BCM_LOG_CLI("bcm_sbus_cmd: u=%d p=%d reg=0x%x cmd=0x%x ring=%d arg[0]=0x%x\n",
              unit, pc->module, reg, cmd, ring, arg[0]); */

    write = (((cmd >> 26) & 0x3f) + 1) & 0x2;

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8806X_SBUS_COMMAND_UPPER_REG(unit, pc, (cmd >> 16) & 0xffff));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8806X_SBUS_COMMAND_LOWER_REG(unit, pc, cmd & 0xffff));

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8806X_SBUS_ADDR_UPPER_REG(unit, pc, (reg >> 16) & 0xffff));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8806X_SBUS_ADDR_LOWER_REG(unit, pc, reg & 0xffff));

    if (write) {
        if (size-- > 0) {
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8806X_SBUS_DATA_BYTES_1_0_REG(unit, pc, arg[0] & 0xffff));
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8806X_SBUS_DATA_BYTES_3_2_REG(unit, pc, (arg[0] >> 16) & 0xffff));
        }

        if (size-- > 0) {
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8806X_SBUS_DATA_BYTES_5_4_REG(unit, pc, arg[1] & 0xffff));
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8806X_SBUS_DATA_BYTES_7_6_REG(unit, pc, (arg[1] >> 16) & 0xffff));
        }

        if (size-- > 0) {
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8806X_SBUS_DATA_BYTES_9_8_REG(unit, pc, arg[2] & 0xffff));
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8806X_SBUS_DATA_BYTES_11_10_REG(unit, pc, (arg[2] >> 16) & 0xffff));
        }

        if (size-- > 0) {
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8806X_SBUS_DATA_BYTES_13_12_REG(unit, pc, arg[3] & 0xffff));
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8806X_SBUS_DATA_BYTES_15_14_REG(unit, pc, (arg[3] >> 16) & 0xffff));
        }
    }

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8806X_SBUS_CONTROL_REG(unit, pc, (1U << 15) | (ring << 12))); /* TOP + go */

    count = 0;

    do {
        rv = READ_PHY8806X_SBUS_STATUS_REG(unit, pc, &status);
        if (((status & (SBUS_STATUS_ERR | SBUS_STATUS_NAK | SBUS_STATUS_DONE)) != 0) ||
            SOC_FAILURE(rv)) {
            break;
        }
    } while (count++ < SCHAN_MAX_POLLS);

    if (count >= SCHAN_MAX_POLLS) {
        BCM_LOG_ERR("bcm_sbus_cmd failed: u=%d p=%d  status=%04x"
                    "SCHAN_MAX_POLLS reached !\n", unit, pc->module, status);
        return (SOC_E_FAIL);
    }

    if ((status & (SBUS_STATUS_ERR | SBUS_STATUS_NAK)) != 0) {
        BCM_LOG_ERR("bcm_sbus_cmd failed: u=%d p=%d "
                    "cmd=0x%04x reg=0x%08x ring=%d d[0]=0x%08x d[1]=0x%08x d[2]=0x%08x d[3]=0x%08x status=%04x\n",
                    unit, pc->module, cmd, reg, ring, arg[0], arg[1], arg[2], arg[3], status);
        return (SOC_E_FAIL);
    }

    if (!write) {

        if (size-- > 0) {
            SOC_IF_ERROR_RETURN
                (READ_PHY8806X_SBUS_DATA_BYTES_1_0_REG(unit, pc, &temp0));
            SOC_IF_ERROR_RETURN
                (READ_PHY8806X_SBUS_DATA_BYTES_3_2_REG(unit, pc, &temp1));
            arg[0] = (((uint32) temp1) << 16) | temp0;
        }

        if (size-- > 0) {
            SOC_IF_ERROR_RETURN
                (READ_PHY8806X_SBUS_DATA_BYTES_5_4_REG(unit, pc, &temp0));
            SOC_IF_ERROR_RETURN
                (READ_PHY8806X_SBUS_DATA_BYTES_7_6_REG(unit, pc, &temp1));
            arg[1] = (((uint32) temp1) << 16) | temp0;
        }

        if (size-- > 0) {
            SOC_IF_ERROR_RETURN
                (READ_PHY8806X_SBUS_DATA_BYTES_9_8_REG(unit, pc, &temp0));
            SOC_IF_ERROR_RETURN
                (READ_PHY8806X_SBUS_DATA_BYTES_11_10_REG(unit, pc, &temp1));
            arg[2] = (((uint32) temp1) << 16) | temp0;
        }

        if (size-- > 0) {
            SOC_IF_ERROR_RETURN
                (READ_PHY8806X_SBUS_DATA_BYTES_13_12_REG(unit, pc, &temp0));
            SOC_IF_ERROR_RETURN
                (READ_PHY8806X_SBUS_DATA_BYTES_15_14_REG(unit, pc, &temp1));
            arg[3] = (((uint32) temp1) << 16) | temp0;
        }
    }

    return (SOC_E_NONE);
}


int
_bcm_pmd_enable(int unit, phy_ctrl_t *pc, int sys)
{
    uint32 data[4], reg_cmd, reg_cmd_1, mem_cmd;
    int ring;
 
    ring = (pc->phy_id & 0x4) ? 1 : 0; /* mig too */

    reg_cmd   = (SBUS_WRITE_REGISTER_CMD_MSG << 26) |(((sys ? 0x6 : 0x2) & 0x7f) << 19) |(4 << 7);
    reg_cmd_1 = (SBUS_WRITE_REGISTER_CMD_MSG << 26) |(((sys ? 0x7 : 0x3) & 0x7f) << 19) |(4 << 7);
    mem_cmd   = (SBUS_WRITE_MEMORY_CMD_MSG   << 26) |(((sys ? 0x7 : 0x3) & 0x7f) << 19) |(16 << 7);

    /* SBUS_LOCK; Don't need to lock sbus here. */
    /* This API is only called in bcm_firmware_set() */
    /* which is called first prior to chip being operational */

    /* CLPORT_POWER_SAVE = 0 */
    data[0] = 0;
    SOC_IF_ERROR_RETURN
        (bcm_sbus_cmd(unit, pc, 0x02020D00, reg_cmd, ring,  data));

    /* CLPORT_XGXS0_CTRL_REG = 0x1C */
    data[0] = 0x1C;
    SOC_IF_ERROR_RETURN
        (bcm_sbus_cmd(unit, pc, 0x02021400, reg_cmd, ring,  data));

    /* CLPORT_XGXS0_CTRL_REG = 0x5 */
    data[0] = 0x5;
    SOC_IF_ERROR_RETURN
        (bcm_sbus_cmd(unit, pc, 0x02021400, reg_cmd, ring,  data));

    /* FCPORT_TOP_CTRL_CONFIG = 0x10 */
    data[0] = 0x10;
    SOC_IF_ERROR_RETURN
        (bcm_sbus_cmd(unit, pc, 0x02000000, reg_cmd_1, ring,  data));

    /* PMD_X1_CONTROL.[1:0] = 0x3 */
    data[0] = PORT_ADDRESS(0x00009010, sys, ring);
    data[1] = 0x0003fffc;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (bcm_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));

    /* CKRST_CTRL_PMD_LANE_RESET_N_PWRDN_PIN_KILL_CONTROL.0 = 0x1 */ 
    data[0] = PORT_ADDRESS(0x0800d0b3, sys, ring);
    data[1] = 0x0001fffe;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (bcm_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));

    /* MICRO_A_COM_CLOCK_CONTROL0.0 = 0x1 */ 
    data[0] = PORT_ADDRESS(0x0800d200, sys, ring);
    data[1] = 0x0001fffe;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (bcm_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));

    /* MICRO_A_COM_RESET_CONTROL0.0 = 0x1, MICRO_A_COM_RESET_CONTROL0.3 = 0x1 */ 
    data[0] = PORT_ADDRESS(0x0800d201, sys, ring);
    data[1] = 0x0009fff6;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (bcm_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));

    /* MICRO_A_COM_AHB_CONTROL0.[9:8] = 0x1 */ 
    data[0] = PORT_ADDRESS(0x0800d202, sys, ring);
    data[1] = 0x0100fcff;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (bcm_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));

    /* tsc_reg_read_test_base(unit, pc->module, sys, 1, 0xd203); */

    /* MICRO_A_COM_AHB_CONTROL0.[9:8] = 0x0 */ 
    data[0] = PORT_ADDRESS(0x0800d202, sys, ring);
    data[1] = 0x0000fcff;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (bcm_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));

    /* MICRO_A_COM_AHB_CONTROL0.[1:0] = 0x2 */ 
    data[0] = PORT_ADDRESS(0x0800d202, sys, ring);
    data[1] = 0x0002fffc;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (bcm_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));

    /* MICRO_A_COM_PRAMIF_CONTROL0.0 = 0x1 */
    data[0] = PORT_ADDRESS(0x0800d20c, sys, ring);
    data[1] = 0x0001fffe;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (bcm_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));

    /* FCPORT_TOP_CTRL_CONFIG = 0x11 */
    data[0] = 0x11;
    SOC_IF_ERROR_RETURN
        (bcm_sbus_cmd(unit, pc, 0x02000000, reg_cmd_1, ring,  data));

    return (SOC_E_NONE);
}

int
_bcm_pmd_disable(int unit, phy_ctrl_t *pc, int sys)
{
    uint32 data[4], reg_cmd_1;
    int ring;

    ring = (pc->phy_id & 0x4) ? 1 : 0;

    reg_cmd_1 = (SBUS_WRITE_REGISTER_CMD_MSG << 26) |(((sys ? 0x7 : 0x3) & 0x7f) << 19) |(4 << 7);

    /* SBUS_LOCK; Don't need to lock sbus here. */
    /* This API is only called in bcm_firmware_set() */
    /* which is called first prior to chip being operational */

    /* FCPORT_TOP_CTRL_CONFIG = 0x10 */
    data[0] = 0x10;
    SOC_IF_ERROR_RETURN
        (bcm_sbus_cmd(unit, pc, 0x02000000, reg_cmd_1, ring,  data));

    return (SOC_E_NONE);
}

int
_bcm_pmd_start(int unit, phy_ctrl_t *pc, int sys)
{
    uint32 data[4], mem_cmd;
    int ring;

    ring = (pc->phy_id & 0x4) ? 1 : 0; /* mig too */

    mem_cmd = (SBUS_WRITE_MEMORY_CMD_MSG   << 26) |(((sys ? 0x7 : 0x3) & 0x7f) << 19) |(16 << 7);

    /* SBUS_LOCK; Don't need to lock sbus here. */
    /* This API is only called in bcm_firmware_set() */
    /* which is called first prior to chip being operational */

    /* MICRO_A_COM_CLOCK_CONTROL0.1 = 0x1 */ 
    data[0] = PORT_ADDRESS(0x0800d200, sys, ring);
    data[1] = 0x0002fffd;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (bcm_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));

    /* MICRO_A_COM_RESET_CONTROL0.1 = 0x1 */ 
    data[0] = PORT_ADDRESS(0x0800d201, sys, ring);
    data[1] = 0x0002fffd;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (bcm_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));

    /* TOP_USER_CONTROL_0.15 = 0x1 */ 
    data[0] = PORT_ADDRESS(0x0800d104, sys, ring);
    data[1] = 0x80007fff;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (bcm_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));

    return (SOC_E_NONE);
}


int 
_bcm_tsc_reg_read(int unit, phy_ctrl_t *pc, int mode, int tsc_port, int pmd, uint32 reg, uint16 *value) 
{
    uint32 data[4], mem_cmd;
    int ring;

    ring = (pc->phy_id & 0x4) ? 1 : 0; /* mig too */

    /* No other SBUS  accesses between the write and the read. Ie. R5s need to be shutdown. */

    mem_cmd = (SBUS_WRITE_MEMORY_CMD_MSG << 26) | ((((tsc_port > 8) ? 0x7 : 0x3) & 0x7f) << 19) | (16 << 7);

    reg &= 0xffff;

    data[0] = ((reg | (pmd ? (1U << 27) : 0)) | ((tsc_port & 0x1f) << 19)) | ((mode & 0x7) << 16);
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    BCM_LOG_DEBUG("_bcm_tsc_reg_read: u=%d mode %d tsc_port 0x%x pmd %d reg 0x%x data[0] 0x%x\n",
                  unit, mode, tsc_port, pmd, reg, data[0]);
/*
    BCM_LOG_CLI("_bcm_tsc_reg_read: mod %d mode %d tsc_port 0x%x pmd %d mem_cmd 0x%x ring 0x%x data[0] 0x%x\n",
                 pc->module, mode, tsc_port, pmd, mem_cmd, ring, data[0]);
*/

    SBUS_LOCK;
    SOC_IF_ERROR_PRINT
        (bcm_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring, data));

    mem_cmd = (SBUS_READ_MEMORY_CMD_MSG << 26) | ((((tsc_port > 8) ? 0x7 : 0x3) & 0x7f) << 19) | (16 << 7);

    SOC_IF_ERROR_PRINT
        (bcm_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring, data));
    SBUS_UNLOCK;

    BCM_LOG_DEBUG("_bcm_tsc_reg_read: u=%d m=%d tsc_port=0x%x"
                  " data[0]=0x%08x data[1]=0x%08x data[2]=0x%08x data[3]=0x%08x\n",
                   unit, pc->module, tsc_port, data[0], data[1], data[2], data[3]);
/*
    BCM_LOG_CLI("_bcm_tsc_reg_read: data[1] 0x%x\n", data[1]);
*/

    *value = (data[1] & 0xffff);

    return SOC_E_NONE;

}

int
_bcm_tsc_reg_read0(int unit, phy_ctrl_t *pc, int sys_port, uint32 reg, uint16 *value)
{
    uint32 data[4], mem_cmd;
    int ring;

    ring = (pc->phy_id & 0x4) ? 1 : 0; /* mig too */

    /* No other SBUS  accesses between the write and the read. Ie. R5s need to be shutdown. */

    mem_cmd = (SBUS_WRITE_MEMORY_CMD_MSG << 26) | ((((sys_port) ? 0x7 : 0x3) & 0x7f) << 19) | (16 << 7);

    data[0] = reg;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    BCM_LOG_DEBUG("_bcm_tsc_reg_read: u=%d sys_port %d reg 0x%x value 0x%x\n",
                  unit, sys_port, reg, *value);
    BCM_LOG_CLI("_bcm_tsc_reg_read: sys_port %d mem_cmd 0x%x ring 0x%x data[0] 0x%x\n",
                 sys_port, mem_cmd, ring, data[0]);

    SBUS_LOCK;
    SOC_IF_ERROR_PRINT
        (bcm_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring, data));

    mem_cmd = (SBUS_READ_MEMORY_CMD_MSG << 26) | ((((sys_port) ? 0x7 : 0x3) & 0x7f) << 19) | (16 << 7);

    SOC_IF_ERROR_PRINT
        (bcm_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring, data));
    SBUS_UNLOCK;

    BCM_LOG_DEBUG("_bcm_tsc_reg_read: u=%d m=%d sys_port=%d"
                  " data[0]=0x%08x data[1]=0x%08x data[2]=0x%08x data[3]=0x%08x\n",
                  unit, pc->module, sys_port, data[0], data[1], data[2], data[3]);
    BCM_LOG_CLI("_bcm_tsc_reg_read: data[1] 0x%x\n", data[1]);

    *value = (data[1] & 0xffff);

    return SOC_E_NONE;
}


int
bcm_tsc_reg_read_test_base(int unit, int module, int sys, int pmd, uint32 reg, uint16 *value) 
{
    phy_ctrl_t *pc;
    int tsc_port;

    pc = EXT_PHY_SW_STATE(unit, module);

    tsc_port = (sys << 3) + (((pc->phy_id & 0x4) ? 1 : 0) << 2) + 1;

    SOC_IF_ERROR_RETURN
        (_bcm_tsc_reg_read(unit, pc, 0, tsc_port, pmd, reg, value)); 

/*     BCM_LOG_DEBUG("bcm_tsc_reg_read_test_base: u=%d m=%d %s "                             */
/*                 "reg=0x%04x value=0x%04x\n", unit, module, sys ? "S":"L", reg, *value); */

    return SOC_E_NONE;
}


int
_bcm_tsc_reg_write(int unit, phy_ctrl_t *pc, int mode, int tsc_port, int pmd, uint32 reg, uint16 value, uint16 mask) 
{
    uint32 data[4], mem_cmd;
    int ring;

    ring = (pc->phy_id & 0x4) ? 1 : 0; /* mig too */

    mem_cmd = (SBUS_WRITE_MEMORY_CMD_MSG << 26) | ((((tsc_port > 8) ? 0x7 : 0x3) & 0x7f) << 19) | (16 << 7);

    reg &= 0xffff;

    data[0] = ((reg | (pmd ? (1U << 27) : 0)) |  ((tsc_port & 0x1f) << 19)) | ((mode & 0x7) << 16);
    data[1] = ((value & 0xffff) << 16) | (mask & 0xffff);
    data[2] = 1;
    data[3] = 0;

    BCM_LOG_DEBUG("_bcm_tsc_reg_write: u=%d m=%d tsc_port=0x%x"
                  " data[0]=0x%08x data[1]=0x%08x data[2]=0x%08x data[3]=0x%08x\n",
                  unit, pc->module, tsc_port, data[0], data[1], data[2], data[3]);
/*
    BCM_LOG_CLI("_bcm_tsc_reg_write: mode %d tsc_port 0x%x pmd %d mem_cmd 0x%x ring 0x%x data[0] 0x%x data[1] 0x%x\n",
                mode, tsc_port, pmd, mem_cmd, ring, data[0], data[1]);
*/

    SBUS_LOCK;
    SOC_IF_ERROR_PRINT
        (bcm_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring, data));
    SBUS_UNLOCK;

    return SOC_E_NONE;

}


int
_bcm_tsc_reg_write0(int unit, phy_ctrl_t *pc, int sys_port, uint32 reg, uint16 value)
{
    uint32 data[4], mem_cmd;
    int ring;

    ring = (pc->phy_id & 0x4) ? 1 : 0; /* mig too */

    mem_cmd = (SBUS_WRITE_MEMORY_CMD_MSG << 26) | ((((sys_port) ? 0x7 : 0x3) & 0x7f) << 19) | (16 << 7);

    data[0] = reg;
    data[1] = ((value & 0xffff) << 16);
    data[2] = 1;
    data[3] = 0;

    BCM_LOG_DEBUG("_bcm_tsc_reg_write: u=%d m=%d sys_port=%d"
                   " data[0]=0x%08x data[1]=0x%08x data[2]=0x%08x data[3]=0x%08x\n",
                   unit, pc->module, sys_port, data[0], data[1], data[2], data[3]);
    BCM_LOG_CLI("_bcm_tsc_reg_write: sys_port %d mem_cmd 0x%x ring 0x%x data[0] 0x%x data[1] 0x%x\n",
                 sys_port, mem_cmd, ring, data[0], data[1]);

    SBUS_LOCK;
    SOC_IF_ERROR_PRINT
        (bcm_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring, data));
    SBUS_UNLOCK;

    return SOC_E_NONE;

}


int
bcm_tsc_reg_write_test_base(int unit, int module, int sys, int pmd, uint32 reg, uint16 value, uint16 mask) 
{
    phy_ctrl_t *pc;
    int tsc_port;

    pc = EXT_PHY_SW_STATE(unit, module);

    tsc_port = (sys << 3) + (((pc->phy_id & 0x4) ? 1 : 0) << 2) + 1;

    SOC_IF_ERROR_RETURN
        (_bcm_tsc_reg_write(unit, pc, 0, tsc_port, pmd, reg, value, mask)); 

/*     BCM_LOG_DEBUG("bcm_tsc_reg_write_test_base: u=%d m=%d %s "                                             */
/*                 "reg=0x%04x value=0x%04x mask=0x%04x\n", unit, module, sys ? "S":"L", reg, value, mask); */

    return SOC_E_NONE;
}


int
bcm_tsc_crc(int unit, int module, int sys, int pmd) 
{
    phy_ctrl_t *pc;
    int tsc_port;
    uint16  temp16;
    int idx;

    pc = EXT_PHY_SW_STATE(unit, module);

    tsc_port = (sys << 3) + (((pc->phy_id & 0x4) ? 1 : 0) << 2) + 1;

    /* wait for ready bit, bit7 */
    for (idx=0; idx<100; idx++) {
        bcm_tsc_reg_read_test_base(unit, module, sys, pmd, 0xd03d, &temp16);
        if (temp16 & 0x80) {
/*             BCM_LOG_DEBUG("bcm_tsc_crc: u=%d m=%d %s value=0x%x, ready\n", unit, module, sys ? "S":"L", temp16); */
            break;
        }
        bcm_udelay(1000);
    }
    if (!(temp16 & 0x80)) {
        BCM_LOG_ERR("bcm_tsc_crc: u=%d m=%d %s value=0x%x, NOT READY\n", unit, module, sys ? "S":"L", temp16);
        return -1;
    }

    /* write image size */
    temp16 = 0x7bb0;
    bcm_tsc_reg_write_test_base(unit, module, sys, pmd, 0xd03e, temp16, 0);

    /* write check CRC cmd */
    temp16 = 20;
    bcm_tsc_reg_write_test_base(unit, module, sys, pmd, 0xd03d, temp16, 0);

    /* wait for ready bit, bit7 */
    for (idx=0; idx<100; idx++) {
        bcm_tsc_reg_read_test_base(unit, module, sys, pmd, 0xd03d, &temp16);
        if (temp16 & 0x80) {
/*             BCM_LOG_DEBUG("bcm_tsc_crc: u=%d m=%d %s value=0x%x, ready\n", unit, module, sys ? "S":"L", temp16); */
            break;
        }
        bcm_udelay(1000);
    }
    if (!(temp16 & 0x80)) {
        BCM_LOG_ERR("bcm_tsc_crc: u=%d m=%d %s value=0x%x, NOT READY\n", unit, module, sys ? "S":"L", temp16);
        return -1;
    }

    bcm_tsc_reg_read_test_base(unit, module, sys, pmd, 0xd03e, &temp16);
    BCM_LOG_CLI("%s() bcm_tsc_crc: u=%d m=%d %s crc=0x%x\n", __func__, unit, module, sys ? "S":"L", temp16);

    return SOC_E_NONE;
}


int
bcm_tsc_reg_access(int unit, int module, int sys_port, int write, int reg, int *val)
{
    int rc = SOC_E_NONE;
    phy_ctrl_t *pc;
    uint16  temp16;

    BCM_LOG_CLI("bcm_tsc_reg_access(): u=%d m=%d sys_port 0x%x write=%d reg=0x%x val=0x%x\n",
        unit, module, sys_port, write, reg, *val);
    pc = EXT_PHY_SW_STATE(unit, module);

    if (write) {
        temp16=*val;
        BCM_LOG_CLI("bcm_tsc_reg_access() write reg=0x%x val=0x%x\n", reg, temp16);
        rc = _bcm_tsc_reg_write0(unit, pc, sys_port, reg, temp16);
    } else {
        rc = _bcm_tsc_reg_read0(unit, pc, sys_port, reg, &temp16);
        BCM_LOG_CLI("bcm_tsc_reg_access() read reg=0x%x val=0x%x\n",  reg, temp16);
        if (rc == SOC_E_NONE) {
            *val = temp16;
        }
    }

    return rc;
}


int
bcm_mdio_access(int unit, int module, int write, int reg, int *val)
{
    int rc = SOC_E_NONE;
    phy_ctrl_t *pc;
    uint16  reg16, temp16;

    BCM_LOG_CLI("bcm_mdio_access(): u=%d m=%d write=%d reg=0x%x val=0x%x\n",
        unit, module, write, reg, *val);
    pc = EXT_PHY_SW_STATE(unit, module);
    reg16=reg;

    if (write) {
        temp16=*val;
        BCM_LOG_CLI("bcm_mdio_access() write reg=0x%x val=0x%x\n", reg, temp16);
        rc = bcm_mdio_write(unit, pc, reg16, temp16);
    } else {
        rc = bcm_mdio_read(unit, pc, reg16, &temp16);
        BCM_LOG_CLI("bcm_mdio_access() read reg=0x%x val=0x%x\n",  reg16, temp16);
        if (rc == SOC_E_NONE) {
            *val=temp16;
        }
    }

    return rc;
}


int
bcm_sbus_access(int unit, int module, uint32 reg, uint32 cmd, int ring, uint32 arg[])
{
    int rc = SOC_E_NONE;
    phy_ctrl_t *pc;

    BCM_LOG_CLI("bcm_sbus_reg_access(): enter unit=%d module=%d reg=0x%x cmd=0x%x ring=%d"
            "arg[0]=0x%x arg[1]=0x%x arg[2]=0x%x arg[3]=0x%x\n",
        unit, module, reg, cmd, ring, arg[0], arg[1], arg[2], arg[3]);
    pc = EXT_PHY_SW_STATE(unit, module);

    SBUS_LOCK;
    rc = bcm_sbus_cmd(unit, pc, reg, cmd, ring, arg);
    SBUS_UNLOCK;

    BCM_LOG_CLI("bcm_sbus_reg_access(): exit unit=%d module=%d reg=0x%x cmd=0x%x ring=%d"
            "arg[0]=0x%x arg[1]=0x%x arg[2]=0x%x arg[3]=0x%x\n",
        unit, module, reg, cmd, ring, arg[0], arg[1], arg[2], arg[3]);

    return rc;
}


int
bcm_axi_reg_access(int unit, int module, int write, uint32 reg, uint32 *val)
{
    int rc = SOC_E_NONE;
    phy_ctrl_t *pc;
    uint32 temp32;

    BCM_LOG_CLI("bcm_axi_reg_access(): u=%d m=%d write=%d reg=0x%x val=0x%x\n",
        unit, module, write, reg, *val);
    pc = EXT_PHY_SW_STATE(unit, module);

    if (write) {
        rc = bcm_axi_write_reg(unit, pc, reg, *val);
        BCM_LOG_CLI("bcm_axi_reg_access() write reg=0x%x val=0x%x\n", reg, *val);
    } else {
        rc = bcm_axi_read_reg(unit, pc, reg, &temp32);
        BCM_LOG_CLI("bcm_axi_reg_access() read reg=0x%x val=0x%x\n", reg, temp32);
        if (rc == SOC_E_NONE) {
            *val = temp32;
        }
    }

    return rc;
}


int
bcm_top_reg_access(int unit, int module, int write, uint32 reg, uint32 *val)
{
    int rc = SOC_E_NONE;
    uint32 temp32;

    BCM_LOG_CLI("bcm_axi_reg_access(): u=%d m=%d write=%d reg=0x%x val=0x%x\n",
        unit, module, write, reg, *val);

    if (write) {
        rc = bcm_reg_write32(unit, module, reg, *val);
        BCM_LOG_CLI("bcm_reg_write32() write reg=0x%x val=0x%x\n", reg, *val);
    } else {
        rc = bcm_reg_read32(unit, module, reg, &temp32);
        BCM_LOG_CLI("bcm_reg_read32() read reg=0x%x val=0x%x\n", reg, temp32);
        if (rc == SOC_E_NONE) {
            *val = temp32;
        }
    }

    return rc;
}


int
bcm_xmod_command_send(int unit, phy_ctrl_t *pc, int mode_opcode,  uint32 arg[], int arg_size)
{
    uint16 status;
    int rv;

    BCM_LOG_DEBUG("%s() enter\n", __func__);
    if (arg_size > 256) {
        BCM_LOG_WARN("PHY8806X command handler (invalid parameters): u=%d p=%d arg_size=%d\n", \
                  unit, pc->module, arg_size);
        return SOC_E_PARAM;
    }
    
    bcm_timeout_init(&TO(pc), MDIO_XMOD_RESPONSE_TIMEOUT, 0);
    do {
        rv = READ_PHY8806X_FW_TO_SW_MESSAGE_REG(unit, pc, &status);
        if (!(status & (0x3 << 13)) || SOC_FAILURE(rv)) {
            break;
        }
    } while (!bcm_timeout_check(&TO(pc)));
    if ((status & (0x3 << 13)) || SOC_FAILURE(rv)) {
        BCM_LOG_WARN("PHY8806X command handler timeout (not ready): u=%d p=%d status=0x%04x\n", \
                  unit, pc->module, status);
        return SOC_E_TIMEOUT;
    }

   /* 
   -> 0x{2,3,4,5}4 0000 - 0x{2,3,4,5}4 00ff (256 bytes), axi_buf = 0x240000 + (pc->phy_id & 0x3) * 0x100000
   <- 0x{2,3,4,5}4 0100 - 0x{2,3,4,5}4 01ff (256 bytes), axi_buf = 0x240100 + (pc->phy_id & 0x3) * 0x100000
    */

    /* write arguments */
    rv = _bcm_axi_write(unit, pc, 0x240000 + (pc->phy_id & 0x3) * 0x100000, arg, arg_size);
    if (SOC_FAILURE(rv)) {
        BCM_LOG_ERR("PHY8806X command handler (AXI write failure): u=%d p=%d\n", \
                  unit, pc->module);
        return SOC_E_FAIL;
    }

    /* issue command */
    rv = WRITE_PHY8806X_SW_TO_FW_INTERRUPT_REG(unit, pc, (1U << 15) | (mode_opcode & 0x7ff));
    if (SOC_FAILURE(rv)) {
        BCM_LOG_ERR("PHY8806X command handler (command failure): u=%d p=%d\n", \
                  unit, pc->module);
        return SOC_E_FAIL;
    }
    
    bcm_timeout_init(&TO(pc), MDIO_XMOD_RESPONSE_TIMEOUT, 0);

    BCM_LOG_DEBUG("%s() exit\n", __func__);
    return SOC_E_NONE;
}


int
bcm_xmod_result_recv(int unit, phy_ctrl_t *pc, uint32 result[], int result_size)
{
    uint16 status;
    int rv;

    BCM_LOG_DEBUG("%s() enter\n", __func__);
    if (result_size > 256) {
        BCM_LOG_WARN("bcm_xmod_result_recv: invalid parameters: u=%d p=%d result_size=%d\n", \
                  unit, pc->module, result_size);
        /* issue ack command */
        bcm_xmod_command_ack(unit, pc);
        return SOC_E_PARAM;
    }

    do {
        rv = READ_PHY8806X_FW_TO_SW_MESSAGE_REG(unit, pc, &status);
        if ((status & (0x1 << 15)) | ((status & (0x3 << 13)) == (0x2 << 13)) || SOC_FAILURE(rv)) {
            break;
        }
    } while (!bcm_timeout_check(&TO(pc)));
    if (SOC_FAILURE(rv)) {
        BCM_LOG_ERR("PHY8806X command handler (status read failure): u=%d p=%d\n", \
                  unit, pc->module);
        /* issue ack command */
        bcm_xmod_command_ack(unit, pc);
        return SOC_E_FAIL;
    }

    if (status & (0x1 << 15)) { /* error */
        BCM_LOG_ERR("PHY8806X command handler failure: u=%d p=%d status=0x%04x\n", \
                  unit, pc->module, status);
        /* issue ack command */
        bcm_xmod_command_ack(unit, pc);
        return SOC_E_FAIL;
    }

    if ((status & (0x3 << 13)) != (0x2 << 13)) { /* result not ready */
        BCM_LOG_ERR("PHY8806X command handler timeout (no result): u=%d p=%d status=0x%04x\n", \
                  unit, pc->module, status);
        /* issue ack command */
        bcm_xmod_command_ack(unit, pc);
        return SOC_E_TIMEOUT;
    }


   /* 
   -> 0x{2,3,4,5}4 0000 - 0x{2,3,4,5}4 00ff (256 bytes), axi_buf = 0x240000 + (pc->phy_id & 0x3) * 0x100000
   <- 0x{2,3,4,5}4 0100 - 0x{2,3,4,5}4 01ff (256 bytes), axi_buf = 0x240100 + (pc->phy_id & 0x3) * 0x100000
    */


    /* read results */
    rv = _bcm_axi_read(unit, pc, 0x240100 + (pc->phy_id & 0x3) * 0x100000, result, result_size);
    if (SOC_FAILURE(rv)) {
        BCM_LOG_ERR("PHY8806X command handler (AXI read failure): u=%d p=%d\n", \
                  unit, pc->module);
        /* issue ack command */
        bcm_xmod_command_ack(unit, pc);

        return SOC_E_FAIL;
    }

    /* issue ack command */
    bcm_xmod_command_ack(unit, pc);

    BCM_LOG_DEBUG("%s() exit\n", __func__);
    return SOC_E_NONE;

}


int
bcm_xmod_command(int unit, int module, int mode_opcode,  uint32 *arg, int arg_size, uint32 *result, int result_size)
{
    phy_ctrl_t *pc;
    int rv;

    BCM_LOG_DEBUG("%s() enter\n", __func__);
    pc = EXT_PHY_SW_STATE(unit, module);
    BCM_LOG_DEBUG("%s() unit %d module %d phy_id %d phy_devad %d arg_size %d result_size %d\n", __func__,
            pc->unit, pc->module, pc->phy_id, pc->phy_devad, arg_size, result_size);

    XMOD_LOCK;
    rv = bcm_xmod_command_send(unit, pc, mode_opcode,  arg, arg_size);

    if (SOC_FAILURE(rv)) {
        BCM_LOG_ERR("bcm_xmod_command: send command failed. u=%d p=%d cmd:%x \n", \
                  unit, pc->module, (mode_opcode & 0xFF));
        XMOD_UNLOCK;
        return SOC_E_FAIL;
    }

    rv = bcm_xmod_result_recv(unit, pc, result, result_size);
    XMOD_UNLOCK;

    if (SOC_FAILURE(rv)) {
        BCM_LOG_ERR("bcm_xmod_command: receive result failed. u=%d p=%d cmd:%x \n", \
                  unit, pc->module, (mode_opcode & 0xFF));
        return SOC_E_FAIL;
    }

    BCM_LOG_DEBUG("%s() exit\n", __func__);
    return SOC_E_NONE;
}


int
bcm_xmod_command_ack(int unit, phy_ctrl_t *pc)
{
    int rv;

    /* issue ack command */
    rv = WRITE_PHY8806X_SW_TO_FW_INTERRUPT_REG(unit, pc, (1U << 15) | (0x17 & 0x7ff));

    if (SOC_FAILURE(rv)) {
        BCM_LOG_ERR("PHY8806X command handler (ack failure): u=%d p=%d\n", \
                  unit, pc->module);
        return SOC_E_FAIL;
    }

    return SOC_E_NONE;
}

