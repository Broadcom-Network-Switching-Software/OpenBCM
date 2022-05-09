/**
 * \file appl_ref_sand.h
 */

#ifndef _APPL_REF_SAND_H_
#define _APPL_REF_SAND_H_

/*************
 * INCLUDES  *
 *************/
#include <soc/sand/shrextend/shrextend_debug.h>

/*************
 * TYPE DEFS *
 *************/

 /**
  * \brief - Default number of queue/connectors per connection
  */
#define APPL_SAND_E2E_SCHEME_COS                           (8)

 /**
  * \brief - Number of ports (pp_dsp's) per modid
  */
#define APPL_SAND_NOF_PORTS_PER_MODID                     (256)


 /**
  * \brief - e2e region size (amount of flows)
  */
#define APPL_SAND_E2E_SCHEME_REGION_SIZE                  (1024)

/*************
 * STRUCTURES *
 *************/

/*************
 * FUNCTIONS *
 *************/

/**
 * \brief - Convert device and logical port to system port id.
 *
 * \param [in] unit - Unit ID
 * \param [in] device_idx - Index of device (aligned to 0)
 * \param [in] nof_sys_ports_per_device - number of system ports per device
 * \param [in] port - logical port
 * \param [Out] sysport - Returned system port index
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int appl_sand_device_to_sysport_convert(
    int unit,
    int device_idx,
    int nof_sys_ports_per_device,
    int port,
    int *sysport);

/**
 * \brief - Convert sysport id to vase voq id
 *
 * \param [in] unit - Unit ID
 * \param [in] sysport - system port ID
 * \param [Out] base_voq - returned base voq ID
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_sand_sysport_id_to_base_voq_id_convert(
    int unit,
    int sysport,
    int *base_voq);

/**
 * \brief - Convert dest port and ingress device to base voq connector
 *
 * \param [in] unit - Unit ID
 * \param [in] dst_port_ftmh_pp_dsp - pp_dsp on ftmh (8 bits)
 * \param [in] dst_port_internal_modid_index - internal index (consecutive number within the device) of dest device modid
 * \param [in] ingress_core_id - Ingress (remote device) core id
 * \param [in] ingress_device_idx - Ingress (remote device) device index
 * \param [in] nof_devices - nof devices in the system
 * \param [out] base_voq_connector - Returned base voq connector
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_sand_device_to_base_voq_connector_id_convert(
        int unit,
        int dst_port_ftmh_pp_dsp,
        int dst_port_internal_modid_index,
        int ingress_core_id,
        int ingress_device_idx,
        int nof_devices,
        int max_nof_ports_per_modid,
        int nof_ingress_cores,
        int *base_voq_connector);

#endif /*_APPL_REF_SAND_H_ */
