/*! \file bcmcth_l2.h
 *
 * Top-level BCMCTH L2 APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_L2_H
#define BCMCTH_L2_H

#include <sal/sal_types.h>
#include <shr/shr_bitop.h>
#include <shr/shr_thread.h>
#include <sal/sal_msgq.h>
#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmcth/bcmcth_info_table_entry.h>
#include <bcmcth/generated/l2_ha.h>

/*! MAC address bytes. */
#define BCM_MAC_BYTES                       6

/*! Maximum number of devices supported. */
#define BCML2_DEV_NUM_MAX                   BCMDRD_CONFIG_MAX_UNITS

/*! HA subcomponent id for Learn driver. */
#define BCML2_LEARN_DRV_COMP_ID             (1)

/*! HA subcomponent id for Learn control. */
#define BCML2_LEARN_CTRL_COMP_ID            (2)

/*! HA subcomponent id for learn cache offset. */
#define BCML2_LEARN_CACHE_OFFSET_COMP_ID    (3)

/*! HA subcomponent id for learn cache. */
#define BCML2_L2_CACHE_COMP_ID              (4)

/*! HA subcomponent id for learn cache entry. */
#define BCML2_L2_CACHE_ENTRY_COMP_ID        (5)

/*! For bitmap declare, not the real pipe number of device. */
#define BCML2_PIPE_NUM_MAX                  8

/*! Data structure to save the pipe info. */
typedef struct bcmcth_l2_pipe_info_s {

    /*! Pipe map. */
    SHR_BITDCLNAME(map, BCML2_PIPE_NUM_MAX);

    /*! Max index for active pipes in bitmap. */
    uint32_t max_idx;

    /*! Max entry count in each pipe. */
    uint32_t tbl_size;

    /*! Entry size in words. */
    uint32_t entry_wsize;

    /*! Sequence number for active pipes. */
    uint32_t act_seq[BCML2_PIPE_NUM_MAX];

    /*! Count of active pipes. */
    uint32_t act_cnt;

} bcmcth_l2_pipe_info_t;

/*! Data structure for traverse status. */
typedef struct l2_trav_status_s {
    /*! H/W cache empty indicator. */
    int cache_empty;

    /*! Indicator that new entries are learned. */
    int new_learn;
} l2_trav_status_t;

/*! Data structure to save the info of HW cache entry. */
typedef struct l2_learn_addr_s {
    /*! PIPE. */
    uint8_t pipe;

    /*! L2_LEARN_DATA_ID. */
    uint16_t data_id;

    /*! VLAN ID. */
    uint16_t vid;

    /*! MAC address. */
    shr_mac_t mac;

    /*! Source type. */
    uint8_t src_type;

    /*! TRUNK ID. */
    uint16_t trunk_id;

    /*! Module ID. */
    uint8_t modid;

    /*! Port number. */
    shr_port_t port;
} l2_learn_addr_t;

/*! L2 destination. */
typedef union l2_dest_s {
    /*! Source port number. */
    shr_port_t port;

    /*! Source trunk ID. */
    uint16_t trunk_id;

    /*! Source L2 multicast group ID. */
    uint16_t l2_mc_grp_id;

    /*! Source L3 multicast group ID. */
    uint16_t l3_mc_grp_id;

    /*! Source next hop ID. */
    uint16_t nhop_id;

    /*! Source ECMP group ID. */
    uint16_t ecmp_grp_id;

    /*! Data. */
    uint16_t data;
} l2_dest_t;

/*! L2 destination type. */
typedef enum l2_dest_type_e {
    L2_DEST_TYPE_PORT       = 0,
    L2_DEST_TYPE_TRUNK      = 1,
    L2_DEST_TYPE_L2_MC_GRP  = 2,
    L2_DEST_TYPE_L3_MC_GRP  = 3,
    L2_DEST_TYPE_NHOP       = 4,
    L2_DEST_TYPE_ECMP_GRP   = 5
} l2_dest_type_t;

/*! Data structure to save the info of HW cache entry. */
typedef struct l2_vfi_learn_addr_s {
    /*! PIPE. */
    uint8_t pipe;

    /*! L2_VFI_LEARN_DATA_ID. */
    uint16_t data_id;

    /*! VFI. */
    uint16_t vfi;

    /*! MAC address. */
    shr_mac_t mac;

    /*! Source virtual port. */
    uint16_t svp;

    /*! Source type. */
    l2_dest_type_t src_type;

    /*! Source value. */
    l2_dest_t source;
} l2_vfi_learn_addr_t;

/*! Number of CACHE_FIELD of LEARN_CACHE_DATAt */
#define NUM_LEARN_CACHE_FIELDS  8

/*! Cache fields */
typedef struct learn_cache_fields_s {
    /*! Data of cache fields. */
    uint16_t data[NUM_LEARN_CACHE_FIELDS];
} learn_cache_fields_t;

/*! Data structure to save the info of learn cache entry. */
typedef struct learn_cache_s {
    /*! learn_cache_data_id. */
    uint16_t learn_cache_data_id;

    /*! Pipe number.*/
    uint8_t pipe;

    /*! Profile index. */
    uint8_t profile_idx;

    /*! Cache fields */
    learn_cache_fields_t cache_flds;
} learn_cache_t;

/*! L2 Learn control structure. */
typedef struct bcmcth_l2_learn_ctrl_s {
    /*! L2 Learn thread control. */
    shr_thread_t thread_ctrl;

    /*! Thread polling interval in usec. */
    sal_usecs_t interval;

    /*! Thread active indicator. */
    int active;

} bcmcth_l2_learn_ctrl_t;

/*!
 * \brief Enable L2 learn.
 *
 * \param [in] unit Unit number.
 * \param [in] tid Transaction ID.
 * \param [in] interactive Interactive path indicator.
 * \param [in] Enable Enable indicator.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Unable to allocate required resources.
 */
typedef int (*bcmcth_l2_drv_hw_enable_f)(int unit, uint32_t tid, bool enable);
/*!
 * \brief L2 learn interrupt handler.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_param Additional interrupt context.
 *
 * \retval void.
 */
typedef void (*bcmcth_l2_drv_intr_handler_f)(int unit, uint32_t intr_param);

/*!
 * \brief Initialize L2 learn driver database.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot indicator.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Unable to allocate required resources.
 */
typedef int (*bcmcth_l2_drv_init_f)(int unit, bool warm);

/*!
 * \brief Destroy L2 learn driver database.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_l2_drv_cleanup_f)(int unit);

/*!
 * \brief Enable/Disable L2 learn interrupts.
 *
 * \param [in] unit Unit number.
 * \param [in] Enable Indicator to enable L2 Learn interrupts.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_l2_drv_intr_enable_f)(int unit, int enable);

/*!
 * \brief Register L2 learn interrupt handler.
 *
 * Register L2 learning ISR.
 *
 * \param [in] unit Unit number.
 * \param [in] handler L2 learn interrupt handler.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fail to register L2 learn interrupt handler.
 */
typedef int (*bcmcth_l2_drv_intr_handler_set_f)(int unit,
                                        bcmcth_l2_drv_intr_handler_f handler);

/*!
 * \brief Traverse the L2 Learn cache table.
 *
 * Traverse all entries of L2 Learn cache table and invoke the callback.
 *
 * \param [in] unit Unit number.
 * \param [out] status Traverse status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Unable to allocate required resources.
 */
typedef int (*bcmcth_l2_drv_cache_traverse_f)(int unit,
                                              l2_trav_status_t *status);

/*!
 * \brief Delete the Learn cache entry.
 *
 * Delete the L2 Learn cache entry.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID.
 * \param [in] data Learn cache entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fail to delete the Learn cache entry.
 */
typedef int (*bcmcth_l2_drv_entry_delete_f)(int unit, void *data);

/*!
 * \brief Parse the learn control entry.
 *
 * Parse the L2 learn control entry.
 *
 * \param [in] unit Unit number.
 * \param [in] key Learn control table entry key fields.
 * \param [in] data Learn control table entry data fields.
 * \param [out] lt_info Parsed result for Learn control LT entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fail to parse the Learn control entry.
 */
typedef int (*bcmcth_l2_drv_entry_parse_f)(int unit, const bcmltd_field_t *key,
                                   const bcmltd_field_t *data, void *lt_info);

/*!
 * \brief L2 driver object
 *
 * L2 driver is used to provide the chip specific information.
 *
 * Each device should provide its own instance to BCMCTH L2 module by
 * \ref bcmcth_l2_drv_init from the top layer. BCMCTH L2 internally
 * will use this interface to get the corresponding information.
 */
typedef struct bcmcth_l2_drv_s {
    /*! L2 learn control logical table id. */
    bcmltd_sid_t cfg_sid;

    /*! L2 learn data logical table id. */
    bcmltd_sid_t state_sid;

    /*! L2 learn data physical table id. */
    bcmdrd_sid_t state_ptsid;

    /*! L2 learn data pipe information. */
    bcmcth_l2_pipe_info_t pipe_info;

    /*! L2 learn software control object. */
    bcmcth_l2_learn_ctrl_t *learn_ctrl;

    /*! L2 learn report control object. */
    bcmcth_l2_learn_report_ctrl_t *report_ctrl;

    /*! Init the L2 learn driver. */
    bcmcth_l2_drv_init_f init;

    /*! cleanup the L2 learn driver. */
    bcmcth_l2_drv_cleanup_f cleanup;

    /*! Enable the L2 hardware learning. */
    bcmcth_l2_drv_hw_enable_f hw_enable;

    /*! Enable L2 learn interrupts. */
    bcmcth_l2_drv_intr_enable_f intr_enable;

    /*! Register L2 learn interrupt handler. */
    bcmcth_l2_drv_intr_handler_set_f intr_handler_set;

    /*! Traverse the HW cache table. */
    bcmcth_l2_drv_cache_traverse_f cache_traverse;

    /*! Delete the entry in HW cache table. */
    bcmcth_l2_drv_entry_delete_f entry_delete;

    /*! Parse the entry in learn control logical table. */
    bcmcth_l2_drv_entry_parse_f entry_parse;
} bcmcth_l2_drv_t;

/*!
 * \brief Get the pipe information for physical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol id of physical table.
 * \param [out] pi Pipe info structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid parameter.
 */
extern int
bcmcth_l2_pipe_info_get(int unit, bcmdrd_sid_t sid, bcmcth_l2_pipe_info_t *pi);

/*!
 * \brief Attach L2 device driver.
 *
 * Initialize device features and install base driver functions.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmcth_l2_attach(int unit, bool warm);

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_cth_l2_attach(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! \cond  Externs for the required functions. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
extern int _bd##_vu##_va##_cth_l2_var_attach(int unit);
#define BCMLTD_VARIANT_OVERRIDE
#include <bcmlrd/chip/bcmlrd_variant.h>
/*! \endcond */

/*!
 * \brief Clean up L2 device driver.
 *
 * Release any resources allocated during attach.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmcth_l2_detach(int unit);

/*!
 * \brief Get pointer to L2 driver structure.
 *
 * \param [in] unit Unit number.
 *
 * \return Pointer to driver structure, or NULL if not found.
 */
extern bcmcth_l2_drv_t *
bcmcth_l2_drv_get(int unit);

/*!
 * \brief Initialize the device-specific L2 driver.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot indicator.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_l2_drv_init(int unit, bool warm);

/*!
 * \brief Destroy L2 learn driver database.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_l2_drv_cleanup(int unit);

/*!
 * \brief Enable L2 hardware learning.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID.
 * \param [in] enable Enable indicator.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Unable to allocate required resources.
 */
extern int
bcmcth_l2_drv_hw_enable(int unit, uint32_t trans_id, bool enable);

/*!
 * \brief Enable/Disable L2 learn interrupts.
 *
 * \param [in] unit Unit number.
 * \param [in] enable Indicator to enable L2 Learn interrupts.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_l2_drv_intr_enable(int unit, int enable);

/*!
 * \brief Register L2 learn interrupt handler.
 *
 * Register L2 Learn interrupt handler.
 *
 * \param [in] unit Unit number.
 * \param [in] learn_isr L2 learning ISR.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fail to register L2 learing ISR.
 */
extern int
bcmcth_l2_drv_intr_handler_set(int unit, bcmcth_l2_drv_intr_handler_f learn_isr);

/*!
 * \brief Traverse the L2 learn cache table.
 *
 * Traverse all entries of L2 learn cache table and invoke the callback.
 *
 * \param [in] unit Unit number.
 * \param [out] status Travese status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Unable to allocate required resources.
 */
extern int
bcmcth_l2_drv_cache_traverse(int unit, l2_trav_status_t *status);

/*!
 * \brief Delete the learn cache entry.
 *
 * Delete the L2 learn cache entry.
 *
 * \param [in] unit Unit number.
 * \param [in] data Learn cache entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fail to delete the Learn cache entry.
 */
extern int
bcmcth_l2_drv_entry_delete(int unit, void *data);

/*!
 * \brief Parse the learn control entry.
 *
 * Parse the L2 learn control entry.
 *
 * \param [in] unit Unit number.
 * \param [in] key Learn control table entry key fields.
 * \param [in] data Learn control table entry data fields.
 * \param [out] lt_info Parsed result for Learn control LT entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fail to parse the Learn control entry.
 */
extern int
bcmcth_l2_drv_entry_parse(int unit, const bcmltd_field_t *key,
                          const bcmltd_field_t *data, void *lt_info);


/*!
 * \brief Register L2 learn IMM LTs callback functions to IMM.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Unable to allocate required resources.
 */
extern int
bcmcth_l2_learn_imm_register(int unit);

/*!
 * \brief Initialize L2 learn control database.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot indicator.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Unable to allocate required resources.
 */
extern int
bcmcth_l2_learn_init(int unit, bool warm);

/*!
 * \brief Destroy L2 learn control database.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_l2_learn_cleanup(int unit);

/*!
 * \brief Start L2 learn.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Fail to start L2 learn.
 */
extern int
bcmcth_l2_learn_start(int unit);

/*!
 * \brief Stop L2 learn.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Fail to stop L2 learn.
 */
extern int
bcmcth_l2_learn_stop(int unit);

/*!
 * \brief Enable or disable L2 learn.
 *
 * Start or stop L2 learn thread.
 * Enable or disable L2 learn interrupts.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID.
 * \param [in] enable Enable or disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT L2 learn control is not initialized.
 * \retval SHR_E_MEMORY Unable to allocate required resources.
 */
extern int
bcmcth_l2_learn_report_enable(int unit, uint32_t trans_id, bool enable);

/*!
 * \brief Poll L2 learning cache slowly.
 *
 * When L2_HOST_TAB is full, we cannot poll L2 learning cache aggressively.
 * When L2_HOST_TAB is not full, we can poll learning cache aggressively.
 *
 * \param [in] unit Unit number.
 * \param [in] slow_poll Poll L2 learning cache mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fail to enable 'slow_poll'.
 */
extern int
bcmcth_l2_learn_slow_poll_enable(int unit, bool slow_poll);

#endif /* BCMCTH_L2_H */
