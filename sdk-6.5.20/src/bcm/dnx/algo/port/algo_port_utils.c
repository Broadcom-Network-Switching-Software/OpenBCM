/** \file algo_port_utils.c
 *
 *  PORT manager, Port utilities, and Port verifiers.
 *
 *  Algo port managment:
 *  * MGMT APIs
 *    - Used to configure algo port module
 *    - located in algo_port_mgmt.h (source code algo_port_mgmt.c)
 *  * Utility APIs
 *    - Provide utilities functions which will make the BCM module cleaner and simpler
 *    - located in algo_port_utils.h (source code algo_port_utils.c
 *  * Verify APIs
 *    - Provide a set of APIs which verify port attributes
 *    - These verifiers used to verify algo port functions input.
 *    - These verifiers can be used out side of the module, if requried.
 *    - located in algo_port_verify.h (source code algo_port_verify.c)
 *  * SOC APIs
 *    - Provide set of functions to set / restore  and remove soc info data
 *    - Only the port related members will be configured by this module (other modules should not set those memebers directly)
 *    - The relevant members are: 'port to block access', 'port names' and 'port bitmaps'
 *    - located in algo_port_soc.c (all the functions are module internals)
 *  * Module internal definitions - algo_port_internal.h
 *  * Data bases - use sw state mechanism - XML file is algo_port.xml
 *
 *  No need to include each sub module seperately.
 *  Including: 'algo_port_mgmt.h' will includes all the sub modules
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

/*
 * Include files.
 * {
 */

#include <bcm/types.h>

#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_bitstream.h>
#include <stdio.h>
#include <stdlib.h>

#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>

#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/init/init_time_analyzer.h>
#include <shared/shrextend/shrextend_debug.h>

#include "algo_port_internal.h"

/*
 * }
 */

shr_error_e
dnx_algo_port_next_logical_on_if_get(
    int unit,
    bcm_core_t core,
    int egr_if,
    bcm_port_t * logical_port_p)
{
    bcm_pbmp_t pbmp;
    int egr_if_i;
    int found, find_first;
    bcm_port_t logical_port_i;
    bcm_port_t input_logical_port;

    SHR_FUNC_INIT_VARS(unit);
    found = FALSE;
    input_logical_port = *logical_port_p;
    if (input_logical_port == DNX_ALGO_PORT_INVALID)
    {
        find_first = TRUE;
    }
    else
    {
        find_first = FALSE;
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, core, DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING, 0, &pbmp));
    BCM_PBMP_ITER(pbmp, logical_port_i)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, logical_port_i, &egr_if_i));
        if (egr_if_i == egr_if)
        {
            if (find_first == TRUE)
            {
                found = TRUE;
                *logical_port_p = logical_port_i;
                break;
            }
            else
            {
                if (logical_port_i == input_logical_port)
                {
                    find_first = TRUE;
                    continue;
                }
            }
        }
    }
    if (found != TRUE)
    {
        *logical_port_p = DNX_ALGO_PORT_INVALID;
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See algo_port_utils.h file
 */
shr_error_e
dnx_algo_port_logicals_get(
    int unit,
    bcm_core_t core,
    dnx_algo_port_logicals_type_e logicals_type,
    uint32 flags,
    bcm_pbmp_t * logicals)
{
    dnx_algo_port_state_e state;
    int handle;
    dnx_algo_port_db_2d_handle_t if_handle;
    bcm_port_t logical_port, master_logical_port;
    uint32 supported_flags;
    bcm_core_t logical_port_core;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);
    DNX_INIT_TIME_ANALYZER_ALGO_PORT_START(unit, DNX_INIT_TIME_ANALYZER_ALGO_PORT_BITMAP);

    /** clear bitmap */
    BCM_PBMP_CLEAR(*logicals);

    /** per type implementation */
    switch (logicals_type)
    {
            /*
             *  bitmap of free ports
             */
        case DNX_ALGO_PORT_LOGICALS_TYPE_INVALID:
        {
            /** This port type can be supported only for all cores */
            if (core != BCM_CORE_ALL)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Core must be BCM_CORE_ALL .\n");
            }

            /** throw an error if unsupported flag requested */
            supported_flags = 0;
            SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                                     "Get invalid logicals flags does not supported the requested flags 0x%x %s%s.\n",
                                     flags, EMPTY, EMPTY);

            /** Iterate all ports and just the relevants to bitmap */
            for (logical_port = 0; logical_port < SOC_MAX_NUM_PORTS; logical_port++)
            {
                /** filter valid ports to bitmap */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.get(unit, logical_port, &state));
                if (state != DNX_ALGO_PORT_STATE_INVALID)
                {
                    continue;
                }

                /** add to bitmap */
                BCM_PBMP_PORT_ADD(*logicals, logical_port);
            }
            break;
        }
            /*
             *  bitmap of valid (used) ports
             */
        case DNX_ALGO_PORT_LOGICALS_TYPE_VALID:
        {
            /** This port type can be supported only for all cores */
            if (core != BCM_CORE_ALL)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Core must be BCM_CORE_ALL .\n");
            }

            /** throw an error if unsupported flag requested */
            supported_flags = 0;
            SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                                     "Get valid logicals flags does not support the requested flags 0x%x %s%s.\n",
                                     flags, EMPTY, EMPTY);

            /** Iterate all ports and just the relevants to bitmap */
            for (logical_port = 0; logical_port < SOC_MAX_NUM_PORTS; logical_port++)
            {
                /** filter invalid ports */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.get(unit, logical_port, &state));
                if (state == DNX_ALGO_PORT_STATE_INVALID)
                {
                    continue;
                }

                /** add to bitmap */
                BCM_PBMP_PORT_ADD(*logicals, logical_port);
            }
            break;
        }
            /*
             * bitmap of fabric ports
             * (logical ports with fabric handles)
             */
        case DNX_ALGO_PORT_LOGICALS_TYPE_FABRIC:
        {
            /** throw an error if unsupported flag requested */
            supported_flags = 0;
            SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                                     "Get fabric logicals flags does not supported the requested flags 0x%x %s%s.\n",
                                     flags, EMPTY, EMPTY);

            /** Iterate all ports and just the relevants to bitmap */
            for (logical_port = 0; logical_port < SOC_MAX_NUM_PORTS; logical_port++)
            {
                /** filter free ports */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.get(unit, logical_port, &state));
                if (state == DNX_ALGO_PORT_STATE_INVALID)
                {
                    continue;
                }

                /** filter invalid fabric handles*/
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.fabric_handle.get(unit, logical_port, &handle));
                if (handle == DNX_ALGO_PORT_HANDLE_INVALID)
                {
                    continue;
                }

                /** add to bitmap */
                BCM_PBMP_PORT_ADD(*logicals, logical_port);
            }
            break;
        }
            /*
             * bitmap of nif ports
             * (logical ports with valid nif handle)
             * Additonal options: master only flag
             */
        case DNX_ALGO_PORT_LOGICALS_TYPE_NIF:
        {
            /** throw an error if unsupported flag requested */
            supported_flags =
                DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY | DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_ELK |
                DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_STIF | DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_FLEXE_PHY;
            SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                                     "Get nif logicals flags does not supported the requested flags 0x%x %s%s.\n",
                                     flags, EMPTY, EMPTY);

            /** Iterate all ports and just add just the relevant to bitmap */
            for (logical_port = 0; logical_port < SOC_MAX_NUM_PORTS; logical_port++)
            {
                /** filter free ports */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.get(unit, logical_port, &state));
                if (state == DNX_ALGO_PORT_STATE_INVALID)
                {
                    continue;
                }

                /** filter invalid nif handles*/
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.nif_handle.get(unit, logical_port, &handle));
                if (handle == DNX_ALGO_PORT_HANDLE_INVALID)
                {
                    continue;
                }

                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.info.get(unit, logical_port, &port_info));
                /** filter port types ILKN ELK if EXCLUDE_ELK flag is set */
                if ((flags & DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_ELK)
                    && DNX_ALGO_PORT_TYPE_IS_NIF_ILKN_ELK(unit, port_info))
                {
                    continue;
                }
                /** filter port types FLEXE_PHY  if EXCLUDE_FLEXE_PHY flag is set */
                if ((flags & DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_FLEXE_PHY)
                    && DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info))
                {
                    continue;
                }
                /** filter port types ETH_STIF  if EXCLUDE_STIF flag is set */
                if ((flags & DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_STIF)
                    && DNX_ALGO_PORT_TYPE_IS_NIF_ETH_STIF(unit, port_info))
                {
                    continue;
                }

                /** in case specific core asked - filter according to core */
                if (core != BCM_CORE_ALL)
                {

                    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.interface_handle.get(unit, logical_port, &if_handle));
                    if (!DNX_ALGO_PORT_IS_2D_HANDLE_VALID(if_handle))
                    {
                        continue;
                    }
                                        /** get core  and filter */
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.
                                    core.get(unit, if_handle.h0, if_handle.h1, &logical_port_core));

                    if (core != logical_port_core)
                    {
                        continue;
                    }
                }
                /*
                 * filter non-master port (if requested).
                 */
                if (flags & DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.nif_handle.get(unit, logical_port, &handle));
                    if (handle == DNX_ALGO_PORT_HANDLE_INVALID)
                    {
                        continue;
                    }

                    SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.master_logical_port.get(unit, handle, &master_logical_port));
                    if (master_logical_port != logical_port)
                    {
                        continue;
                    }
                }

                /** add to bitmap */
                BCM_PBMP_PORT_ADD(*logicals, logical_port);
            }
            break;
        }
            /*
             * bitmap of nif ethrnet ports
             * (logical ports with port type == DNX_ALGO_PORT_TYPE_NIF_ETH)
             * Additonal options: master only flag
             */
        case DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ETH:
        {
            /** throw an error if unsupported flag requested */
            supported_flags = DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_L1 | DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY;
            SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                                     "Get nif logicals flags does not supported the requested flags 0x%x %s%s.\n",
                                     flags, EMPTY, EMPTY);

            /** Iterate all ports and just the relevants to bitmap */
            for (logical_port = 0; logical_port < SOC_MAX_NUM_PORTS; logical_port++)
            {
                /** filter free ports */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.get(unit, logical_port, &state));
                if (state == DNX_ALGO_PORT_STATE_INVALID)
                {
                    continue;
                }

                /** filter port types different from NIF ETH*/
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.info.get(unit, logical_port, &port_info));
                if (!DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, 1, 1))
                {
                    continue;
                }
                /** filter port types ETH L1 if EXCLUDE_L1 flag is set */
                if ((flags & DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_L1) && (DNX_ALGO_PORT_TYPE_IS_L1(unit, port_info)))
                {
                    continue;
                }
                /** in case specific core asked - filter according to core */
                if (core != BCM_CORE_ALL)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.interface_handle.get(unit, logical_port, &if_handle));
                    if (!DNX_ALGO_PORT_IS_2D_HANDLE_VALID(if_handle))
                    {
                        continue;
                    }
                    /** get core  and filter */
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.
                                    core.get(unit, if_handle.h0, if_handle.h1, &logical_port_core));
                    if (core != logical_port_core)
                    {
                        continue;
                    }
                }

                /** filter non-master port (if requested)*/
                if (flags & DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.nif_handle.get(unit, logical_port, &handle));
                    if (handle == DNX_ALGO_PORT_HANDLE_INVALID)
                    {
                        /** NIF is not defined as master */
                        continue;
                    }

                    SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.master_logical_port.get(unit, handle, &master_logical_port));
                    if (master_logical_port != logical_port)
                    {
                        continue;
                    }
                }

                /** add to bitmap */
                BCM_PBMP_PORT_ADD(*logicals, logical_port);
            }
            break;
        }
            /*
             * bitmap of nif FlexE physical ports
             * (logical ports with port type == DNX_ALGO_PORT_TYPE_FLEXE_PHY)
             */
        case DNX_ALGO_PORT_LOGICALS_TYPE_FLEXE_PHY:
        {
            /** throw an error if unsupported flag requested */
            supported_flags = DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_REMOVED_PORT;
            SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                                     "Get nif logicals flags does not supported the requested flags 0x%x %s%s.\n",
                                     flags, EMPTY, EMPTY);

            /** Iterate all ports and just the relevants to bitmap */
            for (logical_port = 0; logical_port < SOC_MAX_NUM_PORTS; logical_port++)
            {
                /** filter free ports */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.get(unit, logical_port, &state));
                if (state == DNX_ALGO_PORT_STATE_INVALID)
                {
                    continue;
                }
                if ((flags & DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_REMOVED_PORT) && (state == DNX_ALGO_PORT_STATE_REMOVED))
                {
                    continue;
                }
                /** filter port types different from NIF ETH*/
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.info.get(unit, logical_port, &port_info));
                if (!DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info))
                {
                    continue;
                }

                /** in case specific core asked - filter according to core */
                if (core != BCM_CORE_ALL)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.interface_handle.get(unit, logical_port, &if_handle));
                    if (!DNX_ALGO_PORT_IS_2D_HANDLE_VALID(if_handle))
                    {
                        continue;
                    }
                    /** get core  and filter */
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.
                                    core.get(unit, if_handle.h0, if_handle.h1, &logical_port_core));
                    if (core != logical_port_core)
                    {
                        continue;
                    }
                }

                /** add to bitmap */
                BCM_PBMP_PORT_ADD(*logicals, logical_port);
            }
            break;
        }
            /*
             * bitmap of nif FlexE MAC Client
             * (logical ports with port type == DNX_ALGO_PORT_TYPE_FLEXE_MAC)
             */
        case DNX_ALGO_PORT_LOGICALS_TYPE_FLEXE_MAC:
        {
            int include_l1;
            /** throw an error if unsupported flag requested */
            supported_flags = DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_L1 | DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_REMOVED_PORT;
            SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                                     "Get nif logicals flags does not supported the requested flags 0x%x %s%s.\n",
                                     flags, EMPTY, EMPTY);

            /** Iterate all ports and just the relevants to bitmap */
            for (logical_port = 0; logical_port < SOC_MAX_NUM_PORTS; logical_port++)
            {
                /** filter free ports */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.get(unit, logical_port, &state));
                if (state == DNX_ALGO_PORT_STATE_INVALID)
                {
                    continue;
                }
                if ((flags & DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_REMOVED_PORT) && (state == DNX_ALGO_PORT_STATE_REMOVED))
                {
                    continue;
                }
                /** filter port types different from FLEXE_MAC / MAC_L1*/
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.info.get(unit, logical_port, &port_info));
                include_l1 = !(flags & DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_L1);
                if (!DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC(unit, port_info, include_l1))
                {
                    continue;
                }
                /** in case specific core asked - filter according to core */
                if (core != BCM_CORE_ALL)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.interface_handle.get(unit, logical_port, &if_handle));
                    if (!DNX_ALGO_PORT_IS_2D_HANDLE_VALID(if_handle))
                    {
                        continue;
                    }
                    /** get core  and filter */
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.
                                    core.get(unit, if_handle.h0, if_handle.h1, &logical_port_core));
                    if (core != logical_port_core)
                    {
                        continue;
                    }
                }

                /** add to bitmap */
                BCM_PBMP_PORT_ADD(*logicals, logical_port);
            }
            break;
        }
            /*
             * bitmap of nif ILKN ports
             * If the logicals_type is DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN:
             *     (logical ports with port type == DNX_ALGO_PORT_TYPE_NIF_ILKN || DNX_ALGO_PORT_TYPE_NIF_ILKN_ELK)
             *     Additonal options: master only flag and exclude ELK flag
             * If the logicals_type is DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN_ELK:
             *     (logical ports with port type == DNX_ALGO_PORT_TYPE_NIF_ILKN_ELK)
             *     Additonal options: None
             */
        case DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN:
        case DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN_ELK:
        {
            /** throw an error if unsupported flag requested */
            supported_flags =
                (logicals_type ==
                 DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN_ELK) ? 0 : (DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY |
                                                                  DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_ELK);
            SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                                     "Get nif logicals flags does not supported the requested flags 0x%x %s%s.\n",
                                     flags, EMPTY, EMPTY);

            /** Iterate all ports and just the relevants to bitmap */
            for (logical_port = 0; logical_port < SOC_MAX_NUM_PORTS; logical_port++)
            {
                /** filter free ports */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.get(unit, logical_port, &state));
                if (state == DNX_ALGO_PORT_STATE_INVALID)
                {
                    continue;
                }

                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.info.get(unit, logical_port, &port_info));
                /** filter port types different from ILKN and ILKN ELK */
                if (!DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 1 /* elk */ , 1 /* L1 */ ))
                {
                    continue;
                }
                /** filter port types ILKN ELK if EXCLUDE_ELK flag is set */
                if ((flags & DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_ELK)
                    && DNX_ALGO_PORT_TYPE_IS_NIF_ILKN_ELK(unit, port_info))
                {
                    continue;
                }
                /** filter port types NON-ILKN ELK if logicals type is ILKN ELK */
                if ((logicals_type == DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN_ELK)
                    && !DNX_ALGO_PORT_TYPE_IS_NIF_ILKN_ELK(unit, port_info))
                {
                    continue;
                }

                /** in case specific core asked - filter according to core */
                if (core != BCM_CORE_ALL)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.interface_handle.get(unit, logical_port, &if_handle));
                    if (!DNX_ALGO_PORT_IS_2D_HANDLE_VALID(if_handle))
                    {
                        continue;
                    }
                                        /** get core id */
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.
                                    core.get(unit, if_handle.h0, if_handle.h1, &logical_port_core));
                    /** filter the un-wanted core */
                    if (core != logical_port_core)
                    {
                        continue;
                    }
                }

                /** filter non-master port (if requested) */
                if (flags & DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.ilkn_handle.get(unit, logical_port, &handle));
                    if (handle == DNX_ALGO_PORT_HANDLE_INVALID)
                    {
                        continue;
                    }
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.ilkn.master_logical_port.get(unit, handle, &master_logical_port));
                    if (master_logical_port != logical_port)
                    {
                        continue;
                    }
                }
                /** add to bitmap */
                BCM_PBMP_PORT_ADD(*logicals, logical_port);
            }
            break;
        }

            /*
             * bitmap of tm ports
             * (logical ports with valid tm handle)
             * Additonal options: master only flag.
             * Assuming that every tm port is pp port and vice versa
             */
        case DNX_ALGO_PORT_LOGICALS_TYPE_PP:
        case DNX_ALGO_PORT_LOGICALS_TYPE_TM:
        case DNX_ALGO_PORT_LOGICALS_TYPE_TM_ING:
        case DNX_ALGO_PORT_LOGICALS_TYPE_TM_E2E_SCH:
        case DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING:
        case DNX_ALGO_PORT_LOGICALS_TYPE_CPU:
        case DNX_ALGO_PORT_LOGICALS_TYPE_SAT:
        {
            /** throw an error if unsupported flag requested */
            supported_flags = DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY;
            SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                                     "Get tm logicals flags does not supported the requested flags 0x%x %s%s.\n", flags,
                                     EMPTY, EMPTY);

            /** Iterate all ports and just the relevants to bitmap */
            for (logical_port = 0; logical_port < SOC_MAX_NUM_PORTS; logical_port++)
            {
                /** filter free ports */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.get(unit, logical_port, &state));
                if (state == DNX_ALGO_PORT_STATE_INVALID)
                {
                    continue;
                }

                /** filter invalid tm handles*/
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.info.get(unit, logical_port, &port_info));
                if (!(DNX_ALGO_PORT_TYPE_IS_TM(unit, port_info) ||
                      (logicals_type == DNX_ALGO_PORT_LOGICALS_TYPE_TM_E2E_SCH &&
                       DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, port_info))))
                {
                    continue;
                }

                 /** in case specific core asked - filter acorrding to core */
                if (core != BCM_CORE_ALL)
                {
                    /** get core  and filter */
                    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &logical_port_core));
                    if (core != logical_port_core)
                    {
                        continue;
                    }
                }

                /** filter non-master port (if requested)*/
                if (flags & DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.interface_handle.get(unit, logical_port, &if_handle));
                    if (!DNX_ALGO_PORT_IS_2D_HANDLE_VALID(if_handle))
                    {
                        continue;
                    }

                    SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.
                                    master_logical_port.get(unit, if_handle.h0, if_handle.h1, &master_logical_port));
                    if (master_logical_port != logical_port)
                    {
                        continue;
                    }
                }

                /** filter ports according to port type */
                /** Note that DNX_ALGO_PORT_LOGICALS_TYPE_TM is already filtered before */
                if (logicals_type == DNX_ALGO_PORT_LOGICALS_TYPE_TM_ING)
                {
                    if (!DNX_ALGO_PORT_TYPE_IS_ING_TM(unit, port_info))
                    {
                        continue;
                    }
                }
                if (logicals_type == DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING)
                {
                    if (!DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
                    {
                        continue;
                    }
                }
                else if (logicals_type == DNX_ALGO_PORT_LOGICALS_TYPE_TM_E2E_SCH)
                {
                    if (!DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, port_info))
                    {
                        continue;
                    }
                }
                else if (logicals_type == DNX_ALGO_PORT_LOGICALS_TYPE_PP)
                {
                    int is_lag;
                    SHR_IF_ERR_EXIT(dnx_algo_port_in_lag(unit, logical_port, &is_lag));
                    if (!DNX_ALGO_PORT_TYPE_IS_PP(unit, port_info, is_lag))
                    {
                        continue;
                    }
                }
                else if (logicals_type == DNX_ALGO_PORT_LOGICALS_TYPE_CPU)
                {
                    if (!DNX_ALGO_PORT_TYPE_IS_CPU(unit, port_info))
                    {
                        continue;
                    }
                }
                else if (logicals_type == DNX_ALGO_PORT_LOGICALS_TYPE_SAT)
                {
                    if (!DNX_ALGO_PORT_TYPE_IS_SAT(unit, port_info))
                    {
                        continue;
                    }
                }

                /** add to bitmap */
                BCM_PBMP_PORT_ADD(*logicals, logical_port);
            }

            break;
        }
        case DNX_ALGO_PORT_LOGICALS_TYPE_L1:
        {
            /** Throw an error if unsupported flag requested */
            supported_flags = DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY;
            SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                                     "Get tm logicals flags does not supported the requested flags 0x%x %s%s.\n", flags,
                                     EMPTY, EMPTY);

            /** Iterate all ports and add just the relevant to bitmap */
            for (logical_port = 0; logical_port < SOC_MAX_NUM_PORTS; logical_port++)
            {
                /** filter free ports */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.get(unit, logical_port, &state));
                if (state == DNX_ALGO_PORT_STATE_INVALID)
                {
                    continue;
                }

                /** Filter non-L1 ports*/
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.info.get(unit, logical_port, &port_info));
                if (!DNX_ALGO_PORT_TYPE_IS_L1(unit, port_info))
                {
                    continue;
                }

                 /** in case specific core asked - filter according to core */
                if (core != BCM_CORE_ALL)
                {
                    /** get core  and filter */
                    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &logical_port_core));
                    if (core != logical_port_core)
                    {
                        continue;
                    }
                }

                /** filter non-master port (if requested)*/
                if (flags & DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.interface_handle.get(unit, logical_port, &if_handle));
                    if (!DNX_ALGO_PORT_IS_2D_HANDLE_VALID(if_handle))
                    {
                        continue;
                    }

                    SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.
                                    master_logical_port.get(unit, if_handle.h0, if_handle.h1, &master_logical_port));
                    if (master_logical_port != logical_port)
                    {
                        continue;
                    }
                }

                /** add to bitmap */
                BCM_PBMP_PORT_ADD(*logicals, logical_port);
            }

            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported logicals type %d .\n", logicals_type);
            break;
        }
    }

exit:
    DNX_INIT_TIME_ANALYZER_ALGO_PORT_STOP(unit, DNX_INIT_TIME_ANALYZER_ALGO_PORT_BITMAP);
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_channels_get(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    bcm_pbmp_t * logical_port_channnels)
{
    uint32 supported_flags;
    bcm_port_t master_port, master_valid_port;
    bcm_pbmp_t valid_ports;
    bcm_port_t valid_port;
    dnx_algo_port_tdm_mode_e tdm_mode;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    supported_flags = DNX_ALGO_PORT_CHANNELS_F_MASK;
    SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                             "Get channels flags does not support the requested flags 0x%x %s%s.\n",
                             flags, EMPTY, EMPTY);

    /*
     * Get logical port master
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, logical_port, 0, &master_port));

    /*
     * Iter all valid ports and add to the bitmap
     */
    BCM_PBMP_CLEAR(*logical_port_channnels);
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_VALID, 0, &valid_ports));
    BCM_PBMP_ITER(valid_ports, valid_port)
    {
        /** filter ports with different master port*/
        SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, valid_port, 0, &master_valid_port));
        if (master_valid_port != master_port)
        {
            continue;
        }

        /** filter non tdm ports according to flags */
        if (flags & DNX_ALGO_PORT_CHANNELS_F_TDM_BYPASS_ONLY)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_tdm_get(unit, valid_port, &tdm_mode));
            if (tdm_mode != DNX_ALGO_PORT_TDM_MODE_BYPASS)
            {
                continue;
            }
        }

        /** filter tdm ports according to flags */
        if (flags & DNX_ALGO_PORT_CHANNELS_F_NON_TDM_BYPASS_ONLY)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_tdm_get(unit, valid_port, &tdm_mode));
            if (tdm_mode == DNX_ALGO_PORT_TDM_MODE_BYPASS)
            {
                continue;
            }
        }

        /** filter both tdm ports and l1 ports according to flags */
        if (flags & DNX_ALGO_PORT_CHANNELS_F_TDM_BYPASS_OR_L1_ONLY)
        {
            dnx_algo_port_info_s port_info;

            SHR_IF_ERR_EXIT(dnx_algo_port_tdm_get(unit, valid_port, &tdm_mode));
            SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, valid_port, &port_info));
            if (!DNX_ALGO_PORT_TYPE_IS_L1(unit, port_info) && (tdm_mode != DNX_ALGO_PORT_TDM_MODE_BYPASS))
            {
                continue;
            }
        }

        /** skip l1 ports according to flags */
        if (flags & DNX_ALGO_PORT_CHANNELS_F_NON_L1_ONLY)
        {
            dnx_algo_port_info_s port_info;

            SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, valid_port, &port_info));
            if (DNX_ALGO_PORT_TYPE_IS_ILKN_L1(unit, port_info))
            {
                continue;
            }
        }

        BCM_PBMP_PORT_ADD(*logical_port_channnels, valid_port);
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_channels_nof_get(
    int unit,
    bcm_port_t logical_port,
    int *nof_channels)
{
    bcm_pbmp_t channels;
    SHR_FUNC_INIT_VARS(unit);

    /** Get bitmap of all channels */
    SHR_IF_ERR_EXIT(dnx_algo_port_channels_get(unit, logical_port, 0, &channels));

    /** Count the number of the members */
    BCM_PBMP_COUNT(channels, *nof_channels);

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_ilkn_sch_prio_channels_in_core_get(
    int unit,
    bcm_port_t logical_port,
    bcm_port_nif_scheduler_t sch_prio,
    bcm_pbmp_t * ilkn_core_channels)
{
    bcm_pbmp_t ilkn_ports;
    bcm_port_t port_i;
    bcm_pbmp_t channels;
    uint32 flags;
    SHR_FUNC_INIT_VARS(unit);

    BCM_PBMP_CLEAR(channels);
    BCM_PBMP_CLEAR(*ilkn_core_channels);

    /*
     * Get all ILKN ports in the same core
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_ports_in_core_get(unit, logical_port, &ilkn_ports));

    BCM_PBMP_ITER(ilkn_ports, port_i)
    {
        if (sch_prio != bcmPortNifSchedulerTDM)
        {
            bcm_port_nif_scheduler_t master_sch_prio;

            /*
             * Get master port ILKN scheduler priority from sw state
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_priority_get(unit, port_i, &master_sch_prio));

            if (master_sch_prio != sch_prio)
            {
                /*
                 * All non-TDM channels on the same port have the
                 * same scheduler priority (High/Low).
                 *
                 * If scheduler priority of master port is different
                 * from the channel scheduler priority, then don't
                 * add the channels from that port.
                 */
                continue;
            }
        }

        /*
         * Get all channels with same scheduler priority
         */
        flags =
            (sch_prio ==
             bcmPortNifSchedulerTDM) ? DNX_ALGO_PORT_CHANNELS_F_TDM_BYPASS_OR_L1_ONLY :
            DNX_ALGO_PORT_CHANNELS_F_NON_TDM_BYPASS_ONLY;
        SHR_IF_ERR_EXIT(dnx_algo_port_channels_get(unit, port_i, flags, &channels));
        /*
         * Add the channels to ILU channels bitmap
         */
        BCM_PBMP_OR(*ilkn_core_channels, channels);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_interface_rate_get(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    int *rate)
{
    int num_of_lanes;
    int data_rate;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get port rate (might be per rate per lane)
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, flags, rate));

    /*
     * Modify the rate just for ilkn
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 1 /* elk */ , 1 /* L1 */ ))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, logical_port, &num_of_lanes));
        SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_serdes_rate_to_data_rate_conv
                        (unit, logical_port, *rate, num_of_lanes, &data_rate));
        *rate = data_rate;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_port_ilkn_serdes_rate_to_data_rate_conv(
    int unit,
    bcm_port_t logical_port,
    int serdes_rate,
    int nof_lanes,
    int *data_rate)
{
    uint32_t per_lane_data_rate;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * FEC not taken into consideration in this calculation.
     */
    per_lane_data_rate =
        (serdes_rate * DNX_ALGO_PORT_ILKN_WORD_SIZE * DNX_ALGO_PORT_ILKN_MAX_BURST_SIZE) /
        (DNX_ALGO_PORT_ILKN_WORD_SIZE_WITH_CTRL_BIT * DNX_ALGO_PORT_ILKN_MAX_BURST_SIZE_WITH_CTRL_WORD);
    *data_rate = per_lane_data_rate * nof_lanes;

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_phys_nof_get(
    int unit,
    bcm_port_t logical_port,
    int *nof_phys)
{
    bcm_pbmp_t phys;
    SHR_FUNC_INIT_VARS(unit);

    /** get bitmap of all phys */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, logical_port, 0, &phys));

    /** count the number of the members in the bitmap */
    BCM_PBMP_COUNT(phys, *nof_phys);

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_pp_pbmp_get(
    int unit,
    bcm_core_t core,
    bcm_pbmp_t * pbmp_pp_arr)
{
    dnx_algo_port_state_e state;
    bcm_port_t logical_port;
    bcm_core_t logical_port_core, pp_port_core;
    dnx_algo_port_info_s port_info;
    uint32 pp_port;
    dnx_algo_port_db_2d_handle_t pp_handle;
    int is_lag;
    int valid = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** Clear bitmap */
    BCM_PBMP_CLEAR(*pbmp_pp_arr);

    for (logical_port = 0; logical_port < SOC_MAX_NUM_PORTS; logical_port++)
    {
        /** Filter free ports */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.get(unit, logical_port, &state));
        if (state == DNX_ALGO_PORT_STATE_INVALID)
        {
            continue;
        }

        /** In case core is specified - filter according to core */
        if (core != BCM_CORE_ALL)
        {
            /** Get core and filter */
            SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &logical_port_core));

            /** Filter the unwanted core */
            if (core != logical_port_core)
            {
                continue;
            }
        }

        /** Get port type and add to bitmap only PP ports (including LAG) */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.info.get(unit, logical_port, &port_info));
        if (DNX_ALGO_PORT_TYPE_IS_PP(unit, port_info, 0))
        {
            /** Get handle to PP DB */
            SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.pp_handle.get(unit, logical_port, &pp_handle));
            DNX_ALGO_PORT_2D_HANDLE_VERIFY(pp_handle);
            SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.is_lag.get(unit, pp_handle.h0, pp_handle.h1, &is_lag));
            if (!is_lag)
            {
                /** Get Port + Core */
                SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_get(unit, logical_port, &pp_port_core, &pp_port));
                /** Add to bitmap*/
                BCM_PBMP_PORT_ADD(pbmp_pp_arr[pp_port_core], pp_port);
            }
        }
    }

    /** Take the pp ports of trunks */
    for (pp_port = 0; pp_port < dnx_data_port.general.nof_pp_ports_get(unit); pp_port++)
    {
        for (pp_port_core = 0; pp_port_core < dnx_data_device.general.nof_cores_get(unit); pp_port_core++)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.ref_count.get(unit, pp_port_core, pp_port, &valid));
            if (valid)
            {
                pp_handle.h0 = pp_port_core;
                pp_handle.h1 = pp_port;
                SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.is_lag.get(unit, pp_handle.h0, pp_handle.h1, &is_lag));
                /** Check if the pp port is in lag and in case it is, add it to the relevant pp port bitmap*/
                if (is_lag)
                {
                    /** Add to pp bitmap*/
                    BCM_PBMP_PORT_ADD(pbmp_pp_arr[pp_port_core], pp_port);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_logical_pbmp_to_pp_pbmp_get(
    int unit,
    bcm_pbmp_t logical_pbmp,
    bcm_pbmp_t * pbmp_pp_arr)
{
    bcm_core_t core_id;
    bcm_port_t logical_port;
    uint32 pp_port;
    SHR_FUNC_INIT_VARS(unit);

    /** Clear output PP ports bitmap */
    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        BCM_PBMP_CLEAR(pbmp_pp_arr[core_id]);
    }

    /*
     * convert logical port bmp(bitmap) to pp port bmp, required per core
     */
    BCM_PBMP_ITER(logical_pbmp, logical_port)
    {
        /** Get Port + Core */
        SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_get(unit, logical_port, &core_id, &pp_port));

        /** Add to bitmap*/
        BCM_PBMP_PORT_ADD(pbmp_pp_arr[core_id], pp_port);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_pp_pbmp_to_logical_pbmp_get(
    int unit,
    bcm_core_t core,
    bcm_pbmp_t pp_ports,
    bcm_pbmp_t * logicals_ports)
{
    bcm_port_t pp_port;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);

    /** Clear output bitmap */
    BCM_PBMP_CLEAR(*logicals_ports);

    /*
     * Iterate over each pp port:
     * * verify the port is valid
     * * convert to logical port
     */
    BCM_PBMP_ITER(pp_ports, pp_port)
    {
        /** Verify pp port valid */
        SHR_IF_ERR_EXIT(dnx_algo_port_pp_to_logical_get(unit, core, pp_port, &logical_port));

        BCM_PBMP_PORT_ADD(*logicals_ports, logical_port);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_pp_to_tm_pbmp_get(
    int unit,
    bcm_core_t core,
    bcm_port_t pp_port,
    bcm_pbmp_t * pbmp_tm_ports)
{
    bcm_port_t logical_port;
    bcm_core_t core_i;
    uint32 pp_port_i;
    dnx_algo_port_db_2d_handle_t pp_handle;
    bcm_core_t tm_core_id;
    uint32 tm_port;
    SHR_FUNC_INIT_VARS(unit);

    /** Clear output TM ports bitmap */
    BCM_PBMP_CLEAR(*pbmp_tm_ports);

    for (logical_port = 0; logical_port < SOC_MAX_NUM_PORTS; logical_port++)
    {

        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.pp_handle.get(unit, logical_port, &pp_handle));

        if (DNX_ALGO_PORT_IS_2D_HANDLE_VALID(pp_handle))
        {
            /** Get core and pp_port */
            SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.core.get(unit, pp_handle.h0, pp_handle.h1, &core_i));
            SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.pp_port.get(unit, pp_handle.h0, pp_handle.h1, &pp_port_i));
            if ((pp_port_i == pp_port) && (core_i == core))
            {
               /** Get tm port and core id */
                SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, logical_port, &tm_core_id, &tm_port));
               /** Add the tm_port to tm_ports pbmp */
                BCM_PBMP_PORT_ADD(*pbmp_tm_ports, tm_port);
            }
        }

    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_diag_info_get(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_diag_info_t * diag_info)
{
    bcm_pbmp_t phy_ports;
    SHR_FUNC_INIT_VARS(unit);

    /** Init output */
    sal_memset(diag_info, 0, sizeof(dnx_algo_port_diag_info_t));

    /** Verify logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** logical port */
    diag_info->logical_port = logical_port;

    /** port type */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &diag_info->port_info));

    /** tm attributes */
    if (DNX_ALGO_PORT_TYPE_IS_TM(unit, diag_info->port_info))
    {
        /** core and tm port */
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, logical_port, &diag_info->core_id, &diag_info->tm_port));

        /** channel id */
        SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, logical_port, &diag_info->channel_id));
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FLEXE(unit, diag_info->port_info, 1, 0))
    {
        /** FlexE clients which are not TM ports */
        /** mark the tm port as invalid */
        diag_info->tm_port = DNX_ALGO_PORT_INVALID;

        /** channel id */
        SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, logical_port, &diag_info->channel_id));
    }
    else if (DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, diag_info->port_info))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &diag_info->core_id));

        /** mark the tm port as invalid */
        diag_info->tm_port = DNX_ALGO_PORT_INVALID;

        /** mark channel_id as invalid */
        diag_info->channel_id = -1;

    }
    else
    {
        /** mark the tm port and channel as invalid */
        diag_info->tm_port = DNX_ALGO_PORT_INVALID;

        /** mark channel_id as invalid */
        diag_info->channel_id = -1;
    }

    /** phy id */
    if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, diag_info->port_info, TRUE, TRUE, TRUE, TRUE))
    {
        /** first phy for nif */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, logical_port, 0, &phy_ports));
        _SHR_PBMP_FIRST(phy_ports, diag_info->phy_id);
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, diag_info->port_info))
    {
        /** link id for fabric */
        SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, logical_port, &diag_info->phy_id));
    }
    else
    {
        diag_info->phy_id = -1;
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_diag_info_all_get(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_diag_info_all_t * diag_info_all)
{
    dnx_algo_port_diag_info_logical_t *logical_info;
    dnx_algo_port_diag_info_nif_t *nif_info;
    dnx_algo_port_diag_info_tm_pp_t *tm_pp_info;
    dnx_algo_port_diag_info_fabric_t *fabric_info;
    dnx_algo_port_diag_info_ilkn_t *ilkn_info;
    dnx_algo_port_db_2d_handle_t pp_handle;
    dnx_algo_port_diag_info_interface_t *interface_info;
    int is_lag;
    SHR_FUNC_INIT_VARS(unit);

    /** Init output */
    sal_memset(diag_info_all, 0, sizeof(dnx_algo_port_diag_info_all_t));

    /** Verify logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /*
     * Logical port info
     */
    logical_info = &diag_info_all->logical_port_info;
    /** logical port */
    logical_info->logical_port = logical_port;
    /** port type */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &logical_info->port_info));
    /** get interface offset */
    SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, logical_port, &logical_info->interface_offset));
    /** master port */
    SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, logical_port, 0, &logical_info->master_port));

    /*
     * NIF info
     */
    nif_info = &diag_info_all->nif_info;
    if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, logical_info->port_info, TRUE, TRUE, TRUE, TRUE))
    {
        /** mark data as valid */
        nif_info->valid = 1;
        /** get phy ports */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, logical_port, 0, &nif_info->first_phy));
        /** get phy ports */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, logical_port, 0, &nif_info->phy_ports));
        /** get logical fifos for the port */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_pbmp_get(unit, logical_port, 0, &nif_info->logical_fifos));

        if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, logical_info->port_info, 1 /* elk */ , 1 /* L1 */ ))
        {
            ilkn_info = &diag_info_all->ilkn_info;
            /** mark data as valid */
            ilkn_info->valid = 1;
            /** get nof segments  */
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_nof_segments_get(unit, logical_port, &ilkn_info->nof_segments));
            if (DNX_ALGO_PORT_TYPE_IS_ING_TM(unit, logical_info->port_info))
            {
                /** get is interleaved  */
                SHR_IF_ERR_EXIT(dnx_algo_port_is_ingress_interleave_get(unit, logical_port,
                                                                        &ilkn_info->is_interleaved));
            }
        }
    }

    /*
     * Interface info
     */
    interface_info = &diag_info_all->interface_info;
    if (!DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, logical_info->port_info))
    {
        interface_info->valid = 1;
        /** core */
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &interface_info->core_id));
        /** interface speed */
        {
            int has_rx_speed, has_tx_speed;
            /** Get Rx speed */
            SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, DNX_ALGO_PORT_SPEED_F_RX, &has_rx_speed));
            if (has_rx_speed == TRUE)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_speed_get
                                (unit, logical_port, DNX_ALGO_PORT_SPEED_F_RX, &interface_info->interface_rx_speed));
            }
            else
            {
                interface_info->interface_rx_speed = -1;
            }
            /** Get Tx speed */
            SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, DNX_ALGO_PORT_SPEED_F_TX, &has_tx_speed));
            if (has_tx_speed == TRUE)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_speed_get
                                (unit, logical_port, DNX_ALGO_PORT_SPEED_F_TX, &interface_info->interface_tx_speed));
            }
            else
            {
                interface_info->interface_tx_speed = -1;
            }
        }

    }
    /*
     * TM & PP info
     */
    tm_pp_info = &diag_info_all->tm_pp_info;
    /** tm attributes */
    if (DNX_ALGO_PORT_TYPE_IS_TM(unit, logical_info->port_info))
    {
        int dummy_core;

        /** mark data as valid */
        tm_pp_info->valid = 1;
        /** core and tm port */
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, logical_port, &dummy_core, &tm_pp_info->tm_port));
        /** channel id */
        SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, logical_port, &tm_pp_info->channel_id));

        /** is channelized */
        SHR_IF_ERR_EXIT(dnx_algo_port_is_channelized_get(unit, logical_port, &tm_pp_info->is_channelized));

        /** channels */
        SHR_IF_ERR_EXIT(dnx_algo_port_channels_get(unit, logical_port, 0, &tm_pp_info->channels_ports));
        /** tdm mode */
        SHR_IF_ERR_EXIT(dnx_algo_port_tdm_get(unit, logical_port, &tm_pp_info->tdm_mode));
        /** tdm interface mode */
        SHR_IF_ERR_EXIT(dnx_algo_port_if_tdm_mode_get(unit, logical_port, &tm_pp_info->if_tdm_mode));
        /** tdm-bypass master port */
        SHR_IF_ERR_EXIT(dnx_algo_port_master_get
                        (unit, logical_port, DNX_ALGO_PORT_MASTER_F_TDM_BYPASS, &tm_pp_info->tdm_bypass_master));
        /** non tdm master port */
        SHR_IF_ERR_EXIT(dnx_algo_port_master_get
                        (unit, logical_port, DNX_ALGO_PORT_MASTER_F_NON_TDM_BYPASS,
                         &tm_pp_info->non_tdm_bypass_master));
    }

    /** TM Egress scheduler */
    if (DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, logical_info->port_info))
    {
        tm_pp_info->sch_valid = 1;
        /** base hr */
        SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &tm_pp_info->base_hr));
        /** sch priorities */
        SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, logical_port, &tm_pp_info->sch_priorities));
        /** scheduler interface id */
        SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, logical_port, &tm_pp_info->sch_if_id));
    }

    /** TM Egress scheduler */
    if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, logical_info->port_info))
    {
        /** egress interface id */
        SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, logical_port, &tm_pp_info->egr_if_id));
        /** is egress interleave */
        SHR_IF_ERR_EXIT(dnx_algo_port_is_egress_interleave_get(unit, logical_port, &tm_pp_info->egr_if_interleaved));
        /** base queue pair */
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &tm_pp_info->base_q_pair));
        /** port priorities */
        SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, logical_port, &tm_pp_info->num_priorities));
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_in_lag(unit, logical_port, &is_lag));
    if (DNX_ALGO_PORT_TYPE_IS_PP(unit, logical_info->port_info, is_lag))
    {
        /** pp port - read directly to be able to read for LAG members too */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.pp_handle.get(unit, logical_port, &pp_handle));
        SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.pp_port.get(unit, pp_handle.h0, pp_handle.h1, &tm_pp_info->pp_port));
    }

    /*
     * Fabric info
     */
    fabric_info = &diag_info_all->fabric_info;
    if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, logical_info->port_info))
    {
        /** mark data as valid */
        fabric_info->valid = 1;
        /** link id for fabric */
        SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, logical_port, &fabric_info->link_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * To String / from String
 * {
 */

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_type_str_get(
    int unit,
    dnx_algo_port_type_e port_type,
    char port_type_str[DNX_ALGO_PORT_TYPE_STR_LENGTH])
{
    SHR_FUNC_INIT_VARS(unit);

    switch (port_type)
    {
        case DNX_ALGO_PORT_TYPE_INVALID:
            sal_snprintf(port_type_str, DNX_ALGO_PORT_TYPE_STR_LENGTH, "%s", "INVALID");
            break;
        case DNX_ALGO_PORT_TYPE_NIF_ETH:
            sal_snprintf(port_type_str, DNX_ALGO_PORT_TYPE_STR_LENGTH, "%s", "NIF_ETH");
            break;
        case DNX_ALGO_PORT_TYPE_FLEXE_PHY:
            sal_snprintf(port_type_str, DNX_ALGO_PORT_TYPE_STR_LENGTH, "%s", "FLEXE_PHY");
            break;
        case DNX_ALGO_PORT_TYPE_FLEXE_CLIENT:
            sal_snprintf(port_type_str, DNX_ALGO_PORT_TYPE_STR_LENGTH, "%s", "FLEXE_CLIENT");
            break;
        case DNX_ALGO_PORT_TYPE_FLEXE_MAC:
            sal_snprintf(port_type_str, DNX_ALGO_PORT_TYPE_STR_LENGTH, "%s", "FLEXE_MAC");
            break;
        case DNX_ALGO_PORT_TYPE_FLEXE_SAR:
            sal_snprintf(port_type_str, DNX_ALGO_PORT_TYPE_STR_LENGTH, "%s", "FLEXE_SAR");
            break;
        case DNX_ALGO_PORT_TYPE_NIF_ILKN:
            sal_snprintf(port_type_str, DNX_ALGO_PORT_TYPE_STR_LENGTH, "%s", "NIF_ILKN");
            break;
        case DNX_ALGO_PORT_TYPE_CPU:
            sal_snprintf(port_type_str, DNX_ALGO_PORT_TYPE_STR_LENGTH, "%s", "CPU");
            break;
        case DNX_ALGO_PORT_TYPE_RCY:
            sal_snprintf(port_type_str, DNX_ALGO_PORT_TYPE_STR_LENGTH, "%s", "RCY");
            break;
        case DNX_ALGO_PORT_TYPE_ERP:
            sal_snprintf(port_type_str, DNX_ALGO_PORT_TYPE_STR_LENGTH, "%s", "ERP");
            break;
        case DNX_ALGO_PORT_TYPE_OLP:
            sal_snprintf(port_type_str, DNX_ALGO_PORT_TYPE_STR_LENGTH, "%s", "OLP");
            break;
        case DNX_ALGO_PORT_TYPE_OAMP:
            sal_snprintf(port_type_str, DNX_ALGO_PORT_TYPE_STR_LENGTH, "%s", "OAMP");
            break;
        case DNX_ALGO_PORT_TYPE_SAT:
            sal_snprintf(port_type_str, DNX_ALGO_PORT_TYPE_STR_LENGTH, "%s", "SAT");
            break;
        case DNX_ALGO_PORT_TYPE_RCY_MIRROR:
            sal_snprintf(port_type_str, DNX_ALGO_PORT_TYPE_STR_LENGTH, "%s", "RCY_MIRROR");
            break;
        case DNX_ALGO_PORT_TYPE_EVENTOR:
            sal_snprintf(port_type_str, DNX_ALGO_PORT_TYPE_STR_LENGTH, "%s", "EVENTOR");
            break;
        case DNX_ALGO_PORT_TYPE_FABRIC:
            sal_snprintf(port_type_str, DNX_ALGO_PORT_TYPE_STR_LENGTH, "%s", "FABRIC");
            break;
        case DNX_ALGO_PORT_TYPE_SCH_ONLY:
            sal_snprintf(port_type_str, DNX_ALGO_PORT_TYPE_STR_LENGTH, "%s", "SCH_ONLY");
            break;
        case DNX_ALGO_PORT_TYPE_CRPS:
            sal_snprintf(port_type_str, DNX_ALGO_PORT_TYPE_STR_LENGTH, "%s", "CRPS");
            break;
        /** In case mapping will be missed - return int value instead */
        default:
            sal_snprintf(port_type_str, DNX_ALGO_PORT_TYPE_STR_LENGTH, "%d", port_type);
            break;
    }

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_phy_to_ethu_id_get(
    int unit,
    int phy,
    int *ethu_id)
{
    int index;
    SHR_FUNC_INIT_VARS(unit);

    index = phy / dnx_data_nif.eth.phy_map_granularity_get(unit);
    *ethu_id = dnx_data_nif.eth.phy_map_get(unit, index)->ethu_index;

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_mac_mode_config_id_ports_get(
    int unit,
    int mac_mode_config_id,
    bcm_pbmp_t * ports)
{
    int port_i, port_i_mac_mode_config_id, port_i_first_phy, core;
    bcm_pbmp_t all_nif_ports, port_i_phys;

    SHR_FUNC_INIT_VARS(unit);

    core =
        mac_mode_config_id * dnx_data_nif.eth.mac_mode_config_lanes_nof_get(unit) >=
        dnx_data_nif.phys.nof_phys_per_core_get(unit) ? 1 : 0;

    BCM_PBMP_CLEAR(*ports);
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, core, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ETH, DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY,
                     &all_nif_ports));
    BCM_PBMP_ITER(all_nif_ports, port_i)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port_i, 0, &port_i_phys));
        _SHR_PBMP_FIRST(port_i_phys, port_i_first_phy);
        port_i_mac_mode_config_id = port_i_first_phy / dnx_data_nif.eth.mac_mode_config_lanes_nof_get(unit);
        if (mac_mode_config_id == port_i_mac_mode_config_id)
        {
            BCM_PBMP_PORT_ADD(*ports, port_i);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_ethu_id_lane_info_get(
    int unit,
    int core,
    int ethu_id_in_core,
    int *first_phy,
    int *nof_lanes)
{
    int ethu_id;
    int pm_index;
    bcm_pbmp_t pm_phys;
    imb_dispatch_type_t type;
    int nof_ethus_per_core;
    SHR_FUNC_INIT_VARS(unit);

    nof_ethus_per_core = dnx_data_nif.eth.ethu_nof_get(unit) / dnx_data_device.general.nof_cores_get(unit);

    ethu_id = core * nof_ethus_per_core + ethu_id_in_core;

    /*
     * Get first lane of first PM
     */
    pm_index = dnx_data_nif.eth.ethu_properties_get(unit, ethu_id)->pms[0];
    pm_phys = dnx_data_nif.eth.pm_properties_get(unit, pm_index)->phys;
    _SHR_PBMP_FIRST(pm_phys, *first_phy);

    /*
     * Get number of lanes by ETHU type
     */
    type = dnx_data_nif.eth.ethu_properties_get(unit, ethu_id)->type;
    *nof_lanes = dnx_data_port.imb.imb_type_info_get(unit, type)->nof_lanes;

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_ilkn_lane_info_get(
    int unit,
    int core,
    int phy,
    int *ethu_id_in_core,
    int *lane)
{

    int nof_lanes, ethu_id, nof_ethus_per_core, first_phy_in_ethu;
    SHR_FUNC_INIT_VARS(unit);

    nof_ethus_per_core = dnx_data_nif.eth.ethu_nof_get(unit) / dnx_data_device.general.nof_cores_get(unit);

    SHR_IF_ERR_EXIT(dnx_algo_phy_to_ethu_id_get(unit, phy, &ethu_id));

    *ethu_id_in_core = ethu_id % nof_ethus_per_core;

    SHR_IF_ERR_EXIT(dnx_algo_ethu_id_lane_info_get(unit, core, *ethu_id_in_core, &first_phy_in_ethu, &nof_lanes));

    *lane = phy - first_phy_in_ethu;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_tdm_mode_str_get(
    int unit,
    dnx_algo_port_tdm_mode_e tdm_mode,
    char tdm_mode_str[DNX_ALGO_PORT_TDM_MODE_STR_LENGTH])
{
    SHR_FUNC_INIT_VARS(unit);

    switch (tdm_mode)
    {
        case DNX_ALGO_PORT_TDM_MODE_INVALID:
            sal_sprintf(tdm_mode_str, "%s", "INVALID");
            break;
        case DNX_ALGO_PORT_TDM_MODE_NONE:
            sal_sprintf(tdm_mode_str, "%s", "NONE");
            break;
        case DNX_ALGO_PORT_TDM_MODE_BYPASS:
            sal_sprintf(tdm_mode_str, "%s", "BYPASS");
            break;
        case DNX_ALGO_PORT_TDM_MODE_PACKET:
            sal_sprintf(tdm_mode_str, "%s", "PACKET");
            break;
        /** In case mapping will be missed - return int value instead */
        default:
            sal_sprintf(tdm_mode_str, "%d", tdm_mode);
            break;
    }

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_if_tdm_mode_str_get(
    int unit,
    dnx_algo_port_if_tdm_mode_e if_tdm_mode,
    char if_tdm_mode_str[DNX_ALGO_PORT_IF_TDM_MODE_STR_LENGTH])
{
    SHR_FUNC_INIT_VARS(unit);

    switch (if_tdm_mode)
    {
        case DNX_ALGO_PORT_IF_NO_TDM:
            sal_snprintf(if_tdm_mode_str, DNX_ALGO_PORT_IF_TDM_MODE_STR_LENGTH, "%s", "NO_TDM");
            break;
        case DNX_ALGO_PORT_IF_TDM_ONLY:
            sal_snprintf(if_tdm_mode_str, DNX_ALGO_PORT_IF_TDM_MODE_STR_LENGTH, "%s", "TDM_ONLY");
            break;
        case DNX_ALGO_PORT_IF_TDM_HYBRID:
            sal_snprintf(if_tdm_mode_str, DNX_ALGO_PORT_IF_TDM_MODE_STR_LENGTH, "%s", "HYBRID");
            break;
        /** In case mapping will be missed - return int value instead */
        default:
            sal_snprintf(if_tdm_mode_str, DNX_ALGO_PORT_IF_TDM_MODE_STR_LENGTH, "%d", if_tdm_mode);
            break;
    }

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_ethu_access_info_get(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_ethu_access_info_t * ethu_access_info)
{
    bcm_core_t core;
    int first_phy_lane, nof_ethus_per_core, nof_lanes_per_mac_mode_config, nof_lanes_per_mac, nof_lanes_per_core;
    int first_lane_in_ethu;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify that the port is ethu (cdu/clu) port
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));

    /*
     * get port information
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, logical_port, 0, &first_phy_lane));

    /*
     * get dnx_data information
     */
    nof_ethus_per_core = dnx_data_nif.eth.ethu_nof_get(unit) / dnx_data_device.general.nof_cores_get(unit);

    nof_lanes_per_core = dnx_data_nif.phys.nof_phys_per_core_get(unit);

    /*
     * calculate port attributes
     */
    ethu_access_info->core = core;
    SHR_IF_ERR_EXIT(dnx_algo_phy_to_ethu_id_get(unit, first_phy_lane, &ethu_access_info->ethu_id));
    ethu_access_info->ethu_id_in_core = ethu_access_info->ethu_id % nof_ethus_per_core;
    SHR_IF_ERR_EXIT(dnx_algo_ethu_id_lane_info_get
                    (unit, core, ethu_access_info->ethu_id_in_core, &first_lane_in_ethu,
                     &ethu_access_info->nof_lanes_in_ethu));
    ethu_access_info->first_lane_in_ethu = first_lane_in_ethu;
    ethu_access_info->first_lane_in_port = first_phy_lane - first_lane_in_ethu;
    ethu_access_info->lane_in_core = first_phy_lane % nof_lanes_per_core;
    ethu_access_info->imb_type_id = dnx_data_nif.eth.ethu_properties_get(unit, ethu_access_info->ethu_id)->type_index;

    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, TRUE, TRUE) || DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info))
    {
        /*
         * Supported only for ETHU port
         */
        ethu_access_info->imb_type = dnx_data_nif.eth.ethu_properties_get(unit, ethu_access_info->ethu_id)->type;

        ethu_access_info->pm_id_in_ethu =
            ethu_access_info->first_lane_in_port /
            dnx_data_port.imb.imb_type_info_get(unit, ethu_access_info->imb_type)->nof_lanes_in_pm;

        nof_lanes_per_mac = dnx_data_nif.eth.mac_lanes_nof_get(unit);
        ethu_access_info->mac_id = ethu_access_info->first_lane_in_port / nof_lanes_per_mac;

        nof_lanes_per_mac_mode_config = dnx_data_nif.eth.mac_mode_config_lanes_nof_get(unit);
        ethu_access_info->mac_mode_config_id = ethu_access_info->first_lane_in_port / nof_lanes_per_mac_mode_config;
    }
    else
    {
        ethu_access_info->imb_type = imbDispatchTypeNone;

        ethu_access_info->pm_id_in_ethu = -1 /* INVALID */ ;
        ethu_access_info->mac_id = -1 /* INVALID */ ;
        ethu_access_info->mac_mode_config_id = -1 /* INVALID */ ;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_ilkn_access_info_get(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_ilkn_access_info_t * ilkn_access_info)
{
    bcm_pbmp_t ilkn_phys;
    int first_phy, ethu_id;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get ILKN id
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, logical_port, &ilkn_access_info->ilkn_id));

    ilkn_access_info->ilkn_core = ilkn_access_info->ilkn_id / dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit);

    ilkn_access_info->port_in_core = ilkn_access_info->ilkn_id % dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, logical_port, 0, &ilkn_phys));
    _SHR_PBMP_FIRST(ilkn_phys, first_phy);

    SHR_IF_ERR_EXIT(dnx_algo_phy_to_ethu_id_get(unit, first_phy, &ethu_id));

    ilkn_access_info->pm_imb_type = dnx_data_nif.eth.ethu_properties_get(unit, ethu_id)->type;

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_first_port_type_get(
    int unit,
    bcm_core_t core,
    dnx_algo_port_type_e port_type,
    bcm_port_t * logical_port)
{
    bcm_port_t logical_port_i;
    dnx_algo_port_info_s port_info_i;
    bcm_core_t core_i;
    dnx_algo_port_state_e state_i;
    dnx_algo_port_db_2d_handle_t handle;
    SHR_FUNC_INIT_VARS(unit);
    *logical_port = DNX_ALGO_PORT_INVALID;

    for (logical_port_i = 0; logical_port_i < SOC_MAX_NUM_PORTS; logical_port_i++)
    {
        /** filter valid ports to bitmap */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.get(unit, logical_port_i, &state_i));
        if (state_i == DNX_ALGO_PORT_STATE_INVALID)
        {
            continue;
        }

        /** in case specific core asked - filter acorrding to core */
        if (core != BCM_CORE_ALL)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.interface_handle.get(unit, logical_port_i, &handle));
            if (!DNX_ALGO_PORT_IS_2D_HANDLE_VALID(handle))
            {
                continue;
            }

            /** get core  and filter */
            SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.core.get(unit, handle.h0, handle.h1, &core_i));
            if (core != core_i)
            {
                continue;
            }
        }

        /** look for the first requested port_info*/
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.info.get(unit, logical_port_i, &port_info_i));
        if (port_info_i.port_type == port_type)
        {
            *logical_port = logical_port_i;
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_pll_index_get(
    int unit,
    bcm_port_t logical_port,
    int *pll_index)
{
    dnx_algo_port_info_s port_info;
    int first_phy, pll_found = 0;
    int is_over_fabric = 0, interface_offset;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 1, 1))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, logical_port, &interface_offset));
        is_over_fabric = dnx_data_nif.ilkn.properties_get(unit, interface_offset)->is_over_fabric;
    }
    if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_info) || is_over_fabric)
    {
        if (is_over_fabric)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, logical_port, 0, &first_phy));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, logical_port, &first_phy));
        }
        for (*pll_index = 0; *pll_index < dnx_data_fabric.general.nof_lcplls_get(unit); (*pll_index)++)
        {
            if ((first_phy >= dnx_data_fabric.general.pll_phys_get(unit, *pll_index)->min_phy_id) &&
                (first_phy <= dnx_data_fabric.general.pll_phys_get(unit, *pll_index)->max_phy_id))
            {
                pll_found = 1;
                break;
            }
        }
    }
    /** if nif port include elk and stif */
    else if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, TRUE, TRUE, TRUE, TRUE))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, logical_port, 0, &first_phy));
        for (*pll_index = 0; *pll_index < dnx_data_nif.global.nof_lcplls_get(unit); (*pll_index)++)
        {
            if ((first_phy >= dnx_data_nif.global.pll_phys_get(unit, *pll_index)->min_phy_id) &&
                (first_phy <= dnx_data_nif.global.pll_phys_get(unit, *pll_index)->max_phy_id))
            {
                pll_found = 1;
                break;
            }
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported Port type %d\n", logical_port);
    }
    if (!pll_found)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "No PLL index is found for port %d\n", logical_port);
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_ilkn_lanes_to_nif_phys_get(
    int unit,
    int interface_id,
    bcm_pbmp_t * ilkn_lanes,
    bcm_pbmp_t * nif_phys)
{
    bcm_port_t phy_iter, interface_base_phy;
    const dnx_data_nif_ilkn_supported_phys_t *interface_supported_phys;
    int is_over_fabric;
    SHR_FUNC_INIT_VARS(unit);

    is_over_fabric = dnx_data_nif.ilkn.properties_get(unit, interface_id)->is_over_fabric;
    interface_supported_phys = dnx_data_nif.ilkn.supported_phys_get(unit, interface_id);

    if (is_over_fabric)
    {
        _SHR_PBMP_FIRST(interface_supported_phys->fabric_phys, interface_base_phy);
    }
    else
    {
        _SHR_PBMP_FIRST(interface_supported_phys->nif_phys, interface_base_phy);
    }

    BCM_PBMP_CLEAR(*nif_phys);
    BCM_PBMP_ITER(*ilkn_lanes, phy_iter)
    {
        BCM_PBMP_PORT_ADD(*nif_phys, phy_iter + interface_base_phy);
    }

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_phys_to_ilkn_lanes_get(
    int unit,
    int interface_id,
    bcm_pbmp_t * nif_phys,
    bcm_pbmp_t * ilkn_lanes)
{
    bcm_port_t phy_iter, interface_base_phy;
    const dnx_data_nif_ilkn_supported_phys_t *interface_supported_phys;
    int is_over_fabric;
    SHR_FUNC_INIT_VARS(unit);

    is_over_fabric = dnx_data_nif.ilkn.properties_get(unit, interface_id)->is_over_fabric;
    interface_supported_phys = dnx_data_nif.ilkn.supported_phys_get(unit, interface_id);

    if (is_over_fabric)
    {
        _SHR_PBMP_FIRST(interface_supported_phys->fabric_phys, interface_base_phy);
    }
    else
    {
        _SHR_PBMP_FIRST(interface_supported_phys->nif_phys, interface_base_phy);
    }

    BCM_PBMP_CLEAR(*ilkn_lanes);
    BCM_PBMP_ITER(*nif_phys, phy_iter)
    {
        BCM_PBMP_PORT_ADD(*ilkn_lanes, phy_iter - interface_base_phy);
    }

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_interface_id_get(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    int *core,
    int *nif_interface_id)
{
    uint32 supported_flags;
    dnx_algo_port_info_s port_info;
    int interface_offset, phy_port, client_index;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));
    /** verify flags */
    supported_flags = 0;
    SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                             "function flags does not support the requested flags 0x%x %s%s.\n", flags, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, core));

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
    switch (port_info.port_type)
    {
        case DNX_ALGO_PORT_TYPE_NIF_ILKN:
            SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, logical_port, &interface_offset /** ilkn id */ ));

            /** ILKN interfaces are located after the last PHY port */
            *nif_interface_id = dnx_data_nif.phys.nof_phys_per_core_get(unit) + interface_offset;
            break;

        case DNX_ALGO_PORT_TYPE_NIF_ETH:
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, logical_port, flags, &phy_port /* zero based */ ));

            *nif_interface_id = (phy_port % dnx_data_nif.phys.nof_phys_per_core_get(unit));
            break;
        case DNX_ALGO_PORT_TYPE_FLEXE_MAC:
            SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, logical_port, &client_index));

            /** FLEX-E interfaces are located after all PHY ports and all ILKN interfaces */
            *nif_interface_id = dnx_data_nif.phys.nof_phys_per_core_get(unit) +
                dnx_data_nif.ilkn.ilkn_if_nof_get(unit) + client_index;
            break;

        default:
            *nif_interface_id = -1;
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "NIF port expected (%d).\n", logical_port);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_phy_ethu_lane_info_get(
    int unit,
    int first_phy,
    int *first_lane_in_ethu,
    int *nof_lanes_in_ethu)
{
    int ethu_id;
    int ethu_core, ethu_id_in_core;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_phy_to_ethu_id_get(unit, first_phy, &ethu_id));
    ethu_id_in_core = ethu_id % (dnx_data_nif.eth.ethu_nof_get(unit) / dnx_data_device.general.nof_cores_get(unit));
    ethu_core = ethu_id / (dnx_data_nif.eth.ethu_nof_get(unit) / dnx_data_device.general.nof_cores_get(unit));
    SHR_IF_ERR_EXIT(dnx_algo_ethu_id_lane_info_get
                    (unit, ethu_core, ethu_id_in_core, first_lane_in_ethu, nof_lanes_in_ethu));
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_phy_pm_info_get(
    int unit,
    int phy_id,
    dnx_algo_pm_info_t * pm_info)
{
    int pm_index, phy_map_granularity;

    SHR_FUNC_INIT_VARS(unit);

    phy_map_granularity = dnx_data_nif.eth.phy_map_granularity_get(unit);
    pm_index = dnx_data_nif.eth.phy_map_get(unit, phy_id / phy_map_granularity)->pm_index;

    _SHR_PBMP_ASSIGN(pm_info->phys, dnx_data_nif.eth.pm_properties_get(unit, pm_index)->phys);
    pm_info->dispatch_type = dnx_data_nif.eth.pm_properties_get(unit, pm_index)->dispatch_type;

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_unit_id_get(
    int unit,
    bcm_port_t logical_port,
    int *nif_unit_id,
    int *internal_offset)
{
    int granularity;
    int core;
    int nif_interface_id;
    int granularity_adjusted_nif_id;

    SHR_FUNC_INIT_VARS(unit);

    if (logical_port == DNX_ALGO_PORT_INVALID)
    {
        SHR_IF_ERR_EXIT(_SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_interface_id_get(unit, logical_port, 0, &core, &nif_interface_id));

/* *INDENT-OFF* */
    /** indent does not make proper indentation when "global" keyword appears */
    granularity = dnx_data_nif.global.nif_interface_id_to_unit_id_granularity_get(unit);


    granularity_adjusted_nif_id = nif_interface_id - nif_interface_id % granularity;

    *nif_unit_id = dnx_data_nif.global.nif_interface_id_to_unit_id_get(unit, granularity_adjusted_nif_id)->unit_id;

    *internal_offset =
        dnx_data_nif.global.nif_interface_id_to_unit_id_get(unit,granularity_adjusted_nif_id)->internal_offset
        + nif_interface_id % granularity;

/* *INDENT-ON* */

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_ilkn_active_ids_get(
    int unit,
    uint32 flags,
    uint32 *active_ilkn_ids)
{
    int nof_ilkn_if, ilkn_id, valid;
    int is_elk;
    uint32 supported_flags;
    SHR_FUNC_INIT_VARS(unit);

    /** throw an error if unsupported flag requested */
    supported_flags =
        DNX_ALGO_PORT_ILKN_ID_GET_ILKN_DATA_ONLY | DNX_ALGO_PORT_ILKN_ID_GET_ILKN_ELK_ONLY |
        DNX_ALGO_PORT_ILKN_ID_GET_ALL;
    SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                             "Get dnx_algo_port_ilkn_active_ids_get flags does not support the requested flags 0x%x %s%s.\n",
                             flags, EMPTY, EMPTY);

    *active_ilkn_ids = 0;
    /** get max number of ILKN IDs */
    nof_ilkn_if = dnx_data_nif.ilkn.ilkn_unit_nof_get(unit) * dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit);
    /** iterate over all IDs and mark the valid ones */
    for (ilkn_id = 0; ilkn_id < nof_ilkn_if; ilkn_id++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_db.ilkn.valid.get(unit, ilkn_id, &valid));
        if (valid)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_db.ilkn.is_elk.get(unit, ilkn_id, &is_elk));
            if (is_elk)
            {
                /** filter elk interfaces if not required */
                if (flags & DNX_ALGO_PORT_ILKN_ID_GET_ILKN_ELK_ONLY)
                {
                    SHR_BITSET(active_ilkn_ids, ilkn_id);
                }
            }
            else
            {
                if (flags & DNX_ALGO_PORT_ILKN_ID_GET_ILKN_DATA_ONLY)
                {
                    SHR_BITSET(active_ilkn_ids, ilkn_id);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_port_ilkn_ports_in_core_get(
    int unit,
    bcm_port_t port,
    bcm_pbmp_t * ilkn_ports)
{
    int ilkn_port_i;
    bcm_core_t core;
    bcm_pbmp_t all_ilkn_ports;
    dnx_algo_port_ilkn_access_info_t ilkn_info, port_i_ilkn_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * clear output pbmp
     */
    BCM_PBMP_CLEAR(*ilkn_ports);

    /*
     * get port and Ilkn info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, core, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN, DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY,
                     &all_ilkn_ports));
    /*
     * iterate over all Ilkn ports and check if MAC is is the same as given port
     */
    BCM_PBMP_ITER(all_ilkn_ports, ilkn_port_i)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, ilkn_port_i, &port_i_ilkn_info));
        if (port_i_ilkn_info.ilkn_core == ilkn_info.ilkn_core)
        {
            BCM_PBMP_PORT_ADD(*ilkn_ports, ilkn_port_i);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_ilkn_nof_ports_get(
    int unit,
    bcm_port_t port,
    int *ilkn_nof_ports)
{
    bcm_pbmp_t ilkn_ports;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_ports_in_core_get(unit, port, &ilkn_ports));
    BCM_PBMP_COUNT(ilkn_ports, *ilkn_nof_ports);

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_prd_nof_profiles_get(
    int unit,
    bcm_port_t port,
    int *nof_profiles)
{
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));

    if (dnx_data_nif.prd.port_profile_map_get(unit, ethu_info.imb_type)->is_supported)
    {
        *nof_profiles = dnx_data_nif.prd.port_profile_map_get(unit, ethu_info.imb_type)->nof_profiles;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, PRD Port to Profile mapping is not supported for port %d", unit, port);
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_prd_profile_get(
    int unit,
    bcm_port_t port,
    uint32 *prd_profile)
{
    dnx_algo_port_ethu_access_info_t ethu_info;
    int port_prd_profile_is_supported;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_prd_port_profile_map_is_supported(unit, port, &port_prd_profile_is_supported));

    if (port_prd_profile_is_supported)
    {
        /*
         * In case the imb_type supports prd profile mapping (CLU or FEU for example) then get the prd profile from that map.
         */
        SHR_IF_ERR_EXIT(imb_prd_port_profile_map_get(unit, port, prd_profile));

        if (*prd_profile == DNX_ALGO_PORT_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "port (%d) is not mapped to prd profile. Please use bcmCosqControlIngressPortDropPortProfileMap",
                         port);
        }
    }
    else
    {
        /*
         * Get ETHU access info
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
        /*
         * Else (mapping is not supported, for example in CDU) - the prd profile is the lane id
         */
        *prd_profile = ethu_info.first_lane_in_port;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get HRF size for ILKN port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] hrf_size - hrf size
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
imb_ilu_prd_hrf_size_get(
    int unit,
    bcm_port_t port,
    int *hrf_size)
{
    int nof_segments, is_extended_memory_used;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_nof_segments_get(unit, port, &nof_segments));

    is_extended_memory_used = (nof_segments == dnx_data_nif.ilkn.segments_max_nof_get(unit)) ? 1 : 0;

    *hrf_size = dnx_data_nif.ilkn.data_rx_hrf_size_get(unit);

    if (is_extended_memory_used)
    {
        *hrf_size *= 2;
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_imb_type_to_pm_type_get(
    int unit,
    int imb_type,
    int is_over_fabric,
    int *pm_type)
{
    SHR_FUNC_INIT_VARS(unit);

    if (imb_type == imbDispatchTypeImb_cdu && !is_over_fabric)
    {
        *pm_type = portmodDispatchTypePm8x50;
    }
    else if (imb_type == imbDispatchTypeImb_cdu && is_over_fabric)
    {
        *pm_type = portmodDispatchTypePm8x50_fabric;
    }
    else if (imb_type == imbDispatchTypeImb_clu)
    {
        *pm_type = portmodDispatchTypePm4x25;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid IMB Type provided (%d)!", imb_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_is_precoder_supported_get(
    int unit,
    bcm_port_t port,
    int *support)
{
    dnx_algo_port_info_s port_info;
    int is_over_fabric = 0;
    int first_phy;
    int ethu_id;
    imb_dispatch_type_t imb_type;
    SHR_FUNC_INIT_VARS(unit);

    *support = 0;

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 1, 1))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, port, &is_over_fabric));
    }

    if (is_over_fabric)
    {
        /*
         * fabric side - supports precoder
         */
        *support = 1;
    }
    else
    {
        /*
         * NIF side - CDU supports precoder
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port, 0, &first_phy));
        SHR_IF_ERR_EXIT(dnx_algo_phy_to_ethu_id_get(unit, first_phy, &ethu_id));

        imb_type = dnx_data_nif.eth.ethu_properties_get(unit, ethu_id)->type;

        if (imb_type == imbDispatchTypeImb_cdu)
        {
            *support = 1;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */
