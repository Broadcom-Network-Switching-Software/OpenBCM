/*! \file bcmcth_meter_sc_util.h
 *
 * This file contains SC Meter Custom handler
 * data structures and macros.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_METER_SC_UTIL_H
#define BCMCTH_METER_SC_UTIL_H

#include <shr/shr_bitop.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmcth/generated/meter_ha.h>

/*! Meter mode bytes. */
#define METER_SC_BYTE_MODE       true
/*! Meter mode packets. */
#define METER_SC_PACKET_MODE     false

/*!
 * \brief Packet quantum information structure.
 *
 * Metering rate and burst size details at each quantum level.
 */
typedef struct bcmcth_meter_sc_pkt_quantum_info_s {
    /*! Packet quantum value at this quantum level. */
    uint32_t    quantum;
    /*! Minimum metering rate at this quantum level. */
    uint32_t    min_rate;
    /*! Maximum metering rate at this quantum level. */
    uint32_t    max_rate;
} bcmcth_meter_sc_pkt_quantum_info_t;

/*!
 * \brief Storm control meter HW params.
 *
 * Storm control meter entry HW params
 */
typedef struct bcmcth_meter_sc_hw_params_s {
    /*! Refresh count. */
    uint32_t    refresh_cnt;
    /*! Bucket size. */
    uint32_t    bkt_sz;
    /*! Bucket count. */
    uint32_t    bkt_cnt;
} bcmcth_meter_sc_hw_params_t;

/*!
 * \brief Storm control meter chip attribute structure.
 *
 * Chip specific attributes for storm control meter.
 */
typedef struct bcmcth_meter_sc_attrib_s {
    /*! Maximum number of ports. */
    uint8_t max_ports;
    /*! Number of meters per port. */
    uint8_t meters_per_port;
    /*! Number of packet quantum levels. */
    uint8_t pkt_quant_levels;
    /*! Min metering rate in kbps. */
    uint32_t meter_kbps_min;
    /*! Max bucket size. */
    uint32_t max_bkt_sz;
    /*! Max refresh count. */
    uint32_t max_refresh_cnt;
    /*! Number of meter refresh cycles per second */
    uint32_t num_refresh_per_sec;
    /*! Minimum number of tokens in each refresh cycle. */
    uint32_t min_tokens_per_refresh;
    /*! Storm control register name. */
    uint32_t config_sid;
    /*! Storm control meter name. */
    uint32_t meter_sid;
    /*! Storm control meter byte mode field name. */
    uint32_t byte_mode_fid;
    /*! Storm control meter packet quantum field name. */
    uint32_t pkt_quantum_fid;
    /*! Storm control meter refresh count field name. */
    uint32_t refresh_count_fid;
    /*! Storm control meter bucket size field name. */
    uint32_t bucket_size_fid;
    /*! Storm control meter bucket count field name. */
    uint32_t bucket_cnt_fid;
    /*! Storm control meter bucket size to kbit mapping. */
    uint32_t *bkt_sz_to_kbit_map;
    /*! Storm control meter bucket size to token mapping. */
    uint32_t *bkt_sz_to_token_map;
    /*! Storm control meter packet quantum structure. */
    bcmcth_meter_sc_pkt_quantum_info_t *pkt_quant_info;
} bcmcth_meter_sc_attrib_t;

/*! Storm control meter insert. */
typedef int (*meter_sc_insert_f)(int unit,
                                 const bcmltd_op_arg_t *op_arg,
                                 bcmcth_meter_sc_entry_t *sc_entry);

/*! Storm control meter lookup. */
typedef int (*meter_sc_lookup_f)(int unit,
                                 const bcmltd_op_arg_t *op_arg,
                                 uint32_t port, uint32_t meter_id,
                                 bcmcth_meter_sc_entry_t **sc_entry);

/*! Storm control meter delete. */
typedef int (*meter_sc_delete_f)(int unit,
                                 const bcmltd_op_arg_t *op_arg,
                                 bcmcth_meter_sc_entry_t *sc_entry);

/*! Get storm control meter byte mode. */
typedef int (*meter_sc_bytemode_get_f)(int unit,
                                       const bcmltd_op_arg_t *op_arg,
                                       uint32_t port,
                                       uint32_t *byte_mode);

/*! Get storm control meter attributes. */
typedef int (*meter_sc_attrib_get_f)(int unit,
                                     bcmcth_meter_sc_attrib_t **attr);

/*!
 * \brief Storm control meter driver.
 */
typedef struct bcmcth_meter_sc_drv_s {
    /*! SC meter insert. */
    meter_sc_insert_f ins;
    /*! SC meter lookup. */
    meter_sc_lookup_f lkp;
    /*! SC meter delete. */
    meter_sc_delete_f del;
    /*! SC meter update. */
    meter_sc_bytemode_get_f bytemode_get;
    /*! Get SC meter attributes. */
    meter_sc_attrib_get_f attrib_get;
} bcmcth_meter_sc_drv_t;

/*!
 * \brief Storm control meter state structure.
 *
 * Meter state of all storm control meters of all ports.
 */
typedef struct bcmcth_meter_sc_sw_state_s {
    /*! Storm control meter entries list. */
    bcmcth_meter_sc_entry_t *meter_entry;
    /*! Storm control meter entries backup list. */
    bcmcth_meter_sc_entry_t *bkp_entry;
} bcmcth_meter_sc_sw_state_t;

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern bcmcth_meter_sc_drv_t *_bc##_cth_meter_sc_drv_get(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! Get the default value of an LT field. */
#define METER_SC_FIELD_DEF(fdef)   ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U8) ?    \
                                    (fdef.def.u8) :                             \
                                    ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U16) ?  \
                                     (fdef.def.u16) :                           \
                                     ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U32) ? \
                                      (fdef.def.u32) :                          \
                                      ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U64) ?\
                                       (fdef.def.u64) :                         \
                                       (fdef.def.is_true)))))

/*! Get the minimum supported value of an LT field. */
#define METER_SC_FIELD_MIN(fdef)   ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U8) ?    \
                                    (fdef.min.u8) :                             \
                                    ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U16) ?  \
                                     (fdef.min.u16) :                           \
                                     ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U32) ? \
                                      (fdef.min.u32) :                          \
                                      ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U64) ?\
                                       (fdef.min.u64) :                         \
                                       (fdef.min.is_true)))))

/*! Get the maximum supported value of an LT field. */
#define METER_SC_FIELD_MAX(fdef)   ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U8) ?    \
                                    (fdef.max.u8) :                             \
                                    ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U16) ?  \
                                     (fdef.max.u16) :                           \
                                     ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U32) ? \
                                      (fdef.max.u32) :                          \
                                      ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U64) ?\
                                       (fdef.max.u64) :                         \
                                       (fdef.max.is_true)))))

/*!
 * \brief Get pointer to storm control meter driver device structure.
 *
 * \param [in] unit Unit number.
 * \param [out] drv Chip driver structure.
 *
 * \return Pointer to device structure, or NULL if not found.
 */
extern int
bcmcth_meter_sc_drv_get (int unit,
                         bcmcth_meter_sc_drv_t **drv);
/*!
 * \brief Get pointer to storm control meter sw state structure.
 *
 * \param [in] unit Unit number.
 * \param [out] ptr Sw state structure.
 *
 * \return Pointer to device structure, or NULL if not found.
 */
extern int
bcmcth_meter_sc_sw_state_get(int unit,
                             bcmcth_meter_sc_sw_state_t **ptr);

#endif /* BCMCTH_METER_SC_UTIL_H */
