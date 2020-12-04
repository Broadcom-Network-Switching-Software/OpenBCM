/*! \file bcmcth_meter_action_internal.h
 *
 * This file contains FP Meter action Custom handler
 * data structures and macros.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_METER_ACTION_INTERNAL_H
#define BCMCTH_METER_ACTION_INTERNAL_H

#include <shr/shr_bitop.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmcth/generated/meter_ha.h>

/*! Action entry is succesfully installed. */
#define METER_ACTION_ENTRY_STATE_VALID       (0)
/*! Some/all actions of this entry are not enabled. */
#define METER_ACTION_ENTRY_STATE_INVALID     (1)
/*! Corresponding PDD has actions exceeding hw capability */
#define METER_ACTION_PDD_WIDTH_EXCEEDS       (2)

/*! Structure to define FID mapping info. */
typedef struct bcmcth_meter_fid_info_s {
    /*! Mapped fid. */
    uint32_t map_fid;
} bcmcth_meter_fid_info_t;

/*! Structure to define Meter action PDD. */
typedef struct bcmcth_meter_pdd_cont_info_s {
    /*! Number of containers used by this action. */
    uint32_t    num_conts;
    /*! Physical container IDs. */
    uint32_t    cont_ids[4];
    /*! Bit width of the containers. */
    uint32_t    cont_width[4];
} bcmcth_meter_pdd_cont_info_t;

/*! Structure for FP meter action PDD. */
typedef struct bcmcth_meter_action_pdd_lt_info_s {
    /*! Logical table ID. */
    uint32_t                     ltid;
    /*! Logical table key field ID. */
    uint32_t                     key_fid;
    /*! Logical table pipe field ID. */
    uint32_t                     pipe_fid;
    /*! Number of actions currently supported by the PDD. */
    uint32_t                     action_count;
    /*! Pointer to fid mapping. */
    bcmcth_meter_fid_info_t     *fid_info;
    /*! Pointer to list of PDD action offsets in the hw table. */
    bcmcth_meter_pdd_cont_info_t  *cont_info;
} bcmcth_meter_action_pdd_lt_info_t;

/*! Structure for FP meter action PDD. */
typedef struct bcmcth_meter_action_pdd_pt_info_s {
    /*! Physical table ID of the action PDD table. */
    uint32_t                     pt_id;
    /*! PDD table field ID. */
    uint32_t                     pt_fid;
    /*! Number of PDD profiles supported. */
    uint32_t                     num_profiles;
    /*! Maximum number of actions that can be supported by the PDD. */
    uint32_t                     pdd_bmp_size;
} bcmcth_meter_action_pdd_pt_info_t;

/*! Structure for FP meter action SBR. */
typedef struct bcmcth_meter_action_sbr_lt_info_s {
    /*! Logical table ID. */
    uint32_t                     ltid;
    /*! Logical table key field ID. */
    uint32_t                     key_fid;
    /*! Logical table pipe field ID. */
    uint32_t                     pipe_fid;
    /*! Number of actions currently supported by the SBR. */
    uint32_t                     action_count;
    /*! Pointer to fid mapping. */
    bcmcth_meter_fid_info_t     *fid_info;
} bcmcth_meter_action_sbr_lt_info_t;

/*! Structure for FP meter action SBR. */
typedef struct bcmcth_meter_action_sbr_pt_info_s {
    /*! Physical table ID of the action PDD table. */
    uint32_t                     pt_id;
    /*! PDD table field ID. */
    uint32_t                     pt_fid;
    /*! Number of bits to indicate priority of a particular action. */
    uint32_t                     bits_per_pri;
    /*! Number of PDD profiles supported. */
    uint32_t                     num_profiles;
} bcmcth_meter_action_sbr_pt_info_t;

/*! Structure to define Meter actions. */
typedef struct bcmcth_meter_action_offset_s {
    /*! Physical field start bit. */
    uint32_t    s_bit;
} bcmcth_meter_action_offset_t;

/*! Structure for FP meter actions. */
typedef struct bcmcth_meter_action_lt_info_s {
    /*! Logical table ID list. */
    uint32_t                    ltid;
    /*! PIPE config LT ID. */
    uint32_t                    pipe_ltid;
    /*! PIPE config field ID. */
    uint32_t                    pipe_cfg_fid;
    /*! Key field ID. */
    uint32_t                    key_fid;
    /*! Logical table pipe field ID. */
    uint32_t                    pipe_fid;
    /*! Logical table key field ID. */
    uint32_t                    pdd_fid;
    /*! Logical table key field ID. */
    uint32_t                    oper_fid;
    /*! Pointer to fid mapping. */
    bcmcth_meter_fid_info_t         *fid_info;
    /*! Pointer to list of meter action offsets in the hw table. */
    bcmcth_meter_action_offset_t    *offset_info;
} bcmcth_meter_action_lt_info_t;

/*! Structure for FP meter actions. */
typedef struct bcmcth_meter_action_pt_info_s {
    /*! List of physical table IDs for the Meter action tables. */
    uint32_t                    *pt_id;
    /*! Width of each color action field. */
    uint32_t                    pt_width;
} bcmcth_meter_action_pt_info_t;

/*! FP meter action db init. */
typedef int (*meter_fp_action_db_init_f)(int unit);

/*! FP meter action driver structure. */
typedef struct bcmcth_meter_action_drv_s {
    /*! Meter action db init. */
    meter_fp_action_db_init_f               init;
    /*! Number of action logical tables. */
    uint32_t                                num_action_lts;
    /*! Number of pipes. */
    uint8_t                                 num_pipes;
    /*! PDD information corresponding to FP meter action. */
    bcmcth_meter_action_pdd_lt_info_t      *pdd_lt_info;
    /*! PDD information corresponding to FP meter action. */
    bcmcth_meter_action_pdd_pt_info_t      *pdd_pt_info;
    /*! SBR information corresponding to FP meter action. */
    bcmcth_meter_action_sbr_lt_info_t      *sbr_lt_info;
    /*! SBR information corresponding to FP meter action. */
    bcmcth_meter_action_sbr_pt_info_t      *sbr_pt_info;
    /*! Action information corresponding to FP meters. */
    bcmcth_meter_action_lt_info_t          *action_lt_info;
    /*! Action information corresponding to FP meters. */
    bcmcth_meter_action_pt_info_t          *action_pt_info;
} bcmcth_meter_action_drv_t;

/*! FP meter action PDD software state. */
typedef struct bcmcth_meter_action_pdd_sw_state_s {
    /*! Action offsets corresponding to each action. */
    bcmcth_meter_action_pdd_offset_t *offset_info;
    /*! PDD state. */
    uint8_t *state;
} bcmcth_meter_action_pdd_sw_state_t;

/*!
 * \brief FP meter action insert
 *
 * Insert an FP Meter action in the harwdare meter table.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  sid           Logical table ID.
 * \param [in]  stage         FP stage.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  key           Input key fields.
 * \param [in]  data          Input data fields.
 * \param [out] out           Output data fields.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_meter_action_entry_insert(int unit,
                                 bcmltd_sid_t sid,
                                 int stage,
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
 * \param [in]  stage         FP stage.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  key           Input key fields.
 * \param [in]  data          Input data fields.
 * \param [out] out           Output data fields.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_meter_action_entry_update(int unit,
                                 bcmltd_sid_t sid,
                                 int stage,
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
 * \param [in]  stage         FP stage.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  key           Input key fields.
 * \param [in]  data          Input data fields.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_meter_action_entry_delete(int unit,
                                 bcmltd_sid_t sid,
                                 int stage,
                                 uint32_t trans_id,
                                 const bcmltd_field_t *key,
                                 const bcmltd_field_t *data);

/*!
 * \brief FP action entries update
 *
 * Update all action entries using this PDD profile
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  stage         FP stage.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  pipe          PIPE number.
 * \param [in]  pdd_id        PDD profile ID.
 * \param [in]  pdd_state     State of the PDD entry.
 * \param [in]  offset_info   Offset information of this PDD profile.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_meter_fp_update_action_entries(int unit,
                                      int stage,
                                      uint32_t trans_id,
                                      int pipe,
                                      uint32_t pdd_id,
                                      uint8_t *pdd_state,
                                      bcmcth_meter_action_pdd_offset_t *offset_info);

#endif /* BCMCTH_METER_ACTION_INTERNAL_H */
