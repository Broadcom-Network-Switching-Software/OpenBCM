/*
 * 
 * ipq_api.c
 *
 *  Created on: Jun 19, 2018
 *      Author: si888124
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>

#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/cosq/cosq.h>
#include <bcm_int/dnx/cosq/ingress/ipq.h>
#include <bcm_int/dnx/cosq/ingress/iqs.h>
#include <bcm_int/dnx/cosq/ingress/iqs_api.h>

#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_ipq_alloc_mngr_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_ipq_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>

#include "ingress_congestion.h"
#include "ipq_dbal.h"
#include "ipq.h"
/*
 * }
 */

/**
 * \brief - verify bcm_dnx_dnx_cosq_ingress_queue_bundle_gport_add parameters
 * this function will check the general verifiers.
 * more specific verifiers can be found in Unicast/ Multicast specific verifiers.
 */
static shr_error_e
dnx_cosq_ingress_queue_bundle_gport_add_verify(
    int unit,
    bcm_cosq_ingress_queue_bundle_gport_config_t * config,
    bcm_gport_t * gport)
{
    int numq, cosq;
    int delay_tolerance_level, delay_tolerance_profile;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(config, _SHR_E_PARAM, "config");
    SHR_NULL_CHECK(gport, _SHR_E_PARAM, "gport");

    /*
     * Verify number of requested queues
     */
    numq = config->numq;
    if ((numq < 0) || (numq > DNX_COSQ_NOF_TC))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid cos levels specified %d\n", numq);
    }

    /*
     * Verify Flags
     */
    if (config->flags & BCM_COSQ_GPORT_ISQ)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ISQ is not supported on this device\n");
    }
    else if (config->flags & BCM_COSQ_GPORT_SW_ONLY)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_COSQ_GPORT_SW_ONLY flag is not supported\n");
    }
    else if (!(config->flags & BCM_COSQ_GPORT_UCAST_QUEUE_GROUP) && !(config->flags & BCM_COSQ_GPORT_MCAST_QUEUE_GROUP))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Ingress queue bundle add  - must specify Unicast or Multicast flag. Not support for flags(0x%x)\n",
                     config->flags);
    }

    /*
     * Verify delay tolerance level
     */
    for (cosq = 0; cosq < numq; cosq++)
    {
        delay_tolerance_level = config->queue_atrributes[cosq].delay_tolerance_level;
        /*
         * convert to actual credit request profile id
         */
        SHR_IF_ERR_EXIT(dnx_iqs_api_delay_tolerance_level_to_profile_id_get
                        (unit, delay_tolerance_level, &delay_tolerance_profile));
        /*
         * verify profile_id valid
         */
        SHR_IF_ERR_EXIT(dnx_iqs_credit_request_profile_valid_verify(unit, delay_tolerance_profile));
    }

    /*
     * Validate Rate Class
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_voq_bundle_gport_config_rate_class_verify(unit, config));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
bcm_dnx_cosq_ingress_queue_bundle_gport_add(
    int unit,
    bcm_cosq_ingress_queue_bundle_gport_config_t * config,
    bcm_gport_t * gport)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_queue_bundle_gport_add_verify(unit, config, gport));

    if (config->flags & BCM_COSQ_GPORT_UCAST_QUEUE_GROUP)
    {
        /*
         * Unicast
         */
        SHR_IF_ERR_EXIT(dnx_cosq_ipq_ucast_gport_add(unit, config, gport));
    }
    else
    {
        /*
         * Multicast
         */
        SHR_IF_ERR_EXIT(dnx_cosq_ipq_mcast_gport_add(unit, config, gport));
    }
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Helper function to validate that new slave SYSPORT points to same MODPORT as master SYSPORT.
 * The way it works:
 *      1. MASTER_SYSPORT -> MASTER_MODPORT
 *      2. SLAVE_SYSPORT -> SLAVE_MODPORT
 *      3. Compare
 * \param [in] unit - unit
 * \param [in] slave_sysport - slave system port gport
 * \param [in] master_sysport - master system port gport
 * \param [out] is_equal - output
 *
 * \return
 *    shr_error_e
 * \remark
 *   None
 * \see
 *   NONE
 */
static int
dnx_cosq_sysport_ingress_queue_map_modports_is_equal(
    int unit,
    bcm_gport_t slave_sysport,
    bcm_gport_t master_sysport,
    int *is_equal)
{
    bcm_gport_t master_modport = 0;
    bcm_gport_t slave_modport = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** 1. MASTER_SYSPORT -> MASTER_MODPORT */
    SHR_IF_ERR_EXIT(bcm_dnx_stk_sysport_gport_get(unit, master_sysport, &master_modport));

    /** 2. SLAVE_SYSPORT -> SLAVE_MODPORT */
    SHR_IF_ERR_EXIT(bcm_dnx_stk_sysport_gport_get(unit, slave_sysport, &slave_modport));

    /** 3. Compare */
    *is_equal = (master_modport == slave_modport) ? TRUE : FALSE;

exit:
    SHR_FUNC_EXIT;
}

/*
 * brief function to validate the parameters to bcm_dnx_cosq_sysport_ingress_queue_map_set
 */
static shr_error_e
dnx_cosq_sysport_ingress_queue_map_set_verify(
    int unit,
    uint32 flags,
    bcm_gport_t sysport,
    bcm_gport_t ingress_queue)
{
    uint32 sysport_id, supported_flags, is_unmap;
    uint32 curr_base_voq = 0, curr_voq_is_valid = 0, new_base_voq = 0, master_sysport_id = 0;
    int core, is_same_modport = 0, is_allocated;
    bcm_gport_t master_sysport;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify Flags
     */
    supported_flags = BCM_COSQ_SYSPORT_INGRESS_QUEUE_UNMAP;
    SHR_MASK_VERIFY(flags, supported_flags, _SHR_E_PARAM, "non supported flags specified.\n");
    is_unmap = flags & BCM_COSQ_SYSPORT_INGRESS_QUEUE_UNMAP;
    /*
     *  Validate sysport
     */
    sysport_id = BCM_GPORT_SYSTEM_PORT_ID_GET(sysport);
    if (sysport_id > dnx_data_device.general.invalid_system_port_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "system port is out of range. Max system port is %d",
                     dnx_data_device.general.invalid_system_port_get(unit));
    }
    /*
     * Check Core range
     */
    core = BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_GET(ingress_queue);
    DNXCMN_CORE_VALIDATE(unit, core, TRUE);

    /** Get the current output VOQ for the system port */
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_system_port_to_voq_base_get
                    (unit, core, sysport_id, &curr_base_voq, &curr_voq_is_valid));

    new_base_voq = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(ingress_queue);

    /*
     * Check if SYSPORT is mapped to the same VOQ and no unmap flag
     */
    if (curr_voq_is_valid && (new_base_voq == curr_base_voq) && !is_unmap)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Sysport 0x%x already mapped to VOQ 0x%x", sysport, curr_base_voq);
    }

    /*
     * Make sure base voq is allocated
     */
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_queue_is_allocated(unit, core, new_base_voq, &is_allocated));
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Can't map to VOQ 0x%x. VOQ is not allocated.", new_base_voq);
    }
    if (is_unmap)
    {
        /** SYSPORT not mapped to VOQ and unmap flag */
        if (!curr_voq_is_valid)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Can't unmap from VOQ 0x%x. Sysport 0x%x is not currently mapped.", new_base_voq,
                         sysport);
        }
        /** Request to unmap from a different VOQ than the SYSPORT is mapped to */
        else if (curr_base_voq != new_base_voq)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Can't unmap from VOQ 0x%x. Sysport 0x%x is currently mapped to VOQ 0x%x ",
                         new_base_voq, sysport, curr_base_voq);
        }
    }

    /** Get the master sysport for the provided VOQ*/
    SHR_IF_ERR_EXIT(dnx_ipq_queue_sysport_map_get(unit, core, new_base_voq, &master_sysport_id));
    /** Checking that SYSPORT to remove is not MASTER */
    if (is_unmap && sysport_id == master_sysport_id)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Master sysport 0x%x can't be unmapped with this API ", master_sysport_id);
    }

    /** Validate that slave SYSPORT and master SYSPORT map to the same MODPORT */
    BCM_GPORT_SYSTEM_PORT_ID_SET(master_sysport, master_sysport_id);
    SHR_IF_ERR_EXIT(dnx_cosq_sysport_ingress_queue_map_modports_is_equal
                    (unit, sysport, master_sysport, &is_same_modport));
    if (!is_same_modport)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "Master (0x%x) and slave (0x%x) sysport point to differemt modports\n",
                     master_sysport, sysport);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This API allows indirect sysport to VOQ mapping
 *      (SYSPORT -> VOQ -> SYSPORT -> MODPORT)
 *    - API bcm_cosq_ingress_queue_bundle_gport_add allocates VOQ and master sysport
 *    - This API adds slave sysports mapping to the same VOQ
 *    - remove slave sysports with flag BCM_COSQ_SYSPORT_INGRESS_QUEUE_UNMAP
 *    - master sysport can't be unmapped using this API
 *    - When using this feature only sysport can be given as input (not modport)
 *    - This API fails if:
 *      1. given sysport is already mapped to another VOQ
 *      2. given VOQ is not allocated
 *    - This API validates that:
 *      1. given sysport isn't pointing to the same MODPORT as master sysport on the given VOQ
 *
 * \param [in] unit - unit
 * \param [in] flags - flags. BCM_COSQ_SYSPORT_INGRESS_QUEUE_UNMAP.
 * \param [in] sysport - system port gport to map (should be only slave system port)
 * \param [in] ingress_queue - VOQ to map to (gport)
 *
 * \return
 *    shr_error_e
 * \remark
 *   None
 * \see
 *   NONE
 */
shr_error_e
bcm_dnx_cosq_sysport_ingress_queue_map_set(
    int unit,
    uint32 flags,
    bcm_gport_t sysport,
    bcm_gport_t ingress_queue)
{
    int is_replace = 0;
    int is_unmap = flags & BCM_COSQ_SYSPORT_INGRESS_QUEUE_UNMAP;
    uint32 curr_base_voq = 0, voq_is_valid = 0, new_base_voq = 0, sysport_id = 0, queue_quartet, new_queue_quartet;
    uint32 min_bundle_size = dnx_data_ipq.queues.min_bundle_size_get(unit);
    int core, core_idx;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_sysport_ingress_queue_map_set_verify(unit, flags, sysport, ingress_queue));

    new_base_voq = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(ingress_queue);
    core = BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_GET(ingress_queue);
    sysport_id = BCM_GPORT_SYSTEM_PORT_ID_GET(sysport);

    /** Get the current output VOQ for the systemport */
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_system_port_to_voq_base_get
                    (unit, core, sysport_id, &curr_base_voq, &voq_is_valid));

    queue_quartet = curr_base_voq / min_bundle_size;
    if (is_unmap)
    {
        /** Make this destination invalid */
        voq_is_valid = FALSE;
        new_base_voq = 0;
        /** Reference counter of destination will be decreased by 1 */
        DNXCMN_CORES_ITER(unit, core, core_idx)
        {
            SHR_IF_ERR_EXIT(dnx_ipq_db.base_queues.sys_port_ref_counter.dec(unit, core_idx, queue_quartet, 1));
        }
    }
    else
    {
        if (voq_is_valid)
        {
            is_replace = TRUE;
        }
        /** Make this destination invalid */
        voq_is_valid = TRUE;
        /** reference counter of destination will be increased by 1 */
        /** SwState array is with elements equal to NOF of Queue
         *  quartet.That is why SwState is acceessed with
         *  base_queue/min_queue_size */
        new_queue_quartet = new_base_voq / min_bundle_size;
        DNXCMN_CORES_ITER(unit, core, core_idx)
        {
            SHR_IF_ERR_EXIT(dnx_ipq_db.base_queues.sys_port_ref_counter.inc(unit, core_idx, new_queue_quartet, 1));
        }
    }

    /** Unmap/Map new SLAVE_SYSPORT to VOQ */
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_system_port_to_voq_base_set(unit, core, sysport_id, new_base_voq, voq_is_valid));

    /** In case of REPLACE, need to decrement previous VOQ reference counter */
    if (is_replace)
    {
        DNXCMN_CORES_ITER(unit, core, core_idx)
        {
            SHR_IF_ERR_EXIT(dnx_ipq_db.base_queues.sys_port_ref_counter.dec(unit, core_idx, queue_quartet, 1));
        }
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/*
 * brief function to validate the parameters to bcm_dnx_cosq_sysport_ingress_queue_map_get
 */
static shr_error_e
dnx_cosq_sysport_ingress_queue_map_get_verify(
    int unit,
    uint32 flags,
    bcm_gport_t sysport,
    bcm_gport_t * ingress_queue)
{
    uint32 sysport_id, supported_flags;
    int core = BCM_CORE_ALL, core_idx;
    uint32 base_voq = -1, temp_base_voq = -1, voq_is_valid = FALSE, temp_voq_is_valid = FALSE;
    uint32 is_first = TRUE;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ingress_queue, _SHR_E_PARAM, "ingress_queue");

    /*
     *  Validate sysport
     */
    sysport_id = BCM_GPORT_SYSTEM_PORT_ID_GET(sysport);
    if (sysport_id > dnx_data_device.general.invalid_system_port_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "system port is out of range. Max system port is %d",
                     dnx_data_device.general.invalid_system_port_get(unit));
    }

    /*
     * Verify Flags
     */
    supported_flags = BCM_COSQ_SYSPORT_INGRESS_QUEUE_CORE_FROM_GPORT;
    SHR_MASK_VERIFY(flags, supported_flags, _SHR_E_PARAM, "non supported flags specified.\n");

    if (flags & BCM_COSQ_SYSPORT_INGRESS_QUEUE_CORE_FROM_GPORT)
    {
        core = BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_GET(*ingress_queue);
        DNXCMN_CORE_VALIDATE(unit, core, TRUE);
    }

    /*
     *  Validate system port is mapped to the same VOQ in all cores.
     */
    if (core == BCM_CORE_ALL)
    {
        DNXCMN_CORES_ITER(unit, core, core_idx)
        {
            SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_system_port_to_voq_base_get
                            (unit, core_idx, sysport_id, &temp_base_voq, &temp_voq_is_valid));

            if (is_first)
            {
                /** First core */
                base_voq = temp_base_voq;
                voq_is_valid = temp_voq_is_valid;
                is_first = FALSE;
            }
            else
            {
                /** Compare other cores to first core */
                if (voq_is_valid != temp_voq_is_valid)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "system port %d has a different queue mapping in each core", sysport_id);
                }

                if (voq_is_valid)
                {
                    if (base_voq != temp_base_voq)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "system port %d is mapped to different queues in each core (%d != %d)", sysport_id,
                                     base_voq, temp_base_voq);
                    }
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - API to get the VOQ to which the given systemport is mapped to.
 *
 * \param [in] unit - unit
 * \param [in] flags - flags. BCM_COSQ_SYSPORT_INGRESS_QUEUE_UNMAP.
 * \param [in] sysport - system port gport to get the VOQ mapped
 * \param [out] ingress_queue - VOQ mapped to the system port (gport)
 *
 * \return
 *    shr_error_e
 * \remark
 *   None
 * \see
 *   NONE
 */
shr_error_e
bcm_dnx_cosq_sysport_ingress_queue_map_get(
    int unit,
    uint32 flags,
    bcm_gport_t sysport,
    bcm_gport_t * ingress_queue)
{
    uint32 base_voq = 0, sysport_id = 0, voq_is_valid = FALSE;
    int core = BCM_CORE_ALL;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_sysport_ingress_queue_map_get_verify(unit, flags, sysport, ingress_queue));

    sysport_id = BCM_GPORT_SYSTEM_PORT_ID_GET(sysport);

    if (flags & BCM_COSQ_SYSPORT_INGRESS_QUEUE_CORE_FROM_GPORT)
    {
        core = BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_GET(*ingress_queue);
    }

    /** Get the current output VOQ for the systemport */
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_system_port_to_voq_base_get(unit, core, sysport_id, &base_voq, &voq_is_valid));

    if (!voq_is_valid)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "Destination for sysport_id(%d) is invalid, sysport isn't mapped to VOQ", sysport_id);
    }

    BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(*ingress_queue, core, base_voq);

exit:
    SHR_FUNC_EXIT;
}

/*
 * brief - verify Gport type for bcm_dnx_cosq_port_mapping_set/get
 */
static shr_error_e
dnx_cosq_port_mapping_gport_verify(
    int unit,
    bcm_gport_t port)
{
    int is_fmq, is_queue_group, is_sysport;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify port type
     */
    if (BCM_COSQ_GPORT_IS_ISQ(port))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ISQ is not supported");
    }
    /*
     * Supported Gport types are FMQ, VOQ groups and System port.
     */
    is_fmq = BCM_COSQ_GPORT_IS_FMQ_ROOT(port);
    is_queue_group = (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) || BCM_GPORT_IS_MCAST_QUEUE_GROUP(port));
    is_sysport = BCM_GPORT_IS_SYSTEM_PORT(port);
    if (!is_fmq && !is_queue_group && !is_sysport)
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Invalid Port %d", port);
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * brief - Verify function for bcm_dnx_cosq_mapping_set
 */
static shr_error_e
dnx_cosq_mapping_set_verify(
    int unit,
    bcm_cos_t priority,
    bcm_cos_queue_t cosq)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Validate priority
     */
    if (priority < BCM_COS_MIN || priority > BCM_COS_MAX)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid priority %d parameter, valid range is 0-7", priority);
    }

    if (cosq < BCM_COS_MIN || cosq > BCM_COS_MAX)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid cosq %d parameter, valid range is 0-7", cosq);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - basic verify function for bcm_dnx_cosq_port_mapping_multi_set/get
 */
static shr_error_e
dnx_cosq_port_mapping_multi_verify(
    int unit,
    bcm_port_t port,
    int count,
    bcm_cos_t priority[],
    bcm_cos_queue_t cosq[])
{
    SHR_FUNC_INIT_VARS(unit);

    /** Basic verifications only */
    SHR_RANGE_VERIFY(count, 1, DNX_COSQ_NOF_TC, _SHR_E_PARAM, "Invalid count %d\n", count);

    SHR_NULL_CHECK(priority, _SHR_E_PARAM, "priority");
    SHR_NULL_CHECK(cosq, _SHR_E_PARAM, "cosq");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Map system TC (TC on the packet) to VOQ offset mapping
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] port - system port or VOQ gport
 *   \param [in] count - number of TCs to map (size of priority and cosq array)
 *   \param [in] priority - array of TCs
 *   \param [in] cosq - array of VOQ offsets
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_cosq_port_mapping_multi_set(
    int unit,
    bcm_port_t port,
    int count,
    bcm_cos_t * priority,
    bcm_cos_queue_t * cosq)
{
    bcm_cosq_queue_offset_mapping_key_t *key_array = NULL;
    bcm_cosq_queue_offset_t *offset_array = NULL;
    int i;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_port_mapping_multi_verify(unit, port, count, priority, cosq));

    key_array = sal_alloc(sizeof(bcm_cosq_queue_offset_mapping_key_t) * count, "key_array");
    offset_array = sal_alloc(sizeof(bcm_cosq_queue_offset_t) * count, "offset_array");
    for (i = 0; i < count; i++)
    {
        bcm_cosq_queue_offset_mapping_key_t_init(&key_array[i]);
        key_array[i].tc = priority[i];
        offset_array[i].cosq = cosq[i];
    }

    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_queue_offset_mapping_set(unit, 0, port, count, key_array, offset_array));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FREE(key_array);
    SHR_FREE(offset_array);
    SHR_FUNC_EXIT;
}
/*
 * Map system TC (TC on the packet) to VOQ offset, for several queue types (FMQ, sysport, MC/UC)
 */
shr_error_e
bcm_dnx_cosq_port_mapping_set(
    int unit,
    bcm_port_t port,
    bcm_cos_t priority,
    bcm_cos_queue_t cosq)
{
    bcm_cos_t priority_arr[1];
    bcm_cos_queue_t cosq_arr[1];

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    priority_arr[0] = priority;
    cosq_arr[0] = cosq;

    SHR_IF_ERR_EXIT(bcm_dnx_cosq_port_mapping_multi_set(unit, port, 1, priority_arr, cosq_arr));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/*
 * brief - Verify function for bcm_dnx_cosq_mapping_get
 */
static shr_error_e
dnx_cosq_mapping_get_verify(
    int unit,
    bcm_cos_t priority,
    bcm_cos_queue_t * cosq)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(cosq, _SHR_E_PARAM, "cosq");

    /*
     * Validate priority
     */
    if (priority < BCM_COS_MIN || priority > BCM_COS_MAX)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid priority %d parameter, valid range is 0-7", priority);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get system TC (TC on the packet) to VOQ offset mapping
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] port - system port or VOQ gport
 *   \param [in] count - number of TCs to map (size of priority and cosq array)
 *   \param [in] priority - array of TCs
 *   \param [out] cosq - array of VOQ offsets
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_cosq_port_mapping_multi_get(
    int unit,
    bcm_port_t port,
    int count,
    bcm_cos_t * priority,
    bcm_cos_queue_t * cosq)
{

    bcm_cosq_queue_offset_mapping_key_t *key_array = NULL;
    bcm_cosq_queue_offset_t *offset_array = NULL;
    int i;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_port_mapping_multi_verify(unit, port, count, priority, cosq));

    key_array = sal_alloc(sizeof(bcm_cosq_queue_offset_mapping_key_t) * count, "key_array");
    offset_array = sal_alloc(sizeof(bcm_cosq_queue_offset_t) * count, "offset_array");

    for (i = 0; i < count; i++)
    {
        bcm_cosq_queue_offset_mapping_key_t_init(&key_array[i]);
        key_array[i].tc = priority[i];
    }

    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_queue_offset_mapping_get(unit, 0, port, count, key_array, offset_array));

    for (i = 0; i < count; i++)
    {
        cosq[i] = offset_array[i].cosq;
    }

exit:
    SHR_FREE(key_array);
    SHR_FREE(offset_array);
    SHR_FUNC_EXIT;
}

/*
 * Get mapping of system TC (TC on the packet) to VOQ offset, for several queue types (FMQ, sysport, MC/UC)
 */
shr_error_e
bcm_dnx_cosq_port_mapping_get(
    int unit,
    bcm_port_t port,
    bcm_cos_t priority,
    bcm_cos_queue_t * cosq)
{
    bcm_cos_t priority_arr[1];
    bcm_cos_queue_t cosq_arr[1];

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    priority_arr[0] = priority;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_port_mapping_multi_get(unit, port, 1, priority_arr, cosq_arr));
    *cosq = cosq_arr[0];

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_mapping_default_profile_mapping_set(
    int unit,
    int is_flow,
    bcm_cos_t sys_tc,
    bcm_cos_queue_t voq_offset)
{
    dnx_ipq_tc_map_t tc_mapping;
    uint32 tc_voq_flow_profile = dnx_data_ipq.tc_map.default_voq_flow_profile_idx_get(unit);
    uint32 tc_sysport_profile = dnx_data_ipq.tc_map.default_sysport_profile_idx_get(unit);
    uint32 tc_profile;
    int ref_count;
    int dp, nof_dps;
    int is_fmq = FALSE;
    int core_idx = 0;

    SHR_FUNC_INIT_VARS(unit);

    tc_profile = is_flow ? tc_voq_flow_profile : tc_sysport_profile;
    nof_dps = dnx_data_ipq.queues.feature_get(unit, dnx_data_ipq_queues_voq_resolution_per_dp) ? DNX_COSQ_NOF_DP : 1;

    for (dp = 0; dp < nof_dps; dp++)
    {
        SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_tc_to_voq_offset_map_set
                        (unit, BCM_CORE_ALL, sys_tc, dp, tc_profile, is_flow, is_fmq, voq_offset));
    }
    /*
     * Change the data on Template manager as well
     */
    if (is_flow)
    {
        DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_idx)
        {
            SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.
                            tc_voq_flow_map.profile_data_get(unit, core_idx, tc_profile, &ref_count, &tc_mapping));
            for (dp = 0; dp < nof_dps; dp++)
            {
                tc_mapping.offset[sys_tc][dp] = voq_offset;
            }
            SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.
                            tc_voq_flow_map.replace_data(unit, core_idx, tc_profile, &tc_mapping));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.
                        tc_sysport_map.profile_data_get(unit, tc_profile, &ref_count, &tc_mapping));
        for (dp = 0; dp < nof_dps; dp++)
        {
            tc_mapping.offset[sys_tc][dp] = voq_offset;
        }
        SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.tc_sysport_map.replace_data(unit, tc_profile, &tc_mapping));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * brief - Map from system TC (TC on the packet) to a VOQ offset for the default TC-profile.
 */
shr_error_e
bcm_dnx_cosq_mapping_set(
    int unit,
    bcm_cos_t priority,
    bcm_cos_queue_t cosq)
{
    uint32 is_flow, sys_tc, voq_offset;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_mapping_set_verify(unit, priority, cosq));

    /*
     * Global mapping between PP TC and Ingress TC.
     * By default, mapping is 1:1
     */
    sys_tc = priority;
    voq_offset = cosq;

    /*
     * we dont know if the user want to set the default profile for Flow or sysport based forwarding,
     * so we set the default profile in both.
     */
    is_flow = TRUE;
    SHR_IF_ERR_EXIT(dnx_cosq_mapping_default_profile_mapping_set(unit, is_flow, sys_tc, voq_offset));
    is_flow = FALSE;
    SHR_IF_ERR_EXIT(dnx_cosq_mapping_default_profile_mapping_set(unit, is_flow, sys_tc, voq_offset));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/*
 * get the mapping from system TC (TC on the packet) to VOQ offset, for the default TC-profile.
 */
shr_error_e
bcm_dnx_cosq_mapping_get(
    int unit,
    bcm_cos_t priority,
    bcm_cos_queue_t * cosq)
{
    uint32 sys_tc, voq_offset, is_flow, tc_profile;
    int dp = 0;
    int is_fmq = FALSE;
    int core = BCM_CORE_ALL;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_mapping_get_verify(unit, priority, cosq));

    sys_tc = priority;
    is_flow = FALSE;
    tc_profile = dnx_data_ipq.tc_map.default_sysport_profile_idx_get(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_tc_to_voq_offset_map_get(unit, core, sys_tc, dp, tc_profile, is_flow, is_fmq,
                                                               &voq_offset));

    *cosq = voq_offset;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_gport_queue_offset_mapping_set_verify(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    int count,
    bcm_cosq_queue_offset_mapping_key_t * key_array,
    bcm_cosq_queue_offset_t * offset_array)
{

    int mapped_key[DNX_COSQ_NOF_TC][DNX_COSQ_NOF_DP] = { {0} };
    int start_dp, end_dp, dp, tc;
    int i;
    int nof_dps;

    SHR_FUNC_INIT_VARS(unit);

    SHR_MASK_VERIFY(flags, 0, _SHR_E_PARAM, "no flags supported.\n");
    SHR_NULL_CHECK(key_array, _SHR_E_PARAM, "key_array");
    SHR_NULL_CHECK(offset_array, _SHR_E_PARAM, "offset_array");

    if (count < 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Count (%d) should be at least 1 \n", count);
    }

    nof_dps = dnx_data_ipq.queues.feature_get(unit, dnx_data_ipq_queues_voq_resolution_per_dp) ? DNX_COSQ_NOF_DP : 1;

    /*
     * Verify TC and VOQ offset
     */
    for (i = 0; i < count; i++)
    {
        if (dnx_data_ipq.queues.feature_get(unit, dnx_data_ipq_queues_voq_resolution_per_dp))
        {
            SHR_RANGE_VERIFY(key_array[i].dp, -1, DNX_COSQ_NOF_DP - 1, _SHR_E_PARAM,
                             "Invalid dp %d\n", key_array[i].dp);
        }
        else
        {
            SHR_VAL_VERIFY(key_array[i].dp, BCM_COS_INVALID, _SHR_E_PARAM, "dp must be BCM_COS_INVALID\n");
        }

        SHR_RANGE_VERIFY(key_array[i].tc, BCM_COS_MIN, BCM_COS_MAX, _SHR_E_PARAM, "Invalid tc %d\n", key_array[i].tc);
        SHR_RANGE_VERIFY(offset_array[i].cosq, BCM_COS_MIN, BCM_COS_MAX, _SHR_E_PARAM,
                         "Invalid cosq %d\n", offset_array[i].cosq);

        if (key_array[i].dp == BCM_COS_INVALID)
        {
            start_dp = 0;
            end_dp = nof_dps;
        }
        else
        {
            start_dp = key_array[i].dp;
            end_dp = key_array[i].dp + 1;

        }

        /** verify uniqness */
        for (dp = start_dp; dp < end_dp; dp++)
        {
            tc = key_array[i].tc;
            if (mapped_key[tc][dp])
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Key (%d,%d) is mapped more than once\n", key_array[i].tc, key_array[i].dp);
            }
            mapped_key[tc][dp] = 1;
        }
    }

    /*
     * Verify Gport type
     */
    SHR_IF_ERR_EXIT(dnx_cosq_port_mapping_gport_verify(unit, gport));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Map system TC (TC on the packet) and other parameters as supported by device to VOQ offset
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] flags - flags
 *   \param [in] gport - system port gport or VOQ gport
 *   \param [in] count - number of keys to map (size of key_array and offset_array)
 *   \param [in] key_array - array of keys
 *   \param [in] offset_array - array of VOQ offsets
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_cosq_gport_queue_offset_mapping_set(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    int count,
    bcm_cosq_queue_offset_mapping_key_t * key_array,
    bcm_cosq_queue_offset_t * offset_array)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_gport_queue_offset_mapping_set_verify(unit, flags, gport, count,
                                                                         key_array, offset_array));

    if (BCM_COSQ_GPORT_IS_FMQ_ROOT(gport))
    {
        /*
         * FMQ mapping
         */
        SHR_IF_ERR_EXIT(dnx_cosq_ipq_fmq_voq_offset_mapping_set(unit, count, key_array, offset_array));
    }
    else
    {
        /*
         * Queue group / sysport mapping
         */
        SHR_IF_ERR_EXIT(dnx_cosq_ipq_voq_offset_mapping_set(unit, gport, count, key_array, offset_array));

    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/*
 * brief - verify function for bcm_dnx_cosq_gport_queue_offset_mapping_get
 */
static shr_error_e
dnx_cosq_gport_queue_offset_mapping_get_verify(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    int count,
    bcm_cosq_queue_offset_mapping_key_t * key_array,
    bcm_cosq_queue_offset_t * offset_array)
{
    int i;

    SHR_FUNC_INIT_VARS(unit);

    if (count < 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Count (%d) should be at least 1 \n", count);
    }

    SHR_MASK_VERIFY(flags, 0, _SHR_E_PARAM, "no flags supported.\n");
    SHR_NULL_CHECK(key_array, _SHR_E_PARAM, "key_array");
    SHR_NULL_CHECK(offset_array, _SHR_E_PARAM, "offset_array");

    /*
     * Verify TC and DP
     */
    for (i = 0; i < count; i++)
    {
        if (dnx_data_ipq.queues.feature_get(unit, dnx_data_ipq_queues_voq_resolution_per_dp))
        {
            SHR_RANGE_VERIFY(key_array[i].dp, -1, DNX_COSQ_NOF_DP - 1, _SHR_E_PARAM,
                             "Invalid dp %d\n", key_array[i].dp);
        }
        else
        {
            SHR_VAL_VERIFY(key_array[i].dp, BCM_COS_INVALID, _SHR_E_PARAM, "dp must be BCM_COS_INVALID\n");
        }

        SHR_RANGE_VERIFY(key_array[i].tc, BCM_COS_MIN, BCM_COS_MAX, _SHR_E_PARAM, "Invalid tc %d\n", key_array[i].tc);

    }

    /*
     * Verify Gport type
     */
    SHR_IF_ERR_EXIT(dnx_cosq_port_mapping_gport_verify(unit, gport));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get system TC (TC on the packet) and other parameters as supported by device to VOQ offset mapping
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] flags - flags
 *   \param [in] gport - system port gport or VOQ gport
 *   \param [in] count - number of keys to map (size of key_array and offset_array)
 *   \param [in] key_array - array of keys
 *   \param [in] offset_array - array of VOQ offsets
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_cosq_gport_queue_offset_mapping_get(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    int count,
    bcm_cosq_queue_offset_mapping_key_t * key_array,
    bcm_cosq_queue_offset_t * offset_array)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_gport_queue_offset_mapping_get_verify(unit, flags, gport, count,
                                                                         key_array, offset_array));

    if (BCM_COSQ_GPORT_IS_FMQ_ROOT(gport))
    {
        /*
         * FMQ mapping
         */
        SHR_IF_ERR_EXIT(dnx_cosq_ipq_fmq_voq_offset_mapping_get(unit, count, key_array, offset_array));
    }
    else
    {
        /*
         * Queue group / sysport mapping
         */
        SHR_IF_ERR_EXIT(dnx_cosq_ipq_voq_offset_mapping_get(unit, gport, count, key_array, offset_array));

    }
exit:
    SHR_FUNC_EXIT;
}
