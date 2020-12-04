/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phy8806x.c
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
 *
 *   tsce_config_t
 *   Driver specific data.  This structure is used by tsce to hold
 *   logical port specific working storage.
 *
 *   soc_phymod_ctrl_t
 *   PHYMOD drivers.  Resides in phy_ctrl_t specifies how many cores
 *   are in this phy and contains an array of pointers to
 *   soc_phymod_phy_t structures, which define a PHYMOD PHY.
 *
 *   soc_phymod_phy_t
 *   This structure contains a pointer to phymod_phy_access_t and a
 *   pointer to the associated soc_phymod_core_t.  Instances if this
 *   structure are created during device probe/init and are maintained
 *   by the SOC.
 *
 *   soc_phymod_core_t
 *   This structure contains per-core information.  Multiple PHYMOD
 *   PHYS can point to a single core.
 *
 *   phymod_phy_access_t
 *   This structure contains information about how to read/write PHY
 *   registers.  A required parameter for PHYMOD PHY APIs
 * 
 *   phymod_core_access_t
 *   This structure contains information about how to read/write PHY
 *   registers.  A required parameter for PHYMOD core APIs.
 */
#include "phydefs.h"      /* Must include before other phy related includes */ 
#if defined(INCLUDE_PHY_8806X)
#include "phyconfig.h"     /* Must include before other phy related includes */

#include <shared/bsl.h>

#include <sal/types.h>
#include <sal/types.h>
#include <sal/core/spl.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/phyreg.h>
#include <soc/port_ability.h>
#include <soc/phy.h>
#include <soc/phy/phyctrl.h>
#include <soc/phy/drv.h>
#include <soc/phy/phymod_ids.h>


#include "phyident.h"
#include "phyreg.h"
#include "phynull.h"
#include "xgxs.h"
#include "serdesid.h"
#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif

#include "phy8806x.h"
#include "phy8806x_ctr.h"
#include "phy8806x_syms.h"
#include <phymod/phymod.h>
#include <phymod/phymod_debug.h>
#include <phymod/phymod_diagnostics.h>
#include <appl/diag/diag.h>
#include "phy8806x_funcs.h"

#ifdef PORTMOD_SUPPORT
#include <soc/portmod/portmod.h>
#endif

#if defined(INCLUDE_FCMAP)
#include <soc/fcmapphy.h>
#endif
#include "../../../libs/phymod/chip/phy8806x/phy8806x_xmod_api.h"

#define PMD_TEST_FIRMWARE
#define PHY8806X_DEBUG              0

#define NUM_LANES             4    /* num of lanes per core */
#define MAX_NUM_LANES         4    /* max num of lanes per port */


#define PHYMOD_INTF_CR4    (1 << SOC_PORT_IF_CR4)

#define PHY8806X_40G_10G_INTF(_if)  ((_if) == SOC_PORT_IF_KR4 || (_if) == SOC_PORT_IF_XLAUI || \
                             (_if) == SOC_PORT_IF_CR4 || (_if) == SOC_PORT_IF_SR || \
                             (_if) == SOC_PORT_IF_KR || (_if) == SOC_PORT_IF_CR || \
                             (_if) == SOC_PORT_IF_XFI || (_if) == SOC_PORT_IF_SFI)

#define PHY8806X_40G_10G_INTF_ALL   (PHY8806X_IF_KR4 | PHY8806X_IF_XLAUI | PHY8806X_IF_CR4 | PHY8806X_IF_SR | \
                        PHY8806X_IF_KR | PHY8806X_IF_CR | PHY8806X_IF_XFI | PHY8806X_IF_SFI)

#define GEARBOX_LINE_LANE_CNT    4

#define PHY8806X_LANE_NAME_LEN   30

#define PHY8806X_MEASSURE_DOWNLOAD_TIME

/* index to TX drive configuration table for each VCO setting.
 */
typedef enum txdrv_inxs {
    TXDRV_XFI_INX = 0, /* 10G XFI */
    TXDRV_XLAUI_INX,   /* 40G XLAUI mode  */
    TXDRV_SFI_INX,     /* 10G SR fiber mode */
    TXDRV_SFIDAC_INX,  /* 10G SFI DAC mode */
    TXDRV_SR4_INX,     /* 40G SR4 */
    TXDRV_AN_INX,      /* a common entry for autoneg mode */
    TXDRV_DFT_INX,     /* temp for any missing speed modes */
    TXDRV_LAST_INX     /* always last */
} TXDRV_INXS_t;

/* Port mode */
typedef enum {
    _BFCMAPPER_PORT_MODE_QUAD = 0,
    _BFCMAPPER_PORT_MODE_TRI_0122 = 1,
    _BFCMAPPER_PORT_MODE_TRI_0023 = 2,
    _BFCMAPPER_PORT_MODE_DUAL = 3,
    _BFCMAPPER_PORT_MODE_SINGLE = 4
} _bfcmapper_port_mode_t;

typedef enum {
    REPEATER_AN_PHASE_NONE = -1,
    REPEATER_AN_PHASE_0,
    REPEATER_AN_PHASE_1,
    REPEATER_AN_PHASE_2
}  repeater_an_phase_t;

#define REPEATER_AN_PHASE0_LOCKED   1
#define REPEATER_AN_PHASE0_CL74     2
#define REPEATER_AN_PHASE0_CL91     4
#define REPEATER_AN_PHASE1_LOCKED   8
#define REPEATER_AN_PHASE0_RETRY    10
#define REPEATER_AN_PHASE1_RETRY    10

#define REPEATER_FS_RESET_TRIAL     8

#define TXDRV_ENTRY_NUM     (TXDRV_LAST_INX)

#define PHY8806X_IF_NULL   (1 << SOC_PORT_IF_NULL)
#define PHY8806X_IF_SR     (1 << SOC_PORT_IF_SR)
#define PHY8806X_IF_SR2  (1 << SOC_PORT_IF_MII)
#define PHY8806X_IF_SR4    (1 << SOC_PORT_IF_SR4)
#define PHY8806X_IF_KR     (1 << SOC_PORT_IF_KR)
#define PHY8806X_IF_KR2    (1 << SOC_PORT_IF_TBI)
#define PHY8806X_IF_KR4    (1 << SOC_PORT_IF_KR4)
#define PHY8806X_IF_CR     (1 << SOC_PORT_IF_CR)
#define PHY8806X_IF_CR2    (1 << SOC_PORT_IF_RGMII)
#define PHY8806X_IF_CR4    (1 << SOC_PORT_IF_CR4)
#define PHY8806X_IF_XFI    (1 << SOC_PORT_IF_XFI)
#define PHY8806X_IF_SFI    (1 << SOC_PORT_IF_SFI)
#define PHY8806X_IF_XLAUI  (1 << SOC_PORT_IF_XLAUI)
#define PHY8806X_IF_XGMII  (1 << SOC_PORT_IF_XGMII)
#define PHY8806X_IF_ILKN   (1 << SOC_PORT_IF_ILKN)

#define PHY8806X_40G_10G_INTF(_if)  ((_if) == SOC_PORT_IF_KR4 || (_if) == SOC_PORT_IF_XLAUI || \
                             (_if) == SOC_PORT_IF_CR4 || (_if) == SOC_PORT_IF_SR || \
                             (_if) == SOC_PORT_IF_KR || (_if) == SOC_PORT_IF_CR || \
                             (_if) == SOC_PORT_IF_XFI || (_if) == SOC_PORT_IF_SFI)

#define PHY8806X_40G_10G_INTF_ALL   (PHY8806X_IF_KR4 | PHY8806X_IF_XLAUI | PHY8806X_IF_CR4 | PHY8806X_IF_SR | \
                        PHY8806X_IF_KR | PHY8806X_IF_CR | PHY8806X_IF_XFI | PHY8806X_IF_SFI)

#define PHY8806X_CL73_HPAM_VS_SW        0x8
#define PHY8806X_CL73_HPAM              0x6
#define PHY8806X_CL73_CL37              0x5
#define PHY8806X_MSA                    0x4
#define PHY8806X_CL73_MSA               0x3
#define PHY8806X_CL73_WO_BAM            0x2
#define PHY8806X_CL73_W_BAM             0x1
#define PHY8806X_CL73_DISABLE           0x0

#define TO(_pc) (((phy8806x_config_t *)((_pc) + 1))->to)
#define FIRMWARE(_pc) (((phy8806x_config_t *)((_pc) + 1))->firmware)
#define FIRMWARE_LEN(_pc) (((phy8806x_config_t *)((_pc) + 1))->firmware_len)
#define DL_DIVIDEND(_pc) (((phy8806x_config_t *)((_pc) + 1))->dl_dividend)
#define DL_DIVISOR(_pc) (((phy8806x_config_t *)((_pc) + 1))->dl_divisor)
#define ETH_CTR(_pc) (((phy8806x_config_t *)((_pc) + 1))->inbctr)
#define PMD_SIZE(_pc) (((phy8806x_config_t *)((_pc) + 1))->pmd_size)
#define PMD_CRC(_pc) (((phy8806x_config_t *)((_pc) + 1))->pmd_crc)
#define TOP_DEV_REV_ID(_pc) (((phy8806x_config_t *)((_pc) + 1))->top_dev_rev_id)
#define LED_TX(_pc) (((phy8806x_config_t *)((_pc) + 1))->led_tx)
#define LED_RX(_pc) (((phy8806x_config_t *)((_pc) + 1))->led_rx)
#define LED_SPEED(_pc) (((phy8806x_config_t *)((_pc) + 1))->led_speed)

#define PORT_ADDRESS(_base_addr, _sys, _mig) ( ((_base_addr) & ((0x1 << 27) | 0xffff)) |  ((((_sys) << 3) + ((_mig) << 2)  + 1) << 19) )
#define LOW16(_x)  ( (_x) & 0xffff )
#define HIGH16(_x) ( ((_x) >> 16) & 0xffff )

#define RETRY_COUNT 5

#define WRITE_BLK_SIZE 4

extern uint8 bcm_8806x_firmware[];

extern int bcm_8806x_firmware_size;

static char *dev_name_8806x = "BCM8806x";
static sal_mutex_t xmod_mutex[SOC_MAX_NUM_DEVICES];

#define XMOD_MUTEX_TIMEOUT 10000000 /* 10ms */

#define PHY_IS_BCM8806X_A0(_pc) (PHY_IS_BCM8806X(_pc) \
                                   && ((TOP_DEV_REV_ID(_pc) & 0x00ff0000) == 0x00010000))
#define PHY_IS_BCM8806X_B0(_pc) (PHY_IS_BCM8806X(_pc) \
                                   && ((TOP_DEV_REV_ID(_pc) & 0x00ff0000) == 0x00110000))
#define PHY_IS_BCM88060(_pc) (PHY_IS_BCM8806X(_pc) \
                                   && ((TOP_DEV_REV_ID(_pc) & 0xff) == 0x60))
#define PHY_IS_BCM88061(_pc) (PHY_IS_BCM8806X(_pc) \
                                   && ((TOP_DEV_REV_ID(_pc) & 0xff) == 0x61))
#define PHY_IS_BCM88064(_pc) (PHY_IS_BCM8806X(_pc) \
                                   && ((TOP_DEV_REV_ID(_pc) & 0xff) == 0x64))
#define PHY_IS_BCM88068(_pc) (PHY_IS_BCM8806X(_pc) \
                                   && ((TOP_DEV_REV_ID(_pc) & 0xff) == 0x68))

#ifdef BE_HOST
#define HOST2LE32(_x) (_shr_swap32((_x)))
#define LE2HOST32(_x) (_shr_swap32((_x)))
#else
#define HOST2LE32(_x) (_x)
#define LE2HOST32(_x) (_x)
#endif

STATIC int phy8806x_cl72_enable_set(soc_phymod_ctrl_t *pmc, uint32 value);
STATIC int phy_8806x_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability);
STATIC int phy_8806x_ability_advert_set(int unit, soc_port_t port,
                          soc_port_ability_t *ability);
STATIC int _phy_8806x_write_to_arm(int unit, phy_ctrl_t *pc, uint32 addr, uint8 *data,int len);
STATIC int _phy_8806x_xmod_command_ack(int unit, phy_ctrl_t *pc);
STATIC int _phy_8806x_xmod_command_send(int unit, phy_ctrl_t *pc, int mode_opcode,  uint32 arg[], int arg_size);
STATIC int _phy_8806x_xmod_result_recv(int unit, phy_ctrl_t *pc, uint32 result[], int result_size);
STATIC int _phy_8806x_axi_write(int unit,phy_ctrl_t *pc, uint32 addr, uint32 data[], int size);
STATIC int _phy_8806x_axi_read(int unit,phy_ctrl_t *pc, uint32 addr, uint32 data[], int size);
STATIC int _pmd_enable(int unit, phy_ctrl_t *pc, int sys);
STATIC int _pmd_disable(int unit, phy_ctrl_t *pc, int sys);
STATIC int _pmd_start(int unit, phy_ctrl_t *pc, int sys);
STATIC int _tsc_reg_read(int unit, phy_ctrl_t *pc, int mode, int tsc_port, int pmd, int pll, uint32 reg, uint16 *value);
STATIC int _tsc_reg_write(int unit, phy_ctrl_t *pc, int mode, int tsc_port, int pmd, int pll, uint32 reg, uint16 value, uint16 mask);
int tsc_reg_read_test_base(int unit, int port, int sys, int pmd, int pll, uint32 reg);
int tsc_reg_write_test_base(int unit, int port, int sys, int pmd, int pll, uint32 reg, uint16 value, uint16 mask); 
int phy_8806x_xmod_command(int unit, int port, int mode_opcode,  uint32 arg[], int arg_size, uint32 result[], int result_size);
STATIC int phy_8806x_an_get(int unit, soc_port_t port, int *an, int *an_done);
STATIC int phy8806x_forced_speed_line_side_set(soc_phymod_ctrl_t *pmc, uint32 value);
STATIC int phy8806x_forced_speed_line_side_get(soc_phymod_ctrl_t *pmc, uint32 *value);
STATIC int phy8806x_flow_control_mode_set(soc_phymod_ctrl_t *pmc, uint32 value, uint8 ingress);
STATIC int phy8806x_flow_control_mode_get(soc_phymod_ctrl_t *pmc, uint8 ingress, uint32 *value);
STATIC int phy8806x_psm_cos_bmp_set(soc_phymod_ctrl_t *pmc, uint32 value);
STATIC int phy8806x_psm_cos_bmp_get(soc_phymod_ctrl_t *pmc, uint32* value);
STATIC int phy8806x_pfc_use_ip_cos_set(soc_phymod_ctrl_t *pmc, uint32 value);
STATIC int phy8806x_pfc_use_ip_cos_get(soc_phymod_ctrl_t *pmc, uint32* value);
STATIC int _phy8806x_dump_buffer(int unit, int port, unsigned char *arg);
STATIC int phy_8806x_an_set(int unit, soc_port_t port, int enable);
STATIC int phy8806x_autoneg_line_side_set(soc_phymod_ctrl_t *pmc, uint32 enable);
STATIC int phy8806x_autoneg_line_side_get(soc_phymod_ctrl_t *pmc, uint32 *enable);
STATIC int phy_8806x_fw_stat_read(int unit, int port, int offset,  uint32_t result[], int result_size);
extern int phy8806x_tsc_pmd_lock_status (const phymod_access_t *pa, uint8_t *pmd_rx_lock);

/*
 * Function:
 *      _8806x_reg_read
 * Doc:
 *      register read operations
 * Parameters:
 *      unit            - (IN)  unit number
 *      core_addr       - (IN)  core address
 *      reg_addr        - (IN)  address to read
 *      val             - (OUT) read value
 */
STATIC int 
_8806x_reg_read(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t *val)
{
    uint16 data16;
    int rv = SOC_E_NONE;
    soc_phymod_core_t *core = (soc_phymod_core_t *)user_acc;
    phymod_core_access_t *pm_core = &core->pm_core;
    phymod_access_t *phyacc = &pm_core->access;
    int pll = PHYMOD_ACC_PLLIDX(phyacc);
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(core->unit, core->port);
    int pmd = (reg_addr & 0x08000000) ? 1 : 0;
    int sys = (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? 1 : 0;
    int tsc_port, lane_mode;

    tsc_port = (sys << 3) + (pc->phy_id & 0x7) + 1;
    lane_mode = (reg_addr >> 16) & 7;

    rv = _tsc_reg_read(core->unit, pc, lane_mode, tsc_port, pmd, pll, (reg_addr & 0xffff), &data16);

    *val = data16;

    return rv;
}

/*
 * Function:
 *      _8806x_reg_write
 * Doc:
 *      register write operations
 * Parameters:
 *      unit            - (IN)  unit number
 *      core_addr       - (IN)  core address
 *      reg_addr        - (IN)  address to write
 *      val             - (IN)  write value
 */
STATIC int 
_8806x_reg_write(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t val)
{
    int rv = SOC_E_NONE;
    soc_phymod_core_t *core = (soc_phymod_core_t *)user_acc;
    phymod_core_access_t *pm_core = &core->pm_core;
    phymod_access_t *phyacc = &pm_core->access;
    int pll = PHYMOD_ACC_PLLIDX(phyacc);
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(core->unit, core->port);
    int pmd = (reg_addr & 0x08000000) ? 1 : 0;
    int sys = (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? 1 : 0;
    int tsc_port, lane_mode;
    uint16 value = val & 0xffff;
    uint16 mask = (val & 0xffff0000) ? ((~val) >> 16) : (val >> 16);

    tsc_port = (sys << 3) + (pc->phy_id & 0x7) + 1;
    lane_mode = (reg_addr >> 16) & 7;

    rv = _tsc_reg_write(core->unit, pc, lane_mode, tsc_port, pmd, pll, (reg_addr & 0xffff), value, mask);

    return rv;
}

#define USE_PHY8806X_FW_STAT

int
_8806x_xmod_command(void *data)
{
    xmod_buf_desc *bdp = (xmod_buf_desc *)data;

    soc_phymod_core_t *soc_core;

    if (bdp) {
        soc_core = PHYMOD_ACC_USER_ACC(bdp->pa);
#ifdef USE_PHY8806X_FW_STAT
        if (bdp->mode_opcode == XMOD_CMD_MODE_ETH(XMOD_PHY_INTERFACE_CONFIG_GET)) {
            return(phy_8806x_fw_stat_read(soc_core->unit, soc_core->port,
                   FWSTAT_PHY_INTERFACE_CONFIG_GET_OFFSET, bdp->xmodrxbuff, bdp->xmodrxlen));
        } else if (bdp->mode_opcode == XMOD_CMD_MODE_ETH(XMOD_PHY_AUTONEG_GET)) {
            return(phy_8806x_fw_stat_read(soc_core->unit, soc_core->port,
                   FWSTAT_PHY_AUTONEG_GET_OFFSET, bdp->xmodrxbuff, bdp->xmodrxlen));
        } else 
#endif
        if (bdp->mode_opcode == XMOD_CMD_MODE_ETH(XMOD_PHY_RX_PMD_LOCKED_GET)) {
            return (phy8806x_tsc_pmd_lock_status (bdp->pa, (uint8_t *)bdp->xmodrxbuff));
        } else {
            return (phy_8806x_xmod_command(soc_core->unit, soc_core->port, bdp->mode_opcode,
                    bdp->xmodtxbuff, bdp->xmodtxlen, bdp->xmodrxbuff, bdp->xmodrxlen));
        }
    }

    return SOC_E_UNAVAIL;
}


#define IS_DUAL_LANE_PORT(_pc) ((_pc)->phy_mode == PHYCTRL_DUAL_LANE_PORT)  
#define IND_LANE_MODE(_pc) (((_pc)->phy_mode == PHYCTRL_DUAL_LANE_PORT) || ((_pc)->phy_mode == PHYCTRL_ONE_LANE_PORT))  

uint8 * phy8806x_eth_ctr_get(phy_ctrl_t *pc)
{
    return ((uint8 *) ETH_CTR(pc));
}

void phy8806x_led_tx_set(phy_ctrl_t *pc, uint8 val)
{
    LED_TX(pc) = val;
    return;
}

void phy8806x_led_rx_set(phy_ctrl_t *pc, uint8 val)
{
    LED_RX(pc) = val;
    return;
}

STATIC int
phy8806x_port_config_init(phy8806x_speed_config_t* speed_config, phymod_port_config_t *port_config)
{
    port_config->interface = speed_config->interface;
    port_config->sys_lane_count = speed_config->sys_lane_count;
    port_config->ln_lane_count = speed_config->ln_lane_count;
    port_config->an_mode= speed_config->an_mode;
    port_config->an_cl72 = speed_config->an_cl72;
    port_config->fs_cl72 = speed_config->fs_cl72;
    port_config->fs_cl72_sys = speed_config->fs_cl72_sys;
    port_config->an_fec = speed_config->an_fec;
    port_config->speed = speed_config->speed;
    port_config->port_is_higig = speed_config->port_is_higig;
    port_config->an_fec = speed_config->an_fec;
    port_config->port_mode = speed_config->port_mode;
    port_config->fiber_pref = speed_config->fiber_pref;
    port_config->fiber_pref_sys = speed_config->fiber_pref_sys;
    port_config->fs_fec = speed_config->fs_fec;
    port_config->fs_fec_sys = speed_config->fs_fec_sys;
    port_config->is_flex = speed_config->is_flex;
    port_config->quad_mode = speed_config->quad_mode;

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy8806x_speed_to_interface_config_get
 * Purpose:     
 *      Convert speed to interface_config struct
 * Parameters:
 *      unit                - BCM unit number.
 *      port                - Port number.
 *      speed               - speed to convert
 *      interface_config    - output interface config struct
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int 
phy8806x_speed_to_interface_config_get(phy8806x_speed_config_t* speed_config, phymod_phy_inf_config_t* interface_config, TXDRV_INXS_t* tx_index)
{
    int port_is_higig;
    int port_num_lanes;
    int fiber_pref;

    port_num_lanes = speed_config->ln_lane_count;
    port_is_higig = speed_config->port_is_higig;
    fiber_pref = speed_config->fiber_pref;

    SOC_IF_ERROR_RETURN(phymod_phy_inf_config_t_init(interface_config));

    interface_config->interface_modes = 0;
    interface_config->data_rate       = speed_config->speed;
    interface_config->pll_divider_req = speed_config->pll_divider_req ;
    interface_config->device_aux_modes= NULL ;

    *tx_index = TXDRV_DFT_INX;

    if (port_is_higig) {
        PHYMOD_INTF_MODES_HIGIG_SET(interface_config);
    }
    switch (speed_config->speed) {
    case 1000:
        if (fiber_pref) {
            interface_config->interface_type = phymodInterface1000X;
            *tx_index = TXDRV_SFI_INX;
        } else {
            interface_config->interface_type = phymodInterfaceSGMII;
        }
        break;
    case 10000:
        if (speed_config->line_interface & PHY8806X_IF_SFI)
            interface_config->interface_type = phymodInterfaceSFI;
        else if (speed_config->line_interface & PHY8806X_IF_SR)
            interface_config->interface_type = phymodInterfaceSR;
        else if (speed_config->line_interface & PHY8806X_IF_CR)
            interface_config->interface_type = phymodInterfaceCR;
        else {
            if (speed_config->fs_cl72_sys)
                interface_config->interface_type = phymodInterfaceCR;
            else
                interface_config->interface_type = phymodInterfaceXFI;
        } 
        break;
    case 20000: 
        if (port_num_lanes == 1) {
            if (speed_config->fs_cl72_sys)
                interface_config->interface_type = phymodInterfaceCR;
            else
                interface_config->interface_type = phymodInterfaceXFI;

            *tx_index = TXDRV_XFI_INX;
        } else if(port_num_lanes == 2) {
            interface_config->interface_type = phymodInterfaceCR2;
        } 
        break ;
    case 25000:
        if (speed_config->fs_cl72_sys)
            interface_config->interface_type = phymodInterfaceCR;
        else
            interface_config->interface_type = phymodInterfaceXFI;
        *tx_index = TXDRV_XFI_INX;
        break ;
    case 40000:
        if(fiber_pref){
            PHYMOD_INTF_MODES_FIBER_SET(interface_config);
        }
        if (port_num_lanes == 4) {
            interface_config->interface_type = phymodInterfaceCR4;
            *tx_index = TXDRV_XLAUI_INX;
        } else {
            interface_config->interface_type = phymodInterfaceCR2;
            *tx_index = TXDRV_XLAUI_INX;
        }
        break;
    case 42000:
        if (port_num_lanes == 4) {
            interface_config->interface_type = phymodInterfaceCR4;
            *tx_index = TXDRV_XLAUI_INX;
        } else {
            interface_config->interface_type = phymodInterfaceCR2;
            *tx_index = TXDRV_XLAUI_INX;
        }
        break;
    case 50000:
        if(fiber_pref){
            PHYMOD_INTF_MODES_FIBER_SET(interface_config);
        }
        if (port_num_lanes == 4) {
            interface_config->interface_type = phymodInterfaceCR4;
            *tx_index = TXDRV_XLAUI_INX;
        } else {
            interface_config->interface_type = phymodInterfaceCR2;
            *tx_index = TXDRV_XLAUI_INX;
        }
        break;
    case 100000:
        interface_config->interface_type = phymodInterfaceCR4;
        *tx_index = TXDRV_XLAUI_INX;
        break;
    case 106000:
        interface_config->interface_type = phymodInterfaceCR4;
        *tx_index = TXDRV_XLAUI_INX;
        break;
    default:
        return SOC_E_PARAM;
    }

    switch (speed_config->port_refclk_int)
    {
    case 156:
        interface_config->ref_clock = phymodRefClk156Mhz;
        break;
    case 125:
        interface_config->ref_clock = phymodRefClk125Mhz;
        break;
    default:
        return SOC_E_PARAM;
    }
    return SOC_E_NONE;
}

STATIC int
phy8806x_phymod_interfacetype_to_soc_type (phymod_interface_t interface_type, soc_port_if_t *pif)
{
    switch (interface_type) {
    case phymodInterface1000X:
        *pif = SOC_PORT_IF_GMII;
        break;
    case phymodInterfaceSGMII:
        *pif = SOC_PORT_IF_SGMII;
        break;
    case phymodInterfaceXFI:
        *pif = SOC_PORT_IF_XFI;
        break;
    case phymodInterfaceSFI:
        *pif = SOC_PORT_IF_SFI;
        break;
    case phymodInterfaceXLAUI:
        *pif = SOC_PORT_IF_XLAUI;
        break;
    case phymodInterfaceKX:
        *pif = SOC_PORT_IF_KX;
        break;
    case phymodInterfaceRXAUI:
        *pif = SOC_PORT_IF_RXAUI;
        break;
    case phymodInterfaceXGMII:
        *pif = SOC_PORT_IF_XGMII;
        break;
    case phymodInterfaceKR2:
         *pif = SOC_PORT_IF_KR2;
        break;
    case phymodInterfaceCR2:
        *pif = SOC_PORT_IF_CR2;
        break;
    case phymodInterfaceSR2:
        *pif = SOC_PORT_IF_SR2;
        break;
    case phymodInterfaceSR4:
        *pif = SOC_PORT_IF_SR4;
        break;
    case phymodInterfaceCR4:
        *pif = SOC_PORT_IF_CR4;
        break;
    case phymodInterfaceKR4:
        *pif = SOC_PORT_IF_KR4;
        break;
    case phymodInterfaceCR:
        *pif = SOC_PORT_IF_CR;
        break;
    case phymodInterfaceKR:
        *pif = SOC_PORT_IF_KR;
        break;
    case phymodInterfaceSR:
        *pif = SOC_PORT_IF_SR;
        break;
    case phymodInterfaceCR10:
        *pif = SOC_PORT_IF_CAUI;
        break;
    case phymodInterfaceCAUI4:
        *pif = SOC_PORT_IF_CAUI;
        break;
    case phymodInterfaceBypass:
        *pif = SOC_PORT_IF_ILKN;
        break;
    default:
        *pif =  SOC_PORT_IF_XGMII;
        break;
    }

    return 0;
}

/* 
 * Given the BMP port number, the function returns
 *  the Quad mode (single, Tri0, Tri1, Dual, Single mode.
 */
int get_quad_mode (int port)
{
    int i = 0;
    int unit = 0;
    int port_idx = 0;
    int lane_cnt = 0;
    int quad_mode = 0;

    while ((i < 4) && (port_idx < 4)) {
        switch (port_idx) {
        case 0:
            lane_cnt = SOC_INFO(unit).port_num_lanes[port+port_idx];
            switch (lane_cnt) {
                case 1:
                    quad_mode = _BFCMAPPER_PORT_MODE_QUAD;  /* QUAD */
                    break;
                case 2:
                    quad_mode = _BFCMAPPER_PORT_MODE_DUAL;  /* DUAL */
                    break;
                case 4:
                    quad_mode = _BFCMAPPER_PORT_MODE_SINGLE;  /* SINGLE */
                    return (quad_mode);
            }
            break;
        case 2:
            lane_cnt = SOC_INFO(unit).port_num_lanes[port+port_idx];
            switch (lane_cnt) {
                case 1:
                    if (quad_mode == _BFCMAPPER_PORT_MODE_DUAL)
                        quad_mode = _BFCMAPPER_PORT_MODE_TRI_0023;  /* TRI0 */
                    break;
                case 2:
                    if (quad_mode == _BFCMAPPER_PORT_MODE_QUAD)
                        quad_mode = _BFCMAPPER_PORT_MODE_TRI_0122;  /* TRI1 */
                    break;

            }
        }

        port_idx += lane_cnt;
        i++;
    }

    return quad_mode;
}

/* 
 * Given a port number, the function returns the BMP port numbrt.
 */
int get_bmp_port (int port, phy_ctrl_t *pc)
{
    int unit = 0;
    uint16 phy_addr=0;

    phy_addr = pc->phy_id;

    phy_addr -= phy_addr & 0x3;
    port= PHY_ADDR_TO_PORT(unit, phy_addr);

    return port;
}

int compute_quad_mode (int unit, int port)
{
    phy_ctrl_t *pc;
    int bmp_port;

    pc = EXT_PHY_SW_STATE(unit, port); 
    bmp_port = get_bmp_port(port, pc);
    return get_quad_mode(bmp_port);
}

/*
 * Function:
 *      phy8806x_config_init
 * Purpose:     
 *      Determine phy configuration data for purposes of PHYMOD initialization.
 * 
 *      A side effect of this procedure is to save some per-logical port
 *      information in (phy8806x_cfg_t *) &pc->driver_data;
 *
 * Parameters:
 *      unit                  - BCM unit number.
 *      port                  - Port number.
 *      logical_lane_offset   - starting logical lane number
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy8806x_config_init(int unit, soc_port_t port, int logical_lane_offset,
                 phymod_core_init_config_t *core_init_config, 
                 phymod_phy_init_config_t  *pm_phy_init_config)
{
    phy_ctrl_t *pc;
    phy8806x_speed_config_t *speed_config;
    phy8806x_config_t *pCfg;
    phymod_firmware_load_method_t fw_ld_method;
    int port_refclk_int;
    int port_num_lanes;
    int core_num;
    int phy_num_lanes;
    int port_is_higig;
    int phy_passthru; 
    int lane_map_rx, lane_map_tx, lane_map_tx_l=0;
    int i;
    int fiber_pref = 1;
    TXDRV_INXS_t tx_index = TXDRV_DFT_INX;
    uint32  temp32;
    soc_port_if_t soc_if;
    phy_ctrl_t *int_pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    int_pc = INT_PHY_SW_STATE(unit, port);
    if (int_pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pCfg = (phy8806x_config_t *) pc->driver_data;

    /* extract data from SOC_INFO */
    /*port_refclk_int = SOC_INFO(unit).port_refclk_int[port];*/
    port_refclk_int = 156;  
    port_num_lanes = SOC_INFO(unit).port_num_lanes[port];
    port_is_higig = PBMP_MEMBER(PBMP_HG_ALL(unit), port);

    /* figure out how many lanes are in this phy */
    core_num = (logical_lane_offset / 4);
    phy_num_lanes = (port_num_lanes - logical_lane_offset);
    if (phy_num_lanes > MAX_NUM_LANES) {
       phy_num_lanes = MAX_NUM_LANES;
    }
    
    /* 
        CORE configuration
    */
    phymod_core_init_config_t_init(core_init_config);
    /* Set the core_mode to Ethernet. Need to add a config variable to get the right core Mode */
#if defined(INCLUDE_FCMAP)
    if (pc->fcmap_enable) {
        /* Set Core Mode to FIBER_CHANNEL_MODE */
        core_init_config->core_mode = 1;
    } else
#endif
    {
        /* Set Core Mode to ETHERNET_MODE */
        core_init_config->core_mode = 0;
    }
    fw_ld_method = phymodFirmwareLoadMethodNone;

    core_init_config->firmware_load_method  = soc_property_port_get(unit, port, 
                                              spn_LOAD_FIRMWARE, fw_ld_method);
    core_init_config->firmware_load_method &= 0xff; /* clear checksum bits */
    core_init_config->lane_map.num_of_lanes = port_num_lanes;
    
    /* Both internal data structure is logic lane base */
    lane_map_rx = soc_property_port_suffix_num_get(unit, port, core_num,
                                        spn_PHY_RX_LANE_MAP, "core", 0x3210);
    for (i=0; i<NUM_LANES; i++) {
        core_init_config->lane_map.lane_map_rx[i] = (lane_map_rx >> (i * 4 /* 4 bit per lane */)) & 0xf;
    }
 
    lane_map_tx = soc_property_port_suffix_num_get(unit, port, core_num,
                                        spn_PHY_TX_LANE_MAP, "core", 0x3210);
    for (i=0; i<NUM_LANES; i++) {
        lane_map_tx_l |= i << 4*((lane_map_tx >> (i*4)) & 0xf) ;
    }
    for (i=0; i<NUM_LANES; i++) {
        core_init_config->lane_map.lane_map_tx[i] = (lane_map_tx_l >> (i * 4 /*4 bit per lane*/)) & 0xf;
    }

    /* next get the tx/rx polairty info */
    core_init_config->polarity_map.rx_polarity = soc_property_port_get(unit, port, spn_PHY_RX_POLARITY_FLIP, 0);
    core_init_config->polarity_map.tx_polarity = soc_property_port_get(unit, port, spn_PHY_TX_POLARITY_FLIP, 0);

    speed_config = &(pCfg->speed_config);
    speed_config->port_refclk_int  = port_refclk_int;
    speed_config->speed  = soc_property_port_get(unit, port, spn_PORT_INIT_SPEED, pc->speed_max);
    speed_config->ln_lane_count    = phy_num_lanes;
    speed_config->port_is_higig    = port_is_higig;
    speed_config->line_interface   = soc_property_port_get(unit, port, spn_SERDES_IF_TYPE, SOC_PORT_IF_CR);
    speed_config->fiber_pref = soc_property_port_get(unit, port, spn_PHY_FIBER_PREF, 0);
    speed_config->fiber_pref_sys = soc_property_port_get(unit, port, spn_SERDES_FIBER_PREF, 0);
    speed_config->port_mode = soc_property_port_get(unit, port, spn_PHY_OPERATIONAL_MODE, 0);
    speed_config->an_fec = soc_property_port_get(unit, port, spn_PHY_EXT_AN_FEC, 0);
    speed_config->fs_cl72 = soc_property_port_get(unit, port, spn_PHY_INIT_CL72, 0);
    speed_config->fs_cl72_sys = soc_property_port_get(unit, port, spn_PORT_INIT_CL72, 0);
    speed_config->an_cl72 = soc_property_port_get(unit, port, spn_PHY_AN_C72, 1);
    speed_config->fs_fec = soc_property_port_get(unit, port, spn_PHY_FEC_ENABLE, 0);
    speed_config->fs_fec_sys = soc_property_port_get(unit, port, spn_SERDES_FEC_ENABLE, 0);
    speed_config->an_mode  = soc_property_port_get(unit, port, spn_PHY_AN_C73, 0);
    speed_config->repeater_an_phase  = REPEATER_AN_PHASE_NONE;
    speed_config->reset_trial = 0;

#ifdef  BCM_TRIDENT2_SUPPORT
    if ((FLAGS(pc) & PHY8806X_FLEX) && SOC_IS_TRIDENT2(unit)) {
        if ((speed_config->ln_lane_count > 1) && ((speed_config->speed == 10000) || (speed_config->speed == 25000))) {
            speed_config->speed = speed_config->ln_lane_count * speed_config->speed;
            PHYCTRL_SPEED_SET(int_pc, int_pc->unit, int_pc->port, speed_config->speed);
        }

        if ((speed_config->ln_lane_count == 1) && ((speed_config->speed == 40000) || (speed_config->speed == 100000))) {
            if (speed_config->ln_lane_count == 1) {
                switch (speed_config->speed) {
                case 40000:
                case 42000:
                case 100000:
                case 106000:
                    speed_config->speed = speed_config->speed/4;
                    break;
                case 20000:
                case 50000:
                    speed_config->speed = speed_config->speed/2;
                    break;
                default:
                    break;
                }
                PHYCTRL_SPEED_SET(int_pc, int_pc->unit, int_pc->port, speed_config->speed);
            }
        }
    }
#endif

    SOC_IF_ERROR_RETURN(soc_mac_probe(unit, port, &(speed_config->macd)));

    if (speed_config->port_mode == ETH_REPEATER) {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_REPEATER);
        if (FLAGS(pc) & PHY8806X_BMP0) {
            SOC_IF_ERROR_RETURN
                (READ_TOP_FC_MODE_CONTROL_REG(unit, pc, &temp32));
                temp32 |= 1 << 2;
            SOC_IF_ERROR_RETURN
                (WRITE_TOP_FC_MODE_CONTROL_REG(unit, pc, temp32));
        }

        if (FLAGS(pc) & PHY8806X_BMP1) {
            SOC_IF_ERROR_RETURN
                (READ_TOP_FC_MODE_CONTROL_REG(unit, pc, &temp32));
                temp32 |= 1 << 3;
            SOC_IF_ERROR_RETURN
                (WRITE_TOP_FC_MODE_CONTROL_REG(unit, pc, temp32));
        }
    }


    if (speed_config->port_mode == ETH_GEARBOX) {
        speed_config->sys_lane_count  = phy_num_lanes;
        speed_config->ln_lane_count   = GEARBOX_LINE_LANE_CNT;
    } else {
        speed_config->sys_lane_count  = speed_config->ln_lane_count;
    }

    SOC_IF_ERROR_RETURN
        (phy8806x_speed_to_interface_config_get(speed_config, &(core_init_config->interface), &tx_index));

    phy8806x_phymod_interfacetype_to_soc_type(core_init_config->interface.interface_type, &soc_if);

    /* coverity[mixed_enums]: ignore */
    speed_config->interface = soc_if;
    /* coverity[mixed_enums]: ignore */
    core_init_config->interface.interface_type = soc_if;
    /* 
        PHY configuration
    */

    phymod_phy_init_config_t_init(pm_phy_init_config);

    
    pm_phy_init_config->cl72_en = soc_property_port_get(unit, port, spn_PHY_AN_C72, TRUE);
    pm_phy_init_config->an_en = TRUE;

    /* check the higig port mode, then set the default cl73 mode */
    if (port_is_higig) {
        pCfg->cl73an = FALSE;
    } else {
        pCfg->cl73an = PHY8806X_CL73_W_BAM;
    }

    /* by default disable cl37 */
    pCfg->an_cl72 = TRUE;
    pCfg->forced_init_cl72 = FALSE;
    pCfg->an_fec = FALSE; 

    pCfg->cl73an  = soc_property_port_get(unit, port,
                                          spn_PHY_AN_C73, pCfg->cl73an); 
    pCfg->an_cl72 = soc_property_port_get(unit, port,
                                        spn_PHY_AN_C72, pCfg->an_cl72);
    pCfg->an_fec = soc_property_port_get(unit, port,
                                        spn_PHY_EXT_AN_FEC, pCfg->an_fec);

    pCfg->forced_init_cl72 = soc_property_port_get(unit, port,
                                        spn_PHY_INIT_CL72, pCfg->forced_init_cl72);

    
    /* 
        phy_ctrl_t configuration (LOGICAL PORT BASED)
        Only do this once, for the first core of the logical port
    */
    if (core_num == 0) {
        

        /* phy_mode, PHYCTRL_MDIO_ADDR_SHARE, PHY_FLAGS_INDEPENDENT_LANE */
        if (port_num_lanes == 4) {
            pc->phy_mode = PHYCTRL_QUAD_LANE_PORT;
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_INDEPENDENT_LANE);
        } else if (port_num_lanes == 2) {
            pc->phy_mode = PHYCTRL_DUAL_LANE_PORT;
            pc->flags |= PHYCTRL_MDIO_ADDR_SHARE;
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_INDEPENDENT_LANE);
        } else if (port_num_lanes == 1) {
            pc->phy_mode = PHYCTRL_ONE_LANE_PORT;
            pc->flags |= PHYCTRL_MDIO_ADDR_SHARE;
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_INDEPENDENT_LANE);
        }

        /* PHY_FLAGS_PASSTHRU */
        phy_passthru = 0;
        if (PHY_EXTERNAL_MODE(unit, port)) {
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_PASSTHRU);
            phy_passthru = soc_property_port_get(unit, port,
                                                 spn_PHY_PCS_REPEATER, 0);
            if (phy_passthru) {
                PHY_FLAGS_SET(unit, port, PHY_FLAGS_PASSTHRU);
            }
        }

        /* PHY_FLAGS_FIBER */
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_FIBER);
        if (fiber_pref) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_FIBER);
        }
    }

    return SOC_E_NONE;
}
/*
    These are communication signals beween Switch SDK and Phy8806x Firmware
    using per core UART buffer free control word. 
    The 32KB uart buffer is located at 0x278000 + lane num * X (each Quad) 
    with the last four words for control.
 */

#define PHY8806X_READY_FOR_RESET         0xd000
#define PHY8806X_READY_FOR_COMMANDS      0xd001
#define PHY8806X_PROBE_FW_NOT_ALIVE      0xdead
#define PHY8806X_TELL_FW_TO_QUIESCE      0xfeed
#define PHY8806X_FW_ACK_RESP             0xbead

/*
    This API reads the control word written by Phy8806x Firmware
    at the other end of the logical conduit.
 */

int signal_marker_read(int unit, phy_ctrl_t *pc, uint32 *marker)
{
    int rv;
    *marker = 0;
    rv = _phy_8806x_axi_read(unit, pc, 0x27fff0 + (pc->phy_id & 0x3) * 0x100000, marker, 1);
    if (SOC_SUCCESS(rv)) {
        *marker = LE2HOST32((*marker));
    } else {
        LOG_ERROR(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                    "u=%d  p=%d phy id %d marker ptr 0x%lX marker read"
                    " Failed by _axi_read, rv %d \n"),
                     unit, pc->port, pc->phy_id & 3, (unsigned long)marker, rv));
    }
    return rv;
}

/*
    This API writes the control word specified by 'data' for 
    Phy8806x Firmware to read at the other end of the logical conduit.
 */

int signal_init_done(int unit, phy_ctrl_t *pc, uint32 data) 
{
    uint32 temp32;
    int rv;
    temp32 = HOST2LE32(data);
    rv = _phy_8806x_axi_write(unit, pc, 0x27fff0 + (pc->phy_id & 0x3) * 0x100000, &temp32, 1);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                    "u=%d p=%d phy id %d data 0x%x marker write -"
                    " Failed by _axi_write, rv %d \n"),
                     unit, pc->port, pc->phy_id & 3, data, rv));
    }
    return rv;
}

/*
 * Function:
 *      phy_8806x_init
 *  
 *      An SDK "phy" is a logical port, which can consist of from 1-10 lanes,
 *          (A phy with more than 4 lanes requires more than one core).
 *      Per-logical port information is saved in (phy8806x_cfg_t *) &pc->driver_data.
 *      An SDK phy is implemented as one or more PHYMOD "phy"s.
 *  
 *      A PHYMOD "phy" resides completely within a single core, which can be
 *      from 1 to 4 lanes.
 *      Per-phymod phy information is kept in (soc_phymod_ctrl_t) *pc->phymod_ctrl
 *      A phymod phy points to its core.  Up to 4 phymod phys can be on a single core
 *  
 * Purpose:     
 *      Initialize a phy8806x phy
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_8806x_init(int unit, soc_port_t port)
{
    phy_ctrl_t *pc; 
    soc_phymod_ctrl_t *pmc = NULL;
    soc_phymod_phy_t *phy = NULL;
    soc_phymod_core_t *core = NULL;
    phy8806x_config_t *pCfg = NULL;
    phy8806x_speed_config_t *speed_config = NULL;
/*     soc_phy_info_t *pi = NULL; */
    phymod_phy_inf_config_t interface_config;
    phymod_core_status_t core_status;
    phymod_core_info_t core_info;
    int idx, rv = SOC_E_NONE;
    int logical_lane_offset;
    soc_port_ability_t ability;
    TXDRV_INXS_t tx_index = TXDRV_DFT_INX;
    phymod_port_config_t port_config;
    uint32         temp32;
    uint16         temp16;
    uint8          *fw = NULL;
    int            fw_len = 0;
    int            bm, cm;
    int            bmp_port = 0; 
    int            fwdlm;
    uint16         status_l = 0, status_s = 0;
    soc_port_if_t  soc_if;
    int            port_is_higig, port_was_higig;
    uint32         timed_spin, marker;

    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    LOG_INFO(BSL_LS_SOC_PHY, (BSL_META_U(unit,
        "u=%d p=%d PHYCTRL_INIT_STATE = %u speed_max = %d - %s\n"),
         unit, pc->port, PHYCTRL_INIT_STATE(pc), pc->speed_max,
         (FLAGS(pc) & PHY8806X_FLEX) ? "FLEX" : "REGULAR" ));

    if ((PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS1) ||
        (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_DEFAULT)) {

        pc->driver_data = (void*)(pc+1);
        pmc = &pc->phymod_ctrl;
        pCfg = (phy8806x_config_t *) pc->driver_data;
    
        port_is_higig = PBMP_MEMBER(PBMP_HG_ALL(unit), port);
        /* Extract previous higig status before we clear the structure.
           This is possible because the SDK reuses the previous phyctrl.
         */
        port_was_higig = pCfg->speed_config.port_is_higig;

        sal_memset(pCfg, 0, sizeof(*pCfg));
        speed_config = &(pCfg->speed_config);

        sal_memset(&ability, 0, sizeof(ability));

        if (xmod_mutex[unit] == NULL) {
            xmod_mutex[unit] = sal_mutex_create("xmod_mutex");
        }
        
        if (xmod_mutex[unit] == NULL) {
            return SOC_E_MEMORY;
        }

        SOC_IF_ERROR_RETURN
            (READ_TOP_DEV_REV_ID_REG(unit, pc, &TOP_DEV_REV_ID(pc)));

        LED_TX(pc) = 0;
        LED_RX(pc) = 0;
        LED_SPEED(pc) = 0;
        FLAGS(pc) = 0;
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_C45);

        /* Use the following config variable for now until a new one is approved. */
        if (soc_property_port_get(unit, port,spn_PHY_FORCE_FIRMWARE_LOAD, FALSE)) {
            FLAGS(pc) |= PHY8806X_PM;
        }

#ifdef PORTMOD_SUPPORT
        if (SOC_USE_PORTCTRL(unit)) {
            FLAGS(pc) |= PHY8806X_PORTMOD;
        }
#endif

        if ((SOC_CONTROL(unit)->soc_flags & SOC_F_ALL_MODULES_INITED) && (!port_is_higig != !port_was_higig)) {
            FLAGS(pc) |= PHY8806X_ENCAP_CHANGE;
        }

        if (soc_phy_boot_master_get(unit, port, &bm, &cm) == SOC_E_NONE) {
            if (bm) {
                if ((pc->phy_id & 0x4) == 0x0) {
                    FLAGS(pc) |= PHY8806X_BMP0;
                } else {
                    FLAGS(pc) |= PHY8806X_BMP1;
                }
                if (cm) {
                    FLAGS(pc) |= PHY8806X_CHM;
                }
            }
        } else {
            if ((pc->phy_id & 0x7) == 0x0) {
                FLAGS(pc) |= PHY8806X_BMP0;
                FLAGS(pc) |= PHY8806X_CHM;
            } else if ((pc->phy_id & 0x7) == 0x4) {
                FLAGS(pc) |= PHY8806X_BMP1;
            }
        }

        if ((fwdlm = soc_property_port_get(unit, port,spn_MDIO_FIRMWARE_DOWNLOAD_MASTER, 0)) == -1) {
            if (FLAGS(pc) & PHY8806X_CHM) {
                FLAGS(pc) |= PHY8806X_AC;
            }
        } else if (fwdlm == -2) {
            if (FLAGS(pc) & PHY8806X_CHM) {
                FLAGS(pc) |= PHY8806X_FWDLM;
            }
        } else if (fwdlm == pc->port) {
            FLAGS(pc) |= PHY8806X_FWDLM;
        }

        if (!(SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit))) {

            temp32 = 0;

            /* Initialize the uart_ptr via AXI memory of the mHost (pc->phy_id & 0x3) before f/w starts to avoid a race condition. */
            SOC_IF_ERROR_RETURN
                (_phy_8806x_axi_write(unit, pc, 0x27fff8 + (pc->phy_id & 0x3) * 0x100000, &temp32, 1));

            /* Initialize the print_ptr via AXI memory of the mHost (pc->phy_id & 0x3) before f/w starts to avoid a race condition. */
            SOC_IF_ERROR_RETURN
                (_phy_8806x_axi_write(unit, pc, 0x27fffc + (pc->phy_id & 0x3) * 0x100000, &temp32, 1));


            temp32 = soc_property_port_get(unit, port, spn_PHY_DIAG_BMP, 1);
            if (temp32 & 0x4) {
                FLAGS(pc) |= PHY8806X_FW_RESTART;
            }
            temp32 = HOST2LE32(temp32 & 0xffff03);

            /* Initialize the mode via AXI memory of the mHost (pc->phy_id & 0x3) */
            SOC_IF_ERROR_RETURN
                (_phy_8806x_axi_write(unit, pc, 0x27fff4 + (pc->phy_id & 0x3) * 0x100000, &temp32, 1));

        }

#if defined(INCLUDE_FCMAP)
        pc->fcmap_enable = soc_property_port_get(unit, port, spn_FCMAP_ENABLE, 0);

        if  (pc->fcmap_enable && !(PHY_IS_BCM88060(pc) || PHY_IS_BCM88061(pc))) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "u=%d p=%d FC not supported on the device !.\n"), unit, port));
            pc->fcmap_enable = 0;
        }

        pc->fcmap_dev_port = 0;
        pc->fcmap_uc_dev_addr = SOC_FCMAPPHY_MDIO_ADDR(unit, pc->phy_id, 1);
        pc->fcmap_dev_addr = SOC_FCMAPPHY_MDIO_ADDR(unit, pc->phy_id, 1);

        LOG_ERROR(BSL_LS_SOC_PHY,
              (BSL_META_U(unit,
                          "FCMAP ENABLE %d, port %d, dev_addr 0x%x\n"),
                           pc->fcmap_enable, port, pc->fcmap_dev_addr ));

#endif

        if (SOC_CONTROL(unit)->soc_flags & SOC_F_ALL_MODULES_INITED) {
            SOC_IF_ERROR_RETURN
                (_phy_8806x_axi_read(unit, pc, 0x00100008, &temp32, 1));
            temp32 = LE2HOST32(temp32);
            if (temp32 == 0x2032544d) { /* 'MT2 ' converted to uint32 LE = ((' '<< 24) + ('2'<< 16) + ('T'<< 8) + ('M'<< 0)) */

#if defined(INCLUDE_FCMAP)
                SOC_IF_ERROR_RETURN
                    (READ_TOP_FC_MODE_CONTROL_REG(unit, pc, &temp32));

                /* Mask out the other quad's bit. */
                temp32 = (pc->phy_id & 0x4) ? (temp32 & (1 << 1)) : (temp32 & (1 << 0)); 

                if ((pc->fcmap_enable && (!temp32)) || ((!pc->fcmap_enable) && temp32)) {
                    FLAGS(pc) |= PHY8806X_MODE_CHANGE;
                } else 
#endif
                {
                    if (!(FLAGS(pc) & PHY8806X_ENCAP_CHANGE)) {
                        FLAGS(pc) |= PHY8806X_FLEX;
                    }
                }
            } else {
                FLAGS(pc) &= ~PHY8806X_FW_RESTART;
            }
        }

        /* To skip restart */
        if (FLAGS(pc) & PHY8806X_ENCAP_CHANGE) {
             goto PASS5_BOOT;
        }

        if ( ((FLAGS(pc) & (PHY8806X_FLEX | PHY8806X_ENCAP_CHANGE))) && 
             (!(SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit))) ){
            /* determine the number of lanes */
            int num_lanes = SOC_INFO(unit).port_num_lanes[port];
            int starting_lane_index = pc->phy_id & 0x7;
            /* TOP_SOFT_RESET Register Bits 6-9 is for RTS0 and bits 10-13 is for RTS1.
               One bit per lane. Create a RMW mask for this register from the phy id and
               number of lanes, as per the following formula
               ((2**num_lanes) - 1) << (starting_lane_index + 6) (ANDed with Oxf = 4 bits) 
            uint32 single_mask = (((1U << num_lanes) - 1) & 0xf) << (starting_lane_index + 6);
            */
            /* TOP_RTS_MISC_CTRL Register Bits 0-7 is for RTS0 and bits 8-15 is for RTS1.
               Two bits per lane. Create a RMW mask for this register from the phy id and
               number of lanes, as per the following formula
               ((2**(2*num_lanes)) - 1) << (2*starting_lane_index) (ANDed with 0xff = 8 bits)
            */
            uint32 dual_mask = (((1U << (num_lanes << 1)) - 1) & 0xff) << (starting_lane_index << 1); 

            /* LOG_CLI((BSL_META_U(unit,
                        "u=%d p=%d single_mask = 0x%08x dual_mask = 0x%08x\n"),
                        unit, pc->port, single_mask, dual_mask ));
            */

            /*  Prepare Phy8806x FW for soft Reset */

            /* On each of the PHY8806X port micro controller up and running FW,
               obtain readyness for issung Reset.
               Phy Firmware at the other end, if alive,  reads the control word
               PHY8806X_PROBE_FW_NOT_ALIVE and responds by writing PHY8806X_FW_ACK_RESP.
             */

            int i;

            rv = signal_init_done(unit, pc, PHY8806X_PROBE_FW_NOT_ALIVE);
            for (i = 10; i; i--) {
                if (SOC_FAILURE(rv)) {
                    break;
                }
                sal_udelay(100);
                rv = signal_marker_read(unit, pc, &marker);
                if (SOC_FAILURE(rv)) {
                    break;
                }
                if (marker != PHY8806X_PROBE_FW_NOT_ALIVE) {
                    break;
                }
            }
            timed_spin = 40;
            if (SOC_SUCCESS(rv) && i) {

                /* Core running FW and needs to be Qiesced */

                rv = signal_init_done(unit, pc, PHY8806X_TELL_FW_TO_QUIESCE);
                LOG_DEBUG(BSL_LS_SOC_PHY, (BSL_META_U(unit,
                        "u=%d p=%d Quiesce the FW Core, rv %d \n"),
                                unit, pc->port, rv));
                do {
                    if (SOC_FAILURE(rv)) {
                        break;
                    }
                    sal_udelay(500);
                    timed_spin--;
                    rv = signal_marker_read(unit, pc, &marker);
                    if (SOC_FAILURE(rv)) {
                        break;
                    }
                } while ((marker & 0xffff) != PHY8806X_READY_FOR_RESET &&
                              (timed_spin != 0));
            }
            if (SOC_FAILURE(rv) || i == 0 || timed_spin == 0) {
                LOG_DEBUG(BSL_LS_SOC_PHY, (BSL_META_U(unit,
                "u-%d port %d rv %d Reset without Quiescing FW - i %d marker 0x%x \n"),
                                 unit, port, rv, i, marker));
            }
            /* 50 ms delay cushion after port Firmware readiness for Reset */
            sal_udelay(1000 * 50);  
            /*  END, preparation for Firmware Micro controller soft Reset */

            /* Place mHosts of the lane in reset(Done further below).
               Per IProc team recommendation, RTS_MISC_CTRL_REG change
               does not have to be braced by Soft Reset Assert/Deassert.
               The Actual Soft Reset to start Firmware excution from beginning
               is issued down below when Firmware Microcontroller is marked
               Runnable in RTS_MISC_CTRL_REG. The #if 0 below are left behind as
               a reminder of this change, to be removed later.
             */
#if 0
            SOC_IF_ERROR_RETURN
                (READ_TOP_SOFT_RESET_REG(unit, pc, &temp32));
                temp32 |= single_mask;
            SOC_IF_ERROR_RETURN
                (WRITE_TOP_SOFT_RESET_REG(unit, pc, temp32));
#endif

            /* Halt the mHosts of the lane */
            SOC_IF_ERROR_RETURN
                (READ_TOP_RTS_MISC_CTRL_REG(unit, pc, &temp32));
                temp32 &= ~dual_mask;
            SOC_IF_ERROR_RETURN
                (WRITE_TOP_RTS_MISC_CTRL_REG(unit, pc, temp32));
#if 0
            /* Bring mHosts of the lane out of reset */
            SOC_IF_ERROR_RETURN
                (READ_TOP_SOFT_RESET_REG(unit, pc, &temp32));
                temp32 &= ~single_mask;
            SOC_IF_ERROR_RETURN
                (WRITE_TOP_SOFT_RESET_REG(unit, pc, temp32));
#endif

/*            if (FLAGS(pc) & PHY8806X_ENCAP_CHANGE) {
                goto PASS4_BOOT;
            }
*/
            /* jump to 4-th pass */
            PHYCTRL_INIT_STATE_SET(pc,PHYCTRL_INIT_STATE_PASS4);
            return SOC_E_NONE;
        }


#ifdef BCM_WARM_BOOT_SUPPORT
        if (SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit)){
            /* jump to 5-th pass */
            PHYCTRL_INIT_STATE_SET(pc,PHYCTRL_INIT_STATE_PASS5);
            return SOC_E_NONE;
        }
#endif

        FIRMWARE(pc) = bcm_8806x_firmware;
        FIRMWARE_LEN(pc) = bcm_8806x_firmware_size;
        pc->flags |= PHYCTRL_MDIO_BCST;
        pc->dev_name = dev_name_8806x;

        /* Now check whether a newer version of firmware is available for this model */
        rv = soc_phy_fw_get(pc->dev_name, &fw, &fw_len);
        if (SOC_SUCCESS(rv)) {
            FIRMWARE(pc) = fw;
            FIRMWARE_LEN(pc) = fw_len;
        }

        if (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS1) {
            /* initiate second pass of init if needed */
            PHYCTRL_INIT_STATE_SET(pc,PHYCTRL_INIT_STATE_PASS2);
            return SOC_E_NONE;
        }

    }

    /* Firmware download happens between PASS1 and PASS2 */


    if ((PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS2) ||
        (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_DEFAULT)) {

        /* The BMPs should be ready to accept commands at this point
           and the others are in halt. */
        /* Issue xmod_core_init(). (BMPs) */
        if ( FLAGS(pc) & (PHY8806X_BMP0 | PHY8806X_BMP1) ) {
            do {

                rv = _tsc_reg_read(unit, pc, 0, (pc->phy_id & 0x7) + 1, 1, 0, 0xd03d, &status_l);
                if (SOC_FAILURE(rv)) {
                    break;
                }
                rv = _tsc_reg_read(unit, pc, 0, (pc->phy_id & 0x7) + 9, 1, 0, 0xd03d, &status_s);
                if (SOC_FAILURE(rv)) {
                    break;
                }
                if ((status_l== 0x80) &&  (status_s == 0x80)) {
                    break;
                }
            } while (!soc_timeout_check(&TO(pc)));

           if ( status_l != 0x80 ) {
                LOG_CLI((BSL_META_U(unit,
                        "PHY8806X PMD (line) firmware failed to compute CRC: u=%d p=%d  status=%04x"
                        " !.\n"), unit, pc->port, status_l));
           } else {
               rv = _tsc_reg_read(unit, pc, 0, (pc->phy_id & 0x7) + 1, 1, 0, 0xd03e, &temp16);
               if (SOC_FAILURE(rv) || (temp16 != PMD_CRC(pc))) {
                   LOG_CLI((BSL_META_U(unit,
                           "PHY8806X PMD (line) firmware CRC failure: u=%d p=%d  expected=0x%04x computed=0x%04x"
                           " !.\n"), unit, pc->port, PMD_CRC(pc), temp16));
               } else {
                   LOG_CLI((BSL_META_U(unit,
                           "PHY8806X PMD (line) firmware CRC Ok: u=%d p=%d computed=0x%04x"
                           " !.\n"), unit, pc->port, temp16));
               }
           }

           if ( status_s != 0x80 ) {
                LOG_CLI((BSL_META_U(unit,
                        "PHY8806X PMD (sys)  firmware failed to compute CRC: u=%d p=%d  status=%04x"
                        " !.\n"), unit, pc->port, status_s));
           } else {
               rv = _tsc_reg_read(unit, pc, 0, (pc->phy_id & 0x7) + 9, 1, 0, 0xd03e, &temp16);
               if (SOC_FAILURE(rv) || (temp16 != PMD_CRC(pc))) {
                   LOG_CLI((BSL_META_U(unit,
                           "PHY8806X PMD (sys)  firmware CRC failure: u=%d p=%d  expected=0x%04x computed=0x%04x"
                           " !.\n"), unit, pc->port, PMD_CRC(pc), temp16));
               } else {
                   LOG_CLI((BSL_META_U(unit,
                           "PHY8806X PMD (sys)  firmware CRC Ok: u=%d p=%d computed=0x%04x"
                           " !.\n"), unit, pc->port, temp16));
               }
           }
        }

        if ( FLAGS(pc) & PHY8806X_PM ) {
            return SOC_E_NONE;
        }

        if (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS2) {
            /* initiate third pass of init if needed */
            PHYCTRL_INIT_STATE_SET(pc,PHYCTRL_INIT_STATE_PASS3);
            return SOC_E_NONE;
        }
    }

    if ((PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS3) ||
        (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_DEFAULT)) {

        if ( FLAGS(pc) & (PHY8806X_BMP0 | PHY8806X_BMP1) ) {
            /* Get results of xmod_core_init(). (BMPs) */

        }

        if (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS3) {
            /* initiate fourth pass of init if needed */
            PHYCTRL_INIT_STATE_SET(pc,PHYCTRL_INIT_STATE_PASS4);
            return SOC_E_NONE;
        }
    }

    if ((PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS4) ||
        (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_DEFAULT)) {

        /* Enable FC Mode for Quad 0 */
        if ((FLAGS(pc) & PHY8806X_BMP0)) {
            SOC_IF_ERROR_RETURN
                (READ_TOP_FC_MODE_CONTROL_REG(unit, pc, &temp32));
#if defined(INCLUDE_FCMAP)
            if (pc->fcmap_enable) {
                temp32 |= 1 << 0;
            } else {
                temp32 &= ~(1 << 0);
            }
#else
            temp32 &= ~(1 << 0);
#endif
            SOC_IF_ERROR_RETURN
                (WRITE_TOP_FC_MODE_CONTROL_REG(unit, pc, temp32));
        }

        /* Enable FC Mode for Quad 1 */
        if ((FLAGS(pc) & PHY8806X_BMP1)) {
            SOC_IF_ERROR_RETURN
                (READ_TOP_FC_MODE_CONTROL_REG(unit, pc, &temp32));
#if defined(INCLUDE_FCMAP)
            if (pc->fcmap_enable) {
                temp32 |= 1 << 1;
            } else {
                temp32 &= ~(1 << 1);
            }
#else
            temp32 &= ~(1 << 1);
#endif
            SOC_IF_ERROR_RETURN
                (WRITE_TOP_FC_MODE_CONTROL_REG(unit, pc, temp32));
        }
/*
PASS4_BOOT:
*/
        /* Start all non BMPs */
        if (( FLAGS(pc) & (PHY8806X_FLEX | PHY8806X_ENCAP_CHANGE)) ||
            ( !( FLAGS(pc) & (PHY8806X_BMP0 | PHY8806X_BMP1)) )) {

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

            timed_spin = 40;
            if (FLAGS(pc) & PHY8806X_FLEX) {
                do {
                    sal_udelay(500);
                    timed_spin--;
                    rv = signal_marker_read(unit, pc, &marker);
                    if (SOC_FAILURE(rv)) {
                        break;;
                    }
                } while ((marker & 0xffff) != PHY8806X_READY_FOR_COMMANDS &&
                          (timed_spin != 0));
            }

            /* 
               Adiitional 50 millisec cushion after FW Ready.
             */

            sal_udelay(50 * 1000);

            LOG_DEBUG(BSL_LS_SOC_PHY, (BSL_META_U(unit,
                     "unit, port %d rv %d FW sig 0x%x timed_spin %d\n"),
                             port, rv, marker, timed_spin));
    
        }
        if (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS4) {
            /* initiate fourth pass of init if needed */
            PHYCTRL_INIT_STATE_SET(pc,PHYCTRL_INIT_STATE_PASS5);
            return SOC_E_NONE;
        }
    }

    if ((PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS5) ||
        (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_DEFAULT)) {

/* don't restart */
PASS5_BOOT:
        pc = EXT_PHY_SW_STATE(unit, port);
        pc->driver_data = (void*)(pc+1);
        pmc = &pc->phymod_ctrl;
        pCfg = (phy8806x_config_t *) pc->driver_data;
        speed_config = &(pCfg->speed_config);

        /* Loop through all phymod phys that support this SDK phy */
        logical_lane_offset = 0;
        for (idx = 0; idx < pmc->num_phys; idx++) {
            phy = pmc->phy[idx];
            core = phy->core;
            /* determine configuration data structure to default values, based on SOC properties */
            SOC_IF_ERROR_RETURN
                (phy8806x_config_init(unit, port,
                              logical_lane_offset,
                              &core->init_config, &phy->init_config));

            
            if (!core->init) { 
               core_status.pmd_active = 0;
               core->init_config.trcvr_host_managed =  soc_property_port_get(unit, port, spn_FCMAP_TRANSCEIVER_HOST_MANAGED, 0);
               SOC_IF_ERROR_RETURN
                   (phymod_core_init(&core->pm_core, &core->init_config, &core_status));
               core->init = TRUE;
            }

            /*read serdes id info */
            SOC_IF_ERROR_RETURN
                (phymod_core_info_get(&core->pm_core, &core_info)); 


            /* for multicore phys, need to ratchet up to the next batch of lanes */
            logical_lane_offset += core->init_config.lane_map.num_of_lanes;
        }

        /* retrieve chip level information for serdes driver info */
/*         pi = &SOC_PHY_INFO(unit, port); */


        /* set the port to its max or init_speed */
        SOC_IF_ERROR_RETURN
            (phy8806x_speed_to_interface_config_get(speed_config, &interface_config, &tx_index));

        phy8806x_phymod_interfacetype_to_soc_type(interface_config.interface_type, &soc_if);

        speed_config->interface = soc_if;

        phy8806x_port_config_init(speed_config, &port_config);

        if ( FLAGS(pc) & (PHY8806X_BMP0 | PHY8806X_BMP1) ) {
            port_config.is_bootmaster = 1;
        } else {
            port_config.is_bootmaster = 0;
        }

        /* Pass flex flag for both ETH and FC modes */
        if (FLAGS(pc) & PHY8806X_FLEX) {
            port_config.is_flex = 1;
        }
        
        /* Port Init for ETH only */
        if (core->init_config.core_mode == 0) {
            bmp_port = get_bmp_port(port, pc);
            port_config.quad_mode = get_quad_mode(bmp_port);       

            SOC_IF_ERROR_RETURN
                (phymod_port_init(&phy->pm_phy, &port_config));
        }

#if defined(INCLUDE_FCMAP)
        if (pc->fcmap_enable) {
            pc->fcmap_port_cfg = (void*)&port_config;
            rv = soc_fcmapphy_init(unit, port, pc, 
                        BFCMAP_CORE_BCM88060_A0, NULL);
            
            if (!SOC_SUCCESS(rv)) {
                LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "soc_fcmapphy_init: FCMAP init for"
                                      " u=%d p=%d FAILED\n "), unit, port));
            } else {
                LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "soc_fcmapphy_init: FCMAP init for"
                                     " u=%d p=%d SUCCESS\n"), unit, port));
            }
            return (rv);
        }
#endif

        /*next check if cl72 needs to be enabled */
        if (pCfg->forced_init_cl72) {
            SOC_IF_ERROR_RETURN
                (phy8806x_cl72_enable_set(pmc, 1));
        }

        /* setup the port's an cap */
        SOC_IF_ERROR_RETURN
            (phy_8806x_ability_local_get(unit, port, &ability));
 
        ability.medium = SOC_PA_MEDIUM_COPPER;
        ability.channel =  SOC_PA_CHANNEL_LONG;
        ability.fec = SOC_PA_FEC_NONE;

        SOC_IF_ERROR_RETURN
            (phy_8806x_ability_advert_set(unit, port, &ability));

    }
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(pc->unit,
                         "phy_8806x_init: u=%d p=%d\n"), unit, port));

    pc->fiber.enable            = TRUE;
    pc->fiber.force_duplex      = TRUE;
    pc->fiber.preferred         = TRUE;

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8806x_link_get
 * Purpose:
 *      Get layer2 connection status.
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      link - address of memory to store link up/down state.
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_8806x_link_get(int unit, soc_port_t port, int *link)
{
    phy_ctrl_t                *pc;
    int                       rv = SOC_E_NONE;
    uint16                    link_st;
    int                       an = 0;
    int                       an_done = 0;
    phy8806x_config_t         *pCfg;
    phy8806x_speed_config_t   *speed_config;
    phy_ctrl_t                *int_pc = NULL;
    phymod_phy_inf_config_t   interface_config;
    phymod_ref_clk_t          ref_clock;
    int                       speed, flag = 0;
    soc_phymod_phy_t          *phy;
    soc_phymod_ctrl_t         *pmc;
    phymod_autoneg_control_t  an_control;
    int                       an_complete;
    phymod_phy_access_t       *pm_phy;
    int                       local_fault = 0;
    int                       remote_fault = 0;
    int                       fec_val=0;
    int                       ext_phy_line_link_status = 0;
    int                       enable = 0;

    *link = 0;
    /* locate phy control */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pmc = &pc->phymod_ctrl;
    pCfg = (phy8806x_config_t *) pc->driver_data;
    speed_config = &(pCfg->speed_config);

#if defined(INCLUDE_FCMAP)
    if (pc->fcmap_enable) {
        rv = READ_FC_LINK_STATUS_REG(unit, pc, &link_st);
        *link = (link_st & VS_MISC_STATUS_LL_STATUS_BIT_MASK) ? TRUE: FALSE;
        return(rv); 
    }
#endif

    pm_phy = &pmc->phy[0]->pm_phy;
    phymod_port_enable_get(pm_phy, (uint32_t*)&enable);

    if (!enable) {
        *link = 0;
        return rv;
    }

    int_pc = INT_PHY_SW_STATE(unit, port);
    if (int_pc == NULL) {
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(PHY_LINK_GET(int_pc->pd, unit, port, link));
    if ((speed_config->repeater_an_phase == REPEATER_AN_PHASE_2) && (!*link)) {
        speed_config->repeater_an_phase = REPEATER_AN_PHASE_NONE;
        speed_config->repeater_an_resolved_speed = 0;
        speed_config->repeater_an_retry_phase0 = 0;
        phy_8806x_an_set(0, port, 0);
        phy_8806x_an_set(0, port, 1);
        return(SOC_E_NONE);
    }

    if (speed_config->port_mode != ETH_REPEATER) {
        SOC_IF_ERROR_RETURN
        (phy_8806x_an_get(unit, port, &an, &an_done));

        if ( an ) {
            if ( !an_done ) {
                #if 0
                LOG_CLI((BSL_META_U(unit,
                    "phy_8806x_link_get: AN not done, return \n")));
                #endif
                *link = 0;
                return(SOC_E_NONE);
            }
        }    

        rv = READ_LINK_STATUS_REG(unit, pc, &link_st);
        *link = (link_st & VS_MISC_STATUS_LL_STATUS_BIT_MASK) >> 3;
    } else if (speed_config->port_mode == ETH_REPEATER) {
        if (speed_config->repeater_an_phase == REPEATER_AN_PHASE_NONE) {
            /* We are in forced speed repeater mode. */

            local_fault = 0;
            remote_fault = 0;
            /* For now, only implemented for Tomohawk. Expand as needed */
#ifdef BCM_TOMAHAWK_SUPPORT
            if (SOC_IS_TOMAHAWK(unit)) {
                /* Read the FAULT if any */
                if (speed_config->macd) {
                    rv = MAC_CONTROL_GET(speed_config->macd, unit, port, SOC_MAC_CONTROL_FAULT_LOCAL_STATUS, &local_fault);
                    rv = MAC_CONTROL_GET(speed_config->macd, unit, port, SOC_MAC_CONTROL_FAULT_REMOTE_STATUS, &remote_fault);
                }
            }
#endif

#ifdef PORTMOD_SUPPORT
            if (FLAGS(pc) & PHY8806X_PORTMOD)  {
                portmod_port_local_fault_status_get(unit, port, &local_fault);
                portmod_port_remote_fault_status_get(unit, port, &remote_fault);
            }
#endif
            /* read the switch internal PHY link status (PCS link) */
            SOC_IF_ERROR_RETURN(PHY_LINK_GET(int_pc->pd, unit, port, link));

#ifdef PORTMOD_SUPPORT
            if (FLAGS(pc) & PHY8806X_PORTMOD)
                portmod_port_link_get(unit, port, PORTMOD_INIT_F_INTERNAL_SERDES_ONLY, link);
#endif
            rv = READ_LINK_STATUS_REG(unit, pc, &link_st);
            ext_phy_line_link_status = (link_st & VS_MISC_STATUS_LL_STATUS_BIT_MASK) >> 3;
            /* If MT2 line side is down/link partner is disabled, return. */
            if (ext_phy_line_link_status == 0) {
                *link = 0;
                speed_config->reset_trial = 0;
                return(rv);
            } 

            /* 
             * JIRA SDK-118465
             * Reset the MT2 sys and line side if any one of the below conditions are met :
             * (a) If the switch internal PHY PCS link is DOWN.
             * (b) If the switch internal PHY PCS link is UP but the MAC is seeing LOCAL or REMOTE fault.
             */ 
            if (!(*link) || 
                ((*link) && (local_fault || remote_fault) && (speed_config->speed != 1000))) {
                speed_config->reset_trial++;
                /* Wait for the link to stabilize during device init. */
                if (speed_config->reset_trial >= REPEATER_FS_RESET_TRIAL) {
                    LOG_DEBUG(BSL_LS_SOC_PHY, (BSL_META_U(unit,
                        "%s: Calling reset_interface: port:%d link:%d local_fault:%d remote_fault:%d\
                        ext_phy_line_link_status:%d trial:%d\n"),
                        FUNCTION_NAME(), port, *link, local_fault, remote_fault,
                        ext_phy_line_link_status, speed_config->reset_trial));
                    speed_config->reset_trial = 0;

                    /* Send XMOD to reset the system and line side interface of MT2 port */  
                    SOC_IF_ERROR_RETURN(phymod_reset_interface(pm_phy));
                }
            } 

            /* Reset Trial count on link UP. */
            if ((*link) && (!local_fault) && (!remote_fault)) {
                speed_config->reset_trial = 0;
            }

        } else {
            /* Repeater AN mode is enabled. */
            pm_phy = &pmc->phy[0]->pm_phy;
            SOC_IF_ERROR_RETURN
                (phymod_phy_autoneg_get(pm_phy, &an_control, (uint32_t *) &an_complete));
            if (speed_config->repeater_an_phase == REPEATER_AN_PHASE_0) {
                if (an_control.flags & REPEATER_AN_PHASE0_LOCKED) {
                    if (REPEATER_AN_PHASE0_CL74 & an_control.flags)
                        fec_val = 1;
                    else
                        fec_val = 0;
                    
                    PHY_CONTROL_SET(int_pc->pd, unit, port, SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION, fec_val);

                    speed_config->repeater_an_phase = REPEATER_AN_PHASE_1;

                    pmc = &pc->phymod_ctrl;
                    phy = pmc->phy[0];
                    ref_clock = phymodRefClk156Mhz;
                    /* note that the flags have an option to indicate whether it's ok to reprogram the PLL */
                    SOC_IF_ERROR_RETURN(phymod_phy_interface_config_get(&phy->pm_phy,
                                             flag, ref_clock, &interface_config));
                    speed = interface_config.data_rate;
                    speed_config->repeater_an_resolved_speed = interface_config.data_rate;

#ifdef PORTMOD_SUPPORT
                    if ((FLAGS(pc) & PHY8806X_PORTMOD) && (interface_config.data_rate != 100000))  {
                        if (REPEATER_AN_PHASE0_CL74 & an_control.flags) {
                            fec_val = 1;
                            portmod_port_fec_enable_set(unit, port, PORTMOD_INIT_F_INTERNAL_SERDES_ONLY, fec_val);
                        }
                    }
#endif

                    if (interface_config.data_rate == 100000) {
                        speed_config->speed = interface_config.data_rate;
                        PHY_CONTROL_SET(int_pc->pd, unit, port, SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION, 0);
                        PHY_CONTROL_SET(int_pc->pd, unit, port, SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION_CL91, 1);
#ifdef PORTMOD_SUPPORT
                        if (FLAGS(pc) & PHY8806X_PORTMOD)  {
                            fec_val = 1;
                            portmod_port_fec_enable_set(unit, port, PORTMOD_INIT_F_INTERNAL_SERDES_ONLY, fec_val);
                            PHYMOD_FEC_CL91_SET(fec_val);
                            portmod_port_fec_enable_set(unit, port, PORTMOD_INIT_F_INTERNAL_SERDES_ONLY, fec_val);
                        }
#endif
                    } else {
                        if (REPEATER_AN_PHASE0_CL91 & an_control.flags) {
                            PHY_CONTROL_SET(int_pc->pd, unit, port, SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION_CL91, 1);
#ifdef PORTMOD_SUPPORT
                        if (FLAGS(pc) & PHY8806X_PORTMOD)  {
                            fec_val = 1;
                            portmod_port_fec_enable_set(unit, port, PORTMOD_INIT_F_INTERNAL_SERDES_ONLY, fec_val);
                            PHYMOD_FEC_CL91_SET(fec_val);
                            portmod_port_fec_enable_set(unit, port, PORTMOD_INIT_F_INTERNAL_SERDES_ONLY, fec_val);
                        }
#endif
                        } else {
                            /* CL91 is supported only for 100G */
                            PHY_CONTROL_SET(int_pc->pd, unit, port, SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION_CL91, 0);
#ifdef PORTMOD_SUPPORT
                        if (FLAGS(pc) & PHY8806X_PORTMOD)  {
                            fec_val = 0;
                            portmod_port_fec_enable_set(unit, port, PORTMOD_INIT_F_INTERNAL_SERDES_ONLY, fec_val);
                            PHYMOD_FEC_CL91_SET(fec_val);
                            portmod_port_fec_enable_set(unit, port, PORTMOD_INIT_F_INTERNAL_SERDES_ONLY, fec_val);
                        }
#endif
                        }
                    }

                    PHY_SPEED_SET(int_pc->pd, unit, port, speed);

                    /* Set for the next phase */
                    phy_8806x_an_set(0, port, 1);
                    *link = 0;
                    return(SOC_E_NONE);
                } else {
                    *link = 0;
                    if (speed_config->repeater_an_retry_phase0 >= REPEATER_AN_PHASE0_RETRY) {
                        speed_config->repeater_an_phase = REPEATER_AN_PHASE_NONE;
                        speed_config->repeater_an_resolved_speed = 0;
                        phy_8806x_an_set(0, port, 0);
                        phy_8806x_an_set(0, port, 1);
                        speed_config->repeater_an_retry_phase0 = 0;
                    }
                    speed_config->repeater_an_retry_phase0++;
                    return(SOC_E_NONE);
                }
            } else if (speed_config->repeater_an_phase == REPEATER_AN_PHASE_1) {
                pm_phy = &pmc->phy[0]->pm_phy;
                SOC_IF_ERROR_RETURN
                    (phymod_phy_autoneg_get(pm_phy, &an_control, (uint32_t *) &an_complete));
                
                if (an_control.flags & REPEATER_AN_PHASE1_LOCKED) {               
                    PHY_LINK_GET(int_pc->pd, unit, port, link);
                    if (*link) {
                        speed_config->repeater_an_phase = REPEATER_AN_PHASE_2;
                    }
                } else {
                    *link = 0;
                    if (speed_config->repeater_an_retry_phase1 >= REPEATER_AN_PHASE1_RETRY) {
                        speed_config->repeater_an_phase = REPEATER_AN_PHASE_NONE;
                        speed_config->repeater_an_resolved_speed = 0;
                        phy_8806x_an_set(0, port, 0);
                        phy_8806x_an_set(0, port, 1);
                        speed_config->repeater_an_retry_phase1 = 0;
                    }
                    speed_config->repeater_an_retry_phase1++;
                }
                return(SOC_E_NONE);
            } else if (speed_config->repeater_an_phase == REPEATER_AN_PHASE_2) {
                PHY_LINK_GET(int_pc->pd, unit, port, link);
                return(SOC_E_NONE);
            }
        }
        return(SOC_E_NONE);
    } 

    return (rv);
}

/*
 * Function:
 *      phy_8806x_enable_set
 * Purpose:
 *      Enable/Disable phy 
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      enable - on/off state to set
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_8806x_enable_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t                *pc;
    soc_phymod_ctrl_t         *pmc;
    phymod_phy_access_t       *pm_phy;
    phy8806x_config_t         *pCfg;
    phy8806x_speed_config_t   *speed_config;

    /* locate phy control */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

#if defined(INCLUDE_FCMAP)
    if (pc->fcmap_enable) {
        int p;
        int rv = SOC_E_NONE;

        p = SOC_FCMAP_PORTID(unit, port);

        /* If not enable, bring FC link down */
        if (!enable) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "Bringing FC link down u=%d p=%d enable=%d rv=%d\n"),
                      unit, port, enable, rv));

            rv = bfcmap_port_shutdown(p);
        } else {
            LOG_ERROR(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "Bringing FC link UP u=%d p=%d enable=%d rv=%d\n"),
                      unit, port, enable, rv));

            rv = bfcmap_port_link_enable(p);
        }

        if (rv != BFCMAP_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "Failed u=%d p=%d enable=%d rv=%d\n"),
                      unit, port, enable, rv));
            return SOC_E_FAIL;
        }
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "Success u=%d p=%d enable=%d rv=%d\n"),
                  unit, port, enable, rv));

        return SOC_E_NONE;
    }
#endif

    if ( FLAGS(pc) & PHY8806X_PM ) {
        return SOC_E_NONE;
    }

    pmc = &pc->phymod_ctrl;
    pCfg = (phy8806x_config_t *) pc->driver_data;
    speed_config = &(pCfg->speed_config);
    pm_phy = &pmc->phy[0]->pm_phy;
    SOC_IF_ERROR_RETURN
        (phymod_port_enable_set(pm_phy, (uint32_t)enable));

    if (enable) {
        speed_config->reset_trial = 0;
    }

    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_8806x_enable_get
 * Purpose:
 *      Enable/Disable phy 
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      enable - on/off state to set
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_8806x_enable_get(int unit, soc_port_t port, int *enable)
{
    phy_ctrl_t                *pc;
    soc_phymod_ctrl_t         *pmc;
    phymod_phy_access_t       *pm_phy;

    /* locate phy control */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

#if defined(INCLUDE_FCMAP)
    /* Skip for FC */
    if (pc->fcmap_enable) {
        *enable = 1;
        return SOC_E_NONE;
    }
#endif

    if ( FLAGS(pc) & PHY8806X_PM ) {
        return SOC_E_NONE;
    }

    pmc = &pc->phymod_ctrl;
    pm_phy = &pmc->phy[0]->pm_phy;
    SOC_IF_ERROR_RETURN
        (phymod_port_enable_get(pm_phy, (uint32_t*)enable));

    *enable = 1;  
    return (SOC_E_NONE);
}


/*
 * Function:
 *      phy_8806x_duplex_get
 * Purpose:
 *      Get PHY duplex mode
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      duplex - current duplex mode
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_8806x_duplex_get(int unit, soc_port_t port, int *duplex)
{
    *duplex = TRUE;
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8806x_speed_set
 * Purpose:
 *      Set PHY speed
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      speed - link speed in Mbps
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_8806x_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t                *pc;
    phy8806x_config_t             *pCfg;
    soc_phymod_ctrl_t         *pmc;
    soc_phymod_phy_t          *phy;
    phymod_phy_inf_config_t   interface_config;
    int rv = SOC_E_NONE;
    phy_ctrl_t                *int_pc;
    phymod_ref_clk_t          ref_clock;

    /* locate phy control */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    if ((speed == 0) || ( FLAGS(pc) & PHY8806X_PM )) {
        return SOC_E_NONE;
    }

    int_pc = INT_PHY_SW_STATE(unit, port);
    
    if (NULL != int_pc) {
        rv = PHY_SPEED_SET(int_pc->pd, unit, port, speed);
    }

    pmc = &pc->phymod_ctrl;
    pCfg = (phy8806x_config_t *) pc->driver_data;

    phy = pmc->phy[0];
    if (phy == NULL) {
        return SOC_E_INTERNAL;
    }

    ref_clock = phymodRefClk156Mhz;
    SOC_IF_ERROR_RETURN
        (phymod_phy_interface_config_get(&phy->pm_phy,
                                            0, ref_clock, &interface_config));
    interface_config.data_rate = speed;

    /* note that the flags have an option to indicate whether it's ok to reprogram the PLL */
    SOC_IF_ERROR_RETURN
        (phymod_phy_interface_config_set(&phy->pm_phy,
                                            0 /* flags */, &interface_config));

    /* record success */
    pCfg->speed_config.speed = speed;

    pc->fiber.force_speed    = speed;;
    return (rv);
}

/*
 * Function:
 *      phy_8806x_speed_get
 * Purpose:
 *      Get PHY speed
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      speed - current link speed in Mbps
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_8806x_speed_get(int unit, soc_port_t port, int *speed)
{
    phy_ctrl_t                *pc;
    soc_phymod_ctrl_t         *pmc;
    soc_phymod_phy_t          *phy;
    phymod_phy_inf_config_t   interface_config;
    int flag = 0;
    phymod_ref_clk_t          ref_clock;
    int                       an = 0;
    int                       an_done = 0;
    phy_ctrl_t                *int_pc = NULL;
    phy8806x_speed_config_t *speed_config;
    phy8806x_config_t *pCfg;

#if 0
#if defined(INCLUDE_FCMAP)
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint32_t xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *buftxptr, *bufrxptr;
    int xmodtxlen, xmodrxlen;
    uint32_t x_cmd, x_response, cmd;
    uint16_t fc_spd = 0;
    int rv;
#endif
#endif

    /* locate phy control */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }


#if 0
#if defined(INCLUDE_FCMAP)
    if (pc->fcmap_enable) {
        cmd = 0x4;
        x_cmd = HOST2LE32(cmd);

        /* write args to xmod buffer */
        buftxptr = (uint8_t *)xmodtxbuff;
        WRITE_XMOD_ARG_BUFF(buftxptr, &x_cmd, sizeof(x_cmd));
        xmodtxlen = sizeof(x_cmd);
        xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

        xmodrxlen = sizeof(x_response);
        xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);

        /* call xmod */
        rv = phy_8806x_xmod_command(pc->unit, pc->port, XMOD_CMD_MODE_CORE(XMOD_DEV_DEBUG_CMD), xmodtxbuff, xmodtxlen, xmodrxbuff, xmodrxlen);

        /* retrieve the argument from the xmod rx buffer */
        bufrxptr = (uint8_t *)xmodrxbuff;
        READ_XMOD_ARG_BUFF(bufrxptr, &x_response, sizeof(x_response));
        fc_spd = LE2HOST32(x_response);
        /* Set speed to 10G */
        *speed = 10000 ;

        return SOC_E_NONE;
    }
#endif
#endif

    pCfg = (phy8806x_config_t *) pc->driver_data;
    speed_config = &(pCfg->speed_config);
    int_pc = INT_PHY_SW_STATE(unit, port);

    if (speed_config->port_mode == ETH_REPEATER) {
        if (speed_config->repeater_an_phase == REPEATER_AN_PHASE_NONE) {
            /* If forced repeater AN mode */
            SOC_IF_ERROR_RETURN(PHY_SPEED_GET(int_pc->pd, unit, port, speed));
            return (SOC_E_NONE);
        } else if (speed_config->repeater_an_phase == 0) {
            *speed = 0;
            LED_SPEED(pc) = 0;
            return (SOC_E_NONE);
        } else if (speed_config->repeater_an_phase == 1) {
            pmc = &pc->phymod_ctrl;
            phy = pmc->phy[0];
            ref_clock = phymodRefClk156Mhz;
            /* note that the flags have an option to indicate whether it's ok to reprogram the PLL */
            SOC_IF_ERROR_RETURN
                (phymod_phy_interface_config_get(&phy->pm_phy,
                                             flag, ref_clock, &interface_config));
            *speed = interface_config.data_rate;
            LED_SPEED(pc) = *speed;
            return(SOC_E_NONE);
        } else if (speed_config->repeater_an_phase >= 1) {
            *speed = speed_config->repeater_an_resolved_speed;
            LED_SPEED(pc) = *speed;
            return(SOC_E_NONE);
        }
    }

    SOC_IF_ERROR_RETURN
    (phy_8806x_an_get(unit, port, &an, &an_done));

    if ( an ) {
        if ( !an_done ) {
            #if 0
            LOG_CLI((BSL_META_U(unit,
                    "phy_8806x_speed_get: AN not done, return \n")));
            #endif
            *speed = 0;
            LED_SPEED(pc) = 0;
            return(SOC_E_NONE);
        }
    }

    pmc = &pc->phymod_ctrl;

    /* initialize the data structure */
    interface_config.data_rate = 0;

    /* now loop through all cores */
    phy = pmc->phy[0];
    if (phy == NULL) {
        return SOC_E_INTERNAL;
    }

    pCfg = (phy8806x_config_t *) pc->driver_data;
    speed_config = &(pCfg->speed_config);

    if (speed_config->port_mode == ETH_REPEATER) {
        int_pc = INT_PHY_SW_STATE(unit, port);
        SOC_IF_ERROR_RETURN(PHY_SPEED_GET(int_pc->pd, unit, port, speed));
    } else {
        ref_clock = phymodRefClk156Mhz;
        /* note that the flags have an option to indicate whether it's ok to reprogram the PLL */
        SOC_IF_ERROR_RETURN
            (phymod_phy_interface_config_get(&phy->pm_phy,
                                             flag, ref_clock, &interface_config));
        *speed = interface_config.data_rate;
    }
    LED_SPEED(pc) = *speed;
    return (SOC_E_NONE);
}

#define AN_MAX_RETRY    10

/*
 * Function:    
 *      phy_8806x_an_set
 * Purpose:     
 *      Enable or disable auto-negotiation on the specified port.
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      an   - Boolean, if true, auto-negotiation is enabled 
 *              (and/or restarted). If false, autonegotiation is disabled.
 * Returns:     
 *      SOC_E_XXX  _soc_triumph_tx
 */
STATIC int
phy_8806x_an_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t                *pc;
    phy8806x_config_t         *pCfg;
    soc_phymod_ctrl_t         *pmc;
    soc_phymod_phy_t          *phy;
    phymod_autoneg_control_t   an;
    soc_info_t                *si;
    phymod_phy_an_status_t     an_status;
    phymod_an_try_enable_control_t an_ctrl;
    phy8806x_speed_config_t* speed_config;
    phy_ctrl_t                *int_pc;
    uint32_t                   fec_val;

    /* locate phy control */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    int_pc = INT_PHY_SW_STATE(unit, port);

#if defined(INCLUDE_FCMAP)
    /* Skip for FC */
    if (pc->fcmap_enable) {
        return SOC_E_NONE;
    }
#endif

    phymod_autoneg_control_t_init(&an);
    pmc = &pc->phymod_ctrl;
    pCfg = (phy8806x_config_t *) pc->driver_data;
    speed_config = &(pCfg->speed_config);
    si = &SOC_INFO(unit);

    /* only request autoneg on the first core */
    phy = pmc->phy[0];
    if (phy == NULL) {
        return SOC_E_INTERNAL;
    }
    an_ctrl.enable = enable;
    an_ctrl.num_lane_adv = si->port_num_lanes[port];
    an_ctrl.an_mode = phymod_AN_MODE_NONE;
    if(pCfg->cl73an) {
        switch (pCfg->cl73an) {
            case PHY8806X_CL73_W_BAM:  
                an_ctrl.an_mode = phymod_AN_MODE_CL73BAM;  
                break ;
            case PHY8806X_CL73_WO_BAM:
                an_ctrl.an_mode = phymod_AN_MODE_CL73;  
                break ;
            case PHY8806X_CL73_HPAM_VS_SW:    
                an_ctrl.an_mode = phymod_AN_MODE_HPAM;  
                break ;  /* against TD+ */
            case PHY8806X_CL73_HPAM:
                an_ctrl.an_mode = phymod_AN_MODE_HPAM;  
                break ;  /* against TR3 */
            case PHY8806X_CL73_CL37:    
                an_ctrl.an_mode = phymod_AN_MODE_CL73;  
                break ;
            default:
                break;
        }
    }

    pc->fiber.autoneg_enable = enable;

    if (!enable) {
        an.enable = 0; /* Disable AN */
        an.num_lane_adv = si->port_num_lanes[port];
        an.an_mode = an_ctrl.an_mode;
        SOC_IF_ERROR_RETURN
            (phymod_phy_autoneg_set(&phy->pm_phy, &an));
        if (speed_config->repeater_an_phase > REPEATER_AN_PHASE_NONE) {
            PHY_CONTROL_SET(int_pc->pd, unit, port, SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION, 0);
            PHY_CONTROL_SET(int_pc->pd, unit, port, SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION_CL91, 0);
        }
        speed_config->repeater_an_phase  = REPEATER_AN_PHASE_NONE;
        speed_config->repeater_an_resolved_speed  = 0;
        speed_config->repeater_an_retry_phase0 = 0;
        
        if ((speed_config->port_mode == ETH_REPEATER) && (speed_config->speed == 100000)) {
            fec_val = 0;
            PHY_CONTROL_SET(int_pc->pd, unit, port, SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION_CL91, fec_val);
#ifdef PORTMOD_SUPPORT
            if (FLAGS(pc) & PHY8806X_PORTMOD)  {
                portmod_port_fec_enable_set(unit, port, PORTMOD_INIT_F_INTERNAL_SERDES_ONLY, fec_val);
                PHYMOD_FEC_CL91_SET(fec_val);
                portmod_port_fec_enable_set(unit, port, PORTMOD_INIT_F_INTERNAL_SERDES_ONLY, fec_val);
            }
#endif
        }

    } else {
        an_ctrl.speed = pc->speed_max;
        if (speed_config->port_mode == ETH_REPEATER) {
            if (speed_config->repeater_an_phase == REPEATER_AN_PHASE_NONE) {
                speed_config->repeater_an_phase = REPEATER_AN_PHASE_0;
                /* Send SET_AN XMOD command to FW for enabling AutoNeg phase 0. */
                SOC_IF_ERROR_RETURN
                    (phymod_phy_autoneg_try_enable(&phy->pm_phy, &an_ctrl, &an_status));
                 return (SOC_E_NONE);
            } else if (speed_config->repeater_an_phase == REPEATER_AN_PHASE_1) {
                an_ctrl.speed = speed_config->repeater_an_resolved_speed;
                SOC_IF_ERROR_RETURN
                    (phymod_phy_autoneg_try_enable(&phy->pm_phy, &an_ctrl, &an_status));
                return (SOC_E_NONE);
            } else if (speed_config->repeater_an_phase == REPEATER_AN_PHASE_2) {
                return (SOC_E_NONE);
            }
        }

        /* Send SET_AN XMOD command to FW for enabling AutoNeg. */
        SOC_IF_ERROR_RETURN
            (phymod_phy_autoneg_try_enable(&phy->pm_phy, &an_ctrl, &an_status));
    }

    return (SOC_E_NONE);
}

/*
 * Function:    
 *      phy_8806x_an_get
 * Purpose:     
 *      Get the current auto-negotiation status (enabled/busy)
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      an   - (OUT) if true, auto-negotiation is enabled.
 *      an_done - (OUT) if true, auto-negotiation is complete. This
 *              value is undefined if an == false.
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
phy_8806x_an_get(int unit, soc_port_t port, int *an, int *an_done)
{
    phy_ctrl_t* pc; 
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t *pm_phy;
    phymod_autoneg_control_t an_control;
    int idx, an_complete;
    phy8806x_config_t         *pCfg;
    phy8806x_speed_config_t   *speed_config;
    phy_ctrl_t                *int_pc;
    int                       link;

    pc = EXT_PHY_SW_STATE(unit, port);
    pmc = &pc->phymod_ctrl;

    pCfg = (phy8806x_config_t *) pc->driver_data;
    speed_config = &(pCfg->speed_config);

#if defined(INCLUDE_FCMAP)
    /* Skip for FC */
    if (pc->fcmap_enable) {
        return SOC_E_NONE;
    }
#endif

    int_pc = INT_PHY_SW_STATE(unit, port);

    sal_memset(&an_control, 0x0, sizeof(an_control));
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;
        SOC_IF_ERROR_RETURN
            (phymod_phy_autoneg_get(pm_phy, &an_control, (uint32_t *) &an_complete));
    }

    if (speed_config->port_mode != ETH_REPEATER) {
        if (an_control.enable) {
            *an = 1;
            *an_done = an_complete;
        } else {
            *an = 0;
            *an_done = 0;
        }
    } else if (speed_config->repeater_an_phase == REPEATER_AN_PHASE_NONE) {
        *an = 0;
        *an_done = 0;
    } else if (speed_config->repeater_an_phase >= REPEATER_AN_PHASE_0) {
        *an = 1;
        *an_done = 0;
    } else if (speed_config->repeater_an_phase >= REPEATER_AN_PHASE_1) {
        *an = 1;
        PHY_LINK_GET(int_pc->pd, unit, port, &link);
        if (link)
            *an_done = 1;
    } else if (speed_config->repeater_an_phase >= REPEATER_AN_PHASE_2) {
        *an = 1;
        *an_done = 1;
    }

    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_8806x_ability_advert_set
 * Purpose:
 *      Set the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number.
 *      ability - Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is set only for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
phy_8806x_ability_advert_set(int unit, soc_port_t port,
                          soc_port_ability_t *ability)
{
    phy_ctrl_t                *pc;
    phy8806x_config_t         *pCfg;
    soc_phymod_ctrl_t         *pmc;
    soc_phymod_phy_t          *phy;
    int                       port_num_lanes;
    int                       line_interface;
    uint32_t                  an_tech_ability;
    uint32_t                  an_bam37_ability;
    uint32_t                  an_bam73_ability;
    _shr_port_mode_t          speed_full_duplex;
    phymod_autoneg_ability_t  phymod_autoneg_ability; 
    phy8806x_speed_config_t*  speed_config;

    /* locate phy control */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

#if defined(INCLUDE_FCMAP)
    /* Skip for FC */
    if (pc->fcmap_enable) {
        return SOC_E_NONE;
    }
#endif

    phymod_autoneg_ability_t_init(&phymod_autoneg_ability);

    pmc = &pc->phymod_ctrl;
    pCfg = (phy8806x_config_t *) pc->driver_data;

    /* only set abilities on the first core */
    phy = pmc->phy[0];
    if (phy == NULL) {
        return SOC_E_INTERNAL;
    }

    pCfg = (phy8806x_config_t *) pc->driver_data;
    port_num_lanes = pCfg->speed_config.ln_lane_count;
    line_interface = pCfg->speed_config.line_interface;

    an_tech_ability  = 0;
    an_bam37_ability = 0;
    an_bam73_ability = 0;
    speed_full_duplex = ability->speed_full_duplex;

    /* 
         an_tech_ability
    */
    if (port_num_lanes == 4) {
		if (ability->medium) {
            if (speed_full_duplex & SOC_PA_SPEED_100GB) {
                if (ability->medium & SOC_PA_MEDIUM_BACKPLANE) {
                    PHYMOD_AN_CAP_100G_KR4_SET(an_tech_ability);
                } else if (ability->medium & SOC_PA_MEDIUM_COPPER) {
                    PHYMOD_AN_CAP_100G_CR4_SET(an_tech_ability);
                } else {
                    PHYMOD_AN_CAP_100G_KR4_SET(an_tech_ability);
                }
            }
            if (speed_full_duplex & SOC_PA_SPEED_40GB) {
                if (ability->medium & SOC_PA_MEDIUM_BACKPLANE) {
                    PHYMOD_AN_CAP_40G_KR4_SET(an_tech_ability);
                } else if (ability->medium & SOC_PA_MEDIUM_COPPER) {
                    PHYMOD_AN_CAP_40G_CR4_SET(an_tech_ability);
                } else {
                    PHYMOD_AN_CAP_40G_KR4_SET(an_tech_ability);
                }
            }
		} else {
			if (speed_full_duplex & SOC_PA_SPEED_100GB) {
				if (line_interface & (1 << SOC_PORT_IF_CR4)) {
					PHYMOD_AN_CAP_100G_CR4_SET(an_tech_ability);
				} else {
					PHYMOD_AN_CAP_100G_KR4_SET(an_tech_ability);
				}
			}
			if (speed_full_duplex & SOC_PA_SPEED_40GB) {
				if (line_interface & (1 << SOC_PORT_IF_CR4)) {
					PHYMOD_AN_CAP_40G_CR4_SET(an_tech_ability);
				} else {
					PHYMOD_AN_CAP_40G_KR4_SET(an_tech_ability);
				}
			}			
		}
    } else if (port_num_lanes == 2) {
        if (ability->medium) {
            if (speed_full_duplex & SOC_PA_SPEED_20GB) {
                if (ability->medium & SOC_PA_MEDIUM_BACKPLANE) {
                    PHYMOD_BAM_CL73_CAP_20G_KR2_SET(an_bam73_ability);
                } else if (ability->medium & SOC_PA_MEDIUM_COPPER) {
                    PHYMOD_BAM_CL73_CAP_20G_CR2_SET(an_bam73_ability);
                } else {
                    PHYMOD_BAM_CL73_CAP_20G_KR2_SET(an_bam73_ability);
                }
            }
            if (speed_full_duplex & SOC_PA_SPEED_40GB) {
                if (ability->medium & SOC_PA_MEDIUM_BACKPLANE) {
                    PHYMOD_BAM_CL73_CAP_40G_KR2_SET(an_bam73_ability);
                } else if (ability->medium & SOC_PA_MEDIUM_COPPER) {
                    PHYMOD_BAM_CL73_CAP_40G_CR2_SET(an_bam73_ability);
                } else {
                    PHYMOD_BAM_CL73_CAP_40G_KR2_SET(an_bam73_ability);
                }
            }
            if (speed_full_duplex & SOC_PA_SPEED_50GB) {
                if (ability->medium & SOC_PA_MEDIUM_BACKPLANE) {
                    PHYMOD_BAM_CL73_CAP_50G_KR2_SET(an_bam73_ability);
                } else if (ability->medium & SOC_PA_MEDIUM_COPPER) {
                    PHYMOD_BAM_CL73_CAP_50G_CR2_SET(an_bam73_ability);
                } else {
                    PHYMOD_BAM_CL73_CAP_50G_KR2_SET(an_bam73_ability);
                }
            }			
		} else {
			if(speed_full_duplex & SOC_PA_SPEED_20GB) {
				/* need to fix CR2 bit shift issue */
				if (line_interface & PHY8806X_IF_CR2) {
					PHYMOD_BAM_CL73_CAP_20G_CR2_SET(an_bam73_ability);
				} else {
					PHYMOD_BAM_CL73_CAP_20G_KR2_SET(an_bam73_ability);
				}
			}
			if(speed_full_duplex & SOC_PA_SPEED_40GB) {
				/* need to fix CR2 bit shift issue */
				if (line_interface & PHY8806X_IF_CR2) {
					PHYMOD_BAM_CL73_CAP_40G_CR2_SET(an_bam73_ability);
				} else {
					PHYMOD_BAM_CL73_CAP_40G_KR2_SET(an_bam73_ability);
				}
			}
			if(speed_full_duplex & SOC_PA_SPEED_50GB) {
				/* need to fix CR2 bit shift issue */
				if (line_interface & PHY8806X_IF_CR2) {
					PHYMOD_BAM_CL73_CAP_50G_CR2_SET(an_bam73_ability);
				} else {
					PHYMOD_BAM_CL73_CAP_50G_KR2_SET(an_bam73_ability);
				}
			}
		}
    } else {
        if ((ability->medium || ability->channel)) {
            if (speed_full_duplex & SOC_PA_SPEED_25GB) {
                if (ability->medium & SOC_PA_MEDIUM_BACKPLANE) {
                    /* When in MSA AN mode : We do not want to populate the base page with IEEE mode. 
                     * If we do, then IEEE takes the higher priority. Hence we add the following exception
                     * block for each 25G case below: if(pCfg->cl73an!=PHY8806X_MSA)  */
                    if (pCfg->cl73an != PHY8806X_MSA) {
                       if (ability->channel & SOC_PA_CHANNEL_SHORT) {
                            PHYMOD_AN_CAP_25G_KRS1_SET(an_tech_ability);
                       }
                       if (ability->channel & SOC_PA_CHANNEL_LONG) {
                            PHYMOD_AN_CAP_25G_KR1_SET(an_tech_ability);
                       }
                    }
                    PHYMOD_BAM_CL73_CAP_25G_KR1_SET(an_bam73_ability);
                } else if (ability->medium & SOC_PA_MEDIUM_COPPER) {
                    if (pCfg->cl73an != PHY8806X_MSA) {
                        if (ability->channel & SOC_PA_CHANNEL_SHORT) {
                            PHYMOD_AN_CAP_25G_CRS1_SET(an_tech_ability);
                        }
 
                        if (ability->channel & SOC_PA_CHANNEL_LONG) {
                            PHYMOD_AN_CAP_25G_CR1_SET(an_tech_ability);
                        }
                    }
                    PHYMOD_BAM_CL73_CAP_25G_CR1_SET(an_bam73_ability);
                } else {
                    if (pCfg->cl73an != PHY8806X_MSA) {
                        if (ability->channel & SOC_PA_CHANNEL_SHORT) {
                            PHYMOD_AN_CAP_25G_KRS1_SET(an_tech_ability);
                        }
 
                        if (ability->channel & SOC_PA_CHANNEL_LONG) {
                            PHYMOD_AN_CAP_25G_KR1_SET(an_tech_ability);
                        }
                    }
                    PHYMOD_BAM_CL73_CAP_25G_KR1_SET(an_bam73_ability);
                }
            }
            if (speed_full_duplex & SOC_PA_SPEED_10GB) {
                PHYMOD_AN_CAP_10G_KR_SET(an_tech_ability);
            }
            if (speed_full_duplex & SOC_PA_SPEED_1000MB) {
                PHYMOD_AN_CAP_1G_KX_SET(an_tech_ability);
            }	
		} else {
			if(speed_full_duplex & SOC_PA_SPEED_25GB) {
				if (line_interface & (1 << SOC_PORT_IF_CR)) {
					/* next check if just CL73 or Cl73BAM */
					PHYMOD_AN_CAP_25G_CR1_SET(an_tech_ability);
					PHYMOD_BAM_CL73_CAP_25G_CR1_SET(an_bam73_ability);
				} else {
					/* next check if just CL73 or Cl73BAM */
					PHYMOD_AN_CAP_25G_KR1_SET(an_tech_ability);
					PHYMOD_BAM_CL73_CAP_25G_KR1_SET(an_bam73_ability);
				}
			}
		}
    }

   if (port_num_lanes == 4) {          /* 4 lanes */
        if(speed_full_duplex & SOC_PA_SPEED_40GB)
            PHYMOD_BAM_CL37_CAP_40G_SET(an_bam37_ability);
    } else if (port_num_lanes == 2) {     /* 2 lanes */
        if(speed_full_duplex & SOC_PA_SPEED_20GB){
            PHYMOD_BAM_CL37_CAP_20G_X2_SET(an_bam37_ability);
            PHYMOD_BAM_CL37_CAP_20G_X2_CX4_SET(an_bam37_ability);
        }
    } else {                              /* 1 lane */
        if (speed_full_duplex & SOC_PA_SPEED_10GB) {
            PHYMOD_AN_CAP_10G_KR_SET(an_tech_ability);
           PHYMOD_AN_CAP_10G_KR_SET(an_tech_ability);
        }
        if (speed_full_duplex & SOC_PA_SPEED_1000MB) {
            PHYMOD_AN_CAP_1G_KX_SET(an_tech_ability);
        }
    }
    phymod_autoneg_ability.an_cap = an_tech_ability;
    phymod_autoneg_ability.cl73bam_cap = an_bam73_ability; 
    phymod_autoneg_ability.cl37bam_cap = an_bam37_ability; 


    PHYMOD_AN_CAP_SYMM_PAUSE_CLR(&phymod_autoneg_ability);
    PHYMOD_AN_CAP_ASYM_PAUSE_CLR(&phymod_autoneg_ability);

    switch (ability->pause & (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX)) {
    case SOC_PA_PAUSE_TX:
        PHYMOD_AN_CAP_ASYM_PAUSE_SET(&phymod_autoneg_ability);
        break;
    case SOC_PA_PAUSE_RX:
        /* an_adv |= MII_ANA_C37_PAUSE | MII_ANA_C37_ASYM_PAUSE; */
        PHYMOD_AN_CAP_ASYM_PAUSE_SET(&phymod_autoneg_ability);
        PHYMOD_AN_CAP_SYMM_PAUSE_SET(&phymod_autoneg_ability);
        break;
    case SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX:
        PHYMOD_AN_CAP_SYMM_PAUSE_SET(&phymod_autoneg_ability);
        break;
    }

    /* also set the sgmii speed */
    phymod_autoneg_ability.sgmii_speed = phymod_CL37_SGMII_1000M;

    /* next check if we need to set cl37 attribute */
    if (pCfg->an_cl72) {
        phymod_autoneg_ability.an_cl72 = 1;
    }
    if (pCfg->hg_mode) {
        phymod_autoneg_ability.an_hg2 = 1;
    }

    /*next need to check FEC ability */
    if (ability->fec == SOC_PA_FEC_NONE) {
        PHYMOD_AN_FEC_OFF_SET(phymod_autoneg_ability.an_fec);
    } else {
        if (ability->fec & SOC_PA_FEC_CL74)
            PHYMOD_AN_FEC_CL74_SET(phymod_autoneg_ability.an_fec);
        if (ability->fec & SOC_PA_FEC_CL91)
            PHYMOD_AN_FEC_CL91_SET(phymod_autoneg_ability.an_fec);
    }
    
    if (pCfg->an_fec == 1)
        PHYMOD_AN_FEC_CL74_SET(phymod_autoneg_ability.an_fec);
    else if (pCfg->an_fec == 2)
        PHYMOD_AN_FEC_CL91_SET(phymod_autoneg_ability.an_fec);

    SOC_IF_ERROR_RETURN
        (phymod_phy_autoneg_ability_set(&phy->pm_phy, &phymod_autoneg_ability));

    pc->fiber.advert_ability = *ability;

    speed_config = &(pCfg->speed_config);
    if (speed_config->an_fec)
        pc->fiber.advert_ability.fec = 1;
    else
        pc->fiber.advert_ability.fec = 0;

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8806x_ability_advert_get
 * Purpose:
 *      Get the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number.
 *      ability - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
phy_8806x_ability_advert_get(int unit, soc_port_t port,
                              soc_port_ability_t *ability)
{
    phy_ctrl_t                *pc;
    soc_phymod_ctrl_t         *pmc;
    soc_phymod_phy_t          *phy;
    int                       reg37_ability;
    int                       reg73_ability;
    int                       reg73bam_ability;
    int                       reg_ability;
    _shr_port_mode_t          speed_full_duplex;
    phymod_autoneg_ability_t  phymod_autoneg_ability;

    /* locate phy control */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

#if defined(INCLUDE_FCMAP) 
    if (pc->fcmap_enable) {
        ability->pause |= SOC_PA_PAUSE_TX;
        ability->pause |= SOC_PA_PAUSE_RX;
        return SOC_E_NONE;
    }
#endif

    pmc = &pc->phymod_ctrl;

    /* only get abilities from the first core */
    phy = pmc->phy[0];
    if (phy == NULL) {
        return SOC_E_INTERNAL;
    }

    phymod_autoneg_ability_t_init(&phymod_autoneg_ability);
    SOC_IF_ERROR_RETURN
        (phymod_phy_autoneg_ability_get(&phy->pm_phy, &phymod_autoneg_ability));


    speed_full_duplex = 0;

    /* retrieve CL73 abilities */
    reg73_ability = phymod_autoneg_ability.an_cap;
   speed_full_duplex|= PHYMOD_AN_CAP_100G_CR4_GET(reg73_ability) ?SOC_PA_SPEED_100GB:0;
    speed_full_duplex|= PHYMOD_AN_CAP_100G_KR4_GET(reg73_ability) ?SOC_PA_SPEED_100GB:0;
    speed_full_duplex|= PHYMOD_AN_CAP_40G_CR4_GET(reg73_ability) ?SOC_PA_SPEED_40GB:0;
    speed_full_duplex|= PHYMOD_AN_CAP_40G_KR4_GET(reg73_ability) ?SOC_PA_SPEED_40GB:0;
    speed_full_duplex|= PHYMOD_AN_CAP_10G_KR_GET(reg73_ability)  ?SOC_PA_SPEED_10GB:0;
    speed_full_duplex|= PHYMOD_AN_CAP_10G_KX4_GET(reg73_ability) ?SOC_PA_SPEED_10GB:0;
    speed_full_duplex|= PHYMOD_AN_CAP_1G_KX_GET(reg73_ability)   ?SOC_PA_SPEED_1000MB:0;
   speed_full_duplex|= PHYMOD_AN_CAP_25G_CR1_GET(reg73_ability) ?SOC_PA_SPEED_25GB:0;
   speed_full_duplex|= PHYMOD_AN_CAP_25G_KR1_GET(reg73_ability) ?SOC_PA_SPEED_25GB:0;
   
    /* retrieve CL73bam abilities */
    reg73bam_ability = phymod_autoneg_ability.cl73bam_cap;
    speed_full_duplex|= PHYMOD_BAM_CL73_CAP_20G_CR2_GET(reg73bam_ability) ?SOC_PA_SPEED_20GB:0;
    speed_full_duplex|= PHYMOD_BAM_CL73_CAP_20G_KR2_GET(reg73bam_ability) ?SOC_PA_SPEED_20GB:0;
    speed_full_duplex|= PHYMOD_BAM_CL73_CAP_40G_CR2_GET(reg73bam_ability) ?SOC_PA_SPEED_40GB:0;
    speed_full_duplex|= PHYMOD_BAM_CL73_CAP_40G_KR2_GET(reg73bam_ability) ?SOC_PA_SPEED_40GB:0;
    speed_full_duplex|= PHYMOD_BAM_CL73_CAP_50G_CR2_GET(reg73bam_ability) ?SOC_PA_SPEED_50GB:0;
    speed_full_duplex|= PHYMOD_BAM_CL73_CAP_50G_KR2_GET(reg73bam_ability) ?SOC_PA_SPEED_50GB:0;
   speed_full_duplex|= PHYMOD_BAM_CL73_CAP_25G_CR1_GET(reg73bam_ability) ?SOC_PA_SPEED_25GB:0;
   speed_full_duplex|= PHYMOD_BAM_CL73_CAP_25G_KR1_GET(reg73bam_ability) ?SOC_PA_SPEED_25GB:0;
   
    /* retrieve CL37 abilities */
    reg37_ability = phymod_autoneg_ability.cl37bam_cap;
    speed_full_duplex|= PHYMOD_BAM_CL37_CAP_40G_GET(reg37_ability) ? SOC_PA_SPEED_40GB:0;
    speed_full_duplex|= PHYMOD_BAM_CL37_CAP_25P455G_GET(reg37_ability) ?    SOC_PA_SPEED_25GB:0;
    speed_full_duplex|= PHYMOD_BAM_CL37_CAP_20G_X2_CX4_GET(reg37_ability)? SOC_PA_SPEED_20GB:0;
    speed_full_duplex|= PHYMOD_BAM_CL37_CAP_20G_X2_GET(reg37_ability)?     SOC_PA_SPEED_20GB:0;
    speed_full_duplex|= PHYMOD_BAM_CL37_CAP_20G_X4_GET(reg37_ability)?     SOC_PA_SPEED_20GB:0;
    speed_full_duplex|= PHYMOD_BAM_CL37_CAP_20G_X4_CX4_GET(reg37_ability)? SOC_PA_SPEED_20GB:0;
    speed_full_duplex|= PHYMOD_BAM_CL37_CAP_10G_X2_CX4_GET(reg37_ability)? SOC_PA_SPEED_10GB:0;
    speed_full_duplex|= PHYMOD_BAM_CL37_CAP_10G_DXGXS_GET(reg37_ability)?  SOC_PA_SPEED_10GB:0;
    speed_full_duplex|= PHYMOD_BAM_CL37_CAP_10G_CX4_GET(reg37_ability)?    SOC_PA_SPEED_10GB:0;

    /* retrieve "pause" abilities */
    reg_ability = phymod_autoneg_ability.capabilities;

    ability->pause = 0;
    if (reg_ability == PHYMOD_AN_CAP_ASYM_PAUSE) {
        ability->pause = SOC_PA_PAUSE_TX;
    } else if (reg_ability == (PHYMOD_AN_CAP_SYMM_PAUSE|PHYMOD_AN_CAP_ASYM_PAUSE)) {
        ability->pause = SOC_PA_PAUSE_RX;
    } else if (reg_ability == PHYMOD_AN_CAP_SYMM_PAUSE) {
        ability->pause = (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX);
    }

    ability->speed_full_duplex = speed_full_duplex;
    ability->encap = SOC_PA_ENCAP_IEEE | SOC_PA_ENCAP_HIGIG | SOC_PA_ENCAP_HIGIG2;
    if (PHYMOD_AN_CAP_25G_KRS1_GET(reg73_ability) || PHYMOD_AN_CAP_25G_CRS1_GET(reg73_ability)) {
        ability->channel = SOC_PA_CHANNEL_SHORT;
    } 

    if (PHYMOD_AN_CAP_25G_KR1_GET(reg73_ability) || PHYMOD_AN_CAP_25G_CR1_GET(reg73_ability)) {
        ability->channel |= SOC_PA_CHANNEL_LONG;
    }

    if (PHYMOD_AN_CAP_100G_CR4_GET(reg73_ability) || PHYMOD_AN_CAP_40G_CR4_GET(reg73_ability)  ||
        PHYMOD_AN_CAP_25G_CR1_GET(reg73_ability)  || PHYMOD_AN_CAP_25G_CRS1_GET(reg73_ability) ||
        PHYMOD_BAM_CL73_CAP_20G_CR2_GET(reg73bam_ability)  || PHYMOD_BAM_CL73_CAP_40G_CR2_GET(reg73bam_ability) ||
        PHYMOD_BAM_CL73_CAP_50G_CR2_GET(reg73bam_ability)  || PHYMOD_BAM_CL73_CAP_25G_CR1_GET(reg73bam_ability) ||
        PHYMOD_BAM_CL73_CAP_50G_CR4_GET(reg73bam_ability)) {
        ability->medium = SOC_PA_MEDIUM_COPPER;
    } else {
        ability->medium = SOC_PA_MEDIUM_BACKPLANE;
    }

    ability->fec = 0;
    if (PHYMOD_AN_FEC_OFF_GET(phymod_autoneg_ability.an_fec)) {
        ability->fec = SOC_PA_FEC_NONE;
    } else {
        if (PHYMOD_AN_FEC_CL74_GET(phymod_autoneg_ability.an_fec)) {
            ability->fec |= SOC_PA_FEC_CL74;
        }
        if (PHYMOD_AN_FEC_CL91_GET(phymod_autoneg_ability.an_fec)) {
            ability->fec |= SOC_PA_FEC_CL91;
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8806x_ability_remote_get
 * Purpose:
 *      Get the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number.
 *      ability - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
phy_8806x_ability_remote_get(int unit, soc_port_t port,
                               soc_port_ability_t *ability)
{
    phy_ctrl_t                *pc;
    soc_phymod_ctrl_t         *pmc;
    soc_phymod_phy_t          *phy;
    int                       reg73_ability;
    int                       reg73bam_ability;
    int                       reg37_ability;
    int                       reg_ability;
    _shr_port_mode_t          speed_full_duplex;
    phymod_autoneg_ability_t  phymod_autoneg_ability; 

    /* locate phy control */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

#if defined(INCLUDE_FCMAP)
    if (pc->fcmap_enable) {
        ability->pause |= SOC_PA_PAUSE_TX;
        ability->pause |= SOC_PA_PAUSE_RX;
        return SOC_E_NONE;
    }
#endif

    pmc = &pc->phymod_ctrl;

    /* only get abilities from the first core */
    phy = pmc->phy[0];
    if (phy == NULL) {
        return SOC_E_INTERNAL;
    }

    phymod_autoneg_ability_t_init(&phymod_autoneg_ability);
    SOC_IF_ERROR_RETURN
        (phymod_phy_autoneg_remote_ability_get(&phy->pm_phy, &phymod_autoneg_ability));

    speed_full_duplex = 0;

    /* retrieve CL73 abilities */
    reg73_ability = phymod_autoneg_ability.an_cap;
    speed_full_duplex|= PHYMOD_AN_CAP_100G_CR4_GET(reg73_ability) ?SOC_PA_SPEED_100GB:0;
    speed_full_duplex|= PHYMOD_AN_CAP_100G_KR4_GET(reg73_ability) ?SOC_PA_SPEED_100GB:0;
    speed_full_duplex|= PHYMOD_AN_CAP_40G_CR4_GET(reg73_ability) ?SOC_PA_SPEED_40GB:0;
    speed_full_duplex|= PHYMOD_AN_CAP_40G_KR4_GET(reg73_ability) ?SOC_PA_SPEED_40GB:0;
    speed_full_duplex|= PHYMOD_AN_CAP_10G_KR_GET(reg73_ability)  ?SOC_PA_SPEED_10GB:0;
    speed_full_duplex|= PHYMOD_AN_CAP_10G_KX4_GET(reg73_ability) ?SOC_PA_SPEED_10GB:0;
    speed_full_duplex|= PHYMOD_AN_CAP_1G_KX_GET(reg73_ability)   ?SOC_PA_SPEED_1000MB:0;
   speed_full_duplex|= PHYMOD_AN_CAP_25G_CR1_GET(reg73_ability) ?SOC_PA_SPEED_25GB:0;
   speed_full_duplex|= PHYMOD_AN_CAP_25G_KR1_GET(reg73_ability) ?SOC_PA_SPEED_25GB:0;
   
    /* retrieve CL73bam abilities */
    reg73bam_ability = phymod_autoneg_ability.cl73bam_cap;
    speed_full_duplex|= PHYMOD_BAM_CL73_CAP_20G_CR2_GET(reg73bam_ability) ?SOC_PA_SPEED_20GB:0;
    speed_full_duplex|= PHYMOD_BAM_CL73_CAP_20G_KR2_GET(reg73bam_ability) ?SOC_PA_SPEED_20GB:0;
    speed_full_duplex|= PHYMOD_BAM_CL73_CAP_40G_CR2_GET(reg73bam_ability) ?SOC_PA_SPEED_40GB:0;
    speed_full_duplex|= PHYMOD_BAM_CL73_CAP_40G_KR2_GET(reg73bam_ability) ?SOC_PA_SPEED_40GB:0;
    speed_full_duplex|= PHYMOD_BAM_CL73_CAP_50G_CR2_GET(reg73bam_ability) ?SOC_PA_SPEED_50GB:0;
    speed_full_duplex|= PHYMOD_BAM_CL73_CAP_50G_KR2_GET(reg73bam_ability) ?SOC_PA_SPEED_50GB:0;
   speed_full_duplex|= PHYMOD_BAM_CL73_CAP_25G_CR1_GET(reg73bam_ability) ?SOC_PA_SPEED_25GB:0;
   speed_full_duplex|= PHYMOD_BAM_CL73_CAP_25G_KR1_GET(reg73bam_ability) ?SOC_PA_SPEED_25GB:0;
   speed_full_duplex|= PHYMOD_BAM_CL73_CAP_20G_CR1_GET(reg73bam_ability) ?SOC_PA_SPEED_20GB:0;
   speed_full_duplex|= PHYMOD_BAM_CL73_CAP_20G_KR1_GET(reg73bam_ability) ?SOC_PA_SPEED_20GB:0;
   
    /* retrieve CL37 abilities */
    reg37_ability = phymod_autoneg_ability.cl37bam_cap;
    speed_full_duplex|= PHYMOD_BAM_CL37_CAP_40G_GET(reg37_ability) ? SOC_PA_SPEED_40GB:0;
    speed_full_duplex|= PHYMOD_BAM_CL37_CAP_25P455G_GET(reg37_ability) ?    SOC_PA_SPEED_25GB:0;
    speed_full_duplex|= PHYMOD_BAM_CL37_CAP_20G_X2_CX4_GET(reg37_ability)? SOC_PA_SPEED_20GB:0;
    speed_full_duplex|= PHYMOD_BAM_CL37_CAP_20G_X2_GET(reg37_ability)?     SOC_PA_SPEED_20GB:0;
    speed_full_duplex|= PHYMOD_BAM_CL37_CAP_20G_X4_GET(reg37_ability)?     SOC_PA_SPEED_20GB:0;
    speed_full_duplex|= PHYMOD_BAM_CL37_CAP_20G_X4_CX4_GET(reg37_ability)? SOC_PA_SPEED_20GB:0;
    speed_full_duplex|= PHYMOD_BAM_CL37_CAP_10G_X2_CX4_GET(reg37_ability)? SOC_PA_SPEED_10GB:0;
    speed_full_duplex|= PHYMOD_BAM_CL37_CAP_10G_DXGXS_GET(reg37_ability)?  SOC_PA_SPEED_10GB:0;
    speed_full_duplex|= PHYMOD_BAM_CL37_CAP_10G_CX4_GET(reg37_ability)?    SOC_PA_SPEED_10GB:0;

    if (PHYMOD_AN_CAP_25G_KRS1_GET(reg73_ability) || PHYMOD_AN_CAP_25G_CRS1_GET(reg73_ability)) {
        ability->channel = SOC_PA_CHANNEL_SHORT;
    }

    if (PHYMOD_AN_CAP_25G_KR1_GET(reg73_ability) || PHYMOD_AN_CAP_25G_CR1_GET(reg73_ability)) {
        ability->channel |= SOC_PA_CHANNEL_LONG;
    }

    if (PHYMOD_AN_CAP_100G_CR4_GET(reg73_ability) || PHYMOD_AN_CAP_40G_CR4_GET(reg73_ability)  ||
        PHYMOD_AN_CAP_25G_CR1_GET(reg73_ability)  || PHYMOD_AN_CAP_25G_CRS1_GET(reg73_ability) ||
        PHYMOD_BAM_CL73_CAP_20G_CR2_GET(reg73bam_ability)  || PHYMOD_BAM_CL73_CAP_40G_CR2_GET(reg73bam_ability) ||
        PHYMOD_BAM_CL73_CAP_50G_CR2_GET(reg73bam_ability)  || PHYMOD_BAM_CL73_CAP_25G_CR1_GET(reg73bam_ability) ||
        PHYMOD_BAM_CL73_CAP_50G_CR4_GET(reg73bam_ability)) {
        ability->medium = SOC_PA_MEDIUM_COPPER;
    } else {
        ability->medium = SOC_PA_MEDIUM_BACKPLANE;
    }
	
    /* retrieve "pause" abilities */
    reg_ability = phymod_autoneg_ability.capabilities;

    ability->pause = 0;
    if (reg_ability == PHYMOD_AN_CAP_ASYM_PAUSE) {
        ability->pause = SOC_PA_PAUSE_TX;
    } else if (reg_ability == (PHYMOD_AN_CAP_SYMM_PAUSE|PHYMOD_AN_CAP_ASYM_PAUSE)) {
        ability->pause = SOC_PA_PAUSE_RX;
    } else if (reg_ability == PHYMOD_AN_CAP_SYMM_PAUSE) {
        ability->pause = (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX);
    }

    ability->speed_full_duplex = speed_full_duplex;
    ability->encap = SOC_PA_ENCAP_IEEE | SOC_PA_ENCAP_HIGIG | SOC_PA_ENCAP_HIGIG2;

    ability->fec = 0;
    if (PHYMOD_AN_FEC_OFF_GET(phymod_autoneg_ability.an_fec)) {
        ability->fec = SOC_PA_FEC_NONE;
    } else {
        if (PHYMOD_AN_FEC_CL74_GET(phymod_autoneg_ability.an_fec)) {
            ability->fec |= SOC_PA_FEC_CL74;
        }
        if (PHYMOD_AN_FEC_CL91_GET(phymod_autoneg_ability.an_fec)) {
            ability->fec |= SOC_PA_FEC_CL91;
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8806x_lb_set
 * Purpose:
 *      Enable/disable PHY loopback mode
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      enable - binary value for on/off (1/0)
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_8806x_lb_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t* pc; 
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t *pm_phy;
    int idx=0;
    phymod_loopback_mode_t lb_mode;

    pc = EXT_PHY_SW_STATE(unit, port);

    pmc = &pc->phymod_ctrl;

    pm_phy = &pmc->phy[idx]->pm_phy;

    if (pc->flags & PHYCTRL_SYS_SIDE_CTRL) {
        lb_mode = phymodLoopbackSysGlobal;
        SOC_IF_ERROR_RETURN
            (phymod_phy_loopback_set(pm_phy, lb_mode, enable));
    } else {
        lb_mode = phymodLoopbackGlobal;
#if defined(INCLUDE_FCMAP)
        if (pc->fcmap_enable) {
            lb_mode = phymodLoopbackGlobalPMD;
        }
#endif
        SOC_IF_ERROR_RETURN
            (phymod_phy_loopback_set(pm_phy, lb_mode, enable));
    }

    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_8806x_lb_get
 * Purpose:
 *      Get current PHY loopback mode
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      enable - address of location to store binary value for on/off (1/0)
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_8806x_lb_get(int unit, soc_port_t port, int *enable)
{
    phy_ctrl_t* pc; 
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t *pm_phy;
    uint32 lb_enable = 0;
    int idx=0;

    pc = EXT_PHY_SW_STATE(unit, port);
#if defined(INCLUDE_FCMAP)
    /* Skip for FC */
    if (pc->fcmap_enable) {
        return SOC_E_NONE;
    }
#endif

    pmc = &pc->phymod_ctrl;

    *enable = 0;

    pm_phy = &pmc->phy[idx]->pm_phy;

    if (pc->flags & PHYCTRL_SYS_SIDE_CTRL) {
        SOC_IF_ERROR_RETURN
            (phymod_phy_loopback_get(pm_phy, phymodLoopbackSysGlobal, &lb_enable));
    } else {
        SOC_IF_ERROR_RETURN
            (phymod_phy_loopback_get(pm_phy, phymodLoopbackGlobal, &lb_enable));
    }

    if (lb_enable) {
        *enable = 1;
    }
     
    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_8806x_ability_local_get
 * Purpose:
 *      xx
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      ability - xx
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_8806x_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    phy_ctrl_t          *pc; 
    /* phy8806x_config_t   *pCfg; */

    pc = EXT_PHY_SW_STATE(unit, port);
    /* pCfg = (phy8806x_config_t *) pc->driver_data; */

    if (NULL == ability) {
        return SOC_E_PARAM;
    }

    sal_memset(ability, 0, sizeof(*ability));

    ability->loopback  = SOC_PA_LB_PHY;
    ability->medium    = SOC_PA_MEDIUM_FIBER | SOC_PA_MEDIUM_COPPER | SOC_PA_MEDIUM_BACKPLANE;
    ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
    ability->fec       = SOC_PA_FEC_NONE |SOC_PA_FEC_CL74 | SOC_PA_FEC_CL91;
    ability->flags     = SOC_PA_AUTONEG;
    ability->channel   = SOC_PA_CHANNEL_SHORT | SOC_PA_CHANNEL_LONG; 

    if (IND_LANE_MODE(pc)) { 
        ability->speed_full_duplex  = SOC_PA_SPEED_1000MB;
        switch(pc->speed_max) {  /* must include all the supported speedss */
            case 50000:
                ability->speed_full_duplex |= SOC_PA_SPEED_50GB;
                /* fall through */
            case 40000:
                ability->speed_full_duplex |= SOC_PA_SPEED_40GB;
                /* fall through */                
            case 25000:
                ability->speed_full_duplex |= SOC_PA_SPEED_25GB;
                /* fall through */
            case 20000:
                ability->speed_full_duplex |= SOC_PA_SPEED_20GB;
                /* fall through */
            case 10000:
                ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
                /* fall through */
            default:
                break;
        }
        ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
        ability->interface = SOC_PA_INTF_GMII | SOC_PA_INTF_SGMII;
    } else {
        ability->speed_half_duplex  = SOC_PA_ABILITY_NONE;
        ability->speed_full_duplex  = SOC_PA_SPEED_1000MB;
        switch(pc->speed_max) {
            case 106000:
                ability->speed_full_duplex |= SOC_PA_SPEED_106GB;
            /* Fall through */
            case 100000:
                ability->speed_full_duplex |= SOC_PA_SPEED_100GB;
            /* Fall through */
            case 40000:
                ability->speed_full_duplex |= SOC_PA_SPEED_40GB;
            /* Fall through */
            default:
                ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
                break; 
        }
        ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
        ability->interface = SOC_PA_INTF_XGMII;
        ability->loopback  = SOC_PA_LB_PHY;
        ability->flags     = SOC_PA_AUTONEG;
    }

    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_8806x_firmware_set
 * Purpose:
 *      xx
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 

 * Returns:     
 *      SOC_E_NONE
 */


STATIC int
phy_8806x_firmware_set(int unit, int port, int offset, uint8 *data,int len)
{
    phy_ctrl_t *pc;
    int dividend, divisor, rv = SOC_E_FAIL;
    uint32 temp32;
    uint16 temp16;
    uint16 status_l = 0, status_s = 0;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (data) {
        return SOC_E_FAIL;
    }

    switch (offset) {
        case PHYCTRL_UCODE_BCST_SETUP:
            /* Turn on broadcast mode */

        if ( FLAGS(pc) & PHY8806X_BMP0 ) {
            /* a.)  bring all mHosts out of reset */
            SOC_IF_ERROR_RETURN
                (READ_TOP_SOFT_RESET_REG(unit, pc, &temp32));

                temp32 &= ~0x40000015;
                /* temp32 |= (0xf << 6); */ /* RTS0 */
                temp32 |= ((0xf << 6) | (0xff << 14)); /* RTS0 + SRAM0 */

            SOC_IF_ERROR_RETURN
                (WRITE_TOP_SOFT_RESET_REG(unit, pc, temp32));

                /* temp32 &= ~(0xf << 6); */
                temp32 &= ~((0xf << 6) | (0xff << 14));

                temp32 |= 0x40000015;
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
                if (!(FLAGS(pc) & (PHY8806X_MODE_CHANGE | PHY8806X_FW_RESTART))) {
                    temp32 |= (0x3 << ((pc->phy_id & 0x3) << 1));
                }
            SOC_IF_ERROR_RETURN
                (WRITE_TOP_RTS_MISC_CTRL_REG(unit, pc, temp32));

            /* f.)  bring all mHosts out of reset */
            SOC_IF_ERROR_RETURN
                (READ_TOP_SOFT_RESET_REG(unit, pc, &temp32));
                temp32 &= ~(0xf << 6);
            SOC_IF_ERROR_RETURN
                (WRITE_TOP_SOFT_RESET_REG(unit, pc, temp32));


            if (!(FLAGS(pc) & (PHY8806X_MODE_CHANGE | PHY8806X_FW_RESTART))) {
                /* g.) */
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY8806X_AXI_CONTROL_F0_REG(unit, pc, (1U << 9), (1U << 9)));

                SOC_IF_ERROR_RETURN
                    (WRITE_PHY8806X_AXI_ADDR_UPPER_F0_REG(unit, pc, 0x0026 + (pc->phy_id & 0x3) * 0x10));
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY8806X_AXI_ADDR_LOWER_F0_REG(unit, pc, 0x0000));
            }

#if defined(INCLUDE_FCMAP)
            /* SDK-85976 : SW WAR for MONTREAL2-859 , only for A0*/
            if (pc->fcmap_enable){
                if (PHY_IS_BCM8806X_A0(pc)) {
                    SOC_IF_ERROR_RETURN
                        (WRITE_LINE_FCPORT_EXT_TX_DISABLE_CTRL(unit, pc, 0));
                }
            } else {
                SOC_IF_ERROR_RETURN
                    (WRITE_LINE_FCPORT_EXT_TX_DISABLE_CTRL(unit, pc, 0xf));
            }
            if (pc->fcmap_enable) {
                SOC_IF_ERROR_RETURN
                    (READ_TOP_SOFT_RESET_REG_2(unit, pc, &temp32));
                temp32 |= 0x115;
                SOC_IF_ERROR_RETURN
                    (WRITE_TOP_SOFT_RESET_REG_2(unit, pc, temp32));
            } else {
                SOC_IF_ERROR_RETURN
                    (WRITE_TOP_SOFT_RESET_REG_2(unit, pc, 0x100));
            }
#else
            SOC_IF_ERROR_RETURN
                (WRITE_LINE_FCPORT_EXT_TX_DISABLE_CTRL(unit, pc, 0xf));
            SOC_IF_ERROR_RETURN
                (WRITE_TOP_SOFT_RESET_REG_2(unit, pc, 0x100));
#endif

            SOC_IF_ERROR_RETURN
                (_pmd_enable(unit, pc, 0)); /* line */
            SOC_IF_ERROR_RETURN
                (_pmd_enable(unit, pc, 1)); /* system */

        } else if ( FLAGS(pc) & PHY8806X_BMP1 ) {
            /* a.) bring all mHosts out of reset */
            SOC_IF_ERROR_RETURN
                (READ_TOP_SOFT_RESET_REG(unit, pc, &temp32));

                temp32 &= ~0x8000002A;
                /* temp32 |= (0xf << 10); */ /* RTS1 */
                temp32 |= ((0xf << 10) | (0xff << 22)); /* RTS1 + SRAM1 */

            SOC_IF_ERROR_RETURN
                (WRITE_TOP_SOFT_RESET_REG(unit, pc, temp32));

                /* temp32 &= ~(0xf << 10); */
                temp32 &= ~((0xf << 10) | (0xff << 22));
                temp32 |= 0x8000002A;

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
                if (!(FLAGS(pc) & (PHY8806X_MODE_CHANGE | PHY8806X_FW_RESTART))) {
                    temp32 |= (0x3 << (((pc->phy_id & 0x3) << 1) + 8));
                }
            SOC_IF_ERROR_RETURN
                (WRITE_TOP_RTS_MISC_CTRL_REG(unit, pc, temp32));

            /* f.) bring all mHosts out of reset */
            SOC_IF_ERROR_RETURN
                (READ_TOP_SOFT_RESET_REG(unit, pc, &temp32));
                temp32 &= ~(0xf << 10);
            SOC_IF_ERROR_RETURN
                (WRITE_TOP_SOFT_RESET_REG(unit, pc, temp32));

            if (!(FLAGS(pc) & (PHY8806X_MODE_CHANGE | PHY8806X_FW_RESTART))) {
                /* g.) */
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY8806X_AXI_CONTROL_F1_REG(unit, pc, (1U << 9), (1U << 9)));

                SOC_IF_ERROR_RETURN
                    (WRITE_PHY8806X_AXI_ADDR_UPPER_F1_REG(unit, pc, 0x0026 + (pc->phy_id & 0x3) * 0x10));
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY8806X_AXI_ADDR_LOWER_F1_REG(unit, pc, 0x0000));
            }
#if defined(INCLUDE_FCMAP)
            /* SDK-85976 : SW WAR for MONTREAL2-859 , only for A0*/
            if (pc->fcmap_enable){
                if (PHY_IS_BCM8806X_A0(pc)) {
                    SOC_IF_ERROR_RETURN
                        (WRITE_LINE_FCPORT_EXT_TX_DISABLE_CTRL(unit, pc, 0));
                }
            } else {
                SOC_IF_ERROR_RETURN
                    (WRITE_LINE_FCPORT_EXT_TX_DISABLE_CTRL(unit, pc, 0xf));
            }
            if (pc->fcmap_enable) {
                SOC_IF_ERROR_RETURN
                    (READ_TOP_SOFT_RESET_REG_2(unit, pc, &temp32));
                temp32 |= 0x115;
                SOC_IF_ERROR_RETURN
                    (WRITE_TOP_SOFT_RESET_REG_2(unit, pc, temp32));
            } else {
                SOC_IF_ERROR_RETURN
                    (WRITE_TOP_SOFT_RESET_REG_2(unit, pc, 0x100));
            }
#else
            SOC_IF_ERROR_RETURN
                (WRITE_LINE_FCPORT_EXT_TX_DISABLE_CTRL(unit, pc, 0xf));
            SOC_IF_ERROR_RETURN
                (WRITE_TOP_SOFT_RESET_REG_2(unit, pc, 0x100));
#endif

            SOC_IF_ERROR_RETURN
                (_pmd_enable(unit, pc, 0)); /* line */
            SOC_IF_ERROR_RETURN
                (_pmd_enable(unit, pc, 1)); /* system */

        }
            break;

        case PHYCTRL_UCODE_BCST_uC_SETUP:
            break;
        case PHYCTRL_UCODE_BCST_ENABLE:
            if ( FLAGS(pc) & (PHY8806X_MODE_CHANGE | PHY8806X_FW_RESTART) ) {
                break;
            }
            /* h.) */
            if ( FLAGS(pc) & PHY8806X_FWDLM ) {
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY8806X_MISC_CONTROL_REG(unit, pc, 0, (1U << 14)));
            }

            if ( FLAGS(pc) & PHY8806X_CHM) {
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY8806X_MISC_CONTROL_REG(unit, pc, (1U << 15), (1U << 15)));
                if ( FLAGS(pc) & PHY8806X_AC ) {
                    if (DL_DIVIDEND(pc) && DL_DIVISOR(pc)) {
                         rv = soc_cmic_rate_param_get(unit, &dividend, &divisor);
                        if (SOC_SUCCESS(rv)) {
                            rv = soc_cmic_rate_param_set(unit, DL_DIVIDEND(pc), DL_DIVISOR(pc));
                        }
                    }
                    SOC_IF_ERROR_RETURN
                        (_phy_8806x_write_to_arm(unit, pc, 0, FIRMWARE(pc), FIRMWARE_LEN(pc)));
                    if (SOC_SUCCESS(rv)) {
                        /* rv contains the return value of previous soc_cmic_rate_param_set() */
                        SOC_IF_ERROR_RETURN
                            (soc_cmic_rate_param_set(unit, dividend, divisor));
                    }
                }
            }

            break;

        case PHYCTRL_UCODE_BCST_LOAD:
            if ( FLAGS(pc) & (PHY8806X_MODE_CHANGE | PHY8806X_FW_RESTART) ) {
                break;
            }
            if (DL_DIVIDEND(pc) && DL_DIVISOR(pc)) {
                 rv = soc_cmic_rate_param_get(unit, &dividend, &divisor);
                if (SOC_SUCCESS(rv)) {
                    rv = soc_cmic_rate_param_set(unit, DL_DIVIDEND(pc), DL_DIVISOR(pc));
                }
            }

            if ( FLAGS(pc) & PHY8806X_FWDLM ) {
               SOC_IF_ERROR_RETURN
                   (_phy_8806x_write_to_arm(unit, pc, 0, FIRMWARE(pc), FIRMWARE_LEN(pc)));
            }
            if (SOC_SUCCESS(rv)) {
                /* rv contains the return value of previous soc_cmic_rate_param_set() */
                SOC_IF_ERROR_RETURN
                    (soc_cmic_rate_param_set(unit, dividend, divisor));
            }
            if ( FLAGS(pc) & (PHY8806X_BMP0 | PHY8806X_BMP1) ) {
                soc_timeout_init(&TO(pc), 35000000, 0);
            }
            break;

        case PHYCTRL_UCODE_BCST_END:
            if (!(FLAGS(pc) & (PHY8806X_MODE_CHANGE | PHY8806X_FW_RESTART))) {
                /* Turn off broadcast mode by reading the following reg. */
                if ( FLAGS(pc) & PHY8806X_CHM ) {
                    SOC_IF_ERROR_RETURN
                        (READ_PHY8806X_MISC_CONTROL_REG(unit, pc, &temp16));
                }

                if ( FLAGS(pc) & PHY8806X_FWDLM ) {
                    SOC_IF_ERROR_RETURN
                        (MODIFY_PHY8806X_MISC_CONTROL_REG(unit, pc, (1U << 14), (1U << 14)));
                }
            }

            if ( FLAGS(pc) & (PHY8806X_BMP0 | PHY8806X_BMP1) ) {
                SOC_IF_ERROR_RETURN
                   (_pmd_disable(unit, pc, 0)); /* line */
                SOC_IF_ERROR_RETURN
                   (_pmd_disable(unit, pc, 1)); /* system */

                if (!(FLAGS(pc) & (PHY8806X_MODE_CHANGE | PHY8806X_FW_RESTART))) {
                    temp16 = 0;
                    do {
                        rv = READ_PHY8806X_FW_TO_SW_MESSAGE_REG(unit, pc, &temp16);
                        if (temp16 | SOC_FAILURE(rv)) {
                            break;
                        }
                    } while (!soc_timeout_check(&TO(pc)));

                        LOG_CLI((BSL_META_U(unit,
                                "PHY8806X_FW_TO_SW_MESSAGE_REG S : u=%d p=%d  status=%04x"
                                " !.\n"), unit, pc->port, temp16));
                    if ((temp16 & 0x5000) != 0x5000) {
                        LOG_CLI((BSL_META_U(unit,
                                "PHY8806X_FW_TO_SW_MESSAGE_REG failed: u=%d p=%d  status=%04x"
                                " !.\n"), unit, pc->port, temp16));
                        /* return SOC_E_FAIL; */
                    }
                }

                temp32 = HOST2LE32(TOP_DEV_REV_ID(pc));

                /*  Place the value in SRAM */
                SOC_IF_ERROR_RETURN
                (_phy_8806x_axi_write(unit, pc, 0x00100000 + 0x100 - 0x4, &temp32, 1));

                /* Start PMDs */
                SOC_IF_ERROR_RETURN
                    (_pmd_start(unit, pc, 0)); /* line */
                SOC_IF_ERROR_RETURN
                    (_pmd_start(unit, pc, 1)); /* system */

                /* Read the stored length and the CRC from SRAM */ 
                _phy_8806x_axi_read(unit, pc, 0x00100000, &temp32, 1);
                PMD_SIZE(pc) = LE2HOST32(temp32);
                _phy_8806x_axi_read(unit, pc, 0x00100004, &temp32, 1);
                PMD_CRC(pc) = LE2HOST32(temp32);

                soc_timeout_init(&TO(pc), 5000000, 0); /* 5s timer */

                do {

                    rv = _tsc_reg_read(unit, pc, 0, (pc->phy_id & 0x7) + 1, 1, 0, 0xd03d, &status_l);
                    if (SOC_FAILURE(rv)) {
                        break;
                    }
                    rv = _tsc_reg_read(unit, pc, 0, (pc->phy_id & 0x7) + 9, 1, 0, 0xd03d, &status_s);
                    if (SOC_FAILURE(rv)) {
                        break;
                    }
                    if ((status_l== 0x80) &&  (status_s == 0x80)) {
                        break;
                    }
                } while (!soc_timeout_check(&TO(pc)));

               if ( status_l != 0x80 ) {
                    LOG_CLI((BSL_META_U(unit,
                            "PHY8806X PMD (line) firmware not running: u=%d p=%d  status=%04x"
                            " !.\n"), unit, pc->port, status_l));
               } else {
                    /* Issue command to compute CRC */
                    _tsc_reg_write(unit, pc, 0, (pc->phy_id & 0x7) + 1, 1, 0, 0xd03e, PMD_SIZE(pc), 0);
                    _tsc_reg_write(unit, pc, 0, (pc->phy_id & 0x7) + 1, 1, 0, 0xd03d, 0x14, 0);
               }

               if ( status_s != 0x80 ) {
                    LOG_CLI((BSL_META_U(unit,
                            "PHY8806X PMD (sys)  firmware not running: u=%d p=%d  status=%04x"
                            " !.\n"), unit, pc->port, status_s));
               } else {
                    /* Issue command to compute CRC */
                    _tsc_reg_write(unit, pc, 0, (pc->phy_id & 0x7) + 9, 1, 0, 0xd03e, PMD_SIZE(pc), 0);
                    _tsc_reg_write(unit, pc, 0, (pc->phy_id & 0x7) + 9, 1, 0, 0xd03d, 0x14, 0);
               }

            }

            soc_timeout_init(&TO(pc), 5000000, 0); /* 5s timer */

            if ( FLAGS(pc) & PHY8806X_BMP0 ) {
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
                    if (!( FLAGS(pc) & PHY8806X_PM )) {
                        temp32 |= (0x2 << ((pc->phy_id & 0x3) << 1));
                    }
                SOC_IF_ERROR_RETURN
                    (WRITE_TOP_RTS_MISC_CTRL_REG(unit, pc, temp32));

                /* f.) Release all mHosts from reset, Only BMP will run. Others halted.
                       In PM all will be halted. */
                SOC_IF_ERROR_RETURN
                    (READ_TOP_SOFT_RESET_REG(unit, pc, &temp32));
                    temp32 &= ~(0xf << 6);
                SOC_IF_ERROR_RETURN
                    (WRITE_TOP_SOFT_RESET_REG(unit, pc, temp32));

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
                    if (!( FLAGS(pc) & PHY8806X_PM )) {
                        temp32 |= (0x2 << (((pc->phy_id & 0x3) << 1) + 8));
                    }
                SOC_IF_ERROR_RETURN
                    (WRITE_TOP_RTS_MISC_CTRL_REG(unit, pc, temp32));

                /* f.) Release all mHosts from reset, Only BMP will run. Others halted.
                       In PM all will be halted. */
                SOC_IF_ERROR_RETURN
                    (READ_TOP_SOFT_RESET_REG(unit, pc, &temp32));
                    temp32 &= ~(0xf << 10);
                SOC_IF_ERROR_RETURN
                    (WRITE_TOP_SOFT_RESET_REG(unit, pc, temp32));

            }

            break;

        default:
            break;
    }



    return SOC_E_NONE;
}

STATIC int
_phy_8806x_validate_interface_type(phy8806x_speed_config_t  *speed_config, soc_port_if_t *pif)
{
    int port_num_lanes;

    port_num_lanes = speed_config->ln_lane_count;

    switch (speed_config->speed) {
    case 1000:
        if (port_num_lanes == 1) {
            switch (*pif) {
            case SOC_PORT_IF_SGMII:
            case SOC_PORT_IF_GMII:
                break;
            default:
                *pif = SOC_PORT_IF_SGMII;
            }
        } else {
            return SOC_E_PARAM;
        }
        break;
    case 10000:
    case 25000:
    case 20000:
        if (port_num_lanes == 1) {
            switch (*pif) {
                case SOC_PORT_IF_CR:
                case SOC_PORT_IF_SR:
                case SOC_PORT_IF_KR:
                case SOC_PORT_IF_XFI:
                case SOC_PORT_IF_XAUI:
                    break;
                case SOC_PORT_IF_LR:
                    *pif = SOC_PORT_IF_SR;
                    break;
                default:
                    *pif = SOC_PORT_IF_CR;
                    break;
            }
        } else if(port_num_lanes == 2) {
            switch (*pif) {
                case SOC_PORT_IF_CR2:
                case SOC_PORT_IF_SR2:
                case SOC_PORT_IF_KR2:
                    break;
                case SOC_PORT_IF_LR2:
                    *pif = SOC_PORT_IF_SR2;
                    break;
                default:
                    *pif = SOC_PORT_IF_CR2;
                    break;
            }
        } else {
            return SOC_E_PARAM;
        } 
        break ;
    case 40000:
    case 42000:
    case 50000:
        if (port_num_lanes == 2) {
            switch (*pif) {
                case SOC_PORT_IF_CR2:
                case SOC_PORT_IF_SR2:
                case SOC_PORT_IF_KR2:
                    break;
                case SOC_PORT_IF_LR2:
                    *pif = SOC_PORT_IF_SR2;
                    break;
                default:
                    *pif = SOC_PORT_IF_CR2;
                    break;
            }
        } else if (port_num_lanes == 4) {
            switch (*pif) {
                case SOC_PORT_IF_CR4:
                case SOC_PORT_IF_SR4:
                case SOC_PORT_IF_KR4:
                case SOC_PORT_IF_XLAUI:
                    break;
                case SOC_PORT_IF_LR4:
                    *pif = SOC_PORT_IF_SR4;
                    break;
                default:
                    *pif = SOC_PORT_IF_CR4;
                    break;
            }
        } else {
            return SOC_E_PARAM;
        }
        break;
    case 100000:
    case 106000:
        if (port_num_lanes == 4) {
            switch (*pif) {
                case SOC_PORT_IF_CR4:
                case SOC_PORT_IF_SR4:
                case SOC_PORT_IF_KR4:
                case SOC_PORT_IF_CAUI:
                    break;
                case SOC_PORT_IF_LR4:
                    *pif = SOC_PORT_IF_SR4;
                    break;
                default:
                    *pif = SOC_PORT_IF_CR4;
                    break; 
            } 
        } else {
            return SOC_E_PARAM;
        }
        break;
    default:
        return SOC_E_PARAM;
    }

    return 0;
}


STATIC int
phy_8806x_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
    phy_ctrl_t                *pc;
    soc_phymod_ctrl_t         *pmc;
    soc_phymod_phy_t          *phy;
    phymod_phy_inf_config_t   interface_config;
    int flag = 0;
    phymod_ref_clk_t            ref_clock;
    phy8806x_speed_config_t     *speed_config;
    phy8806x_config_t           *pCfg;
    phymod_phy_access_t       pm_phy_copy;

    /* locate phy control */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pmc = &pc->phymod_ctrl;

    /* initialize the data structure */
    sal_memset(&interface_config, 0x0, sizeof(phymod_phy_inf_config_t));

    /* now loop through all cores */
    phy = pmc->phy[0];
    if (phy == NULL) {
        return SOC_E_INTERNAL;
    }

    ref_clock = phymodRefClk156Mhz;

    sal_memcpy(&pm_phy_copy, &phy->pm_phy, sizeof(pm_phy_copy));

    pm_phy_copy.port_loc = (pc->flags & PHYCTRL_SYS_SIDE_CTRL) \
        ? phymodPortLocSys :  ((pc->flags & PHYCTRL_LINE_SIDE_CTRL) \
        ?  phymodPortLocLine : phymodPortLocDC);

    SOC_IF_ERROR_RETURN
        (phymod_phy_interface_config_get(&pm_phy_copy,
                                         flag, ref_clock, &interface_config));

    pCfg = (phy8806x_config_t *) pc->driver_data;
    speed_config = &(pCfg->speed_config);

    if ((pif == SOC_PORT_IF_SFI) ||
        (pif == SOC_PORT_IF_SR4) ||
        (pif == SOC_PORT_IF_LR4) ||
        (pif == SOC_PORT_IF_SR) ||
        (pif == SOC_PORT_IF_SR2) ||
        (pif == SOC_PORT_IF_LR) ||
        (pif == SOC_PORT_IF_LR2) ||
        (pif == SOC_PORT_IF_GMII)) {
#if defined(INCLUDE_FCMAP)
        if (pc->fcmap_enable)
            pCfg->speed_config.fiber_pref_sys = 1;
#endif
        pCfg->speed_config.fiber_pref = 1;  
    } else {
#if defined(INCLUDE_FCMAP)
        if (pc->fcmap_enable)
            pCfg->speed_config.fiber_pref_sys = 0;
#endif
        pCfg->speed_config.fiber_pref = 0;
    }

    SOC_IF_ERROR_RETURN(_phy_8806x_validate_interface_type(speed_config, &pif));

    /* coverity[mixed_enums]: ignore */
    interface_config.interface_type = pif;
    
    if (!interface_config.data_rate)
        interface_config.data_rate = pCfg->speed_config.speed;

    /* note that the flags have an option to indicate whether it's ok to reprogram the PLL */
    SOC_IF_ERROR_RETURN
        (phymod_phy_interface_config_set(&pm_phy_copy,
                                         flag,  &interface_config));

    return (SOC_E_NONE);
}

STATIC int
phy_8806x_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    phy_ctrl_t                *pc;
    soc_phymod_ctrl_t         *pmc;
    soc_phymod_phy_t          *phy;
    phymod_phy_inf_config_t   interface_config;
    int flag = 0;
    phymod_ref_clk_t          ref_clock;
    phymod_phy_access_t       pm_phy_copy;

    /* locate phy control */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pmc = &pc->phymod_ctrl;

    /* initialize the data structure */
    sal_memset(&interface_config, 0x0, sizeof(phymod_phy_inf_config_t));

    /* now loop through all cores */
    phy = pmc->phy[0];
    if (phy == NULL) {
        return SOC_E_INTERNAL;
    }

    ref_clock = phymodRefClk156Mhz;

    sal_memcpy(&pm_phy_copy, &phy->pm_phy, sizeof(pm_phy_copy));

    pm_phy_copy.port_loc = (pc->flags & PHYCTRL_SYS_SIDE_CTRL) \
        ? phymodPortLocSys :  ((pc->flags & PHYCTRL_LINE_SIDE_CTRL) \
        ?  phymodPortLocLine : phymodPortLocDC);

    /* note that the flags have an option to indicate whether it's ok to reprogram the PLL */
    SOC_IF_ERROR_RETURN
        (phymod_phy_interface_config_get(&pm_phy_copy,
                                         flag, ref_clock, &interface_config));

    /* coverity[mixed_enums]: ignore */
    *pif = interface_config.interface_type;

    return (SOC_E_NONE);

}


/*
 * Function:
 *      phy8806x_uc_status_dump
 * Purpose:
 *      display all the serdes related parameters
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 

 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy8806x_uc_status_dump(int unit, soc_port_t port, void *arg)
{
    phy_ctrl_t* pc; 
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t *pm_phy;
    int idx;

    /* locate phy control, phymod control, and the configuration data */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    pmc = &pc->phymod_ctrl;

    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;
        SOC_IF_ERROR_RETURN
            (phymod_phy_pmd_info_dump(pm_phy, arg)); 
    }
    return (SOC_E_NONE);
}

/* 
 * phy8806x_tx_lane_squelch
 */
STATIC int
phy8806x_tx_lane_squelch(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t *pm_phy;
    phymod_phy_tx_lane_control_t  tx_control;
    int                 idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        if (value == 1) {
            tx_control = phymodTxSquelchOn;
        } else {
            tx_control = phymodTxSquelchOff;
        }
        SOC_IF_ERROR_RETURN
            (phymod_phy_tx_lane_control_set(pm_phy, tx_control));
    }
    return(SOC_E_NONE);
}

/* 
 * phy8806x_tx_polarity_set
 */
STATIC int 
phy8806x_tx_polarity_set(soc_phymod_ctrl_t *pmc, phymod_polarity_t *cfg_polarity, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_polarity_t    polarity;
    int                  idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        sal_memcpy(&polarity, cfg_polarity, sizeof(polarity));
        polarity.tx_polarity = value;
        SOC_IF_ERROR_RETURN(phymod_phy_polarity_set(pm_phy, &polarity));

        /* after successfully setting the parity, update the configured value */
        cfg_polarity->tx_polarity = value;
    }

    return(SOC_E_NONE);
}

/* 
 * phy8806x_rx_polarity_set
 */

STATIC int 
phy8806x_rx_polarity_set(soc_phymod_ctrl_t *pmc, phymod_polarity_t *cfg_polarity, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_polarity_t    polarity;
    int                  idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        sal_memcpy(&polarity, cfg_polarity, sizeof(polarity));
        polarity.rx_polarity = value;
        SOC_IF_ERROR_RETURN(phymod_phy_polarity_set(pm_phy, &polarity));

        /* after successfully setting the parity, update the configured value */
        cfg_polarity->rx_polarity = value;
    }

    return(SOC_E_NONE);
}

/* 
 * phy8806x_rx_reset
 */
STATIC int
phy8806x_rx_reset(soc_phymod_ctrl_t *pmc, phymod_phy_reset_t *cfg_reset, uint32 value)
{
    phymod_phy_access_t *pm_phy;
    phymod_phy_reset_t  reset;
    int                 idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        sal_memcpy(&reset, cfg_reset, sizeof(reset));
        reset.rx = (phymod_reset_direction_t) value;
        SOC_IF_ERROR_RETURN(phymod_phy_reset_set(pm_phy, &reset));

        /* after successfully setting the parity, update the configured value */
        cfg_reset->rx = (phymod_reset_direction_t) value;
    }

    return(SOC_E_NONE);
}

/* 
 * phy8806x_tx_reset
 */
STATIC int
phy8806x_tx_reset(soc_phymod_ctrl_t *pmc, phymod_phy_reset_t *cfg_reset, uint32 value)
{
    phymod_phy_access_t *pm_phy;
    phymod_phy_reset_t  reset;
    int                 idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        sal_memcpy(&reset, cfg_reset, sizeof(reset));
        reset.tx = (phymod_reset_direction_t) value;
        SOC_IF_ERROR_RETURN(phymod_phy_reset_set(pm_phy, &reset));

        /* after successfully setting the parity, update the configured value */
        cfg_reset->tx = (phymod_reset_direction_t) value;
    }

    return(SOC_E_NONE);
}

/* 
 * phy8806x_cl72_enable_set
 */
STATIC int
phy8806x_cl72_enable_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t *pm_phy;
    int                 idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN(phymod_phy_cl72_set(pm_phy, value));
    }

    return(SOC_E_NONE);
}

STATIC int
phy8806x_lane_map_set(soc_phymod_ctrl_t *pmc, uint32 value){
    int idx;
    phymod_lane_map_t lane_map;

    lane_map.num_of_lanes = NUM_LANES;
    if(pmc->phy[0] == NULL){
        return(SOC_E_INTERNAL);
    }
    for (idx=0; idx < NUM_LANES; idx++) {
        lane_map.lane_map_rx[idx] = (value >> (idx * 4 /* 4 bit per lane */)) & 0xf;
    }
    for (idx=0; idx < NUM_LANES; idx++) {
        lane_map.lane_map_tx[idx] = (value >> (16 + idx * 4 /* 4 bit per lane */)) & 0xf;
    }
    SOC_IF_ERROR_RETURN(phymod_core_lane_map_set(&pmc->phy[0]->core->pm_core, &lane_map));
    return(SOC_E_NONE);
}

/* 
 * phy8806x_per_lane_prbs_poly_set
 */
int
phy8806x_per_lane_prbs_poly_set(soc_phymod_ctrl_t *pmc, int lane, uint32 value)
{
   return(SOC_E_UNAVAIL);
}

STATIC int
phy8806x_sdk_poly_to_phymod_poly(uint32 sdk_poly, phymod_prbs_poly_t *phymod_poly){
    switch(sdk_poly){
    case SOC_PHY_PRBS_POLYNOMIAL_X7_X6_1:
        *phymod_poly = phymodPrbsPoly7;
        break;
    case SOC_PHY_PRBS_POLYNOMIAL_X15_X14_1:
        *phymod_poly = phymodPrbsPoly15;
        break;
    case SOC_PHY_PRBS_POLYNOMIAL_X23_X18_1:
        *phymod_poly = phymodPrbsPoly23;
        break;
    case SOC_PHY_PRBS_POLYNOMIAL_X31_X28_1:
        *phymod_poly = phymodPrbsPoly31;
        break;
    case SOC_PHY_PRBS_POLYNOMIAL_X9_X5_1:
        *phymod_poly = phymodPrbsPoly9;
        break;
    case SOC_PHY_PRBS_POLYNOMIAL_X11_X9_1:
        *phymod_poly = phymodPrbsPoly11;
        break;
    case SOC_PHY_PRBS_POLYNOMIAL_X58_X31_1:
        *phymod_poly = phymodPrbsPoly58;
        break;
    default:
        return SOC_E_INTERNAL;
    }
    return SOC_E_NONE;
}

/* 
 * phy8806x_prbs_tx_poly_set
 */
int
phy8806x_prbs_tx_poly_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t        prbs;
    uint32_t flags = 0;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }
    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(pm_phy,  flags, &prbs));
    SOC_IF_ERROR_RETURN(phy8806x_sdk_poly_to_phymod_poly(value, &prbs.poly));
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_set(pm_phy, flags, &prbs));
    return(SOC_E_NONE);
}

/* 
 * phy8806x_prbs_rx_poly_set
 */
STATIC int
phy8806x_prbs_rx_poly_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t        prbs;
    uint32_t flags = 0;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }
    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(pm_phy, flags, &prbs));
    SOC_IF_ERROR_RETURN(phy8806x_sdk_poly_to_phymod_poly(value, &prbs.poly));
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_set(pm_phy, flags, &prbs));
    return(SOC_E_NONE);
}


/* 
 * phy8806x_per_lane_prbs_tx_invert_data_set
 */
int
phy8806x_per_lane_prbs_tx_invert_data_set(soc_phymod_ctrl_t *pmc, int lane, uint32 value)
{
   return(SOC_E_UNAVAIL);
}

/* 
 * phy8806x_prbs_tx_invert_data_set
 */
STATIC int
phy8806x_prbs_tx_invert_data_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t        prbs;
    uint32_t flags = 0;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(pm_phy, flags, &prbs));
    prbs.invert = value;
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_set(pm_phy, flags,  &prbs));
    return(SOC_E_NONE);
}

/* 
 * phy8806x_prbs_rx_invert_data_set
 */
int
phy8806x_prbs_rx_invert_data_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t        prbs;
    uint32_t flags = 0;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(pm_phy, flags,  &prbs));
    prbs.invert = value;
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_set(pm_phy, flags, &prbs));
    return(SOC_E_NONE);
}

/* 
 * phy8806x_per_lane_prbs_tx_enable_set
 */
int
phy8806x_per_lane_prbs_tx_enable_set(soc_phymod_ctrl_t *pmc, int lane, uint32 value)
{
   return(SOC_E_UNAVAIL);
}
/* 
 * phy8806x_prbs_tx_enable_set
 */
STATIC int
phy8806x_prbs_tx_enable_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    uint32_t flags = 0;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }
    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_enable_set(pm_phy, flags, value));
    return(SOC_E_NONE);
}

/* 
 * phy8806x_prbs_rx_enable_set
 */
STATIC int
phy8806x_prbs_rx_enable_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    uint32_t flags = 0;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_enable_set(pm_phy, flags,  value));
    return(SOC_E_NONE);
}

/* 
 * phy8806x_loopback_internal_set
 */
STATIC int 
phy8806x_loopback_internal_set(soc_phymod_ctrl_t *pmc, int sys, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
#if defined(INCLUDE_FCMAP)
    soc_phymod_core_t *core;
    phy_ctrl_t        *pc;
#endif
    phymod_loopback_mode_t lb_mode;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    if (sys) {
        SOC_IF_ERROR_RETURN
            (phymod_phy_loopback_set(pm_phy, phymodLoopbackSysGlobal, value));
    } else {
        lb_mode = phymodLoopbackGlobal;
#if defined(INCLUDE_FCMAP)
        core = pmc->phy[0]->core;
        pc = EXT_PHY_SW_STATE(core->unit, core->port);
        if (pc->fcmap_enable) {
            lb_mode = phymodLoopbackGlobalPMD;
        }
#endif
        SOC_IF_ERROR_RETURN
            (phymod_phy_loopback_set(pm_phy, lb_mode, value));

    }
    return(SOC_E_NONE);
}

/* 
 * phy8806x_loopback_pmd_set
 */
STATIC int 
phy8806x_loopback_pmd_set(soc_phymod_ctrl_t *pmc, int sys, uint32 value)
{
    phymod_phy_access_t  *pm_phy;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    if (sys) {
        SOC_IF_ERROR_RETURN
            (phymod_phy_loopback_set(pm_phy, phymodLoopbackSysGlobalPMD, value));
    } else {
        SOC_IF_ERROR_RETURN
            (phymod_phy_loopback_set(pm_phy, phymodLoopbackGlobalPMD, value));
    }

    return(SOC_E_NONE);
}

/* 
 * phy8806x_loopback_remote_set
 */
STATIC int 
phy8806x_loopback_remote_set(soc_phymod_ctrl_t *pmc, int sys, uint32 value)
{
    phymod_phy_access_t  *pm_phy;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    if (sys) {
        SOC_IF_ERROR_RETURN
            (phymod_phy_loopback_set(pm_phy, phymodLoopbackSysRemotePMD, value));
    } else {
        SOC_IF_ERROR_RETURN
            (phymod_phy_loopback_set(pm_phy, phymodLoopbackRemotePMD, value));
    }
    return(SOC_E_NONE);
}

/* 
 * phy8806x_loopback_pcs_set
 */
/* STATIC */ int 
phy8806x_loopback_pcs_set(soc_phymod_ctrl_t *pmc, int sys, uint32 value)
{
    phymod_phy_access_t  *pm_phy;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    if (sys) {
        SOC_IF_ERROR_RETURN
            (phymod_phy_loopback_set(pm_phy, phymodLoopbackSysGlobalPCS, value));
    } else {
        SOC_IF_ERROR_RETURN
            (phymod_phy_loopback_set(pm_phy, phymodLoopbackGlobalPCS, value));
    }

    return(SOC_E_NONE);
}

/* 
 * phy8806x_loopback_remote_pcs_set
 */
STATIC int 
phy8806x_loopback_remote_pcs_set(soc_phymod_ctrl_t *pmc, int sys, uint32 value)
{
    phymod_phy_access_t  *pm_phy;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    if (sys) {
        SOC_IF_ERROR_RETURN
            (phymod_phy_loopback_set(pm_phy, phymodLoopbackSysRemotePCS, value));
    } else {
        SOC_IF_ERROR_RETURN
            (phymod_phy_loopback_set(pm_phy, phymodLoopbackRemotePCS, value));
    }

    return(SOC_E_NONE);
}

/* 
 * phy8806x_fec_enable_set
 */
STATIC int 
phy8806x_fec_enable_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t     *pm_phy;
    int                     idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        SOC_IF_ERROR_RETURN(phymod_phy_fec_enable_set(pm_phy, value));
    }
    return(SOC_E_NONE);
}

/* 
 * phy8806x_power_set
 */
STATIC int
phy8806x_power_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t     *pm_phy;
    phymod_phy_power_t      power;
    int                     idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        phymod_phy_power_t_init(&power);
        if (value) {
            power.tx = phymodPowerOn;
            power.rx = phymodPowerOn;
        } 
        else {
            power.tx = phymodPowerOff;
            power.rx = phymodPowerOff;
        }

        SOC_IF_ERROR_RETURN(phymod_phy_power_set(pm_phy, &power));
    }

    return(SOC_E_NONE);
}

/* 
 * given a pc (phymod_ctrl_t) and logical lane number, 
 *    find the correct soc_phymod_phy_t object and lane
 */
STATIC int
_phy8806x_find_soc_phy_lane(soc_phymod_ctrl_t *pmc, uint32_t lane, 
                        soc_phymod_phy_t **p_phy, uint32 *lane_map)
{
    phymod_phy_access_t *pm_phy;
    int idx, lnx, ln_cnt, found;
    uint32 lane_map_copy;

    /* Traverse lanes belonging to this port */
    found = 0;
    ln_cnt = 0;
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;
        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        lane_map_copy = pm_phy->access.lane_mask;
        for (lnx = 0; lnx < 4; lnx++) {
            if ((1 << lnx) & lane_map_copy) {
                if (ln_cnt == lane) {
                    found = 1;
                    break;
                }
                ln_cnt++;
            }
        }
        if (found) {
            *p_phy = pmc->phy[idx];
            *lane_map = (1 << lnx);
            break;
        }
    } 

    if (!found) {
        /* No such lane */
        return SOC_E_PARAM;
    }
    return (SOC_E_NONE);
}


/* 
 * phy8806x_per_lane_preemphasis_set
 */
STATIC int
phy8806x_per_lane_preemphasis_set(soc_phymod_ctrl_t *pmc, int lane, uint32 value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_tx_t         phymod_tx;
    soc_phymod_core_t *soc_core;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy8806x_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;

    soc_core = PHYMOD_ACC_USER_ACC(&pm_phy_copy.access);
    pm_phy_copy.port_loc = (EXT_PHY_SW_STATE(soc_core->unit, soc_core->port)->flags & PHYCTRL_SYS_SIDE_CTRL) ? phymodPortLocSys : phymodPortLocLine;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
    phymod_tx.pre = value & 0xff;
    phymod_tx.main = (value >> 8) & 0xff;
    phymod_tx.post = (value >> 16) & 0xff;
    SOC_IF_ERROR_RETURN(phymod_phy_tx_set(&pm_phy_copy, &phymod_tx));

    return(SOC_E_NONE);
}

/* 
 * phy8806x_preemphasis_set
 */
STATIC int
phy8806x_preemphasis_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t *pm_phy, pm_phy_copy;
    phymod_tx_t         phymod_tx;
    int                 idx;
    soc_phymod_core_t *soc_core;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        soc_core = PHYMOD_ACC_USER_ACC(&pm_phy_copy.access);
        pm_phy_copy.port_loc = (EXT_PHY_SW_STATE(soc_core->unit, soc_core->port)->flags & PHYCTRL_SYS_SIDE_CTRL) ? phymodPortLocSys : phymodPortLocLine;

        SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
        phymod_tx.pre = value & 0xff;
        phymod_tx.main = (value >> 8) & 0xff;
        phymod_tx.post = (value >> 16) & 0xff;
        SOC_IF_ERROR_RETURN(phymod_phy_tx_set(&pm_phy_copy, &phymod_tx));
    }

    return(SOC_E_NONE);
}

/* 
 * phy8806x_tx_fir_pre_set
 */
STATIC int
phy8806x_tx_fir_pre_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t *pm_phy, pm_phy_copy;
    phymod_tx_t         phymod_tx;
    int                 idx;
    soc_phymod_core_t *soc_core;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        soc_core = PHYMOD_ACC_USER_ACC(&pm_phy_copy.access);
        pm_phy_copy.port_loc = (EXT_PHY_SW_STATE(soc_core->unit, soc_core->port)->flags & PHYCTRL_SYS_SIDE_CTRL) ? phymodPortLocSys : phymodPortLocLine;


        SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
        phymod_tx.pre = value;
        SOC_IF_ERROR_RETURN(phymod_phy_tx_set(&pm_phy_copy, &phymod_tx));
    }

    return(SOC_E_NONE);
}

/* 
 * phy8806x_tx_fir_main_set
 */
STATIC int
phy8806x_tx_fir_main_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t *pm_phy, pm_phy_copy;
    phymod_tx_t         phymod_tx;
    int                 idx;
    soc_phymod_core_t *soc_core;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        soc_core = PHYMOD_ACC_USER_ACC(&pm_phy_copy.access);
        pm_phy_copy.port_loc = (EXT_PHY_SW_STATE(soc_core->unit, soc_core->port)->flags & PHYCTRL_SYS_SIDE_CTRL) ? phymodPortLocSys : phymodPortLocLine;


        SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
        phymod_tx.main = value;
        SOC_IF_ERROR_RETURN(phymod_phy_tx_set(&pm_phy_copy, &phymod_tx));
    }

    return(SOC_E_NONE);
}

/* 
 * phy8806x_tx_fir_post_set
 */
STATIC int
phy8806x_tx_fir_post_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t *pm_phy, pm_phy_copy;
    phymod_tx_t         phymod_tx;
    int                 idx;
    soc_phymod_core_t *soc_core;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        soc_core = PHYMOD_ACC_USER_ACC(&pm_phy_copy.access);
        pm_phy_copy.port_loc = (EXT_PHY_SW_STATE(soc_core->unit, soc_core->port)->flags & PHYCTRL_SYS_SIDE_CTRL) ? phymodPortLocSys : phymodPortLocLine;


        SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
        phymod_tx.post = value;
        SOC_IF_ERROR_RETURN(phymod_phy_tx_set(&pm_phy_copy, &phymod_tx));
    }

    return(SOC_E_NONE);
}

/* 
 * phy8806x_tx_fir_post2_set
 */
STATIC int
phy8806x_tx_fir_post2_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t *pm_phy, pm_phy_copy;
    phymod_tx_t         phymod_tx;
    int                 idx;
    soc_phymod_core_t *soc_core;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        soc_core = PHYMOD_ACC_USER_ACC(&pm_phy_copy.access);
        pm_phy_copy.port_loc = (EXT_PHY_SW_STATE(soc_core->unit, soc_core->port)->flags & PHYCTRL_SYS_SIDE_CTRL) ? phymodPortLocSys : phymodPortLocLine;


        SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
        phymod_tx.post2 = value;
        SOC_IF_ERROR_RETURN(phymod_phy_tx_set(&pm_phy_copy, &phymod_tx));
    }

    return(SOC_E_NONE);
}

/* 
 * phy8806x_tx_fir_post3_set
 */
STATIC int
phy8806x_tx_fir_post3_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t *pm_phy, pm_phy_copy;
    phymod_tx_t         phymod_tx;
    int                 idx;
    soc_phymod_core_t *soc_core;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        soc_core = PHYMOD_ACC_USER_ACC(&pm_phy_copy.access);
        pm_phy_copy.port_loc = (EXT_PHY_SW_STATE(soc_core->unit, soc_core->port)->flags & PHYCTRL_SYS_SIDE_CTRL) ? phymodPortLocSys : phymodPortLocLine;


        SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
        phymod_tx.post3 = value;
        SOC_IF_ERROR_RETURN(phymod_phy_tx_set(&pm_phy_copy, &phymod_tx));
    }

    return(SOC_E_NONE);
}

/* 
 * phy8806x_per_lane_driver_current_set
 */
STATIC int
phy8806x_per_lane_driver_current_set(soc_phymod_ctrl_t *pmc, int lane, uint32 value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_tx_t         phymod_tx;
    soc_phymod_core_t *soc_core;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy8806x_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    soc_core = PHYMOD_ACC_USER_ACC(&pm_phy_copy.access);
    pm_phy_copy.port_loc = (EXT_PHY_SW_STATE(soc_core->unit, soc_core->port)->flags & PHYCTRL_SYS_SIDE_CTRL) ? phymodPortLocSys : phymodPortLocLine;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
    phymod_tx.amp = value;
    SOC_IF_ERROR_RETURN(phymod_phy_tx_set(&pm_phy_copy, &phymod_tx));

    return(SOC_E_NONE);
}


/* 
 * phy8806x_driver_current_set
 */
STATIC int
phy8806x_driver_current_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t *pm_phy, pm_phy_copy;
    phymod_tx_t         phymod_tx;
    int                 idx;
    soc_phymod_core_t *soc_core;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        soc_core = PHYMOD_ACC_USER_ACC(&pm_phy_copy.access);
        pm_phy_copy.port_loc = (EXT_PHY_SW_STATE(soc_core->unit, soc_core->port)->flags & PHYCTRL_SYS_SIDE_CTRL) ? phymodPortLocSys : phymodPortLocLine;


        SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
        phymod_tx.amp = value;
        SOC_IF_ERROR_RETURN(phymod_phy_tx_set(&pm_phy_copy, &phymod_tx));
    }

    return(SOC_E_NONE);
}

STATIC int
phy8806x_firmware_unreliable_los_set(soc_phymod_ctrl_t *pmc, uint32 enable)
{
    phymod_phy_access_t    pm_phy_copy, *pm_phy;
    phymod_firmware_lane_config_t fw_config;
    int                    idx;
    soc_phymod_core_t      *soc_core;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        if (pmc->phy[idx] == NULL) {
            return(SOC_E_INTERNAL);
        }

        pm_phy = &pmc->phy[idx]->pm_phy;
        if (pm_phy == NULL) {
            return(SOC_E_INTERNAL);
        }

        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        soc_core = PHYMOD_ACC_USER_ACC(&pm_phy_copy.access);
        pm_phy_copy.port_loc = (EXT_PHY_SW_STATE(soc_core->unit, soc_core->port)->flags & PHYCTRL_SYS_SIDE_CTRL) ? phymodPortLocSys : phymodPortLocLine;

        SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_get(&pm_phy_copy, &fw_config));

        if (enable) {
            fw_config.UnreliableLos = 1;
        } else {
            fw_config.UnreliableLos = 0;
        }

        SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_set(&pm_phy_copy, fw_config));
    }

    return(SOC_E_NONE);
}


STATIC int
phy8806x_firmware_dfe_enable_set(soc_phymod_ctrl_t *pmc, uint32 enable)
{
    phymod_phy_access_t    pm_phy_copy, *pm_phy;
    phymod_firmware_lane_config_t fw_config;
    int                    idx;
    soc_phymod_core_t      *soc_core;
    
    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        if (pmc->phy[idx] == NULL) {
            return(SOC_E_INTERNAL);
        }

        pm_phy = &pmc->phy[idx]->pm_phy;
        if (pm_phy == NULL) {
            return(SOC_E_INTERNAL);
        }

        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        soc_core = PHYMOD_ACC_USER_ACC(&pm_phy_copy.access);
        pm_phy_copy.port_loc = (EXT_PHY_SW_STATE(soc_core->unit, soc_core->port)->flags & PHYCTRL_SYS_SIDE_CTRL) ? phymodPortLocSys : phymodPortLocLine;

        SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_get(&pm_phy_copy, &fw_config));

        if (enable) {
            fw_config.DfeOn = 1;
        } else {
            fw_config.DfeOn = 0;
        }

        SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_set(&pm_phy_copy, fw_config));
    }

    return(SOC_E_NONE);
}

/*
 * tscf_firmware_mode_set
 */
STATIC int
phy8806x_firmware_lp_dfe_enable_set(soc_phymod_ctrl_t *pmc, uint32 enable)
{
    phymod_phy_access_t    pm_phy_copy, *pm_phy;
    phymod_firmware_lane_config_t fw_config;
    int                    idx;
    soc_phymod_core_t      *soc_core;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        if (pmc->phy[idx] == NULL) {
            return(SOC_E_INTERNAL);
        }

        pm_phy = &pmc->phy[idx]->pm_phy;
        if (pm_phy == NULL) {
            return(SOC_E_INTERNAL);
        }

        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        soc_core = PHYMOD_ACC_USER_ACC(&pm_phy_copy.access);
        pm_phy_copy.port_loc = (EXT_PHY_SW_STATE(soc_core->unit, soc_core->port)->flags & PHYCTRL_SYS_SIDE_CTRL) ? phymodPortLocSys : phymodPortLocLine;

        SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_get(&pm_phy_copy, &fw_config));

        if (enable) {
            fw_config.DfeOn = 1;
            fw_config.LpDfeOn = 1;
        } else {
            fw_config.LpDfeOn = 0;
        }

        SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_set(&pm_phy_copy, fw_config));
    }

    return(SOC_E_NONE);
}

STATIC int
phy8806x_firmware_br_dfe_enable_set(soc_phymod_ctrl_t *pmc, uint32 enable)
{
    phymod_phy_access_t    pm_phy_copy, *pm_phy;
    phymod_firmware_lane_config_t fw_config;
    int                    idx;
    soc_phymod_core_t      *soc_core;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        if (pmc->phy[idx] == NULL) {
            return(SOC_E_INTERNAL);
        }

        pm_phy = &pmc->phy[idx]->pm_phy;
        if (pm_phy == NULL) {
            return(SOC_E_INTERNAL);
        }

        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        soc_core = PHYMOD_ACC_USER_ACC(&pm_phy_copy.access);
        pm_phy_copy.port_loc = (EXT_PHY_SW_STATE(soc_core->unit, soc_core->port)->flags & PHYCTRL_SYS_SIDE_CTRL) ? phymodPortLocSys : phymodPortLocLine;

        SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_get(&pm_phy_copy, &fw_config));

        if (enable) {
            fw_config.DfeOn = 1;
            fw_config.ForceBrDfe = 1;
        } else {
            fw_config.ForceBrDfe = 0;
        }

        SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_set(&pm_phy_copy, fw_config));
    }

    return(SOC_E_NONE);
}

/*
 * Function:
 *      phy_8806x_control_set
 * Purpose:
 *      Configure PHY device specific control fucntion. 
 * Parameters:
 *      unit  - BCM unit number.
 *      port  - Port number. 
 *      type  - Control to update 
 *      value - New setting for the control 
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_8806x_control_set(int unit, soc_port_t port, soc_phy_control_t type, uint32 value)
{
    int                 rv;
    phy_ctrl_t          *pc;
    soc_phymod_ctrl_t   *pmc;
    phy8806x_config_t       *pCfg;

    rv = SOC_E_UNAVAIL;

    if ((type < 0) || (type >= SOC_PHY_CONTROL_COUNT)) {
        return (SOC_E_PARAM);
    }

    /* locate phy control, phymod control, and the configuration data */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

#if defined(INCLUDE_FCMAP)
    /* Skip these loopback set for FC */
    if ((pc->fcmap_enable) && 
         (type == SOC_PHY_CONTROL_LOOPBACK_REMOTE_PCS_BYPASS)) {
        return SOC_E_NONE;
    }
#endif

    pmc = &pc->phymod_ctrl;
    pCfg = (phy8806x_config_t *) pc->driver_data;

    switch(type) {
    case SOC_PHY_CONTROL_UNRELIABLE_LOS:
        rv = phy8806x_firmware_unreliable_los_set(pmc, value);
        break;
    case SOC_PHY_CONTROL_FIRMWARE_DFE_ENABLE:
        rv = phy8806x_firmware_dfe_enable_set(pmc, value);
        break;
    case SOC_PHY_CONTROL_FIRMWARE_LP_DFE_ENABLE:
        rv = phy8806x_firmware_lp_dfe_enable_set(pmc, value);
        break;
    case SOC_PHY_CONTROL_FIRMWARE_BR_DFE_ENABLE:
        rv = phy8806x_firmware_br_dfe_enable_set(pmc, value);
        break;

    /* TX LANE SQUELCH */
    case SOC_PHY_CONTROL_TX_LANE_SQUELCH:
        rv = phy8806x_tx_lane_squelch(pmc, value);
        break;

    /* POLARITY */
    case SOC_PHY_CONTROL_RX_POLARITY:
        rv = phy8806x_rx_polarity_set(pmc, &pCfg->phy_polarity_config, value);;
        break;
    case SOC_PHY_CONTROL_TX_POLARITY:
        rv = phy8806x_tx_polarity_set(pmc, &pCfg->phy_polarity_config, value);
        break;

    /* RESET */
    case SOC_PHY_CONTROL_RX_RESET:
        rv = phy8806x_rx_reset(pmc, &pCfg->phy_reset_config, value);
        break;
    case SOC_PHY_CONTROL_TX_RESET:
        rv = phy8806x_tx_reset(pmc, &pCfg->phy_reset_config, value);
        break;

    /* CL72 ENABLE */
    case SOC_PHY_CONTROL_CL72:
        if (pc->flags & PHYCTRL_SYS_SIDE_CTRL) {
            value = value | PORT_SYS_SIDE_CTRL;
        }
        rv = phy8806x_cl72_enable_set(pmc, value);
        break;

    /* LANE SWAP */
    case SOC_PHY_CONTROL_LANE_SWAP:
        rv = phy8806x_lane_map_set(pmc, value);
        break;

    /* for legacy prbs usage mainly set both tx/rx the same */
    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
        rv = phy8806x_prbs_tx_poly_set(pmc, value);
        rv = phy8806x_prbs_rx_poly_set(pmc, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
        rv = phy8806x_prbs_tx_invert_data_set(pmc, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
        rv = phy8806x_prbs_tx_enable_set(pmc, value);
        rv = phy8806x_prbs_rx_enable_set(pmc, value);
        break;
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
        rv = phy8806x_prbs_tx_enable_set(pmc, value);
        rv = phy8806x_prbs_rx_enable_set(pmc, value);
        break;

    /* LOOPBACK */
    case SOC_PHY_CONTROL_LOOPBACK_INTERNAL:
        rv = phy8806x_loopback_internal_set(pmc, pc->flags & PHYCTRL_SYS_SIDE_CTRL, value);
       break;
    case SOC_PHY_CONTROL_LOOPBACK_PMD:
        rv = phy8806x_loopback_pmd_set(pmc, pc->flags & PHYCTRL_SYS_SIDE_CTRL, value);
       break;
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE:
        rv = phy8806x_loopback_remote_set(pmc, pc->flags & PHYCTRL_SYS_SIDE_CTRL, value);
        break;
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE_PCS_BYPASS:
        rv = phy8806x_loopback_remote_pcs_set(pmc, pc->flags & PHYCTRL_SYS_SIDE_CTRL, value);
        break;

    /* FEC */
    case SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION:
        if (value)     /* 1 for CL74 */
             value = 1;
        if (pc->flags & PHYCTRL_SYS_SIDE_CTRL) {
            value = value | PORT_SYS_SIDE_CTRL;
        }
        rv = phy8806x_fec_enable_set(pmc, value);
        break;

    case SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION_CL91:
        if (value)     
             value = 2; /* 1 for CL74, 2 for Cl91 */
        if (pc->flags & PHYCTRL_SYS_SIDE_CTRL) {
            value = value | PORT_SYS_SIDE_CTRL;
        }
        rv = phy8806x_fec_enable_set(pmc, value);
        break;

    /* POWER */
    case SOC_PHY_CONTROL_POWER:
       rv = phy8806x_power_set(pmc, value);
       break;

    case SOC_PHY_CONTROL_FORCED_SPEED_LINE_SIDE:
       if (pCfg->speed_config.port_mode == ETH_ETHERRAM)
           rv = phy8806x_forced_speed_line_side_set(pmc, value);
       else 
           rv = SOC_E_UNAVAIL;
       break; 
    case SOC_PHY_CONTROL_AUTONEG_LINE_SIDE:
       if (pCfg->speed_config.port_mode == ETH_ETHERRAM) 
           rv = phy8806x_autoneg_line_side_set(pmc, value);
       else
           rv = SOC_E_UNAVAIL;
       break;
    case SOC_PHY_CONTROL_EGRESS_FLOW_CONTROL_MODE:
        if (pCfg->speed_config.port_mode == ETH_ETHERRAM)
            rv = phy8806x_flow_control_mode_set(pmc, value, 0);
        else
            rv = SOC_E_UNAVAIL;
        break;
    case SOC_PHY_CONTROL_INGRESS_FLOW_CONTROL_MODE:
        if (pCfg->speed_config.port_mode == ETH_ETHERRAM)
            rv = phy8806x_flow_control_mode_set(pmc, value, 1);
        else
            rv = SOC_E_UNAVAIL;
        break;
    case SOC_PHY_CONTROL_PSM_COS_BMP:
        if (pCfg->speed_config.port_mode == ETH_ETHERRAM)
            rv = phy8806x_psm_cos_bmp_set(pmc, value);
        else
            rv = SOC_E_UNAVAIL;
        break;
    case SOC_PHY_CONTROL_PFC_USE_IP_COS:
        if (pCfg->speed_config.port_mode == ETH_ETHERRAM) {
            rv = phy8806x_pfc_use_ip_cos_set(pmc, value); 
        } else {
            rv = SOC_E_UNAVAIL;
        }
        break;
    case SOC_PHY_CONTROL_TX_FIR_PRE:
        rv = phy8806x_tx_fir_pre_set(pmc, value);
        break;
    case SOC_PHY_CONTROL_TX_FIR_MAIN:
        rv = phy8806x_tx_fir_main_set(pmc, value);
        break;
    case SOC_PHY_CONTROL_TX_FIR_POST:
        rv = phy8806x_tx_fir_post_set(pmc, value);
        break;
    case SOC_PHY_CONTROL_TX_FIR_POST2:
        rv = phy8806x_tx_fir_post2_set(pmc, value);
        break;
    case SOC_PHY_CONTROL_TX_FIR_POST3:
        rv = phy8806x_tx_fir_post3_set(pmc, value);
        break;
    /* PREEMPHASIS */
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE0:
        rv = phy8806x_per_lane_preemphasis_set(pmc, 0, value);
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE1:
        rv = phy8806x_per_lane_preemphasis_set(pmc, 1, value);
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE2:
        rv = phy8806x_per_lane_preemphasis_set(pmc, 2, value);
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE3:
        rv = phy8806x_per_lane_preemphasis_set(pmc, 3, value);
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS:
    {
        /* we need to check if the value is 0xffffff, if yes, then clear
        the user_set_tx bit */
        if (value == 0xffffff) {
            pCfg->user_set_tx = 0;
        } else {     
            rv = phy8806x_preemphasis_set(pmc, value);
        }
        break;
    }
    /* DRIVER CURRENT */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0:
        rv = phy8806x_per_lane_driver_current_set(pmc, 0, value);
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1:
        rv = phy8806x_per_lane_driver_current_set(pmc, 1, value);
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2:
        rv = phy8806x_per_lane_driver_current_set(pmc, 2, value);
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3:
        rv = phy8806x_per_lane_driver_current_set(pmc, 3, value);
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        rv = phy8806x_driver_current_set(pmc, value);
        break;
    case SOC_PHY_CONTROL_INTERFACE:
        rv = phy_8806x_interface_set(unit, port, value);
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break; 
    }
    return rv;
}

STATIC int
phy8806x_pfc_use_ip_cos_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t     *pm_phy;
    int                     idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        SOC_IF_ERROR_RETURN(phymod_phy_pfc_use_ip_cos_set(pm_phy, value));
    }
    return(SOC_E_NONE);
}

STATIC int
phy8806x_pfc_use_ip_cos_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t     *pm_phy;
    int                     idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        SOC_IF_ERROR_RETURN(phymod_phy_pfc_use_ip_cos_get(pm_phy, value));
    }

    return(SOC_E_NONE);
}

STATIC int
phy8806x_psm_cos_bmp_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t     *pm_phy;
    int                     idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        SOC_IF_ERROR_RETURN(phymod_phy_psm_cos_bmp_set(pm_phy, value));
    }
    return(SOC_E_NONE);
}

STATIC int
phy8806x_psm_cos_bmp_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t     *pm_phy;
    int                     idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        SOC_IF_ERROR_RETURN(phymod_phy_psm_cos_bmp_get(pm_phy, value));
    }

    return(SOC_E_NONE);
}

STATIC int                               
phy8806x_flow_control_mode_set(soc_phymod_ctrl_t *pmc, uint32 value, uint8 ingress)
{
    phymod_phy_access_t     *pm_phy;
    int                     idx;
                             
    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;
                             
        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        SOC_IF_ERROR_RETURN(phymod_phy_flow_control_mode_set(pm_phy, value, ingress));
    }
    return(SOC_E_NONE);
}

STATIC int
phy8806x_flow_control_mode_get(soc_phymod_ctrl_t *pmc, uint8 ingress, uint32 *value)
{
    phymod_phy_access_t     *pm_phy;
    int                     idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        SOC_IF_ERROR_RETURN(phymod_phy_flow_control_mode_get(pm_phy, ingress, value));
    }

    return(SOC_E_NONE);
}

STATIC int
phy8806x_forced_speed_line_side_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t     *pm_phy;
    int                     idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        SOC_IF_ERROR_RETURN(phymod_phy_forced_speed_line_side_set(pm_phy, value));
    }

    return(SOC_E_NONE);
}

STATIC int
phy8806x_forced_speed_line_side_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t     *pm_phy;
    int                     idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        SOC_IF_ERROR_RETURN(phymod_phy_forced_speed_line_side_get(pm_phy, value));
    }

    return(SOC_E_NONE);
}


STATIC int
phy8806x_autoneg_line_side_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t     *pm_phy;
    int                     idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        SOC_IF_ERROR_RETURN(phymod_phy_autoneg_line_side_set(pm_phy, value));
    }


    return(SOC_E_NONE);
}

STATIC int
phy8806x_autoneg_line_side_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t     *pm_phy;
    int                     idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        SOC_IF_ERROR_RETURN(phymod_phy_autoneg_line_side_get(pm_phy, value));
    }

    return(SOC_E_NONE);
}

/* 
 * phy8806x_cl72_enable_get
 */
STATIC int
phy8806x_cl72_enable_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
         return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(phymod_phy_cl72_get(pm_phy, value));

    return(SOC_E_NONE);
}

/* 
 * phy8806x_cl72_status_get
 */
STATIC int
phy8806x_cl72_status_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_cl72_status_t status;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_core_t *soc_core;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    /* Make a copy of the phy access and overwrite the desired lane */
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));

    soc_core = PHYMOD_ACC_USER_ACC(&pm_phy_copy.access);
    pm_phy_copy.port_loc = (EXT_PHY_SW_STATE(soc_core->unit, soc_core->port)->flags & PHYCTRL_SYS_SIDE_CTRL) ? phymodPortLocSys : phymodPortLocLine;

    SOC_IF_ERROR_RETURN(phymod_phy_cl72_status_get(&pm_phy_copy, &status));
    *value = status.locked;

    return(SOC_E_NONE);
}

/* 
 * phy8806x_prbs_rx_status_get
 */
STATIC int
phy8806x_prbs_rx_status_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t    *pm_phy;
    phymod_prbs_status_t   prbs_tmp;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }
    /* $$$ Need to add diagnostic API */
    SOC_IF_ERROR_RETURN
        (phymod_phy_prbs_status_get(pm_phy, 0, &prbs_tmp));
    if (prbs_tmp.prbs_lock == 0) {
        *value = -1;
    } else if ((prbs_tmp.prbs_lock_loss == 1) && (prbs_tmp.prbs_lock == 1)) {
        *value = -2;
    } else {
        *value = prbs_tmp.error_count;
    }     

    return(SOC_E_NONE);
}

/* 
 * phy8806x_loopback_internal_get
 */
STATIC int 
phy8806x_loopback_internal_get(soc_phymod_ctrl_t *pmc, int sys, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    uint32_t               enable;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    if (sys) {
        SOC_IF_ERROR_RETURN
            (phymod_phy_loopback_get(pm_phy, phymodLoopbackSysGlobal, &enable));
    } else {
        SOC_IF_ERROR_RETURN
            (phymod_phy_loopback_get(pm_phy, phymodLoopbackGlobal, &enable));

    }

    *value = enable;

    return(SOC_E_NONE);
}


/* 
 * phy8806x_loopback_pmd_get (this is the PMD global loopback)
 */
STATIC int 
phy8806x_loopback_pmd_get(soc_phymod_ctrl_t *pmc, int sys, uint32 *value)
{
    phymod_phy_access_t    *pm_phy;
    uint32_t               enable;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    if (sys) {
        SOC_IF_ERROR_RETURN(phymod_phy_loopback_get(pm_phy, phymodLoopbackSysGlobalPMD, &enable));
    } else {
        SOC_IF_ERROR_RETURN(phymod_phy_loopback_get(pm_phy, phymodLoopbackGlobalPMD, &enable));
    }
    *value = enable;
    return(SOC_E_NONE);
}


/* 
 * phy8806x_loopback_remote_get
 */
STATIC int 
phy8806x_loopback_remote_get(soc_phymod_ctrl_t *pmc, int sys, uint32 *value)
{
    phymod_phy_access_t    *pm_phy;
    uint32_t               enable;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    if (sys) {
        SOC_IF_ERROR_RETURN(phymod_phy_loopback_get(pm_phy, phymodLoopbackSysRemotePMD, &enable));
    } else {
        SOC_IF_ERROR_RETURN(phymod_phy_loopback_get(pm_phy, phymodLoopbackRemotePMD, &enable));
    }

    *value = enable;
    return(SOC_E_NONE);
}

/* 
 * phy8806x_loopback_pcs_get
 */
/* STATIC */ int 
phy8806x_loopback_pcs_get(soc_phymod_ctrl_t *pmc, int sys, uint32 *value)
{
    phymod_phy_access_t    *pm_phy;
    uint32_t               enable;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    if (sys) {
        SOC_IF_ERROR_RETURN(phymod_phy_loopback_get(pm_phy, phymodLoopbackSysGlobalPCS, &enable));
    } else {
        SOC_IF_ERROR_RETURN(phymod_phy_loopback_get(pm_phy, phymodLoopbackGlobalPCS, &enable));
    }
    *value = enable;
    return(SOC_E_NONE);
}

/* 
 * phy8806x_loopback_remote_pcs_get
 */
STATIC int 
phy8806x_loopback_remote_pcs_get(soc_phymod_ctrl_t *pmc, int sys, uint32 *value)
{
    phymod_phy_access_t    *pm_phy;
    uint32_t               enable;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    if (sys) {
        SOC_IF_ERROR_RETURN(phymod_phy_loopback_get(pm_phy, phymodLoopbackSysRemotePCS, &enable));
    } else {
        SOC_IF_ERROR_RETURN(phymod_phy_loopback_get(pm_phy, phymodLoopbackRemotePCS, &enable));
    }
    *value = enable;
    return(SOC_E_NONE);
}

/* 
 * phy8806x_fec_get
 */
STATIC int 
phy8806x_fec_enable_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t    *pm_phy;
    uint32_t               enable;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(phymod_phy_fec_enable_get(pm_phy,  &enable));
    *value = enable;

    return(SOC_E_NONE);
}


/* 
 * phy8806x_tx_fir_pre_get
 */
STATIC int
phy8806x_tx_fir_pre_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t *pm_phy, pm_phy_copy;
    phymod_tx_t         phymod_tx;
    soc_phymod_core_t *soc_core;

    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    soc_core = PHYMOD_ACC_USER_ACC(&pm_phy_copy.access);
    pm_phy_copy.port_loc = (EXT_PHY_SW_STATE(soc_core->unit, soc_core->port)->flags & PHYCTRL_SYS_SIDE_CTRL) ? phymodPortLocSys : phymodPortLocLine;


    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
    *value = phymod_tx.pre;

    return(SOC_E_NONE);
}

/* 
 * phy8806x_tx_fir_main_get
 */
STATIC int
phy8806x_tx_fir_main_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t *pm_phy, pm_phy_copy;
    phymod_tx_t         phymod_tx;
    soc_phymod_core_t *soc_core;

    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    soc_core = PHYMOD_ACC_USER_ACC(&pm_phy_copy.access);
    pm_phy_copy.port_loc = (EXT_PHY_SW_STATE(soc_core->unit, soc_core->port)->flags & PHYCTRL_SYS_SIDE_CTRL) ? phymodPortLocSys : phymodPortLocLine;


    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
    *value = phymod_tx.main;

    return(SOC_E_NONE);
}

/* 
 * phy8806x_tx_fir_post_get
 */
STATIC int
phy8806x_tx_fir_post_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t *pm_phy, pm_phy_copy;
    phymod_tx_t         phymod_tx;
    soc_phymod_core_t *soc_core;

    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    soc_core = PHYMOD_ACC_USER_ACC(&pm_phy_copy.access);
    pm_phy_copy.port_loc = (EXT_PHY_SW_STATE(soc_core->unit, soc_core->port)->flags & PHYCTRL_SYS_SIDE_CTRL) ? phymodPortLocSys : phymodPortLocLine;


    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
    *value = phymod_tx.post;

    return(SOC_E_NONE);
}

/* 
 * phy8806x_tx_fir_post2_get
 */
STATIC int
phy8806x_tx_fir_post2_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t *pm_phy, pm_phy_copy;
    phymod_tx_t         phymod_tx;
    soc_phymod_core_t *soc_core;

    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    soc_core = PHYMOD_ACC_USER_ACC(&pm_phy_copy.access);
    pm_phy_copy.port_loc = (EXT_PHY_SW_STATE(soc_core->unit, soc_core->port)->flags & PHYCTRL_SYS_SIDE_CTRL) ? phymodPortLocSys : phymodPortLocLine;


    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
    *value = phymod_tx.post2;

    return(SOC_E_NONE);
}

/* 
 * phy8806x_tx_fir_post3_get
 */
STATIC int
phy8806x_tx_fir_post3_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t *pm_phy, pm_phy_copy;
    phymod_tx_t         phymod_tx;
    soc_phymod_core_t *soc_core;

    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    soc_core = PHYMOD_ACC_USER_ACC(&pm_phy_copy.access);
    pm_phy_copy.port_loc = (EXT_PHY_SW_STATE(soc_core->unit, soc_core->port)->flags & PHYCTRL_SYS_SIDE_CTRL) ? phymodPortLocSys : phymodPortLocLine;


    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
    *value = phymod_tx.post3;

    return(SOC_E_NONE);
}


/* 
 * phy8806x_per_lane_preemphasis_get
 */
STATIC int
phy8806x_per_lane_preemphasis_get(soc_phymod_ctrl_t *pmc, int lane, uint32 *value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_tx_t         phymod_tx;
    soc_phymod_core_t *soc_core;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy8806x_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    soc_core = PHYMOD_ACC_USER_ACC(&pm_phy_copy.access);
    pm_phy_copy.port_loc = (EXT_PHY_SW_STATE(soc_core->unit, soc_core->port)->flags & PHYCTRL_SYS_SIDE_CTRL) ? phymodPortLocSys : phymodPortLocLine;
    pm_phy_copy.access.lane_mask = lane_map;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
    *value = phymod_tx.pre  | (phymod_tx.main << 8) | (phymod_tx.post << 16);

    return(SOC_E_NONE);
}


/* 
 * phy8806x_preemphasis_get
 */
STATIC int
phy8806x_preemphasis_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t *pm_phy, pm_phy_copy;
    phymod_tx_t         phymod_tx;
    soc_phymod_core_t *soc_core;

    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    soc_core = PHYMOD_ACC_USER_ACC(&pm_phy_copy.access);
    pm_phy_copy.port_loc = (EXT_PHY_SW_STATE(soc_core->unit, soc_core->port)->flags & PHYCTRL_SYS_SIDE_CTRL) ? phymodPortLocSys : phymodPortLocLine;


    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
    *value = phymod_tx.pre  | (phymod_tx.main << 8) | (phymod_tx.post << 16);

    return(SOC_E_NONE);
}


/* 
 * phy8806x_per_lane_driver_current_get
 */
STATIC int
phy8806x_per_lane_driver_current_get(soc_phymod_ctrl_t *pmc, int lane, uint32 *value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_tx_t         phymod_tx;
    soc_phymod_core_t *soc_core;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy8806x_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    soc_core = PHYMOD_ACC_USER_ACC(&pm_phy_copy.access);
    pm_phy_copy.port_loc = (EXT_PHY_SW_STATE(soc_core->unit, soc_core->port)->flags & PHYCTRL_SYS_SIDE_CTRL) ? phymodPortLocSys : phymodPortLocLine;
    pm_phy_copy.access.lane_mask = lane_map;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
    *value = phymod_tx.amp;

    return(SOC_E_NONE);
}

STATIC int
phy8806x_firmware_dfe_enable_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t    pm_phy_copy, *pm_phy;
    phymod_firmware_lane_config_t fw_config;
    soc_phymod_core_t *soc_core;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }

    pm_phy = &pmc->phy[0]->pm_phy;
    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    soc_core = PHYMOD_ACC_USER_ACC(&pm_phy_copy.access);
    pm_phy_copy.port_loc = (EXT_PHY_SW_STATE(soc_core->unit, soc_core->port)->flags & PHYCTRL_SYS_SIDE_CTRL) ? phymodPortLocSys : phymodPortLocLine;

    SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_get(&pm_phy_copy, &fw_config));

    if (fw_config.DfeOn) {
        *value = 1;
    } else {
        *value = 0;
    }

    return(SOC_E_NONE);
}

STATIC int
phy8806x_firmware_unreliable_los_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t    pm_phy_copy, *pm_phy;
    phymod_firmware_lane_config_t fw_config;
    soc_phymod_core_t *soc_core;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }

    pm_phy = &pmc->phy[0]->pm_phy;
    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    soc_core = PHYMOD_ACC_USER_ACC(&pm_phy_copy.access);
    pm_phy_copy.port_loc = (EXT_PHY_SW_STATE(soc_core->unit, soc_core->port)->flags & PHYCTRL_SYS_SIDE_CTRL) ? phymodPortLocSys : phymodPortLocLine;

    SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_get(&pm_phy_copy, &fw_config));

    if (fw_config.UnreliableLos) {
        *value = 1;
    } else {
        *value = 0;
    }

    return(SOC_E_NONE);
}

STATIC int
phy8806x_firmware_lp_dfe_enable_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t    pm_phy_copy, *pm_phy;
    phymod_firmware_lane_config_t fw_config;
    soc_phymod_core_t *soc_core;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }

    pm_phy = &pmc->phy[0]->pm_phy;
    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    soc_core = PHYMOD_ACC_USER_ACC(&pm_phy_copy.access);
    pm_phy_copy.port_loc = (EXT_PHY_SW_STATE(soc_core->unit, soc_core->port)->flags & PHYCTRL_SYS_SIDE_CTRL) ? phymodPortLocSys : phymodPortLocLine;

    SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_get(&pm_phy_copy, &fw_config));

    if (fw_config.LpDfeOn) {
        *value = 1;
    } else {
        *value = 0;
    }

    return(SOC_E_NONE);
}

/*
 * tscf_firmware_dfe_enable_get
 */
STATIC int
phy8806x_firmware_br_dfe_enable_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t    pm_phy_copy, *pm_phy;
    phymod_firmware_lane_config_t fw_config;
    soc_phymod_core_t *soc_core;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }

    pm_phy = &pmc->phy[0]->pm_phy;
    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    soc_core = PHYMOD_ACC_USER_ACC(&pm_phy_copy.access);
    pm_phy_copy.port_loc = (EXT_PHY_SW_STATE(soc_core->unit, soc_core->port)->flags & PHYCTRL_SYS_SIDE_CTRL) ? phymodPortLocSys : phymodPortLocLine;

    SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_get(&pm_phy_copy, &fw_config));

    if (fw_config.ForceBrDfe) {
        *value = 1;
    } else {
        *value = 0;
    }

    return(SOC_E_NONE);
}


/*
 * Function:
 *      phy_8806x_control_get
 * Purpose:
 *      Get current control settings of the PHY. 
 * Parameters:
 *      unit  - BCM unit number.
 *      port  - Port number. 
 *      type  - Control to update 
 *      value - (OUT) Current setting for the control 
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_8806x_control_get(int unit, soc_port_t port, soc_phy_control_t type, uint32 *value)
{
    int                 rv;
    phy_ctrl_t          *pc;
    soc_phymod_ctrl_t   *pmc;
    /* phy8806x_config_t       *pCfg; */

    if ((type < 0) || (type >= SOC_PHY_CONTROL_COUNT)) {
        return (SOC_E_PARAM);
    }

    /* locate phy control */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

#if defined(INCLUDE_FCMAP)
    /* Skip all loopback get for FC */
    if ((pc->fcmap_enable) && 
         (type == SOC_PHY_CONTROL_LOOPBACK_REMOTE_PCS_BYPASS)) {
        return SOC_E_NONE;
    }
#endif

    pmc = &pc->phymod_ctrl;
    /* pCfg = (phy8806x_config_t *) pc->driver_data; */

    switch(type) {
    case SOC_PHY_CONTROL_UNRELIABLE_LOS:
       rv = phy8806x_firmware_unreliable_los_get(pmc, value);
       break;
    case SOC_PHY_CONTROL_FIRMWARE_DFE_ENABLE:
       rv = phy8806x_firmware_dfe_enable_get(pmc, value);
       break;
    case SOC_PHY_CONTROL_FIRMWARE_LP_DFE_ENABLE:
       rv = phy8806x_firmware_lp_dfe_enable_get(pmc, value);
       break;
    case SOC_PHY_CONTROL_FIRMWARE_BR_DFE_ENABLE:
       rv = phy8806x_firmware_br_dfe_enable_get(pmc, value);
       break; 
   /* CL72 */
    case SOC_PHY_CONTROL_CL72:
      if (pc->flags & PHYCTRL_SYS_SIDE_CTRL) {
        *value = *value | PORT_SYS_SIDE_CTRL;
      }
      rv = phy8806x_cl72_enable_get(pmc, value);
      break;
    case SOC_PHY_CONTROL_CL72_STATUS:
      rv = phy8806x_cl72_status_get(pmc, value);
      break;

    case SOC_PHY_CONTROL_PRBS_RX_STATUS:
      rv = phy8806x_prbs_rx_status_get(pmc, value);
      break;

    /* LOOPBACK */
    case SOC_PHY_CONTROL_LOOPBACK_INTERNAL:
       rv = phy8806x_loopback_internal_get(pmc, pc->flags & PHYCTRL_SYS_SIDE_CTRL, value);
       break;
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE:
       rv = phy8806x_loopback_remote_get(pmc, pc->flags & PHYCTRL_SYS_SIDE_CTRL, value);
       break;
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE_PCS_BYPASS:
       rv = phy8806x_loopback_remote_pcs_get(pmc, pc->flags & PHYCTRL_SYS_SIDE_CTRL, value);
       break;
    case SOC_PHY_CONTROL_LOOPBACK_PMD:
       rv = phy8806x_loopback_pmd_get(pmc, pc->flags & PHYCTRL_SYS_SIDE_CTRL, value);
       break;

    /* FEC */
    case SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION:
        *value = 0;
        if (pc->flags & PHYCTRL_SYS_SIDE_CTRL) {
            LOG_CLI((BSL_META_U(unit, "PHYCTRL_SYS_SIDE_CTRL \n")));
            *value = *value | PORT_SYS_SIDE_CTRL;
        }
      rv = phy8806x_fec_enable_get(pmc, value);
      if (*value != 1) /* Clear if the FEC is not CL74 (value 1) */
            *value = 0;
      break;

    case SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION_CL91:
        *value = 0;
        if (pc->flags & PHYCTRL_SYS_SIDE_CTRL) {
            LOG_CLI((BSL_META_U(unit, "PHYCTRL_SYS_SIDE_CTRL \n")));
            *value = *value | PORT_SYS_SIDE_CTRL;
        }
      rv = phy8806x_fec_enable_get(pmc, value);

      if (*value == 2) /* Set only if the FEC is CL91 (value 2) */
        *value = 1;    
      else              
        *value = 0;   /* Clear if the FEC is not set to CL91 (value 2) */
          
      break;

    case SOC_PHY_CONTROL_FORCED_SPEED_LINE_SIDE:
       rv = phy8806x_forced_speed_line_side_get(pmc, value);
       break; 
    case SOC_PHY_CONTROL_AUTONEG_LINE_SIDE:
       rv = phy8806x_autoneg_line_side_get(pmc, value);
       break;
    case SOC_PHY_CONTROL_EGRESS_FLOW_CONTROL_MODE:
        rv = phy8806x_flow_control_mode_get(pmc, 0, value);
        break; 
    case SOC_PHY_CONTROL_INGRESS_FLOW_CONTROL_MODE:
        rv = phy8806x_flow_control_mode_get(pmc, 1, value);
        break;
    case SOC_PHY_CONTROL_PSM_COS_BMP:
        rv = phy8806x_psm_cos_bmp_get(pmc, value);
        break;
    case SOC_PHY_CONTROL_PFC_USE_IP_COS:
        rv = phy8806x_pfc_use_ip_cos_get(pmc, value);
        break;
    /* PREEMPHASIS */
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE0:
        rv = phy8806x_per_lane_preemphasis_get(pmc, 0, value);
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE1:
        rv = phy8806x_per_lane_preemphasis_get(pmc, 1, value);
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE2:
        rv = phy8806x_per_lane_preemphasis_get(pmc, 2, value);
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE3:
        rv = phy8806x_per_lane_preemphasis_get(pmc, 3, value);
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS:
        rv = phy8806x_preemphasis_get(pmc, value);
            break;
    case SOC_PHY_CONTROL_TX_FIR_PRE:
        /* assume they are all the same as lane 0 */
        rv = phy8806x_tx_fir_pre_get(pmc, value);
        break;
    case SOC_PHY_CONTROL_TX_FIR_MAIN:
        /* assume they are all the same as lane 0 */
        rv = phy8806x_tx_fir_main_get(pmc, value);
        break;
    case SOC_PHY_CONTROL_TX_FIR_POST:
        /* assume they are all the same as lane 0 */
        rv = phy8806x_tx_fir_post_get(pmc, value);
        break;
    case SOC_PHY_CONTROL_TX_FIR_POST2:
        /* assume they are all the same as lane 0 */
        rv = phy8806x_tx_fir_post2_get(pmc, value);
        break;
    case SOC_PHY_CONTROL_TX_FIR_POST3:
        /* assume they are all the same as lane 0 */
        rv = phy8806x_tx_fir_post3_get(pmc, value);
        break;

    /* DRIVER CURRENT */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0:
        rv = phy8806x_per_lane_driver_current_get(pmc, 0, value);
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1:
        rv = phy8806x_per_lane_driver_current_get(pmc, 1, value);
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2:
        rv = phy8806x_per_lane_driver_current_get(pmc, 2, value);
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3:
        rv = phy8806x_per_lane_driver_current_get(pmc, 3, value);
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        rv = phy8806x_per_lane_driver_current_get(pmc, 0, value);
        break;
    case SOC_PHY_CONTROL_SOFTWARE_RX_LOS:
    case SOC_PHY_CONTROL_EEE:
    case SOC_PHY_CONTROL_EEE_AUTO:
        *value = FALSE;
        rv = SOC_E_NONE;
        break;
    case SOC_PHY_CONTROL_INTERFACE:
        rv = phy_8806x_interface_get(unit, port, (soc_port_if_t *)value);
        break;
   /* UNAVAIL */
    default:
        rv = SOC_E_UNAVAIL;
        break; 
    }
    return rv;
}     

/*
 * Function:
 *      phy_8806x_reg_read
 * Purpose:
 *      Routine to read PHY register
 * Parameters:
 *      unit         - BCM unit number
 *      port         - Port number
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_data     - (OUT) Value read from PHY register
 * Note:
 *      This register read function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
STATIC int
phy_8806x_reg_read(int unit, soc_port_t port, uint32 flags,
                  uint32 phy_reg_addr, uint32 *phy_reg_data)
{
    phy_ctrl_t *pc;
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t *pm_phy;
    int idx=0;
    uint32 data = 0;

    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pmc = &pc->phymod_ctrl;
    pm_phy = &pmc->phy[idx]->pm_phy;
    if (flags & SOC_PHY_I2C_DATA8) {
        SOC_IF_ERROR_RETURN
            (phymod_phy_i2c_read(pm_phy, 0, SOC_PHY_I2C_DEVAD(phy_reg_addr), SOC_PHY_I2C_REGAD(phy_reg_addr), 1, (uint8 *)&data));
        data = LE2HOST32(data);
        *phy_reg_data = data & 0xff;
    } else if (flags & SOC_PHY_I2C_DATA16) {
        SOC_IF_ERROR_RETURN
            (phymod_phy_i2c_read(pm_phy, 0, SOC_PHY_I2C_DEVAD(phy_reg_addr), SOC_PHY_I2C_REGAD(phy_reg_addr), 2, (uint8 *)&data));
        data = LE2HOST32(data);
        *phy_reg_data = data & 0xffff;
    } else if (flags & SOC_PHY_PVT_DATA) {
        SOC_IF_ERROR_RETURN
            (phymod_phy_private_read(pm_phy, phy_reg_addr, phy_reg_data));
    } else {
        SOC_IF_ERROR_RETURN
            (phymod_phy_reg_read(pm_phy, phy_reg_addr, phy_reg_data));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8806x_reg_write
 * Purpose:
 *      Routine to write PHY register
 * Parameters:
 *      uint         - BCM unit number
 *      pc           - PHY state
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_data     - Value write to PHY register
 * Note:
 *      This register read function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
STATIC int
phy_8806x_reg_write(int unit, soc_port_t port, uint32 flags,
                   uint32 phy_reg_addr, uint32 phy_reg_data)
{
    phy_ctrl_t *pc;
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t *pm_phy;
    int idx;
    uint32 data = 0;

    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pmc = &pc->phymod_ctrl;
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;
        if (flags & SOC_PHY_I2C_DATA8) {
            data = phy_reg_data & 0xFF;
            SOC_IF_ERROR_RETURN
                (phymod_phy_i2c_write(pm_phy, 0, SOC_PHY_I2C_DEVAD(phy_reg_addr), SOC_PHY_I2C_REGAD(phy_reg_addr), 1, (uint8 *)&data));
        } else if (flags & SOC_PHY_I2C_DATA16) {
            data = phy_reg_data & 0xFFFF;
            SOC_IF_ERROR_RETURN
                (phymod_phy_i2c_write(pm_phy, 0, SOC_PHY_I2C_DEVAD(phy_reg_addr), SOC_PHY_I2C_REGAD(phy_reg_addr), 2, (uint8 *)&data));
        } else if (flags & SOC_PHY_PVT_DATA) {
            SOC_IF_ERROR_RETURN
                (phymod_phy_private_write(pm_phy, phy_reg_addr, phy_reg_data));
        } else {
            SOC_IF_ERROR_RETURN
                (phymod_phy_reg_write(pm_phy, phy_reg_addr, phy_reg_data));
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8806x_reg_modify
 * Purpose:
 *      Routine to write PHY register
 * Parameters:
 *      uint         - BCM unit number
 *      pc           - PHY state
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_mo_data  - New value for the bits specified in phy_mo_mask
 *      phy_mo_mask  - Bit mask to modify
 * Note:
 *      This function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
STATIC int
phy_8806x_reg_modify(int unit, soc_port_t port, uint32 flags,
                    uint32 phy_reg_addr, uint32 phy_data,
                    uint32 phy_data_mask)
{
    phy_ctrl_t *pc;
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t *pm_phy;
    uint32 data32, tmp;
    int idx;

    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pmc = &pc->phymod_ctrl;
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;
        SOC_IF_ERROR_RETURN
            (phymod_phy_reg_read(pm_phy, phy_reg_addr, &data32));
        tmp = data32;
        data32 &= ~(phy_data_mask);
        data32 |= (phy_data & phy_data_mask);
        if (data32 != tmp) {
            SOC_IF_ERROR_RETURN
                (phymod_phy_reg_write(pm_phy, phy_reg_addr, data32));
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_8806x_multi_reg_read
 * Purpose:
 *      Routine to read PHY I2C data
 * Parameters:
 *      unit         - BCM unit number
 *      port         - Port number
 *      flags        - Flags which specify the register type
 *      dev_addr     - device address
 *      offset       - IC map offset
 *      maxsize      - size of buffer provided to read
 *      data         - (OUT) Value read from PHY register
 *      actual_size  - (OUT) actual amount of data read
 * Note:
 *      This register read function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
STATIC int
phy_8806x_multi_reg_read(int unit, soc_port_t port, uint32 flags,
               uint32 dev_addr, uint32 offset, int max_size, uint8 *data, int *actual_size)
{
    phy_ctrl_t *pc;
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t *pm_phy;
    int idx=0;

    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pmc = &pc->phymod_ctrl;
    pm_phy = &pmc->phy[idx]->pm_phy;

    /* Endian unaware byte stream Returned */
    *actual_size = 0;
    SOC_IF_ERROR_RETURN
        (phymod_phy_i2c_read(pm_phy, 0, dev_addr, offset, max_size, (uint8 *)data));
    *actual_size = max_size;
    return SOC_E_NONE;
}

STATIC void
phy_8806x_cleanup(soc_phymod_ctrl_t *pmc)
{
    int idx;
    soc_phymod_phy_t *phy;
    soc_phymod_core_t *core;

    for (idx = 0; idx < pmc->num_phys ; idx++) {
        phy = pmc->phy[idx];
        if (phy == NULL) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(pmc->unit,
                                 "phy object is empty")));
            continue;
        }

        core = phy->core;

        /* Destroy core object if not used anymore */
        if (core && core->ref_cnt) {
            if (--core->ref_cnt == 0) {
                soc_phymod_core_destroy(pmc->unit, core);
            }
        }

        /* Destroy phy object */
        if (phy) {
            soc_phymod_phy_destroy(pmc->unit, phy);
        }
    }
    pmc->num_phys = 0;
}

STATIC void
phy_8806x_core_init(phy_ctrl_t *pc, soc_phymod_core_t *core,
                   phymod_bus_t *core_bus, uint32 core_addr)
{
    phymod_core_access_t *pm_core;
    phymod_access_t *pm_acc;

    core->unit = pc->unit;
    core->port = pc->port;
    core->read = pc->read;
    core->write = pc->write;
    core->wrmask = pc->wrmask;

    pm_core = &core->pm_core;
    phymod_core_access_t_init(pm_core);
    pm_acc = &pm_core->access;
    phymod_access_t_init(pm_acc);
    PHYMOD_ACC_USER_ACC(pm_acc) = core;
    PHYMOD_ACC_BUS(pm_acc) = core_bus;
    PHYMOD_ACC_ADDR(pm_acc) = core_addr;
    PHYMOD_ACC_EXT_ACC(pm_acc) = _8806x_xmod_command;

    if (soc_property_port_get(pc->unit, pc->port, "phy8806x_sim", 0) == 45) {
        PHYMOD_ACC_F_CLAUSE45_SET(pm_acc);
    }

    return;
}

/*
 * Function:
 *      phy_8806x_probe
 * Purpose:
 *      xx
 * Parameters:
 *      pc->phy_id   (IN)
 *      pc->unit     (IN)
 *      pc->port     (IN)
 *      pc->size     (OUT) - memory required by phy port
 *      pc->dev_name (OUT) - set to port device name
 *  
 * Note:
 */
STATIC int
phy_8806x_probe(int unit, phy_ctrl_t *pc)
{
    int rv, idx;
    uint32 lane_map, num_phys, core_id, phy_id /*, found */;
    uint32 top_dev_rev_id;
    phymod_bus_t core_bus;
    phymod_dispatch_type_t phy_type;
    phymod_core_access_t *pm_core;
    phymod_phy_access_t *pm_phy;
    phymod_access_t *pm_acc;
    soc_phymod_ctrl_t *pmc;
    soc_phymod_phy_t *phy;
    soc_phymod_core_t *core;
    soc_phymod_core_t core_probe;
    soc_info_t *si;
    phyident_core_info_t core_info[8];  
    int array_max = 8;
    int array_size = 0;
    int port;
    int phy_port;  /* physical port number */

    if (!(SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit))) {
        /* Release the top level resets before the AXI reads. */
        SOC_IF_ERROR_RETURN
            (WRITE_TOP_SOFT_RESET_REG(unit, pc, 0xc000003f));
    }

    SOC_IF_ERROR_RETURN
        (READ_TOP_DEV_REV_ID_REG(unit, pc, &top_dev_rev_id));

    switch (top_dev_rev_id & 0xff) {
        case 0x60:
            pc->dev_name = "BCM88060";
            break;

        case 0x61:
            pc->dev_name = "BCM88061";
            break;

        case 0x64:
            pc->dev_name = "BCM88064";
            break;

        case 0x68:
            pc->dev_name = "BCM88068";
            break;

        default:
            break;
    }

    /* Initialize PHY bus */
    SOC_IF_ERROR_RETURN(phymod_bus_t_init(&core_bus));
    core_bus.bus_name = "phy8806x"; 
    core_bus.read = _8806x_reg_read; 
    core_bus.write = _8806x_reg_write;

    /* Configure PHY bus properties */
    PHYMOD_BUS_CAP_WR_MODIFY_SET(&core_bus);
    PHYMOD_BUS_CAP_LANE_CTRL_SET(&core_bus);

    port = pc->port;
    pmc = &pc->phymod_ctrl;
    si = &SOC_INFO(unit);
    if (soc_feature(unit, soc_feature_logical_port_num)) {
        phy_port = si->port_l2p_mapping[port];
    } else {
        phy_port = port;
    }

    /* Install clean up function */
    pmc->unit = pc->unit;
    pmc->cleanup = phy_8806x_cleanup;
    #if 0
    pmc->symbols = NULL; /*&bcmi_8806x_xgxs_symbols;*/
    #endif

    
    pc->lane_num = SOC_PORT_BINDEX(unit, phy_port);
    pc->chip_num = SOC_BLOCK_NUMBER(unit, SOC_PORT_BLOCK(unit, phy_port));

    /* request memory for the configuration structure */
    pc->size = sizeof(phy8806x_config_t);

    /* Bit N corresponds to lane N in lane_map */
    lane_map = 0xf;
    num_phys = 1;
    switch (si->port_num_lanes[port]) {
    case 4:
        pc->phy_mode = PHYCTRL_QUAD_LANE_PORT; 
        break;
    case 2:
        lane_map = 0x3;
        pc->phy_mode = PHYCTRL_DUAL_LANE_PORT;
        break;
    case 1:
    case 0:
        lane_map = 0x1;
        pc->phy_mode = PHYCTRL_ONE_LANE_PORT;
        break;
    default:
        return SOC_E_CONFIG;
    }
    lane_map <<= pc->lane_num;

    /* we need to get the other core id if more than 1 core per port */
    if (num_phys > 1) {
        /* get the other core address */
        SOC_IF_ERROR_RETURN
            (soc_phy_addr_multi_get(unit, port, array_max, &array_size, &core_info[0]));
    } else {
        core_info[0].mdio_addr = pc->phy_id;
    }

    phy_type = phymodDispatchTypePhy8806x;   /* Change to XMOD SRINATH */

    /* Probe cores */
    for (idx = 0; idx < num_phys ; idx++) {
        phy_8806x_core_init(pc, &core_probe, &core_bus,
                           core_info[idx].mdio_addr);
        pm_core = &core_probe.pm_core;
        pm_core->type = phy_type;
    }

    rv = SOC_E_NONE;
    for (idx = 0; idx < num_phys ; idx++) {
        /* Set core and phy IDs based on PHY address */
        core_id = pc->phy_id + idx;
        phy_id = (lane_map << 16) | core_id;

        /* Create PHY object */
        rv = soc_phymod_phy_create(unit, phy_id, &pmc->phy[idx]);
        if (SOC_FAILURE(rv)) {
            break;
        }
        pmc->num_phys++;

        /* Initialize phy object */
        phy = pmc->phy[idx];
        pm_phy = &phy->pm_phy;
        phymod_phy_access_t_init(pm_phy);

        /* Find or create associated core object */
        rv = soc_phymod_core_find_by_id(unit, core_id, &phy->core);
        if (rv == SOC_E_NOT_FOUND) {
            rv = soc_phymod_core_create(unit, core_id, &phy->core);
        }
        if (SOC_FAILURE(rv)) {
            break;
        }        
    }
    if (SOC_FAILURE(rv)) {
        phy_8806x_cleanup(pmc);
        return rv;
    }

    for (idx = 0; idx < pmc->num_phys ; idx++) {
        phy = pmc->phy[idx];
        core = phy->core;
        pm_core = &core->pm_core;

        /* Initialize core object if newly created */
        if (core->ref_cnt == 0) {
            sal_memcpy(&core->pm_bus, &core_bus, sizeof(core->pm_bus));
            phy_8806x_core_init(pc, core, &core->pm_bus,
                               core_info[idx].mdio_addr);
            /* Set dispatch type */
            pm_core->type = phy_type;
        }
        core->ref_cnt++;        

        /* Initialize phy access based on associated core */
        pm_acc = &phy->pm_phy.access;
        sal_memcpy(pm_acc, &pm_core->access, sizeof(*pm_acc));
        phy->pm_phy.type = phy_type;
        PHYMOD_ACC_LANE_MASK(pm_acc) = lane_map;
    }

    return SOC_E_NONE;
}


/***********************************************************************
 *
 * PASS-THROUGH NOTIFY ROUTINES
 */

/*
 * Function:
 *      _8806x_notify_duplex
 * Purpose:
 *      Program duplex if (and only if) serdesphy8806x is an intermediate PHY.
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number.
 *      duplex - Boolean, TRUE indicates full duplex, FALSE
 *              indicates half.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      If PHY_FLAGS_FIBER is set, it indicates the PHY is being used to
 *      talk directly to an external fiber module.
 *
 *      If PHY_FLAGS_FIBER is clear, the PHY is being used in
 *      pass-through mode to talk to an external SGMII PHY.
 *
 *      When used in pass-through mode, autoneg must be turned off and
 *      the speed/duplex forced to match that of the external PHY.
 */


/*
 * Function:
 *      _8806x_stop
 * Purpose:
 *      Put serdesphy8806x SERDES in or out of reset depending on conditions
 */

STATIC int
_8806x_stop(int unit, soc_port_t port)
{
    phy_ctrl_t* pc; 
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t *pm_phy;
    phymod_phy_power_t phy_power;
    int  stop, copper, speed;

    pc = EXT_PHY_SW_STATE(unit, port);
    pmc = &pc->phymod_ctrl;
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pc->phy_mode != PHYCTRL_ONE_LANE_PORT) {
        return SOC_E_NONE;
    }

    /* 'Stop' only for speeds < 10G. */ 
    SOC_IF_ERROR_RETURN
        (phy_8806x_speed_get(unit,port,&speed));
    if(10000 <= speed) {
        return SOC_E_NONE;
    }
    copper = (pc->stop &
              PHY_STOP_COPPER) != 0;

    stop = ((pc->stop &
             (PHY_STOP_PHY_DIS |
              PHY_STOP_DRAIN)) != 0 ||
            (copper &&
             (pc->stop &
              (PHY_STOP_MAC_DIS |
               PHY_STOP_DUPLEX_CHG |
               PHY_STOP_SPEED_CHG)) != 0));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(pc->unit,
                         "qsgmiie_stop: u=%d p=%d copper=%d stop=%d flg=0x%x\n"),
              unit, port, copper, stop,
              pc->stop));
    if (stop) {
        phy_power.tx = phymodPowerOff;
        phy_power.rx = phymodPowerOff;
    } else {
        phy_power.tx = phymodPowerOn;
        phy_power.rx = phymodPowerOn;
    }
    SOC_IF_ERROR_RETURN
        (phymod_phy_power_set(pm_phy, &phy_power));
    return SOC_E_NONE;
}

/*
 * Function:
 *      _8806x_notify_stop
 * Purpose:
 *      Add a reason to put serdesphy8806x PHY in reset.
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number.
 *      flags - Reason to stop
 * Returns:     
 *      SOC_E_XXX
 */

int
_8806x_notify_stop(int unit, soc_port_t port, uint32 flags)
{
    EXT_PHY_SW_STATE(unit, port)->stop |= flags;
    return _8806x_stop(unit, port);
}

/*  
 * Function:
 *      _8806x_notify_resume
 * Purpose:
 *      Remove a reason to put serdesphy8806x PHY in reset.
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number.
 *      flags - Reason to stop
 * Returns:     
 *      SOC_E_XXX
 */

int
_8806x_notify_resume(int unit, soc_port_t port, uint32 flags)
{   
    EXT_PHY_SW_STATE(unit, port)->stop &= ~flags;
    return _8806x_stop(unit, port);
}

/*
 * Function:
 *      phy_8806x_diag_ctrl
 * Purpose:
 *      xx
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 

 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_8806x_diag_ctrl(
    int unit,        /* unit */
    soc_port_t port, /* port */
    uint32 inst,     /* the specific device block the control action directs to */
    int op_type,     /* operation types: read,write or command sequence */
    int op_cmd,      /* command code */
    void *arg)       /* command argument based on op_type/op_cmd */
{
    int32 intf, prev_intf;
    phy_ctrl_t          *pc;
    int rv = SOC_E_NONE;

    /* locate phy control, phymod control, and the configuration data */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    /* Temporarily Set Interface type in PC flags, for the duration of this diag */

    intf = PHY_DIAG_INST_INTF(inst);
    if (intf == PHY_DIAG_INTF_DFLT) {
        intf = PHY_DIAG_INTF_LINE;
    }

    prev_intf = (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? 1 : 0;

    if (intf == PHY_DIAG_INTF_SYS) {
        pc->flags |= PHYCTRL_SYS_SIDE_CTRL;
    } else {
        pc->flags &= ~PHYCTRL_SYS_SIDE_CTRL;
    }

    switch(op_cmd) {
        case PHY_DIAG_CTRL_DSC:
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "phy_mt2_diag_ctrl: "
                                 "u=%d p=%d PHY_DIAG_CTRL_DSC 0x%x\n"),
                                  unit, port, PHY_DIAG_CTRL_DSC));
            rv = phy8806x_uc_status_dump(unit, port, arg);
            break;
        case PHY_DIAG_CTRL_STATE_GENERIC:
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "phy_mt2_diag_ctrl: "
                                 "u=%d p=%d PHY_DIAG_CTRL_STATE_GENERIC 0x%x\n"),
                                  unit, port, PHY_DIAG_CTRL_STATE_GENERIC));
            rv = _phy8806x_dump_buffer(unit, port, (unsigned char *)arg);
            break;
        default:
            if (op_type == PHY_DIAG_CTRL_SET) {
                rv = phy_8806x_control_set(unit,port,op_cmd,PTR_TO_INT(arg));
            } else if (op_type == PHY_DIAG_CTRL_GET) {
                rv = phy_8806x_control_get(unit,port,op_cmd,(uint32 *)arg);
            }
            break ;
    }

    if (prev_intf) {
        pc->flags |= PHYCTRL_SYS_SIDE_CTRL;
    } else {
        pc->flags &= ~PHYCTRL_SYS_SIDE_CTRL;
    }

    SOC_IF_ERROR_RETURN(rv);
    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_8806x_linkup
 * Purpose:
 *      Link up handler
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 * Returns:
 *      SOC_E_NONE
 */
STATIC int 
phy_8806x_linkup(int unit, soc_port_t port)
{
    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_8806x_linkfault_get
 * Purpose:
 *    Get layer2 connection status.
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *    fault - address of memory to store link fault state.
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int
phy_8806x_linkfault_get(int unit, soc_port_t port, int *fault)
{
    int        rv = SOC_E_NONE;
    phy_ctrl_t *pc;
#if defined(INCLUDE_FCMAP)
    uint16     fault_st;
#endif
    uint16     reg_val;

    pc    = EXT_PHY_SW_STATE(unit, port);

    *fault = FALSE;      /* Default return */

#if defined(INCLUDE_FCMAP)
    if (pc->fcmap_enable) {
        rv = READ_FC_LINK_FAULT_REG(unit, pc, &fault_st);
        *fault = (fault_st & VS_MISC_STATUS_STATUS_BIT_MASK) ? TRUE: FALSE;
        return(rv); 
    }
#endif

    rv = READ_ETH_LOCAL_FAULT_REG(unit, pc, &reg_val);
    if (reg_val & VS_MISC_STATUS_LIVE_STATUS_BIT_MASK) {
        *fault |= SOC_PORT_FAULT_LOCAL;
    }

    rv = READ_ETH_REMOTE_FAULT_REG(unit, pc, &reg_val);
    if (reg_val & VS_MISC_STATUS_LIVE_STATUS_BIT_MASK) {
        *fault |= SOC_PORT_FAULT_REMOTE;
    }

    return (rv);
}

/* Utility functions */

STATIC int
_phy_8806x_write_to_arm(int unit, phy_ctrl_t *pc, uint32 addr, uint8 *data,int len)
{
    int n_writes, i;

#ifdef PHY8806X_MEASSURE_DOWNLOAD_TIME
    sal_usecs_t t;
    t = sal_time_usecs();
#endif

    n_writes = (len + WRITE_BLK_SIZE -1) / WRITE_BLK_SIZE;

    for ( i = 0; i < n_writes; i++ ) {

        /* address frames are ignored in the download mode */
        SOC_IF_ERROR_RETURN
            (PHY8806X_REG_WRITE(unit, pc, 
                SOC_PHY_CLAUSE45_ADDR(((1U << 3) | 0U), (((uint16)data[0])<<8)|data[1]),
                (((uint16)data[2])<<8)|data[3]) );

        data += WRITE_BLK_SIZE;

    }
 
#ifdef PHY8806X_MEASSURE_DOWNLOAD_TIME
    LOG_CLI((BSL_META_U(unit,
                        "u=%d p=%d raw download took %u us\n"), unit, pc->port, SAL_USECS_SUB(sal_time_usecs(), t)));
#endif

    return (SOC_E_NONE);
}


int
phy_8806x_sbus_cmd(int unit,phy_ctrl_t *pc, uint32 reg, uint32 cmd, int ring,  uint32 arg[])
{
    int    count, rv, write, size;
    uint16 status, temp0, temp1;

    size = ((cmd >> 7) & 0x7f) >> 2;

    if ((size < 1) || (size > 4)) {
        return (SOC_E_PARAM);
    }


    if (ring > 0x2) {
        LOG_CLI((BSL_META_U(unit,
                        "phy_8806x_sbus_reg_cmd: u=%d p=%d  : invalid ring !."
                        " \n"), unit, pc->port));
        return (SOC_E_PARAM);
    }

    ring &= 0x3;

    /* LOG_CLI((BSL_META_U(unit,
                        "phy_8806x_sbus_reg_cmd: u=%d p=%d  cmd_reg=%08x"
                        " \n"), unit, pc->port, cmd)); */

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
        LOG_CLI((BSL_META_U(unit,
                            "phy_8806x_sbus_reg_cmd failed: u=%d p=%d  status=%04x"
                            "SCHAN_MAX_POLLS reached !.\n"), unit, pc->port, status));
        return (SOC_E_FAIL);
    }

    if ((status & (SBUS_STATUS_ERR | SBUS_STATUS_NAK)) != 0) {
        LOG_CLI((BSL_META_U(unit,
                            "phy_8806x_sbus_reg_cmd failed: u=%d p=%d "
                            "cmd=0x%04x reg=0x%08x ring=%d d[0]=0x%08x d[1]=0x%08x d[2]=0x%08x d[3]=0x%08x status=%04x\n"), unit, pc->port, cmd, reg, ring, arg[0], arg[1], arg[2], arg[3], status));
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

STATIC int _pmd_enable(int unit, phy_ctrl_t *pc, int sys)
{
    uint32 data[4], reg_cmd, reg_cmd_1, mem_cmd  /* , reg_rcmd, reg_rcmd_1 */;
    int ring, rv;
    uint16 status = 0;
 
    ring = (pc->phy_id & 0x4) ? 1 : 0; /* mig too */

    reg_cmd   = (SBUS_WRITE_REGISTER_CMD_MSG << 26) |(((sys ? 0x6 : 0x2) & 0x7f) << 19) |(4 << 7);
    reg_cmd_1 = (SBUS_WRITE_REGISTER_CMD_MSG << 26) |(((sys ? 0x7 : 0x3) & 0x7f) << 19) |(4 << 7);
    mem_cmd   = (SBUS_WRITE_MEMORY_CMD_MSG   << 26) |(((sys ? 0x7 : 0x3) & 0x7f) << 19) |(16 << 7);

    /*
    reg_rcmd   = (SBUS_READ_REGISTER_CMD_MSG << 26) |(((sys ? 0x6 : 0x2) & 0x7f) << 19) |(4 << 7);
    reg_rcmd_1 = (SBUS_READ_REGISTER_CMD_MSG << 26) |(((sys ? 0x7 : 0x3) & 0x7f) << 19) |(4 << 7);
     */

#if defined(INCLUDE_FCMAP)
    if (pc->fcmap_enable && !sys) {
        /* FCPORT_PLL0_CTRL_CONFIGr = 0x3 */
        data[0] = 0x3;
        SOC_IF_ERROR_RETURN
            (phy_8806x_sbus_cmd(unit, pc, 0x02000f00, reg_cmd_1, ring,  data));

        /*
        SOC_IF_ERROR_RETURN
            (phy_8806x_sbus_cmd(unit, pc, 0x02000f00, reg_rcmd_1, ring,  data));
         */

        /* FCPORT_PLL1_CTRL_CONFIGr = 0x1 */
        data[0] = 0x1;
        SOC_IF_ERROR_RETURN
            (phy_8806x_sbus_cmd(unit, pc, 0x02001000, reg_cmd_1, ring,  data));
        /*
        SOC_IF_ERROR_RETURN
            (phy_8806x_sbus_cmd(unit, pc, 0x02001000, reg_rcmd_1, ring,  data));
         */
    }
#endif

    /* CLPORT_POWER_SAVE = 0 */
    data[0] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x02020D00, reg_cmd, ring,  data));

    /* CLPORT_XGXS0_CTRL_REG = 0x1C */
    data[0] = 0x1C;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x02021400, reg_cmd, ring,  data));

    /* CLPORT_XGXS0_CTRL_REG = 0x1E */
    data[0] = 0x1e;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x02021400, reg_cmd, ring,  data));

    /* CLPORT_XGXS0_CTRL_REG = 0x0E */
    data[0] = 0x0e;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x02021400, reg_cmd, ring,  data));

    sal_udelay(10000);
    /* CLPORT_XGXS0_CTRL_REG = 0x06 */
    data[0] = 0x06;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x02021400, reg_cmd, ring,  data));

    sal_udelay(10000);
    /* CLPORT_XGXS0_CTRL_REG = fc ? 0x7 : 0x5 */
    data[0] = 0x5;
#if defined(INCLUDE_FCMAP)
    if (pc->fcmap_enable) {
        data[0] = 0x7;
    }
#endif
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x02021400, reg_cmd, ring,  data));

    /* CLPORT_POWER_SAVE = 0 */

    /* FCPORT_TOP_CTRL_CONFIG = 0x10 */
    data[0] = 0x10;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x02000000, reg_cmd_1, ring,  data));

    /* PMD_X1_CONTROL.[1:0] = 0x3 */
    data[0] = PORT_ADDRESS(0x00009010, sys, ring);
    data[1] = 0x0003fffc;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));

    /*  RESET_CONTROL_PMD.0 = 0x0 core_s_rstb = 0x0  */ 
    data[0] = PORT_ADDRESS(0x0800d101, sys, ring);
    data[1] = 0x0000fffe;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    /*  RESET_CONTROL_PMD.0 = 0x1 core_s_rstb = 0x1  */ 
    data[0] = PORT_ADDRESS(0x0800d101, sys, ring);
    data[1] = 0x0001fffe;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    /* PMD_X4_CONTROL.[1:0] = 0x3 */
    data[0] = PORT_ADDRESS(0x0000c010, sys, ring);
    data[1] = 0x0003fffc;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));

/* new additions */ 

    /* MICRO_A_COM_CLOCK_CONTROL0.1 = 0x0, micro_core_clk_en = 0 */ 
    data[0] = PORT_ADDRESS(0x0800d200, sys, ring);
    data[1] = 0x0000fffd;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));

    /* MICRO_A_COM_CLOCK_CONTROL0.0 = 0x0, micro_master_clk_en = 0 */ 
    data[0] = PORT_ADDRESS(0x0800d200, sys, ring);
    data[1] = 0x0000fffe;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    /* MICRO_C_ram_control0.1 = 1, micro_protect_fwcode = 1 */
    data[0] = PORT_ADDRESS(0x0800d225, sys, ring);
    data[1] = 0x0002fffd;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    data[0] = PORT_ADDRESS(0x0800d200, sys, ring);
    data[1] = 0x00000000;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));

    data[0] = PORT_ADDRESS(0x0800d201, sys, ring);
    data[1] = 0x00000000;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));

    data[0] = PORT_ADDRESS(0x0800d202, sys, ring);
    data[1] = 0x00000000;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    data[0] = PORT_ADDRESS(0x0800d203, sys, ring);
    data[1] = 0x00000000;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    data[0] = PORT_ADDRESS(0x0800d204, sys, ring);
    data[1] = 0x00000000;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    data[0] = PORT_ADDRESS(0x0800d205, sys, ring);
    data[1] = 0x00000000;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    data[0] = PORT_ADDRESS(0x0800d206, sys, ring);
    data[1] = 0x00000000;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    data[0] = PORT_ADDRESS(0x0800d207, sys, ring);
    data[1] = 0x00000000;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    data[0] = PORT_ADDRESS(0x0800d208, sys, ring);
    data[1] = 0x00000000;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    data[0] = PORT_ADDRESS(0x0800d209, sys, ring);
    data[1] = 0x00000000;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    data[0] = PORT_ADDRESS(0x0800d20a, sys, ring);
    data[1] = 0x00000000;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    data[0] = PORT_ADDRESS(0x0800d20b, sys, ring);
    data[1] = 0x00000000;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    data[0] = PORT_ADDRESS(0x0800d20c, sys, ring);
    data[1] = 0x00000000;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    data[0] = PORT_ADDRESS(0x0800d20d, sys, ring);
    data[1] = 0x00000000;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    data[0] = PORT_ADDRESS(0x0800d20e, sys, ring);
    data[1] = 0x00000000;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    data[0] = PORT_ADDRESS(0x0800d20f, sys, ring);
    data[1] = 0x00000000;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    data[0] = PORT_ADDRESS(0x0800d210, sys, ring);
    data[1] = 0x00000000;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    data[0] = PORT_ADDRESS(0x0800d211, sys, ring);
    data[1] = 0x00000000;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    data[0] = PORT_ADDRESS(0x0800d212, sys, ring);
    data[1] = 0x00000000;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    data[0] = PORT_ADDRESS(0x0800d213, sys, ring);
    data[1] = 0x00000000;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    data[0] = PORT_ADDRESS(0x0800d214, sys, ring);
    data[1] = 0x00000000;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    data[0] = PORT_ADDRESS(0x0800d215, sys, ring);
    data[1] = 0x00000000;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    data[0] = PORT_ADDRESS(0x0800d216, sys, ring);
    data[1] = 0x00070000;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    data[0] = PORT_ADDRESS(0x0800d217, sys, ring);
    data[1] = 0x00000000;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    data[0] = PORT_ADDRESS(0x0800d218, sys, ring);
    data[1] = 0x00000000;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    data[0] = PORT_ADDRESS(0x0800d219, sys, ring);
    data[1] = 0x00000000;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    data[0] = PORT_ADDRESS(0x0800d21a, sys, ring);
    data[1] = 0x00000000;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    data[0] = PORT_ADDRESS(0x0800d21b, sys, ring);
    data[1] = 0x00000000;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    data[0] = PORT_ADDRESS(0x0800d220, sys, ring);
    data[1] = 0x00000000;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    data[0] = PORT_ADDRESS(0x0800d221, sys, ring);
    data[1] = 0x00000000;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    data[0] = PORT_ADDRESS(0x0800d224, sys, ring);
    data[1] = 0x00000000;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    data[0] = PORT_ADDRESS(0x0800d226, sys, ring);
    data[1] = 0x00000000;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    data[0] = PORT_ADDRESS(0x0800d225, sys, ring);
    data[1] = 0x88030000;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    data[0] = PORT_ADDRESS(0x0800d228, sys, ring);
    data[1] = 0x01010000;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    data[0] = PORT_ADDRESS(0x0800d229, sys, ring);
    data[1] = 0x00000000;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));



    /* MICRO_C_ram_control0.1 = 1, micro_protect_fwcode = 0 */
    data[0] = PORT_ADDRESS(0x0800d225, sys, ring);
    data[1] = 0x0000fffd;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));

/* end of new additions */ 

#if 0
    /* CKRST_CTRL_PMD_LANE_RESET_N_PWRDN_PIN_KILL_CONTROL.0 = 0x1 */ 
    data[0] = PORT_ADDRESS(0x0800d0b3, sys, ring);
    data[1] = 0x0001fffe;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));
#endif

    /* MICRO_A_COM_CLOCK_CONTROL0.0 = 0x1 micro_master_clk_en = 0x1 */ 
    data[0] = PORT_ADDRESS(0x0800d200, sys, ring);
    data[1] = 0x0001fffe;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));

    /* MICRO_A_COM_RESET_CONTROL0.0 = 0x0, MICRO_A_COM_RESET_CONTROL0.3 = 0x0, micro_pram_if_rstb=0 micro_master_rstb= 0x0  */
    data[0] = PORT_ADDRESS(0x0800d201, sys, ring);
    data[1] = 0x0000fff6;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));

    /* MICRO_A_COM_RESET_CONTROL0.0 = 0x1, MICRO_A_COM_RESET_CONTROL0.3 = 0x1, micro_pram_if_rstb=1 micro_master_rstb= 0x1  */ 
    data[0] = PORT_ADDRESS(0x0800d201, sys, ring);
    data[1] = 0x0009fff6;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));

    if (!(FLAGS(pc) & (PHY8806X_MODE_CHANGE | PHY8806X_FW_RESTART))) {
        /* MICRO_A_COM_AHB_CONTROL0.[9:8] = 0x1 */ 
        data[0] = PORT_ADDRESS(0x0800d202, sys, ring);
        data[1] = 0x0100fcff;
        data[2] = 0x1;
        data[3] = 0;
        SOC_IF_ERROR_RETURN
            (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));

        /* tsc_reg_read_test_base(unit, pc->port, sys, 1, 0, 0xd203); */

        /* sal_udelay(250000); */

       soc_timeout_init(&TO(pc), 250000, 0); /* 250ms timer */

        do {

            rv = _tsc_reg_read(unit, pc, 0, (pc->phy_id & 0x7) +  (sys ? 9 : 1), 1, 0, 0xd203, &status);
            if (SOC_FAILURE(rv)) {
                break;
            }
                if ((status & 0x1) == 0x1) {
                    break;
                }
        } while (!soc_timeout_check(&TO(pc)));

        if ((status & 0x1) != 0x1) {
            LOG_CLI((BSL_META_U(unit,
                    "PHY8806X PMD (%s) failed to clear code RAM: u=%d p=%d  status=%04x"
                    " !.\n"), sys ? "sys" : "line", unit, pc->port, status));
        }

        /* MICRO_A_COM_AHB_CONTROL0.[9:8] = 0x0 */ 
        data[0] = PORT_ADDRESS(0x0800d202, sys, ring);
        data[1] = 0x0000fcff;
        data[2] = 0x1;
        data[3] = 0;
        SOC_IF_ERROR_RETURN
            (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));

        /* MICRO_A_COM_AHB_CONTROL0.[1:0] = 0x2, micro_ra_wrdatasize = 0x2 (32-bit) */ 
        data[0] = PORT_ADDRESS(0x0800d202, sys, ring);
        data[1] = 0x0002fffc;
        data[2] = 0x1;
        data[3] = 0;
        SOC_IF_ERROR_RETURN
            (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));
    }

    /* MICRO_A_COM_PRAMIF_CONTROL0.0 = 0x1 , micro_pramif_en = 1 */
    data[0] = PORT_ADDRESS(0x0800d20c, sys, ring);
    data[1] = 0x0001fffe;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));

    if (FLAGS(pc) & (PHY8806X_MODE_CHANGE | PHY8806X_FW_RESTART)) {
        /* No need to set UCMEM_ACCESS_CONTROL=1 if no  f/w is downloaded */
        return (SOC_E_NONE);
    }

    /* FCPORT_TOP_CTRL_CONFIG = 0x11 */
    data[0] = 0x11;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x02000000, reg_cmd_1, ring,  data));

    return (SOC_E_NONE);
}

STATIC int _pmd_disable(int unit, phy_ctrl_t *pc, int sys)
{
    uint32 data[4], /* reg_cmd, */ reg_cmd_1, mem_cmd;
    int ring, rv;
    uint16 status = 0;
 
    ring = (pc->phy_id & 0x4) ? 1 : 0; /* mig too */

    /* reg_cmd   = (SBUS_WRITE_REGISTER_CMD_MSG << 26) |(((sys ? 0x6 : 0x2) & 0x7f) << 19) |(4 << 7); */
    reg_cmd_1 = (SBUS_WRITE_REGISTER_CMD_MSG << 26) |(((sys ? 0x7 : 0x3) & 0x7f) << 19) |(4 << 7);
    mem_cmd   = (SBUS_WRITE_MEMORY_CMD_MSG   << 26) |(((sys ? 0x7 : 0x3) & 0x7f) << 19) |(16 << 7);

    if (!(FLAGS(pc) & (PHY8806X_MODE_CHANGE | PHY8806X_FW_RESTART))) {
        /* FCPORT_TOP_CTRL_CONFIG = 0x10 */
        data[0] = 0x10;
        SOC_IF_ERROR_RETURN
            (phy_8806x_sbus_cmd(unit, pc, 0x02000000, reg_cmd_1, ring,  data));
    }

    /* MICRO_A_COM_AHB_CONTROL0.[9:8] = 0x2, clear data ram */ 
    data[0] = PORT_ADDRESS(0x0800d202, sys, ring);
    data[1] = 0x0200fcff;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));

    /* tsc_reg_read_test_base(unit, pc->port, sys, 1, 0, 0xd203); */

    /* sal_udelay(250000); */

    soc_timeout_init(&TO(pc), 250000, 0); /* 250ms timer */

    do {
           rv = _tsc_reg_read(unit, pc, 0, (pc->phy_id & 0x7) +  (sys ? 9 : 1), 1, 0, 0xd203, &status);
           if (SOC_FAILURE(rv)) {
               break;
           }
           if ((status & 0x1) == 0x1) {
                break;
           }
    } while (!soc_timeout_check(&TO(pc)));

    if ((status & 0x1) != 0x1) {
        LOG_CLI((BSL_META_U(unit,
                "PHY8806X PMD (%s) failed to clear data RAM: u=%d p=%d  status=%04x"
                " !.\n"), sys ? "sys" : "line", unit, pc->port, status));
    }

    /* MICRO_A_COM_AHB_CONTROL0.[9:8] = 0x0 */ 
    data[0] = PORT_ADDRESS(0x0800d202, sys, ring);
    data[1] = 0x0000fcff;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    /* CKRST_CTRL_PMD_LANE_RESET_N_PWRDN_PIN_KILL_CONTROL.0 = 0x1 */ 
    data[0] = PORT_ADDRESS(0x0800d0b3, sys, ring);
    data[1] = 0x0001fffe;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));

    return (SOC_E_NONE);
}

STATIC int _pmd_start(int unit, phy_ctrl_t *pc, int sys)
{
    uint32 data[4], mem_cmd;
    int ring;

    ring = (pc->phy_id & 0x4) ? 1 : 0; /* mig too */

    mem_cmd = (SBUS_WRITE_MEMORY_CMD_MSG   << 26) |(((sys ? 0x7 : 0x3) & 0x7f) << 19) |(16 << 7);

    /* TOP_USER_CONTROL_0.15 = 0x0 */ 
    data[0] = PORT_ADDRESS(0x0800d104, sys, ring);
    data[1] = 0x00007fff;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    /* TOP_USER_CONTROL_0.15 = 0x1 */ 
    data[0] = PORT_ADDRESS(0x0800d104, sys, ring);
    data[1] = 0x80007fff;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));


    /* CKRST_CTRL_PMD_LANE_RESET_N_PWRDN_PIN_KILL_CONTROL.0 = 0x0 */ 
    data[0] = PORT_ADDRESS(0x0800d0b3, sys, ring);
    data[1] = 0x0000fffe;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));

    /* MICRO_A_COM_CLOCK_CONTROL0.1 = 0x1 */ 
    data[0] = PORT_ADDRESS(0x0800d200, sys, ring);
    data[1] = 0x0002fffd;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));

    /* MICRO_A_COM_RESET_CONTROL0.1 = 0x1 */ 
    data[0] = PORT_ADDRESS(0x0800d201, sys, ring);
    data[1] = 0x0002fffd;
    data[2] = 0x1;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));

    return (SOC_E_NONE);
}


/* _phy_8806x_toplvl_sbus_{read|write}() need to be optimized when the data only transactions are
   implemented in the CMIC MDIO code.
 */
 
int
_phy_8806x_toplvl_sbus_write(int unit,phy_ctrl_t *pc, uint32 reg, uint32 arg[], int size)
{
    uint32 cmd;

    if ((size < 1) || (size > 4)) {
        return (SOC_E_PARAM);
    }

   cmd = (SBUS_WRITE_REGISTER_CMD_MSG << 26) |
         ((montreal2_BLKTYPE_TOP & 0x7f) << 19) |
         ((4 * size) << 7);

    return (phy_8806x_sbus_cmd(unit, pc, reg, cmd, 2, arg));

}

int
_phy_8806x_fcport_sbus_write(int unit,phy_ctrl_t *pc, int sys, uint32 reg, uint32 arg[], int size)
{
     uint32 cmd, blk, addr;
     int ring;

     if ((size < 1) || (size > 4)) {
         return (SOC_E_PARAM);
     }

    blk = sys ? montreal2_BLKTYPE_SYS_FCPORT : montreal2_BLKTYPE_LINE_FCPORT;
    addr = reg & 0x03ffffff;
    addr |= ((blk & 0x3f) << 26);

    /* Ports 0-3 = MIG0, 4-7 = MIG1 */
    ring = ((pc->phy_id % 8) < 4) ? 0 : 1;

   cmd = (SBUS_WRITE_REGISTER_CMD_MSG << 26) |
         ((blk & 0x7f) << 19) |
         ((4 * size) << 7);

    return (phy_8806x_sbus_cmd(unit, pc, addr, cmd, ring, arg));

}

int
_sbus_write_test(int unit,int port, uint32 reg, uint32 value)
{
    phy_ctrl_t *pc;
    uint32 temp32 = value;
    int rv;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = _phy_8806x_toplvl_sbus_write(unit, pc, reg, &temp32, 1);

    return rv;
}

int
_phy_8806x_toplvl_sbus_read(int unit,phy_ctrl_t *pc, uint32 reg, uint32 arg[], int size)
{
    uint32 cmd;

    if ((size < 1) || (size > 4)) {
        return (SOC_E_PARAM);
    }

    cmd = (SBUS_READ_REGISTER_CMD_MSG << 26) |
          ((montreal2_BLKTYPE_TOP & 0x7f) << 19) |
          ((4 * size) << 7);

    return (phy_8806x_sbus_cmd(unit, pc, reg, cmd, 2, arg));
}

#if defined(INCLUDE_FCMAP)
int
_phy_8806x_fcport_sbus_read(int unit,phy_ctrl_t *pc, int sys, uint32 reg, uint32 arg[], int size)
{
     uint32 cmd, blk, addr;
     int ring;

     if ((size < 1) || (size > 4)) {
         return (SOC_E_PARAM);
     }

    blk = sys ? montreal2_BLKTYPE_SYS_FCPORT : montreal2_BLKTYPE_LINE_FCPORT;
    addr = reg & 0x03ffffff;
    addr |= ((blk & 0x3f) << 26);

    /* Ports 0-3 = MIG0, 4-7 = MIG1 */
    ring = ((pc->phy_id % 8) < 4) ? 0 : 1;

    cmd = (SBUS_READ_REGISTER_CMD_MSG << 26) |
          ((blk & 0x7f) << 19) |
          ((4 * size) << 7);

     return (phy_8806x_sbus_cmd(unit, pc, addr, cmd, ring, arg));
}
#endif

int
_sbus_read_test(int unit,int port, uint32 reg)
{
    phy_ctrl_t *pc;
    uint32 temp32;
    int rv;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = _phy_8806x_toplvl_sbus_read(unit, pc, reg, &temp32, 1);

    LOG_CLI((BSL_META_U(unit,
                        "_sbus_read_test: u=%d p=%d "
                        "value=0x%08x\n"), unit, pc->port, temp32));
    return rv;
}

int 
_phy_8806x_write_top_soft_reset_reg(int unit, phy_ctrl_t *pc, uint32 val)
{
    uint32 temp = val;
    int rv;
    rv = _phy_8806x_toplvl_sbus_write(unit, pc, montreal2_TOP_SOFT_RESET_REGr, &temp, 1);
    return (rv);
}

int
_phy_8806x_write_top_rts_misc_ctrl_reg(int unit, phy_ctrl_t *pc, uint32 val)
{
    uint32 temp = val;
    int rv;
    rv = _phy_8806x_toplvl_sbus_write(unit, pc, montreal2_TOP_RTS_MISC_CTRLr, &temp, 1);
    return (rv);
}

int 
_phy_8806x_write_top_soft_reset_reg_2(int unit, phy_ctrl_t *pc, uint32 val)
{
    uint32 temp = val;
    int rv;
    rv = _phy_8806x_toplvl_sbus_write(unit, pc, montreal2_TOP_SOFT_RESET_REG_2r, &temp, 1);
    return (rv);
}

int 
_phy_8806x_write_top_fc_mode_control_reg(int unit, phy_ctrl_t *pc, uint32 val)
{
    uint32 temp = val;
    int rv;
    rv = _phy_8806x_toplvl_sbus_write(unit, pc, montreal2_TOP_FC_MODE_CONTROLr, &temp, 1);
    return (rv);
}

int 
_phy_8806x_write_fcport_ext_tx_disable_ctrl(int unit, phy_ctrl_t *pc, int sys, uint32 val)
{
    uint32 temp = val;
    int rv;
    rv = _phy_8806x_fcport_sbus_write(unit, pc, sys, montreal2_FCPORT_EXT_TX_DISABLE_CTRLr, &temp, 1);
    return (rv);
}

int
_fwdl_test(int unit,int port)
{
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (_phy_8806x_write_to_arm(unit, pc, 0, FIRMWARE(pc), FIRMWARE_LEN(pc)));

    return (SOC_E_NONE);
}


/* _phy_8806x_axi_{read|write}() need to be optimized when the data only transactions are
   implemented in the CMIC MDIO code.
 */
 
STATIC int
_phy_8806x_axi_write(int unit,phy_ctrl_t *pc, uint32 addr, uint32 data[], int size)
{
    int    count, rv, i;
    uint16 control, status;
    uint32 temp32;

    if ( addr & 0x3 ) {
        LOG_CLI((BSL_META_U(unit,
                            "_phy_8806x_axi_write failed: u=%d p=%d "
                            "addr=%08x - unaligned !.\n"), unit, pc->port, addr));
        return (SOC_E_FAIL);
    }

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8806X_AXI_ADDR_UPPER_FX_REG(unit, pc, (addr >> 16) & 0xffff));
    SOC_IF_ERROR_RETURN
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
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8806X_AXI_DATA_BYTES_5_4_FX_REG(unit, pc, temp32 & 0xffff));
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8806X_AXI_DATA_BYTES_7_6_FX_REG(unit, pc, (temp32 >> 16) & 0xffff));
        } else {
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8806X_AXI_DATA_BYTES_1_0_FX_REG(unit, pc, temp32 & 0xffff));
            SOC_IF_ERROR_RETURN
                (WRITE_PHY8806X_AXI_DATA_BYTES_3_2_FX_REG(unit, pc, (temp32 >> 16) & 0xffff));
        }

        SOC_IF_ERROR_RETURN
            (WRITE_PHY8806X_AXI_CONTROL_FX_REG(unit, pc, (1U << 13))); /* pre-go */

        SOC_IF_ERROR_RETURN
            (WRITE_PHY8806X_AXI_CONTROL_FX_REG(unit, pc, (5U << 13))); /* go */

        count = 0;

        do {
            rv = READ_PHY8806X_AXI_CONTROL_FX_REG(unit, pc, &control);
            if (((control & (1U << 15) ) == 0) ||
                SOC_FAILURE(rv)) {
                break;
            }
        } while (count++ < AXI_MAX_POLLS);

        rv = READ_PHY8806X_AXI_STATUS_FX_REG(unit, pc, &status);

        if (count >= AXI_MAX_POLLS) {
            LOG_CLI((BSL_META_U(unit,
                                "_phy_8806x_axi_write failed: u=%d p=%d addr=0x%08x control=0x%04x status=0x%04x"
                                "AXI_MAX_POLLS reached !.\n"), unit, pc->port, addr + (i << 2), control, status));
            return (SOC_E_FAIL);
        }

        if ((status & (1U << 9)) != 0) {
            LOG_CLI((BSL_META_U(unit,
                                "_phy_8806x_axi_write failed: u=%d p=%d "
                                "addr=0x%08x status=0x%04x\n"), unit, pc->port, addr + (i << 2), status));
            return (SOC_E_FAIL);
        }
        i++;
    }

    return (SOC_E_NONE);
}

int
_axi_write_test(int unit,int port, uint32 reg, uint32 value)
{
    phy_ctrl_t *pc;
    uint32 temp32 = value;
    int rv;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = _phy_8806x_axi_write(unit, pc, reg, &temp32, 1);

    return rv;
}

int
_axi_write_test2(int unit,int port, uint32 reg, uint32 value)
{
    phy_ctrl_t *pc;
    uint32 temp32[2];
    int rv;

    temp32[0] = value;
    temp32[1] = value ^ 0xffffffff;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = _phy_8806x_axi_write(unit, pc, reg, temp32, 2);

    LOG_CLI((BSL_META_U(unit,
                        "_sbus_read_test 2: u=%d p=%d "
                        "temp32[0]=0x%08x temp32[1]=0x%08x\n"), unit, pc->port, temp32[0], temp32[1]));
    return rv;
}

STATIC int
_phy_8806x_axi_read(int unit,phy_ctrl_t *pc, uint32 addr, uint32 data[], int size)
{
    int    count, rv, i;
    uint16 control, status;
    uint16 temp0, temp1;
    uint32 temp32;

    if ( addr & 0x3 ) {
        LOG_CLI((BSL_META_U(unit,
                            "_phy_8806x_axi_read failed: u=%d p=%d "
                            "addr=%08x - unaligned !.\n"), unit, pc->port, addr));
        return (SOC_E_FAIL);
    }

    SOC_IF_ERROR_RETURN
        (WRITE_PHY8806X_AXI_ADDR_UPPER_FX_REG(unit, pc, (addr >> 16) & 0xffff));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY8806X_AXI_ADDR_LOWER_FX_REG(unit, pc, addr & 0xffff));

    i = 0;
    while (size-- > 0) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY8806X_AXI_CONTROL_FX_REG(unit, pc, (3U << 13))); /* pre-go */

        SOC_IF_ERROR_RETURN
            (WRITE_PHY8806X_AXI_CONTROL_FX_REG(unit, pc, (7U << 13))); /* go */

        count = 0;

        do {
            rv = READ_PHY8806X_AXI_CONTROL_FX_REG(unit, pc, &control);
            if (((control & (1U << 15)) == 0) ||
                SOC_FAILURE(rv)) {
                break;
            }
        } while (count++ < AXI_MAX_POLLS);

        rv = READ_PHY8806X_AXI_STATUS_FX_REG(unit, pc, &status);

        if (count >= AXI_MAX_POLLS) {
            LOG_CLI((BSL_META_U(unit,
                                "_phy_8806x_axi_read failed: u=%d p=%d addr=0x%08x control=0x%04x status=0x%04x\n"
                                "AXI_MAX_POLLS reached !.\n"), unit, pc->port, addr + (i << 2), control, status));
            return (SOC_E_FAIL);
        }

        if ((status & (1U << 9)) != 0) {
            LOG_CLI((BSL_META_U(unit,
                                "_phy_8806x_axi_read failed: u=%d p=%d "
                                "addr=0x%08x status=0x%04x\n"), unit, pc->port, addr + (i << 2), status));
            return (SOC_E_FAIL);
        }

        /* 
           When the current source address is 8 byte aligned read from AXI_DATA_BYTES_1_2_FX_REG and 
           AXI_DATA_BYTES_3_4_FX_REG else read from AXI_DATA_BYTES_5_6_FX_REG and AXI_DATA_BYTES_7_8_FX_REG.
         */
        if ( (addr + (i << 2)) & 0x7 ) {
            SOC_IF_ERROR_RETURN
                (READ_PHY8806X_AXI_DATA_BYTES_5_4_FX_REG(unit, pc, &temp0));
            SOC_IF_ERROR_RETURN
                (READ_PHY8806X_AXI_DATA_BYTES_7_6_FX_REG(unit, pc, &temp1));
        } else {
            SOC_IF_ERROR_RETURN
                (READ_PHY8806X_AXI_DATA_BYTES_1_0_FX_REG(unit, pc, &temp0));
            SOC_IF_ERROR_RETURN
                (READ_PHY8806X_AXI_DATA_BYTES_3_2_FX_REG(unit, pc, &temp1));
        }
        temp32 = (((uint32) temp1) << 16) | temp0;

        /* The following preserves the byte order. */
        data[i] = LE2HOST32(temp32);

        i++;
    }

    return (SOC_E_NONE);

}

int
_axi_read_test(int unit,int port, uint32 reg)
{
    phy_ctrl_t *pc;
    uint32 temp32;
    int rv;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = _phy_8806x_axi_read(unit, pc, reg, &temp32, 1);

    LOG_CLI((BSL_META_U(unit,
                        "_sbus_read_test: u=%d p=%d "
                        "value=0x%08x\n"), unit, pc->port, temp32));
    return rv;
}

int
_axi_read_test2(int unit,int port, uint32 reg)
{
    phy_ctrl_t *pc;
    uint32 temp32[2];
    int rv;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = _phy_8806x_axi_read(unit, pc, reg, temp32, 2);

    LOG_CLI((BSL_META_U(unit,
                        "_sbus_read_test: u=%d p=%d "
                        "temp32[0]=0x%08x temp32[1]=0x%08x\n"), unit, pc->port, temp32[0], temp32[1]));
    return rv;
}

int
phy_is_8806x(phy_ctrl_t *pc)
{
    return (PHY_IS_BCM8806X(pc) ? SOC_E_NONE : SOC_E_FAIL);
}

int
phy_8806x_fw_stat_read(int unit, int port, int offset,  uint32 result[], int result_size)
{
    phy_ctrl_t *pc;
    int rv = SOC_E_NONE;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (sal_mutex_take(xmod_mutex[unit], XMOD_MUTEX_TIMEOUT) < 0)
    {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_8806x_fw_stat_read: Couldn't aquire lock. u=%d p=%d\n"), \
                  unit, pc->port));
        return SOC_E_FAIL;
    }

    /* 0x{2,3,4,5}4 0200 : axi_buf = 0x240200 + (pc->phy_id & 0x3) * 0x100000 */
    rv = _phy_8806x_axi_read(unit, pc, 0x240200 + ((pc->phy_id & 0x3) * 0x100000)+(offset*4), result, result_size);

    sal_mutex_give(xmod_mutex[unit]);
    return rv;
}

STATIC int
_phy_8806x_xmod_command_send(int unit, phy_ctrl_t *pc, int mode_opcode,  uint32 arg[], int arg_size)
{
    uint16 status;
    int rv, ack_count = 0;

    if ( FLAGS(pc) & PHY8806X_PM ) {
        return SOC_E_NONE;
    }

    if (arg_size > 256) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PHY8806X command handler (invalid parameters): u=%d p=%d arg_size=%d\n"), \
                  unit, pc->port, arg_size));
        return SOC_E_PARAM;
    }

    retry_after_ack:
    soc_timeout_init(&TO(pc), 10000000, 0); /* 10000 ms timer */

    do {
        rv = READ_PHY8806X_FW_TO_SW_MESSAGE_REG(unit, pc, &status);
        if (!(status & (0x3 << 13)) || SOC_FAILURE(rv)) {
            break;
        }
    } while (!soc_timeout_check(&TO(pc)));
    if ((status & (0x3 << 13)) || SOC_FAILURE(rv)) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PHY8806X command handler timeout (not ready): u=%d p=%d status=0x%04x\n"), \
                  unit, pc->port, status));
        _phy_8806x_xmod_command_ack(unit, pc);
        if (ack_count++ < RETRY_COUNT ) {
            goto retry_after_ack;
        }
        return SOC_E_TIMEOUT;
    }

   /* 
   -> 0x{2,3,4,5}4 0000 - 0x{2,3,4,5}4 00ff (256 bytes), axi_buf = 0x240000 + (pc->phy_id & 0x3) * 0x100000
   <- 0x{2,3,4,5}4 0100 - 0x{2,3,4,5}4 01ff (256 bytes), axi_buf = 0x240100 + (pc->phy_id & 0x3) * 0x100000
    */

    /* write arguments */
    rv = _phy_8806x_axi_write(unit, pc, 0x240000 + (pc->phy_id & 0x3) * 0x100000, arg, arg_size);

    if (SOC_FAILURE(rv)) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PHY8806X command handler (AXI write failure): u=%d p=%d\n"), \
                  unit, pc->port));
        return SOC_E_FAIL;
    }

    /* issue command */
    rv = WRITE_PHY8806X_SW_TO_FW_INTERRUPT_REG(unit, pc, (1U << 15) | (mode_opcode & 0x7fff));

    if (SOC_FAILURE(rv)) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PHY8806X command handler (command failure): u=%d p=%d\n"), \
                  unit, pc->port));
        return SOC_E_FAIL;
    }

    soc_timeout_init(&TO(pc), 10000000, 0); /* 10000 ms timer */

    return SOC_E_NONE;
}


STATIC int
_phy_8806x_xmod_result_recv(int unit, phy_ctrl_t *pc, uint32 result[], int result_size)
{
    uint16 status;
    int rv, rval;

    if ( FLAGS(pc) & PHY8806X_PM ) {
        return SOC_E_NONE;
    }

    if (result_size > 256) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "_phy_8806x_xmod_result_recv: invalid parameters: u=%d p=%d result_size=%d\n"), \
                  unit, pc->port, result_size));
        /* issue ack command */
        _phy_8806x_xmod_command_ack(unit, pc);
        return SOC_E_PARAM;
    }

    do {
        rv = READ_PHY8806X_FW_TO_SW_MESSAGE_REG(unit, pc, &status);
        if ((status & (0x1 << 15)) | ((status & (0x3 << 13)) == (0x2 << 13)) || SOC_FAILURE(rv)) {
            break;
        }
    } while (!soc_timeout_check(&TO(pc)));

    if (SOC_FAILURE(rv)) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PHY8806X command handler (status read failure): u=%d p=%d\n"), \
                  unit, pc->port));
        /* issue ack command */
        _phy_8806x_xmod_command_ack(unit, pc);
        return SOC_E_FAIL;
    }

    if (status & (0x1 << 15)) { /* error */
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PHY8806X command handler failure: u=%d p=%d status=0x%04x\n"), \
                  unit, pc->port, status));
        /* issue ack command */
        _phy_8806x_xmod_command_ack(unit, pc);
        rval = -(status & BFCMAP_ERROR_CODE_MASK);
	/* could err Code map before */
        return rval;
    }

    if ((status & (0x3 << 13)) != (0x2 << 13)) { /* result not ready */
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PHY8806X command handler timeout (no result): u=%d p=%d status=0x%04x\n"), \
                  unit, pc->port, status));
        /* issue ack command */
        _phy_8806x_xmod_command_ack(unit, pc);
        return SOC_E_TIMEOUT;
    }


   /* 
   -> 0x{2,3,4,5}4 0000 - 0x{2,3,4,5}4 00ff (256 bytes), axi_buf = 0x240000 + (pc->phy_id & 0x3) * 0x100000
   <- 0x{2,3,4,5}4 0100 - 0x{2,3,4,5}4 01ff (256 bytes), axi_buf = 0x240100 + (pc->phy_id & 0x3) * 0x100000
    */


    /* read results */
    rv = _phy_8806x_axi_read(unit, pc, 0x240100 + (pc->phy_id & 0x3) * 0x100000, result, result_size);

    if (SOC_FAILURE(rv)) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PHY8806X command handler (AXI read failure): u=%d p=%d\n"), \
                  unit, pc->port));
        /* issue ack command */
        _phy_8806x_xmod_command_ack(unit, pc);

        return SOC_E_FAIL;
    }

    /* issue ack command */
    _phy_8806x_xmod_command_ack(unit, pc);

    return SOC_E_NONE;

}

int
phy_8806x_xmod_command(int unit, int port, int mode_opcode,  uint32 arg[], int arg_size, uint32 result[], int result_size)
{
    phy_ctrl_t *pc;
    int rv;

    pc = EXT_PHY_SW_STATE(unit, port);

    /* MT2 XMOD set opcodes have the LSB set */
    if ((SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit)) && (mode_opcode & 0x1)) {
        return SOC_E_NONE;
    }

    if (sal_mutex_take(xmod_mutex[unit], XMOD_MUTEX_TIMEOUT) < 0)
    {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_8806x_xmod_command: Couldn't aquire lock. u=%d p=%d mode:0x%x cmd:0x%x \n"), \
                  unit, pc->port, (mode_opcode >> 8), (mode_opcode & 0xFF)));
        return SOC_E_FAIL;
    }

    rv = _phy_8806x_xmod_command_send(unit, pc, mode_opcode,  arg, arg_size);

    if (SOC_FAILURE(rv)) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_8806x_xmod_command: send command failed. u=%d p=%d mode:0x%x cmd:0x%x \n"), \
                  unit, pc->port, (mode_opcode >> 8), (mode_opcode & 0xFF)));
        sal_mutex_give(xmod_mutex[unit]);
        return SOC_E_FAIL;
    }

    rv = _phy_8806x_xmod_result_recv(unit, pc, result, result_size);

    if (SOC_FAILURE(rv)) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_8806x_xmod_command: receive result failed. u=%d p=%d mode:0x%x cmd:0x%x \n"), \
                  unit, pc->port, (mode_opcode >> 8), (mode_opcode & 0xFF)));
        sal_mutex_give(xmod_mutex[unit]);
        return rv;
    }

    sal_mutex_give(xmod_mutex[unit]);
    return SOC_E_NONE;
}

STATIC int
_phy_8806x_xmod_command_ack(int unit, phy_ctrl_t *pc)
{
    int rv;

    if ( FLAGS(pc) & PHY8806X_PM ) {
        return SOC_E_NONE;
    }

    /* issue ack command */
    rv = WRITE_PHY8806X_SW_TO_FW_INTERRUPT_REG(unit, pc, (1U << 15) | (0x17 & 0x7ff));

    if (SOC_FAILURE(rv)) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PHY8806X command handler (ack failure): u=%d p=%d\n"), \
                  unit, pc->port));
        return SOC_E_FAIL;
    }

    return SOC_E_NONE;
}


int xmod_test(int unit, int port) 
{
    uint32 temp32[2];
    int rv;

    temp32[0] = 0xaaaaaaaa;

    rv = phy_8806x_xmod_command(unit, port, 0x12, &temp32[0], 1, &temp32[1], 1);

    LOG_CLI((BSL_META_U(unit,
                        "_phy_8806x_xmod_command: u=%d p=%d "
                        "temp32[0]=0x%08x temp32[1]=0x%08x\n"), unit, port, temp32[0], temp32[1]));
    return rv;


}

STATIC int _tsc_reg_read(int unit, phy_ctrl_t *pc, int mode, int tsc_port, int pmd, int pll, uint32 reg, uint16 *value) 
{
    uint32 data[4], mem_cmd;
    int ring;

    ring = (pc->phy_id & 0x4) ? 1 : 0; /* mig too */

    /* No other SBUS  accesses between the write and the read. Ie. R5s need to be shutdown. */

    mem_cmd = (SBUS_WRITE_MEMORY_CMD_MSG   << 26) |((((tsc_port > 8) ? 0x7 : 0x3) & 0x7f) << 19) |(16 << 7);

    reg &= 0xffff;

    data[0] = ( (reg | (pmd ? (1U << 27) : 0) ) |  ( (tsc_port & 0x1f) << 19) ) | ((mode & 0x7) << 16) | (pll << 24);
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));

    mem_cmd = (SBUS_READ_MEMORY_CMD_MSG   << 26) |((((tsc_port > 8) ? 0x7 : 0x3) & 0x7f) << 19) |(16 << 7);

    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));

    LOG_INFO(BSL_LS_SOC_PHY, (BSL_META_U(unit,
                        "tsc_reg_read: u=%d p=%d tsc_port=%d"
                        " data[0]=0x%08x data[1]=0x%08x data[2]=0x%08x data[3]=0x%08x\n"),
        unit, pc->port, tsc_port, data[0], data[1], data[2], data[3]));

    *value = (data[1] & 0xffff);

    return SOC_E_NONE;

}

int tsc_reg_read_test_base(int unit, int port, int sys, int pmd, int pll, uint32 reg) 
{
    phy_ctrl_t *pc;
    uint16 value;
    int tsc_port;

    pc = EXT_PHY_SW_STATE(unit, port);

    tsc_port = (sys << 3) + (((pc->phy_id & 0x4) ? 1 : 0) << 2)  + 1;

    SOC_IF_ERROR_RETURN
        (_tsc_reg_read(unit, pc, 0, tsc_port, pmd, pll, reg, &value)); 

    LOG_CLI((BSL_META_U(unit,
                        "tsc_reg_read_test_base: u=%d p=%d %s "
                        "reg=0x%04x value=0x%04x\n"), unit, port, sys ? "S":"L", reg, value));

    return SOC_E_NONE;
}


STATIC int _tsc_reg_write(int unit, phy_ctrl_t *pc, int mode, int tsc_port, int pmd, int pll, uint32 reg, uint16 value, uint16 mask) 
{
    uint32 data[4], mem_cmd;
    int ring;

    ring = (pc->phy_id & 0x4) ? 1 : 0; /* mig too */

    mem_cmd = (SBUS_WRITE_MEMORY_CMD_MSG   << 26) |((((tsc_port > 8) ? 0x7 : 0x3) & 0x7f) << 19) |(16 << 7);

    reg &= 0xffff;

    data[0] = ( (reg | (pmd ? (1U << 27) : 0) ) |  ( (tsc_port & 0x1f) << 19) ) | ((mode & 0x7) << 16) | (pll << 24);
    data[1] = ((value & 0xffff) << 16) | (mask & 0xffff);
    data[2] = 1;
    data[3] = 0;

    LOG_INFO(BSL_LS_SOC_PHY, (BSL_META_U(unit,
                        "tsc_reg_write: u=%d p=%d tsc_port=%d"
                        " data[0]=0x%08x data[1]=0x%08x data[2]=0x%08x data[3]=0x%08x\n"),
        unit, pc->port, tsc_port, data[0], data[1], data[2], data[3]));

    SOC_IF_ERROR_RETURN
        (phy_8806x_sbus_cmd(unit, pc, 0x00000000, mem_cmd, ring,  data));

    return SOC_E_NONE;

}

int tsc_reg_write_test_base(int unit, int port, int sys, int pmd, int pll, uint32 reg, uint16 value, uint16 mask) 
{
    phy_ctrl_t *pc;
    int tsc_port;

    pc = EXT_PHY_SW_STATE(unit, port);

    tsc_port = (sys << 3) + (((pc->phy_id & 0x4) ? 1 : 0) << 2)  + 1;

    SOC_IF_ERROR_RETURN
        (_tsc_reg_write(unit, pc, 0, tsc_port, pmd, pll, reg, value, mask)); 

    LOG_CLI((BSL_META_U(unit,
                        "tsc_reg_write_test_base: u=%d p=%d %s "
                        "reg=0x%04x value=0x%04x\n"), unit, port, sys ? "S":"L", reg, value));

    return SOC_E_NONE;
}

STATIC int
phy8806x_medium_get(int unit, soc_port_t port, soc_port_medium_t *medium)
{
    if (medium == NULL) {
        return SOC_E_PARAM;
    }

    *medium = SOC_PORT_MEDIUM_FIBER;

    return SOC_E_NONE;
}


STATIC int
phy8806x_medium_config_get(int unit, soc_port_t port,
                           soc_port_medium_t medium,
                           soc_phy_config_t *cfg)
{
    int            rv;
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    switch (medium) {
    case SOC_PORT_MEDIUM_FIBER:
        sal_memcpy(cfg, &pc->fiber, sizeof (*cfg));
        break;
    default:
        rv = SOC_E_PARAM;
        break;
    }

    return rv;
}

 
STATIC int _phy8806x_dump_buffer(int unit, int port, unsigned char *arg)
{
    uint32 uart_ptr = 0, print_ptr = 0, temp32, uart_ptr_r = 0, print_ptr_r = 0, mode = 0;
    int cur_buf_size_r = 0, cur_buf_size = 0, avail_buf_size = 0, rv = SOC_E_NONE;
    phy_ctrl_t *pc = NULL;
    unsigned char *ch;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (pc == NULL) {
        return SOC_E_FAIL;
    }
    ch = arg - 4;

    avail_buf_size = (ch[0] | (ch[1] << 8) | (ch[2] << 16) | (ch[3] << 24)) - 8; /* 8 for rounding the AXI buffer */

    if (avail_buf_size < 0) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "Insufficient UART buffer u=%d p=%d\n"), \
                  unit, pc->port));
        return SOC_E_FAIL;
    }

    if (sal_mutex_take(xmod_mutex[unit], XMOD_MUTEX_TIMEOUT) < 0)
    {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_8806x_fw_stat_read: Couldn't aquire lock. u=%d p=%d\n"), \
                  unit, pc->port));
        return SOC_E_FAIL;
    }

    if (SOC_SUCCESS(rv)) {
        /* read the mode (uart_silent_mode) from the AXI memory of the mHost (pc->phy_id & 0x3) */
        rv = _phy_8806x_axi_read(unit, pc, 0x27fff4 + (pc->phy_id & 0x3) * 0x100000, &temp32, 1);
        mode = LE2HOST32(temp32) & 0x3;
    }

    if (! mode ) {
        /* Real uart is active in f/w. Let it fall through. */
        rv = SOC_E_FAIL;
    }

    
    if (SOC_SUCCESS(rv)) {
        /* read uart_ptr from the AXI memory of the mHost (pc->phy_id & 0x3) */
        rv = _phy_8806x_axi_read(unit, pc, 0x27fff8 + (pc->phy_id & 0x3) * 0x100000, &temp32, 1);
        uart_ptr = LE2HOST32(temp32);
    }
 
    if (SOC_SUCCESS(rv)) {
        /* read print_ptr from the AXI memory of the mHost (pc->phy_id & 0x3) */
        rv = _phy_8806x_axi_read(unit, pc, 0x27fffc + (pc->phy_id & 0x3) * 0x100000, &temp32, 1);
        print_ptr = LE2HOST32(temp32);
    }

    /* printf("Port %d mode = %d uart ptr 0x%x print ptr 0x%x\n", port, mode, uart_ptr, print_ptr); */
    cur_buf_size = print_ptr - uart_ptr;

    if ((cur_buf_size > 0) && (cur_buf_size > avail_buf_size)) {
        print_ptr -= cur_buf_size - avail_buf_size;
    }

    if ((cur_buf_size < 0) && ( (0x8000 - 0x10 + cur_buf_size) > avail_buf_size)) {
        int temp;

        temp = print_ptr - (0x8000 - 0x10 + cur_buf_size - avail_buf_size);
        if (temp < 0) {
            temp += 0x8000 - 0x10;
        }
        print_ptr = temp;
    }

    if (SOC_SUCCESS(rv)) {
        uart_ptr_r = uart_ptr & ~0x3;                /* uart_ptr rounded to the previous 4 byte boundary */
        print_ptr_r = (print_ptr + 3) & ~0x3;        /* print_ptr rounded to the next 4 byte boundary */
        cur_buf_size_r = (print_ptr_r - uart_ptr_r); /* rounded cur_buf_size */
    }

    if (cur_buf_size_r > 0) {

        if (SOC_SUCCESS(rv)) {
            rv = _phy_8806x_axi_read(unit, pc, 0x278000 + uart_ptr_r + ((pc->phy_id & 0x3) << 20), (uint32 *)arg, cur_buf_size_r >> 2 ); /* << 20 = * 0x100000 */
            arg[cur_buf_size_r] = 0;
        }
    } else if (cur_buf_size_r < 0) {

        if (SOC_SUCCESS(rv)) {
            /* Read from uart_ptr to end */
            rv = _phy_8806x_axi_read(unit, pc, 0x278000 + uart_ptr_r + ((pc->phy_id & 0x3) << 20), (uint32 *)arg, ((0x8000 - 0x10 - uart_ptr_r) >> 2));
        }

        if (SOC_SUCCESS(rv)) {
            /* Read from start to print_ptr */
            rv = _phy_8806x_axi_read(unit, pc, 0x278000 + ((pc->phy_id & 0x3) << 20), (uint32 *)(arg + 0x8000 - 0x10 - uart_ptr_r), print_ptr_r >> 2);
        }

        arg[0x8000 - 0x10 + cur_buf_size_r] = 0;
    } else {
        arg[0] = 0;
    }
    
    if (SOC_SUCCESS(rv)) {
        /* Write the print_prt value to the uart_prt localtion. */
        temp32 = HOST2LE32(print_ptr);
        rv = _phy_8806x_axi_write(unit, pc, 0x27fff8 + ((pc->phy_id & 0x3) << 20), &temp32, 1);
    }

#if 0
/* Read back values */
    _phy_8806x_axi_read(unit, pc, 0x27fff8 +  ((pc->phy_id & 0x3) << 20), &temp32, 1);
    uart_ptr = LE2HOST32(temp32);
 
    _phy_8806x_axi_read(unit, pc, 0x27fffc +  ((pc->phy_id & 0x3) << 20), &temp32, 1);
    print_ptr = LE2HOST32(temp32);

    printf("After write Port %d uart ptr 0x%x print ptr 0x%x\n", port, uart_ptr, print_ptr);
#endif

    sal_mutex_give(xmod_mutex[unit]);

    if (SOC_FAILURE(rv)) {
        arg[0] = 0;
    }
    return rv;
}

int phy8806x_xmod_debug_cmd(int unit, int port)
{
    phy_ctrl_t                *pc;
    soc_phymod_phy_t          *phy;
    soc_phymod_ctrl_t         *pmc;
    int                        rv = SOC_E_NONE;

    /* locate phy control */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

#if defined(INCLUDE_FCMAP)
    if (pc->fcmap_enable) {
        return SOC_E_UNAVAIL;
    }
#endif

    pmc = &pc->phymod_ctrl;
    phy = pmc->phy[0]; 

    rv = phymod_debug_ether(&phy->pm_phy);

    return rv;
}

STATIC uint32
_phy8806x_speed_led_encode(int speed)
{
    if (speed == 0) {
        return 0;
    } else if (speed == 1000000) { /* 1G */
        return 0x1;
    } else if (speed == 10000000) { /* 10G */
        return 0x2;
    } else if (speed == 20000000) { /* 20G */
        return 0x3;
    } else if (speed == 25000000) { /* 25G */
        return 0x4;
    } else if (speed == 40000000) { /* 40G */
        return 0x5;
    } else if (speed == 50000000) { /* 50G */
        return 0x6;
    } else if (speed == 100000000) { /* 100G */
        return 0x7;
    } else if (speed == 4000000) { /* 4G - FC Only */
        return 0x8;
    } else if (speed == 8000000) { /* 8G - FC Only */
        return 0x9;
    } else if (speed == 16000000) { /* 16G - FC Only */
        return 0xa;
    } else if (speed == 32000000) { /* 32G - FC Only */
        return 0xb;
    } else {
        return 0xf;
    }
}

int
phy_mt2_update_led_speed_activity(int unit, soc_port_t port)
{
    uint32 speed, portdata = 0;
#ifdef BCM_TOMAHAWK_SUPPORT
    int byte;
    uint32 prev_data, led_dram_base;
    int phy_port = 0;  /* physical port number */
    soc_info_t *si = &SOC_INFO(unit);
#endif /* BCM_TOMAHAWK_SUPPORT */
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

    if ((pc == NULL) || (!PHY_IS_BCM8806X(pc))){
        return SOC_E_NONE;
    }

    /* Update TX activity */
    if (LED_TX(pc)) {
        portdata |= 0x2;
    }
    /* Update RX activity */
    if (LED_RX(pc)) {
        portdata |= 0x4;
    }
    /*Update Speed */
    speed = _phy8806x_speed_led_encode(LED_SPEED(pc));
    portdata |= (speed << 3);

    /* For now, only implemented for Tomohawk. Expand as needed */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWK(unit)) {
        phy_port = si->port_l2p_mapping[port];
        led_dram_base = CMIC_LEDUP0_DATA_RAM_OFFSET;
        /* Tomahawk: Port 1-32 and 97-128 are in ledproc0
         *           Port 33-96 are in ledproc1
         *           Port 129 & 131 are in ledproc2
         */
        if (phy_port > 32 && phy_port < 97) {
            led_dram_base = CMIC_LEDUP1_DATA_RAM_OFFSET;
            phy_port -=  32;
        } else if (phy_port == 129 ) {
            led_dram_base = CMIC_LEDUP2_DATA_RAM_OFFSET;
            phy_port = 1;
        } else if (phy_port == 131 ) {
            led_dram_base = CMIC_LEDUP2_DATA_RAM_OFFSET;
            phy_port = 2;
        } else if (phy_port > 32) {
            phy_port -= 64;
        }
        byte = LS_LED_DATA_OFFSET_A0 + phy_port - 1;

        prev_data = soc_pci_read(unit, led_dram_base + CMIC_LED_REG_SIZE * byte);
        prev_data &= ~0x7e;
        /* if link is down, mark activity as down
           as activity is not being updated ..*/
        if (prev_data & 0x01) {
            portdata |= prev_data;
        } else {
            portdata = prev_data;
        }
        soc_pci_write(unit, led_dram_base + CMIC_LED_REG_SIZE * byte, portdata);

    }
#endif /* BCM_TOMAHAWK_SUPPORT */

    return SOC_E_NONE;
}


#if PHY8806X_DEBUG
int phy8806x_dev_debug_cmd(int unit, int port, int cmd, void *txbuf, int txlen, int *rsp, void *rxbuf, int rxlen)
{
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint32_t xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *buftxptr, *bufrxptr;
    int xmodtxlen, xmodrxlen;
    uint32_t x_cmd, x_response;
    int rv, tx_cmd_len, rx_cmd_len;

    /* setting up xmod xmod_dev_debug_cmd:                             */
    /* int xmod_dev_debug_cmd(IN uint32_t cmd, OUT uint32_t response); */

    tx_cmd_len = txlen + sizeof(x_cmd);
    rx_cmd_len = rxlen + sizeof(x_response);

    /* check for tx lengths */
    if (tx_cmd_len > XMOD_DEV_DEBUG_CMD_IN_LEN) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                      "ERROR, TX length (%d) too long > %d\n"), tx_cmd_len, XMOD_DEV_DEBUG_CMD_IN_LEN));
        return SOC_E_PARAM;
    }
    /* check for rx lengths */
    if (rx_cmd_len > XMOD_DEV_DEBUG_CMD_OUT_LEN) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                      "ERROR, RX length (%d) too long > %d\n"), rx_cmd_len, XMOD_DEV_DEBUG_CMD_OUT_LEN));
        return SOC_E_PARAM;
    }

    x_cmd = HOST2LE32(cmd);

    /* write args to xmod buffer */
    buftxptr = (uint8_t *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &x_cmd, sizeof(x_cmd));
    xmodtxlen = sizeof(x_cmd);
    if ((txbuf!=NULL) && txlen) {
        WRITE_XMOD_ARG_BUFF(buftxptr, txbuf, txlen);
        xmodtxlen += txlen;
    }
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    /* get rx len */
    xmodrxlen = sizeof(x_response);
    if ((rxbuf!=NULL) && rxlen) {
        xmodrxlen += rxlen;
    }
    xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);

    /* call xmod */
    rv = phy_8806x_xmod_command(unit, port, XMOD_CMD_MODE_CORE(XMOD_DEV_DEBUG_CMD), xmodtxbuff, xmodtxlen, xmodrxbuff, xmodrxlen);

    /* retrieve the argument from the xmod rx buffer */
    bufrxptr = (uint8_t *)xmodrxbuff;
    READ_XMOD_ARG_BUFF(bufrxptr, &x_response, sizeof(x_response));
    if ((rxbuf!=NULL) && rxlen) {
        READ_XMOD_ARG_BUFF(bufrxptr, rxbuf, rxlen);
    }
    *rsp = LE2HOST32(x_response);

    return rv;
}
#endif

/*
 * Variable:
 *      phy8806x_drv
 * Purpose:
 *      Phy Driver for BCM8806X PHYs. 
 */
phy_driver_t phy_8806xdrv = {
    /* .drv_name                      = */ "BCM8806X PHYMOD PHY Driver",
    /* .pd_init                       = */ phy_8806x_init,
    /* .pd_reset                      = */ phy_null_reset,
    /* .pd_link_get                   = */ phy_8806x_link_get,
    /* .pd_enable_set                 = */ phy_8806x_enable_set,
    /* .pd_enable_get                 = */ phy_8806x_enable_get,
    /* .pd_duplex_set                 = */ phy_null_set,
    /* .pd_duplex_get                 = */ phy_8806x_duplex_get,
    /* .pd_speed_set                  = */ phy_8806x_speed_set,
    /* .pd_speed_get                  = */ phy_8806x_speed_get,
    /* .pd_master_set                 = */ phy_null_set,
    /* .pd_master_get                 = */ phy_null_zero_get,
    /* .pd_an_set                     = */ phy_8806x_an_set,
    /* .pd_an_get                     = */ phy_8806x_an_get,
    /* .pd_adv_local_set              = */ NULL, /* Deprecated */
    /* .pd_adv_local_get              = */ NULL, /* Deprecated */
    /* .pd_adv_remote_get             = */ NULL, /* Deprecated */ 
    /* .pd_lb_set                     = */ phy_8806x_lb_set,
    /* .pd_lb_get                     = */ phy_8806x_lb_get,
    /* .pd_interface_set              = */ phy_8806x_interface_set,
    /* .pd_interface_get              = */ phy_8806x_interface_get,
    /* .pd_ability                    = */ NULL, /* Deprecated */ 
    /* .pd_linkup_evt                 = */ phy_8806x_linkup,
    /* .pd_linkdn_evt                 = */ NULL,
    /* .pd_mdix_set                   = */ phy_null_mdix_set,
    /* .pd_mdix_get                   = */ phy_null_mdix_get,
    /* .pd_mdix_status_get            = */ phy_null_mdix_status_get,
    /* .pd_medium_config_set          = */ NULL,
    /* .pd_medium_config_get          = */ phy8806x_medium_config_get,
    /* .pd_medium_get                 = */ phy8806x_medium_get,
    /* .pd_cable_diag                 = */ NULL,
    /* .pd_link_change                = */ NULL,
    /* .pd_control_set                = */ phy_8806x_control_set,
    /* .pd_control_get                = */ phy_8806x_control_get,
    /* .pd_reg_read                   = */ phy_8806x_reg_read,
    /* .pd_reg_write                  = */ phy_8806x_reg_write,
    /* .pd_reg_modify                 = */ phy_8806x_reg_modify,
    /* .pd_notify                     = */ NULL,
    /* .pd_probe                      = */ phy_8806x_probe,
    /* .pd_ability_advert_set         = */ phy_8806x_ability_advert_set, 
    /* .pd_ability_advert_get         = */ phy_8806x_ability_advert_get,
    /* .pd_ability_remote_get         = */ phy_8806x_ability_remote_get,
    /* .pd_ability_local_get          = */ phy_8806x_ability_local_get,
    /* .pd_firmware_set               = */ phy_8806x_firmware_set,
    /* .pd_timesync_config_set        = */ NULL,
    /* .pd_timesync_config_get        = */ NULL,
    /* .pd_timesync_control_set       = */ NULL,
    /* .pd_timesync_control_set       = */ NULL,
    /* .pd_diag_ctrl                  = */ phy_8806x_diag_ctrl,
    /* .pd_lane_control_set           = */ NULL,    
    /* .pd_lane_control_get           = */ NULL,
    /* .pd_lane_reg_read              = */ NULL,
    /* .pd_lane_reg_write             = */ NULL,
    /* .pd_oam_config_set             = */ NULL,
    /* .pd_oam_config_get             = */ NULL,
    /* .pd_oam_control_set            = */ NULL,
    /* .pd_oam_control_get            = */ NULL,
    /* .pd_timesync_enhanced_capture_get = */ NULL,
    /* .pd_multi_get                  = */ phy_8806x_multi_reg_read,
    /* .pd_precondition_before_probe  = */ NULL,
    /* .pd_linkfault_get              = */ phy_8806x_linkfault_get
};

#else
typedef int _8806x_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_PHY_8806X */

