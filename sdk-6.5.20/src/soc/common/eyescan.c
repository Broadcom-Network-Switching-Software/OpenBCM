/* 
 * $Id:
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC EYESCAN
 */


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PORT

#include <shared/bsl.h>

#include <sal/types.h>
#include <soc/eyescan.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/mem.h>
#include <soc/phy/phyctrl.h>

#ifdef PHYMOD_SUPPORT
#include <phymod/phymod_diagnostics.h>
#include <phymod/phymod_diag.h>
#endif

#ifdef PORTMOD_SUPPORT
#include <soc/portmod/portmod.h>
#endif

#ifndef __KERNEL__
#include <math.h>
#endif /* #ifndef __KERNEL__ */


STATIC soc_port_phy_eyescan_counter_cb_t counter_func[SOC_MAX_NUM_DEVICES][socPortPhyEyescanNofCounters] = {{{0}}};

int
soc_port_phy_eyescan_counter_register(int unit, soc_port_phy_eyscan_counter_t counter, soc_port_phy_eyescan_counter_cb_t* cf)
{
    if (!SOC_UNIT_VALID(unit)) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "Invalid unit\n")));
        return SOC_E_UNIT;
    }

    if(counter >= socPortPhyEyescanNofCounters || counter < 0) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "Counter %d isn't supported\n"), counter));
        return SOC_E_PARAM;
    }

    if(NULL == cf) { /*clear callbacks*/
        counter_func[unit][counter].clear_func = NULL;
        counter_func[unit][counter].get_func = NULL;
    } else {
        counter_func[unit][counter] = *cf;
    }

    return SOC_E_NONE;
}

STATIC int
soc_port_phy_eyescan_check_bounds(int unit, soc_port_t port, uint32 inst, int flags, soc_port_phy_eyscan_counter_t counter, soc_port_phy_eye_bounds_t* bounds)
{
    int vmax, vmin, l_hmax, r_hmax;
    int locked;
    int rv = SOC_E_NONE;

    if((socPortPhyEyescanCounterRelativePhy!=counter) && bounds->vertical_min<0) {
        LOG_CLI((BSL_META_U(unit,
                            "Counter mode doesn't support negative vertical_min. Updated to 0.\n")));
        bounds->vertical_min = 0;
    }
    if(flags & SRD_EYESCAN_FLAG_VERTICAL_ONLY) {
        bounds->horizontal_min = 0;
        bounds->horizontal_max = 0;
    }

    MIIM_LOCK(unit);
    locked = 1;

    if(!(flags & SRD_EYESCAN_FLAG_VERTICAL_ONLY)){
        rv = soc_phyctrl_diag_ctrl(unit, port, inst,
                                   PHY_DIAG_CTRL_CMD, PHY_DIAG_CTRL_EYE_GET_MAX_LEFT_HOFFSET, (int *) (&l_hmax));
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rv)));
            goto exit;
        }
        rv = soc_phyctrl_diag_ctrl(unit, port, inst,
                                   PHY_DIAG_CTRL_CMD, PHY_DIAG_CTRL_EYE_GET_MAX_RIGHT_HOFFSET, (int *) (&r_hmax));
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rv)));
            goto exit;
        }
    }

    rv = soc_phyctrl_diag_ctrl(unit, port, inst,
                               PHY_DIAG_CTRL_CMD, PHY_DIAG_CTRL_EYE_GET_MIN_VOFFSET, (int *) (&vmin));
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rv)));
        goto exit;
    }
    rv = soc_phyctrl_diag_ctrl(unit, port, inst,
                               PHY_DIAG_CTRL_CMD, PHY_DIAG_CTRL_EYE_GET_MAX_VOFFSET, (int *) (&vmax));
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rv)));
        goto exit;
    }

    locked = 0;
    MIIM_UNLOCK(unit);

    if(bounds->vertical_min < vmin){
        LOG_CLI((BSL_META_U(unit,
                            "vertical_min smaller than min available. Updated to %d.\n"), vmin));
        bounds->vertical_min = vmin;
    }
    if (bounds->vertical_max > vmax) {
        LOG_CLI((BSL_META_U(unit,
                            "vertical_max larger than max available. Updated to %d.\n"), vmax));
        bounds->vertical_max = vmax;
    }

    if(!(flags & SRD_EYESCAN_FLAG_VERTICAL_ONLY)){
        if(bounds->horizontal_min < l_hmax){
            LOG_CLI((BSL_META_U(unit,
                                "horizontal_min smaller than min available. Updated to %d.\n"), l_hmax));
            bounds->horizontal_min = l_hmax;
        }
        if (bounds->horizontal_max > r_hmax) {
            LOG_CLI((BSL_META_U(unit,
                                "horizontal_max larger than max available. Updated to %d.\n"), r_hmax));
            bounds->horizontal_max = r_hmax;
        }
    }

    if(bounds->vertical_min < -1*SOC_PORT_PHY_EYESCAN_V_INDEX){
        LOG_CLI((BSL_META_U(unit,
                            "vertical_min smaller than min available. Updated to %d.\n"), -1*SOC_PORT_PHY_EYESCAN_V_INDEX));
        bounds->vertical_min = -1*SOC_PORT_PHY_EYESCAN_V_INDEX;
    }
    if (bounds->vertical_max > SOC_PORT_PHY_EYESCAN_V_INDEX) {
        LOG_CLI((BSL_META_U(unit,
                            "vertical_max larger than max available. Updated to %d.\n"), SOC_PORT_PHY_EYESCAN_V_INDEX));
        bounds->vertical_max = SOC_PORT_PHY_EYESCAN_V_INDEX;
    }

    if(!(flags & SRD_EYESCAN_FLAG_VERTICAL_ONLY)){
        if(bounds->horizontal_min < -1*SOC_PORT_PHY_EYESCAN_H_INDEX){
            LOG_CLI((BSL_META_U(unit,
                                "horizontal_min smaller than min available. Updated to %d.\n"), -1*SOC_PORT_PHY_EYESCAN_H_INDEX));
            bounds->horizontal_min = -1*SOC_PORT_PHY_EYESCAN_H_INDEX;
        }
        if (bounds->horizontal_max > SOC_PORT_PHY_EYESCAN_H_INDEX) {
            LOG_CLI((BSL_META_U(unit,
                                "horizontal_max larger than max available. Updated to %d.\n"), SOC_PORT_PHY_EYESCAN_H_INDEX));
            bounds->horizontal_max = SOC_PORT_PHY_EYESCAN_H_INDEX;
        }
    }

exit:
    if (locked) {
        MIIM_UNLOCK(unit);
    }
    return rv;
}


STATIC int
soc_port_phy_eyescan_enable(int unit, uint32 inst, int flags, soc_port_t port,
                            soc_port_phy_eyscan_counter_t counter, int enable, int* voffset)
{
    int  data;
    int p1_mapped[] = {0,1,3,2,4,5,7,6,8,9,11,10,12,13,15,14,16,17,19,18,20,21,23,22,24,25,27,26,28,29,31,30};
    int hoffset_init=0;
    int rv = SOC_E_NONE;

    MIIM_LOCK(unit);

    switch(counter) {
    case socPortPhyEyescanCounterRelativePhy:
        if(enable) {
            rv = soc_phyctrl_diag_ctrl(unit, port, inst,
                                       PHY_DIAG_CTRL_CMD, PHY_DIAG_CTRL_EYE_ENABLE_LIVELINK, (void *) (0));
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "%s\n"), soc_errmsg(rv)));
                goto exit;
            }
            rv = soc_phyctrl_diag_ctrl(unit, port, inst,
                                       PHY_DIAG_CTRL_CMD, PHY_DIAG_CTRL_EYE_GET_INIT_VOFFSET, voffset);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "%s\n"), soc_errmsg(rv)));
                goto exit;
            }

            /* use the mapped P1 value */
            data = *voffset & 0x1f;
            data = p1_mapped[data];
            data |= (*voffset) & 0x20;
            *voffset = data;

        } else {
            if(SRD_EYESCAN_INVALID_VOFFSET != *voffset) {
                if(!(flags & SRD_EYESCAN_FLAG_VERTICAL_ONLY)) {
                    rv = soc_phyctrl_diag_ctrl(unit, port, inst,
                                               PHY_DIAG_CTRL_CMD, PHY_DIAG_CTRL_EYE_SET_HOFFSET, &hoffset_init);
                    if (SOC_FAILURE(rv)) {
                        LOG_ERROR(BSL_LS_SOC_PHY,
                                  (BSL_META_U(unit,
                                              "%s\n"), soc_errmsg(rv)));
                        goto exit;
                    }
                }
                rv = soc_phyctrl_diag_ctrl(unit, port, inst,
                                           PHY_DIAG_CTRL_CMD, PHY_DIAG_CTRL_EYE_SET_VOFFSET, (int *) voffset);
                if (SOC_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_SOC_PHY,
                              (BSL_META_U(unit,
                                          "%s\n"), soc_errmsg(rv)));
                    goto exit;
                }
            }
            rv = soc_phyctrl_diag_ctrl(unit, port, inst,
                                       PHY_DIAG_CTRL_CMD, PHY_DIAG_CTRL_EYE_DISABLE_LIVELINK, (void *) (0));
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "%s\n"), soc_errmsg(rv)));
                goto exit;
            }
        }
        break;
    case socPortPhyEyescanCounterPrbsPhy:
    case socPortPhyEyescanCounterPrbsMac:
    case socPortPhyEyescanCounterCrcMac:
    case socPortPhyEyescanCounterBerMac:
    default:
        if(enable) {
            rv = soc_phyctrl_diag_ctrl(unit, port, inst,
                                       PHY_DIAG_CTRL_CMD, PHY_DIAG_CTRL_EYE_ENABLE_DEADLINK, (void *) (0));
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "%s\n"), soc_errmsg(rv)));
                goto exit;
            }
        } else {
            rv = soc_phyctrl_diag_ctrl(unit, port, inst,
                                       PHY_DIAG_CTRL_CMD, PHY_DIAG_CTRL_EYE_SET_HOFFSET,  &hoffset_init);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "%s\n"), soc_errmsg(rv)));
                goto exit;
            }
            rv = soc_phyctrl_diag_ctrl(unit, port, inst,
                                       PHY_DIAG_CTRL_CMD, PHY_DIAG_CTRL_EYE_SET_VOFFSET,  &hoffset_init);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "%s\n"), soc_errmsg(rv)));
                goto exit;
            }
            rv = soc_phyctrl_diag_ctrl(unit, port, inst,
                                       PHY_DIAG_CTRL_CMD, PHY_DIAG_CTRL_EYE_DISABLE_DEADLINK, (void *) (0));
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "%s\n"), soc_errmsg(rv)));
                goto exit;
            }
        }
        break;
    }

exit:
    MIIM_UNLOCK(unit);
    return rv;
}

STATIC int
soc_port_phy_eyescan_counter_clear(int unit, soc_port_t port, uint32 inst, soc_port_phy_eyscan_counter_t counter, sal_usecs_t* start_time)
{
    uint32 error;
    int rv = SOC_E_NONE;

     if(NULL != counter_func[unit][counter].get_func) {
        counter_func[unit][counter].clear_func(unit, port);
    } else {
        switch(counter) {
        case socPortPhyEyescanCounterRelativePhy:
            rv = soc_phyctrl_diag_ctrl(unit, port, inst,
                                       PHY_DIAG_CTRL_CMD, PHY_DIAG_CTRL_EYE_CLEAR_LIVELINK, (void *) (0));
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "%s\n"), soc_errmsg(rv)));
                return rv;
            }
            break;
        case socPortPhyEyescanCounterPrbsPhy:
            rv = soc_phyctrl_diag_ctrl(unit, port, inst,
                                       PHY_DIAG_CTRL_CMD, PHY_DIAG_CTRL_EYE_READ_DEADLINK, &error);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "%s\n"), soc_errmsg(rv)));
                return rv;
            }
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "counter isn't supported by the device\n")));
            return SOC_E_PARAM;
            break;
        }
    }

    *start_time = sal_time_usecs();

    return rv;
}

STATIC int
soc_port_phy_eyescan_start(int unit,  soc_port_t port, uint32 inst, soc_port_phy_eyscan_counter_t counter)
{
    int rv = SOC_E_NONE;

    switch(counter) {
    case socPortPhyEyescanCounterRelativePhy:
        rv = soc_phyctrl_diag_ctrl(unit, port, inst,
                                   PHY_DIAG_CTRL_CMD, PHY_DIAG_CTRL_EYE_START_LIVELINK, (void *) (0));
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rv)));
        }
        break;
    case socPortPhyEyescanCounterPrbsMac:
    case socPortPhyEyescanCounterPrbsPhy:
    case socPortPhyEyescanCounterCrcMac:
    case socPortPhyEyescanCounterBerMac:
    default:
        rv = soc_phyctrl_diag_ctrl(unit, port, inst,
                                   PHY_DIAG_CTRL_CMD, PHY_DIAG_CTRL_EYE_START_DEADLINK, (void *) (0));
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rv)));
        }
        break;
    }

    return rv;
}

STATIC int
soc_port_phy_eyescan_stop(int unit, soc_port_t port, uint32 inst, soc_port_phy_eyscan_counter_t counter, sal_usecs_t* end_time)
{
    int rv = SOC_E_NONE;

    switch(counter) {
    case socPortPhyEyescanCounterRelativePhy:
        rv = soc_phyctrl_diag_ctrl(unit, port, inst,
                                   PHY_DIAG_CTRL_CMD, PHY_DIAG_CTRL_EYE_STOP_LIVELINK, (void *) (0));
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rv)));
            return rv;
        }
        break;
    default:
        break;
    }

    *end_time = sal_time_usecs();

    return rv;
}

STATIC int
soc_port_phy_eyescan_counter_get(int unit, soc_port_t port, uint32 inst, soc_port_phy_eyscan_counter_t counter, uint32* err_count)
{
    int rv = SOC_E_NONE;

    if(NULL != counter_func[unit][counter].get_func) {
        counter_func[unit][counter].get_func(unit, port, err_count);
    } else {

        switch(counter) {
        case socPortPhyEyescanCounterRelativePhy:
            rv = soc_phyctrl_diag_ctrl(unit, port, inst,
                                       PHY_DIAG_CTRL_CMD, PHY_DIAG_CTRL_EYE_READ_LIVELINK, err_count);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "%s\n"), soc_errmsg(rv)));
            }
            break;
        case socPortPhyEyescanCounterPrbsPhy:
            rv = soc_phyctrl_diag_ctrl(unit, port, inst,
                                       PHY_DIAG_CTRL_CMD, PHY_DIAG_CTRL_EYE_READ_DEADLINK, err_count);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "%s\n"), soc_errmsg(rv)));
            }
            break;
        default:
            rv = SOC_E_PARAM;
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "Counter isn't supported by the device\n")));
            break;
        }
    }

    return rv;
}

#ifdef PHYMOD_SUPPORT
int _soc_port_phy_is_glue_driver(int unit, soc_port_t port)
{
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    if (!pc) {
        return(0);
    }
    if (!pc->pd) {
        return(0);
    }

    /* Checking id for Sesto */
    if ((pc->phy_id0 == 0xae02) && (pc->phy_id1 == 0x5290)) {
        return(1);
    }

    /* Checking id for furia */
    if ((pc->phy_id0 == 0xae02) && (pc->phy_id1 == 0x5230)) {
        return(1);
    }

    /* Checking id for madura */
    if ((pc->phy_id0 == 0xae02) && (pc->phy_id1 == 0x52b0)) {
        return(1);
    }

    /* Checking id for Qudra28 */
    if ((pc->phy_id0 == 0xae02) && (pc->phy_id1 == 0x5250)) {
        return(1);
    }

    /* Checking id for Dino */
    if ((pc->phy_id0 == 0xae02) && (pc->phy_id1 == 0x5390)) {
        return(1);
    }

    return(0);
}
#endif

int _soc_port_phy_eyescan_run(
    int unit,
    uint32 inst,            /* phy instance containing phy number and interface */
    int flags,
    soc_port_phy_eyescan_params_t* params,
    uint32 nof_ports,
    soc_port_t* ports,
    int* lane_num,
    soc_port_phy_eyescan_results_t* results /*array of result per port*/)
{
#if 0
    int port=0,index=0;
#endif
    int h,v,j;
    int under_threshold, first_run, default_sample_time;
    int total_time, desired_time, time_before;
    sal_usecs_t* start_time = NULL;
    sal_usecs_t end_time = 0;
    uint32 err_count;
    int* v_init_offset = NULL;
    soc_port_t* local_ports = NULL;
    int* local_lane_num = NULL;
    pbmp_t valid_ports_bitmap;
    uint32 is_enable;
#ifdef PHYMOD_SUPPORT
    uint32 done;
#endif
    int dev, intf;
    int rv = SOC_E_NONE;

    /*validate input*/
    if (!SOC_UNIT_VALID(unit)) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "Invalid unit\n")));
        return SOC_E_UNIT;
    }

    if (params == NULL || ports == NULL || results == NULL) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "Null parameter\n")));
        return SOC_E_PARAM;
    }

    if(params->counter >= socPortPhyEyescanNofCounters || params->counter < 0) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "Counter %d isn't supported\n"), params->counter));
        return SOC_E_PARAM;
    }

    /*allocate & initialize local_ports array*/
    local_ports = (soc_port_t*)sal_alloc(sizeof(soc_port_t)*nof_ports,"eyecan local ports");
    for (j = 0; j < nof_ports; j++) {
        local_ports[j] = -1;
    }
    if(NULL == local_ports) {
        rv = SOC_E_MEMORY;
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "Failed to allocate local_ports array\n")));
        goto exit;
    }

    /*allocate & initialize local_lane_num array*/
    local_lane_num = (int*)sal_alloc(sizeof(int)*nof_ports,"eyecan local_lane_num");
    if(NULL == local_lane_num) {
        rv = SOC_E_MEMORY;
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "Failed to allocate local_lane_num array\n")));
        goto exit;
    }

    /*get valid ports*/
    SOC_PBMP_ASSIGN(valid_ports_bitmap, SOC_INFO(unit).port.bitmap);

    intf = PHY_DIAG_INTF_DFLT;
    dev = PHY_DIAG_INST_DEV(inst);
    if (dev == PHY_DIAG_DEV_EXT) {
        intf = PHY_DIAG_INST_INTF(inst);
    }

    for(j = 0 ; j < nof_ports ; j++) {
        if (SOC_PBMP_MEMBER(valid_ports_bitmap, ports[j])) {
            if (dev == PHY_DIAG_DEV_EXT) {
                if (lane_num != NULL) {
                    rv = soc_phyctrl_redirect_control_get(unit, ports[j], dev, lane_num[j],
                        (intf == PHY_DIAG_INTF_SYS), SOC_PHY_CONTROL_RX_SEQ_DONE,
                        &is_enable);
                } else {
                    rv = soc_phyctrl_redirect_control_get(unit, ports[j], dev, -1,
                        (intf == PHY_DIAG_INTF_SYS),
                        SOC_PHY_CONTROL_RX_SEQ_DONE, &is_enable);
                }
                if (SOC_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_SOC_PHY,
                              (BSL_META_U(unit,
                                          "%s\n"), soc_errmsg(rv)));
                    goto exit;
                }
            } else {
                rv = soc_phyctrl_control_get(unit, ports[j],
                                             SOC_PHY_CONTROL_RX_SEQ_DONE,
                                             &is_enable);
                if (SOC_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_SOC_PHY,
                              (BSL_META_U(unit,
                                          "%s\n"), soc_errmsg(rv)));
                    goto exit;
                }
            }
            if(is_enable) {
                local_ports[j] = ports[j];
            } else {
                /*do not run eyescan*/
                local_ports[j] = -1;
            }
        } else {
            rv = SOC_E_PORT;
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "Invalid port %d\n"), ports[j]));
            goto exit;
        }
    }

#ifdef PHYMOD_SUPPORT
    /*enable BER proj*/
    for (j = 0; j < nof_ports; j++) {
        /*if serdes is unlock continue*/
        if (local_ports[j] == -1) continue;
        if(_soc_port_phy_is_glue_driver(unit, local_ports[j]) && (params->type == 4)) {
            rv = soc_phyctrl_diag_ctrl(unit, local_ports[j], inst,
                               PHY_DIAG_CTRL_CMD, PHY_DIAG_CTRL_BER, (void *) (params));
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "%s\n"), soc_errmsg(rv)));
                goto exit;
            }
            /* label this port has been handled */
            local_ports[j] = -1;
        }
    }

    done = 1;
    for (j = 0; j < nof_ports; j++) {
        if(local_ports[j] != -1) {
           done = 0;
           break;
        }
    }
    if(done == 1) {
        goto exit;
    }
#endif

    /* check if the lane num is NULL */
    /* pc->lane_num used to indicate which lane to focus on */
    /* in the leagcy driver                                 */
    if (lane_num != NULL) {
        phy_ctrl_t *pc;
        for(j = 0 ; j < nof_ports ; j++) {
            if (local_ports[j] == -1) continue;
            pc = INT_PHY_SW_STATE(unit, local_ports[j]);
            local_lane_num[j] = pc->lane_num;
            if (pc->phy_mode == PHYCTRL_DUAL_LANE_PORT) {
                pc->lane_num += lane_num[j];
            } else {
                pc->lane_num = lane_num[j];
            }
        }
    }

    /*allocate start_time array*/
    start_time = (uint32*)sal_alloc(sizeof(sal_usecs_t)*nof_ports,"eyecan start time");
    if(NULL == start_time) {
        rv = SOC_E_MEMORY;
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "Failed to allocate start_time array\n")));
        goto exit;
    }

    /*allocate v_init_offset array*/
    v_init_offset = (int*)sal_alloc(sizeof(int)*nof_ports,"eyecan v_init_offset");
    if(NULL == v_init_offset) {
        rv = SOC_E_MEMORY;
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "Failed to allocate v_init_offset array\n")));
        goto exit;
    }
    for(j = 0 ; j < nof_ports ; j++) {
        v_init_offset[j] = SRD_EYESCAN_INVALID_VOFFSET;
    }

    /*enable eyescan*/
    for (j = 0; j < nof_ports; j++) {
        /*if serdes is unlock continue*/
        if (local_ports[j] == -1) continue;
        rv = soc_port_phy_eyescan_enable(unit, inst, flags, local_ports[j], params->counter, 1, &(v_init_offset[j]));
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rv)));
            goto exit;
        }
    }

    sal_usleep(100000);
    /*check bounds*/
    for (j = 0; j < nof_ports; j++) {
        /*if serdes is unlock continue*/
        if (local_ports[j] == -1) continue;
        rv = soc_port_phy_eyescan_check_bounds(unit, local_ports[j], inst, flags, params->counter, &(params->bounds));
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rv)));
            goto exit;
        }
    }

    default_sample_time = params->sample_time;

    /*scan*/
    for(h = params->bounds.horizontal_min; h <= params->bounds.horizontal_max; h++) {
        if(h % params->sample_resolution != 0) {
            if(!(flags & SRD_EYESCAN_FLAG_VERTICAL_ONLY)){
                continue;
            }
        }
        desired_time = 0;
        params->sample_time = default_sample_time;

        for(v = params->bounds.vertical_max; v >= params->bounds.vertical_min; v--) {
            if(v % params->sample_resolution != 0) {
                continue;
            }

            if (flags & SRD_EYESCAN_FLAG_DSC_PRINT) {
                LOG_CLI((BSL_META_U(unit,
                                    "\nStarting BER msmt at offset: v=%d h=%d\n"), v, h));
            }

            under_threshold = params->nof_threshold_links;
            first_run = 1;
            total_time = 0;
            for (j = 0; j < nof_ports; j++) {
                /*if serdes is unlock continue*/
                if (local_ports[j] == -1) continue;

                results[j].run_time[h + SOC_PORT_PHY_EYESCAN_H_INDEX][v + SOC_PORT_PHY_EYESCAN_V_INDEX] = 0;
                results[j].error_count[h + SOC_PORT_PHY_EYESCAN_H_INDEX][v + SOC_PORT_PHY_EYESCAN_V_INDEX] = 0;
            }

            while ((under_threshold >= params->nof_threshold_links) && (params->sample_time <= params->time_upper_bound)) {
                under_threshold = 0;
                if (first_run) {
                    desired_time = params->sample_time;
                    total_time = params->sample_time;
                } else {
                    if ((desired_time >= params->time_upper_bound) || (total_time >= params->time_upper_bound)) {
                        params->sample_time = (params->time_upper_bound + 1);
                        break;
                    }
                    desired_time = desired_time * TIME_MULTIPLIER;
                    params->sample_time = desired_time - total_time;
                    if (params->sample_time > params->time_upper_bound) {
                        params->sample_time = params->time_upper_bound - total_time;
                    }
                    total_time += params->sample_time;
                }

                for (j = 0; (j < nof_ports); j++) {
                    /*if serdes is unlock continue*/
                    if (local_ports[j] == -1) continue;

                    if (first_run) {
                        /*set vertical and horizinal*/
                        if(!(flags & SRD_EYESCAN_FLAG_VERTICAL_ONLY)) {
                            rv = soc_phyctrl_diag_ctrl(unit, local_ports[j], inst,
                                                       PHY_DIAG_CTRL_CMD, PHY_DIAG_CTRL_EYE_SET_HOFFSET, (int*) (&h));
                            if (SOC_FAILURE(rv)) {
                                LOG_ERROR(BSL_LS_SOC_PHY,
                                          (BSL_META_U(unit,
                                                      "%s\n"), soc_errmsg(rv)));
                                goto exit;
                            }
                        }
                        rv = soc_phyctrl_diag_ctrl(unit, local_ports[j], inst,
                                                   PHY_DIAG_CTRL_CMD, PHY_DIAG_CTRL_EYE_SET_VOFFSET, (int *) (&v));/*start measure*/
                        if (SOC_FAILURE(rv)) {
                            LOG_ERROR(BSL_LS_SOC_PHY,
                                      (BSL_META_U(unit,
                                                  "%s\n"), soc_errmsg(rv)));
                            goto exit;
                        }
                        rv = soc_port_phy_eyescan_start(unit, local_ports[j], inst, params->counter);
                        if (SOC_FAILURE(rv)) {
                            LOG_ERROR(BSL_LS_SOC_PHY,
                                      (BSL_META_U(unit,
                                                  "%s\n"), soc_errmsg(rv)));
                            goto exit;
                        }

                        if ((flags & SRD_EYESCAN_FLAG_DSC_PRINT)){
                            soc_phyctrl_control_set(unit, local_ports[j], SOC_PHY_CONTROL_DUMP, 1);
                        }
                    }

                    /*clear counters*/
                    rv = soc_port_phy_eyescan_counter_clear(unit, local_ports[j], inst, params->counter, &(start_time[j]));
                    if (SOC_FAILURE(rv)) {
                        LOG_ERROR(BSL_LS_SOC_PHY,
                                  (BSL_META_U(unit,
                                              "%s\n"), soc_errmsg(rv)));
                        goto exit;
                    }
                }
                sal_usleep(params->sample_time*1000);

                for (j = 0; j < nof_ports; j++) {

                    /*if serdes is unlock continue*/
                    if (local_ports[j] == -1) continue;

                    /*stop measure*/
                    rv = soc_port_phy_eyescan_stop(unit, local_ports[j], inst, params->counter, &end_time);
                    if (SOC_FAILURE(rv)) {
                        LOG_ERROR(BSL_LS_SOC_PHY,
                                  (BSL_META_U(unit,
                                              "%s\n"), soc_errmsg(rv)));
                        goto exit;
                    }
                    rv = soc_port_phy_eyescan_counter_get(unit, local_ports[j], inst,  params->counter, &err_count);
                    if (SOC_FAILURE(rv)) {
                        LOG_ERROR(BSL_LS_SOC_PHY,
                                  (BSL_META_U(unit,
                                              "%s\n"), soc_errmsg(rv)));
                        goto exit;
                    }

                    time_before = results[j].run_time[h + SOC_PORT_PHY_EYESCAN_H_INDEX][v + SOC_PORT_PHY_EYESCAN_V_INDEX];

                    if(end_time > start_time[j]) {
                        results[j].run_time[h + SOC_PORT_PHY_EYESCAN_H_INDEX][v + SOC_PORT_PHY_EYESCAN_V_INDEX] += (end_time - start_time[j])/1000;
                    } else {
                        results[j].run_time[h + SOC_PORT_PHY_EYESCAN_H_INDEX][v + SOC_PORT_PHY_EYESCAN_V_INDEX] += ((SAL_USECS_TIMESTAMP_ROLLOVER - start_time[j]) + end_time)/1000;
                    }
                    results[j].error_count[h + SOC_PORT_PHY_EYESCAN_H_INDEX][v + SOC_PORT_PHY_EYESCAN_V_INDEX] += (err_count);

                    if (results[j].error_count[h + SOC_PORT_PHY_EYESCAN_H_INDEX][v + SOC_PORT_PHY_EYESCAN_V_INDEX] < params->error_threshold) {
                        under_threshold++;
                    }

                    if (flags & SRD_EYESCAN_FLAG_DSC_PRINT) {
                         LOG_CLI((BSL_META_U(unit,
                                             "Added %d errors in %d miliseconds for port %d\n"), err_count, (results[j].run_time[h + SOC_PORT_PHY_EYESCAN_H_INDEX][v + SOC_PORT_PHY_EYESCAN_V_INDEX] - time_before), local_ports[j]));
                    }
#if 0
                    port = local_ports[j];
                    index = j;
#endif
                }

                first_run = 0;
                if (params->counter == socPortPhyEyescanCounterRelativePhy) {
                    break;
                }
            }
            if (first_run) {
                for (j = 0; j < nof_ports; j++) {
                    results[j].run_time[h + SOC_PORT_PHY_EYESCAN_H_INDEX][v + SOC_PORT_PHY_EYESCAN_V_INDEX] = (params->sample_time-1);
                }
            }

            if ((params->counter != socPortPhyEyescanCounterRelativePhy) && (desired_time < params->time_upper_bound)) {
                params->sample_time = total_time;
            }

            if ((params->sample_time < params->time_upper_bound) && (total_time >= params->time_upper_bound)) {
                params->sample_time = params->time_upper_bound;
            }
        }

        if(flags & SRD_EYESCAN_FLAG_VERTICAL_ONLY) {
            break;
        }
    }

    /*highest number of errors for unlock serdes*/
     for (j = 0; j < nof_ports; j++) {
         /*if serdes is unlock continue*/
         if (local_ports[j] != -1) continue;

         LOG_CLI((BSL_META_U(unit,
                             "ERROR: link %d is unlock.\n"), ports[j]));
        /*error results*/
        for(h = params->bounds.horizontal_min ; h <= params->bounds.horizontal_max ; h++) {
            if(h % params->sample_resolution != 0) {
                if(!(flags & SRD_EYESCAN_FLAG_VERTICAL_ONLY)){
                    continue;
                }
            }
            for (v = params->bounds.vertical_min ; v <= params->bounds.vertical_max ; v++) {
                if(v % params->sample_resolution != 0) {
                    continue;
                }

                results[j].run_time[h + SOC_PORT_PHY_EYESCAN_H_INDEX][v + SOC_PORT_PHY_EYESCAN_V_INDEX] = 0;
                results[j].error_count[h + SOC_PORT_PHY_EYESCAN_H_INDEX][v + SOC_PORT_PHY_EYESCAN_V_INDEX] = 0;

            }
        }
     }

exit:
    if (start_time != NULL) {
        sal_free(start_time);
    }

    if (v_init_offset) {

        /*disable eyescan mode*/
        for (j = 0; j < nof_ports; j++) {
            /*if serdes is unlock continue*/
            if (local_ports != NULL) {
                if (local_ports[j] == -1) continue;

                soc_port_phy_eyescan_enable(unit, inst, flags, local_ports[j], params->counter, 0, &(v_init_offset[j]));
            }
        }
        if (v_init_offset != NULL) {
            sal_free(v_init_offset);
        }
    }

    /* check if the lane num needs to be recovered */
    if (lane_num != NULL) {
        phy_ctrl_t *pc;
        for (j = 0 ; j < nof_ports ; j++) {
            if (local_ports != NULL) {
                if (local_ports[j] == -1) continue;
                pc = INT_PHY_SW_STATE(unit, local_ports[j]);
                if (local_lane_num != NULL) {
                    pc->lane_num = local_lane_num[j];
                }
            }
        }
    }

    if (local_ports != NULL) {
        sal_free(local_ports);
    }
    if (local_lane_num != NULL) {
        sal_free(local_lane_num);
    }

    return rv;
}

#ifdef PHYMOD_SUPPORT

/*
 * The term "legacy mode" here is referred to the code before TSCE/TSCF projects.
 * TSCE/TSCE introduces the capability to use uC to assist eyescan, ber and etc.
 * The legacy mode/method used pd_diag_ctrl channel.  However, a phy glue logic
 * could build the phy_diag_ctrl channel on the surface as well, but still
 * deploy the uC assist deep down.
 */
int is_eyescan_algorithm_legacy_mode(int unit, uint32 nof_ports, soc_port_t* ports, uint32 inst)
{
    int legacy;
    phy_ctrl_t *pc;
    soc_phymod_ctrl_t         *pmc;
    soc_phymod_phy_t          *phy;

    legacy = 1;   /* 1 means to use phy_diag_ctrl legacy mode/method */

    if (!nof_ports) return (legacy);

    if (!ext_phy_ctrl[unit]) {
        return (0);
    }
    /* external phy */
    if (PHY_DIAG_INST_DEV(inst) == PHY_DIAG_DEV_EXT) {
        pc = EXT_PHY_SW_STATE(unit, ports[0]);

        if (SOC_USE_PORTCTRL(unit)) {
            /* some phymod device could use phy_diag mode
             * currently a NULL under portmod is assigned for phymod
             * and phymod is assumed to be all non phy_diag mode
             */
            if (!pc) {
                return (0);
            }
            if (!pc->pd) {
                return (0);
            }
        } else {
            if (!pc || !pc->pd) {
                LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "Ext Phy ID0: non portmod mode has NULL extPHY\n")));
                return (0);
            }
        }
        /* if pc is available, use the following check to figure
         * out legacy mode or not
         */
        LOG_INFO(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "Ext Phy ID0: 0x%X, Phy ID1: 0x%X Phy Model: 0x%X \n"),
                   pc->phy_id0, pc->phy_id1, pc->phy_model));

        /* Note:
         *  we should phase out the following code methods soon
         *  for accuracy and portability.
         */
        /* Checking id for furia */
        if ((pc->phy_id0 == 0xae02) && (pc->phy_id1 == 0x5230)) {
            legacy = 0;
        }
        /* Checking id for QSGMIIE and viper*/
        if ((pc->phy_id0 == 0x0143) && (pc->phy_id1 == 0xbff0)) {
            legacy = 0;
        }
        /* Checking id for Qudra28 */
        if ((pc->phy_id0 == 0xae02) && (pc->phy_id1 == 0x5250)) {
            legacy = 0;
        }
        /* Checking id for Sesto */
        if ((pc->phy_id0 == 0xae02) && (pc->phy_id1 == 0x5290)) {
            legacy = 0;
        }
        if ((pc->phy_id0 == 0xae02) && (pc->phy_id1 == 0x52b0)) {
            legacy = 0;
        }
        /* Checking id for PHY8806X */
        if ((pc->phy_id0 == 0xae02) && (pc->phy_id1 == 0x52c0)) {
            legacy = 0;
        }
    } else {
        /*
         * internal phy
         */
        if (SOC_USE_PORTCTRL(unit)) {
            /* Under portmod, internal phymod is assumed to be non phy_diag_ctrl mode.
             * But the assumption could be wrong one day.
             */
            return 0 ;
        } else {
            pc = INT_PHY_SW_STATE(unit, ports[0]);
            if (!pc) {
                return(0);
            }
            LOG_INFO(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "Int Phy ID0: 0x%X, Phy ID1: 0x%X Phy Model: 0x%X \n"),
                       pc->phy_id0, pc->phy_id1, pc->phy_model));

            /* Note:
             *    we should phase out the following code methods soon
             *    for accuracy and portability.
             */
            if ((pc->phy_id0 == 0x143) && (pc->phy_id1 == 0xbff0) && (pc->phy_model == 0x3f)) {
                return (legacy);
            }
            /* Checking id for TSC/WC family; check phymod object for TSCE/TSCF */
            if ((pc->phy_id0 == 0x600d) && (pc->phy_id1 == 0x8770)) {
                pmc = &pc->phymod_ctrl;
                if (pmc != NULL) {
                    phy = pmc->phy[0];
                    if (phy != NULL) {
                        legacy = 0 ;
                    }
                }
            }
        }
    }
    return (legacy);
}

/*
   Some phyctrl_diag mode is using uC assist.  Then it doesn't need to use the reporting code
   from the legacy method.
 */
int is_eyescan_algorithm_legacy_rpt_mode(int unit, uint32 nof_ports, soc_port_t* ports, uint32 inst)
{
    /* need to expand more cases  */
    return is_eyescan_algorithm_legacy_mode(unit, nof_ports, ports, inst) ;
}

const phymod_phy_access_t* _get_phymod_phy_access(soc_phymod_ctrl_t *pmc, int phy_idx)
{
    if (!pmc) return (NULL);
    if (!pmc->phy[phy_idx]) return (NULL);
    return(&pmc->phy[phy_idx]->pm_phy);
}

/*
   To find out what is the first lane position (0, 1, ...)
   for a given port.   Retunr the lane postion value or -1.
*/
int first_lane_mask_position(uint32 lane_mask) {
    int lane_pos =0, found=0 ;

    while(lane_mask) {
        if(lane_mask & 0x1) {
            found = 1 ;
            break ;
        }
        lane_pos  +=1 ;
        lane_mask >>= 1 ;
    }
    if(found) { return lane_pos ; }
    else      { return -1 ; }
}

STATIC int
_soc_port_generate_phy_array(
    int                             unit,
    uint32                          inst,       /* phy instance containing phy number and interface */
    uint32                          nof_ports,
    soc_port_t*                     ports,
    int*                            lane_num,   /* array of lane index offset 0, 1, ... */
    int                             lane,       /* secondray lane offset indication */
    int                             check_pmd_lock,
    int                             phy_acc_max_length,
    phymod_phy_access_t*            phy_acc,    /* out */
    int*                            port_ids,   /* out */
    int*                            line_rates, /* out, optional */
    int*                            phy_acc_length /* out */)

{
    int                 phy_acc_count, j, idx;
    int                 local_lane_num, rv;
    soc_phymod_ctrl_t   *pmc = NULL;
    phy_ctrl_t          *pc;
    const phymod_phy_access_t *pa;
    uint32 locked;
    int    lane_pos=0;
    int    is_legacy = 0;
    int pre_lane_num = -1;
    int pre_phy_addr = -1;

    /* Generate phys array */
    phy_acc_count = 0;
    for(j = 0 ; j < nof_ports ; j++) {

        if(!SOC_PBMP_MEMBER(PBMP_PORT_ALL(unit), ports[j])){
            LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit, "Port %d can't run eyescan \n"), ports[j]));
            continue;
        }
        if ((j == 0) && (PHY_DIAG_INST_DEV(inst) == PHY_DIAG_DEV_EXT)) {
            if ((ext_phy_ctrl[unit]) && (pc = EXT_PHY_SW_STATE(unit, ports[0])) && (pc->pd)) {
                is_legacy = 1;
            }
        }

        if ((SOC_USE_PORTCTRL(unit)) && !is_legacy) {
#ifdef PORTMOD_SUPPORT
    uint32  saved_lane_mask[3];
    int     i, pm8x50 = 0, pm8x50_fabric = 0;
            portmod_access_get_params_t params;
            phymod_phy_access_t access[PHYMOD_CONFIG_MAX_CORES_PER_PORT];
            int nof_phys, dev, intf;
            portmod_port_interface_config_t if_config;
            portmod_speed_config_t speed_config;
            portmod_dispatch_type_t pm_type = portmodDispatchTypeCount;

            SOC_IF_ERROR_RETURN(portmod_port_pm_type_get(unit, ports[j], &ports[j], &pm_type));

            intf = PHY_DIAG_INTF_DFLT;
            SOC_IF_ERROR_RETURN(portmod_access_get_params_t_init(unit, &params));

            params.lane = lane_num ? lane_num[j] : lane;
            params.phyn = 0; /* internal phy */
            params.apply_lane_mask = 1;

            dev = PHY_DIAG_INST_DEV(inst);
            if (dev == PHY_DIAG_DEV_EXT) {
                params.phyn = 1; /* external phy */
                intf = PHY_DIAG_INST_INTF(inst);
                if(intf == PHY_DIAG_INTF_SYS) {
                    params.sys_side = PORTMOD_SIDE_SYSTEM;
                } else {
                    params.sys_side = PORTMOD_SIDE_LINE;
                }
            }

            /* it picks one lane or all lane mask based on params.lane */
            rv = portmod_port_phy_lane_access_get(unit, ports[j], &params, PHYMOD_CONFIG_MAX_CORES_PER_PORT,
                                                  access, &nof_phys, NULL);
            SOC_IF_ERROR_RETURN(rv);

            /* 4 QSGMII ports belong to 1 PMD lane.
             * Skip ports for the same lane in the same core, or else eyescan will fail.
             */
            if (IS_QSGMII_PORT(unit, ports[j]) && (nof_phys == 1)) {
                int current_lane_num = 0, mask;

                mask = access[0].access.lane_mask;

                while (mask) {
                    if (mask & 0xf) {
                        break;
                    }
                    mask = mask >> 4;
                    current_lane_num ++;
                }

                /* Save phy lane number and address of current port.
                 * If the lane number and address are same as previous port, skip this port. The eyescan of this lane has been running.
                 */
                if ((current_lane_num == pre_lane_num) && (access[0].access.addr == pre_phy_addr)) {
                    /* skip port for the same lane in the same core */
                    continue;
                } else {
                    /* save the lane number and core address */
                    pre_lane_num = current_lane_num;
                    pre_phy_addr = access[0].access.addr;
                }
            }

            
            if ( nof_phys == 3 ){
                for(i=0; i < nof_phys; i++) {
                    saved_lane_mask[i] = access[i].access.lane_mask;
                    access[i].access.lane_mask = 0; /* need this to do compare. */
                }

                if( !sal_memcmp( &access[0],&access[1], sizeof(phymod_phy_access_t)) &&
                    !sal_memcmp( &access[0],&access[2], sizeof(phymod_phy_access_t))){
                    access[0].access.lane_mask = ( saved_lane_mask[0]| saved_lane_mask[1]| saved_lane_mask[2]);
                    phymod_access_t_init(&access[1].access);
                    phymod_access_t_init(&access[2].access);
                    /* cli_out("Consolidating entries.\n");*/
                    nof_phys = 1;
                } else {
                    /* restore lane mask */
                    for(i=0; i < nof_phys; i++) {
                        access[i].access.lane_mask = saved_lane_mask[i];
                    }
                }
            }

            for(idx = 0 ; idx < nof_phys ; idx++) {
                pa = &(access[idx]);

                if(check_pmd_lock) {
                    rv = phymod_phy_rx_pmd_locked_get(pa, &locked);
                    SOC_IF_ERROR_RETURN(rv);

                    if(!locked) {
                        if(nof_phys > 1) {
                            LOG_WARN(BSL_LS_SOC_PHY,
                                     (BSL_META_U(unit, "p=%d phy idx=%0d is skipped, phy rx isn't locked \n"), ports[j], idx));
                        } else {
                            LOG_WARN(BSL_LS_SOC_PHY,
                                     (BSL_META_U(unit, "Port %d is skipped, Phy RX isn't locked \n"), ports[j]));
                        }
                        continue;
                    }
                }

                if(phy_acc_count >= phy_acc_max_length) {
                    LOG_ERROR(BSL_LS_SOC_PHY, (BSL_META_U(unit,
                                          "ERROR: No space left for phy_acc\n")));
                    return SOC_E_INTERNAL;
                }
#ifdef PORTMOD_PM8X50_SUPPORT
                if(pm_type == portmodDispatchTypePm8x50) {
                    pm8x50 = 1;
                }
#endif
#ifdef PORTMOD_PM8X50_FABRIC_SUPPORT
                if(pm_type == portmodDispatchTypePm8x50_fabric) {
                    pm8x50_fabric = 1;
                }
#endif

                if (line_rates) {
                    if(pm8x50 == 1) {
                        rv = portmod_port_speed_get(unit, ports[j], &line_rates[phy_acc_count]);
                        SOC_IF_ERROR_RETURN(rv);
                    } else if (pm8x50_fabric == 1) {
                        rv = portmod_port_speed_config_get(unit, ports[j], &speed_config);
                        SOC_IF_ERROR_RETURN(rv);
                        line_rates[phy_acc_count] = speed_config.speed;
                    }else  {
                        rv = portmod_port_interface_config_get(unit, ports[j], &if_config, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY);
                        SOC_IF_ERROR_RETURN(rv);
                        line_rates[phy_acc_count] = if_config.speed;
                    }
                }

                sal_memcpy(&(phy_acc[phy_acc_count]), pa, sizeof(phymod_phy_access_t));
                port_ids[phy_acc_count] = ports[j] ;
                phy_acc_count++;
            }

#endif /*PORTMOD_SUPPORT*/
        } else {
            int intf = PHY_DIAG_INTF_DFLT;
            int dev = PHY_DIAG_INST_DEV(inst);
            if (dev == PHY_DIAG_DEV_EXT) {
                pc = EXT_PHY_SW_STATE(unit, ports[j]);
            } else {
                pc = INT_PHY_SW_STATE(unit, ports[j]);
            }

            if (pc == NULL) {
                LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit, "Err: port %d, pc is NULL \n"), ports[j]));
                continue;
            }

            if (dev == PHY_DIAG_DEV_EXT) {
                intf = PHY_DIAG_INST_INTF(inst);
                if (intf == PHY_DIAG_INTF_SYS) {
                    pc->flags |= PHYCTRL_SYS_SIDE_CTRL;
                } else {
                    pc->flags &= ~PHYCTRL_SYS_SIDE_CTRL;
                }
            }

            pmc = &pc->phymod_ctrl;
            if (pmc == NULL) {
                LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit, "Err: port %d, pmc is NULL \n"), ports[j]));
                continue;
            }

            /* 4 QSGMII ports belong to 1 PMD lane.
             * Skip ports for the same lane in the same core, or else eyescan will fail.
             */
            if (IS_QSGMII_PORT(unit, ports[j]) && (dev == PHY_DIAG_DEV_INT)) {
                int current_lane_num = 0;

                current_lane_num = pc->lane_num/4;

                /* Save phy lane number and address of current port.
                 * If the lane number and address are same as previous port, skip this port. The eyescan of this lane has been running.
                 */
                if ((current_lane_num == pre_lane_num) && (pc->phy_id == pre_phy_addr)) {
                    /* skip port for the same lane in the same core */
                    continue;
                } else {
                    /* save the lane number and core address */
                    pre_lane_num = current_lane_num;
                    pre_phy_addr = pc->phy_id;
                }
            }

            local_lane_num = pc->lane_num;  /* pc->lane_num used to indicate which lane to focus on */
            if (lane_num != NULL) {         /* in the leagcy driver                                 */
                pc->lane_num = lane_num[j];
            }
#ifdef INCLUDE_FCMAP
            else  if (pc->fcmap_enable && (intf == PHY_DIAG_INTF_LINE)) {
                    pc->lane_num = 0;
            }
#endif

            for (idx = 0; idx < pmc->num_phys; idx++) {
                pa = _get_phymod_phy_access(pmc, idx);
                if (!pa) {
                    if(pmc->num_phys>1) {
                        LOG_ERROR(BSL_LS_SOC_PHY,
                              (BSL_META_U(unit,
                                          "ERROR: p=%0d phymod idx=%0d access is NULL\n"), ports[j], idx));
                    } else {
                        LOG_ERROR(BSL_LS_SOC_PHY,
                                  (BSL_META_U(unit,
                                          "ERROR: phymod Access on port%d is NULL\n"), ports[j]));
                    }
                    continue;
                }

                if(check_pmd_lock) {
                    rv = phymod_phy_rx_pmd_locked_get(pa, &locked);
                    SOC_IF_ERROR_RETURN(rv);

                    if(!locked) {
                        if(pmc->num_phys>1) {
                            LOG_WARN(BSL_LS_SOC_PHY,
                                     (BSL_META_U(unit, "p=%d phy idx=%0d is skipped, phy rx isn't locked \n"), ports[j], idx));
                        } else {
                            LOG_WARN(BSL_LS_SOC_PHY,
                                     (BSL_META_U(unit, "Port %d is skipped, Phy RX isn't locked \n"), ports[j]));
                        }
                        continue;
                    }
                }

                if(phy_acc_count >= phy_acc_max_length) {
                    LOG_ERROR(BSL_LS_SOC_PHY, (BSL_META_U(unit,
                                          "ERROR: No space left for phy_acc\n")));
                    pc->lane_num = local_lane_num;
                    return SOC_E_INTERNAL;
                }

                if(line_rates) {
                    rv = soc_phyctrl_speed_get(unit, ports[j], &(line_rates[phy_acc_count]));
                    if(SOC_FAILURE(rv)) {
                        LOG_ERROR(BSL_LS_SOC_PHY, (BSL_META_U(unit,
                                          "ERROR: Failed to retrive speed for port %d \n"), ports[j]));
                        pc->lane_num = local_lane_num;
                        return SOC_E_INTERNAL;
                    }
                }
                port_ids[phy_acc_count] = ports[j] ;
                sal_memcpy(&(phy_acc[phy_acc_count]), pa, sizeof(phymod_phy_access_t));
                /* fix the lane mask by lane_num */
                if(lane_num != NULL) {
                    lane_pos = first_lane_mask_position(phy_acc[phy_acc_count].access.lane_mask) ;
                    if(lane_pos>=0) {
                        phy_acc[phy_acc_count].access.lane_mask &= (1<< (lane_num[j]+lane_pos)) ;
                    }
                    /* lane_mask exception is handled later in the functions call this code */
                }
#ifdef INCLUDE_FCMAP
                else  if (pc->fcmap_enable && (intf == PHY_DIAG_INTF_LINE)) {
                    lane_pos = first_lane_mask_position(phy_acc[phy_acc_count].access.lane_mask) ;
                    if(lane_pos>=0) {
                        phy_acc[phy_acc_count].access.lane_mask &= (1<< lane_pos) ;
                    }
                }
#endif
                phy_acc_count++;
            }
            pc->lane_num = local_lane_num;
        }
    }

    *phy_acc_length = phy_acc_count;
    return SOC_E_NONE;
}

int _soc_port_phy_eyescan_run_lowber(
    int                             unit,
    uint32                          inst,    /* phy instance containing phy number and interface */
    int                             flags,
    soc_port_phy_eyescan_params_t*  params,
    uint32                          nof_ports,
    soc_port_t*                     ports,
    int*                            lane_num,
    soc_port_phy_eyescan_results_t* results)    /*array of result per port*/
{
    phymod_phy_access_t* phy_acc = NULL;
    phymod_phy_eyescan_options_t eyescan_options;
    int rv = SOC_E_NONE;
    int* line_rates;
    int* port_ids;
    int array_max_size, phy_acc_length;
    int i;
    /*validate input*/
    if (params == NULL || ports == NULL || results == NULL) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "Null parameter\n")));
        return SOC_E_PARAM;
    }

    if(params->counter >= socPortPhyEyescanNofCounters || params->counter < 0) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "Counter %d isn't supported\n"), params->counter));
        return SOC_E_PARAM;
    }


    /* default_sample_time = params->sample_time; */
    eyescan_options.timeout_in_milliseconds = params->sample_time;
    eyescan_options.horz_max = params->bounds.horizontal_max;
    eyescan_options.horz_min = params->bounds.horizontal_min;
    eyescan_options.hstep = params->sample_resolution;
    eyescan_options.vert_max = params->bounds.vertical_max;
    eyescan_options.vert_min = params->bounds.vertical_min;
    eyescan_options.vstep = params->sample_resolution;
    eyescan_options.mode = (params->type == 3)? 1 << 2: 0; /*Mode should be DIAG_BER_INTR for type3*/
    /* following param are used in BER projection */
    eyescan_options.ber_proj_scan_mode = params->ber_proj_scan_mode;
    eyescan_options.ber_proj_timer_cnt = params->ber_proj_timer_cnt;
    eyescan_options.ber_proj_err_cnt = params->ber_proj_err_cnt;

    /* Allocate Mem */
    array_max_size = nof_ports*PHYMOD_CONFIG_MAX_CORES_PER_PORT;
    phy_acc = (phymod_phy_access_t*) sal_alloc(sizeof(phymod_phy_access_t)*array_max_size, "eyescan phymod_phy_access_t");
    line_rates = (int*)sal_alloc(sizeof(int)*array_max_size, "eyescan line rates");
    port_ids   = (int*)sal_alloc(sizeof(int)*array_max_size, "eyescan port ids");
    if(NULL == phy_acc || NULL == line_rates || NULL == port_ids) {
        LOG_ERROR(BSL_LS_SOC_PHY, (BSL_META_U(unit,
                "Failed to allocate Memory phy_acc=%d line_rates=%d\n"), phy_acc?1:0, line_rates?1:0));
        rv = SOC_E_MEMORY;
        goto exit;
    }

    sal_memset(phy_acc, 0, sizeof(phymod_phy_access_t)*array_max_size);

    {
        /* Generate phys array */
        rv = _soc_port_generate_phy_array(unit,
                                     inst,
                                     nof_ports,
                                     ports,
                                     lane_num,
                                     -1,
                                     1,
                                     array_max_size,
                                     phy_acc,
                                     port_ids,
                                     line_rates,
                                     &phy_acc_length);
    }
    if(SOC_FAILURE(rv)) {
        goto exit;
    }

    for(i=0; i < phy_acc_length; i++) {
        if (phy_acc[i].access.lane_mask == 0) {
            LOG_ERROR(BSL_LS_SOC_PHY, (BSL_META_U(unit, "Illegal lane_mask of 0 on port %d, aborted.\n"), port_ids[i]));
            goto exit;
        }
    }

    /*enable eyescan*/
    if(params->type == 4) {
        rv = phymod_diag_eyescan_run(phy_acc, unit, port_ids, line_rates, phy_acc_length, phymodEyescanModeBERProj, &eyescan_options);
    } else if(params->type >4) {
        LOG_ERROR(BSL_LS_SOC_PHY, (BSL_META_U(unit,
                  "Eyescan type %d is not supported (type should be <= 4)\n"), params->type));
        goto exit;
    } else {
        rv = phymod_diag_eyescan_run(phy_acc, unit, port_ids, line_rates, phy_acc_length, phymodEyescanModeLowBER, &eyescan_options);
    }


exit:
    if(phy_acc != NULL) {
        sal_free(phy_acc);
    }
    if(line_rates != NULL) {
        sal_free(line_rates);
    }
    if(port_ids != NULL) {
        sal_free(port_ids);
    }
    return rv;
}

/* no eyescan params requred */
int _soc_port_phy_eyescan_run_fast_uc(
    int                             unit,
    uint32                          inst,    /* phy instance containing phy number and interface */
    int                             flags,
    soc_port_phy_eyescan_params_t*  params,
    uint32                          nof_ports,
    soc_port_t*                     ports,
    int*                            lane_num,
    soc_port_phy_eyescan_results_t* results)    /*array of result per port*/
{
    phymod_phy_access_t *phy_acc;

    int                 phy_acc_length;
    int                 rv = SOC_E_NONE;
    int i;
    int *port_ids ;

    /*validate input*/
    if (!SOC_UNIT_VALID(unit)) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "Invalid unit\n")));
        return SOC_E_UNIT;
    }

    if (params == NULL || ports == NULL || results == NULL) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "Null parameter\n")));
        return SOC_E_PARAM;
    }

    /* Allocate Mem */
    phy_acc = (phymod_phy_access_t*) sal_alloc(sizeof(phymod_phy_access_t)*nof_ports*PHYMOD_CONFIG_MAX_CORES_PER_PORT, "eyescan phymod_phy_access_t");
    if(NULL == phy_acc) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "Failed to allocate Memory for phy_acc\n")));
        return SOC_E_MEMORY;
    }
    port_ids = (int*)sal_alloc(sizeof(int)*nof_ports*PHYMOD_CONFIG_MAX_CORES_PER_PORT, "eyescan port ids");
    if(NULL == port_ids) {
        sal_free(phy_acc);
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "Failed to allocate Memory for port_ids.\n")));
        return SOC_E_MEMORY;
    }
    sal_memset(phy_acc, 0, sizeof(phymod_phy_access_t)*nof_ports*PHYMOD_CONFIG_MAX_CORES_PER_PORT);
        /* Generate phys array */
#if defined(BCM_MONTEREY_SUPPORT)
    if ((IS_ROE_PORT(unit, ports[0])) && SOC_IS_MONTEREY(unit)) {
        rv = _soc_port_generate_phy_array(unit,
                                     inst,
                                     nof_ports,
                                     ports,
                                     lane_num,
                                     /* lane, */
                                     -1,
                                     0,
                                     nof_ports*PHYMOD_CONFIG_MAX_CORES_PER_PORT,
                                     phy_acc,
                                     port_ids,
                                     NULL,
                                     &phy_acc_length);
    } else
#endif
    {
        rv = _soc_port_generate_phy_array(unit,
                                     inst,
                                     nof_ports,
                                     ports,
                                     lane_num,
                                     /* lane, */
                                     -1,
                                     1,
                                     nof_ports*PHYMOD_CONFIG_MAX_CORES_PER_PORT,
                                     phy_acc,
                                     port_ids,
                                     NULL,
                                     &phy_acc_length);
    }
    if(SOC_FAILURE(rv)) {
        goto exit;
    }

    for(i=0; i < phy_acc_length; i++ ) {
        if ( phy_acc[i].access.lane_mask == 0 ) {
            LOG_ERROR(BSL_LS_SOC_PHY, (BSL_META_U(unit, "p=%0d lane_mask=0 causes to abort.\n"), port_ids[i]));
            goto exit;
        }
    }

    /* Run fast eyescan: no eyescan options required */
    rv = phymod_diag_eyescan_run(phy_acc, unit, port_ids, NULL, phy_acc_length, phymodEyescanModeFast, NULL);
exit:
    if(phy_acc != NULL) {
        sal_free(phy_acc);
    }
    if(port_ids != NULL) {
        sal_free(port_ids);
    }
    return rv;
}
#endif


/* Please consult thephymod team firt before modify this function or functions called inside */
/* this function.  each port group in the function argument would be required to expect the  */
/* same phy_diag call mode or phymod call mode.                                              */

int soc_port_phy_eyescan_run (
    int unit,
    uint32 inst,            /* phy instance containing phy number and interface */
    int flags,
    soc_port_phy_eyescan_params_t* params,
    uint32 nof_ports,
    soc_port_t* ports,
    int* lane_num,
    soc_port_phy_eyescan_results_t* results /*array of result per port*/)
{
#ifdef PHYMOD_SUPPORT

    if (is_eyescan_algorithm_legacy_mode(unit, nof_ports, ports,inst)) {
        return _soc_port_phy_eyescan_run(unit, inst, flags, params, nof_ports,
                                       ports, lane_num, results);
    } else {
        int rv, i;
        int *saved_intf = NULL;
        phy_ctrl_t *pc = NULL;
        int is_legacy = 0;

        if (PHY_DIAG_INST_DEV(inst) == PHY_DIAG_DEV_EXT) {
            if ((ext_phy_ctrl[unit]) && (pc = EXT_PHY_SW_STATE(unit, ports[0])) && (pc->pd)) {
                is_legacy = 1;
            }
        }

        if ((!SOC_USE_PORTCTRL(unit)) || (is_legacy)) {
        /* First, Save the PhyCtrl SYS/LINE Flags */
        saved_intf = sal_alloc((nof_ports) * sizeof(int), "eyescan saved intf");
        if (saved_intf != NULL) {
            for(i=0; i<nof_ports; i++ ) {
                int dev = PHY_DIAG_INST_DEV(inst);
                if (dev == PHY_DIAG_DEV_EXT) {
                    pc = EXT_PHY_SW_STATE(unit, ports[i]);
                } else {
                    pc = INT_PHY_SW_STATE(unit, ports[i]);
                }
                if(pc == NULL) {
                  LOG_ERROR(BSL_LS_SOC_PHY, (BSL_META_U(unit, "Can not find port INT/EXT phy\n")));
                  sal_free(saved_intf);
                  return SOC_E_INTERNAL;
                }
                saved_intf[i] = (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? 1 : 0;
            }
        } else {
            cli_out("Failed to allocate for saved_intf. Just using teh default");
        }
        }
        if (params->type == 1) {
            rv = _soc_port_phy_eyescan_run_fast_uc(
                                unit, inst, flags, params, nof_ports,
                                ports, lane_num, results);
        } else {
            rv = _soc_port_phy_eyescan_run_lowber(
                                unit, inst, flags, params, nof_ports,
                                ports, lane_num, results);
        }

        if ((!SOC_USE_PORTCTRL(unit)) || (is_legacy)) {
            /* Restore the PhyCtrl SYS/LINE Flags */
            if (saved_intf != NULL) {
                for(i=0; i<nof_ports; i++ ) {
                    int dev = PHY_DIAG_INST_DEV(inst);
                    if (dev == PHY_DIAG_DEV_EXT) {
                        pc = EXT_PHY_SW_STATE(unit, ports[i]);
                    } else {
                        pc = INT_PHY_SW_STATE(unit, ports[i]);
                    }
                    if (saved_intf[i]) {
                        pc->flags |= PHYCTRL_SYS_SIDE_CTRL;
                    } else {
                        pc->flags &= ~PHYCTRL_SYS_SIDE_CTRL;
                    }
                }
                sal_free(saved_intf);
            }
        }
        return rv;
    }
#else
    return (_soc_port_phy_eyescan_run(unit, inst, flags, params, nof_ports, ports, lane_num, results));
#endif
}


#ifndef __KERNEL__

#ifdef COMPILER_HAS_DOUBLE
STATIC COMPILER_DOUBLE
_eyescan_util_round_real( COMPILER_DOUBLE original_value, int decimal_places )
{

    COMPILER_DOUBLE shift_digits[EYESCAN_UTIL_MAX_ROUND_DIGITS+1] = { 0.0, 10.0, 100.0, 1000.0,
                          10000.0, 100000.0, 1000000.0, 10000000.0, 100000000.0 };
    COMPILER_DOUBLE shifted;
    COMPILER_DOUBLE rounded;

    if (decimal_places > EYESCAN_UTIL_MAX_ROUND_DIGITS ) {
        LOG_CLI((BSL_META("ERROR: Maximum digits to the right of decimal for rounding "
                          "exceeded. Max %d, requested %d\n"),
                 EYESCAN_UTIL_MAX_ROUND_DIGITS, decimal_places));
        return 0.0;
    }
    /* shift to preserve the desired digits to the right of the decimal */
    shifted = original_value * shift_digits[decimal_places];

    /* convert to integer and back to COMPILER_DOUBLE to truncate undesired precision */
    shifted = (COMPILER_DOUBLE)(floor(shifted+0.5));

    /* shift back to place decimal point correctly */
    rounded = shifted / shift_digits[decimal_places];
    return rounded;
}
#endif /* COMPILER HAS DOUBLE */

#ifdef COMPILER_HAS_DOUBLE
STATIC int
_eye_margin_diagram_cal(int unit, EYE_DIAG_INFOt *pInfo, soc_port_phy_eyescan_results_t* results)
{
    COMPILER_DOUBLE lbers[MAX_EYE_LOOPS]={0}; /*Internal linear scale sqrt(-log(ber))*/
    COMPILER_DOUBLE margins[MAX_EYE_LOOPS]={0}; /* Eye margin @ each measurement*/
    COMPILER_DOUBLE bers[MAX_EYE_LOOPS]={0}; /* computed bit error rate */
    int delta_n;
    COMPILER_DOUBLE Exy = 0.0;
    COMPILER_DOUBLE Eyy = 0.0;
    COMPILER_DOUBLE Exx = 0.0;
    COMPILER_DOUBLE Ey  = 0.0;
    COMPILER_DOUBLE Ex  = 0.0;
    COMPILER_DOUBLE alpha;
    COMPILER_DOUBLE beta;
    COMPILER_DOUBLE proj_ber;
    COMPILER_DOUBLE proj_margin_12;
    COMPILER_DOUBLE proj_margin_15;
    COMPILER_DOUBLE proj_margin_18;
    COMPILER_DOUBLE sq_err2;
    COMPILER_DOUBLE ierr;
    int beta_max=0;
    int ber_conf_scale[20];
    int start_n;
    int stop_n;
    int low_confidence;
    int loop_index;
    COMPILER_DOUBLE outputs[4];
    COMPILER_DOUBLE eye_unit;
    int n_mono;
    int loop_var, hi_confidence_cnt, veye_cnt, veye_cnt_orig ;

    /* Initialize BER confidence scale */
    ber_conf_scale[0] = 3.02;
    ber_conf_scale[1] = 4.7863;
    ber_conf_scale[2] = 3.1623;
    ber_conf_scale[3] = 2.6303;
    ber_conf_scale[4] = 2.2909;
    ber_conf_scale[5] = 2.138;
    ber_conf_scale[6] = 1.9953;
    ber_conf_scale[7] = 1.9055;
    ber_conf_scale[8] = 1.8197;
    ber_conf_scale[9] = 1.7783;
    ber_conf_scale[10] = 1.6982;
    ber_conf_scale[11] = 1.6596;
    ber_conf_scale[12] = 1.6218;
    ber_conf_scale[13] = 1.6218;
    ber_conf_scale[14] = 1.5849;
    ber_conf_scale[15] = 1.5488;
    ber_conf_scale[16] = 1.5488;
    ber_conf_scale[17] = 1.5136;
    ber_conf_scale[18] = 1.5136;
    ber_conf_scale[19] = 1.4791;

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "first_good_ber_idx: %d, first_small_errcnt_idx: %d\n"),
                 pInfo->first_good_ber_idx,pInfo->first_small_errcnt_idx));

    /* re-calculate the range selected for extroplation */
    hi_confidence_cnt = 0;
    veye_cnt          = 0;
    veye_cnt_orig     = pInfo->veye_cnt ;
    for (loop_var = 0; loop_var < pInfo->offset_max; loop_var++) {
        veye_cnt += 1;
        if (pInfo->total_errs[loop_var] >= HI_CONFIDENCE_ERR_CNT) {
            hi_confidence_cnt = hi_confidence_cnt + 1;
        }

        if (((hi_confidence_cnt >= 2) && (pInfo->total_errs[loop_var] <
                                          HI_CONFIDENCE_MIN_ERR_CNT)) ||
            ((hi_confidence_cnt <  2) &&
             (pInfo->total_errs[loop_var] < LO_CONFIDENCE_MIN_ERR_CNT)) ) {
            break;  /* exit for loop */
        }
    }
    pInfo->veye_cnt = veye_cnt ;

    /* Find the highest data point to use, currently based on at least 1E-8 BER level */
    if (pInfo->first_good_ber_idx == INDEX_UNINITIALIZED) {
        start_n = pInfo->veye_cnt;
    } else {
        start_n = pInfo->first_good_ber_idx;
        if(start_n > pInfo->veye_cnt) {
            start_n =  pInfo->veye_cnt;
        }
    }
    stop_n = pInfo->veye_cnt;

    /* Find the lowest data point to use */
    if (pInfo->first_small_errcnt_idx == INDEX_UNINITIALIZED) {
        stop_n = pInfo->veye_cnt;
    } else {
        stop_n = pInfo->first_small_errcnt_idx;
        if(stop_n > pInfo->veye_cnt) {
            stop_n = pInfo->veye_cnt;
        }
    }

    /* determine the number of non-monotonic outliers */
    n_mono = 0;
    for (loop_index = start_n; loop_index < stop_n; loop_index++) {
        if (pInfo->mono_flags[loop_index] == 1) {
            n_mono = n_mono + 1;
        }
    }

    eye_unit = VEYE_UNIT;

    for (loop_index = 0; loop_index < pInfo->veye_cnt; loop_index++) {
        if (pInfo->total_errs[loop_index] == 0) {
            bers[loop_index] = (1000.0/(COMPILER_DOUBLE)pInfo->total_elapsed_time[loop_index])/pInfo->rate;
        } else {
            bers[loop_index] = (COMPILER_DOUBLE)(1000.0*(COMPILER_DOUBLE)pInfo->total_errs[loop_index])/
                   (COMPILER_DOUBLE)pInfo->total_elapsed_time[loop_index]/pInfo->rate;
        }
        bers[loop_index] /= 1000;
        margins[loop_index] = (pInfo->offset_max-loop_index)*eye_unit;
    }

    if( start_n >= pInfo->veye_cnt ) {
        outputs[0] = -_eyescan_util_round_real(log(bers[pInfo->veye_cnt-1])/log(10), 1);
        outputs[1] = -100.0;
        outputs[2] = -100.0;
        outputs[3] = -100.0;
        /*  No need to print out the decimal portion of the BER */
        LOG_VERBOSE(BSL_LS_SOC_PHY,
                    (BSL_META_U(unit,
                                "BER *worse* than 1e-%d\n"),
                     (int)outputs[0]));
        LOG_VERBOSE(BSL_LS_SOC_PHY,
                    (BSL_META_U(unit,
                                "Negative margin @ 1e-12, 1e-15 & 1e-18\n")));
        results->ext_better = -1;
        results->ext_results_int = (int)outputs[0];
        results->ext_results_remainder = 0;
    } else {
        low_confidence = 0;
        if( (stop_n-start_n - n_mono) < 2 ) {
            /* Code triggered when less than 2 statistically valid extrapolation points */
            LOG_CLI((BSL_META_U(unit,
                                "\nWARNING: less accurate extrapolation because of less than 2 statistically valid extrapolation points.\n")));
            for( loop_index = stop_n; loop_index < pInfo->veye_cnt; loop_index++ ) {
                if( pInfo->total_errs[loop_index] < 20 ) {
                    bers[loop_index] = ber_conf_scale[pInfo->total_errs[loop_index]] * bers[loop_index];
                } else {
                    bers[loop_index] = ber_conf_scale[19] * bers[loop_index];
                }
                pInfo->mono_flags[loop_index] = 0;    /* remove flags; or assess again */
            }
            /* Add artificial point at 100% margin to enable interpolation */
            margins[pInfo->veye_cnt] = 100.0;
            bers[pInfo->veye_cnt] = 0.1;
            low_confidence = 1;
            stop_n = pInfo->veye_cnt + 1;
        }

        /* Below this point the code assumes statistically valid point available */
        delta_n = stop_n - start_n - n_mono;

        LOG_VERBOSE(BSL_LS_SOC_PHY,
                    (BSL_META_U(unit,
                                "start_n: %d, stop_n: %d, veye: %d, n_mono: %d\n"),
                     start_n,stop_n,pInfo->veye_cnt,n_mono));

        /* Find all the correlations */
        for (loop_index = start_n; loop_index < stop_n; loop_index++) {
            lbers[loop_index] = (COMPILER_DOUBLE)sqrt(-log(bers[loop_index]));
        }

        LOG_VERBOSE(BSL_LS_SOC_PHY,
                    (BSL_META_U(unit,
                                "\tstart=%d, stop=%d, low_confidence=%d\n"),
                     start_n, stop_n, low_confidence));
        for (loop_index = start_n; loop_index < stop_n; loop_index++){
            LOG_VERBOSE(BSL_LS_SOC_PHY,
                        (BSL_META_U(unit,
                                    "\ttotal_errs[%d]=0x%08x\n"),
                         loop_index,(int)pInfo->total_errs[loop_index]));
            LOG_VERBOSE(BSL_LS_SOC_PHY,
                        (BSL_META_U(unit,
                                    "\tbers[%d]=%f\n"),
                         loop_index,bers[loop_index]));
            LOG_VERBOSE(BSL_LS_SOC_PHY,
                        (BSL_META_U(unit,
                                    "\tlbers[%d]=%f\n"),
                         loop_index,lbers[loop_index]));
        }

        LOG_VERBOSE(BSL_LS_SOC_PHY,
                    (BSL_META_U(unit,
                                "delta_n = %d\n"),
                     delta_n));

        for( loop_index = start_n; loop_index < stop_n; loop_index++ ) {
            if (pInfo->mono_flags[loop_index] == 0) {
                Exy = Exy + margins[loop_index] * lbers[loop_index]/(COMPILER_DOUBLE)delta_n;
                Eyy = Eyy + lbers[loop_index]*lbers[loop_index]/(COMPILER_DOUBLE)delta_n;
                Exx = Exx + margins[loop_index]*margins[loop_index]/(COMPILER_DOUBLE)delta_n;
                Ey  = Ey + lbers[loop_index]/(COMPILER_DOUBLE)delta_n;
                Ex  = Ex + margins[loop_index]/(COMPILER_DOUBLE)delta_n;
            }
        }

        /* Compute fit slope and offset */
        alpha = (Exy - Ey * Ex)/(Exx - Ex * Ex);
        beta = Ey - Ex * alpha;

        LOG_VERBOSE(BSL_LS_SOC_PHY,
                    (BSL_META_U(unit,
                                "Exy=%f, Eyy=%f, Exx=%f, Ey=%f,Ex=%f alpha=%f, beta=%f\n"),
                     Exy,Eyy,Exx,Ey,Ex,alpha,beta));

        /* JPA> Due to the limit of floats, I need to test for a maximum Beta of 9.32 */
        if(beta > 9.32){
            beta_max=1;
            LOG_CLI((BSL_META_U(unit,
                                "\n\tWARNING: intermediate float variable is maxed out, what this means is:\n")));
            LOG_CLI((BSL_META_U(unit,
                                "\t\t- The *extrapolated* minimum BER will be reported as 1E-37.\n")));
            LOG_CLI((BSL_META_U(unit,
                                "\t\t- This may occur if the channel is near ideal (e.g. test loopback)\n")));
            LOG_CLI((BSL_META_U(unit,
                                "\t\t- While not discrete, reporting an extrapolated BER < 1E-37 is numerically corect, and informative\n\n")));
        }


        proj_ber = exp(-beta * beta);
        proj_margin_12 = (sqrt(-log(1e-12)) - beta)/alpha;
        proj_margin_15 = (sqrt(-log(1e-15)) - beta)/alpha;
        proj_margin_18 = (sqrt(-log(1e-18)) - beta)/alpha;

        sq_err2 = 0;
        for (loop_index = start_n; loop_index < stop_n; loop_index++) {
            ierr = (lbers[loop_index] - (alpha*margins[loop_index] + beta));
            sq_err2 = sq_err2 + ierr*ierr/(COMPILER_DOUBLE)delta_n;
        }

        outputs[0] = -_eyescan_util_round_real(log(proj_ber)/log(10),1);
        outputs[1] = _eyescan_util_round_real(proj_margin_18,1);
        outputs[2] = _eyescan_util_round_real(proj_margin_12,1);
        outputs[3] = _eyescan_util_round_real(proj_margin_15,1);

        LOG_VERBOSE(BSL_LS_SOC_PHY,
                    (BSL_META_U(unit,
                                "\t\tlog1e-12=%f, sq=%f\n"),
                     (COMPILER_DOUBLE)log(1e-12),(COMPILER_DOUBLE)sqrt(-log(1e-12))));
        LOG_VERBOSE(BSL_LS_SOC_PHY,
                    (BSL_META_U(unit,
                                "\t\talpha=%f\n"),
                     alpha));
        LOG_VERBOSE(BSL_LS_SOC_PHY,
                    (BSL_META_U(unit,
                                "\t\tbeta=%f\n"),
                     beta));
        LOG_VERBOSE(BSL_LS_SOC_PHY,
                    (BSL_META_U(unit,
                                "\t\tproj_ber=%f\n"),
                     proj_ber));
        LOG_VERBOSE(BSL_LS_SOC_PHY,
                    (BSL_META_U(unit,
                                "\t\tproj_margin12=%f\n"),
                     proj_margin_12));
        LOG_VERBOSE(BSL_LS_SOC_PHY,
                    (BSL_META_U(unit,
                                "\t\tproj_margin12=%f\n"),
                     proj_margin_15));
        LOG_VERBOSE(BSL_LS_SOC_PHY,
                    (BSL_META_U(unit,
                                "\t\tproj_margin18=%f\n"),
                     proj_margin_18));
        LOG_VERBOSE(BSL_LS_SOC_PHY,
                    (BSL_META_U(unit,
                                "\t\toutputs[0]=%f\n"),
                     outputs[0]));
        LOG_VERBOSE(BSL_LS_SOC_PHY,
                    (BSL_META_U(unit,
                                "\t\toutputs[1]=%f\n"),
                     outputs[1]));
        LOG_VERBOSE(BSL_LS_SOC_PHY,
                    (BSL_META_U(unit,
                                "\t\toutputs[2]=%f\n"),
                     outputs[2]));

        /* Extrapolated results, low confidence */
        if( low_confidence == 1 ) {
            if(beta_max){
                LOG_VERBOSE(BSL_LS_SOC_PHY,
                            (BSL_META_U(unit,
                                        "BER(extrapolated) is *better* than 1e-37\n")));
                LOG_VERBOSE(BSL_LS_SOC_PHY,
                            (BSL_META_U(unit,
                                        "Margin @ 1e-12    is *better* than %f\n"),
                             outputs[2]));
                LOG_VERBOSE(BSL_LS_SOC_PHY,
                            (BSL_META_U(unit,
                                        "Margin @ 1e-15    is *better* than %f\n"),
                             outputs[3]));
                LOG_VERBOSE(BSL_LS_SOC_PHY,
                            (BSL_META_U(unit,
                                        "Margin @ 1e-18    is *better* than %f\n"),
                             outputs[1]));
                results->ext_better = 1;
                results->ext_results_int = 37;
                results->ext_results_remainder = 0;
            }
            else{
                LOG_VERBOSE(BSL_LS_SOC_PHY,
                            (BSL_META_U(unit,
                                        "BER(extrapolated) is *better* than 1e-%f\n"),
                             outputs[0]));
                LOG_VERBOSE(BSL_LS_SOC_PHY,
                            (BSL_META_U(unit,
                                        "Margin @ 1e-12    is *better* than %f\n"),
                             outputs[2]));
                LOG_VERBOSE(BSL_LS_SOC_PHY,
                            (BSL_META_U(unit,
                                        "Margin @ 1e-15    is *better* than %f\n"),
                             outputs[3]));
                LOG_VERBOSE(BSL_LS_SOC_PHY,
                            (BSL_META_U(unit,
                                        "Margin @ 1e-18    is *better* than %f\n"),
                             outputs[1]));
                results->ext_better = 1;
                results->ext_results_int = (int)outputs[0];
                results->ext_results_remainder = (((int)(outputs[0]*100))%100);
            }

        /* JPA> Extrapolated results, high confidence */
        } else {
            if(beta_max){
                LOG_VERBOSE(BSL_LS_SOC_PHY,
                            (BSL_META_U(unit,
                                        "BER(extrapolated) = 1e-37\n")));
                LOG_VERBOSE(BSL_LS_SOC_PHY,
                            (BSL_META_U(unit,
                                        "Margin @ 1e-12    is *better* than %f\n"),
                             outputs[2]));
                LOG_VERBOSE(BSL_LS_SOC_PHY,
                            (BSL_META_U(unit,
                                        "Margin @ 1e-15    is *better* than %f\n"),
                             outputs[3]));
                LOG_VERBOSE(BSL_LS_SOC_PHY,
                            (BSL_META_U(unit,
                                        "Margin @ 1e-18    is *better* than %f\n"),
                             outputs[1]));
                results->ext_better = 0;
                results->ext_results_int = 37;
                results->ext_results_remainder = 0;
            }
            else{
                LOG_VERBOSE(BSL_LS_SOC_PHY,
                            (BSL_META_U(unit,
                                        "BER(extrapolated) = 1e-%4.2f\n"),
                             outputs[0]));
                LOG_VERBOSE(BSL_LS_SOC_PHY,
                            (BSL_META_U(unit,
                                        "Margin @ 1e-12    = %4.2f%%\n"),
                             outputs[2]));
                LOG_VERBOSE(BSL_LS_SOC_PHY,
                            (BSL_META_U(unit,
                                        "Margin @ 1e-15    = %4.2f%%\n"),
                             outputs[3]));
                LOG_VERBOSE(BSL_LS_SOC_PHY,
                            (BSL_META_U(unit,
                                        "Margin @ 1e-18    = %4.2f%%\n"),
                             outputs[1]));
                results->ext_better = 0;
                results->ext_results_int = (int)outputs[0];
                results->ext_results_remainder = (((int)(outputs[0]*100))%100);
            }
        }
    }
    pInfo->veye_cnt = veye_cnt_orig ;
    return SOC_E_NONE;
}
#else
STATIC int
_eye_margin_diagram_cal(int unit, EYE_DIAG_INFOt *pInfo, soc_port_phy_eyescan_results_t* results)
{
    LOG_ERROR(BSL_LS_SOC_PHY,
              (BSL_META_U(unit,
                          "_eye_margin_diagram_cal unavailable with this compiler\n")));

    return SOC_E_UNAVAIL;
}
#endif

#ifdef COMPILER_HAS_DOUBLE
STATIC int
_eye_margin_ber_cal(int unit, EYE_DIAG_INFOt *pInfo)
{
    COMPILER_DOUBLE bers[MAX_EYE_LOOPS]; /* computed bit error rate */
    COMPILER_DOUBLE margins[MAX_EYE_LOOPS]; /* Eye margin @ each measurement*/
    int loop_var;
    COMPILER_DOUBLE eye_unit;
    COMPILER_DOUBLE curr_ber_log;
    COMPILER_DOUBLE prev_ber_log = 0;
    COMPILER_DOUBLE good_ber_level = -7.8;

    /* Initialize BER array */
    for (loop_var = 0; loop_var < MAX_EYE_LOOPS; loop_var++) {
        bers[loop_var] = 0.0;
    }

    eye_unit = VEYE_UNIT;

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "\nBER measurement at each offset, num_data_points: %d\n"),
                 pInfo->veye_cnt));

    for (loop_var = 0; loop_var < pInfo->veye_cnt; loop_var++) {
        margins[loop_var] = (pInfo->offset_max-loop_var)*eye_unit;
        LOG_VERBOSE(BSL_LS_SOC_PHY,
                    (BSL_META_U(unit,
                                "BER measurement at offset: %f\n"),
                     margins[loop_var]));

        /* Compute BER */
        if (pInfo->total_errs[loop_var] == 0) {
            bers[loop_var] = 1000.0/(COMPILER_DOUBLE)pInfo->total_elapsed_time[loop_var]/pInfo->rate;
            bers[loop_var] /= 1000;

            LOG_VERBOSE(BSL_LS_SOC_PHY,
                        (BSL_META_U(unit,
                                    "BER @ %04f %% = 1e%04f (%d errors in %d miliseconds)\n"),
                         (COMPILER_DOUBLE)((pInfo->offset_max-loop_var)*eye_unit),
                         1.0*(log10(bers[loop_var])),
                         pInfo->total_errs[loop_var],
                         pInfo->total_elapsed_time[loop_var]));
        } else {
            bers[loop_var] = ((COMPILER_DOUBLE)1000.0*((COMPILER_DOUBLE)(pInfo->total_errs[loop_var])))/
                             (COMPILER_DOUBLE)pInfo->total_elapsed_time[loop_var]/pInfo->rate;

            /* the rate unit is KHZ, add -3(log10(1/1000)) for actual display  */
            bers[loop_var] /= 1000;
            LOG_VERBOSE(BSL_LS_SOC_PHY,
                        (BSL_META_U(unit,
                                    "BER @ %2.2f%% = 1e%2.2f (%d errors in %d miliseconds)\n"),
                         (pInfo->offset_max-loop_var)*eye_unit,
                         log10(bers[loop_var]),
                         pInfo->total_errs[loop_var],
                         pInfo->total_elapsed_time[loop_var]));
        }
        curr_ber_log = log10(bers[loop_var]);

        /* Detect and record nonmonotonic data points */
        if ((loop_var > 0) && (curr_ber_log > prev_ber_log)) {
            pInfo->mono_flags[loop_var] = 1;
        }
        prev_ber_log = curr_ber_log;

        LOG_VERBOSE(BSL_LS_SOC_PHY,
                    (BSL_META_U(unit,
                                "cur_be_log %2.2f\n"),
                     curr_ber_log));
        /* find the first data point with good BER */
        if ((curr_ber_log <= good_ber_level) &&
            (pInfo->first_good_ber_idx == INDEX_UNINITIALIZED)) {
            LOG_VERBOSE(BSL_LS_SOC_PHY,
                        (BSL_META_U(unit,
                                    "cur_be_log %2.2f, loop_var %d\n"),
                         curr_ber_log,loop_var));
            pInfo->first_good_ber_idx = loop_var;
        }
        if ((pInfo->total_errs[loop_var] < HI_CONFIDENCE_MIN_ERR_CNT) && (pInfo->first_small_errcnt_idx == INDEX_UNINITIALIZED)) {
            /* find the first data point with small error count */
            pInfo->first_small_errcnt_idx = loop_var;
        }

    }   /* for loop_var */

    return SOC_E_NONE;
}
#endif /* COMPILER_HAS_DOUBLE */

int
soc_port_phy_eyescan_extrapolate(int unit,
                                 int flags,
                                 soc_port_phy_eyescan_params_t* params,
                                 uint32 nof_ports,
                                 soc_port_t* ports,
                                 soc_port_phy_eyescan_results_t* results)
{
#ifdef COMPILER_HAS_DOUBLE
    int i, cnt, vertical_i;
    int rv = SOC_E_NONE, speed;
    EYE_DIAG_INFOt veye_info;

    sal_memset(&veye_info, 0, sizeof(EYE_DIAG_INFOt));
    /* extrapolate */
    if (!(flags & SRD_EYESCAN_FLAG_AVOID_EXTRAPOLATION) && (flags & SRD_EYESCAN_FLAG_VERTICAL_ONLY) &&
        (params->counter!=socPortPhyEyescanCounterRelativePhy)){

        for (i = 0 ; i < nof_ports; i++) {
            rv = soc_phyctrl_speed_get(unit, ports[i], &speed);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "soc_phyctrl_speed_get failed\n")));
                return rv;
            }

            veye_info.first_good_ber_idx = INDEX_UNINITIALIZED;
            veye_info.first_small_errcnt_idx = INDEX_UNINITIALIZED;
            veye_info.offset_max = params->bounds.vertical_max;
            veye_info.rate = speed*1000;

            cnt = 0;
            for (vertical_i = params->bounds.vertical_min ; vertical_i <= params->bounds.vertical_max; vertical_i++){
                if(vertical_i % params->sample_resolution != 0) {
                   continue;
                }
                cnt++;
                if(cnt == MAX_EYE_LOOPS) {
                    break;
                }
            }
            veye_info.veye_cnt = cnt;
            for (vertical_i = params->bounds.vertical_min ; vertical_i <= params->bounds.vertical_max; vertical_i++){
                cnt--;
                if(vertical_i % params->sample_resolution != 0) {
                   continue;
                }
                veye_info.total_errs[cnt] = results[i].error_count[SOC_PORT_PHY_EYESCAN_H_INDEX][vertical_i + SOC_PORT_PHY_EYESCAN_V_INDEX];
                veye_info.total_elapsed_time[cnt] = results[i].run_time[SOC_PORT_PHY_EYESCAN_H_INDEX][vertical_i + SOC_PORT_PHY_EYESCAN_V_INDEX];
                veye_info.mono_flags[cnt] = 0;
            }
            if (veye_info.veye_cnt > MAX_EYE_LOOPS) {
                rv = SOC_E_PARAM;
                LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "ERROR: veye_cnt > MAX_EYE_LOOPS\n")));
                return rv;
            }
            _eye_margin_ber_cal(unit, &veye_info);
            _eye_margin_diagram_cal(unit, &veye_info, &(results[i]));
            results[i].ext_done = 1;
        }
    }

    return rv;
#else
    return SOC_E_NONE;
#endif /* COMPILER_HAS_DOUBLE*/
}

#else   /* #ifndef __KERNEL__ */

int
soc_port_phy_eyescan_extrapolate(int unit,
                                 int flags,
                                 soc_port_phy_eyescan_params_t* params,
                                 uint32 nof_ports,
                                 soc_port_t* ports,
                                 soc_port_phy_eyescan_results_t* results){
    LOG_CLI((BSL_META_U(unit,
                        "\nExtrapolation is not supported in Linux kernel mode\n")));

    return SOC_E_NONE;
}

#endif /* #ifndef __KERNEL__ */
