/* \file cosq_thd.h
 *
 * cosq_thd headfiles to declare internal APIs for TM devices.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMINT_LTSW_TM_COSQ_THD_H
#define BCMINT_LTSW_TM_COSQ_THD_H

#include <bcm/cosq.h>
#include <bcm/types.h>

typedef struct tm_ltsw_cosq_cpu_cosq_config_s {
    int q_min_limit;
    int q_shared_limit;
    int q_limit_dynamic;
    int q_limit_enable;
    int q_color_limit_enable;
} tm_ltsw_cosq_cpu_cosq_config_t;

/*
 * \brief Convert int to alpha value
 *
 * \param [in]  alpha         alpha.
 *
 * \retval alpha              alpha.
 */
typedef const char* (*cosq_thd_int_to_alpha_t)(
    int alpha);

/*
 * \brief Convert alpha value to int
 *
 * \param [in]  alpha      alpha.
 *
 * \retval alpha           alpha.
 */
typedef int (*cosq_thd_alpha_to_int_t)(
    const char *alpha);

/*
 * \brief Convert int to percent value
 *
 * \param [in]  percent      percent.
 *
 * \retval percent           percent.
 */
typedef const char* (*cosq_thd_int_to_percent_t)(
    int percent);

/*
 * \brief Convert percent value to int
 *
 * \param [in]  percent   percent.
 *
 * \retval percent        percent.
 */
typedef int (*cosq_thd_percent_to_int_t)(
    const char *percent);

/*
 * \brief Convert int to ADT priority group value
 *
 * \param [in]  adt_pri_group         BCM macro format adt_pri_group.
 *
 * \retval adt_pri_group              LT string adt_pri_group.
 */
typedef const char* (*cosq_thd_int_to_adt_pri_group_t)(
    int adt_pri_group);

/*
 * \brief Convert ADT priority group value to int
 *
 * \param [in]  adt_pri_group      LT string adt_pri_group.
 *
 * \retval adt_pri_group           BCM macro adt_pri_group.
 */
typedef int (*cosq_thd_adt_pri_group_to_int_t)(
    const char *adt_pri_group);

/*
 * \brief Cosq_thd driver structure.
 */
typedef struct tm_ltsw_cosq_thd_chip_driver_s {

    /* Convert int to alpha value. */
    cosq_thd_int_to_alpha_t cosq_thd_int_to_alpha;

    /* Convert alpha value to int. */
    cosq_thd_alpha_to_int_t cosq_thd_alpha_to_int;

    /* Convert int to percent value. */
    cosq_thd_int_to_percent_t cosq_thd_int_to_percent;

    /* Convert percent value to int. */
    cosq_thd_percent_to_int_t cosq_thd_percent_to_int;

    /* Convert int to ADT priority group value. */
    cosq_thd_int_to_adt_pri_group_t cosq_thd_int_to_adt_pri_group;

    /* Convert ADT priority group value to int. */
    cosq_thd_adt_pri_group_to_int_t cosq_thd_adt_pri_group_to_int;

} tm_ltsw_cosq_thd_chip_driver_t;

/*
 * \brief COSQ_THD Driver callback registration
 *
 * \param [in] unit Unit number.
 * \param [in] drv  Driver pointer.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
tm_ltsw_cosq_thd_chip_driver_register(int unit, tm_ltsw_cosq_thd_chip_driver_t* drv);

/*
 * \brief COSQ_THD Driver callback deregistration
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
tm_ltsw_cosq_thd_chip_driver_deregister(int unit);

/*
 * \brief COSQ_THD Driver get.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
tm_ltsw_cosq_thd_chip_driver_get(int unit, tm_ltsw_cosq_thd_chip_driver_t **drv);

/*
 * \brief COS queue bandwidth control bucket setting.
 *
 * \param [in]   unit                Unit number.
 * \param [in]   port                GPORT ID.
 * \param [in]   cosq                COS queue.
 * \param [in]   type                Feature.
 * \param [out]  arg                 Feature value.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
tm_ltsw_cosq_thd_bandwidth_burst_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg);

/*
 * \brief COS queue bandwidth control bucket setting.
 *
 * \param [in]   unit                Unit number.
 * \param [in]   port                GPORT ID.
 * \param [in]   cosq                COS queue.
 * \param [in]   type                Feature.
 * \param [in]   arg                 Feature value.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
tm_ltsw_cosq_thd_bandwidth_burst_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg);

/*
 * \brief COS queue egress service pool ID setting
 *
 * \param [in] unit Unit Number.
 * \param [in] gport Gport Number.
 * \param [in] cosq COS queue.
 * \param [in] type Feature.
 * \param [in] arg Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_egr_spid_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg);

/*
 * \brief COS queue egress service pool ID get
 *
 * \param [in] unit Unit Number.
 * \param [in] gport Gport Number.
 * \param [in] cosq COS queue.
 * \param [in] type Feature.
 * \param [out] arg Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_egr_spid_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg);

/*
 * \brief Egress service pool setting
 *
 * \param [in] unit Unit Number.
 * \param [in] gport Gport Number.
 * \param [in] cosq COS queue.
 * \param [in] buffer Buffer id.
 * \param [in] type Feature.
 * \param [in] arg Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_egr_pool_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_buffer_id_t buffer,
    bcm_cosq_control_t type,
    int arg);

/*
 * \brief Egress service pool get
 *
 * \param [in] unit Unit Number.
 * \param [in] gport Gport Number.
 * \param [in] cosq COS queue.
 * \param [in] buffer Buffer id.
 * \param [in] type Feature.
 * \param [out] arg Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_egr_pool_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_buffer_id_t buffer,
    bcm_cosq_control_t type,
    int *arg);

/*
 * \brief Egress queue setting
 *
 * \param [in] unit Unit Number.
 * \param [in] gport Gport Number.
 * \param [in] cosq COS queue.
 * \param [in] type Feature.
 * \param [in] arg Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_egr_queue_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg);

/*
 * \brief Egress queue get
 *
 * \param [in] unit Unit Number.
 * \param [in] gport Gport Number.
 * \param [in] cosq COS queue.
 * \param [in] type Feature.
 * \param [out] arg Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_egr_queue_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg);

/*
 * \brief Egress port pool setting
 *
 * \param [in] unit Unit Number.
 * \param [in] gport Gport Number.
 * \param [in] cosq COS queue.
 * \param [in] type Feature.
 * \param [in] arg Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_egr_port_pool_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg);

/*
 * \brief Egress port pool get
 *
 * \param [in] unit Unit Number.
 * \param [in] gport Gport Number.
 * \param [in] cosq COS queue.
 * \param [in] type Feature.
 * \param [out] arg Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_egr_port_pool_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg);

/*
 * \brief Ingress service pool setting
 *
 * \param [in] unit Unit Number.
 * \param [in] gport Gport Number.
 * \param [in] prigroup Priority group.
 * \param [in] buffer Buffer id.
 * \param [in] type Feature.
 * \param [in] arg Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_ing_pool_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t prigroup,
    bcm_cosq_buffer_id_t buffer,
    bcm_cosq_control_t type,
    int arg);

/*
 * \brief Ingress service pool get
 *
 * \param [in] unit Unit Number.
 * \param [in] gport Gport Number.
 * \param [in] prigroup Priority group.
 * \param [in] buffer Buffer id.
 * \param [in] type Feature.
 * \param [out] arg Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_ing_pool_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t prigroup,
    bcm_cosq_buffer_id_t buffer,
    bcm_cosq_control_t type,
    int *arg);

/*
 * \brief Ingress port service pool setting
 *
 * \param [in] unit Unit Number.
 * \param [in] gport Gport Number.
 * \param [in] prigroup Priority group.
 * \param [in] type Feature.
 * \param [in] arg Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_ing_port_pool_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t prigroup,
    bcm_cosq_control_t type,
    int arg);

/*
 * \brief Ingress port service pool get
 *
 * \param [in] unit Unit Number.
 * \param [in] gport Gport Number.
 * \param [in] prigroup Priority group.
 * \param [in] type Feature.
 * \param [out] arg Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_ing_port_pool_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t prigroup,
    bcm_cosq_control_t type,
    int *arg);

/*
 * \brief Ingress pg service pool setting
 *
 * \param [in] unit Unit Number.
 * \param [in] gport Gport Number.
 * \param [in] prigroup Priority group.
 * \param [in] type Feature.
 * \param [in] arg Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_ing_pg_pool_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t prigroup,
    bcm_cosq_control_t type,
    int arg);

/*
 * \brief Ingress pg service pool get
 *
 * \param [in] unit Unit Number.
 * \param [in] gport Gport Number.
 * \param [in] prigroup Priority group.
 * \param [in] type Feature.
 * \param [out] arg Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_ing_pg_pool_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t prigroup,
    bcm_cosq_control_t type,
    int *arg);

/*
 * \brief Ingress port pg setting
 *
 * \param [in] unit Unit Number.
 * \param [in] gport Gport Number.
 * \param [in] prigroup Priority group.
 * \param [in] type Feature.
 * \param [in] arg Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_ing_port_pg_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t prigroup,
    bcm_cosq_control_t type,
    int arg);

/*
 * \brief Ingress port pg get
 *
 * \param [in] unit Unit Number.
 * \param [in] gport Gport Number.
 * \param [in] prigroup Priority group.
 * \param [in] type Feature.
 * \param [out] arg Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_ing_port_pg_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t prigroup,
    bcm_cosq_control_t type,
    int *arg);

/*
 * \brief Alpha value set for dynamic threshold
 *
 * \param [in] unit Unit Number.
 * \param [in] gport Gport Number.
 * \param [in] cosq cos queue.
 * \param [in] arg Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_alpha_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_drop_limit_alpha_value_t arg);

/*
 * \brief Alpha value get for dynamic threshold
 *
 * \param [in] unit Unit Number.
 * \param [in] gport Gport Number.
 * \param [in] cosq Cos queue.
 * \param [out] arg Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_alpha_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_drop_limit_alpha_value_t *arg);

/*
 * \brief Port UC service pool id get.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] queue queue id.
 * \param [out] spid pool id.
 * \param [out] use_qgroup_min true/false.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_port_uc_spid_get(
    int unit,
    bcm_port_t port,
    int queue,
    int *spid,
    int *use_qgroup_min);

/*
 * \brief TM db shared limit get.
 *
 * \param [in] unit Unit Number.
 * \param [in] itm itm.
 * \param [in] spid ing service pool id.
 * \param [out] shared_limit shared limit value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_shared_limit_get(
    int unit,
    int itm,
    int spid,
    int *shared_limit);

/*
 * \brief Set the profile for Input priortty to PG mapping for a port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] profile_id Profile index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_port_inppri_profile_set(
    int unit,
    bcm_port_t port,
    int profile_id);

/*
 * \brief Get the profile for Input priortty to PG mapping for a port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [out] profile_id Profile index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_port_inppri_profile_get(
    int unit,
    bcm_port_t port,
    int *profile_id);

/*
 * \brief Set the profile for PG to SPID/HPID/PFCPRI mapping for a port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] profile_id Profile index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_port_pg_profile_set(
    int unit,
    bcm_port_t port,
    int profile_id);

/*
 * \brief Get the profile for PG to SPID/HPID/PFCPRI mapping for a port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [out] profile_id Profile index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_port_pg_profile_get(
    int unit,
    bcm_port_t port,
    int *profile_id);

/*
 * \brief To Set enable or disable status of service pool for a given pool id.
 *
 * \param [in] unit Unit Number.
 * \param [in] id Service pool id.
 * \param [in] cosq_service_pool Enable/Disable given type service pool.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_service_pool_set(
    int unit,
    bcm_service_pool_id_t id,
    bcm_cosq_service_pool_t cosq_service_pool);

/*
 * \brief To get enable or disable status of service pool for a given pool id.
 *
 * \param [in] unit Unit Number.
 * \param [in] id Service pool id.
 * \param [out] cosq_service_pool Get enable or disable status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_service_pool_get(
    int unit,
    bcm_service_pool_id_t id,
    bcm_cosq_service_pool_t *cosq_service_pool);

/*
 * \brief Enable or disable service pool override.
 *
 * \param [in] unit Unit Number.
 * \param [in] type Service pool override type.
 * \param [in] service_pool Service pool id.
 * \param [in] enable Enable or disable service pool override.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_service_pool_override_set(
    int unit,
    bcm_cosq_service_pool_override_type_t type,
    bcm_service_pool_id_t service_pool,
    int enable);

/*
 * \brief Enable or disable service pool override.
 *
 * \param [in] unit Unit Number.
 * \param [in] type Service pool override type.
 * \param [out] service_pool Service pool id.
 * \param [out] enable Enable or disable service pool override.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_service_pool_override_get(
    int unit,
    bcm_cosq_service_pool_override_type_t type,
    bcm_service_pool_id_t *service_pool,
    int *enable);

/*
 * \brief Set a profile for the various mappings.
 *
 * \param [in] unit Unit Number.
 * \param [in] profile_index profile index.
 * \param [in] type Mapping type.
 * \param [in] array_count Number of mappings in array.
 * \param [in] arg Value of input to mapping type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_priority_group_mapping_profile_set(
    int unit,
    int profile_index,
    bcm_cosq_priority_group_mapping_profile_type_t type,
    int array_count,
    int *arg);

/*
 * \brief Get a profile for the various mappings.
 *
 * \param [in] unit Unit Number.
 * \param [in] profile_index profile index.
 * \param [in] type Mapping type.
 * \param [in] array_max Number of mappings allocated in array.
 * \param [out] arg Array of input to mapping type.
 * \param [out] array_count Number of mappings returned.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_priority_group_mapping_profile_get(
    int unit,
    int profile_index,
    bcm_cosq_priority_group_mapping_profile_type_t type,
    int array_max,
    int *arg,
    int *array_count);

/*
 * \brief Setup per port per priority group properties.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] priority_group_id priority group.
 * \param [in] type Port/priority group property.
 * \param [in] arg set value of property.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_port_priority_group_property_set(
    int unit,
    bcm_port_t port,
    int priority_group_id,
    bcm_cosq_port_prigroup_control_t type,
    int arg);

/*
 * \brief Setup per port per priority group properties.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] priority_group_id priority group.
 * \param [in] type Port/priority group property.
 * \param [out] arg set value of property.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_port_priority_group_property_get(
    int unit,
    bcm_port_t port,
    int priority_group_id,
    bcm_cosq_port_prigroup_control_t type,
    int *arg);

/*
 * \brief To enable/disable Rx of packets on the specified CPU cosq.
 *
 * \param [in] unit Unit Number.
 * \param [in] cosq CPU Cosq ID.
 * \param [in] enable Enable/Disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_cpu_cosq_enable_set(
    int unit,
    bcm_cos_queue_t cosq,
    tm_ltsw_cosq_cpu_cosq_config_t *cpu_cosq_config);

/*
 * \brief To get enable/disable status on Rx of packets on the specified CPU cosq.
 *
 * \param [in] unit Unit Number.
 * \param [in] cosq CPU Cosq ID.
 * \param [out] enable Enable/Disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_cpu_cosq_enable_get(
    int unit,
    bcm_cos_queue_t cosq,
    tm_ltsw_cosq_cpu_cosq_config_t *cpu_cosq_config);

/*
 * \brief ADT priority group setting
 *
 * \param [in] unit Unit Number.
 * \param [in] gport Gport Number.
 * \param [in] cosq COS queue.
 * \param [in] arg Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_adt_pri_group_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int arg);

/*
 * \brief ADT priority group getting
 *
 * \param [in] unit Unit Number.
 * \param [in] gport Gport Number.
 * \param [in] cosq COS queue.
 * \param [out] arg Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_adt_pri_group_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *arg);

/*
 * \brief ADT low priority monitor pool setting
 *
 * \param [in] unit Unit Number.
 * \param [in] gport Gport Number.
 * \param [in] cosq COS queue.
 * \param [in] buffer Buffer id.
 * \param [in] type Feature.
 * \param [in] arg Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_adt_low_pri_mon_pool_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_buffer_id_t buffer,
    bcm_cosq_control_t type,
    int arg);

/*
 * \brief ADT low priority monitor pool getting
 *
 * \param [in] unit Unit Number.
 * \param [in] gport Gport Number.
 * \param [in] cosq COS queue.
 * \param [in] buffer Buffer id.
 * \param [in] type Feature.
 * \param [in] arg Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_adt_low_pri_mon_pool_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_buffer_id_t buffer,
    bcm_cosq_control_t type,
    int *arg);

/*
 * \brief Clear ADT low priority monitor pool table
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_adt_low_pri_mon_pool_clear(
    int unit);

/*
 * \brief Clear egress service pool ADT control table.
 *
 * \param [in]  unit          Unit number.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
tm_ltsw_cosq_thd_egr_service_pool_adt_clear(
    int unit);

/*!
 * \brief Calculate and set Headroom Pool limit based on number of lossless classes and packet distribution.
 *
 * \param [in] unit Unit Number.
 * \param [in] hdrm_pool Headroom Pool number.
 * \param [in] num_lossless_class Max Number of lossless classes expected to use Headroom at a given time.
 * \param [in] arr_size Array size of packet distribution array.
 * \param [in] pkt_dist_array Packet distribution array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_hdrm_pool_limit_set(
    int unit,
    int hdrm_pool,
    int num_lossless_class,
    int arr_size,
    bcm_cosq_pkt_size_dist_t *pkt_dist_array);

/*!
 * \brief Setup Port PriorityGroup headroom value based on cable length and packet distribution.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port number.
 * \param [in] pri_bmp Internal Priority bitmap for which PG headroom needs to be modified.
 * \param [in] cable_len Cable length (in meters).
 * \param [in] arr_size Array size of packet distribution array.
 * \param [in] pkt_dist_array Packet distribution array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_thd_port_prigrp_hdrm_set(
    int unit,
    bcm_port_t port,
    uint32_t pri_bmp,
    int cable_len,
    int arr_size,
    bcm_cosq_pkt_size_dist_t *pkt_dist_array);

#endif /* BCMINT_LTSW_TM_COSQ_THD_H */
