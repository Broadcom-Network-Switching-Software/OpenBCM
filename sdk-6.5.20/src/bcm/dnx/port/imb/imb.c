/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <sal/types.h>
#include <sal/core/alloc.h>
#include <soc/defs.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/port/imb/imb_dispatch.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_port_imb_access.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_port_imb_types.h>
#include <bcm_int/dnx/port/imb/imb_common.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <soc/portmod/portmod.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

/**
 * See imb_internal.h
 */
int
imb_id_type_get(
    int unit,
    int imb_id,
    imb_dispatch_type_t * type)
{
    imb_create_info_t imb;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imbm.imb.get(unit, imb_id, &imb));
    *type = imb.type;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get imb_type from ethu_id
 *
 * \param [in] unit   - chip unit id.
 * \param [in] ethu_id - ethu id.
 * \param [in] type   - imb dispatcher type.
 *
 *
 * \return
 *   int  - err code. see _SHR_E_*
 *
 * \see
 *   * None
 */
int
ethu_id_type_get(
    int unit,
    int ethu_id,
    imb_dispatch_type_t * type)
{
    SHR_FUNC_INIT_VARS(unit);

    *type = dnx_data_nif.eth.ethu_properties_get(unit, ethu_id)->type;

    SHR_FUNC_EXIT;
}

/**
 * \brief - get imb_type from imb_id
 *
 * \param [in]  unit     - chip unit id.
 * \param [out] nof_imbs - number of imb instances in use
 *
 *
 * \return
 *   int  - err code. see _SHR_E_*
 *
 * \see
 *   * None
 */
int
imb_nof_get(
    int unit,
    uint32 *nof_imbs)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imbm.imbs_in_use.get(unit, nof_imbs));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See imb.h
 */
int
imb_init_all(
    int unit)
{

    bcm_port_t port;
    bcm_port_t logical_port;
    uint32 index, imb_id, nof_types, nof_ethus, nof_ilkn_units, nof_fabric_pms, nof_imbs;
    uint32 nof_feu_imbs, nof_feu_phy_imbs;
    int ilkn_core_id;
    uint32 max_num_ports = 0;
    uint8 ilkn_unit_elk = 0;
    uint8 elk_array[DNX_DATA_MAX_NIF_ILKN_ILKN_UNIT_NOF] = { 0 };
    const dnx_data_nif_ilkn_ilkn_cores_t *ilkn_core_info;
    const dnx_data_port_static_add_ucode_port_t *ucode_port;
    imb_create_info_t imb;
    portmod_pm_instances_t pm_types_and_instances[DNX_DATA_MAX_NIF_PORTMOD_PM_TYPES_NOF];
    int flags = 0;
    SHR_FUNC_INIT_VARS(unit);

    nof_types = dnx_data_nif.portmod.pm_types_nof_get(unit);
    nof_ethus = dnx_data_nif.eth.ethu_nof_get(unit);
    nof_ilkn_units = dnx_data_nif.ilkn.ilkn_unit_nof_get(unit);
    nof_fabric_pms = dnx_data_fabric.blocks.nof_pms_get(unit);
    nof_feu_imbs = dnx_data_nif.flexe.feu_nof_get(unit);
    nof_feu_phy_imbs = dnx_data_nif.flexe.feu_nof_get(unit);
    nof_imbs = nof_ethus + nof_ilkn_units + nof_fabric_pms + nof_feu_imbs + nof_feu_phy_imbs;

    if (!sw_state_is_warm_boot(unit))
    {
        /*
         * Allocate a IMBM for this unit
         */
        SHR_IF_ERR_EXIT(imbm.init(unit));
        SHR_IF_ERR_EXIT(imbm.imbs_in_use.set(unit, 0));
        SHR_IF_ERR_EXIT(imbm.imb.alloc(unit, nof_imbs));

        for (port = 0; port < SOC_MAX_NUM_PORTS; ++port)
        {
            SHR_IF_ERR_EXIT(imbm.imb_type.set(unit, port, imbDispatchTypeNone));
        }

        /*
         * init mutex
         */
        SHR_IF_ERR_EXIT(imbm.credit_tx_reset_mutex.create(unit));
    }
    /*
     * Initialize portmod module
     */
    for (index = 0; index < nof_types; ++index)
    {
        pm_types_and_instances[index].instances =
            dnx_data_nif.portmod.pm_types_and_interfaces_get(unit, index)->instances;
        pm_types_and_instances[index].type = dnx_data_nif.portmod.pm_types_and_interfaces_get(unit, index)->type;
    }
    if (dnx_data_nif.eth.feature_get(unit, dnx_data_nif_eth_portmod_thread_disable))
    {
        PORTMOD_CREATE_F_PORTMOD_THREAD_DISABLE_SET(flags);
    }
    SHR_IF_ERR_EXIT(portmod_create
                    (unit, flags, SOC_MAX_NUM_PORTS, SOC_MAX_NUM_PORTS, nof_types, pm_types_and_instances));
    if (!dnx_data_dev_init.general.access_only_get(unit) || dnx_data_dev_init.general.heat_up_get(unit))
    {
        imb_id = 0;

        /*
         * Init all Ethernet units
         */
        for (index = 0; index < nof_ethus; ++index)
        {
            SHR_IF_ERR_EXIT(imbm.imb.get(unit, imb_id, &imb));
            imb.type = dnx_data_nif.eth.ethu_properties_get(unit, index)->type;
            imb.inst_id = index;
            SHR_IF_ERR_EXIT(imb_init(unit, &imb, &imb.imb_specific_info));
            SHR_IF_ERR_EXIT(imbm.imb.set(unit, imb_id, imb));
            ++imb_id;
        }

        /*
         * Init all Fabric units
         */
        for (index = 0; index < nof_fabric_pms; ++index)
        {
            SHR_IF_ERR_EXIT(imbm.imb.get(unit, imb_id, &imb));
            imb.type = imbDispatchTypeImb_fabric;
            imb.inst_id = index;
            SHR_IF_ERR_EXIT(imb_init(unit, &imb, &imb.imb_specific_info));
            SHR_IF_ERR_EXIT(imbm.imb.set(unit, imb_id, imb));
            ++imb_id;
        }

        /*
         * if port is ilkn mark if KBP is enabled for its ilkn unit
         */
        max_num_ports = dnx_data_port.general.fabric_port_base_get(unit);
        for (logical_port = 0; logical_port < max_num_ports; logical_port++)
        {
            ucode_port = dnx_data_port.static_add.ucode_port_get(unit, logical_port);
            if (ucode_port->interface == BCM_PORT_IF_ILKN)
            {
                ilkn_core_id = ucode_port->interface_offset / dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit);
                elk_array[ilkn_core_id] = (ucode_port->is_kbp) ? TRUE : FALSE;
            }
        }

        /*
         * Init all ILKN units
         */
        for (index = 0; index < nof_ilkn_units; ++index)
        {
            ilkn_core_info = dnx_data_nif.ilkn.ilkn_cores_get(unit, index);
            /*
             * The ilkn interface will be initialized as ilkn for elk (ILE) if it's core is ELK only or: if it is
             * 'combined' mode (can be used both for data and for ELK) and there is ilkn port on this core which is defined
             * for ELK.
             */
            if ((!ilkn_core_info->is_data_supported) ||
                ((ilkn_core_info->is_data_supported) && (ilkn_core_info->is_elk_supported) && (elk_array[index])))
            {
                ilkn_unit_elk = TRUE;
            }
            else
            {
                ilkn_unit_elk = FALSE;
            }
            SHR_IF_ERR_EXIT(imbm.imb.get(unit, imb_id, &imb));
            imb.type = (ilkn_unit_elk) ? imbDispatchTypeImb_ile : imbDispatchTypeImb_ilu;
            imb.inst_id = index;
            SHR_IF_ERR_EXIT(imb_init(unit, &imb, &imb.imb_specific_info));
            SHR_IF_ERR_EXIT(imbm.imb.set(unit, imb_id, imb));
            ++imb_id;
        }

        /*
         * Init all FEU PHY units
         */
        for (index = 0; index < nof_feu_phy_imbs; ++index)
        {
            SHR_IF_ERR_EXIT(imbm.imb.get(unit, imb_id, &imb));
            imb.type = imbDispatchTypeImb_feu_phy;
            imb.inst_id = index;
            SHR_IF_ERR_EXIT(imb_init(unit, &imb, &imb.imb_specific_info));
            SHR_IF_ERR_EXIT(imbm.imb.set(unit, imb_id, imb));
            ++imb_id;
        }

        /*
         * Init all FEU units
         */
        for (index = 0; index < nof_feu_imbs; ++index)
        {
            SHR_IF_ERR_EXIT(imbm.imb.get(unit, imb_id, &imb));
            imb.type = imbDispatchTypeImb_feu;
            imb.inst_id = index;
            SHR_IF_ERR_EXIT(imb_init(unit, &imb, &imb.imb_specific_info));
            SHR_IF_ERR_EXIT(imbm.imb.set(unit, imb_id, imb));
            ++imb_id;
        }

        if (!sw_state_is_warm_boot(unit))
        {
            SHR_IF_ERR_EXIT(imbm.imbs_in_use.set(unit, imb_id));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See imb.h
 */
int
imb_deinit_all(
    int unit)
{
    int index, nof_ethus, nof_ilkn_units, nof_imbs, nof_fabric_pms;
    int nof_feu_imbs, nof_feu_phy_imbs;
    imb_create_info_t imb;
    uint8 is_init = 0;
    SHR_FUNC_INIT_VARS(unit);

    nof_ethus = dnx_data_nif.eth.ethu_nof_get(unit);
    nof_ilkn_units = dnx_data_nif.ilkn.ilkn_unit_nof_get(unit);
    nof_fabric_pms = dnx_data_fabric.blocks.nof_pms_get(unit);
    nof_feu_imbs = dnx_data_nif.flexe.feu_nof_get(unit);
    nof_feu_phy_imbs = dnx_data_nif.flexe.feu_nof_get(unit);
    nof_imbs = nof_ethus + nof_ilkn_units + nof_fabric_pms + nof_feu_imbs + nof_feu_phy_imbs;

    /*
     * de-init Portmod module
     */
    SHR_IF_ERR_CONT(portmod_destroy(unit));
    /*
     * de-init all IMBs
     */
    SHR_IF_ERR_CONT(imbm.is_init(unit, &is_init));
    if (is_init == TRUE)
    {
        for (index = 0; index < nof_imbs; ++index)
        {
            /*
             * de-init should continue even if one of the method fails
             */
            SHR_IF_ERR_CONT(imbm.imb.get(unit, index, &imb));
            SHR_IF_ERR_CONT(imb_deinit(unit, &imb, &imb.imb_specific_info));
        }
    }

    /*
     * sw state module deinit is done automatically at device deinit
     */

    SHR_FUNC_EXIT;
}

int
imb_port_add(
    int unit,
    bcm_port_t port,
    uint32 flags)
{
    imb_dispatch_type_t imb_type;
    dnx_algo_port_info_s port_info;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);

    if (IMB_PORT_ADD_F_SKIP_SETTINGS_GET(flags) == FALSE)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

        if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN_ELK(unit, port_info))
        {
            imb_type = imbDispatchTypeImb_ile;
        }
        else if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 0, 1))
        {
            imb_type = imbDispatchTypeImb_ilu;
        }
        else if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, 1, 1))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
            imb_type = dnx_data_nif.eth.ethu_properties_get(unit, ethu_info.ethu_id)->type;
        }
        else if (DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info))
        {
            imb_type = imbDispatchTypeImb_feu_phy;
        }
        else if (DNX_ALGO_PORT_TYPE_IS_FLEXE(unit, port_info, 1, 0))
        {
            imb_type = imbDispatchTypeImb_feu;
        }
        else if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_info))
        {
            imb_type = imbDispatchTypeImb_fabric;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Port %d interface type is not supported in IMB", port);
        }

        SHR_IF_ERR_EXIT(imbm.imb_type.set(unit, port, imb_type));

        IMB_PORT_ADD_F_SKIP_SETTINGS_CLR(flags);
    }

    SHR_IF_ERR_EXIT(imb_port_attach(unit, port, flags));

exit:
    SHR_FUNC_EXIT;
}

int
imb_port_remove(
    int unit,
    bcm_port_t port)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_port_detach(unit, port));

    SHR_IF_ERR_EXIT(imbm.imb_type.set(unit, port, imbDispatchTypeNone));

exit:
    SHR_FUNC_EXIT;
}

int
imb_llfc_from_glb_rsc_enable_all(
    int unit,
    uint32 enable)
{
    int index;
    uint32 nof_imbs;
    imb_create_info_t imb;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imbm.imbs_in_use.get(unit, &nof_imbs));

    for (index = 0; index < nof_imbs; ++index)
    {
        SHR_IF_ERR_EXIT(imbm.imb.get(unit, index, &imb));
        if (__imb__dispatch__[imb.type]->f_imb_llfc_from_glb_rsc_enable_set != NULL)
        {
            SHR_IF_ERR_EXIT(__imb__dispatch__[imb.type]->f_imb_llfc_from_glb_rsc_enable_set(unit, &imb, enable));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

int
imb_instru_counters_reset_all(
    int unit)
{
    int index;
    uint32 nof_imbs;
    imb_create_info_t imb;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imbm.imbs_in_use.get(unit, &nof_imbs));

    for (index = 0; index < nof_imbs; ++index)
    {
        SHR_IF_ERR_EXIT(imbm.imb.get(unit, index, &imb));
        if (__imb__dispatch__[imb.type]->f_imb_instru_counters_reset != NULL)
        {
            SHR_IF_ERR_EXIT(__imb__dispatch__[imb.type]->f_imb_instru_counters_reset(unit, &imb));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

int
imb_pfc_deadlock_counters_reset_all(
    int unit)
{
    int index;
    uint32 nof_imbs;
    imb_create_info_t imb;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imbm.imbs_in_use.get(unit, &nof_imbs));

    for (index = 0; index < nof_imbs; ++index)
    {
        SHR_IF_ERR_EXIT(imbm.imb.get(unit, index, &imb));
        if (__imb__dispatch__[imb.type]->f_imb_pfc_deadlock_counters_reset != NULL)
        {
            SHR_IF_ERR_EXIT(__imb__dispatch__[imb.type]->f_imb_pfc_deadlock_counters_reset(unit, &imb));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

int
imb_fc_reset_all(
    int unit,
    uint32 in_reset)
{
    int index;
    uint32 nof_imbs;
    imb_create_info_t imb;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imbm.imbs_in_use.get(unit, &nof_imbs));

    for (index = 0; index < nof_imbs; ++index)
    {
        SHR_IF_ERR_EXIT(imbm.imb.get(unit, index, &imb));
        if (__imb__dispatch__[imb.type]->f_imb_fc_reset_set != NULL)
        {
            SHR_IF_ERR_EXIT(__imb__dispatch__[imb.type]->f_imb_fc_reset_set(unit, &imb, in_reset));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

int
imb_inband_ilkn_fc_init_all(
    int unit)
{
    int index;
    uint32 nof_imbs;
    imb_create_info_t imb;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imbm.imbs_in_use.get(unit, &nof_imbs));

    for (index = 0; index < nof_imbs; ++index)
    {
        SHR_IF_ERR_EXIT(imbm.imb.get(unit, index, &imb));
        if (__imb__dispatch__[imb.type]->f_imb_inband_ilkn_fc_init != NULL)
        {
            SHR_IF_ERR_EXIT(__imb__dispatch__[imb.type]->f_imb_inband_ilkn_fc_init(unit, &imb));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * See imb.h
 */
int
imb_port_pre_add_validate_wrap(
    int unit,
    bcm_port_t port,
    bcm_pbmp_t phys)
{
    uint32 nof_imbs;
    imb_create_info_t imb_info;
    int index, port_to_add_first_phy, port_to_add_ethu_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imbm.imbs_in_use.get(unit, &nof_imbs));

    for (index = 0; index < nof_imbs; ++index)
    {
        SHR_IF_ERR_EXIT(imbm.imb.get(unit, index, &imb_info));
        if (__imb__dispatch__[imb_info.type]->f_imb_port_pre_add_validate != NULL)
        {
            /**
             * Speed optimization:
             * Dispatch only if ETHU id matches the one of the prot to be added
             */
            _SHR_PBMP_FIRST(phys, port_to_add_first_phy);
            SHR_IF_ERR_EXIT(dnx_algo_phy_to_ethu_id_get(unit, port_to_add_first_phy, &port_to_add_ethu_id));

            if (imb_info.inst_id == port_to_add_ethu_id)
            {
                SHR_IF_ERR_EXIT(__imb__dispatch__[imb_info.type]->f_imb_port_pre_add_validate(unit, port, phys));
                break;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

int
imb_prd_port_profile_map_is_supported(
    int unit,
    bcm_port_t port,
    int *is_supported)
{
    imb_dispatch_type_t __type__;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imbm.imb_type.get(unit, port, &__type__));
    *is_supported = dnx_data_nif.prd.port_profile_map_get(unit, __type__)->is_supported;

exit:
    SHR_FUNC_EXIT;
}

int
imb_prd_nof_port_profiles_get(
    int unit,
    bcm_port_t port,
    int *nof_port_profiles)
{
    imb_dispatch_type_t __type__;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imbm.imb_type.get(unit, port, &__type__));
    *nof_port_profiles = dnx_data_nif.prd.port_profile_map_get(unit, __type__)->nof_profiles;

exit:
    SHR_FUNC_EXIT;

}

#undef BSL_LOG_MODULE
