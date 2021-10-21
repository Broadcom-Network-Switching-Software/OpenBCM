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
    int dst_port_ftmh_pp_dsp,
    int dst_port_internal_modid_index,
    int ingress_core_id,
    int ingress_device_idx,
    int nof_devices,
    int max_nof_ports_per_modid,
    int nof_ingress_cores,
    int *base_voq_connector)
{

    int voq_conn_id, voq_conn_region_id, voq_conn_internal_id, port_index, nof_voq_conn_per_core;

    SHR_FUNC_INIT_VARS(unit);

    /** verify that the provided pp_dsp is smaller than the assumed maximal amount of ports per modid */
    if (dst_port_ftmh_pp_dsp >= max_nof_ports_per_modid)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "dest port ftmh_pp_dsp: %d is bigger than the maximal amount of ports per modid: "
                "%d\n", dst_port_ftmh_pp_dsp, max_nof_ports_per_modid);
    }

    /** Calculate a unique index for the given dst_port info (modid_index and ftmh_pp_dsp) */
    port_index = dst_port_internal_modid_index*max_nof_ports_per_modid + dst_port_ftmh_pp_dsp;


    /** Each port has a region of consecutive connectors for all remote devices */
    voq_conn_id = APPL_SAND_E2E_SCHEME_VOQ_CONNECTOR_START_OFFSET +
        (port_index * nof_devices + ingress_device_idx) * APPL_SAND_E2E_SCHEME_COS;

    /**
     * For multi core support (SYMMETRIC MODE)
     * Each 1k connectors are divided according to the amount of cores in the ingress device in order internally
     * allocate voq_connector bundles for all the ingress cores.
     * So this map-remap connectors to valid range in symmetric mode
     */
    nof_voq_conn_per_core = APPL_SAND_E2E_SCHEME_REGION_SIZE / nof_ingress_cores;
    voq_conn_region_id = voq_conn_id / nof_voq_conn_per_core;
    voq_conn_internal_id = voq_conn_id % nof_voq_conn_per_core;

    *base_voq_connector = (voq_conn_region_id * APPL_SAND_E2E_SCHEME_REGION_SIZE) +
                          (APPL_SAND_E2E_SCHEME_CORE_ID2INDEX(ingress_core_id) * nof_voq_conn_per_core) +
                          voq_conn_internal_id;

exit:
    SHR_FUNC_EXIT;
}
