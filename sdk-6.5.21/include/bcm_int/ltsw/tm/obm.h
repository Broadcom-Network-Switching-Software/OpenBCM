/* \file obm.h
 *
 * OBM headfiles to declare internal APIs for TM devices.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMINT_LTSW_TM_OBM_H
#define BCMINT_LTSW_TM_OBM_H

#include <bcm/types.h>
#include <bcm/cosq.h>
#include <bcm/switch.h>

#include <bcm_int/ltsw/obm.h>

/*
 * \brief Get packet pirority type in symbol.
 *
 * \param [in] unit Unit Number.
 * \param [in] piority_type piority type.
 * \param [out] sym_val  piority type in symbol.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*obm_pkt_pri_type_get_t) (
    int unit,
    int piority_type,
    const char **sym_val);

/*
 * \brief Obm driver structure.
 */
typedef struct tm_ltsw_obm_chip_driver_s {

    /* Get packet pirority type in symbol. */
    obm_pkt_pri_type_get_t    pkt_pri_type_get;

} tm_ltsw_obm_chip_driver_t;

/*
 * \brief OBM Driver callback registration
 *
 * \param [in] unit Unit number.
 * \param [in] drv  Driver pointer.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
tm_ltsw_obm_chip_driver_register(int unit, tm_ltsw_obm_chip_driver_t* drv);

/*
 * \brief OBM Driver callback deregistration
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
tm_ltsw_obm_chip_driver_deregister(int unit);

/*
 * \brief OBM Driver get.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
tm_ltsw_obm_chip_driver_get(int unit, tm_ltsw_obm_chip_driver_t **drv);

/*
 * \brief Get OBM traffic class for max watermark mode
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port ID.
 * \param [out] obm_wm_mode OBM traffic class for max watermark mode selection.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_obm_port_max_usage_mode_get(
    int unit,
    bcm_port_t port,
    bcm_obm_max_watermark_mode_t *obm_wm_mode);

/*
 * \brief Set OBM traffic class for max watermark mode
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port ID.
 * \param [in] obm_wm_mode OBM traffic class for max watermark mode selection.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_obm_port_max_usage_mode_set(
    int unit,
    bcm_port_t port,
    bcm_obm_max_watermark_mode_t obm_wm_mode);

/*
 * \brief Get OBM flow control configuration
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port ID.
 * \param [in] obm_traffic_class OBM traffic classes.
 * \param [in] max_pri_count Max count.
 * \param [out] priority_list priority array.
 * \param [out] pri_count Count.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_obm_traffic_class_pfc_priority_mapping_get(
    int unit,
    bcm_port_t port,
    bcm_obm_traffic_class_t obm_traffic_class,
    int max_pri_count,
    int *priority_list,
    int *pri_count);

/*
 * \brief Set OBM flow control configuration
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port ID.
 * \param [in] obm_traffic_class OBM traffic classes.
 * \param [in] max_pri_count Max count.
 * \param [in] priority_list priority array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_obm_traffic_class_pfc_priority_mapping_set(
    int unit,
    bcm_port_t port,
    bcm_obm_traffic_class_t obm_traffic_class,
    int max_pri_count,
    int *priority_list);

/*
 * \brief Initialize the obm module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_obm_init(int unit);

/*
 * \brief Detach the obm module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_obm_detach(int unit);

/*
 * \brief Enable obm port flow control setting.
 *
 * \param [in] unit Unit Number.
 * \param [in] lport Logical port ID.
 * \param [in] fc_type Flow control type, 0: parse, 1: PFC.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_obm_port_fc_enable_set(
    int unit,
    int lport,
    int fc_type);

/*
 * \brief obm_port_pkt_parse_get.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Logical port ID.
 * \param [in] type bcmi_ltsw_obm_port_control_t.
 * \param [out] arg arg.
 * \param [in] check_state check_state.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_obm_port_pkt_parse_get(
    int unit,
    bcm_port_t port,
    bcmi_ltsw_obm_port_control_t type,
    int *arg,
    int check_state);

/*
 * \brief obm_port_pkt_parse_set.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Logical port ID.
 * \param [in] type bcmi_ltsw_obm_port_control_t.
 * \param [in] arg arg.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_obm_port_pkt_parse_set(
    int unit,
    bcm_port_t port,
    bcmi_ltsw_obm_port_control_t type,
    int arg);

/*
 * \brief obm_pm_pkt_parse_tpid_get.
 *
 * \param [in] unit Unit Number.
 * \param [in] pm_id pm ID.
 * \param [in] type bcmi_ltsw_obm_port_control_t.
 * \param [out] arg arg.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_obm_pm_pkt_parse_tpid_get(
    int unit,
    int pm_id,
    bcmi_ltsw_obm_port_control_t type,
    int *arg);

/*
 * \brief obm_pm_pkt_parse_tpid_set.
 *
 * \param [in] unit Unit Number.
 * \param [in] pm_id pm ID.
 * \param [in] type bcmi_ltsw_obm_port_control_t.
 * \param [in] arg arg.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_obm_pm_pkt_parse_tpid_set(
    int unit,
    int pm_id,
    bcmi_ltsw_obm_port_control_t type,
    int arg);

/*
 * \brief obm_pm_pkt_parse_ethertype_get.
 *
 * \param [in] unit Unit Number.
 * \param [in] pm_id pm ID.
 * \param [in] type bcmi_ltsw_obm_port_control_t.
 * \param [out] arg arg.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_obm_pm_pkt_parse_ethertype_get(
    int unit,
    int pm_id,
    bcmi_ltsw_obm_port_control_t type,
    int *arg);

/*
 * \brief obm_pm_pkt_parse_ethertype_set.
 *
 * \param [in] unit Unit Number.
 * \param [in] pm_id pm ID.
 * \param [in] type bcmi_ltsw_obm_port_control_t.
 * \param [in] arg arg.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_obm_pm_pkt_parse_ethertype_set(
    int unit,
    int pm_id,
    bcmi_ltsw_obm_port_control_t type,
    int arg);

/*
 * \brief obm_pm_pkt_parse_user_define_ethertype_get.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Logical port ID.
 * \param [in] index index.
 * \param [out] switch_obm_classifier bcm_switch_obm_classifier_t.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_obm_pm_pkt_parse_user_define_ethertype_get(
    int unit,
    bcm_port_t port,
    int index,
    bcm_switch_obm_classifier_t *switch_obm_classifier);

/*
 * \brief obm_pm_pkt_parse_user_define_ethertype_set.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Logical port ID.
 * \param [in] index index.
 * \param [in] switch_obm_classifier bcm_switch_obm_classifier_t.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_obm_pm_pkt_parse_user_define_ethertype_set(
    int unit,
    bcm_port_t port,
    int index,
    bcm_switch_obm_classifier_t *switch_obm_classifier);

/*
 * \brief obm_port_pkt_pri_tc_map_get.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Logical port ID.
 * \param [in] piority_type bcm_switch_obm_classifier_type_t.
 * \param [in] priority priority.
 * \param [out] obm_tc bcm_switch_obm_priority_t.
 * \param [in] check_state check_state.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_obm_port_pkt_pri_tc_map_get(
    int unit,
    bcm_port_t port,
    bcm_switch_obm_classifier_type_t piority_type,
    int priority,
    bcm_switch_obm_priority_t *obm_tc,
    int check_state);

/*
 * \brief obm_port_pkt_pri_tc_map_set.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Logical port ID.
 * \param [in] piority_type bcm_switch_obm_classifier_type_t.
 * \param [in] priority priority.
 * \param [in] obm_tc bcm_switch_obm_priority_t.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_obm_port_pkt_pri_tc_map_set(
    int unit,
    bcm_port_t port,
    bcm_switch_obm_classifier_type_t piority_type,
    int priority,
    bcm_switch_obm_priority_t obm_tc);

#endif /* BCMINT_LTSW_TM_OBM_H */
