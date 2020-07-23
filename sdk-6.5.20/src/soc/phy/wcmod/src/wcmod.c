/*
 *  
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        wcmod.c
 * Purpose:     Broadcom WarpLite SerDes 
 *              (Warpcore 40nm with x1 and x4 lane support)
 */
#include <sal/types.h>
#include <sal/core/spl.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/phyreg.h>
#include <soc/port_ability.h>
#include <soc/phyctrl.h>

#include <soc/phy.h>
#include <soc/phy/phyctrl.h>
#include <soc/phy/drv.h>

#include "phydefs.h"      /* Must include before other phy related includes */

#if defined(INCLUDE_XGXS_WCMOD)
#include "phyconfig.h"     /* Must include before other phy related includes */
#include "phyreg.h"
#include "phyfege.h"
#include "phynull.h"
#include "serdesid.h"
#include "wcmod.h"
#include "wcmod_extra.h"
#include "wcmod_phyreg.h"
#include "wcmod_main.h"
#include "wcmod_defines.h"
#include "wcmod_functions.h"
#include "wcmod_diagnostics.h"



#define WCMOD_SDK32387_REVS(_pc) (WCMOD_REVID_A0(_pc) || WCMOD_REVID_A1(_pc) || WCMOD_REVID_B0(_pc))
#define WCMOD_PHY400_REVS(_pc) (WCMOD_REVID_B0(_pc))

/* static variables */
STATIC char wcmod_device_name[] = "WarpLite";


int (*_phy_wcmod_firmware_set_helper[SOC_MAX_NUM_DEVICES]) (int unit , int port, uint8 *arr,int arr_len) = {NULL};
 
/* uController's firmware */
extern uint8 wcmod_ucode_WarpLite_WarpCoreC0_revA0[];
extern int   wcmod_ucode_WarpLite_WarpCoreC0_revA0_len;

STATIC WCMOD_UCODE_DESC wcmod_ucodes[] = {
    {&wcmod_ucode_WarpLite_WarpCoreC0_revA0[0], &wcmod_ucode_WarpLite_WarpCoreC0_revA0_len, WCMOD_SERDES_ID0_REVID_A0},
    {&wcmod_ucode_WarpLite_WarpCoreC0_revA0[0], &wcmod_ucode_WarpLite_WarpCoreC0_revA0_len, WCMOD_SERDES_ID0_REVID_A1}
};
#define WCMOD_UCODE_NUM_ENTRIES  (sizeof(wcmod_ucodes)/sizeof(wcmod_ucodes[0]))

typedef struct wcmod_spd_intf_params_s {
    wcmod_spd_intfc_set spd_intf;
    int tx_inx;
    int turn_off_pll;
    int vco_freq;
    int pll_divider;
    int oversample_mode;
} wcmod_spd_intf_params_t;

/* function forward declaration */
STATIC int _wcmod_phy_parameter_copy(phy_ctrl_t *pc, wcmod_st *wc);
STATIC int _phy_wcmod_notify_duplex(int unit, soc_port_t port, uint32 duplex);
STATIC int _phy_wcmod_notify_speed(int unit, soc_port_t port, uint32 speed);
STATIC int _phy_wcmod_notify_stop(int unit, soc_port_t port, uint32 flags);
STATIC int _phy_wcmod_notify_resume(int unit, soc_port_t port, uint32 flags);
STATIC int _phy_wcmod_notify_interface(int unit, soc_port_t port, uint32 intf);
STATIC int _phy_wcmod_control_tx_driver_set(int unit, phy_ctrl_t *pc,
                                soc_phy_control_t type, uint32 value);
STATIC int _phy_wcmod_control_preemphasis_set(int unit, phy_ctrl_t *pc,
                                soc_phy_control_t type, uint32 value);
STATIC int _phy_wcmod_combo_speed_get(int unit, soc_port_t port, int *speed,
                                     int *intf, int *scr);
STATIC int _phy_wcmod_ind_speed_get(int unit, soc_port_t port, int *speed,
                                   int *intf, int *scr);
STATIC int phy_wcmod_an_set(int unit, soc_port_t port, int an);
STATIC int phy_wcmod_an_set2(int unit, soc_port_t port, int an);
STATIC int phy_wcmod_lb_set(int unit, soc_port_t port, int enable);
STATIC int phy_wcmod_ability_advert_set(int unit, soc_port_t port,
                                        soc_port_ability_t *ability);
STATIC int phy_wcmod_ability_local_get(int unit, soc_port_t port,
                                       soc_port_ability_t *ability);
STATIC int phy_wcmod_speed_set(int unit, soc_port_t port, int speed);
STATIC int phy_wcmod_speed_get(int unit, soc_port_t port, int *speed);
STATIC int phy_wcmod_an_get(int unit, soc_port_t port, int *an, int *an_done);
STATIC int _wcmod_soft_reset(int unit, phy_ctrl_t  *pc);
STATIC int phy_wcmod_reg_aer_read(int unit, phy_ctrl_t *pc, uint32 flags,
                  uint32 phy_reg_addr, uint16 *phy_data);
STATIC int phy_wcmod_reg_aer_write(int unit, phy_ctrl_t *pc,  uint32 flags,
                   uint32 phy_reg_addr, uint16 phy_data);
STATIC int phy_wcmod_reg_aer_modify(int unit, phy_ctrl_t *pc,  uint32 flags,
                    uint32 phy_reg_addr, uint16 phy_data,uint16 phy_data_mask);
STATIC int _wcmod_chip_init_done(int unit,int chip_num,int phy_mode);
STATIC int phy_wcmod_firmware_load(int unit, int port, wcmod_st *ws, int offset, 
                                 uint8 *array,int datalen);
STATIC int phy_wcmod_lb_get(int unit, soc_port_t port, int *enable);
STATIC int phy_wcmod_lb_raw_get(int unit, soc_port_t port, int *enable);
STATIC int phy_wcmod_diag_ctrl(int, soc_port_t, uint32, int, int, void *); 
STATIC int phy_wcmod_100g_plus_config(int unit, soc_port_t port, int speed);
STATIC int _phy_wcmod_rx_polarity_set(int unit, phy_ctrl_t *pc, uint32 enable);
STATIC int _phy_wcmod_tx_polarity_set(int unit, phy_ctrl_t *pc, uint32 enable);
STATIC int _phy_wcmod_ind_speed_set(int unit, soc_port_t port, int speed);
STATIC int _phy_wcmod_speed_set(int unit, soc_port_t port, int speed);
STATIC int phy_wcmod_interlaken_config(int unit, soc_port_t port, int speed);
STATIC int _phy_wcmod_multi_core_polarity_config(int unit, soc_port_t port);
STATIC int _phy_wcmod_mld_lane_swap_config(int unit, soc_port_t port);
STATIC int _phy_wcmod_ind_speed_intf_get(int unit, soc_port_t port, int speed, wcmod_spd_intf_params_t* spd_params);
STATIC int _phy_wcmod_speed_intf_get(int unit, soc_port_t port, int speed, wcmod_spd_intfc_set* spd_intf, int* tx_index);
STATIC int phy_wcmod_xgxs16g1l_an_set(int unit, soc_port_t port, int an) ;
STATIC int phy_wcmod_control_set(int unit, soc_port_t port,soc_phy_control_t type, uint32 value);
STATIC int phy_wcmod_control_get(int unit, soc_port_t port,soc_phy_control_t type, uint32 *value);
STATIC int _phy_wcmod_control_vco_disturbed_set(int unit, soc_port_t port);
STATIC int _phy_wcmod_control_firmware_mode_set(int unit, phy_ctrl_t *pc, uint32 value);
STATIC int _phy_wcmod_control_prbs_decouple_tx_invert_data_get(wcmod_st *ws, uint32 *value);
STATIC int _phy_wcmod_control_prbs_decouple_tx_polynomial_get(wcmod_st *ws, uint32 *value);
STATIC int _phy_wcmod_control_prbs_decouple_tx_enable_get(wcmod_st *ws, uint32 *value);
STATIC int _phy_wcmod_control_prbs_decouple_rx_invert_data_get(wcmod_st *ws, uint32 *value);
STATIC int _phy_wcmod_control_prbs_decouple_rx_polynomial_get(wcmod_st *ws, uint32 *value);
STATIC int _phy_wcmod_control_prbs_decouple_rx_enable_get(wcmod_st *ws, uint32 *value);
int wcmod_eye_margin(int unit, soc_port_t port, int type);
STATIC int phy_wcmod_medium_get(int unit, soc_port_t port, soc_port_medium_t *medium);


#if 0 
/* debug info */
extern void wcmod_ability_speed_print(soc_port_ability_t * ability);
extern int wcmod_control_info_set(int unit, soc_port_t port, uint32 info_val);
extern int wcmod_control_info_get(int unit, soc_port_t port, uint32 *info_val);
extern int wcmod_info_reg_get(wcmod_st *ws, int lane, uint32 addr) ;
extern int wcmod_info_reg_set(wcmod_st *ws, int lane, uint32 addr, uint16 data);
extern int wcmod_info_get(wcmod_st *ws) ;
#endif

STATIC int 
_wcmod_phy_parameter_copy(phy_ctrl_t  *pc, wcmod_st *wc)
{
    int refclk = 0 ;
    if ((pc == NULL) || (wc == NULL))
    {
        return SOC_E_EMPTY;    
    }
    else
    {
       if (IS_MULTI_CORE_PORT(pc)) {
            phyident_core_info_t core_info[8];
            int j, num_cores;  
            int array_max = 8;
            int array_size = 0;
            wcmod_st    *temp_ws;    

            num_cores = 0;
            SOC_IF_ERROR_RETURN(soc_phy_addr_multi_get(pc->unit, pc->port, array_max, &array_size, &core_info[0]));
            for(j = 0 ; j < array_size ; j++){
                temp_ws   =  (wcmod_st *)( wc + j);
                if(core_info[j].core_type == phyident_core_type_wc) {
                    temp_ws->this_lane = core_info[j].first_phy_in_core;
                    switch (temp_ws->this_lane) {
                    case 0: 
                        temp_ws->lane_select = WCMOD_LANE_0_0_0_1;
                        break;
                    case 1: 
                        temp_ws->lane_select = WCMOD_LANE_0_0_1_0;
                        break;
                    case 2: 
                        temp_ws->lane_select = WCMOD_LANE_0_1_0_0;
                        break;
                    case 3: 
                        temp_ws->lane_select = WCMOD_LANE_1_0_0_0;
                        break;
                    default:
                        break;
                    }
                    temp_ws->num_of_lane = core_info[j].nof_lanes_in_core;
                    temp_ws->unit = pc->unit;
                    temp_ws->port = pc->port;
                    temp_ws->phy_ad =  core_info[j].mdio_addr;
                    temp_ws->mdio_type = WCMOD_MDIO_CL22;
                    temp_ws->read = pc->read;
                    temp_ws->write = pc->write;
                    temp_ws->read = pc->read;
                    temp_ws->refclk = DEV_CFG_PTR(pc)->refclk ;
                    refclk = DEV_CFG_PTR(pc)->refclk ;
                    if (SOC_IS_ARAD(pc->unit) || SOC_IS_FE1600(pc->unit)) {
                        if( refclk == 0 ) {
                            temp_ws->refclk = 156 ;
                        } else if (refclk == 1 ) {
                            temp_ws->refclk = 125 ;
                        } else {
                            temp_ws->refclk = refclk ;
                        }
                    } else {
                        temp_ws->refclk = 156 ;
                    }
                    temp_ws->firmware_mode = DEV_CFG_PTR(pc)->firmware_mode;
                    num_cores++;
                } else {
                /*this mld info */
                    temp_ws->phy_ad =  core_info[j].mdio_addr;
                    temp_ws->mdio_type = WCMOD_MDIO_CL22;
                    temp_ws->read = pc->read;
                    temp_ws->write = pc->write;
                    temp_ws->read = pc->read;
                    temp_ws->unit = pc->unit;
                    temp_ws->port = pc->port;
                    temp_ws->this_lane = 0;
                }
            }
       } else {
            wc->unit       = pc->unit;
            wc->port       = pc->port;
            wc->phy_ad     = pc->phy_id;
            if (PHY_INDEPENDENT_LANE_MODE(pc->unit, pc->port)) {
                wc->port_type  = WCMOD_INDEPENDENT;
                if (IS_DUAL_LANE_PORT(pc)) {
                    /* wc->dxgxs is for broadcast mode */
                    wc->dual_type = (pc->lane_num) ? 2 : 1 ;  
                    /* dual_type is copied to dxgxs when dual bcst reg access if possible */
                    wc->dxgxs     = wc->dual_type ;
                } 
            }  else {
                if(PHYCTRL_MULTI_CORE_PORT != pc->phy_mode) {
                   wc->port_type  = WCMOD_COMBO;
                }
            }
              wc->mdio_type  = WCMOD_MDIO_CL22;
            switch (pc->lane_num) {
            case 0: 
                wc->lane_select = WCMOD_LANE_0_0_0_1;
                break;
            case 1: 
                wc->lane_select = WCMOD_LANE_0_0_1_0;
                break;
            case 2: 
                wc->lane_select = WCMOD_LANE_0_1_0_0;
                break;
            case 3: 
                wc->lane_select = WCMOD_LANE_1_0_0_0;
                break;
            case 4: 
                wc->lane_select = WCMOD_LANE_0_0_0_1_0_0_0_0;
                break;
            case 5: 
                wc->lane_select = WCMOD_LANE_0_0_1_0_0_0_0_0;
                break;
            case 6: 
                wc->lane_select = WCMOD_LANE_0_1_0_0_0_0_0_0;
                break;
            case 7: 
                wc->lane_select = WCMOD_LANE_1_0_0_0_0_0_0_0;
                break;
            default:
                return SOC_E_PARAM;
            }
            wc->platform_info = pc;
            wc->this_lane = pc->lane_num;
            wc->read = pc->read;
            wc->write = pc->write;

            wc->refclk = DEV_CFG_PTR(pc)->refclk ;
            refclk = DEV_CFG_PTR(pc)->refclk ;
            if (SOC_IS_ARAD(pc->unit) || SOC_IS_FE1600(pc->unit)) {
                if( refclk == 0 ) {
                    wc->refclk = 156 ;
                } else if (refclk == 1 ) {
                    wc->refclk = 125 ;
                } else {
                    wc->refclk = refclk ;
                }
            } else {
                wc->refclk = 156 ;
            }
            wc->firmware_mode = DEV_CFG_PTR(pc)->firmware_mode;
        }
        return SOC_E_NONE;
    }
}


STATIC int
_phy_wcmod_ucode_get(int unit, soc_port_t port, uint8 **ppdata, int *len, 
                    int *mem_alloced)
{
    int ix;
    phy_ctrl_t     *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    *mem_alloced = FALSE;
    *ppdata = NULL;

    /* check which firmware to load */
    for (ix = 0; ix < WCMOD_UCODE_NUM_ENTRIES; ix++) {
        if (wcmod_ucodes[ix].chip_rev == WCMOD_REVID(pc)) {
            break;
        }
    }
    if (ix >= WCMOD_UCODE_NUM_ENTRIES) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(pc->unit,
                             "no firmware matches the chip rev number!!! use default\n")));
        ix = WCMOD_UCODE_NUM_ENTRIES - 1;
    }  
    for (; ix >= 0; ix--) {
        if ((wcmod_ucodes[ix].pdata != NULL) && 
            (*(wcmod_ucodes[ix].plen) != 0)) {
            break;
        }
    }
    if (ix < 0) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(pc->unit,
                             "no valid firmware found!!!\n")));
        return SOC_E_NOT_FOUND;
    }
    
    *ppdata = wcmod_ucodes[ix].pdata;
    *len = *(wcmod_ucodes[ix].plen);
    return SOC_E_NONE;
}



STATIC int
_init_wcmod_st(wcmod_st *pc)
{
      /* printf("%s: Initializing wcmod_st object\n", FUNCTION_NAME()); */
      pc->id                 = 0x33333333;
      pc->unit               = 0;
      pc->port               = 0;
      pc->phy_ad             = 0;
      pc->prt_ad             = 0;
      pc->this_lane          = 0;
      pc->lane_select        = WCMOD_LANE_ILLEGAL;
      pc->lane_num_ignore    = 0;
      pc->per_lane_control   = 0;
      pc->en_dis             = 0;
      pc->aer_ofs_strap      = 0;
      pc->multiport_addr_en  = 0; 
      pc->aer_bcst_ofs_strap = 0x1ff; /* good for TR3 and FE1600 */
    #if defined (_MDK_WCMOD_)
      pc->aer_bcst_ofs_strap = 0x1ff; /* only guaranteed for Radian. */
    #endif
      pc->spd_intf           = WCMOD_SPD_ILLEGAL;
      pc->mdio_type          = WCMOD_MDIO_CL22;
      pc->port_type          = WCMOD_PORT_MODE_ILLEGAL;
      pc->os_type            = WCMOD_OS_ILLEGAL;
      pc->dxgxs              = 0;
      pc->dual_type          = 0;
      pc->wc_touched         = 0;
      pc->lane_swap          = 0x32103210;
      pc->duplex             = 0;
      pc->accAddr            = 0;
      pc->accData            = 0;
      pc->diag_type          = WCMOD_DIAG_ILLEGAL;
      pc->model_type         = WCMOD_MODEL_TYPE_ILLEGAL;
      pc->an_type            = WCMOD_AN_TYPE_ILLEGAL;
      pc->an_rf              = 0;
      pc->an_pause           = 0;
      pc->an_tech_ability    = 0;
      pc->an_fec             = 0;
      pc->platform_info      = 0; /* this will be initialized later. */
      pc->verbosity          = 0;
      pc->asymmetric_mode    = 0;
      pc->s100g_plus         = 0;
      pc->wcmod_sdk_ctrl_type = 0x77777777;
      if (pc->verbosity > 0) printf("%s: Done Initializing wcmod_st object\n", FUNCTION_NAME());
      return SOC_E_NONE;
}

int 
soc_phy_set_verbose(int unit, int port, int verbose)
{
    phy_ctrl_t      *pc;
    WCMOD_DEV_DESC_t *pDesc;
    wcmod_st    *ws;    

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    ws->verbosity = verbose;

    return SOC_E_NONE;

}

STATIC int
_phy_wcmod_config_init(int unit, soc_port_t port)
{
     phy_ctrl_t      *pc;      
    WCMOD_DEV_CFG_t *pCfg;
    WCMOD_DEV_INFO_t  *pInfo;
    WCMOD_DEV_DESC_t *pDesc;
     wcmod_st    *ws, *temp_ws;  
    soc_phy_info_t *pi;
    int len, num_core, num_mld, loop, num_lanes;
    uint16 serdes_id0, chip_num;
    int i, lane, tx_lane_swap, rx_lane_swap, preemphasis, pdriver, idriver, post2_driver;    
    WCMOD_TX_DRIVE_t *p_tx;
    uint16 data16;

    pc = INT_PHY_SW_STATE(unit, port);

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);
    temp_ws = ws;
    num_mld = 0;
    num_lanes = 4;

    /* clear the memory */
    WCMOD_MEM_SET(pDesc, 0, sizeof(*pDesc));
    num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
    if ((SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) || (SOC_INFO(unit).port_num_lanes[pc->port] >= 10)) {
        num_mld = (SOC_INFO(unit).port_num_lanes[pc->port] + 11) / 12;
    }
    /*will be true for Special ILKLN because num_of_mld=1 and num_core =1 or 2 and together we get 2 or 3 as expected*/
    for (i = 0; i < num_core + num_mld; i++) {
        temp_ws   =  (wcmod_st *)( ws + i);
        WCMOD_MEM_SET(temp_ws, 0, sizeof(*temp_ws));
        /* initialize the ws*/
        SOC_IF_ERROR_RETURN
            (_init_wcmod_st(temp_ws));
    }

    pCfg = &pDesc->cfg; 
    pInfo = &pDesc->info;
    /*added for the ws structure init. */
    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    SOC_IF_ERROR_RETURN
        (READ_WC40_SERDESID_SERDESID0r(unit, ws,  &serdes_id0));
    pInfo->serdes_id0 = serdes_id0;

    /* init the default configuration values */
    for (i = 0; i < MAX_NUM_LANES; i++) {
        pCfg->preemph[i] = WCMOD_NO_CFG_VALUE;
        pCfg->idriver[i] = WCMOD_NO_CFG_VALUE;
        pCfg->pdriver[i] = WCMOD_NO_CFG_VALUE;
        pCfg->post2driver[i] = WCMOD_NO_CFG_VALUE;
    }
    /* init the speed interface */
    pCfg->speed = WCMOD_NO_CFG_VALUE;
    pCfg->max_speed = WCMOD_NO_CFG_VALUE;

    /* put the default line interface */
    pCfg->line_intf = WCMOD_IF_NULL;

   /* default */
    p_tx = &pCfg->tx_drive[TXDRV_DFT_INX];
    p_tx->u.tap.post  = 0x00;
    p_tx->u.tap.main  = 0x00;
    p_tx->u.tap.pre   = 0x00;
    p_tx->u.tap.force = 0x00;
    p_tx->post2     = 0x00;
    p_tx->idrive    = 0x09;
    p_tx->ipredrive = 0x09;

    pCfg->hg_mode    = FALSE;
    pCfg->sgmii_mstr = FALSE;
    pCfg->pdetect10g = TRUE;
    pCfg->cx42hg     = FALSE;
    pCfg->asymmetric_mode = FALSE;
    /* initialize  all the warpore lane swap */
    for (i = 0; i < 6; i++) {
        pCfg->rxlane_map[i] = WCMOD_RX_LANE_MAP_DEFAULT;
        pCfg->txlane_map[i] = WCMOD_TX_LANE_MAP_DEFAULT;
    }
    pCfg->medium = SOC_PORT_MEDIUM_FIBER;
    pCfg->cx4_10g    = TRUE;
    pCfg->lane0_rst  = TRUE;  
    pCfg->rxaui      = FALSE;
    pCfg->rxaui_mode = 1;
    pCfg->load_mthd = 2;             
    pCfg->uc_cksum  = 0;
    if(SOC_IS_SAND(unit)) {
        pCfg->refclk = SOC_INFO(unit).port_refclk_int[pc->port];
    } else {
        pCfg->refclk    = 156;
    }

    /* intialize all the MLD lane swap variable */
    for (i = 0; i < 48; i++) {
        pCfg->mld_lane_swap[i] = i % 24;
    }
    /*initialize the 100G active lane map */
    pCfg->active_lane_map = 0x3ff;
    /* intialize the pcs_bypass  variable */
    pCfg->pcs_bypass = 0;
    /* initialize the scrambler_enable */
    pCfg->scrambler_en = 0;
    /* initialize the firmware mode */
    /* use force-dfe for ILKN port */
    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), port)) {
        pCfg->firmware_mode = SOC_PHY_FIRMWARE_FORCE_OSDFE;
    } else if (SOC_IS_ARAD(unit) && SOC_PBMP_MEMBER(PBMP_XL_ALL(unit), port)) {
        pCfg->firmware_mode = SOC_PHY_FIRMWARE_XLAUI;
    } else {
        pCfg->firmware_mode = SOC_PHY_FIRMWARE_DEFAULT;
    }

    /* initialize the serdes_1000x_at_6250_vco to be 0 */
    pCfg->serdes_1000x_at_6250_vco = 0;
    pCfg->serdes_1000x_at_6250_vco = soc_property_port_get(unit, port,
                                        spn_SERDES_1000X_AT_6250_VCO,
                                        pCfg->serdes_1000x_at_6250_vco);

    /* this property is for debug and diagnostic purpose. byte0: 
     * 0: not loading WC firmware
     * 1: load from MDIO. default method.
     * 2: load from parallel bus if applicable. Provide fast downloading time
     * 
     * byte1:
     * 0: inform uC not to perform checksum calculation(default). Save ~70ms for WC init time
     * 1: inform uC to perform checksum calculation. 
     */
    pCfg->load_mthd = soc_property_port_get(unit, port, spn_LOAD_FIRMWARE, pCfg->load_mthd);
    pCfg->load_mthd &= 0xff;
    pCfg->uc_cksum = soc_property_port_get(unit, port, spn_LOAD_FIRMWARE, pCfg->uc_cksum);
    pCfg->uc_cksum = (pCfg->uc_cksum >> 8) & 0xff;

    /* set the default value for the sw_rx_los */
    DEV_CFG_PTR(pc)->sw_rx_los.enable      = FALSE ;
    DEV_CFG_PTR(pc)->sw_rx_los.sys_link    = 0;
    DEV_CFG_PTR(pc)->sw_rx_los.state       = RESET;
    DEV_CFG_PTR(pc)->sw_rx_los.link_status = 0;
    DEV_CFG_PTR(pc)->sw_rx_los.rf_dis      = 0;

    /* set the default value for the MLD_LINK_WAR */
    DEV_CFG_PTR(pc)->mld_war_timer_count = 0;
 
    serdes_id0 = pInfo->serdes_id0;

    if (SOC_IS_TRIUMPH3(unit) || SOC_IS_ARAD(unit) || SOC_IS_HURRICANE2(unit)) {
        soc_info_t *si;
        int phy_port;  /* physical port number */

        si = &SOC_INFO(unit);
        if (soc_feature(unit, soc_feature_logical_port_num)) {
            phy_port = si->port_l2p_mapping[port];
        } else {
            phy_port = port;
        }
        pc->lane_num = SOC_PORT_BINDEX(unit, phy_port);
        pc->chip_num = SOC_BLOCK_NUMBER(unit, SOC_PORT_BLOCK(unit, phy_port));
        num_lanes = si->port_num_lanes[port];
        /* convert to phy_ctrl macros */
        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) {
            pc->phy_mode = PHYCTRL_MULTI_CORE_PORT;
            pc->flags |= PHYCTRL_MDIO_ADDR_SHARE;
        } else if (si->port_num_lanes[port] == 4) {
            pc->phy_mode = PHYCTRL_QUAD_LANE_PORT;
        } else if (si->port_num_lanes[port] == 2) {
            pc->phy_mode = PHYCTRL_DUAL_LANE_PORT;
            pc->flags |= PHYCTRL_MDIO_ADDR_SHARE;
        } else if (si->port_num_lanes[port] == 1) {
            pc->phy_mode = PHYCTRL_ONE_LANE_PORT;
            pc->flags |= PHYCTRL_MDIO_ADDR_SHARE;
        } else if (si->port_num_lanes[port] >= 10) {
            pc->phy_mode = PHYCTRL_MULTI_CORE_PORT;
            pc->flags |= PHYCTRL_MDIO_ADDR_SHARE;
        }
        if ((serdes_id0 & SERDESID_SERDESID0_MODEL_NUMBER_MASK) == MODEL_QSGMII) {
            if (SOC_IS_HURRICANE2(unit)) {
                pc->lane_num = (phy_port - 2) % 8;
                pc->chip_num = (phy_port - 2) / 8;
            } else {
                pc->lane_num = (phy_port - 1) % 8;
                pc->chip_num = (phy_port - 1) / 8;
            }   

        } else if ((serdes_id0 & SERDESID_SERDESID0_MODEL_NUMBER_MASK) == MODEL_WARPLITE){
            pc->lane_num = (phy_port) % 4;
            pc->chip_num = (phy_port) / 4;
        } else if (SOC_IS_HELIX4(unit) && 
                  ((serdes_id0 & SERDESID_SERDESID0_MODEL_NUMBER_MASK) == MODEL_QUADSGMII)) {
            pc->lane_num = (phy_port - 3) % 4;
            pc->chip_num = (phy_port) / 4;
        } else {
            pc->lane_num = (phy_port - 1) % 4;
            pc->chip_num = (phy_port - 1) / 4;
        }
    }

    if (SOC_IS_DFE(unit)) {
        soc_info_t *si;
        int phy_port;  /* physical port number */

        si = &SOC_INFO(unit);
        if (soc_feature(unit, soc_feature_logical_port_num)) {
            phy_port = si->port_l2p_mapping[port];
        } else {
            phy_port = port;
        }
        pc->lane_num = SOC_PORT_BINDEX(unit, phy_port);
        pc->chip_num = SOC_BLOCK_NUMBER(unit, SOC_PORT_BLOCK(unit, phy_port));
        num_lanes = si->port_num_lanes[port];

        /* convert to phy_ctrl macros */
        if (si->port_num_lanes[port] == 4) {
            pc->phy_mode = PHYCTRL_QUAD_LANE_PORT;
        } else if (si->port_num_lanes[port] == 2) {
            pc->phy_mode = PHYCTRL_DUAL_LANE_PORT;
            pc->flags |= PHYCTRL_MDIO_ADDR_SHARE;
        } else if (si->port_num_lanes[port] == 1) {
            pc->phy_mode = PHYCTRL_ONE_LANE_PORT;
            pc->flags |= PHYCTRL_MDIO_ADDR_SHARE;
        }
        pc->lane_num = (phy_port ) % 4;
        pc->chip_num = (phy_port) / 4;
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_INDEPENDENT_LANE);
    }

    /* retrieve chip level information for warpcore */

    pi       = &SOC_PHY_INFO(unit, port);


    if ((serdes_id0 & SERDESID_SERDESID0_MODEL_NUMBER_MASK) == MODEL_WARPCORE_C) {
        sal_strcpy(pInfo->name,"WC-");
        len = sal_strlen(pInfo->name);
        /* add rev letter */
        pInfo->name[len++] = 'C' + ((serdes_id0 >>
                              SERDESID_SERDESID0_REV_LETTER_SHIFT) & 0x3);
        /* add rev number */
        pInfo->name[len++] = '0' + ((serdes_id0 >>
                              SERDESID_SERDESID0_REV_NUMBER_SHIFT) & 0x7);
        pInfo->name[len++] = '/';
        
        if(SOC_IS_TRIUMPH3(unit) && (pc->chip_num > 12)) {
            /* XC = 0-12, WC= 13-19*/
            chip_num = pc->chip_num - 13;
        } else {
            chip_num = pc->chip_num;
        }
        pInfo->name[len++] = (chip_num / 10) % 10 + '0';
        pInfo->name[len++] = chip_num % 10 + '0';
        pInfo->name[len++] = '/';

        /* phy_mode: 0 single port mode, port uses all four lanes of Warpcore
         *           1 dual port mode, port uses 2 lanes
         *           2 quad port mode, port uses 1 lane
         */
        if (IS_DUAL_LANE_PORT(pc)) { /* dual-xgxs mode */
            if (pc->lane_num < 2) { /* the first dual-xgxs port */
                pInfo->name[len++] = '0';
                pInfo->name[len++] = '-';
                pInfo->name[len++] = '1';
            } else {
                pInfo->name[len++] = '2';
                pInfo->name[len++] = '-';
                pInfo->name[len++] = '3';
            }
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_INDEPENDENT_LANE);
        } else if (pc->phy_mode == PHYCTRL_ONE_LANE_PORT || CUSTOM_MODE(pc)) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_INDEPENDENT_LANE);
            pInfo->name[len++] = pc->lane_num + '0';
        } else {
            if (num_lanes <= 9) {
                pInfo->name[len++] = '0' + num_lanes;
            } else {
                pInfo->name[len++] = '1';
                pInfo->name[len++] = '0' + num_lanes - 10;
            }
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_INDEPENDENT_LANE);
        }
        pInfo->name[len] = 0;  /* string terminator */

        if (len > WCMOD_LANE_NAME_LEN) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(pc->unit,
                                  "WC info string length %d exceeds max length 0x%x: u=%d p=%d\n"),
                       len,WCMOD_LANE_NAME_LEN,unit, port));
            return SOC_E_MEMORY;
        }
    }
    else if ((serdes_id0 & SERDESID_SERDESID0_MODEL_NUMBER_MASK) == MODEL_WARPLITE) {
        sal_strcpy(pInfo->name,"WL-");
        len = sal_strlen(pInfo->name);
        /* add rev letter */
        pInfo->name[len++] = 'A' + ((serdes_id0 >>
                              SERDESID_SERDESID0_REV_LETTER_SHIFT) & 0x3);
        /* add rev number */
        pInfo->name[len++] = '0' + ((serdes_id0 >>
                              SERDESID_SERDESID0_REV_NUMBER_SHIFT) & 0x7);
        pInfo->name[len++] = '/';
        pInfo->name[len++] = (pc->chip_num / 10) % 10 + '0';
        pInfo->name[len++] = pc->chip_num % 10 + '0';
        pInfo->name[len++] = '/';
        pInfo->name[len] = 0;  /* string terminator */        

        PHY_FLAGS_SET(unit, port, PHY_FLAGS_INDEPENDENT_LANE);

        if (len > WCMOD_LANE_NAME_LEN) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(pc->unit,
                                  "WC info string length %d exceeds max length 0x%x: u=%d p=%d\n"),
                       len,WCMOD_LANE_NAME_LEN,unit, port));
            return SOC_E_MEMORY;
        }
    }
    
    if ((serdes_id0 & SERDESID_SERDESID0_MODEL_NUMBER_MASK) == MODEL_XGXS_16G) {
        sal_strcpy(pInfo->name,"XGXS13G-");
        len = sal_strlen(pInfo->name);
        /* add rev letter */
        pInfo->name[len++] = 'A' + ((serdes_id0 >>
                              SERDESID_SERDESID0_REV_LETTER_SHIFT) & 0x3);
        /* add rev number */
        pInfo->name[len++] = '0' + ((serdes_id0 >>
                              SERDESID_SERDESID0_REV_NUMBER_SHIFT) & 0x7);
        pInfo->name[len++] = '/';
        pInfo->name[len++] = (pc->chip_num / 10) % 10 + '0';
        pInfo->name[len++] = pc->chip_num % 10 + '0';
        pInfo->name[len++] = '/';

        /* phy_mode: 0 single port mode, port uses all four lanes of Warpcore
         *           1 dual port mode, port uses 2 lanes
         *           2 quad port mode, port uses 1 lane
         */
        if (IS_DUAL_LANE_PORT(pc)) { /* dual-xgxs mode */
            if (pc->lane_num < 2) { /* the first dual-xgxs port */
                pInfo->name[len++] = '0';
                pInfo->name[len++] = '-';
                pInfo->name[len++] = '1';
            } else {
                pInfo->name[len++] = '2';
                pInfo->name[len++] = '-';
                pInfo->name[len++] = '3';
            }
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_INDEPENDENT_LANE);
        } else if (pc->phy_mode == PHYCTRL_ONE_LANE_PORT || CUSTOM_MODE(pc)) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_INDEPENDENT_LANE);
            pInfo->name[len++] = pc->lane_num + '0';
        } else {
            pInfo->name[len++] = '4';
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_INDEPENDENT_LANE);
        }
        pInfo->name[len] = 0;  /* string terminator */

        if (len > WCMOD_LANE_NAME_LEN) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(pc->unit,
                                  "XGXS13G info string length %d exceeds max length 0x%x: u=%d p=%d\n"),
                       len,WCMOD_LANE_NAME_LEN,unit, port));
            return SOC_E_MEMORY;
        }
    }

    if ((serdes_id0 & SERDESID_SERDESID0_MODEL_NUMBER_MASK) == MODEL_QSGMII) {
        sal_strcpy(pInfo->name,"QSGMII-");
        len = sal_strlen(pInfo->name);
        /* add rev letter */
        pInfo->name[len++] = 'A' + ((serdes_id0 >>
                              SERDESID_SERDESID0_REV_LETTER_SHIFT) & 0x3);
        /* add rev number */
        pInfo->name[len++] = '0' + ((serdes_id0 >>
                              SERDESID_SERDESID0_REV_NUMBER_SHIFT) & 0x7);
        pInfo->name[len++] = '/';
        pInfo->name[len++] = (pc->chip_num / 10) % 10 + '0';
        pInfo->name[len++] = pc->chip_num % 10 + '0';
        pInfo->name[len++] = '/';

        /* phy_mode: 0 single port mode, port uses all four lanes of Warpcore
         *           1 dual port mode, port uses 2 lanes
         *           2 quad port mode, port uses 1 lane
         */
        if (IS_DUAL_LANE_PORT(pc)) { /* dual-xgxs mode */
            if (pc->lane_num < 2) { /* the first dual-xgxs port */
                pInfo->name[len++] = '0';
                pInfo->name[len++] = '-';
                pInfo->name[len++] = '1';
            } else {
                pInfo->name[len++] = '2';
                pInfo->name[len++] = '-';
                pInfo->name[len++] = '3';
            }
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_INDEPENDENT_LANE);
        } else if (pc->phy_mode == PHYCTRL_ONE_LANE_PORT || CUSTOM_MODE(pc)) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_INDEPENDENT_LANE); 
            pInfo->name[len++] = pc->lane_num + '0';
        } else {
            pInfo->name[len++] = '4';
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_INDEPENDENT_LANE);
        }
        pInfo->name[len] = 0;  /* string terminator */

        if (len > WCMOD_LANE_NAME_LEN) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(pc->unit,
                                  "QSGMII info string length %d exceeds max length 0x%x: u=%d p=%d\n"),
                       len,WCMOD_LANE_NAME_LEN,unit, port));
            return SOC_E_MEMORY;
        }
    }

    if ((serdes_id0 & SERDESID_SERDESID0_MODEL_NUMBER_MASK) == MODEL_QUADSGMII) {
        sal_strcpy(pInfo->name,"QUADSGMII-");
        len = sal_strlen(pInfo->name);
        /* add rev letter */
        pInfo->name[len++] = 'A' + ((serdes_id0 >>
                              SERDESID_SERDESID0_REV_LETTER_SHIFT) & 0x3);
        /* add rev number */
        pInfo->name[len++] = '0' + ((serdes_id0 >>
                              SERDESID_SERDESID0_REV_NUMBER_SHIFT) & 0x7);
        pInfo->name[len++] = '/';
        pInfo->name[len++] = (pc->chip_num / 10) % 10 + '0';
        pInfo->name[len++] = pc->chip_num % 10 + '0';
        pInfo->name[len++] = '/';

        /* phy_mode: 0 single port mode, port uses all four lanes of Warpcore
         *           1 dual port mode, port uses 2 lanes
         *           2 quad port mode, port uses 1 lane
         */
        if (IS_DUAL_LANE_PORT(pc)) { /* dual-xgxs mode */
            if (pc->lane_num < 2) { /* the first dual-xgxs port */
                pInfo->name[len++] = '0';
                pInfo->name[len++] = '-';
                pInfo->name[len++] = '1';
            } else {
                pInfo->name[len++] = '2';
                pInfo->name[len++] = '-';
                pInfo->name[len++] = '3';
            }
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_INDEPENDENT_LANE);
        } else if (pc->phy_mode == PHYCTRL_ONE_LANE_PORT || CUSTOM_MODE(pc)) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_INDEPENDENT_LANE);
            pInfo->name[len++] = pc->lane_num + '0';
        } else {
            pInfo->name[len++] = '4';
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_INDEPENDENT_LANE);
        }
        pInfo->name[len] = 0;  /* string terminator */

        if (len > WCMOD_LANE_NAME_LEN) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(pc->unit,
                                  "QUADSGMII info string length %d exceeds max length 0x%x: u=%d p=%d\n"),
                       len,WCMOD_LANE_NAME_LEN,unit, port));
            return SOC_E_MEMORY;
        }
    }

    if (!PHY_EXTERNAL_MODE(unit, port)) {
        pi->phy_name = pInfo->name;
    }

    if (CUSTOM_MODE(pc)) {
        pCfg->custom = pc->phy_mode;
    }
    if (CUSTOM1_MODE(pc)) {
        pCfg->custom1 = pc->phy_mode;
    }

    pCfg->hg_mode = IS_HG_PORT(unit,port);
    if (PHY_EXTERNAL_MODE(unit, port)) {
        /* always IEEE speed mode if connected with an external PHY */
        pCfg->hg_mode = FALSE;
    }

    pCfg->phy_sgmii_an = FALSE;
    pCfg->forced_init_cl72 = FALSE;

    if (PHY_INDEPENDENT_LANE_MODE(unit, port)) {
        if ((serdes_id0 & SERDESID_SERDESID0_MODEL_NUMBER_MASK) == MODEL_XGXS_16G) {
            pCfg->lane_mode = xgxs_operationModes_XGXS; 
        } else { 

            if (PHY_EXTERNAL_MODE(unit, port)) {
                pCfg->line_intf =  WCMOD_IF_XFI;
            } else {
                pCfg->line_intf =  WCMOD_IF_SFI;
            }
            pCfg->auto_medium = FALSE;
            if (CUSTOM_MODE(pc) || CUSTOM1_MODE(pc)) {
                pCfg->lane_mode = xgxs_operationModes_IndLane_OS5;
            } else {
                pCfg->lane_mode = xgxs_operationModes_IndLane_OS8;
            }
        }

    } else {
        if ((serdes_id0 & SERDESID_SERDESID0_MODEL_NUMBER_MASK) == MODEL_XGXS_16G) {
            pCfg->line_intf =  WCMOD_IF_CR4;
        } else {        
            pCfg->line_intf =  WCMOD_IF_XLAUI;
        }
        pCfg->auto_medium = TRUE;
        pCfg->lane_mode  = xgxs_operationModes_ComboCoreMode;

        if ((serdes_id0 & SERDESID_SERDESID0_MODEL_NUMBER_MASK) != MODEL_QSGMII)  {
            pc->lane_num = 0;  /* make sure lane_num is 0 */
        }
    }

    if ((PHY_FIBER_MODE(unit, port) && !PHY_EXTERNAL_MODE(unit, port)) ||
        (PHY_PASSTHRU_MODE(unit, port)) ||
        (!PHY_INDEPENDENT_LANE_MODE(unit, port)) ||
         (pc->speed_max >= 10000)) {
        pCfg->fiber_pref = TRUE;
    } else {
        pCfg->fiber_pref = FALSE;
    }

    if (PHY_INDEPENDENT_LANE_MODE(unit, port)) {
        pCfg->cl73an     = WCMOD_CL73_WO_CL73BAM; /*WC40_CL73_AND_CL73BAM; */
    } else {
       if (IS_HG_PORT(unit, port)) {
           pCfg->cl73an     = FALSE;
       } else {
           pCfg->cl73an     = WCMOD_CL73_WO_CL73BAM;
       }
    }
    
    if((serdes_id0 & SERDESID_SERDESID0_MODEL_NUMBER_MASK) == MODEL_XGXS_16G){
       pCfg->cl73an     = FALSE;  /* CL73 not supported as default for Xenia*/
    }

    /* Default setting for CL37 */ 
    pCfg->cl37an     = WCMOD_CL37_AND_CL37BAM; 

    if ((PHY_FIBER_MODE(unit, port) && !PHY_EXTERNAL_MODE(unit, port)) ||
        PHY_PASSTHRU_MODE(unit, port) ||
        PHY_SGMII_AUTONEG_MODE(unit, port)) {
        pCfg->pdetect1000x = TRUE;
    } else {
        pCfg->pdetect1000x = FALSE;
    }

    p_tx = &pCfg->tx_drive[TXDRV_ILKN];
    p_tx->u.tap.post  = 0x12;
    p_tx->u.tap.main  = 0x2d;
    p_tx->u.tap.pre   = 0x00;
    p_tx->u.tap.force = 0x01;
    p_tx->post2     = 0x00;
    p_tx->idrive    = 0x06;
    p_tx->ipredrive = 0x09;

    /* PCS by pass mode */
    p_tx = &pCfg->tx_drive[TXDRV_6250_INX];
    p_tx->u.tap.post  = 0x00;
    p_tx->u.tap.main  = 0x3f;
    p_tx->u.tap.pre   = 0x00;
    p_tx->u.tap.force = 0x01;
    p_tx->post2     = 0x00;
    p_tx->idrive    = 0x06;
    p_tx->ipredrive = 0x09;

    /* PCS by pass mode */
    p_tx = &pCfg->tx_drive[TXDRV_103125_INX];
    p_tx->u.tap.post  = 0x00;
    p_tx->u.tap.main  = 0x3f;
    p_tx->u.tap.pre   = 0x00;
    p_tx->u.tap.force = 0x01;
    p_tx->post2     = 0x00;
    p_tx->idrive    = 0x06;
    p_tx->ipredrive = 0x09;

    /* PCS by pass mode */
    p_tx = &pCfg->tx_drive[TXDRV_109375_INX];
    p_tx->u.tap.post  = 0x00;
    p_tx->u.tap.main  = 0x3f;
    p_tx->u.tap.pre   = 0x00;
    p_tx->u.tap.force = 0x01;
    p_tx->post2     = 0x00;
    p_tx->idrive    = 0x06;
    p_tx->ipredrive = 0x09;

    /* PCS by pass mode */
    p_tx = &pCfg->tx_drive[TXDRV_12500_INX];
    p_tx->u.tap.post  = 0x00;
    p_tx->u.tap.main  = 0x3f;
    p_tx->u.tap.pre   = 0x00;
    p_tx->u.tap.force = 0x01;
    p_tx->post2     = 0x00;
    p_tx->idrive    = 0x06;
    p_tx->ipredrive = 0x09;

    /* PCS by pass mode */
    p_tx = &pCfg->tx_drive[TXDRV_11500_INX];
    p_tx->u.tap.post  = 0x00;
    p_tx->u.tap.main  = 0x3f;
    p_tx->u.tap.pre   = 0x00;
    p_tx->u.tap.force = 0x01;
    p_tx->post2     = 0x00;
    p_tx->idrive    = 0x06;
    p_tx->ipredrive = 0x09;

    /* XFI  */
    p_tx = &pCfg->tx_drive[TXDRV_XFI_INX];
    p_tx->u.tap.post  = 0x08;
    p_tx->u.tap.main  = 0x37;
    p_tx->u.tap.pre   = 0x00;
    p_tx->u.tap.force = 0x01;
    p_tx->post2     = 0x00;
    p_tx->idrive    = 0x02;
    p_tx->ipredrive = 0x03;

    /* SFI SR */ 
    p_tx = &pCfg->tx_drive[TXDRV_SFI_INX];
    p_tx->u.tap.post  = 0x12;
    p_tx->u.tap.main  = 0x2d;
    p_tx->u.tap.pre   = 0x00;
    p_tx->u.tap.force = 0x01;
    p_tx->post2     = 0x02;
    p_tx->idrive    = 0x02;
    p_tx->ipredrive = 0x02;

    /* SFI DAC */ 
    p_tx = &pCfg->tx_drive[TXDRV_SFIDAC_INX];
    p_tx->u.tap.post  = 0x12;
    p_tx->u.tap.main  = 0x2d;
    p_tx->u.tap.pre   = 0x00;
    p_tx->u.tap.force = 0x01;
    p_tx->post2     = 0x02;
    p_tx->idrive    = 0x02;
    p_tx->ipredrive = 0x02;

    /* SR4 */ 
    p_tx = &pCfg->tx_drive[TXDRV_SR4_INX];
    p_tx->u.tap.post  = 0x13;
    p_tx->u.tap.main  = 0x2c;
    p_tx->u.tap.pre   = 0x00;
    p_tx->u.tap.force = 0x01;
    p_tx->post2     = 0x02;
    p_tx->idrive    = 0x05;
    p_tx->ipredrive = 0x05;

    /* XLAUI */ 
    p_tx = &pCfg->tx_drive[TXDRV_XLAUI_INX];
    p_tx->u.tap.post  = 0x18;
    p_tx->u.tap.main  = 0x27;
    p_tx->u.tap.pre   = 0x00;
    p_tx->u.tap.force = 0x01;
    p_tx->post2     = 0x02;
    p_tx->idrive    = 0x04;
    p_tx->ipredrive = 0x04;

    /* TXDRV_6GOS1_INX */ 
    p_tx = &pCfg->tx_drive[TXDRV_6GOS1_INX];
    p_tx->u.tap.post  = 0x00;
    p_tx->u.tap.main  = 0x00;
    p_tx->u.tap.pre   = 0x00;
    p_tx->u.tap.force = 0x01;
    p_tx->post2     = 0x00;
    p_tx->idrive    = 0x09;
    p_tx->ipredrive = 0x09;

    /* TXDRV_6GOS2_INX */ 
    p_tx = &pCfg->tx_drive[TXDRV_6GOS2_INX];
    p_tx->u.tap.post  = 0x00;
    p_tx->u.tap.main  = 0x3f;
    p_tx->u.tap.pre   = 0x00;
    p_tx->u.tap.force = 0x01;
    p_tx->post2     = 0x00;
    p_tx->idrive    = 0x09;
    p_tx->ipredrive = 0x09;

    /* TXDRV_6GOS2_CX4_INX */ 
    p_tx = &pCfg->tx_drive[TXDRV_6GOS2_CX4_INX];
    p_tx->u.tap.post  = 0x0e;
    p_tx->u.tap.main  = 0x31;
    p_tx->u.tap.pre   = 0x00;
    p_tx->u.tap.force = 0x01;
    p_tx->post2     = 0x00;
    p_tx->idrive    = 0x08;
    p_tx->ipredrive = 0x08;

    /* Autoneg */ 
    p_tx = &pCfg->tx_drive[TXDRV_AN_INX];
    p_tx->u.tap.post  = 0x00;
    p_tx->u.tap.main  = 0x00;
    p_tx->u.tap.pre   = 0x00;
    p_tx->u.tap.force = 0x00;
    p_tx->post2     = 0x00;
    p_tx->idrive    = 0x06;
    p_tx->ipredrive = 0x09;

    /* default */
    p_tx = &pCfg->tx_drive[TXDRV_DFT_INX];
    p_tx->u.tap.post  = 0x00;
    p_tx->u.tap.main  = 0x00;
    p_tx->u.tap.pre   = 0x00;
    p_tx->u.tap.force = 0x00;
    p_tx->post2     = 0x00;
    p_tx->idrive    = 0x09;
    p_tx->ipredrive = 0x09;
    
    pCfg->rx_los_ext_enable = 0;
    pCfg->linkscan_interval = 250000;
    /* config property overrides */

    /* enable sw-rx_los for 100G and plus port as default */
    if ((!SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) && (SOC_INFO(unit).port_num_lanes[pc->port] >= 10)) {
        /*disable this feature for DNX device */
        if SOC_IS_ARAD(unit) {
            pCfg->sw_rx_los.enable = 0;
        } else {        
            pCfg->sw_rx_los.enable = WCMOD_RX_LOS_SW_ENABLE | WCMOD_RXLOS_LINK_FLAP_WAR;
        }
    }

    /*will be correct for special ILKN configuration because it handled well in tx parameters set function*/
    if ((!SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) && (SOC_INFO(unit).port_num_lanes[pc->port] >= 10)) {
        loop = 12;
    } else {
        loop = SOC_INFO(unit).port_num_lanes[pc->port];
    }     

    for (i = 0; i < loop; i++) {
        preemphasis = soc_property_port_suffix_num_get(unit, port, i,
                                    spn_SERDES_PREEMPHASIS,
                                    "lane", pCfg->preemph[i]);
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(pc->unit,
                             "Port %d   - lane  %d -> preemphasis 0x%x\n"),
                  port, i, preemphasis));
        pCfg->preemph[i] = preemphasis;
    }

    for (i = 0; i < loop; i++) {
        idriver = soc_property_port_suffix_num_get(unit, port, i,
                                    spn_SERDES_DRIVER_CURRENT,
                                    "lane", pCfg->idriver[i]);
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(pc->unit,
                             "Port %d   - lane  %d -> idriver 0x%x\n"),
                  port, i, idriver));
        pCfg->idriver[i] = idriver;
    }

    for (i = 0; i < loop; i++) {
        pdriver = soc_property_port_suffix_num_get(unit, port, i,
                                    spn_SERDES_PRE_DRIVER_CURRENT,
                                    "lane", pCfg->pdriver[i]);
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(pc->unit,
                             "Port %d   - lane  %d -> pdriver 0x%x\n"),
                  port, i, pdriver));
        pCfg->pdriver[i] = pdriver;
    }

    for (i = 0; i < loop; i++) {
        post2_driver = soc_property_port_suffix_num_get(unit, port, i,
                                    spn_SERDES_POST2_DRIVER_CURRENT,
                                    "lane", pCfg->post2driver[i]);
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(pc->unit,
                             "Port %d   - lane  %d -> post2_driver 0x%x\n"),
                  port, i, post2_driver));
        pCfg->post2driver[i] = post2_driver;
    }

    pCfg->auto_medium = soc_property_port_get(unit, port,
                              spn_SERDES_AUTOMEDIUM, pCfg->auto_medium);
    pCfg->fiber_pref = soc_property_port_get(unit, port,
                              spn_SERDES_FIBER_PREF, pCfg->fiber_pref);
    pCfg->sgmii_mstr = soc_property_port_get(unit, port,
                              spn_SERDES_SGMII_MASTER, pCfg->sgmii_mstr);
    pCfg->pdetect10g = soc_property_port_get(unit, port,
                              spn_XGXS_PDETECT_10G, pCfg->pdetect10g);
    pCfg->cx42hg = soc_property_port_get(unit, port,
                              spn_CX4_TO_HIGIG, pCfg->cx42hg);
    pCfg->rxlane_map[0] = soc_property_port_get(unit, port,
                                        spn_XGXS_RX_LANE_MAP, pCfg->rxlane_map[0]);
    pCfg->txlane_map[0] = soc_property_port_get(unit, port,
                                        spn_XGXS_TX_LANE_MAP, pCfg->txlane_map[0]);
    pCfg->active_lane_map = soc_property_port_get(unit, port,
                                        spn_PHY_XAUI_ACTIVE_LANE_MAP, pCfg->active_lane_map);
    pCfg->cl73an = soc_property_port_get(unit, port,
                                        spn_PHY_AN_C73, pCfg->cl73an);
    pCfg->cl37an = soc_property_port_get(unit, port,
                                        spn_PHY_AN_C37, pCfg->cl37an);
    pCfg->cl72 = soc_property_port_get(unit, port,
                                        spn_PHY_AN_C72, pCfg->cl72);
    pCfg->forced_init_cl72 = soc_property_port_get(unit, port,
                                        spn_PORT_INIT_CL72, pCfg->forced_init_cl72);
    pCfg->rxaui = soc_property_port_get(unit, port,
                                    spn_SERDES_2WIRE_XAUI, FALSE);
    pCfg->txpol = soc_property_port_get(unit, port,
                            spn_PHY_XAUI_TX_POLARITY_FLIP, pCfg->txpol);
    pCfg->rxpol = soc_property_port_get(unit, port,
                            spn_PHY_XAUI_RX_POLARITY_FLIP, pCfg->rxpol);
    pCfg->refclk = soc_property_port_get(unit, port,
                                        spn_XGXS_LCPLL_XTAL_REFCLK,  pCfg->refclk) ;
    pCfg->pcs_bypass = soc_property_port_get(unit, port,
                            spn_PHY_PCS_BYPASS, pCfg->pcs_bypass);
    pCfg->rxaui_mode = soc_property_port_get(unit, port,
                            spn_SERDES_RXAUI_MODE, pCfg->rxaui_mode);
    if (PHY_INDEPENDENT_LANE_MODE(unit, port)) {
        pCfg->lane_mode = soc_property_port_get(unit, port,
                          spn_PHY_HL65_1LANE_MODE, pCfg->lane_mode);
    }
    pCfg->cx4_10g = soc_property_port_get(unit, port,
                          spn_10G_IS_CX4, pCfg->cx4_10g);
    pCfg->lane0_rst = soc_property_get(unit,
                             spn_SERDES_LANE0_RESET, pCfg->lane0_rst);
    pCfg->scrambler_en = soc_property_port_get(unit, port,
                             spn_SERDES_SCRAMBLER_ENABLE, pCfg->scrambler_en);
    pCfg->firmware_mode = soc_property_port_get(unit, port,
                             spn_SERDES_FIRMWARE_MODE, pCfg->firmware_mode);
    pCfg->speed = soc_property_port_get(unit, port,
                             spn_PORT_INIT_SPEED, pCfg->speed);
    pCfg->max_speed = soc_property_port_get(unit, port,
                             spn_PORT_MAX_SPEED, pCfg->max_speed);
    pCfg->line_intf = soc_property_port_get(unit, port,
                             spn_SERDES_IF_TYPE, pCfg->line_intf);
    pCfg->phy_sgmii_an =  soc_property_port_get(unit, port,
                             spn_PHY_SGMII_AUTONEG, pCfg->phy_sgmii_an);
    pCfg->asymmetric_mode = soc_property_port_get(unit, port,
                             spn_SERDES_ASYMMETRIC_SPEED_MODE, pCfg->asymmetric_mode);
    pCfg->rx_los_ext_enable = soc_property_port_get(unit, port,
                             spn_SERDES_RX_LOS, pCfg->rx_los_ext_enable); 
    pCfg->linkscan_interval = soc_property_port_get(unit, port,
                             spn_BCM_LINKSCAN_INTERVAL, pCfg->linkscan_interval);
    if (PHY_EXTERNAL_MODE(unit, port)) {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_PASSTHRU);
        pCfg->phy_passthru = soc_property_port_get(unit, port,
                            spn_PHY_PCS_REPEATER, pCfg->phy_passthru);
        if (pCfg->phy_passthru) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_PASSTHRU);
        }
    }

    pCfg->hg_mode = IS_HG_PORT(unit,port);
    if (PHY_EXTERNAL_MODE(unit, port)) {
        /* default: always IEEE speed mode if connected with an external PHY */
        pCfg->hg_mode = FALSE;

        /* exception: combo mode && passthru */
       if ((!PHY_INDEPENDENT_LANE_MODE(unit, port)) && pCfg->phy_passthru) {
           pCfg->hg_mode = IS_HG_PORT(unit,port);
       }
    }

    if (!PHY_EXTERNAL_MODE(unit, port)) {

        if (pCfg->fiber_pref) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_FIBER);
        } else {
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_FIBER);
        }

        if (pCfg->cl73an) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_C73);
        }
    }

    /*get all the warpCore tx/rx lane swap for multi-core port */
    /*arad look in other soc property - no handle special ILKN*/
    if ((SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) || (SOC_INFO(unit).port_num_lanes[pc->port] >= 10)) {
        num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        for (i = 0; i < num_core; i++) {
            tx_lane_swap = soc_property_port_suffix_num_get(unit, port, i,
                                        spn_XGXS_TX_LANE_MAP,
                                        "core", pCfg->txlane_map[i]);
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(pc->unit,
                                 "Port %d WarpCore tx lane swap - core %d -> tx_lane_swap 0x%x\n"),
                      port, i, tx_lane_swap));
            pCfg->txlane_map[i] = tx_lane_swap;
        }
    }

    /*get all the warpCore tx/rx lane swap for multi-core port */
    if ((SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) || (SOC_INFO(unit).port_num_lanes[pc->port] >= 10)) {
        num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        for (i = 0; i < num_core; i++) {
            rx_lane_swap = soc_property_port_suffix_num_get(unit, port, i,
                                        spn_XGXS_RX_LANE_MAP,
                                        "core", pCfg->rxlane_map[i]);
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(pc->unit,
                                 "Port %d WarpCore rx lane swap - core %d -> rx_lane_swap 0x%x\n"),
                      port, i, rx_lane_swap));
            pCfg->rxlane_map[i] = rx_lane_swap;
        }
    }

    /* for ILKN and 100G plus need to get MLD lane swap info */
    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) {
        if (SOC_INFO(unit).port_num_lanes[pc->port] > 12) {
                for (i = 0; i < 48; i++) {
                    lane = soc_property_port_suffix_num_get(unit, port, i,
                                                spn_MLD_LANE_SWAP,
                                                "lane", i);
                    LOG_INFO(BSL_LS_SOC_PHY,
                             (BSL_META_U(pc->unit,
                                         "Port %d MLD lane swap - Lane %d -> Lane %d\n"),
                              port, i, lane));
                    pCfg->mld_lane_swap[i] = lane;
                }
        } else {
                for (i = 0; i < 24; i++) {
                    lane = soc_property_port_suffix_num_get(unit, port, i,
                                                spn_MLD_LANE_SWAP,
                                                "lane", i);
                    LOG_INFO(BSL_LS_SOC_PHY,
                             (BSL_META_U(pc->unit,
                                         "Port %d MLD lane swap - Lane %d -> Lane %d\n"),
                              port, i, lane));
                    pCfg->mld_lane_swap[i] = lane;
                }
        }
   } else {
        if (SOC_INFO(unit).port_num_lanes[pc->port] >= 10) {
            for (i = 0; i < 24; i++) {
                lane = soc_property_port_suffix_num_get(unit, port, i,
                                            spn_MLD_LANE_SWAP,
                                            "lane", i);
                LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(pc->unit,
                                     "Port %d MLD lane swap - Lane %d -> Lane %d\n"),
                          port, i, lane));
                pCfg->mld_lane_swap[i] = lane;
            }
        }
    }

    SOC_IF_ERROR_RETURN
        (READ_WC40_UC_INFO_B1_FIRMWARE_MODEr(unit,ws,&data16));

    /* coverity[large_shift] */
    if (((data16 >> (pc->lane_num * 4)) & 0xf) == WCMOD_SFP_DAC) {
        pCfg->medium = SOC_PORT_MEDIUM_COPPER;

    /* coverity[large_shift] */
    } else if (((data16 >> (pc->lane_num * 4)) & 0xf) == WCMOD_SFP_OPT_SR4) {
        pCfg->medium = SOC_PORT_MEDIUM_FIBER;
    }

/* the warm boot support */
#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        int intf;
        int scr;
        int speed;

        intf = 0;
        /* recovery software states from device */
        /* recover interface value from hardware */
        if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
            SOC_IF_ERROR_RETURN
                (_phy_wcmod_combo_speed_get(unit, port, &speed,&intf,&scr));
        } else {
            SOC_IF_ERROR_RETURN
                (_phy_wcmod_ind_speed_get(unit, port, &speed,&intf,&scr));
        }
        pCfg->line_intf =  1 << intf;
        pCfg->scrambler_en = scr;
        pCfg->speed = speed;
    } 
#endif 

    return SOC_E_NONE;
}

STATIC
int _wcmod_soft_reset(int unit, phy_ctrl_t  *pc)
{
    wcmod_st            *ws;      /*wcmod structure */
    WCMOD_DEV_DESC_t    *pDesc;
    int rv;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    /*copy the phy control pataremeter into wcmod*/
    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("CORE_RESET", ws, &rv));

    return(SOC_E_NONE);
}

STATIC
int phy_wcmod_soft_reset(int unit, soc_port_t port, void * pointer)
{
    phy_ctrl_t    *pc;      /* PHY software state */

    pc = INT_PHY_SW_STATE(unit, port);

    /*copy the phy control pataremeter into wcmod*/
    SOC_IF_ERROR_RETURN
        (_wcmod_soft_reset(unit, pc));

    return(SOC_E_NONE);
}

STATIC
int _phy_wcmod_tx_reset(int unit, phy_ctrl_t  *pc, int enable)
{
    wcmod_st            *ws;      /*wcmod structure */
    WCMOD_DEV_DESC_t    *pDesc;
    int rv, i, j, size;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
        ws   =  (wcmod_st *)( pDesc + 1);

    /*copy the phy control pataremeter into wcmod*/
    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    if (IS_MULTI_CORE_PORT(pc)) {
        int num_core;
        wcmod_st *temp_ws;
        /*check the special ILKN port */
        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
            num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
        } else {
            num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        }
        for (i = 0 ; i < num_core; i++) {
            temp_ws   =  (wcmod_st *)( ws + i);
            size = temp_ws->num_of_lane;
            for(j = temp_ws->this_lane; j <  temp_ws->this_lane + size; j++) {
                temp_ws->per_lane_control = enable << (j * 8);
                switch(j) {
                    case 0: temp_ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                    case 1: temp_ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                    case 2: temp_ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                    case 3: temp_ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
                default:
                    break;
                }
                SOC_IF_ERROR_RETURN
                   (wcmod_tier1_selector("TX_RESET", temp_ws, &rv));
            }
        }
    } else {
        ws->per_lane_control = enable << ((pc->lane_num) * 8);
        SOC_IF_ERROR_RETURN
          (wcmod_tier1_selector("TX_RESET", ws, &rv));
    }
    return(SOC_E_NONE);
}

STATIC
int _phy_wcmod_rx_reset(int unit, phy_ctrl_t  *pc, int enable)
{
    wcmod_st            *ws;      /*wcmod structure */
    WCMOD_DEV_DESC_t    *pDesc;
    int rv, i, j, size;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
        ws   =  (wcmod_st *)( pDesc + 1);

    /*copy the phy control pataremeter into wcmod*/
    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    if (IS_MULTI_CORE_PORT(pc)) {
        int num_core;
        wcmod_st *temp_ws;
        /*check the special ILKN port */
        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
            num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
        } else {
            num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        }
        for (i = 0 ; i < num_core; i++) {
            temp_ws   =  (wcmod_st *)( ws + i);
            size = temp_ws->num_of_lane;
            for(j = temp_ws->this_lane; j <  temp_ws->this_lane + size; j++) {
                temp_ws->per_lane_control = enable << (j * 8);
                switch(j) {
                    case 0: temp_ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                    case 1: temp_ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                    case 2: temp_ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                    case 3: temp_ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
                default:
                    break;
                }
                SOC_IF_ERROR_RETURN
                   (wcmod_tier1_selector("RX_RESET", temp_ws, &rv));
            }
        }
    } else {
 
        ws->per_lane_control = enable << ((pc->lane_num) * 8);

        SOC_IF_ERROR_RETURN
          (wcmod_tier1_selector("RX_RESET", ws, &rv));
    }
    return(SOC_E_NONE);
}


STATIC
int _phy_wcmod_rx_reset_get(int unit, phy_ctrl_t  *pc, uint32* enable)
{
    wcmod_st            *ws;      /*wcmod structure */
    WCMOD_DEV_DESC_t    *pDesc;
    uint16 data16;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
        ws   =  (wcmod_st *)( pDesc + 1);

    /*copy the phy control pataremeter into wcmod*/
    READ_WC40_DIGITAL5_MISC6r(pc->unit, ws, &data16);

    *enable = (data16 & DIGITAL5_MISC6_RESET_RX_ASIC_MASK) >> DIGITAL5_MISC6_RESET_RX_ASIC_SHIFT;

    return(SOC_E_NONE);
}


/* WC prior C0 style coding */
STATIC int 
_phy_wcmod_pC0_cl72_enable(int unit, soc_port_t port, int en)
{
    phy_ctrl_t     *pc;
    WCMOD_DEV_DESC_t *pDesc;
    wcmod_st *ws;
    WCMOD_DEV_CFG_t *pCfg;
    int            tmp_lane_select ;

    pc    = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws    =  (wcmod_st *)( pDesc + 1);
    pCfg = &pDesc->cfg; 

    if (IND_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        /* XXXX revisit for dualport mode */
        /* disable KR PMD start-up protocol */
        SOC_IF_ERROR_RETURN
          (MODIFY_WC40_PMD_IEEE9BLK_TENGBASE_KR_PMD_CONTROL_REGISTER_150r(unit, ws, 
             en? PMD_IEEE9BLK_TENGBASE_KR_PMD_CONTROL_REGISTER_150_TRAINING_ENABLE_MASK: 0,
             PMD_IEEE9BLK_TENGBASE_KR_PMD_CONTROL_REGISTER_150_TRAINING_ENABLE_MASK));

        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_CL72_USERB0_CL72_MISC1_CONTROLr(unit, ws, 
               en? 0:CL72_USERB0_CL72_MISC1_CONTROL_LINK_CONTROL_FORCE_MASK,
                     CL72_USERB0_CL72_MISC1_CONTROL_LINK_CONTROL_FORCE_MASK|
                     CL72_USERB0_CL72_MISC1_CONTROL_LINK_CONTROL_FORCEVAL_MASK));
        if(en) {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_CL72_USERB0_CL72_TX_FIR_TAP_REGISTERr
                 (pc->unit, ws, 0,
                  CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_FORCE_MASK));
        }

    } else {  /* combo mode */
        tmp_lane_select  = ws->lane_select ; 
        ws->lane_select  = WCMOD_LANE_BCST ;

        /* don't touch forced cl72 bit in port_init_cl72 is set */
        if (!(pCfg->forced_init_cl72)) {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_PMD_IEEE9BLK_TENGBASE_KR_PMD_CONTROL_REGISTER_150r(unit, ws, 
                 en? PMD_IEEE9BLK_TENGBASE_KR_PMD_CONTROL_REGISTER_150_TRAINING_ENABLE_MASK: 0,
                 PMD_IEEE9BLK_TENGBASE_KR_PMD_CONTROL_REGISTER_150_TRAINING_ENABLE_MASK));

            /*              =0x3 force enable cl72     ( 1, 1 )
             *              =0x2 force disable cl72    ( 1, 0 )
             *              =0x0 controlled by autoneg ( 0, 0 )
             */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_CL72_USERB0_CL72_MISC1_CONTROLr(unit, ws,
                   en? 0:CL72_USERB0_CL72_MISC1_CONTROL_LINK_CONTROL_FORCE_MASK,
                         CL72_USERB0_CL72_MISC1_CONTROL_LINK_CONTROL_FORCE_MASK|
                         CL72_USERB0_CL72_MISC1_CONTROL_LINK_CONTROL_FORCEVAL_MASK));
        }
        if(en){
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_CL72_USERB0_CL72_TX_FIR_TAP_REGISTERr
                 (pc->unit, ws, 0,
                  CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_FORCE_MASK));
        }

        ws->lane_select  = tmp_lane_select ;
    }
    return SOC_E_NONE;

}

STATIC int
_phy_wcmod_cl72_enable(int unit, phy_ctrl_t *pc, int en)
{
    int rv, i, j, size;
    wcmod_st *temp_ws, *ws;
    WCMOD_DEV_DESC_t     *pDesc;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    /*copy the phy control pataremeter into wcmod  */
    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    if (IS_MULTI_CORE_PORT(pc)) {
        int num_core, tmpLane;
        /*check the special ILKN port */
        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
            num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
        } else {
            num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        }
        for (i = 0 ; i < num_core; i++) {
            temp_ws   =  (wcmod_st *)( ws + i);
            size = temp_ws->num_of_lane;
            tmpLane = temp_ws->this_lane;
            for(j = tmpLane; j < tmpLane + size; j++) {
                temp_ws->per_lane_control = en;
                temp_ws->this_lane = j;
                switch(j) {
                    case 0: temp_ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                    case 1: temp_ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                    case 2: temp_ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                    case 3: temp_ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
                default:
                    break;
                }
                SOC_IF_ERROR_RETURN
                   (wcmod_tier1_selector("CLAUSE_72_CONTROL", temp_ws, &rv));
            }
        }
    } else {
 
        int tmp_lane, tmp_lane_select;
        size = SOC_INFO(unit).port_num_lanes[pc->port];
        tmp_lane = ws->this_lane;
        tmp_lane_select = ws->lane_select; 
        for(j = pc->lane_num; j < size + pc->lane_num; j++) {
            ws->this_lane = j;
            switch(j) {
                case 0: ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                case 1: ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                case 2: ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                case 3: ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
            default:
                break;
            }
            ws->per_lane_control = en;

            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("CLAUSE_72_CONTROL", ws, &rv));
       }
       ws->this_lane = tmp_lane;
       ws->lane_select =  tmp_lane_select;
    }

    return SOC_E_NONE;
}

STATIC int
_phy_wcmod_cl72_enable_get(int unit, phy_ctrl_t *pc, uint32 *en)
{
    wcmod_st    *temp_ws, *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    uint16 data16;
    int temp_data,i,  j;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    /*copy the phy control pataremeter into wcmod  */
    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    *en = 0x0;

    if (IS_MULTI_CORE_PORT(pc)) {
        int num_core, tmpLane, total_lane = 0;
        /*check the special ILKN port */
        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
            num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
        } else {
            num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        }
        for (i = 0 ; i < num_core; i++) {
            temp_ws   =  (wcmod_st *)( ws + i);
            tmpLane = temp_ws->this_lane; 
            for(j = tmpLane; j < tmpLane + temp_ws->num_of_lane ; j++) {
                temp_ws->this_lane = j;
                SOC_IF_ERROR_RETURN
                  (READ_WC40_PMD_IEEE9BLK_TENGBASE_KR_PMD_CONTROL_REGISTER_150r(unit,  temp_ws, &data16));
                temp_data = (data16 & PMD_IEEE9BLK_TENGBASE_KR_PMD_CONTROL_REGISTER_150_TRAINING_ENABLE_MASK) >>
                       PMD_IEEE9BLK_TENGBASE_KR_PMD_CONTROL_REGISTER_150_TRAINING_ENABLE_SHIFT;
                *en |= temp_data << total_lane ;
                total_lane++;
           }
        }
    } else {
        if (PHY_INDEPENDENT_LANE_MODE(unit, pc->port)) {
        SOC_IF_ERROR_RETURN
          (READ_WC40_PMD_IEEE9BLK_TENGBASE_KR_PMD_CONTROL_REGISTER_150r(unit,  ws, &data16));

        *en = (data16 & PMD_IEEE9BLK_TENGBASE_KR_PMD_CONTROL_REGISTER_150_TRAINING_ENABLE_MASK) >>
               PMD_IEEE9BLK_TENGBASE_KR_PMD_CONTROL_REGISTER_150_TRAINING_ENABLE_SHIFT;
        } else {
            int temp_lane;
            temp_lane = ws->this_lane;
            for(j = 0; j < 4; j++) {
                ws->this_lane = j;
                SOC_IF_ERROR_RETURN
                  (READ_WC40_PMD_IEEE9BLK_TENGBASE_KR_PMD_CONTROL_REGISTER_150r(unit,  ws, &data16));
                temp_data = (data16 & PMD_IEEE9BLK_TENGBASE_KR_PMD_CONTROL_REGISTER_150_TRAINING_ENABLE_MASK) >>
                       PMD_IEEE9BLK_TENGBASE_KR_PMD_CONTROL_REGISTER_150_TRAINING_ENABLE_SHIFT;
                *en |= temp_data << j;
            }
            ws->this_lane = temp_lane;
        }
    }
    return SOC_E_NONE;
}

STATIC int
_phy_wcmod_cl72_status_get(int unit, phy_ctrl_t *pc, uint32 *status)
{
    uint16 data;
    wcmod_st   *temp_ws, *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int temp_data, i, j;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    /*copy the phy control pataremeter into wcmod  */
    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    *status = 0;

    if (IS_MULTI_CORE_PORT(pc)) {
        int num_core, tmpLane, total_lane = 0;
        /*check the special ILKN port */
        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
            num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
        } else {
            num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        }

        for (i = 0 ; i < num_core; i++) {
            temp_ws   =  (wcmod_st *)( ws + i);
            tmpLane = temp_ws->this_lane; 
            for(j = tmpLane; j < tmpLane + temp_ws->num_of_lane ; j++) {
                temp_ws->this_lane = j;
                SOC_IF_ERROR_RETURN
                    (READ_WC40_PMD_IEEE9BLK_TENGBASE_KR_PMD_STATUS_REGISTER_151r(unit, temp_ws, &data));
                temp_data  = data & PMD_IEEE9BLK_TENGBASE_KR_PMD_STATUS_REGISTER_151_RECEIVER_STATUS_MASK;
                *status |= temp_data << total_lane;
                total_lane++;
           }
        }
    } else {
        if (PHY_INDEPENDENT_LANE_MODE(unit, pc->port)) {
        SOC_IF_ERROR_RETURN
            (READ_WC40_PMD_IEEE9BLK_TENGBASE_KR_PMD_STATUS_REGISTER_151r(unit, ws, &data));
        *status = data & PMD_IEEE9BLK_TENGBASE_KR_PMD_STATUS_REGISTER_151_RECEIVER_STATUS_MASK;
        } else {
            int temp_lane;
            temp_lane = ws->this_lane;
            for(j = 0; j < 4; j++) {
                ws->this_lane = j;
                SOC_IF_ERROR_RETURN
                    (READ_WC40_PMD_IEEE9BLK_TENGBASE_KR_PMD_STATUS_REGISTER_151r(unit, ws, &data));
                temp_data  = data & PMD_IEEE9BLK_TENGBASE_KR_PMD_STATUS_REGISTER_151_RECEIVER_STATUS_MASK;
                *status |= temp_data << j;
            }
            ws->this_lane = temp_lane;
        }
    }
    return SOC_E_NONE;
}

STATIC int
_phy_wcmod_force_cl72_sw_link_recovery(int unit, soc_port_t port,int link)
{
    int i, lane_start, lane_end, tmp_lane, rv;
    WCMOD_DEV_DESC_t     *pDesc;
    phy_ctrl_t     *pc;
    wcmod_st  *ws;
    uint16 data16, pmd_restarted = 0x1;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    /*copy the phy control pataremeter into wcmod  */
    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    tmp_lane = ws->this_lane;

    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        lane_start = 0;
        lane_end   = 3; 
    } else if (IS_DUAL_LANE_PORT(pc)) {
        lane_start = pc->lane_num;
        lane_end = lane_start + 1;
    } else {  /* quad-port mode */
        lane_start = pc->lane_num;
        lane_end   = lane_start;
    }

    switch (FORCE_CL72_STATE(pc)) {
        case WCMOD_CL72_STATE_WAIT_FOR_LINK:
            if (link == FALSE) {
                FORCE_CL72_TICK_CNT(pc) += 1;
                if (FORCE_CL72_TICK_CNT(pc) > WCMOD_CL72_LINK_WAIT_CNT_LIMIT) {
                    FORCE_CL72_STATE(pc) = WCMOD_CL72_STATE_RESTART_PMD;
                    FORCE_CL72_TICK_CNT(pc) = 0;
                    FORCE_CL72_RESTART_CNT(pc) +=1;
                }
            } else {  /* link up */
                FORCE_CL72_STATE(pc) = WCMOD_CL72_STATE_LINK_GOOD;
                FORCE_CL72_TICK_CNT(pc) = 0;
                FORCE_CL72_RESTART_CNT(pc) = 0;
                /* reset the status */
                for(i = lane_start; i <= lane_end; ++i) {
                    ws->this_lane = 0;
                SOC_IF_ERROR_RETURN
                        (WC40_REG_MODIFY(unit, ws, 0x00, 0x81f7 + (0x1 * i), 0, 1));
                }
                ws->this_lane = tmp_lane;
            }
            LOG_VERBOSE(BSL_LS_SOC_PHY,
                        (BSL_META_U(pc->unit,
                                    "force cl72: u=%d p=%d state WAIT_FOR_LINK "
                                    "link: 0x%x\n"), unit, port,link));
            break;

        case WCMOD_CL72_STATE_RESTART_PMD:
            /* check the uc status to see if it's ready */
            for(i = lane_start; i <= lane_end; ++i) {
                ws->this_lane = i;
                /* wait for uC ready for command:  bit7=1 */
                rv = wcmod_regbit_set_wait_check(ws, DSC1B0_UC_CTRLr,
                         DSC1B0_UC_CTRL_READY_FOR_CMD_MASK, 1, WC40_PLL_WAIT*1000);
                if (rv == SOC_E_TIMEOUT) {
                    LOG_VERBOSE(BSL_LS_SOC_PHY,
                                (BSL_META_U(pc->unit,
                                "force cl72:  uController not ready: u=%d p=%d\n"), 
                                unit, port));
                    return (SOC_E_TIMEOUT);
                }
            }
            FORCE_CL72_STATE(pc) = WCMOD_CL72_STATE_WAIT_FOR_LINK;
            /* restart all 4 lanes */
            for(i = lane_start; i <= lane_end; ++i) {
                ws->this_lane = i;
                SOC_IF_ERROR_RETURN
                    (WRITE_WC40_DSC1B0_UC_CTRLr(unit, ws, 0x301));
            }
            ws->this_lane = tmp_lane;
            FORCE_CL72_TICK_CNT(pc) = 0;
            LOG_VERBOSE(BSL_LS_SOC_PHY,
                        (BSL_META_U(pc->unit,
                                    "force cl72: u=%d p=%d state RESTART_PMD \n"), unit, port));
            break;

        case WCMOD_CL72_STATE_LINK_GOOD:
            if (link == FALSE) {
                pmd_restarted = 0x1; 
                for(i = lane_start; i <= lane_end; ++i) {
                SOC_IF_ERROR_RETURN
                        (WC40_REG_READ(unit, ws, 0, 
                                 0x81f7 + (0x1 * i), &data16));
                    pmd_restarted &= (data16 & 0x1);
                    if(!pmd_restarted) {
                        LOG_VERBOSE(BSL_LS_SOC_PHY,
                                    (BSL_META_U(pc->unit,
                                     "force cl72:  u=%d p=%d state LINK GOOD "
                                     "pmd_restarted=0: " "link: 0x%x\n"), unit, port, link));
                        break;
                    }
                }
                FORCE_CL72_STATE(pc) = pmd_restarted ? 
                                     WCMOD_CL72_STATE_WAIT_FOR_LINK : 
                                     WCMOD_CL72_STATE_RESTART_PMD;
                FORCE_CL72_TICK_CNT(pc) = 0;
            }
            LOG_VERBOSE(BSL_LS_SOC_PHY,
                        (BSL_META_U(pc->unit,
                       "force cl72: u=%d p=%d state LINK_GOOD: link 0x%x\n"), 
                                 unit, port,link));
            break;
        default:
            break;
    } 
    return SOC_E_NONE;
}

STATIC int
_phy_wcmod_tx_control_get(int unit, phy_ctrl_t *pc, WCMOD_TX_DRIVE_t *tx_drv, int inx)
{
    int rv = SOC_E_NONE;
    int size;
    int i;

    if ((!SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) && (SOC_INFO(unit).port_num_lanes[pc->port] >= 10)) {
        size = 12;
    } else {
    size = (SOC_INFO(unit).port_num_lanes[pc->port]);
    }

    for (i = 0; i < size; i++) {
        if (DEV_CFG_PTR(pc)->idriver[i] == WCMOD_NO_CFG_VALUE) {
            /* take from the default value table */
            (tx_drv + i)->idrive    = DEV_CFG_PTR(pc)->tx_drive[inx].idrive;
        } else {
            /* taking from user */
            (tx_drv + i)->idrive = DEV_CFG_PTR(pc)->idriver[i];
        }
        if (DEV_CFG_PTR(pc)->pdriver[i] == WCMOD_NO_CFG_VALUE) {
            /* take from the default value table */
            (tx_drv + i)->ipredrive = DEV_CFG_PTR(pc)->tx_drive[inx].ipredrive;
        } else {
            /* taking from user */
            (tx_drv + i)->ipredrive = DEV_CFG_PTR(pc)->pdriver[i];
        }
        if (DEV_CFG_PTR(pc)->post2driver[i] == WCMOD_NO_CFG_VALUE) {
            /* take from the default value table */
            (tx_drv + i)->post2 = DEV_CFG_PTR(pc)->tx_drive[inx].post2;
        } else {
            /* taking from user */
            (tx_drv + i)->post2 = DEV_CFG_PTR(pc)->post2driver[i];
        }

        /* in autoneg mode no forced preemphasis */
        if ((DEV_CFG_PTR(pc)->preemph[i] == WCMOD_NO_CFG_VALUE) ||
            ((inx == TXDRV_AN_INX)) ) {
            /* take from the default value table */
            (tx_drv + i)->u.preemph = TXDRV_PREEMPH(DEV_CFG_PTR(pc)->tx_drive[inx].u.tap);
        } else {
            /* taking from user */
            (tx_drv + i)->u.preemph = DEV_CFG_PTR(pc)->preemph[i];
        }
    }
    return rv;
}

STATIC int
_phy_wcmod_tx_control_set(int unit, phy_ctrl_t *pc, WCMOD_TX_DRIVE_t *tx_drv)
{
    uint32          preemph;
    uint32          idriver;
    uint32          pdriver;
    uint32          post2;    
    int i, lane, num_core, core_num, tempLane, size , special_ilkn_flag = 0;
    wcmod_st    *ws_warpcore[8];
    WCMOD_DEV_DESC_t     *pDesc;
    int main_tap=0, post=0;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws_warpcore[0]   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN(_wcmod_phy_parameter_copy(pc, ws_warpcore[0]));

    if (IS_MULTI_CORE_PORT(pc)) {
        /*check the special ILKN port */
        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws_warpcore[0]->this_lane != 0)) {
            num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
            special_ilkn_flag = 1;
        } else {
            num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        }
    } else {
        num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
    }

    for (i = 0 ; i < num_core; i++) {
        ws_warpcore[i]   =  (wcmod_st *)( ws_warpcore[0] + i);
    }

    if ((!SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) && (SOC_INFO(unit).port_num_lanes[pc->port] >= 10)) {
        size = 12;
        if(SOC_IS_ARADPLUS(unit)){
            size = 0;
            for (i = 0 ; i < num_core; i++) {
                size += ws_warpcore[i]->num_of_lane;
            }
        }
    } else{
        size = (SOC_INFO(unit).port_num_lanes[pc->port]);
    }

    tempLane = ws_warpcore[0]->this_lane;
    
    for(i = 0; i < size; i++) {
        idriver = (tx_drv+i)->idrive;
        pdriver = (tx_drv+i)->ipredrive;
        post2   = (tx_drv+i)->post2;
        preemph = (tx_drv+i)->u.preemph;

        if (IS_MULTI_CORE_PORT(pc)) {
            if (special_ilkn_flag) {
                if (size == 4) {
                    core_num = (i + 2) / 4;
                    lane = i % 2 + 2;
                    ws_warpcore[core_num]->this_lane = lane;
                } else {
                    core_num = (i + 2) / 4;
                    if ((core_num == 0) || (core_num == 2)) {
                        lane = i % 2 + 2;
                    } else {
                        lane = i - 2;
                    }
                    ws_warpcore[core_num]->this_lane = lane;
                }
            } else {
                lane = (i + pc->lane_num) % 4;
                core_num = i / 4;
                ws_warpcore[core_num]->this_lane = lane;
            }

        } else {  
            lane = (i + pc->lane_num) % 4;
            core_num = i / 4;
            ws_warpcore[core_num]->this_lane = lane;
        }

      if(ws_warpcore[core_num]->model_type == WCMOD_QS_A0) {
          ws_warpcore[core_num]->this_lane = ((i+pc->lane_num) / 4) * 4;
          
          main_tap =(preemph & CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_MAIN_MASK) 
                            >> CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_MAIN_SHIFT;       
          post     =(preemph & CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_POST_MASK) 
                            >> CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_POST_SHIFT;
   
          SOC_IF_ERROR_RETURN
              (MODIFY_WC40_TX0_ANATXACONTROL2r(unit, ws_warpcore[core_num],  main_tap << 10, 0x7c00));    /* 0x8066 */
          SOC_IF_ERROR_RETURN 
              (MODIFY_WC40_TX0_ANATXACONTROL2r(unit, ws_warpcore[core_num], (post & 0x1) << 15, 0x8000)); 
          SOC_IF_ERROR_RETURN
              (MODIFY_WC40_TX0_TX_DRIVERr(unit, ws_warpcore[core_num], (post & 0x1e) >> 1, 0xf));         /* 0x8067 */
          
          SOC_IF_ERROR_RETURN
              (MODIFY_WC40_TX0_TX_DRIVERr(unit, ws_warpcore[core_num], idriver << 6, 0x3f << 6));
        
      } else {

        MODIFY_WC40_CL72_USERB0_CL72_TX_FIR_TAP_REGISTERr(unit, ws_warpcore[core_num],
          (preemph | CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_FORCE_MASK), 0xffff);
        switch(lane) {
            case 0: 
              SOC_IF_ERROR_RETURN
                  (MODIFY_WC40_TX0_TX_DRIVERr(unit, ws_warpcore[core_num], 
                          idriver << TXB_TX_DRIVER_IDRIVER_SHIFT,
                          TXB_TX_DRIVER_IDRIVER_MASK));
              SOC_IF_ERROR_RETURN
                  (MODIFY_WC40_TX0_TX_DRIVERr(unit, ws_warpcore[core_num], 
                          pdriver << TXB_TX_DRIVER_IPREDRIVER_SHIFT,
                          TXB_TX_DRIVER_IPREDRIVER_MASK));
              SOC_IF_ERROR_RETURN
                  (MODIFY_WC40_TX0_TX_DRIVERr(unit, ws_warpcore[core_num], 
                          post2 << TXB_TX_DRIVER_POST2_COEFF_SHIFT,
                          TXB_TX_DRIVER_POST2_COEFF_MASK));
              break ;
            case 1: 
              SOC_IF_ERROR_RETURN
                  (MODIFY_WC40_TX1_TX_DRIVERr(unit, ws_warpcore[core_num], 
                          idriver << TXB_TX_DRIVER_IDRIVER_SHIFT,
                          TXB_TX_DRIVER_IDRIVER_MASK));
              SOC_IF_ERROR_RETURN
                  (MODIFY_WC40_TX1_TX_DRIVERr(unit, ws_warpcore[core_num], 
                          pdriver << TXB_TX_DRIVER_IPREDRIVER_SHIFT,
                          TXB_TX_DRIVER_IPREDRIVER_MASK));
              SOC_IF_ERROR_RETURN
                  (MODIFY_WC40_TX1_TX_DRIVERr(unit, ws_warpcore[core_num], 
                          post2 << TXB_TX_DRIVER_POST2_COEFF_SHIFT,
                          TXB_TX_DRIVER_POST2_COEFF_MASK));
              break ;
            case 2: 
              SOC_IF_ERROR_RETURN
                  (MODIFY_WC40_TX2_TX_DRIVERr(unit, ws_warpcore[core_num], 
                          idriver << TXB_TX_DRIVER_IDRIVER_SHIFT,
                          TXB_TX_DRIVER_IDRIVER_MASK));
              SOC_IF_ERROR_RETURN
                  (MODIFY_WC40_TX2_TX_DRIVERr(unit, ws_warpcore[core_num], 
                          pdriver << TXB_TX_DRIVER_IPREDRIVER_SHIFT,
                          TXB_TX_DRIVER_IPREDRIVER_MASK));
              SOC_IF_ERROR_RETURN
                  (MODIFY_WC40_TX2_TX_DRIVERr(unit, ws_warpcore[core_num], 
                          post2 << TXB_TX_DRIVER_POST2_COEFF_SHIFT,
                          TXB_TX_DRIVER_POST2_COEFF_MASK));
              break;
            case 3: 
              SOC_IF_ERROR_RETURN
                  (MODIFY_WC40_TX3_TX_DRIVERr(unit, ws_warpcore[core_num], 
                          idriver << TXB_TX_DRIVER_IDRIVER_SHIFT,
                          TXB_TX_DRIVER_IDRIVER_MASK));
              SOC_IF_ERROR_RETURN
                  (MODIFY_WC40_TX3_TX_DRIVERr(unit, ws_warpcore[core_num], 
                          pdriver << TXB_TX_DRIVER_IPREDRIVER_SHIFT,
                          TXB_TX_DRIVER_IPREDRIVER_MASK));
              SOC_IF_ERROR_RETURN
                  (MODIFY_WC40_TX3_TX_DRIVERr(unit, ws_warpcore[core_num], 
                          post2 << TXB_TX_DRIVER_POST2_COEFF_SHIFT,
                          TXB_TX_DRIVER_POST2_COEFF_MASK));
              break;
        default:
            if(ws_warpcore[core_num]->verbosity>0) printf ("%s: config error lane_num=%0d size=%0d\n", FUNCTION_NAME(), pc->lane_num, size);
            return SOC_E_PARAM ;
        }
      }
    }
    /*restore back the this lane info */
    ws_warpcore[0]->this_lane = tempLane;
    if (num_core > 1) {
        for (i = 1; i < num_core; i++) {
            ws_warpcore[i]->this_lane = 0;
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_AERBLK_AERr(unit,ws_warpcore[i],0));
        }
    }

    return SOC_E_NONE;

}

/* clone of _phy_wcmod_tx_control_set except that _phy_wcmod_control_preemphasis_set is not called */
STATIC int
_phy_wcmod_tx_control_amp_set(int unit, soc_port_t port, WCMOD_TX_DRIVE_t *tx_drv)
{
    phy_ctrl_t     *pc;
    uint32          idriver;
    uint32          pdriver;
    uint32          post2;    
    int size, i, lane, tmpSelect, savedSelect, tmpLane ;
    wcmod_port_type tmpPort =  WCMOD_INDEPENDENT ;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;

    
    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        size = 4;
    } else if (IS_DUAL_LANE_PORT(pc)) {
        size = 2;
    } else {
        size = 1;
    }
    
    tmpPort        = ws->port_type ;
    ws->port_type  = WCMOD_INDEPENDENT ;  /* to allow combo to program each lane independently */
    tmpSelect      = ws->lane_select ;
    tmpLane        = ws->this_lane ; 
    for(i=0; i<size; i++) {
        idriver = (tx_drv+i)->idrive;
        pdriver = (tx_drv+i)->ipredrive;
        post2   = (tx_drv+i)->post2;

        lane = i + pc->lane_num ;
        ws->this_lane = lane ; 
        switch(lane) {
            case 0: ws->lane_select = WCMOD_LANE_0_0_0_1 ; break ;
            case 1: ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
            case 2: ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
            case 3: ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
        default:
            /* XXXX */
            if(ws->verbosity>0) printf ("%s: config error lane_num=%0d size=%0d\n", FUNCTION_NAME(), pc->lane_num, size);
            return SOC_E_PARAM ;
        }
        savedSelect = ws->lane_select ;
        ws->port_type  = WCMOD_INDEPENDENT ;
        if(IS_DUAL_LANE_PORT(pc)) {  ws->dxgxs     =  0  ; }
        if (ws->verbosity) 
            printf ("%s: port=%0d pc=%p ixd=%0d lane_num=%0d sel=%0x size=%0d\n", 
                    FUNCTION_NAME(), port, (void *)pc, i, pc->lane_num, ws->lane_select, size);
        /* each func below will reset ws->lane_select and ws->port_type through _wcmod_phy_parameter_copy */
        /* next set Tx driver, pdriver and post2 */
        _phy_wcmod_control_tx_driver_set(unit, pc, SOC_PHY_CONTROL_DRIVER_CURRENT, idriver);
        ws->lane_select = savedSelect ;
        ws->port_type  = WCMOD_INDEPENDENT ;
        if(IS_DUAL_LANE_PORT(pc)) {  ws->dxgxs     =  0  ; }
        _phy_wcmod_control_tx_driver_set(unit, pc, SOC_PHY_CONTROL_PRE_DRIVER_CURRENT,pdriver);
        ws->lane_select = savedSelect ;
        ws->port_type  = WCMOD_INDEPENDENT ;
        if(IS_DUAL_LANE_PORT(pc)) {  ws->dxgxs     =  0  ; }
        _phy_wcmod_control_tx_driver_set(unit, pc, SOC_PHY_CONTROL_DRIVER_POST2_CURRENT,post2); 
    }
    ws->port_type   = tmpPort ;  /* restore value */
    ws->lane_select = tmpSelect ;
    ws->this_lane   = tmpLane ; 
    if(IS_DUAL_LANE_PORT(pc)) {  ws->dxgxs     = ws->dual_type ; }
    return SOC_E_NONE;

}

int
_phy_wcmod_independent_lane_init(wcmod_st *ws)
{
    int rv; 
    uint16 data16 = 0; 
        
    /* disable AN on all lanes */
    ws->per_lane_control = 0;
    SOC_IF_ERROR_RETURN
       (wcmod_tier1_selector("AUTONEG_CONTROL", ws, &rv));

    /* set the TX default polairy: make sure ws->tx_pol is updated first with pCfg->txpol*/
    ws->per_lane_control = (ws->tx_pol<<2);
    
    /*set enable \ disable*/
    if(ws->tx_pol){
        ws->per_lane_control |=  0x1;
    }
    /*set TX*/
    ws->per_lane_control |=  0x2;
    SOC_IF_ERROR_RETURN (wcmod_tier1_selector("SET_POLARITY", ws, &rv));

    /* next set RX polarity: make sure ws->rx_pol is updated first with pCfg->rxpol*/
    ws->per_lane_control = (ws->rx_pol<<2);
    /*set enable \ disable*/
    if(ws->rx_pol){
        ws->per_lane_control |=  0x1;
    }
    /*set RX \ TX - 0 on bit 1 means RX, no need to configure*/
    SOC_IF_ERROR_RETURN (wcmod_tier1_selector("SET_POLARITY", ws, &rv));

     /* Put the Serdes in Fiber or SGMII mode  */
    if (ws->fiber_pref) {
        data16 = SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(ws->unit, ws, data16, SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK));

    return SOC_E_NONE;
}

STATIC int
_phy_wcmod_combo_core_init(int unit, soc_port_t port)
{
    phy_ctrl_t         *pc;               
    WCMOD_DEV_DESC_t *pDesc;
    WCMOD_DEV_CFG_t *pCfg;
    wcmod_st *ws;
    uint8 *pdata;
    int   ucode_len;
    int   alloc_flag, tmp_lane_select;
    int rv, rvtmp=0;
    int index = -1;
    WCMOD_TX_DRIVE_t tx_drv[MAX_NUM_LANES];
    uint16 data, mask;
    wcmod_spd_intfc_set spd_intf = WCMOD_SPD_ZERO;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);
    pCfg = &pDesc->cfg; 

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    SOC_IF_ERROR_RETURN (_wcmod_getRevDetails(ws));

    if ((ws->model_type) == WCMOD_XN) {
        DEV_CFG_PTR(pc)->lane_mode = xgxs_operationModes_ComboCoreMode;
    }

    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("CORE_RESET", ws, &rv));

    /* disable PLL      */
    ws->per_lane_control = WCMOD_DISABLE;

    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("PLL_SEQUENCER_CONTROL", ws, &rv));

    ws->port_type  = WCMOD_COMBO;
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("SET_PORT_MODE", ws, &rv));
                                                            
    /* turn off RX sequencer    */
    ws->per_lane_control = WCMOD_DISABLE;
    SOC_IF_ERROR_RETURN
        (wcmod_tier1_selector("RX_SEQ_CONTROL", ws, &rv));

    /* disable AN on all lanes */
    ws->per_lane_control = 0;
    SOC_IF_ERROR_RETURN
       (wcmod_tier1_selector("AUTONEG_CONTROL", ws, &rv));

    /* disable parallel detect on all lanes */
    ws->per_lane_control = 0;
    SOC_IF_ERROR_RETURN
       (wcmod_tier1_selector("PARALLEL_DETECT_CONTROL", ws, &rv));


    /* disable 100FX on all lanes  */
    ws->per_lane_control = 0;
    SOC_IF_ERROR_RETURN
       (wcmod_tier1_selector("100FX_CONTROL", ws, &rv));

    /* set the default polarity setting */
    _phy_wcmod_tx_polarity_set(ws->unit, pc,  pCfg->txpol);
    _phy_wcmod_rx_polarity_set(ws->unit, pc,  pCfg->rxpol);

    /* set the default lane swap */
    ws->per_lane_control = (pCfg->txlane_map[0]) << 16 | (pCfg->rxlane_map[0]);
    SOC_IF_ERROR_RETURN
       (wcmod_tier1_selector("LANE_SWAP", ws, &rv));

    /* set the default firmware mode */
    data = (pCfg->firmware_mode) | (pCfg->firmware_mode) << 4 |
            (pCfg->firmware_mode) << 8 | (pCfg->firmware_mode) << 12; 
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_UC_INFO_B1_FIRMWARE_MODEr(ws->unit, ws, data));

    /* tr3 WC pll lock workaround */
    if ((((ws->model_type) == WCMOD_WC_C0) && SOC_IS_TRIUMPH3(unit)) || SOC_IS_ARAD(unit)) {
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_PLL_ANAPLLACONTROL1r(unit,ws, 
                   0x1 << PLL_ANAPLLACONTROL1_ICLKIBUF4_SHIFT,
                   PLL_ANAPLLACONTROL1_ICLKIBUF4_MASK));
    }

    if (pCfg->speed == WCMOD_NO_CFG_VALUE) {
        SOC_IF_ERROR_RETURN(_phy_wcmod_speed_intf_get(unit, port, pc->speed_max, &spd_intf, &index));
        ws->spd_intf = spd_intf;
    } else {
        SOC_IF_ERROR_RETURN(_phy_wcmod_speed_intf_get(unit, port, pCfg->speed, &spd_intf, &index));
        ws->spd_intf = spd_intf;
    }

    /*enable the LPI pass through mode */
    tmp_lane_select = ws->lane_select;
    ws->lane_select = WCMOD_LANE_1_1_1_1;
    ws->per_lane_control = WCMOD_ENABLE;
    rvtmp = wcmod_tier1_selector("EEE_PASS_THRU_SET", ws, &rv);
    ws->lane_select = tmp_lane_select;
    SOC_IF_ERROR_RETURN(rvtmp);

    /* set speed and interface   */
    SOC_IF_ERROR_RETURN
       (wcmod_tier1_selector("SET_SPD_INTF", ws, &rv));
    /*config tx driver current */
    SOC_IF_ERROR_RETURN
        (_phy_wcmod_tx_control_get(unit, pc, &tx_drv[0], index));
    SOC_IF_ERROR_RETURN
        (_phy_wcmod_tx_control_set(unit, pc, &tx_drv[0]));

    /* to enable the forced_init_cl72 if 42G higig port */
    if (((pc->speed_max == 42000) || (pCfg->speed == 42000)) && (pCfg->forced_init_cl72)) {
        /*config tx driver current */
        SOC_IF_ERROR_RETURN
            (_phy_wcmod_tx_control_get(unit, pc, &tx_drv[0], TXDRV_AN_INX));
        SOC_IF_ERROR_RETURN
            (_phy_wcmod_tx_control_set(unit, pc, &tx_drv[0]));

        /* forced cl72 aggregate lane modes require per-lane clocking */
        tmp_lane_select     = ws->lane_select;        /* save this */
        ws->lane_select = WCMOD_LANE_BCST; /* go to broadcast mode */
        MODIFY_WC40_CL72_USERB0_CL72_MISC4_CONTROLr(ws->unit, ws, 0xc00, 0xc00);
        ws->lane_select = tmp_lane_select;

        FORCE_CL72_ENABLED(pc) = 1;
        FORCE_CL72_STATE(pc) = WCMOD_CL72_STATE_WAIT_FOR_LINK;
        FORCE_CL72_TICK_CNT(pc) = 0;
        FORCE_CL72_RESTART_CNT(pc) = 0;
        /*first use WCMOD_CL72_woAN for all lanes */
        data = WCMOD_CL72_woAN << 12 | WCMOD_CL72_woAN << 8 |
               WCMOD_CL72_woAN << 4 | WCMOD_CL72_woAN ;
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_UC_INFO_B1_FIRMWARE_MODEr(ws->unit, ws, data));
             
        /*enable cl72 0x82e3[15:14] = 0x3 */
        tmp_lane_select     = ws->lane_select;        /* save this */
        ws->lane_select = WCMOD_LANE_BCST; /* go to broadcast mode */
        data = CL72_USERB0_CL72_MISC1_CONTROL_LINK_CONTROL_FORCE_MASK |
                CL72_USERB0_CL72_MISC1_CONTROL_LINK_CONTROL_FORCEVAL_MASK;
        SOC_IF_ERROR_RETURN(MODIFY_WC40_CL72_USERB0_CL72_MISC1_CONTROLr(unit, ws, data,data));
        /* disable TXFIR force bit */
        MODIFY_WC40_CL72_USERB0_CL72_TX_FIR_TAP_REGISTERr(unit, ws, 0, 
        CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_FORCE_MASK);
             
        /* Disable CL72 timers */
        data = CL72_USERB0_CL72_MISC2_CONTROL_DIS_MAX_WAIT_TIMER_FORCE_MASK |
                CL72_USERB0_CL72_MISC2_CONTROL_DIS_MAX_WAIT_TIMER_FORCEVAL_MASK;
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_CL72_USERB0_CL72_MISC2_CONTROLr(unit, ws, 
                data, data));

        ws->lane_select =  tmp_lane_select; 
        /* Program Core PLL speed by setting 0x81FB[11:7] == 0x16 forced cl72 */
        data = 0x16 << UC_INFO_B1_MISC_CTRL_SERDES_PLL_RATE_SHIFT;
        mask = UC_INFO_B1_MISC_CTRL_SERDES_PLL_RATE_MASK;
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_UC_INFO_B1_REGBr(unit,ws, data, mask));
    }

    /* no uc load for xenia */
    if ((ws->model_type) != WCMOD_XN) {
        if (DEV_CFG_PTR(pc)->load_mthd && !SOC_WARM_BOOT(unit) && !SOC_IS_RELOADING(unit)) { 
            SOC_IF_ERROR_RETURN
                (_phy_wcmod_ucode_get(unit,port,&pdata,&ucode_len,&alloc_flag));
            SOC_IF_ERROR_RETURN
                (phy_wcmod_firmware_load(unit,port,ws,0,pdata, ucode_len));   

            if (alloc_flag) {
                sal_free(pdata);
            }
        } else {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(pc->unit,
                                 "WCMOD combo mode : uC RAM download skipped: u=%d p=%d\n"),
                      unit, port));
        }
    }
    /* enable PLL sequencer   */
    ws->per_lane_control = WCMOD_PLL_ENABLE_NO_WAIT;
    SOC_IF_ERROR_RETURN
       (wcmod_tier1_selector("PLL_SEQUENCER_CONTROL", ws, &rv));

    /* turn on RX sequencer    */
    ws->per_lane_control = WCMOD_ENABLE;
    SOC_IF_ERROR_RETURN
       (wcmod_tier1_selector("RX_SEQ_CONTROL", ws, &rv));
       
    _phy_wcmod_control_vco_disturbed_set(ws->unit, ws->port);

    if (SOC_IS_TRIUMPH3(unit)) {
        if (pCfg->forced_init_cl72 != TRUE) {
            if (!IS_HG_PORT(unit, port)) {
                if (pc->speed_max == 42000) {
                     _phy_wcmod_speed_set(unit, port, 40000);
                } else {
                     _phy_wcmod_speed_set(unit, port, pc->speed_max);   
                }
            } else {
                _phy_wcmod_speed_set(unit, port, pc->speed_max);
            }
        }
    }
     
    /* check to see if we need to recover the enable/disable flag for warm boot */
    if (SOC_WARM_BOOT(unit)) {
        /*read current HW status */
        uint16 data;
        ws->lane_num_ignore = 1;
        SOC_IF_ERROR_RETURN(READ_WC40_XGXSBLK1_LANECTRL3r(unit, ws, &data));
        ws->lane_num_ignore = 0;
        data  &= XGXSBLK1_LANECTRL3_PWRDN_RX_MASK;
        data >>= ws->this_lane;
        data &= 0x1; /*this_lane mask*/
        if (data) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
        } else {
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
        }
    }                

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(pc->unit,
                         "phy_wcmod_init: u=%d p=%d\n"), unit, port));
    return SOC_E_NONE;
}

STATIC int
_phy_wcmod_quad_sgmii_core_init(int unit, soc_port_t port)
{
    phy_ctrl_t         *pc;               
    WCMOD_DEV_DESC_t *pDesc;
    wcmod_st *qsgmii_ws;
    int rv;    
    

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    qsgmii_ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, qsgmii_ws));

    SOC_IF_ERROR_RETURN (_wcmod_getRevDetails(qsgmii_ws));

    /*check if we need to bypass the init for Iproc */
    if (soc_cm_get_bus_type(unit) & SOC_AXI_DEV_TYPE) {
        return SOC_E_NONE;
    }

    qsgmii_ws->this_lane = 0;
    qsgmii_ws->lane_select = WCMOD_LANE_0_0_0_1;

    /* disable PLL      */
    qsgmii_ws->per_lane_control = WCMOD_DISABLE;

    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("PLL_SEQUENCER_CONTROL", qsgmii_ws, &rv));

    /* disable AN on all lanes */
    qsgmii_ws->per_lane_control = 0;
    SOC_IF_ERROR_RETURN
       (wcmod_tier1_selector("AUTONEG_CONTROL", qsgmii_ws, &rv));

    /* disable parallel detect on all lanes */
    qsgmii_ws->per_lane_control = 0;
    SOC_IF_ERROR_RETURN
       (wcmod_tier1_selector("PARALLEL_DETECT_CONTROL", qsgmii_ws, &rv));

    /* disable 100FX on all lanes  */
    qsgmii_ws->per_lane_control = 0;
    SOC_IF_ERROR_RETURN
       (wcmod_tier1_selector("100FX_CONTROL", qsgmii_ws, &rv));

    /* set speed and interface   */
    qsgmii_ws->spd_intf = WCMOD_SPD_1000_SGMII;
    SOC_IF_ERROR_RETURN
       (wcmod_tier1_selector("SET_SPD_INTF", qsgmii_ws, &rv));

    /* enable PLL sequencer   */
    qsgmii_ws->per_lane_control = WCMOD_ENABLE;
    SOC_IF_ERROR_RETURN
       (wcmod_tier1_selector("PLL_SEQUENCER_CONTROL", qsgmii_ws, &rv));

    return SOC_E_NONE;
}


STATIC int
_phy_wcmod_qsgmii_core_init(int unit, soc_port_t port)
{
    phy_ctrl_t         *pc;               
    WCMOD_DEV_DESC_t *pDesc;
    WCMOD_DEV_CFG_t *pCfg;
    wcmod_st *qsgmii_ws;
    int rv, temp_lane_select;
    int tx_inx=0;
    WCMOD_TX_DRIVE_t tx_drv[NUM_LANES];
    int lane_temp = 0 ;
    int preemph=0, main_tap=0, post=0;
    
    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    qsgmii_ws   =  (wcmod_st *)( pDesc + 1);
    pCfg = &pDesc->cfg; 

    {
     
        SOC_IF_ERROR_RETURN
            (_wcmod_phy_parameter_copy(pc, qsgmii_ws));
        temp_lane_select = qsgmii_ws->lane_select;

        SOC_IF_ERROR_RETURN (_wcmod_getRevDetails(qsgmii_ws));
        /* first check if the init is already done in this core */
        if (!_wcmod_chip_init_done(unit,pc->chip_num,pc->phy_mode)) {
            /* for QSGMII mode */
            if (SOC_INFO(unit).port_num_lanes[pc->port] == 1) { 
                qsgmii_ws->lane_select = WCMOD_LANE_1_1_1_1_1_1_1_1;
                qsgmii_ws->aer_bcst_ofs_strap = 0x1f; 

                SOC_IF_ERROR_RETURN
                  (wcmod_tier1_selector("CORE_RESET", qsgmii_ws, &rv));

                /* disable PLL      */
                qsgmii_ws->per_lane_control = WCMOD_DISABLE;

                SOC_IF_ERROR_RETURN
                  (wcmod_tier1_selector("PLL_SEQUENCER_CONTROL", qsgmii_ws, &rv));

                qsgmii_ws->port_type  = WCMOD_INDEPENDENT;
                qsgmii_ws->os_type = WCMOD_OS6;
                SOC_IF_ERROR_RETURN
                  (wcmod_tier1_selector("SET_PORT_MODE", qsgmii_ws, &rv));
                                                        
                /* turn off RX sequencer    */
                qsgmii_ws->per_lane_control = WCMOD_DISABLE;
                SOC_IF_ERROR_RETURN
                    (wcmod_tier1_selector("RX_SEQ_CONTROL", qsgmii_ws, &rv));
                qsgmii_ws->lane_select = temp_lane_select;

                /* disable AN on all lanes */
                qsgmii_ws->per_lane_control = 0;
                qsgmii_ws->lane_select = WCMOD_LANE_1_1_1_1_1_1_1_1;
                SOC_IF_ERROR_RETURN
                   (wcmod_tier1_selector("AUTONEG_CONTROL", qsgmii_ws, &rv));

                /* enable parallel detect on all lanes */
                qsgmii_ws->per_lane_control = 1;
                qsgmii_ws->lane_select = WCMOD_LANE_1_1_1_1_1_1_1_1;
                SOC_IF_ERROR_RETURN
                   (wcmod_tier1_selector("PARALLEL_DETECT_CONTROL", qsgmii_ws, &rv));
                
                /* disable 100FX on all lanes  */
                qsgmii_ws->per_lane_control = 0;
                SOC_IF_ERROR_RETURN
                   (wcmod_tier1_selector("100FX_CONTROL", qsgmii_ws, &rv));


                /* QS_A0 configure the TX driver parameters per speed mode */  
                qsgmii_ws->lane_select = WCMOD_LANE_1_1_1_1_1_1_1_1;
                tx_inx = TXDRV_6GOS2_INX;
                SOC_IF_ERROR_RETURN
                    (_phy_wcmod_tx_control_get(unit, pc, &tx_drv[0],tx_inx));
                SOC_IF_ERROR_RETURN
                    (_phy_wcmod_tx_control_set(unit, pc, &tx_drv[0]));

                qsgmii_ws->lane_select = WCMOD_LANE_0_0_0_1_0_0_0_0 ;
                lane_temp              = qsgmii_ws->this_lane;
                qsgmii_ws->this_lane   = 4 ;
                tx_inx = TXDRV_6GOS2_INX;
                SOC_IF_ERROR_RETURN
                    (_phy_wcmod_tx_control_get(unit, pc, &tx_drv[0],tx_inx));

                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_TX0_TX_DRIVERr(unit, qsgmii_ws, tx_drv[0].idrive << 6, 0x3f << 6));

                preemph = tx_drv[0].u.preemph ;
                main_tap =(preemph & CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_MAIN_MASK) 
                           >> CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_MAIN_SHIFT;       
                post     =(preemph & CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_POST_MASK) 
                           >> CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_POST_SHIFT;
   
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_TX0_ANATXACONTROL2r(unit, qsgmii_ws,  main_tap << 10, 0x7c00));    /* 0x8066 */
                SOC_IF_ERROR_RETURN 
                    (MODIFY_WC40_TX0_ANATXACONTROL2r(unit, qsgmii_ws, (post & 0x1) << 15, 0x8000)); 
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_TX0_TX_DRIVERr(unit, qsgmii_ws, (post & 0x1e) >> 1, 0xf));         /* 0x8067 */
  
                /* recover */
                qsgmii_ws->this_lane   = lane_temp ;
                qsgmii_ws->lane_select = WCMOD_LANE_1_1_1_1_1_1_1_1;

                /* set speed and interface   */
                qsgmii_ws->spd_intf = WCMOD_SPD_1000_SGMII;
                SOC_IF_ERROR_RETURN
                   (wcmod_tier1_selector("SET_SPD_INTF", qsgmii_ws, &rv));
                qsgmii_ws->lane_select = temp_lane_select;

                /* enable PLL sequencer   */
                qsgmii_ws->per_lane_control = WCMOD_ENABLE;
                SOC_IF_ERROR_RETURN
                   (wcmod_tier1_selector("PLL_SEQUENCER_CONTROL", qsgmii_ws, &rv));

                /* turn on RX sequencer    */
                qsgmii_ws->per_lane_control = WCMOD_ENABLE;
                SOC_IF_ERROR_RETURN
                   (wcmod_tier1_selector("RX_SEQ_CONTROL", qsgmii_ws, &rv));
                qsgmii_ws->lane_select = temp_lane_select;

                /*check to see if we need to enable SGMII autoneg */
                if (pCfg->phy_sgmii_an) {
                    qsgmii_ws->lane_select = WCMOD_LANE_BCST; /* go to broadcast mode */
                    SOC_IF_ERROR_RETURN (MODIFY_WC40_IEEE0BLK_MIICNTLr(qsgmii_ws->unit, qsgmii_ws,
                                             IEEE0BLK_MIICNTL_AUTONEG_ENABLE_MASK,
                                             IEEE0BLK_MIICNTL_AUTONEG_ENABLE_MASK));
                } else {
                    if (pc->speed_max > 1000) {
                        _phy_wcmod_speed_set(unit, port, pc->speed_max);
                    }
                }
                qsgmii_ws->lane_select = temp_lane_select;
            /* combo mode for QSGMII  for HR2/HX4 */
            } else {
                temp_lane_select = qsgmii_ws->lane_select;
                qsgmii_ws->lane_select = WCMOD_LANE_0_0_0_1_0_0_0_1;
                qsgmii_ws->aer_bcst_ofs_strap = 0x1f; 

                SOC_IF_ERROR_RETURN
                  (wcmod_tier1_selector("CORE_RESET", qsgmii_ws, &rv));

                /* disable PLL      */
                qsgmii_ws->per_lane_control = WCMOD_DISABLE;

                SOC_IF_ERROR_RETURN
                  (wcmod_tier1_selector("PLL_SEQUENCER_CONTROL", qsgmii_ws, &rv));

                qsgmii_ws->port_type  = WCMOD_COMBO;
                qsgmii_ws->lane_select = WCMOD_LANE_BCST;
                /*Put the core into combo mode */
                SOC_IF_ERROR_RETURN (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit, qsgmii_ws,
                                                                       (XGXSBLK0_XGXSCONTROL_MODE_10G_ComboCoreMode <<
                                                                        XGXSBLK0_XGXSCONTROL_MODE_10G_SHIFT),
                                                                       XGXSBLK0_XGXSCONTROL_MODE_10G_MASK));
                /*disable the QSGMII mode for both tx/rx only lane 0 or lane 4 is valid for this write*/
                SOC_IF_ERROR_RETURN(MODIFY_WC40_XGXSBLK1_LANECTRL0r(unit, qsgmii_ws, 0x0, 0xff00));
 
                /* enable the sgmii mode also */
                if (pCfg->fiber_pref) {
                    SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(pc->unit, qsgmii_ws, 0x1,
                                                                         SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK));
                } else {
                    SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(pc->unit, qsgmii_ws, 0x0,
                                                                         SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK));
                }                                         
                /* turn off RX sequencer    */
                qsgmii_ws->per_lane_control = WCMOD_DISABLE;
                SOC_IF_ERROR_RETURN
                    (wcmod_tier1_selector("RX_SEQ_CONTROL", qsgmii_ws, &rv));
                qsgmii_ws->lane_select = temp_lane_select;

                /* disable AN on all lanes */
                qsgmii_ws->per_lane_control = 0;
                qsgmii_ws->lane_select = WCMOD_LANE_1_1_1_1_1_1_1_1;
                SOC_IF_ERROR_RETURN
                   (wcmod_tier1_selector("AUTONEG_CONTROL", qsgmii_ws, &rv));

                /* enable parallel detect on all lanes */
                qsgmii_ws->per_lane_control = 1;
                qsgmii_ws->lane_select = WCMOD_LANE_1_1_1_1_1_1_1_1;
                SOC_IF_ERROR_RETURN
                   (wcmod_tier1_selector("PARALLEL_DETECT_CONTROL", qsgmii_ws, &rv));

                /* disable 100FX on all lanes  */
                qsgmii_ws->per_lane_control = 0;
                SOC_IF_ERROR_RETURN
                   (wcmod_tier1_selector("100FX_CONTROL", qsgmii_ws, &rv));

                /* enable PLL sequencer   */
                qsgmii_ws->per_lane_control = WCMOD_ENABLE;
                SOC_IF_ERROR_RETURN
                   (wcmod_tier1_selector("PLL_SEQUENCER_CONTROL", qsgmii_ws, &rv));

                /* turn on RX sequencer    */
                qsgmii_ws->per_lane_control = WCMOD_ENABLE;
                SOC_IF_ERROR_RETURN
                   (wcmod_tier1_selector("RX_SEQ_CONTROL", qsgmii_ws, &rv));
                qsgmii_ws->lane_select = temp_lane_select;

                /*check to see if we need to enable SGMII autoneg */
                if (pCfg->phy_sgmii_an) {
                    qsgmii_ws->lane_select = WCMOD_LANE_BCST; /* go to broadcast mode */
                    SOC_IF_ERROR_RETURN (MODIFY_WC40_IEEE0BLK_MIICNTLr(qsgmii_ws->unit, qsgmii_ws,
                                             IEEE0BLK_MIICNTL_AUTONEG_ENABLE_MASK,
                                             IEEE0BLK_MIICNTL_AUTONEG_ENABLE_MASK));
                } 
            }
        }
        pc->flags |= PHYCTRL_INIT_DONE;

        /* Change the lane number depending on which half of the QSGMII core
               is being used by this port */
        temp_lane_select = qsgmii_ws->lane_select;
        if (qsgmii_ws->this_lane < 4) {
            qsgmii_ws->lane_select = WCMOD_LANE_0_0_0_1;
        } else {
            qsgmii_ws->lane_select = WCMOD_LANE_0_0_0_1_0_0_0_0;
        }

        /* TX polarity inversion has effect if any of the ports set it */
        if (pCfg->txpol) {
            qsgmii_ws->per_lane_control = (pCfg->txpol <<2) ;
            qsgmii_ws->per_lane_control |=  0x1;
            qsgmii_ws->per_lane_control |=  0x2; /*set TX*/
            SOC_IF_ERROR_RETURN
                (wcmod_tier1_selector("SET_POLARITY", qsgmii_ws, &rv));
        }

        /* RX polarity inversion has effect if any of the ports set it */
        if (pCfg->rxpol) {
            qsgmii_ws->per_lane_control = (pCfg->rxpol <<2) ;
            qsgmii_ws->per_lane_control |=  0x1;
            SOC_IF_ERROR_RETURN
                (wcmod_tier1_selector("SET_POLARITY", qsgmii_ws, &rv));
        }

        qsgmii_ws->lane_select = temp_lane_select;

        /*force the speed to its maximum 2.5G fiber mode if not 1G */
        if ((pCfg->phy_sgmii_an == 0) && (pc->speed_max > 1000)) {
            _phy_wcmod_speed_set(unit, port, pc->speed_max);
        }
    }
    return SOC_E_NONE;
}

STATIC int
_phy_wcmod_multi_core_init(int unit, soc_port_t port)
{
    phy_ctrl_t         *pc;               
    WCMOD_DEV_DESC_t *pDesc;
    wcmod_st *ws, *temp_ws;
    WCMOD_DEV_CFG_t *pCfg;
    uint8 *pdata;
    int   ucode_len;
    int   alloc_flag;
    int rv, i;
    int num_core = 0;
    phyident_core_info_t core_info[8];  
    int array_max = 8;
    int array_size = 0;
    uint16 data;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    pCfg = &pDesc->cfg; 
    temp_ws   =  (wcmod_st *)( pDesc + 1);

    /* get the mdio address */
    SOC_IF_ERROR_RETURN
        (soc_phy_addr_multi_get(unit, port, array_max, &array_size, &core_info[0]));

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, temp_ws));

    /*first check if specail Arad+ ILKN port */
    if (core_info[0].index_in_mld == PHYIDENT_INFO_NOT_SET) {
        num_core = array_size;
        for( i = 0; i < num_core; i++ ) {
             ws   =  (wcmod_st *)( temp_ws + i);
            SOC_IF_ERROR_RETURN (_wcmod_getRevDetails(ws));
            /*check to see if we need to initialize the warpcore */
            if (core_info[i].nof_lanes_in_core == 4) {
                int tmp_lane_select = ws->lane_select;
                ws->this_lane = 0;
                SOC_IF_ERROR_RETURN
                  (wcmod_tier1_selector("CORE_RESET", ws, &rv));

                /* disable PLL      */
                ws->per_lane_control = WCMOD_DISABLE;

                SOC_IF_ERROR_RETURN
                  (wcmod_tier1_selector("PLL_SEQUENCER_CONTROL", ws, &rv));

                ws->port_type  = WCMOD_INDEPENDENT;
                ws->os_type = WCMOD_OS5;
                SOC_IF_ERROR_RETURN
                  (wcmod_tier1_selector("SET_PORT_MODE", ws, &rv));
                                                            
                /* turn off RX sequencer    */
                ws->per_lane_control = WCMOD_DISABLE;
                ws->lane_select = WCMOD_LANE_1_1_1_1;
                SOC_IF_ERROR_RETURN
                    (wcmod_tier1_selector("RX_SEQ_CONTROL", ws, &rv));

                /* disable AN on all lanes */
                ws->per_lane_control = 0;
                SOC_IF_ERROR_RETURN
                   (wcmod_tier1_selector("AUTONEG_CONTROL", ws, &rv));

                /* disable parallel detect on all lanes */
                ws->per_lane_control = 0;
                SOC_IF_ERROR_RETURN
                   (wcmod_tier1_selector("PARALLEL_DETECT_CONTROL", ws, &rv));

                /* disable 100FX on all lanes  */
                ws->per_lane_control = 0;
                SOC_IF_ERROR_RETURN
                   (wcmod_tier1_selector("100FX_CONTROL", ws, &rv));

                /* set speed and interface to be 10.3125G pcs bypass  */
                ws->spd_intf = WCMOD_SPD_PCSBYP_10P3125;
                SOC_IF_ERROR_RETURN
                   (wcmod_tier1_selector("SET_SPD_INTF", ws, &rv));

                ws->lane_select = tmp_lane_select;
                /*set the firmware mode properly based on the soc config */
                data = (pCfg->firmware_mode) | (pCfg->firmware_mode) << 4 |
                        (pCfg->firmware_mode) << 8 | (pCfg->firmware_mode) << 12; 
                SOC_IF_ERROR_RETURN
                    (WRITE_WC40_UC_INFO_B1_FIRMWARE_MODEr(ws->unit, ws, data));

                /*config the lane swap for this core */
                ws->per_lane_control = (pCfg->txlane_map[i]) << 16 | (pCfg->rxlane_map[i]);
                SOC_IF_ERROR_RETURN
                   (wcmod_tier1_selector("LANE_SWAP", ws, &rv));

                /* no uc load for xenia */
                if ((ws->model_type) != WCMOD_XN) {
                    if (DEV_CFG_PTR(pc)->load_mthd && !SOC_WARM_BOOT(unit) && !SOC_IS_RELOADING(unit)) { 
                        SOC_IF_ERROR_RETURN
                            (_phy_wcmod_ucode_get(unit,port,&pdata,&ucode_len,&alloc_flag));
                        SOC_IF_ERROR_RETURN
                            (phy_wcmod_firmware_load(unit,port,ws,0,pdata, ucode_len));   

                        if (alloc_flag) {
                            sal_free(pdata);
                        }
                    } else {
                        LOG_WARN(BSL_LS_SOC_PHY,
                                 (BSL_META_U(pc->unit,
                                             "WCMOD multi core mode : uC RAM download skipped: u=%d p=%d\n"),
                                  unit, port));
                    }
                }
                /* enable PLL sequencer   */
                ws->per_lane_control = WCMOD_PLL_ENABLE_NO_WAIT;
                SOC_IF_ERROR_RETURN
                   (wcmod_tier1_selector("PLL_SEQUENCER_CONTROL", ws, &rv));

                ws->lane_select = WCMOD_LANE_1_1_1_1;
                /* turn on RX sequencer    */
                ws->per_lane_control = WCMOD_ENABLE;
                SOC_IF_ERROR_RETURN
                   (wcmod_tier1_selector("RX_SEQ_CONTROL", ws, &rv));
                ws->lane_select = tmp_lane_select;
                
                _phy_wcmod_control_vco_disturbed_set(ws->unit, ws->port);
            } else {
                ws->this_lane = core_info[i].first_phy_in_core;
            }
        }
        /* check to see if we need to recover the enable/disable flag for warm boot */
        if (SOC_WARM_BOOT(unit)) {
            /*read current HW status */
            uint16 data;
            temp_ws->lane_num_ignore = 1;
            SOC_IF_ERROR_RETURN(READ_WC40_XGXSBLK1_LANECTRL3r(unit, temp_ws, &data));
            temp_ws->lane_num_ignore = 0;
            data  &= XGXSBLK1_LANECTRL3_PWRDN_RX_MASK;
            data >>= temp_ws->this_lane;
            data &= 0x1; /*this_lane mask*/
            if (data) {
                PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
            } else {
                PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
            }
        }                
    } else {
        SOC_IF_ERROR_RETURN
            (_wcmod_phy_parameter_copy(pc, temp_ws));

        ws = temp_ws;
        num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;

        for( i = 0; i < num_core; i++ ) {
            ws   =  (wcmod_st *)( temp_ws + i);
            SOC_IF_ERROR_RETURN (_wcmod_getRevDetails(ws));

            SOC_IF_ERROR_RETURN(READ_WC40_UC_INFO_B1_VERSIONr(unit, ws, &data));
            if(data != 0) {
                continue;
            }

            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("CORE_RESET", ws, &rv));

            /* disable PLL      */
            ws->per_lane_control = WCMOD_DISABLE;

            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("PLL_SEQUENCER_CONTROL", ws, &rv));

            ws->port_type  = WCMOD_INDEPENDENT;
            ws->os_type = WCMOD_OS5;
            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("SET_PORT_MODE", ws, &rv));
                                                            
            /* turn off RX sequencer    */
            ws->per_lane_control = WCMOD_DISABLE;
            SOC_IF_ERROR_RETURN
                (wcmod_tier1_selector("RX_SEQ_CONTROL", ws, &rv));

            /* disable AN on all lanes */
            ws->per_lane_control = 0;
            SOC_IF_ERROR_RETURN
               (wcmod_tier1_selector("AUTONEG_CONTROL", ws, &rv));

            /* disable parallel detect on all lanes */
            ws->per_lane_control = 0;
            SOC_IF_ERROR_RETURN
               (wcmod_tier1_selector("PARALLEL_DETECT_CONTROL", ws, &rv));

            /* disable 100FX on all lanes  */
            ws->per_lane_control = 0;
            SOC_IF_ERROR_RETURN
               (wcmod_tier1_selector("100FX_CONTROL", ws, &rv));

            /* set speed and interface   */
            SOC_IF_ERROR_RETURN
               (wcmod_tier1_selector("SET_SPD_INTF", ws, &rv));

            /*set the firmware mode properly based on the soc config */
            data = (pCfg->firmware_mode) | (pCfg->firmware_mode) << 4 |
                    (pCfg->firmware_mode) << 8 | (pCfg->firmware_mode) << 12; 
            SOC_IF_ERROR_RETURN
                (WRITE_WC40_UC_INFO_B1_FIRMWARE_MODEr(ws->unit, ws, data));

            /*config the lane swap for this core */
            ws->per_lane_control = (pCfg->txlane_map[i]) << 16 | (pCfg->rxlane_map[i]);
            SOC_IF_ERROR_RETURN
               (wcmod_tier1_selector("LANE_SWAP", ws, &rv));

            /* tr3 WC pll lock workaround */
            if ((((ws->model_type) == WCMOD_WC_C0) && SOC_IS_TRIUMPH3(unit)) || SOC_IS_ARAD(unit)) {
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_PLL_ANAPLLACONTROL1r(unit,ws, 
                           0x1 << PLL_ANAPLLACONTROL1_ICLKIBUF4_SHIFT,
                           PLL_ANAPLLACONTROL1_ICLKIBUF4_MASK));
            }

            /* no uc load for xenia */
            if ((ws->model_type) != WCMOD_XN) {
                if (DEV_CFG_PTR(pc)->load_mthd && !SOC_WARM_BOOT(unit) && !SOC_IS_RELOADING(unit)) { 
                    SOC_IF_ERROR_RETURN
                        (_phy_wcmod_ucode_get(unit,port,&pdata,&ucode_len,&alloc_flag));
                    SOC_IF_ERROR_RETURN
                        (phy_wcmod_firmware_load(unit,port,ws,0,pdata, ucode_len));   

                    if (alloc_flag) {
                        sal_free(pdata);
                    }
                } else {
                    LOG_WARN(BSL_LS_SOC_PHY,
                             (BSL_META_U(pc->unit,
                                         "WCMOD multi core mode : uC RAM download skipped: u=%d p=%d\n"),
                              unit, port));
                }
            }
            /* enable PLL sequencer   */
            ws->per_lane_control = WCMOD_PLL_ENABLE_NO_WAIT;
            SOC_IF_ERROR_RETURN
               (wcmod_tier1_selector("PLL_SEQUENCER_CONTROL", ws, &rv));

            /* turn on RX sequencer    */
            ws->per_lane_control = WCMOD_ENABLE;
            SOC_IF_ERROR_RETURN
               (wcmod_tier1_selector("RX_SEQ_CONTROL", ws, &rv));
               
            _phy_wcmod_control_vco_disturbed_set(ws->unit, ws->port);
        } /* for loop */

        if (num_core <= 3) {
        /* set the mld mdio address */
            ws   =  (wcmod_st *)( temp_ws + num_core);
            ws->phy_ad = core_info[num_core].mdio_addr;
        } else {
            ws   =  (wcmod_st *)( temp_ws + num_core);
            ws->phy_ad = core_info[num_core].mdio_addr;
            ws   =  (wcmod_st *)( temp_ws + num_core + 1);
            ws->phy_ad = core_info[num_core+1].mdio_addr;
        }
        
        /*config the warpCore polarity */
        SOC_IF_ERROR_RETURN
            (_phy_wcmod_multi_core_polarity_config(unit, port));

        /*program the MLD lane swap */
        SOC_IF_ERROR_RETURN
            (_phy_wcmod_mld_lane_swap_config(unit, port));

        if  (SOC_IS_ARAD(unit)) {
            if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), port)) {
                if (core_info[0].index_in_mld != PHYIDENT_INFO_NOT_SET) {
                    SOC_IF_ERROR_RETURN
                        (phy_wcmod_interlaken_config(unit, port, pCfg->speed));     
                }
            } else {
                if (pCfg->speed != WCMOD_NO_CFG_VALUE) {
                    phy_wcmod_100g_plus_config(unit, port, pCfg->speed);
                } else {
                    phy_wcmod_100g_plus_config(unit, port, 100000);
                }
            }
        }

        /* set to the max speed */
        if (SOC_IS_TRIUMPH3(unit)) {
            if (!SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) {
                _phy_wcmod_speed_set(unit, port, pc->speed_max);
            }
        }

        /* check to see if we need to recover the enable/disable flag for warm boot */
        if (SOC_WARM_BOOT(unit)) {
            /*read current HW status */
            uint16 data;
            temp_ws->lane_num_ignore = 1;
            SOC_IF_ERROR_RETURN(READ_WC40_XGXSBLK1_LANECTRL3r(unit, temp_ws, &data));
            temp_ws->lane_num_ignore = 0;
            data  &= XGXSBLK1_LANECTRL3_PWRDN_RX_MASK;
            data >>= temp_ws->this_lane;
            data &= 0x1; /*this_lane mask*/
            if (data) {
                PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
            } else {
                PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
            }
        }                
    }
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(pc->unit,
                         "phy_wcmod_init: u=%d p=%d\n"), unit, port));
    return SOC_E_NONE;
}

STATIC int
_phy_wcmod_custom_mode_init(int unit, soc_port_t port)
{
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_wcmod_ind_init_common
 * Purpose:     
 *      Initialization common for hc phys.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 * Returns:     
 *      SOC_E_NONE
 */
int
_phy_wcmod_ind_init_common(wcmod_st *ws, int fw_load_method) {
    uint8 *pdata;
    int   ucode_len;
    int   alloc_flag;
    int rv;
    uint16 data16;
    int tmp_lane_select;
    int spd_intf;

    /* configure and initialize the resource shared by all 4 lanes*/

    /* first check if Multiport enable is set or not*/
    SOC_IF_ERROR_RETURN (READ_WC40_XGXSBLK0_MMDSELECTr(ws->unit, ws,  &data16));
    data16  &= XGXSBLK0_MMDSELECT_MULTIPRTS_EN_MASK;

    /* configure and initialize the resource shared by all 4 lanes*/
    if (SOC_IS_FE1600(ws->unit) || (SOC_IS_ARAD(ws->unit) && IS_SFI_PORT(ws->unit, ws->port)))
    {
        SOC_IF_ERROR_RETURN
            (wcmod_tier1_selector("SOFT_RESET", ws, &rv));
    } else {
        SOC_IF_ERROR_RETURN
            (wcmod_tier1_selector("CORE_RESET", ws, &rv));
    }

    /* restore back the multi port enable if that bit is set */
    if (data16) {
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_XGXSBLK0_MMDSELECTr(ws->unit, ws, 
             XGXSBLK0_MMDSELECT_MULTIPRTS_EN_MASK, XGXSBLK0_MMDSELECT_MULTIPRTS_EN_MASK));
    }

    /* disable PLL      */
    ws->per_lane_control = WCMOD_DISABLE;
    ws->port_type  = WCMOD_INDEPENDENT;
   
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("PLL_SEQUENCER_CONTROL", ws, &rv));

    ws->port_type  = WCMOD_INDEPENDENT;
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("SET_PORT_MODE", ws, &rv));
                                                            
    /* turn off RX sequencer    */
    ws->per_lane_control = WCMOD_DISABLE;
    SOC_IF_ERROR_RETURN
        (wcmod_tier1_selector("RX_SEQ_CONTROL", ws, &rv));

    /* turn off clause 72     */
    tmp_lane_select = ws->lane_select;
    ws->lane_select = WCMOD_LANE_1_1_1_1; 
    ws->per_lane_control = 0;
    SOC_IF_ERROR_RETURN
       (wcmod_tier1_selector("CLAUSE_72_CONTROL", ws, &rv));

    /* disable AN on all lanes */
    ws->per_lane_control = 0;
    SOC_IF_ERROR_RETURN
       (wcmod_tier1_selector("AUTONEG_CONTROL", ws, &rv));

    /* disable parallel detect on all lanes */
    ws->per_lane_control = 0;
    SOC_IF_ERROR_RETURN
       (wcmod_tier1_selector("PARALLEL_DETECT_CONTROL", ws, &rv));


    /* disable 100FX on all lanes  */
    ws->per_lane_control = 0;
    SOC_IF_ERROR_RETURN
       (wcmod_tier1_selector("100FX_CONTROL", ws, &rv));
    ws->lane_select = tmp_lane_select; 


    /* set the default lane swap */
    ws->per_lane_control = ws->lane_swap;
    SOC_IF_ERROR_RETURN
       (wcmod_tier1_selector("LANE_SWAP", ws, &rv));

    /* tr3 WC pll lock workaround */
    if ((((ws->model_type) == WCMOD_WC_C0) && SOC_IS_TRIUMPH3(ws->unit)) || SOC_IS_ARAD(ws->unit)) {
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_PLL_ANAPLLACONTROL1r(ws->unit,ws, 
                   0x1 << PLL_ANAPLLACONTROL1_ICLKIBUF4_SHIFT,
                   PLL_ANAPLLACONTROL1_ICLKIBUF4_MASK));
    }

    /*enable the LPI pass through mode */
    tmp_lane_select = ws->lane_select;
    ws->lane_select = WCMOD_LANE_1_1_1_1;
    ws->per_lane_control = WCMOD_ENABLE;
    SOC_IF_ERROR_RETURN
       (wcmod_tier1_selector("EEE_PASS_THRU_SET", ws, &rv));
    ws->lane_select = tmp_lane_select;

    /*added the workaround for lane 0 rxclk issue on Arad */
    if (SOC_IS_ARAD(ws->unit)) {
        tmp_lane_select = ws->lane_select;
        spd_intf = ws->spd_intf;
        ws->spd_intf = WCMOD_SPD_10000_XFI;
        ws->lane_select = WCMOD_LANE_1_1_1_1;
        SOC_IF_ERROR_RETURN
           (wcmod_tier1_selector("SET_SPD_INTF", ws, &rv));
        ws->lane_select = tmp_lane_select;
        ws->spd_intf = spd_intf;
    }

    /* set speed and interface   */
    tmp_lane_select = ws->lane_select;
    ws->lane_select = WCMOD_LANE_1_1_1_1;
    SOC_IF_ERROR_RETURN
       (wcmod_tier1_selector("SET_SPD_INTF", ws, &rv));
    ws->lane_select = tmp_lane_select;

    /* set the default firmware mode */
    data16 = (ws->firmware_mode) | (ws->firmware_mode) << 4 |
            (ws->firmware_mode) << 8 | (ws->firmware_mode) << 12; 
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_UC_INFO_B1_FIRMWARE_MODEr(ws->unit, ws, data16));

    if ((ws->model_type) != WCMOD_XN) {
        if (fw_load_method  && !SOC_WARM_BOOT(ws->unit) && !SOC_IS_RELOADING(ws->unit)) { 
            SOC_IF_ERROR_RETURN
                (_phy_wcmod_ucode_get(ws->unit,ws->port,&pdata,&ucode_len,&alloc_flag));
            SOC_IF_ERROR_RETURN
                (phy_wcmod_firmware_load(ws->unit,ws->port,ws,0,pdata, ucode_len));   

            if (alloc_flag) {
                sal_free(pdata);
            }
        }
    }

    /* enable PLL sequencer   */
    ws->per_lane_control = WCMOD_PLL_ENABLE_NO_WAIT;
    SOC_IF_ERROR_RETURN
       (wcmod_tier1_selector("PLL_SEQUENCER_CONTROL", ws, &rv));

    ws->per_lane_control = WCMOD_ENABLE;
    SOC_IF_ERROR_RETURN
        (wcmod_tier1_selector("RX_SEQ_CONTROL", ws, &rv));
        
    _phy_wcmod_control_vco_disturbed_set(ws->unit, ws->port); 

    return SOC_E_NONE;
       
}


/*
 * Function:
 *      phy_wcmod_ind_init
 * Purpose:     
 *      Initialize hc phys
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
_phy_wcmod_ind_init(int unit, soc_port_t port)
{
    phy_ctrl_t         *pc;               
    WCMOD_DEV_DESC_t *pDesc;
    WCMOD_DEV_CFG_t *pCfg;
    wcmod_st *ws;
    int  tmp_lane_select ;
    uint16 data16, mask16, mode_1000x ;
    WCMOD_TX_DRIVE_t tx_drv[MAX_NUM_LANES];
    wcmod_spd_intf_params_t spd_params;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);
    pCfg = &pDesc->cfg; 

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));
    SOC_IF_ERROR_RETURN (_wcmod_getRevDetails(ws));
  
    ws->lane_swap = (pCfg->txlane_map[0]) << 16 | (pCfg->rxlane_map[0]);
    ws->tx_pol =  pCfg->txpol;
    ws->rx_pol =  pCfg->rxpol;

    ws->firmware_mode = pCfg->firmware_mode;
    ws->fiber_pref = pCfg->fiber_pref;
    ws->asymmetric_mode = pCfg->asymmetric_mode;

    if (pCfg->speed == WCMOD_NO_CFG_VALUE) {
        ws->spd_intf = WCMOD_SPD_ILLEGAL;
    } else {
        if (pCfg->speed == 0) {
            pCfg->speed = pc->speed_max;
        }        
        SOC_IF_ERROR_RETURN(_phy_wcmod_ind_speed_intf_get(unit, port, pCfg->speed, &spd_params));
        ws->spd_intf = spd_params.spd_intf;
        if(SOC_PHY_NON_CANNED_SPEED != pCfg->speed && WCMOD_SPD_CUSTOM_BYPASS == spd_params.spd_intf) {
            ws->oversample_mode = spd_params.oversample_mode;
            ws->pll_divider = spd_params.pll_divider;
            ws->vco_freq = spd_params.vco_freq;
        }
    }

    ws->port_type  = WCMOD_INDEPENDENT;
    /* for TR3, use default os 8 */
    if (ws->model_type == WCMOD_WC_C0) {
        if (pCfg->serdes_1000x_at_6250_vco) {
            ws->os_type    = WCMOD_OS5 ;
        } else {        
            ws->os_type    = WCMOD_OS8; 
            if(ws->dual_type) {   /* WCMOD_OS correction for dual port */
                if(pc->speed_max <=17000 ) {
                    /* printf ("os_type corrected by max_spd=%0d\n", pc->speed_max) ;  */
                    ws->os_type    = WCMOD_OS5 ;
                }
            }
        }
    }

    if ((ws->model_type) == WCMOD_XN) {
        DEV_CFG_PTR(pc)->lane_mode = xgxs_operationModes_XGXS;
    }

    if (!_wcmod_chip_init_done(unit,pc->chip_num,pc->phy_mode)) {
        /* configure and initialize the resource shared by all 4 lanes */

        if (ws->model_type == WCMOD_WC_C0) {
            /* warpcore, frist check if 1G needs to be run at 6.25G vco */
            if (pCfg->serdes_1000x_at_6250_vco) {
                ws->os_type    = WCMOD_OS5 ;
            } else {        
                ws->os_type    = WCMOD_OS8; 
                if(ws->dual_type) {   /* WCMOD_OS correction for dual port */
                    if(pc->speed_max <=17000 ) {
                        /* printf ("os_type corrected by max_spd=%0d\n", pc->speed_max) ;  */
                        ws->os_type    = WCMOD_OS5 ;
                    }
                }
            }
        }

        _phy_wcmod_ind_init_common(ws, DEV_CFG_PTR(pc)->load_mthd);

        if(!(DEV_CFG_PTR(pc)->load_mthd)) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(pc->unit,
                                 "WCMOD independent mode : uC RAM download skipped: u=%d p=%d\n"),
                      ws->unit, ws->port));
        }
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(pc->unit,
                             "phy_wcmod_init: u=%d p=%d\n"), unit, port));
    }

    if (pCfg->speed != WCMOD_NO_CFG_VALUE) {
        /*config tx driver current */
        SOC_IF_ERROR_RETURN(_phy_wcmod_tx_control_get(unit, pc, &tx_drv[0], spd_params.tx_inx));
        SOC_IF_ERROR_RETURN(_phy_wcmod_tx_control_set(unit, pc, &tx_drv[0]));
    }

    if ((ws->model_type) == WCMOD_WC_C0) {
        tmp_lane_select  = ws->lane_select ; 
        ws->lane_select  = WCMOD_LANE_BCST ;
        /* config RX clock compensation used in single-lane and dual-lane ports */
        if (IS_DUAL_LANE_PORT(pc)) { 
            SOC_IF_ERROR_RETURN
                (WRITE_WC40_XGXSBLK2_UNICOREMODE10Gr(unit, ws, 
                 0x8000 | IND_CLOCK_COMP_DEFAULT));
        } else {
            SOC_IF_ERROR_RETURN
            (MODIFY_WC40_RX66_CONTROLr(unit, ws, 
                RX66_CONTROL_CC_EN_MASK | RX66_CONTROL_CC_DATA_SEL_MASK, 
                RX66_CONTROL_CC_EN_MASK | RX66_CONTROL_CC_DATA_SEL_MASK));
        }
        
        /*  support back to back packets in higig+/higig2 mode without IPG */
        if (IS_DUAL_LANE_PORT(pc)) {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_XGXSBLK0_XGMIIRCONTROLr(unit, ws, 
            DEV_CFG_PTR(pc)->hg_mode? XGXSBLK0_XGMIIRCONTROL_CKCMP_NOIPG_EN_MASK: 0, 
                XGXSBLK0_XGMIIRCONTROL_CKCMP_NOIPG_EN_MASK));
        } else { /* quad-port mode */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_RX66_CONTROLr(unit, ws,
            DEV_CFG_PTR(pc)->hg_mode? RX66_CONTROL_CC_NOIPG_EN_MASK: 0, 
                RX66_CONTROL_CC_NOIPG_EN_MASK));
        }
        ws->lane_select  = tmp_lane_select ;
    }
    
    if ((ws->model_type) == WCMOD_XN) {
       
       data16 = MII_CTRL_FD | MII_CTRL_SS_1000;
       SOC_IF_ERROR_RETURN
          (MODIFY_WC40_COMBO_IEEE0_MIICNTLr(ws->unit, ws, data16, data16)) ;
       
       if ((PHY_FIBER_MODE(unit, port) && !PHY_EXTERNAL_MODE(unit, port)) ||
           PHY_PASSTHRU_MODE(unit, port) ||
           PHY_SGMII_AUTONEG_MODE(unit, port)) {
          /* Enable 1000X parallel detect */
          data16 = SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK;
          mask16 = SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK;
       } else {
          data16 = 0;
          mask16 = SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK |
             SERDESDIGITAL_CONTROL1000X2_FILTER_FORCE_LINK_MASK;
       }
       SOC_IF_ERROR_RETURN
          (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X2r
           (unit,ws, data16, mask16)) ;
 
       mode_1000x = 0;
       if ( (PHY_FIBER_MODE(unit, port) && !PHY_EXTERNAL_MODE(unit, port)) ||
            PHY_PASSTHRU_MODE(unit, port)) {
          mode_1000x = SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;
       }
       
       /* Allow property to override */
       mask16 = SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;
       if (soc_property_port_get(unit, port,
                                 spn_SERDES_FIBER_PREF, mode_1000x)) {
          data16 = SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;
       } else {
          data16 = 0 ;
       }
       SOC_IF_ERROR_RETURN
          (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r (unit, ws, data16, mask16)) ;
       
       if (!PHY_CLAUSE73_MODE(unit, port)) {
          /* disable CL73 AN device*/
          SOC_IF_ERROR_RETURN
             (WRITE_WC40_AN_IEEE0BLK_AN_IEEECONTROL1r(unit,ws,0));
       }
    
    } /* end of xenia mode */

    SOC_IF_ERROR_RETURN
        (_phy_wcmod_independent_lane_init(ws));

    if (SOC_IS_TRIUMPH3(unit)) {
        if (pCfg->max_speed != WCMOD_NO_CFG_VALUE) {
            pc->speed_max = pCfg->max_speed;
        }
        /*next to check if the max speed is valid or not */
        if ((!IS_DUAL_LANE_PORT(pc)) && (!IS_HG_PORT(unit, port))) {
            if (pc->speed_max == 11000) {
                _phy_wcmod_ind_speed_set(unit, port, 10000);
            } else {
                _phy_wcmod_ind_speed_set(unit, port, pc->speed_max);
            }
        } else {
            _phy_wcmod_ind_speed_set(unit, port, pc->speed_max);
        }
    }

    /* check to see if we need to recover the enable/disable flag for warm boot */
    if (SOC_WARM_BOOT(unit)) {
        /*read current HW status */
        uint16 data;
        ws->lane_num_ignore = 1;
        SOC_IF_ERROR_RETURN(READ_WC40_XGXSBLK1_LANECTRL3r(unit, ws, &data));
        ws->lane_num_ignore = 0;
        data  &= XGXSBLK1_LANECTRL3_PWRDN_RX_MASK;
        data >>= ws->this_lane;
        data &= 0x1; /*this_lane mask*/
        if (data) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
        } else {
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
        }
    }                
            

    pc->flags |= PHYCTRL_INIT_DONE;

    return SOC_E_NONE;
}
    
/*
 * Function:
 *      phy_wcmod_init
 * Purpose:     
 *      Initialize hc phys
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_wcmod_init(int unit, soc_port_t port)
{
    phy_ctrl_t         *pc;               
    soc_port_ability_t  ability;
    WCMOD_DEV_DESC_t *pDesc;
    WCMOD_DEV_INFO_t  *pInfo;
    
    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    pInfo = &pDesc->info;

    /* init the configuration value */
    _phy_wcmod_config_init(unit,port);

    pc = INT_PHY_SW_STATE(unit, port);

    if (pc->phy_mode == PHYCTRL_MULTI_CORE_PORT) {
        SOC_IF_ERROR_RETURN
            (_phy_wcmod_multi_core_init(unit, port));  
    } else {
        if (PHY_INDEPENDENT_LANE_MODE(unit, port)) {
            if  (((pInfo->serdes_id0) & SERDESID_SERDESID0_MODEL_NUMBER_MASK) == MODEL_QSGMII) {
                SOC_IF_ERROR_RETURN
                    (_phy_wcmod_qsgmii_core_init(unit, port));  
            } else if  (((pInfo->serdes_id0) & SERDESID_SERDESID0_MODEL_NUMBER_MASK) == MODEL_QUADSGMII) {
                        SOC_IF_ERROR_RETURN
                            (_phy_wcmod_quad_sgmii_core_init(unit, port));  
            } else if (DEV_CFG_PTR(pc)->custom) {
                SOC_IF_ERROR_RETURN
                    (_phy_wcmod_custom_mode_init(unit, port));
            } else {
                SOC_IF_ERROR_RETURN
                    (_phy_wcmod_ind_init(unit,port));
            }
        } else {
            /* Force to comboCore mode.
             * Support SGMII 10/100/1000Mbps, 1000X, 2500X, 10G, and 12G.
             */
            if  (((pInfo->serdes_id0) & SERDESID_SERDESID0_MODEL_NUMBER_MASK) == MODEL_QSGMII) {
                SOC_IF_ERROR_RETURN
                    (_phy_wcmod_qsgmii_core_init(unit, port));
            } else {          
                SOC_IF_ERROR_RETURN
                    (_phy_wcmod_combo_core_init(unit, port));  
            }
        } 
    }

    /* Only configure AN advertisement if there is no external PHY controlled by the SDK */
    if (!PHY_EXTERNAL_MODE(unit, port)) {
        /* setup the port's an cap */
        SOC_IF_ERROR_RETURN
            (phy_wcmod_ability_local_get(unit, port, &ability));

        SOC_IF_ERROR_RETURN
            (phy_wcmod_ability_advert_set(unit, port, &ability));
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(pc->unit,
                         "phy_wcmod_init: u=%d p=%d\n"), unit, port));

    return SOC_E_NONE;
}

#define PCS_STATUS_LANE4_SYNC   0x80F

/*
 *      _phy_wcmod_mld_link_war
 * Purpose:
 *      Keep link partner down if local device RX fiber is broken.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      pcs_link - pcs link status.
 * Returns:
 *      SOC_E_NONE
 * Description:
 *      SW WAR for JIRA SDK-56837.
 *      if local device pcs link down, look for rxSeqDone bit if one of the active
 *      lanes has a rxSeqDone set to 0, then set the AM space to non IEEE value that would
 *      bring LPs PCS link down.
 *      If all active lanes have rxSeqDone set to 1 or local device pcs link up,
 *      restore AM space to IEEE value.
 */
STATIC int
_phy_wcmod_mld_link_war(int unit, soc_port_t port, int pcs_link)
{
    phy_ctrl_t *pc;
    wcmod_st  *ws, *mld, *wcmod_temp;
    WCMOD_DEV_DESC_t *pDesc;
    int lane_start = 0, lane_end = 0, index = 0;
    uint16 data16 = 0;
    int lane_map, is_non_ieee_am = 0;
    int timer_count;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);
    mld   =  (wcmod_st *)( ws + 3);
    timer_count = 150000 / (DEV_CFG_PTR(pc)->linkscan_interval) + 1;

    if (pcs_link) {
        DEV_CFG_PTR(pc)->mld_war_timer_count = 0;
        SOC_IF_ERROR_RETURN (WC40_REG_READ(unit, mld, 0x0, 0x8420, &data16));

        if ((data16 & CL82_USERB0_TX_CONTROL_1_AM_TIMER_INIT_VAL_TX_HI_MASK) >>
                CL82_USERB0_TX_CONTROL_1_AM_TIMER_INIT_VAL_TX_HI_SHIFT) {
            /* Enable ieee AM spacing */
            data16 = 0;
            SOC_IF_ERROR_RETURN (WC40_REG_READ(unit, mld, 0x0, 0x8420, &data16));
            data16 = data16 & 0xFFC7; /* Enable IEEE Spacing */
            SOC_IF_ERROR_RETURN (WC40_REG_WRITE(unit, mld, 0x0, 0x8420, data16));
        }
    } else {
        DEV_CFG_PTR(pc)->mld_war_timer_count++;
        if (DEV_CFG_PTR(pc)->mld_war_timer_count > timer_count){
            lane_start = 0;
            lane_end   = 11;
            lane_map = DEV_CFG_PTR(pc)->active_lane_map;

            /* Read RX SEQUENCE DONE status */
            for (index = lane_start; index <= lane_end; index++) {
                 wcmod_temp = (wcmod_st *)(ws + index / 4);
                 wcmod_temp->this_lane = 0;

                 if (((lane_map >> index) & 0x1) == 0) continue;

                 SOC_IF_ERROR_RETURN
                      (WC40_REG_MODIFY(unit, wcmod_temp, 0x0, 0x80b1 + (0x10 * (index % 4)),
                          RXB_ANARXCONTROL_STATUS_SEL_sigdetStatus,RXB_ANARXCONTROL_STATUS_SEL_MASK));
                 SOC_IF_ERROR_RETURN
                      (WC40_REG_READ(unit, wcmod_temp,  0x0, 0x80b0 + (0x10 *  (index % 4)), &data16));

                 /* try to get all lanes' rx_seq_done status of this port */
                 is_non_ieee_am |= !(data16 & RX1_ANARXSTATUS_SIGDET_STATUS_RXSEQDONE_MASK);

                 /* some lane is not rx_seq_done */
                 if (is_non_ieee_am) {
                     data16 = 0;
                     /* Program AM spacing to non IEEE Value */
                     SOC_IF_ERROR_RETURN (WC40_REG_READ(unit, mld, 0x0, 0x8420, &data16));
                     data16 = data16 | 0x0008; /* Enable non IEEE Spacing */
                     SOC_IF_ERROR_RETURN (WC40_REG_WRITE(unit, mld, 0x0, 0x8420, data16));

                     break;
                 }
             }

            /* all lanes in this port have rx_seq_done, restore IEEE AM spacing */
            if (!is_non_ieee_am) {
                data16 = 0;
                SOC_IF_ERROR_RETURN (WC40_REG_READ(unit, mld, 0x0, 0x8420, &data16));
                data16 = data16 & 0xFFC7; /* Enable IEEE Spacing */
                SOC_IF_ERROR_RETURN (WC40_REG_WRITE(unit, mld, 0x0, 0x8420, data16));
            }
        }
    }
    return SOC_E_NONE;
}

/*
 *      phy_wcmod_link_get
 * Purpose:
 *      Get layer2 connection status.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      link - address of memory to store link up/down state.
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_wcmod_link_get(int unit, soc_port_t port, int *link)
{
    phy_ctrl_t *pc;               
    wcmod_st  *ws;
    WCMOD_DEV_DESC_t *pDesc;
    int rv, temp_lane;       
    int pcs_link = 0;
    int pmd_link = 0;
    uint8 sys_link = 0;
    SOFTWARE_RX_LOS_STATES_t rx_los_state;
    int lane_start = 0, lane_end = 0, index = 0;
    uint16 data16 = 0, rx_seq_done = 0;
    int timer_count;
    
    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    timer_count = 150000 / (DEV_CFG_PTR(pc)->linkscan_interval) + 1;

    /* restore mld local fault configuration one second after autoneg starts
     * It needs to be disabled during autoneg
     */
    if (!WCMOD_CL73_SOFT_KR2(pc)) {
       if (WCMOD_AN_VALID(pc)) {
          if (WCMOD_AN_CHECK_TIMEOUT(pc)) {
             SOC_IF_ERROR_RETURN
                (WC40_REG_MODIFY(unit, ws, 0x0, 0x842A,0,(1 << 5)));
             WCMOD_AN_VALID_RESET(pc);
          }
       }
    }

   if (SOC_IS_TRIUMPH3(unit)) {
       if(DEV_CFG_PTR(pc)->sw_rx_los.enable & WCMOD_RXLOS_LINK_FLAP_WAR) {
           if(DEV_CFG_PTR(pc)->macd==NULL) {
               SOC_IF_ERROR_RETURN
                   (soc_mac_probe(unit, port, &DEV_CFG_PTR(pc)->macd));
           }
       }
    }
    /* if 100G and plus check MLD pcs link up status */
    if (!SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) {
        if (SOC_INFO(unit).port_num_lanes[pc->port] >= 10) {
            wcmod_st  *mld, *wcmod_temp;
            int lane_map;
            mld   =  (wcmod_st *)( ws + 3);

            if((DEV_CFG_PTR(pc)->sw_rx_los.enable) 
                 && (DEV_CFG_PTR(pc)->sw_rx_los.link_status)) {
                SOC_IF_ERROR_RETURN
                    (WC40_REG_READ(unit, mld, 0x0, 0x18000001,&data16));
            }

            SOC_IF_ERROR_RETURN
                (WC40_REG_READ(unit, mld, 0x0, 0x18000001,&data16));
            if (data16 & 0x4) {
              *link = 1;
            } else {
              *link = 0;
            }

            if(!SOC_IS_ARAD(unit)) {
                (void)_phy_wcmod_mld_link_war(unit, port, *link);
            }

            /* added the rx_los for 100G port */
            if(DEV_CFG_PTR(pc)->sw_rx_los.enable) {
                pcs_link = *link; 
                sys_link = DEV_CFG_PTR(pc)->sw_rx_los.sys_link;
                rx_los_state = DEV_CFG_PTR(pc)->sw_rx_los.state;
                DEV_CFG_PTR(pc)->sw_rx_los.link_status = 0;
               
                lane_start = 0;
                lane_end   = 11;
                lane_map = DEV_CFG_PTR(pc)->active_lane_map;
                /* Read RX SEQUENCE DONE status */
                for (index = lane_start; index <= lane_end; index++) {
                    wcmod_temp = (wcmod_st *)(ws + index / 4);
                    wcmod_temp->this_lane = 0;
                    if (((lane_map >> index) & 0x1) == 0) continue;
                    SOC_IF_ERROR_RETURN
                        (WC40_REG_MODIFY(unit, wcmod_temp, 0x0,
                                         0x80b1 + (0x10 * (index % 4)), 
                                         RXB_ANARXCONTROL_STATUS_SEL_sigdetStatus,
                                         RXB_ANARXCONTROL_STATUS_SEL_MASK));
                    SOC_IF_ERROR_RETURN
                        (WC40_REG_READ(unit, wcmod_temp,  0x0,
                                         0x80b0 + (0x10 *  (index % 4)), &data16));

                    if(data16 & RX1_ANARXSTATUS_SIGDET_STATUS_RXSEQDONE_MASK) {
                        rx_seq_done = 1;
                    } else {
                        rx_seq_done = 0;
                        break;
                    }
                }
                pmd_link = rx_seq_done;

                /* 
                  Current State: Don't Care
                  Next State: IDLE
                  If link was previously TRUE and is still true, 
                  set the state to IDLE
                */
                if((sys_link == TRUE) && (pcs_link == TRUE)) {
                    rx_los_state = IDLE;
                }

                /* 
                  Current State: IDLE
                  Next State: RESET
                  If the state was IDLE(link up) but now link is down, 
                  enter the RESET state 
                */
                if((rx_los_state == IDLE) && (pcs_link == FALSE)) {
                    rx_los_state = RESET;
                    sys_link = FALSE;
                } 

                /* 
                  Current State: RESET
                  Next State: INITIAL_LINK or START_TIMER
                  If state is RESET, it's next state could be INITIAL LINK
                  or START_TIMER depending upon the pcs_link state
                */
                if(rx_los_state == RESET) {
                    DEV_CFG_PTR(pc)->sw_rx_los.timer_count++;
                    if (DEV_CFG_PTR(pc)->sw_rx_los.timer_count > timer_count)  {
                        if ((pcs_link == TRUE) && (rx_seq_done)){
                            rx_los_state = INITIAL_LINK;
                        } else if (rx_seq_done){
                            rx_los_state = START_TIMER;
                        }
                    }
                }
                /* Take action depending upon the current state */
                switch (rx_los_state) {
                    case IDLE:
                        /* Do nothing */
                        /* Link ON condition */
                        break;

                    case RESET:
                         if (SOC_IS_TRIUMPH3(unit)) {
                             if((DEV_CFG_PTR(pc)->sw_rx_los.enable & WCMOD_RXLOS_LINK_FLAP_WAR)&&
                                (DEV_CFG_PTR(pc)->sw_rx_los.rf_dis==1)) {
                                 SOC_IF_ERROR_RETURN
                                     (MAC_CONTROL_SET(DEV_CFG_PTR(pc)->macd, unit, port, 
                                                      SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE, TRUE));
                                 DEV_CFG_PTR(pc)->sw_rx_los.rf_dis = 0 ;
                            }
                        }
                        sys_link = FALSE;
                        break;

                    case INITIAL_LINK:
                         if (SOC_IS_TRIUMPH3(unit)) {
                             if(DEV_CFG_PTR(pc)->sw_rx_los.enable & WCMOD_RXLOS_LINK_FLAP_WAR) {
                                 SOC_IF_ERROR_RETURN
                                     (MAC_CONTROL_SET(DEV_CFG_PTR(pc)->macd, unit, port, 
                                                      SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE, FALSE));
                                DEV_CFG_PTR(pc)->sw_rx_los.rf_dis     = 1 ;
                             }
                         }
                        /* rxSeqStart */
                        for (index = lane_start; index <= lane_end; index++) {
                            wcmod_temp = (wcmod_st *)(ws + index / 4);
                            wcmod_temp->this_lane = index % 4;;
                            if (((lane_map >> index) & 0x1) == 0) continue;
                            SOC_IF_ERROR_RETURN
                                (MODIFY_WC40_DSC2B0_DSC_MISC_CTRL0r(unit, wcmod_temp, 
                                    DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_MASK, 
                                    DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_MASK));
                        }
                        DEV_CFG_PTR(pc)->sw_rx_los.timer_count = 0;
                        /* Skip the link transition for this rx reset */
                        DEV_CFG_PTR(pc)->sw_rx_los.link_status = 1;
                        rx_los_state = LINK;
                        LOG_VERBOSE(BSL_LS_SOC_PHY,
                                    (BSL_META_U(pc->unit,
                                                "SOFTWARE RX LOS : u=%d p=%d state:0x%x \n"), 
                                     unit, port, rx_los_state));
                        break;

                    case LINK:
                        DEV_CFG_PTR(pc)->sw_rx_los.timer_count++;
                        if (DEV_CFG_PTR(pc)->sw_rx_los.timer_count > timer_count)  {
                            if( pcs_link == TRUE) {
                                sys_link = TRUE;
                                rx_los_state = IDLE;
                                if (SOC_IS_TRIUMPH3(unit)) {

                                    /* enable the fault once the link status is stable */
                                    if((DEV_CFG_PTR(pc)->sw_rx_los.enable & WCMOD_RXLOS_LINK_FLAP_WAR)&&
                                    (DEV_CFG_PTR(pc)->sw_rx_los.rf_dis==1)) {
                                        SOC_IF_ERROR_RETURN
                                         (MAC_CONTROL_SET(DEV_CFG_PTR(pc)->macd, unit, port,
                                                          SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE, TRUE));
                                        DEV_CFG_PTR(pc)->sw_rx_los.rf_dis = 0 ;
                                    }
                                }
                            } else {
                                rx_los_state = RESET;
                            }
                        }
                        LOG_VERBOSE(BSL_LS_SOC_PHY,
                                    (BSL_META_U(pc->unit,
                                                "SOFTWARE RX LOS : u=%d p=%d state:0x%x \n"), 
                                     unit, port, rx_los_state));
                        break;

                    case START_TIMER:
                        rx_los_state = RX_RESTART;  
                        LOG_VERBOSE(BSL_LS_SOC_PHY,
                                    (BSL_META_U(pc->unit,
                                                "SOFTWARE RX LOS : u=%d p=%d state:0x%x \n"), 
                                     unit, port, rx_los_state));
                        break;
                    case RX_RESTART:
                         if (SOC_IS_TRIUMPH3(unit)) {
                             if(DEV_CFG_PTR(pc)->sw_rx_los.enable & WCMOD_RXLOS_LINK_FLAP_WAR) {
                                 SOC_IF_ERROR_RETURN
                                     (MAC_CONTROL_SET(DEV_CFG_PTR(pc)->macd, unit, port, 
                                                      SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE, FALSE));
                                DEV_CFG_PTR(pc)->sw_rx_los.rf_dis     = 1 ;
                             }
                             /* CLMAC(RS) can't originate remote fault when restart rx sequencer.
                                Modify AM spacing to none IEEE to keep LP down */
                             if ((pmd_link == TRUE) && (pcs_link == FALSE)) {
                                 SOC_IF_ERROR_RETURN (WC40_REG_READ(unit, mld, 0x0, 0x8420, &data16));
                                 data16 = data16 | 0x0008; /* Enable non IEEE Spacing */
                                 SOC_IF_ERROR_RETURN (WC40_REG_WRITE(unit, mld, 0x0, 0x8420, data16));
                             }
                        }
                        /* rxSeqStart */ 
                        for (index = lane_start; index <= lane_end; index++) {
                            wcmod_temp = (wcmod_st *)(ws + index / 4);
                            wcmod_temp->this_lane = index % 4;;
                            if (((lane_map >> index) & 0x1) == 0) continue;
                            SOC_IF_ERROR_RETURN
                                (MODIFY_WC40_DSC2B0_DSC_MISC_CTRL0r(unit, wcmod_temp, 
                                    DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_MASK, 
                                    DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_MASK));
                        }
                        if( pcs_link == FALSE) {
                            rx_los_state = RESET;
                            /* wait for RS originate remote fault and restore AM spacing to IEEE */
                            sal_udelay(10000);
                            SOC_IF_ERROR_RETURN (WC40_REG_READ(unit, mld, 0x0, 0x8420, &data16));
                            data16 = data16 & 0xFFC7; /* Enable IEEE Spacing */
                            SOC_IF_ERROR_RETURN (WC40_REG_WRITE(unit, mld, 0x0, 0x8420, data16));
                        } else {
                            rx_los_state = LINK;
                        }
                        LOG_VERBOSE(BSL_LS_SOC_PHY,
                                    (BSL_META_U(pc->unit,
                                                "SOFTWARE RX LOS : u=%d p=%d state:0x%x \n"), 
                                     unit, port, rx_los_state));
                        /* Skip the link transition for this rx reset */
                        DEV_CFG_PTR(pc)->sw_rx_los.link_status = 1;
                        DEV_CFG_PTR(pc)->sw_rx_los.timer_count = 0;
                        break;
                    default:
                        break;
                }
                DEV_CFG_PTR(pc)->sw_rx_los.sys_link = sys_link;;
                DEV_CFG_PTR(pc)->sw_rx_los.state = rx_los_state;
                *link = sys_link;
            }
            return (SOC_E_NONE);
        } else {
            ws->diag_type = WCMOD_DIAG_LINK;
            ws->per_lane_control = 0x1;  

            if (CUSTOM_MODE(pc)) {
                /* interlaken mode */
                /* configure the status register for sigdet */
                ws->per_lane_control = 0x10;    
            }
            /* IF SW RX LOS is enabled and it's link_status is set, it means the 
               lane is reset during the previous link get - read the link status 
               again to skip the OFF link transition that was forced during the
               SW RX LOS. */
            if((DEV_CFG_PTR(pc)->sw_rx_los.enable) 
                 && (DEV_CFG_PTR(pc)->sw_rx_los.link_status)) {
                    SOC_IF_ERROR_RETURN
                      (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));   
                    *link = ws->accData; 
            }
            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));   
            *link = ws->accData; 

            if (FORCE_CL72_ENABLED(pc)) {
                (void)_phy_wcmod_force_cl72_sw_link_recovery(unit,port,*link);
            }

            temp_lane = ws->this_lane;
            if(DEV_CFG_PTR(pc)->sw_rx_los.enable) {
                pcs_link = *link; 
                sys_link = DEV_CFG_PTR(pc)->sw_rx_los.sys_link;
                rx_los_state = DEV_CFG_PTR(pc)->sw_rx_los.state;
                DEV_CFG_PTR(pc)->sw_rx_los.link_status = 0;
               
                if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
                    lane_start = 0;
                    lane_end   = 3;
                } else if (IS_DUAL_LANE_PORT(pc)) {
                    lane_start = pc->lane_num;
                    lane_end = lane_start + 1;
                } else {  /* quad-port mode */
                    lane_start = pc->lane_num;
                    lane_end   = lane_start;
                }
                /* Read RX SEQUENCE DONE status */
                for (index = lane_start; index <= lane_end; index++) {
                    ws->this_lane = 0;
                    SOC_IF_ERROR_RETURN
                        (WC40_REG_MODIFY(unit, ws, 0x0,
                                         0x80b1 + (0x10 * index), 
                                         RXB_ANARXCONTROL_STATUS_SEL_sigdetStatus,
                                         RXB_ANARXCONTROL_STATUS_SEL_MASK));
                    SOC_IF_ERROR_RETURN
                        (WC40_REG_READ(unit, ws,  0x0,
                                         0x80b0 + (0x10 * index), &data16));

                    if(data16 & RX1_ANARXSTATUS_SIGDET_STATUS_RXSEQDONE_MASK) {
                        rx_seq_done = 1;
                    } else {
                        rx_seq_done = 0;
                        break;
                    }
                }

                /* 
                  Current State: Don't Care
                  Next State: IDLE
                  If link was previously TRUE and is still true, 
                  set the state to IDLE
                */
                if((sys_link == TRUE) && (pcs_link == TRUE)) {
                    rx_los_state = IDLE;
                }

                /* 
                  Current State: IDLE
                  Next State: RESET
                  If the state was IDLE(link up) but now link is down, 
                  enter the RESET state 
                */
                if((rx_los_state == IDLE) && (pcs_link == FALSE)) {
                    rx_los_state = RESET;
                    sys_link = FALSE;
                } 

                /* 
                  Current State: RESET
                  Next State: INITIAL_LINK or START_TIMER
                  If state is RESET, it's next state could be INITIAL LINK
                  or START_TIMER depending upon the pcs_link state
                */
                if(rx_los_state == RESET) {
                    if ((pcs_link == TRUE) && (rx_seq_done)){
                        rx_los_state = INITIAL_LINK;
                    } else if (rx_seq_done){
                        rx_los_state = START_TIMER;
                    }
                }

                /* Take action depending upon the current state */
                switch (rx_los_state) {
                    case IDLE:
                        /* Do nothing */
                        /* Link ON condition */
                        break;

                    case RESET:
                        sys_link = FALSE;
                        break;

                    case INITIAL_LINK:
                        /* rxSeqStart */
                        for (index = lane_start; index <= lane_end; index++) {
                            ws->this_lane = index;
                            SOC_IF_ERROR_RETURN
                                (MODIFY_WC40_DSC2B0_DSC_MISC_CTRL0r(unit, ws, 
                                    DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_MASK, 
                                    DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_MASK));
                        }
                        /* Skip the link transition for this rx reset */
                        DEV_CFG_PTR(pc)->sw_rx_los.link_status = 1;
                        rx_los_state = LINK;
                        LOG_VERBOSE(BSL_LS_SOC_PHY,
                                    (BSL_META_U(pc->unit,
                                                "SOFTWARE RX LOS : u=%d p=%d state:0x%x \n"), 
                                     unit, port, rx_los_state));
                        break;

                    case LINK:
                        if( pcs_link == TRUE) {
                            sys_link = TRUE;
                            rx_los_state = IDLE;
                        } else {
                            rx_los_state = RESET;
                        }
                        LOG_VERBOSE(BSL_LS_SOC_PHY,
                                    (BSL_META_U(pc->unit,
                                                "SOFTWARE RX LOS : u=%d p=%d state:0x%x \n"), 
                                     unit, port, rx_los_state));
                        break;

                    case START_TIMER:
                        rx_los_state = RX_RESTART;  
                        LOG_VERBOSE(BSL_LS_SOC_PHY,
                                    (BSL_META_U(pc->unit,
                                                "SOFTWARE RX LOS : u=%d p=%d state:0x%x \n"), 
                                     unit, port, rx_los_state));
                        break;

                    case RX_RESTART:
                        /* rxSeqStart */ 
                        for (index = lane_start; index <= lane_end; index++) {
                            ws->this_lane = index;
                            SOC_IF_ERROR_RETURN
                                (MODIFY_WC40_DSC2B0_DSC_MISC_CTRL0r(unit, ws, 
                                    DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_MASK, 
                                    DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_MASK));
                        }
                        if( pcs_link == FALSE) {
                            rx_los_state = RESET;
                        } else {
                            rx_los_state = LINK;
                        }
                        LOG_VERBOSE(BSL_LS_SOC_PHY,
                                    (BSL_META_U(pc->unit,
                                                "SOFTWARE RX LOS : u=%d p=%d state:0x%x \n"), 
                                     unit, port, rx_los_state));
                        /* Skip the link transition for this rx reset */
                        DEV_CFG_PTR(pc)->sw_rx_los.link_status = 1;
                        break;

                    default:
                        break;
                }
                DEV_CFG_PTR(pc)->sw_rx_los.sys_link = sys_link;;
                DEV_CFG_PTR(pc)->sw_rx_los.state = rx_los_state;
                *link = sys_link;
            }
            ws->this_lane = temp_lane;
            return (SOC_E_NONE);
        }
    }
    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_wcmod_ind_enable_set
 * Purpose:
 *      Enable/Disable phy
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      enable - on/off state to set
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
_phy_wcmod_ind_enable_set(int unit, soc_port_t port, int enable)
{
    int rv;
    int data = 0;
    phy_ctrl_t *pc;               
    wcmod_st  *ws;
    WCMOD_DEV_DESC_t *pDesc;
    
    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    if (enable) {
        if(ws->dual_type) {
            switch (pc->lane_num) {
            case 0: data  = CHANGE_TX0_POWERED_STATE | CHANGE_RX0_POWERED_STATE;
                    data |= CHANGE_TX1_POWERED_STATE | CHANGE_RX1_POWERED_STATE;
                break;
            case 2: data  = CHANGE_TX2_POWERED_STATE | CHANGE_RX2_POWERED_STATE;
                    data |= CHANGE_TX3_POWERED_STATE | CHANGE_RX3_POWERED_STATE;
                break;
            default:
                LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(pc->unit,
                                     "_phy_wcmod_ind_enable_set dual port error. lane_num=%0d\n"), pc->lane_num));
                break;
            }
        } else {
            switch (pc->lane_num % 4) {
            case 0: data = CHANGE_TX0_POWERED_STATE | CHANGE_RX0_POWERED_STATE;
                break;
            case 1: data = CHANGE_TX1_POWERED_STATE | CHANGE_RX1_POWERED_STATE;
                break;
            case 2: data = CHANGE_TX2_POWERED_STATE | CHANGE_RX2_POWERED_STATE;
                break;
            case 3: data = CHANGE_TX3_POWERED_STATE | CHANGE_RX3_POWERED_STATE;
                break;
            default:
                break;
            }
        }
    } else {
        if(ws->dual_type) {
            switch(pc->lane_num) {
            case 0: data  = CHANGE_TX0_POWERED_STATE | CHANGE_RX0_POWERED_STATE | POWER_DOWN_TX0 | POWER_DOWN_RX0;
                    data |= CHANGE_TX1_POWERED_STATE | CHANGE_RX1_POWERED_STATE | POWER_DOWN_TX1 | POWER_DOWN_RX1;
                break;
            case 2: data  = CHANGE_TX2_POWERED_STATE | CHANGE_RX2_POWERED_STATE | POWER_DOWN_TX2 | POWER_DOWN_RX2;
                    data |= CHANGE_TX3_POWERED_STATE | CHANGE_RX3_POWERED_STATE | POWER_DOWN_TX3 | POWER_DOWN_RX3;
                break;
            default:
                break;
            }   
        } else {
            switch (pc->lane_num % 4) {
            case 0: data = CHANGE_TX0_POWERED_STATE | CHANGE_RX0_POWERED_STATE | POWER_DOWN_TX0 | POWER_DOWN_RX0;
                break;
            case 1: data = CHANGE_TX1_POWERED_STATE | CHANGE_RX1_POWERED_STATE | POWER_DOWN_TX1 | POWER_DOWN_RX1;
                break;
            case 2: data = CHANGE_TX2_POWERED_STATE | CHANGE_RX2_POWERED_STATE | POWER_DOWN_TX2 | POWER_DOWN_RX2;
                break;
            case 3: data = CHANGE_TX3_POWERED_STATE | CHANGE_RX3_POWERED_STATE | POWER_DOWN_TX3 | POWER_DOWN_RX3;
                break;
            default:
                break;
            }
        }
    }
    ws->per_lane_control = data;
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("POWER_CONTROL", ws, &rv));
 
    return SOC_E_NONE;
}

STATIC int
_phy_wcmod_enable_set(int unit, soc_port_t port, int enable)
{
    int rv;
    int data = 0;
    phy_ctrl_t *pc;               
    wcmod_st  *ws;
    WCMOD_DEV_DESC_t *pDesc;
    
    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    /* check to see if specail ILKN port */
    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
        int i, num_core;
        wcmod_st *temp_ws;
        num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
        for (i = 0 ; i < num_core; i++) {
            temp_ws   =  (wcmod_st *)( ws + i);
            if (temp_ws->this_lane == 0) {
                if (enable) {
                    data = CHANGE_TX0_POWERED_STATE | CHANGE_RX0_POWERED_STATE;
                    data |= CHANGE_TX1_POWERED_STATE | CHANGE_RX1_POWERED_STATE;
                    data |= CHANGE_TX2_POWERED_STATE | CHANGE_RX2_POWERED_STATE;
                    data |= CHANGE_TX3_POWERED_STATE | CHANGE_RX3_POWERED_STATE;
                } else {
                    data = CHANGE_TX0_POWERED_STATE | CHANGE_RX0_POWERED_STATE | POWER_DOWN_TX0 | POWER_DOWN_RX0;
                    data |= CHANGE_TX1_POWERED_STATE | CHANGE_RX1_POWERED_STATE | POWER_DOWN_TX1 | POWER_DOWN_RX1;
                    data |= CHANGE_TX2_POWERED_STATE | CHANGE_RX2_POWERED_STATE | POWER_DOWN_TX2 | POWER_DOWN_RX2;
                    data |= CHANGE_TX3_POWERED_STATE | CHANGE_RX3_POWERED_STATE | POWER_DOWN_TX3 | POWER_DOWN_RX3;
                }
            } else {
                if (enable) {
                    data = CHANGE_TX2_POWERED_STATE | CHANGE_RX2_POWERED_STATE;
                    data |= CHANGE_TX3_POWERED_STATE | CHANGE_RX3_POWERED_STATE;
                } else {
                    data = CHANGE_TX2_POWERED_STATE | CHANGE_RX2_POWERED_STATE | POWER_DOWN_TX2 | POWER_DOWN_RX2;
                    data |= CHANGE_TX3_POWERED_STATE | CHANGE_RX3_POWERED_STATE | POWER_DOWN_TX3 | POWER_DOWN_RX3;
                }
            }
            temp_ws->per_lane_control = data;
            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("POWER_CONTROL", temp_ws, &rv));
        }
    } else if ((SOC_INFO(unit).port_num_lanes[pc->port] == 10)) {
        int /* temp_id, */ i, num_core = 0;
        wcmod_st *temp_ws;
        /*SOC_IF_ERROR_RETURN
            (_wcmod_phy_parameter_copy(pc, ws)); */
  
        if (enable) {
            data = CHANGE_TX0_POWERED_STATE | CHANGE_RX0_POWERED_STATE;
            data |= CHANGE_TX1_POWERED_STATE | CHANGE_RX1_POWERED_STATE;
            data |= CHANGE_TX2_POWERED_STATE | CHANGE_RX2_POWERED_STATE;
            data |= CHANGE_TX3_POWERED_STATE | CHANGE_RX3_POWERED_STATE;
        } else {
            data = CHANGE_TX0_POWERED_STATE | CHANGE_RX0_POWERED_STATE | POWER_DOWN_TX0 | POWER_DOWN_RX0;
            data |= CHANGE_TX1_POWERED_STATE | CHANGE_RX1_POWERED_STATE | POWER_DOWN_TX1 | POWER_DOWN_RX1;
            data |= CHANGE_TX2_POWERED_STATE | CHANGE_RX2_POWERED_STATE | POWER_DOWN_TX2 | POWER_DOWN_RX2;
            data |= CHANGE_TX3_POWERED_STATE | CHANGE_RX3_POWERED_STATE | POWER_DOWN_TX3 | POWER_DOWN_RX3;
        }
        num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        for (i = 0 ; i < num_core; i++) {
            temp_ws   =  (wcmod_st *)( ws + i);
            /*temp_id = temp_ws->phy_ad;
            SOC_IF_ERROR_RETURN(_wcmod_phy_parameter_copy(pc, temp_ws));
            temp_ws->phy_ad = temp_id; */
            if (i == 2) {
                if (enable) {
                    data = CHANGE_TX0_POWERED_STATE | CHANGE_RX0_POWERED_STATE;
                    data |= CHANGE_TX1_POWERED_STATE | CHANGE_RX1_POWERED_STATE;
                } else {
                    data = CHANGE_TX0_POWERED_STATE | CHANGE_RX0_POWERED_STATE | POWER_DOWN_TX0 | POWER_DOWN_RX0;
                    data |= CHANGE_TX1_POWERED_STATE | CHANGE_RX1_POWERED_STATE | POWER_DOWN_TX1 | POWER_DOWN_RX1;
                }
            }
            temp_ws->per_lane_control = data;
            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("POWER_CONTROL", temp_ws, &rv));
        }
    } else {
        /* this is for 4 lane combo port case */
        /*SOC_IF_ERROR_RETURN
            (_wcmod_phy_parameter_copy(pc, ws)); */

        if (enable) {
            int i = 0;
            data = CHANGE_TX0_POWERED_STATE | CHANGE_RX0_POWERED_STATE;
            data |= CHANGE_TX1_POWERED_STATE | CHANGE_RX1_POWERED_STATE;
            data |= CHANGE_TX2_POWERED_STATE | CHANGE_RX2_POWERED_STATE;
            data |= CHANGE_TX3_POWERED_STATE | CHANGE_RX3_POWERED_STATE;
            /* enable all the PMD tx */
            SOC_IF_ERROR_RETURN
               (WRITE_WC40_XGXSBLK0_MISCCONTROL1r(unit, ws, 0x0));
            if (ws->model_type == WCMOD_WC_C0) {
                /* restart all 4 lanes */
                for(i = 0; i <= 3; ++i) {
                    ws->this_lane = i;
                    SOC_IF_ERROR_RETURN
                        (WRITE_WC40_DSC1B0_UC_CTRLr(unit, ws, 0x301));
                }
                ws->this_lane = 0; 
            }
        } else {
            data = CHANGE_TX0_POWERED_STATE | CHANGE_RX0_POWERED_STATE | POWER_DOWN_TX0 | POWER_DOWN_RX0;
            data |= CHANGE_TX1_POWERED_STATE | CHANGE_RX1_POWERED_STATE | POWER_DOWN_TX1 | POWER_DOWN_RX1;
            data |= CHANGE_TX2_POWERED_STATE | CHANGE_RX2_POWERED_STATE | POWER_DOWN_TX2 | POWER_DOWN_RX2;
            data |= CHANGE_TX3_POWERED_STATE | CHANGE_RX3_POWERED_STATE | POWER_DOWN_TX3 | POWER_DOWN_RX3;
            /* disable the all the PMD tx */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_XGXSBLK0_MISCCONTROL1r(unit, ws, 
                             XGXSBLK0_MISCCONTROL1_GLOBAL_PMD_TX_DISABLE_MASK,
                             XGXSBLK0_MISCCONTROL1_GLOBAL_PMD_TX_DISABLE_MASK));
        }
        ws->per_lane_control = data;
        SOC_IF_ERROR_RETURN
          (wcmod_tier1_selector("POWER_CONTROL", ws, &rv));
        
    }
    return SOC_E_NONE;

}

/*
 * Function:
 *      phy_wcmod_enable_set
 * Purpose:
 *      Enable/Disable phy 
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      enable - on/off state to set
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_wcmod_enable_set(int unit, soc_port_t port, int enable)
{
    int rv = 0;
    phy_ctrl_t  *pc;
    wcmod_st  *ws;
    WCMOD_DEV_DESC_t *pDesc;
    
    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    if (SOC_WARM_BOOT(unit)) {
        /*read current HW status */
        uint16 data;
        ws->lane_num_ignore = 1;
        SOC_IF_ERROR_RETURN(READ_WC40_XGXSBLK1_LANECTRL3r(unit, ws, &data));
        ws->lane_num_ignore = 0;
        data  &= XGXSBLK1_LANECTRL3_PWRDN_RX_MASK;
        data >>= ws->this_lane;
        data &= 0x1; /*this_lane mask*/
        if (data) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
        } else {
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
        }    
    } else {
        if (enable) {
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
        } else {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
        }

        if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
            rv = _phy_wcmod_enable_set(unit,port,enable);
        } else {
            rv = _phy_wcmod_ind_enable_set(unit, port, enable);
        }
    }
    return rv;

}

/*
 * Function:
 *      phy_wcmod_enable_get
 * Purpose:
 *      Get Enable/Disable status
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      enable - address of where to store on/off state
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_wcmod_enable_get(int unit, soc_port_t port, int *enable)
{
    *enable = !PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE); 
    
    return SOC_E_NONE;
}

STATIC int
_phy_wcmod_ind_speed_intf_get(int unit, soc_port_t port, int speed, wcmod_spd_intf_params_t* spd_params)
{
    phy_ctrl_t  *pc;
    WCMOD_DEV_DESC_t *pDesc;
    wcmod_st    *ws;
    int hg10g_port = FALSE;
    uint16  sgmii_status;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);


    if (DEV_CFG_PTR(pc)->hg_mode) { 
        hg10g_port = TRUE;
    }

    spd_params->spd_intf = WCMOD_SPD_ZERO;
    spd_params->turn_off_pll = 0;
    spd_params->tx_inx = TXDRV_DFT_INX;
    spd_params->vco_freq = -1;
    spd_params->pll_divider = -1;
    spd_params->oversample_mode = -1;

    switch (speed) {
    case 10:
        spd_params->spd_intf = WCMOD_SPD_10_SGMII;
        spd_params->tx_inx = TXDRV_6GOS2_INX;
        spd_params->turn_off_pll = 0;
        break;
    case 100:
        /*Warpcore C0 does not support 100FX */
        if (ws->model_type == WCMOD_WC_C0) {
            spd_params->spd_intf = WCMOD_SPD_100_SGMII;
        } else { 
            SOC_IF_ERROR_RETURN
                (READ_WC40_SERDESDIGITAL_STATUS1000X1r(unit, ws, &sgmii_status));
            sgmii_status &= SERDESDIGITAL_STATUS1000X1_SGMII_MODE_MASK;
            if (sgmii_status) {
               spd_params->spd_intf = WCMOD_SPD_100_SGMII;
            } else {
               spd_params->spd_intf = WCMOD_SPD_100_FX;
            }
        }
        spd_params->tx_inx = TXDRV_6GOS2_INX;
        spd_params->turn_off_pll = 0;
        break;
    case SOC_PHY_NON_CANNED_SPEED:
        spd_params->spd_intf = WCMOD_SPD_CUSTOM_BYPASS;
        spd_params->turn_off_pll = 1;
        break;
    case 1000:
        SOC_IF_ERROR_RETURN
            (READ_WC40_SERDESDIGITAL_STATUS1000X1r(unit, ws, &sgmii_status));
        sgmii_status &= SERDESDIGITAL_STATUS1000X1_SGMII_MODE_MASK;
        if ((!sgmii_status) && !(DEV_CFG_PTR(pc)->fiber_pref)) {
            sgmii_status = 1;
        }         
        if (sgmii_status) {
            spd_params->spd_intf = WCMOD_SPD_1000_SGMII;
        } else {
            spd_params->spd_intf = WCMOD_SPD_1000_FIBER;
        }
        spd_params->tx_inx = TXDRV_6GOS2_INX;
        spd_params->turn_off_pll = 0;
        break;
    case 2500:
        spd_params->spd_intf = WCMOD_SPD_2500;
        spd_params->tx_inx = TXDRV_6GOS2_INX;
        spd_params->turn_off_pll = 0;
        break;
    case 3125:
        spd_params->tx_inx = TXDRV_6250_INX;  
        spd_params->spd_intf = WCMOD_SPD_PCSBYP_3P125G;
        spd_params->turn_off_pll = 1;
        break;
    case 5750:
        if(PHY_IS_SUPPORT_DUAL_RATE(unit, pc->port)){
            spd_params->vco_freq = 11500;
            spd_params->pll_divider = 92;
            spd_params->oversample_mode = 1; 
            spd_params->spd_intf = WCMOD_SPD_CUSTOM_BYPASS;
        } else {
            spd_params->spd_intf = WCMOD_SPD_PCSBYP_5P75G;
        }
        spd_params->tx_inx = TXDRV_11500_INX;
        spd_params->turn_off_pll = 1;
        break;
    case 6250:
        if(125 == DEV_CFG_PTR(pc)->refclk) {
            spd_params->spd_intf = WCMOD_SPD_CUSTOM_BYPASS;
            if(PHY_IS_SUPPORT_DUAL_RATE(unit, pc->port)) { /*for dual rate OS2 is required*/
                spd_params->vco_freq = 12500;
                spd_params->pll_divider = 100;
                spd_params->oversample_mode = 1;  
            } else {
                spd_params->vco_freq = 6250;
                spd_params->pll_divider = 50;
                spd_params->oversample_mode = 0;   
            }
        } else {
            if(PHY_IS_SUPPORT_DUAL_RATE(unit, pc->port)){
                spd_params->vco_freq = 12500;
                spd_params->spd_intf = WCMOD_SPD_CUSTOM_BYPASS;
                spd_params->pll_divider = 80;
                spd_params->oversample_mode = 1;  
            } else {   
                spd_params->spd_intf = WCMOD_SPD_PCSBYP_6P25G;
            }
        }
        spd_params->tx_inx = TXDRV_6250_INX;
        spd_params->turn_off_pll = 1;
        break;
    case 8125:
        spd_params->vco_freq = 8125;
        spd_params->pll_divider = 52;
        spd_params->oversample_mode = 0;
        spd_params->spd_intf = WCMOD_SPD_CUSTOM_BYPASS;
        spd_params->tx_inx = TXDRV_103125_INX;
        spd_params->turn_off_pll = 1;
        break;
    case 8500:
        spd_params->vco_freq = 8500;
        spd_params->pll_divider = 68;
        spd_params->oversample_mode = 0;
        spd_params->spd_intf = WCMOD_SPD_CUSTOM_BYPASS;
        spd_params->tx_inx = TXDRV_103125_INX;
        spd_params->turn_off_pll = 1;
        break;
    case 10000:
        if (IS_DUAL_LANE_PORT(pc)) {
            if (DEV_CFG_PTR(pc)->rxaui_mode)  {
                spd_params->spd_intf = WCMOD_SPD_X2_10000;  /* speed_x2_10000 */
                spd_params->tx_inx = TXDRV_6GOS2_INX;
            } else if (hg10g_port == TRUE) {
                /* speed_val = 0x21;   10.5HiG dual-XGXS SCR only */
                /* speed_val = 0x2D 10GHig DXGXS SCR */
                if (DEV_CFG_PTR(pc)->scrambler_en) {
                    spd_params->spd_intf =WCMOD_SPD_10000_HI_DXGXS_SCR ;
                } else {
                    spd_params->spd_intf =WCMOD_SPD_10000_HI_DXGXS ;
                }
                spd_params->tx_inx = TXDRV_6GOS1_INX;
            } else {
                /* speed_val = 0x20;  10G ethernet dual-XGXS */
                /* speed_val = 0x2E 10G ethernet DXGXS SCR */
                if (DEV_CFG_PTR(pc)->scrambler_en) {
                    spd_params->spd_intf = WCMOD_SPD_10000_DXGXS_SCR;
                } else {
                    spd_params->spd_intf = WCMOD_SPD_10000_DXGXS;        
                }
                spd_params->tx_inx = TXDRV_6GOS1_INX; 
            }
        } else {
           if ((DEV_CFG_PTR(pc)->line_intf & WCMOD_IF_SFI) ||
               (DEV_CFG_PTR(pc)->line_intf & WCMOD_IF_SR) ||
               (DEV_CFG_PTR(pc)->line_intf & WCMOD_IF_CR)) {
                spd_params->spd_intf = WCMOD_SPD_10000_SFI;
                spd_params->tx_inx = TXDRV_SFI_INX;
           } else if (DEV_CFG_PTR(pc)->line_intf & WCMOD_IF_XFI) {
               spd_params->spd_intf = WCMOD_SPD_10000_XFI;
               spd_params->tx_inx = TXDRV_XFI_INX; 
           } else {
               /* XXX default to XFI */
               spd_params->spd_intf = WCMOD_SPD_10000_XFI;
               spd_params->tx_inx = TXDRV_XFI_INX; 
           }
            /*config tx index properly */
            if (spd_params->spd_intf == WCMOD_SPD_10000_SFI) {
                if (DEV_CFG_PTR(pc)->line_intf & WCMOD_IF_SR) {
                     spd_params->tx_inx = TXDRV_SFI_INX;
                } else if (DEV_CFG_PTR(pc)->line_intf & WCMOD_IF_CR) {
                     spd_params->tx_inx = TXDRV_SFIDAC_INX;
                } else {
                     spd_params->tx_inx = TXDRV_SFI_INX;
                }
            } 
        }
        spd_params->turn_off_pll = 1;
        break;
    case 10312:
        spd_params->tx_inx = TXDRV_103125_INX; 
        spd_params->spd_intf = WCMOD_SPD_PCSBYP_10P3125;
        spd_params->turn_off_pll = 1;
        break;
    case 10500: 
        spd_params->tx_inx = TXDRV_6GOS1_INX; 
        if (IS_DUAL_LANE_PORT(pc)) {
            if (hg10g_port == TRUE) {
                spd_params->spd_intf =  WCMOD_SPD_10500_HI_DXGXS ;
            } else {
                spd_params->spd_intf =  WCMOD_SPD_10500_DXGXS ;
            }
        } 
        spd_params->turn_off_pll = 1;
        break ;
    case 10937:
        spd_params->tx_inx = TXDRV_109375_INX; 
        spd_params->spd_intf = WCMOD_SPD_PCSBYP_10P9375;
        spd_params->turn_off_pll = 1;
        break;
    case 11000:
        spd_params->tx_inx = TXDRV_XFI_INX; 
        spd_params->spd_intf = WCMOD_SPD_10600_XFI_HG;
        spd_params->turn_off_pll = 1;
        break;
    case 11250:
        spd_params->vco_freq = 11250;
        spd_params->pll_divider = 72;
        spd_params->oversample_mode = 0;
        spd_params->spd_intf = WCMOD_SPD_CUSTOM_BYPASS;
        spd_params->tx_inx = TXDRV_103125_INX;
        spd_params->turn_off_pll = 1;
        break;
    case 11500:
        if (ws->model_type == WCMOD_WL_A0) {
            spd_params->tx_inx = TXDRV_11500_INX; 
            spd_params->spd_intf = WCMOD_SPD_PCSBYP_11P5;
        } else {
            if (DEV_CFG_PTR(pc)->pcs_bypass) {
                spd_params->tx_inx = TXDRV_11500_INX; 
                spd_params->spd_intf = WCMOD_SPD_PCSBYP_11P5;
            } else {
                spd_params->tx_inx = TXDRV_XFI_INX; 
                spd_params->spd_intf = WCMOD_SPD_11P5;
            }
        }         
        spd_params->turn_off_pll = 1;
        break;
    case 12000:
        spd_params->tx_inx = TXDRV_6GOS1_INX; 
        if( IS_DUAL_LANE_PORT(pc)) {
            /* to check if (DEV_CFG_PTR(pc)->custom1) */
            spd_params->spd_intf = WCMOD_SPD_R2_12000 ; 
        } else {
            spd_params->tx_inx = TXDRV_XFI_INX; 
            spd_params->spd_intf = WCMOD_SPD_11P5;
        }
        spd_params->turn_off_pll = 1;
        break ;
    case 12375:
        if ((ws->model_type == WCMOD_WL_A0) || (DEV_CFG_PTR(pc)->pcs_bypass)) {
            if(125 == DEV_CFG_PTR(pc)->refclk) {
                spd_params->vco_freq = 12375;
                spd_params->pll_divider = 100;
                spd_params->oversample_mode = 0;  
                spd_params->spd_intf = WCMOD_SPD_CUSTOM_BYPASS;
            } else {        
                spd_params->spd_intf = WCMOD_SPD_PCSBYP_12P5;
            }
            spd_params->tx_inx = TXDRV_12500_INX; 
        }         
        spd_params->turn_off_pll = 1;
        break;
    case 12500:
        if ((ws->model_type == WCMOD_WL_A0) || (DEV_CFG_PTR(pc)->pcs_bypass)) {
            if(125 == DEV_CFG_PTR(pc)->refclk) {
                spd_params->vco_freq = 12500;
                spd_params->pll_divider = 100;
                spd_params->oversample_mode = 0;  
                spd_params->spd_intf = WCMOD_SPD_CUSTOM_BYPASS;
            } else {        
                spd_params->spd_intf = WCMOD_SPD_PCSBYP_12P5;
            }
            spd_params->tx_inx = TXDRV_12500_INX; 
        } else {
            spd_params->tx_inx = TXDRV_XFI_INX; 
            spd_params->spd_intf = WCMOD_SPD_12P5;
        }         
        spd_params->turn_off_pll = 1;
        break;
    case 12773:   /*  12733Mbps */
        spd_params->tx_inx = TXDRV_6GOS1_INX; 
        if( IS_DUAL_LANE_PORT(pc)) {
            if (hg10g_port == TRUE) {
                spd_params->spd_intf = WCMOD_SPD_12773_HI_DXGXS ;
            } else {
                spd_params->spd_intf = WCMOD_SPD_12773_DXGXS ;
            }
        } 
        spd_params->turn_off_pll = 1;
        break ;
    case 15750:
    case 16000:  /* setting RX66_CONTROL_CC_EN/CC_DATA_SEL failed this 16000 HI_DXGXS mode */
        spd_params->tx_inx = TXDRV_6GOS1_INX; 
        if(IS_DUAL_LANE_PORT(pc)) {
            spd_params->spd_intf =  WCMOD_SPD_15750_HI_DXGXS ;
        } 
        spd_params->turn_off_pll = 1;
        break ;
        /*  WCMOD_SPD_21G_HI_DXGXS ?? */
    case 20000: 
        spd_params->tx_inx = TXDRV_SFI_INX ;
        if(IS_DUAL_LANE_PORT(pc)) {
            if( (hg10g_port == TRUE) ) {  /* higest priority */
                spd_params->spd_intf = WCMOD_SPD_20000_HI_DXGXS ;
            } else {
                if( (DEV_CFG_PTR(pc)->line_intf & WCMOD_IF_KR4) ||   
                    (DEV_CFG_PTR(pc)->line_intf & WCMOD_IF_KR) ) {
                    spd_params->spd_intf = WCMOD_SPD_20G_KR2 ;
                } else if( (DEV_CFG_PTR(pc)->line_intf & WCMOD_IF_CR4) || 
                           (DEV_CFG_PTR(pc)->line_intf & WCMOD_IF_CR) ) {
                    spd_params->spd_intf = WCMOD_SPD_20G_CR2 ;
                } else if (DEV_CFG_PTR(pc)->scrambler_en) {
                    spd_params->spd_intf = WCMOD_SPD_20G_DXGXS_SCR;
                } else {
                    spd_params->spd_intf = WCMOD_SPD_20G_DXGXS ;
                }
            }
        } 
        spd_params->turn_off_pll = 1;
        break ;
    case 21000:
        spd_params->tx_inx = TXDRV_SFI_INX ;
        if(IS_DUAL_LANE_PORT(pc)) {
            spd_params->spd_intf = WCMOD_SPD_21G_HI_DXGXS ;
        }
        spd_params->turn_off_pll = 1;
        break ;
    case 23000:
       if (IS_DUAL_LANE_PORT(pc)) {
          spd_params->spd_intf = WCMOD_SPD_X2_23000;
          spd_params->tx_inx = TXDRV_SFI_INX; 
       }
       spd_params->turn_off_pll = 1;
       break ; 
    default:
        return SOC_E_PARAM;
    }
    return SOC_E_NONE;
}

STATIC int
_phy_wcmod_speed_intf_get(int unit, soc_port_t port, int speed, wcmod_spd_intfc_set* spd_intf, int* tx_inx)
{
    phy_ctrl_t  *pc;
    int hg10g_port = FALSE;

    pc = INT_PHY_SW_STATE(unit, port);

    if ((DEV_CFG_PTR(pc)->hg_mode) && (!DEV_CFG_PTR(pc)->cx4_10g)) { 
        hg10g_port = TRUE;
    }

    switch (speed) {
    case 0:
        /* Do not change speed */
        return SOC_E_NONE;
    case 2500:
        *spd_intf = WCMOD_SPD_2500;
        break;
    case 10000:
        *spd_intf = WCMOD_SPD_10000; /* 10G xaui */
        *tx_inx = TXDRV_6GOS2_CX4_INX;
        if (hg10g_port == TRUE) {
            *spd_intf = WCMOD_SPD_10000_HI; /* 10G HiG */
            *tx_inx = TXDRV_6GOS2_INX;
        }
        break;
    case 12000:
        *spd_intf = DEV_CFG_PTR(pc)->rxaui? WCMOD_SPD_12773_HI_DXGXS: /* 12.7HiG dual-XGXS*/
                     WCMOD_SPD_12000_HI;   /* 12 HiG */
        *tx_inx = TXDRV_6GOS2_INX;
        break;
    case 12500:
        *spd_intf = WCMOD_SPD_12500;
        *tx_inx = TXDRV_6GOS2_INX;
        break;
    case 13000:
        *spd_intf = WCMOD_SPD_13000;
        *tx_inx = TXDRV_6GOS2_INX;
        break;
    case 15000:
        *spd_intf = WCMOD_SPD_15000;
        *tx_inx = TXDRV_6GOS2_INX;
        break;
    case 16000:
        *spd_intf = WCMOD_SPD_16000;
        *tx_inx = TXDRV_6GOS2_INX;
        break;
    case 20000:
        *spd_intf = WCMOD_SPD_20000;
        *tx_inx = TXDRV_6GOS1_INX;
        /* speed_val = 0x2C 20G_SCR */
        /*
        speed_val = DEV_CFG_PTR(pc)->scrambler_en? 
                    WCMOD_SPD_20000_SCR:
                    SERDESDIGITAL_MISC1_FORCE_SPEED_dr_20GHiGig_X4;
                    */
        break;
    case 21000:
        *spd_intf = WCMOD_SPD_21000;
        *tx_inx = TXDRV_6GOS2_INX;
        break;
    case 25000:
        *spd_intf = WCMOD_SPD_25455;
        *tx_inx = TXDRV_6GOS2_INX;
        break;
    case 30000:
        *spd_intf = WCMOD_SPD_31500;
        *tx_inx = TXDRV_XLAUI_INX;
        break;
    case 40000:
        if (DEV_CFG_PTR(pc)->hg_mode) {
            *spd_intf = WCMOD_SPD_40G;   /* brcm 40G */
            *tx_inx = TXDRV_XLAUI_INX;
        } else {
            if ((DEV_CFG_PTR(pc)->line_intf & WCMOD_IF_XLAUI) ||
                (DEV_CFG_PTR(pc)->line_intf & WCMOD_IF_KR4) ||
                (DEV_CFG_PTR(pc)->line_intf & WCMOD_IF_SR) || 
                (DEV_CFG_PTR(pc)->line_intf & WCMOD_IF_KR)) { 
                *spd_intf = WCMOD_SPD_40G_KR4;  /* 40GKR */
                *tx_inx = TXDRV_XLAUI_INX;
            } else if ((DEV_CFG_PTR(pc)->line_intf & WCMOD_IF_CR4) ||
                       (DEV_CFG_PTR(pc)->line_intf & WCMOD_IF_CR) ) {
                *spd_intf = WCMOD_SPD_40G_CR4;  /* 40GCR4 */
                *tx_inx = TXDRV_XLAUI_INX;
            }
            /* for now use 40G KR4 */
            *spd_intf = WCMOD_SPD_40G_KR4;
            *tx_inx = TXDRV_XLAUI_INX;
        }
        break;
    case 42000:
        *spd_intf = WCMOD_SPD_42G;
        *tx_inx = TXDRV_XLAUI_INX;
        break;
    case 44000:
        *spd_intf = WCMOD_SPD_44G;
        *tx_inx = TXDRV_XLAUI_INX;
        break;
    case 48000:
        *spd_intf = WCMOD_SPD_48G;
        *tx_inx = TXDRV_XLAUI_INX;
        break;
    default:
        return SOC_E_PARAM;
    }
    return SOC_E_NONE;

}

STATIC int
_phy_wcmod_ind_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t  *pc;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t *pDesc;
    wcmod_spd_intfc_set spd_intf;
    int rv;
    int tx_inx=0;
    int previous_speed;
    int turn_off_pll=0;
    int tmp_lane_select ;
    int rxseqCYA = 0;
    uint16  data16 = 0, mask16;
    WCMOD_TX_DRIVE_t tx_drv[NUM_LANES];
    wcmod_spd_intf_params_t spd_params;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    if (WCMOD_LINK_WAR_REVS(pc)) {
        WCMOD_SP_VALID_RESET(pc);
    }

    /* first read the previous speed */
    SOC_IF_ERROR_RETURN
      (phy_wcmod_speed_get(unit, port, &previous_speed));

    spd_intf = WCMOD_SPD_ILLEGAL ;
    
    /* xxxx dual port mode write on one lane for now */

    if(0 == speed) {
        /* Do not change speed */
        return SOC_E_NONE;
    }
    rv  = _phy_wcmod_ind_speed_intf_get(unit, port, speed, &spd_params);
    SOC_IF_ERROR_RETURN(rv);

    spd_intf = spd_params.spd_intf;
    tx_inx = spd_params.tx_inx;
    turn_off_pll = spd_params.turn_off_pll;

    if(SOC_PHY_NON_CANNED_SPEED != speed && WCMOD_SPD_CUSTOM_BYPASS == spd_intf) {
        ws->oversample_mode = spd_params.oversample_mode;
        ws->pll_divider = spd_params.pll_divider;
        ws->vco_freq = spd_params.vco_freq;
    }

    if(10000 == speed && !IS_DUAL_LANE_PORT(pc)) {
            if(1) {   /* XXXX: need to revisist */
               SOC_IF_ERROR_RETURN
                   (MODIFY_WC40_RX66_CONTROLr(ws->unit, ws, 
                                RX66_CONTROL_CC_EN_MASK | RX66_CONTROL_CC_DATA_SEL_MASK,
                                RX66_CONTROL_CC_EN_MASK | RX66_CONTROL_CC_DATA_SEL_MASK)); /* ckcmp enabled */
            }
            /*config firmare mode properly */
            if(!SOC_IS_ARAD(unit) || ws->firmware_mode == SOC_PHY_FIRMWARE_DEFAULT) {
                mask16 = 0xf << (pc->lane_num * 4);
                if (spd_intf == WCMOD_SPD_10000_SFI) {
                    if (DEV_CFG_PTR(pc)->line_intf & WCMOD_IF_SR) {
                        data16 = (WCMOD_UC_CTRL_SR4 & 0xf) << (pc->lane_num * 4);
                    } else if (DEV_CFG_PTR(pc)->line_intf & WCMOD_IF_CR) {
                        data16 = WCMOD_UC_CTRL_SFP_DAC << (pc->lane_num * 4);
                    } else {
                        data16 = (WCMOD_UC_CTRL_SR4 & 0xf) << (pc->lane_num * 4);
                    }
                } else if (spd_intf == WCMOD_SPD_10000_XFI) {
                        data16 = (WCMOD_UC_CTRL_XLAUI & 0xf) << (pc->lane_num * 4);
                }
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_UC_INFO_B1_FIRMWARE_MODEr(unit,ws,data16,mask16));
            }

            /*first check the current PLL setting */
            SOC_IF_ERROR_RETURN
                (READ_WC40_PLL_ANAPLLSTATUSr(unit, ws, &data16));
            if ((data16 & PLL_ANAPLLSTATUS_PLL_MODE_AFE_MASK) == PLL_ANAPLLSTATUS_PLL_MODE_AFE_div66) {
                turn_off_pll = 0;
            }
    }

    /*for 12.5G default is to use force OS DFE firmware mode */
    if (spd_intf == WCMOD_SPD_12P5) {
        mask16 = 0xf << (pc->lane_num * 4);
        if (ws->firmware_mode == SOC_PHY_FIRMWARE_DEFAULT) {
            data16 = (WCMOD_UC_CTRL_FORCE_OSDFE & 0xf) << (pc->lane_num * 4);
        } else {
            data16 = (ws->firmware_mode) << (pc->lane_num * 4);
        }
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_UC_INFO_B1_FIRMWARE_MODEr(unit,ws,data16,mask16));
    }
            
    /* to check if we need to switch to the correct PLL for 1G and 2.5G */
    if (((speed == 1000) || (speed == 2500)) && (ws->model_type == WCMOD_WC_C0)) {
        /*first check the current PLL setting */
        SOC_IF_ERROR_RETURN
            (READ_WC40_PLL_ANAPLLSTATUSr(unit, ws, &data16));
        if ((data16 & PLL_ANAPLLSTATUS_PLL_MODE_AFE_MASK) != PLL_ANAPLLSTATUS_PLL_MODE_AFE_div66) {
            turn_off_pll = 1;
        }
    }

    if( spd_intf == WCMOD_SPD_ILLEGAL ) return SOC_E_PARAM ;
    
    /* hold tx/rx in reset */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DIGITAL5_MISC6r(unit,ws,
              DIGITAL5_MISC6_RESET_RX_ASIC_MASK | DIGITAL5_MISC6_RESET_TX_ASIC_MASK, 
              DIGITAL5_MISC6_RESET_RX_ASIC_MASK | DIGITAL5_MISC6_RESET_TX_ASIC_MASK));

    /* needs to enable the link signaling for  dual lane HG port*/
    tmp_lane_select  = ws->lane_select;
    if (IS_DUAL_LANE_PORT(pc)) {    
        ws->lane_select = WCMOD_LANE_BCST;
        MODIFY_WC40_XGXSBLK1_TESTRXr(ws->unit, ws,
             (DEV_CFG_PTR(pc)->hg_mode) ? 0 : XGXSBLK1_TESTRX_RX_INBANDMDIO_QFLTR_EN_MASK,
             XGXSBLK1_TESTRX_RX_INBANDMDIO_QFLTR_EN_MASK);
        /* need to set rxseqCYA bit for dula lane 8/10 bit speed mode*/
        /* set the 0x8162 bit 14 for 8/10bit speed mode*/
        if ((speed == 10000) || (speed == 10500)) {
            rxseqCYA = 1;
        }
        MODIFY_WC40_XGXSBLK8_CL73CONTROL9r(ws->unit, ws,
            rxseqCYA ? XGXSBLK8_CL73CONTROL9_RXSEQDONE_CYA_MASK : 0,  
             XGXSBLK8_CL73CONTROL9_RXSEQDONE_CYA_MASK);
    }

    ws->lane_select = tmp_lane_select; 
    /* configure the TX driver parameters per speed mode */        
    SOC_IF_ERROR_RETURN
        (_phy_wcmod_tx_control_get(unit, pc, &tx_drv[0],tx_inx));
    SOC_IF_ERROR_RETURN
        (_phy_wcmod_tx_control_set(unit, pc,&tx_drv[0]));

   /* check to see if PLL needs to turned down */
    tmp_lane_select  = ws->lane_select ;
    /* use dual bcst for dual lanes.
    if( IS_DUAL_LANE_PORT(pc)) {    
        if(pc->lane_num ) {
            ws->lane_select = WCMOD_LANE_1_1_0_0 ;
        } else {
            ws->lane_select = WCMOD_LANE_0_0_1_1 ;
        }
     } */
   /* check to see if PLL needs to turned down */
    if (turn_off_pll) { 
        /* turn off RX sequencer    */
        ws->per_lane_control = WCMOD_DISABLE;
        SOC_IF_ERROR_RETURN
            (wcmod_tier1_selector("RX_SEQ_CONTROL", ws, &rv));

        /* disable PLL      */
        ws->per_lane_control = WCMOD_DISABLE;
        SOC_IF_ERROR_RETURN
          (wcmod_tier1_selector("PLL_SEQUENCER_CONTROL", ws, &rv));
    }

    ws->spd_intf = spd_intf;

    /* control the speeds thru pma/pmd register */
   if (SOC_IS_ARAD(unit)) {
        if ((ws->model_type == WCMOD_WC_C0) && (!IS_DUAL_LANE_PORT(pc))) {
            ws->accAddr = PMA_PMD_SPEED_ENFORCE; 
        }
    } else  {
        if ((ws->model_type == WCMOD_WC_C0) && (!IS_DUAL_LANE_PORT(pc))) {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_SERDESDIGITAL_MISC2r(unit, ws,
                               SERDESDIGITAL_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN_MASK,
                               SERDESDIGITAL_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN_MASK));
            ws->accAddr = PMA_PMD_SPEED_ENFORCE; 
        }
    }

    SOC_IF_ERROR_RETURN
       (wcmod_tier1_selector("SET_SPD_INTF", ws, &rv));

    /* control the speeds thru pma/pmd register */
    if ((ws->model_type == WCMOD_WC_C0) && (!IS_DUAL_LANE_PORT(pc))) { 
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_SERDESDIGITAL_MISC2r(unit, ws,0,
                           SERDESDIGITAL_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN_MASK));
        ws->accAddr = 0; 
    }

    if (turn_off_pll) { 
        /* enable PLL sequencer   */
        ws->per_lane_control = WCMOD_ENABLE;
        SOC_IF_ERROR_RETURN
           (wcmod_tier1_selector("PLL_SEQUENCER_CONTROL", ws, &rv));

        /* turn on RX sequencer    */
        ws->per_lane_control = WCMOD_ENABLE;
        SOC_IF_ERROR_RETURN
           (wcmod_tier1_selector("RX_SEQ_CONTROL", ws, &rv));
           
        _phy_wcmod_control_vco_disturbed_set(ws->unit, ws->port);
    }


    /* release rx/tx reset */
    /*XXXTTT_ORIG_CODE  not cond in orig code Don't release rx/tx until MAC is "ready" for XE and GE*/
    {
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DIGITAL5_MISC6r(unit,ws, 0,
                                         DIGITAL5_MISC6_RESET_RX_ASIC_MASK |
                                         DIGITAL5_MISC6_RESET_TX_ASIC_MASK));
    }

    ws->lane_select = tmp_lane_select ; 
   
    return rv;
}


STATIC int
_phy_wcmod_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t  *pc;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t *pDesc;
    WCMOD_DEV_CFG_t *pCfg;
    wcmod_spd_intfc_set spd_intf = 0;
    uint16  sgmii_status;
    int rv;
    int tx_inx=0; 
    int tmp_lane_select;
    /* int previous_speed; */
    int turn_off_pll=1;
    int hg10g_port = FALSE;
    int per_lane_clock = 0;
    int rxseqdone_cya = 0;
    WCMOD_TX_DRIVE_t tx_drv[MAX_NUM_LANES];

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);
    pCfg = &pDesc->cfg; 

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    /* first check if interlaken speed */
    if ((SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) && (SOC_IS_ARAD(unit))) {
        SOC_IF_ERROR_RETURN
            (phy_wcmod_interlaken_config(unit, port, speed));     
        return SOC_E_NONE;  
    }
    /* in case it was enabled in previous speed mode */
    /* force_cl72 = FALSE; */

    /* set default entry first */
    tx_inx = TXDRV_DFT_INX;

    pc = INT_PHY_SW_STATE(unit, port);
    if ((DEV_CFG_PTR(pc)->hg_mode) && (!DEV_CFG_PTR(pc)->cx4_10g)) { 
        hg10g_port = TRUE;
    }
 
    /* don't reset to default if clock comp is enabled by user */
    if (!(DEV_CTRL_PTR(pc)->clk_comp)) {
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_XGXSBLK2_UNICOREMODE10Gr(unit, ws, 
                  COMBO_HG_XGXS_nLQnCC,
                  XGXSBLK2_UNICOREMODE10G_UNICOREMODE10GHIG_MASK));
    }

    tmp_lane_select = ws->lane_select;        /* save this */
    per_lane_clock = 0;
    switch (speed) {
    case 0:
        /* Do not change speed */
        return SOC_E_NONE;
    case 10:
        SOC_IF_ERROR_RETURN
            (READ_WC40_SERDESDIGITAL_STATUS1000X1r(unit, ws, &sgmii_status));
        sgmii_status &= SERDESDIGITAL_STATUS1000X1_SGMII_MODE_MASK;
        if (sgmii_status) {
            spd_intf = WCMOD_SPD_10_SGMII;
            tx_inx = TXDRV_6GOS2_INX;
            break;
        } else {
            return SOC_E_PARAM;
        }
    case 100:
        SOC_IF_ERROR_RETURN
            (READ_WC40_SERDESDIGITAL_STATUS1000X1r(unit, ws, &sgmii_status));
        sgmii_status &= SERDESDIGITAL_STATUS1000X1_SGMII_MODE_MASK;
        if (sgmii_status) {
            spd_intf = WCMOD_SPD_100_SGMII;
            tx_inx = TXDRV_6GOS2_INX;
            break;
        } else {
            return SOC_E_PARAM;
        }
    case 1000:
        SOC_IF_ERROR_RETURN
            (READ_WC40_SERDESDIGITAL_STATUS1000X1r(unit, ws, &sgmii_status));
        sgmii_status &= SERDESDIGITAL_STATUS1000X1_SGMII_MODE_MASK;
        if (sgmii_status || !(pCfg->fiber_pref)) {
            spd_intf = WCMOD_SPD_1000_SGMII;
        } else {
            spd_intf = WCMOD_SPD_1000_FIBER;
        }
        tx_inx = TXDRV_6GOS2_INX;
        turn_off_pll = 1;
        rxseqdone_cya = 1;
        break;
    case 2500:
    case 3000:
        spd_intf = WCMOD_SPD_2500;
        tx_inx = TXDRV_6GOS2_INX;
        turn_off_pll = 1;
        rxseqdone_cya = 1;
        break;
    case 10000:
        spd_intf = WCMOD_SPD_10000; /* 10G xaui */
        tx_inx = TXDRV_6GOS2_CX4_INX;

        tmp_lane_select     = ws->lane_select;        /* save this */
        ws->lane_select = WCMOD_LANE_BCST; /* go to broadcast mode */
        /*we need to turn off  bit 11,10 to 1 of reg0x82ec for 10G XAUI mode*/
        MODIFY_WC40_CL72_USERB0_CL72_MISC4_CONTROLr(pc->unit, ws, 0x000, 0xc00); 
        ws->lane_select = tmp_lane_select; /* restore */

        if ((hg10g_port == TRUE) && DEV_CFG_PTR(pc)->rxaui) {
            /* speed_val = 0x1f;   10HiG dual-XGXS */
            /* speed_val = 0x2D 10GHig DXGXS SCR */
            spd_intf = DEV_CFG_PTR(pc)->scrambler_en? WCMOD_SPD_10000_HI_DXGXS_SCR: WCMOD_SPD_10000_HI_DXGXS;
            tx_inx = TXDRV_6GOS1_INX;
        } else if (DEV_CFG_PTR(pc)->rxaui) {
            /* speed_val = 0x20;  10G ethernet dual-XGXS */
            /* speed_val = 0x2E 10G ethernet DXGXS SCR */
            spd_intf = DEV_CFG_PTR(pc)->scrambler_en? WCMOD_SPD_10000_DXGXS_SCR: WCMOD_SPD_10000_DXGXS;
            tx_inx = TXDRV_6GOS1_INX;
        } else if (hg10g_port == TRUE) {
            spd_intf = WCMOD_SPD_10000_HI; /* 10G HiG */
            tx_inx = TXDRV_6GOS2_INX;
        }
        turn_off_pll = 1;
        rxseqdone_cya = 1;
        break;
    case 12000:
        spd_intf = DEV_CFG_PTR(pc)->rxaui? WCMOD_SPD_12773_HI_DXGXS: /* 12.7HiG dual-XGXS*/
                     WCMOD_SPD_12000_HI;   /* 12 HiG */
        tx_inx = TXDRV_6GOS2_INX;
        turn_off_pll = 1;
        rxseqdone_cya = 1;
        break;
    case 12500:
        spd_intf = WCMOD_SPD_12500;
        tx_inx = TXDRV_6GOS2_INX;
        turn_off_pll = 1;
        rxseqdone_cya = 1;
        break;
    case 13000:
        spd_intf = WCMOD_SPD_13000;
        tx_inx = TXDRV_6GOS2_INX;
        turn_off_pll = 1;
        rxseqdone_cya = 1;
        break;
    case 15000:
        spd_intf = WCMOD_SPD_15000;
        tx_inx = TXDRV_6GOS2_INX;
        turn_off_pll = 1;
        rxseqdone_cya = 1;
        break;
    case 16000:
        spd_intf = WCMOD_SPD_16000;
        tx_inx = TXDRV_6GOS2_INX;
        turn_off_pll = 1;
        rxseqdone_cya = 1;
        break;
    case 20000:
        spd_intf = DEV_CFG_PTR(pc)->scrambler_en? 
                        WCMOD_SPD_20000_SCR:
                        WCMOD_SPD_20000;
        if (!(DEV_CFG_PTR(pc)->hg_mode)) {
            /* don't change  if clock comp is enabled by user */
            if (!(DEV_CTRL_PTR(pc)->clk_comp)) {
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_XGXSBLK2_UNICOREMODE10Gr(unit, ws,
                      COMBO_HG_XGXS_nCC, 
                      XGXSBLK2_UNICOREMODE10G_UNICOREMODE10GHIG_MASK));
            }
        }             
        tx_inx = TXDRV_6GOS1_INX;
        turn_off_pll = 1;
        rxseqdone_cya = 1;
        break;
    case 21000:
        spd_intf = WCMOD_SPD_21000;
        turn_off_pll = 1;
        rxseqdone_cya = 1;
        break;
    case 25000:
        spd_intf = WCMOD_SPD_25455;
        per_lane_clock = 1;
        turn_off_pll = 1;
        break;
    case 30000:
        spd_intf = WCMOD_SPD_31500;
        turn_off_pll = 1;
        per_lane_clock = 1;
        tx_inx = TXDRV_XLAUI_INX;
        break;
    case 40000:
        if (DEV_CFG_PTR(pc)->hg_mode) {
            tx_inx = TXDRV_XLAUI_INX;
            if((DEV_CFG_PTR(pc)->line_intf & WCMOD_IF_KR4)){
                spd_intf = WCMOD_SPD_40G_KR4;
            }else{
                spd_intf = WCMOD_SPD_40G;   /* brcm 40G */
            }
        } else {
            if ((DEV_CFG_PTR(pc)->line_intf & WCMOD_IF_XLAUI) ||
                (DEV_CFG_PTR(pc)->line_intf & WCMOD_IF_KR4) ||
                (DEV_CFG_PTR(pc)->line_intf & WCMOD_IF_SR) || 
                (DEV_CFG_PTR(pc)->line_intf & WCMOD_IF_KR)) { 
                spd_intf = WCMOD_SPD_40G_KR4;  /* 40GKR */
                tx_inx = TXDRV_XLAUI_INX;
            } else if ((DEV_CFG_PTR(pc)->line_intf & WCMOD_IF_CR4) ||
                       (DEV_CFG_PTR(pc)->line_intf & WCMOD_IF_CR) ) {
                spd_intf = WCMOD_SPD_40G_CR4;  /* 40GCR4 */
                tx_inx = TXDRV_XLAUI_INX;
            } else {
                /* for now use 40G KR4 */
                spd_intf = WCMOD_SPD_40G_KR4;
                tx_inx = TXDRV_XLAUI_INX;
            }
        }
        if (DEV_CFG_PTR(pc)->line_intf & WCMOD_IF_XLAUI) {
            SOC_IF_ERROR_RETURN
                (WRITE_WC40_UC_INFO_B1_FIRMWARE_MODEr(unit, ws, WCMOD_UC_CTRL_XLAUI));
            tx_inx = TXDRV_XLAUI_INX;
        } else if (DEV_CFG_PTR(pc)->line_intf & WCMOD_IF_SR) {
            SOC_IF_ERROR_RETURN
                (WRITE_WC40_UC_INFO_B1_FIRMWARE_MODEr(unit, ws, WCMOD_UC_CTRL_SR4));
            tx_inx = TXDRV_SR4_INX;
        }
        turn_off_pll = 1;
        per_lane_clock = 1;
        break;
    case 42000:
       {
            uint32 cl72_en = 0;
            _phy_wcmod_cl72_enable_get(unit, pc, &cl72_en);            
            /* if cl72 enabled is set don't reset the speed */
            if (cl72_en) {
                return SOC_E_NONE;
            } else {    
                spd_intf = WCMOD_SPD_42G;
                tx_inx = TXDRV_XLAUI_INX;
                if (DEV_CFG_PTR(pc)->line_intf & WCMOD_IF_XLAUI) {
                    SOC_IF_ERROR_RETURN
                        (WRITE_WC40_UC_INFO_B1_FIRMWARE_MODEr(unit, ws, 
                                                              WCMOD_UC_CTRL_XLAUI));
                } else if (DEV_CFG_PTR(pc)->line_intf & WCMOD_IF_SR) {
                    SOC_IF_ERROR_RETURN
                        (WRITE_WC40_UC_INFO_B1_FIRMWARE_MODEr(unit, ws, 
                                                               WCMOD_UC_CTRL_SR4));
                }
                if (DEV_CFG_PTR(pc)->line_intf & WCMOD_IF_KR4) {
                    spd_intf = WCMOD_SPD_42G_KR4;
                }
                turn_off_pll = 1;
                per_lane_clock = 1;
                break;
            }
        }
    case 44000:
        per_lane_clock = 1;
        spd_intf = WCMOD_SPD_44G;
        tx_inx = TXDRV_XLAUI_INX;
        if (DEV_CFG_PTR(pc)->line_intf & WCMOD_IF_XLAUI) {
            SOC_IF_ERROR_RETURN
                (WRITE_WC40_UC_INFO_B1_FIRMWARE_MODEr(unit, ws, 
                                                      0x4444));
        } else if (DEV_CFG_PTR(pc)->line_intf & WCMOD_IF_SR) {
            SOC_IF_ERROR_RETURN
                (WRITE_WC40_UC_INFO_B1_FIRMWARE_MODEr(unit, ws, 
                                                       0x4444));
        }
        /* also set the vco rate to be 11.5G */
        /* Program Core PLL speed by setting 0x81FB[11:7] == 0x1c  */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_UC_INFO_B1_REGBr(unit,ws, 
             0x18 << UC_INFO_B1_MISC_CTRL_SERDES_PLL_RATE_SHIFT,
             UC_INFO_B1_MISC_CTRL_SERDES_PLL_RATE_MASK));
        turn_off_pll = 1;
        break;

    case 48000:
        per_lane_clock = 1;
        spd_intf = WCMOD_SPD_48G;
        tx_inx = TXDRV_XLAUI_INX;
        if (DEV_CFG_PTR(pc)->line_intf & WCMOD_IF_XLAUI) {
            SOC_IF_ERROR_RETURN
                (WRITE_WC40_UC_INFO_B1_FIRMWARE_MODEr(unit, ws, 
                                                      0x4444));
        } else if (DEV_CFG_PTR(pc)->line_intf & WCMOD_IF_SR) {
            SOC_IF_ERROR_RETURN
                (WRITE_WC40_UC_INFO_B1_FIRMWARE_MODEr(unit, ws, 
                                                       0x4444));
        }
        /* also set the vco rate to be 12.5G */
        /* Program Core PLL speed by setting 0x81FB[11:7] == 0x1c  */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_UC_INFO_B1_REGBr(unit,ws, 
             0x1c << UC_INFO_B1_MISC_CTRL_SERDES_PLL_RATE_SHIFT,
             UC_INFO_B1_MISC_CTRL_SERDES_PLL_RATE_MASK));
        turn_off_pll = 1;
        break;
    case 100000:
    case 120000:
    case 106000:
    case 127000:
        /* call the 100g plus config setting */
        ws->s100g_plus = 1;
        rv = phy_wcmod_100g_plus_config(unit, port, speed);
        return rv;
    default:
        return SOC_E_PARAM;
    }

    ws->lane_select = tmp_lane_select;
    /* hold tx/rx in reset */
    tmp_lane_select     = ws->lane_select;        /* save this */
    ws->lane_select = WCMOD_LANE_BCST; /* go to broadcast mode */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DIGITAL5_MISC6r(unit,ws,
              DIGITAL5_MISC6_RESET_RX_ASIC_MASK | DIGITAL5_MISC6_RESET_TX_ASIC_MASK, 
              DIGITAL5_MISC6_RESET_RX_ASIC_MASK | DIGITAL5_MISC6_RESET_TX_ASIC_MASK));

    /* Some aggregate lane modes require per-lane clocking.  Enable it if necessary */
    MODIFY_WC40_CL72_USERB0_CL72_MISC4_CONTROLr(ws->unit, ws, per_lane_clock ? 0xc00 : 0, 0xc00);
    /* set the 0x8162 bit 14 for 8/10bit speed mode*/
    MODIFY_WC40_XGXSBLK8_CL73CONTROL9r(ws->unit, ws,
        rxseqdone_cya ? XGXSBLK8_CL73CONTROL9_RXSEQDONE_CYA_MASK : 0,  
         XGXSBLK8_CL73CONTROL9_RXSEQDONE_CYA_MASK);

    /* need enabling link signal for HG port */
    MODIFY_WC40_XGXSBLK1_TESTRXr(ws->unit, ws,
         (DEV_CFG_PTR(pc)->hg_mode) ? 0 : XGXSBLK1_TESTRX_RX_INBANDMDIO_QFLTR_EN_MASK,
         XGXSBLK1_TESTRX_RX_INBANDMDIO_QFLTR_EN_MASK);

    ws->lane_select = tmp_lane_select;
    
    /* configure the TX driver parameters per speed mode */        
    SOC_IF_ERROR_RETURN
        (_phy_wcmod_tx_control_get(unit, pc, &tx_drv[0],tx_inx));
    SOC_IF_ERROR_RETURN
        (_phy_wcmod_tx_control_set(unit, pc, &tx_drv[0]));

   /* check to see if PLL needs to turned down */
    if (turn_off_pll) { 
        /* disable PLL      */
        ws->per_lane_control = WCMOD_DISABLE;
        SOC_IF_ERROR_RETURN
          (wcmod_tier1_selector("PLL_SEQUENCER_CONTROL", ws, &rv));

        /* turn off RX sequencer    */
        ws->per_lane_control = WCMOD_DISABLE;
        SOC_IF_ERROR_RETURN
            (wcmod_tier1_selector("RX_SEQ_CONTROL", ws, &rv));

        /* disable parallel detect on all lanes */
        ws->per_lane_control = 0;
        SOC_IF_ERROR_RETURN
           (wcmod_tier1_selector("PARALLEL_DETECT_CONTROL", ws, &rv));


        /* disable 100FX on all lanes  */
        ws->per_lane_control = 0;
        SOC_IF_ERROR_RETURN
           (wcmod_tier1_selector("100FX_CONTROL", ws, &rv));
   }

    ws->spd_intf = spd_intf;

    SOC_IF_ERROR_RETURN
       (wcmod_tier1_selector("SET_SPD_INTF", ws, &rv));

    if (turn_off_pll) { 
        /* enable PLL sequencer   */
        ws->per_lane_control = WCMOD_ENABLE;
        SOC_IF_ERROR_RETURN
           (wcmod_tier1_selector("PLL_SEQUENCER_CONTROL", ws, &rv));

        /* turn on RX sequencer    */
        ws->per_lane_control = WCMOD_ENABLE;
        SOC_IF_ERROR_RETURN
           (wcmod_tier1_selector("RX_SEQ_CONTROL", ws, &rv));
           
        _phy_wcmod_control_vco_disturbed_set(ws->unit, ws->port);
    }

    /* release tx/rx in reset */
    tmp_lane_select     = ws->lane_select;        /* save this */
    ws->lane_select = WCMOD_LANE_BCST; /* go to broadcast mode */

    /*XXXTTT_ORIG_CODE  not cond in orig code Don't release rx/tx until MAC is "ready" for XE and GE*/
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DIGITAL5_MISC6r(unit,ws, 0,
                                     DIGITAL5_MISC6_RESET_RX_ASIC_MASK |
                                     DIGITAL5_MISC6_RESET_TX_ASIC_MASK));
  
    ws->lane_select = tmp_lane_select;

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_wcmod_speed_set
 * Purpose:
 *      Set PHY speed
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      speed - link speed in Mbps
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_wcmod_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t  *pc;
    int rv;

    pc = INT_PHY_SW_STATE(unit, port);

    if (WCMOD_LINK_WAR_REVS(pc)) {
        WCMOD_SP_VALID_RESET(pc);
    }

    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        
        rv = _phy_wcmod_speed_set(unit,port,speed); 
        if (WCMOD_LINK_WAR_REVS(pc)) {
            if (speed >= 25000) {  /* BRCM 64B/66B mode */
                WCMOD_SP_VALID_SET(pc);
            }
        }
    } else {
            rv = _phy_wcmod_ind_speed_set(unit,port,speed);
            if (WCMOD_LINK_WAR_REVS(pc)) {
                if (speed >= 12000) {  /* BRCM 64B/66B mode */
                    WCMOD_SP_VALID_SET(pc);
                }
            }
    }

    return rv;
}


STATIC int
_phy_wcmod_tx_fifo_reset(int unit, soc_port_t port,uint32 speed)
{
    phy_ctrl_t  *pc;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int num_core;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
      (_wcmod_phy_parameter_copy(pc, ws));

    if ((SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) || (SOC_INFO(unit).port_num_lanes[pc->port] >= 10)) {
        int i;
        wcmod_st *temp_ws;
        num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        for (i = 0 ; i < num_core; i++) {
            temp_ws   =  (wcmod_st *)( ws + i);
            /*temp_id = temp_ws->phy_ad;
            SOC_IF_ERROR_RETURN(_wcmod_phy_parameter_copy(pc, temp_ws));
            temp_ws->phy_ad = temp_id; */
            
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_TX0_ANATXACONTROL0r(unit, temp_ws,
                                                 TX0_ANATXACONTROL0_TX1G_FIFO_RST_MASK,
                                                 TX0_ANATXACONTROL0_TX1G_FIFO_RST_MASK)); 
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_TX0_ANATXACONTROL0r(unit, temp_ws, 0,
                                                 TX0_ANATXACONTROL0_TX1G_FIFO_RST_MASK)); 
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_TX1_ANATXACONTROL0r(unit, temp_ws,
                                                 TX0_ANATXACONTROL0_TX1G_FIFO_RST_MASK,
                                                 TX0_ANATXACONTROL0_TX1G_FIFO_RST_MASK)); 
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_TX1_ANATXACONTROL0r(unit, temp_ws, 0,
                                                 TX0_ANATXACONTROL0_TX1G_FIFO_RST_MASK)); 
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_TX2_ANATXACONTROL0r(unit, temp_ws,
                                                 TX0_ANATXACONTROL0_TX1G_FIFO_RST_MASK,
                                                 TX0_ANATXACONTROL0_TX1G_FIFO_RST_MASK)); 
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_TX2_ANATXACONTROL0r(unit, temp_ws, 0,
                                                 TX0_ANATXACONTROL0_TX1G_FIFO_RST_MASK)); 
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_TX3_ANATXACONTROL0r(unit, temp_ws,
                                                 TX0_ANATXACONTROL0_TX1G_FIFO_RST_MASK,
                                                 TX0_ANATXACONTROL0_TX1G_FIFO_RST_MASK)); 
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_TX3_ANATXACONTROL0r(unit, temp_ws, 0,
                                                 TX0_ANATXACONTROL0_TX1G_FIFO_RST_MASK)); 
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_wcmod_notify_mac_loopback
 * Purpose:
 *      turn on rx clock compensation in mac loopback mode for
 *      applicable XGXS devices
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      enable - TRUE mac loopback mode, FALSE not in mac loopback mode
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_phy_wcmod_notify_mac_loopback(int unit, soc_port_t port, uint32 enable)
{
    phy_ctrl_t  *pc;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t *pDesc;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_wcmod_notify_mac_loopback: u=%d p=%d enable=0x%x\n"),
              unit, port, enable));

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    /* XMAC loopback mode needs to enable clock comp for single-port mode */
    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        if ((DEV_CTRL_PTR(pc)->clk_comp && (!enable)) ||
            ((!DEV_CTRL_PTR(pc)->clk_comp) && enable)) {
            /* clock compensation configuration in combo mode */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_XGXSBLK2_UNICOREMODE10Gr(unit, ws,
                     enable? COMBO_HG_XGXS_nLQ | COMBO_XE_XGXS:
                            COMBO_CLOCK_COMP_DEFAULT,
                     COMBO_CLOCK_COMP_MASK));
            DEV_CTRL_PTR(pc)->clk_comp = enable;
        }
    }
    return SOC_E_NONE;
}

STATIC int
_phy_wcmod_speed_mode_decode(int speed_mode, int *speed, int *intf, int *scr)
{
    *scr  = FALSE;
    *intf = SOC_PORT_IF_XGMII;  /* default to XGMII */
    switch (speed_mode) {
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_10M:
            *speed = 10;
            *intf = SOC_PORT_IF_MII;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_100M:
            *speed = 100;
            *intf = SOC_PORT_IF_MII;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_1G:
            *speed = 1000;
            *intf = SOC_PORT_IF_GMII;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_2p5G:
            *speed = 2500;
            *intf = SOC_PORT_IF_GMII;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_5G_X4:
            *speed = 5000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_6G_X4:
            *speed = 6000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_10G_HiG          :
            *speed = 10000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_10G_CX4          :
            *speed = 10000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_12G_HiG          :
            *speed = 12000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_12p5G_X4         :
            *speed = 12500;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_13G_X4           :
            *speed = 13000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_15G_X4           :
            *speed = 15000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_16G_X4           :
            *speed = 16000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_1G_KX            :
            *speed = 1000;
            *intf = SOC_PORT_IF_KX;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_10G_KX4          :
            *speed = 10000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_10G_KR           :
            *speed = 10000;
            *intf = SOC_PORT_IF_KR;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_5G               :
            *speed = 5000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_6p4G             :
            *speed = 6000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_20G_X4           :
            *speed = 20000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_20G_KR2           :
            *speed = 20000;
            *intf = SOC_PORT_IF_KR;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_20G_CR2           :
            *speed = 20000;
            *intf = SOC_PORT_IF_CR;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_21G_X4           :
            *speed = 21000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_25G_X4           :
            *speed = 25000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_10G_HiG_DXGXS    :
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_10G_DXGXS        :
            *speed = 10000;
            *intf = SOC_PORT_IF_RXAUI; 
            break ;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_10p5G_HiG_DXGXS  :
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_10p5G_DXGXS      :
            *speed = 10500;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_12p773G_HiG_DXGXS:
            *speed = 12773;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_12p773G_DXGXS    :
            *speed = 12773;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_10G_XFI          :
            *speed = 10000;
            *intf = SOC_PORT_IF_XFI;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_40G              :
            *speed = 40000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_20G_HiG_DXGXS    :
            *speed = 20000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_20G_DXGXS        :
            *speed = 20000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_10G_SFI          :
            *speed = 10000;
            *intf = SOC_PORT_IF_SFI;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_31p5G            :
            *speed = 30000;  /* MAC has only 30G speed*/
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_32p7G            :
            *speed = 32000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_20G_SCR          :
            *scr = TRUE;
            *speed = 20000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_10G_HiG_DXGXS_SCR:
            *scr = TRUE;
            *speed = 10000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_10G_DXGXS_SCR    :
            *scr = TRUE;
            *speed = 10000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_12G_R2           :
            *speed = 12000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_10G_X2           :
            *speed = 10000;
            *intf = SOC_PORT_IF_RXAUI;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_40G_KR4          :
            *speed = 40000;
            *intf = SOC_PORT_IF_KR4;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_40G_CR4          :
            *speed = 40000;
            *intf = SOC_PORT_IF_CR4;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_100G_CR10        :
            *speed = 100000;
            *intf = SOC_PORT_IF_XLAUI;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_15p75G_DXGXS :
            *speed = 16000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_1250_PCSBYPASS        :
            *speed = 1250;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_3125_PCSBYPASS        :
            *speed = 3125;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_6250_PCSBYPASS        :
            *speed = 6250;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_10p3125_PCSBYPASS        :
            *speed = 10312;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_10p9375_PCSBYPASS        :
            *speed = 10937;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_12500_PCSBYPASS        :
            *speed = 12500;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_11500_PCSBYPASS        :
            *speed = 11500;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_5750_PCSBYPASS        :
            *speed = 5750;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_NON_CANNED_PCSBYPASS        :
            *speed = SOC_PHY_NON_CANNED_SPEED;
            break;
        default:
            break;
    }

    return SOC_E_NONE;
}

STATIC int
_phy_wcmod_ind_speed_get(int unit, soc_port_t port, int *speed, int *intf, int *scr)
{
    phy_ctrl_t *pc;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t *pDesc;
    int rv=0, refclk_freq;
    int speed_mode, vco_freq, oversample_i, oversample_r, divider;
    uint16 data, os_mode;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    ws->diag_type = WCMOD_DIAG_SPEED;

    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));

    speed_mode = ws->accData;
    rv = _phy_wcmod_speed_mode_decode(speed_mode, speed, intf,scr);

    /*added the fix for the warpLite canned speed of 5.75G */
    if (*speed == 11500) {
        SOC_IF_ERROR_RETURN (READ_WC40_PLL_ANAPLLSTATUSr(unit, ws, &data));
        if ((data & 0xf) == 0) {
            *speed = 5750;
        }
    }

    if((*speed==20000)||(*speed==10000)) {
        SOC_IF_ERROR_RETURN (READ_WC40_PLL_ANAPLLSTATUSr(unit, ws, &data));
        if(IS_DUAL_LANE_PORT(pc)) {
            if(*speed==20000) {
                if ((data & PLL_ANAPLLSTATUS_PLL_MODE_AFE_MASK) == PLL_ANAPLLSTATUS_PLL_MODE_AFE_div70) {
                    *speed = 21000 ;
                }
            }
            if((*speed==10000) && 
              (speed_mode== XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_10G_X2)) {
                if ((data & PLL_ANAPLLSTATUS_PLL_MODE_AFE_MASK) == PLL_ANAPLLSTATUS_PLL_MODE_AFE_div92){
                 *speed = 23000 ;
                }
            }
        } else {
            if((*speed==10000) &&
            (speed_mode==XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_10G_XFI)) {
                if ((data & PLL_ANAPLLSTATUS_PLL_MODE_AFE_MASK) == PLL_ANAPLLSTATUS_PLL_MODE_AFE_div70) {
                *speed = 11000 ;
                }
                if ((data & PLL_ANAPLLSTATUS_PLL_MODE_AFE_MASK) == PLL_ANAPLLSTATUS_PLL_MODE_AFE_div92) {
                *speed = 11500 ;
                }
                if ((data & PLL_ANAPLLSTATUS_PLL_MODE_AFE_MASK) == PLL_ANAPLLSTATUS_PLL_MODE_AFE_div80) {
                *speed = 12500 ;
                }
            }
        }
    } else if(SOC_PHY_NON_CANNED_SPEED == *speed) { /*handle non canned speeds*/
        SOC_IF_ERROR_RETURN(READ_WC40_PLL_ANAPLLSTATUSr(unit, ws, &data));
        switch(data & PLL_ANAPLLSTATUS_PLL_MODE_AFE_MASK){
            case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div36: divider  = 72;  break;
            case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div40: divider  = 40;  break;
            case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div42: divider  = 42;  break;
            case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div48: divider  = 48;  break;
            case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div50: divider  = 50;  break;
            case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div52: divider  = 52;  break;
            case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div54: divider  = 54;  break;
            case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div60: divider  = 60;  break;
            case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div64: divider  = 64;  break;
            case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div66: divider  = 66;  break;
            case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div68: divider  = 68;  break;
            case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div70: divider  = 70;  break;
            case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div80: divider  = 80;  break;
            case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div92: divider  = 92;  break;
            case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div100: divider = 100; break;
            default: 
                divider  = 46;
                break;
        } 

        refclk_freq = (DEV_CFG_PTR(pc)->refclk == 0 || DEV_CFG_PTR(pc)->refclk == 156) ? 25 : 0;
        vco_freq = DEV_CFG_PTR(pc)->refclk * divider + (refclk_freq * divider)/100;

        SOC_IF_ERROR_RETURN(READ_WC40_DIGITAL5_MISC7r(unit, ws, &data));
        os_mode = (data & DIGITAL5_MISC7_FORCE_OSCDR_MODE_VAL_MASK) >> DIGITAL5_MISC7_FORCE_OSCDR_MODE_VAL_SHIFT;
        switch(os_mode) {
            case 0x0: oversample_i = 1;  oversample_r = 0;  break;
            case 0x1: oversample_i = 2;  oversample_r = 0;  break;
            case 0x2: oversample_i = 4;  oversample_r = 0;  break;
            case 0x3: oversample_i = 5;  oversample_r = 0;  break;
            case 0x6: oversample_i = 8;  oversample_r = 25; break;
            case 0x7: oversample_i = 3;  oversample_r = 3;  break;
            case 0x8: oversample_i = 10; oversample_r = 0;  break;
            case 0x9: oversample_i = 3;  oversample_r = 0;  break;
            case 0xA: oversample_i = 8;  oversample_r = 0;  break;
            default: return SOC_E_CONFIG;
        }

        *speed = vco_freq*100 / (oversample_i*100 + oversample_r);
    }

    return SOC_E_NONE;
}

STATIC int
_phy_wcmod_combo_speed_get(int unit, soc_port_t port, int *speed, int *intf, int *scr)
{
    phy_ctrl_t *pc;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t *pDesc;
    int rv=0;
    int speed_mode;
    uint16 data;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    ws->diag_type = WCMOD_DIAG_SPEED;

    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));

    speed_mode = ws->accData;
    rv = _phy_wcmod_speed_mode_decode(speed_mode, speed, intf,scr);

    if (*speed == 40000) {
        /* read the PLL status to see if 40G or 42G or 48G */
        SOC_IF_ERROR_RETURN (READ_WC40_PLL_ANAPLLSTATUSr(unit, ws, &data));
        if ((data & PLL_ANAPLLSTATUS_PLL_MODE_AFE_MASK) == PLL_ANAPLLSTATUS_PLL_MODE_AFE_div70) {
            *speed = 42000;
        }
        if ((data & PLL_ANAPLLSTATUS_PLL_MODE_AFE_MASK) == PLL_ANAPLLSTATUS_PLL_MODE_AFE_div80) {
            *speed = 48000;
        }
    }

    if (!SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) {
        if (SOC_INFO(unit).port_num_lanes[pc->port] >= 10) {
            wcmod_st  *mld;
            uint16 data16;
            mld   =  (wcmod_st *)( ws + 3);
            /*temp_id = mld->phy_ad;
            SOC_IF_ERROR_RETURN
                (_wcmod_phy_parameter_copy(pc, mld));
            mld->phy_ad = temp_id; */
            /* read MLD pcs status */
            SOC_IF_ERROR_RETURN
                (WC40_REG_READ(unit, mld, 0x0, 0x8420, &data16));
            if (data16 & 0xf00) {
                /*12 lane active */
                if (DEV_CFG_PTR(pc)->hg_mode) {
                    SOC_IF_ERROR_RETURN (READ_WC40_PLL_ANAPLLSTATUSr(unit, ws, &data));
                    if ((data & PLL_ANAPLLSTATUS_PLL_MODE_AFE_MASK) == PLL_ANAPLLSTATUS_PLL_MODE_AFE_div70) {
                        *speed = 127000;
                    } else {
                        *speed  = 120000;
                    }
                } else if (SOC_INFO(unit).port_num_lanes[pc->port] == 10) {
                    *speed = 100000;
                } else {
                    *speed  = 120000;
                }
            } else {
                /* 10 lane active */
                if (DEV_CFG_PTR(pc)->hg_mode) {
                    *speed = 106000;
                } else { 
                    *speed  = 100000;
                }
            }
        }
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_wcmod_speed_get
 * Purpose:
 *      Get PHY speed
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      speed - current link speed in Mbps
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_wcmod_speed_get(int unit, soc_port_t port, int *speed)
{
    phy_ctrl_t *pc;
    int rv;
    int intf;
    int scr;

    pc = INT_PHY_SW_STATE(unit, port);

    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) { 
        rv = _phy_wcmod_combo_speed_get(unit, port, speed,&intf,&scr);
    } else {
            rv = _phy_wcmod_ind_speed_get(unit, port, speed,&intf,&scr);
    }
    return rv;
}

/***************************/
/*                         */
/* start of xenia AN codes */
/*                         */
/***************************/
STATIC int
phy_wcmod_xgxs16g1l_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t  *pc;
    uint16      speed_val, mask;
    uint16      sgmii_status, data16 ;
    uint16      speed_mii;
    /* int asymmetric_speed_mode = 0;  */
     wcmod_st    *ws;
    WCMOD_DEV_DESC_t *pDesc;
   
    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);
    
    if (speed > 2500) {
        return (SOC_E_CONFIG);
    }

    /* first check if qsgmii core and in combo mode */
    if (((ws->model_type) == WCMOD_QS_A0) && (ws->port_type == WCMOD_COMBO)) {
        /* Update speed */
        SOC_IF_ERROR_RETURN
            (phy_wcmod_speed_set(unit, port, speed));
        return SOC_E_NONE;
    }

    speed_val = 0;
    speed_mii = 0;
    mask      = 0x10 | 0xf ; 
       /* SERDESDIGITAL_MISC1_FORCE_SPEED_SEL_MASK |
          SERDESDIGITAL_MISC1_FORCE_SPEED_MASK;  */

    /* asymmetric_speed_mode = soc_property_port_get(unit, port, spn_SERDES_ASYMMETRIC_SPEED_MODE, 0); */
    
    switch (speed) {
    case 0:
        /* Do not change speed */
        return SOC_E_NONE;
    case 10:
        speed_mii = MII_CTRL_SS_10;
        break;
    case 100:
        speed_mii = MII_CTRL_SS_100;
        break;
    case 1000:
        speed_mii = MII_CTRL_SS_1000;
        break;
    case 2500:
        speed_val = 0;
        break;
    default:
        return SOC_E_PARAM;
    }

    /* 2.5G only valid in fiber mode */
    if (speed == 2500) {
       speed_val |= 0x10 ; /* SERDESDIGITAL_MISC1_FORCE_SPEED_SEL_MASK; */
    }

    /* hold TX FIFO in reset */
    SOC_IF_ERROR_RETURN
       (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X3r(unit, ws, 0x1, 0x1)) ;
    /*  SERDESDIGITAL_CONTROL1000X3_TX_FIFO_RST_MASK */

    /* XXXX asymmetric_speed_mode code not ported */
    
    /* XXXX disable 100FX idle detect & disable 100FX and 100FX auto-detect not ported */
    
    SOC_IF_ERROR_RETURN
          (READ_WC40_SERDESDIGITAL_STATUS1000X1r(unit, ws, &sgmii_status));
    
    sgmii_status &= SERDESDIGITAL_STATUS1000X1_SGMII_MODE_MASK;

    SOC_IF_ERROR_RETURN
       (MODIFY_WC40_SERDESDIGITAL_MISC1r(unit, ws, speed_val, mask));
     
    if ( (!sgmii_status && (speed == 100)) ) {  /* XXXX asymmetric_speed_mode condition removed */
       /* fiber mode 100fx, enable */
       SOC_IF_ERROR_RETURN
          (MODIFY_WC40_FX100_CONTROL1r(unit,ws,
                                          FX100_CONTROL1_FAR_END_FAULT_EN_MASK |
                                          FX100_CONTROL1_ENABLE_MASK,
                                          FX100_CONTROL1_FAR_END_FAULT_EN_MASK |
                                          FX100_CONTROL1_ENABLE_MASK));
       
       /* enable 100fx extended packet size */
       SOC_IF_ERROR_RETURN
          (MODIFY_WC40_FX100_CONTROL2r(unit,ws,
                                          FX100_CONTROL2_EXTEND_PKT_SIZE_MASK,
                                          FX100_CONTROL2_EXTEND_PKT_SIZE_MASK));
       
    } else {
       SOC_IF_ERROR_RETURN
          (READ_WC40_COMBO_IEEE0_MIICNTLr(unit, ws, &data16));
       data16 &= ~(MII_CTRL_SS_LSB | MII_CTRL_SS_MSB);
       data16 |= speed_mii;
       SOC_IF_ERROR_RETURN
          (WRITE_WC40_COMBO_IEEE0_MIICNTLr(unit, ws, data16));
    }
    
    /* XXXX asymmetric_speed_mode code not ported */

    /* release TX FIFO reset */
    SOC_IF_ERROR_RETURN
       (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X3r(unit, ws,
                                                    0,
                                                    SERDESDIGITAL_CONTROL1000X3_TX_FIFO_RST_MASK));
    
    return (SOC_E_NONE);
}


STATIC int
_phy_wcmod_xgxs16g1l_stop(int unit, soc_port_t port)
{
    uint16 mask16,data16;
    int          stop, copper;
    phy_ctrl_t  *pc;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t *pDesc;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

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
                         "phy_xgxs16g1l_stop: u=%d p=%d copper=%d stop=%d flg=0x%x\n"),
              unit, port, copper, stop,
              pc->stop));

    mask16 = (1 << pc->lane_num);    /* rx lane */
    mask16 |= (mask16 << 4); /* add tx lane */
                                                                               
    /* for qsgmii core, use 0xffe0 instead */
    if ((ws->model_type) == WCMOD_QS_A0) {
        if (stop) {
            data16 = 1 << IEEE0BLK_MIICNTL_PWRDWN_SW_SHIFT;
        } else {
            data16 = 0;
        }         
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_COMBO_IEEE0_MIICNTLr(unit, ws, data16, 
                                              IEEE0BLK_MIICNTL_PWRDWN_SW_MASK));

    } else {
        mask16 = (1 << pc->lane_num);    /* rx lane */
        mask16 |= (mask16 << 4); /* add tx lane */
                                                                                   
        if (stop) {
            data16 = mask16;
        } else {
            data16 = 0;
        }
                                                                                   
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_XGXSBLK1_LANECTRL3r(unit,ws,data16,mask16));
    }                                                                           
    return SOC_E_NONE;
}

STATIC int
_phy_wcmod_xgxs16g1l_notify_stop(int unit, soc_port_t port, uint32 flags)
{
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    pc->stop |= flags;

    return _phy_wcmod_xgxs16g1l_stop(unit, port);
}

STATIC int
_phy_wcmod_xgxs16g1l_notify_resume(int unit, soc_port_t port, uint32 flags)
{
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    pc->stop &= ~flags;

    return _phy_wcmod_xgxs16g1l_stop(unit, port);
}

STATIC int
_phy_wcmod_xgxs16g1l_notify_speed(int unit, soc_port_t port, uint32 speed)
{
    /* Put SERDES PHY in reset */
    SOC_IF_ERROR_RETURN
        (_phy_wcmod_xgxs16g1l_notify_stop(unit, port, PHY_STOP_SPEED_CHG));

    /* Update speed */
    SOC_IF_ERROR_RETURN
        (phy_wcmod_xgxs16g1l_speed_set(unit, port, speed));

    /* Take SERDES PHY out of reset */
    SOC_IF_ERROR_RETURN
        (_phy_wcmod_xgxs16g1l_notify_resume(unit, port, PHY_STOP_SPEED_CHG));

    /* Autonegotiation must be turned off to talk to external PHY */
    if (!PHY_SGMII_AUTONEG_MODE(unit, port) && PHY_EXTERNAL_MODE(unit, port)) {        
       SOC_IF_ERROR_RETURN
          (phy_wcmod_xgxs16g1l_an_set(unit, port, FALSE));
    }

    return (SOC_E_NONE);
}


/*
 * Function:    
 *      phy_xgxs16g1l_an_set
 * Purpose:     
 *      Enable or disabled auto-negotiation on the specified port.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      an   - Boolean, if true, auto-negotiation is enabled 
 *              (and/or restarted). If false, autonegotiation is disabled.
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int
phy_wcmod_xgxs16g1l_an_set(int unit, soc_port_t port, int an)
{
    phy_ctrl_t  *pc;
    uint16             an_enable;
    uint16             auto_det;
    WCMOD_DEV_DESC_t *pDesc;
    wcmod_st *ws;
    uint16  data16, mask16, mode_1000x;
    
/*                XGXS_16G          XGXS_13G                    Quad SGMII
 *   CL37         lane0 only        lane0 only                  lane0 only
 *   CL37 BAM     no                no                          no
 *   CL73         lane0 only        all four lanes              all four lanes
 *   CL73 BAM     no                no                          no
 */                                                                                
    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);
                                                        
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(pc->unit,
                         "phy_wc_xgxs16g1l_an_se:t u=%d p=%d an=%d\n"),
              unit, port, an));
                                                           
    an_enable = 0;
    auto_det  = 0;

    /* Disable BAM37 mode and Teton mode */
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_DIGITAL6_MP5_NEXTPAGECTRLr(unit, ws, 0));
    /* disable CL73 BAM */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_CL73_USERB0_CL73_BAMCTRL1r(unit,ws, 0,
                                    CL73_USERB0_CL73_BAMCTRL1_CL73_BAMEN_MASK));
    SOC_IF_ERROR_RETURN
       (WRITE_WC40_DIGITAL6_UD_FIELDr(unit, ws, 0));
    
    if (!PHY_CLAUSE73_MODE(unit, port)) {
        /* disable CL73 AN device*/
        SOC_IF_ERROR_RETURN
           (WRITE_WC40_AN_IEEE0BLK_AN_IEEECONTROL1r(unit,ws,0));
    }
        
    if (an) {                                                                         
        an_enable = MII_CTRL_AE | MII_CTRL_RAN;
           
        data16 = MII_CTRL_FD | MII_CTRL_SS_1000;   /* default to 1G */
        SOC_IF_ERROR_RETURN
           (MODIFY_WC40_COMBO_IEEE0_MIICNTLr(ws->unit, ws, data16, data16)) ;
        
        /* set fiber mode if required */
        if ((PHY_FIBER_MODE(unit, port) && !PHY_EXTERNAL_MODE(unit, port)) ||
            PHY_PASSTHRU_MODE(unit, port) ||
            PHY_SGMII_AUTONEG_MODE(unit, port)) {
           /* Enable 1000X parallel detect */
           data16 = SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK;
           mask16 = SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK;
        } else {
           data16 = 0;
           mask16 = SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK |
              SERDESDIGITAL_CONTROL1000X2_FILTER_FORCE_LINK_MASK;
        }
        SOC_IF_ERROR_RETURN
           (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X2r
            (unit,ws, data16, mask16)) ;
        
        mode_1000x = 0;
        if ( (PHY_FIBER_MODE(unit, port) && !PHY_EXTERNAL_MODE(unit, port)) ||
             PHY_PASSTHRU_MODE(unit, port)) {
           mode_1000x = SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;
        }
        
        /* Allow property to override */
        mask16 = SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;
        if (soc_property_port_get(unit, port,
                                  spn_SERDES_FIBER_PREF, mode_1000x)) {
           data16 = SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;
        } else {
           data16 = 0 ;
        }
        SOC_IF_ERROR_RETURN
           (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r (unit, ws, data16, mask16)) ;
        
        /* enable parallel detection */
        SOC_IF_ERROR_RETURN
           (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X2r
            (unit,ws,
             SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK, 
             SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK));    
        
        /*
         * Should read one during init and cache it in Phy flags
         */
        if (soc_property_port_get(unit, port,
                                  spn_SERDES_AUTOMEDIUM,
                                  FALSE)) {
            auto_det = SERDESDIGITAL_CONTROL1000X1_AUTODET_EN_MASK;
        }
                                                                                
        /* If auto-neg is enabled, make sure not forcing any speed */
        /* first disable pll force bit */
        SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_MISC1r(unit, ws, 0x0,
                     SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK | 
                     SERDESDIGITAL_MISC1_FORCE_LN_MODE_MASK));    

        /* next disable the asymmetric bit in 0x8169 */
        MODIFY_WC40_XGXSBLK8_TXLNSWAP1r(pc->unit, ws, 0, 
               XGXSBLK8_TXLNSWAP1_ASYMMETRIC_MODE_EN_MASK);

        /*clear the tx/rx setting on the oversample of 1G speed */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_TXB_ANATXACONTROL1r(unit, ws, 0x0, 0x40));
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_RXB_ANARXACONTROL1r(unit, ws, 0x0, 0x40)); 

        /* Enable/Disable auto detect */
        SOC_IF_ERROR_RETURN
             (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(unit, ws, auto_det,
                                 SERDESDIGITAL_CONTROL1000X1_AUTODET_EN_MASK));
                                                                                
    } else {
        /* Disable auto detect */
        SOC_IF_ERROR_RETURN
             (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(unit, ws, auto_det,
                                 SERDESDIGITAL_CONTROL1000X1_AUTODET_EN_MASK));
                                                                                
    }
                                                                                
    /* restart the autoneg if enabled, or disable the autoneg */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_COMBO_IEEE0_MIICNTLr(unit, ws, an_enable,
                                      MII_CTRL_AE | MII_CTRL_RAN));

    /* might require BCST. revist */
    if (PHY_CLAUSE73_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_AN_IEEE0BLK_AN_IEEECONTROL1r(unit, ws, an_enable, 
                                                      MII_CTRL_AE | MII_CTRL_RAN));
    }
                                                                                
    pc->fiber.autoneg_enable = an;
                                                                                
    return SOC_E_NONE;
}

STATIC int
phy_wcmod_xgxs16g1l_an_get(int unit, soc_port_t port, int *an, int *an_done)
{
    uint16      mii_ctrl;
    uint16      mii_stat;
    phy_ctrl_t *pc;
    WCMOD_DEV_DESC_t *pDesc;
    wcmod_st *ws;

    /* printf ("phy_wc_xgxs16g1l_an_get: u=%0d p=%0d an=%0d an_done=%0d\n", unit, port, *an, *an_done) ; */
                                                
    if ((NULL == an) || (NULL == an_done)) {
        return (SOC_E_PARAM);
    }

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (READ_WC40_COMBO_IEEE0_MIICNTLr(unit, ws, &mii_ctrl));
    SOC_IF_ERROR_RETURN
        (READ_WC40_COMBO_IEEE0_MIISTATr(unit, ws, &mii_stat));
                                                                                
    *an      = (mii_ctrl & MII_CTRL_AE) ? TRUE : FALSE;        /* 2^12 */
    *an_done = (mii_stat & MII_STAT_AN_DONE) ? TRUE : FALSE;   /* 2^5 */
    /* XXXX need to revisit the logic here: the codes mean if CL37 are not completed, check CL73  */
    
    if (!((*an == TRUE) && (*an_done == TRUE))) {
        if (PHY_CLAUSE73_MODE(unit, port)) {
            /* check clause 73 */
            SOC_IF_ERROR_RETURN
                (READ_WC40_AN_IEEE0BLK_AN_IEEECONTROL1r(unit, ws, &mii_ctrl)); 
            SOC_IF_ERROR_RETURN
                (READ_WC40_AN_IEEE0BLK_AN_IEEESTATUS1r(unit, ws, &mii_stat));

            *an      = (mii_ctrl & MII_CTRL_AE) ? TRUE : FALSE;
            *an_done = (mii_stat & MII_STAT_AN_DONE) ? TRUE : FALSE;
        }
    }

    return SOC_E_NONE;
}


STATIC int
_phy_wcmod_xgxs16g1l_c73_adv_local_set(int unit, soc_port_t port,
                            soc_port_ability_t *ability)
{
    uint16            an_adv;
    uint16            pause;
    phy_ctrl_t       *pc;
    WCMOD_DEV_DESC_t *pDesc;
    wcmod_st *ws;
    
    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    an_adv = (ability->speed_full_duplex & SOC_PA_SPEED_1000MB) ?
               CL73_AN_ADV_TECH_1G_KX : 0;
    an_adv |= (ability->speed_full_duplex & SOC_PA_SPEED_10GB) ?
              CL73_AN_ADV_TECH_10G_KX4 : 0;

    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_AN_IEEE1BLK_AN_ADVERTISEMENT1r(unit, ws, an_adv, (0x7 << 5))) ;
         /* CL73_AN_ADV_TECH_SPEEDS_MASK = 0x7 << 5, in wcmod.h is different */

    switch (ability->pause & (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX)) {
    case SOC_PA_PAUSE_TX:
        pause = CL73_AN_ADV_ASYM_PAUSE;
        break;
    case SOC_PA_PAUSE_RX:
        pause = CL73_AN_ADV_PAUSE | CL73_AN_ADV_ASYM_PAUSE;
        break;
    case SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX:
        pause = CL73_AN_ADV_PAUSE;
        break;
    default:
        pause = 0;
        break;
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_AN_IEEE1BLK_AN_ADVERTISEMENT0r(unit, ws, pause, 
                                     (CL73_AN_ADV_PAUSE |
                                      CL73_AN_ADV_ASYM_PAUSE)));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(pc->unit,
                         "_phy_wc_xgxs16g1l_c73_adv_local_set: u=%d p=%d pause=%08x speeds=%04x,adv=0x%x\n"),
              unit, port, pause, an_adv,ability->speed_full_duplex));
    return SOC_E_NONE;
}

STATIC int
_phy_wcmod_xgxs16g1l_c73_adv_local_get(int unit, soc_port_t port,
                            soc_port_ability_t *ability)
{
    uint16            an_adv;
    soc_port_mode_t   speeds,pause;
    phy_ctrl_t       *pc;
    WCMOD_DEV_DESC_t *pDesc;
    wcmod_st *ws;
    

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (READ_WC40_AN_IEEE1BLK_AN_ADVERTISEMENT1r(unit, ws, &an_adv));

    speeds  = (an_adv & CL73_AN_ADV_TECH_1G_KX) ? SOC_PA_SPEED_1000MB : 0;
    speeds |= (an_adv & CL73_AN_ADV_TECH_10G_KX4) ? SOC_PA_SPEED_10GB : 0;

    ability->speed_full_duplex |= speeds;

    SOC_IF_ERROR_RETURN
        (READ_WC40_AN_IEEE1BLK_AN_ADVERTISEMENT0r(unit, ws, &an_adv));

    switch (an_adv & (CL73_AN_ADV_PAUSE | CL73_AN_ADV_ASYM_PAUSE)) {
        case CL73_AN_ADV_PAUSE:
            pause = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
            break;
        case CL73_AN_ADV_ASYM_PAUSE:
            pause = SOC_PA_PAUSE_TX;
            break;
        case CL73_AN_ADV_PAUSE | CL73_AN_ADV_ASYM_PAUSE:
            pause = SOC_PA_PAUSE_RX;
            break;
        default:
            pause = 0;
            break;
    }
    ability->pause = pause;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(pc->unit,
                         "_phy_wc_xgxs16g1l_c73_adv_local_get: u=%d p=%d pause=%08x speeds=%04x\n"),
              unit, port, pause, speeds));
    return SOC_E_NONE;
}

STATIC int
_phy_wcmod_xgxs16g1l_c73_adv_remote_get(int unit, soc_port_t port,
                             soc_port_ability_t *ability)
{
    uint16            an_adv;
    soc_port_mode_t   mode;
    phy_ctrl_t       *pc;
    WCMOD_DEV_DESC_t *pDesc;
    wcmod_st *ws;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);
    
    SOC_IF_ERROR_RETURN
        (READ_WC40_AN_IEEE1BLK_AN_LP_BASEPAGEABILITY1r(unit, ws, &an_adv));

    mode  = (an_adv & CL73_AN_ADV_TECH_1G_KX) ? SOC_PA_SPEED_1000MB : 0;
    mode |= (an_adv & CL73_AN_ADV_TECH_10G_KX4) ? SOC_PA_SPEED_10GB : 0;
    ability->speed_full_duplex |= mode;

    SOC_IF_ERROR_RETURN
        (READ_WC40_AN_IEEE1BLK_AN_LP_BASEPAGEABILITY0r(unit, ws, &an_adv));

    mode = 0;
    switch (an_adv & (CL73_AN_ADV_PAUSE | CL73_AN_ADV_ASYM_PAUSE)) {
        case CL73_AN_ADV_PAUSE:
            mode = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
            break;
        case CL73_AN_ADV_ASYM_PAUSE:
            mode = SOC_PA_PAUSE_TX;
            break;
        case CL73_AN_ADV_PAUSE | CL73_AN_ADV_ASYM_PAUSE:
            mode = SOC_PA_PAUSE_RX;
            break;
    }
    ability->pause = mode;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(pc->unit,
                         "_phy_wc_xgxs16g1l_c73_adv_local_get: u=%d p=%d pause=%08x speeds=%04x\n"),
              unit, port, mode, ability->speed_full_duplex));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_xgxs16g1l_ability_advert_set
 * Purpose:
 *      Set the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is set only for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
phy_wcmod_xgxs16g1l_ability_advert_set(int unit, soc_port_t port,
                          soc_port_ability_t *ability)
{
    uint16           an_adv;
    phy_ctrl_t      *pc;
    WCMOD_DEV_DESC_t *pDesc;
    wcmod_st *ws;
                                                                               
    if (NULL == ability) {
        return (SOC_E_PARAM);
    }
                                                                                
    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);
                                                               
    /* Set advertised duplex (only FD supported).  */
    an_adv = (ability->speed_full_duplex & SOC_PA_SPEED_1000MB) ? 
              MII_ANA_C37_FD : 0;
                                                                                
    /* Set advertised pause bits in link code word.  */
    switch (ability->pause & (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX)) {
    case SOC_PA_PAUSE_TX:
        an_adv |= MII_ANA_C37_ASYM_PAUSE;
        break;
    case SOC_PA_PAUSE_RX:
        an_adv |= MII_ANA_C37_PAUSE | MII_ANA_C37_ASYM_PAUSE;
        break;
    case SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX:
        an_adv |= MII_ANA_C37_PAUSE;
        break;
    }
    /* Update less than 1G capability */
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_COMBO_IEEE0_AUTONEGADVr(unit, ws, an_adv));
     
    if (PHY_CLAUSE73_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (_phy_wcmod_xgxs16g1l_c73_adv_local_set(unit, port, ability));
    }
                                                                                
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(pc->unit,
                         "phy_wc_xgxs16g1l_ability_advert_set: u=%d p=%d pause=%08x OVER1G_UP1 %04x\n"),
              unit, port, ability->pause, an_adv));

    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_xgxs16g1l_ability_advert_get
 * Purpose:
 *      Get the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
phy_wcmod_xgxs16g1l_ability_advert_get(int unit, soc_port_t port,
                              soc_port_ability_t *ability)
{
    phy_ctrl_t      *pc;
    uint16           an_adv;
    WCMOD_DEV_DESC_t *pDesc;
    wcmod_st *ws;

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    WCMOD_MEM_SET(ability, 0, sizeof(*ability));

    SOC_IF_ERROR_RETURN
        (READ_WC40_COMBO_IEEE0_AUTONEGADVr(unit, ws, &an_adv));
                                                                                
    ability->speed_full_duplex = 
                  (an_adv & MII_ANA_C37_FD) ? SOC_PA_SPEED_1000MB : 0;

    switch (an_adv & (MII_ANA_C37_PAUSE | MII_ANA_C37_ASYM_PAUSE)) {
        case MII_ANA_C37_PAUSE:
            ability->pause = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
            break;
        case MII_ANA_C37_ASYM_PAUSE:
            ability->pause = SOC_PA_PAUSE_TX;
            break;
        case MII_ANA_C37_PAUSE | MII_ANA_C37_ASYM_PAUSE:
            ability->pause = SOC_PA_PAUSE_RX;
            break;
    }

    /* check for clause73 */
    if (PHY_CLAUSE73_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (_phy_wcmod_xgxs16g1l_c73_adv_local_get(unit, port, ability));
    }
                                                                             
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(pc->unit,
                         "phy_wc_xgxs16g1l_ability_advert_get:unit=%d p=%d pause=%08x sp=%08x\n"),
              unit, port, ability->pause, ability->speed_full_duplex));
                                                                                
    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_xgxs16g1l_ability_remote_get
 * Purpose:
 *      Get the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
phy_wcmod_xgxs16g1l_ability_remote_get(int unit, soc_port_t port,
                               soc_port_ability_t *ability)
{
    phy_ctrl_t       *pc;
    uint16            an_adv;
    soc_port_mode_t   mode;
    WCMOD_DEV_DESC_t *pDesc;
    wcmod_st *ws;

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    WCMOD_MEM_SET(ability, 0, sizeof(*ability));

    SOC_IF_ERROR_RETURN
        (READ_WC40_COMBO_IEEE0_AUTONEGLPABILr(unit, ws, &an_adv));

    mode = (an_adv & MII_ANP_C37_FD) ? SOC_PA_SPEED_1000MB : 0;
    ability->speed_full_duplex = mode;

    mode = 0;
    switch (an_adv & (MII_ANP_C37_PAUSE | MII_ANP_C37_ASYM_PAUSE)) {
        case MII_ANP_C37_PAUSE:
            mode |= SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
            break;
        case MII_ANP_C37_ASYM_PAUSE:
            mode |= SOC_PA_PAUSE_TX;
            break;
        case MII_ANP_C37_PAUSE | MII_ANP_C37_ASYM_PAUSE:
            mode |= SOC_PA_PAUSE_RX;
            break;
    }
    ability->pause = mode;

    if (PHY_CLAUSE73_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (_phy_wcmod_xgxs16g1l_c73_adv_remote_get(unit, port, ability));
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(pc->unit,
                         "phy_wcmod_xgxs16g1l_ability_remote_get:unit=%d p=%d pause=%08x sp=%08x\n"),
              unit, port, ability->pause, ability->speed_full_duplex));

    return (SOC_E_NONE);
}

int
phy_wcmod_100g_an_set(int unit, soc_port_t port, int an)
{
    int rv;
    phy_ctrl_t *pc;
    wcmod_st    *ws_master, *ws_slave0, *ws_slave1, *ws_mld;
    WCMOD_DEV_DESC_t     *pDesc;
    WCMOD_TX_DRIVE_t tx_drv[MAX_NUM_LANES];  

    rv =SOC_E_NONE;
    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws_master   =  (wcmod_st *)( pDesc + 1);
                                                
    SOC_IF_ERROR_RETURN(_wcmod_phy_parameter_copy(pc, ws_master));

    ws_slave0   =  (wcmod_st *)( ws_master + 1);
    ws_slave1   =  (wcmod_st *)( ws_master + 2);
    ws_mld      =  (wcmod_st *)( ws_master + 3);

    /* first we need to disable pll and tx  */
    ws_master->lane_select = 0 ;
    ws_slave0->lane_select = 0;
    ws_slave1->lane_select = 0;
    ws_master->this_lane = 0;
    ws_slave0->this_lane = 0;
    ws_slave1->this_lane = 0;
    ws_mld->this_lane = 0;
    SOC_IF_ERROR_RETURN
        (WC40_REG_WRITE(unit, ws_master, 0x00, 0x8000009, 0x1));
    SOC_IF_ERROR_RETURN
        (WC40_REG_WRITE(unit, ws_master, 0x00, 0x8000, 0xc2f));
    SOC_IF_ERROR_RETURN
        (WC40_REG_WRITE(unit, ws_slave0, 0x00, 0x8000, 0xc2f));
    SOC_IF_ERROR_RETURN
        (WC40_REG_WRITE(unit, ws_slave1, 0x00, 0x8000, 0xc2f));

    /* next conifg the tx parameter */
    SOC_IF_ERROR_RETURN
        (_phy_wcmod_tx_control_get(unit, pc, &tx_drv[0], TXDRV_AN_INX));
    SOC_IF_ERROR_RETURN
        (_phy_wcmod_tx_control_set(unit, pc, &tx_drv[0]));

    /* next config master core and slave core lane config */
    SOC_IF_ERROR_RETURN
        (WC40_REG_WRITE(unit, ws_master, 0x00, 0x8197, 0x9000));
    SOC_IF_ERROR_RETURN
        (WC40_REG_WRITE(unit, ws_slave0, 0x00, 0x8197, 0xc000));
    SOC_IF_ERROR_RETURN
        (WC40_REG_WRITE(unit, ws_slave1, 0x00, 0x8197, 0xc000));


    /* next diable cl37 on all the lanes */
    ws_master->lane_select = WCMOD_LANE_BCST; /* go to broadcast mode */
    ws_slave0->lane_select = WCMOD_LANE_BCST; /* go to broadcast mode */
    ws_slave1->lane_select = WCMOD_LANE_BCST; /* go to broadcast mode */
    SOC_IF_ERROR_RETURN
        (WC40_REG_WRITE(unit, ws_master, 0x00, 0xffe0, 0x140));
    SOC_IF_ERROR_RETURN
        (WC40_REG_WRITE(unit, ws_slave0, 0x00, 0xffe0, 0x140));
    SOC_IF_ERROR_RETURN
        (WC40_REG_WRITE(unit, ws_slave1, 0x00, 0xffe0, 0x140));

    /* next disable cl73 on all the slave lanes*/
    SOC_IF_ERROR_RETURN
        (WC40_REG_WRITE(unit, ws_slave0, 0x00, 0x38000000, 0x0));
    SOC_IF_ERROR_RETURN
        (WC40_REG_WRITE(unit, ws_slave1, 0x00, 0x38000000, 0x0));

    /* next enable cl72 on all the lanes */
    SOC_IF_ERROR_RETURN
        (WC40_REG_WRITE(unit, ws_master, 0x00, 0x82e3, 0x3f00));
    SOC_IF_ERROR_RETURN
        (WC40_REG_WRITE(unit, ws_slave0, 0x00, 0x82e3, 0x3f00));
    SOC_IF_ERROR_RETURN
        (WC40_REG_WRITE(unit, ws_slave1, 0x00, 0x82e3, 0x3f00));

    /* clear the forced speed */
    SOC_IF_ERROR_RETURN
        (WC40_REG_WRITE(unit, ws_master, 0x00, 0x833e, 0x0800));
    SOC_IF_ERROR_RETURN
        (WC40_REG_WRITE(unit, ws_slave0, 0x00, 0x833e, 0x0800));
    SOC_IF_ERROR_RETURN
        (WC40_REG_WRITE(unit, ws_slave1, 0x00, 0x833e, 0x0800));

    SOC_IF_ERROR_RETURN
        (WC40_REG_WRITE(unit, ws_master, 0x00, 0x82e2, 0x00));
    SOC_IF_ERROR_RETURN
        (WC40_REG_WRITE(unit, ws_slave0, 0x00, 0x82e2, 0x00));
    SOC_IF_ERROR_RETURN
        (WC40_REG_WRITE(unit, ws_slave1, 0x00, 0x82e2, 0x00));

    /* next reg 0x8141 to be 0x401b */
    ws_master->this_lane = 0;
    ws_slave0->this_lane = 0;
    ws_slave1->this_lane = 0;
    SOC_IF_ERROR_RETURN
        (WC40_REG_WRITE(unit, ws_master, 0x00, 0x8141, 0x401b));
    SOC_IF_ERROR_RETURN
        (WC40_REG_WRITE(unit, ws_slave0, 0x00, 0x8141, 0x401b));
    SOC_IF_ERROR_RETURN
        (WC40_REG_WRITE(unit, ws_slave1, 0x00, 0x8141, 0x401b));

    /* next reg 0x8169 to be 0xfe4 */
    SOC_IF_ERROR_RETURN
        (WC40_REG_WRITE(unit, ws_master, 0x00, 0x8169, 0xfe4));
    SOC_IF_ERROR_RETURN
        (WC40_REG_WRITE(unit, ws_slave0, 0x00, 0x8169, 0xfe4));
    SOC_IF_ERROR_RETURN
        (WC40_REG_WRITE(unit, ws_slave1, 0x00, 0x8169, 0xfe4));

    /* next diable cl37 on all the lanes */
    ws_master->lane_select = WCMOD_LANE_BCST; /* go to broadcast mode */
    ws_slave0->lane_select = WCMOD_LANE_BCST; /* go to broadcast mode */
    ws_slave1->lane_select = WCMOD_LANE_BCST; /* go to broadcast mode */
    /* programn 0x82ec to be 0x401*/
    SOC_IF_ERROR_RETURN
        (WC40_REG_WRITE(unit, ws_master, 0x00, 0x82ec, 0x401));
    SOC_IF_ERROR_RETURN
        (WC40_REG_WRITE(unit, ws_slave0, 0x00, 0x82ec, 0x401));
    SOC_IF_ERROR_RETURN
        (WC40_REG_WRITE(unit, ws_slave1, 0x00, 0x82ec, 0x401));

    /* next restart pll on the master core */
    ws_master->lane_select = 0;
    ws_master->this_lane = 0;
    SOC_IF_ERROR_RETURN
        (WC40_REG_WRITE(unit, ws_master, 0x00, 0x8000, 0x2c2f));

    sal_usleep(1000);

    SOC_IF_ERROR_RETURN
        (WC40_REG_WRITE(unit, ws_master, 0x00, 0x8000009, 0x0));

    /*  next restart AN */
    ws_master->this_lane = 0;
    if (an) {
        ws_master->per_lane_control = USE_CLAUSE_73_AN << (ws_master->this_lane * 8);
    } else {
        ws_master->per_lane_control = 0;
    }
    SOC_IF_ERROR_RETURN
       (wcmod_tier1_selector("AUTONEG_CONTROL", ws_master, &rv)); 

    return SOC_E_NONE;
}



STATIC int
phy_wcmod_xgxs16g1l_ability_local_get(int unit, soc_port_t port,
                                      soc_port_ability_t *ability)
{
    phy_ctrl_t *pc;
    
    pc = INT_PHY_SW_STATE(unit, port);

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    sal_memset(ability, 0, sizeof(*ability));

    if ( PHY_INDEPENDENT_LANE_MODE(unit, port) ) {
        ability->speed_half_duplex  = SOC_PA_ABILITY_NONE;
        ability->speed_full_duplex  = SOC_PA_SPEED_1000MB;
        if ( PHY_FIBER_MODE(unit, port) )   {
           ability->speed_full_duplex |= SOC_PA_SPEED_2500MB;
           ability->speed_full_duplex |= SOC_PA_SPEED_100MB;
        } else {
           ability->speed_half_duplex  = SOC_PA_SPEED_10MB |
                                         SOC_PA_SPEED_100MB;
           ability->speed_full_duplex |= SOC_PA_SPEED_10MB   |
                                         SOC_PA_SPEED_100MB  |
                                         SOC_PA_SPEED_2500MB ;
        }
        ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
        ability->interface = SOC_PA_INTF_GMII | SOC_PA_INTF_SGMII;
        ability->medium    = SOC_PA_MEDIUM_FIBER;
        ability->loopback  = SOC_PA_LB_PHY;
        ability->flags     = SOC_PA_AUTONEG;
    } else {
        /* combo */
        ability->speed_half_duplex  = SOC_PA_ABILITY_NONE;
        ability->speed_full_duplex  = SOC_PA_SPEED_10GB ;
        
        ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
        ability->interface = SOC_PA_INTF_XGMII ;
        ability->medium    = SOC_PA_MEDIUM_FIBER;
        ability->loopback  = SOC_PA_LB_PHY;
        ability->flags     = 0 ;    /*SOC_PA_AUTONEG */
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(pc->unit,
                         "phy_wc_xgxs16g11_ability_local_get:unit=%d p=%d sp=%08x\n"),
              unit, pc->port, ability->speed_full_duplex));

    return (SOC_E_NONE);
}

/* end of xenia AN codes */

/*
 * Function:    
 *      phy_wcmod_an_set
 * Purpose:     
 *      Enable or disabled auto-negotiation on the specified port.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      an   - Boolean, if true, auto-negotiation is enabled 
 *              (and/or restarted). If false, autonegotiation is disabled.
 * Returns:     
 *      SOC_E_XXX
 */
int
phy_wcmod_an_set(int unit, soc_port_t port, int an)
{
    phy_ctrl_t         *pc;               
    WCMOD_DEV_DESC_t *pDesc;
    wcmod_st *ws;
    uint16 data73;
    uint16 dataup1;
    uint16 data_up1_mask;
    uint16 dataup3;
    uint16 data_up3_mask;
    int rv;  

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN    (_wcmod_phy_parameter_copy(pc, ws));

    if((ws->model_type == WCMOD_XN) || (ws->model_type == WCMOD_QS_A0) || (ws->model_type == WCMOD_QS)) {
        return phy_wcmod_xgxs16g1l_an_set(unit, port, an);
    }
    if( ws->model_type == WCMOD_WC_C0 ) {
        return phy_wcmod_an_set2(unit, port, an) ;
    }
    if (an) {
        SOC_IF_ERROR_RETURN (READ_WC40_DIGITAL3_UP1r(ws->unit, ws,&dataup1));    
        SOC_IF_ERROR_RETURN (READ_WC40_DIGITAL3_UP3r(ws->unit, ws,&dataup3));

        data_up1_mask = DIGITAL3_UP1_DATARATE_20GX4_MASK |
                        DIGITAL3_UP1_DATARATE_16GX4_MASK |
                        DIGITAL3_UP1_DATARATE_15GX4_MASK |
                        DIGITAL3_UP1_DATARATE_13GX4_MASK |
                        DIGITAL3_UP1_DATARATE_12P5GX4_MASK |
                        DIGITAL3_UP1_DATARATE_12GX4_MASK |
                        DIGITAL3_UP1_DATARATE_10GCX4_MASK |
                        DIGITAL3_UP1_DATARATE_10GX4_MASK |
                        DIGITAL3_UP1_DATARATE_6GX4_MASK |
                        DIGITAL3_UP1_DATARATE_5GX4_MASK |
                        DIGITAL3_UP1_DATARATE_2P5GX1_MASK;
    
        data_up3_mask = DIGITAL3_UP3_DATARATE_21GX4_MASK |
                        DIGITAL3_UP3_DATARATE_25P45GX4_MASK |
                        DIGITAL3_UP3_DATARATE_31P5G_MASK |
                        DIGITAL3_UP3_DATARATE_40G_MASK;

        /*read the 73 adv reg */
        SOC_IF_ERROR_RETURN (READ_WC40_AN_IEEE1BLK_AN_ADVERTISEMENT1r(ws->unit, ws, &data73));
        if (data73 & AN_IEEE1BLK_AN_ADVERTISEMENT1_TECHABILITY_MASK) {
            an = USE_CLAUSE_73_AN;
        } else if (dataup1 & data_up1_mask) {
            an = USE_CLAUSE_37_AN | USE_CLAUSE_37_AN_WITH_BAM;
        } else if (dataup3 & data_up3_mask) {
            an = USE_CLAUSE_37_AN | USE_CLAUSE_37_AN_WITH_BAM;
        } else {
            an = USE_CLAUSE_37_AN;
        }
        /* need to revisit the above */
    }

    SOC_IF_ERROR_RETURN(_phy_wcmod_pC0_cl72_enable(unit, port, an)) ; 

    ws->per_lane_control = an << (ws->this_lane * 8);
    SOC_IF_ERROR_RETURN
       (wcmod_tier1_selector("AUTONEG_CONTROL", ws, &rv));
    return SOC_E_NONE;
}

int
phy_wcmod_an_set2(int unit, soc_port_t port, int an)
{
    phy_ctrl_t         *pc;               
    WCMOD_DEV_DESC_t *pDesc;
    wcmod_st *ws;
    int rv;  
    uint16             an_enable;
    uint16             auto_det;
    WCMOD_TX_DRIVE_t   tx_drv[MAX_NUM_LANES];

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    
    an_enable = 0;
    auto_det  = 0;

    SOC_IF_ERROR_RETURN    (_wcmod_phy_parameter_copy(pc, ws));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(pc->unit,
                         "phy_wcmod_an_set2: u=%d p=%d an=%d lane=%d select=%x\n"),
              unit, port, an, ws->this_lane, ws->lane_select));
    
    if((ws->model_type == WCMOD_XN) || (ws->model_type == WCMOD_QS_A0) || (ws->model_type == WCMOD_QS)) {
        return phy_wcmod_xgxs16g1l_an_set(unit, port, an);
    }

    /* xxxx dual port mode write on one lane for now  */
    if(IS_DUAL_LANE_PORT(pc)) {  ws->dxgxs     =  0  ; }

    /*
    if( an ) {
           // clear force cl72 configuration 
        if (FORCE_CL72_ENABLED(pc) == TRUE) {
            SOC_IF_ERROR_RETURN
                (_phy_wc40_force_cl72_config(unit,port,FALSE));
            FORCE_CL72_ENABLED(pc) = FALSE;
        }  // need to revist 
    }
    */
    /* enable cl72 device in autoneg mode and disable it in forced mode */
    SOC_IF_ERROR_RETURN(_phy_wcmod_pC0_cl72_enable(unit, port, an)) ; 
    
    if (an) {
        /* disable the cl82 local fault. Needed to allow device link up at 40G KR4
         * when XMAC interface is still in XGMII mode. Once WC indicates 40G KR4 linkup,
         * the port update function will set the XMAC in XLGMII mode.
         */
        /* this register bit needs to reset by phy_wc40_link_get if set (in B0). */

        if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
           ws->lane_select = WCMOD_LANE_BCST  ;

        }
        SOC_IF_ERROR_RETURN
            (WC40_REG_MODIFY(unit, ws, 0x00, 0x842A, (1 << 5),(1 << 5)));
        if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
           ws->lane_select = WCMOD_LANE_0_0_0_1  ;
        }

        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_CL82_USERB0_RX_CONTROL_4r(unit, ws,
                         CL82_USERB0_RX_CONTROL_4_OUTIDLE_DEF_CYA_MASK,
                         CL82_USERB0_RX_CONTROL_4_OUTIDLE_DEF_CYA_MASK));   /* 0x842a:cya */
        WCMOD_AN_VALID_SET(pc);
        WCMOD_AN_RECORD_TIME(pc);

        /* we need to change the firmware mode to be default for AN */
        SOC_IF_ERROR_RETURN
            (_phy_wcmod_control_firmware_mode_set(unit, pc, SOC_PHY_FIRMWARE_DEFAULT));
        
        if (IS_DUAL_LANE_PORT(pc)) {
            /* do we need revB workaround ? */

            /* tx_out uses div33 clock. mixed speeds(10G/1G) on each lane */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_XGXSBLK6_XGXSX2CONTROL2r(unit, ws, 
                       XGXSBLK6_XGXSX2CONTROL2_TXCKOUT33_OVERRIDE_MASK, 
                       XGXSBLK6_XGXSX2CONTROL2_TXCKOUT33_OVERRIDE_MASK));
        }
        
        /* XXX: could be key */
        if(1) {
        /* configure the TX driver parameters for autoneg mode, use AN entry  */        
        SOC_IF_ERROR_RETURN
            (_phy_wcmod_tx_control_get(unit, pc, &tx_drv[0],TXDRV_AN_INX));
        /* no need to force pre-emphasis values */
        SOC_IF_ERROR_RETURN
            (_phy_wcmod_tx_control_amp_set(unit, port,&tx_drv[0]));
        }

        if (DEV_CFG_PTR(pc)->auto_medium) {
            auto_det = SERDESDIGITAL_CONTROL1000X1_AUTODET_EN_MASK; 
        }
        
        /* need to switch to a different VCO ? */

        /* do wee need to stop rxseq and txfifo, need to properly configure for 1G speeds
         * before doing AN ? revisit */
        /* do we need dual port bcst ? */

        /* do we need to hold tx/rx in reset ? revisit  */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DIGITAL5_MISC6r(unit, ws,
                  DIGITAL5_MISC6_RESET_RX_ASIC_MASK | DIGITAL5_MISC6_RESET_TX_ASIC_MASK,
                  DIGITAL5_MISC6_RESET_RX_ASIC_MASK | DIGITAL5_MISC6_RESET_TX_ASIC_MASK));

        /* If auto-neg is enabled, make sure not forcing any speed */
        /* do we need Jira# SDK-32387 ?? */
        /* control the speeds thru pma/pmd register */
        if (!SOC_IS_ARAD(unit)) {
            if (!IS_DUAL_LANE_PORT(pc))  {
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_SERDESDIGITAL_MISC2r(unit, ws,
                               SERDESDIGITAL_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN_MASK,
                               SERDESDIGITAL_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN_MASK));
            }
        }

        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DIGITAL4_MISC3r(unit, ws, 0,
                           DIGITAL4_MISC3_FORCE_SPEED_B5_MASK));

        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_SERDESDIGITAL_MISC1r(unit, ws, 0, 
                           SERDESDIGITAL_MISC1_FORCE_SPEED_MASK));
        
        /* revisit  */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DIGITAL4_MISC3r(unit, ws, DIGITAL4_MISC3_IND_40BITIF_MASK, 
            DIGITAL4_MISC3_IND_40BITIF_MASK | DIGITAL4_MISC3_FORCE_SPEED_B5_MASK)); 
        
        /* do wee need Jira# SDK-32387 ?? */
        /* control the speeds thru pma/pmd register */
        if (!SOC_IS_ARAD(unit)) {
            if (!IS_DUAL_LANE_PORT(pc))  {
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_SERDESDIGITAL_MISC2r(unit, ws,
                                   0,
                                   SERDESDIGITAL_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN_MASK));
            }
        }

        /* release the tx/rx reset ?? */

        /*XXXTTT_ORIG_CODE  not cond in orig code Don't release rx/tx until MAC is "ready" for XE and GE*/
        {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DIGITAL5_MISC6r(unit,ws, 0,
                                             DIGITAL5_MISC6_RESET_RX_ASIC_MASK |
                                             DIGITAL5_MISC6_RESET_TX_ASIC_MASK));
        }
 
        /* disable dual port bcst */

        /* Enable/Disable auto detect */
        SOC_IF_ERROR_RETURN
             (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(unit, ws, auto_det,
                                 SERDESDIGITAL_CONTROL1000X1_AUTODET_EN_MASK));

        if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
           ws->lane_select = WCMOD_LANE_BCST  ;
        }
        
        /* no force speed */
        SOC_IF_ERROR_RETURN
           (MODIFY_WC40_SERDESDIGITAL_MISC1r
            (unit, ws, 0, SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK));
        
        if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
           ws->lane_select = WCMOD_LANE_0_0_0_1  ;
        }

        if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
            /* do wee to clear 2wire XAUI configuration ?? */
            
            /* set default clock comp clear the configuration for 20X XE combo mode ?? */
            /* don't change  if clock comp is enabled by user */

            /* only in combo mode, reset the sequence. In independent mode,
             * resetting sequence affects all lanes
             */
            
            /* disable PLL      */
            ws->per_lane_control = WCMOD_DISABLE;
            SOC_IF_ERROR_RETURN
                (wcmod_tier1_selector("PLL_SEQUENCER_CONTROL", ws, &rv));

            /* turn off RX sequencer    */
            ws->per_lane_control = WCMOD_DISABLE;
            SOC_IF_ERROR_RETURN
                (wcmod_tier1_selector("RX_SEQ_CONTROL", ws, &rv));


            /* for soft kr2, disable parallel detect */
            /* Enable 1000X parallel detect */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X2r(unit, ws,  
                      SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK, 
                      SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK));

            /* enable 10G parallel detect */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_XGXSBLK5_PARDET10GCONTROLr(unit, ws,
                          XGXSBLK5_PARDET10GCONTROL_PARDET10G_EN_MASK, 
                          XGXSBLK5_PARDET10GCONTROL_PARDET10G_EN_MASK));
        }
        
        if (DEV_CFG_PTR(pc)->cl37an) {
            if (DEV_CFG_PTR(pc)->cl37an == WCMOD_CL37_WO_CL37BAM) {
                an_enable |= USE_CLAUSE_37_AN;
            } else {
                an_enable |= USE_CLAUSE_37_AN_WITH_BAM;
            }
        }
        if (DEV_CFG_PTR(pc)->cl73an) {
                /* CL73 BAM setup, revisit BAMNP_AFTER_BP bit, any conflict
                 * with EEE page?
                 */
                /* not to enable CL73 BAM in independent channel mode:
                 * has problem with 8073 phy
                 */

            /* XXX temp  */
            if(1) {
                if (DEV_CFG_PTR(pc)->cl73an == WCMOD_CL73_WO_CL73BAM) {
                    /* need to enable these two bits even BAM is disabled to interop
                     * with other BRCM devices properly. 
                     */
                    an_enable |= USE_CLAUSE_73_AN ;
                } else {
                    an_enable |= USE_CLAUSE_73_AN_WITH_BAM ;
                }
            }
            /* an_enable |= USE_CLAUSE_73_AN ;   */
        }
        if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) { 
            /* restart the sequence */
            ws->per_lane_control = 1 ;
            SOC_IF_ERROR_RETURN
                (wcmod_tier1_selector("PLL_SEQUENCER_CONTROL", ws, &rv));
            /* turn on RX sequencer    */
            ws->per_lane_control = WCMOD_ENABLE;
            SOC_IF_ERROR_RETURN
                (wcmod_tier1_selector("RX_SEQ_CONTROL", ws, &rv));
                
            _phy_wcmod_control_vco_disturbed_set(ws->unit, ws->port);
        }
    } else {
        /* an = 0 */
        if (IS_DUAL_LANE_PORT(pc)) {
            /* revB workaround removed ?? */

            /* 0x8141 txckout33 */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_XGXSBLK6_XGXSX2CONTROL2r(unit, ws, 
                            0, XGXSBLK6_XGXSX2CONTROL2_TXCKOUT33_OVERRIDE_MASK));
            
        }
        /* Disable auto detect */
        SOC_IF_ERROR_RETURN
             (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(unit, ws, auto_det,
                                 SERDESDIGITAL_CONTROL1000X1_AUTODET_EN_MASK));

        if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
            /* disable PLL      */
            ws->per_lane_control = WCMOD_DISABLE ;
            SOC_IF_ERROR_RETURN
                (wcmod_tier1_selector("PLL_SEQUENCER_CONTROL", ws, &rv));
            /* turn off RX sequencer    */
            ws->per_lane_control = WCMOD_DISABLE;
            SOC_IF_ERROR_RETURN
                (wcmod_tier1_selector("RX_SEQ_CONTROL", ws, &rv));

            /* disable 1000X parallel detect */
            SOC_IF_ERROR_RETURN
                  (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X2r(unit, ws, 
                  0,
                  SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK));
                                                                                
            /* disable 10G parallel detect */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_XGXSBLK5_PARDET10GCONTROLr(unit, ws, 
                      0,
                      XGXSBLK5_PARDET10GCONTROL_PARDET10G_EN_MASK));
        }

        if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
            /* restart the sequence */
            ws->per_lane_control = WCMOD_ENABLE ;
            SOC_IF_ERROR_RETURN
                (wcmod_tier1_selector("PLL_SEQUENCER_CONTROL", ws, &rv));
            /* turn on RX sequencer    */
            ws->per_lane_control = WCMOD_ENABLE;
            SOC_IF_ERROR_RETURN
                (wcmod_tier1_selector("RX_SEQ_CONTROL", ws, &rv));
                
            _phy_wcmod_control_vco_disturbed_set(ws->unit, ws->port);
        }
    }
    pc->fiber.autoneg_enable = an;
    ws->per_lane_control = an_enable << (ws->this_lane * 8);
    SOC_IF_ERROR_RETURN
       (wcmod_tier1_selector("AUTONEG_CONTROL", ws, &rv));

    if(IS_DUAL_LANE_PORT(pc)) {  ws->dxgxs     =  ws->dual_type  ; }
    return SOC_E_NONE;

}

/*
 * Function:    
 *      phy_wcmod_an_get
 * Purpose:     
 *      Get the current auto-negotiation status (enabled/busy)
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      an   - (OUT) if true, auto-negotiation is enabled.
 *      an_done - (OUT) if true, auto-negotiation is complete. This
 *              value is undefined if an == false.
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
phy_wcmod_an_get(int unit, soc_port_t port, int *an, int *an_done)
{
    phy_ctrl_t *pc;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t *pDesc;
    int rv;  

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    if((ws->model_type == WCMOD_XN) || (ws->model_type == WCMOD_QS_A0) || (ws->model_type == WCMOD_QS)) {
        return phy_wcmod_xgxs16g1l_an_get(unit, port, an, an_done) ;
    }
    ws->diag_type = WCMOD_DIAG_ANEG;

    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
    /* the return definitions modified.  Need to double check */
    *an      = ((ws->accData) & AUTONEG_ENABLE_MASK) ? TRUE : FALSE;
    *an_done = ((ws->accData) & AUTONEG_DONE_MASK) ? TRUE : FALSE ;
    return SOC_E_NONE;
}

STATIC int
_phy_wcmod_c73_adv_local_set(int unit, soc_port_t port,
                            soc_port_ability_t *ability)
{
    uint16            an_adv;
    uint16            pause;
#ifdef WC_EEE_SUPPORT
    uint16            mask16;
    uint16            data16;
#endif
    phy_ctrl_t *pc;
    WCMOD_DEV_DESC_t           *pDesc;
    wcmod_st   *ws;      /*wcmod structure */

   
    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws    =  (wcmod_st *)( pDesc + 1);

  SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));
#if 0

    if (WC40_CL73_SOFT_KR2(pc)) {
        if (ability->speed_full_duplex & SOC_PA_SPEED_20GB) {
            DEV_CTRL_PTR(pc)->soft_kr2.ld_kr2 = 1;
        } else {
            DEV_CTRL_PTR(pc)->soft_kr2.ld_kr2 = 0;
        }
    }
#endif
    an_adv = (ability->speed_full_duplex & SOC_PA_SPEED_1000MB) ? 
              CL73_AN_ADV_TECH_1G_KX : 0;

    if (IND_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        an_adv |= (ability->speed_full_duplex & SOC_PA_SPEED_10GB) ? 
               CL73_AN_ADV_TECH_10G_KR : 0;
    }

    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        an_adv |= (ability->speed_full_duplex & SOC_PA_SPEED_10GB) ? 
              CL73_AN_ADV_TECH_10G_KX4 : 0;
    }    

    an_adv |= (ability->speed_full_duplex & SOC_PA_SPEED_40GB) ?
          CL73_AN_ADV_TECH_40G_KR4 : 0;
    if (DEV_CFG_PTR(pc)->line_intf & WCMOD_IF_CR4) {
        an_adv |= (ability->speed_full_duplex & SOC_PA_SPEED_40GB) ?
              CL73_AN_ADV_TECH_40G_CR4 : 0;
    }

    an_adv |= (ability->speed_full_duplex & SOC_PA_SPEED_100GB) ?
          CL73_AN_ADV_TECH_100G_CR10 : 0;

    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_AN_IEEE1BLK_AN_ADVERTISEMENT1r(unit, ws, an_adv,
                                     CL73_AN_ADV_TECH_SPEEDS_MASK));

#ifdef WC_EEE_SUPPORT
    /* EEE advertisement 0x3c: 
     * 06 RW EEE_10G_KR 1 = EEE is supported for 10GBASE-KR.
     *                  0 = EEE is not supported for 10GBASE-KR. 
     * 05 RW EEE_10G_KX4 1 = EEE is supported for 10GBASE-KX4.
     *                   0 = EEE is not supported for 10GBASE-KX4. 
     * 04 RW EEE_1G_KX 1 = EEE is supported for 1000BASE-KX.
     *                 0 = EEE is not supported for 1000BASE-KX.
     * XXX assume supporting all speeds. use PA_EEE 
     */
    mask16 = AN_IEEE3BLK_EEE_ADV_EEE_10G_KR_MASK |
             AN_IEEE3BLK_EEE_ADV_EEE_10G_KX4_MASK |
             AN_IEEE3BLK_EEE_ADV_EEE_1G_KX_MASK;
    data16 = (ability->eee & SOC_PA_EEE_10GB_KX) ? 
              AN_IEEE3BLK_EEE_ADV_EEE_1G_KX_MASK : 0;
    data16 |= (ability->eee & SOC_PA_EEE_10GB_KX4) ? 
              AN_IEEE3BLK_EEE_ADV_EEE_10G_KX4_MASK:0;
    data16 |= (ability->eee & SOC_PA_EEE_10GB_KR) ? 
              AN_IEEE3BLK_EEE_ADV_EEE_10G_KR_MASK: 0;
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_AN_IEEE3BLK_EEE_ADVr(unit, pc,data16, mask16));
#endif

    /* CL73 UD_code_field */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_CL73_USERB0_CL73_BAMCTRL3r(unit,ws,1,
                CL73_USERB0_CL73_BAMCTRL3_UD_CODE_FIELD_MASK));

    switch (ability->pause & (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX)) {
    case SOC_PA_PAUSE_TX:
        pause = CL73_AN_ADV_ASYM_PAUSE;
        break;
    case SOC_PA_PAUSE_RX:
        pause = CL73_AN_ADV_PAUSE | CL73_AN_ADV_ASYM_PAUSE;
        break;
    case SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX:
        pause = CL73_AN_ADV_PAUSE;
        break;
    default:
        pause = 0;
        break;
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_AN_IEEE1BLK_AN_ADVERTISEMENT0r(unit, ws,  pause,
                                     (CL73_AN_ADV_PAUSE |
                                      CL73_AN_ADV_ASYM_PAUSE)));
    
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(pc->unit,
                         "_phy_wcmod_c73_adv_local_set: u=%d p=%d pause=%08x speeds=%04x,adv=0x%x\n"),
              unit, port, pause, ability->speed_full_duplex, an_adv));

    return SOC_E_NONE;
}


STATIC int
_phy_wcmod_c73_adv_local_get(int unit, soc_port_t port,
                            soc_port_ability_t *ability)
{
    uint16            an_adv;
    soc_port_mode_t   speeds,pause;
    phy_ctrl_t *pc;
    WCMOD_DEV_DESC_t           *pDesc;
    wcmod_st   *ws;      /*wcmod structure */

   
    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws    =  (wcmod_st *)( pDesc + 1);
    SOC_IF_ERROR_RETURN    (_wcmod_phy_parameter_copy(pc, ws));

    SOC_IF_ERROR_RETURN
        (READ_WC40_AN_IEEE1BLK_AN_ADVERTISEMENT1r(unit, ws,  &an_adv));

    speeds = (an_adv & CL73_AN_ADV_TECH_1G_KX) ? SOC_PA_SPEED_1000MB : 0;
    speeds |= (an_adv & CL73_AN_ADV_TECH_10G_KX4) ? SOC_PA_SPEED_10GB : 0;
    speeds |= (an_adv & CL73_AN_ADV_TECH_10G_KR) ? SOC_PA_SPEED_10GB : 0;
    speeds |= (an_adv & CL73_AN_ADV_TECH_40G_KR4) ? SOC_PA_SPEED_40GB : 0;
    speeds |= (an_adv & CL73_AN_ADV_TECH_40G_CR4) ? SOC_PA_SPEED_40GB : 0;
#if 0
    if (WC40_CL73_SOFT_KR2(pc)) {
        speeds |= DEV_CTRL_PTR(pc)->soft_kr2.ld_kr2? SOC_PA_SPEED_20GB:0;
    }
#endif
    ability->speed_full_duplex |= speeds;
#ifdef WC_EEE_SUPPORT
    SOC_IF_ERROR_RETURN
        (READ_WC40_AN_IEEE3BLK_EEE_ADVr(unit, pc, 0x00, &an_adv));
    ability->eee = (an_adv & AN_IEEE3BLK_EEE_ADV_EEE_10G_KR_MASK)?
                   SOC_PA_EEE_10GB_KR:0;
    ability->eee |= (an_adv & AN_IEEE3BLK_EEE_ADV_EEE_10G_KX4_MASK)?
                    SOC_PA_EEE_10GB_KX4:0;
    ability->eee |= (an_adv & AN_IEEE3BLK_EEE_ADV_EEE_1G_KX_MASK)?
                    SOC_PA_EEE_10GB_KX:0;
#endif

    /* advert register 0x12, bit 15 FEC requested,bit 14 FEC ability */
    /* SOC_IF_ERROR_RETURN
     *    (READ_WC40_AN_IEEE1BLK_AN_ADVERTISEMENT2r(unit, ws, &an_adv));
     */
    /* (an_adv & AN_IEEE1BLK_AN_ADVERTISEMENT2_FEC_REQUESTED_MASK) */

    SOC_IF_ERROR_RETURN
        (READ_WC40_AN_IEEE1BLK_AN_ADVERTISEMENT0r(unit, ws, &an_adv));

    switch (an_adv & (CL73_AN_ADV_PAUSE | CL73_AN_ADV_ASYM_PAUSE)) {
        case CL73_AN_ADV_PAUSE:
            pause = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
            break;
        case CL73_AN_ADV_ASYM_PAUSE:
            pause = SOC_PA_PAUSE_TX;
            break;
        case CL73_AN_ADV_PAUSE | CL73_AN_ADV_ASYM_PAUSE:
            pause = SOC_PA_PAUSE_RX;
            break;
        default:
            pause = 0;
            break;
    }
    ability->pause = pause;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(pc->unit,
                         "_phy_wcmod_c73_adv_local_get: u=%d p=%d pause=%08x speeds=%04x\n"),
              unit, port, pause, speeds));

    return SOC_E_NONE;
}




/*
 * Function:
 *      phy_wcmod_ability_advert_set
 * Purpose:
 *      Set the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is set only for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
phy_wcmod_ability_advert_set(int unit, soc_port_t port,
                       soc_port_ability_t *ability)
{
    uint16           an_adv;
    uint16           an_sp_20g;
    soc_port_mode_t  mode;
    phy_ctrl_t      *pc;
    uint16 mask16;
    uint16 data16;
    WCMOD_DEV_DESC_t *pDesc;
    wcmod_st *ws;
    uint16 up3;

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    if((ws->model_type) == WCMOD_XN) {
        return phy_wcmod_xgxs16g1l_ability_advert_set(unit, port, ability) ;
    }

    /* If the phy medium is copper, only set MII speed and advertised duplex. */
    if (!DEV_CFG_PTR(pc)->fiber_pref) {
        an_adv = 0;
        if (ability->speed_full_duplex & SOC_PA_SPEED_1000MB) {
            an_adv = MII_CTRL_SS_1000;
        } else if (ability->speed_full_duplex & SOC_PA_SPEED_100MB) {
            an_adv = MII_CTRL_SS_100;
        } else if (ability->speed_full_duplex & SOC_PA_SPEED_10MB) {
            an_adv = MII_CTRL_SS_10;
        }

        an_adv |= ability->speed_full_duplex ? (MII_CTRL_FD | MII_CTRL_AE) : 0;
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_COMBO_IEEE0_MIICNTLr(unit, ws, an_adv,
                MII_CTRL_AE | MII_CTRL_FD | MII_CTRL_SS_LSB | MII_CTRL_SS_MSB));

        return SOC_E_NONE;
    }

    /*
     * Set advertised duplex (only FD supported).
     */
    an_adv = ability->speed_full_duplex? MII_ANA_C37_FD : 0;

    /*
     * Set advertised pause bits in link code word.
     */
    switch (ability->pause & (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX)) {
    case SOC_PA_PAUSE_TX:
        an_adv |= MII_ANA_C37_ASYM_PAUSE;
        break;
    case SOC_PA_PAUSE_RX:
        an_adv |= MII_ANA_C37_PAUSE | MII_ANA_C37_ASYM_PAUSE;
        break;
    case SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX:
        an_adv |= MII_ANA_C37_PAUSE;
        break;
    }
    /* Update less than 1G capability */ 
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_COMBO_IEEE0_AUTONEGADVr(unit, ws, an_adv));

    mode = ability->speed_full_duplex;
    an_adv = 0;
    an_sp_20g = 0;
    an_adv |= (mode & SOC_PA_SPEED_2500MB) ? 
               DIGITAL3_UP1_DATARATE_2P5GX1_MASK : 0;

    if (IND_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_DIGITAL3_UP1r(unit, ws, an_adv));

        SOC_IF_ERROR_RETURN
            (WRITE_WC40_DIGITAL3_UP3r(unit, ws, 
                          DIGITAL3_UP3_CL72_MASK | DIGITAL3_UP3_LAST_MASK)); 

        if (DEV_CFG_PTR(pc)->cl73an) { 
            SOC_IF_ERROR_RETURN
                (_phy_wcmod_c73_adv_local_set(unit, port, ability));
        }

        /*next need to check FEC ability */
        mask16 = AN_IEEE1BLK_AN_ADVERTISEMENT2_FEC_REQUESTED_MASK;
        data16 = 0;
        if (ability->fec == SOC_PA_FEC_CL74) {
            data16 = mask16;
            up3 = DIGITAL3_UP3_FEC_MASK;
        } else {
            data16 = 1 << AN_IEEE1BLK_AN_ADVERTISEMENT2_FEC_REQUESTED_SHIFT;
            up3 = 0;
        }
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_AN_IEEE1BLK_AN_ADVERTISEMENT2r(unit, ws, data16, mask16));
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DIGITAL3_UP3r(unit, ws, up3, DIGITAL3_UP3_FEC_MASK));

        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(pc->unit,
                             "phy_wcmod_ability_advert_set: u=%d p=%d pause=%08x OVER1G_UP1 %04x\n"),
                  unit, port, ability->pause, an_adv));
        return SOC_E_NONE;
    }

    /* high speeds, 40G,30G and 20G, 20G/scrambler */ 
    an_adv |= (mode & SOC_PA_SPEED_5000MB) ? 
                DIGITAL3_UP1_DATARATE_5GX4_MASK : 0;
    an_adv |= (mode & SOC_PA_SPEED_6000MB) ? 
                DIGITAL3_UP1_DATARATE_6GX4_MASK : 0;
    an_adv |= (mode & SOC_PA_SPEED_12GB) ? 
                DIGITAL3_UP1_DATARATE_12GX4_MASK : 0;
    an_adv |= (mode & SOC_PA_SPEED_12P5GB) ? 
                DIGITAL3_UP1_DATARATE_12P5GX4_MASK : 0;
    an_adv |= (mode & SOC_PA_SPEED_13GB) ? 
                    DIGITAL3_UP1_DATARATE_13GX4_MASK : 0;
    an_adv |= (mode & SOC_PA_SPEED_15GB) ? 
                DIGITAL3_UP1_DATARATE_15GX4_MASK : 0;
    an_adv |= (mode & SOC_PA_SPEED_16GB) ? 
                DIGITAL3_UP1_DATARATE_16GX4_MASK : 0;
    an_adv |= (mode & SOC_PA_SPEED_20GB) ? 
              DIGITAL3_UP1_DATARATE_20GX4_MASK : 0;
    /* advertise 20G with scrambler */
    if (mode & SOC_PA_SPEED_20GB) {
        data16 = DIGITAL6_UP4_DATARATE_20G_MASK;
    } else {
        data16 = 0;
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DIGITAL6_UP4r(unit, ws, 
                 data16,
                 DIGITAL6_UP4_DATARATE_20G_MASK));
    
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DIGITAL6_UP4r(unit, ws, 
                 DIGITAL6_UP4_LAST_MASK,
                 DIGITAL6_UP4_LAST_MASK));

    an_sp_20g |= (mode & SOC_PA_SPEED_21GB) ? 
                  DIGITAL3_UP3_DATARATE_21GX4_MASK : 0;
    an_sp_20g |= (mode & SOC_PA_SPEED_25GB) ? 
                  DIGITAL3_UP3_DATARATE_25P45GX4_MASK : 0;
    an_sp_20g |= (mode & SOC_PA_SPEED_30GB) ? 
                  DIGITAL3_UP3_DATARATE_31P5G_MASK : 0;

    if (DEV_CFG_PTR(pc)->hg_mode) {
        an_sp_20g |= (mode & SOC_PA_SPEED_40GB) ? 
                  DIGITAL3_UP3_DATARATE_40G_MASK : 0;
    }

    if (mode & SOC_PA_SPEED_10GB) {
        if (DEV_CFG_PTR(pc)->hg_mode) {
            /* For Draco and Hercules, use pre-CX4 signalling */
            an_adv |= DIGITAL3_UP1_DATARATE_10GX4_MASK; 
            if (DEV_CFG_PTR(pc)->cx4_10g) {
                /* Also include 10G CX4 signalling by default */
                an_adv |= DIGITAL3_UP1_DATARATE_10GCX4_MASK; 
            }
        } else {
            an_adv |= DIGITAL3_UP1_DATARATE_10GCX4_MASK; 
        }
    }
    
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_DIGITAL3_UP1r(unit, ws, an_adv));

    /* CL72, FEC and Higig2, check Higig2 cap on this port before adv 
     * UP3 last page bit DIGITAL3_UP3_LAST_MASK needs to be set to work with HC/HL.
     */
    mask16 = OVER1G_UP3_20GPLUS_MASK;
    data16 = an_sp_20g;
    mask16 |= DIGITAL3_UP3_CL72_MASK | DIGITAL3_UP3_FEC_MASK |
             DIGITAL3_UP3_HIGIG2_MASK;

    /* 20G with CL72 advertised in WC B0 doesn't work. But cl72 should not
     * be advertised below 21G anyway
     */ 
    if (!(DEV_CFG_PTR(pc)->hg_mode &&
        (!(mode & (SOC_PA_SPEED_40GB | SOC_PA_SPEED_21GB |
                 SOC_PA_SPEED_25GB | SOC_PA_SPEED_30GB)))) ) {
        data16 |= DIGITAL3_UP3_CL72_MASK;
    }
#if 0 
    data16 |= DIGITAL3_UP3_FEC_MASK;
#endif

    /* XXX revisit: no autoneg in dxgxs mode */
    
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DIGITAL3_UP3r(unit, ws, data16, mask16));

    /* preemphasis attributes */
    /*
     * mask16 = DIGITAL3_UP2_PREEMPHASIS_MASK | DIGITAL3_UP2_IDRIVER_MASK |
     *          DIGITAL3_UP2_IPREDRIVER_MASK | DIGITAL3_UP2_VALID_MASK;
     * data16 = (DEV_CFG_PTR(pc)->preemph << DIGITAL3_UP2_PREEMPHASIS_SHIFT) |
     *          (DEV_CFG_PTR(pc)->idriver << DIGITAL3_UP2_IDRIVER_SHIFT) |
     *          (DEV_CFG_PTR(pc)->pdriver << DIGITAL3_UP2_IPREDRIVER_SHIFT);
     * data16 |= DIGITAL3_UP2_VALID_MASK;
     * SOC_IF_ERROR_RETURN
     *     (MODIFY_WC40_DIGITAL3_UP2r(unit, pc, 0x00, data16,mask16));
     */

    if (DEV_CFG_PTR(pc)->cl73an) { 
        SOC_IF_ERROR_RETURN
            (_phy_wcmod_c73_adv_local_set(unit, port, ability));
    }

    /*next need to check FEC ability */
    mask16 = AN_IEEE1BLK_AN_ADVERTISEMENT2_FEC_REQUESTED_MASK;
    data16 = 0;
    if (ability->fec == SOC_PA_FEC_CL74) {
        data16 = mask16;
        up3 = DIGITAL3_UP3_FEC_MASK;
    } else {
        data16 = 1 << AN_IEEE1BLK_AN_ADVERTISEMENT2_FEC_REQUESTED_SHIFT;
        up3 = 0;
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_AN_IEEE1BLK_AN_ADVERTISEMENT2r(unit, ws, data16, mask16));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DIGITAL3_UP3r(unit, ws, up3, DIGITAL3_UP3_FEC_MASK));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(pc->unit,
                         "phy_wcmod_ability_advert_set: u=%d p=%d pause=%08x OVER1G_UP1 %04x\n"),
              unit, port, ability->pause, an_adv));

    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_wcmod_ability_advert_get
 * Purpose:
 *      Get the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
phy_wcmod_ability_advert_get(int unit, soc_port_t port,
                           soc_port_ability_t *ability)
{
    uint16           an_adv;
    uint16           up4;
    uint16           up3;
    uint16           up2;
    soc_port_mode_t  mode;
    phy_ctrl_t      *pc;
    WCMOD_DEV_DESC_t *pDesc;
    wcmod_st *ws;
    soc_port_mode_t  fec;

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    if((ws->model_type) == WCMOD_XN) {
        return phy_wcmod_xgxs16g1l_ability_advert_get(unit, port, ability) ;
    }

    WCMOD_MEM_SET(ability, 0, sizeof(*ability));

    if (!DEV_CFG_PTR(pc)->fiber_pref) {
        SOC_IF_ERROR_RETURN
          (READ_WC40_COMBO_IEEE0_MIICNTLr(unit, ws, &an_adv));
        mode = 0;
        mode = (an_adv & MII_CTRL_SS_MSB) ? SOC_PA_SPEED_1000MB :
               (an_adv & MII_CTRL_SS_LSB) ? SOC_PA_SPEED_100MB :
                SOC_PA_SPEED_10MB;
        ability->speed_full_duplex = mode;

        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN
        (READ_WC40_DIGITAL3_UP1r(unit, ws, &an_adv));

    SOC_IF_ERROR_RETURN
        (READ_WC40_DIGITAL3_UP2r(unit, ws, &up2));

    SOC_IF_ERROR_RETURN
        (READ_WC40_DIGITAL3_UP3r(unit, ws, &up3));

    SOC_IF_ERROR_RETURN
        (READ_WC40_DIGITAL6_UP4r(unit, ws, &up4));

    /* preemphasis settings */
    /* preemphasis (up2 & DIGITAL3_UP2_PREEMPHASIS_MASK) >>
     *              DIGITAL3_UP2_PREEMPHASIS_SHIFT
     * idriver     (up2 & DIGITAL3_UP2_IDRIVER_MASK) >>
     *              DIGITAL3_UP2_IDRIVER_SHIFT
     * ipredriver  (up2 & DIGITAL3_UP2_IPREDRIVER_MASK) >>
     *              DIGITAL3_UP2_IPREDRIVER_SHIFT
     */
    /* CL72,FEC and Higig2 
     * CL72 cap - (up3 & DIGITAL3_UP3_CL72_MASK)
     * FEC cap  - (up3 & DIGITAL3_UP3_FEC_MASK)
     * Higig2   - (up3 & DIGITAL3_UP3_HIGIG2_MASK)
     */
    ability->encap = (up3 & DIGITAL3_UP3_HIGIG2_MASK)?
                      SOC_PA_ENCAP_HIGIG2: 0;
    
    mode = 0;
    mode |= (up3 & DIGITAL3_UP3_DATARATE_40G_MASK) ?
              SOC_PA_SPEED_40GB : 0;
    mode |= (up3 & DIGITAL3_UP3_DATARATE_31P5G_MASK) ?
              SOC_PA_SPEED_30GB : 0;
    mode |= (up3 & DIGITAL3_UP3_DATARATE_25P45GX4_MASK) ?
              SOC_PA_SPEED_25GB : 0;
    mode |= (up3 & DIGITAL3_UP3_DATARATE_21GX4_MASK) ?
              SOC_PA_SPEED_21GB : 0;

    mode |= (up4 & DIGITAL6_UP4_DATARATE_20G_MASK) ? SOC_PA_SPEED_20GB : 0;
    mode |= (an_adv & DIGITAL3_UP1_DATARATE_20GX4_MASK) ? SOC_PA_SPEED_20GB : 0;
    mode |= (an_adv & DIGITAL3_UP1_DATARATE_16GX4_MASK) ? SOC_PA_SPEED_16GB : 0;
    mode |= (an_adv & DIGITAL3_UP1_DATARATE_15GX4_MASK) ? SOC_PA_SPEED_15GB : 0;
    mode |= (an_adv & DIGITAL3_UP1_DATARATE_13GX4_MASK) ? SOC_PA_SPEED_13GB : 0;
    mode |= (an_adv & DIGITAL3_UP1_DATARATE_12P5GX4_MASK) ? 
            SOC_PA_SPEED_12P5GB : 0;
    mode |= (an_adv & DIGITAL3_UP1_DATARATE_12GX4_MASK) ? SOC_PA_SPEED_12GB : 0;
    mode |= (an_adv & DIGITAL3_UP1_DATARATE_10GCX4_MASK) ? SOC_PA_SPEED_10GB: 0;
    mode |= (an_adv & DIGITAL3_UP1_DATARATE_10GX4_MASK) ? SOC_PA_SPEED_10GB : 0;
    mode |= (an_adv & DIGITAL3_UP1_DATARATE_6GX4_MASK)? SOC_PA_SPEED_6000MB : 0;
    mode |= (an_adv & DIGITAL3_UP1_DATARATE_5GX4_MASK)? SOC_PA_SPEED_5000MB : 0;
    mode |= (an_adv & DIGITAL3_UP1_DATARATE_2P5GX1_MASK) ? 
                    SOC_PA_SPEED_2500MB : 0;

    SOC_IF_ERROR_RETURN
        (READ_WC40_COMBO_IEEE0_AUTONEGADVr(unit, ws, &an_adv));
      
    mode |= (an_adv & MII_ANA_C37_FD) ? SOC_PA_SPEED_1000MB : 0;
    ability->speed_full_duplex = mode;

    mode = 0;
    switch (an_adv & (MII_ANA_C37_PAUSE | MII_ANA_C37_ASYM_PAUSE)) {
        case MII_ANA_C37_PAUSE:
            mode = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
            break;
        case MII_ANA_C37_ASYM_PAUSE:
            mode = SOC_PA_PAUSE_TX;
            break;
        case MII_ANA_C37_PAUSE | MII_ANA_C37_ASYM_PAUSE:
            mode = SOC_PA_PAUSE_RX;
            break;
    }
    ability->pause = mode;

    /* check for clause73 */
    if (DEV_CFG_PTR(pc)->cl73an) {
        SOC_IF_ERROR_RETURN
            (_phy_wcmod_c73_adv_local_get(unit, port, ability));
    }

    /* Check FEC base on AN and CL37BAM*/
    SOC_IF_ERROR_RETURN
        (READ_WC40_AN_IEEE1BLK_AN_ADVERTISEMENT2r(unit, ws, &an_adv));
    if ((an_adv & AN_IEEE1BLK_AN_ADVERTISEMENT2_FEC_REQUESTED_MASK) ==
                    AN_IEEE1BLK_AN_ADVERTISEMENT2_FEC_REQUESTED_MASK) {
        fec = SOC_PA_FEC_CL74;
    } else if (an_adv & (1<<AN_IEEE1BLK_AN_ADVERTISEMENT2_FEC_REQUESTED_SHIFT)) {
        fec = SOC_PA_FEC_NONE;
    } else {
        fec = 0;
    }

    fec |= (up3 & DIGITAL3_UP3_FEC_MASK) ? SOC_PA_FEC_CL74 : SOC_PA_FEC_NONE;

    if (fec & SOC_PA_FEC_CL74) {
        ability->fec = SOC_PA_FEC_CL74;
    } else {
        ability->fec = SOC_PA_FEC_NONE;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(pc->unit,
                         "phy_wcmod_ability_advert_get:unit=%d p=%d pause=%08x sp=%08x max_spd=%0d\n"),
              unit, port, ability->pause, ability->speed_full_duplex, pc->speed_max));


    return SOC_E_NONE;
}

STATIC int
_phy_wcmod_c73_adv_remote_get(int unit, soc_port_t port,
                             soc_port_ability_t *ability)
{
    uint16            an_adv;
    soc_port_mode_t   mode;
    phy_ctrl_t *pc;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

     SOC_IF_ERROR_RETURN
        (READ_WC40_AN_IEEE1BLK_AN_LP_BASEPAGEABILITY1r(unit, ws, &an_adv));

    mode =  (an_adv & CL73_AN_ADV_TECH_1G_KX) ? SOC_PA_SPEED_1000MB : 0;
    mode |= (an_adv & CL73_AN_ADV_TECH_10G_KX4) ? SOC_PA_SPEED_10GB : 0;
    mode |= (an_adv & CL73_AN_ADV_TECH_10G_KR) ? SOC_PA_SPEED_10GB : 0;
    mode |= (an_adv & CL73_AN_ADV_TECH_40G_KR4) ? SOC_PA_SPEED_40GB : 0;
    mode |= (an_adv & CL73_AN_ADV_TECH_40G_CR4) ? SOC_PA_SPEED_40GB : 0;

    SOC_IF_ERROR_RETURN
        (READ_WC40_AN_IEEE1BLK_AN_LP_BASEPAGEABILITY2r(unit, ws, &an_adv));
    mode |= (an_adv & CL73_AN_ADV_TECH_20G_KR2) ? SOC_PA_SPEED_20GB : 0;

    ability->speed_full_duplex |= mode;

    /* advert register 0x12, bit 15 FEC requested,bit 14 FEC ability */
    /* SOC_IF_ERROR_RETURN
     *   (READ_WC40_AN_IEEE1BLK_AN_LP_BASEPAGEABILITY2r(unit, pc, 0x00, &an_adv));
     */
    /* (an_adv & AN_IEEE1BLK_AN_ADVERTISEMENT2_FEC_REQUESTED_MASK) */


    SOC_IF_ERROR_RETURN
        (READ_WC40_AN_IEEE1BLK_AN_LP_BASEPAGEABILITY0r(unit, ws, &an_adv));

    mode = 0;
    switch (an_adv & (CL73_AN_ADV_PAUSE | CL73_AN_ADV_ASYM_PAUSE)) {
        case CL73_AN_ADV_PAUSE:
            mode = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
            break;
        case CL73_AN_ADV_ASYM_PAUSE:
            mode = SOC_PA_PAUSE_TX;
            break;
        case CL73_AN_ADV_PAUSE | CL73_AN_ADV_ASYM_PAUSE:
            mode = SOC_PA_PAUSE_RX;
            break;
    }
    ability->pause = mode;

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_wcmod_ability_remote_get
 * Purpose:
 *      Get the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
phy_wcmod_ability_remote_get(int unit, soc_port_t port,
                        soc_port_ability_t *ability)
{
    uint16           an_adv;
    uint16           up4;
    uint16           up3;
    uint16           up2;
    uint16           data16;
    uint16           mii_ctrl;
    int              an_enable, an_done;
    int              link_1000x = FALSE;
    int              link_combo = FALSE;
    soc_port_mode_t  mode;
    phy_ctrl_t *pc;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    soc_port_mode_t  fec;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);


    if((ws->model_type) == WCMOD_XN) {
        return phy_wcmod_xgxs16g1l_ability_remote_get(unit, port, ability) ;
    }

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));    
    SOC_IF_ERROR_RETURN
        (READ_WC40_COMBO_IEEE0_MIICNTLr(unit, ws, &data16));

    /* Check clause-73 autoneg */
    SOC_IF_ERROR_RETURN
        (READ_WC40_AN_IEEE0BLK_AN_IEEECONTROL1r(unit, ws, &mii_ctrl));
    
    if (SOC_IS_ARAD(unit)) {
        SOC_IF_ERROR_RETURN
            (phy_wcmod_an_get(unit, port, &an_enable, &an_done));
        an_enable &= an_done;
    }
    else {
        an_enable = ((data16 & MII_CTRL_AE) || (mii_ctrl & MII_CTRL_AE)) ? TRUE : FALSE;
    }

    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        SOC_IF_ERROR_RETURN
            (READ_WC40_XGXSBLK4_XGXSSTATUS1r(unit, ws, &data16));
        if (data16 & (XGXSBLK4_XGXSSTATUS1_LINKSTAT_MASK |
                               XGXSBLK4_XGXSSTATUS1_LINK10G_MASK)) {
            link_combo = TRUE;
        }
    } else {
        SOC_IF_ERROR_RETURN
            (READ_WC40_GP2_REG_GP2_1r(unit, ws, &data16));
        if (data16 & (1 << pc->lane_num)) {
            link_1000x = TRUE;
        }
    } 

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(pc->unit,
                         "u=%d p=%d an_enable=%04x link_1000x=%04x link_combo=%04x\n"),
              unit, port, an_enable, link_1000x,link_combo));

    WCMOD_MEM_SET(ability, 0, sizeof(*ability));
    mode = 0;

    if (an_enable && (link_1000x || link_combo)) {
        /* Decode remote advertisement only when link is up and autoneg is 
         * completed.
         */
 
        SOC_IF_ERROR_RETURN
            (READ_WC40_DIGITAL3_LP_UP1r(unit, ws, &an_adv));

        SOC_IF_ERROR_RETURN
            (READ_WC40_DIGITAL3_LP_UP2r(unit, ws, &up2));

        SOC_IF_ERROR_RETURN
            (READ_WC40_DIGITAL3_LP_UP3r(unit, ws, &up3));

        SOC_IF_ERROR_RETURN
            (READ_WC40_DIGITAL5_LP_UP4r(unit, ws, &up4));

        /* preemphasis settings */
        /* preemphasis (up2 & DIGITAL3_UP2_PREEMPHASIS_MASK) >>
         *              DIGITAL3_UP2_PREEMPHASIS_SHIFT
         * idriver     (up2 & DIGITAL3_UP2_IDRIVER_MASK) >>
         *              DIGITAL3_UP2_IDRIVER_SHIFT
         * ipredriver  (up2 & DIGITAL3_UP2_IPREDRIVER_MASK) >>
         *              DIGITAL3_UP2_IPREDRIVER_SHIFT
         */
        /* CL72,FEC and Higig2 
         * CL72 cap - (up3 & DIGITAL3_UP3_CL72_MASK)
         * FEC cap  - (up3 & DIGITAL3_UP3_FEC_MASK)
         * Higig2   - (up3 & DIGITAL3_UP3_HIGIG2_MASK)
         */
        ability->encap = (up3 & DIGITAL3_UP3_HIGIG2_MASK)?
                      SOC_PA_ENCAP_HIGIG2: 0;

        mode |= (up3 & DIGITAL3_LP_UP3_DATARATE_40G_MASK)?
                SOC_PA_SPEED_40GB: 0;
        mode |= (up3 & DIGITAL3_LP_UP3_DATARATE_31P5G_MASK)?
                SOC_PA_SPEED_30GB: 0;
        mode |= (up3 & DIGITAL3_LP_UP3_DATARATE_25P45GX4_MASK)?
                SOC_PA_SPEED_25GB: 0;
        mode |= (up3 & DIGITAL3_LP_UP3_DATARATE_21GX4_MASK)?
                SOC_PA_SPEED_21GB: 0;
        mode |= (an_adv & DIGITAL3_LP_UP1_DATARATE_20GX4_MASK) ? 
                SOC_PA_SPEED_20GB : 0;
        mode |= (up4 & DIGITAL5_LP_UP4_DATARATE_20G_MASK) ? 
                SOC_PA_SPEED_20GB : 0;
        mode |= (an_adv & DIGITAL3_LP_UP1_DATARATE_16GX4_MASK) ? 
                SOC_PA_SPEED_16GB : 0;
        mode |= (an_adv & DIGITAL3_LP_UP1_DATARATE_15GX4_MASK) ? 
                SOC_PA_SPEED_15GB : 0;
        mode |= (an_adv & DIGITAL3_LP_UP1_DATARATE_13GX4_MASK) ? 
                SOC_PA_SPEED_13GB : 0;
        mode |= (an_adv & DIGITAL3_LP_UP1_DATARATE_12P5GX4_MASK) ? 
                SOC_PA_SPEED_12P5GB : 0;
        mode |= (an_adv & DIGITAL3_LP_UP1_DATARATE_12GX4_MASK) ? 
                SOC_PA_SPEED_12GB : 0;
        mode |= (an_adv & DIGITAL3_LP_UP1_DATARATE_10GCX4_MASK) ? 
                SOC_PA_SPEED_10GB : 0;
        mode |= (an_adv & DIGITAL3_LP_UP1_DATARATE_10GX4_MASK) ? 
                SOC_PA_SPEED_10GB : 0;
        mode |= (an_adv & DIGITAL3_LP_UP1_DATARATE_6GX4_MASK) ?
                 SOC_PA_SPEED_6000MB : 0;
        mode |= (an_adv & DIGITAL3_LP_UP1_DATARATE_5GX4_MASK) ?
                 SOC_PA_SPEED_5000MB : 0;
        mode |= (an_adv & DIGITAL3_LP_UP1_DATARATE_2P5GX1_MASK) ?
                 SOC_PA_SPEED_2500MB : 0;

        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(pc->unit,
                             "u=%d p=%d over1G an_adv=%04x\n"),
                  unit, port, an_adv));

        SOC_IF_ERROR_RETURN
            (READ_WC40_COMBO_IEEE0_AUTONEGLPABILr(unit, ws, &an_adv));

        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(pc->unit,
                             "u=%d p=%d combo an_adv=%04x\n"),
                  unit, port, an_adv));

        mode |= (an_adv & MII_ANP_C37_FD) ? SOC_PA_SPEED_1000MB : 0;
        ability->speed_full_duplex = mode;
          
        switch (an_adv & (MII_ANP_C37_PAUSE | MII_ANP_C37_ASYM_PAUSE)) {
            case MII_ANP_C37_PAUSE:
                ability->pause |= SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
                break;
            case MII_ANP_C37_ASYM_PAUSE:
                ability->pause |= SOC_PA_PAUSE_TX;
                break;
            case MII_ANP_C37_PAUSE | MII_ANP_C37_ASYM_PAUSE:
                ability->pause |= SOC_PA_PAUSE_RX;
                break;
        }
        if (DEV_CFG_PTR(pc)->cl73an) {
            /* make sure it is autoneged link, don't use IEEE0BLK_AN_IEEESTATUS1r,
             * read by soft kr2
             */
            SOC_IF_ERROR_RETURN
                (READ_WC40_GP2_REG_GP2_1r(unit, ws, &data16));
            if (GP2_REG_GP2_1_LINK_AN_MASK & data16 & (1 << pc->lane_num)) {
                SOC_IF_ERROR_RETURN
                    (_phy_wcmod_c73_adv_remote_get(unit, port, ability));
            }
        }
    } else {
        /* Simply return local abilities */
        phy_wcmod_ability_advert_get(unit, port, ability);
    }

     /* Check FEC base on AN and CL37BAM*/
    SOC_IF_ERROR_RETURN
        (READ_WC40_AN_IEEE1BLK_AN_LP_BASEPAGEABILITY2r(unit, ws, &an_adv));
    if ((an_adv & AN_IEEE1BLK_AN_LP_BASEPAGEABILITY2_FEC_REQUESTED_MASK) ==
                    AN_IEEE1BLK_AN_LP_BASEPAGEABILITY2_FEC_REQUESTED_MASK) {
        fec = SOC_PA_FEC_CL74;
    } else if (an_adv & (1<<AN_IEEE1BLK_AN_LP_BASEPAGEABILITY2_FEC_REQUESTED_SHIFT)) {
        fec = SOC_PA_FEC_NONE;
    } else {
        fec = 0;
    }

    fec |= (up3 & DIGITAL3_LP_UP3_FEC_MASK) ? SOC_PA_FEC_CL74 : SOC_PA_FEC_NONE;

    if (fec & SOC_PA_FEC_CL74) {
        ability->fec = SOC_PA_FEC_CL74;
    } else {
        ability->fec = SOC_PA_FEC_NONE;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(pc->unit,
                         "phy_wcmod_ability_remote_get:unit=%d p=%d pause=%08x sp=%08x\n"),
              unit, port, ability->pause, ability->speed_full_duplex));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_wcmod_lb_set
 * Purpose:
 *      Put hc/FusionCore in PHY loopback
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      enable - binary value for on/off (1/0)
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_wcmod_lb_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t *pc;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int value = 0, rv, result;
    uint32 data, i, field_mask;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);
    data = 0;
    field_mask = 3 ;

    SOC_IF_ERROR_RETURN
      (_wcmod_phy_parameter_copy(pc, ws));

    if (IS_MULTI_CORE_PORT(pc)) {
        int i,lane, num_core;
        wcmod_st *temp_ws, *mld;
        uint16 data16;
        uint32 tmp_lane;

        mld = (wcmod_st *)( ws + 3);

        /*check the special ILKN port */
        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {

            num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
        }
        else {
            num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        }
        for (i = 0 ; i < num_core; i++) {
            temp_ws   =  (wcmod_st *)( ws + i);
                
            tmp_lane = temp_ws->this_lane;
            temp_ws->this_lane = 0;

            result = READ_WC40_XGXSBLK1_LANECTRL2r(temp_ws->unit, temp_ws, &data16);
            temp_ws->this_lane = tmp_lane;
            SOC_IF_ERROR_RETURN(result);

            data16 &= XGXSBLK1_LANECTRL2_GLOOP1G_MASK;
            data = 0;
            for(lane  = 0 ; lane < 4; lane++){
                /*if the lane is part of the port*/
                if((lane >= temp_ws->this_lane) && (lane < temp_ws->num_of_lane +  temp_ws->this_lane)){
                    data |= enable << (lane * 8);
                }
                else{
                    /*if not part of the port keep the lane in the current state*/
                    data |= ((data16>>lane) & 0x1 )<<(lane*8);
                }
            }
            temp_ws->per_lane_control  = data;
            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("TX_LOOPBACK_CONTROL", temp_ws, &rv));
        }

        /* enable IEEE AM in loopback in case of AM spacing is set to none IEEE in 100G fault WAR */
        if (!SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && enable) {
            SOC_IF_ERROR_RETURN (WC40_REG_READ(unit, mld, 0x0, 0x8420, &data16));

            if ((data16 & CL82_USERB0_TX_CONTROL_1_AM_TIMER_INIT_VAL_TX_HI_MASK) >>
                   CL82_USERB0_TX_CONTROL_1_AM_TIMER_INIT_VAL_TX_HI_SHIFT) {
                data16 = 0;
                SOC_IF_ERROR_RETURN (WC40_REG_READ(unit, mld, 0x0, 0x8420, &data16));
                data16 = data16 & 0xFFC7; /* Enable IEEE Spacing */
                SOC_IF_ERROR_RETURN (WC40_REG_WRITE(unit, mld, 0x0, 0x8420, data16));
            }
        }
    } else {
        /* return value is 2 bit per port */
        SOC_IF_ERROR_RETURN
            (phy_wcmod_lb_raw_get(unit, port, &value));

        if ( XENIA_MODE( DEV_CFG_PTR(pc)->lane_mode) || PHY_INDEPENDENT_LANE_MODE(unit, port)) {  
             /* for dual port mode, preserving lpbk setting for un-trageted port  */         
            if(IS_DUAL_LANE_PORT(pc)) {    
                for(i = 0; i < 4; i=i+2)  {
                    if(pc->lane_num == i) {
                        data |= enable << (i*8) | enable << ((i+1)*8) ;
                        continue;
                    } else {
                        data |= (((value>>2*i) &      field_mask) << (8 * i)) | 
                                (((value>>2*(i+1)) &  field_mask) << (8 * (i+1))) ;   /* lane even and lane odd */
                    }
                }  
            } else if ((ws->model_type) == WCMOD_QS_A0) {
                data |= enable << (0 * 8) | enable << (1 * 8) | enable << (2 * 8) | enable << (3 * 8);
            } else {
                for(i = 0; i < 4; i++)  {
                    if( ((pc->lane_num) % 4) == i) {
                        data |= enable << (i*8) ;
                        continue;
                    } else {
                        data |= (((value>>2*i) & field_mask )<< (8 * i)) ;
                    }
                }
            }
        } else {  /* combo, use all lanes */
            data |= enable << (0 * 8) | enable << (1 * 8) | enable << (2 * 8) | enable << (3 * 8);
        }

        ws->per_lane_control = data;
        SOC_IF_ERROR_RETURN
          (wcmod_tier1_selector("TX_LOOPBACK_CONTROL", ws, &rv));
    }
     
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_wcmod_lb_get
 * Purpose:
 *      Get hc/FusionCore PHY loopback state
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      enable - address of location to store binary value for on/off (1/0) per lane
 *               var enable bit 0 is for lane0 of each port.
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_wcmod_lb_get(int unit, soc_port_t port, int *enable)
{
    phy_ctrl_t *pc;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t *pDesc;
    uint16 lane;
    uint16 lane_mask=0;
    int rv;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    ws->diag_type = WCMOD_DIAG_TX_LOOPBACK;

    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
    lane = ws->accData;

    lane &= XGXSBLK1_LANECTRL2_GLOOP1G_MASK;

    if (IS_MULTI_CORE_PORT(pc)) {
        lane_mask = 1 << ws->this_lane;
    }else if (IND_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode) ||
        XENIA_MODE( DEV_CFG_PTR(pc)->lane_mode)) {
        if (IS_DUAL_LANE_PORT(pc)) {
            lane_mask = 1 << pc->lane_num;
            lane_mask |= (lane_mask << 1);
        } else {
            lane_mask = 1 << ((pc->lane_num) % 4);
        }
    } else {  /* combo mode */
            lane_mask = XGXSBLK1_LANECTRL2_GLOOP1G_MASK;
    }
    lane &= lane_mask;

    if (IS_MULTI_CORE_PORT(pc)) {
        lane >>= ws->this_lane;
    } else{
        lane >>= ((pc->lane_num) % 4);
    }
    *enable = (int)lane;

    return SOC_E_NONE;
}

/* return value enable[1:0] for lane0, enable[3:2] for lane1,
                enable[5:4] for lane2, enable[7:6] for lane3.
 */
STATIC int
phy_wcmod_lb_raw_get(int unit, soc_port_t port, int *enable)
{
    phy_ctrl_t *pc;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t *pDesc;
    uint16 lane, i;
    uint16 tmp_select; 
    uint16 tmp_lane ;
    int rv;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    tmp_select    = ws->lane_select ;
    tmp_lane      = ws->this_lane ;
    ws->diag_type = WCMOD_DIAG_TX_LOOPBACK;
    
    if (ws->this_lane >= 4) {
        ws->lane_select = WCMOD_LANE_0_0_0_1_0_0_0_0;
    } else {
    ws->lane_select = WCMOD_LANE_0_0_0_1 ;
    }
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
    lane = ws->accData;
    ws->lane_select = tmp_select ; /* restore */
    ws->this_lane   = tmp_lane ;

    lane &=  XGXSBLK1_LANECTRL2_GLOOP1G_MASK;
    lane >>= XGXSBLK1_LANECTRL2_GLOOP1G_SHIFT ;
    *enable = 0 ;
    for(i=0; i<NUM_LANES; i++) {
        if( (lane >>i) & 1 ) *enable |= (1 <<(2*i)) ;
    }
    return SOC_E_NONE;
}


STATIC int
phy_wcmod_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
    phy_ctrl_t      *pc;
    uint32 intf;

    pc = INT_PHY_SW_STATE(unit, port);

    if (pif > 31) {
        return SOC_E_PARAM;
    }

    /* need to check valid interfaces
     * ALL 40G/10G interfaces are mutually exclusive
     */
    intf = DEV_CFG_PTR(pc)->line_intf;

    if (WCMOD_40G_10G_INTF(pif)) {
        intf &= ~WCMOD_40G_10G_INTF_ALL;  /* clear all 10G/40G interfaces */
        intf |= 1 << pif;
    } else {
        intf |= 1 << pif;
    }

    DEV_CFG_PTR(pc)->line_intf = intf;
    return SOC_E_NONE;
}

STATIC int
phy_wcmod_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    phy_ctrl_t *pc;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t *pDesc;
    int speed;
    int intf;
    int scr;
    int rv;
    uint16 data16;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        rv = _phy_wcmod_combo_speed_get(unit, port, &speed,&intf,&scr);
    } else {
        rv = _phy_wcmod_ind_speed_get(unit, port, &speed,&intf,&scr);
    }

    if (IND_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        if (speed < 10000) {
            if (intf == SOC_PORT_IF_KX) {
                *pif = intf;
            } else if (DEV_CFG_PTR(pc)->fiber_pref) {
                *pif = SOC_PORT_IF_GMII;
            } else {
                *pif = SOC_PORT_IF_SGMII;
            }
        } else {
            *pif = intf;
        }
    } else { /* combo mode */
        if (intf == SOC_PORT_IF_KR4) {
            SOC_IF_ERROR_RETURN
                (READ_WC40_UC_INFO_B1_FIRMWARE_MODEr(ws->unit, ws, &data16));
            if (data16 == WCMOD_UC_CTRL_XLAUI) {
                intf = SOC_PORT_IF_XLAUI;
            } else if (data16 == WCMOD_UC_CTRL_SR4) {
                intf = SOC_PORT_IF_SR;
            }
            if (DEV_CFG_PTR(pc)->line_intf & WCMOD_IF_KR4) {
                intf = SOC_PORT_IF_KR4;
            }
        }
        *pif = intf;
        if (speed < 10000) {
            if (DEV_CFG_PTR(pc)->fiber_pref) {
                *pif = SOC_PORT_IF_GMII;
            } else {
                *pif = SOC_PORT_IF_SGMII;
            }
        }
    }

    return rv;
}

STATIC int
phy_wcmod_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    phy_ctrl_t *pc;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t *pDesc;

    if (NULL == ability) {
        return SOC_E_PARAM;
    }

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    if((ws->model_type) == WCMOD_XN) {
        return phy_wcmod_xgxs16g1l_ability_local_get(unit, port, ability);
    }

    WCMOD_MEM_SET(ability, 0, sizeof(*ability));

    if (CUSTOM_MODE(pc)) {
        ability->speed_full_duplex  = SOC_PA_SPEED_3000MB; 
        ability->medium    = SOC_PA_MEDIUM_FIBER;
        ability->loopback  = SOC_PA_LB_PHY;
        return (SOC_E_NONE);
    }

    if (CUSTOM1_MODE(pc)) {    
        ability->speed_full_duplex  = SOC_PA_SPEED_12GB;
        ability->medium    = SOC_PA_MEDIUM_FIBER;
        ability->loopback  = SOC_PA_LB_PHY;
        ability->interface |= SOC_PA_INTF_XGMII;
        return (SOC_E_NONE);
    }
    ability->fec = SOC_PA_FEC_NONE | SOC_PA_FEC_CL74;

    if (IND_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) { 
        ability->speed_full_duplex  = SOC_PA_SPEED_1000MB;
        if (DEV_CFG_PTR(pc)->fiber_pref)   {
            ability->speed_full_duplex  |= SOC_PA_SPEED_2500MB |
                                           SOC_PA_SPEED_10GB;
                ability->speed_full_duplex  |= SOC_PA_SPEED_100MB;
                ability->speed_half_duplex  = SOC_PA_SPEED_100MB;
        } else {
            ability->speed_half_duplex  = SOC_PA_SPEED_10MB |
                                          SOC_PA_SPEED_100MB;
            ability->speed_full_duplex  |= SOC_PA_SPEED_10MB |
                                           SOC_PA_SPEED_100MB;
        } 
        switch(pc->speed_max) {  /* must include all the supported speedss */
            case 21000:
                ability->speed_full_duplex |= SOC_PA_SPEED_21GB;
                /* fall through */
            case 20000:
                ability->speed_full_duplex |= SOC_PA_SPEED_20GB;
                /* fall through */
            case 16000: 
                ability->speed_full_duplex |= SOC_PA_SPEED_16GB; 
                /* fall through */
                /* not supported speed
                  case 15750:
                  ability->speed_full_duplex |= SOC_PA_SPEED_15P75GB; */
                /* fall through */
            case 15000:
                ability->speed_full_duplex |= SOC_PA_SPEED_15GB;
                /* fall through */
                /* not supported speed
                   case 12730:
                   ability->speed_full_duplex |= SOC_PA_SPEED_12P73GB;  */
                /* fall through */
            case 12000:
                ability->speed_full_duplex |= SOC_PA_SPEED_12GB;
                /* fall through */
            case 11000:
                ability->speed_full_duplex |= SOC_PA_SPEED_11GB;
                /* not supported speed 
                   case 10500: 
                   ability->speed_full_duplex |= SOC_PA_SPEED_10P5GB; */
                /* fall through */
            case 10000:
                ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
                /* fall through */
            default:
                break;
        }
        ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
        ability->interface = SOC_PA_INTF_GMII | SOC_PA_INTF_SGMII;
        if (pc->speed_max >= 10000) {
            ability->interface |= SOC_PA_INTF_XGMII;
        }
        phy_wcmod_medium_get(unit, port, &ability->medium);
        ability->loopback  = SOC_PA_LB_PHY;
        if (IS_DUAL_LANE_PORT(pc)) {
            ability->flags     = 0;
        } else {
            ability->flags     = SOC_PA_AUTONEG;
        }
    } else {
        ability->speed_half_duplex  = SOC_PA_ABILITY_NONE;
        ability->speed_full_duplex  = SOC_PA_SPEED_1000MB;
        if (DEV_CFG_PTR(pc)->fiber_pref)   {
            ability->speed_full_duplex  |= SOC_PA_SPEED_2500MB;
                ability->speed_full_duplex  |= SOC_PA_SPEED_100MB;
                ability->speed_half_duplex  = SOC_PA_SPEED_100MB;
        } else {
            ability->speed_half_duplex  = SOC_PA_SPEED_10MB |
                                      SOC_PA_SPEED_100MB;
            ability->speed_full_duplex  |= SOC_PA_SPEED_10MB |
                                       SOC_PA_SPEED_100MB;
        }
        switch(pc->speed_max) {
            case 127000:
                ability->speed_full_duplex |= SOC_PA_SPEED_127GB;
            case 106000:
                ability->speed_full_duplex |= SOC_PA_SPEED_106GB;
            case 120000:
                ability->speed_full_duplex |= SOC_PA_SPEED_120GB;
            case 100000:
                ability->speed_full_duplex |= SOC_PA_SPEED_100GB;
            case 42000:
                ability->speed_full_duplex |= SOC_PA_SPEED_42GB;
            case 40000:
                ability->speed_full_duplex |= SOC_PA_SPEED_40GB;
            case 30000:
                ability->speed_full_duplex |= SOC_PA_SPEED_30GB;
            case 25000:
                ability->speed_full_duplex |= SOC_PA_SPEED_25GB;
            case 21000:
                ability->speed_full_duplex |= SOC_PA_SPEED_21GB;
            case 20000:
                ability->speed_full_duplex |= SOC_PA_SPEED_20GB;
                /* fall through */
            case 16000:
                ability->speed_full_duplex |= SOC_PA_SPEED_16GB; 
                /* fall through */
            case 15000:
                ability->speed_full_duplex |= SOC_PA_SPEED_15GB; 
                /* fall through */
            case 13000:
                ability->speed_full_duplex |= SOC_PA_SPEED_13GB;
                /* fall through */
            case 12000:
                ability->speed_full_duplex |= SOC_PA_SPEED_12GB;
                /* fall through */
            default:
                ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
                break; 
        }
        ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
        ability->interface = SOC_PA_INTF_XGMII;
        phy_wcmod_medium_get(unit, port, &ability->medium);
        ability->loopback  = SOC_PA_LB_PHY;
        ability->flags     = SOC_PA_AUTONEG;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(pc->unit,
                         "phy_wcmod_ability_local_get:unit=%d p=%d sp=%08x\n"),
              unit, port, ability->speed_full_duplex));

    return (SOC_E_NONE);
}

/*
Bits lane assignment

\li 31:28  TX lane 3
\li 27:24  TX lane 2
\li 23:20  TX lane 1
\li 19:16  TX lane 0

\li 15:12  RX lane 3
\li 11:8   RX lane 2
\li 7:4    RX lane 1
\li 3:0    RX lane 0
*/
STATIC int
_phy_wcmod_control_lane_swap_set(int unit, phy_ctrl_t *pc, uint32 value)
{
    int rv;
    wcmod_st  *ws;
    WCMOD_DEV_DESC_t *pDesc;
    
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    ws->per_lane_control = value;
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("LANE_SWAP", ws, &rv));

    return SOC_E_NONE;
}

#define PHY_WCMOD_LANEPRBS_LANE_SHIFT   4

int
_phy_wcmod_control_prbs_decouple_tx_enable_set(wcmod_st *ws, uint32 value)
{
    int rv;
    uint32 data, inv_data, poly_data;

    /*first get inv and poly value */
    SOC_IF_ERROR_RETURN
        (_phy_wcmod_control_prbs_decouple_tx_invert_data_get(ws, &inv_data));

    SOC_IF_ERROR_RETURN
        (_phy_wcmod_control_prbs_decouple_tx_polynomial_get(ws, &poly_data));
    data = (0x1 << 7) | (0x1 << 5);
    data |= poly_data;
    data |= inv_data << 3;
    ws->per_lane_control = (data  | (value << 4)) << ((ws->this_lane) * 8) ;
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("PRBS_DECOUPLE_CONTROL", ws, &rv));

    return SOC_E_NONE;
}

STATIC
int phy_wcmod_control_prbs_decouple_tx_enable_set(int unit, soc_port_t port, uint32 value)
{
    phy_ctrl_t *pc;
    wcmod_st  *temp_ws,  *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int size, j;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
      (_wcmod_phy_parameter_copy(pc, ws));

    if ((SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) || (SOC_INFO(unit).port_num_lanes[pc->port] >= 10)) {
        int i, tmpLane, num_core = 0;
        
        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
            num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
        } else {
            num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        }
        for (i = 0 ; i < num_core; i++) {
             temp_ws   =  (wcmod_st *)( ws + i);
            /*temp_id = temp_ws->phy_ad;
            SOC_IF_ERROR_RETURN(_wcmod_phy_parameter_copy(pc, temp_ws));
            temp_ws->phy_ad = temp_id; */
            size = temp_ws->num_of_lane;
            tmpLane = temp_ws->this_lane;
            for(j = tmpLane; j < tmpLane + size; j++) {
                temp_ws->this_lane = j;
                switch(j) {
                    case 0: temp_ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                    case 1: temp_ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                    case 2: temp_ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                    case 3: temp_ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
                default:
                    break;
                }
                SOC_IF_ERROR_RETURN
                    (_phy_wcmod_control_prbs_decouple_tx_enable_set(temp_ws, value));
           }
           temp_ws->lane_select = WCMOD_LANE_0_0_0_1;
        }
    } else {
        int tmp_lane, tmp_lane_select;
        size = SOC_INFO(unit).port_num_lanes[pc->port];
        tmp_lane = ws->this_lane;
        tmp_lane_select = ws->lane_select; 
        for(j = pc->lane_num; j < size + pc->lane_num; j++) {
            ws->this_lane = j;
            switch(j) {
                case 0: ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                case 1: ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                case 2: ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                case 3: ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
            default:
                break;
            }
            SOC_IF_ERROR_RETURN
                (_phy_wcmod_control_prbs_decouple_tx_enable_set(ws, value));
       }
       ws->this_lane = tmp_lane;
       ws->lane_select =  tmp_lane_select;
    } 
    return(SOC_E_NONE);
}


int
_phy_wcmod_control_prbs_decouple_tx_invert_data_set(wcmod_st *ws, uint32 value)
{
    int rv;
    uint32 data, en_data, poly_data;

    /*first get inv and poly value */
    SOC_IF_ERROR_RETURN
        (_phy_wcmod_control_prbs_decouple_tx_enable_get(ws, &en_data));

    SOC_IF_ERROR_RETURN
        (_phy_wcmod_control_prbs_decouple_tx_polynomial_get(ws, &poly_data));

    data = (0x1 << 7) | (0x1 << 5);
    data |= en_data << 4;
    data |= poly_data;
    ws->per_lane_control = (data  | value << 3) << ((ws->this_lane) * 8) ;
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("PRBS_DECOUPLE_CONTROL", ws, &rv));

    return SOC_E_NONE;
}

STATIC
int phy_wcmod_control_prbs_decouple_tx_invert_data_set(int unit, soc_port_t port, uint32 value)
{
    phy_ctrl_t *pc;
    wcmod_st  *temp_ws,  *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int size, j;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
      (_wcmod_phy_parameter_copy(pc, ws));

    if ((SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) || (SOC_INFO(unit).port_num_lanes[pc->port] >= 10)) {
        int i, tmpLane, num_core = 0;
        
        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
            num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
        } else {
            num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        }
        for (i = 0 ; i < num_core; i++) {
             temp_ws   =  (wcmod_st *)( ws + i);
            /*temp_id = temp_ws->phy_ad;
            SOC_IF_ERROR_RETURN(_wcmod_phy_parameter_copy(pc, temp_ws));
            temp_ws->phy_ad = temp_id;*/
            size = temp_ws->num_of_lane;
            tmpLane = temp_ws->this_lane;
            for(j = tmpLane; j < tmpLane + size; j++) {
                temp_ws->this_lane = j;
                switch(j) {
                    case 0: temp_ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                    case 1: temp_ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                    case 2: temp_ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                    case 3: temp_ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
                default:
                    break;
                }
                SOC_IF_ERROR_RETURN
                    (_phy_wcmod_control_prbs_decouple_tx_invert_data_set(temp_ws, value));
           }
           temp_ws->lane_select = WCMOD_LANE_0_0_0_1;
        }
    } else {
        int tmp_lane, tmp_lane_select;
        size = SOC_INFO(unit).port_num_lanes[pc->port];
        tmp_lane = ws->this_lane;
        tmp_lane_select = ws->lane_select; 
        for(j = pc->lane_num; j < size + pc->lane_num; j++) {
            ws->this_lane = j;
            switch(j) {
                case 0: ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                case 1: ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                case 2: ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                case 3: ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
            default:
                break;
            }
            SOC_IF_ERROR_RETURN
                (_phy_wcmod_control_prbs_decouple_tx_invert_data_set(ws, value));
       }
       ws->this_lane = tmp_lane;
       ws->lane_select =  tmp_lane_select;
    } 
    return(SOC_E_NONE);
}


int
_phy_wcmod_control_prbs_decouple_tx_polynomial_set(wcmod_st *ws, uint32 value)
{
    int rv;
    uint32 data, en_data, inv_data;

    /*first get inv and poly value */
    SOC_IF_ERROR_RETURN
        (_phy_wcmod_control_prbs_decouple_tx_enable_get(ws, &en_data));
    SOC_IF_ERROR_RETURN
        (_phy_wcmod_control_prbs_decouple_tx_invert_data_get(ws, &inv_data));

    data = (0x1 << 7) | (0x1 << 5);
    data |= en_data << 4;
    data |= inv_data << 3 ;

    ws->per_lane_control = (data  | value ) << ((ws->this_lane) * 8) ;
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("PRBS_DECOUPLE_CONTROL", ws, &rv));

    return SOC_E_NONE;
}

STATIC
int phy_wcmod_control_prbs_decouple_tx_polynomial_set(int unit, soc_port_t port, uint32 value)
{
    phy_ctrl_t *pc;
    wcmod_st  *temp_ws,  *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int size, j;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
      (_wcmod_phy_parameter_copy(pc, ws));

    if ((SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) || (SOC_INFO(unit).port_num_lanes[pc->port] >= 10)) {
        int i, tmpLane, num_core = 0;
        
        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
            num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
        } else {
            num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        }
        for (i = 0 ; i < num_core; i++) {
             temp_ws   =  (wcmod_st *)( ws + i);
            /*temp_id = temp_ws->phy_ad;
            SOC_IF_ERROR_RETURN(_wcmod_phy_parameter_copy(pc, temp_ws));
            temp_ws->phy_ad = temp_id; */
            size = temp_ws->num_of_lane;
            tmpLane = temp_ws->this_lane;
            for(j = tmpLane; j < tmpLane + size; j++) {
                temp_ws->this_lane = j;
                switch(j) {
                    case 0: temp_ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                    case 1: temp_ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                    case 2: temp_ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                    case 3: temp_ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
                default:
                    break;
                }
                SOC_IF_ERROR_RETURN
                    (_phy_wcmod_control_prbs_decouple_tx_polynomial_set(temp_ws, value));
           }
           temp_ws->lane_select = WCMOD_LANE_0_0_0_1;
        }
    } else {
        int tmp_lane, tmp_lane_select;
        size = SOC_INFO(unit).port_num_lanes[pc->port];
        tmp_lane = ws->this_lane;
        tmp_lane_select = ws->lane_select; 
        for(j = pc->lane_num; j < size + pc->lane_num; j++) {
            ws->this_lane = j;
            switch(j) {
                case 0: ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                case 1: ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                case 2: ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                case 3: ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
            default:
                break;
            }
            SOC_IF_ERROR_RETURN
                (_phy_wcmod_control_prbs_decouple_tx_polynomial_set(ws, value));
       }
       ws->this_lane = tmp_lane;
       ws->lane_select =  tmp_lane_select;
    } 
    return(SOC_E_NONE);
}


int
_phy_wcmod_control_prbs_decouple_rx_enable_set(wcmod_st *ws, uint32 value)
{
    int rv;
    uint32 data, inv_data, poly_data;

    /*first get inv and poly value */
    SOC_IF_ERROR_RETURN
        (_phy_wcmod_control_prbs_decouple_rx_invert_data_get(ws, &inv_data));
    SOC_IF_ERROR_RETURN
        (_phy_wcmod_control_prbs_decouple_rx_polynomial_get(ws, &poly_data));

    data = (0x1 << 6) | (0x1 << 5);
    data |= poly_data;
    data |= inv_data << 3;

    ws->per_lane_control = (data | value << 4) << ((ws->this_lane) * 8) ;
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("PRBS_DECOUPLE_CONTROL", ws, &rv));

    return SOC_E_NONE;
}

STATIC
int phy_wcmod_control_prbs_decouple_rx_enable_set(int unit, soc_port_t port, uint32 value)
{
    phy_ctrl_t *pc;
    wcmod_st  *temp_ws,  *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int size, j;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
      (_wcmod_phy_parameter_copy(pc, ws));

    if ((SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) || (SOC_INFO(unit).port_num_lanes[pc->port] >= 10)) {
        int i, tmpLane, num_core = 0;
        
        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
            num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
        } else {
            num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        }
        for (i = 0 ; i < num_core; i++) {
             temp_ws   =  (wcmod_st *)( ws + i);
            /*temp_id = temp_ws->phy_ad;
            SOC_IF_ERROR_RETURN(_wcmod_phy_parameter_copy(pc, temp_ws));
            temp_ws->phy_ad = temp_id; */
            size = temp_ws->num_of_lane;
            tmpLane = temp_ws->this_lane;
            for(j = tmpLane; j < tmpLane + size; j++) {
                temp_ws->this_lane = j;
                switch(j) {
                    case 0: temp_ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                    case 1: temp_ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                    case 2: temp_ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                    case 3: temp_ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
                default:
                    break;
                }
                SOC_IF_ERROR_RETURN
                    (_phy_wcmod_control_prbs_decouple_rx_enable_set(temp_ws, value));
           }
           temp_ws->lane_select = WCMOD_LANE_0_0_0_1;
        }
    } else {
        int tmp_lane, tmp_lane_select;
        size = SOC_INFO(unit).port_num_lanes[pc->port];
        tmp_lane = ws->this_lane;
        tmp_lane_select = ws->lane_select; 
        for(j = pc->lane_num; j < size + pc->lane_num; j++) {
            ws->this_lane = j;
            switch(j) {
                case 0: ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                case 1: ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                case 2: ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                case 3: ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
            default:
                break;
            }
            SOC_IF_ERROR_RETURN
                (_phy_wcmod_control_prbs_decouple_rx_enable_set(ws, value));
       }
       ws->this_lane = tmp_lane;
       ws->lane_select =  tmp_lane_select;
    } 
    return(SOC_E_NONE);
}


int
_phy_wcmod_control_prbs_decouple_rx_invert_data_set(wcmod_st *ws, uint32 value)
{
    int rv;
    uint32 data, en_data, poly_data;

    /*first get inv and poly value */
    SOC_IF_ERROR_RETURN
        (_phy_wcmod_control_prbs_decouple_rx_enable_get(ws, &en_data));

    SOC_IF_ERROR_RETURN
        (_phy_wcmod_control_prbs_decouple_rx_polynomial_get(ws, &poly_data));

    data = (0x1 << 6) | (0x1 << 5);
    data |= en_data << 4;
    data |= poly_data;

    ws->per_lane_control = (data  | value << 3) << ((ws->this_lane) * 8) ;
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("PRBS_DECOUPLE_CONTROL", ws, &rv));

    return SOC_E_NONE;
}

STATIC
int phy_wcmod_control_prbs_decouple_rx_invert_data_set(int unit, soc_port_t port, uint32 value)
{
    phy_ctrl_t *pc;
    wcmod_st  *temp_ws,  *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int size, j;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
      (_wcmod_phy_parameter_copy(pc, ws));

    if ((SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) || (SOC_INFO(unit).port_num_lanes[pc->port] >= 10)) {
        int i, tmpLane, num_core = 0;
        
        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
            num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
        } else {
            num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        }
        for (i = 0 ; i < num_core; i++) {
             temp_ws   =  (wcmod_st *)( ws + i);
            /*temp_id = temp_ws->phy_ad;
            SOC_IF_ERROR_RETURN(_wcmod_phy_parameter_copy(pc, temp_ws));
            temp_ws->phy_ad = temp_id; */
            size = temp_ws->num_of_lane;
            tmpLane = temp_ws->this_lane;
            for(j = tmpLane; j < tmpLane + size; j++) {
                temp_ws->this_lane = j;
                switch(j) {
                    case 0: temp_ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                    case 1: temp_ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                    case 2: temp_ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                    case 3: temp_ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
                default:
                    break;
                }
                SOC_IF_ERROR_RETURN
                    (_phy_wcmod_control_prbs_decouple_rx_invert_data_set(temp_ws, value));
           }
           temp_ws->lane_select = WCMOD_LANE_0_0_0_1;
        }
    } else {
        int tmp_lane, tmp_lane_select;
        size = SOC_INFO(unit).port_num_lanes[pc->port];
        tmp_lane = ws->this_lane;
        tmp_lane_select = ws->lane_select; 
        for(j = pc->lane_num; j < size + pc->lane_num; j++) {
            ws->this_lane = j;
            switch(j) {
                case 0: ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                case 1: ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                case 2: ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                case 3: ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
            default:
                break;
            }
            SOC_IF_ERROR_RETURN
                (_phy_wcmod_control_prbs_decouple_rx_invert_data_set(ws, value));
       }
       ws->this_lane = tmp_lane;
       ws->lane_select =  tmp_lane_select;
    } 
    return(SOC_E_NONE);
}


int
_phy_wcmod_control_prbs_decouple_rx_polynomial_set(wcmod_st *ws, uint32 value)
{
    int rv;
    uint32 data, en_data, inv_data;

    /*first get inv and poly value */
    SOC_IF_ERROR_RETURN
        (_phy_wcmod_control_prbs_decouple_rx_enable_get(ws, &en_data));

    SOC_IF_ERROR_RETURN
        (_phy_wcmod_control_prbs_decouple_rx_invert_data_get(ws, &inv_data));

    data = (0x1 << 6) | (0x1 << 5);
    data |= en_data << 4;
    data |= inv_data << 3 ;

    ws->per_lane_control = (data  | value ) << ((ws->this_lane) * 8) ;
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("PRBS_DECOUPLE_CONTROL", ws, &rv));

    return SOC_E_NONE;
}

STATIC
int phy_wcmod_control_prbs_decouple_rx_polynomial_set(int unit, soc_port_t port, uint32 value)
{
    phy_ctrl_t *pc;
    wcmod_st  *temp_ws,  *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int size, j;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
      (_wcmod_phy_parameter_copy(pc, ws));

    if ((SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) || (SOC_INFO(unit).port_num_lanes[pc->port] >= 10)) {
        int i, tmpLane, num_core = 0;
        
        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
            num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
        } else {
            num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        }

        for (i = 0 ; i < num_core; i++) {
             temp_ws   =  (wcmod_st *)( ws + i);
            /*temp_id = temp_ws->phy_ad;
            SOC_IF_ERROR_RETURN(_wcmod_phy_parameter_copy(pc, temp_ws));
            temp_ws->phy_ad = temp_id; */
            size = temp_ws->num_of_lane;
            tmpLane = temp_ws->this_lane;
            for(j = tmpLane; j < tmpLane + size; j++) {
                temp_ws->this_lane = j;
                switch(j) {
                    case 0: temp_ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                    case 1: temp_ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                    case 2: temp_ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                    case 3: temp_ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
                default:
                    break;
                }
                SOC_IF_ERROR_RETURN
                    (_phy_wcmod_control_prbs_decouple_rx_polynomial_set(temp_ws, value));
           }
           temp_ws->lane_select = WCMOD_LANE_0_0_0_1;
        }
    } else {
        int tmp_lane, tmp_lane_select;
        size = SOC_INFO(unit).port_num_lanes[pc->port];
        tmp_lane = ws->this_lane;
        tmp_lane_select = ws->lane_select; 
        for(j = pc->lane_num; j < size + pc->lane_num; j++) {
            ws->this_lane = j;
            switch(j) {
                case 0: ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                case 1: ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                case 2: ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                case 3: ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
            default:
                break;
            }
            SOC_IF_ERROR_RETURN
                (_phy_wcmod_control_prbs_decouple_rx_polynomial_set(ws, value));
       }
       ws->this_lane = tmp_lane;
       ws->lane_select =  tmp_lane_select;
    } 
    return(SOC_E_NONE);
}

int
_phy_wcmod_control_prbs_decouple_tx_enable_get(wcmod_st *ws, uint32 *value)
{
    int rv;
    
    ws->diag_type = WCMOD_DIAG_PRBS_DECOUPLE_TX;
    ws->per_lane_control = 0x1;

    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
    *value = ws->accData;
    return SOC_E_NONE;
}

STATIC
int phy_wcmod_control_prbs_decouple_tx_enable_get(int unit, soc_port_t port, uint32 *value)
{
    phy_ctrl_t *pc;
    wcmod_st  *temp_ws,  *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int size, j;
    uint32 tmp_value;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
      (_wcmod_phy_parameter_copy(pc, ws));

    *value = 0; 
    if ((SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) || (SOC_INFO(unit).port_num_lanes[pc->port] >= 10)) {
        int i, tmpLane, lane_num = 0, num_core = 0;
        
        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
            num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
        } else {
            num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        }
        for (i = 0 ; i < num_core; i++) {
             temp_ws   =  (wcmod_st *)( ws + i);
            /*temp_id = temp_ws->phy_ad;
            SOC_IF_ERROR_RETURN(_wcmod_phy_parameter_copy(pc, temp_ws));
            temp_ws->phy_ad = temp_id; */
            size = temp_ws->num_of_lane;
            tmpLane = temp_ws->this_lane;
            for(j = tmpLane; j < tmpLane + size; j++) {
                temp_ws->this_lane = j;
                switch(j) {
                    case 0: temp_ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                    case 1: temp_ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                    case 2: temp_ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                    case 3: temp_ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
                default:
                    break;
                }
                tmp_value = 0;
                SOC_IF_ERROR_RETURN
                    (_phy_wcmod_control_prbs_decouple_tx_enable_get(temp_ws, &tmp_value));
                *value |= tmp_value << lane_num;
                lane_num++;
           }
           temp_ws->lane_select = WCMOD_LANE_0_0_0_1;
           temp_ws->this_lane = 0;
        }
    } else {
        int tmp_lane, tmp_lane_select;
        size = SOC_INFO(unit).port_num_lanes[pc->port];
        tmp_lane = ws->this_lane;
        tmp_lane_select = ws->lane_select; 
        for(j = pc->lane_num; j < pc->lane_num + size; j++) {
            ws->this_lane = j;
            switch(j) {
                case 0: ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                case 1: ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                case 2: ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                case 3: ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
            default:
                break;
            }
            tmp_value = 0;
            SOC_IF_ERROR_RETURN
                (_phy_wcmod_control_prbs_decouple_tx_enable_get(ws, &tmp_value));
            if (size == 1) {
                *value = tmp_value;
            } else {
                *value |= tmp_value << j;
            }
       }
       ws->this_lane = tmp_lane;
       ws->lane_select =  tmp_lane_select;
    } 

    return(SOC_E_NONE);
}


int
_phy_wcmod_control_prbs_decouple_tx_invert_data_get(wcmod_st *ws, uint32 *value)
{
    int rv;
    
    ws->diag_type = WCMOD_DIAG_PRBS_DECOUPLE_TX;
    ws->per_lane_control = 0x2;

    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
    *value = ws->accData;
    return SOC_E_NONE;
}

STATIC
int phy_wcmod_control_prbs_decouple_tx_invert_data_get(int unit, soc_port_t port, uint32 *value)
{
    phy_ctrl_t *pc;
    wcmod_st  *temp_ws,  *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int size, j;
    uint32 tmp_value;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
      (_wcmod_phy_parameter_copy(pc, ws));

    *value = 0; 
    if ((SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) || (SOC_INFO(unit).port_num_lanes[pc->port] >= 10)) {
        int i, tmpLane, lane_num = 0, num_core = 0;
        
        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
            num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
        } else {
            num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        }

        for (i = 0 ; i < num_core; i++) {
             temp_ws   =  (wcmod_st *)( ws + i);
            /*temp_id = temp_ws->phy_ad;
            SOC_IF_ERROR_RETURN(_wcmod_phy_parameter_copy(pc, temp_ws));
            temp_ws->phy_ad = temp_id; */
            size = temp_ws->num_of_lane;
            tmpLane = temp_ws->this_lane;
            for(j = tmpLane; j < tmpLane + size; j++) {
                temp_ws->this_lane = j;
                switch(j) {
                    case 0: temp_ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                    case 1: temp_ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                    case 2: temp_ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                    case 3: temp_ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
                default:
                    break;
                }
                tmp_value = 0;
                SOC_IF_ERROR_RETURN
                    (_phy_wcmod_control_prbs_decouple_tx_invert_data_get(temp_ws, &tmp_value));
                *value |= tmp_value << (lane_num);
                lane_num++;
           }
           temp_ws->lane_select = WCMOD_LANE_0_0_0_1;
           temp_ws->this_lane = 0;
        }
    } else {
        int tmp_lane, tmp_lane_select;
        size = SOC_INFO(unit).port_num_lanes[pc->port];
        tmp_lane = ws->this_lane;
        tmp_lane_select = ws->lane_select; 
        for(j = pc->lane_num; j < pc->lane_num + size; j++) {
            ws->this_lane = j;
            switch(j) {
                case 0: ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                case 1: ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                case 2: ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                case 3: ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
            default:
                break;
            }
            tmp_value = 0;
            SOC_IF_ERROR_RETURN
                (_phy_wcmod_control_prbs_decouple_tx_invert_data_get(ws, &tmp_value));
            if (size == 1) {
                *value = tmp_value;
            } else {
                *value |= tmp_value << j;
            }
       }
       ws->this_lane = tmp_lane;
       ws->lane_select =  tmp_lane_select;
    } 
    return(SOC_E_NONE);
}


int
_phy_wcmod_control_prbs_decouple_tx_polynomial_get(wcmod_st *ws, uint32 *value)
{
    int rv;
    
    ws->diag_type = WCMOD_DIAG_PRBS_DECOUPLE_TX;
    ws->per_lane_control = 0x4;

    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
    *value = ws->accData;
    return SOC_E_NONE;
}

STATIC
int phy_wcmod_control_prbs_decouple_tx_polynomial_get(int unit, soc_port_t port, uint32 *value)
{
    phy_ctrl_t *pc;
    wcmod_st  *temp_ws,  *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int size, j;
    uint32 tmp_value;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
      (_wcmod_phy_parameter_copy(pc, ws));

    *value = 0; 
    if ((SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) || (SOC_INFO(unit).port_num_lanes[pc->port] >= 10)) {
        int i, tmpLane, lane_num = 0, num_core = 0;
        
        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
            num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
        } else {
            num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        }

        for (i = 0 ; i < num_core; i++) {
             temp_ws   =  (wcmod_st *)( ws + i);
            /*temp_id = temp_ws->phy_ad;
            SOC_IF_ERROR_RETURN(_wcmod_phy_parameter_copy(pc, temp_ws));
            temp_ws->phy_ad = temp_id; */
            size = temp_ws->num_of_lane;
            tmpLane = temp_ws->this_lane;
            for(j = tmpLane; j < tmpLane + size; j++) {
                temp_ws->this_lane = j;
                switch(j) {
                    case 0: temp_ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                    case 1: temp_ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                    case 2: temp_ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                    case 3: temp_ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
                default:
                    break;
                }
                tmp_value = 0;
                SOC_IF_ERROR_RETURN
                    (_phy_wcmod_control_prbs_decouple_tx_polynomial_get(temp_ws, &tmp_value));
                *value |= tmp_value << (2 * (lane_num));
                lane_num++;
           }
           temp_ws->lane_select = WCMOD_LANE_0_0_0_1;
           temp_ws->this_lane = 0;
        }
    } else {
        int tmp_lane, tmp_lane_select;
        size = SOC_INFO(unit).port_num_lanes[pc->port];
        tmp_lane = ws->this_lane;
        tmp_lane_select = ws->lane_select; 
        for(j = pc->lane_num; j < size + pc->lane_num; j++) {
            ws->this_lane = j;
            switch(j) {
                case 0: ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                case 1: ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                case 2: ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                case 3: ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
            default:
                break;
            }
            tmp_value = 0;
            SOC_IF_ERROR_RETURN
                (_phy_wcmod_control_prbs_decouple_tx_polynomial_get(ws, &tmp_value));
            if (size == 1) {
                *value = tmp_value;
            } else {
                *value |= tmp_value << (2 * j);
            }
       }
       ws->this_lane = tmp_lane;
       ws->lane_select =  tmp_lane_select;
    } 
    return(SOC_E_NONE);
}


int
_phy_wcmod_control_prbs_decouple_rx_enable_get(wcmod_st *ws, uint32 *value)
{
    int rv;
    
    ws->diag_type = WCMOD_DIAG_PRBS_DECOUPLE_RX;
    ws->per_lane_control = 0x1;

    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
    *value = ws->accData;
    return SOC_E_NONE;
}

STATIC
int phy_wcmod_control_prbs_decouple_rx_enable_get(int unit, soc_port_t port, uint32 *value)
{
    phy_ctrl_t *pc;
    wcmod_st  *temp_ws,  *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int size, j;
    uint32 tmp_value;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
      (_wcmod_phy_parameter_copy(pc, ws));

    *value = 0; 
    if ((SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) || (SOC_INFO(unit).port_num_lanes[pc->port] >= 10)) {
        int i, tmpLane, lane_num = 0, num_core = 0;
        
        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
            num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
        } else {
            num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        }

        for (i = 0 ; i < num_core; i++) {
             temp_ws   =  (wcmod_st *)( ws + i);
            /*temp_id = temp_ws->phy_ad;
            SOC_IF_ERROR_RETURN(_wcmod_phy_parameter_copy(pc, temp_ws));
            temp_ws->phy_ad = temp_id; */
            size = temp_ws->num_of_lane;
            tmpLane = temp_ws->this_lane;
            for(j = tmpLane; j < tmpLane + size; j++) {
                temp_ws->this_lane = j;
                switch(j) {
                    case 0: temp_ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                    case 1: temp_ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                    case 2: temp_ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                    case 3: temp_ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
                default:
                    break;
                }
                tmp_value = 0;
                SOC_IF_ERROR_RETURN
                    (_phy_wcmod_control_prbs_decouple_rx_enable_get(temp_ws, &tmp_value));
                *value |= tmp_value << (lane_num);
                lane_num++;
           }
           temp_ws->lane_select = WCMOD_LANE_0_0_0_1;
           temp_ws->this_lane = 0;
        }
    } else {
        int tmp_lane, tmp_lane_select;
        size = SOC_INFO(unit).port_num_lanes[pc->port];
        tmp_lane = ws->this_lane;
        tmp_lane_select = ws->lane_select; 
        for(j = pc->lane_num; j < pc->lane_num + size; j++) {
            ws->this_lane = j;
            switch(j) {
                case 0: ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                case 1: ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                case 2: ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                case 3: ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
            default:
                break;
            }
            tmp_value = 0;
            SOC_IF_ERROR_RETURN
                (_phy_wcmod_control_prbs_decouple_rx_enable_get(ws, &tmp_value));
            if (size == 1) {
                *value = tmp_value;
            } else {
                *value |= tmp_value << j;
            }
       }
       ws->this_lane = tmp_lane;
       ws->lane_select =  tmp_lane_select;
    } 

    return(SOC_E_NONE);
}


int
_phy_wcmod_control_prbs_decouple_rx_invert_data_get(wcmod_st *ws, uint32 *value)
{
    int rv;
    
    ws->diag_type = WCMOD_DIAG_PRBS_DECOUPLE_RX;
    ws->per_lane_control = 0x2;

    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
    *value = ws->accData;
    return SOC_E_NONE;
}

STATIC
int phy_wcmod_control_prbs_decouple_rx_invert_data_get(int unit, soc_port_t port, uint32 *value)
{
    phy_ctrl_t *pc;
    wcmod_st  *temp_ws,  *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int size, j;
    uint32 tmp_value;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
      (_wcmod_phy_parameter_copy(pc, ws));

    *value = 0; 
    if ((SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) || (SOC_INFO(unit).port_num_lanes[pc->port] >= 10)) {
        int i, tmpLane, lane_num = 0, num_core = 0;
        
        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
            num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
        } else {
            num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        }

        for (i = 0 ; i < num_core; i++) {
             temp_ws   =  (wcmod_st *)( ws + i);
            /*temp_id = temp_ws->phy_ad;
            SOC_IF_ERROR_RETURN(_wcmod_phy_parameter_copy(pc, temp_ws));
            temp_ws->phy_ad = temp_id; */
            size = temp_ws->num_of_lane;
            tmpLane = temp_ws->this_lane;
            for(j = tmpLane; j < tmpLane + size; j++) {
                temp_ws->this_lane = j;
                switch(j) {
                    case 0: temp_ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                    case 1: temp_ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                    case 2: temp_ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                    case 3: temp_ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
                default:
                    break;
                }
                tmp_value = 0;
                SOC_IF_ERROR_RETURN
                    (_phy_wcmod_control_prbs_decouple_rx_invert_data_get(temp_ws, &tmp_value));
                *value |= tmp_value << (lane_num);
                lane_num++;
           }
           temp_ws->lane_select = WCMOD_LANE_0_0_0_1;
           temp_ws->this_lane = 0;
        }
    } else {
        int tmp_lane, tmp_lane_select;
        size = SOC_INFO(unit).port_num_lanes[pc->port];
        tmp_lane = ws->this_lane;
        tmp_lane_select = ws->lane_select; 
        for(j = pc->lane_num; j < pc->lane_num + size; j++) {
            ws->this_lane = j;
            switch(j) {
                case 0: ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                case 1: ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                case 2: ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                case 3: ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
            default:
                break;
            }
            tmp_value = 0;
            SOC_IF_ERROR_RETURN
                (_phy_wcmod_control_prbs_decouple_rx_invert_data_get(ws, &tmp_value));
            if (size == 1) {
                *value = tmp_value;
            } else {
                *value |= tmp_value << j;
            }
       }
       ws->this_lane = tmp_lane;
       ws->lane_select =  tmp_lane_select;
    } 
    return(SOC_E_NONE);
}


int
_phy_wcmod_control_prbs_decouple_rx_polynomial_get(wcmod_st *ws, uint32 *value)
{
    int rv;
    
    ws->diag_type = WCMOD_DIAG_PRBS_DECOUPLE_RX;
    ws->per_lane_control = 0x4;

    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
    *value = ws->accData;
    return SOC_E_NONE;
}

STATIC
int phy_wcmod_control_prbs_decouple_rx_polynomial_get(int unit, soc_port_t port, uint32 *value)
{
    phy_ctrl_t *pc;
    wcmod_st  *temp_ws,  *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int size, j;
    uint32 tmp_value;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
      (_wcmod_phy_parameter_copy(pc, ws));

    *value = 0; 
    if ((SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) || (SOC_INFO(unit).port_num_lanes[pc->port] >= 10)) {
        int i, tmpLane, lane_num = 0, num_core = 0;
        
        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
            num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
        } else {
            num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        }

        for (i = 0 ; i < num_core; i++) {
             temp_ws   =  (wcmod_st *)( ws + i);
           /* temp_id = temp_ws->phy_ad;
            SOC_IF_ERROR_RETURN(_wcmod_phy_parameter_copy(pc, temp_ws));
            temp_ws->phy_ad = temp_id;*/
            size = temp_ws->num_of_lane;
            tmpLane = temp_ws->this_lane;
            for(j = tmpLane; j < tmpLane + size; j++) {
                temp_ws->this_lane = j;
                switch(j) {
                    case 0: temp_ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                    case 1: temp_ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                    case 2: temp_ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                    case 3: temp_ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
                default:
                    break;
                }
                tmp_value = 0;
                SOC_IF_ERROR_RETURN
                    (_phy_wcmod_control_prbs_decouple_rx_polynomial_get(temp_ws, &tmp_value));
                *value |= tmp_value << (2 * (lane_num));
                lane_num++;
           }
           temp_ws->lane_select = WCMOD_LANE_0_0_0_1;
           temp_ws->this_lane = 0;
        }
    } else {
        int tmp_lane, tmp_lane_select;
        size = SOC_INFO(unit).port_num_lanes[pc->port];
        tmp_lane = ws->this_lane;
        tmp_lane_select = ws->lane_select; 
        for(j = pc->lane_num; j < size + pc->lane_num; j++) {
            ws->this_lane = j;
            switch(j) {
                case 0: ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                case 1: ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                case 2: ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                case 3: ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
            default:
                break;
            }
            tmp_value = 0;
            SOC_IF_ERROR_RETURN
                (_phy_wcmod_control_prbs_decouple_rx_polynomial_get(ws, &tmp_value));
            if (size == 1) {
                *value = tmp_value;
            } else {
                *value |= tmp_value << (2 * j);
            }
       }
       ws->this_lane = tmp_lane;
       ws->lane_select =  tmp_lane_select;
    } 
    return(SOC_E_NONE);
}

int
_phy_wcmod_control_prbs_polynomial_set(wcmod_st *ws, uint32 value)
{
    uint16 data16 = 0;
    int rv;
    uint32 temp1, temp2;


    SOC_IF_ERROR_RETURN
        (_phy_wcmod_control_prbs_tx_invert_data_get(ws,&temp1));
    temp1 <<= XGXSBLK1_LANEPRBS_PRBS_INV0_SHIFT;

    SOC_IF_ERROR_RETURN
        (_phy_wcmod_control_prbs_enable_get(ws,&temp2));
    temp2 <<= XGXSBLK1_LANEPRBS_PRBS_EN0_SHIFT; 

    value  = value & XGXSBLK1_LANEPRBS_PRBS_ORDER0_MASK ; 
    value |= temp1 | temp2;
    data16 =(ws->this_lane) * 8;
    ws->per_lane_control = value<<data16;

    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("PRBS_CONTROL", ws, &rv));

    return SOC_E_NONE;
}

STATIC
int phy_wcmod_control_prbs_polynomial_set(int unit, soc_port_t port, uint32 value)
{
    phy_ctrl_t *pc;
    wcmod_st  *temp_ws,  *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int size, j, port_is_100g;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
      (_wcmod_phy_parameter_copy(pc, ws));

    SOC_IF_ERROR_RETURN (_wcmod_getRevDetails(ws));

    if ((ws->model_type) == WCMOD_XN) {
        return SOC_E_UNAVAIL;
    }

    port_is_100g = 0;
    if ((SOC_INFO(unit).port_num_lanes[pc->port] == 10) && !(SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port))) {
        port_is_100g = 1;
    }
    if (IS_MULTI_CORE_PORT(pc)) {
        int i, num_core, tmpLane;
        /*check the special ILKN port */
        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
            num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
        } else {
            num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        }

        for (i = 0 ; i < num_core; i++) {
            temp_ws   =  (wcmod_st *)( ws + i);
            size = temp_ws->num_of_lane;
            tmpLane = temp_ws->this_lane;
            if (port_is_100g) {
                size = 4;
                tmpLane = 0;
            }
            for(j = tmpLane; j < tmpLane + size; j++) {
                temp_ws->this_lane = j;
                switch(j) {
                    case 0: temp_ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                    case 1: temp_ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                    case 2: temp_ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                    case 3: temp_ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
                default:
                    break;
                }
                SOC_IF_ERROR_RETURN
                    (_phy_wcmod_control_prbs_polynomial_set(temp_ws, value));
            }
        }
    } else {
        int tmp_lane, tmp_lane_select;
        size = SOC_INFO(unit).port_num_lanes[pc->port];
        tmp_lane = ws->this_lane;
        tmp_lane_select = ws->lane_select; 
        for(j = pc->lane_num; j < size + pc->lane_num; j++) {
            ws->this_lane = j;
            switch(j) {
                case 0: ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                case 1: ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                case 2: ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                case 3: ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
            default:
                break;
            }
            SOC_IF_ERROR_RETURN
                (_phy_wcmod_control_prbs_polynomial_set(ws, value));
       }
       ws->this_lane = tmp_lane;
       ws->lane_select =  tmp_lane_select;
    } 
    return(SOC_E_NONE);
}

STATIC
int phy_wcmod_per_lane_control_prbs_polynomial_set(int unit, soc_port_t port, int lane, uint32 value)
{
    int lane_num, core_num, max_core_num ;
    wcmod_st    *ws, *temp_ws;
    WCMOD_DEV_DESC_t     *pDesc;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
      (_wcmod_phy_parameter_copy(pc, ws));

    if ((ws->model_type) == WCMOD_XN) {
        return SOC_E_UNAVAIL;
    }

    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
        /*next to check if 4 or 8 lane port */
        if (SOC_INFO(unit).port_num_lanes[pc->port] == 4) {
            max_core_num = 2;
            core_num = lane / 2;
            lane_num = 2 + lane % 2;
        } else {
            max_core_num = 3;
            if (lane < 2) {
                core_num = 0;
                lane_num = 2 + lane % 2 ;
            } else if (lane >= 6) {
                core_num = 2;
                lane_num = 2 + lane % 2;
            } else {
                core_num = 1;
                lane_num = (lane + 2) % 4;
            }
        }
        temp_ws =  (wcmod_st *)( ws + core_num);
    } else {
        max_core_num = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        lane_num = (pc->lane_num + lane) % 4;
        core_num = (pc->lane_num + lane) / 4;
        temp_ws =  (wcmod_st *)( ws + core_num);
    }

    if( core_num >= max_core_num ) { return SOC_E_PARAM; }

    switch(lane_num) {
    case 0: temp_ws->lane_select = WCMOD_LANE_0_0_0_1 ; break ;
    case 1: temp_ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
    case 2: temp_ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
    case 3: temp_ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
    }     
    temp_ws->this_lane = lane_num ;

    SOC_IF_ERROR_RETURN
            (_phy_wcmod_control_prbs_polynomial_set(temp_ws, value));   

   return SOC_E_NONE ;
}

int
_phy_wcmod_control_prbs_polynomial_get(wcmod_st *ws, uint32 *value)
{
    int rv;
    
    ws->diag_type = WCMOD_DIAG_PRBS;

    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
    *value = (ws->accData) & XGXSBLK1_LANEPRBS_PRBS_ORDER0_MASK;
    return SOC_E_NONE;
}

STATIC
int phy_wcmod_control_prbs_polynomial_get(int unit, soc_port_t port, uint32 *value)
{
    phy_ctrl_t *pc;
    wcmod_st  *temp_ws,  *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int size, j;
    uint32 tmp_value;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
      (_wcmod_phy_parameter_copy(pc, ws));

    SOC_IF_ERROR_RETURN (_wcmod_getRevDetails(ws));

    if ((ws->model_type) == WCMOD_XN) {
        *value = 0;
        return SOC_E_UNAVAIL;
    }

    *value = 0; 
    if ((SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) || (SOC_INFO(unit).port_num_lanes[pc->port] >= 10)) {
        int i, tmpLane, lane_num = 0, num_core = 0;
        
        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
            num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
        } else {
            num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        }

        for (i = 0 ; i < num_core; i++) {
             temp_ws   =  (wcmod_st *)( ws + i);
            /*temp_id = temp_ws->phy_ad;
            SOC_IF_ERROR_RETURN(_wcmod_phy_parameter_copy(pc, temp_ws));
            temp_ws->phy_ad = temp_id; */
            size = temp_ws->num_of_lane;
            tmpLane = temp_ws->this_lane;
            for(j = tmpLane; j < tmpLane + size; j++) {
                temp_ws->this_lane = j;
                switch(j) {
                    case 0: temp_ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                    case 1: temp_ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                    case 2: temp_ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                    case 3: temp_ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
                default:
                    break;
                }
                tmp_value = 0;
                SOC_IF_ERROR_RETURN
                    (_phy_wcmod_control_prbs_polynomial_get(temp_ws, &tmp_value));
                *value |= tmp_value << (2 * (lane_num));
                lane_num++;
           }
           temp_ws->lane_select = WCMOD_LANE_0_0_0_1;
           temp_ws->this_lane = 0;
        }
    } else {
        int tmp_lane, tmp_lane_select;
        size = SOC_INFO(unit).port_num_lanes[pc->port];
        tmp_lane = ws->this_lane;
        tmp_lane_select = ws->lane_select; 
        for(j = pc->lane_num; j < size + pc->lane_num; j++) {
            ws->this_lane = j;
            switch(j) {
                case 0: ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                case 1: ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                case 2: ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                case 3: ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
            default:
                break;
            }
            tmp_value = 0;
            SOC_IF_ERROR_RETURN
                (_phy_wcmod_control_prbs_polynomial_get(ws, &tmp_value));
            if (size == 1) {
                *value = tmp_value;
            } else {
                *value |= tmp_value << (2 * j);
            }
       }
       ws->this_lane = tmp_lane;
       ws->lane_select =  tmp_lane_select;
    } 
    return(SOC_E_NONE);
}

STATIC
int phy_wcmod_per_lane_control_prbs_polynomial_get(int unit, soc_port_t port, int lane, uint32 *value)
{
    int lane_num, core_num, max_core_num ;
    wcmod_st    *ws, *temp_ws;
    WCMOD_DEV_DESC_t     *pDesc;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
      (_wcmod_phy_parameter_copy(pc, ws));

    SOC_IF_ERROR_RETURN (_wcmod_getRevDetails(ws));

    if ((ws->model_type) == WCMOD_XN) {
        *value = 0;
        return SOC_E_UNAVAIL;
    }

    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
        /*next to check if 4 or 8 lane port */
        if (SOC_INFO(unit).port_num_lanes[pc->port] == 4) {
            max_core_num = 2;
            core_num = lane / 2;
            lane_num = 2 + lane % 2;
        } else {
            max_core_num = 3;
            if (lane < 2) {
                core_num = 0;
                lane_num = 2 + lane % 2 ;
            } else if (lane >= 6) {
                core_num = 2;
                lane_num = 2 + lane % 2;
            } else {
                core_num = 1;
                lane_num = (lane + 2) % 4;
            }
        }
        temp_ws =  (wcmod_st *)( ws + core_num);
    } else {
        max_core_num = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        lane_num = (pc->lane_num + lane) % 4;
        core_num = (pc->lane_num + lane) / 4;
        temp_ws =  (wcmod_st *)( ws + core_num);
    }

    if( core_num >= max_core_num ) { return SOC_E_PARAM; }

    switch(lane_num) {
    case 0: temp_ws->lane_select = WCMOD_LANE_0_0_0_1 ; break ;
    case 1: temp_ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
    case 2: temp_ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
    case 3: temp_ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
    }     
    temp_ws->this_lane = lane_num ;

    SOC_IF_ERROR_RETURN
            (_phy_wcmod_control_prbs_polynomial_get(temp_ws, value));

   return SOC_E_NONE ;
}


#define INV_SHIFTER(ln)  (PHY_WCMOD_LANEPRBS_LANE_SHIFT * (ln))

int
_phy_wcmod_control_prbs_tx_invert_data_set(wcmod_st *ws, uint32 value)
{
    uint16 data16 = 0;
    int rv;
    uint32 temp1, temp2;

    SOC_IF_ERROR_RETURN
        (_phy_wcmod_control_prbs_polynomial_get(ws, &temp1));

    SOC_IF_ERROR_RETURN
        (_phy_wcmod_control_prbs_enable_get(ws,&temp2));
    temp2 <<= XGXSBLK1_LANEPRBS_PRBS_EN0_SHIFT; 

    value <<= XGXSBLK1_LANEPRBS_PRBS_INV0_SHIFT;
    value |= temp1 | temp2;  

    data16 =(ws->this_lane) * 8 ;
    ws->per_lane_control = value << data16;
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("PRBS_CONTROL", ws, &rv));

    return SOC_E_NONE;
}


STATIC
int phy_wcmod_control_prbs_tx_invert_data_set(int unit, soc_port_t port, uint32 value)
{
    phy_ctrl_t *pc;
    wcmod_st  *temp_ws,  *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int size, j, port_is_100g;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
      (_wcmod_phy_parameter_copy(pc, ws));

    SOC_IF_ERROR_RETURN (_wcmod_getRevDetails(ws));

    if ((ws->model_type) == WCMOD_XN) {
        return SOC_E_UNAVAIL;
    }

    port_is_100g = 0;
    if ((SOC_INFO(unit).port_num_lanes[pc->port] == 10) && !(SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port))) {
        port_is_100g = 1;
    }
    if (IS_MULTI_CORE_PORT(pc)) {
        int i, num_core, tmpLane;
        /*check the special ILKN port */
        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
            num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
        } else {
            num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        }

        for (i = 0 ; i < num_core; i++) {
            temp_ws   =  (wcmod_st *)( ws + i);
            size = temp_ws->num_of_lane;
            tmpLane = temp_ws->this_lane;
            if (port_is_100g) {
                size = 4;
                tmpLane = 0;
            }
            for(j = tmpLane; j < tmpLane + size; j++) {
                temp_ws->this_lane = j;
                switch(j) {
                    case 0: temp_ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                    case 1: temp_ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                    case 2: temp_ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                    case 3: temp_ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
                default:
                    break;
                }
                SOC_IF_ERROR_RETURN
                    (_phy_wcmod_control_prbs_tx_invert_data_set(temp_ws, value));
            }
        }
    } else {
        int tmp_lane, tmp_lane_select;
        size = SOC_INFO(unit).port_num_lanes[pc->port];
        tmp_lane = ws->this_lane;
        tmp_lane_select = ws->lane_select; 
        for(j = pc->lane_num; j < pc->lane_num + size ; j++) {
            ws->this_lane = j;
            switch(j) {
                case 0: ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                case 1: ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                case 2: ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                case 3: ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
            default:
                break;
            }
            SOC_IF_ERROR_RETURN
                (_phy_wcmod_control_prbs_tx_invert_data_set(ws, value));
       }
       ws->this_lane = tmp_lane;
       ws->lane_select =  tmp_lane_select;
    } 
    return(SOC_E_NONE);
}

STATIC
int phy_wcmod_per_lane_control_prbs_tx_invert_data_set(int unit, soc_port_t port, int lane, uint32 value)
{
    int lane_num, core_num, max_core_num ;
    wcmod_st    *ws, *temp_ws;
    WCMOD_DEV_DESC_t     *pDesc;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
      (_wcmod_phy_parameter_copy(pc, ws));

    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
        /*next to check if 4 or 8 lane port */
        if (SOC_INFO(unit).port_num_lanes[pc->port] == 4) {
            max_core_num = 2;
            core_num = lane / 2;
            lane_num = 2 + lane % 2;
        } else {
            max_core_num = 3;
            if (lane < 2) {
                core_num = 0;
                lane_num = 2 + lane % 2 ;
            } else if (lane >= 6) {
                core_num = 2;
                lane_num = 2 + lane % 2;
            } else {
                core_num = 1;
                lane_num = (lane + 2) % 4;
            }
        }
        temp_ws =  (wcmod_st *)( ws + core_num);
    } else {
        max_core_num = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        lane_num = (pc->lane_num + lane) % 4;
        core_num = (pc->lane_num + lane) / 4;
        temp_ws =  (wcmod_st *)( ws + core_num);
    }

    if( core_num >= max_core_num ) { return SOC_E_PARAM; }

    switch(lane_num) {
    case 0: temp_ws->lane_select = WCMOD_LANE_0_0_0_1 ; break ;
    case 1: temp_ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
    case 2: temp_ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
    case 3: temp_ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
    }     
    temp_ws->this_lane = lane_num ;


   SOC_IF_ERROR_RETURN
      (_phy_wcmod_control_prbs_tx_invert_data_set(temp_ws, value));

   return(SOC_E_NONE);
}


int
_phy_wcmod_control_prbs_tx_invert_data_get(wcmod_st *ws,uint32 *value)
{
    int rv;

    ws->diag_type = WCMOD_DIAG_PRBS;

    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
    *value = (ws->accData) & XGXSBLK1_LANEPRBS_PRBS_INV0_MASK;
    *value >>= XGXSBLK1_LANEPRBS_PRBS_INV0_SHIFT;
    return SOC_E_NONE;
}

STATIC
int phy_wcmod_control_prbs_tx_invert_data_get(int unit, soc_port_t port, uint32 *value)
{
    phy_ctrl_t *pc;
    wcmod_st  *temp_ws,  *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int size, j;
    uint32 tmp_value;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
      (_wcmod_phy_parameter_copy(pc, ws));

    SOC_IF_ERROR_RETURN (_wcmod_getRevDetails(ws));

    if ((ws->model_type) == WCMOD_XN) {
        *value = 0;
        return SOC_E_UNAVAIL;
    }

    *value = 0; 
    if ((SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) || (SOC_INFO(unit).port_num_lanes[pc->port] >= 10)) {
        int i, tmpLane, lane_num = 0, num_core = 0;
        
        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
            num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
        } else {
            num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        }

        for (i = 0 ; i < num_core; i++) {
             temp_ws   =  (wcmod_st *)( ws + i);
            /*temp_id = temp_ws->phy_ad;
            SOC_IF_ERROR_RETURN(_wcmod_phy_parameter_copy(pc, temp_ws));
            temp_ws->phy_ad = temp_id; */
            size = temp_ws->num_of_lane;
            tmpLane = temp_ws->this_lane;
            for(j = tmpLane; j < tmpLane + size; j++) {
                temp_ws->this_lane = j;
                switch(j) {
                    case 0: temp_ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                    case 1: temp_ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                    case 2: temp_ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                    case 3: temp_ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
                default:
                    break;
                }
                tmp_value = 0;
                SOC_IF_ERROR_RETURN
                    (_phy_wcmod_control_prbs_tx_invert_data_get(temp_ws, &tmp_value));
                *value |= tmp_value << (lane_num);
                lane_num++;
           }
           temp_ws->lane_select = WCMOD_LANE_0_0_0_1;
           temp_ws->this_lane = 0;
        }
    } else {
        int tmp_lane, tmp_lane_select;
        size = SOC_INFO(unit).port_num_lanes[pc->port];
        tmp_lane = ws->this_lane;
        tmp_lane_select = ws->lane_select; 
        for(j = pc->lane_num; j < pc->lane_num + size; j++) {
            ws->this_lane = j;
            switch(j) {
                case 0: ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                case 1: ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                case 2: ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                case 3: ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
            default:
                break;
            }
            tmp_value = 0;
            SOC_IF_ERROR_RETURN
                (_phy_wcmod_control_prbs_tx_invert_data_get(ws, &tmp_value));
            if (size == 1) {
                *value = tmp_value;
            } else {
                *value |= tmp_value << j;
            }
       }
       ws->this_lane = tmp_lane;
       ws->lane_select =  tmp_lane_select;
    } 
    return(SOC_E_NONE);
}

STATIC
int phy_wcmod_per_lane_control_prbs_tx_invert_data_get(int unit, soc_port_t port, int lane, uint32 *value)
{
    int lane_num, core_num, max_core_num ;
    wcmod_st    *ws, *temp_ws;
    WCMOD_DEV_DESC_t     *pDesc;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
      (_wcmod_phy_parameter_copy(pc, ws));

    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
        /*next to check if 4 or 8 lane port */
        if (SOC_INFO(unit).port_num_lanes[pc->port] == 4) {
            max_core_num = 2;
            core_num = lane / 2;
            lane_num = 2 + lane % 2;
        } else {
            max_core_num = 3;
            if (lane < 2) {
                core_num = 0;
                lane_num = 2 + lane % 2 ;
            } else if (lane >= 6) {
                core_num = 2;
                lane_num = 2 + lane % 2;
            } else {
                core_num = 1;
                lane_num = (lane + 2) % 4;
            }
        }
        temp_ws =  (wcmod_st *)( ws + core_num);
    } else {
        max_core_num = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        lane_num = (pc->lane_num + lane) % 4;
        core_num = (pc->lane_num + lane) / 4;
        temp_ws =  (wcmod_st *)( ws + core_num);
    }

    if( core_num >= max_core_num ) { return SOC_E_PARAM; }

    switch(lane_num) {
    case 0: temp_ws->lane_select = WCMOD_LANE_0_0_0_1 ; break ;
    case 1: temp_ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
    case 2: temp_ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
    case 3: temp_ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
    }     
    temp_ws->this_lane = lane_num ;

    SOC_IF_ERROR_RETURN
            (_phy_wcmod_control_prbs_tx_invert_data_get(temp_ws, value));

    return(SOC_E_NONE);
}


#define PRBS_LANES_MASK  (XGXSBLK1_LANEPRBS_PRBS_EN3_MASK | \
                XGXSBLK1_LANEPRBS_PRBS_EN2_MASK | \
                XGXSBLK1_LANEPRBS_PRBS_EN1_MASK | \
                XGXSBLK1_LANEPRBS_PRBS_EN0_MASK)

/* PRBS test
 * this routine enables the PRBS generator on applicable lanes depending on
 * the port mode. Before calling this function, a forced speed mode should
 * be set and either the external loopback or internal loopback should be
 * configured. Once this function is called, application should wait to
 * let the test run for a while and then calls the 
 * _phy_wcmod_control_prbs_rx_status_get() to retrieve PRBS test status.
 * When calling this function to disable the PRBS test, the device or 
 * specific lane will be re-initialized.  
 */

int
_phy_wcmod_control_prbs_enable_set(wcmod_st *ws, uint32 enable)
{
    uint16 data16 = 0;
    int rv;
    uint32 temp1, temp2;

    SOC_IF_ERROR_RETURN
        (_phy_wcmod_control_prbs_polynomial_get(ws, &temp1));

    SOC_IF_ERROR_RETURN
        (_phy_wcmod_control_prbs_tx_invert_data_get(ws,&temp2));

    temp2<<= XGXSBLK1_LANEPRBS_PRBS_INV0_SHIFT;
    enable <<= XGXSBLK1_LANEPRBS_PRBS_EN0_SHIFT;
    enable |= temp1 | temp2;
    data16 =(ws->this_lane) * 8;
    ws->per_lane_control = enable << data16;
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("PRBS_CONTROL", ws, &rv));

    return SOC_E_NONE;
}

STATIC
int phy_wcmod_control_prbs_enable_set(int unit, soc_port_t port, uint32 enable)
{
    phy_ctrl_t *pc;
    wcmod_st  *temp_ws,  *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int size, j, port_is_100g;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
      (_wcmod_phy_parameter_copy(pc, ws));

    SOC_IF_ERROR_RETURN (_wcmod_getRevDetails(ws));

    if ((ws->model_type) == WCMOD_XN) {
        return SOC_E_UNAVAIL;
    }

    port_is_100g = 0;
    if ((SOC_INFO(unit).port_num_lanes[pc->port] == 10) && !(SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port))) {
        port_is_100g = 1;
    }

    if (IS_MULTI_CORE_PORT(pc)) {
        int i, num_core, tmpLane;
        /*check the special ILKN port */
        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
            num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
        } else {
            num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        }

        for (i = 0 ; i < num_core; i++) {
            temp_ws   =  (wcmod_st *)( ws + i);
            size = temp_ws->num_of_lane;
            tmpLane = temp_ws->this_lane;
            if (port_is_100g) {
                size = 4;
                tmpLane = 0;
            }
            for(j = tmpLane; j < tmpLane + size; j++) {
                temp_ws->this_lane = j;
                switch(j) {
                    case 0: temp_ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                    case 1: temp_ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                    case 2: temp_ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                    case 3: temp_ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
                default:
                    break;
                }
                SOC_IF_ERROR_RETURN
                    (_phy_wcmod_control_prbs_enable_set(temp_ws, enable));
            }
        }
    } else {
        int tmp_lane, tmp_lane_select;
        size = SOC_INFO(unit).port_num_lanes[pc->port];
        tmp_lane = ws->this_lane;
        tmp_lane_select = ws->lane_select; 
        for(j = pc->lane_num; j < pc->lane_num + size; j++) {
            ws->per_lane_control = enable;
            ws->this_lane = j;
            switch(j) {
                case 0: ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                case 1: ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                case 2: ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                case 3: ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
            default:
                break;
            }
            SOC_IF_ERROR_RETURN
                (_phy_wcmod_control_prbs_enable_set(ws, enable));
       }
       ws->this_lane = tmp_lane;
       ws->lane_select =  tmp_lane_select;
    } 
    return(SOC_E_NONE);
}

STATIC
int phy_wcmod_per_lane_control_prbs_enable_set(int unit, soc_port_t port, int lane, uint32 enable)
{
    int lane_num, core_num, max_core_num ;
    wcmod_st    *ws, *temp_ws;
    WCMOD_DEV_DESC_t     *pDesc;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
      (_wcmod_phy_parameter_copy(pc, ws));

    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
        /*next to check if 4 or 8 lane port */
        if (SOC_INFO(unit).port_num_lanes[pc->port] == 4) {
            max_core_num = 2;
            core_num = lane / 2;
            lane_num = 2 + lane % 2;
        } else {
            max_core_num = 3;
            if (lane < 2) {
                core_num = 0;
                lane_num = 2 + lane % 2 ;
            } else if (lane >= 6) {
                core_num = 2;
                lane_num = 2 + lane % 2;
            } else {
                core_num = 1;
                lane_num = (lane + 2) % 4;
            }
        }
        temp_ws =  (wcmod_st *)( ws + core_num);
    } else {
        max_core_num = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        lane_num = (pc->lane_num + lane) % 4;
        core_num = (pc->lane_num + lane) / 4;
        temp_ws =  (wcmod_st *)( ws + core_num);
    }

    if( core_num >= max_core_num ) { return SOC_E_PARAM; }

    switch(lane_num) {
    case 0: temp_ws->lane_select = WCMOD_LANE_0_0_0_1 ; break ;
    case 1: temp_ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
    case 2: temp_ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
    case 3: temp_ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
    }     
    temp_ws->this_lane = lane_num ;

    SOC_IF_ERROR_RETURN
            (_phy_wcmod_control_prbs_enable_set(temp_ws, enable));

    return(SOC_E_NONE);
}

int
_phy_wcmod_control_prbs_enable_get(wcmod_st *ws, uint32 *value)
{
    int rv;

    ws->diag_type = WCMOD_DIAG_PRBS;

    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
    *value = (ws->accData) & XGXSBLK1_LANEPRBS_PRBS_EN0_MASK;
    *value >>= XGXSBLK1_LANEPRBS_PRBS_EN0_SHIFT;
    return SOC_E_NONE;
}

STATIC
int phy_wcmod_control_prbs_enable_get(int unit, soc_port_t port, uint32 *value)
{
    phy_ctrl_t *pc;
    wcmod_st  *temp_ws,  *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int size, j;
    uint32 tmp_value;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
      (_wcmod_phy_parameter_copy(pc, ws));

    SOC_IF_ERROR_RETURN (_wcmod_getRevDetails(ws));

    if ((ws->model_type) == WCMOD_XN) {
        *value = 0;
        return SOC_E_UNAVAIL;
    }

    *value = 0; 
    if ((SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) || (SOC_INFO(unit).port_num_lanes[pc->port] >= 10)) {
        int i, tmpLane, lane_num = 0, num_core = 0;

        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
            num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
        } else {
            num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        }

        for (i = 0 ; i < num_core; i++) {
             temp_ws   =  (wcmod_st *)( ws + i);
            /*temp_id = temp_ws->phy_ad;
            SOC_IF_ERROR_RETURN(_wcmod_phy_parameter_copy(pc, temp_ws));
            temp_ws->phy_ad = temp_id;*/
            size = temp_ws->num_of_lane;
            tmpLane = temp_ws->this_lane;
            for(j = tmpLane; j < tmpLane + size; j++) {
                temp_ws->this_lane = j;
                switch(j) {
                    case 0: temp_ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                    case 1: temp_ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                    case 2: temp_ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                    case 3: temp_ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
                default:
                    break;
                }
                tmp_value = 0;
                SOC_IF_ERROR_RETURN
                    (_phy_wcmod_control_prbs_enable_get(temp_ws, &tmp_value));
                *value |= tmp_value << (lane_num);
                lane_num++;
           }
           temp_ws->lane_select = WCMOD_LANE_0_0_0_1;
           temp_ws->this_lane = 0;
        }
    } else {
        int tmp_lane, tmp_lane_select;
        size = SOC_INFO(unit).port_num_lanes[pc->port];
        tmp_lane = ws->this_lane;
        tmp_lane_select = ws->lane_select; 
        for(j = pc->lane_num; j < pc->lane_num + size; j++) {
            ws->this_lane = j;
            switch(j) {
                case 0: ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                case 1: ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                case 2: ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                case 3: ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
            default:
                break;
            }
            tmp_value = 0;
            SOC_IF_ERROR_RETURN
                (_phy_wcmod_control_prbs_enable_get(ws, &tmp_value));
            if (size == 1) {
                *value = tmp_value;
            } else {
                *value |= tmp_value << j;
            }
       }
       ws->this_lane = tmp_lane;
       ws->lane_select =  tmp_lane_select;
    } 

    return(SOC_E_NONE);
}

STATIC
int phy_wcmod_per_lane_control_prbs_enable_get(int unit, soc_port_t port, int lane, uint32 *value)
{
    int lane_num, core_num, max_core_num ;
    wcmod_st    *ws, *temp_ws;
    WCMOD_DEV_DESC_t     *pDesc;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
      (_wcmod_phy_parameter_copy(pc, ws));

    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
        /*next to check if 4 or 8 lane port */
        if (SOC_INFO(unit).port_num_lanes[pc->port] == 4) {
            max_core_num = 2;
            core_num = lane / 2;
            lane_num = 2 + lane % 2;
        } else {
            max_core_num = 3;
            if (lane < 2) {
                core_num = 0;
                lane_num = 2 + lane % 2 ;
            } else if (lane >= 6) {
                core_num = 2;
                lane_num = 2 + lane % 2;
            } else {
                core_num = 1;
                lane_num = (lane + 2) % 4;
            }
        }
        temp_ws =  (wcmod_st *)( ws + core_num);
    } else {
        max_core_num = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        lane_num = (pc->lane_num + lane) % 4;
        core_num = (pc->lane_num + lane) / 4;
        temp_ws =  (wcmod_st *)( ws + core_num);
    }

    if( core_num >= max_core_num ) { return SOC_E_PARAM; }

    switch(lane_num) {
    case 0: temp_ws->lane_select = WCMOD_LANE_0_0_0_1 ; break ;
    case 1: temp_ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
    case 2: temp_ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
    case 3: temp_ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
    }     
    temp_ws->this_lane = lane_num ;

    SOC_IF_ERROR_RETURN
            (_phy_wcmod_control_prbs_enable_get(temp_ws, value));

    return(SOC_E_NONE);
}

/*
 * Returns value 
 *      ==  0: PRBS receive is in sync
 *      == -1: PRBS receive never got into sync
 *      ==  n: number of errors
 */
int
_phy_wcmod_control_prbs_rx_status_get(wcmod_st *ws, uint32 *value)
{
    int rv;

    SOC_IF_ERROR_RETURN (wcmod_tier1_selector("PRBS_CHECK", ws, &rv));
    *value = ws->accData;
    return SOC_E_NONE;
}

STATIC
int phy_wcmod_control_prbs_rx_status_get(int unit, soc_port_t port, uint32 *value)
{
    phy_ctrl_t *pc;
    wcmod_st  *temp_ws,  *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int size, j, port_is_100g, lane_map;
    uint32 tmp_value;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
      (_wcmod_phy_parameter_copy(pc, ws));

    SOC_IF_ERROR_RETURN (_wcmod_getRevDetails(ws));

    if ((ws->model_type) == WCMOD_XN) {
        *value = 0;
        return SOC_E_UNAVAIL;
    }

    port_is_100g = 0;
    lane_map = 0;
    if ((SOC_INFO(unit).port_num_lanes[pc->port] == 10) && !(SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port))) {
        port_is_100g = 1;
        lane_map = DEV_CFG_PTR(pc)->active_lane_map;
    }

    *value = 0; 
    if ((SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) || (SOC_INFO(unit).port_num_lanes[pc->port] >= 10)) {
        int i, tmpLane, lane_num = 0, num_core = 0;
        
        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
            num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
        } else {
            num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        }

        for (i = 0 ; i < num_core; i++) {
             temp_ws   =  (wcmod_st *)( ws + i);
            /*temp_id = temp_ws->phy_ad;
            SOC_IF_ERROR_RETURN(_wcmod_phy_parameter_copy(pc, temp_ws));
            temp_ws->phy_ad = temp_id;*/
            size = temp_ws->num_of_lane;
            tmpLane = temp_ws->this_lane;
            if (port_is_100g) {
                size = 4;
                tmpLane = 0;
            }
            for(j = tmpLane; j < size + tmpLane; j++) {
                temp_ws->this_lane = j;
                if (port_is_100g) {
                    if (((lane_map >> ((4 * i) + j)) & 0x1) == 0) continue;
                }
                switch(j) {
                    case 0: temp_ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                    case 1: temp_ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                    case 2: temp_ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                    case 3: temp_ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
                default:
                    break;
                }
                tmp_value = 0;
                SOC_IF_ERROR_RETURN
                    (_phy_wcmod_control_prbs_rx_status_get(temp_ws, &tmp_value));
                LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(pc->unit,
                                     "PRBS status: WC %d lane %d: %s in sync, errors %u\n"),
                          i, j, (tmp_value)? "not":"", tmp_value));
                
                *value |= (tmp_value!=0) << (lane_num);
                lane_num++;
           }
           temp_ws->lane_select = WCMOD_LANE_0_0_0_1;
           temp_ws->this_lane = 0;
        }
    } else {
        int tmp_lane, tmp_lane_select;
        size = SOC_INFO(unit).port_num_lanes[pc->port];
        tmp_lane = ws->this_lane;
        tmp_lane_select = ws->lane_select; 
        for(j = pc->lane_num; j < pc->lane_num + size; j++) {
            ws->this_lane = j;
            switch(j) {
                case 0: ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                case 1: ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                case 2: ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                case 3: ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
            default:
                break;
            }
            tmp_value = 0;
            SOC_IF_ERROR_RETURN
                (_phy_wcmod_control_prbs_rx_status_get(ws, &tmp_value));
                    LOG_INFO(BSL_LS_SOC_PHY,
                             (BSL_META_U(pc->unit,
                                         "PRBS status: port %d lane %d: %s in sync, errors %u\n"),
                              ws->port, j, (tmp_value)? "not":"", tmp_value));
            if (size == 1) {
                *value = tmp_value;
            } else {
                *value |= tmp_value << j;
            }
       }
       ws->this_lane = tmp_lane;
       ws->lane_select =  tmp_lane_select;
    } 
    return(SOC_E_NONE);
}

STATIC
int phy_wcmod_per_lane_control_prbs_rx_status_get(int unit, soc_port_t port, int lane, uint32 *value)
{
    int lane_num, core_num, max_core_num ;
    wcmod_st    *ws, *temp_ws;
    WCMOD_DEV_DESC_t     *pDesc;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
      (_wcmod_phy_parameter_copy(pc, ws));

    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
        /*next to check if 4 or 8 lane port */
        if (SOC_INFO(unit).port_num_lanes[pc->port] == 4) {
            max_core_num = 2;
            core_num = lane / 2;
            lane_num = 2 + lane % 2;
        } else {
            max_core_num = 3;
            if (lane < 2) {
                core_num = 0;
                lane_num = 2 + lane % 2 ;
            } else if (lane >= 6) {
                core_num = 2;
                lane_num = 2 + lane % 2;
            } else {
                core_num = 1;
                lane_num = (lane + 2) % 4;
            }
        }
        temp_ws =  (wcmod_st *)( ws + core_num);
    } else {
        max_core_num = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        lane_num = (pc->lane_num + lane) % 4;
        core_num = (pc->lane_num + lane) / 4;
        temp_ws =  (wcmod_st *)( ws + core_num);
    }

    if( core_num >= max_core_num ) { return SOC_E_PARAM; }

    switch(lane_num) {
    case 0: temp_ws->lane_select = WCMOD_LANE_0_0_0_1 ; break ;
    case 1: temp_ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
    case 2: temp_ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
    case 3: temp_ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
    }     
    temp_ws->this_lane = lane_num ;

    SOC_IF_ERROR_RETURN
            (_phy_wcmod_control_prbs_rx_status_get(temp_ws, value));

    return(SOC_E_NONE);
}


STATIC int
_phy_wcmod_control_8b10b_set(int unit, phy_ctrl_t *pc, uint32 value)
{
    return SOC_E_NONE;
}

/* The value format:
 * bit 15:  1 enable forced preemphasis ctrl, 0 auto config
 * bit 14:10:  post_tap value
 * bit 09:04:  main tap value
 * bit 03:00:  pre_tap value 
 usage: need to set proper ws->lane_select prior to call this func if type SOC_PHY_CONTROL_PREEMPHASIS is and no lane specified 
   by soc_phy_control_t type.  By default ws->lane_select is one hot (lane0), even in combo mode  */ 
/* Note that this function always set the TX_FIR_TAP_FORCE bit even tx_driver.u.force=0 */
STATIC int
_phy_wcmod_control_preemphasis_set(int unit, phy_ctrl_t *pc,
                                soc_phy_control_t type, uint32 value)
{
    int tmpLane = 0, preTap = 0, mainTap = 0;
    int postTap = 0, regTap = 0, tmpSelect = 0;
    int rv;
    int i, size, lane_ctrl = 0;
    wcmod_st    *ws, *temp_ws;
    WCMOD_DEV_DESC_t     *pDesc;

    preTap  = 0x1000000;
    
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);
    /* To set the pre,main and post override values, set the values in the
     * first, second and third byte of wcmod_st::per_lane_control respectively,
     * and set bit 0, bit 1, and bit 2 respectively in the last(4th) byte of
     * wcmod_st::per_lane_control Setting bit 3 in the last(4th) byte of
     * wcmod_st::per_lane_control will release the force enable value and undo
     * all the overrides.
     */
    
    tmpSelect = ws->lane_select ;
    tmpLane   = ws->this_lane;

    SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, ws));
    /* discard ws->lane_select from _wcmod_phy_parameter_copy() call */
    /* discard ws->this_lane from _wcmod_phy_parameter_copy() call */
    ws->lane_select = tmpSelect ;
    ws->this_lane   = tmpLane ;

    if (ws->model_type == WCMOD_XN) {
       regTap = value;
    } else {
        /* translate the tx tap value into wcmod format */
        preTap |= value & CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_PRE_MASK;
        mainTap = (value & CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_MAIN_MASK) >> CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_MAIN_SHIFT;
        mainTap = 0x2000000 | (mainTap << 8);
        postTap = (value & CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_POST_MASK) >> CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_POST_SHIFT;
        postTap = 0x4000000 | (postTap << 16);
        regTap  = preTap| mainTap | postTap;
    }

    ws->per_lane_control = regTap;

    if (IS_MULTI_CORE_PORT(pc)) {
        int j, num_core, tmpLane, index = 0;
        /*check the special ILKN port */
        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
            num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
        } else {
            num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        }

        for (i = 0 ; i < num_core; i++) {
            temp_ws   =  (wcmod_st *)( ws + i);
            size = temp_ws->num_of_lane;
            tmpLane = temp_ws->this_lane;
            temp_ws->per_lane_control = regTap;
            for(j = tmpLane; j < tmpLane + size; j++) {
                temp_ws->this_lane = j;
                switch(j) {
                    case 1: temp_ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                    case 2: temp_ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                    case 3: temp_ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
                default:
                    temp_ws->lane_select = WCMOD_LANE_0_0_0_1 ;
                    break;
                }
                SOC_IF_ERROR_RETURN
                   (wcmod_tier1_selector("TX_TAP_CONTROL", temp_ws, &rv));
                DEV_CFG_PTR(pc)->preemph[index] = value;
                index++;
            }
        }
    } else {

        if( IS_DUAL_LANE_PORT(pc)) {  ws->dxgxs     = 0 ; }

        if (type == SOC_PHY_CONTROL_PREEMPHASIS_LANE0) {
            lane_ctrl = LANE0_ACCESS;
            ws->lane_select = WCMOD_LANE_0_0_0_1 ;
            ws->this_lane  = 0;
            SOC_IF_ERROR_RETURN
               (wcmod_tier1_selector("TX_TAP_CONTROL", ws, &rv));
        } else if (type == SOC_PHY_CONTROL_PREEMPHASIS_LANE1) {
            lane_ctrl = LANE1_ACCESS;
            ws->lane_select = WCMOD_LANE_0_0_1_0 ;
            ws->this_lane  = 1;
            SOC_IF_ERROR_RETURN
               (wcmod_tier1_selector("TX_TAP_CONTROL", ws, &rv));
        } else if (type == SOC_PHY_CONTROL_PREEMPHASIS_LANE2) {
            lane_ctrl = LANE2_ACCESS;
            ws->lane_select = WCMOD_LANE_0_1_0_0 ;
            ws->this_lane  = 2;
            SOC_IF_ERROR_RETURN
               (wcmod_tier1_selector("TX_TAP_CONTROL", ws, &rv));
        } else if (type == SOC_PHY_CONTROL_PREEMPHASIS_LANE3) {
            lane_ctrl = LANE3_ACCESS;
            ws->lane_select = WCMOD_LANE_1_0_0_0 ;
            ws->this_lane  = 3;
            SOC_IF_ERROR_RETURN
               (wcmod_tier1_selector("TX_TAP_CONTROL", ws, &rv));
        } else if (type == SOC_PHY_CONTROL_PREEMPHASIS) {
            int j = 0;
            if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
                for(j = 0; j < 4; j++) {
                    ws->per_lane_control = regTap;
                    switch(j) {
                        case 1: ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                        case 2: ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                        case 3: ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
                    default:
                        ws->lane_select = WCMOD_LANE_0_0_0_1 ;
                        break;
                    }
                    SOC_IF_ERROR_RETURN
                       (wcmod_tier1_selector("TX_TAP_CONTROL", ws, &rv));
                    DEV_CFG_PTR(pc)->preemph[j] = value;
               }
                SOC_IF_ERROR_RETURN
                    (WRITE_WC40_AERBLK_AERr(unit,ws,0));
            } else if( IS_DUAL_LANE_PORT(pc)) {
                for(j = ws->this_lane; j < ws->this_lane + 2; j++) {
                    ws->per_lane_control = regTap;
                    switch(j) {
                        case 1: ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                        case 2: ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                        case 3: ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
                    default:
                        ws->lane_select = WCMOD_LANE_0_0_0_1 ;
                        break;
                    }
                    SOC_IF_ERROR_RETURN
                       (wcmod_tier1_selector("TX_TAP_CONTROL", ws, &rv));
                    DEV_CFG_PTR(pc)->preemph[j % 2] = value;
               }
                SOC_IF_ERROR_RETURN
                    (WRITE_WC40_AERBLK_AERr(unit,ws,0));
            } else {
                if(1) 
                switch(ws->this_lane) {
                    case 0: ws->lane_select = WCMOD_LANE_0_0_0_1 ; break ;
                    case 1: ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                    case 2: ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                    case 3: ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
                default:
                    if(ws->model_type == WCMOD_QS_A0) {
                        ws->lane_select = WCMOD_LANE_0_0_0_1_0_0_0_0 ;
                    } else {
                        ws->lane_select = WCMOD_LANE_0_0_0_1 ;
                    }
                    break;
                }
                SOC_IF_ERROR_RETURN
                   (wcmod_tier1_selector("TX_TAP_CONTROL", ws, &rv));
                DEV_CFG_PTR(pc)->preemph[0] = value;
            }
        } else {
            lane_ctrl = TX_DRIVER_DFT_LN_CTRL; 
        }

        if(ws->verbosity >0) 
            printf("%s lane_sel=%x preTag=%0x mainTap=%0x postTap=%0x lane_ctrl=%x\n", FUNCTION_NAME(), ws->lane_select,  preTap, mainTap, postTap, lane_ctrl) ;
        ws->this_lane = tmpLane;
        ws->lane_select = tmpSelect ;
        if(IS_DUAL_LANE_PORT(pc)) {  ws->dxgxs     =  ws->dual_type  ; }
    }
    return SOC_E_NONE;
}

/* usage: need to set proper ws->lane_select prior to call this func if type SOC_PHY_CONTROL_PREEMPHASIS is and no lane specified 
   by soc_phy_control_t type.  By default ws->lane_select is one hot (lane0), even in combo mode  */ 
/* Note that this function always set the TX_FIR_TAP_FORCE bit even tx_driver.u.force=0 */
STATIC int
_phy_wcmod_per_lane_control_preemphasis_set(int unit, soc_port_t port, int lane, uint32 value)
{
    int tmpLane, preTap, mainTap, postTap, regTap, temp_port_type, temp_dxgxs = 0;
    int rv;
    int lane_num, core_num = 0, max_core_num;
    wcmod_st    *ws, *temp_ws;
    WCMOD_DEV_DESC_t     *pDesc;
    phy_ctrl_t *pc;

    preTap  = 0x1000000;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
      (_wcmod_phy_parameter_copy(pc, ws));

    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
        /*next to check if 4 or 8 lane port */
        if (SOC_INFO(unit).port_num_lanes[pc->port] == 4) {
            max_core_num = 2;
            core_num = lane / 2;
            lane_num = 2 + lane % 2;
        } else {
            max_core_num = 3;
            if (lane < 2) {
                core_num = 0;
                lane_num = 2 + lane % 2 ;
            } else if (lane >= 6) {
                core_num = 2;
                lane_num = 2 + lane % 2;
            } else {
                core_num = 1;
                lane_num = (lane + 2) % 4;
            }
        }
        temp_ws =  (wcmod_st *)( ws + core_num);
    } else {
        max_core_num = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        lane_num = (pc->lane_num + lane) % 4;
        core_num = (pc->lane_num + lane) / 4;
        temp_ws =  (wcmod_st *)( ws + core_num);
    }

    if( core_num >= max_core_num ) { return SOC_E_PARAM; }

    /* translate the tx tap value into wcmod format */
    preTap |= value & CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_PRE_MASK;
    mainTap = (value & CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_MAIN_MASK) >> CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_MAIN_SHIFT;
    mainTap = 0x2000000 | (mainTap << 8);
    postTap = (value & CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_POST_MASK) >> CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_POST_SHIFT;
    postTap = 0x4000000 | (postTap << 16);
    regTap  = preTap| mainTap | postTap;

    temp_ws->per_lane_control = regTap;
    switch (lane_num) {
        case 0: 
            temp_ws->lane_select = WCMOD_LANE_0_0_0_1;
            break;
        case 1: 
            temp_ws->lane_select = WCMOD_LANE_0_0_1_0;
            break;
        case 2: 
            temp_ws->lane_select = WCMOD_LANE_0_1_0_0;
            break;
        case 3: 
            temp_ws->lane_select = WCMOD_LANE_1_0_0_0;
            break;
        default:
            break;
    }
    temp_port_type = temp_ws->port_type;
    temp_ws->port_type = WCMOD_INDEPENDENT;
    tmpLane = temp_ws->this_lane;
    temp_ws->this_lane = lane_num;
    SOC_IF_ERROR_RETURN
       (wcmod_tier1_selector("TX_TAP_CONTROL", temp_ws, &rv));
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_AERBLK_AERr(unit,temp_ws,0));
    temp_ws->port_type = temp_port_type;
    temp_ws->this_lane = tmpLane;
    if( IS_DUAL_LANE_PORT(pc)) { 
        temp_ws->dxgxs  = temp_dxgxs;
    }
    
    return SOC_E_NONE;
}


STATIC int
_phy_wcmod_control_tx_driver_field_get(soc_phy_control_t type,int *ln_ctrl,uint16 *mask,int *shfter)
{
    int lane_ctrl;
    
    lane_ctrl = TX_DRIVER_DFT_LN_CTRL;
    *mask = 0;
    *shfter = 0;
    /* _LANEn(n=0-3) control type only applies to combo mode or dxgxs in
     * independent channel mode
     */
    switch(type) {
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0:
    /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1:
    /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2:
    /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3:
    /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        *shfter = TXB_TX_DRIVER_IDRIVER_SHIFT;
        *mask = TXB_TX_DRIVER_IDRIVER_MASK;
        if (type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0) {
            lane_ctrl = 0;
        } else if (type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1) {
            lane_ctrl = 1;
        } else if (type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2) {
            lane_ctrl = 2;
        } else if (type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3) {
            lane_ctrl = 3;
        }
        break;

    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE0:
    /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE1:
    /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE2:
    /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE3:
    /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        *shfter = TXB_TX_DRIVER_IPREDRIVER_SHIFT;
        *mask = TXB_TX_DRIVER_IPREDRIVER_MASK;
        if (type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE0) {
            lane_ctrl = 0;
        } else if (type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE1) {
            lane_ctrl = 1;
        } else if (type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE2) {
            lane_ctrl = 2;
        } else if (type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE3) {
            lane_ctrl = 3;
        }
        break;
    case SOC_PHY_CONTROL_DRIVER_POST2_CURRENT:
        *shfter = TXB_TX_DRIVER_POST2_COEFF_SHIFT;
        *mask = TXB_TX_DRIVER_POST2_COEFF_MASK;
        break;
    default:
         /* should never get here */
        return SOC_E_PARAM;
    }
    *ln_ctrl = lane_ctrl;
    return SOC_E_NONE;
}

/* usage: need to set proper ws->lane_select prior to call this func if type is CURRENT and no lane specified
   by soc_phy_control_t type.  By default ws->lane_select is one hot (lane0), even in combo mode  */ 
STATIC int
_phy_wcmod_control_tx_driver_set(int unit, phy_ctrl_t *pc,
                                soc_phy_control_t type, uint32 value)
{
    int rv;
    int j, tmpLane, tmpSelect, tmpValue=0 ;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int     lane_ctrl;
    int     shifter;
    uint16  mask;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws    =  (wcmod_st *)( pDesc + 1);

    if(ws->verbosity>0) printf("%s p=%0d pc=%p per_lane_control=%x lane_select=%x type=%0x\n", 
                               FUNCTION_NAME(), pc->port, (void *)pc, ws->per_lane_control, ws->lane_select, type) ; 
    SOC_IF_ERROR_RETURN
        (_phy_wcmod_control_tx_driver_field_get(type,&lane_ctrl,&mask,&shifter));

    tmpValue = (value & 0xff); /* only 8 bits taken */

    /* Idriver (4 bits) and pdriver (4 bits) controls programmable TX amplitude.
     * idriver and pdriver are provided #wcmod_st::per_lane_control fields.
     * This function has to be called once per lane. (i.e. lane_select must be
     * one hot) first byte  Ipredriver second byte Idriver third byte
     * post2_coef fourth byte control (bits 1, 2, and 4 choose idriver
     */
    
    tmpSelect       = ws->lane_select ;
    SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, ws));  /* this call resets lane_select */
    tmpLane         = ws->this_lane;

    ws->lane_select = tmpSelect ;
    if(ws->verbosity>0) 
        printf("%s p=%0d pc=%p per_lane_control=%x lane_select=%x tmp_sel=%0x type=%0x\n", 
               FUNCTION_NAME(), pc->port, (void *)pc, ws->per_lane_control, ws->lane_select, tmpSelect, type) ;

    if        (type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE0) {
        ws->this_lane   = 0;
        ws->lane_select = WCMOD_LANE_0_0_0_1 ;
        tmpValue |= 0x1000000;
    } else if (type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE1) {
        ws->this_lane  = 1;
        ws->lane_select = WCMOD_LANE_0_0_1_0 ;
        tmpValue |= 0x1000000;
    } else if (type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE2) {
        ws->this_lane  = 2;
        ws->lane_select = WCMOD_LANE_0_1_0_0 ;
        tmpValue |= 0x1000000;
    } else if (type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE3) {
        ws->this_lane  = 3;
        ws->lane_select = WCMOD_LANE_1_0_0_0 ;
        tmpValue |= 0x1000000;
    } else if (type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT) {
        /* lane select is unchanged */
        tmpValue |= 0x1000000;
    } else if (type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0) {
        ws->this_lane  = 0;
        ws->lane_select = WCMOD_LANE_0_0_0_1 ;
        tmpValue <<= 8;
        tmpValue |= 0x2000000;
    } else if (type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1) {
        ws->this_lane  = 1;
        ws->lane_select = WCMOD_LANE_0_0_1_0 ;
        tmpValue <<= 8;
        tmpValue |= 0x2000000;
    } else if (type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2) {
        ws->this_lane  = 2;
        ws->lane_select = WCMOD_LANE_0_1_0_0 ;
        tmpValue <<= 8;
        tmpValue |= 0x2000000;
    } else if (type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3) {
        ws->this_lane  = 3;
        ws->lane_select = WCMOD_LANE_1_0_0_0 ;
        tmpValue <<= 8;
        tmpValue |= 0x2000000;
    } else if (type == SOC_PHY_CONTROL_DRIVER_CURRENT) {
        /* lane select is unchanged */
        tmpValue <<= 8;
        tmpValue |= 0x2000000;
    } else if (type == SOC_PHY_CONTROL_DRIVER_POST2_CURRENT) {
        /* lane select is unchanged */
        tmpValue <<= 16;
        tmpValue |= 0x4000000;
    }

    if (IS_MULTI_CORE_PORT(pc)) {
        int i, num_core;
        wcmod_st    *ws_temp;
        /*check the special ILKN port */
        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
            num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
        } else {
            num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        }

        for (i = 0; i < num_core; i++) {
            ws_temp   =  (wcmod_st *)( ws +  i);
            ws_temp->per_lane_control = tmpValue;
            if  ((ws_temp->this_lane == 0 ) && (ws_temp->num_of_lane == 2)) {
                ws_temp->lane_select = WCMOD_LANE_0_0_1_1;
            } else if ((ws_temp->this_lane == 2 ) && (ws_temp->num_of_lane == 2)) { 
                ws_temp->lane_select = WCMOD_LANE_1_1_0_0;
            } else {
                ws_temp->lane_select = WCMOD_LANE_1_1_1_1;
            }
            SOC_IF_ERROR_RETURN (wcmod_tier1_selector("TX_AMP_CONTROL", ws_temp, &rv));

        }
    } else {
        ws->per_lane_control = tmpValue; 
        /* ws->this_lane is usually modified bywcmod_tier1_selector */ 
        if(ws->verbosity>0) 
            printf("%s per_lane_control=%x lane_select=%x tmp_sel=%0x type=%0x\n", FUNCTION_NAME(), ws->per_lane_control, ws->lane_select, tmpSelect, type) ;
        SOC_IF_ERROR_RETURN (wcmod_tier1_selector("TX_AMP_CONTROL", ws, &rv));
    }

    /*save the value into data array */
    if ((SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) || (SOC_INFO(unit).port_num_lanes[pc->port] >= 10)) {
        int size = SOC_INFO(unit).port_num_lanes[pc->port]; 
        for(j = 0; j < size; j++) {
            if (shifter == TXB_TX_DRIVER_IDRIVER_SHIFT) {
                DEV_CFG_PTR(pc)->idriver[j] = value;
            } else if (shifter == TXB_TX_DRIVER_IPREDRIVER_SHIFT) {
                DEV_CFG_PTR(pc)->pdriver[j] = value;
            } else if (shifter == TXB_TX_DRIVER_POST2_COEFF_SHIFT) {
                 DEV_CFG_PTR(pc)->post2driver[j] = value;
            }
        }
    } else if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        for(j = 0; j < 4; j++) {
            if (shifter == TXB_TX_DRIVER_IDRIVER_SHIFT) {
                DEV_CFG_PTR(pc)->idriver[j] = value;
            } else if (shifter == TXB_TX_DRIVER_IPREDRIVER_SHIFT) {
                DEV_CFG_PTR(pc)->pdriver[j] = value;
            } else if (shifter == TXB_TX_DRIVER_POST2_COEFF_SHIFT) {
                 DEV_CFG_PTR(pc)->post2driver[j] = value;
            }
        }
    } else if (IS_DUAL_LANE_PORT(pc)) {
        for(j = 0; j <  2; j++) {
            if (shifter == TXB_TX_DRIVER_IDRIVER_SHIFT) {
                DEV_CFG_PTR(pc)->idriver[j] = value;
            } else if (shifter == TXB_TX_DRIVER_IPREDRIVER_SHIFT) {
                DEV_CFG_PTR(pc)->pdriver[j] = value;
            } else if (shifter == TXB_TX_DRIVER_POST2_COEFF_SHIFT) {
                DEV_CFG_PTR(pc)->post2driver[j] = value;
            }
        }
    } else {
        if (shifter == TXB_TX_DRIVER_IDRIVER_SHIFT) {
            DEV_CFG_PTR(pc)->idriver[0] = value;
        } else if (shifter == TXB_TX_DRIVER_IPREDRIVER_SHIFT) {
            DEV_CFG_PTR(pc)->pdriver[0] = value;
        } else if (shifter == TXB_TX_DRIVER_POST2_COEFF_SHIFT) {
            DEV_CFG_PTR(pc)->post2driver[0] = value;
        }
    }         
    ws->this_lane   = tmpLane;     /* restore value */
    ws->lane_select = tmpSelect ; /* restore value */

    return SOC_E_NONE;
}

/* usage: need to set proper ws->lane_select prior to call this func if type is CURRENT and no lane specified
   by soc_phy_control_t type.  By default ws->lane_select is one hot (lane0), even in combo mode  */ 
STATIC int
_phy_wcmod_per_lane_control_tx_driver_set(int unit, soc_port_t port, int lane,
                                soc_phy_control_t type, uint32 value)
{
    int rv, result;
    int tmpLane, tmpSelect, tmpValue = 0, temp_port_type, temp_dxgxs = 0;
    wcmod_st    *ws, *temp_ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int     lane_ctrl, core_num, max_core_num, lane_num;
    int     shifter;
    uint16  mask;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
      (_wcmod_phy_parameter_copy(pc, ws));

    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
        /*next to check if 4 or 8 lane port */
        if (SOC_INFO(unit).port_num_lanes[pc->port] == 4) {
            max_core_num = 2;
            core_num = lane / 2;
            lane_num = 2 + lane % 2;
        } else {
            max_core_num = 3;
            if (lane < 2) {
                core_num = 0;
                lane_num = 2 + lane % 2 ;
            } else if (lane >= 6) {
                core_num = 2;
                lane_num = 2 + lane % 2;
            } else {
                core_num = 1;
                lane_num = (lane + 2) % 4;
            }
        }
        temp_ws =  (wcmod_st *)( ws + core_num);
    } else {
        max_core_num = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        lane_num = (pc->lane_num + lane) % 4;
        core_num = (pc->lane_num + lane) / 4;
        temp_ws =  (wcmod_st *)( ws + core_num);
    }

    if( core_num >= max_core_num ) { return SOC_E_PARAM; }

    SOC_IF_ERROR_RETURN
        (_phy_wcmod_control_tx_driver_field_get(type,&lane_ctrl,&mask,&shifter));

    tmpValue = (value & 0xff); /* only 8 bits taken */

    /* Idriver (4 bits) and pdriver (4 bits) controls programmable TX amplitude.
     * idriver and pdriver are provided #wcmod_st::per_lane_control fields.
     * This function has to be called once per lane. (i.e. lane_select must be
     * one hot) first byte  Ipredriver second byte Idriver third byte
     * post2_coef fourth byte control (bits 1, 2, and 4 choose idriver
     */
    
    tmpLane  = temp_ws->this_lane;
    tmpSelect = temp_ws->lane_select;
    if( IS_DUAL_LANE_PORT(pc)) { 
        temp_dxgxs = temp_ws->dxgxs; 
        temp_ws->dxgxs     = 0 ;
    }

    if(temp_ws->verbosity>0) 
        printf("%s p=%0d pc=%p per_lane_control=%x lane_select=%x tmp_sel=%0x type=%0x\n", 
               FUNCTION_NAME(), port, (void *)pc, temp_ws->per_lane_control, temp_ws->lane_select, tmpSelect, type) ;

    if (type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT) {
        /* lane select is unchanged */
        tmpValue |= 0x1000000;
    } else if (type == SOC_PHY_CONTROL_DRIVER_CURRENT) {
        /* lane select is unchanged */
        tmpValue <<= 8;
        tmpValue |= 0x2000000;
    } else if (type == SOC_PHY_CONTROL_DRIVER_POST2_CURRENT) {
        /* lane select is unchanged */
        tmpValue <<= 16;
        tmpValue |= 0x4000000;
    }

    temp_ws->per_lane_control = tmpValue;
    switch (lane_num) {
        case 0: 
            temp_ws->lane_select = WCMOD_LANE_0_0_0_1;
            break;
        case 1: 
            temp_ws->lane_select = WCMOD_LANE_0_0_1_0;
            break;
        case 2: 
            temp_ws->lane_select = WCMOD_LANE_0_1_0_0;
            break;
        case 3: 
            temp_ws->lane_select = WCMOD_LANE_1_0_0_0;
            break;
        default:
            break;
    }
    temp_port_type = temp_ws->port_type;
    temp_ws->port_type = WCMOD_INDEPENDENT;
    tmpLane = temp_ws->this_lane;
    temp_ws->this_lane = lane_num;
    
    result = wcmod_tier1_selector("TX_AMP_CONTROL", temp_ws, &rv);
    if( SOC_SUCCESS(result) ){
        result = WRITE_WC40_AERBLK_AERr(unit,temp_ws,0);
    }

    /*restore all the info */
    temp_ws->port_type = temp_port_type;
    temp_ws->this_lane = tmpLane;
    temp_ws->lane_select = tmpSelect ;
    if( IS_DUAL_LANE_PORT(pc)) { 
        temp_ws->dxgxs  = temp_dxgxs;
    }

    SOC_IF_ERROR_RETURN(result);
    return SOC_E_NONE;
}

STATIC int
_phy_wcmod_control_tx_lane_squelch_set(int unit, phy_ctrl_t *pc, uint32 value)
{
   int rv;
    wcmod_st    *ws, *temp_ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int i, j;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws    =  (wcmod_st *)( pDesc + 1);
    SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, ws));
    if (IS_MULTI_CORE_PORT(pc)) {
        int num_core, tmpLane;
        /*check the special ILKN port */
        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
            num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
        } else {
            num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        }

        for (i = 0 ; i < num_core; i++) {
            temp_ws   =  (wcmod_st *)( ws + i);
            tmpLane = temp_ws->this_lane;
            for(j = tmpLane; j < tmpLane + temp_ws->num_of_lane ; j++) {
                temp_ws->this_lane = j;
                temp_ws->per_lane_control = value  << (8 * j);
                switch(j) {
                    case 0: temp_ws->lane_select = WCMOD_LANE_0_0_0_1 ; break ;
                    case 1: temp_ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                    case 2: temp_ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                    case 3: temp_ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
                default:
                    break;
                }
                SOC_IF_ERROR_RETURN
                  (wcmod_tier1_selector("TX_LANE_DISABLE", temp_ws, &rv));
           }
        }
    } else {
        int tmpSelect, lane, size, tmpLane;
        tmpLane = ws->this_lane;
        if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
            size = 4;
        } else if (IS_DUAL_LANE_PORT(pc)) {
            size = 2;
        } else {
            size = 1;
        }
        tmpSelect  = ws->lane_select ;
        for(i=0; i<size; i++) {
            lane = i + pc->lane_num ;
            ws->per_lane_control =(value  << (8 * lane));
            ws->this_lane = lane;
            switch(lane) {
                case 0: ws->lane_select = WCMOD_LANE_0_0_0_1 ; break ;
                case 1: ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                case 2: ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                case 3: ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
            default:
                break;
            }
            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("TX_LANE_DISABLE", ws, &rv));
       }
       ws->lane_select = tmpSelect ;
       ws->this_lane = tmpLane;
    }
    return SOC_E_NONE;
}

STATIC int
_phy_wcmod_control_tx_lane_squelch_get(int unit, phy_ctrl_t *pc, uint32 *value)
{
   int rv;
    wcmod_st    *ws, *temp_ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int i, j;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws    =  (wcmod_st *)( pDesc + 1);
    SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, ws));
    if (IS_MULTI_CORE_PORT(pc)) {
        int num_core, tmpLane;
        /*check the special ILKN port */
        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
            num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
        } else {
            num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        }

        for (i = 0 ; i < num_core; i++) {
            temp_ws   =  (wcmod_st *)( ws + i);
            tmpLane = temp_ws->this_lane;
            for(j = tmpLane; j < tmpLane + temp_ws->num_of_lane ; j++) {
                temp_ws->this_lane = j;
                switch(j) {
                    case 0: temp_ws->lane_select = WCMOD_LANE_0_0_0_1 ; break ;
                    case 1: temp_ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                    case 2: temp_ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                    case 3: temp_ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
                default:
                    break;
                }
                temp_ws->diag_type = WCMOD_DIAG_TX_SQUELCH;
                SOC_IF_ERROR_RETURN
                  (wcmod_tier1_selector("WCMOD_DIAG", temp_ws, &rv));
                *value |= ws->accData << (4 * i + j);
           }
        }
    } else {
        int tmpSelect, lane, size;
        if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
            size = 4;
        } else if (IS_DUAL_LANE_PORT(pc)) {
            size = 2;
        } else {
            size = 1;
        }
        tmpSelect  = ws->lane_select ;
        for(i=0; i<size; i++) {
            lane = i + pc->lane_num ;
            switch(lane) {
                case 0: ws->lane_select = WCMOD_LANE_0_0_0_1 ; break ;
                case 1: ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                case 2: ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                case 3: ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
            default:
                break;
            }
            ws->diag_type = WCMOD_DIAG_TX_SQUELCH;
            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
            *value |= ws->accData << i;
       }
       ws->lane_select = tmpSelect ;
    }
    return SOC_E_NONE;
}


STATIC int
_phy_wcmod_control_preemphasis_get(int unit, phy_ctrl_t *pc,
                                soc_phy_control_t type, uint32 *value)
{
    int rv, tmpLane, tmpSelect;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws    =  (wcmod_st *)( pDesc + 1);
    tmpLane   = ws->this_lane; /* save it to restore later */
    tmpSelect = ws->lane_select;

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    if (type == SOC_PHY_CONTROL_PREEMPHASIS_LANE0) {
      ws->this_lane   = 0;
      ws->lane_select = WCMOD_LANE_0_0_0_1 ;
    } else if (type == SOC_PHY_CONTROL_PREEMPHASIS_LANE1) {
      ws->this_lane   = 1;
      ws->lane_select = WCMOD_LANE_0_0_1_0 ;
    } else if (type == SOC_PHY_CONTROL_PREEMPHASIS_LANE2) {
      ws->this_lane   = 2;
      ws->lane_select = WCMOD_LANE_0_1_0_0 ;
    } else if (type == SOC_PHY_CONTROL_PREEMPHASIS_LANE3) {
      ws->this_lane   = 3;
      ws->lane_select = WCMOD_LANE_1_0_0_0 ;
    } else {
      ws->this_lane = pc->lane_num;  /* assume default lanes */
    }

    ws->diag_type = WCMOD_DIAG_TX_TAPS;
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
    *value = ws->accData;
    ws->this_lane   = tmpLane; /* restore this_lane */
    ws->lane_select = tmpSelect ; 
    return SOC_E_NONE;
}

/* usage: need to set proper ws->lane_select prior to call this func if type SOC_PHY_CONTROL_PREEMPHASIS is and no lane specified 
   by soc_phy_control_t type.  By default ws->lane_select is one hot (lane0), even in combo mode  */ 
/* Note that this function always set the TX_FIR_TAP_FORCE bit even tx_driver.u.force=0 */
STATIC int
_phy_wcmod_per_lane_control_preemphasis_get(int unit, soc_port_t port, int lane,
                                uint32 *value)
{
    int tmpLane, tmpSelect, temp_port_type;
    int rv;
    int lane_num, core_num = 0, max_core_num;
    wcmod_st    *ws, *temp_ws;
    WCMOD_DEV_DESC_t     *pDesc;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
      (_wcmod_phy_parameter_copy(pc, ws));

    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
        /*next to check if 4 or 8 lane port */
        if (SOC_INFO(unit).port_num_lanes[pc->port] == 4) {
            max_core_num = 2;
            core_num = lane / 2;
            lane_num = 2 + lane % 2;
        } else {
            max_core_num = 3;
            if (lane < 2) {
                core_num = 0;
                lane_num = 2 + lane % 2 ;
            } else if (lane >= 6) {
                core_num = 2;
                lane_num = 2 + lane % 2;
            } else {
                core_num = 1;
                lane_num = (lane + 2) % 4;
            }
        }
        temp_ws =  (wcmod_st *)( ws + core_num);
    } else {
        max_core_num = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        lane_num = (pc->lane_num + lane) % 4;
        core_num = (pc->lane_num + lane) / 4;
        temp_ws =  (wcmod_st *)( ws + core_num);
    }

    if( core_num >= max_core_num ) { return SOC_E_PARAM; }

    tmpSelect = temp_ws->lane_select;

    switch (lane_num) {
        case 0: 
            temp_ws->lane_select = WCMOD_LANE_0_0_0_1;
            break;
        case 1: 
            temp_ws->lane_select = WCMOD_LANE_0_0_1_0;
            break;
        case 2: 
            temp_ws->lane_select = WCMOD_LANE_0_1_0_0;
            break;
        case 3: 
            temp_ws->lane_select = WCMOD_LANE_1_0_0_0;
            break;
        default:
            break;
    }
    temp_port_type = temp_ws->port_type;
    temp_ws->port_type = WCMOD_INDEPENDENT;
    tmpLane = temp_ws->this_lane;
    temp_ws->this_lane = lane_num;
    temp_ws->diag_type = WCMOD_DIAG_TX_TAPS;
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("WCMOD_DIAG", temp_ws, &rv));
    *value = temp_ws->accData;
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_AERBLK_AERr(unit,temp_ws,0));
    temp_ws->port_type = temp_port_type;
    temp_ws->this_lane = tmpLane;
    temp_ws->lane_select = tmpSelect ;
    return SOC_E_NONE;
}


STATIC int
_phy_wcmod_control_tx_driver_get(int unit, phy_ctrl_t *pc,
                                soc_phy_control_t type, uint32 *value)
{
    int rv, tmpLane, tmpSelect;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws    =  (wcmod_st *)( pDesc + 1);
    tmpLane   = ws->this_lane; /* save it to restore later */
    tmpSelect = ws->lane_select;

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    if (type == SOC_PHY_CONTROL_DRIVER_POST2_CURRENT) {
        ws->accData = 0x3;
    } else if (type == SOC_PHY_CONTROL_DRIVER_CURRENT) {
        ws->accData = 0x2;
    } else if (type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT) {
        ws->accData = 0x1;
    } else if (type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0) {
        ws->this_lane   = 0;
        ws->lane_select = WCMOD_LANE_0_0_0_1;
        ws->accData  = 0x2;
    } else if (type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1) {
        ws->this_lane   = 1;
        ws->lane_select = WCMOD_LANE_0_0_1_0;
        ws->accData = 0x2;
    } else if (type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2) {
        ws->this_lane   = 2;
        ws->lane_select = WCMOD_LANE_0_1_0_0;
        ws->accData = 0x2;
    } else if (type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3) {
        ws->this_lane   = 3;
        ws->lane_select = WCMOD_LANE_1_0_0_0;
        ws->accData = 0x2;
    } else if (type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE0) {
        ws->this_lane   = 0;
        ws->lane_select = WCMOD_LANE_0_0_0_1;
        ws->accData = 0x1;
    } else if (type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE1) {
        ws->this_lane   = 1;
        ws->lane_select = WCMOD_LANE_0_0_1_0;
        ws->accData = 0x1;
    } else if (type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE2) {
        ws->this_lane   = 2;
        ws->lane_select = WCMOD_LANE_0_1_0_0;
        ws->accData = 0x1;
    } else if (type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE3) {
        ws->this_lane   = 3;
        ws->lane_select = WCMOD_LANE_1_0_0_0;
        ws->accData = 0x1;
    } else {
        return SOC_E_PARAM;
    }
    ws->diag_type = WCMOD_DIAG_TX_AMPS;
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
    *value = ws->accData;
    ws->this_lane   = tmpLane; /* restore this_lane */
    ws->lane_select = tmpSelect ;
    return SOC_E_NONE;
}

STATIC int
_phy_wcmod_per_lane_control_tx_driver_get(int unit, soc_port_t port, int lane,
                                soc_phy_control_t type, uint32 *value)
{
    int tmpLane, tmpSelect, temp_port_type;
    int rv;
    int lane_num, core_num = 0, max_core_num;
    wcmod_st    *ws, *temp_ws;
    WCMOD_DEV_DESC_t     *pDesc;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
      (_wcmod_phy_parameter_copy(pc, ws));

    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
        /*next to check if 4 or 8 lane port */
        if (SOC_INFO(unit).port_num_lanes[pc->port] == 4) {
            max_core_num = 2;
            core_num = lane / 2;
            lane_num = 2 + lane % 2;
        } else {
            max_core_num = 3;
            if (lane < 2) {
                core_num = 0;
                lane_num = 2 + lane % 2 ;
            } else if (lane >= 6) {
                core_num = 2;
                lane_num = 2 + lane % 2;
            } else {
                core_num = 1;
                lane_num = (lane + 2) % 4;
            }
        }
        temp_ws =  (wcmod_st *)( ws + core_num);
    } else {
        max_core_num = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        lane_num = (pc->lane_num + lane) % 4;
        core_num = (pc->lane_num + lane) / 4;
        temp_ws =  (wcmod_st *)( ws + core_num);
    }

    if( core_num >= max_core_num ) { return SOC_E_PARAM; }

    tmpSelect = temp_ws->lane_select;

    switch (lane_num) {
        case 0: 
            temp_ws->lane_select = WCMOD_LANE_0_0_0_1;
            break;
        case 1: 
            temp_ws->lane_select = WCMOD_LANE_0_0_1_0;
            break;
        case 2: 
            temp_ws->lane_select = WCMOD_LANE_0_1_0_0;
            break;
        case 3: 
            temp_ws->lane_select = WCMOD_LANE_1_0_0_0;
            break;
        default:
            break;
    }

    if (type == SOC_PHY_CONTROL_DRIVER_POST2_CURRENT) {
        temp_ws->accData = 0x3;
    } else if (type == SOC_PHY_CONTROL_DRIVER_CURRENT) {
        temp_ws->accData = 0x2;
    } else if (type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT) {
        temp_ws->accData = 0x1;
    } else {
        return SOC_E_PARAM;
    }

    temp_port_type = temp_ws->port_type;
    temp_ws->port_type = WCMOD_INDEPENDENT;
    tmpLane = temp_ws->this_lane;
    temp_ws->this_lane = lane_num;
    temp_ws->diag_type = WCMOD_DIAG_TX_AMPS;
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("WCMOD_DIAG", temp_ws, &rv));
    *value = temp_ws->accData;
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_AERBLK_AERr(unit,temp_ws,0));
    temp_ws->port_type = temp_port_type;
    temp_ws->this_lane = tmpLane;
    temp_ws->lane_select = tmpSelect ;
    return SOC_E_NONE;
}



/*
 * Works for 10GX4 8B/10B endec in forced mode. May not work with scrambler 
 * enabled. Doesn't work for 1G/2.5G. Doesn't work for 10G XFI and 40G 
 * using 64/66 endec yet.
 */ 
STATIC int
_phy_wcmod_control_bert_set(int unit, phy_ctrl_t *pc,
                                          soc_phy_control_t type, uint32 value)
{
    return SOC_E_NONE;
}

STATIC int
_phy_wcmod_control_bert_get(int unit, phy_ctrl_t *pc,
                                          soc_phy_control_t type, uint32 *value)
{
    int rv = SOC_E_NONE;
    return rv;
}

#ifdef WC_EEE_SUPPORT     
STATIC int
_phy_wcmod_control_eee_set(int unit, phy_ctrl_t *pc, 
                          soc_phy_control_t type,uint32 enable)
{
    uint16 data0;
    uint16 mask0;
    uint16 data1;
    uint16 mask1;
    wcmod_st *ws ;
    WCMOD_DEV_DESC_t     *pDesc;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);
    switch (type) { 
        case SOC_PHY_CONTROL_EEE:
            mask0 = EEE_USERB0_EEE_COMBO_CONTROL0_EEE_EN_FORCE_VAL_MASK |
                     EEE_USERB0_EEE_COMBO_CONTROL0_EEE_EN_FORCE_MASK |
                     EEE_USERB0_EEE_COMBO_CONTROL0_LPI_EN_FORCE_MASK;
            if (enable == 1) {
                data0 = mask0;
                /* enable EEE, disable the LPI pass-thru */
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_DIGITAL4_MISC5r(unit, ws, 0,
                                (1 << 14) | (1 << 15)));
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_XGXSBLK7_EEECONTROLr(unit, ws, 0,
                                XGXSBLK7_EEECONTROL_LPI_EN_TX_MASK |
                                XGXSBLK7_EEECONTROL_LPI_EN_RX_MASK));
            } else if (enable == 2) {
                /* enable the LPI pass-thru */
                data0 = mask0; 
                mask1 = (1 << 14) | (1 << 15);
                data1 = mask1;
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_DIGITAL4_MISC5r(unit, ws, data1, mask1));
                mask1 = XGXSBLK7_EEECONTROL_LPI_EN_TX_MASK |
                        XGXSBLK7_EEECONTROL_LPI_EN_RX_MASK;
                data1 = mask1;
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_XGXSBLK7_EEECONTROLr(unit, ws, data1, mask1));
            } else {
                /* disable EEE */
                data0 = EEE_USERB0_EEE_COMBO_CONTROL0_EEE_EN_FORCE_MASK;
            }
            /* forced EEE  */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_EEE_USERB0_EEE_COMBO_CONTROL0r(unit, ws,
                      data0, mask0));
            break;

        case SOC_PHY_CONTROL_EEE_AUTO:
            /* EEE through autoneg */
            mask0 = EEE_USERB0_EEE_COMBO_CONTROL0_EEE_EN_FORCE_MASK |
                     EEE_USERB0_EEE_COMBO_CONTROL0_LPI_EN_FORCE_MASK;
            if (enable == 1) {
                data0 = 0;
            } else {
                data0 = mask0;
            }
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_EEE_USERB0_EEE_COMBO_CONTROL0r(unit, ws, data0,
                      mask0));
            break;

    default:
         /* should never get here */
         return SOC_E_UNAVAIL;
    }
    return SOC_E_NONE;
}
#endif 

STATIC int
_phy_wcmod_control_tx_pattern_20bit_set(int unit, phy_ctrl_t *pc, 
                          uint32 pattern_data)
{
    int rv;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    uint32 temp;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));
    if (pattern_data != 0) {
        temp = (pattern_data & 0xffc) >> 10;
        temp <<= 16;
        temp |= pattern_data & 0x3ff;
        ws->per_lane_control = temp | 0x80000000;
    } else {
        ws->per_lane_control = 0x0;
    }
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("PROG_DATA", ws, &rv));
    return SOC_E_NONE;
}

STATIC int
_phy_wcmod_control_tx_pattern_20bit_get(int unit, phy_ctrl_t *pc, 
                        uint32 *value)
{
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    uint16 data;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws    =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));
    switch (pc->lane_num) {
        case 0:
            SOC_IF_ERROR_RETURN (READ_WC40_TX0_ANATXACONTROL0r(unit, ws, &data));
            *value =  data & TX0_ANATXACONTROL0_TX_MDATA_EN_MASK;
            break;
        case 1:
            SOC_IF_ERROR_RETURN (READ_WC40_TX1_ANATXACONTROL0r(unit, ws, &data));
            *value =  data & TX1_ANATXACONTROL0_TX_MDATA_EN_MASK;
            break;
        case 2:
            SOC_IF_ERROR_RETURN (READ_WC40_TX2_ANATXACONTROL0r(unit, ws, &data));
            *value =  data & TX2_ANATXACONTROL0_TX_MDATA_EN_MASK;
            break;
        case 3:
            SOC_IF_ERROR_RETURN (READ_WC40_TX3_ANATXACONTROL0r(unit, ws, &data));
            *value =  data & TX3_ANATXACONTROL0_TX_MDATA_EN_MASK;
            break;
        default:
            break;
    }
    return SOC_E_NONE;
}

STATIC int
_phy_wcmod_control_tx_pattern_256bit_set(int unit, phy_ctrl_t *pc, 
                         uint32 enable)
{
    int rv;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));
     
    ws->per_lane_control_x = 1;
    ws->per_lane_control = enable;

    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("PROG_DATA", ws, &rv));
    return SOC_E_NONE;
}

STATIC int
_phy_wcmod_control_tx_pattern_256bit_get(int unit, phy_ctrl_t *pc, 
                        uint32 *value)
{
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    uint16 data;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws    =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));
    SOC_IF_ERROR_RETURN
        (READ_WC40_PATGEN256_1_CTRLr(ws->unit, ws, &data));
    *value = (uint32) (data & 0x1);         
    return SOC_E_NONE;
}



STATIC int 
_phy_wcmod_rx_DFE_tap1_ovr_control_set(int unit, phy_ctrl_t *pc, uint32 enable)
{
    int rv, i, num_core, temp_lane_select;
    wcmod_st    *ws, *temp_ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int per_lane_control;


    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    if ( enable == 0x8000) {
        per_lane_control = 0;
    } else {
        per_lane_control = enable | RX_UC_CONTROL;
    }
    /* check to see if specail ILKN port */
    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
        /*next to check if 4 or 8 lane port */
        if (SOC_INFO(unit).port_num_lanes[pc->port] == 4) {
            num_core = 2;
        } else {
            num_core = 3;
        }
        for (i = 0 ; i < num_core ; i++) {
            temp_ws   =  (wcmod_st *)( ws + i);
            temp_lane_select = temp_ws->lane_select;
            if (temp_ws->this_lane == 0) {
                temp_ws->lane_select = WCMOD_LANE_1_1_1_1;
            } else {
                temp_ws->lane_select = WCMOD_LANE_1_1_0_0;
            }
            temp_ws->per_lane_control = per_lane_control;
            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("RX_DFE_TAP1_CONTROL", temp_ws, &rv));
            temp_ws->lane_select = temp_lane_select;
        }
    } else if ((SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) || (SOC_INFO(unit).port_num_lanes[pc->port] >= 10)) {
        num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        for (i = 0 ; i < num_core; i++) {
            temp_ws   =  (wcmod_st *)( ws + i);
            temp_lane_select = temp_ws->lane_select;
            temp_ws->per_lane_control  = per_lane_control;
            if ((SOC_INFO(unit).port_num_lanes[pc->port] == 10) && (i == 2)) {
                temp_ws->lane_select = WCMOD_LANE_0_0_1_1;
            } else {
                temp_ws->lane_select = WCMOD_LANE_1_1_1_1;
            }
            SOC_IF_ERROR_RETURN
                (wcmod_tier1_selector("RX_DFE_TAP1_CONTROL", temp_ws, &rv));
            temp_ws->lane_select = temp_lane_select;
        }
    } else if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) { 
        temp_lane_select = ws->lane_select;
        ws->lane_select = WCMOD_LANE_1_1_1_1;
        ws->per_lane_control  = per_lane_control;
        SOC_IF_ERROR_RETURN
          (wcmod_tier1_selector("RX_DFE_TAP1_CONTROL", ws, &rv));
        ws->lane_select = temp_lane_select;
    } else {
        ws->per_lane_control  = per_lane_control;
        SOC_IF_ERROR_RETURN
          (wcmod_tier1_selector("RX_DFE_TAP1_CONTROL", ws, &rv));
    }
    return SOC_E_NONE;
}

STATIC int 
_phy_wcmod_per_lane_rx_DFE_tap1_ovr_control_set(int unit, soc_port_t port, int lane, uint32 enable)
{
    int tmpLane, temp_port_type;
    int rv;
    int lane_num, core_num = 0, max_core_num;
    wcmod_st    *ws, *temp_ws;
    WCMOD_DEV_DESC_t     *pDesc;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);
    SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, ws));

    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
        /*next to check if 4 or 8 lane port */
        if (SOC_INFO(unit).port_num_lanes[pc->port] == 4) {
            max_core_num = 2;
            core_num = lane / 2;
            lane_num = 2 + lane % 2;
        } else {
            max_core_num = 3;
            if (lane < 2) {
                core_num = 0;
                lane_num = 2 + lane % 2 ;
            } else if (lane >= 6) {
                core_num = 2;
                lane_num = 2 + lane % 2;
            } else {
                core_num = 1;
                lane_num = (lane - 2) % 4;
            }
        }
        temp_ws =  (wcmod_st *)( ws + core_num);
    } else {
        max_core_num = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        lane_num = (pc->lane_num + lane) % 4;
        core_num = (pc->lane_num + lane) / 4;
        temp_ws =  (wcmod_st *)( ws + core_num);
    }

    if( core_num >= max_core_num ) { return SOC_E_PARAM; }

    /*SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, temp_ws));
    temp_ws->phy_ad = temp_id; */
    
    if ( enable == 0x8000) {
        temp_ws->per_lane_control = 0;
    } else {
        temp_ws->per_lane_control = enable | RX_UC_CONTROL;
    }
    switch (lane_num) {
        case 0: 
            temp_ws->lane_select = WCMOD_LANE_0_0_0_1;
            break;
        case 1: 
            temp_ws->lane_select = WCMOD_LANE_0_0_1_0;
            break;
        case 2: 
            temp_ws->lane_select = WCMOD_LANE_0_1_0_0;
            break;
        case 3: 
            temp_ws->lane_select = WCMOD_LANE_1_0_0_0;
            break;
        default:
            break;
    }
    temp_port_type = temp_ws->port_type;
    temp_ws->port_type = WCMOD_INDEPENDENT;
    tmpLane = temp_ws->this_lane;
    temp_ws->this_lane = lane_num;
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("RX_DFE_TAP1_CONTROL", temp_ws, &rv));
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_AERBLK_AERr(unit,temp_ws,0));
    /*restore all the info */
    temp_ws->port_type = temp_port_type;
    temp_ws->this_lane = tmpLane;
    return SOC_E_NONE;
}



STATIC int 
_phy_wcmod_rx_DFE_tap2_ovr_control_set(int unit, phy_ctrl_t *pc, uint32 enable)
{
    int rv, i, num_core, temp_lane_select;
    wcmod_st    *ws, *temp_ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int per_lane_control;


    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);
    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    if ( enable == 0x8000) {
        per_lane_control = 0;
    } else {
        per_lane_control = enable | RX_UC_CONTROL;
    }
    /* check to see if specail ILKN port */
    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
        /*next to check if 4 or 8 lane port */
        if (SOC_INFO(unit).port_num_lanes[pc->port] == 4) {
            num_core = 2;
        } else {
            num_core = 3;
        }
        for (i = 0 ; i < num_core ; i++) {
            temp_ws   =  (wcmod_st *)( ws + i);
            temp_lane_select = temp_ws->lane_select;
            if (temp_ws->this_lane == 0) {
                temp_ws->lane_select = WCMOD_LANE_1_1_1_1;
            } else {
                temp_ws->lane_select = WCMOD_LANE_1_1_0_0;
            }
            temp_ws->per_lane_control = per_lane_control;
            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("RX_DFE_TAP2_CONTROL", temp_ws, &rv));
            temp_ws->lane_select = temp_lane_select;
        }
    } else if ((SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) || (SOC_INFO(unit).port_num_lanes[pc->port] >= 10)) {
        num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        for (i = 0 ; i < num_core; i++) {
            temp_ws   =  (wcmod_st *)( ws + i);
            temp_lane_select = temp_ws->lane_select;
            temp_ws->per_lane_control  = per_lane_control;
            if ((SOC_INFO(unit).port_num_lanes[pc->port] == 10) && (i == 2)) {
                temp_ws->lane_select = WCMOD_LANE_0_0_1_1;
            } else {
                temp_ws->lane_select = WCMOD_LANE_1_1_1_1;
            }
            SOC_IF_ERROR_RETURN
                (wcmod_tier1_selector("RX_DFE_TAP2_CONTROL", temp_ws, &rv));
            temp_ws->lane_select = temp_lane_select;
        }
    } else if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) { 
        temp_lane_select = ws->lane_select;
        ws->lane_select = WCMOD_LANE_1_1_1_1;
        ws->per_lane_control  = per_lane_control;
        SOC_IF_ERROR_RETURN
          (wcmod_tier1_selector("RX_DFE_TAP2_CONTROL", ws, &rv));
        ws->lane_select = temp_lane_select;
    } else {
        ws->per_lane_control  = per_lane_control;
        SOC_IF_ERROR_RETURN
          (wcmod_tier1_selector("RX_DFE_TAP2_CONTROL", ws, &rv));
    }
    return SOC_E_NONE;
}

STATIC int 
_phy_wcmod_per_lane_rx_DFE_tap2_ovr_control_set(int unit, soc_port_t port, int lane, uint32 enable)
{
    int tmpLane, temp_port_type;
    int rv;
    int lane_num, core_num = 0, max_core_num;
    wcmod_st    *ws, *temp_ws;
    WCMOD_DEV_DESC_t     *pDesc;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1); 

    SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, ws));

    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
        /*next to check if 4 or 8 lane port */
        if (SOC_INFO(unit).port_num_lanes[pc->port] == 4) {
            max_core_num = 2;
            core_num = lane / 2;
            lane_num = 2 + lane % 2;
        } else {
            max_core_num = 3;
            if (lane < 2) {
                core_num = 0;
                lane_num = 2 + lane % 2 ;
            } else if (lane >= 6) {
                core_num = 2;
                lane_num = 2 + lane % 2;
            } else {
                core_num = 1;
                lane_num = (lane - 2) % 4;
            }
        }
        temp_ws =  (wcmod_st *)( ws + core_num);
    } else {
        max_core_num = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        lane_num = (pc->lane_num + lane) % 4;
        core_num = (pc->lane_num + lane) / 4;
        temp_ws =  (wcmod_st *)( ws + core_num);
    }

    if( core_num >= max_core_num ) { return SOC_E_PARAM; }

    /*temp_ws->phy_ad = temp_id;
    tmpSelect = temp_ws->lane_select;*/

    if ( enable == 0x8000) {
        temp_ws->per_lane_control = 0;
    } else {
        temp_ws->per_lane_control = enable | RX_UC_CONTROL;
    }
    switch (lane_num) {
        case 0: 
            temp_ws->lane_select = WCMOD_LANE_0_0_0_1;
            break;
        case 1: 
            temp_ws->lane_select = WCMOD_LANE_0_0_1_0;
            break;
        case 2: 
            temp_ws->lane_select = WCMOD_LANE_0_1_0_0;
            break;
        case 3: 
            temp_ws->lane_select = WCMOD_LANE_1_0_0_0;
            break;
        default:
            break;
    }
    temp_port_type = temp_ws->port_type;
    temp_ws->port_type = WCMOD_INDEPENDENT;
    tmpLane = temp_ws->this_lane;
    temp_ws->this_lane = lane_num;
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("RX_DFE_TAP2_CONTROL", temp_ws, &rv));
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_AERBLK_AERr(unit,temp_ws,0));
    /*restore all the info */
    temp_ws->port_type = temp_port_type;
    temp_ws->this_lane = tmpLane;
    return SOC_E_NONE;
}

STATIC int 
_phy_wcmod_rx_DFE_tap3_ovr_control_set(int unit, phy_ctrl_t *pc, uint32 enable)
{
    int rv, i, num_core, temp_lane_select;
    wcmod_st    *ws, *temp_ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int per_lane_control;


    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    if ( enable == 0x8000) {
        per_lane_control = 0;
    } else {
        per_lane_control = enable | RX_UC_CONTROL;
    }
    /* check to see if specail ILKN port */
    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
        /*next to check if 4 or 8 lane port */
        if (SOC_INFO(unit).port_num_lanes[pc->port] == 4) {
            num_core = 2;
        } else {
            num_core = 3;
        }
        for (i = 0 ; i < num_core ; i++) {
            temp_ws   =  (wcmod_st *)( ws + i);
            temp_lane_select = temp_ws->lane_select;
            if (temp_ws->this_lane == 0) {
                temp_ws->lane_select = WCMOD_LANE_1_1_1_1;
            } else {
                temp_ws->lane_select = WCMOD_LANE_1_1_0_0;
            }
            temp_ws->per_lane_control = per_lane_control;
            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("RX_DFE_TAP3_CONTROL", temp_ws, &rv));
            temp_ws->lane_select = temp_lane_select;
        }
    } else if ((SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) || (SOC_INFO(unit).port_num_lanes[pc->port] >= 10)) {
        num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        for (i = 0 ; i < num_core; i++) {
            temp_ws   =  (wcmod_st *)( ws + i);
            temp_lane_select = temp_ws->lane_select;
            temp_ws->per_lane_control  = per_lane_control;
            if ((SOC_INFO(unit).port_num_lanes[pc->port] == 10) && (i == 2)) {
                temp_ws->lane_select = WCMOD_LANE_0_0_1_1;
            } else {
                temp_ws->lane_select = WCMOD_LANE_1_1_1_1;
            }
            SOC_IF_ERROR_RETURN
                (wcmod_tier1_selector("RX_DFE_TAP3_CONTROL", temp_ws, &rv));
            temp_ws->lane_select = temp_lane_select;
        }
    } else if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) { 
        temp_lane_select = ws->lane_select;
        ws->lane_select = WCMOD_LANE_1_1_1_1;
        ws->per_lane_control  = per_lane_control;
        SOC_IF_ERROR_RETURN
          (wcmod_tier1_selector("RX_DFE_TAP3_CONTROL", ws, &rv));
        ws->lane_select = temp_lane_select;
    } else {
        ws->per_lane_control  = per_lane_control;
        SOC_IF_ERROR_RETURN
          (wcmod_tier1_selector("RX_DFE_TAP3_CONTROL", ws, &rv));
    }
    return SOC_E_NONE;
}

STATIC int 
_phy_wcmod_per_lane_rx_DFE_tap3_ovr_control_set(int unit, soc_port_t port, int lane, uint32 enable)
{
    int tmpLane, temp_port_type;
    int rv;
    int lane_num, core_num = 0, max_core_num;
    wcmod_st    *ws, *temp_ws;
    WCMOD_DEV_DESC_t     *pDesc;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1); 

    SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, ws));

    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
        /*next to check if 4 or 8 lane port */
        if (SOC_INFO(unit).port_num_lanes[pc->port] == 4) {
            max_core_num = 2;
            core_num = lane / 2;
            lane_num = 2 + lane % 2;
        } else {
            max_core_num = 3;
            if (lane < 2) {
                core_num = 0;
                lane_num = 2 + lane % 2 ;
            } else if (lane >= 6) {
                core_num = 2;
                lane_num = 2 + lane % 2;
            } else {
                core_num = 1;
                lane_num = (lane - 2) % 4;
            }
        }
        temp_ws =  (wcmod_st *)( ws + core_num);
    } else {
        max_core_num = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        lane_num = (pc->lane_num + lane) % 4;
        core_num = (pc->lane_num + lane) / 4;
        temp_ws =  (wcmod_st *)( ws + core_num);
    }

    if( core_num >= max_core_num ) { return SOC_E_PARAM; }

    /*SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, temp_ws));
    temp_ws->phy_ad = temp_id;
    tmpSelect = temp_ws->lane_select; */

    if ( enable == 0x8000) {
        temp_ws->per_lane_control = 0;
    } else {
        temp_ws->per_lane_control = enable | RX_UC_CONTROL;
    }
    switch (lane_num) {
        case 0: 
            temp_ws->lane_select = WCMOD_LANE_0_0_0_1;
            break;
        case 1: 
            temp_ws->lane_select = WCMOD_LANE_0_0_1_0;
            break;
        case 2: 
            temp_ws->lane_select = WCMOD_LANE_0_1_0_0;
            break;
        case 3: 
            temp_ws->lane_select = WCMOD_LANE_1_0_0_0;
            break;
        default:
            break;
    }
    temp_port_type = temp_ws->port_type;
    temp_ws->port_type = WCMOD_INDEPENDENT;
    tmpLane = temp_ws->this_lane;
    temp_ws->this_lane = lane_num;
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("RX_DFE_TAP3_CONTROL", temp_ws, &rv));
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_AERBLK_AERr(unit,temp_ws,0));
    /*restore all the info */
    temp_ws->port_type = temp_port_type;
    temp_ws->this_lane = tmpLane;
    return SOC_E_NONE;
}



STATIC int 
_phy_wcmod_rx_DFE_tap4_ovr_control_set(int unit, phy_ctrl_t *pc, uint32 enable)
{
    int rv, i, num_core, temp_lane_select;
    wcmod_st    *ws, *temp_ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int per_lane_control;


    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    if ( enable == 0x8000) {
        per_lane_control = 0;
    } else {
        per_lane_control = enable | RX_UC_CONTROL;
    }
    /* check to see if specail ILKN port */
    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
        /*next to check if 4 or 8 lane port */
        if (SOC_INFO(unit).port_num_lanes[pc->port] == 4) {
            num_core = 2;
        } else {
            num_core = 3;
        }
        for (i = 0 ; i < num_core ; i++) {
            temp_ws   =  (wcmod_st *)( ws + i);
            temp_lane_select = temp_ws->lane_select;
            if (temp_ws->this_lane == 0) {
                temp_ws->lane_select = WCMOD_LANE_1_1_1_1;
            } else {
                temp_ws->lane_select = WCMOD_LANE_1_1_0_0;
            }
            temp_ws->per_lane_control = per_lane_control;
            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("RX_DFE_TAP4_CONTROL", temp_ws, &rv));
            temp_ws->lane_select = temp_lane_select;
        }
    } else if ((SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) || (SOC_INFO(unit).port_num_lanes[pc->port] >= 10)) {
        num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        for (i = 0 ; i < num_core; i++) {
            temp_ws   =  (wcmod_st *)( ws + i);
            temp_lane_select = temp_ws->lane_select;
            temp_ws->per_lane_control  = per_lane_control;
            if ((SOC_INFO(unit).port_num_lanes[pc->port] == 10) && (i == 2)) {
                temp_ws->lane_select = WCMOD_LANE_0_0_1_1;
            } else {
                temp_ws->lane_select = WCMOD_LANE_1_1_1_1;
            }
            SOC_IF_ERROR_RETURN
                (wcmod_tier1_selector("RX_DFE_TAP4_CONTROL", temp_ws, &rv));
            temp_ws->lane_select = temp_lane_select;
        }
    } else if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) { 
        temp_lane_select = ws->lane_select;
        ws->lane_select = WCMOD_LANE_1_1_1_1;
        ws->per_lane_control  = per_lane_control;
        SOC_IF_ERROR_RETURN
          (wcmod_tier1_selector("RX_DFE_TAP4_CONTROL", ws, &rv));
        ws->lane_select = temp_lane_select;
    } else {
        ws->per_lane_control  = per_lane_control;
        SOC_IF_ERROR_RETURN
          (wcmod_tier1_selector("RX_DFE_TAP4_CONTROL", ws, &rv));
    }
    return SOC_E_NONE;
}

STATIC int 
_phy_wcmod_per_lane_rx_DFE_tap4_ovr_control_set(int unit, soc_port_t port, int lane, uint32 enable)
{
    int tmpLane, temp_port_type;
    int rv;
    int lane_num, core_num = 0, max_core_num;
    wcmod_st    *ws, *temp_ws;
    WCMOD_DEV_DESC_t     *pDesc;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1); 

    SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, ws));

    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
        /*next to check if 4 or 8 lane port */
        if (SOC_INFO(unit).port_num_lanes[pc->port] == 4) {
            max_core_num = 2;
            core_num = lane / 2;
            lane_num = 2 + lane % 2;
        } else {
            max_core_num = 3;
            if (lane < 2) {
                core_num = 0;
                lane_num = 2 + lane % 2 ;
            } else if (lane >= 6) {
                core_num = 2;
                lane_num = 2 + lane % 2;
            } else {
                core_num = 1;
                lane_num = (lane - 2) % 4;
            }
        }
        temp_ws =  (wcmod_st *)( ws + core_num);
    } else {
        max_core_num = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        lane_num = (pc->lane_num + lane) % 4;
        core_num = (pc->lane_num + lane) / 4;
        temp_ws =  (wcmod_st *)( ws + core_num);
    }

    if( core_num >= max_core_num ) { return SOC_E_PARAM; }

   /* SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, temp_ws));
    temp_ws->phy_ad = temp_id;
    tmpSelect = temp_ws->lane_select; */

    if ( enable == 0x8000) {
        temp_ws->per_lane_control = 0;
    } else {
        temp_ws->per_lane_control = enable | RX_UC_CONTROL;
    }
    switch (lane_num) {
        case 0: 
            temp_ws->lane_select = WCMOD_LANE_0_0_0_1;
            break;
        case 1: 
            temp_ws->lane_select = WCMOD_LANE_0_0_1_0;
            break;
        case 2: 
            temp_ws->lane_select = WCMOD_LANE_0_1_0_0;
            break;
        case 3: 
            temp_ws->lane_select = WCMOD_LANE_1_0_0_0;
            break;
        default:
            break;
    }
    temp_port_type = temp_ws->port_type;
    temp_ws->port_type = WCMOD_INDEPENDENT;
    tmpLane = temp_ws->this_lane;
    temp_ws->this_lane = lane_num;
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("RX_DFE_TAP4_CONTROL", temp_ws, &rv));
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_AERBLK_AERr(unit,temp_ws,0));
    /*restore all the info */
    temp_ws->port_type = temp_port_type;
    temp_ws->this_lane = tmpLane;
    return SOC_E_NONE;
}


STATIC int 
_phy_wcmod_rx_DFE_tap5_ovr_control_set(int unit, phy_ctrl_t *pc, uint32 enable)
{
    int rv, i, num_core, temp_lane_select;
    wcmod_st    *ws, *temp_ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int per_lane_control;


    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);
    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));


    if ( enable == 0x8000) {
        per_lane_control = 0;
    } else {
        per_lane_control = enable | RX_UC_CONTROL;
    }
    /* check to see if specail ILKN port */
    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
        /*next to check if 4 or 8 lane port */
        if (SOC_INFO(unit).port_num_lanes[pc->port] == 4) {
            num_core = 2;
        } else {
            num_core = 3;
        }
        for (i = 0 ; i < num_core ; i++) {
            temp_ws   =  (wcmod_st *)( ws + i);
            temp_lane_select = temp_ws->lane_select;
            if (temp_ws->this_lane == 0) {
                temp_ws->lane_select = WCMOD_LANE_1_1_1_1;
            } else {
                temp_ws->lane_select = WCMOD_LANE_1_1_0_0;
            }
            temp_ws->per_lane_control = per_lane_control;
            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("RX_DFE_TAP5_CONTROL", temp_ws, &rv));
            temp_ws->lane_select = temp_lane_select;
        }
    } else if ((SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) || (SOC_INFO(unit).port_num_lanes[pc->port] >= 10)) {
        num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        for (i = 0 ; i < num_core; i++) {
            temp_ws   =  (wcmod_st *)( ws + i);
            temp_lane_select = temp_ws->lane_select;
            temp_ws->per_lane_control  = per_lane_control;
            if ((SOC_INFO(unit).port_num_lanes[pc->port] == 10) && (i == 2)) {
                temp_ws->lane_select = WCMOD_LANE_0_0_1_1;
            } else {
                temp_ws->lane_select = WCMOD_LANE_1_1_1_1;
            }
            SOC_IF_ERROR_RETURN
                (wcmod_tier1_selector("RX_DFE_TAP5_CONTROL", temp_ws, &rv));
            temp_ws->lane_select = temp_lane_select;
        }
    } else if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) { 
        temp_lane_select = ws->lane_select;
        ws->lane_select = WCMOD_LANE_1_1_1_1;
        ws->per_lane_control  = per_lane_control;
        SOC_IF_ERROR_RETURN
          (wcmod_tier1_selector("RX_DFE_TAP5_CONTROL", ws, &rv));
        ws->lane_select = temp_lane_select;
    } else {
        ws->per_lane_control  = per_lane_control;
        SOC_IF_ERROR_RETURN
          (wcmod_tier1_selector("RX_DFE_TAP5_CONTROL", ws, &rv));
    }
    return SOC_E_NONE;
}

STATIC int 
_phy_wcmod_per_lane_rx_DFE_tap5_ovr_control_set(int unit, soc_port_t port, int lane, uint32 enable)
{
    int tmpLane, temp_port_type;
    int rv;
    int lane_num, core_num = 0, max_core_num;
    wcmod_st    *ws, *temp_ws;
    WCMOD_DEV_DESC_t     *pDesc;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1); 

    SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, ws));

    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
        /*next to check if 4 or 8 lane port */
        if (SOC_INFO(unit).port_num_lanes[pc->port] == 4) {
            max_core_num = 2;
            core_num = lane / 2;
            lane_num = 2 + lane % 2;
        } else {
            max_core_num = 3;
            if (lane < 2) {
                core_num = 0;
                lane_num = 2 + lane % 2 ;
            } else if (lane >= 6) {
                core_num = 2;
                lane_num = 2 + lane % 2;
            } else {
                core_num = 1;
                lane_num = (lane - 2) % 4;
            }
        }
        temp_ws =  (wcmod_st *)( ws + core_num);
    } else {
        max_core_num = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        lane_num = (pc->lane_num + lane) % 4;
        core_num = (pc->lane_num + lane) / 4;
        temp_ws =  (wcmod_st *)( ws + core_num);
    }

    if( core_num >= max_core_num ) { return SOC_E_PARAM; }
    
    /*SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, temp_ws));
    temp_ws->phy_ad = temp_id;
    tmpSelect = temp_ws->lane_select; */

    if ( enable == 0x8000) {
        temp_ws->per_lane_control = 0;
    } else {
        temp_ws->per_lane_control = enable | RX_UC_CONTROL;
    }
    switch (lane_num) {
        case 0: 
            temp_ws->lane_select = WCMOD_LANE_0_0_0_1;
            break;
        case 1: 
            temp_ws->lane_select = WCMOD_LANE_0_0_1_0;
            break;
        case 2: 
            temp_ws->lane_select = WCMOD_LANE_0_1_0_0;
            break;
        case 3: 
            temp_ws->lane_select = WCMOD_LANE_1_0_0_0;
            break;
        default:
            break;
    }
    temp_port_type = temp_ws->port_type;
    temp_ws->port_type = WCMOD_INDEPENDENT;
    tmpLane = temp_ws->this_lane;
    temp_ws->this_lane = lane_num;
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("RX_DFE_TAP5_CONTROL", temp_ws, &rv));
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_AERBLK_AERr(unit,temp_ws,0));
    /*restore all the info */
    temp_ws->port_type = temp_port_type;
    temp_ws->this_lane = tmpLane;
    return SOC_E_NONE;
}



STATIC int 
_phy_wcmod_rx_vga_ovr_control_set(int unit, phy_ctrl_t *pc, uint32 value)
{
    int rv, i, num_core, temp_lane_select;
    wcmod_st    *ws, *temp_ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int per_lane_control;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    if ( value == 0x8000) {
        per_lane_control = 0;
    } else {
        per_lane_control = value | RX_UC_CONTROL;
    }
    /* check to see if specail ILKN port */
    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
        /*next to check if 4 or 8 lane port */
        if (SOC_INFO(unit).port_num_lanes[pc->port] == 4) {
            num_core = 2;
        } else {
            num_core = 3;
        }
        for (i = 0 ; i < num_core ; i++) {
            temp_ws   =  (wcmod_st *)( ws + i);
            temp_lane_select = temp_ws->lane_select;
            if (temp_ws->this_lane == 0) {
                temp_ws->lane_select = WCMOD_LANE_1_1_1_1;
            } else {
                temp_ws->lane_select = WCMOD_LANE_1_1_0_0;
            }
            temp_ws->per_lane_control = per_lane_control;
            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("RX_VGA_CONTROL", temp_ws, &rv));
            temp_ws->lane_select = temp_lane_select;
        }
    } else if ((SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) || (SOC_INFO(unit).port_num_lanes[pc->port] >= 10)) {
        num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        for (i = 0 ; i < num_core; i++) {
            temp_ws   =  (wcmod_st *)( ws + i);
            temp_lane_select = temp_ws->lane_select;
            temp_ws->per_lane_control  = per_lane_control;
            if ((SOC_INFO(unit).port_num_lanes[pc->port] == 10) && (i == 2)) {
                temp_ws->lane_select = WCMOD_LANE_0_0_1_1;
            } else {
                temp_ws->lane_select = WCMOD_LANE_1_1_1_1;
            }
            SOC_IF_ERROR_RETURN
                (wcmod_tier1_selector("RX_VGA_CONTROL", temp_ws, &rv));
            temp_ws->lane_select = temp_lane_select;
        }
    } else if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) { 
        temp_lane_select = ws->lane_select;
        ws->lane_select = WCMOD_LANE_1_1_1_1;
        ws->per_lane_control  = per_lane_control;
        SOC_IF_ERROR_RETURN
          (wcmod_tier1_selector("RX_VGA_CONTROL", ws, &rv));
        ws->lane_select = temp_lane_select;
    } else {
        ws->per_lane_control  = per_lane_control;
        SOC_IF_ERROR_RETURN
          (wcmod_tier1_selector("RX_VGA_CONTROL", ws, &rv));
    }
    return SOC_E_NONE;
}

STATIC int 
_phy_wcmod_per_lane_rx_vga_ovr_control_set(int unit, soc_port_t port, int lane, uint32 value)
{
    int tmpLane, temp_port_type;
    int rv;
    int lane_num, core_num = 0, max_core_num;
    wcmod_st    *ws, *temp_ws;
    WCMOD_DEV_DESC_t     *pDesc;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1); 

    SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, ws));

    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
        /*next to check if 4 or 8 lane port */
        if (SOC_INFO(unit).port_num_lanes[pc->port] == 4) {
            max_core_num = 2;
            core_num = lane / 2;
            lane_num = 2 + lane % 2;
        } else {
            max_core_num = 3;
            if (lane < 2) {
                core_num = 0;
                lane_num = 2 + lane % 2 ;
            } else if (lane >= 6) {
                core_num = 2;
                lane_num = 2 + lane % 2;
            } else {
                core_num = 1;
                lane_num = (lane - 2) % 4;
            }
        }
        temp_ws =  (wcmod_st *)( ws + core_num);
    } else {
        max_core_num = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        lane_num = (pc->lane_num + lane) % 4;
        core_num = (pc->lane_num + lane) / 4;
        temp_ws =  (wcmod_st *)( ws + core_num);
    }

    if( core_num >= max_core_num ) { return SOC_E_PARAM; }
    
    /*SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, temp_ws));
    temp_ws->phy_ad = temp_id;
    tmpSelect = temp_ws->lane_select; */

    if ( value == 0x8000) {
        temp_ws->per_lane_control = 0;
    } else {
        temp_ws->per_lane_control = value | RX_UC_CONTROL;
    }
    switch (lane_num) {
        case 0: 
            temp_ws->lane_select = WCMOD_LANE_0_0_0_1;
            break;
        case 1: 
            temp_ws->lane_select = WCMOD_LANE_0_0_1_0;
            break;
        case 2: 
            temp_ws->lane_select = WCMOD_LANE_0_1_0_0;
            break;
        case 3: 
            temp_ws->lane_select = WCMOD_LANE_1_0_0_0;
            break;
        default:
            break;
    }
    temp_port_type = temp_ws->port_type;
    temp_ws->port_type = WCMOD_INDEPENDENT;
    tmpLane = temp_ws->this_lane;
    temp_ws->this_lane = lane_num;
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("RX_VGA_CONTROL", temp_ws, &rv));
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_AERBLK_AERr(unit,temp_ws,0));
    /*restore all the info */
    temp_ws->port_type = temp_port_type;
    temp_ws->this_lane = tmpLane;
    return SOC_E_NONE;
}

STATIC int 
_phy_wcmod_rx_pf_ovr_control_set(int unit, phy_ctrl_t *pc, uint32 value)
{
    int rv, i, num_core, temp_lane_select;
    wcmod_st    *ws, *temp_ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int per_lane_control;


    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));
    if ( value == 0x8000) {
        per_lane_control = 0;
    } else {
        per_lane_control =  value | CHANGE_RX_PF_VALUES | RX_UC_CONTROL;
    }
    /* check to see if specail ILKN port */
    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
        /*next to check if 4 or 8 lane port */
        if (SOC_INFO(unit).port_num_lanes[pc->port] == 4) {
            num_core = 2;
        } else {
            num_core = 3;
        }
        for (i = 0 ; i < num_core ; i++) {
            temp_ws   =  (wcmod_st *)( ws + i);
            temp_lane_select = temp_ws->lane_select;
            if (temp_ws->this_lane == 0) {
                temp_ws->lane_select = WCMOD_LANE_1_1_1_1;
            } else {
                temp_ws->lane_select = WCMOD_LANE_1_1_0_0;
            }
            temp_ws->per_lane_control = per_lane_control;
            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("RX_PF_CONTROL", temp_ws, &rv));
            temp_ws->lane_select = temp_lane_select;
        }
    } else if ((SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) || (SOC_INFO(unit).port_num_lanes[pc->port] >= 10)) {
        num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        for (i = 0 ; i < num_core; i++) {
            temp_ws   =  (wcmod_st *)( ws + i);
            temp_lane_select = temp_ws->lane_select;
            temp_ws->per_lane_control  = per_lane_control;
            if ((SOC_INFO(unit).port_num_lanes[pc->port] == 10) && (i == 2)) {
                temp_ws->lane_select = WCMOD_LANE_0_0_1_1;
            } else {
                temp_ws->lane_select = WCMOD_LANE_1_1_1_1;
            }
            SOC_IF_ERROR_RETURN
                (wcmod_tier1_selector("RX_PF_CONTROL", temp_ws, &rv));
            temp_ws->lane_select = temp_lane_select;
        }
    } else if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) { 
        temp_lane_select = ws->lane_select;
        ws->lane_select = WCMOD_LANE_1_1_1_1;
        ws->per_lane_control  = per_lane_control;
        SOC_IF_ERROR_RETURN
          (wcmod_tier1_selector("RX_PF_CONTROL", ws, &rv));
        ws->lane_select = temp_lane_select;
    } else {
        ws->per_lane_control  = per_lane_control;
        SOC_IF_ERROR_RETURN
          (wcmod_tier1_selector("RX_PF_CONTROL", ws, &rv));
    }
    return SOC_E_NONE;
}

STATIC int 
_phy_wcmod_per_lane_rx_pf_ovr_control_set(int unit, soc_port_t port, int lane, uint32 value)
{
    int tmpLane, temp_port_type;
    int rv;
    int lane_num, core_num = 0, max_core_num;
    wcmod_st    *ws, *temp_ws;
    WCMOD_DEV_DESC_t     *pDesc;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1); 

    SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, ws));

    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
        /*next to check if 4 or 8 lane port */
        if (SOC_INFO(unit).port_num_lanes[pc->port] == 4) {
            max_core_num = 2;
            core_num = lane / 2;
            lane_num = 2 + lane % 2;
        } else {
            max_core_num = 3;
            if (lane < 2) {
                core_num = 0;
                lane_num = 2 + lane % 2 ;
            } else if (lane >= 6) {
                core_num = 2;
                lane_num = 2 + lane % 2;
            } else {
                core_num = 1;
                lane_num = (lane - 2) % 4;
            }
        }
        temp_ws =  (wcmod_st *)( ws + core_num);
    } else {
        max_core_num = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        lane_num = (pc->lane_num + lane) % 4;
        core_num = (pc->lane_num + lane) / 4;
        temp_ws =  (wcmod_st *)( ws + core_num);
    }

    if( core_num >= max_core_num ) { return SOC_E_PARAM; }
    
    /*SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, temp_ws));
    temp_ws->phy_ad = temp_id;
    tmpSelect = temp_ws->lane_select; */

    if ( value == 0x8000) {
        temp_ws->per_lane_control = 0;
    } else {
        temp_ws->per_lane_control = value | CHANGE_RX_PF_VALUES | RX_UC_CONTROL;
    }
    switch (lane_num) {
        case 0: 
            temp_ws->lane_select = WCMOD_LANE_0_0_0_1;
            break;
        case 1: 
            temp_ws->lane_select = WCMOD_LANE_0_0_1_0;
            break;
        case 2: 
            temp_ws->lane_select = WCMOD_LANE_0_1_0_0;
            break;
        case 3: 
            temp_ws->lane_select = WCMOD_LANE_1_0_0_0;
            break;
        default:
            break;
    }
    temp_port_type = temp_ws->port_type;
    temp_ws->port_type = WCMOD_INDEPENDENT;
    tmpLane = temp_ws->this_lane;
    temp_ws->this_lane = lane_num;
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("RX_PF_CONTROL", temp_ws, &rv));
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_AERBLK_AERr(unit,temp_ws,0));
    /*restore all the info */
    temp_ws->port_type = temp_port_type;
    temp_ws->this_lane = tmpLane;
    return SOC_E_NONE;
}


STATIC int 
_phy_wcmod_rx_low_freq_pf_ovr_control_set(int unit, phy_ctrl_t *pc, uint32 value)
{
    int rv, i, num_core, temp_lane_select;
    wcmod_st    *ws, *temp_ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int per_lane_control;


    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    if ( value == 0x8000) {
        per_lane_control = 0;
    } else {
        per_lane_control =  value | CHANGE_RX_LOW_FREQ_PF_VALUES | RX_UC_CONTROL;
    }
    /* check to see if specail ILKN port */
    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
        /*next to check if 4 or 8 lane port */
        if (SOC_INFO(unit).port_num_lanes[pc->port] == 4) {
            num_core = 2;
        } else {
            num_core = 3;
        }
        for (i = 0 ; i < num_core ; i++) {
            temp_ws   =  (wcmod_st *)( ws + i);
            temp_lane_select = temp_ws->lane_select;
            if (temp_ws->this_lane == 0) {
                temp_ws->lane_select = WCMOD_LANE_1_1_1_1;
            } else {
                temp_ws->lane_select = WCMOD_LANE_1_1_0_0;
            }
            temp_ws->per_lane_control = per_lane_control;
            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("RX_LOW_FREQ_PF_CONTROL", temp_ws, &rv));
            temp_ws->lane_select = temp_lane_select;
        }
    } else if ((SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) || (SOC_INFO(unit).port_num_lanes[pc->port] >= 10)) {
        num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        for (i = 0 ; i < num_core; i++) {
            temp_ws   =  (wcmod_st *)( ws + i);
            temp_lane_select = temp_ws->lane_select;
            temp_ws->per_lane_control  = per_lane_control;
            if ((SOC_INFO(unit).port_num_lanes[pc->port] == 10) && (i == 2)) {
                temp_ws->lane_select = WCMOD_LANE_0_0_1_1;
            } else {
                temp_ws->lane_select = WCMOD_LANE_1_1_1_1;
            }
            SOC_IF_ERROR_RETURN
                (wcmod_tier1_selector("RX_LOW_FREQ_PF_CONTROL", temp_ws, &rv));
            temp_ws->lane_select = temp_lane_select;
        }
    } else if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) { 
        temp_lane_select = ws->lane_select;
        ws->lane_select = WCMOD_LANE_1_1_1_1;
        ws->per_lane_control  = per_lane_control;
        SOC_IF_ERROR_RETURN
          (wcmod_tier1_selector("RX_LOW_FREQ_PF_CONTROL", ws, &rv));
        ws->lane_select = temp_lane_select;
    } else {
        ws->per_lane_control  = per_lane_control;
        SOC_IF_ERROR_RETURN
          (wcmod_tier1_selector("RX_LOW_FREQ_PF_CONTROL", ws, &rv));
    }
    return SOC_E_NONE;
}

STATIC int 
_phy_wcmod_per_lane_rx_low_freq_pf_ovr_control_set(int unit, soc_port_t port, int lane, uint32 value)
{
    int tmpLane, temp_port_type;
    int rv;
    int lane_num, core_num = 0, max_core_num;
    wcmod_st    *ws, *temp_ws;
    WCMOD_DEV_DESC_t     *pDesc;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1); 

    SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, ws));

    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
        /*next to check if 4 or 8 lane port */
        if (SOC_INFO(unit).port_num_lanes[pc->port] == 4) {
            max_core_num = 2;
            core_num = lane / 2;
            lane_num = 2 + lane % 2;
        } else {
            max_core_num = 3;
            if (lane < 2) {
                core_num = 0;
                lane_num = 2 + lane % 2 ;
            } else if (lane >= 6) {
                core_num = 2;
                lane_num = 2 + lane % 2;
            } else {
                core_num = 1;
                lane_num = (lane - 2) % 4;
            }
        }
        temp_ws =  (wcmod_st *)( ws + core_num);
    } else {
        max_core_num = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        lane_num = (pc->lane_num + lane) % 4;
        core_num = (pc->lane_num + lane) / 4;
        temp_ws =  (wcmod_st *)( ws + core_num);
    }

    if( core_num >= max_core_num ) { return SOC_E_PARAM; }
    /*SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, temp_ws));
    temp_ws->phy_ad = temp_id;
    tmpSelect = temp_ws->lane_select; */

    if ( value == 0x8000) {
        temp_ws->per_lane_control = 0;
    } else {
        temp_ws->per_lane_control = value | CHANGE_RX_LOW_FREQ_PF_VALUES | RX_UC_CONTROL;
    }
    switch (lane_num) {
        case 0: 
            temp_ws->lane_select = WCMOD_LANE_0_0_0_1;
            break;
        case 1: 
            temp_ws->lane_select = WCMOD_LANE_0_0_1_0;
            break;
        case 2: 
            temp_ws->lane_select = WCMOD_LANE_0_1_0_0;
            break;
        case 3: 
            temp_ws->lane_select = WCMOD_LANE_1_0_0_0;
            break;
        default:
            break;
    }
    temp_port_type = temp_ws->port_type;
    temp_ws->port_type = WCMOD_INDEPENDENT;
    tmpLane = temp_ws->this_lane;
    temp_ws->this_lane = lane_num;
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("RX_LOW_FREQ_PF_CONTROL", temp_ws, &rv));
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_AERBLK_AERr(unit,temp_ws,0));
    /*restore all the info */
    temp_ws->port_type = temp_port_type;
    temp_ws->this_lane = tmpLane;
    return SOC_E_NONE;
}

STATIC int 
_phy_wcmod_tx_pi_control_set(int unit, phy_ctrl_t *pc, uint32 value)
{
    int rv;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    ws->per_lane_control = value;
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("TX_PI_CONTROL", ws, &rv));
    return SOC_E_NONE;
}


STATIC int 
_phy_wcmod_rx_p1_slicer_control_set(int unit, phy_ctrl_t *pc, uint32 enable)
{
    int rv;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    ws->per_lane_control = enable;
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("RX_P1_SLICER_CONTROL", ws, &rv));
    return SOC_E_NONE;
}

STATIC int 
_phy_wcmod_per_lane_rx_p1_slicer_control_set(int unit, soc_port_t port, int lane, uint32 enable)
{
    int tmpLane, temp_port_type;
    int rv;
    int lane_num, core_num = 0, max_core_num;
    wcmod_st    *ws, *temp_ws;
    WCMOD_DEV_DESC_t     *pDesc;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1); 

    SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, ws));

    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
        /*next to check if 4 or 8 lane port */
        if (SOC_INFO(unit).port_num_lanes[pc->port] == 4) {
            max_core_num = 2;
            core_num = lane / 2;
            lane_num = 2 + lane % 2;
        } else {
            max_core_num = 3;
            if (lane < 2) {
                core_num = 0;
                lane_num = 2 + lane % 2 ;
            } else if (lane >= 6) {
                core_num = 2;
                lane_num = 2 + lane % 2;
            } else {
                core_num = 1;
                lane_num = (lane - 2) % 4;
            }
        }
        temp_ws =  (wcmod_st *)( ws + core_num);
    } else {
        max_core_num = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        lane_num = (pc->lane_num + lane) % 4;
        core_num = (pc->lane_num + lane) / 4;
        temp_ws =  (wcmod_st *)( ws + core_num);
    }

    if( core_num >= max_core_num ) { return SOC_E_PARAM; }
    
    /*SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, temp_ws));
    temp_ws->phy_ad = temp_id;
    tmpSelect = temp_ws->lane_select; */

    temp_ws->per_lane_control = enable;
    switch (lane_num) {
        case 0: 
            temp_ws->lane_select = WCMOD_LANE_0_0_0_1;
            break;
        case 1: 
            temp_ws->lane_select = WCMOD_LANE_0_0_1_0;
            break;
        case 2: 
            temp_ws->lane_select = WCMOD_LANE_0_1_0_0;
            break;
        case 3: 
            temp_ws->lane_select = WCMOD_LANE_1_0_0_0;
            break;
        default:
            break;
    }
    temp_port_type = temp_ws->port_type;
    temp_ws->port_type = WCMOD_INDEPENDENT;
    tmpLane = temp_ws->this_lane;
    temp_ws->this_lane = lane_num;
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("RX_P1_SLICER_CONTROL", temp_ws, &rv));
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_AERBLK_AERr(unit,temp_ws,0));
    /*restore all the info */
    temp_ws->port_type = temp_port_type;
    temp_ws->this_lane = tmpLane;

    return SOC_E_NONE;
}


STATIC int 
_phy_wcmod_rx_m1_slicer_control_set(int unit, phy_ctrl_t *pc, uint32 enable)
{
    int rv;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    ws->per_lane_control = enable;
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("RX_M1_SLICER_CONTROL", ws, &rv));
    return SOC_E_NONE;
}


STATIC int 
_phy_wcmod_per_lane_rx_m1_slicer_control_set(int unit, soc_port_t port, int lane, uint32 enable)
{
    int tmpLane, temp_port_type;
    int rv;
    int lane_num, core_num = 0, max_core_num;
    wcmod_st    *ws, *temp_ws;
    WCMOD_DEV_DESC_t     *pDesc;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1); 

    SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, ws));

    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
        /*next to check if 4 or 8 lane port */
        if (SOC_INFO(unit).port_num_lanes[pc->port] == 4) {
            max_core_num = 2;
            core_num = lane / 2;
            lane_num = 2 + lane % 2;
        } else {
            max_core_num = 3;
            if (lane < 2) {
                core_num = 0;
                lane_num = 2 + lane % 2 ;
            } else if (lane >= 6) {
                core_num = 2;
                lane_num = 2 + lane % 2;
            } else {
                core_num = 1;
                lane_num = (lane - 2) % 4;
            }
        }
        temp_ws =  (wcmod_st *)( ws + core_num);
    } else {
        max_core_num = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        lane_num = (pc->lane_num + lane) % 4;
        core_num = (pc->lane_num + lane) / 4;
        temp_ws =  (wcmod_st *)( ws + core_num);
    }

    if( core_num >= max_core_num ) { return SOC_E_PARAM; }
    
    /*SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, temp_ws));
    temp_ws->phy_ad = temp_id;
    tmpSelect = temp_ws->lane_select; */

    temp_ws->per_lane_control = enable;
    switch (lane_num) {
        case 0: 
            temp_ws->lane_select = WCMOD_LANE_0_0_0_1;
            break;
        case 1: 
            temp_ws->lane_select = WCMOD_LANE_0_0_1_0;
            break;
        case 2: 
            temp_ws->lane_select = WCMOD_LANE_0_1_0_0;
            break;
        case 3: 
            temp_ws->lane_select = WCMOD_LANE_1_0_0_0;
            break;
        default:
            break;
    }
    temp_port_type = temp_ws->port_type;
    temp_ws->port_type = WCMOD_INDEPENDENT;
    tmpLane = temp_ws->this_lane;
    temp_ws->this_lane = lane_num;
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("RX_M1_SLICER_CONTROL", temp_ws, &rv));
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_AERBLK_AERr(unit,temp_ws,0));
    /*restore all the info */
    temp_ws->port_type = temp_port_type;
    temp_ws->this_lane = tmpLane;
    return SOC_E_NONE;
}

STATIC int 
_phy_wcmod_rx_d_slicer_control_set(int unit, phy_ctrl_t *pc, uint32 enable)
{
    int rv;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    ws->per_lane_control = enable;
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("RX_D_SLICER_CONTROL", ws, &rv));
    return SOC_E_NONE;
}

STATIC int 
_phy_wcmod_per_lane_rx_d_slicer_control_set(int unit, soc_port_t port, int lane, uint32 enable)
{
    int tmpLane, temp_port_type;
    int rv;
    int lane_num, core_num = 0, max_core_num;
    wcmod_st    *ws, *temp_ws;
    WCMOD_DEV_DESC_t     *pDesc;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1); 

    SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, ws));

    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
        /*next to check if 4 or 8 lane port */
        if (SOC_INFO(unit).port_num_lanes[pc->port] == 4) {
            max_core_num = 2;
            core_num = lane / 2;
            lane_num = 2 + lane % 2;
        } else {
            max_core_num = 3;
            if (lane < 2) {
                core_num = 0;
                lane_num = 2 + lane % 2 ;
            } else if (lane >= 6) {
                core_num = 2;
                lane_num = 2 + lane % 2;
            } else {
                core_num = 1;
                lane_num = (lane - 2) % 4;
            }
        }
        temp_ws =  (wcmod_st *)( ws + core_num);
    } else {
        max_core_num = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        lane_num = (pc->lane_num + lane) % 4;
        core_num = (pc->lane_num + lane) / 4;
        temp_ws =  (wcmod_st *)( ws + core_num);
    }

    if( core_num >= max_core_num ) { return SOC_E_PARAM; }
    /*SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, temp_ws));
    temp_ws->phy_ad = temp_id;
    tmpSelect = temp_ws->lane_select; */

    temp_ws->per_lane_control = enable;
    switch (lane_num) {
        case 0: 
            temp_ws->lane_select = WCMOD_LANE_0_0_0_1;
            break;
        case 1: 
            temp_ws->lane_select = WCMOD_LANE_0_0_1_0;
            break;
        case 2: 
            temp_ws->lane_select = WCMOD_LANE_0_1_0_0;
            break;
        case 3: 
            temp_ws->lane_select = WCMOD_LANE_1_0_0_0;
            break;
        default:
            break;
    }
    temp_port_type = temp_ws->port_type;
    temp_ws->port_type = WCMOD_INDEPENDENT;
    tmpLane = temp_ws->this_lane;
    temp_ws->this_lane = lane_num;
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("RX_D_SLICER_CONTROL", temp_ws, &rv));
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_AERBLK_AERr(unit,temp_ws,0));
    /*restore all the info */
    temp_ws->port_type = temp_port_type;
    temp_ws->this_lane = tmpLane;
    return SOC_E_NONE;
}


STATIC int 
_phy_wcmod_pi_control_set (int unit, phy_ctrl_t *pc, uint32 enable)
{
    int rv;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    ws->per_lane_control = enable;
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("TX_PI_CONTROL", ws, &rv));
    return SOC_E_NONE;
}

STATIC int 
_phy_wcmod_rloop_set(int unit, phy_ctrl_t *pc, uint32 enable)
{
    int rv;
    wcmod_st  *temp_ws,  *ws ;
    WCMOD_DEV_DESC_t     *pDesc;
    wcmod_spd_intfc_set spd_intf, tmp_spd_intf;
    int  tmp_thislane;

    spd_intf = WCMOD_SPD_ILLEGAL;
    tmp_spd_intf = WCMOD_SPD_ILLEGAL;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws    =  (wcmod_st *)( pDesc + 1);
   
    SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, ws));


    if (SOC_INFO(unit).port_num_lanes[pc->port] == 10) {
        spd_intf =  WCMOD_SPD_100G_CR10;
    }

    if ((SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) || (SOC_INFO(unit).port_num_lanes[pc->port] >= 10)) {
        int i, num_core = 0, j = 0, size = 4;
        wcmod_st *mld;
        uint16 data16 = 0;

        mld   =  (wcmod_st *)ws + 3;
      
        SOC_IF_ERROR_RETURN (WC40_REG_READ(unit, mld, 0x0, 0x8011, &data16));
        data16 = enable ? (data16|0x80):(data16&~(0x80)); 
        SOC_IF_ERROR_RETURN (WC40_REG_WRITE(unit, mld, 0x0, 0x8011, data16));  
        num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        for (i = 0 ; i < num_core; i++) {
             temp_ws   =  (wcmod_st *)( ws + i);
             tmp_spd_intf = temp_ws->spd_intf;
             temp_ws->spd_intf = spd_intf;
             tmp_thislane = temp_ws->this_lane;
             for(j = 0; j < size; j++) {
                 temp_ws->per_lane_control = enable ? (0x1 << (j * 8)): 0;
                 temp_ws->this_lane = j;
                 switch(j) {
                     case 0: temp_ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                     case 1: temp_ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                     case 2: temp_ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                     case 3: temp_ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
                 
                    /* coverity[dead_error_begin:FALSE] */
                     default:
                        temp_ws->lane_select = WCMOD_LANE_0_0_0_1;
                     break;
                 }
                 SOC_IF_ERROR_RETURN
                     (wcmod_tier1_selector("RX_LOOPBACK_CONTROL", temp_ws, &rv));

             }
             
             temp_ws->spd_intf = tmp_spd_intf;
             temp_ws->lane_select = WCMOD_LANE_0_0_0_1;
             temp_ws->this_lane = tmp_thislane;
       }
   } else {
    
        if (enable) {
            ws->per_lane_control = 0x1 << ((ws->this_lane) * 8);
        } else {
            ws->per_lane_control = 0;
        }
        SOC_IF_ERROR_RETURN (wcmod_tier1_selector("RX_LOOPBACK_CONTROL", ws, &rv));
    }    
    return SOC_E_NONE;
}

STATIC int 
_phy_wcmod_rx_polarity_set(int unit, phy_ctrl_t *pc, uint32 enable)
{
    int rv, num_core;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    uint32 temp_enable;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws    =  (wcmod_st *)( pDesc + 1);
    temp_enable = enable;

    SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, ws));
    
    /* enable becomes the polarity vector */
    ws->per_lane_control = (enable <<2) ;
  
    /* always enable this bit to do the "SET_POLARITY" job */
    ws->per_lane_control |=  0x1;  
    /*set RX \ TX - 0 on bit 1 means RX, no need to configure*/
    
    SOC_IF_ERROR_RETURN (wcmod_tier1_selector("SET_POLARITY", ws, &rv));

    /* next check if multi-core port */
    if (pc->phy_mode == PHYCTRL_MULTI_CORE_PORT) {
       wcmod_st *temp_ws;
        int  i;
        num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        for (i = 0 ; i < num_core; i++) {
            temp_ws   =  (wcmod_st *)( ws + 1 + i);
           /* temp_id = temp_ws->phy_ad;
            SOC_IF_ERROR_RETURN(_wcmod_phy_parameter_copy(pc, temp_ws));
            temp_ws->phy_ad = temp_id; */

             temp_enable = (enable >> (4 * (i + 1))) & 0xf;
            /* enable becomes the polarity vector */
            temp_ws->per_lane_control = (temp_enable <<2) ;
          
            /* always enable this bit to do the "SET_POLARITY" job */
            temp_ws->per_lane_control |=  0x1;  
            /*set RX \ TX - 0 on bit 1 means RX, no need to configure*/
    
            SOC_IF_ERROR_RETURN (wcmod_tier1_selector("SET_POLARITY", temp_ws, &rv));
        }
    }

    return SOC_E_NONE;
}

STATIC int 
_phy_wcmod_tx_polarity_set(int unit, phy_ctrl_t *pc, uint32 enable)
{
    int rv, num_core;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    uint32 temp_enable;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws    =  (wcmod_st *)( pDesc + 1);
    temp_enable = enable;

    SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, ws));

    /* enable becomes the polarity vector */
    ws->per_lane_control = (enable <<2) ;
    
    /* always enable this bit to do the "SET_POLARITY" job */
    ws->per_lane_control |=  0x1;
    /*set TX*/
    ws->per_lane_control |=  0x2;
   
    SOC_IF_ERROR_RETURN (wcmod_tier1_selector("SET_POLARITY", ws, &rv));

    /* next check if multi-core port */
    if (pc->phy_mode == PHYCTRL_MULTI_CORE_PORT) {
        wcmod_st *temp_ws;
        int i;
        num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        for (i = 0 ; i < num_core; i++) {
             temp_ws   =  (wcmod_st *)( ws + 1 + i);
            /*temp_id = temp_ws->phy_ad;
            SOC_IF_ERROR_RETURN(_wcmod_phy_parameter_copy(pc, temp_ws));
            temp_ws->phy_ad = temp_id; */

             temp_enable = (enable >> (4 * (i + 1))) & 0xf;
            /* enable becomes the polarity vector */
            temp_ws->per_lane_control = (temp_enable <<2) ;
          
            /* always enable this bit to do the "SET_POLARITY" job */
            temp_ws->per_lane_control |=  0x1;
            /*set TX*/
            temp_ws->per_lane_control |=  0x2;
    
            SOC_IF_ERROR_RETURN (wcmod_tier1_selector("SET_POLARITY", temp_ws, &rv));
        }
    }


    return SOC_E_NONE;
}

STATIC int 
_phy_wcmod_rx_polarity_get(int unit, phy_ctrl_t *pc, uint32 *val)
{
int rv;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws    =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, ws));
    ws->per_lane_control = 0 ;
    ws->diag_type = WCMOD_DIAG_POLARITY;
    SOC_IF_ERROR_RETURN (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
    *val=ws->accData;
    return SOC_E_NONE;
}

STATIC int 
_phy_wcmod_tx_polarity_get(int unit, phy_ctrl_t *pc, uint32 *val)
{
    int rv;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws    =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, ws));
    /*set TX*/
    ws->per_lane_control =  0x1;
    ws->diag_type = WCMOD_DIAG_POLARITY;
    
    SOC_IF_ERROR_RETURN (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
    *val=ws->accData;
    return SOC_E_NONE;
}



#if 0
STATIC int 
phy_wcmod_tx_lane_disable(int unit, phy_ctrl_t *pc, uint32 disable)
{
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int rv;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    ws->this_lane = pc->lane_num;
    switch (pc->lane_num) {
    case 0: 
       ws->per_lane_control = disable;
       break;
    case 1: 
       ws->per_lane_control = disable << 8;
       break;
    case 2: 
       ws->per_lane_control = disable << 16;
       break;
    case 3: 
       ws->per_lane_control = disable << 24;
       break;
   default:
       break;
   }
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("TX_LANE_DISABLE", ws, &rv));
    return SOC_E_NONE;
}
#endif

STATIC int
_phy_wcmod_rloop_pcsbypass_set(int unit, soc_port_t port, uint32 enable)
{
    int rv=0;
    phy_ctrl_t *pc;
    wcmod_st  *temp_ws,  *ws;
    wcmod_spd_intfc_set spd_intf, tmp_spd_intf;
    WCMOD_DEV_DESC_t     *pDesc;
    int size, j;

    spd_intf = WCMOD_SPD_ILLEGAL;
    tmp_spd_intf = WCMOD_SPD_ILLEGAL;


    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    if (SOC_INFO(unit).port_num_lanes[pc->port] == 10) {
        spd_intf =  WCMOD_SPD_100G_CR10;          
    }        

    if ((SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) || (SOC_INFO(unit).port_num_lanes[pc->port] >= 10)) {
        int i, num_core = 0;
        num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        for (i = 0 ; i < num_core; i++) {
             temp_ws   =  (wcmod_st *)( ws + i);
             tmp_spd_intf = temp_ws->spd_intf;
             temp_ws->spd_intf = spd_intf;
            size = 4;
            for(j = 0; j < size; j++) {
                temp_ws->per_lane_control = enable << j;
                temp_ws->this_lane = j;
                switch(j) {
                    case 0: temp_ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                    case 1: temp_ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                    case 2: temp_ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                    case 3: temp_ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
                }
                SOC_IF_ERROR_RETURN
                  (wcmod_tier1_selector("RX_LOOPBACK_PCS_BYPASS", temp_ws, &rv));
           }
           temp_ws->spd_intf = tmp_spd_intf; 
           temp_ws->lane_select = WCMOD_LANE_0_0_0_1;
        }
    } else {
        int tmp_lane, tmp_lane_select;
        size = SOC_INFO(unit).port_num_lanes[pc->port];
        tmp_lane = ws->this_lane;
        tmp_lane_select = ws->lane_select; 
        for(j = pc->lane_num; j < pc->lane_num + size; j++) {
            ws->per_lane_control = enable << j;
            ws->this_lane = j;
            switch(j) {
                case 0: ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                case 1: ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                case 2: ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                case 3: ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
            default:
                break;
            }
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("RX_LOOPBACK_PCS_BYPASS", ws, &rv));
       }
       ws->this_lane = tmp_lane;
       ws->lane_select =  tmp_lane_select;
    } 

    return SOC_E_NONE;
}
STATIC int
_phy_wcmod_rloop_pcs_bypass_get(int unit, phy_ctrl_t *pc, uint32 *value)
{
    int rv;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    ws->diag_type = WCMOD_DIAG_RX_PCS_BYPASS_LOOPBACK;

    SOC_IF_ERROR_RETURN (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
    *value = ws->accData;

    return SOC_E_NONE;
}


STATIC int
_phy_wcmod_rloop_get(int unit, phy_ctrl_t *pc, uint32 *value)
{
    int rv;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    ws->diag_type = WCMOD_DIAG_RX_LOOPBACK;

    SOC_IF_ERROR_RETURN (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
    *value = ws->accData;

    return SOC_E_NONE;
}


STATIC int
_phy_wcmod_fec_set(int unit, phy_ctrl_t *pc, uint32 fec_ctrl)
{
    int rv = SOC_E_NONE;
    uint16 mask16;
    uint16 data16;
    wcmod_st  *temp_ws,   *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int multi_core_port = 0;
    int temp_lane_select;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));
    if (!SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) &&
        (pc->phy_mode == PHYCTRL_MULTI_CORE_PORT)) { 
        multi_core_port = 1;
    }

    temp_lane_select = ws->lane_select;
    if (!multi_core_port) {
        if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
            ws->lane_select = WCMOD_LANE_BCST;
        }
    }

    mask16 = CL74_USERB0_UFECCONTROL3_FEC_ENABLE_OVR_VAL_MASK |
             CL74_USERB0_UFECCONTROL3_FEC_ENABLE_OVR_MASK;
    switch (fec_ctrl) {
        case SOC_PHY_CONTROL_FEC_OFF:
            if (multi_core_port) {
                /* we need to enable fec from ext mld side */
                temp_ws   =  (wcmod_st *)( ws + 3);
                SOC_IF_ERROR_RETURN
                    (WC40_REG_WRITE(unit, temp_ws, 0x0, 0x080000ab,0x0));
            } else {
                data16 = CL74_USERB0_UFECCONTROL3_FEC_ENABLE_OVR_MASK;
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_CL74_USERB0_UFECCONTROL3r(unit,ws,data16,mask16));
            }
            break;
        case SOC_PHY_CONTROL_FEC_ON:
            if (multi_core_port) {
                /* we need to enable fec from ext mld side */
                temp_ws   =  (wcmod_st *)( ws + 3);
                SOC_IF_ERROR_RETURN
                    (WC40_REG_WRITE(unit, temp_ws, 0x0, 0x080000ab,0x1));

            } else {
                data16 = mask16;
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_CL74_USERB0_UFECCONTROL3r(unit,ws,data16,mask16));
            }
            break;
        case SOC_PHY_CONTROL_FEC_AUTO:
            /* turn off forced override */
            data16 = 0; 
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_CL74_USERB0_UFECCONTROL3r(unit,ws,data16,mask16));

            /* configure the CL73 FEC advertisement */
            mask16 = AN_IEEE1BLK_AN_ADVERTISEMENT2_FEC_REQUESTED_MASK;
            data16 = mask16;
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_AN_IEEE1BLK_AN_ADVERTISEMENT2r(unit, ws, data16,
                                     mask16));
            /* configure the CL37 FEC advertisement */
            data16 = mask16 = DIGITAL3_UP3_FEC_MASK;
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DIGITAL3_UP3r(unit, ws, data16, mask16));

            break;
        default:
            rv = SOC_E_PARAM;
            break;
    } 
    ws->lane_select = temp_lane_select;
    return rv;
}

STATIC int
_phy_wcmod_fec_get(int unit, phy_ctrl_t *pc, uint32 *value)
{
    uint16 data16;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));


    SOC_IF_ERROR_RETURN
        (READ_WC40_CL74_USERB0_UFECCONTROL3r(unit,ws,&data16));
    if (data16 & CL74_USERB0_UFECCONTROL3_FEC_ENABLE_OVR_MASK) {
        *value = data16 & CL74_USERB0_UFECCONTROL3_FEC_ENABLE_OVR_VAL_MASK?
                 SOC_PHY_CONTROL_FEC_ON:
                 SOC_PHY_CONTROL_FEC_OFF; 
    } else {
        *value = SOC_PHY_CONTROL_FEC_AUTO;
    }
    return SOC_E_NONE;         
}


STATIC int
_phy_wcmod_control_linkdown_transmit_set(int unit, soc_port_t port, uint32 value)
{
    uint16      ctrl2_data;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    phy_ctrl_t *pc;
    
    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    if (value) {
        ctrl2_data = (SERDESDIGITAL_CONTROL1000X2_DISABLE_FALSE_LINK_BITS <<
                      SERDESDIGITAL_CONTROL1000X2_DISABLE_FALSE_LINK_SHIFT) |
                     (SERDESDIGITAL_CONTROL1000X2_FILTER_FORCE_LINK_BITS  <<
                      SERDESDIGITAL_CONTROL1000X2_FILTER_FORCE_LINK_SHIFT) |
                     (SERDESDIGITAL_CONTROL1000X2_FORCE_XMIT_DATA_ON_TXSIDE_BITS <<
                      SERDESDIGITAL_CONTROL1000X2_FORCE_XMIT_DATA_ON_TXSIDE_SHIFT);
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_SERDESDIGITAL_CONTROL1000X2r(unit, ws, ctrl2_data));

    } else {
        ctrl2_data = (SERDESDIGITAL_CONTROL1000X2_FILTER_FORCE_LINK_BITS <<
                      SERDESDIGITAL_CONTROL1000X2_FILTER_FORCE_LINK_SHIFT) |
                     (SERDESDIGITAL_CONTROL1000X2_DISABLE_FALSE_LINK_BITS <<
                      SERDESDIGITAL_CONTROL1000X2_DISABLE_FALSE_LINK_SHIFT) |
                     (SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_BITS <<
                      SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_SHIFT);
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_SERDESDIGITAL_CONTROL1000X2r(unit, ws, ctrl2_data));
    }

    return SOC_E_NONE;
}
STATIC int
_phy_wcmod_control_linkdown_transmit_get(int unit,phy_ctrl_t *pc, uint32 *value)
{
    uint16      data;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));


    SOC_IF_ERROR_RETURN
        (READ_WC40_SERDESDIGITAL_CONTROL1000X2r(unit, ws, &data));

    /* Check if FORCE_XMIT_DATA_ON_TXSIDE is set */
    *value = (data & (SERDESDIGITAL_CONTROL1000X2_FORCE_XMIT_DATA_ON_TXSIDE_BITS <<
                      SERDESDIGITAL_CONTROL1000X2_FORCE_XMIT_DATA_ON_TXSIDE_SHIFT))
             ? 1 : 0;

    return SOC_E_NONE;
}

#if 0
STATIC int
_phy_wcmod_control_rx_taps_get(int unit, soc_port_t port)
{
    phy_ctrl_t *pc;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int rv;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    ws->diag_type = WCMOD_DIAG_RX_TAPS;

    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
    return SOC_E_NONE;
}

STATIC int
_phy_wcmod_control_slicers_get(int unit, soc_port_t port)
{
    phy_ctrl_t *pc;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int rv;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    ws->diag_type = WCMOD_DIAG_SLICERS;

    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
    return SOC_E_NONE;
}
#endif /* disable for now */


STATIC int
_phy_wcmod_control_rx_ppm_get(int unit, phy_ctrl_t *pc,
                                soc_phy_control_t type, uint32 *value)
{
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int rv;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    ws->diag_type = WCMOD_DIAG_RX_PPM;

    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
    *value = ws->accData;
    return SOC_E_NONE;
}
STATIC int
_phy_wcmod_control_diag_rx_taps(int unit, phy_ctrl_t *pc,
                                soc_phy_control_t type, uint32 *value)
{
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int rv;
    int tmpLane;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws    =  (wcmod_st *)( pDesc + 1);
    tmpLane = ws->this_lane;
    SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, ws));

    if        (type==SOC_PHY_CONTROL_RX_PEAK_FILTER) { ws->accData = 0x1;
    } else if (type==SOC_PHY_CONTROL_RX_VGA)         { ws->accData = 0x2;
    } else if (type==SOC_PHY_CONTROL_RX_TAP1)        { ws->accData = 0x3;
    } else if (type==SOC_PHY_CONTROL_RX_TAP2)        { ws->accData = 0x4;
    } else if (type==SOC_PHY_CONTROL_RX_TAP3)        { ws->accData = 0x5;
    } else if (type==SOC_PHY_CONTROL_RX_TAP4)        { ws->accData = 0x6;
    } else if (type==SOC_PHY_CONTROL_RX_TAP5)        { ws->accData = 0x7;
    } else if (type==SOC_PHY_CONTROL_RX_LOW_FREQ_PEAK_FILTER) { ws->accData = 0x8;
    }
    ws->diag_type = WCMOD_DIAG_RX_TAPS;
    SOC_IF_ERROR_RETURN(wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
    ws->this_lane = tmpLane; /* restore this_lane */
    *value = ws->accData;
    return SOC_E_NONE;
}

STATIC int
_phy_wcmod_per_lane_control_diag_rx_taps(int unit, soc_port_t port, int lane,
                                soc_phy_control_t type, uint32 *value)
{
    int tmpLane, temp_port_type;
    int rv;
    int lane_num, core_num = 0, max_core_num;
    wcmod_st    *ws, *temp_ws;
    WCMOD_DEV_DESC_t     *pDesc;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1); 

    SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, ws));

    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
        /*next to check if 4 or 8 lane port */
        if (SOC_INFO(unit).port_num_lanes[pc->port] == 4) {
            max_core_num = 2;
            core_num = lane / 2;
            lane_num = 2 + lane % 2;
        } else {
            max_core_num = 3;
            if (lane < 2) {
                core_num = 0;
                lane_num = 2 + lane % 2 ;
            } else if (lane >= 6) {
                core_num = 2;
                lane_num = 2 + lane % 2;
            } else {
                core_num = 1;
                lane_num = (lane - 2) % 4;
            }
        }
        temp_ws =  (wcmod_st *)( ws + core_num);
    } else {
        max_core_num = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        lane_num = (pc->lane_num + lane) % 4;
        core_num = (pc->lane_num + lane) / 4;
        temp_ws =  (wcmod_st *)( ws + core_num);
    }

    if( core_num >= max_core_num ) { return SOC_E_PARAM; }

    if        (type==SOC_PHY_CONTROL_RX_PEAK_FILTER) { temp_ws->accData = 0x1;
    } else if (type==SOC_PHY_CONTROL_RX_VGA)         { temp_ws->accData = 0x2;
    } else if (type==SOC_PHY_CONTROL_RX_TAP1)        { temp_ws->accData = 0x3;
    } else if (type==SOC_PHY_CONTROL_RX_TAP2)        { temp_ws->accData = 0x4;
    } else if (type==SOC_PHY_CONTROL_RX_TAP3)        { temp_ws->accData = 0x5;
    } else if (type==SOC_PHY_CONTROL_RX_TAP4)        { temp_ws->accData = 0x6;
    } else if (type==SOC_PHY_CONTROL_RX_TAP5)        { temp_ws->accData = 0x7;
    } else if (type==SOC_PHY_CONTROL_RX_LOW_FREQ_PEAK_FILTER) { temp_ws->accData = 0x8;
    }
    temp_ws->diag_type = WCMOD_DIAG_RX_TAPS;

    switch (lane_num) {
        case 0: 
            temp_ws->lane_select = WCMOD_LANE_0_0_0_1;
            break;
        case 1: 
            temp_ws->lane_select = WCMOD_LANE_0_0_1_0;
            break;
        case 2: 
            temp_ws->lane_select = WCMOD_LANE_0_1_0_0;
            break;
        case 3: 
            temp_ws->lane_select = WCMOD_LANE_1_0_0_0;
            break;
        default:
            break;
    }
    temp_port_type = temp_ws->port_type;
    temp_ws->port_type = WCMOD_INDEPENDENT;
    tmpLane = temp_ws->this_lane;
    temp_ws->this_lane = lane_num;
    SOC_IF_ERROR_RETURN(wcmod_tier1_selector("WCMOD_DIAG", temp_ws, &rv));
    *value = temp_ws->accData;
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_AERBLK_AERr(unit,temp_ws,0));
    /*restore all the info */
    temp_ws->port_type = temp_port_type;
    temp_ws->this_lane = tmpLane;
    return SOC_E_NONE;
}


STATIC int
_phy_wcmod_control_diag_slicers(int unit, phy_ctrl_t *pc,
                                soc_phy_control_t type, uint32 *value)
{
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int rv;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws    =  (wcmod_st *)( pDesc + 1);
    if       ( type == SOC_PHY_CONTROL_RX_PLUS1_SLICER ) { ws->accData = 0x1;
    } else if( type == SOC_PHY_CONTROL_RX_MINUS1_SLICER) { ws->accData = 0x2;
    } else if( type == SOC_PHY_CONTROL_RX_D_SLICER     ) { ws->accData = 0x3;
    }

    SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, ws));

    ws->diag_type = WCMOD_DIAG_SLICERS;
    SOC_IF_ERROR_RETURN (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
    *value = ws->accData;
    return SOC_E_NONE;
}

STATIC int
_phy_wcmod_per_lane_control_diag_slicers(int unit, soc_port_t port, int lane,
                                soc_phy_control_t type, uint32 *value)
{
    int tmpLane, temp_port_type;
    int rv;
    int lane_num, core_num = 0, max_core_num;
    wcmod_st    *ws, *temp_ws;
    WCMOD_DEV_DESC_t     *pDesc;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1); 

    SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, ws));

    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
        /*next to check if 4 or 8 lane port */
        if (SOC_INFO(unit).port_num_lanes[pc->port] == 4) {
            max_core_num = 2;
            core_num = lane / 2;
            lane_num = 2 + lane % 2;
        } else {
            max_core_num = 3;
            if (lane < 2) {
                core_num = 0;
                lane_num = 2 + lane % 2 ;
            } else if (lane >= 6) {
                core_num = 2;
                lane_num = 2 + lane % 2;
            } else {
                core_num = 1;
                lane_num = (lane - 2) % 4;
            }
        }
        temp_ws =  (wcmod_st *)( ws + core_num);
    } else {
        max_core_num = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        lane_num = (pc->lane_num + lane) % 4;
        core_num = (pc->lane_num + lane) / 4;
        temp_ws =  (wcmod_st *)( ws + core_num);
    }

    if( core_num >= max_core_num ) { return SOC_E_PARAM; }
    
    if       ( type == SOC_PHY_CONTROL_RX_PLUS1_SLICER ) { temp_ws->accData = 0x1;
    } else if( type == SOC_PHY_CONTROL_RX_MINUS1_SLICER) { temp_ws->accData = 0x2;
    } else if( type == SOC_PHY_CONTROL_RX_D_SLICER     ) { temp_ws->accData = 0x3;
    }

    switch (lane_num) {
        case 0: 
            temp_ws->lane_select = WCMOD_LANE_0_0_0_1;
            break;
        case 1: 
            temp_ws->lane_select = WCMOD_LANE_0_0_1_0;
            break;
        case 2: 
            temp_ws->lane_select = WCMOD_LANE_0_1_0_0;
            break;
        case 3: 
            temp_ws->lane_select = WCMOD_LANE_1_0_0_0;
            break;
        default:
            break;
    }
    temp_port_type = temp_ws->port_type;
    temp_ws->port_type = WCMOD_INDEPENDENT;
    tmpLane = temp_ws->this_lane;
    temp_ws->this_lane = lane_num;

    temp_ws->diag_type = WCMOD_DIAG_SLICERS;
    SOC_IF_ERROR_RETURN (wcmod_tier1_selector("WCMOD_DIAG", temp_ws, &rv));
    *value = temp_ws->accData;
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_AERBLK_AERr(unit,temp_ws,0));
    /*restore all the info */
    temp_ws->port_type = temp_port_type;
    temp_ws->this_lane = tmpLane;
    return SOC_E_NONE;
}


STATIC int
_phy_wcmod_control_rx_sigdet(int unit, phy_ctrl_t *pc, uint32 *value)
{
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int rv;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws    =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, ws));

    ws->diag_type = WCMOD_DIAG_RX_SIGDET;
    SOC_IF_ERROR_RETURN (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
    *value = ws->accData;
    return SOC_E_NONE;
}

STATIC int
_phy_wcmod_control_rx_seq_done_get(int unit, phy_ctrl_t *pc, uint32 *value)
{
    wcmod_st    *ws, *temp_ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int i, j, size, rv;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws    =  (wcmod_st *)( pDesc + 1);
    *value = 0;

    SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, ws));

    if (IS_MULTI_CORE_PORT(pc)) {
        int num_core, total_lane, tmpLane;
        total_lane = 0;
        /*check the special ILKN port */
        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
            num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
        } else {
            num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        }

        for (i = 0 ; i < num_core; i++) {
            temp_ws   =  (wcmod_st *)( ws + i);
            temp_ws->per_lane_control = 0x10;
            tmpLane = temp_ws->this_lane;
            for(j = tmpLane; j < tmpLane + temp_ws->num_of_lane ; j++) {
                temp_ws->this_lane = j;
                switch(j) {
                    case 0: temp_ws->lane_select = WCMOD_LANE_0_0_0_1 ; break ;
                    case 1: temp_ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                    case 2: temp_ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                    case 3: temp_ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
                default:
                    break;
                }
                SOC_IF_ERROR_RETURN
                   (wcmod_tier1_selector("RX_SEQ_CONTROL", temp_ws, &rv));
                *value |= temp_ws->accData << total_lane;
                total_lane++;
           }
        }
    } else {
        int tmpSelect, lane;
        ws->per_lane_control = 0x10;
        if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
            size = 4;
        } else if (IS_DUAL_LANE_PORT(pc)) {
            size = 2;
        } else {
            size = 1;
        }
        tmpSelect      = ws->lane_select ;
        for(i=0; i<size; i++) {
            lane = i + pc->lane_num ;
            switch(lane) {
                case 0: ws->lane_select = WCMOD_LANE_0_0_0_1 ; break ;
                case 1: ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                case 2: ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                case 3: ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
            default:
                break;
            }
            SOC_IF_ERROR_RETURN
               (wcmod_tier1_selector("RX_SEQ_CONTROL", ws, &rv));
                    *value |= ws->accData << i;
       }
       ws->lane_select = tmpSelect ;
    }
    return SOC_E_NONE;
}

STATIC int
_phy_wcmod_control_firmware_mode_set(int unit, phy_ctrl_t *pc, uint32 value)
{
    wcmod_st    *ws, *temp_ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int rv, per_lane_control = 0, size = 0, i = 0, j = 0;
    int link = 0;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws    =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, ws));

    SOC_IF_ERROR_RETURN (phy_wcmod_link_get(unit, pc->port, &link));
    ws->accAddr = link;

    /* check RX seq done status */
    switch (value) {
    case SOC_PHY_FIRMWARE_DEFAULT:
        per_lane_control = WCMOD_DEFAULT;
        break;
    case SOC_PHY_FIRMWARE_SFP_OPT_SR4:
        per_lane_control = WCMOD_SFP_OPT_SR4;
        break;
    case SOC_PHY_FIRMWARE_SFP_DAC:
        per_lane_control = WCMOD_SFP_DAC;
        break;
    case SOC_PHY_FIRMWARE_XLAUI:
        per_lane_control = WCMOD_XLAUI;
        break;
    case SOC_PHY_FIRMWARE_FORCE_OSDFE:
        per_lane_control = WCMOD_FORCE_OSDFE;
        break;
    case SOC_PHY_FIRMWARE_FORCE_BRDFE:
        per_lane_control = WCMOD_FORCE_BRDFE;
        break;
    case SOC_PHY_FIRMWARE_SW_CL72:
        per_lane_control = WCMOD_SW_CL72;
        break;
    case SOC_PHY_FIRMWARE_CL72_WITHOUT_AN:
        per_lane_control = WCMOD_CL72_woAN;
        break;
    default:
        break;
    }

    if (IS_MULTI_CORE_PORT(pc)) {
        int num_core, tmpLane;
        /*check the special ILKN port */
        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
            num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
        } else {
            num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        }
        for (i = 0 ; i < num_core; i++) {
            temp_ws   =  (wcmod_st *)( ws + i);
            temp_ws->per_lane_control = per_lane_control;
            tmpLane = temp_ws->this_lane; 
            for(j = tmpLane; j < tmpLane + temp_ws->num_of_lane ; j++) {
                temp_ws->this_lane = j;
                switch(j) {
                    case 0: temp_ws->lane_select = WCMOD_LANE_0_0_0_1 ; break ;
                    case 1: temp_ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                    case 2: temp_ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                    case 3: temp_ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
                default:
                    break;
                }
                SOC_IF_ERROR_RETURN
                   (wcmod_tier1_selector("FIRMWARE_MODE_SET", temp_ws, &rv));
           }
           temp_ws->this_lane = tmpLane;
        }
    } else {
        int tmpSelect, lane;
        ws->per_lane_control = per_lane_control;
        if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
            size = 4;
        } else if (IS_DUAL_LANE_PORT(pc)) {
            size = 2;
        } else {
            size = 1;
        }
        tmpSelect      = ws->lane_select ;
        for(i=0; i<size; i++) {
            lane = i + pc->lane_num ;
            switch(lane) {
                case 0: ws->lane_select = WCMOD_LANE_0_0_0_1 ; break ;
                case 1: ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                case 2: ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                case 3: ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
            default:
                break;
            }
            SOC_IF_ERROR_RETURN
               (wcmod_tier1_selector("FIRMWARE_MODE_SET", ws, &rv));
       }
       ws->lane_select = tmpSelect ;

    }
    return SOC_E_NONE;
}

STATIC int
_phy_wcmod_control_firmware_mode_get(int unit, phy_ctrl_t *pc, uint32 *value)
{
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    uint16 data;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws    =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, ws));

    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        /*read the firmware mode */
        SOC_IF_ERROR_RETURN (READ_WC40_UC_INFO_B1_FIRMWARE_MODEr(unit, ws, &data));
        data &= 0xf;
    } else {
        SOC_IF_ERROR_RETURN (READ_WC40_UC_INFO_B1_FIRMWARE_MODEr(unit, ws, &data));
        data >>= (ws->this_lane)*4;
        data &= 0xf;
    }

    switch (data) {
    case 0: *value = SOC_PHY_FIRMWARE_DEFAULT;
         break;
    case 1: *value = SOC_PHY_FIRMWARE_SFP_OPT_SR4;
         break;
    case 2: *value = SOC_PHY_FIRMWARE_SFP_DAC;
         break;
    case 3: *value = SOC_PHY_FIRMWARE_XLAUI;
         break;
    case 4: *value = SOC_PHY_FIRMWARE_FORCE_OSDFE;
         break;
    case 5: *value = SOC_PHY_FIRMWARE_FORCE_BRDFE;
         break;
    case 6: *value = SOC_PHY_FIRMWARE_SW_CL72;
         break;
    case 7: *value = SOC_PHY_FIRMWARE_CL72_WITHOUT_AN;
         break;
    default:
         break;
    }
    return SOC_E_NONE;
}

STATIC int
_phy_wcmod_control_vco_freq_get(int unit, phy_ctrl_t *pc, uint32 *value)
{
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    uint16 data, divider;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws    =  (wcmod_st *)( pDesc + 1);

    divider = 66; 

    SOC_IF_ERROR_RETURN(READ_WC40_PLL_ANAPLLSTATUSr(unit, ws, &data));

    switch(data & PLL_ANAPLLSTATUS_PLL_MODE_AFE_MASK){
        case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div32: divider  = 46;  break;
        case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div36: divider  = 72;  break;
        case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div40: divider  = 40;  break;
        case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div42: divider  = 42;  break;
        case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div48: divider  = 48;  break;
        case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div50: divider  = 50;  break;
        case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div52: divider  = 52;  break;
        case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div54: divider  = 54;  break;
        case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div60: divider  = 60;  break;
        case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div64: divider  = 64;  break;
        case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div66: divider  = 66;  break;
        case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div68: divider  = 68;  break;
        case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div70: divider  = 70;  break;
        case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div80: divider  = 80;  break;
        case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div92: divider  = 92;  break;
        case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div100: divider = 100; break;
        /* default: return SOC_E_CONFIG; */
    } 
    *value = ws->refclk * divider;
    return(SOC_E_NONE);
}
/* Make use of the TX0_ANATXACONTROL5.TX_PRBS_W_VAL as a scratch pad for VCO status */
#define TX0_ANATXACONTROL5_VCO_DISTURBED_MASK                            0x4000
#define TX0_ANATXACONTROL5_VCO_DISTURBED_ALIGN                           0
#define TX0_ANATXACONTROL5_VCO_DISTURBED_BITS                            1
#define TX0_ANATXACONTROL5_VCO_DISTURBED_SHIFT                           14

STATIC int
_phy_wcmod_control_vco_disturbed_get(int unit, phy_ctrl_t *pc, uint32 *value)
{
    wcmod_st            *ws;
    WCMOD_DEV_DESC_t    *pDesc;
    uint16              data;
    if(!pc) {
        return SOC_E_NONE;
    }
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws    = (wcmod_st *)( pDesc + 1);
    
    SOC_IF_ERROR_RETURN(READ_WC40_TX0_ANATXACONTROL5r(unit, ws, &data));
    SOC_IF_ERROR_RETURN(MODIFY_WC40_TX0_ANATXACONTROL5r(unit, ws, 0, (1 << TX0_ANATXACONTROL5_VCO_DISTURBED_SHIFT)));
    
    *value = (data & TX0_ANATXACONTROL5_VCO_DISTURBED_MASK) ? 1 : 0;
    
    return SOC_E_NONE;
}

STATIC int
_phy_wcmod_control_vco_disturbed_set(int unit, soc_port_t port)
{
    phy_ctrl_t          *pc;    
    wcmod_st            *ws;
    WCMOD_DEV_DESC_t    *pDesc;
    uint16              data, mask, tmpLane;
    int rv;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws    = (wcmod_st *)( pDesc + 1);
    
    SOC_IF_ERROR_RETURN
      (_wcmod_phy_parameter_copy(pc, ws));

    data = 1 << TX0_ANATXACONTROL5_VCO_DISTURBED_SHIFT;
    mask = data;
    
    tmpLane = ws->this_lane;
    ws->this_lane = 0;
    rv = MODIFY_WC40_TX0_ANATXACONTROL5r(unit, ws, data, mask);
    ws->this_lane = tmpLane; 
    
    return rv;
}

STATIC int 
phy_wcmod_rx_seq_toggle_set(int unit,  soc_port_t port, uint32 enable)
{
    wcmod_st  *temp_ws,   *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    phy_ctrl_t       *pc;
    int rv;
    int size, i, j;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN
      (_wcmod_phy_parameter_copy(pc, ws));

    if (IS_MULTI_CORE_PORT(pc)) {
        int num_core, tmpLane;
        /*check the special ILKN port */
        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
            num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
        } else {
            num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        }

        for (i = 0 ; i < num_core; i++) {
            temp_ws   =  (wcmod_st *)( ws + i);
            tmpLane = temp_ws->this_lane; 
            for(j = tmpLane; j < tmpLane + temp_ws->num_of_lane ; j++) {
                temp_ws->this_lane = j;
                switch(j) {
                    case 0: temp_ws->lane_select = WCMOD_LANE_0_0_0_1 ; break ;
                    case 1: temp_ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                    case 2: temp_ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                    case 3: temp_ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
                default:
                    break;
                }
                temp_ws->per_lane_control = WCMOD_DISABLE;
                SOC_IF_ERROR_RETURN
                     (wcmod_tier1_selector("RX_SEQ_CONTROL", temp_ws, &rv));
                temp_ws->per_lane_control = WCMOD_ENABLE;
                SOC_IF_ERROR_RETURN
                     (wcmod_tier1_selector("RX_SEQ_CONTROL", temp_ws, &rv));
           }
        }
    } else {
        int tmp_lane, tmp_lane_select;
        size = SOC_INFO(unit).port_num_lanes[pc->port];
        tmp_lane = ws->this_lane;
        tmp_lane_select = ws->lane_select; 
        for(j = pc->lane_num; j < size + pc->lane_num; j++) {
            ws->this_lane = j;
            switch(j) {
                case 0: ws->lane_select = WCMOD_LANE_0_0_0_1 ; break;
                case 1: ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                case 2: ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                case 3: ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
            default:
                break;
            }
            ws->per_lane_control = WCMOD_DISABLE;
            SOC_IF_ERROR_RETURN
                 (wcmod_tier1_selector("RX_SEQ_CONTROL", ws, &rv));
            ws->per_lane_control = WCMOD_ENABLE;
            SOC_IF_ERROR_RETURN
                 (wcmod_tier1_selector("RX_SEQ_CONTROL", ws, &rv));
       }
       ws->this_lane = tmp_lane;
       ws->lane_select =  tmp_lane_select;
    } 
    return(SOC_E_NONE);
}


/*
 * Function:
 *      phy_wcmod_diag_ctrl
 * Purpose:
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 * Returns:
 *      SOC_E_NONE
 * Notes:
 */

STATIC int
phy_wcmod_diag_ctrl(
   int unit, /* unit */
   soc_port_t port, /* port */
   uint32 inst, /* the specific device block the control action directs to */
   int op_type,  /* operation types: read,write or command sequence */
   int op_cmd,   /* command code */
   void *arg)     /* command argument based on op_type/op_cmd */
{
    phy_ctrl_t       *pc;
    wcmod_st            *ws, *temp_ws = NULL;      /*wcmod structure */
    WCMOD_DEV_DESC_t     *pDesc;
    int rv;
    int * pData = (int *)arg;
    int pc_lane_num;

    pc   = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);
    temp_ws   =  (wcmod_st *)( pDesc + 1);

    /*copy the phy control pataremeter into wcmod  */
    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    /* if the command is eyescan related, need to fisure out which core and lane to use */
    if ((op_cmd != PHY_DIAG_CTRL_REG_WRITE) && (op_cmd != PHY_DIAG_CTRL_REG_READ)) {
        pc_lane_num = pc->lane_num;
        if (IS_MULTI_CORE_PORT(pc)) {
            int i, num_core, port_is_100g, lane_map;
            port_is_100g = 0;
            if ((SOC_INFO(unit).port_num_lanes[pc->port] == 10) && !(SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port))) {
                port_is_100g = 1;
            }

            /*check the special ILKN port */
            if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {

                num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
            }
            else {                              
                num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
            }
            /*next we need to double check the active lane map for 100G application */
            if (port_is_100g) {
                lane_map = DEV_CFG_PTR(pc)->active_lane_map;
                for (i = 0; i < 12; i++) {
                    if (((lane_map >> i) & 0x1) == 0) continue;
                    if (pc_lane_num == 0) {
                        break;
                    } else {
                        pc_lane_num--;
                    }
                }
                /* now figure out which lane and which core to use */
                temp_ws   =  (wcmod_st *)( ws + i / 4);
                temp_ws->this_lane = i % 4;
            } else {
                for (i = 0 ; i < num_core; i++) {
                    temp_ws   =  (wcmod_st *)( ws + i);
                    if (pc_lane_num + 1 - temp_ws->num_of_lane > 0 ) {
                        pc_lane_num -= temp_ws->num_of_lane;
                    } else {
                        temp_ws->this_lane += pc_lane_num;
                        break;
                    }
                }
            }
            switch (temp_ws->this_lane) {
                case 0: temp_ws->lane_select = WCMOD_LANE_0_0_0_1 ; break ;
                case 1: temp_ws->lane_select = WCMOD_LANE_0_0_1_0 ; break ;
                case 2: temp_ws->lane_select = WCMOD_LANE_0_1_0_0 ; break ;
                case 3: temp_ws->lane_select = WCMOD_LANE_1_0_0_0 ; break ;
            default:
                break;
            }
            sal_memcpy(&ws, &temp_ws, sizeof(temp_ws));
        }                                 
    }
     
    switch(op_cmd) {
#if 0
        case PHY_DIAG_CTRL_EYE_MARGIN_LIVE_LINK:
            ws->diag_type = WCMOD_DIAG_EYE;
            ws->per_lane_control = WC_UTIL_2D_LIVELINK_EYESCAN;
            ws->arg = arg;
            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
            break;
#endif
        case PHY_DIAG_CTRL_EYE_MARGIN_PRBS:
            ws->diag_type = WCMOD_DIAG_EYE;
            ws->arg = arg;
            ws->per_lane_control = WC_UTIL_2D_PRBS_EYESCAN;
            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
            break;
        case PHY_DIAG_CTRL_EYE_ENABLE_LIVELINK:
            ws->diag_type = WCMOD_DIAG_EYE;
            ws->per_lane_control = WC_UTIL_ENABLE_LIVELINK;
            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
            break;
        case PHY_DIAG_CTRL_EYE_DISABLE_LIVELINK:
            ws->diag_type = WCMOD_DIAG_EYE;
            ws->per_lane_control = WC_UTIL_DISABLE_LIVELINK;
            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
            break;
        case PHY_DIAG_CTRL_EYE_ENABLE_DEADLINK:
            ws->diag_type = WCMOD_DIAG_EYE;
            ws->per_lane_control = WC_UTIL_ENABLE_DEADLINK;
            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
            break;
        case PHY_DIAG_CTRL_EYE_DISABLE_DEADLINK:
            ws->diag_type = WCMOD_DIAG_EYE;
            ws->per_lane_control = WC_UTIL_DISABLE_DEADLINK;
            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
            break;
        case PHY_DIAG_CTRL_EYE_SET_VOFFSET:
            ws->diag_type = WCMOD_DIAG_EYE;
            ws->accData =  *((int *)arg);
            ws->per_lane_control = WC_UTIL_SET_VOFFSET;
            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
            break;
        case PHY_DIAG_CTRL_EYE_SET_HOFFSET:
            ws->diag_type = WCMOD_DIAG_EYE;
            ws->accData =  *((int *)arg);
            ws->per_lane_control = WC_UTIL_SET_HOFFSET;
            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
            break;
        case PHY_DIAG_CTRL_EYE_GET_MAX_VOFFSET:
            ws->diag_type = WCMOD_DIAG_EYE;
            ws->per_lane_control = WC_UTIL_GET_MAX_VOFFSET;
            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
            *((int *)arg) = ws->accData;
            break;
        case PHY_DIAG_CTRL_EYE_GET_MIN_VOFFSET:
            ws->diag_type = WCMOD_DIAG_EYE;
            ws->per_lane_control = WC_UTIL_GET_MIN_VOFFSET;
            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
            *((int *)arg) = ws->accData;
            break;
        case PHY_DIAG_CTRL_EYE_GET_INIT_VOFFSET:
            ws->diag_type = WCMOD_DIAG_EYE;
            ws->per_lane_control = WC_UTIL_GET_INIT_VOFFSET;
            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
            *((int *)arg) = ws->accData;
            break;
        case PHY_DIAG_CTRL_EYE_GET_MAX_LEFT_HOFFSET:
            ws->diag_type = WCMOD_DIAG_EYE;
            ws->per_lane_control = WC_UTIL_GET_MAX_LEFT_HOFFSET;
            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
            *((int *)arg) = ws->accData;
            break;
        case PHY_DIAG_CTRL_EYE_GET_MAX_RIGHT_HOFFSET:
            ws->diag_type = WCMOD_DIAG_EYE;
            ws->per_lane_control = WC_UTIL_GET_MAX_RIGHT_HOFFSET;
            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
            *((int *)arg) = ws->accData;
            break;
        case PHY_DIAG_CTRL_EYE_START_LIVELINK:
            ws->diag_type = WCMOD_DIAG_EYE;
            ws->per_lane_control = WC_UTIL_START_LIVELINK;
            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
            break;
        case PHY_DIAG_CTRL_EYE_START_DEADLINK:
            ws->diag_type = WCMOD_DIAG_EYE;
            ws->per_lane_control = WC_UTIL_START_DEADLINK;
            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
            break;
        case PHY_DIAG_CTRL_EYE_STOP_LIVELINK:
            ws->diag_type = WCMOD_DIAG_EYE;
            ws->per_lane_control = WC_UTIL_STOP_LIVELINK;
            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
            break;
        case PHY_DIAG_CTRL_EYE_CLEAR_LIVELINK:
            ws->diag_type = WCMOD_DIAG_EYE;
            ws->per_lane_control = WC_UTIL_CLEAR_LIVELINK;
            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
            break;
        case PHY_DIAG_CTRL_EYE_READ_LIVELINK:
            ws->diag_type = WCMOD_DIAG_EYE;
            ws->per_lane_control = WC_UTIL_READ_LIVELINK;
            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
            *((int *)arg) = ws->accData;
            break;
        case PHY_DIAG_CTRL_EYE_READ_DEADLINK:
            ws->diag_type = WCMOD_DIAG_EYE;
            ws->per_lane_control = WC_UTIL_READ_DEADLINK;
            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
            *((int *)arg) = ws->accData;
            break;
        case PHY_DIAG_CTRL_EYE_MARGIN_VEYE_UP:
            ws->live_link = *pData++;
            ws->ber       = *pData++;
            ws->tol       = *pData;
            ws->per_lane_control = 1 << (pc->lane_num * 8);
            if(ws->verbosity > 0) printf("veye_up\n");
            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
            break;

        case PHY_DIAG_CTRL_EYE_MARGIN_VEYE_DOWN:
            ws->live_link = *pData++;
            ws->ber       = *pData++;
            ws->tol       = *pData;
            ws->per_lane_control = 2 << (pc->lane_num * 8);
            if(ws->verbosity > 0) printf("veye_down\n");
            SOC_IF_ERROR_RETURN
              (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
            break;

        case PHY_DIAG_CTRL_EYE_MARGIN_HEYE_RIGHT:
            if(ws->verbosity > 0) printf("heye_right\n");
            SOC_IF_ERROR_RETURN
              (wcmod_eye_margin(unit, port, WC_UTIL_HEYE_R));
            break;

        case PHY_DIAG_CTRL_EYE_MARGIN_HEYE_LEFT:
            if(ws->verbosity > 0) printf("heye_left\n");
            SOC_IF_ERROR_RETURN
              (wcmod_eye_margin(unit, port, WC_UTIL_HEYE_L));
            break;

        case PHY_DIAG_CTRL_EYE_MARGIN_VEYE:
            if(ws->verbosity > 0) printf("veye\n");
            SOC_IF_ERROR_RETURN
              (wcmod_eye_margin(unit, port,WC_UTIL_VEYE));
             break;
 
        case PHY_DIAG_CTRL_DSC:
             LOG_INFO(BSL_LS_SOC_PHY,
                      (BSL_META_U(pc->unit,
                                  "phy_wcmod_diag_ctrl: "
                                  "u=%d p=%d PHY_DIAG_CTRL_DSC 0x%x\n"),
                       unit, port, PHY_DIAG_CTRL_DSC));
            wcmod_uc_status_dump(unit, port, (soc_phy_wcmod_uc_desc_t*)arg);
            break;

        case PHY_DIAG_CTRL_REG_READ:
            {
                int core_num, aer, lane_num, temp_lane, i;
                uint32 addr, base_addr, reg_addr;
                uint16 reg_data[16], data16;

                core_num = *pData++;
                aer = *pData++;
                aer &= 0xffff;
                addr = *pData++;
                temp_ws = (wcmod_st *)( ws + core_num);
                /*figure out the lane num based on the aer */
                lane_num = aer & 0xf;
                addr |= aer << 16;
                temp_lane = temp_ws->this_lane;
                temp_ws->this_lane  = lane_num;
                base_addr = addr & 0xfffffff0;
                for (i = 0; i < 16; i++) {
                    reg_addr = base_addr + i;
                    wcmod_reg_aer_read(unit, temp_ws, reg_addr, &data16);
                    reg_data[i] = data16;
                }        

                if (addr & 0x8000) {
                    data16 = 0x10;
                } else {
                    data16 = 0x0;
                }
                
                LOG_CLI((BSL_META_U(unit,
                                    "PHY addr 0x%04x \n"), temp_ws->phy_ad));
                for (i = 0; i < 4; i++) {
                    printf("0x%02x: 0x%04x   0x%02x: 0x%04x   0x%02x: 0x%04x   0x%02x: 0x%04x \n",
                            data16 + i * 4,  reg_data[0 + i * 4], 
                            data16 + 1 + i * 4, reg_data[1 + i * 4], 
                            data16 + 2 + i * 4, reg_data[2 + i * 4],
                            data16 + 3 + i * 4, reg_data[3 + i * 4]);
                }                
                temp_ws->this_lane = temp_lane;
                break;
            }
        case PHY_DIAG_CTRL_REG_WRITE:
            {
                int core_num, aer,lane_num, temp_lane, tmp_lane_select;
                uint32 addr;
                uint16 wr_data;

                core_num = *pData++;
                aer = *pData++;
                aer &= 0xffff;
                addr = *pData++;
                wr_data = *pData++;
                temp_ws  = (wcmod_st *)( ws + core_num);
                temp_lane = temp_ws->this_lane;
                tmp_lane_select = temp_ws->lane_select;
                /*figure out the lane num based on the aer */
                lane_num = aer & 0x1ff;
                if (lane_num == 0x1ff) {
                    temp_ws->lane_select = WCMOD_LANE_BCST;
                } else {
                    lane_num &= 0x3;
                }
                addr |= aer << 16;
                temp_lane = temp_ws->this_lane;
                temp_ws->this_lane  = lane_num;
                wcmod_reg_aer_write(unit, temp_ws, addr, wr_data);
                temp_ws->this_lane = temp_lane;
                temp_ws->lane_select = tmp_lane_select;
                break;
            }
        default:
            if (op_type == PHY_DIAG_CTRL_SET) {
                (void)phy_wcmod_control_set(unit,port,op_cmd,PTR_TO_INT(arg));
            } else if (op_type == PHY_DIAG_CTRL_GET) {
                (void)phy_wcmod_control_get(unit,port,op_cmd,(uint32 *)arg);
            }
        break;
    }
    return SOC_E_NONE;
}
/*
 * Function:
 *      phy_wcmod_per_lane_control_set
 * Purpose:
 *      Configure PHY device specific control fucntion. 
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #. 
 *      lane  - lane number
 *      type  - Control to update 
 *      value - New setting for the control 
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_wcmod_per_lane_control_set(int unit, soc_port_t port, int lane,
                     soc_phy_control_t type, uint32 value)
{
    int rv;
    if ((type < 0) || (type >= SOC_PHY_CONTROL_COUNT)) {
        return SOC_E_PARAM;
    }

    rv = SOC_E_UNAVAIL;
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
        /* TX_ENABLE does both tx and rx */
        rv = _phy_wcmod_per_lane_control_preemphasis_set(unit, port, lane, value);
        break; 
    case SOC_PHY_CONTROL_DRIVER_CURRENT:           /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:       /* fall through */
    case SOC_PHY_CONTROL_DRIVER_POST2_CURRENT:     /* fall through */
        rv = _phy_wcmod_per_lane_control_tx_driver_set(unit, port, lane, type, value);
      break;
    case SOC_PHY_CONTROL_RX_TAP1:
       rv = _phy_wcmod_per_lane_rx_DFE_tap1_ovr_control_set (unit, port, lane, value);
      break;
    case SOC_PHY_CONTROL_RX_TAP1_RELEASE:
       rv = _phy_wcmod_per_lane_rx_DFE_tap1_ovr_control_set (unit, port, lane, 0x8000);
      break;
    case SOC_PHY_CONTROL_RX_TAP2:
       rv = _phy_wcmod_per_lane_rx_DFE_tap2_ovr_control_set (unit, port, lane, value);
      break;
    case SOC_PHY_CONTROL_RX_TAP2_RELEASE:
       rv = _phy_wcmod_per_lane_rx_DFE_tap2_ovr_control_set (unit, port, lane, 0x8000);
      break;
    case SOC_PHY_CONTROL_RX_TAP3:
       rv = _phy_wcmod_per_lane_rx_DFE_tap3_ovr_control_set (unit, port, lane, value);
      break;
    case SOC_PHY_CONTROL_RX_TAP3_RELEASE:
       rv = _phy_wcmod_per_lane_rx_DFE_tap3_ovr_control_set (unit, port, lane, 0x8000);
      break;
    case SOC_PHY_CONTROL_RX_TAP4:
       rv = _phy_wcmod_per_lane_rx_DFE_tap4_ovr_control_set (unit, port, lane, value);
      break;
    case SOC_PHY_CONTROL_RX_TAP4_RELEASE:
       rv = _phy_wcmod_per_lane_rx_DFE_tap4_ovr_control_set (unit, port, lane, 0x8000);
      break;
    case SOC_PHY_CONTROL_RX_TAP5:
       rv = _phy_wcmod_per_lane_rx_DFE_tap5_ovr_control_set (unit, port, lane, value);
      break;
    case SOC_PHY_CONTROL_RX_TAP5_RELEASE:
       rv = _phy_wcmod_per_lane_rx_DFE_tap5_ovr_control_set (unit, port, lane, 0x8000);
      break;
    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
       rv = phy_wcmod_per_lane_control_prbs_polynomial_set(unit, port, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
       rv = phy_wcmod_per_lane_control_prbs_tx_invert_data_set(unit, port, lane, value);
       break;
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
       /* TX_ENABLE does both tx and rx */
       rv = phy_wcmod_per_lane_control_prbs_enable_set(unit, port, lane, value);
       break; 
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
       rv = SOC_E_NONE;
       break;
    case SOC_PHY_CONTROL_RX_PEAK_FILTER:
       rv = _phy_wcmod_per_lane_rx_pf_ovr_control_set(unit, port,lane,  value);
      break;
    case SOC_PHY_CONTROL_RX_LOW_FREQ_PEAK_FILTER:
       rv = _phy_wcmod_per_lane_rx_low_freq_pf_ovr_control_set(unit, port, lane, value);
      break;
    case SOC_PHY_CONTROL_RX_VGA:
       rv = _phy_wcmod_per_lane_rx_vga_ovr_control_set(unit, port, lane, value);
      break;
    case SOC_PHY_CONTROL_RX_VGA_RELEASE:
       rv = _phy_wcmod_per_lane_rx_vga_ovr_control_set(unit, port, lane,  0x8000);
      break;
    case SOC_PHY_CONTROL_RX_PLUS1_SLICER:
       rv = _phy_wcmod_per_lane_rx_p1_slicer_control_set(unit, port, lane, value);
       break; 
    case SOC_PHY_CONTROL_RX_MINUS1_SLICER:
       rv = _phy_wcmod_per_lane_rx_m1_slicer_control_set(unit, port, lane, value);
       break; 
    case SOC_PHY_CONTROL_RX_D_SLICER:
       rv = _phy_wcmod_per_lane_rx_d_slicer_control_set(unit, port, lane, value);
      break;
    default:
       rv = SOC_E_UNAVAIL;
       break; 
    }
    return rv;
}

/*
 * Function:
 *      phy_wcmod_per_lane_control_get
 * Purpose:
 *      Configure PHY device specific control fucntion. 
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #. 
 *      lane  - lane number
 *      type  - Control to update 
 *      value - New setting for the control 
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_wcmod_per_lane_control_get(int unit, soc_port_t port, int lane,
                     soc_phy_control_t type, uint32 *value)
{
    int rv;
    if ((type < 0) || (type >= SOC_PHY_CONTROL_COUNT)) {
        return SOC_E_PARAM;
    }

    rv = SOC_E_UNAVAIL;
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
        /* TX_ENABLE does both tx and rx */
        rv = _phy_wcmod_per_lane_control_preemphasis_get(unit, port, lane, value);
        break; 
    case SOC_PHY_CONTROL_DRIVER_CURRENT:           /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:       /* fall through */
    case SOC_PHY_CONTROL_DRIVER_POST2_CURRENT:     /* fall through */
        rv = _phy_wcmod_per_lane_control_tx_driver_get(unit, port, lane, type, value);
      break;
    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
       rv = phy_wcmod_per_lane_control_prbs_polynomial_get(unit, port, lane, value);
       break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
       rv = phy_wcmod_per_lane_control_prbs_tx_invert_data_get(unit, port, lane, value);
      break;
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE: /* fall through */
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE: /* fall through */
       rv = phy_wcmod_per_lane_control_prbs_enable_get(unit, port, lane, value);
       break;
    case SOC_PHY_CONTROL_PRBS_RX_STATUS:
       rv = phy_wcmod_per_lane_control_prbs_rx_status_get(unit, port, lane, value);
       break;
    case SOC_PHY_CONTROL_RX_PEAK_FILTER:   /* fall through */
    case SOC_PHY_CONTROL_RX_VGA:           /* fall through */
    case SOC_PHY_CONTROL_RX_TAP1:          /* fall through */
    case SOC_PHY_CONTROL_RX_TAP2:          /* fall through */
    case SOC_PHY_CONTROL_RX_TAP3:          /* fall through */
    case SOC_PHY_CONTROL_RX_TAP4:          /* fall through */
    case SOC_PHY_CONTROL_RX_TAP5:          /* fall through */
    case SOC_PHY_CONTROL_RX_LOW_FREQ_PEAK_FILTER:  /* fall through */
      rv = _phy_wcmod_per_lane_control_diag_rx_taps(unit, port, lane, type, value);
      break;
    case SOC_PHY_CONTROL_RX_PLUS1_SLICER:  /*fall through */
    case SOC_PHY_CONTROL_RX_MINUS1_SLICER: /*fall through */
    case SOC_PHY_CONTROL_RX_D_SLICER:      /* fall through */
      rv = _phy_wcmod_per_lane_control_diag_slicers(unit, port, lane, type, value);  
      break;
    default:
        rv = SOC_E_UNAVAIL;
        break; 
    }
    return rv;
}



/*
 * Function:
 *      phy_wcmod_control_set
 * Purpose:
 *      Configure PHY device specific control fucntion. 
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #. 
 *      type  - Control to update 
 *      value - New setting for the control 
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_wcmod_control_set(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 value)
{
    phy_ctrl_t *pc;
    wcmod_st            *ws;      /*wcmod structure */
    WCMOD_DEV_DESC_t     *pDesc;    
    int rv;
    if ((type < 0) || (type >= SOC_PHY_CONTROL_COUNT)) {
        return SOC_E_PARAM;
    }

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);
    /*copy the phy control pataremeter into wcmod  */
    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));
    ws->this_lane = pc->lane_num;

    rv = SOC_E_UNAVAIL;
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE0: /* fall through */
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE1: /* fall through */
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE2: /* fall through */
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE3: /* fall through */
    case SOC_PHY_CONTROL_PREEMPHASIS:
        rv = _phy_wcmod_control_preemphasis_set(unit, pc, type, value);
        break;

    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0:     /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1:     /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2:     /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3:     /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE0: /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE1: /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE2: /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE3: /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT:           /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:       /* fall through */
    case SOC_PHY_CONTROL_DRIVER_POST2_CURRENT:     /* fall through */
        rv = _phy_wcmod_control_tx_driver_set(unit, pc, type, value);
      break;
    case SOC_PHY_CONTROL_DUMP:
        rv = wcmod_uc_status_dump(unit, port, NULL);
      break;
    case SOC_PHY_CONTROL_TX_LANE_SQUELCH:
        rv = _phy_wcmod_control_tx_lane_squelch_set(unit, pc, value);
      break;
    case SOC_PHY_CONTROL_RX_PEAK_FILTER:
       rv = _phy_wcmod_rx_pf_ovr_control_set(unit, pc, value);
      break;
    case SOC_PHY_CONTROL_RX_LOW_FREQ_PEAK_FILTER:
       rv = _phy_wcmod_rx_low_freq_pf_ovr_control_set(unit, pc, value);
      break;
    case SOC_PHY_CONTROL_RX_VGA:
       rv = _phy_wcmod_rx_vga_ovr_control_set(unit, pc, value);
      break;
    case SOC_PHY_CONTROL_RX_VGA_RELEASE:
       rv = _phy_wcmod_rx_vga_ovr_control_set(unit, pc, 0x8000);
      break;
    case SOC_PHY_CONTROL_RX_TAP1:
       rv = _phy_wcmod_rx_DFE_tap1_ovr_control_set (unit, pc, value);
      break;
    case SOC_PHY_CONTROL_RX_TAP1_RELEASE:
       rv = _phy_wcmod_rx_DFE_tap1_ovr_control_set (unit, pc, 0x8000);
      break;
    case SOC_PHY_CONTROL_RX_TAP2:
       rv = _phy_wcmod_rx_DFE_tap2_ovr_control_set (unit, pc, value);
      break;
    case SOC_PHY_CONTROL_RX_TAP2_RELEASE:
       rv = _phy_wcmod_rx_DFE_tap2_ovr_control_set (unit, pc, 0x8000);
      break;
    case SOC_PHY_CONTROL_RX_TAP3:
       rv = _phy_wcmod_rx_DFE_tap3_ovr_control_set (unit, pc, value);
      break;
    case SOC_PHY_CONTROL_RX_TAP3_RELEASE:
       rv = _phy_wcmod_rx_DFE_tap3_ovr_control_set (unit, pc, 0x8000);
      break;
    case SOC_PHY_CONTROL_RX_TAP4:
       rv = _phy_wcmod_rx_DFE_tap4_ovr_control_set (unit, pc, value);
      break;
    case SOC_PHY_CONTROL_RX_TAP4_RELEASE:
       rv = _phy_wcmod_rx_DFE_tap4_ovr_control_set (unit, pc, 0x8000);
      break;
    case SOC_PHY_CONTROL_RX_TAP5:
       rv = _phy_wcmod_rx_DFE_tap5_ovr_control_set (unit, pc, value);
      break;
    case SOC_PHY_CONTROL_RX_TAP5_RELEASE:
       rv = _phy_wcmod_rx_DFE_tap5_ovr_control_set (unit, pc, 0x8000);
      break;
    case SOC_PHY_CONTROL_RX_PLUS1_SLICER:
       rv = _phy_wcmod_rx_p1_slicer_control_set (unit, pc, value);
      break;
    case SOC_PHY_CONTROL_RX_MINUS1_SLICER:
       rv = _phy_wcmod_rx_m1_slicer_control_set (unit, pc, value);
      break;
    case SOC_PHY_CONTROL_RX_D_SLICER:
       rv = _phy_wcmod_rx_d_slicer_control_set (unit, pc, value);
      break;
    case SOC_PHY_CONTROL_PHASE_INTERP:
       rv = _phy_wcmod_pi_control_set (unit, pc, value);
      break;
    case SOC_PHY_CONTROL_RX_POLARITY:
       rv = _phy_wcmod_rx_polarity_set (unit, pc, value);
      break;
    case SOC_PHY_CONTROL_TX_POLARITY:
       rv = _phy_wcmod_tx_polarity_set (unit, pc, value);
      break;
    case SOC_PHY_CONTROL_TX_RESET:
       rv = _phy_wcmod_tx_reset (unit, pc, value);
      break;
    case SOC_PHY_CONTROL_RX_RESET:
       rv = _phy_wcmod_rx_reset (unit, pc, value);
      break;   
    case SOC_PHY_CONTROL_CL72:
       rv = _phy_wcmod_cl72_enable (unit, pc, value);
      break;
    case SOC_PHY_CONTROL_LANE_SWAP:
       rv = _phy_wcmod_control_lane_swap_set (unit, pc, value);
      break;
    case SOC_PHY_CONTROL_FIRMWARE_MODE:
       rv = _phy_wcmod_control_firmware_mode_set (unit, pc, value);
      break; 
    case SOC_PHY_CONTROL_TX_PATTERN_20BIT:
       rv = _phy_wcmod_control_tx_pattern_20bit_set (unit, pc, value);
      break;
    case SOC_PHY_CONTROL_TX_PATTERN_256BIT:
       rv = _phy_wcmod_control_tx_pattern_256bit_set (unit, pc, value);
      break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA0:
        ws->per_lane_control_1 = value;
        rv = SOC_E_NONE; 
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA1:
        ws->per_lane_control_2 = value;
        rv = SOC_E_NONE; 
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA2:
        ws->per_lane_control_3 = value;
        rv = SOC_E_NONE; 
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA3:
        ws->per_lane_control_4 = value;
        rv = SOC_E_NONE; 
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA4:
        ws->per_lane_control_5 = value;
        rv = SOC_E_NONE; 
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA5:
        ws->per_lane_control_6 = value;
        rv = SOC_E_NONE; 
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA6:
        ws->per_lane_control_7 = value;
        rv = SOC_E_NONE; 
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA7:
        ws->per_lane_control_8 = value;
        rv = SOC_E_NONE; 
        break;
    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
        rv = phy_wcmod_control_prbs_polynomial_set(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
        rv = phy_wcmod_control_prbs_tx_invert_data_set(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
        /* TX_ENABLE does both tx and rx */
        rv = phy_wcmod_control_prbs_enable_set(unit, port, value);
        break; 
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_POLYNOMIAL:
        rv = phy_wcmod_control_prbs_decouple_tx_polynomial_set(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_INVERT_DATA:
        rv = phy_wcmod_control_prbs_decouple_tx_invert_data_set(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_ENABLE:
        /* TX_ENABLE does both tx and rx */
        rv = phy_wcmod_control_prbs_decouple_tx_enable_set(unit, port, value);
        break; 
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_POLYNOMIAL:
        rv = phy_wcmod_control_prbs_decouple_rx_polynomial_set(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_INVERT_DATA:
        rv = phy_wcmod_control_prbs_decouple_rx_invert_data_set(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_ENABLE:
        /* TX_ENABLE does both tx and rx */
        rv = phy_wcmod_control_prbs_decouple_rx_enable_set(unit, port, value);
        break; 
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
        rv = SOC_E_NONE;
        break;
    case SOC_PHY_CONTROL_8B10B:
        rv = _phy_wcmod_control_8b10b_set(unit, pc, value);
        break;
    case SOC_PHY_CONTROL_RX_SEQ_TOGGLE:
        rv = phy_wcmod_rx_seq_toggle_set(unit, port, value);
        break;
#ifdef WC_EEE_SUPPORT 
    case SOC_PHY_CONTROL_EEE:           /* fall through */
    case SOC_PHY_CONTROL_EEE_AUTO:
        rv = _phy_wcmod_control_eee_set(unit,pc,type,value);
        break;
#endif
    case SOC_PHY_CONTROL_LOOPBACK_INTERNAL: 
        rv = phy_wcmod_lb_set(unit, port, value); 
        break; 
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE:
        rv = _phy_wcmod_rloop_set(unit,pc,value);
        break;
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE_PCS_BYPASS:
        rv = _phy_wcmod_rloop_pcsbypass_set(unit,port,value);
        break;
    case SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION:
        rv = _phy_wcmod_fec_set(unit,pc,value);
        break;
    case SOC_PHY_CONTROL_VCO_FREQ:
        ws->vco_freq = value;
        rv = SOC_E_NONE;
        break;
    case SOC_PHY_CONTROL_OVERSAMPLE_MODE:
        ws->oversample_mode = value;
        rv = SOC_E_NONE;
        break;
    case SOC_PHY_CONTROL_PLL_DIVIDER:
        ws->pll_divider = value;
        rv = SOC_E_NONE;
        break;
    case SOC_PHY_CONTROL_REF_CLK:
        DEV_CFG_PTR(pc)->refclk = value;
        rv = SOC_E_NONE;
        break;
    /* XXX obsolete */
    case SOC_PHY_CONTROL_CUSTOM1:
        DEV_CFG_PTR(pc)->custom1 = value? TRUE: FALSE;
        rv = SOC_E_NONE;
        break;
    case SOC_PHY_CONTROL_SCRAMBLER:
        DEV_CFG_PTR(pc)->scrambler_en = value? TRUE: FALSE;
        rv = SOC_E_NONE;
        break;
    case SOC_PHY_CONTROL_LINKDOWN_TRANSMIT:
        rv = _phy_wcmod_control_linkdown_transmit_set(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PARALLEL_DETECTION:
        /* enable 1000X parallel detect */
        SOC_IF_ERROR_RETURN
          (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X2r(unit, ws,
            value? SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK:0,
              SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK));
        rv = SOC_E_NONE;
        break;
    case SOC_PHY_CONTROL_BERT_PATTERN:     /* fall through */
    case SOC_PHY_CONTROL_BERT_RUN:         /* fall through */
    case SOC_PHY_CONTROL_BERT_PACKET_SIZE: /* fall through */
    case SOC_PHY_CONTROL_BERT_IPG:
        rv = _phy_wcmod_control_bert_set(unit,pc,type,value);
        break;
    case SOC_PHY_CONTROL_SOFTWARE_RX_LOS:
#if 0 
        rv = SOC_E_FAIL;
        phy_wc40_interface_get(unit, port, &interface);
        /* Only enable the feature for 10G/40G fiber */
        if( (interface == SOC_PORT_IF_SFI) || (interface == SOC_PORT_IF_SR)) { 
            SOC_IF_ERROR_RETURN
                (READ_WC40_UC_INFO_B1_FIRMWARE_MODEr(unit, pc, 0x00, &data16));
            if ((data16 == WC40_UC_CTRL_DEFAULT) 
                || (data16 == WC40_UC_CTRL_SR4) 
                || (data16 == WC40_UC_CTRL_OPT_PF_TEMP_COMP)) {

#endif
        /* Only enable the feature for 10G/40G fiber */
        DEV_CFG_PTR(pc)->sw_rx_los.enable = (value == 0? 0: 1);
        DEV_CFG_PTR(pc)->sw_rx_los.sys_link = 0; 
        DEV_CFG_PTR(pc)->sw_rx_los.state = RESET;
        DEV_CFG_PTR(pc)->sw_rx_los.link_status = 0;
        /* Manage the _SERVICE_INT_PHY_LINK_GET flag so that 
           if external phy is present link_get for 
           internal phy is still called to process 
           software rx los feature */ 
        if(value) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_SERVICE_INT_PHY_LINK_GET);
        } else {
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_SERVICE_INT_PHY_LINK_GET);
        }
        rv = SOC_E_NONE;
        break;     
    case SOC_PHY_CONTROL_TX_PPM_ADJUST:
        rv = _phy_wcmod_tx_pi_control_set(unit, pc, value);
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break; 
    }
    return rv;
}

/*
 * Function:
 *      phy_wcmod_control_get
 * Purpose:
 *      Get current control settign of the PHY. 
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #. 
 *      type  - Control to update 
 *      value - (OUT)Current setting for the control 
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_wcmod_control_get(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 *value)
{
    int rv;
    uint16 data16;
    int intf;
    int scr;
    int speed;
    phy_ctrl_t *pc;
    WCMOD_DEV_DESC_t           *pDesc;
    wcmod_st   *ws;      /*wcmod structure */

    if (NULL == value) {
      return SOC_E_PARAM;
    }
    if ((type < 0) || (type >= SOC_PHY_CONTROL_COUNT)) {
      return SOC_E_PARAM;
    }
   
    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws    =  (wcmod_st *)( pDesc + 1);
    /*copy the phy control parameter into wcmod */
    SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, ws));
    rv = SOC_E_UNAVAIL;

    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE0: /* fall through */
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE1: /* fall through */
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE2: /* fall through */
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE3: /* fall through */
    case SOC_PHY_CONTROL_PREEMPHASIS:
      rv = _phy_wcmod_control_preemphasis_get(unit, pc, type, value);
      break;

    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0:     /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1:     /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2:     /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3:     /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE0: /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE1: /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE2: /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE3: /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT:           /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:       /* fall through */
    case SOC_PHY_CONTROL_DRIVER_POST2_CURRENT:
      rv = _phy_wcmod_control_tx_driver_get(unit, pc, type, value);
      break;
    case SOC_PHY_CONTROL_RX_PEAK_FILTER:   /* fall through */
    case SOC_PHY_CONTROL_RX_VGA:           /* fall through */
    case SOC_PHY_CONTROL_RX_TAP1:          /* fall through */
    case SOC_PHY_CONTROL_RX_TAP2:          /* fall through */
    case SOC_PHY_CONTROL_RX_TAP3:          /* fall through */
    case SOC_PHY_CONTROL_RX_TAP4:          /* fall through */
    case SOC_PHY_CONTROL_RX_TAP5:          /* fall through */
    case SOC_PHY_CONTROL_RX_LOW_FREQ_PEAK_FILTER:  /* fall through */
      rv = _phy_wcmod_control_diag_rx_taps(unit, pc, type, value);
      break;
    case SOC_PHY_CONTROL_RX_PLUS1_SLICER:  /* fall through */
    case SOC_PHY_CONTROL_RX_MINUS1_SLICER: /* fall through */
    case SOC_PHY_CONTROL_RX_D_SLICER:      /* fall through */
      rv = _phy_wcmod_control_diag_slicers(unit, pc, type, value);
      break;
    case SOC_PHY_CONTROL_RX_SIGNAL_DETECT:
      rv = _phy_wcmod_control_rx_sigdet(unit, pc, value);
      break;
    case SOC_PHY_CONTROL_RX_SEQ_DONE:
      rv = _phy_wcmod_control_rx_seq_done_get(unit, pc, value);
      break;
    case SOC_PHY_CONTROL_RX_PPM:
      rv = _phy_wcmod_control_rx_ppm_get(unit, pc, type, value);
      break;
    case SOC_PHY_CONTROL_RX_RESET:
       rv = _phy_wcmod_rx_reset_get (unit, pc, value);
      break;  
    case SOC_PHY_CONTROL_CL72:
      rv = _phy_wcmod_cl72_enable_get(unit, pc, value);
      break;
    case SOC_PHY_CONTROL_CL72_STATUS:
      rv = _phy_wcmod_cl72_status_get(unit, pc, value);
      break;
    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
      rv = phy_wcmod_control_prbs_polynomial_get(unit, port, value);
      break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
      rv = phy_wcmod_control_prbs_tx_invert_data_get(unit, port, value);
      break;
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE: /* fall through */
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE: /* fall through */
      rv = phy_wcmod_control_prbs_enable_get(unit, port, value);
      break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_ENABLE: /* fall through */
      rv = phy_wcmod_control_prbs_decouple_tx_enable_get(unit, port, value);
      break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_ENABLE: /* fall through */
      rv = phy_wcmod_control_prbs_decouple_rx_enable_get(unit, port, value);
      break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_INVERT_DATA: /* fall through */
      rv = phy_wcmod_control_prbs_decouple_tx_invert_data_get(unit, port, value);
      break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_INVERT_DATA: /* fall through */
      rv = phy_wcmod_control_prbs_decouple_rx_invert_data_get(unit, port, value);
      break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_POLYNOMIAL: /* fall through */
      rv = phy_wcmod_control_prbs_decouple_tx_polynomial_get(unit, port, value);
      break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_POLYNOMIAL: /* fall through */
      rv = phy_wcmod_control_prbs_decouple_rx_polynomial_get(unit, port, value);
      break;
    case SOC_PHY_CONTROL_PRBS_RX_STATUS:
      rv = phy_wcmod_control_prbs_rx_status_get(unit, port, value);
      break;
    case SOC_PHY_CONTROL_LOOPBACK_INTERNAL: 
      rv = phy_wcmod_lb_get(unit, port, (int *)value);
      break; 
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE:
      rv = _phy_wcmod_rloop_get(unit,pc,value);
      break;
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE_PCS_BYPASS:
      rv = _phy_wcmod_rloop_pcs_bypass_get(unit, pc, value);
      break;
    case SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION:
        rv = _phy_wcmod_fec_get(unit,pc,value);
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_256BIT:
       rv = _phy_wcmod_control_tx_pattern_256bit_get (unit, pc, value);
      break;
    case SOC_PHY_CONTROL_TX_PATTERN_20BIT:
       rv = _phy_wcmod_control_tx_pattern_20bit_get (unit, pc, value);
      break;
    case SOC_PHY_CONTROL_CUSTOM1:
      *value = DEV_CFG_PTR(pc)->custom1;
      rv = SOC_E_NONE;
      break;
    case SOC_PHY_CONTROL_VCO_FREQ:
      rv = _phy_wcmod_control_vco_freq_get(unit, pc, value);
      rv = SOC_E_NONE;
      break;
    case SOC_PHY_CONTROL_VCO_DISTURBED:
      rv = _phy_wcmod_control_vco_disturbed_get(unit, pc, value);
      rv = SOC_E_NONE;
      break;
    case SOC_PHY_CONTROL_OVERSAMPLE_MODE:
      *value = ws->oversample_mode;
      rv = SOC_E_NONE;
      break;
    case SOC_PHY_CONTROL_PLL_DIVIDER:
      *value = ws->pll_divider;
      rv = SOC_E_NONE;
      break;
    case SOC_PHY_CONTROL_REF_CLK:
      *value = DEV_CFG_PTR(pc)->refclk;
      rv = SOC_E_NONE;
      break;
    case SOC_PHY_CONTROL_SCRAMBLER:
      if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        rv = _phy_wcmod_combo_speed_get(unit, port, &speed,&intf,&scr);
      } else {
        rv = _phy_wcmod_ind_speed_get(unit, port, &speed,&intf,&scr);
      }
      *value = scr;
      break;
    case SOC_PHY_CONTROL_LINKDOWN_TRANSMIT:
      rv = _phy_wcmod_control_linkdown_transmit_get(unit, pc, value);
      break;
    case SOC_PHY_CONTROL_PARALLEL_DETECTION:
        SOC_IF_ERROR_RETURN
            (READ_WC40_SERDESDIGITAL_CONTROL1000X2r(unit, ws, &data16));
        *value = data16 & SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK?
                 TRUE: FALSE;
        rv = SOC_E_NONE;
        break;
    case SOC_PHY_CONTROL_BERT_TX_PACKETS:       /* fall through */
    case SOC_PHY_CONTROL_BERT_RX_PACKETS:       /* fall through */
    case SOC_PHY_CONTROL_BERT_RX_ERROR_BITS:    /* fall through */
    case SOC_PHY_CONTROL_BERT_RX_ERROR_BYTES:   /* fall through */
    case SOC_PHY_CONTROL_BERT_RX_ERROR_PACKETS: /* fall through */
    case SOC_PHY_CONTROL_BERT_PATTERN:          /* fall through */
    case SOC_PHY_CONTROL_BERT_PACKET_SIZE:      /* fall through */
    case SOC_PHY_CONTROL_BERT_IPG:              /* fall through */
        rv = _phy_wcmod_control_bert_get(unit,pc,type,value);
        break;
    case SOC_PHY_CONTROL_RX_POLARITY:
        rv = _phy_wcmod_rx_polarity_get (unit, pc, value);
        break;
    case SOC_PHY_CONTROL_TX_POLARITY:
        rv = _phy_wcmod_tx_polarity_get (unit, pc, value);
        break;
    case SOC_PHY_CONTROL_FIRMWARE_MODE:
       rv = _phy_wcmod_control_firmware_mode_get (unit, pc, value);
       break;
    case SOC_PHY_CONTROL_SOFTWARE_RX_LOS:
        *value = DEV_CFG_PTR(pc)->sw_rx_los.enable;
        rv = SOC_E_NONE;
        break; 
    case SOC_PHY_CONTROL_TX_LANE_SQUELCH:
        rv = _phy_wcmod_control_tx_lane_squelch_get(unit, pc, value);
      break;
    default:
        rv = SOC_E_UNAVAIL;
        break; 
    }
    return rv;
}     

typedef struct {
    uint16 start;
    uint16 end;
} WCMOD_LANE0_REG_BLOCK;

STATIC WCMOD_LANE0_REG_BLOCK wcmod_lane0_reg_block[] = {
    {0x8050,0x80FE},  /* register in this block only valid at lane0 */
    {0x8000,0x8001},  /* valid at lane0 */
    {0x8015,0x8019}  /* valid at lane0 */
};

STATIC int
_wcmod_lane0_reg_access(int unit, phy_ctrl_t *pc,uint16 reg_addr)
{
    int ix = 0;
    int num_blk;
    WCMOD_LANE0_REG_BLOCK * pBlk;

    num_blk = sizeof(wcmod_lane0_reg_block) / sizeof(wcmod_lane0_reg_block[0]);
    for (ix = 0; ix < num_blk; ix++) {
        pBlk = &wcmod_lane0_reg_block[ix];
        if ((reg_addr >= pBlk->start) && (reg_addr <= pBlk->end)) {
            return TRUE;
        }
    }
    return FALSE;
}

STATIC int
phy_wcmod_reg_aer_read(int unit, phy_ctrl_t *pc, uint32 flags,
                  uint32 phy_reg_addr, uint16 *phy_data)
{
    uint16 data;     
    uint32 lane;         /* lane to access, override the default lane */
    wcmod_st            *ws;      /*wcmod structure */
    WCMOD_DEV_DESC_t    *pDesc;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    lane = flags & LANE_ACCESS_MASK;

    /* safety check */
    if ((lane > 4) || (lane == LANE_BCST)) {
        lane = LANE0_ACCESS;
    }

    /* check if register only available on lane0 */
    if (_wcmod_lane0_reg_access(unit,pc,(uint16)(phy_reg_addr & 0xffff))) {
        lane = LANE0_ACCESS;
    }
 
    /*copy the phy control pataremeter into wcmod  */
    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    if (lane)
    {
        ws->lane_num_ignore = 1;
    }
    SOC_IF_ERROR_RETURN
        (wcmod_reg_aer_read(unit, ws, phy_reg_addr, &data));

    *phy_data = data;

    ws->lane_num_ignore = 0 ;
    return SOC_E_NONE;
}

STATIC int
phy_wcmod_reg_aer_write(int unit, phy_ctrl_t *pc, uint32 flags,
                   uint32 phy_reg_addr, uint16 phy_data)
{
    uint16       dxgxs,data;     
    uint32 lane;
    wcmod_st            *ws;      /*wcmod structure */
    WCMOD_DEV_DESC_t    *pDesc;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    data      = (uint16) (phy_data & 0x0000FFFF);
    dxgxs = DEV_CFG_PTR(pc)->dxgxs;
 
    lane = flags & LANE_ACCESS_MASK;

    /* safety check */
    if ((lane > 4) && (lane != LANE_BCST)) {
        lane = LANE0_ACCESS;
    }

    /* check if register only available on lane0 or should be done from lane0 */
    if ((_wcmod_lane0_reg_access(unit,pc,(uint16)(phy_reg_addr & 0xffff))) || dxgxs) {
        lane = LANE0_ACCESS;
    }

    /*copy the phy control pataremeter into wcmod  */
    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    if (lane)
    {
        ws->lane_num_ignore = 1;
    }
    SOC_IF_ERROR_RETURN
        (wcmod_reg_aer_write(unit, ws, phy_reg_addr, data));

    ws->lane_num_ignore = 0 ;
    return SOC_E_NONE;
}

STATIC int
phy_wcmod_reg_aer_modify(int unit, phy_ctrl_t *pc, uint32 flags,
                    uint32 phy_reg_addr, uint16 phy_data,
                    uint16 phy_data_mask)
{
    uint16               data;     /* Temporary holder for phy_data */
    uint16               mask;
    uint16               lane;     /*lane number */
    wcmod_st            *ws;      /*wcmod structure */
    WCMOD_DEV_DESC_t    *pDesc;

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    data      = (uint16) (phy_data & 0x0000FFFF);
    mask      = (uint16) (phy_data_mask & 0x0000FFFF);

    lane = flags & LANE_ACCESS_MASK;

    /* safety check */
    if ((lane > 4) || (lane == LANE_BCST)) {
        lane = LANE0_ACCESS;
    }

    /* check if register only available on lane0 */
    if (_wcmod_lane0_reg_access(unit,pc,(uint16)(phy_reg_addr & 0xffff))) {
        lane = LANE0_ACCESS;
    }

    /*copy the phy control pataremeter into wcmod*/
    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    if (lane)
    {
        ws->lane_num_ignore = 1;
    }

    SOC_IF_ERROR_RETURN
        (wcmod_reg_aer_modify(unit, ws, phy_reg_addr, data, mask));

    ws->lane_num_ignore = 0;
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_wcmod_reg_read
 * Purpose:
 *      Routine to read PHY register
 * Parameters:
 *      uint         - BCM unit number
 *      pc           - PHY state
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_data     - (OUT) Value read from PHY register
 * Note:
 *      This register read function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
STATIC int
phy_wcmod_reg_read(int unit, soc_port_t port, uint32 flags,
                  uint32 phy_reg_addr, uint32 *phy_data)
{
    uint16               data = 0;/* Temporary holder for phy_data */
    phy_ctrl_t          *pc;      /* PHY software state */

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (phy_wcmod_reg_aer_read(unit, pc, 0x00, phy_reg_addr, &data));

    *phy_data = data;


    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_wcmod_reg_write
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
phy_wcmod_reg_write(int unit, soc_port_t port, uint32 flags,
                   uint32 phy_reg_addr, uint32 phy_data)
{
    uint16               data;     /* Temporary holder for phy_data */
    phy_ctrl_t          *pc;      /* PHY software state */

    pc = INT_PHY_SW_STATE(unit, port);

    data      = (uint16) (phy_data & 0x0000FFFF);

    SOC_IF_ERROR_RETURN
        (phy_wcmod_reg_aer_write(unit, pc, 0x00, phy_reg_addr, data));

    return SOC_E_NONE;
}  

/*
 * Function:
 *      phy_wcmod_reg_modify
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
 *      This register read function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
STATIC int
phy_wcmod_reg_modify(int unit, soc_port_t port, uint32 flags,
                    uint32 phy_reg_addr, uint32 phy_data,
                    uint32 phy_data_mask)
{
    uint16               data;     /* Temporary holder for phy_data */
    uint16               mask;
    phy_ctrl_t    *pc;      /* PHY software state */

    pc = INT_PHY_SW_STATE(unit, port);

    data      = (uint16) (phy_data & 0x0000FFFF);
    mask      = (uint16) (phy_data_mask & 0x0000FFFF);

    SOC_IF_ERROR_RETURN
        (phy_wcmod_reg_aer_modify(unit, pc, 0x00, phy_reg_addr, data, mask));

    return SOC_E_NONE;
}

STATIC int
phy_wcmod_probe(int unit, phy_ctrl_t *pc)
{
    wcmod_st            ws;      /* wcmod structure    */
    uint16      serdes_id0;
    int num_core;
    int rv;     

    sal_memset(&ws, 0, sizeof(wcmod_st));    
    ws.port = pc->port;
    ws.unit = pc->unit;
    ws.phy_ad     = pc->phy_id;
    ws.mdio_type  = WCMOD_MDIO_CL22;
    ws.port_type  = WCMOD_INDEPENDENT;
    ws.lane_num_ignore = TRUE;
    ws.read  = pc->read;
    ws.write = pc->write;

    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("REVID_READ", &ws, &rv));

    serdes_id0 = ws.accData;
#if 0
    LOG_CLI((BSL_META_U(pc->unit,
                        "WCmod ID 0x%04x\n"), serdes_id0));
#endif

    if (((serdes_id0 & SERDESID_SERDESID0_MODEL_NUMBER_MASK) == MODEL_WARPLITE) || 
        ((serdes_id0 & SERDESID_SERDESID0_MODEL_NUMBER_MASK) == MODEL_WARPCORE_C) ||
        ((serdes_id0 & SERDESID_SERDESID0_MODEL_NUMBER_MASK) == MODEL_QSGMII) ||
        ((serdes_id0 & SERDESID_SERDESID0_MODEL_NUMBER_MASK) == MODEL_QUADSGMII) ||
        ((serdes_id0 & SERDESID_SERDESID0_MODEL_NUMBER_MASK) == MODEL_XGXS_16G)) { 
    /* if ((serdes_id0 & SERDESID_SERDESID0_MODEL_NUMBER_MASK)==MODEL_WARPCORE){  */
    } else {
      return SOC_E_NOT_FOUND;
    }

    if ((serdes_id0 & SERDESID_SERDESID0_MODEL_NUMBER_MASK) == MODEL_XGXS_16G) { 
        if (!SOC_IS_TRIUMPH3(unit)) {
            return SOC_E_NOT_FOUND;
        }
    }

    if ((serdes_id0 & SERDESID_SERDESID0_MODEL_NUMBER_MASK) == MODEL_QSGMII) { 
        if (!SOC_IS_HELIX4(unit) && !SOC_IS_HURRICANE2(unit)) {
            return SOC_E_NOT_FOUND;
        }
    }

    if ((serdes_id0 & SERDESID_SERDESID0_MODEL_NUMBER_MASK) == MODEL_QUADSGMII) { 
        if (!SOC_IS_HELIX4(unit)) {
            return SOC_E_NOT_FOUND;
        }
    }

    /* if xenia core, enable Multiport for now */
    if ((serdes_id0 & SERDESID_SERDESID0_MODEL_NUMBER_MASK) == MODEL_XGXS_16G) {
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_XGXSBLK0_MMDSELECTr(unit, &ws, 
             XGXSBLK0_MMDSELECT_MULTIPRTS_EN_MASK, XGXSBLK0_MMDSELECT_MULTIPRTS_EN_MASK));
    }

    /* ask to allocate the driver specific descripor  */
    /* first check to see if the interlocken port */
    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) {
        num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        if (num_core > 3) {
            pc->size = sizeof(WCMOD_DEV_DESC_t) + sizeof(wcmod_st) * (num_core + 2);
            pc->dev_name = wcmod_device_name;
        } else {
            pc->size = sizeof(WCMOD_DEV_DESC_t) + sizeof(wcmod_st) * (num_core + 1);
            pc->dev_name = wcmod_device_name;
        }
    } else {
        if (SOC_INFO(unit).port_speed_max[pc->port] >= 100000) {
            pc->size = sizeof(WCMOD_DEV_DESC_t) + sizeof(wcmod_st)*4;
            pc->dev_name = wcmod_device_name;
        } else {
            pc->size = sizeof(WCMOD_DEV_DESC_t) + sizeof(wcmod_st);
            pc->dev_name = wcmod_device_name;
        }
    }
    return SOC_E_NONE;
}

STATIC int
phy_wcmod_notify(int unit, soc_port_t port,
                       soc_phy_event_t event, uint32 value)
{
    int             rv;
    phy_ctrl_t    *pc;      /* PHY software state */

    pc = INT_PHY_SW_STATE(unit, port);

    if (event >= phyEventCount) {
        return SOC_E_PARAM;
    }

    if (CUSTOM_MODE(pc)) {
        return SOC_E_NONE;
    }

    rv = SOC_E_NONE;
    switch(event) {
    case phyEventInterface:
        rv = (_phy_wcmod_notify_interface(unit, port, value));
        break;
    case phyEventDuplex:
        rv = (_phy_wcmod_notify_duplex(unit, port, value));
        break;
    case phyEventSpeed:
        rv = (_phy_wcmod_notify_speed(unit, port, value));
        break;
    case phyEventStop:
        rv = (_phy_wcmod_notify_stop(unit, port, value));
        break;
    case phyEventResume:
        rv = (_phy_wcmod_notify_resume(unit, port, value));
        break;
    case phyEventAutoneg:
        rv = (phy_wcmod_an_set(unit, port, value));
        break;
    case phyEventTxFifoReset:
        rv = (_phy_wcmod_tx_fifo_reset(unit, port, value));
        break;
    case phyEventMacLoopback:
        rv = (_phy_wcmod_notify_mac_loopback(unit, port, value));
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break;
    }

    return rv;
}


STATIC int
_phy_wcmod_multi_core_polarity_config(int unit, soc_port_t port)
{
    int i, num_core, rv = SOC_E_NONE;
    phy_ctrl_t *pc;
    wcmod_st   *ws, *temp_ws;
    WCMOD_DEV_DESC_t     *pDesc;
    WCMOD_DEV_CFG_t *pCfg;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws =  (wcmod_st *)( pDesc + 1);
    pCfg = &pDesc->cfg;
    SOC_IF_ERROR_RETURN(_wcmod_phy_parameter_copy(pc, ws));

    /*check the special ILKN port */
    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
        num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
        if (SOC_INFO(unit).port_num_lanes[pc->port] == 4) {
            for (i= 0; i < num_core;  i++) {
                temp_ws   =  (wcmod_st *)( ws + i);
                temp_ws->dual_type = 1;
                temp_ws->per_lane_control  = (pCfg->rxpol>>(i * 2)) <<2 ; 
                if ((pCfg->rxpol>>(i * 2))) temp_ws->per_lane_control |= 0x1;
                SOC_IF_ERROR_RETURN (wcmod_tier1_selector("SET_POLARITY", temp_ws, &rv));

                temp_ws->per_lane_control  = (pCfg->txpol>>(i * 2)) <<2 ; 
                if ((pCfg->txpol>>(i * 2))) temp_ws->per_lane_control |= 0x1;
                temp_ws->per_lane_control |=  0x2;
                SOC_IF_ERROR_RETURN (wcmod_tier1_selector("SET_POLARITY", temp_ws, &rv));
            }
        } else {
            for (i= 0; i < num_core;  i++) {
                int shift = 0;
                if (i == 1) {
                    shift = 2;
                } else if (i == 2) {
                    shift = 6;
                }
                temp_ws   =  (wcmod_st *)( ws + i);
                if (temp_ws->this_lane == 0) {
                    temp_ws->dual_type = 0;
                    temp_ws->port_type = WCMOD_COMBO;
                } else {
                    temp_ws->dual_type = 1;
                }
                temp_ws->per_lane_control  = (pCfg->rxpol>>shift) <<2 ; 
                if ((pCfg->rxpol>>shift)) temp_ws->per_lane_control |= 0x1;
                SOC_IF_ERROR_RETURN (wcmod_tier1_selector("SET_POLARITY", temp_ws, &rv));

                temp_ws->per_lane_control  = (pCfg->txpol>>shift) <<2 ; 
                if (pCfg->txpol>>shift) temp_ws->per_lane_control |= 0x1;
                temp_ws->per_lane_control |=  0x2;
                SOC_IF_ERROR_RETURN (wcmod_tier1_selector("SET_POLARITY", temp_ws, &rv));
            }
        }
    } else {
        num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        for (i= 0; i < num_core;  i++) {
            temp_ws   =  (wcmod_st *)( ws + i);
            temp_ws->port_type = WCMOD_COMBO;
            temp_ws->per_lane_control  = (pCfg->rxpol>>(i * NUM_LANES)) <<2 ; 
            if ((pCfg->rxpol>>(i * NUM_LANES))) temp_ws->per_lane_control |= 0x1;
            SOC_IF_ERROR_RETURN (wcmod_tier1_selector("SET_POLARITY", temp_ws, &rv));

            temp_ws->per_lane_control  = (pCfg->txpol>>(i * NUM_LANES)) <<2 ; 
            if ((pCfg->txpol>>(i * NUM_LANES))) temp_ws->per_lane_control |= 0x1;
            temp_ws->per_lane_control |=  0x2;
            SOC_IF_ERROR_RETURN (wcmod_tier1_selector("SET_POLARITY", temp_ws, &rv));
        }
    }     
    return rv;
}

STATIC int
_phy_wcmod_mld_lane_swap_config(int unit, soc_port_t port)
{
    int i, j, size, num_core;
    phy_ctrl_t *pc;
    wcmod_st   *ws, *ws_mld[2];
    WCMOD_DEV_DESC_t     *pDesc;
    WCMOD_DEV_CFG_t *pCfg;
    uint16 regData = 0;
    uint32 regAddr;

    ws_mld[0] = NULL;
    ws_mld[1] = NULL;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws =  (wcmod_st *)( pDesc + 1);
    pCfg = &pDesc->cfg;

    num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;

    SOC_IF_ERROR_RETURN(_wcmod_phy_parameter_copy(pc, ws));

    if (num_core > 3) {
        ws_mld[0] =  (wcmod_st *)( ws + num_core);
        ws_mld[1] =  (wcmod_st *)( ws + num_core + 1);
        size = 48;
    } else {
        ws_mld[0] =  (wcmod_st *)( ws + num_core);
        size = 24;
    }
    
    j = (size + 2) / 3;
    for (i= 0; i < j;  i++) {
        regData  = pCfg->mld_lane_swap[3 * i + 0] | 
               pCfg->mld_lane_swap[3 * i + 1] << 5 |
               pCfg->mld_lane_swap[3 * i + 2] << 10;
        if (i < 8) {
            regAddr = 0x84c6 + i;
            SOC_IF_ERROR_RETURN (WC40_REG_WRITE(unit, ws_mld[0], 0x0, regAddr, regData));
        } else {
            regAddr = 0x84c6 + i - 8;
            SOC_IF_ERROR_RETURN (WC40_REG_WRITE(unit, ws_mld[1], 0x0, regAddr, regData));
        }
    }
    return SOC_E_NONE;
}

STATIC int
phy_wcmod_interlaken_config(int unit, soc_port_t port, int speed)
{
    int i,num_core, myReturnValue, returnValue = 0, result;
    int tmp_lane_select = 0;
    phy_ctrl_t *pc;
    wcmod_st    *ws_warpcore[6], *ws_mld[2] ; 
    WCMOD_DEV_DESC_t     *pDesc;
    uint16 data16 = 0;
    WCMOD_TX_DRIVE_t tx_drv[MAX_NUM_LANES];
    int tx_inx=TXDRV_ILKN; 
    phyident_core_info_t core_info[8];  
    int array_max = 8;
    int array_size = 0;
    uint16 mld_bit_shift = 0;
    int skip_mld = 0; 
    

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws_warpcore[0]   =  (wcmod_st *)( pDesc + 1);

    /* get the ILKN num of lane info */
    SOC_IF_ERROR_RETURN
        (soc_phy_addr_multi_get(unit, port, array_max, &array_size, &core_info[0]));


    SOC_IF_ERROR_RETURN(_wcmod_phy_parameter_copy(pc, ws_warpcore[0]));

    /* return SOC_E_NONE;*/

    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws_warpcore[0]->this_lane != 0)) {
        num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
        skip_mld = 1;
    } else {
        num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
    }

    for (i = 0 ; i < num_core; i++) {
        ws_warpcore[i]   =  (wcmod_st *)( ws_warpcore[0] + i);
        ws_warpcore[i]->os_type   = WCMOD_OS5;
        ws_warpcore[i]->port_type = WCMOD_INDEPENDENT;
        ws_warpcore[i]->lane_select = WCMOD_LANE_1_1_1_1;

        SOC_IF_ERROR_RETURN (wcmod_tier1_selector(SET_PORT_MODE, ws_warpcore[i], &myReturnValue));
        returnValue |= myReturnValue;
        /*hold rx/tx asic reset */
        tmp_lane_select  = ws_warpcore[i]->lane_select;        /* save this */
        ws_warpcore[i]->lane_select = WCMOD_LANE_BCST; /* go to broadcast mode */

        result = MODIFY_WC40_DIGITAL5_MISC6r(unit,ws_warpcore[i],
                    DIGITAL5_MISC6_RESET_RX_ASIC_MASK | DIGITAL5_MISC6_RESET_TX_ASIC_MASK, 
                    DIGITAL5_MISC6_RESET_RX_ASIC_MASK | DIGITAL5_MISC6_RESET_TX_ASIC_MASK);

        ws_warpcore[i]->lane_select = tmp_lane_select;
        SOC_IF_ERROR_RETURN(result);

        /*disable the rx sequencer */
        ws_warpcore[i]->per_lane_control = WCMOD_DISABLE;
        SOC_IF_ERROR_RETURN
           (wcmod_tier1_selector("RX_SEQ_CONTROL", ws_warpcore[i], &myReturnValue));
        /* disable PLL */
        ws_warpcore[i]->per_lane_control = WCMOD_DISABLE;
        SOC_IF_ERROR_RETURN (wcmod_tier1_selector(PLL_SEQUENCER_CONTROL, ws_warpcore[i], &myReturnValue));
        returnValue |= myReturnValue;

        /* next disable 8b10b, 64b66b and prbs */
        SOC_IF_ERROR_RETURN (WRITE_WC40_XGXSBLK1_LANECTRL2r(unit, ws_warpcore[i], 0x0));
        SOC_IF_ERROR_RETURN (WRITE_WC40_XGXSBLK1_LANECTRL0r(unit, ws_warpcore[i], 0x0));
        SOC_IF_ERROR_RETURN (WRITE_WC40_XGXSBLK1_LANEPRBSr(unit, ws_warpcore[i], 0x0));
       
        /* set the pma_pmd forced speed */
        tmp_lane_select  = ws_warpcore[i]->lane_select;        /* save this */
        ws_warpcore[i]->lane_select = WCMOD_LANE_BCST; /* go to broadcast mode */
        switch (speed) {
        case 5750:
                data16 = /* 0x6707 */
                (   SERDESDIGITAL_MISC1_REFCLK_SEL_clk_125MHz 
                 << SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) | 
                (   SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div54 
                 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT) | 
                (   SERDESDIGITAL_MISC1_FORCE_SPEED_dr_5p75G
                 << SERDESDIGITAL_MISC1_FORCE_SPEED_SHIFT) ;
                SOC_IF_ERROR_RETURN (WRITE_WC40_SERDESDIGITAL_MISC1r(unit, ws_warpcore[i], data16));
            break;
        case 6250: 
                data16 = /* 0x6703 */
                (   SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz 
                 << SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) | 
                (   SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div54 
                 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT) | 
                (   SERDESDIGITAL_MISC1_FORCE_SPEED_dr_6p25G
                 << SERDESDIGITAL_MISC1_FORCE_SPEED_SHIFT) ;
                SOC_IF_ERROR_RETURN (WRITE_WC40_SERDESDIGITAL_MISC1r(unit, ws_warpcore[i], data16));
            break;
        case 10312:
                data16 = /* 0x6704 */
                (   SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz 
                 << SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) | 
                (   SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div54 
                 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT) | 
                (   SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10p3125G
                 << SERDESDIGITAL_MISC1_FORCE_SPEED_SHIFT) ;
                SOC_IF_ERROR_RETURN (WRITE_WC40_SERDESDIGITAL_MISC1r(unit, ws_warpcore[i], data16));
            break;
        case 10937:
                data16 = /* 0x6705 */
                (   SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz 
                 << SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) | 
                (   SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div54 
                 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT) | 
                (   SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10p9375G
                 << SERDESDIGITAL_MISC1_FORCE_SPEED_SHIFT) ;
                SOC_IF_ERROR_RETURN (WRITE_WC40_SERDESDIGITAL_MISC1r(unit, ws_warpcore[i], data16));
            break;
        case 11500:
                data16 = /* 0x6707 */
                (   SERDESDIGITAL_MISC1_REFCLK_SEL_clk_125MHz 
                 << SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) | 
                (   SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div54 
                 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT) | 
                (   SERDESDIGITAL_MISC1_FORCE_SPEED_dr_11p5G
                 << SERDESDIGITAL_MISC1_FORCE_SPEED_SHIFT) ;
                SOC_IF_ERROR_RETURN (WRITE_WC40_SERDESDIGITAL_MISC1r(unit, ws_warpcore[i], data16));
            break;
        case 12500:
                data16 = /* 0x6706 */
                (   SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz 
                 << SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) | 
                (   SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div54 
                 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT) | 
                (   SERDESDIGITAL_MISC1_FORCE_SPEED_dr_12p5G
                 << SERDESDIGITAL_MISC1_FORCE_SPEED_SHIFT) ;
                SOC_IF_ERROR_RETURN (WRITE_WC40_SERDESDIGITAL_MISC1r(unit, ws_warpcore[i], data16));

            break;
        case 11250:
                ws_warpcore[i]->oversample_mode = 0;
                ws_warpcore[i]->pll_divider = 72;
                ws_warpcore[i]->vco_freq = 11250;
                ws_warpcore[i]->spd_intf = WCMOD_SPD_CUSTOM_BYPASS; 
                SOC_IF_ERROR_RETURN
                   (wcmod_tier1_selector("SET_SPD_INTF", ws_warpcore[i], &myReturnValue));
            break;
        case 12375:
                if (ws_warpcore[i]->refclk == 125) {
                    ws_warpcore[i]->oversample_mode = 0;
                    ws_warpcore[i]->pll_divider = 100;
                    ws_warpcore[i]->vco_freq = 12375;
                    ws_warpcore[i]->spd_intf = WCMOD_SPD_CUSTOM_BYPASS; 
                    SOC_IF_ERROR_RETURN
                       (wcmod_tier1_selector("SET_SPD_INTF", ws_warpcore[i], &myReturnValue));
                } else {
                    data16 = /* 0x6706 */
                    (   SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz 
                     << SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) | 
                    (   SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div54 
                     << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT) | 
                    (   SERDESDIGITAL_MISC1_FORCE_SPEED_dr_12p5G
                     << SERDESDIGITAL_MISC1_FORCE_SPEED_SHIFT) ;
                    SOC_IF_ERROR_RETURN (WRITE_WC40_SERDESDIGITAL_MISC1r(unit, ws_warpcore[i], data16));
                }
            break;
        default:
            break;
        }

        data16 = /* 0x6190 */
            SERDESDIGITAL_MISC2_RXCK_MII_GEN_SEL_FORCE_MASK | 
            SERDESDIGITAL_MISC2_RXCK_MII_GEN_SEL_VAL_MASK |
            SERDESDIGITAL_MISC2_MIIGMIIDLY_EN_MASK |
            SERDESDIGITAL_MISC2_MIIGMIIMUX_EN_MASK |
            SERDESDIGITAL_MISC2_FIFO_ERR_CYA_MASK ;
        SOC_IF_ERROR_RETURN (WRITE_WC40_SERDESDIGITAL_MISC2r(unit, ws_warpcore[i], data16));
        data16 = /* 0x181 */
            SERDESDIGITAL_CONTROL1000X1_COMMA_DET_EN_MASK |
            SERDESDIGITAL_CONTROL1000X1_CRC_CHECKER_DISABLE_MASK |
            SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK ;
        SOC_IF_ERROR_RETURN (WRITE_WC40_SERDESDIGITAL_CONTROL1000X1r(unit, ws_warpcore[i], data16));
        data16 = /* 0x840 */
            DIGITAL4_MISC5_TGEN_LINK_MASK |
            DIGITAL4_MISC5_FORCE_SPEED_B6_MASK ;
        SOC_IF_ERROR_RETURN (WRITE_WC40_DIGITAL4_MISC5r(unit, ws_warpcore[i], data16));

        ws_warpcore[i]->lane_select = tmp_lane_select;

        /* same div20 is selected for all lanes, 
         this should be 0xc01 for quad mode for individual div20 for all lanes  */
        tmp_lane_select  = ws_warpcore[i]->lane_select;        /* save this */
        ws_warpcore[i]->lane_select = WCMOD_LANE_BCST; /* go to broadcast mode */
        data16 = /* 0xc01 */
            (   CL72_USERB0_CL72_MISC4_CONTROL_RX_WCLK_20_QUAD_MODE 
             << CL72_USERB0_CL72_MISC4_CONTROL_RX_WCLK_20_MODE_SEL_SHIFT ) |   
                CL72_USERB0_CL72_MISC4_CONTROL_CL72_EN_MASK_MASK ;
        SOC_IF_ERROR_RETURN (WRITE_WC40_CL72_USERB0_CL72_MISC4_CONTROLr(unit, ws_warpcore[i], data16));
        data16 = /* 0x401b */
            XGXSBLK6_XGXSX2CONTROL2_100G_BIT_MASK |
            XGXSBLK6_XGXSX2CONTROL2_XGXSX2_BA_DATAOUT_CYA_MASK |
            XGXSBLK6_XGXSX2CONTROL2_XGXSX2_BA_SYNC_STATUS_CYA_MASK |
            XGXSBLK6_XGXSX2CONTROL2_XGXSX2_BA_LINK_EN_MASK |
            XGXSBLK6_XGXSX2CONTROL2_XGXSX2_BA_HYST_EN_MASK ;
        SOC_IF_ERROR_RETURN (WRITE_WC40_XGXSBLK6_XGXSX2CONTROL2r(unit, ws_warpcore[i], data16));
        data16 = /* 0x2002 */
            SERDESDIGITAL_CONTROL1000X3_DISABLE_TX_CRS_MASK |
            (    SERDESDIGITAL_CONTROL1000X3_FIFO_ELASICITY_TX_10KB 
              << SERDESDIGITAL_CONTROL1000X3_FIFO_ELASICITY_TX_SHIFT ) ;
        SOC_IF_ERROR_RETURN (WRITE_WC40_SERDESDIGITAL_CONTROL1000X3r(unit, ws_warpcore[i], data16));

        /* disable AN on all lanes */
        ws_warpcore[i]->per_lane_control = 0;
        SOC_IF_ERROR_RETURN (wcmod_tier1_selector(AUTONEG_CONTROL, ws_warpcore[i], &myReturnValue));
        ws_warpcore[i]->lane_select = tmp_lane_select;
    }

    /* configure the TX driver parameters per speed mode */        
    SOC_IF_ERROR_RETURN
        (_phy_wcmod_tx_control_get(unit, pc, &tx_drv[0],tx_inx));
    SOC_IF_ERROR_RETURN
        (_phy_wcmod_tx_control_set(unit, pc,&tx_drv[0])); 

    for (i = 0 ; i < num_core; i++) {
         ws_warpcore[i]   =  (wcmod_st *)( ws_warpcore[0] + i);
        /* enable PLL sequencer */
        ws_warpcore[i]->per_lane_control = WCMOD_ENABLE;
        SOC_IF_ERROR_RETURN (wcmod_tier1_selector(PLL_SEQUENCER_CONTROL, ws_warpcore[i], &myReturnValue));
        /* enable  RX sequencer    */
        tmp_lane_select = ws_warpcore[i]->lane_select;
        ws_warpcore[i]->lane_select = WCMOD_LANE_1_1_1_1;
        SOC_IF_ERROR_RETURN
           (wcmod_tier1_selector("RX_SEQ_CONTROL", ws_warpcore[i], &myReturnValue));
        ws_warpcore[i]->lane_select = tmp_lane_select;
    }

    /* setup  MLD */
    if (!skip_mld) {
        if (num_core > 3) {
            ws_mld[0] =  (wcmod_st *)( ws_warpcore[0] + num_core);
           /* temp_id[num_core] =  ws_mld[0]->phy_ad;
            SOC_IF_ERROR_RETURN(_wcmod_phy_parameter_copy(pc, ws_mld[0]));
            ws_mld[0]->phy_ad = temp_id[num_core];  */

            ws_mld[1] =  (wcmod_st *)( ws_warpcore[0] + num_core + 1);
            /*temp_id[num_core + 1] =  ws_mld[1]->phy_ad;
            SOC_IF_ERROR_RETURN(_wcmod_phy_parameter_copy(pc, ws_mld[1]));
            ws_mld[1]->phy_ad = temp_id[num_core + 1]; */
            SOC_IF_ERROR_RETURN (WC40_REG_WRITE(unit, ws_mld[0], 0x0, 0x8011, 0x670));
            for (i = 3; i < num_core; i++) {
                mld_bit_shift |=  1 << ( 4 + core_info[i].index_in_mld);
            }
            data16 = 0x600 | mld_bit_shift; 
            SOC_IF_ERROR_RETURN (WC40_REG_WRITE(unit, ws_mld[1], 0x0, 0x8011, data16));
        } else {
            ws_mld[0] =  (wcmod_st *)( ws_warpcore[0] + num_core);
           /* temp_id[num_core] =  ws_mld[0]->phy_ad;
            SOC_IF_ERROR_RETURN(_wcmod_phy_parameter_copy(pc, ws_mld[0]));
            ws_mld[0]->phy_ad = temp_id[num_core];*/
            for (i = 0; i < num_core; i++) {
                mld_bit_shift |=  1 << ( 4 + core_info[i].index_in_mld);
            }
            /*dont override old value*/
            SOC_IF_ERROR_RETURN (WC40_REG_READ(unit, ws_mld[0], 0x0, 0x8011, &data16));
            data16 |= (0x600 | mld_bit_shift); 
            SOC_IF_ERROR_RETURN (WC40_REG_WRITE(unit, ws_mld[0], 0x0, 0x8011, data16));
        }
    }
    /*release the rx/tx asic reset on the all the related WarpCore */
    for (i = 0; i < num_core; i++) {
        tmp_lane_select = ws_warpcore[i]->lane_select;
        ws_warpcore[i]->lane_select = WCMOD_LANE_BCST;

        SOC_IF_ERROR_RETURN(MODIFY_WC40_DIGITAL5_MISC6r(unit, ws_warpcore[i], 0,
                  DIGITAL5_MISC6_RESET_RX_ASIC_MASK | DIGITAL5_MISC6_RESET_TX_ASIC_MASK));

        ws_warpcore[i]->lane_select = tmp_lane_select;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_wcmod_100G_config
 * Purpose:
 *      Indicate the configured medium
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      medium - (OUT) One of:
 *              SOC_PORT_MEDIUM_COPPER
 *              SOC_PORT_MEDIUM_FIBER
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
phy_wcmod_100g_plus_config(int unit, soc_port_t port, int speed)
{
    uint16 data16 ;
    int myReturnValue, tmp_lane_select, returnValue = 0;
    phy_ctrl_t *pc;
    wcmod_st    *ws_master, *ws_slave0, *ws_slave1, *ws_mld;
    WCMOD_DEV_DESC_t     *pDesc;
    WCMOD_TX_DRIVE_t tx_drv[MAX_NUM_LANES]; 
    WCMOD_DEV_CFG_t *pCfg;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws_master   =  (wcmod_st *)( pDesc + 1);
    pCfg = &pDesc->cfg; 

    SOC_IF_ERROR_RETURN(_wcmod_phy_parameter_copy(pc, ws_master));

    ws_slave0   =  (wcmod_st *)( ws_master + 1);
    ws_slave1   =  (wcmod_st *)( ws_master + 2);
    ws_mld      =  (wcmod_st *)( ws_master + 3);
    ws_master->s100g_plus = 1;

    /* next config 100G master */
    ws_master->os_type   = WCMOD_OS5;
    ws_master->port_type = WCMOD_INDEPENDENT;
    ws_master->lane_swap = 0x32103210;
    ws_master->lane_select = WCMOD_LANE_1_1_1_1;

    SOC_IF_ERROR_RETURN (wcmod_tier1_selector(SET_PORT_MODE, ws_master, &myReturnValue));
    /*returnValue |= myReturnValue;*/
    /*hold rx/tx asic reset */
    tmp_lane_select  = ws_master->lane_select;        /* save this */
    ws_master->lane_select = WCMOD_LANE_BCST; /* go to broadcast mode */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DIGITAL5_MISC6r(unit,ws_master,
              DIGITAL5_MISC6_RESET_RX_ASIC_MASK | DIGITAL5_MISC6_RESET_TX_ASIC_MASK, 
              DIGITAL5_MISC6_RESET_RX_ASIC_MASK | DIGITAL5_MISC6_RESET_TX_ASIC_MASK));
    ws_master->lane_select = tmp_lane_select;

    /* disable PLL */
    ws_master->per_lane_control = WCMOD_DISABLE;
    SOC_IF_ERROR_RETURN (wcmod_tier1_selector(PLL_SEQUENCER_CONTROL, ws_master, &myReturnValue));
    /*returnValue |= myReturnValue;*/

    /* next disable 8b10b, 64b66b and prbs */
    SOC_IF_ERROR_RETURN (WRITE_WC40_XGXSBLK1_LANECTRL2r(unit, ws_master, 0x0));
    SOC_IF_ERROR_RETURN (WRITE_WC40_XGXSBLK1_LANECTRL0r(unit, ws_master, 0x0));
    SOC_IF_ERROR_RETURN (WRITE_WC40_XGXSBLK1_LANEPRBSr(unit, ws_master, 0x0));
   
    /* set the pma_pmd forced speed */
    tmp_lane_select  = ws_master->lane_select;        /* save this */
    ws_master->lane_select = WCMOD_LANE_BCST; /* go to broadcast mode */
    if ((speed == 127000)  || (speed == 106000)){
        data16 = /* 0x7c04 */
            (   SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz 
                << SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) | 
            ( SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK ) |
            (   SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div70 
                << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT) | 
            (    SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10p3125G
                << SERDESDIGITAL_MISC1_FORCE_SPEED_SHIFT) ;
        SOC_IF_ERROR_RETURN (WRITE_WC40_SERDESDIGITAL_MISC1r(unit, ws_master, data16));
    } else {
        data16 = /* 0x6704 */
            (   SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz 
             << SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) | 
            (   SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div54 
             << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT) | 
            (   SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10p3125G
             << SERDESDIGITAL_MISC1_FORCE_SPEED_SHIFT) ;
        SOC_IF_ERROR_RETURN (WRITE_WC40_SERDESDIGITAL_MISC1r(unit, ws_master, data16));
    }
    data16 = /* 0x6190 */
        SERDESDIGITAL_MISC2_RXCK_MII_GEN_SEL_FORCE_MASK | 
        SERDESDIGITAL_MISC2_RXCK_MII_GEN_SEL_VAL_MASK |
        SERDESDIGITAL_MISC2_MIIGMIIDLY_EN_MASK |
        SERDESDIGITAL_MISC2_MIIGMIIMUX_EN_MASK |
        SERDESDIGITAL_MISC2_FIFO_ERR_CYA_MASK ;
    SOC_IF_ERROR_RETURN (WRITE_WC40_SERDESDIGITAL_MISC2r(unit, ws_master, data16));
    data16 = /* 0x181 */
        SERDESDIGITAL_CONTROL1000X1_COMMA_DET_EN_MASK |
        SERDESDIGITAL_CONTROL1000X1_CRC_CHECKER_DISABLE_MASK |
        SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK ;
    SOC_IF_ERROR_RETURN (WRITE_WC40_SERDESDIGITAL_CONTROL1000X1r(unit, ws_master, data16));
    data16 = /* 0x840 */
        DIGITAL4_MISC5_TGEN_LINK_MASK |
        DIGITAL4_MISC5_FORCE_SPEED_B6_MASK ;
    SOC_IF_ERROR_RETURN (WRITE_WC40_DIGITAL4_MISC5r(unit, ws_master, data16));
    ws_master->lane_select = tmp_lane_select;

    /* gearbox enable,Gearbox bypass value is 0xffe4 */
    data16 = /* 0xffe4 */
        XGXSBLK8_TXLNSWAP1_FORCE_GB_BYPASS_VAL_MASK |
        XGXSBLK8_TXLNSWAP1_FORCE_GB_BYPASS_EN_MASK;
    SOC_IF_ERROR_RETURN (MODIFY_WC40_XGXSBLK8_TXLNSWAP1r(unit, ws_master, data16 , data16));
  

    /* same div20 is selected for all lanes, 
     this should be 0xc01 for quad mode for individual div20 for all lanes  */
    tmp_lane_select  = ws_master->lane_select;        /* save this */
    ws_master->lane_select = WCMOD_LANE_BCST; /* go to broadcast mode */
    data16 = /* 0xc01 */
        (   CL72_USERB0_CL72_MISC4_CONTROL_RX_WCLK_20_QUAD_MODE 
         << CL72_USERB0_CL72_MISC4_CONTROL_RX_WCLK_20_MODE_SEL_SHIFT ) |   
            CL72_USERB0_CL72_MISC4_CONTROL_CL72_EN_MASK_MASK ;
    SOC_IF_ERROR_RETURN (WRITE_WC40_CL72_USERB0_CL72_MISC4_CONTROLr(unit, ws_master, data16));
    data16 = /* 0x401b */
        XGXSBLK6_XGXSX2CONTROL2_100G_BIT_MASK |
        XGXSBLK6_XGXSX2CONTROL2_XGXSX2_BA_DATAOUT_CYA_MASK |
        XGXSBLK6_XGXSX2CONTROL2_XGXSX2_BA_SYNC_STATUS_CYA_MASK |
        XGXSBLK6_XGXSX2CONTROL2_XGXSX2_BA_LINK_EN_MASK |
        XGXSBLK6_XGXSX2CONTROL2_XGXSX2_BA_HYST_EN_MASK ;
    SOC_IF_ERROR_RETURN (WRITE_WC40_XGXSBLK6_XGXSX2CONTROL2r(unit, ws_master, data16));
    data16 = /* 0x2002 */
        SERDESDIGITAL_CONTROL1000X3_DISABLE_TX_CRS_MASK |
        (    SERDESDIGITAL_CONTROL1000X3_FIFO_ELASICITY_TX_10KB 
          << SERDESDIGITAL_CONTROL1000X3_FIFO_ELASICITY_TX_SHIFT ) ;
    SOC_IF_ERROR_RETURN (WRITE_WC40_SERDESDIGITAL_CONTROL1000X3r(unit, ws_master, data16));


    /* disable AN on all lanes */
    ws_master->per_lane_control = 0;
    SOC_IF_ERROR_RETURN (wcmod_tier1_selector(AUTONEG_CONTROL, ws_master, &myReturnValue));
    ws_master->lane_select = tmp_lane_select;

    /*next check if we need to enable rx_los external pin */
    if (pCfg->rx_los_ext_enable == WCMOD_RX_LOS_EXT_PIN_ENABLE) {
        SOC_IF_ERROR_RETURN(MODIFY_WC40_RX0_ANARXCONTROLr(unit, ws_master, 0x0080, 0x0080)); 
        SOC_IF_ERROR_RETURN(MODIFY_WC40_RX1_ANARXCONTROLr(unit, ws_master, 0x0080, 0x0080)); 
        SOC_IF_ERROR_RETURN(MODIFY_WC40_RX2_ANARXCONTROLr(unit, ws_master, 0x0080, 0x0080)); 
        SOC_IF_ERROR_RETURN(MODIFY_WC40_RX3_ANARXCONTROLr(unit, ws_master, 0x0080, 0x0080));
        SOC_IF_ERROR_RETURN(MODIFY_WC40_RX0_ANARXSIGDETr(unit, ws_master, 0x40, 0x40));  
        SOC_IF_ERROR_RETURN(MODIFY_WC40_RX1_ANARXSIGDETr(unit, ws_master, 0x40, 0x40));  
        SOC_IF_ERROR_RETURN(MODIFY_WC40_RX2_ANARXSIGDETr(unit, ws_master, 0x40, 0x40));  
        SOC_IF_ERROR_RETURN(MODIFY_WC40_RX3_ANARXSIGDETr(unit, ws_master, 0x40, 0x40));
    } 
          
    /* enable PLL sequencer */
    ws_master->per_lane_control = WCMOD_ENABLE;
    SOC_IF_ERROR_RETURN (wcmod_tier1_selector(PLL_SEQUENCER_CONTROL, ws_master, &myReturnValue));

    /* next config 100G slave_0 setting */
    ws_slave0->os_type    = WCMOD_OS5;
    ws_slave0->port_type  = WCMOD_INDEPENDENT;
    ws_slave0->lane_swap  = 0x32103210;
    ws_slave0->lane_select = WCMOD_LANE_1_1_1_1;

    SOC_IF_ERROR_RETURN (wcmod_tier1_selector(SET_PORT_MODE, ws_slave0, &myReturnValue));
    /*hold rx/tx asic reset */
    tmp_lane_select  = ws_slave0->lane_select;        /* save this */
    ws_slave0->lane_select = WCMOD_LANE_BCST; /* go to broadcast mode */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DIGITAL5_MISC6r(unit,ws_slave0,
              DIGITAL5_MISC6_RESET_RX_ASIC_MASK | DIGITAL5_MISC6_RESET_TX_ASIC_MASK, 
              DIGITAL5_MISC6_RESET_RX_ASIC_MASK | DIGITAL5_MISC6_RESET_TX_ASIC_MASK));
    ws_slave0->lane_select = tmp_lane_select;

    /* disable PLL */
    ws_slave0->per_lane_control = WCMOD_DISABLE;
    SOC_IF_ERROR_RETURN (wcmod_tier1_selector(PLL_SEQUENCER_CONTROL, ws_slave0, &myReturnValue));
    /* next disable 8b10b, 64b66b and prbs */
    SOC_IF_ERROR_RETURN (WRITE_WC40_XGXSBLK1_LANECTRL2r(unit, ws_slave0, 0x0));
    SOC_IF_ERROR_RETURN (WRITE_WC40_XGXSBLK1_LANECTRL0r(unit, ws_slave0, 0x0));
    SOC_IF_ERROR_RETURN (WRITE_WC40_XGXSBLK1_LANEPRBSr(unit, ws_slave0, 0x0));

    /* set the pma_pmd forced speed */
    tmp_lane_select  = ws_slave0->lane_select;        /* save this */
    ws_slave0->lane_select = WCMOD_LANE_BCST; /* go to broadcast mode */
    if ((speed == 127000) || (speed == 106000)){
        data16 = /* 0x7c04 */
            (   SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz 
                << SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) | 
            ( SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK ) |
            (   SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div70 
                << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT) | 
            (   SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10p3125G 
                << SERDESDIGITAL_MISC1_FORCE_SPEED_SHIFT) ;
        SOC_IF_ERROR_RETURN (WRITE_WC40_SERDESDIGITAL_MISC1r(unit, ws_slave0, data16));
    } else {
        data16 = /* 0x6704 */
            (   SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz 
             << SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) | 
            (   SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div54 
             << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT) | 
            (    SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10p3125G
             << SERDESDIGITAL_MISC1_FORCE_SPEED_SHIFT) ;
        SOC_IF_ERROR_RETURN (WRITE_WC40_SERDESDIGITAL_MISC1r(unit, ws_slave0, data16));
    }
    data16 = /* 0x6190 */
        SERDESDIGITAL_MISC2_RXCK_MII_GEN_SEL_FORCE_MASK | 
        SERDESDIGITAL_MISC2_RXCK_MII_GEN_SEL_VAL_MASK |
        SERDESDIGITAL_MISC2_MIIGMIIDLY_EN_MASK |
        SERDESDIGITAL_MISC2_MIIGMIIMUX_EN_MASK |
        SERDESDIGITAL_MISC2_FIFO_ERR_CYA_MASK ;
    SOC_IF_ERROR_RETURN (WRITE_WC40_SERDESDIGITAL_MISC2r(unit, ws_slave0, data16));
    data16 = /* 0x181 */
        SERDESDIGITAL_CONTROL1000X1_COMMA_DET_EN_MASK |
        SERDESDIGITAL_CONTROL1000X1_CRC_CHECKER_DISABLE_MASK |
        SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK ;
    SOC_IF_ERROR_RETURN (WRITE_WC40_SERDESDIGITAL_CONTROL1000X1r(unit, ws_slave0, data16));
    data16 = /* 0x840 */
        DIGITAL4_MISC5_TGEN_LINK_MASK |
        DIGITAL4_MISC5_FORCE_SPEED_B6_MASK ;
    SOC_IF_ERROR_RETURN (WRITE_WC40_DIGITAL4_MISC5r(unit, ws_slave0, data16));
    ws_slave0->lane_select = tmp_lane_select;

    /* gearbox enable,Gearbox bypass value is 0xffe4 */
    data16 = /* 0xffe4 */
        XGXSBLK8_TXLNSWAP1_FORCE_GB_BYPASS_VAL_MASK |
        XGXSBLK8_TXLNSWAP1_FORCE_GB_BYPASS_EN_MASK;
    SOC_IF_ERROR_RETURN (MODIFY_WC40_XGXSBLK8_TXLNSWAP1r(unit, ws_slave0, data16 , data16));

    /* same div20 is selected for all lanes, 
     this should be 0xc01 for quad mode for individual div20 for all lanes  */
    tmp_lane_select  = ws_slave0->lane_select;        /* save this */
    ws_slave0->lane_select = WCMOD_LANE_BCST; /* go to broadcast mode */
    data16 = /* 0xc01 */
        (   CL72_USERB0_CL72_MISC4_CONTROL_RX_WCLK_20_QUAD_MODE 
         << CL72_USERB0_CL72_MISC4_CONTROL_RX_WCLK_20_MODE_SEL_SHIFT ) |   
            CL72_USERB0_CL72_MISC4_CONTROL_CL72_EN_MASK_MASK ;
    SOC_IF_ERROR_RETURN (WRITE_WC40_CL72_USERB0_CL72_MISC4_CONTROLr(unit, ws_slave0, data16));
    data16 = /* 0x401b */
        XGXSBLK6_XGXSX2CONTROL2_100G_BIT_MASK |
        XGXSBLK6_XGXSX2CONTROL2_XGXSX2_BA_DATAOUT_CYA_MASK |
        XGXSBLK6_XGXSX2CONTROL2_XGXSX2_BA_SYNC_STATUS_CYA_MASK |
        XGXSBLK6_XGXSX2CONTROL2_XGXSX2_BA_LINK_EN_MASK |
        XGXSBLK6_XGXSX2CONTROL2_XGXSX2_BA_HYST_EN_MASK ;
    SOC_IF_ERROR_RETURN (WRITE_WC40_XGXSBLK6_XGXSX2CONTROL2r(unit, ws_slave0, data16));
    data16 = /* 0x2002 */
        SERDESDIGITAL_CONTROL1000X3_DISABLE_TX_CRS_MASK |
        (    SERDESDIGITAL_CONTROL1000X3_FIFO_ELASICITY_TX_10KB 
          << SERDESDIGITAL_CONTROL1000X3_FIFO_ELASICITY_TX_SHIFT ) ;
    SOC_IF_ERROR_RETURN (WRITE_WC40_SERDESDIGITAL_CONTROL1000X3r(unit, ws_slave0, data16));

    /* disable AN on all lanes */
    ws_slave0->per_lane_control = 0;
    SOC_IF_ERROR_RETURN (wcmod_tier1_selector(AUTONEG_CONTROL, ws_slave0, &myReturnValue));
    ws_slave0->lane_select = tmp_lane_select;

    /*next check if we need to enable rx_los external pin */
    if (pCfg->rx_los_ext_enable == WCMOD_RX_LOS_EXT_PIN_ENABLE) {
        SOC_IF_ERROR_RETURN(MODIFY_WC40_RX0_ANARXCONTROLr(unit, ws_slave0, 0x0080, 0x0080)); 
        SOC_IF_ERROR_RETURN(MODIFY_WC40_RX1_ANARXCONTROLr(unit, ws_slave0, 0x0080, 0x0080)); 
        SOC_IF_ERROR_RETURN(MODIFY_WC40_RX2_ANARXCONTROLr(unit, ws_slave0, 0x0080, 0x0080)); 
        SOC_IF_ERROR_RETURN(MODIFY_WC40_RX3_ANARXCONTROLr(unit, ws_slave0, 0x0080, 0x0080));
        SOC_IF_ERROR_RETURN(MODIFY_WC40_RX0_ANARXSIGDETr(unit, ws_slave0, 0x40, 0x40));  
        SOC_IF_ERROR_RETURN(MODIFY_WC40_RX1_ANARXSIGDETr(unit, ws_slave0, 0x40, 0x40));  
        SOC_IF_ERROR_RETURN(MODIFY_WC40_RX2_ANARXSIGDETr(unit, ws_slave0, 0x40, 0x40));  
        SOC_IF_ERROR_RETURN(MODIFY_WC40_RX3_ANARXSIGDETr(unit, ws_slave0, 0x40, 0x40));
    } 
          
    /* enable PLL sequencer */
    ws_slave0->per_lane_control = WCMOD_ENABLE;
    SOC_IF_ERROR_RETURN (wcmod_tier1_selector(PLL_SEQUENCER_CONTROL, ws_slave0, &myReturnValue));

    /* next config 100G slave_1 setting */

    ws_slave1->os_type    = WCMOD_OS5;
    ws_slave1->port_type  = WCMOD_INDEPENDENT;
    ws_slave1->lane_swap  = 0x32103210;
    ws_slave1->lane_select = WCMOD_LANE_1_1_1_1;

    SOC_IF_ERROR_RETURN (wcmod_tier1_selector(SET_PORT_MODE, ws_slave1, &myReturnValue));
    /*hold rx/tx asic reset */
    tmp_lane_select  = ws_slave1->lane_select;        /* save this */
    ws_slave1->lane_select = WCMOD_LANE_BCST; /* go to broadcast mode */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DIGITAL5_MISC6r(unit,ws_slave1,
              DIGITAL5_MISC6_RESET_RX_ASIC_MASK | DIGITAL5_MISC6_RESET_TX_ASIC_MASK, 
              DIGITAL5_MISC6_RESET_RX_ASIC_MASK | DIGITAL5_MISC6_RESET_TX_ASIC_MASK));
    ws_slave1->lane_select = tmp_lane_select;

    /* disable PLL */
    ws_slave1->per_lane_control = WCMOD_DISABLE;
    SOC_IF_ERROR_RETURN (wcmod_tier1_selector(PLL_SEQUENCER_CONTROL, ws_slave1, &myReturnValue));
    /* next disable 8b10b, 64b66b and prbs */
    SOC_IF_ERROR_RETURN (WRITE_WC40_XGXSBLK1_LANECTRL2r(unit, ws_slave1, 0x0));
    SOC_IF_ERROR_RETURN (WRITE_WC40_XGXSBLK1_LANECTRL0r(unit, ws_slave1, 0x0));
    SOC_IF_ERROR_RETURN (WRITE_WC40_XGXSBLK1_LANEPRBSr(unit, ws_slave1, 0x0));
  
    /* set the pma_pmd forced speed */
    tmp_lane_select  = ws_slave1->lane_select;        /* save this */
    ws_slave1->lane_select = WCMOD_LANE_BCST; /* go to broadcast mode */
    if ((speed == 127000) || (speed == 106000)){
        data16 = /* 0x7c04 */
            (   SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz 
             << SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) | 
            (   SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK ) |
            (   SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div70 
             << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT) | 
            (   SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10p3125G 
             << SERDESDIGITAL_MISC1_FORCE_SPEED_SHIFT) ;
        SOC_IF_ERROR_RETURN (WRITE_WC40_SERDESDIGITAL_MISC1r(unit, ws_slave1, data16));
    } else {
        data16 = /* 0x6704 */
            (   SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz 
             << SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) | 
            (   SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div54 
             << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT) | 
            (   SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10p3125G
             << SERDESDIGITAL_MISC1_FORCE_SPEED_SHIFT) ;
        SOC_IF_ERROR_RETURN (WRITE_WC40_SERDESDIGITAL_MISC1r(unit, ws_slave1, data16));
    }
    data16 = /* 0x6190 */
        SERDESDIGITAL_MISC2_RXCK_MII_GEN_SEL_FORCE_MASK | 
        SERDESDIGITAL_MISC2_RXCK_MII_GEN_SEL_VAL_MASK |
        SERDESDIGITAL_MISC2_MIIGMIIDLY_EN_MASK |
        SERDESDIGITAL_MISC2_MIIGMIIMUX_EN_MASK |
        SERDESDIGITAL_MISC2_FIFO_ERR_CYA_MASK ;
    SOC_IF_ERROR_RETURN (WRITE_WC40_SERDESDIGITAL_MISC2r(unit, ws_slave1, data16));
    data16 = /* 0x181 */
        SERDESDIGITAL_CONTROL1000X1_COMMA_DET_EN_MASK |
        SERDESDIGITAL_CONTROL1000X1_CRC_CHECKER_DISABLE_MASK |
        SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK ;
    SOC_IF_ERROR_RETURN (WRITE_WC40_SERDESDIGITAL_CONTROL1000X1r(unit, ws_slave1, data16));
    data16 = /* 0x840 */
        DIGITAL4_MISC5_TGEN_LINK_MASK |
        DIGITAL4_MISC5_FORCE_SPEED_B6_MASK ;
    SOC_IF_ERROR_RETURN (WRITE_WC40_DIGITAL4_MISC5r(unit, ws_slave1, data16));
    ws_slave1->lane_select = tmp_lane_select;

    /* gearbox enable,Gearbox bypass value is 0xffe4 */
    /* gearbox enable,Gearbox bypass value is 0xffe4 */
    data16 = /* 0xffe4 */
        XGXSBLK8_TXLNSWAP1_FORCE_GB_BYPASS_VAL_MASK |
        XGXSBLK8_TXLNSWAP1_FORCE_GB_BYPASS_EN_MASK;
    SOC_IF_ERROR_RETURN (MODIFY_WC40_XGXSBLK8_TXLNSWAP1r(unit, ws_slave1, data16 , data16));


    /* same div20 is selected for all lanes, 
     this should be 0xc01 for quad mode for individual div20 for all lanes  */
    tmp_lane_select  = ws_slave1->lane_select;        /* save this */
    ws_slave1->lane_select = WCMOD_LANE_BCST; /* go to broadcast mode */
    data16 = /* 0xc01 */
        (   CL72_USERB0_CL72_MISC4_CONTROL_RX_WCLK_20_QUAD_MODE 
         << CL72_USERB0_CL72_MISC4_CONTROL_RX_WCLK_20_MODE_SEL_SHIFT ) |   
            CL72_USERB0_CL72_MISC4_CONTROL_CL72_EN_MASK_MASK ;
    SOC_IF_ERROR_RETURN (WRITE_WC40_CL72_USERB0_CL72_MISC4_CONTROLr(unit, ws_slave1, data16));
    data16 = /* 0x401b */
        XGXSBLK6_XGXSX2CONTROL2_100G_BIT_MASK |
        XGXSBLK6_XGXSX2CONTROL2_XGXSX2_BA_DATAOUT_CYA_MASK |
        XGXSBLK6_XGXSX2CONTROL2_XGXSX2_BA_SYNC_STATUS_CYA_MASK |
        XGXSBLK6_XGXSX2CONTROL2_XGXSX2_BA_LINK_EN_MASK |
        XGXSBLK6_XGXSX2CONTROL2_XGXSX2_BA_HYST_EN_MASK ;
    SOC_IF_ERROR_RETURN (WRITE_WC40_XGXSBLK6_XGXSX2CONTROL2r(unit, ws_slave1, data16));
    data16 = /* 0x2002 */
        SERDESDIGITAL_CONTROL1000X3_DISABLE_TX_CRS_MASK |
        (    SERDESDIGITAL_CONTROL1000X3_FIFO_ELASICITY_TX_10KB 
          << SERDESDIGITAL_CONTROL1000X3_FIFO_ELASICITY_TX_SHIFT ) ;
    SOC_IF_ERROR_RETURN (WRITE_WC40_SERDESDIGITAL_CONTROL1000X3r(unit, ws_slave1, data16));

    /* disable AN on all lanes */
    ws_slave1->per_lane_control = 0;
    SOC_IF_ERROR_RETURN (wcmod_tier1_selector(AUTONEG_CONTROL, ws_slave1, &myReturnValue));

    ws_slave1->lane_select = tmp_lane_select;

    /*next check if we need to enable rx_los external pin */
    if (pCfg->rx_los_ext_enable == WCMOD_RX_LOS_EXT_PIN_ENABLE) {
        SOC_IF_ERROR_RETURN(MODIFY_WC40_RX0_ANARXCONTROLr(unit, ws_slave1, 0x0080, 0x0080)); 
        SOC_IF_ERROR_RETURN(MODIFY_WC40_RX1_ANARXCONTROLr(unit, ws_slave1, 0x0080, 0x0080)); 
        SOC_IF_ERROR_RETURN(MODIFY_WC40_RX2_ANARXCONTROLr(unit, ws_slave1, 0x0080, 0x0080)); 
        SOC_IF_ERROR_RETURN(MODIFY_WC40_RX3_ANARXCONTROLr(unit, ws_slave1, 0x0080, 0x0080));
        SOC_IF_ERROR_RETURN(MODIFY_WC40_RX0_ANARXSIGDETr(unit, ws_slave1, 0x40, 0x40));  
        SOC_IF_ERROR_RETURN(MODIFY_WC40_RX1_ANARXSIGDETr(unit, ws_slave1, 0x40, 0x40));  
        SOC_IF_ERROR_RETURN(MODIFY_WC40_RX2_ANARXSIGDETr(unit, ws_slave1, 0x40, 0x40));  
        SOC_IF_ERROR_RETURN(MODIFY_WC40_RX3_ANARXSIGDETr(unit, ws_slave1, 0x40, 0x40));
    }
           
    /* enable PLL sequencer */
    ws_slave1->per_lane_control = WCMOD_ENABLE;
    SOC_IF_ERROR_RETURN (wcmod_tier1_selector(PLL_SEQUENCER_CONTROL, ws_slave1, &myReturnValue));

    /*next conifg the tx parameter */
    SOC_IF_ERROR_RETURN
        (_phy_wcmod_tx_control_get(unit, pc, &tx_drv[0], TXDRV_ILKN));
    SOC_IF_ERROR_RETURN
        (_phy_wcmod_tx_control_set(unit, pc, &tx_drv[0]));  

    /* sleep */
    sal_usleep(150000) ; /* wait for 150ms */

    /* next program the MLD register */
    if ((speed == 106000) || (speed == 100000)) {
        if (speed == 100000) {
            SOC_IF_ERROR_RETURN (WC40_REG_WRITE(unit, ws_mld, 0x0, 0x8420, 0x6081));
        } else {
            SOC_IF_ERROR_RETURN (WC40_REG_WRITE(unit, ws_mld, 0x0, 0x8420, 0x6083));
        }
        SOC_IF_ERROR_RETURN (WC40_REG_WRITE(unit, ws_mld, 0x0, 0x8427, 0x7a12));
    } else {
        if (speed == 120000) {
            SOC_IF_ERROR_RETURN (WC40_REG_WRITE(unit, ws_mld, 0x0, 0x8420, 0x6981));
        } else {
            SOC_IF_ERROR_RETURN (WC40_REG_WRITE(unit, ws_mld, 0x0, 0x8420, 0x6983));
        }     
        SOC_IF_ERROR_RETURN (WC40_REG_WRITE(unit, ws_mld, 0x0, 0x8427, 0x54c5));
    }

    /* mld soft reset */
    SOC_IF_ERROR_RETURN (WC40_REG_WRITE(unit, ws_mld, 0x0, 0x8011, 0xa));
    sal_usleep(15000) ; /* target is 15ms */
    SOC_IF_ERROR_RETURN (WC40_REG_WRITE(unit, ws_mld, 0x0, 0x8011, 0x2));

    /* next check if need to enable cl72 */
    if (pCfg->forced_init_cl72) {
        /* SOC_IF_ERROR_RETURN(_phy_wcmod_cl72_enable(unit, pc, 1)); */
    }

    /*next release  rx/tx asic reset for all the core*/
    tmp_lane_select     = ws_master->lane_select;        /* save this */
    ws_master->lane_select = WCMOD_LANE_BCST; /* go to broadcast mode */

    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DIGITAL5_MISC6r(unit,ws_master, 0,
                                     DIGITAL5_MISC6_RESET_RX_ASIC_MASK |
                                     DIGITAL5_MISC6_RESET_TX_ASIC_MASK));
    
     ws_master->lane_select = tmp_lane_select;

    tmp_lane_select     = ws_slave0->lane_select;        /* save this */
    ws_slave0->lane_select = WCMOD_LANE_BCST; /* go to broadcast mode */


    SOC_IF_ERROR_RETURN(
        MODIFY_WC40_DIGITAL5_MISC6r(unit, ws_slave0, 0,
                                    DIGITAL5_MISC6_RESET_RX_ASIC_MASK | 
                                    DIGITAL5_MISC6_RESET_TX_ASIC_MASK));

    ws_slave0->lane_select = tmp_lane_select;

    tmp_lane_select     = ws_slave1->lane_select;        /* save this */
    ws_slave1->lane_select = WCMOD_LANE_BCST; /* go to broadcast mode */

    SOC_IF_ERROR_RETURN(
        MODIFY_WC40_DIGITAL5_MISC6r(unit, ws_slave1, 0,
                                    DIGITAL5_MISC6_RESET_RX_ASIC_MASK | 
                                    DIGITAL5_MISC6_RESET_TX_ASIC_MASK));
    
    ws_slave1->lane_select = tmp_lane_select;

    return returnValue;
}


STATIC int
phy_wcmod_linkup_evt (int unit, soc_port_t port)
{
   phy_ctrl_t  *pc;
   wcmod_st    *ws;
   WCMOD_DEV_DESC_t *pDesc;
 
   pc = INT_PHY_SW_STATE(unit, port);
   pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
   ws   =  (wcmod_st *)( pDesc + 1);
 
   /* restore mld local fault configuration */
   if (WCMOD_CL73_SOFT_KR2(pc)) {
      if (WCMOD_AN_VALID(pc)) {
         if (WCMOD_AN_CHECK_TIMEOUT(pc)) {
            SOC_IF_ERROR_RETURN
               (WC40_REG_MODIFY(unit, ws, 0x00, 0x842A,0,(1 << 5)));
            WCMOD_AN_VALID_RESET(pc);
         }
      }
   }
   return SOC_E_NONE;
}

/*
 * Function:
 *      phy_wcmod_duplex_set
 * Purpose:
 *      Set the current duplex mode (forced).
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      duplex - Boolean, true indicates full duplex, false indicates half.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_wcmod_duplex_set(int unit, soc_port_t port, int duplex)
{
    uint16       data16;
    phy_ctrl_t  *pc;
    wcmod_st    *ws;
    int duplex_value;
    WCMOD_DEV_DESC_t     *pDesc;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws    =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, ws));

    pc = INT_PHY_SW_STATE(unit, port);
    if (ws->model_type != WCMOD_QS_A0){
        if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode) ||
            CUSTOMX_MODE(pc)) {
            return SOC_E_NONE;
        }
    }

    SOC_IF_ERROR_RETURN
        (READ_WC40_SERDESDIGITAL_STATUS1000X1r(ws->unit, ws, &data16));

    if (!(data16 & SERDESDIGITAL_STATUS1000X1_SGMII_MODE_MASK)) {

        /* 1000X fiber mode, 100fx  */
            
        duplex_value = duplex? FX100_CONTROL1_FULL_DUPLEX_MASK:0;
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_FX100_CONTROL1r(ws->unit, ws,
                   duplex_value,
                   FX100_CONTROL1_FULL_DUPLEX_MASK));
        
        /* 1000X should always be full duplex */
        duplex = TRUE;
    }

    data16 = duplex? MII_CTRL_FD: 0;

    /* program the duplex setting */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_COMBO_IEEE0_MIICNTLr(ws->unit, ws, data16,MII_CTRL_FD));

    return SOC_E_NONE;
}

STATIC int
phy_wcmod_duplex_get(int unit, soc_port_t port, int *duplex)
{
    uint16       reg0_16;
    uint16       mii_ctrl;
    phy_ctrl_t  *pc;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws    =  (wcmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN (_wcmod_phy_parameter_copy(pc, ws));

    if (ws->model_type != WCMOD_QS_A0) {
        if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode) ||
        CUSTOMX_MODE(pc)) {
            *duplex = TRUE;
            return SOC_E_NONE;
        }
    }

    SOC_IF_ERROR_RETURN
        (READ_WC40_SERDESDIGITAL_STATUS1000X1r(ws->unit, ws, &reg0_16));

    /* default to fiber mode duplex */
    *duplex = (reg0_16 & SERDESDIGITAL_STATUS1000X1_DUPLEX_STATUS_MASK)?
              TRUE:FALSE;

    if (reg0_16 & SERDESDIGITAL_STATUS1000X1_SGMII_MODE_MASK) {

    /* retrieve the duplex setting in SGMII mode */
        SOC_IF_ERROR_RETURN
            (READ_WC40_COMBO_IEEE0_MIICNTLr(ws->unit, ws,&mii_ctrl));

        if (mii_ctrl & MII_CTRL_AE) {
            SOC_IF_ERROR_RETURN
                (READ_WC40_COMBO_IEEE0_AUTONEGLPABILr(ws->unit, ws,&reg0_16));

            /* make sure link partner is also in SGMII mode
             * otherwise fall through to use the FD bit in MII_CTRL reg
             */
            if (reg0_16 & MII_ANP_SGMII_MODE) {
                if (reg0_16 & MII_ANP_SGMII_FD) {
                    *duplex = TRUE;
                } else {
                    *duplex = FALSE;
                }
                return SOC_E_NONE;
            }
        }
        *duplex = (mii_ctrl & MII_CTRL_FD) ? TRUE : FALSE;
    } 

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_wcmod_firmware_set
 * Purpose:
 *      write the given firmware to the uController's RAM 
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      offset - offset to the data stream
 *      array  - the given data
 *      datalen- the data length
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
phy_wcmod_firmware_set(int unit, int port, int offset, uint8 *array,int datalen)
{
    return SOC_E_FAIL;
}

STATIC int
phy_wcmod_firmware_load(int unit, int port, wcmod_st *ws, int offset, uint8 *array,int datalen)
{
    phy_ctrl_t  *pc;
    int rv;
    int i;
    int len;
    uint16 data16;
    uint16 mask16;
    uint16 ver;
    uint16 cksum = 0;
    int no_cksum;

    pc = INT_PHY_SW_STATE(unit, port);

    no_cksum = !(DEV_CFG_PTR(pc)->uc_cksum);

    rv = SOC_E_NONE;

    /* initialize the RAM */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_MICROBLK_COMMANDr(unit, ws, MICROBLK_COMMAND_INIT_CMD_MASK,
                                      MICROBLK_COMMAND_INIT_CMD_MASK));

   /* wait for init done */
    rv = wcmod_regbit_set_wait_check(ws,WCMOD_MICROBLK_DOWNLOAD_STATUSr,
                     MICROBLK_DOWNLOAD_STATUS_INIT_DONE_MASK,1,2000000);

    if (rv == SOC_E_TIMEOUT) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(pc->unit,
                             "WCMOD : uC init fails: u=%d p=%d\n"),
                  unit, port));
        return (SOC_E_TIMEOUT);
    }

    /* enable uC timers */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_MICROBLK_COMMAND2r(unit,ws,MICROBLK_COMMAND2_TMR_EN_MASK,
                    MICROBLK_COMMAND2_TMR_EN_MASK));

   /* starting RAM location */
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_MICROBLK_ADDRESSr(unit, ws, offset));

    if ((_phy_wcmod_firmware_set_helper[unit] != NULL) && (DEV_CFG_PTR(pc)->load_mthd == 2)) {

        /* transfer size, 16bytes quantity*/
        if (datalen % 16) {
            len = (((datalen / 16) +1 ) * 16) -1;
        } else {
            len = datalen - 1;
        }
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_MICROBLK_RAMWORDr(unit, ws, len));

        /* allow external access from Warpcore */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_MICROBLK_COMMAND3r(unit, ws,
                            MICROBLK_COMMAND3_EXT_MEM_ENABLE_MASK,
                            MICROBLK_COMMAND3_EXT_MEM_ENABLE_MASK));
        WCMOD_UDELAY(1000);
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_MICROBLK_COMMAND3r(unit, ws,
                            MICROBLK_COMMAND3_EXT_CLK_ENABLE_MASK,
                            MICROBLK_COMMAND3_EXT_CLK_ENABLE_MASK));
        WCMOD_UDELAY(1000);

        rv = _phy_wcmod_firmware_set_helper[unit](unit,port,array,datalen);

        /* restore back no external access from Warpcore */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_MICROBLK_COMMAND3r(unit, ws, 0,
                            MICROBLK_COMMAND3_EXT_MEM_ENABLE_MASK));
        WCMOD_UDELAY(1000);
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_MICROBLK_COMMAND3r(unit, ws, 0,
                            MICROBLK_COMMAND3_EXT_CLK_ENABLE_MASK));
        WCMOD_UDELAY(1000);

    } else {
        /* transfer size */
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_MICROBLK_RAMWORDr(unit, ws, datalen - 1));

        /* start write operation */
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_MICROBLK_COMMANDr(unit, ws,
                 MICROBLK_COMMAND_WRITE_MASK | MICROBLK_COMMAND_RUN_MASK));


        /* write 16-bit word to data register */
        for (i = 0; i < datalen / 2; i++) {
            data16 = array[2 * i] | (array[2 * i + 1] << 8);
            SOC_IF_ERROR_RETURN
                (WRITE_WC40_MICROBLK_WRDATAr(unit, ws, data16));
        }

        /* check if the firmware size is odd number */
        if (datalen % 2) {
            data16 = array[datalen - 1];
            SOC_IF_ERROR_RETURN
                (WRITE_WC40_MICROBLK_WRDATAr(unit, ws, data16));
        }
        /* complete the writing */
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_MICROBLK_COMMANDr(unit, ws, MICROBLK_COMMAND_STOP_MASK));
    }

    SOC_IF_ERROR_RETURN
            (READ_WC40_MICROBLK_DOWNLOAD_STATUSr(unit, ws, &data16));

    mask16 = MICROBLK_DOWNLOAD_STATUS_ERR0_MASK | MICROBLK_DOWNLOAD_STATUS_ERR1_MASK;

    if (data16 & mask16) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(pc->unit,
                             "WCMOD : uC RAM download fails: u=%d p=%d\n"),
                  unit, port));
        return (SOC_E_FAIL);
    }

    /* write a non-zero value to this register to signal uC not perform cksum calculation */
    if (no_cksum) {
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_UC_INFO_B1_CRCr(unit, ws, 0x1234));
    }

    /* release uC's reset */
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_MICROBLK_COMMANDr(unit, ws,
                            MICROBLK_COMMAND_MDIO_UC_RESET_N_MASK));

    /* wait for checksum to be written to this register by uC */

    if (!no_cksum) {
        rv = wcmod_regbit_set_wait_check(ws,0x81fe,0xffff,1,1000000);
        if (rv == SOC_E_TIMEOUT) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(pc->unit,
                                 "WCMOD : uC download: u=%d p=%d timeout: wait for checksum\n"),
                      unit, port));
        } else {
           SOC_IF_ERROR_RETURN
            (READ_WC40_UC_INFO_B1_CRCr(unit, ws, &cksum));
        } 
    } 

    SOC_IF_ERROR_RETURN
        (READ_WC40_UC_INFO_B1_VERSIONr(unit, ws, &ver));

    if (ver == 0) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(pc->unit,
                              "WCMOD : uC RAM download fails: u=%d p=%d\n"),
                   unit, port));
        return (SOC_E_FAIL);
    } else {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(pc->unit,
                             "WCMOD : uC RAM download success: u=%d p=%d ver=%x"), unit, port,ver));
    } 

    if (!no_cksum) {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(pc->unit,
                             " cksum=0x%x\n"), cksum));
    } else {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(pc->unit,
                             "\n")));
    }
    return rv;
}

/*
 * Function:
 *      phy_wcmod_medium_config_set
 * Purpose:
 *      set the configured medium the device is operating on.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      port - Port number
 *      medium - SOC_PORT_MEDIUM_COPPER/FIBER
 *      cfg - not used
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
phy_wcmod_medium_config_set(int unit, soc_port_t port,
                           soc_port_medium_t  medium,
                           soc_phy_config_t  *cfg)
{
    phy_ctrl_t       *pc;
    WCMOD_DEV_DESC_t *pDesc;
    WCMOD_DEV_CFG_t  *pCfg;
    wcmod_st         *ws;
    uint16           fiber_mode = 0;

    pc            = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws = (wcmod_st *)( pDesc + 1);
    pCfg = &pDesc->cfg;

    SOC_IF_ERROR_RETURN(_wcmod_phy_parameter_copy(pc, ws));

    switch (medium) {
    case SOC_PORT_MEDIUM_COPPER:
        DEV_CFG_PTR(pc)->medium = SOC_PORT_MEDIUM_COPPER;
        fiber_mode = 0;
        break;
    case SOC_PORT_MEDIUM_FIBER:
        DEV_CFG_PTR(pc)->medium = SOC_PORT_MEDIUM_FIBER;
        fiber_mode = SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;
        break;
    default:
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(pc->unit, ws, fiber_mode,
                        SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK));
    pCfg->fiber_pref = fiber_mode;
    ws->fiber_pref = pCfg->fiber_pref;

    return SOC_E_NONE;
}
/*
 * Function:
 *      phy_wcmod_medium_get
 * Purpose:
 *      Indicate the configured medium
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      medium - (OUT) One of:
 *              SOC_PORT_MEDIUM_COPPER
 *              SOC_PORT_MEDIUM_FIBER
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
phy_wcmod_medium_get(int unit, soc_port_t port, soc_port_medium_t *medium)
{
    phy_ctrl_t       *pc;

    if (medium == NULL) {
        return SOC_E_PARAM;
    }

    pc            = INT_PHY_SW_STATE(unit, port);
    *medium = DEV_CFG_PTR(pc)->medium;
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_wcmod_master_get
 * Purpose:
 *      this function is meant for 1000Base-T PHY. Added here to support
 *      internal PHY regression test
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      master - (OUT) SOC_PORT_MS_*
 * Returns:
 *      SOC_E_NONE
 * Notes:
 *      The master mode is retrieved for the ACTIVE medium.
 */

STATIC int
phy_wcmod_master_get(int unit, soc_port_t port, int *master)
{
    *master = SOC_PORT_MS_NONE;
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_wcmod_diag_ctrl
 * Purpose:
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 * Returns:
 *      SOC_E_NONE
 * Notes:
 */

/*
 * Variable:
 *      phy_wcmod_drv
 * Purpose:
 *      Phy Driver for 10G (XAUI x 4) Serdes PHY. 
 */
phy_driver_t phy_wcmod_hg = {
    /* .drv_name                      = */ "Warpcore PHY Driver",
    /* .pd_init                       = */ phy_wcmod_init,
    /* .pd_reset                      = */ phy_wcmod_soft_reset,
    /* .pd_link_get                   = */ phy_wcmod_link_get,
    /* .pd_enable_set                 = */ phy_wcmod_enable_set,
    /* .pd_enable_get                 = */ phy_wcmod_enable_get,
    /* .pd_duplex_set                 = */ phy_wcmod_duplex_set,
    /* .pd_duplex_get                 = */ phy_wcmod_duplex_get,
    /* .pd_speed_set                  = */ phy_wcmod_speed_set,
    /* .pd_speed_get                  = */ phy_wcmod_speed_get,
    /* .pd_master_set                 = */ phy_null_set,
    /* .pd_master_get                 = */ phy_wcmod_master_get,
    /* .pd_an_set                     = */ phy_wcmod_an_set,
    /* .pd_an_get                     = */ phy_wcmod_an_get,
    /* .pd_adv_local_set              = */ NULL, /* Deprecated */
    /* .pd_adv_local_get              = */ NULL, /* Deprecated */
    /* .pd_adv_remote_get             = */ NULL, /* Deprecated */
    /* .pd_lb_set                     = */ phy_wcmod_lb_set,
    /* .pd_lb_get                     = */ phy_wcmod_lb_get,
    /* .pd_interface_set              = */ phy_wcmod_interface_set,
    /* .pd_interface_get              = */ phy_wcmod_interface_get,
    /* .pd_ability                    = */ NULL, /* Deprecated */
    /* .pd_linkup_evt                 = */ phy_wcmod_linkup_evt,
    /* .pd_linkdn_evt                 = */ NULL,
    /* .pd_mdix_set                   = */ phy_null_mdix_set,
    /* .pd_mdix_get                   = */ phy_null_mdix_get,
    /* .pd_mdix_status_get            = */ phy_null_mdix_status_get,
    /* .pd_medium_config_set          = */ phy_wcmod_medium_config_set,
    /* .pd_medium_config_get          = */ NULL, 
    /* .pd_medium_get                 = */ phy_wcmod_medium_get,
    /* .pd_cable_diag                 = */ NULL,
    /* .pd_link_change                = */ NULL,
    /* .pd_control_set                = */ phy_wcmod_control_set,    
    /* .pd_control_get                = */ phy_wcmod_control_get,
    /* .pd_reg_read                   = */ phy_wcmod_reg_read,
    /* .pd_reg_write                  = */ phy_wcmod_reg_write, 
    /* .pd_reg_modify                 = */ phy_wcmod_reg_modify,
    /* .pd_notify                     = */ phy_wcmod_notify,
    /* .pd_probe                      = */ phy_wcmod_probe,
    /* .pd_ability_advert_set         = */ phy_wcmod_ability_advert_set,
    /* .pd_ability_advert_get         = */ phy_wcmod_ability_advert_get,
    /* .pd_ability_remote_get         = */ phy_wcmod_ability_remote_get,
    /* .pd_ability_local_get          = */ phy_wcmod_ability_local_get,
    /* .pd_firmware_set               = */ phy_wcmod_firmware_set,
    /* .pdpd_timesync_config_set      = */ NULL,
    /* .pdpd_timesync_config_get      = */ NULL,
    /* .pdpd_timesync_control_set     = */ NULL,
    /* .pdpd_timesync_control_set     = */ NULL,
    /* .pd_diag_ctrl                  = */ phy_wcmod_diag_ctrl,
    /* .pd_lane_control_set           = */ phy_wcmod_per_lane_control_set,    
    /* .pd_lane_control_get           = */ phy_wcmod_per_lane_control_get,
    /* .pd_lane_reg_read              = */ NULL,
    /* .pd_lane_reg_write             = */ NULL,
    /* .pd_oam_config_set             = */ NULL,
    /* .pd_oam_config_get             = */ NULL,
    /* .pd_oam_control_set            = */ NULL,
    /* .pd_oam_control_get            = */ NULL,
    /* .pd_timesync_enhanced_capture_get   = */ NULL,
    /* .pd_multi_get                       = */ NULL,
    /* .pd_precondition_before_probe       = */ NULL,
    /* .pd_linkfault_get                   = */ NULL,
    /* .pd_synce_clk_ctrl_set              = */ NULL,
    /* .pd_synce_clk_ctrl_get              = */ NULL,
    /* .pd_fec_error_inject_set              = */ NULL,
    /* .pd_fec_error_inject_get              = */ NULL
};

/***********************************************************************
 *
 * PASS-THROUGH NOTIFY ROUTINES
 */

/*
 * Function:
 *      _phy_wcmod_notify_duplex
 * Purpose:
 *      Program duplex if (and only if) serdeswcmod is an intermediate PHY.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
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

STATIC int
_phy_wcmod_notify_duplex(int unit, soc_port_t port, uint32 duplex)
{
    int                 fiber;
    uint16              mii_ctrl;
    phy_ctrl_t  *pc;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws    =  (wcmod_st *)( pDesc + 1);

    fiber = DEV_CFG_PTR(pc)->fiber_pref;
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(pc->unit,
                         "_phy_wc40_notify_duplex: "
                         "u=%d p=%d duplex=%d fiber=%d\n"),
              unit, port, duplex, fiber));

    if (SAL_BOOT_SIMULATION) {
        return SOC_E_NONE;
    }

    if (fiber) {
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_COMBO_IEEE0_MIICNTLr(unit, ws,
                                          MII_CTRL_FD, MII_CTRL_FD));
        return SOC_E_NONE;
    }

    /* Put SERDES PHY in reset */
    SOC_IF_ERROR_RETURN
        (_phy_wcmod_notify_stop(unit, port, PHY_STOP_DUPLEX_CHG));

    /* Update duplexity */
    mii_ctrl = (duplex) ? MII_CTRL_FD : 0;
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_COMBO_IEEE0_MIICNTLr(unit, ws, mii_ctrl, MII_CTRL_FD));

    /* Take SERDES PHY out of reset */
    SOC_IF_ERROR_RETURN
        (_phy_wcmod_notify_resume(unit, port, PHY_STOP_DUPLEX_CHG));

    /* Autonegotiation must be turned off to talk to external PHY if
     * SGMII autoneg is not enabled.
     */
    if ((!PHY_SGMII_AUTONEG_MODE(unit, port)) && 
        (!DEV_CFG_PTR(pc)->phy_passthru)) {
        SOC_IF_ERROR_RETURN
            (phy_wcmod_an_set(unit, port, FALSE));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_wcmod_notify_speed
 * Purpose:
 *      Program duplex if (and only if) serdeswcmod is an intermediate PHY.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      speed - Speed to program.
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

STATIC int
_phy_wcmod_notify_speed(int unit, soc_port_t port, uint32 speed)
{
    phy_ctrl_t  *pc;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    int          fiber;
    uint16       sgmii_status;
   
    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws    =  (wcmod_st *)( pDesc + 1);

    if (((ws->model_type) == WCMOD_XN) || ((ws->model_type) == WCMOD_QS_A0) || ((ws->model_type) == WCMOD_QS))  {
        return _phy_wcmod_xgxs16g1l_notify_speed(unit, port, speed) ;
    } else {
        fiber = DEV_CFG_PTR(pc)->fiber_pref;

        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(pc->unit,
                             "_phy_wcmod_notify_speed: "
                             "u=%d p=%d speed=%d fiber=%d\n"),
                  unit, port, speed, fiber));

        if (SAL_BOOT_SIMULATION) {
            return SOC_E_NONE;
        }
        SOC_IF_ERROR_RETURN
            (READ_WC40_SERDESDIGITAL_STATUS1000X1r(unit, ws, &sgmii_status));

        /* Put SERDES PHY in reset */
        SOC_IF_ERROR_RETURN
            (_phy_wcmod_notify_stop(unit, port, PHY_STOP_SPEED_CHG));

        /* Update speed */
        SOC_IF_ERROR_RETURN
            (phy_wcmod_speed_set(unit, port, speed));

        /* Take SERDES PHY out of reset */
        SOC_IF_ERROR_RETURN
            (_phy_wcmod_notify_resume(unit, port, PHY_STOP_SPEED_CHG));

        /* Autonegotiation must be turned off to talk to external PHY */
        if (!PHY_SGMII_AUTONEG_MODE(unit, port) && PHY_EXTERNAL_MODE(unit, port)) {
            SOC_IF_ERROR_RETURN
                (phy_wcmod_an_set(unit, port, FALSE));
        }

        /* Toggle TX FIFO reset if SGMII mode.
               Necessary to address traffic corruption
             */  
        if((speed <= 1000) && 
                (sgmii_status & SERDESDIGITAL_STATUS1000X1_SGMII_MODE_MASK)) {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DIGITAL5_MISC6r(unit,ws, 
                                 DIGITAL5_MISC6_TX_OS8_FRST_SM_MASK, 
                                 DIGITAL5_MISC6_TX_OS8_FRST_SM_MASK));
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DIGITAL5_MISC6r(unit,ws,0, 
                                 DIGITAL5_MISC6_TX_OS8_FRST_SM_MASK));
            LOG_VERBOSE(BSL_LS_SOC_PHY,
                        (BSL_META_U(pc->unit,
                                    "_phy_wcmod_notify_speed: APPLY TX FIFO RST u=%d p=%d \n"),
                         unit, port));
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_wcmod_stop
 * Purpose:
 *      Put serdeswcmod SERDES in or out of reset depending on conditions
 */

STATIC int
_phy_wcmod_stop(int unit, soc_port_t port)
{
    int  stop, copper;
    uint16 mask16,data16;
    phy_ctrl_t  *pc;
    int speed;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;
   
    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws    =  (wcmod_st *)( pDesc + 1);
    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        return SOC_E_NONE;
    }

    /* 'Stop' only for speeds <= 10G. 
     */  
    SOC_IF_ERROR_RETURN
        (phy_wcmod_speed_get(unit,port,&speed));
    if(10000 < speed) {
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
                         "phy_wcmod_stop: u=%d p=%d copper=%d stop=%d flg=0x%x\n"),
              unit, port, copper, stop,
              pc->stop));

    /* tx/rx reset */
    mask16 = DIGITAL5_MISC6_RESET_RX_ASIC_MASK | DIGITAL5_MISC6_RESET_TX_ASIC_MASK;
    data16 = stop? mask16: 0;
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DIGITAL5_MISC6r(unit,ws, data16, mask16));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(pc->unit,
                         "phy_wcmod_stop: u=%d p=%d mask=0x%x value=0x%x\n"),
              unit, port, mask16, data16));

    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_wcmod_notify_stop
 * Purpose:
 *      Add a reason to put serdeswcmod PHY in reset.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      flags - Reason to stop
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
_phy_wcmod_notify_stop(int unit, soc_port_t port, uint32 flags)
{
    phy_ctrl_t  *pc;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws    =  (wcmod_st *)( pDesc + 1);

    INT_PHY_SW_STATE(unit, port)->stop |= flags;

    if (((ws->model_type) == WCMOD_XN) || ((ws->model_type) == WCMOD_QS_A0) || ((ws->model_type) == WCMOD_QS))  {
        return _phy_wcmod_xgxs16g1l_notify_stop(unit, port, flags) ;
    } else {
        INT_PHY_SW_STATE(unit, port)->stop |= flags;
        return _phy_wcmod_stop(unit, port);
    }
}

/*  
 * Function:
 *      _phy_wcmod_notify_resume
 * Purpose:
 *      Remove a reason to put serdeswcmod PHY in reset.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      flags - Reason to stop
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
_phy_wcmod_notify_resume(int unit, soc_port_t port, uint32 flags)
{   
    phy_ctrl_t  *pc;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws    =  (wcmod_st *)( pDesc + 1);

    if (((ws->model_type) == WCMOD_XN) || ((ws->model_type) == WCMOD_QS_A0) || ((ws->model_type) == WCMOD_QS))  {
        return _phy_wcmod_xgxs16g1l_notify_resume(unit, port, flags) ;
    } else {
        INT_PHY_SW_STATE(unit, port)->stop &= ~flags;
        return _phy_wcmod_stop(unit, port);
    }
}

/*
 * Function:
 *      phy_wcmod_media_setup
 * Purpose:     
 *      Configure 
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      fiber_mode - Configure for fiber mode
 *      fiber_pref - Fiber preferrred (if fiber mode)
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int
_phy_wcmod_notify_interface(int unit, soc_port_t port, uint32 intf)
{
    phy_ctrl_t  *pc;
    wcmod_st    *ws;
    WCMOD_DEV_DESC_t     *pDesc;
    uint16 data16;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws    =  (wcmod_st *)( pDesc + 1);

    data16 = 0;
    if (intf != SOC_PORT_IF_SGMII) {
        data16 = SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;
    }
 
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(unit, ws, data16,
                           SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK));
 
    return SOC_E_NONE;
}


STATIC
int _wcmod_chip_init_done(int unit,int chip_num,int phy_mode)
{
    int port;
    phy_ctrl_t  *pc;

    PBMP_ALL_ITER(unit, port) {
        pc = INT_PHY_SW_STATE(unit, port);
        if (pc == NULL) {
            continue;
        }
        if (!pc->dev_name || pc->dev_name != wcmod_device_name) {
            continue;
        }
        if (pc->chip_num != chip_num) {
            continue;
        }
        if ((pc->flags & PHYCTRL_INIT_DONE) && (phy_mode == pc->phy_mode)) {
            return TRUE;
        }
    }
    return FALSE;
}

/******************************************************************************
*              Begin Diagnostic Code                                          *
 */
#ifdef BROADCOM_DEBUG
int
_phy_wcmod_cfg_dump(int unit, soc_port_t port)
{
    phy_ctrl_t  *pc;
    WCMOD_DEV_CFG_t *pCfg;
    WCMOD_DEV_INFO_t  *pInfo;
    WCMOD_DEV_DESC_t *pDesc;
    wcmod_st         *ws; 
    soc_phy_info_t *pi;
    int i;
    int size, rv, speed_mode ;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws    =  (wcmod_st *)( pDesc + 1);

    pi = &SOC_PHY_INFO(unit, port);

    pCfg = &pDesc->cfg;
    pInfo = &pDesc->info;
    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        size = 4;
    } else if (IS_DUAL_LANE_PORT(pc)) {
        size = 2;
    } else {
        size = 1;
    }

    ws->diag_type = WCMOD_DIAG_SPEED;
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
    speed_mode = ws->accData;

    LOG_CLI((BSL_META_U(pc->unit,
                        "pc = 0x%x, pCfg = 0x%x, pInfo = 0x%x\n"), (int)(size_t)pc,
             (int)(size_t)pCfg,(int)(size_t)pInfo));
    for (i = 0; i < size; i++) { 
        LOG_CLI((BSL_META_U(pc->unit,
                            "preemph%d     0x%x\n"),
                 i, pCfg->preemph[i]));
        LOG_CLI((BSL_META_U(pc->unit,
                            "idriver%d     0x%04x\n"),
                 i, pCfg->idriver[i]));
        LOG_CLI((BSL_META_U(pc->unit,
                            "pdriver%d     0x%04x\n"),
                 i, pCfg->pdriver[i]));
    }
    LOG_CLI((BSL_META_U(pc->unit,
                        "auto_medium  0x%04x\n"),
             pCfg->auto_medium));
    LOG_CLI((BSL_META_U(pc->unit,
                        "fiber_pref   0x%04x\n"),
             pCfg->fiber_pref));
    LOG_CLI((BSL_META_U(pc->unit,
                        "sgmii_mstr   0x%04x\n"),
             pCfg->sgmii_mstr));
    LOG_CLI((BSL_META_U(pc->unit,
                        "pdetect10g   0x%04x\n"),
             pCfg->pdetect10g));
    LOG_CLI((BSL_META_U(pc->unit,
                        "pdetect1000x 0x%04x\n"),
             pCfg->pdetect1000x));
    LOG_CLI((BSL_META_U(pc->unit,
                        "cx42hg       0x%04x\n"),
             pCfg->cx42hg));
    LOG_CLI((BSL_META_U(pc->unit,
                        "rxlane_map   0x%04x\n"),
             pCfg->rxlane_map[0]));
    LOG_CLI((BSL_META_U(pc->unit,
                        "txlane_map   0x%04x\n"),
             pCfg->txlane_map[0]));
    LOG_CLI((BSL_META_U(pc->unit,
                        "rxpol        0x%04x\n"),
             pCfg->rxpol));
    LOG_CLI((BSL_META_U(pc->unit,
                        "txpol        0x%04x\n"),
             pCfg->txpol));
    LOG_CLI((BSL_META_U(pc->unit,
                        "cl73an       0x%04x\n"),
             pCfg->cl73an));
    LOG_CLI((BSL_META_U(pc->unit,
                        "phy_mode     0x%04x\n"),
             pc->phy_mode));
    LOG_CLI((BSL_META_U(pc->unit,
                        "cx4_10g      0x%04x\n"),
             pCfg->cx4_10g));
    LOG_CLI((BSL_META_U(pc->unit,
                        "lane0_rst    0x%04x\n"),
             pCfg->lane0_rst));
    LOG_CLI((BSL_META_U(pc->unit,
                        "rxaui        0x%04x\n"),
             pCfg->rxaui));
    LOG_CLI((BSL_META_U(pc->unit,
                        "dxgxs        0x%04x\n"),
             pCfg->dxgxs));
    LOG_CLI((BSL_META_U(pc->unit,
                        "line_intf    0x%04x\n"),
             pCfg->line_intf));
    LOG_CLI((BSL_META_U(pc->unit,
                        "chip_num     0x%04x\n"),
             pc->chip_num));
    LOG_CLI((BSL_META_U(pc->unit,
                        "lane_num     0x%04x\n"),
             pc->lane_num));
    LOG_CLI((BSL_META_U(pc->unit,
                        "speedMax     0x%04d\n"),
             pc->speed_max));
    LOG_CLI((BSL_META_U(pc->unit,
                        "pc->flags    0x%04x\n"),
             pc->flags));
    LOG_CLI((BSL_META_U(pc->unit,
                        "pc->stop     0x%04x\n"),
             pc->stop));
    LOG_CLI((BSL_META_U(pc->unit,
                        "pi->phy_flags   0x%04x\n"),
             pi->phy_flags));
    LOG_CLI((BSL_META_U(pc->unit,
                        "lane_select  0x%04x\n"),
             ws->lane_select));
    LOG_CLI((BSL_META_U(pc->unit,
                        "spd_intf     0x%04x(%s)\n"),
             ws->spd_intf,e2s_wcmod_spd_intfc_set[ws->spd_intf])); 
    LOG_CLI((BSL_META_U(pc->unit,
                        "spd_mode     0x%04x\n"),
             speed_mode));
    LOG_CLI((BSL_META_U(pc->unit,
                        "higi scrm    0x%02x 0x%02x\n"),
             pCfg->hg_mode, pCfg->scrambler_en));  
    LOG_CLI((BSL_META_U(pc->unit,
                        "phy_id       0x%04x\n"),
             pc->phy_id));
    return SOC_E_NONE;
}

#endif  /* BROADCOM_DEBUG */

#define WC_UC_DEBUG
#ifdef WC_UC_DEBUG

static soc_phy_wcmod_uc_desc_t uc_desc_s[MAX_NUM_LANES];

int
wcmod_uc_status_dump(int unit, soc_port_t port, soc_phy_wcmod_uc_desc_t* uc_desc /* size of array MAX_NUM_LANES,  may be NULL */)
{
    phy_ctrl_t  *pc;
    WCMOD_DEV_DESC_t * portDesc;
    soc_phy_wcmod_uc_desc_t * pDesc=NULL;
    wcmod_st  *ws[6];      /* wcmod structure    */
    int i, num_core, temp_monitor[6], core_num = 0, size;   
    int tmp_lane_select = 0 ;
    uint16 data16, tmpLane, tempData, timeout;
    int p1_threshold[] ={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,24,26,28,30,32,34,36,38,40};
    int p1_mapped[] = {0,1,3,2,4,5,7,6,8,9,11,10,12,13,15,14,16,17,19,18,20,21,23,22,24,25,27,26,28,29,31,30};
    uint16 mask16; 
    int regval, rv, osr_mode, pmd_mode = 0, fw_mode = 0;


    pc = INT_PHY_SW_STATE(unit, port);

    portDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws[0]    =  (wcmod_st *)( portDesc + 1);
    num_core = 1;
    SOC_IF_ERROR_RETURN(_wcmod_phy_parameter_copy(pc, ws[0]));

    if (IS_MULTI_CORE_PORT(pc)) {
        /*check the special ILKN port */
        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws[0]->this_lane != 0)) {
            num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
        } else {
            num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        }
    }
    size = 4 * num_core;

    for (i = 0 ; i < num_core; i++) {
        ws[i]   =  (wcmod_st *)( ws[0] + i);
    }

    tmpLane = ws[0]->this_lane; /* save it to restore later */
/*
 *Const DSC_1_ADDR = &H8200&
 *Const DSC_2_ADDR = &H8210&
 *Const DSC_3_ADDR = &H8220&
 *Const DSC_4_ADDR = &H8230&
 *Const DSC_5_ADDR = &H8240&
 * 0x822c,2d,2e,
*/
    if(IS_DUAL_LANE_PORT(pc)) {  ws[0]->dxgxs     = 0  ; } 

    for (i = 0; i < size; i++) {

        if(uc_desc != NULL) {
            pDesc = &uc_desc[i];
        } else {
            pDesc = &uc_desc_s[i];
        }

        WCMOD_MEM_SET((char *)pDesc, 0, (sizeof(soc_phy_wcmod_uc_desc_t)));

        core_num = i / 4;
        ws[core_num]->this_lane = i % 4;

        pDesc->phy_ad = ws[core_num]->phy_ad;
        pDesc->lane = i % 4;

        tmp_lane_select = ws[core_num]->lane_select ;
        switch (i % 4) {
        case 0:
            ws[core_num]->lane_select = WCMOD_LANE_0_0_0_1;
            break;
        case 1:
            ws[core_num]->lane_select = WCMOD_LANE_0_0_1_0;
            break;
        case 2:
            ws[core_num]->lane_select = WCMOD_LANE_0_1_0_0;
            break;
        case 3:
            ws[core_num]->lane_select = WCMOD_LANE_1_0_0_0;
            break;
        default:
            return SOC_E_PARAM;
        }

        /* first check if diagnostic_enable set or not */
        SOC_IF_ERROR_RETURN(READ_WC40_DSC1B0_DSC_DIAG_CTRL0r(ws[core_num]->unit, ws[core_num], &data16));
        if ((data16 & DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK) == 0) {
            /* first make sure that we pause the uc */
            /* check if uc is active or not */
            SOC_IF_ERROR_RETURN(READ_WC40_UC_INFO_B1_VERSIONr(ws[core_num]->unit, ws[core_num], &data16));
            SOC_IF_ERROR_RETURN(READ_WC40_UC_INFO_B1_CRCr(ws[core_num]->unit, ws[core_num], &tempData));
            if ((data16 != 0 ) && (tempData != 0))  {
                /* wait for cmd to be read by micro. i.e it is ready to take next cmd */
                timeout = 0;
                do {
                    SOC_IF_ERROR_RETURN(READ_WC40_DSC1B0_UC_CTRLr(ws[core_num]->unit, ws[core_num], &data16));
                } while (!(data16 & DSC1B0_UC_CTRL_READY_FOR_CMD_MASK)
                        && (++timeout <2000));

                /* check for error */
                SOC_IF_ERROR_RETURN(READ_WC40_DSC1B0_UC_CTRLr(ws[core_num]->unit, ws[core_num], &data16));
                if (data16 & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) {
                    printf("%-22s: Micro reported error\n", FUNCTION_NAME());
                }
                /* next issue command to stop Uc gracefully */
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_DSC1B0_UC_CTRLr(ws[core_num]->unit, ws[core_num], 0x0, DSC1B0_UC_CTRL_READY_FOR_CMD_MASK));
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_DSC1B0_UC_CTRLr(ws[core_num]->unit, ws[core_num], 0x0, DSC1B0_UC_CTRL_ERROR_FOUND_MASK));
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_DSC1B0_UC_CTRLr(ws[core_num]->unit, ws[core_num], 0x0, DSC1B0_UC_CTRL_CMD_INFO_MASK));

                data16 = (0x1 << DSC1B0_UC_CTRL_GP_UC_REQ_SHIFT) |
                      (0x0 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT);
                mask16  = (DSC1B0_UC_CTRL_GP_UC_REQ_MASK  |
                         DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK);
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_DSC1B0_UC_CTRLr(ws[core_num]->unit, ws[core_num], data16, mask16));

                sal_usleep(1000);

                /* wait for uC ready for command:  bit7=1    */
                rv = wcmod_regbit_set_wait_check(ws[core_num], DSC1B0_UC_CTRLr,
                         DSC1B0_UC_CTRL_READY_FOR_CMD_MASK, 1, WC40_PLL_WAIT*1000);

                data16 = ws[core_num]->accData;
                ws[core_num]->accData = ((ws[core_num]->accData) & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) >> DSC1B0_UC_CTRL_ERROR_FOUND_SHIFT;

                if ((rv <0) || (ws[core_num]->accData != 0)) {
                   printf("DSC dump : uController not ready pass 1!: u=%d\n", ws[core_num]->unit);
                }
            }
        }

        if(ws[core_num]->verbosity>0)printf("%s this_lane=%0d\n", FUNCTION_NAME(), ws[core_num]->this_lane) ;
        ws[core_num]->lane_select = tmp_lane_select ;
        /* get the tx taps from tx block */
        switch (i % 4) {
        case 0:
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_TX0_ANATXMDATA1r(unit, ws[core_num], 1 << TX0_ANATXMDATA1_TXSTATUSSELECT_SHIFT, 
                 TX0_ANATXMDATA1_TXSTATUSSELECT_MASK));
            SOC_IF_ERROR_RETURN
                (READ_WC40_TX0_ANATXASTATUS0r(unit, ws[core_num], &data16)); 
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_TX0_ANATXMDATA1r(unit, ws[core_num], 0, 
                 TX0_ANATXMDATA1_TXSTATUSSELECT_MASK));
            break;
        case 1:
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_TX1_ANATXMDATA1r(unit, ws[core_num], 1 << TX1_ANATXMDATA1_TXSTATUSSELECT_SHIFT, 
                 TX1_ANATXMDATA1_TXSTATUSSELECT_MASK));
            SOC_IF_ERROR_RETURN
                (READ_WC40_TX1_ANATXASTATUS0r(unit, ws[core_num], &data16)); 
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_TX1_ANATXMDATA1r(unit, ws[core_num], 0, 
                 TX1_ANATXMDATA1_TXSTATUSSELECT_MASK));
            break;
        case 2:
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_TX2_ANATXMDATA1r(unit, ws[core_num], 1 << TX2_ANATXMDATA1_TXSTATUSSELECT_SHIFT, 
                 TX2_ANATXMDATA1_TXSTATUSSELECT_MASK));
            SOC_IF_ERROR_RETURN
                (READ_WC40_TX2_ANATXASTATUS0r(unit, ws[core_num], &data16)); 
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_TX2_ANATXMDATA1r(unit, ws[core_num], 0, 
                 TX2_ANATXMDATA1_TXSTATUSSELECT_MASK));
            break;
        case 3:
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_TX3_ANATXMDATA1r(unit, ws[core_num], 1 << TX3_ANATXMDATA1_TXSTATUSSELECT_SHIFT, 
                 TX3_ANATXMDATA1_TXSTATUSSELECT_MASK));
            SOC_IF_ERROR_RETURN
                (READ_WC40_TX3_ANATXASTATUS0r(unit, ws[core_num], &data16)); 
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_TX3_ANATXMDATA1r(unit, ws[core_num], 0, 
                 TX3_ANATXMDATA1_TXSTATUSSELECT_MASK));
            break;
        default:
            break;
        }
        /* tx_pre_cursor */
        pDesc->tx_pre_cursor = (data16 & TX0_ANATXASTATUS0_TXSTATUSSELECT_EQUALS_1_TX_FIR_TAP_PRE_MASK)
                                >> TX0_ANATXASTATUS0_TXSTATUSSELECT_EQUALS_1_TX_FIR_TAP_PRE_SHIFT;
        /* tx_main */
        pDesc->tx_main = (data16 & TX0_ANATXASTATUS0_TXSTATUSSELECT_EQUALS_1_TX_FIR_TAP_MAIN_MASK)
                          >> TX0_ANATXASTATUS0_TXSTATUSSELECT_EQUALS_1_TX_FIR_TAP_MAIN_SHIFT;
        /* tx_post_cursor */
        pDesc->tx_post_cursor = (data16 & TX0_ANATXASTATUS0_TXSTATUSSELECT_EQUALS_1_TX_FIR_TAP_POST_MASK)
                                 >> TX0_ANATXASTATUS0_TXSTATUSSELECT_EQUALS_1_TX_FIR_TAP_POST_SHIFT;

        /*tempeture monitor */
        SOC_IF_ERROR_RETURN
            (READ_WC40_UC_INFO_B1_TEMPERATUREr(unit, ws[core_num], &data16));
        regval = data16 & UC_INFO_B1_TEMPERATURE_TEMPERATURE_MASK;
        pDesc->temp = 407 - (regval * 538)/1000;
        temp_monitor[core_num] = pDesc->temp;

        /* vga_bias_reduced */
        SOC_IF_ERROR_RETURN
            (READ_WC40_RX0_ANARXACONTROL2r(unit, ws[core_num], &data16));
        regval = data16 & RX0_ANARXACONTROL2_IMIN_VGA_MASK; 
        if (regval) {
            pDesc->vga_bias_reduced = "88%";
        } else {
            pDesc->vga_bias_reduced = "100%";
        }

        /* postc_metric(lane) = rd22_postc_metric (phy, lane), DSC_5_ADDR=0x8241 */
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC5B0_TUNING_SM_STATUS1r(unit, ws[core_num], &data16));
        regval = data16 & DSC5B0_TUNING_SM_STATUS1_POSTC_METRIC_MASK;
        mask16 = (DSC5B0_TUNING_SM_STATUS1_POSTC_METRIC_MASK >> 1) + 1;
        if (regval >= mask16) {
            regval -= mask16 << 1;
        } 
        pDesc->postc_metric = regval;

        /* read firmware PMD mode */
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_CDR_STATUS2r(unit, ws[core_num], &data16));
        osr_mode = (data16 & 0x800) >> 11;
        osr_mode = (osr_mode * 8) + ((data16 & DSC3B0_CDR_STATUS2_OSCDR_MODE_MASK) >> DSC3B0_CDR_STATUS2_OSCDR_MODE_SHIFT);
         
        /* next get sigdet signal and tx_drvr */
        switch (i % 4) {
        case 0:
            SOC_IF_ERROR_RETURN
                (READ_WC40_RX0_ANARXCONTROLr(unit, ws[core_num], &tempData));
            tempData = tempData & RX0_ANARXCONTROL_STATUS_SEL_MASK;     
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_RX0_ANARXCONTROLr(unit, ws[core_num], 
                 RX0_ANARXCONTROL_STATUS_SEL_sigdetStatus,
                  RX0_ANARXCONTROL_STATUS_SEL_MASK));
            SOC_IF_ERROR_RETURN
                (READ_WC40_RX0_ANARXSTATUSr(unit, ws[core_num], &data16));
            pDesc->sd = (data16 & RX0_ANARXSTATUS_SIGDET_STATUS_CX4_SIGDET_MASK)
                         >> RX0_ANARXSTATUS_SIGDET_STATUS_CX4_SIGDET_SHIFT;
            /* next restore the value */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_RX0_ANARXCONTROLr(unit, ws[core_num], 
                 tempData, RX0_ANARXCONTROL_STATUS_SEL_MASK));
            SOC_IF_ERROR_RETURN
                (READ_WC40_TX0_TX_DRIVERr(unit, ws[core_num], &data16));
            pDesc->tx_drvr = data16; 
            SOC_IF_ERROR_RETURN
                (READ_WC40_UC_INFO_B1_FIRMWARE_MODEr(unit, ws[core_num], &data16));
            fw_mode = data16 & UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_MASK;
            SOC_IF_ERROR_RETURN
                (READ_WC40_UC_INFO_B0_SPEED_DEBUG_LN0r(unit, ws[core_num], &data16));
            pmd_mode = (data16 & 0x700) >> 8;
             
            break;
        case 1:
            SOC_IF_ERROR_RETURN
                (READ_WC40_RX1_ANARXCONTROLr(unit, ws[core_num], &tempData));
            tempData = tempData & RX1_ANARXCONTROL_STATUS_SEL_MASK;     
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_RX1_ANARXCONTROLr(unit, ws[core_num], 
                 RX1_ANARXCONTROL_STATUS_SEL_sigdetStatus,
                  RX1_ANARXCONTROL_STATUS_SEL_MASK));
            SOC_IF_ERROR_RETURN
                (READ_WC40_RX1_ANARXSTATUSr(unit, ws[core_num], &data16));
            pDesc->sd = (data16 & RX1_ANARXSTATUS_SIGDET_STATUS_CX4_SIGDET_MASK)
                         >> RX1_ANARXSTATUS_SIGDET_STATUS_CX4_SIGDET_SHIFT;
            /* next restore the value */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_RX1_ANARXCONTROLr(unit, ws[core_num], 
                 tempData, RX1_ANARXCONTROL_STATUS_SEL_MASK));
            SOC_IF_ERROR_RETURN
                (READ_WC40_TX1_TX_DRIVERr(unit, ws[core_num], &data16));
            pDesc->tx_drvr = data16; 
            SOC_IF_ERROR_RETURN
                (READ_WC40_UC_INFO_B1_FIRMWARE_MODEr(unit, ws[core_num], &data16));
            fw_mode = (data16 & UC_INFO_B1_FIRMWARE_MODE_LN1_MODE_MASK) >> UC_INFO_B1_FIRMWARE_MODE_LN1_MODE_SHIFT;
            SOC_IF_ERROR_RETURN
                (READ_WC40_UC_INFO_B0_SPEED_DEBUG_LN1r(unit, ws[core_num], &data16));
            pmd_mode = (data16 & 0x700) >> 8;
            break;
        case 2:
            SOC_IF_ERROR_RETURN
                (READ_WC40_RX2_ANARXCONTROLr(unit, ws[core_num], &tempData));
            tempData = tempData & RX2_ANARXCONTROL_STATUS_SEL_MASK;     
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_RX2_ANARXCONTROLr(unit, ws[core_num], 
                 RX2_ANARXCONTROL_STATUS_SEL_sigdetStatus,
                  RX2_ANARXCONTROL_STATUS_SEL_MASK));
            SOC_IF_ERROR_RETURN
                (READ_WC40_RX2_ANARXSTATUSr(unit, ws[core_num], &data16));
            pDesc->sd = (data16 & RX2_ANARXSTATUS_SIGDET_STATUS_CX4_SIGDET_MASK)
                         >> RX2_ANARXSTATUS_SIGDET_STATUS_CX4_SIGDET_SHIFT;
            /* next restore the value */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_RX2_ANARXCONTROLr(unit, ws[core_num], 
                 tempData, RX2_ANARXCONTROL_STATUS_SEL_MASK));
            SOC_IF_ERROR_RETURN
                (READ_WC40_TX2_TX_DRIVERr(unit, ws[core_num], &data16));
            pDesc->tx_drvr = data16; 
            SOC_IF_ERROR_RETURN
                (READ_WC40_UC_INFO_B1_FIRMWARE_MODEr(unit, ws[core_num], &data16));
            fw_mode = (data16 & UC_INFO_B1_FIRMWARE_MODE_LN2_MODE_MASK) >> UC_INFO_B1_FIRMWARE_MODE_LN2_MODE_SHIFT;
            SOC_IF_ERROR_RETURN
                (READ_WC40_UC_INFO_B0_SPEED_DEBUG_LN2r(unit, ws[core_num], &data16));
            pmd_mode = (data16 & 0x700) >> 8;
            break;
        case 3:
            SOC_IF_ERROR_RETURN
                (READ_WC40_RX3_ANARXCONTROLr(unit, ws[core_num], &tempData));
            tempData = tempData & RX3_ANARXCONTROL_STATUS_SEL_MASK;     
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_RX3_ANARXCONTROLr(unit, ws[core_num], 
                 RX3_ANARXCONTROL_STATUS_SEL_sigdetStatus,
                  RX3_ANARXCONTROL_STATUS_SEL_MASK));
            SOC_IF_ERROR_RETURN
                (READ_WC40_RX3_ANARXSTATUSr(unit, ws[core_num], &data16));
            pDesc->sd = (data16 & RX3_ANARXSTATUS_SIGDET_STATUS_CX4_SIGDET_MASK)
                         >> RX3_ANARXSTATUS_SIGDET_STATUS_CX4_SIGDET_SHIFT;
            /* next restore the value */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_RX3_ANARXCONTROLr(unit, ws[core_num], 
                 tempData, RX3_ANARXCONTROL_STATUS_SEL_MASK));
            SOC_IF_ERROR_RETURN
                (READ_WC40_TX3_TX_DRIVERr(unit, ws[core_num], &data16));
            pDesc->tx_drvr = data16; 
            SOC_IF_ERROR_RETURN
                (READ_WC40_UC_INFO_B1_FIRMWARE_MODEr(unit, ws[core_num], &data16));
            fw_mode = (data16 & UC_INFO_B1_FIRMWARE_MODE_LN3_MODE_MASK) >> UC_INFO_B1_FIRMWARE_MODE_LN3_MODE_SHIFT;
            SOC_IF_ERROR_RETURN
                (READ_WC40_UC_INFO_B0_SPEED_DEBUG_LN3r(unit, ws[core_num], &data16));
            pmd_mode = (data16 & 0x700) >> 8;
            break;
        default:
            break;
        }
        if (osr_mode == 0 ) {
            if (pmd_mode == 1) {
                strcpy(pDesc->pmd_mode_s,  "OSx1(");
            } else if (pmd_mode == 2) {
                strcpy(pDesc->pmd_mode_s, "OSx1+DFE(");
            } else {
                strcpy(pDesc->pmd_mode_s, "BRx1+DFE(");
            }
        } else if (osr_mode == 1) {
            if (pmd_mode == 1) {
                strcpy(pDesc->pmd_mode_s, "OSx2(");
            } else if (pmd_mode == 2) {
                strcpy(pDesc->pmd_mode_s, "OSx2+DFE(");
            } else {
                strcpy(pDesc->pmd_mode_s, "BRx2+DFE(");
            }
        } else if (osr_mode == 2) {
            strcpy(pDesc->pmd_mode_s, "OSx4(");
        } else if (osr_mode == 3) {
            strcpy(pDesc->pmd_mode_s, "OSx5(");
        } else if (osr_mode == 6) {
            strcpy(pDesc->pmd_mode_s, "OSx8.2(");
        } else if (osr_mode == 7) {
            strcpy(pDesc->pmd_mode_s, "OSx3.3(");
        } else if (osr_mode == 8) {
            strcpy(pDesc->pmd_mode_s, "OSx10(");
        } else if (osr_mode == 9) {
            strcpy(pDesc->pmd_mode_s, "OSx3(");
        } else if (osr_mode == 10) {
            strcpy(pDesc->pmd_mode_s, "OSx8(");
        }
        switch (fw_mode) {
        case 0:
            strcat(pDesc->pmd_mode_s, "0)");
            break;
        case 1:
            strcat(pDesc->pmd_mode_s, "1)");
            break;
        case 2:
            strcat(pDesc->pmd_mode_s, "2)");
            break;
        case 3:
            strcat(pDesc->pmd_mode_s, "3)");
            break;
        case 4:
            strcat(pDesc->pmd_mode_s, "4)");
            break;
        case 5:
            strcat(pDesc->pmd_mode_s, "5)");
            break;
        case 6:
            strcat(pDesc->pmd_mode_s, "6)");
            break;
        case 7:
            strcat(pDesc->pmd_mode_s, "7)");
            break;
        default:
            break;
        }

        /* pf_ctrl(lane) = rd22_pf_ctrl (phy, lane) 0x822b */
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_ANA_STATUS0r(unit, ws[core_num], &data16));
        pDesc->pf_ctrl = data16 & DSC3B0_ANA_STATUS0_PF_CTRL_BIN_MASK;

        /* low pass filter(lane) = rd22_pf_ctrl (phy, lane) 0x821d */
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC2B0_DSC_ANA_CTRL4r(unit, ws[core_num], &data16));
        pDesc->lopf = (data16 & 0x700) >> 8;

        /* vga_sum(lane) = rd22_vga_sum (phy, lane) 0x8225  */
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_DFE_VGA_STATUS0r(unit, ws[core_num], &data16));
        pDesc->vga_sum = data16 & DSC3B0_DFE_VGA_STATUS0_VGA_SUM_MASK;

        /* dfe1_bin(lane) = rd22_dfe_tap_1_bin (phy, lane) 0x8225  */
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_DFE_VGA_STATUS0r(unit, ws[core_num], &data16));
        regval = (data16 & DSC3B0_DFE_VGA_STATUS0_DFE_TAP_1_BIN_MASK) >> DSC3B0_DFE_VGA_STATUS0_DFE_TAP_1_BIN_SHIFT;
        pDesc->dfe1_bin = regval;

        /* dfe2_bin(lane) = rd22_dfe_tap_2_bin (phy, lane) 0x8226  */
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_DFE_VGA_STATUS1r(unit, ws[core_num], &data16));
        regval = data16 & DSC3B0_DFE_VGA_STATUS1_DFE_TAP_2_BIN_MASK;
        mask16 = (DSC3B0_DFE_VGA_STATUS1_DFE_TAP_2_BIN_MASK >> 1) + 1;
        if (regval >= mask16) {
            regval -= mask16 << 1;
        }
        pDesc->dfe2_bin = regval;
        
        /* dfe3_bin(lane) = rd22_dfe_tap_3_bin (phy, lane) 0x8226  */
        regval = (data16 & DSC3B0_DFE_VGA_STATUS1_DFE_TAP_3_BIN_MASK) >> DSC3B0_DFE_VGA_STATUS1_DFE_TAP_3_BIN_SHIFT;
        mask16 = ((DSC3B0_DFE_VGA_STATUS1_DFE_TAP_3_BIN_MASK) >> (DSC3B0_DFE_VGA_STATUS1_DFE_TAP_3_BIN_SHIFT + 1)) + 1;
        if (regval >= mask16) {
            regval -= mask16 << 1;
        }
        pDesc->dfe3_bin = regval;
        
        /* dfe4_bin(lane) = rd22_dfe_tap_4_bin (phy, lane) 0x8227  */
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_DFE_VGA_STATUS2r(unit, ws[core_num], &data16));
        regval = data16 & DSC3B0_DFE_VGA_STATUS2_DFE_TAP_4_BIN_MASK;
        mask16 = (DSC3B0_DFE_VGA_STATUS2_DFE_TAP_4_BIN_MASK >> 1) + 1;
        if (regval >= mask16) {
            regval -= mask16 << 1;
        }
        pDesc->dfe4_bin = regval;
  
        /* dfe5_bin(lane) = rd22_dfe_tap_5_bin (phy, lane) 0x8227  */
        regval = (data16 & DSC3B0_DFE_VGA_STATUS2_DFE_TAP_5_BIN_MASK) >> DSC3B0_DFE_VGA_STATUS2_DFE_TAP_5_BIN_SHIFT;
        mask16 = ((DSC3B0_DFE_VGA_STATUS2_DFE_TAP_5_BIN_MASK) >> (DSC3B0_DFE_VGA_STATUS2_DFE_TAP_5_BIN_SHIFT + 1)) + 1;
        if (regval >= mask16) {
            regval -= mask16 << 1;
        }
        pDesc->dfe5_bin = regval;

        /* dsc ststus 0x822a */
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_ACQ_SM_STATUS1r(unit, ws[core_num], &data16));
        pDesc->dsc_sm = data16;

        /* integ_reg(lane) = rd22_integ_reg (phy, lane) DSC_3_ADDR=0x8220  */
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_CDR_STATUS0r(unit, ws[core_num], &data16));
        regval = data16 & DSC3B0_CDR_STATUS0_INTEG_REG_MASK;
        mask16 = (DSC3B0_CDR_STATUS0_INTEG_REG_MASK >> 1) + 1;
        if (regval >= mask16) {
            regval -= mask16 << 1;
        }
        regval /= 84;
        pDesc->integ_reg = regval;

        /* integ_reg_xfer(lane) = rd22_integ_reg_xfer (phy, lane)  0x8221  */
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_CDR_STATUS1r(unit, ws[core_num], &data16));
        regval = data16 & DSC3B0_CDR_STATUS1_INTEG_REG_XFER_MASK;
        mask16 = (DSC3B0_CDR_STATUS1_INTEG_REG_XFER_MASK >> 1) + 1;
        if (regval >= mask16) {
            regval -= mask16 << 1;
        }
        pDesc->integ_reg_xfer = regval;

        /* clkp1_offset(lane) = rd22_clkp1_phase_offset (phy, lane)  0x8223 */
        /* first set the p1 offset selelction it */
         SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_PI_CTRL0r(unit, ws[core_num], 0x400, 0x400));

        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_PI_STATUS0r(unit, ws[core_num], &data16));
        pDesc->clkp1_offset = (data16 & DSC3B0_PI_STATUS0_CLK90_PHASE_OFFSET_MASK) >> DSC3B0_PI_STATUS0_CLK90_PHASE_OFFSET_SHIFT;
        pDesc->clkp1_offset = pDesc->clkp1_offset - (data16 & 0x7f);
        if (pDesc->clkp1_offset < 0) {
          pDesc->clkp1_offset += 128;
        }
         SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_PI_CTRL0r(unit, ws[core_num], 0x000, 0x400));

        /* clk90_offset(lane) = rd22_clk90_phase_offset (phy, lane)  0x8223 */
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_PI_STATUS0r(unit, ws[core_num], &data16));
        pDesc->clk90_offset = (data16 & DSC3B0_PI_STATUS0_CLK90_PHASE_OFFSET_MASK) >> DSC3B0_PI_STATUS0_CLK90_PHASE_OFFSET_SHIFT;

        /* Compute P1 ladder level  rd22_rx_p1_eyediag (phy, lane)   0x821c */
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC2B0_DSC_ANA_CTRL3r(unit, ws[core_num], &data16));
        tempData = (data16 & DSC2B0_DSC_ANA_CTRL3_RESERVED_FOR_ECO0_MASK) >> DSC2B0_DSC_ANA_CTRL3_RESERVED_FOR_ECO0_SHIFT;
        data16 = tempData;
        data16 &= 0x1f;
        data16 = p1_mapped[data16];
        pDesc->p1_lvl = p1_threshold[data16];
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC2B0_DSC_ANA_CTRL0r(unit, ws[core_num], &data16));
        data16 = (data16 & DSC2B0_DSC_ANA_CTRL0_RESERVED_FOR_ECO0_MASK) >> DSC2B0_DSC_ANA_CTRL0_RESERVED_FOR_ECO0_SHIFT;
        pDesc->p1_lvl = ((pDesc->p1_lvl) * (150 + 100 * data16)) / 40;
        if ( tempData > 31)   pDesc->p1_lvl = -pDesc->p1_lvl;
        
        /* Compute M1 slicer level  rd22_rx_p1_eyediag (phy, lane)   0x821c */
        pDesc->m1_lvl = 0;
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC2B0_DSC_ANA_CTRL3r(unit, ws[core_num], &data16));
        if (data16 & DSC2B0_DSC_ANA_CTRL3_FORCE_RX_M1_THRESH_ZERO_MASK) {
            tempData = (data16 & DSC2B0_DSC_ANA_CTRL3_RX_THRESH_SEL_MASK) >> DSC2B0_DSC_ANA_CTRL3_RX_THRESH_SEL_SHIFT;
            tempData = tempData *25 + 125;
            pDesc->m1_lvl = (data16 & DSC2B0_DSC_ANA_CTRL3_RX_M1_THRESH_ZERO_MASK) >> DSC2B0_DSC_ANA_CTRL3_RX_M1_THRESH_ZERO_SHIFT; 
            pDesc->m1_lvl =  1 - pDesc->m1_lvl; 
            pDesc->m1_lvl *=  tempData; 
        }  

        /* slicer_target(lane) = ((25*rd22_rx_thresh_sel (phy, lane)) + 125)   0x821c */
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC2B0_DSC_ANA_CTRL3r(unit, ws[core_num], &data16));
        regval = (data16 & DSC2B0_DSC_ANA_CTRL3_RX_THRESH_SEL_MASK) >> DSC2B0_DSC_ANA_CTRL3_RX_THRESH_SEL_SHIFT;
        pDesc->slicer_target = regval * 25 + 125;

        /* offset_pe(lane) = rd22_slicer_offset_pe (phy, lane)   0x822c*/
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_ANA_STATUS1r(unit, ws[core_num],&data16));
        regval = data16 & DSC3B0_ANA_STATUS1_SLICER_OFFSET_PE_MASK;
        mask16 = (DSC3B0_ANA_STATUS1_SLICER_OFFSET_PE_MASK >> 1) + 1;
        if (regval >= mask16) {
            regval -= mask16 << 1;
        }
        pDesc->offset_pe = regval;

        /* offset_ze(lane) = rd22_slicer_offset_ze (phy, lane)  0x822d  */
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_ANA_STATUS2r(unit, ws[core_num], &data16));
        regval = data16 & DSC3B0_ANA_STATUS2_SLICER_OFFSET_ZE_MASK;
        mask16 = (DSC3B0_ANA_STATUS2_SLICER_OFFSET_ZE_MASK >> 1) + 1;
        if (regval >= mask16) {
            regval -= mask16 << 1;
        }
        pDesc->offset_ze = regval;

        /* offset_me(lane) = rd22_slicer_offset_me (phy, lane)  0x822e */
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_ANA_STATUS3r(unit, ws[core_num], &data16));
        regval = data16 & DSC3B0_ANA_STATUS3_SLICER_OFFSET_ME_MASK;
        mask16 = (DSC3B0_ANA_STATUS3_SLICER_OFFSET_ME_MASK >> 1) + 1;
        if (regval >= mask16) {
            regval -= mask16 << 1;
        }
        pDesc->offset_me = regval;

        /* 0x822c  offset_po(lane) = rd22_slicer_offset_po (phy, lane)  0x822c */
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_ANA_STATUS1r(unit, ws[core_num], &data16));
        regval = (data16 & DSC3B0_ANA_STATUS1_SLICER_OFFSET_PO_MASK) >> DSC3B0_ANA_STATUS1_SLICER_OFFSET_PO_SHIFT;
        mask16 = (DSC3B0_ANA_STATUS1_SLICER_OFFSET_PO_MASK >> (DSC3B0_ANA_STATUS1_SLICER_OFFSET_PO_SHIFT + 1)) + 1;
        if (regval >= mask16) {
            regval -= mask16 << 1;
        }
        pDesc->offset_po = regval;

        /*0x822d  offset_zo(lane) = rd22_slicer_offset_zo (phy, lane) 0x822d */
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_ANA_STATUS2r(unit, ws[core_num], &data16));
        regval = (data16 & DSC3B0_ANA_STATUS2_SLICER_OFFSET_ZO_MASK) >> DSC3B0_ANA_STATUS2_SLICER_OFFSET_ZO_SHIFT;
        mask16 = ((DSC3B0_ANA_STATUS2_SLICER_OFFSET_ZO_MASK) >> (DSC3B0_ANA_STATUS2_SLICER_OFFSET_ZO_SHIFT + 1)) + 1;
        if (regval >= mask16) {
            regval -= mask16 << 1;
        }
        pDesc->offset_zo = regval;

        /*0x822e  offset_mo(lane) = rd22_slicer_offset_mo (phy, lane)  0x822e */
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_ANA_STATUS3r(unit, ws[core_num], &data16));
        regval = (data16 & DSC3B0_ANA_STATUS3_SLICER_OFFSET_MO_MASK) >> DSC3B0_ANA_STATUS3_SLICER_OFFSET_MO_SHIFT;
        mask16 = ((DSC3B0_ANA_STATUS3_SLICER_OFFSET_MO_MASK) >> (DSC3B0_ANA_STATUS3_SLICER_OFFSET_MO_SHIFT + 1)) + 1;
        if (regval >= mask16) {
            regval -= mask16 << 1;
        }
        pDesc->offset_mo = regval;

        /* next resume uc */
        /* first check if diagnostic_enable set or not */
        SOC_IF_ERROR_RETURN(READ_WC40_DSC1B0_DSC_DIAG_CTRL0r(ws[core_num]->unit, ws[core_num], &data16));
        if ((data16 & DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK) == 0) {
            /* check if uc is active or not */
            SOC_IF_ERROR_RETURN(READ_WC40_UC_INFO_B1_VERSIONr(ws[core_num]->unit, ws[core_num], &data16));
            SOC_IF_ERROR_RETURN(READ_WC40_UC_INFO_B1_CRCr(ws[core_num]->unit, ws[core_num], &tempData));
            if ((data16 != 0 ) && (tempData != 0))  {
                /* wait for cmd to be read by micro. i.e it is ready to take next cmd */
                timeout = 0;
                do {
                    SOC_IF_ERROR_RETURN(READ_WC40_DSC1B0_UC_CTRLr(ws[core_num]->unit, ws[core_num], &data16));
                } while ((data16 & DSC1B0_UC_CTRL_READY_FOR_CMD_MASK)
                        && (++timeout <2000));

                /* check for error */
                SOC_IF_ERROR_RETURN(READ_WC40_DSC1B0_UC_CTRLr(ws[core_num]->unit, ws[core_num], &data16));
                if (data16 & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) {
                    printf("%-22s: Micro reported error\n", FUNCTION_NAME());
                }
                /* next issue command to resume Uc gracefully */
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_DSC1B0_UC_CTRLr(ws[core_num]->unit, ws[core_num], 0x0, DSC1B0_UC_CTRL_READY_FOR_CMD_MASK));
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_DSC1B0_UC_CTRLr(ws[core_num]->unit, ws[core_num], 0x0, DSC1B0_UC_CTRL_ERROR_FOUND_MASK));
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_DSC1B0_UC_CTRLr(ws[core_num]->unit, ws[core_num], 0x0, DSC1B0_UC_CTRL_CMD_INFO_MASK));

                data16 = (0x1 << DSC1B0_UC_CTRL_GP_UC_REQ_SHIFT) |
                      (0x2 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT);
                mask16  = (DSC1B0_UC_CTRL_GP_UC_REQ_MASK  |
                         DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK);
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_DSC1B0_UC_CTRLr(ws[core_num]->unit, ws[core_num], data16, mask16));

                sal_usleep(1000);

                /* wait for uC ready for command:  bit7=1    */
                rv = wcmod_regbit_set_wait_check(ws[core_num], DSC1B0_UC_CTRLr,
                         DSC1B0_UC_CTRL_READY_FOR_CMD_MASK, 1, WC40_PLL_WAIT*1000);

                data16 = ws[core_num]->accData;
                ws[core_num]->accData = ((ws[core_num]->accData) & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) >> DSC1B0_UC_CTRL_ERROR_FOUND_SHIFT;

                if ((rv <0) || (ws[core_num]->accData != 0)) {
                   printf("DSC dump : uController not ready pass 1!: u=%d\n", ws[core_num]->unit);
                }
            }
        }
        /* 0x8058, bits [13:6].  */
        SOC_IF_ERROR_RETURN
            (READ_WC40_PLL_ANAPLLASTATUS1r(unit, ws[core_num], &data16));
        data16 = (data16 & PLL_ANAPLLASTATUS1_PLL_RANGE_MASK) >> PLL_ANAPLLASTATUS1_PLL_RANGE_SHIFT;
        
        pDesc->pll_range = data16;


    }  /* end of for loop */
    

    /* display */
    if(uc_desc == NULL) {
        LOG_CLI((BSL_META_U(pc->unit,
                            "\n\nDSC parameters for port %d\n\n"), port));
        LOG_CLI((BSL_META_U(pc->unit,
                            "junction temperature: ")));
        for (i = 0; i < size / 4 ; i++) {
           LOG_CLI((BSL_META_U(pc->unit,
                               "core%d %dc "),i, temp_monitor[i]));
        }
        LOG_CLI((BSL_META_U(pc->unit,
                            " \n\n")));

        LOG_CLI((BSL_META_U(pc->unit,
                            "PLL Range: %d\n\n"),data16));

#if 0
        LOG_CLI((BSL_META_U(pc->unit,
                            "LN SD PMD(FW_MODE) PPM  clk90_ofs |clkp1_of p1_lvl m1_lvl "
                            "PF lowp |VGA BIAS DFE1 DFE2 "
                            "DFE3 |DFE4 DFE5 TX_DRVR |PREC MAIN POSTC MTRC PE   "
                            "|ZE   ME  PO  ZO  MO |DSC_SM\n")));
#endif
        LOG_CLI((BSL_META_U(pc->unit,
                            "%2s %2s %12s %4s %9s|%8s %6s %6s %4s %4s|%4s %4s %4s %4s " 
                            "%4s|%4s %4s %7s|%4s %4s %5s %4s %4s|%4s %4s "
                            "%4s %4s %4s|%6s\n"),
                 "LN", "SD", "PMD(FW_MODE)", "PPM", "clk90_ofs", "clkp1_of",
                 "p1_lvl", "m1_lvl", "PF", "lowp", "VGA",
                 "BIAS", "DFE1", "DFE2",
                 "DFE3", "DFE4", "DFE5", "TX_DRVR", "PREC",
                 "MAIN", "POSTC", "MTRC", "PE",
                 "ZE",   "ME",  "PO",  "ZO",  "MO", "DSC_SM"));
     
        for (i = 0; i < size; i++) { 
            pDesc = &uc_desc_s[i]; 
            LOG_CLI((BSL_META_U(pc->unit,
                                "%2d %2d %12s %4d %9d|%8d %6d %6d %4d %4d|"
                                "%4d %4s %4d %4d " 
                                "%4d|%4d %4d %#7x|%4d %4d %5d %4d %4d|"
                                "%4d %4d %4d %4d %4d|%#6x\n"), 
                     i, pDesc->sd, pDesc->pmd_mode_s,
                     pDesc->integ_reg, pDesc->clk90_offset,
                     pDesc->clkp1_offset, pDesc->p1_lvl, 
                     pDesc->m1_lvl, pDesc->pf_ctrl,
                     pDesc->lopf, pDesc->vga_sum, pDesc->vga_bias_reduced, 
                     pDesc->dfe1_bin, pDesc->dfe2_bin,
                     pDesc->dfe3_bin, pDesc->dfe4_bin, 
                     pDesc->dfe5_bin, pDesc->tx_drvr,
                     pDesc->tx_pre_cursor, pDesc->tx_main,
                     pDesc->tx_post_cursor, 
                     pDesc->postc_metric, pDesc->offset_pe,
                     pDesc->offset_ze, pDesc->offset_me, 
                     pDesc->offset_po, pDesc->offset_zo,
                     pDesc->offset_mo, pDesc->dsc_sm)); 
        } 
    }

    /* restore this lane info */
    ws[0]->this_lane = tmpLane;
    ws[0]->lane_select = tmp_lane_select ;
    if(IS_DUAL_LANE_PORT(pc)) {  ws[0]->dxgxs     =  ws[0]->dual_type  ; } 
    /* reset the AER back to lane 0 */
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_AERBLK_AERr(unit,ws[0],0));

    if (num_core > 1) {
        for (i = 1; i < num_core; i++) {
            ws[i]->this_lane = 0;
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_AERBLK_AERr(unit,ws[i],0));
        }
    }

    return SOC_E_NONE;

}

#endif /* WC_UC_DEBUG */ 

#if 0
/* debug info */
/* #ifdef BROADCOM_DEBUG  */
/* make sure link off before calling this func */
STATIC int 
_phy_wcmod_diag_dump(int unit, soc_port_t port, int lane, int diag_val)
{
    int rv, tmp_this_lane, tmp_lane_select, tmp_verbosity ;
    phy_ctrl_t      *pc;      
    /* WCMOD_DEV_CFG_t *pCfg;  */
    /* WCMOD_DEV_INFO_t  *pInfo;  */
    WCMOD_DEV_DESC_t *pDesc;
    wcmod_st    *ws;  
    pc = INT_PHY_SW_STATE(unit, port);

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);
    
    tmp_this_lane   = ws->this_lane ;
    tmp_lane_select = ws->lane_select ;
    tmp_verbosity   = ws->verbosity ;
    ws->verbosity   = 2 ;

    ws->diag_type   = diag_val ;
    ws->this_lane = lane ;
    switch(lane) {
    case 0: ws->lane_select = WCMOD_LANE_0_0_0_1 ;  break;
    case 1: ws->lane_select = WCMOD_LANE_0_0_1_0 ;  break;
    case 2: ws->lane_select = WCMOD_LANE_0_1_0_0 ;  break;
    case 3: ws->lane_select = WCMOD_LANE_1_0_0_0 ;  break;
    default: ws->lane_select = WCMOD_LANE_0_0_0_1 ;break;
    }
    LOG_CLI((BSL_META_U(pc->unit,
                        "\n_phy_wcmod_diag_dump call lane=%0d "
                        "diag_type=%x per_lane_ctl=%x\n\n"),
             lane, diag_val, ws->per_lane_control));
    SOC_IF_ERROR_RETURN
      (wcmod_tier1_selector("WCMOD_DIAG", ws, &rv));
    
    ws->this_lane   = tmp_this_lane ; 
    ws->lane_select = tmp_lane_select ;
    ws->verbosity   = tmp_verbosity ;

    return rv;
}


STATIC int 
_phy_wcmod_info_get(int unit, soc_port_t port, uint32 info_val) 
{
    phy_ctrl_t      *pc;      
    /* WCMOD_DEV_CFG_t *pCfg;  */
    /* WCMOD_DEV_INFO_t  *pInfo;  */
    WCMOD_DEV_DESC_t *pDesc;
    wcmod_st    *ws;  
    pc = INT_PHY_SW_STATE(unit, port);

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);
    
    ws->info_type   = info_val ;
    wcmod_info_get(ws) ;
    return SOC_E_NONE;
}

STATIC int 
_phy_wcmod_info_reg_get(int unit, soc_port_t port, uint32 addr)
{
    int rv, i ;
    phy_ctrl_t      *pc;      
    /* WCMOD_DEV_CFG_t *pCfg;  */
    /* WCMOD_DEV_INFO_t  *pInfo;  */
    WCMOD_DEV_DESC_t *pDesc;
    wcmod_st    *ws;  
    pc = INT_PHY_SW_STATE(unit, port);

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);
    
    rv =  SOC_E_NONE ;
    for(i=0; i<4; i++) {
        rv |= wcmod_info_reg_get(ws, i, addr) ;
    }
    return rv ;
}

STATIC int 
_phy_wcmod_info_reg_set(int unit, soc_port_t port, int lane, uint32 addr, uint16 data)
{
    phy_ctrl_t      *pc;      
    /* WCMOD_DEV_CFG_t *pCfg;  */
    /* WCMOD_DEV_INFO_t  *pInfo;  */
    WCMOD_DEV_DESC_t *pDesc;
    wcmod_st    *ws;  
    pc = INT_PHY_SW_STATE(unit, port);

    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    return wcmod_info_reg_set(ws, lane, addr, data) ;
}
/* #endif   BROADCOM_DEBUG */

/* for bcm_port_phy_control_set/get (phy_wcmod_control_set/get) with 
   SOC_PHY_CONTROL_DUMP */
STATIC int
wcmod_control_info_set(int unit, soc_port_t port, uint32 info_val)
{
    phy_ctrl_t       *pc;
    wcmod_st         *ws;      /*wcmod structure */
    WCMOD_DEV_DESC_t *pDesc;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);

    ws->info_type = info_val ;
    return SOC_E_NONE;
}

STATIC void
wcmod_ability_speed_print(soc_port_ability_t * ability) 
{
    char msg2[128] = {0};
    char msg3[128] = {0};
    sal_sprintf(msg2, "\t\t speed_abilityI(%x):", ability->speed_full_duplex);
    if(ability->speed_full_duplex & SOC_PA_SPEED_10MB )   strcat(msg2,"10M,") ;
    if(ability->speed_full_duplex & SOC_PA_SPEED_100MB )  strcat(msg2,"100M,") ;
    if(ability->speed_full_duplex & SOC_PA_SPEED_1000MB)  strcat(msg2,"1G,") ;
    if(ability->speed_full_duplex & SOC_PA_SPEED_2500MB)  strcat(msg2,"2.5G,") ;
    if(ability->speed_full_duplex & SOC_PA_SPEED_3000MB ) strcat(msg2,"3G,") ;
    if(ability->speed_full_duplex & SOC_PA_SPEED_6000MB ) strcat(msg2,"6G,") ; 
    if(ability->speed_full_duplex & SOC_PA_SPEED_10GB )   strcat(msg2,"10G,") ;
    if(ability->speed_full_duplex & SOC_PA_SPEED_12GB)    strcat(msg2,"12G,") ;
    if(ability->speed_full_duplex & SOC_PA_SPEED_13GB)    strcat(msg2,"13G,") ;
    if(ability->speed_full_duplex & SOC_PA_SPEED_15GB )   strcat(msg2,"15G,") ; 
    if(ability->speed_full_duplex & SOC_PA_SPEED_16GB )   strcat(msg2,"16G,") ; 
    if(ability->speed_full_duplex & SOC_PA_SPEED_20GB )   strcat(msg2,"20G,") ;
    printf("%s\n", msg2);

    strcpy(msg3, "\t\t speed_abilityII:");
    if(ability->speed_full_duplex & SOC_PA_SPEED_21GB )   strcat(msg3,"21G,") ;
    if(ability->speed_full_duplex & SOC_PA_SPEED_25GB )   strcat(msg3,"25G,") ;
    if(ability->speed_full_duplex & SOC_PA_SPEED_30GB )   strcat(msg3,"30G,") ;
    if(ability->speed_full_duplex & SOC_PA_SPEED_40GB )   strcat(msg3,"40G,") ;
    if(ability->speed_full_duplex & SOC_PA_SPEED_42GB )   strcat(msg3,"42G,") ;
    if(ability->speed_full_duplex & SOC_PA_SPEED_100GB )  strcat(msg3,"100G,") ;
    if(ability->speed_full_duplex & SOC_PA_SPEED_120GB )  strcat(msg3,"120G,") ;
    if(ability->speed_full_duplex & SOC_PA_SPEED_127GB )  strcat(msg3,"127G,") ;
    printf("%s\n", msg3); 
}

STATIC int 
wcmod_ability_local_print(int unit, soc_port_t port) 
{
    phy_ctrl_t       *pc;
    wcmod_st         *ws;      /*wcmod structure */
    WCMOD_DEV_DESC_t *pDesc;
    soc_port_ability_t ability ;
    char msg1[128] = {0};
    char msg4[32]  = {0} ;
    uint16 dataup1, data73;
    uint16 dataup3;
    uint16 data_up1_mask;
    uint16 data_up3_mask;
    int an ;
    

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);
    
    phy_wcmod_ability_local_get(unit, port, &ability) ; 
    
    sal_sprintf(msg1, "Unit=%0d Port=%0d ability: cl73an=%0d ", unit, port, DEV_CFG_PTR(pc)->cl73an);
    
    if(ability.pause & SOC_PA_PAUSE )          strcat(msg1, "pause,") ;
    if(ability.pause & SOC_PA_PAUSE_ASYMM )    strcat(msg1, "pause_asym,") ;

    if(ability.medium & SOC_PA_MEDIUM_FIBER)   strcat(msg1, "fiber,") ;
    
    if(ability.interface & SOC_PA_INTF_XGMII)  strcat(msg1, "xgmii,") ;
    if(ability.interface & SOC_PA_INTF_GMII )  strcat(msg1, "gmii,") ;
    if(ability.interface & SOC_PA_INTF_SGMII ) strcat(msg1, "sgmii,") ;

    if(ability.loopback & SOC_PA_LB_PHY)       strcat(msg1, "lb,") ;
    
    if(ability.flags & SOC_PA_AUTONEG)         strcat(msg1, "an,") ;

    printf("%s\n", msg1);

    wcmod_ability_speed_print(&ability) ;
    
    SOC_IF_ERROR_RETURN (READ_WC40_DIGITAL3_UP1r(ws->unit, ws,&dataup1));    
    SOC_IF_ERROR_RETURN (READ_WC40_DIGITAL3_UP3r(ws->unit, ws,&dataup3));
    
    data_up1_mask = DIGITAL3_UP1_DATARATE_20GX4_MASK |
                        DIGITAL3_UP1_DATARATE_16GX4_MASK |
                        DIGITAL3_UP1_DATARATE_15GX4_MASK |
                        DIGITAL3_UP1_DATARATE_13GX4_MASK |
                        DIGITAL3_UP1_DATARATE_12P5GX4_MASK |
                        DIGITAL3_UP1_DATARATE_12GX4_MASK |
                        DIGITAL3_UP1_DATARATE_10GCX4_MASK |
                        DIGITAL3_UP1_DATARATE_10GX4_MASK |
                        DIGITAL3_UP1_DATARATE_6GX4_MASK |
                        DIGITAL3_UP1_DATARATE_5GX4_MASK |
                        DIGITAL3_UP1_DATARATE_2P5GX1_MASK;
    
    data_up3_mask = DIGITAL3_UP3_DATARATE_21GX4_MASK |
                        DIGITAL3_UP3_DATARATE_25P45GX4_MASK |
                        DIGITAL3_UP3_DATARATE_31P5G_MASK |
                        DIGITAL3_UP3_DATARATE_40G_MASK;

    /*read the 73 adv reg */
    SOC_IF_ERROR_RETURN (READ_WC40_AN_IEEE1BLK_AN_ADVERTISEMENT1r(ws->unit, ws, &data73));
    if (data73 & AN_IEEE1BLK_AN_ADVERTISEMENT1_TECHABILITY_MASK) {
        an = USE_CLAUSE_73_AN;
    } else if (dataup1 & data_up1_mask) {
        an = USE_CLAUSE_37_AN | USE_CLAUSE_37_AN_WITH_BAM;
    } else if (dataup3 & data_up3_mask) {
        an = USE_CLAUSE_37_AN | USE_CLAUSE_37_AN_WITH_BAM;
    } else {
        an = USE_CLAUSE_37_AN;
    }

    sal_sprintf(msg4, "ested AN mode(an=%0d)", an) ;
    if(  an & USE_CLAUSE_73_AN )          strcat(msg4, "cl73=1") ;
    if(  an & USE_CLAUSE_73_AN_WITH_BAM ) strcat(msg4, "bam73=1") ;
    if(  an & USE_CLAUSE_37_AN )          strcat(msg4, "cl37=1") ;
    if(  an & USE_CLAUSE_37_AN_WITH_BAM ) strcat(msg4, "bam37=1") ;
    printf("%s\n", msg4) ;

    return SOC_E_NONE;
}

STATIC int
wcmod_control_info_get(int unit, soc_port_t port, uint32 *info_val)
{
    int rv, i ;
    phy_ctrl_t       *pc;
    wcmod_st         *ws;      /*wcmod structure */
    WCMOD_DEV_DESC_t *pDesc;
    uint16            data ;  int lane ;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
     ws   =  (wcmod_st *)( pDesc + 1);
    
    rv = SOC_E_NONE;
    if(ws->info_type) {
        if( ws->info_type & WCMOD_INFO_RD ) {
            for(i=0; i<4; i++) {
                rv |= wcmod_info_reg_get(ws, i, *info_val) ;  /* *info_val as addr input */
            }
        } else if (ws->info_type & WCMOD_INFO_WR ) {
            data = (ws->info_type >> 16) ;
            lane = (ws->info_type & 0x3 ) ;
            rv = wcmod_info_reg_set(ws, lane, *info_val, data) ; /* *info_val as addr input, data=(ws->info_type >>16) */
        } else {
            /* ws->info_type should be set already */
            if( ws->info_type & WCMOD_INFO_ANEG ) wcmod_ability_local_print(unit, port) ; 
            rv = wcmod_info_get(ws) ;
        }
    } else {
        wcmod_uc_status_dump(unit, port) ;
    }
   return rv;
}

#endif  /* end of if 0 */


/********   Eye Margin Diagnostic  **********/

#ifndef __KERNEL__
#include <math.h> 
/* DEBUG function: manually construct a set of test data to test
 * extrapolation algorithem. 
 * type: 0 vertical eye, 1 horizontal right eye, 2 horizontal left eye
 * max_offset: the slicer max offset, for example 23
 * the clock rate is hardcoded to 10312500Khz
 */ 

static char *eye_test_name_str[] = {"Vertical Eye", "", "", "Right Eye", "Left Eye"};

int
wcmod_eye_margin(int unit, soc_port_t port, int type)
{
   phy_ctrl_t      *pc;
    wcmod_st            *ws, *temp_ws;      /*wcmod structure */
    WCMOD_DEV_DESC_t    *pDesc;
    int lane_start, lane_end, tmp_lane, lane_num, i;
    WCMOD_EYE_DIAG_INFOt *veye_info;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WCMOD_DEV_DESC_t *)(pc + 1);
    ws   =  (wcmod_st *)( pDesc + 1);
    temp_ws = (wcmod_st *)( pDesc + 1);

    /*copy the phy control pataremeter into wcmod  */
    SOC_IF_ERROR_RETURN
        (_wcmod_phy_parameter_copy(pc, ws));

    ws->diag_type = WCMOD_DIAG_EYE;
    tmp_lane = ws->this_lane;

    veye_info = WCMOD_MEM_ALLOC(sizeof(WCMOD_EYE_DIAG_INFOt)*12, "wcmod_eye_info");
    if (veye_info == NULL ) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "Unable to allocate memory for "
                              "wcmod eye info\n")));
        return SOC_E_MEMORY;
    }
    /* clear the memory */
    WCMOD_MEM_SET(veye_info, 0, sizeof(WCMOD_EYE_DIAG_INFOt)*12);
    for (i = 0; i < 12; i++) {
         veye_info[i].first_good_ber_idx = INDEX_UNINITIALIZED;
         veye_info[i].first_small_errcnt_idx = INDEX_UNINITIALIZED;
    }

    printf("\nPort %d : Start BER extrapolation for %s\n", port,eye_test_name_str[type]); 
    printf("Port %d : Test time varies from a few minutes to over 20 minutes. Please wait ...\n", port);
    if (IS_MULTI_CORE_PORT(pc)) {
        int num_core;
        /*check the special ILKN port */
        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port) && (ws->this_lane != 0)) {
            num_core = SOC_INFO(unit).port_num_lanes[pc->port] / 4 + 1;
        } else {
            num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        }
        for (i = 0 ; i < num_core; i++) {
            temp_ws   =  (wcmod_st *)( ws + i);
            tmp_lane = temp_ws->this_lane;
            for (lane_num = 0; lane_num <= 3; lane_num++) {
                printf("\nStart test for core %d, lane %d\n", i, lane_num);
                temp_ws->this_lane = lane_num;
                SOC_IF_ERROR_CLEAN_RETURN
                    (_wcmod_eye_margin_data_get(temp_ws, &veye_info[lane_num + i * 4], type), sal_free(veye_info));
                SOC_IF_ERROR_CLEAN_RETURN
                    (_wcmod_eye_margin_ber_cal(&veye_info[lane_num + i * 4], type), sal_free(veye_info));
                SOC_IF_ERROR_CLEAN_RETURN
                    (_wcmod_eye_margin_diagram_cal(&veye_info[lane_num + i * 4], type), sal_free(veye_info));
            }
            temp_ws->this_lane = tmp_lane;
        }
     }else {
        if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
            lane_start = 0;
            lane_end   = 3;
        } else if (IS_DUAL_LANE_PORT(pc)) {
            lane_start = pc->lane_num;
            lane_end = lane_start + 1;
        } else {  /* quad-port mode */
            lane_start = pc->lane_num;
            lane_end   = lane_start;
        }

        for (i = lane_start; i <= lane_end; i++) {
            if (lane_start != lane_end) {
                printf("\nStart test for lane %d\n", i);
            }
            ws->this_lane = i;
            SOC_IF_ERROR_CLEAN_RETURN
                (_wcmod_eye_margin_data_get(ws, &veye_info[i], type), sal_free(veye_info));
            SOC_IF_ERROR_CLEAN_RETURN
                (_wcmod_eye_margin_ber_cal(&veye_info[i], type), sal_free(veye_info));
            SOC_IF_ERROR_CLEAN_RETURN
                (_wcmod_eye_margin_diagram_cal(&veye_info[i], type), sal_free(veye_info));
        }
    }
    ws->this_lane = tmp_lane;
    sal_free(veye_info);

    return SOC_E_NONE;
}
#else   /* #ifndef __KERNEL__ */

int
wcmod_eye_margin(int unit, soc_port_t port, int type)
{
    printf("\nThis function is not supported in Linux kernel mode\n");

    return SOC_E_NONE;
}
#endif /* #ifndef __KERNEL__ */
#else /* INCLUDE_XGXS_WCMOD */
typedef int _xgxs_wcmod_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_XGXS_WCMOD */
