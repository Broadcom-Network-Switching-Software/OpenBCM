/*! \file rm_alpm.h
 *
 * Main routines for RM ALPM
 *
 * This file contains main routines which are internal to
 * ALPM resource manager
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef RM_ALPM_H
#define RM_ALPM_H

/*******************************************************************************
  Includes
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_dq.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmptm/bcmptm.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm_rm_alpm_internal.h>

#include "rm_alpm_common.h"
#include "rm_alpm_trie.h"
#include "rm_alpm_trie_util.h"

/*******************************************************************************
  Typedefs
 */

/*!
 * \brief ALPM controls for interacting with comdproc
 */
typedef struct alpm_cmd_ctrl_s {
    bool            write_hw;               /*!< Write to HW */
    bool            write_cache;            /*!< Write to cache */
    bool            set_cache_vbit;         /*!< Set Cache vbit */
    bcmltd_sid_t    req_ltid;               /*!< Request ltid */
    uint16_t        req_format_id;          /*!< Request format id */
    uint64_t        req_flags;              /*!< Request flags */
    bool            rsp_entry_cache_vbit;   /*!< Responsed vbit */
    bcmltd_sid_t    rsp_ltid;               /*!< Responsed ltid */
    uint16_t        rsp_dfield_format_id;   /*!< Responsed format id */
    uint32_t        rsp_flags;              /*!< Responsed flags */
    int             inst;                   /*!< Table instance */
    uint32_t        cseq_id;                /*!< Cseq Id */
} alpm_cmd_ctrl_t;

/*!
 * \brief ALPM info, the top structure contains everything describing ALPM
 */
typedef struct alpm_info_s {
    /*!< Comproc controls */
    alpm_cmd_ctrl_t cmd_ctrl;

    /*!< Hardware entry info for this LT. */
    bcmptm_rm_alpm_hw_entry_info_t *hw_info;

    /*! \brief More info as needed by ALPM rsrc_mgr from LRD */
    const bcmptm_rm_alpm_more_info_t *rm_more_info;

    /*!< ALPM Configurations */
    alpm_config_t config;
} alpm_info_t;

/*******************************************************************************
  Function prototypes
 */
/*!
 * \brief Get more info for alpm resource manager.
 *
 * \param [in] u Device u.
 *
 * \return Pointer of rm_alpm more info.
 */
extern bcmptm_rm_alpm_more_info_t *
bcmptm_rm_alpm_more_info(int u, int m);

/*!
 * \brief ALPM mode
 *
 * \param [in] u Device u.
 *
 * \return True if parallel mode, otherwise false.
 */
extern int
bcmptm_rm_alpm_is_parallel(int u, int m);

/*!
 * \brief Increase trans id, for ut only.
 *
 * \param [in] u Device u.
 *
 * \return nothing.
 */
extern void
bcmptm_rm_alpm_trans_id_incr(int u, int m);

/*!
 * \brief ALPM hit support.
 *
 * \param [in] u Device u.
 *
 * \return Hit support type.
 */
uint8_t
bcmptm_rm_alpm_hit_support(int u, int m);

/*!
 * \brief Is packing mode 128 set
 *
 * \param [in] u Device u.
 *
 * \return True or false
 */
extern bool
bcmptm_rm_alpm_pkm128_is_set(int u, int m);

/*!
 * \brief UFT banks
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [out] num_banks Number of banks
 * \param [out] start_bank Start bank
 * \param [out] bucket_bits Bucket bits
 *
 * \return SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_uft_banks(int u, int m, int db, int ln,
                         uint8_t *num_banks, uint8_t *start_bank,
                         uint8_t *bucket_bits, uint32_t *bank_bitmap);

/*!
 * \brief Get ALPM info
 *
 * \param [in] u Device u.
 *
 * \return ALPM info ptr
 */
extern alpm_info_t *
bcmptm_rm_alpm_info_get(int u, int m);


/*!
 * \brief Insert entry into ALPM database
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [in] levels ALPM levels
 *
 * \return SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_insert(int u, int m, bcmltd_sid_t ltid, alpm_arg_t *arg, int levels);

/*!
 * \brief Exact match entry in ALPM database
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [in] levels ALPM levels
 *
 * \return SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_match(int u, int m, alpm_arg_t *arg, int levels);

/*!
 * \brief Delete entry from ALPM database
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [in] levels ALPM levels
 *
 * \return SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_delete(int u, int m, bcmltd_sid_t ltid, alpm_arg_t *arg, int levels);

/*!
 * \brief LPM find entry in ALPM database
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [in] levels ALPM levels
 *
 * \return SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_find(int u, int m, alpm_arg_t *arg, int levels);

extern int
bcmptm_rm_alpm_req_arg(int u, bcmltd_sid_t req_ltid,
                       bcmptm_op_type_t req_op,
                       bcmptm_rm_alpm_req_info_t *req,
                       uint8_t *l1v,
                       int *mo,
                       alpm_arg_t *arg);


extern int
bcmptm_rm_alpm_lts_count(int u, int m);

extern bcmlrd_sid_t
bcmptm_rm_alpm_lt_get(int u, int m, int i);

extern bcmptm_rm_alpm_more_info_t *
bcmptm_rm_alpm_lt_info_get(int u, int m, int i);

#endif /* RM_ALPM_H */
