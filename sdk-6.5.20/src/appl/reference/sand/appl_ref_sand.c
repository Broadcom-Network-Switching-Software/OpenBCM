/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/** \file appl_ref_sand.c
 * 
 *
 * Reference application methods and definitions shared between DNX and DPP
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_OTHER

/*************
 * INCLUDES  *
 */
#include <soc/drv.h>
#include <bcm/types.h>
#include <bcm/error.h>

#include <shared/bsl.h>
#include <shared/bslenum.h>

#include <appl/reference/sand/appl_ref_sand.h>

/*************
 * TYPE DEFS *
 */
/**
 * \brief - convert BCM core ID to an index for scheduler elements allocation
 */
#define APPL_SAND_E2E_SCHEME_CORE_ID2INDEX(core_id) ((core_id == BCM_CORE_ALL) ? 0 : core_id)

/**
 * \brief - Offset for valid VoQs
 */
#define APPL_DNX_E2E_SCHEME_VOQ_START_OFFSET              (32)

/**
 * \brief - Offset for valid VoQ connectors
 */
#define APPL_SAND_E2E_SCHEME_VOQ_CONNECTOR_START_OFFSET    (32)

/*************
 * FUNCTIONS *
 */
/*
 * See .h files
 */
int
appl_sand_device_to_sysport_convert(
    int unit,
    int device_idx,
    int nof_sys_ports_per_device,
    int port,
    int *sysport)
{
    SHR_FUNC_INIT_VARS(unit);

    if (port >= nof_sys_ports_per_device)
    {
        *sysport = -1;
        LOG_ERROR(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "system port couldn't be created for port: %d \n"), port));
        goto exit;
    }

    *sysport = device_idx * nof_sys_ports_per_device + port;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h files
 */
shr_error_e
appl_sand_sysport_id_to_base_voq_id_convert(
    int unit,
    int sysport,
    int *base_voq)
{
    SHR_FUNC_INIT_VARS(unit);

    *base_voq = APPL_DNX_E2E_SCHEME_VOQ_START_OFFSET + sysport * APPL_SAND_E2E_SCHEME_COS;

    SHR_FUNC_EXIT;
}

/*
 * See .h files
 */
shr_error_e
appl_sand_device_to_base_voq_connector_id_convert(
    int unit,
    int local_port,
    int remote_core_id,
    int ingress_device_idx,
    int nof_devices,
    int *base_voq_connector)
{

    int voq_conn_id, num_of_512, voq_conn_internal_id;

    SHR_FUNC_INIT_VARS(unit);

    /** Each port has a region of consecutive connectors for all remote devices */
    voq_conn_id = APPL_SAND_E2E_SCHEME_VOQ_CONNECTOR_START_OFFSET +
        (local_port * nof_devices + ingress_device_idx) * APPL_SAND_E2E_SCHEME_COS;

    /*
     * For dual core support (SYMMETRIC MODE)
     * Each 1024 connectors are devided to 512 connected to core 0 and 512 connected to core 1
     * so this map remap connectors to valid range in symmetric mode
     */
    num_of_512 = voq_conn_id / 512;
    voq_conn_internal_id = voq_conn_id % 512;

    *base_voq_connector = (num_of_512 * 1024) + (APPL_SAND_E2E_SCHEME_CORE_ID2INDEX(remote_core_id) * 512) +
        voq_conn_internal_id;

    SHR_FUNC_EXIT;
}
