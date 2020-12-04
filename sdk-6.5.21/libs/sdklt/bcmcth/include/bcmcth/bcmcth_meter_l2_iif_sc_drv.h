/*! \file bcmcth_meter_l2_iif_sc_drv.h
 *
 * This file contains storm control meter Custom handler
 * data structures and macros.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_METER_L2_IIF_SC_DRV_H
#define BCMCTH_METER_L2_IIF_SC_DRV_H

#include <shr/shr_bitop.h>
#include <bcmimm/bcmimm_int_comp.h>

/*! Structure to hold storm control meter entry. */
typedef struct bcmcth_meter_l2_iif_sc_entry_s {
    /*! Storm control meter ID. */
    uint32_t meter_id;
    /*! PIPE number. */
    int      pipe;
    /*! Byte mode status. */
    uint32_t byte_mode;
    /*! Enable status of each meter offset. */
    bool     meter_en[4];
    /*! Metering rate in kbps. */
    uint64_t rate_kbps[4];
    /*! Metering rate in pps. */
    uint64_t rate_pps[4];
    /*! Burst size in kbits. */
    uint64_t burst_kbits[4];
    /*! Burst size in packets. */
    uint64_t burst_pkts[4];
    /*! Operational metering rate in kbps. */
    uint64_t rate_kbps_oper[4];
    /*! Operational metering rate in pps. */
    uint64_t rate_pps_oper[4];
    /*! Operational burst size in kbits. */
    uint64_t burst_kbits_oper[4];
    /*! Operational burst size in packets. */
    uint64_t burst_pkts_oper[4];
    /*! HW Packet quantum value. */
    uint32_t quantum;
    /*! HW refresh count. */
    uint32_t refresh_count[4];
    /*! HW bucket count value. */
    uint32_t bucket_count[4];
    /*! HW bucket size value. */
    uint32_t bucket_size[4];
} bcmcth_meter_l2_iif_sc_entry_t;

/*! Get the default value of an LT field. */
#define METER_L2_IIF_FIELD_DEF(fdef)   ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U8) ?     \
                                           (fdef.def.u8) :                              \
                                           ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U16) ?   \
                                            (fdef.def.u16) :                            \
                                            ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U32) ?  \
                                             (fdef.def.u32) :                           \
                                             ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U64) ? \
                                              (fdef.def.u64) :                          \
                                              (fdef.def.is_true)))))

/*!
 * \brief FP meter action insert
 *
 * Insert an FP Meter action in the harwdare meter table.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  sid           Logical table ID.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  key           Input key fields.
 * \param [in]  data          Input data fields.
 * \param [in]  out           Output data fields.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_meter_l2_iif_sc_insert(int unit,
                              bcmltd_sid_t sid,
                              uint32_t trans_id,
                              const bcmltd_field_t *key,
                              const bcmltd_field_t *data,
                              bcmltd_fields_t *out);

/*!
 * \brief FP meter action update.
 *
 * Update Insert an FP Meter action in the harwdare meter table.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  sid           Logical table ID.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  key           Input key fields.
 * \param [in]  data          Input data fields.
 * \param [in]  out           Output data fields.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_meter_l2_iif_sc_update(int unit,
                              bcmltd_sid_t sid,
                              uint32_t trans_id,
                              const bcmltd_field_t *key,
                              const bcmltd_field_t *data,
                              bcmltd_fields_t *out);
/*!
 * \brief FP meter action delete
 *
 * Delete an FP Meter action from the harwdare meter table.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  sid           Logical table ID.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  key           Input key fields.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_meter_l2_iif_sc_delete(int unit,
                              bcmltd_sid_t sid,
                              uint32_t trans_id,
                              const bcmltd_field_t *key);

#endif /* BCMCTH_METER_L2_IIF_SC_DRV_H */
