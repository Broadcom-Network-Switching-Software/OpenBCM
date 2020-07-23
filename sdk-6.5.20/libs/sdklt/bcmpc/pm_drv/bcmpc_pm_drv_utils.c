/*! \file bcmpc_pm_drv_utils.c
 *
 * PM driver utility routines.
 *
 * This files is the place holder for utility routines
 * used by all the Port Macro drivers.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>
#include <bcmpc/bcmpc_pm.h>
#include <bcmpc/bcmpc_pm_drv_utils.h>

/*******************************************************************************
 * Local definitions
 */
/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_DEV

/*******************************************************************************
 * Common PM driver function vector.
 */


/*******************************************************************************
 * Public data structure initialization functions.
 */
int
pm_speed_config_t_init(int unit, pm_speed_config_t *speed_config)
{
    SHR_FUNC_ENTER(unit);

    if (speed_config == NULL) {
        SHR_IF_ERR_MSG_EXIT (SHR_E_PARAM,
             (BSL_META_U(unit, "pm_speed_config NULL parameter.\n")));
    }

    sal_memset(speed_config, 0, sizeof(pm_speed_config_t));
    speed_config->fec = PM_PORT_FEC_NONE;
    sal_memset(&(speed_config->lane_config), 0,
                 sizeof(speed_config->lane_config));
    speed_config->flags = 0;

exit:
    SHR_FUNC_EXIT();
}



/*******************************************************************************
 * Public functions
 */

int
pm_drv_port_interface_type_get(int unit, pm_info_t *pm_info,
                               pm_speed_config_t *speed_cfg,
                               phymod_interface_t *interface_type)
{
   pm_port_medium_t media_type = speed_cfg->medium_type;

    switch (speed_cfg->speed){
        case 10:
            *interface_type = phymodInterfaceSGMII;
            break;
        case 100:
            *interface_type = phymodInterfaceSGMII;
            break;
        case 1000:
            if (media_type == PM_PORT_MEDIUM_FIBER) {
                *interface_type = phymodInterface1000X;
            } else if (media_type ==PM_PORT_MEDIUM_BACKPLANE) {
                *interface_type = phymodInterfaceKX;
            } else {
                *interface_type = phymodInterfaceSGMII;
            }
            break;
        case 2500:
            if (media_type ==PM_PORT_MEDIUM_FIBER) {
                *interface_type = phymodInterface1000X;
            } else {
                *interface_type = phymodInterfaceSGMII;
            }
            break;
        case 5000:
            if (speed_cfg->num_lanes == 1){
                *interface_type = phymodInterfaceXFI;
            } else if (speed_cfg->num_lanes == 2) {
                *interface_type = phymodInterfaceRXAUI;
            } else {
                *interface_type = phymodInterfaceXFI;
            }
            break;
        case 10000:
            if (speed_cfg->num_lanes == 1){
                if (media_type ==PM_PORT_MEDIUM_FIBER) {
                   *interface_type = phymodInterfaceSFI;
                } else {
                   *interface_type = phymodInterfaceXFI;
                }
            } else if (speed_cfg->num_lanes == 2) {
               *interface_type = phymodInterfaceRXAUI;
            } else {
               *interface_type = phymodInterfaceXAUI;
            }
            break;
        case 11000:
            *interface_type = phymodInterfaceXGMII;
            break;
        case 20000:
            *interface_type = phymodInterfaceXAUI;
            break;
        case 21000:
            *interface_type = phymodInterfaceXAUI;
            break;
        case 25000:
            *interface_type = phymodInterfaceXAUI;
            break;
        case 27000:
            *interface_type = phymodInterfaceKR;
            break;
        case 40000:
    /*        if (speed_cfg->encap_mode){
                *interface_type = phymodInterfaceXGMII;
            } elsei*/ {
                *interface_type = phymodInterfaceXLAUI;
            }
            break;
        case 42000:
            *interface_type = phymodInterfaceKR4;
            break;
        case 50000:
            if (speed_cfg->num_lanes == 1){
                 if (media_type ==PM_PORT_MEDIUM_BACKPLANE) {
                     *interface_type = phymodInterfaceKR;
                 } else if (media_type ==PM_PORT_MEDIUM_FIBER) {
                     *interface_type = phymodInterfaceSR;
                 } else {
                     *interface_type = phymodInterfaceCR;
                 }
            } else if (speed_cfg->num_lanes == 2) {
                 if (media_type ==PM_PORT_MEDIUM_BACKPLANE) {
                     *interface_type = phymodInterfaceKR2;
                 } else if (media_type ==PM_PORT_MEDIUM_FIBER) {
                     *interface_type = phymodInterfaceSR2;
                 } else {
                     *interface_type = phymodInterfaceCR2;
                 }
            } else {
                *interface_type = phymodInterfaceKR;
            }
            break;
        case 53000:
            *interface_type = phymodInterfaceKR2;
            break;
        case 100000:
            if (speed_cfg->num_lanes == 2) {
                 if (media_type ==PM_PORT_MEDIUM_BACKPLANE) {
                     *interface_type = phymodInterfaceKR2;
                 } else if (media_type ==PM_PORT_MEDIUM_FIBER) {
                     *interface_type = phymodInterfaceSR2;
                 } else {
                     *interface_type = phymodInterfaceCR2;
                 }
            } else if (speed_cfg->num_lanes == 4) {
                 if (media_type ==PM_PORT_MEDIUM_BACKPLANE) {
                     *interface_type = phymodInterfaceKR4;
                 } else if (media_type ==PM_PORT_MEDIUM_FIBER) {
                     *interface_type = phymodInterfaceSR4;
                 } else {
                     *interface_type = phymodInterfaceCR4;
                 }
            } else {
                *interface_type = phymodInterfaceKR4;
            }
            break;
        case 106000:
            *interface_type = phymodInterfaceKR4;
            break;
        case 200000:
            if (speed_cfg->num_lanes == 4) {
                if (media_type ==PM_PORT_MEDIUM_BACKPLANE) {
                    *interface_type = phymodInterfaceKR4;
                } else if (media_type ==PM_PORT_MEDIUM_FIBER) {
                    *interface_type = phymodInterfaceSR4;
                } else {
                    *interface_type = phymodInterfaceCR4;
                }
            } else {
                *interface_type = phymodInterfaceKR4;
            }
            break;
        case 300000:
            break;
        case 350000:
            break;
        case 400000:
            if (speed_cfg->num_lanes == 4) {
                if (media_type ==PM_PORT_MEDIUM_BACKPLANE) {
                    *interface_type = phymodInterfaceKR4;
                } else if (media_type ==PM_PORT_MEDIUM_FIBER){
                    *interface_type = phymodInterfaceSR4;
                } else {
                    *interface_type = phymodInterfaceCR4;
                }
            } else {
                *interface_type = phymodInterfaceKR4;
            }
            break;
    }

    return SHR_E_NONE;
}

void
pm_port_oper_status_set(pm_oper_status_t *op_st, pm_oper_status_type_t st)
{
    if (st == PM_OPER_SUCCESS) {
        return;
    }
    op_st->valid = true;
    op_st->status = st;
    op_st->num_port = 0;
}

int
pm_port_ability_to_phy_ability_get(int unit, int num_abilities,
                                pm_port_ability_t *abilities,
                                phymod_autoneg_advert_abilities_t *an_abilities)
{
    int idx;
    phymod_autoneg_advert_ability_t *an_ability = NULL;

    SHR_FUNC_ENTER(unit);

    an_abilities->num_abilities = num_abilities;

    for (idx = 0; idx < num_abilities; idx++) {
        an_ability = &(an_abilities->autoneg_abilities[idx]);
        an_ability->speed = abilities[idx].speed;
        an_ability->resolved_num_lanes = abilities[idx].num_lanes;

        switch (abilities[idx].fec_type) {
            case PM_PORT_FEC_NONE:
                an_ability->fec = phymod_fec_None;
                break;
            case PM_PORT_FEC_BASE_R:
                an_ability->fec = phymod_fec_CL74;
                break;
            case PM_PORT_FEC_RS_528:
                an_ability->fec = phymod_fec_CL91;
                break;
            case PM_PORT_FEC_RS_544:
                an_ability->fec = phymod_fec_RS544;
                break;
            case PM_PORT_FEC_RS_544_2XN:
                an_ability->fec = phymod_fec_RS544_2XN;
                break;
            case PM_PORT_FEC_RS_272:
                an_ability->fec = phymod_fec_RS272;
                break;
            case PM_PORT_FEC_RS_272_2XN:
                an_ability->fec = phymod_fec_RS272_2XN;
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }

        switch (abilities[idx].pause) {
            case PM_PORT_PAUSE_NONE:
                an_ability->pause = phymod_pause_none;
                break;
            case PM_PORT_PAUSE_TX:
                an_ability->pause = phymod_pause_asym;
                break;
            case PM_PORT_PAUSE_RX:
                an_ability->pause = phymod_pause_asym_symm;
                break;
            case PM_PORT_PAUSE_SYMM:
                an_ability->pause = phymod_pause_symm;
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }

        switch (abilities[idx].an_mode) {
            case PM_PORT_AUTONEG_MODE_CL73:
                an_ability->an_mode = phymod_AN_MODE_CL73;
                break;
            case PM_PORT_AUTONEG_MODE_CL73_BAM:
                an_ability->an_mode = phymod_AN_MODE_CL73BAM;
                break;
            case PM_PORT_AUTONEG_MODE_CL73_MSA:
                an_ability->an_mode = phymod_AN_MODE_MSA;
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if (abilities[idx].medium == PM_PORT_MEDIUM_COPPER) {
            an_ability->medium = phymodFirmwareMediaTypeCopperCable;
        } else {
            an_ability->medium = phymodFirmwareMediaTypePcbTraceBackPlane;
        }

        if (abilities[idx].channel == PM_PORT_CHANNEL_SHORT) {
            an_ability->channel = phymod_channel_short;
        } else {
            an_ability->channel = phymod_channel_long;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
pm_phy_ability_to_port_ability_get(int unit,
                         const phymod_autoneg_advert_abilities_t *an_abilities,
                         uint32_t *num_abilities, pm_port_ability_t *abilities)
{
    uint32_t idx;
    phymod_autoneg_advert_ability_t *an_ability = NULL;

    SHR_FUNC_ENTER(unit);

    *num_abilities = an_abilities->num_abilities;

    for (idx = 0; idx < *num_abilities; idx++) {
        an_ability = &(an_abilities->autoneg_abilities[idx]);
        abilities[idx].speed = an_ability->speed;
        abilities[idx].num_lanes = an_ability->resolved_num_lanes;
        switch (an_ability->fec) {
            case phymod_fec_None:
                abilities[idx].fec_type = PM_PORT_FEC_NONE;
                break;
            case phymod_fec_CL74:
                abilities[idx].fec_type = PM_PORT_FEC_BASE_R;
                break;
            case phymod_fec_CL91:
                abilities[idx].fec_type = PM_PORT_FEC_RS_528;
                break;
            case phymod_fec_RS544:
                abilities[idx].fec_type = PM_PORT_FEC_RS_544;
                break;
            case phymod_fec_RS544_2XN:
                abilities[idx].fec_type = PM_PORT_FEC_RS_544_2XN;
                break;
            case phymod_fec_RS272:
                abilities[idx].fec_type = PM_PORT_FEC_RS_272;
                break;
            case phymod_fec_RS272_2XN:
                abilities[idx].fec_type = PM_PORT_FEC_RS_272_2XN;
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }

        switch (an_ability->pause) {
            case phymod_pause_none:
                abilities[idx].pause = PM_PORT_PAUSE_NONE;
                break;
            case phymod_pause_asym:
                abilities[idx].pause = PM_PORT_PAUSE_TX;
                break;
            case phymod_pause_asym_symm:
                abilities[idx].pause = PM_PORT_PAUSE_RX;
                break;
            case phymod_pause_symm:
                abilities[idx].pause = PM_PORT_PAUSE_SYMM;
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }

        switch (an_ability->an_mode) {
            case phymod_AN_MODE_CL73:
                abilities[idx].an_mode = PM_PORT_AUTONEG_MODE_CL73;
                break;
            case phymod_AN_MODE_CL73BAM:
                abilities[idx].an_mode = PM_PORT_AUTONEG_MODE_CL73_BAM;
                break;
            case phymod_AN_MODE_MSA:
                abilities[idx].an_mode = PM_PORT_AUTONEG_MODE_CL73_MSA;
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if (an_ability->medium == phymodFirmwareMediaTypeCopperCable) {
            abilities[idx].medium = PM_PORT_MEDIUM_COPPER;
        } else {
            abilities[idx].medium = PM_PORT_MEDIUM_BACKPLANE;
        }

        if (an_ability->channel == phymod_channel_short) {
            abilities[idx].channel = PM_PORT_CHANNEL_SHORT;
        } else {
            abilities[idx].channel = PM_PORT_CHANNEL_LONG;
        }

    }

exit:
    SHR_FUNC_EXIT();
}

