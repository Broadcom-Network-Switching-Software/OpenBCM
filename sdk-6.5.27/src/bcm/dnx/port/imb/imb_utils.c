/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif /* BCM_DNX_SUPPORT */

#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm/port.h>
#include <soc/portmod/portmod.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_port_imb_access.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_port_imb_types.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/imb/imb_dispatch.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/recovery/generic_state_journal.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

typedef struct imb_portmod_preemption_control_er_journal_s
{
    /*
     * port for which control will be updated
     */
    bcm_port_t port;
    /*
     * portmod type of the port
     */
    portmod_preemption_control_t portmod_type;
} imb_portmod_preemption_control_er_journal_t;

/**
 * \brief - Translate from BCM loopback to Portmod loopback
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - port id
 * \param [in] bcm_loopback - BCM loopback type
 * \param [out] portmod_loopback - Portmod loopback type
 *   
 * \return
 *   int - see _SHR_E_ * 
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_portmod_loopback_from_bcm_loopback_get(
    int unit,
    bcm_port_t port,
    int bcm_loopback,
    portmod_loopback_mode_t * portmod_loopback)
{
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);
    switch (bcm_loopback)
    {
        case BCM_PORT_LOOPBACK_NONE:
            *portmod_loopback = portmodLoopbackCount;
            break;
        case BCM_PORT_LOOPBACK_MAC:
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
            if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_info))
            {
                *portmod_loopback = portmodLoopbackMacAsyncFifo;
            }
            else
            {
                *portmod_loopback = portmodLoopbackMacOuter;
            }
            break;
        }
        case BCM_PORT_LOOPBACK_PHY:
            *portmod_loopback = portmodLoopbackPhyGloopPMD;
            break;
        case BCM_PORT_LOOPBACK_PHY_REMOTE:
            *portmod_loopback = portmodLoopbackPhyRloopPMD;
            break;
        case BCM_PORT_LOOPBACK_MAC_REMOTE:
            *portmod_loopback = portmodLoopbackMacRloop;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                         "BCM loopback type %d did not match any portmod loopback type", bcm_loopback);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Translate from Portmod loopback to BCM loopback
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - port id
 * \param [in] portmod_loopback - Portmod loopback type
 * \param [out] bcm_loopback - BCM loopback type
 *   
 * \return
 *   int - see _SHR_E_ *
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_bcm_loopback_from_portmod_loopback_get(
    int unit,
    bcm_port_t port,
    portmod_loopback_mode_t portmod_loopback,
    int *bcm_loopback)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (portmod_loopback)
    {
        case portmodLoopbackCount:
            *bcm_loopback = BCM_PORT_LOOPBACK_NONE;
            break;
        case portmodLoopbackMacOuter:
        case portmodLoopbackMacAsyncFifo:
            *bcm_loopback = BCM_PORT_LOOPBACK_MAC;
            break;
        case portmodLoopbackPhyGloopPCS:
        case portmodLoopbackPhyGloopPMD:
            *bcm_loopback = BCM_PORT_LOOPBACK_PHY;
            break;
        case portmodLoopbackPhyRloopPMD:
            *bcm_loopback = BCM_PORT_LOOPBACK_PHY_REMOTE;
            break;
        case portmodLoopbackMacRloop:
            *bcm_loopback = BCM_PORT_LOOPBACK_MAC_REMOTE;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                         "Portmod loopback type %d did not match any BCM loopback type", portmod_loopback);
    }
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Translate from BCM autoneg abilities to PORTMOD abilities
 * 
 * See .h file
 */
void
imb_portmod_an_ability_from_bcm_an_ability_get(
    int num_abilities,
    const bcm_port_speed_ability_t * bcm_abilities,
    portmod_port_speed_ability_t * portmod_abilities)
{
    int i;
    for (i = 0; i < num_abilities; ++i)
    {
        portmod_abilities[i].speed = bcm_abilities[i].speed;
        portmod_abilities[i].num_lanes = bcm_abilities[i].resolved_num_lanes;
        portmod_abilities[i].fec_type = (portmod_fec_t) bcm_abilities[i].fec_type;
        portmod_abilities[i].medium = bcm_abilities[i].medium;
        portmod_abilities[i].pause = (portmod_port_phy_pause_t) bcm_abilities[i].pause;
        portmod_abilities[i].channel = (portmod_port_phy_channel_t) bcm_abilities[i].channel;
        portmod_abilities[i].an_mode = (portmod_port_phy_control_autoneg_mode_t) bcm_abilities[i].an_mode;
    }
}

/**
 * \brief - Translate from PORTMOD autoneg abilities to BCM abilities
 * 
 * See .h file
 */
void
imb_bcm_an_ability_from_portmod_an_ability_get(
    int num_abilities,
    portmod_port_speed_ability_t * portmod_abilities,
    bcm_port_speed_ability_t * bcm_abilities)
{
    int i;
    for (i = 0; i < num_abilities; ++i)
    {
        bcm_abilities[i].speed = portmod_abilities[i].speed;
        bcm_abilities[i].resolved_num_lanes = portmod_abilities[i].num_lanes;
        bcm_abilities[i].fec_type = (bcm_port_phy_fec_t) (portmod_abilities[i].fec_type);
        bcm_abilities[i].medium = portmod_abilities[i].medium;
        bcm_abilities[i].pause = (bcm_port_phy_pause_t) (portmod_abilities[i].pause);
        bcm_abilities[i].channel = (bcm_port_phy_channel_t) (portmod_abilities[i].channel);
        bcm_abilities[i].an_mode = (bcm_port_autoneg_mode_t) (portmod_abilities[i].an_mode);
    }
}

/**
 * \brief - Verify input AN abilities.
 *
 * See .h file
 */
int
imb_clu_mgu_port_autoneg_ability_verify(
    int unit,
    bcm_port_t port,
    int num_ability,
    const bcm_port_speed_ability_t * abilities)
{
    int is_ability_valid = 0;
    bcm_port_autoneg_mode_t pre_an_mode = bcmPortAnModeCount;
    uint32 pre_resolved_num_lanes = 0;
    bcm_port_phy_fec_t pre_fec_type = bcmPortPhyFecInvalid;
    bcm_port_medium_t pre_medium = BCM_PORT_MEDIUM_NONE;
    bcm_port_phy_channel_t pre_channel = bcmPortPhyChannelCount;
    bcm_port_phy_pause_t pre_pause = bcmPortPhyPauseCount;
    int nof_lanes;
    int i, ii;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, port, &nof_lanes));

    for (i = 0; i < num_ability; ++i)
    {
        /** Only one AN mode */
        /** Only one medium type */
        /** Only one pause type */
        /** Only one channel type */
        /** FEC should be either enabled or disabled among given abilities */
        if (i > 0)
        {
            if ((pre_an_mode != abilities[i].an_mode) || (pre_channel != abilities[i].channel)
                || (pre_medium != abilities[i].medium) || (pre_pause != abilities[i].pause)
                || (pre_resolved_num_lanes != abilities[i].resolved_num_lanes)
                || ((pre_fec_type != abilities[i].fec_type) && (pre_fec_type == bcmPortPhyFecNone)))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "All autoneg abilities adverted should have the same AN mode, FEC enable status, media type, pause type and channel type.\r\n");
            }
        }
        else
        {
            if (abilities[i].pause > bcmPortPhyPauseSymm)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid pause type %d.\r\n", abilities[i].pause);
            }
            if (abilities[i].resolved_num_lanes != nof_lanes)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "resolved_num_lanes should equals to port's number of lanes.\r\n");
            }
            pre_an_mode = abilities[i].an_mode;
            pre_fec_type = abilities[i].fec_type;
            pre_medium = abilities[i].medium;
            pre_pause = abilities[i].pause;
            pre_channel = abilities[i].channel;
            pre_resolved_num_lanes = abilities[i].resolved_num_lanes;
        }

        /** each of the abilities should be member of the AN ability table */
        is_ability_valid = 0;
        for (ii = 0; ii < dnx_data_nif.eth.supported_clu_mgu_an_abilities_info_get(unit)->key_size[0]; ++ii)
        {
            if (dnx_data_nif.eth.supported_clu_mgu_an_abilities_get(unit, ii)->is_valid)
            {
                if (abilities[i].an_mode == dnx_data_nif.eth.supported_clu_mgu_an_abilities_get(unit, ii)->an_mode)
                {
                    if (abilities[i].speed == dnx_data_nif.eth.supported_clu_mgu_an_abilities_get(unit, ii)->speed)
                    {
                        if (abilities[i].resolved_num_lanes ==
                            dnx_data_nif.eth.supported_clu_mgu_an_abilities_get(unit, ii)->nof_lanes)
                        {
                            if (abilities[i].fec_type ==
                                dnx_data_nif.eth.supported_clu_mgu_an_abilities_get(unit, ii)->fec_type)
                            {
                                if (abilities[i].medium ==
                                    dnx_data_nif.eth.supported_clu_mgu_an_abilities_get(unit, ii)->medium)
                                {
                                    if ((abilities[i].channel < bcmPortPhyChannelAll) || (abilities[i].speed != 25000))
                                    {
                                        is_ability_valid = 1;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if (!is_ability_valid)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The autoneg ability[%d] is not supported by CLU.\r\n", i);
        }
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Translate from BCM autoneg abilities to legacy PORTMOD abilities
 * 
 * See .h file
 */
void
imb_portmod_legacy_an_ability_from_bcm_an_ability_get(
    int num_abilities,
    const bcm_port_speed_ability_t * bcm_abilities,
    portmod_port_ability_t * portmod_abilities)
{
    int i;

    for (i = 0; i < num_abilities; ++i)
    {
        /** speeds */
        if (10000 == bcm_abilities[i].speed)
        {
            portmod_abilities->speed_full_duplex |= SOC_PA_SPEED_10GB;
        }
        else if (25000 == bcm_abilities[i].speed)
        {
            portmod_abilities->speed_full_duplex |= SOC_PA_SPEED_25GB;
        }
        else if (40000 == bcm_abilities[i].speed)
        {
            portmod_abilities->speed_full_duplex |= SOC_PA_SPEED_40GB;
        }
        else if (50000 == bcm_abilities[i].speed)
        {
            portmod_abilities->speed_full_duplex |= SOC_PA_SPEED_50GB;
        }
        else if (100000 == bcm_abilities[i].speed)
        {
            portmod_abilities->speed_full_duplex |= SOC_PA_SPEED_100GB;
        }

        /** fec type */
        if (bcmPortPhyFecBaseR == bcm_abilities[i].fec_type)
        {
            portmod_abilities->fec |= SOC_PA_FEC_CL74;
        }
        else if (bcmPortPhyFecRsFec == bcm_abilities[i].fec_type)
        {
            portmod_abilities->fec |= SOC_PA_FEC_CL91;
        }
        else if (bcmPortPhyFecNone == bcm_abilities[i].fec_type)
        {
            portmod_abilities->fec |= SOC_PA_FEC_NONE;
        }
    }

    /** can be only one medium type */
    if (BCM_PORT_MEDIUM_COPPER == bcm_abilities[0].medium)
    {
        portmod_abilities->medium = _SHR_PA_MEDIUM_COPPER;
    }
    else if (BCM_PORT_MEDIUM_FIBER == bcm_abilities[0].medium)
    {
        portmod_abilities->medium = _SHR_PA_MEDIUM_FIBER;
    }
    else if (BCM_PORT_MEDIUM_BACKPLANE == bcm_abilities[0].medium)
    {
        portmod_abilities->medium = _SHR_PA_MEDIUM_BACKPLANE;
    }

    /** can be only one pause type */
    if (bcmPortPhyPauseTx == bcm_abilities[0].pause)
    {
        portmod_abilities->pause = _SHR_PA_PAUSE_TX;
    }
    else if (bcmPortPhyPauseRx == bcm_abilities[0].pause)
    {
        portmod_abilities->pause = _SHR_PA_PAUSE_RX;
    }
    else if (bcmPortPhyPauseSymm == bcm_abilities[0].pause)
    {
        portmod_abilities->pause = _SHR_PA_PAUSE_RX | _SHR_PA_PAUSE_TX;
    }

    /** can be only one channel type */
    if (bcmPortPhyChannelShort == bcm_abilities[0].channel)
    {
        portmod_abilities->channel = _SHR_PA_CHANNEL_SHORT;
    }
    else if (bcmPortPhyChannelLong == bcm_abilities[0].channel)
    {
        portmod_abilities->channel = _SHR_PA_CHANNEL_LONG;
    }
}

/**
 * \brief - Translate from PORTMOD legacy autoneg abilities to BCM abilities
 * 
 * See .h file
 */
int
imb_bcm_an_ability_from_portmod_legacy_an_ability_get(
    int unit,
    bcm_port_t port,
    int max_num_abilities,
    const portmod_port_ability_t * portmod_abilities,
    int *actual_num_ability,
    bcm_port_speed_ability_t * bcm_abilities)
{
    int i;
    phymod_autoneg_control_t an;
    bcm_port_autoneg_mode_t an_mode = (bcm_port_autoneg_mode_t) phymod_AN_MODE_NONE;
    bcm_port_medium_t medium = _SHR_PORT_MEDIUM_NONE;
    bcm_port_phy_pause_t pause = bcmPortPhyPauseNone;
    bcm_port_phy_fec_t fec = bcmPortPhyFecInvalid;
    uint32 speed = 0;
    int nof_lanes, cnt_abilities = 0;
    const int fec_portmod_to_bcm[] = { bcmPortPhyFecInvalid, bcmPortPhyFecNone, bcmPortPhyFecBaseR,
        bcmPortPhyFecInvalid, bcmPortPhyFecRsFec
    };
    const int channel_portmod_to_bcm[] =
        { bcmPortPhyChannelAll, bcmPortPhyChannelLong, bcmPortPhyChannelShort, bcmPortPhyChannelAll };

    SHR_FUNC_INIT_VARS(unit);

    /** resolve AN mode */
    SHR_IF_ERR_EXIT(portmod_port_autoneg_get(unit, port, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY, &an));
    if (phymod_AN_MODE_CL73 == an.an_mode)
    {
        an_mode = bcmPortAnModeCL73;
    }
    else if (phymod_AN_MODE_CL73BAM == an.an_mode)
    {
        an_mode = bcmPortAnModeCL73BAM;
    }

    /** resolve medium type */
    if (_SHR_PA_MEDIUM_COPPER == portmod_abilities->medium)
    {
        medium = BCM_PORT_MEDIUM_COPPER;
    }
    else if (_SHR_PA_MEDIUM_FIBER == portmod_abilities->medium)
    {
        medium = BCM_PORT_MEDIUM_FIBER;
    }
    else if (_SHR_PA_MEDIUM_BACKPLANE == portmod_abilities->medium)
    {
        medium = BCM_PORT_MEDIUM_BACKPLANE;
    }

    /** resolve pause type */
    if ((portmod_abilities->pause & _SHR_PA_PAUSE_RX) && (portmod_abilities->pause & _SHR_PA_PAUSE_TX))
    {
        pause = bcmPortPhyPauseSymm;
    }
    else if (portmod_abilities->pause & _SHR_PA_PAUSE_RX)
    {
        pause = bcmPortPhyPauseRx;
    }
    else if (portmod_abilities->pause & _SHR_PA_PAUSE_TX)
    {
        pause = bcmPortPhyPauseTx;
    }

    /** retrieve number of lane */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, port, &nof_lanes));

    /** iterate supported AN abilities table */
    for (i = 0; i < dnx_data_nif.eth.supported_clu_mgu_an_abilities_info_get(unit)->key_size[0]; ++i)
    {
        if (dnx_data_nif.eth.supported_clu_mgu_an_abilities_get(unit, i)->is_valid)
        {
            if ((an_mode == dnx_data_nif.eth.supported_clu_mgu_an_abilities_get(unit, i)->an_mode)
                && (nof_lanes == dnx_data_nif.eth.supported_clu_mgu_an_abilities_get(unit, i)->nof_lanes)
                && (medium == dnx_data_nif.eth.supported_clu_mgu_an_abilities_get(unit, i)->medium))
            {
                fec = dnx_data_nif.eth.supported_clu_mgu_an_abilities_get(unit, i)->fec_type;
                if ((fec == fec_portmod_to_bcm[(portmod_abilities->fec & _SHR_PA_FEC_NONE ? _SHR_PA_FEC_NONE : 0)])
                    ||
                    (fec == fec_portmod_to_bcm[(portmod_abilities->fec & _SHR_PA_FEC_CL74 ? _SHR_PA_FEC_CL74 : 0)])
                    || (fec == fec_portmod_to_bcm[(portmod_abilities->fec & _SHR_PA_FEC_CL91 ? _SHR_PA_FEC_CL91 : 0)]))
                {
                    speed = dnx_data_nif.eth.supported_clu_mgu_an_abilities_get(unit, i)->speed;
                    if (((speed == 10000) && (portmod_abilities->speed_full_duplex & _SHR_PA_SPEED_10GB))
                        || ((speed == 20000) && (portmod_abilities->speed_full_duplex & _SHR_PA_SPEED_20GB))
                        || ((speed == 25000) && (portmod_abilities->speed_full_duplex & _SHR_PA_SPEED_25GB))
                        || ((speed == 40000) && (portmod_abilities->speed_full_duplex & _SHR_PA_SPEED_40GB))
                        || ((speed == 50000) && (portmod_abilities->speed_full_duplex & _SHR_PA_SPEED_50GB))
                        || ((speed == 100000) && (portmod_abilities->speed_full_duplex & _SHR_PA_SPEED_100GB)))
                    {
                        if (cnt_abilities < max_num_abilities)
                        {
                            bcm_abilities[cnt_abilities].an_mode = an_mode;
                            bcm_abilities[cnt_abilities].speed = speed;
                            bcm_abilities[cnt_abilities].resolved_num_lanes = nof_lanes;
                            bcm_abilities[cnt_abilities].medium = medium;
                            bcm_abilities[cnt_abilities].fec_type = fec;
                            bcm_abilities[cnt_abilities].channel = channel_portmod_to_bcm[portmod_abilities->channel];
                            bcm_abilities[cnt_abilities].pause = pause;
                            ++cnt_abilities;
                        }
                        else
                        {
                            SHR_ERR_EXIT(_SHR_E_PARAM, "max_num_abilities %d is too small.\r\n", max_num_abilities);
                        }
                    }
                }
            }
        }
    }

    *actual_num_ability = cnt_abilities;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Translate from PORTMOD legacy autoneg abilities to BCM local abilities
 * 
 * See .h file
 */
int
imb_bcm_local_an_ability_from_portmod_legacy_an_ability_get(
    int unit,
    bcm_port_t port,
    int max_num_abilities,
    const portmod_port_ability_t * portmod_abilities,
    int *actual_num_ability,
    bcm_port_speed_ability_t * bcm_abilities)
{
    int i;
    uint32 speed = 0;
    int nof_lanes, cnt_abilities = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** retrieve number of lane */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, port, &nof_lanes));

    /** iterate supported AN abilities table */
    for (i = 0; i < dnx_data_nif.eth.supported_clu_mgu_an_abilities_info_get(unit)->key_size[0]; ++i)
    {
        if (dnx_data_nif.eth.supported_clu_mgu_an_abilities_get(unit, i)->is_valid)
        {
            if (nof_lanes == dnx_data_nif.eth.supported_clu_mgu_an_abilities_get(unit, i)->nof_lanes)
            {
                speed = dnx_data_nif.eth.supported_clu_mgu_an_abilities_get(unit, i)->speed;
                if (((speed == 10000) && (portmod_abilities->speed_full_duplex & _SHR_PA_SPEED_10GB))
                    || ((speed == 20000) && (portmod_abilities->speed_full_duplex & _SHR_PA_SPEED_20GB))
                    || ((speed == 25000) && (portmod_abilities->speed_full_duplex & _SHR_PA_SPEED_25GB))
                    || ((speed == 40000) && (portmod_abilities->speed_full_duplex & _SHR_PA_SPEED_40GB))
                    || ((speed == 50000) && (portmod_abilities->speed_full_duplex & _SHR_PA_SPEED_50GB))
                    || ((speed == 100000) && (portmod_abilities->speed_full_duplex & _SHR_PA_SPEED_100GB)))
                {
                    if (cnt_abilities < max_num_abilities)
                    {
                        if (cnt_abilities > 0)
                        {
                            if ((bcm_abilities[cnt_abilities - 1].an_mode ==
                                 dnx_data_nif.eth.supported_clu_mgu_an_abilities_get(unit, i)->an_mode)
                                && (bcm_abilities[cnt_abilities - 1].speed == speed)
                                && (bcm_abilities[cnt_abilities - 1].resolved_num_lanes == nof_lanes)
                                && (bcm_abilities[cnt_abilities - 1].fec_type ==
                                    dnx_data_nif.eth.supported_clu_mgu_an_abilities_get(unit, i)->fec_type))
                            {
                                continue;
                            }
                        }

                        bcm_abilities[cnt_abilities].an_mode =
                            dnx_data_nif.eth.supported_clu_mgu_an_abilities_get(unit, i)->an_mode;
                        bcm_abilities[cnt_abilities].speed = speed;
                        bcm_abilities[cnt_abilities].resolved_num_lanes = nof_lanes;
                        if (speed == 10000)
                        {
                            bcm_abilities[cnt_abilities].medium =
                                dnx_data_nif.eth.supported_clu_mgu_an_abilities_get(unit, i)->medium;
                        }
                        else
                        {
                            bcm_abilities[cnt_abilities].medium = _SHR_PORT_MEDIUM_ALL;
                        }
                        bcm_abilities[cnt_abilities].fec_type =
                            dnx_data_nif.eth.supported_clu_mgu_an_abilities_get(unit, i)->fec_type;
                        bcm_abilities[cnt_abilities].channel = bcmPortPhyChannelAll;
                        bcm_abilities[cnt_abilities].pause = bcmPortPhyPauseALL;
                        ++cnt_abilities;
                    }
                    else
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM, "max_num_abilities %d is too small.\r\n", max_num_abilities);
                    }
                }
            }
        }
    }

    *actual_num_ability = cnt_abilities;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - roll-back a 'portmod enable set'
 */
static shr_error_e
imb_generic_portmod_enable_journal_rollback(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint8 *metadata,
    uint8 *payload,
    char *stamp)
{
    int *port, *enable;

    SHR_FUNC_INIT_VARS(unit);

    if (NULL == metadata)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "portmod journal ERROR: invalid port detected.\n");
    }
    if (NULL == payload)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "portmod journal ERROR: invalid enable detected.\n");
    }

    port = (int *) (metadata);
    enable = (int *) (payload);

    /*
     * restore portmod enable by portmod api
     */
    SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, *port, 0, *enable));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Translate from PORTMOD legacy autoneg abilities to BCM local abilities
 * 
 * See .h file
 */
int
imb_er_portmod_port_enable_set(
    int unit,
    int port,
    int flags,
    int enable)
{
    int enable_orig;

    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_SUPPRESS(unit);

    SHR_IF_ERR_CONT(portmod_port_enable_get(unit, port, 0, &enable_orig));
    if (SHR_FUNC_ERR())
    {
        DNX_ERR_RECOVERY_UNSUPPRESS(unit);
        SHR_EXIT();
    }

    SHR_IF_ERR_CONT(portmod_port_enable_set(unit, port, flags, enable));
    if (SHR_FUNC_ERR())
    {
        DNX_ERR_RECOVERY_UNSUPPRESS(unit);
        SHR_EXIT();
    }
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);

    SHR_IF_ERR_EXIT(dnx_generic_state_journal_log_entry(unit,
                                                        (uint8 *) &port,
                                                        sizeof(int),
                                                        (uint8 *) &enable_orig,
                                                        sizeof(int),
                                                        &imb_generic_portmod_enable_journal_rollback, FALSE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - roll-back a 'portmod tx threshold set'
 */
static shr_error_e
imb_generic_portmod_tx_threshold_journal_rollback(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint8 *metadata,
    uint8 *payload,
    char *stamp)
{
    int *port, *value;

    SHR_FUNC_INIT_VARS(unit);

    if (NULL == metadata)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "portmod journal ERROR: invalid port detected.\n");
    }
    if (NULL == payload)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "portmod journal ERROR: invalid enable detected.\n");
    }

    port = (int *) (metadata);
    value = (int *) (payload);

    /*
     * restore tx threshold set by portmod api
     */
    SHR_IF_ERR_EXIT(portmod_port_tx_threshold_set(unit, *port, *value));

exit:
    SHR_FUNC_EXIT;
}

/**
 * Set tx threshold
 * See .h file
 */
int
imb_er_portmod_port_tx_threshold_set(
    int unit,
    int port,
    int value)
{
    int value_orig;

    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_SUPPRESS(unit);

    SHR_IF_ERR_CONT(portmod_port_tx_threshold_get(unit, port, &value_orig));
    if (SHR_FUNC_ERR())
    {
        DNX_ERR_RECOVERY_UNSUPPRESS(unit);
        SHR_EXIT();
    }

    SHR_IF_ERR_CONT(portmod_port_tx_threshold_set(unit, port, value));
    if (SHR_FUNC_ERR())
    {
        DNX_ERR_RECOVERY_UNSUPPRESS(unit);
        SHR_EXIT();
    }
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);

    SHR_IF_ERR_EXIT(dnx_generic_state_journal_log_entry(unit,
                                                        (uint8 *) &port,
                                                        sizeof(int),
                                                        (uint8 *) &value_orig,
                                                        sizeof(int),
                                                        &imb_generic_portmod_tx_threshold_journal_rollback, FALSE));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
imb_er_portmod_port_remove_rollback(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint8 *metadata,
    uint8 *payload,
    char *stamp)
{
    int *port;
    portmod_port_add_info_t *add_info;
    int nof_channels;

    SHR_FUNC_INIT_VARS(unit);

    if (NULL == metadata)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "portmod journal ERROR: invalid port detected.\n");
    }
    if (NULL == payload)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "portmod journal ERROR: invalid add_info detected.\n");
    }

    port = (int *) (metadata);
    add_info = (portmod_port_add_info_t *) (payload);

    /*
     * restore portmod add by portmod api
     */
    SHR_IF_ERR_EXIT(imb_portmod_port_add(unit, *port, add_info));

    SHR_IF_ERR_EXIT(dnx_algo_port_channels_nof_get(unit, *port, &nof_channels));
    if (nof_channels == 1)
    {
        SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, *port, 0, 0));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
imb_er_portmod_port_add_rollback(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint8 *metadata,
    uint8 *payload,
    char *stamp)
{
    int *port;
    int orin_enable;
    int nof_channels;
    int is_master_port, next_master_port;
    SHR_FUNC_INIT_VARS(unit);

    if (NULL == metadata)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "portmod journal ERROR: invalid port detected.\n");
    }

    port = (int *) (metadata);

    /*
     * restore portmod add by portmod api
     */
    SHR_IF_ERR_EXIT(portmod_port_enable_get(unit, *port, 0, &orin_enable));
    SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, *port, 0, 0));
    SHR_IF_ERR_EXIT(portmod_port_remove(unit, *port));
    SHR_IF_ERR_EXIT(dnx_algo_port_channels_nof_get(unit, *port, &nof_channels));
    if (nof_channels > 1)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_is_master_get(unit, *port, 0, &is_master_port));
        if (is_master_port)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, *port, DNX_ALGO_PORT_MASTER_F_NEXT, &next_master_port));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, *port, 0, &next_master_port));
        }
        SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, next_master_port, 0, orin_enable));
    }

exit:
    SHR_FUNC_EXIT;
}


int
imb_er_portmod_port_add(
    int unit,
    int port,
    portmod_port_add_info_t * add_info)
{
    int suppressed = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_SUPPRESS(unit);
    suppressed = TRUE;

    SHR_IF_ERR_EXIT(imb_portmod_port_add(unit, port, add_info));
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    suppressed = FALSE;

    SHR_IF_ERR_EXIT(dnx_generic_state_journal_log_entry(unit,
                                                        (uint8 *) &port,
                                                        sizeof(int),
                                                        (uint8 *) &port,
                                                        sizeof(int), &imb_er_portmod_port_add_rollback, FALSE));

exit:
    if (suppressed)
    {
        DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    }
    SHR_FUNC_EXIT;
}

int
imb_er_portmod_port_remove(
    int unit,
    int port)
{
    int suppressed = FALSE;
    portmod_port_add_info_t add_info;
    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_SUPPRESS(unit);
    suppressed = TRUE;
    SHR_IF_ERR_EXIT(portmod_port_remove(unit, port));
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    suppressed = FALSE;

    SHR_IF_ERR_EXIT(imb_portmod_add_info_config(unit, port, &add_info));

    SHR_IF_ERR_EXIT(dnx_generic_state_journal_log_entry(unit,
                                                        (uint8 *) &port,
                                                        sizeof(int),
                                                        (uint8 *) &add_info,
                                                        sizeof(portmod_port_add_info_t),
                                                        &imb_er_portmod_port_remove_rollback, FALSE));

exit:
    if (suppressed)
    {
        DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief - roll-back a 'portmod preemption control set'
 */
static shr_error_e
imb_generic_portmod_preemption_control_journal_rollback(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint8 *metadata,
    uint8 *payload,
    char *stamp)
{
    uint32 *value;
    imb_portmod_preemption_control_er_journal_t *journal_entry = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (NULL == metadata)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "portmod preemption control journal ERROR: invalid port or portmod detected.\n");
    }
    if (NULL == payload)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "portmod preemption control journal ERROR: invalid value detected.\n");
    }

    journal_entry = (imb_portmod_preemption_control_er_journal_t *) (metadata);
    value = (uint32 *) (payload);

    /*
     * restore portmod enable by portmod api
     */
    SHR_IF_ERR_EXIT(portmod_preemption_control_set(unit, journal_entry->port, journal_entry->portmod_type, *value));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
int
imb_er_portmod_preemption_control_set(
    int unit,
    bcm_port_t port,
    portmod_preemption_control_t portmod_type,
    uint32 value)
{
    uint32 value_orig;
    imb_portmod_preemption_control_er_journal_t journal_entry;

    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_SUPPRESS(unit);

    SHR_IF_ERR_CONT(portmod_preemption_control_get(unit, port, portmod_type, &value_orig));
    if (SHR_FUNC_ERR())
    {
        DNX_ERR_RECOVERY_UNSUPPRESS(unit);
        SHR_EXIT();
    }

    SHR_IF_ERR_CONT(portmod_preemption_control_set(unit, port, portmod_type, value));
    if (SHR_FUNC_ERR())
    {
        DNX_ERR_RECOVERY_UNSUPPRESS(unit);
        SHR_EXIT();
    }
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    journal_entry.port = port;
    journal_entry.portmod_type = portmod_type;

    SHR_IF_ERR_EXIT(dnx_generic_state_journal_log_entry(unit,
                                                        (uint8 *) &journal_entry,
                                                        sizeof(imb_portmod_preemption_control_er_journal_t),
                                                        (uint8 *) &value_orig,
                                                        sizeof(uint32),
                                                        &imb_generic_portmod_preemption_control_journal_rollback,
                                                        FALSE));

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
