/*! \file bcmcth_mon_drv.h
 *
 * BCMCTH Monitor driver object.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_MON_DRV_H
#define BCMCTH_MON_DRV_H

#include <sal/sal_types.h>
#include <shr/shr_bitop.h>
#include <bcmcth/bcmcth_mon_util.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmcth/bcmcth_mon_telemetry_drv.h>
#include <bcmcth/bcmcth_mon_int_drv.h>
#include <bcmcth/bcmcth_mon_flowtracker_drv.h>
#include <bcmlrd/bcmlrd_map.h>

/*! Maximum field ID of MON logical tables. */
#define MON_FIELD_ID_MAX (32)

/*! Max mirror session number. */
#define BCMMON_MIRROR_SESSION_MAX (8)

/*!
 * Max mirror container number.
 * 1-on-1 mapping between container and session.
 */
#define BCMMON_MIRROR_CONTAINER_MAX BCMMON_MIRROR_SESSION_MAX

/*! The data structure for MON_TRACE_EVENTt entry. */
typedef struct bcmcth_mon_trace_s {
    /*! Bitmap of Fields to be operated. */
    SHR_BITDCLNAME(fbmp, MON_FIELD_ID_MAX);

    /*! Bitmap of mirr[] to be operated. */
    SHR_BITDCLNAME(fbmp_mir, BCMMON_MIRROR_CONTAINER_MAX);

    /*! Ingress Event ID. */
    uint32_t ievent;

    /*! Egress Event ID. */
    uint32_t eevent;

    /*! Enable to copy packets to the CPU. */
    uint32_t cpu;

    /*! Enable to mirror packets. */
    uint32_t mirr[BCMMON_MIRROR_CONTAINER_MAX];
} bcmcth_mon_trace_t;

/*! The data structure for MON_DROP_EVENTt entry. */
typedef struct bcmcth_mon_drop_s {
    /*! Bitmap of Fields to be operated. */
    SHR_BITDCLNAME(fbmp, MON_FIELD_ID_MAX);

    /*! Ingress Event ID. */
    uint32_t ievent;

    /*! Egress Event ID. */
    uint32_t eevent;

    /*! Enable to copy packets to the CPU. */
    uint32_t cpu;

    /*! Enable to mirror packets. */
    uint32_t mirr;

    /*! Enable to drop loopback copy packets. */
    uint32_t loopback;
} bcmcth_mon_drop_t;

/*! Enum of MON counters. */
typedef enum bcmcth_mon_ctr_type_e {
    BCMCTH_MON_CTR_FIRST = 0,

    BCMCTH_MON_CTR_ETRAP_CANDIDATE_FILTER_EXCEEDED = BCMCTH_MON_CTR_FIRST,

    BCMCTH_MON_CTR_ETRAP_FLOW_INSERT_SUCCESS = 1,

    BCMCTH_MON_CTR_ETRAP_FLOW_INSERT_FAILURE = 2,

    BCMCTH_MON_CTR_ETRAP_FLOW_ELEPHANT = 3,

    /*! Must be the last. */
    BCMCTH_MON_CTR_COUNT = 4
} bcmcth_mon_ctr_type_t;

/*! The data structure for MON counters. */
typedef struct bcmcth_mon_ctr_s {
    /*! Bitmap of counters to be operated. */
    SHR_BITDCLNAME(fbmp, BCMCTH_MON_CTR_COUNT);

    /*! Counter value. */
    uint64_t cnt[BCMCTH_MON_CTR_COUNT];
} bcmcth_mon_ctr_t;

/*! Opcodes for writing MON_ING_TRACE_EVENTt hardware. */
typedef enum bcmcth_mon_trace_op_e {
    /*! Update ingress trace mirror event. */
    BCMCTH_MON_ING_TRACE_MIRR_OP_SET,

    /*! Delete ingress trace mirror event. */
    BCMCTH_MON_ING_TRACE_MIRR_OP_DEL,

    /*! Update ingress trace copy2cpu event. */
    BCMCTH_MON_ING_TRACE_CPU_OP_SET,

    /*! Delete ingress trace copy2cpu event. */
    BCMCTH_MON_ING_TRACE_CPU_OP_DEL,

    /*! Must be the last. */
    BCMCTH_MON_TRACE_OP_COUNT

} bcmcth_mon_trace_op_t;

/*! Opcodes for writing MON_ING_DROP_EVENTt hardware. */
typedef enum bcmcth_mon_drop_op_e {
    /*! Update ingress drop mirror event. */
    BCMCTH_MON_ING_DROP_MIRR_OP_SET,

    /*! Delete ingress drop mirror event. */
    BCMCTH_MON_ING_DROP_MIRR_OP_DEL,

    /*! Update ingress drop copy2cpu event. */
    BCMCTH_MON_ING_DROP_CPU_OP_SET,

    /*! Delete ingress drop copy2cpu event. */
    BCMCTH_MON_ING_DROP_CPU_OP_DEL,

    /*! Update ingress drop event loopback copy. */
    BCMCTH_MON_ING_DROP_LOOPBACK_OP_SET,

    /*! Delete ingress drop event loopback copy. */
    BCMCTH_MON_ING_DROP_LOOPBACK_OP_DEL,

    /*! Must be the last. */
    BCMCTH_MON_DROP_OP_COUNT

} bcmcth_mon_drop_op_t;

/*! Opcodes for hardware of MON counters. */
typedef enum bcmcth_mon_ctr_op_e {
    /*! Get CTR_XXXt counter. */
    BCMCTH_MON_CTR_OP_GET,

    /*! Clear CTR_XXXt counter. */
    BCMCTH_MON_CTR_OP_CLR,

    /*! Must be the last. */
    BCMCTH_MON_CTR_OP_COUNT

} bcmcth_mon_ctr_op_t;

/*! Ingress trace event state. */
typedef struct bcmmon_trace_state_s {
    /*! Instance bitmap of ingress mirror status. */
    bcmdrd_pbmp_t ievent_mirr[BCMMON_MIRROR_SESSION_MAX];

    /*! Instance bitmap of ingress copy-to-cpu status. */
    bcmdrd_pbmp_t ievent_cpu;
} bcmmon_trace_state_t;

/*! Ingress drop event state. */
typedef struct bcmmon_drop_state_s {
    /*! Instance bitmap of ingress mirror status. */
    bcmdrd_pbmp_t ievent_mirr;

    /*! Instance bitmap of ingress copy-to-cpu status. */
    bcmdrd_pbmp_t ievent_cpu;

    /*! Instance bitmap of ingress drop loopback status. */
    bcmdrd_pbmp_t ievent_no_lp;
} bcmmon_drop_state_t;

/*! Recircle trace event state. */
typedef struct bcmmon_redirect_trace_state_s {
    /*! Instance bitmap of egress redirect action status. */
    bcmdrd_pbmp_t eevent_redirect;
} bcmmon_redirect_trace_state_t;


/*! Opcodes for writing MON_EGR_REDIRECT_TRACE_EVENTt hardware. */
typedef enum bcmcth_mon_redirect_trace_op_e {
    /*! Update egress redirect action event. */
    BCMCTH_MON_EGR_TRACE_RECIRCLE_OP_SET,

    /*! Delete ingress trace mirror event. */
    BCMCTH_MON_EGR_TRACE_RECIRCLE_OP_DEL,

    /*! Must be the last. */
    BCMCTH_MON_RECIRCLE_TRACE_OP_COUNT

} bcmcth_mon_redirect_trace_op_t;

/*! The data structure for MON_EGR_REDIRECT_TRACE_EVENTt entry. */
typedef struct bcmcth_mon_redirect_trace_s {
    /*! Bitmap of Fields to be operated. */
    SHR_BITDCLNAME(fbmp, MON_FIELD_ID_MAX);

    /*! Egress Event ID. */
    uint32_t eevent;

    /*! Enable to redirect action. */
    uint32_t redirect

} bcmcth_mon_redirect_trace_t;

/*!
 * \brief Initialize the MON.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot indicator.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_mon_init_f)(int unit, bool warm);

/*!
 * \brief De-initialize the MON.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_mon_deinit_f)(int unit);

/*!
 * \brief Update MON_TRACE_EVENTt hardware register(s).
 *
 * This device-specific function is called by the IMM callback handler
 * in order to update the actual hardware registers associated with
 * the MON_TRACE_EVENTt table.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID.
 * \param [in] lt_id Logical table ID.
 * \param [in] op Type of operation (see \ref bcmcth_mon_trace_op_t).
 * \param [in] entry Trace Event to update.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO I/O access error.
 */
typedef int (*bcmcth_mon_trace_op_f)(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t lt_id,
    bcmcth_mon_trace_op_t op,
    bcmcth_mon_trace_t *entry);

/*!
 * \brief Update MON_DROP_EVENTt hardware register(s).
 *
 * This device-specific function is called by the IMM callback handler
 * in order to update the actual hardware registers associated with
 * the MON_DROP_EVENTt table.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID.
 * \param [in] lt_id Logical table ID.
 * \param [in] op Type of operation (see \ref bcmcth_mon_drop_op_t).
 * \param [in] entry Drop Event to update.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO I/O access error.
 */
typedef int (*bcmcth_mon_drop_op_f)(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t lt_id,
    bcmcth_mon_drop_op_t op,
    bcmcth_mon_drop_t *entry);

/*!
 * \brief Access MON related CTR_XXXt hardware register(s).
 *
 * This device-specific function is called by the IMM callback handler
 * in order to access the actual hardware registers associated with
 * the MON related CTR_xxxt table.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID.
 * \param [in] lt_id Logical table ID.
 * \param [in] op Type of operation (see \ref bcmcth_mon_ctr_op_t).
 * \param [in/out] entry Counter information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO I/O access error.
 */
typedef int (*bcmcth_mon_ctr_op_f)(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t lt_id,
    bcmcth_mon_ctr_op_t op,
    bcmcth_mon_ctr_t *entry);

/*!
 * \brief Update MON_TELEMETRY_CONTROLt embedded application.
 *
 * This device-specific function is called by the IMM callback handler
 * in order to update the actual hardware registers associated with
 * the MON_TELEMETRY_CONTROLt table.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID.
 * \param [in] lt_id Logical table ID.
 * \param [in] op Type of operation(see \ref bcmcth_mon_telemetry_control_op_t).
 * \param [in] entry telemetry control to update.
 * \param [out] output_fields out fields to be updated if any.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO I/O access error.
 */
typedef int (*bcmcth_mon_telemetry_control_op_f)(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t lt_id,
    bcmcth_mon_telemetry_control_op_t op,
    bcmcth_mon_telemetry_control_t *entry,
    bcmltd_fields_t *output_fields);

/*!
 * \brief Update MON_TELEMETRY_INSTANCEt embedded application.
 *
 * This device-specific function is called by the IMM callback handler
 * in order to update the actual hardware registers associated with
 * the MON_TELEMETRY_INSTANCEt table.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID.
 * \param [in] lt_id Logical table ID.
 * \param [in] op Type of operation(see\ref bcmcth_mon_telemetry_instance_op_t).
 * \param [in] entry telemetry instance to update.
 * \param [out] output_fields out fields to be updated if any.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO I/O access error.
 */
typedef int (*bcmcth_mon_telemetry_instance_op_f)(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t lt_id,
    bcmcth_mon_telemetry_instance_op_t op,
    bcmcth_mon_telemetry_instance_t *entry,
    bcmltd_fields_t *output_fields);

/*!
 * \brief Update MON_COLLECTOR_IPV4t vlan information.
 *
 * This device-specific function is called by the APP to copy
 * vlans information based TAG type defined in chip file
 *
 * \param [in] unit Unit number.
 * \param [in] collector collector to get vlan info.
 * \param [out] l2 L2 header to be updated with vlan info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO I/O access error.
 */
typedef int (*bcmcth_mon_collector_ipv4_vlan_info_copy_f)(
    int unit,
    bcmcth_mon_collector_ipv4_t *collector,
    shr_util_pack_l2_header_t   *l2);

/*!
 * \brief Update MON_COLLECTOR_IPV6t vlan information.
 *
 * This device-specific function is called by the APP to copy
 * vlans information based TAG type defined in chip file
 *
 * \param [in] unit Unit number.
 * \param [in] collector collector to get vlan info.
 * \param [out] l2 L2 header to be updated with vlan info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO I/O access error.
 */
typedef int (*bcmcth_mon_collector_ipv6_vlan_info_copy_f)(
    int unit,
    bcmcth_mon_collector_ipv6_t *collector,
    shr_util_pack_l2_header_t   *l2);

/*!
 * \brief MON driver object
 *
 * MON driver is used to provide the chip specific information.
 *
 * Each device should provide its own instance to BCMCTH MON module by
 * \ref bcmcth_mon_drv_init() from the top layer. BCMCTH MON internally
 * will use this interface to get the corresponding information.
 */
typedef struct bcmcth_mon_drv_s {
    /*! Init the MON feature. */
    bcmcth_mon_init_f mon_init;

    /*! De-init the MON feature. */
    bcmcth_mon_deinit_f mon_deinit;

    /*! Trace Event information. */
    bcmcth_mon_trace_op_f trace_op;

    /*! Drop Event information. */
    bcmcth_mon_drop_op_f drop_op;

    /*! CTR information. */
    bcmcth_mon_ctr_op_f ctr_op;

    /*! Telemetry control information. */
    bcmcth_mon_telemetry_control_op_f telemetry_control_op;

    /*! Telemetry instance information. */
    bcmcth_mon_telemetry_instance_op_f telemetry_instance_op;

    /*! Copy ipv4 collector vlan information. */
    bcmcth_mon_collector_ipv4_vlan_info_copy_f collector_ipv4_vlan_info_copy;

    /*! Copy ipv6 collector vlan information. */
    bcmcth_mon_collector_ipv6_vlan_info_copy_f collector_ipv6_vlan_info_copy;

    /*! INT driver functions */
    bcmcth_mon_int_drv_t *int_drv;

    /*! Flowtracker information. */
    bcmcth_mon_flowtracker_drv_t *ft_drv;

} bcmcth_mon_drv_t;

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern bcmcth_mon_drv_t *_bc##_cth_mon_drv_get(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief Get the MON driver of the device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern bcmcth_mon_drv_t *
bcmcth_mon_drv_get(int unit);

/*!
 * \brief Initialize the MON driver of the device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_mon_drv_init(int unit);

/*!
 * \brief Initialize the Mon event.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot indicator.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_mon_init(int unit, bool warm);

/*!
 * \brief De-initialize the Mon event.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_mon_deinit(int unit);

/*!
 * \brief Update MON_TRACE_EVENTt hardware register(s).
 *
 * This device-specific function is called by the IMM callback handler
 * in order to update the actual hardware registers associated with
 * the MON_TRACE_EVENTt table.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID.
 * \param [in] lt_id Logical table ID.
 * \param [in] op Type of operation (see \ref bcmcth_mon_trace_op_t).
 * \param [in] entry Trace Event to update.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO I/O access error.
 */
extern int
bcmcth_mon_trace_op(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t lt_id,
    bcmcth_mon_trace_op_t op,
    bcmcth_mon_trace_t *entry);

/*!
 * \brief Update MON_DROP_EVENTt hardware register(s).
 *
 * This device-specific function is called by the IMM callback handler
 * in order to update the actual hardware registers associated with
 * the MON_DROP_EVENTt table.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID.
 * \param [in] lt_id Logical table ID.
 * \param [in] op Type of operation (see \ref bcmcth_mon_drop_op_t).
 * \param [in] entry Drop Event to update.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO I/O access error.
 */
extern int
bcmcth_mon_drop_op(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t lt_id,
    bcmcth_mon_drop_op_t op,
    bcmcth_mon_drop_t *entry);

/*!
 * \brief Access MON related CTR_xxxt hardware register(s).
 *
 * This device-specific function is called by the IMM callback handler
 * in order to access the actual hardware registers associated with
 * the MON related CTR_xxx table.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID.
 * \param [in] lt_id Logical table ID.
 * \param [in] op Type of operation (see \ref bcmcth_mon_ctr_op_t).
 * \param [in] entry Couneter information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO I/O access error.
 */
extern int
bcmcth_mon_ctr_op(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t lt_id,
    bcmcth_mon_ctr_op_t op,
    bcmcth_mon_ctr_t *entry);

/*!
 * \brief Initialize IMM callbacks.
 *
 * The MON custom table handling is done via the IMM component,
 * which needs a callback interface to commit changes to hardware.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_mon_imm_init(int unit, bool warm);

/*!
 * \brief Register Telemetry LT handlers.
 *
 * Register telemetry LT handlers as part of MON registration
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmcth_mon_telemetry_register(int unit);

#endif /* BCMCTH_MON_DRV_H */
