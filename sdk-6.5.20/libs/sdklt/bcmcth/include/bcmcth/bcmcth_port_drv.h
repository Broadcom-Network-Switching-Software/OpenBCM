/*! \file bcmcth_port_drv.h
 *
 * BCMCTH PORT driver objects.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_PORT_DRV_H
#define BCMCTH_PORT_DRV_H

#include <sal/sal_types.h>
#include <bcmcth/bcmcth_port_util.h>
#include <shr/shr_bitop.h>
#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmcth/bcmcth_info_table_entry.h>

/*! Opcodes for writing PORT_MIRRORt hardware entries. */
typedef enum bcmcth_port_mirror_op_e {

    /*! Update port mirror. */
    BCMCTH_PORT_MIRROR_OP_SET,

    /*! Delete port mirror. */
    BCMCTH_PORT_MIRROR_OP_DEL,

    /*! Must be the last. */
    BCMCTH_PORT_MIRROR_OP_COUNT

} bcmcth_port_mirror_op_t;

/*! Mirror container information. */
typedef struct bcmport_mirr_state_s {
    /*! Reference count. */
    uint8_t ref_cnt;

    /*! Mirror session. */
    uint8_t sess;

    /*! Mirror instance. */
    uint8_t inst;

} bcmport_mirr_state_t;

/*!
 * \brief Initialize the PORT module.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot indicator.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_port_init_f)(int unit, bool warm);

/*!
 * \brief De-initialize the PORT module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_port_deinit_f)(int unit);

/*!
 * \brief Set the hardware for PORT_BRIDGEt_BRIDGEf.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table ID.
 * \param [in] port Logical port.
 * \param [in] bridge Bridge indicator.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_port_bridge_set_f)(int unit,
                                        const bcmltd_op_arg_t *op_arg,
                                        bcmltd_sid_t lt_id,
                                        shr_port_t port,
                                        bool bridge);

/*!
 * \brief Set the hardware for PORT_BRIDGEt_BRIDGEf.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table ID.
 * \param [in] port Logical port.
 * \param [out] bridge Bridge indicator.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_port_bridge_get_f)(
    int unit,
    const bcmltd_op_arg_t *op_arg,
    bcmltd_sid_t lt_id,
    shr_port_t port,
    bool *bridge);

/*!
 * \brief Set the hardware for PORT_MEMBERSHIP_POLICYt.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table ID.
 * \param [in] port Logical port.
 * \param [in] lt_info Logical table info.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_port_membership_policy_set_f)(
    int unit, const bcmltd_op_arg_t *op_arg,
    bcmltd_sid_t lt_id, shr_port_t port,
    port_membership_policy_info_t *lt_info);

/*!
 * \brief Set the hardware for PORT_MEMBERSHIP_POLICYt.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg The operation arguments.
 * \param [in] lt_id Logical table ID.
 * \param [in] port Logical port.
 * \param [out] lt_info Logical table info.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_port_membership_policy_get_f)(
    int unit, const bcmltd_op_arg_t *op_arg,
    bcmltd_sid_t lt_id, shr_port_t port,
    port_membership_policy_info_t *lt_info);

/*!
 * \brief Get the per port modid.
 *
 * \param [in] unit Unit number.
 * \param [in] port Logical port.
 * \param [out] modid Module ID.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_port_modid_get_f)(int unit,
                                       int port,
                                       int *modid);

/*!
 * \brief Get the per port system source.
 *
 * \param [in] unit Unit number.
 * \param [in] port Logical port.
 * \param [out] system source
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_port_system_source_get_f)(int unit,
                                               int port,
                                               int *system_source);

/*!
 * \brief Set the hardware for PORT_SYSTEM_DESTINATIONt.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table ID.
 * \param [in] set Set indicator.
 * \param [in] entry Logical table entry info.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_port_system_dest_set_f)(int unit,
                                             const bcmltd_op_arg_t *op_arg,
                                             bcmltd_sid_t lt_id,
                                             bool set,
                                             port_system_dest_entry_t *entry);

/*!
 * \brief Set the hardware for PORT_TRUNK_PORT_CONTROLt.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table ID.
 * \param [in] entry Logical table entry info.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_port_trunk_pctl_set_f)(int unit,
                                            const bcmltd_op_arg_t *op_arg,
                                            bcmltd_sid_t lt_id,
                                            port_trunk_pctl_entry_t *entry);

/*!
 * \brief Set the hardware for PORT_EGR_MIRRORt.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table ID.
 * \param [in] cfg Data structure to save the info of PORT_EGR_MIRRORt entry.
 * \param [in] mirror_enable Mirror enable.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_port_egress_mirror_set_f)(int unit,
                                               const bcmltd_op_arg_t *op_arg,
                                               bcmltd_sid_t lt_id,
                                               port_emirror_t *cfg,
                                               bool mirror_enable);

/*!
 * \brief Set the hardware for PORT_ING_MIRRORt.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table ID.
 * \param [in] cfg Data structure to save the info of PORT_ING_MIRRORt entry.
 * \param [in] mirror_enable Mirror enable.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_port_ingress_mirror_set_f)(int unit,
                                                const bcmltd_op_arg_t *op_arg,
                                                bcmltd_sid_t lt_id,
                                                port_imirror_t *cfg,
                                                bool mirror_enable);
/*!
 * \brief Get the hardware for PORT_ING_MIRRORt.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table ID.
 * \param [in] cfg Data structure to save the info of PORT_ING_MIRRORt entry.
 * \param [out] mirror_enable Mirror enable.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_port_ingress_mirror_get_f)(int unit,
                                                const bcmltd_op_arg_t *op_arg,
                                                bcmltd_sid_t lt_id,
                                                port_imirror_t *cfg,
                                                bool *mirror_enable);

/*!
 * \brief Set the hardware for PORT_MIRRORt.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table ID.
 * \param [in] mirror_op Type of operation (see \ref bcmcth_port_mirror_op_t).
 * \param [in] mirror_inst Data structure to save the info of PORT_MIRRORt entry.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_port_mirror_set_f)(int unit,
                                        const bcmltd_op_arg_t *op_arg,
                                        bcmltd_sid_t lt_id,
                                        bcmcth_port_mirror_op_t mirror_op,
                                        port_mirror_t *mirror_inst);

/*!
 * \brief Set the hardware for PORT_SYSTEM_ING_MIRROR_PROFILEt.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table ID.
 * \param [in] tbl_inst Port system profile instance ID.
 * \param [in] port_system_profile_id Port system profile ID.
 * \param [in] mirror_inst Mirror instance.
 * \param [in] mirror_enable Mirror enable.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_port_system_ing_mirror_profile_set_f)(
    int unit, const bcmltd_op_arg_t *op_arg,
    bcmltd_sid_t lt_id, int tbl_inst, uint16_t port_system_profile_id,
    uint32_t mirror_inst, bool mirror_enable);


/*!
 * \brief PORT driver object
 *
 * PORT driver is used to provide the chip specific information.
 *
 * Each device should provide its own instance to BCMCTH PORT module by
 * \ref bcmcth_port_drv_init from the top layer. BCMCTH PORT internally
 * uses this interface to get the corresponding information.
 */
typedef struct bcmcth_port_drv_s {
    /*! Init the PORT module. */
    bcmcth_port_init_f port_init;

    /*! De-init the PORT module. */
    bcmcth_port_deinit_f port_deinit;

    /*! Get the hardware for PORT_BRIDGEt_BRIDGEf driver. */
    bcmcth_port_bridge_get_f port_bridge_get;

    /*! Set the hardware for PORT_BRIDGEt_BRIDGEf driver. */
    bcmcth_port_bridge_set_f port_bridge_set;

    /*! Get the hardware for PORT_MEMBERSHIP_POLICYt driver. */
    bcmcth_port_membership_policy_get_f port_membership_policy_get;

    /*! Set the hardware for PORT_MEMBERSHIP_POLICYt driver. */
    bcmcth_port_membership_policy_set_f port_membership_policy_set;

    /*! Get the per port modid driver. */
    bcmcth_port_modid_get_f port_modid_get;

    /*! Get the system source port driver. */
    bcmcth_port_system_source_get_f port_system_source_get;

    /*! Set the hardware for PORT_SYSTEM_DESTINATIONt driver. */
    bcmcth_port_system_dest_set_f port_system_dest_set;

    /*! Set the hardware for PORT_TRUNK_PORT_CONTROLt driver. */
    bcmcth_port_trunk_pctl_set_f port_trunk_pctl_set;

    /*! Set the hardware for PORT_EGR_MIRRORt driver. */
    bcmcth_port_egress_mirror_set_f port_egress_mirror_set;

    /*! Set the hardware for PORT_ING_MIRRORt driver. */
    bcmcth_port_ingress_mirror_set_f port_ingress_mirror_set;

    /*! Set the hardware for PORT_MIRRORt driver. */
    bcmcth_port_mirror_set_f port_mirror_set;

    /*! Set the hardware for PORT_SYSTEM_ING_MIRROR_PROFILEt driver. */
    bcmcth_port_system_ing_mirror_profile_set_f port_system_ing_mirror_profile_set;

    /*! Set the hardware for PORT_SVP_ING_MIRRORt driver. */
    bcmcth_port_ingress_mirror_set_f port_svp_ingress_mirror_set;

    /*! Get the hardware for PORT_SVP_ING_MIRRORt driver. */
    bcmcth_port_ingress_mirror_get_f port_svp_ingress_mirror_get;
} bcmcth_port_drv_t;

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    extern bcmcth_port_drv_t *_bc##_cth_port_drv_get(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief Install the device-specific PORT driver.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot indicator.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_port_drv_init(int unit, bool warm);

/*!
 * \brief Initialize the PORT module.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot indicator.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_port_init(int unit, bool warm);

/*!
 * \brief De-Initialize the PORT.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_port_deinit(int unit);

/*!
 * \brief Enable/disable ingress mirror on a port.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table ID.
 * \param [in] mirror_op Type of operation (see \ref bcmcth_port_mirror_op_t).
 * \param [in] mirror_inst Data structure to save the info of PORT_MIRRORt entry.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 * \retval SHR_E_MEMORY Out of memory.
 * \retval SHR_E_PARAM Invalid parameter.
 */
extern int
bcmcth_port_mirror_set(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       bcmltd_sid_t lt_id,
                       bcmcth_port_mirror_op_t mirror_op,
                       port_mirror_t *mirror_inst);

/*!
 * \brief Set the hardware for PORT_BRIDGEt_BRIDGEf.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg The operation arguments.
 * \param [in] lt_id Logical table ID.
 * \param [in] port Logical port.
 * \param [in] bridge Bridge indicator.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_port_bridge_set(
    int unit,
    const bcmltd_op_arg_t *op_arg,
    bcmltd_sid_t lt_id,
    shr_port_t port,
    bool bridge);

/*!
 * \brief Set the hardware for PORT_BRIDGEt_BRIDGEf.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg The operation arguments.
 * \param [in] lt_id Logical table ID.
 * \param [in] port Logical port.
 * \param [out] bridge Bridge indicator.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_port_bridge_get(
    int unit,
    const bcmltd_op_arg_t *op_arg,
    bcmltd_sid_t lt_id,
    shr_port_t port,
    bool *bridge);

/*!
 * \brief Get the modid for the given port.
 *
 * Get the modid for the given port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] modid Module ID.
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Parameter error.
 */
extern int
bcmcth_port_modid_get(int unit,  int port, int *modid);

/*!
 * \brief Get the system source for the given port.
 *
 * Get the system source for the given port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] system_source Pointer to system_source port.
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Parameter error.
 */
extern int
bcmcth_port_system_source_get(int unit,  int port, int *system_source);


/*!
 * \brief Enable/disable ingress mirror on a port.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical tabld ID.
 * \param [in] op_arg Operation arguments.
 * \param [in] cfg Data structure to save the info of PORT_ING_MIRRORt entry.
 * \param [in] mirror_enable Enable/disable ingress mirror.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 * \retval SHR_E_MEMORY Out of memory.
 * \retval SHR_E_PARAM Invalid parameter.
 */
extern int
bcmcth_port_ingress_mirror_set(int unit,
                               const bcmltd_op_arg_t *op_arg,
                               bcmltd_sid_t sid,
                               port_imirror_t *cfg,
                               bool mirror_enable);
/*!
 * \brief Set the hardware for PORT_MEMBERSHIP_POLICYt.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table ID.
 * \param [in] port Logical port.
 * \param [in] lt_info Logical table info.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_port_membership_policy_set(int unit,
                                  const bcmltd_op_arg_t *op_arg,
                                  bcmltd_sid_t lt_id,
                                  shr_port_t port,
                                  port_membership_policy_info_t *lt_info);

/*!
 * \brief Set the hardware for PORT_MEMBERSHIP_POLICYt.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg The operation arguments.
 * \param [in] lt_id Logical table ID.
 * \param [in] port Logical port.
 * \param [in] lt_info Logical table info.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_port_membership_policy_set(
    int unit,
    const bcmltd_op_arg_t *op_arg,
    bcmltd_sid_t lt_id,
    shr_port_t port,
    port_membership_policy_info_t *lt_info);

/*!
 * \brief Set the hardware for PORT_MEMBERSHIP_POLICYt.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg The operation arguments.
 * \param [in] lt_id Logical table ID.
 * \param [in] port Logical port.
 * \param [out] lt_info Logical table info.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_port_membership_policy_get(int unit,
                                  const bcmltd_op_arg_t *op_arg,
                                  bcmltd_sid_t lt_id,
                                  shr_port_t port,
                                  port_membership_policy_info_t *lt_info);

/*!
 * \brief Enable/disable egress mirror per ingress-port and egress-port pair.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg Operation arguments.
 * \param [in] sid Logical tabld ID.
 * \param [in] cfg Data structure to save the info of PORT_EGR_MIRRORt entry.
 * \param [in] mirror_enable Enable/disable egress mirror.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 * \retval SHR_E_MEMORY Out of memory.
 * \retval SHR_E_PARAM Invalid parameter.
 */
extern int
bcmcth_port_egress_mirror_set(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              bcmltd_sid_t sid,
                              port_emirror_t *cfg,
                              bool mirror_enable);
/*!
 * \brief Set the hardware for PORT_SYSTEM_ING_MIRROR_PROFILEt.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table ID.
 * \param [in] tbl_inst Port system profile instance ID.
 * \param [in] port_system_profile_id Port system profile ID.
 * \param [in] mirror_inst Mirror instance.
 * \param [in] mirror_enable Mirror enable.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_port_system_ingress_mirror_profile_set(int unit,
                                              const bcmltd_op_arg_t *op_arg,
                                              bcmltd_sid_t lt_id,
                                              int tbl_inst,
                                              uint16_t port_system_profile_id,
                                              uint32_t mirror_inst,
                                              bool mirror_enable);

/*!
 * \brief Set the hardware for PORT_SYSTEM_DESTINATIONt.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table ID.
 * \param [in] set Set indicator.
 * \param [in] entry Logical table entry info.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_port_system_dest_set(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            bcmltd_sid_t lt_id,
                            bool set,
                            port_system_dest_entry_t *entry);

/*!
 * \brief Set the hardware for PORT_TRUNK_PORT_CONTROLt.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table ID.
 * \param [in] entry Logical table entry info.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_port_trunk_pctl_set(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           bcmltd_sid_t lt_id,
                           port_trunk_pctl_entry_t *entry);

/*!
 * \brief Enable/disable ingress mirror on a source virtual port.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical tabld ID.
 * \param [in] op_arg Operation arguments.
 * \param [in] cfg Data structure to save the info of PORT_ING_MIRRORt entry.
 * \param [in] mirror_enable Enable/disable ingress mirror.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 * \retval SHR_E_MEMORY Out of memory.
 * \retval SHR_E_PARAM Invalid parameter.
 */
extern int
bcmcth_port_svp_ingress_mirror_set(int unit,
                                   const bcmltd_op_arg_t *op_arg,
                                   bcmltd_sid_t sid,
                                   port_imirror_t *cfg,
                                   bool mirror_enable);

/*!
 * \brief Get ingress mirror status on a source virtual port.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical tabld ID.
 * \param [in] op_arg Operation arguments.
 * \param [in] cfg Data structure to save the info of PORT_ING_MIRRORt entry.
 * \param [out] mirror_enable Enable/disable ingress mirror.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 * \retval SHR_E_MEMORY Out of memory.
 * \retval SHR_E_PARAM Invalid parameter.
 */
extern int
bcmcth_port_svp_ingress_mirror_get(int unit,
                                   const bcmltd_op_arg_t *op_arg,
                                   bcmltd_sid_t sid,
                                   port_imirror_t *cfg,
                                   bool *mirror_enable);

/*!
 * \brief IMM-based database cleanup.
 *
 * Delete field array local memory manager handler.
 *
 * \param [in] unit Unit number.
 *
 * \return none.
 */
extern void
bcmcth_port_imm_db_cleanup(int unit);

/*!
 * \brief Initialize BCMPORT IMM resources for BCMPORT IMM LTs.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
extern int
bcmcth_port_imm_init(int unit);

/*!
 * \brief Derive variant based table id.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Unable to allocate required resources.
 */
typedef int (*bcmcth_port_info_get_data_f)(int unit, bcmcth_info_table_t *data);

/*!
 * \brief PORT driver object
 *
 * PORT driver is used to provide the chip specific information.
 *
 * Each device should provide its own instance to BCMCTH PORT module. BCMCTH PORT
 * internally will use this interface to get the corresponding information.
 */
typedef struct bcmcth_port_info_drv_s {

    /*! Derive variant based table id. */
    bcmcth_port_info_get_data_f          get_data;

} bcmcth_port_info_drv_t;

/*! \cond  Externs for the required functions. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
extern bcmcth_port_info_drv_t *_bd##_vu##_va##_cth_port_ing_egr_block_profile_info_drv_get(int unit);
#define BCMLTD_VARIANT_OVERRIDE
#include <bcmlrd/chip/bcmlrd_variant.h>
/*! \endcond */

#endif /* BCMCTH_PORT_DRV_H */
