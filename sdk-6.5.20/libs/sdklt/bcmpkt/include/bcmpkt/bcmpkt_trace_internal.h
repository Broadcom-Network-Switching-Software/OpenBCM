/*! \file bcmpkt_trace_internal.h
 *
 * Packet trace internal library.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPKT_TRACE_INTERNAL_H
#define BCMPKT_TRACE_INTERNAL_H

#include <shr/shr_types.h>
#include <bcmpkt/bcmpkt_trace.h>
#include <bcmpkt/bcmpkt_internal.h>

/*! Map PT ID to DOP ID. */
#define BCMPKT_TRACE_PT_TO_DOP_NUM                  0

/*! Map PT ID to DOP Group ID. */
#define BCMPKT_TRACE_PT_TO_DOP_GROUP                1

/*! Map PT ID to DOP Field ID. */
#define BCMPKT_TRACE_PT_TO_DOP_FID                  2

/*! Get PT lookup information */
#define BCMPKT_TRACE_PT_LOOKUP_INFO                 3

/*! Get PT lookup Hit/Miss information. */
#define BCMPKT_TRACE_PT_HIT_INFO                    4

/*! TRACE_PT_TO_DOP_INFO count */
#define BCMPKT_TRACE_PT_TO_DOP_INFO_COUNT           5

/*!
 * Array of TRACE field get functions for a particular device
 * type.
 */
typedef struct bcmpkt_trace_fget_s {
    /*! Trace field get function. */
    bcmpkt_field_get_f fget[BCMPKT_TRACE_FID_COUNT];
} bcmpkt_trace_fget_t;

/*!
 * Array of TRACE field address and length getter functions for a multiple
 * words field of a particular device type. *addr is output address and return
 * length.
 */
typedef struct bcmpkt_trace_figet_s {
    /*! Trace internal field get function. */
    bcmpkt_ifield_get_f fget[BCMPKT_TRACE_I_FID_COUNT];
} bcmpkt_trace_figet_t;

/*!
 * Array of TRACE DOP get functions that returns the list of DOP field IDs
 * for a a given DOP ID.
 */
typedef struct bcmpkt_trace_dop_get_s {
    /*! Trace DOP get function. */
    bcmpkt_dop_fids_get_f fget[BCMPKT_TRACE_DOP_ID_COUNT];
} bcmpkt_trace_dop_fids_get_t;

/*!
 * Array of TRACE DOP field get functions that returns the DOP field value
 * for a a given DOP field.
 */
typedef struct bcmpkt_trace_dop_fget_s {
    /*! Trace DOP field get function. */
    bcmpkt_dop_field_get_f fget[BCMPKT_TRACE_DOP_FID_COUNT];
} bcmpkt_trace_dop_fget_t;

/*!
 * Array of TRACE DOP internal attribute get functions for a particular device
 * type.
 */
typedef struct bcmpkt_trace_dop_iget_s {
    /*! Trace DOP interal field get function. */
    bcmpkt_dop_iget_f fget[BCMPKT_TRACE_DOP_I_FID_COUNT];
} bcmpkt_trace_dop_iget_t;

/*!
 * Array of PT to DOP Info  get functions for a particular device type.
 */
typedef struct bcmpkt_trace_pt_to_dop_info_get_s {
    /*! Trace DOP field get function. */
    bcmpkt_pt_to_dop_info_get_f fget[BCMPKT_TRACE_PT_TO_DOP_INFO_COUNT];
} bcmpkt_trace_pt_to_dop_info_get_t;

/*!
 * Structure mapping global DOP ID to PT ID to chip specific local
 * DOP ID and Group ID.
 */
typedef struct bcmpkt_trace_global_to_local_dop_id_map_s {
    /*! Global DOP number. */
    uint32_t    global_dop_num;

    /*! Chip specific local DOP ID. */
    uint16_t    local_dop_id;

    /*! DOP group ID (0:Ingress, 1:Egress). */
    uint8_t     group_id;
} bcmpkt_trace_global_to_local_dop_id_map_t;

/*!
 * Array of mapping between global DOP ID to chip specific local
 * DOP ID and Group ID.
 */
typedef struct bcmpkt_trace_dop_id_map_s {
    /*! Mapping global DOP ID to local DOP Id. */
    bcmpkt_trace_global_to_local_dop_id_map_t dop_id_map[BCMPKT_TRACE_DOP_ID_COUNT];
} bcmpkt_trace_dop_id_map_t;

/*!
 * Structure mapping between PT ID to <DOP Number, DOP Group Id, DOP Field Id>.
 */
typedef struct bcmpkt_trace_pt_to_dop_map_s {
    /*! Physical table name. */
    char        *pt_name;

    /*! Global DOP Number. */
    uint32_t    dop_num;

    /*! DOP group ID (0:Ingress, 1:Egress). */
    uint32_t    group;

    /*! DOP Field ID. */
    uint32_t    fid;
} bcmpkt_trace_pt_to_dop_map_t;

/*!
 * \brief Init packet trace profile register.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int (*bcmpkt_trace_profile_init_f)(int unit);

/*!
 * \brief Get loopback pipe for trace packet.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int (*bcmpkt_trace_get_lbpipe_f)(int unit, int pipe, int *lbpipe);

/*!
 * \brief Read packet trace raw data.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int (*bcmpkt_trace_read_data_f)(int unit, int port, uint32_t *len, uint8_t *data);

/*!
 * \brief Read packet trace raw data for a given DOP ID.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int (*bcmpkt_trace_read_dop_data_f)(int unit, int port, uint32_t dop_id,
                                            uint8_t group_id, uint32_t *len,
                                            uint8_t *data);

/*!
 * \brief Maps a global DOP number to chip specific DOP ID and Group ID.
 *
 * \param [in]      unit        unit Unit number.
 * \param [in]      dop_num     global DOP number.
 * \param [out]     dop_info    DOP Information.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int (*bcmpkt_trace_dop_info_get_f)(int unit, uint32_t dop_num,
                                           bcmpkt_trace_dop_info_t *dop_info);

/*!
 * \brief Get chip specific configuration value in txpmd header for trace packet.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int(*bcmpkt_trace_txpmd_chip_val_get_f)(uint32_t dev_type, int fid, int *val);

/*!
 * \brief Shift trace data for trace counter.
 *
 * \param [in] counter Pointer to counter data.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef void(*bcmpkt_trace_counter_shift_f)(uint32_t *counter, uint32_t *shift_counter);

/*!
 * Array of TRACE drviver functions.
 */
typedef struct bcmpkt_trace_drv_s {

    /*! Trace profile init function. */
    bcmpkt_trace_profile_init_f profile_init;

    /*! Trace raw data read function. */
    bcmpkt_trace_get_lbpipe_f get_lbpipe;

    /*! Trace raw data read function. */
    bcmpkt_trace_read_data_f data_read;

    /*! Trace raw dop data read function. */
    bcmpkt_trace_read_dop_data_f dop_data_read;

    /*! Trace DOP number to DOP information map get function. */
    bcmpkt_trace_dop_info_get_f dop_info_get;

    /*! Trace packet txpmd chip specific value get function. */
    bcmpkt_trace_txpmd_chip_val_get_f get_txpmd_val;

    /*! Trace counter data shift function. */
    bcmpkt_trace_counter_shift_f counter_shift;
} bcmpkt_trace_drv_t;

/*!
 * \brief Maps a given PT ID to DOP ID, DOP group and DOP field ID.
 *
 * \param [in]      unit            Device number.
 * \param [in]      pt_name         PT Name.
 * \param [in]      data            DOP Data.
 * \param [out]     dop_id          DOP ID.
 * \param [out]     group_id        DOP Group ID.
 * \param [out]     fid             DOP field Id.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not support the field.
 */
extern int
bcmpkt_trace_pt_to_dop_info_get(int unit, const char *pt_name,
                                uint8_t *data,
                                uint32_t *dop_id,
                                uint32_t *group_id,
                                uint32_t *fid);

/*!
 * \brief Get trace driver handle.
 * \param [in] unit Unit number.
 */
extern bcmpkt_trace_drv_t*
bcmpkt_trace_drv_get(int unit);

/*! Mask with bit count. */
#define MASK(_bn) (((uint32_t)0x1<<(_bn))-1)

/*! Extract Field Value. */
#define WORD_FIELD_GET(_d,_s,_l) (((_d) >> (_s)) & MASK(_l))

/*! \cond  Externs for trace drop reason decode functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    extern void _bd##_trace_drop_reason_decode(const uint32_t* data, bcmpkt_trace_drop_reasons_t* reasons);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! \cond  Externs for trace counter decode functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    extern void _bd##_trace_counter_decode(const uint32_t* data, bcmpkt_trace_counters_t* reasons);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! \cond  Externs for trace driver attach functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    extern int _bd##_trace_drv_attach(bcmpkt_trace_drv_t *drv);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

#endif /* BCMPKT_TRACE_INTERNAL_H */
