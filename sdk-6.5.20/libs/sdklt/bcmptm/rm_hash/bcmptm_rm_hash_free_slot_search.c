/*! \file bcmptm_rm_hash_free_slot_search.c
 *
 * Software State of The Resource Manager for Hash Table
 *
 * This file defines software objects that record software state of resource
 * manager of hash tables, and defines corresponding functions that get and set
 * these software objects.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <sal/sal_alloc.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include "bcmptm_rm_hash.h"
#include "bcmptm_rm_hash_lt_ctrl.h"
#include "bcmptm_rm_hash_pt_register.h"
#include "bcmptm_rm_hash_state_mgmt.h"
#include "bcmptm_rm_hash_trans_mgmt.h"
#include "bcmptm_rm_hash_utils.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMHASH

#define RM_HASH_BASE_BKT_SIZE                   4

/*******************************************************************************
 * Typedefs
 */
/*
 *! \brief Free base entry info for a base bucket
 *
 * This data structure describes free base entry info for a base bucket.
 */
typedef struct rm_hash_free_ent_s {
    /*! \brief Free base entry is available or not */
    bool avail;

    /*! \brief Free base entry count */
    uint8_t fbe_cnt;

    /*! \brief Free base entry bitmap */
    uint8_t bmp;

    /*! \brief Free base entry offset */
    uint8_t offset;
} rm_hash_free_ent_t;

/*
 *! \brief Base entry to logical entry mapping
 *
 * This data structure describes base entry to logical entry mapping
 */
typedef struct rm_hash_be_to_le_map_s {
    /*! \brief Base entry bitmap is valid or not */
    bool valid;

    /*! \brief Logical entry bitmap for the given base entry bmp */
    uint8_t le_bmp;
} rm_hash_be_to_le_map_t;

/*
 *! \brief Size of the entry in the unit of base entry
 *
 * This data structure describes the size of all the possible entries.
 */
typedef struct rm_hash_ent_size_map_s {
    /*! \brief Base entry bitmap is valid or not */
    bool valid;

    /*! \brief Size of the entry */
    uint8_t size;
} rm_hash_ent_size_map_t;

/*
 *! \brief Free narrow mode entry info within a base entry.
 *
 * This data structure describes free narrow entry info in a base entry.
 */
typedef struct rm_hash_nm_free_ent_s {
    /*! \brief Free narrow mode entry slot is available or not. */
    bool avail;

    /*! \brief Free narrow mode entry slot count. */
    uint8_t fne_cnt;

    /*! \brief Free narrow mode entry slot bitmap. */
    uint8_t bmp;

    /*! \brief Free narrow mode entry slot offset. */
    uint8_t offset;
} rm_hash_nm_free_ent_t;

/*
 *! \brief The result of free slot search for the requested entry.
 *
 * This data structure describes the search result for the requested entry.
 */
typedef struct rm_hash_slb_free_slot_info_s {
    /*! \brief Flag indicating if free space has been found. */
    bool e_loc_found;

    /*! \brief Location of the free space. */
    rm_hash_ent_loc_t e_loc;

    /*! \brief Maximum free base entry number. */
    uint8_t max_fbe_num;

    /*! \brief The offset of free narrow entry slot within the base entry. */
    uint8_t nm_offset;
} rm_hash_slb_free_slot_info_t;

/*
 *! \brief The result of free slot search for multi group SLB info.
 *
 * This data structure describes the search result for multi group SLB info.
 */
typedef struct rm_hash_mgrp_slb_info_s {
    /* \brief Slb free slot information. */
    rm_hash_slb_free_slot_info_t slb_fsi[RM_HASH_MAX_RSRC_GRP_COUNT][RM_HASH_MAX_GRP_INST_COUNT];

    /* \brief Slb index information. */
    uint8_t slb_idx[RM_HASH_MAX_RSRC_GRP_COUNT][RM_HASH_MAX_GRP_INST_COUNT];
} rm_hash_mgrp_slb_info_t;

/*
 *! \brief Function used to search a free slot within a list of SLBs.
 *
 * This data structure describes the signature of the function used to search
 * a free slot within a list of SLBs.
 */
typedef int (*fn_ent_free_slot_srch_t)(int unit,
                                       rm_hash_pt_info_t *pt_info,
                                       uint8_t *rbank_list,
                                       int tbl_inst,
                                       rm_hash_ent_slb_info_t *slb_info_list,
                                       uint8_t num_slb_info,
                                       rm_hash_req_ent_attr_t *e_attr,
                                       rm_hash_free_slot_info_t *result);

/*
 *! \brief Function used to search a free slot in an SLB.
 *
 * This data structure describes the signature of the function used to search
 * a free slot in an SLB.
 */
typedef int (*fn_ent_slb_srch_t)(int unit,
                                 rm_hash_pt_info_t *pt_info,
                                 uint8_t rbank,
                                 int tbl_inst,
                                 rm_hash_ent_slb_info_t slb_info,
                                 rm_hash_req_ent_attr_t *e_attr,
                                 rm_hash_slb_free_slot_info_t *result);

/*******************************************************************************
 * Private variables
 */
static const rm_hash_nm_free_ent_t half_ent_free_info[64] = {
    {TRUE,  0x02, 0x07, 0x00}, /* 000000 */
    {FALSE, 0x00, 0x00, 0x00}, /* 000001 */
    {FALSE, 0x00, 0x00, 0x00}, /* 000010 */
    {FALSE, 0x00, 0x00, 0x00}, /* 000011 */
    {FALSE, 0x00, 0x00, 0x00}, /* 000100 */
    {FALSE, 0x00, 0x00, 0x00}, /* 000101 */
    {FALSE, 0x00, 0x00, 0x00}, /* 000110 */
    {TRUE,  0x01, 0x38, 0x01}, /* 000111 */
    {FALSE, 0x00, 0x00, 0x00}, /* 001000 */
    {FALSE, 0x00, 0x00, 0x00}, /* 001001 */
    {FALSE, 0x00, 0x00, 0x00}, /* 001010 */
    {FALSE, 0x00, 0x00, 0x00}, /* 001011 */
    {FALSE, 0x00, 0x00, 0x00}, /* 001100 */
    {FALSE, 0x00, 0x00, 0x00}, /* 001101 */
    {FALSE, 0x00, 0x00, 0x00}, /* 001110 */
    {FALSE, 0x00, 0x00, 0x00}, /* 001111 */
    {FALSE, 0x00, 0x00, 0x00}, /* 010000 */
    {FALSE, 0x00, 0x00, 0x00}, /* 010001 */
    {FALSE, 0x00, 0x00, 0x00}, /* 010010 */
    {FALSE, 0x00, 0x00, 0x00}, /* 010011 */
    {FALSE, 0x00, 0x00, 0x00}, /* 010100 */
    {FALSE, 0x00, 0x00, 0x00}, /* 010101 */
    {FALSE, 0x00, 0x00, 0x00}, /* 010110 */
    {FALSE, 0x00, 0x00, 0x00}, /* 010111 */
    {FALSE, 0x00, 0x00, 0x00}, /* 011000 */
    {FALSE, 0x00, 0x00, 0x00}, /* 011001 */
    {FALSE, 0x00, 0x00, 0x00}, /* 011010 */
    {FALSE, 0x00, 0x00, 0x00}, /* 011011 */
    {FALSE, 0x00, 0x00, 0x00}, /* 011100 */
    {FALSE, 0x00, 0x00, 0x00}, /* 011101 */
    {FALSE, 0x00, 0x00, 0x00}, /* 011110 */
    {FALSE, 0x00, 0x00, 0x00}, /* 011111 */
    {FALSE, 0x00, 0x00, 0x00}, /* 100000 */
    {FALSE, 0x00, 0x00, 0x00}, /* 100001 */
    {FALSE, 0x00, 0x00, 0x00}, /* 100010 */
    {FALSE, 0x00, 0x00, 0x00}, /* 100011 */
    {FALSE, 0x00, 0x00, 0x00}, /* 100100 */
    {FALSE, 0x00, 0x00, 0x00}, /* 100101 */
    {FALSE, 0x00, 0x00, 0x00}, /* 100110 */
    {FALSE, 0x00, 0x00, 0x00}, /* 100111 */
    {FALSE, 0x00, 0x00, 0x00}, /* 101000 */
    {FALSE, 0x00, 0x00, 0x00}, /* 101001 */
    {FALSE, 0x00, 0x00, 0x00}, /* 101010 */
    {FALSE, 0x00, 0x00, 0x00}, /* 101011 */
    {FALSE, 0x00, 0x00, 0x00}, /* 101100 */
    {FALSE, 0x00, 0x00, 0x00}, /* 101101 */
    {FALSE, 0x00, 0x00, 0x00}, /* 101110 */
    {FALSE, 0x00, 0x00, 0x00}, /* 101111 */
    {FALSE, 0x00, 0x00, 0x00}, /* 110000 */
    {FALSE, 0x00, 0x00, 0x00}, /* 110001 */
    {FALSE, 0x00, 0x00, 0x00}, /* 110010 */
    {FALSE, 0x00, 0x00, 0x00}, /* 110011 */
    {FALSE, 0x00, 0x00, 0x00}, /* 110100 */
    {FALSE, 0x00, 0x00, 0x00}, /* 110101 */
    {FALSE, 0x00, 0x00, 0x00}, /* 110110 */
    {FALSE, 0x00, 0x00, 0x00}, /* 110111 */
    {TRUE,  0x01, 0x07, 0x00}, /* 111000 */
    {FALSE, 0x00, 0x00, 0x00}, /* 111001 */
    {FALSE, 0x00, 0x00, 0x00}, /* 111010 */
    {FALSE, 0x00, 0x00, 0x00}, /* 111011 */
    {FALSE, 0x00, 0x00, 0x00}, /* 111100 */
    {FALSE, 0x00, 0x00, 0x00}, /* 111101 */
    {FALSE, 0x00, 0x00, 0x00}, /* 111110 */
    {FALSE, 0x00, 0x00, 0x00}, /* 111111 */
};

static const rm_hash_nm_free_ent_t third_ent_free_info[64] = {
    {TRUE,  0x03, 0x03, 0x00}, /* 000000 */
    {FALSE, 0x00, 0x00, 0x00}, /* 000001 */
    {FALSE, 0x00, 0x00, 0x00}, /* 000010 */
    {TRUE,  0x02, 0x0C, 0x01}, /* 000011 */
    {FALSE, 0x00, 0x00, 0x00}, /* 000100 */
    {FALSE, 0x00, 0x00, 0x00}, /* 000101 */
    {FALSE, 0x00, 0x00, 0x00}, /* 000110 */
    {FALSE, 0x00, 0x00, 0x00}, /* 000111 */
    {FALSE, 0x00, 0x00, 0x00}, /* 001000 */
    {FALSE, 0x00, 0x00, 0x00}, /* 001001 */
    {FALSE, 0x00, 0x00, 0x00}, /* 001010 */
    {FALSE, 0x00, 0x00, 0x00}, /* 001011 */
    {TRUE,  0x02, 0x03, 0x00}, /* 001100 */
    {FALSE, 0x00, 0x00, 0x00}, /* 001101 */
    {FALSE, 0x00, 0x00, 0x00}, /* 001110 */
    {TRUE,  0x01, 0x30, 0x02}, /* 001111 */
    {FALSE, 0x00, 0x00, 0x00}, /* 010000 */
    {FALSE, 0x00, 0x00, 0x00}, /* 010001 */
    {FALSE, 0x00, 0x00, 0x00}, /* 010010 */
    {FALSE, 0x00, 0x00, 0x00}, /* 010011 */
    {FALSE, 0x00, 0x00, 0x00}, /* 010100 */
    {FALSE, 0x00, 0x00, 0x00}, /* 010101 */
    {FALSE, 0x00, 0x00, 0x00}, /* 010110 */
    {FALSE, 0x00, 0x00, 0x00}, /* 010111 */
    {FALSE, 0x00, 0x00, 0x00}, /* 011000 */
    {FALSE, 0x00, 0x00, 0x00}, /* 011001 */
    {FALSE, 0x00, 0x00, 0x00}, /* 011010 */
    {FALSE, 0x00, 0x00, 0x00}, /* 011011 */
    {FALSE, 0x00, 0x00, 0x00}, /* 011100 */
    {FALSE, 0x00, 0x00, 0x00}, /* 011101 */
    {FALSE, 0x00, 0x00, 0x00}, /* 011110 */
    {FALSE, 0x00, 0x00, 0x00}, /* 011111 */
    {FALSE, 0x00, 0x00, 0x00}, /* 100000 */
    {FALSE, 0x00, 0x00, 0x00}, /* 100001 */
    {FALSE, 0x00, 0x00, 0x00}, /* 100010 */
    {FALSE, 0x00, 0x00, 0x00}, /* 100011 */
    {FALSE, 0x00, 0x00, 0x00}, /* 100100 */
    {FALSE, 0x00, 0x00, 0x00}, /* 100101 */
    {FALSE, 0x00, 0x00, 0x00}, /* 100110 */
    {FALSE, 0x00, 0x00, 0x00}, /* 100111 */
    {FALSE, 0x00, 0x00, 0x00}, /* 101000 */
    {FALSE, 0x00, 0x00, 0x00}, /* 101001 */
    {FALSE, 0x00, 0x00, 0x00}, /* 101010 */
    {FALSE, 0x00, 0x00, 0x00}, /* 101011 */
    {FALSE, 0x00, 0x00, 0x00}, /* 101100 */
    {FALSE, 0x00, 0x00, 0x00}, /* 101101 */
    {FALSE, 0x00, 0x00, 0x00}, /* 101110 */
    {FALSE, 0x00, 0x00, 0x00}, /* 101111 */
    {TRUE,  0x02, 0x03, 0x00}, /* 110000 */
    {FALSE, 0x00, 0x00, 0x00}, /* 110001 */
    {FALSE, 0x00, 0x00, 0x00}, /* 110010 */
    {TRUE,  0x01, 0x0c, 0x01}, /* 110011 */
    {FALSE, 0x00, 0x00, 0x00}, /* 110100 */
    {FALSE, 0x00, 0x00, 0x00}, /* 110101 */
    {FALSE, 0x00, 0x00, 0x00}, /* 110110 */
    {FALSE, 0x00, 0x00, 0x00}, /* 110111 */
    {FALSE, 0x00, 0x00, 0x00}, /* 111000 */
    {FALSE, 0x00, 0x00, 0x00}, /* 111001 */
    {FALSE, 0x00, 0x00, 0x00}, /* 111010 */
    {FALSE, 0x00, 0x00, 0x00}, /* 111011 */
    {TRUE,  0x01, 0x03, 0x00}, /* 111100 */
    {FALSE, 0x00, 0x00, 0x00}, /* 111101 */
    {FALSE, 0x00, 0x00, 0x00}, /* 111110 */
    {FALSE, 0x00, 0x00, 0x00}, /* 111111 */
};

static const rm_hash_free_ent_t sw_ent_free_info [16] = {
    {TRUE,  0x04, 0x01, 0x00}, /* 0000 */
    {TRUE,  0x03, 0x02, 0x01}, /* 0001 */
    {TRUE,  0x03, 0x01, 0x00}, /* 0010 */
    {TRUE,  0x02, 0x04, 0x02}, /* 0011 */
    {TRUE,  0x03, 0x01, 0x00}, /* 0100 */
    {TRUE,  0x02, 0x02, 0x01}, /* 0101 */
    {TRUE,  0x02, 0x01, 0x00}, /* 0110 */
    {TRUE,  0x01, 0x08, 0x03}, /* 0111 */
    {TRUE,  0x03, 0x01, 0x00}, /* 1000 */
    {TRUE,  0x02, 0x02, 0x01}, /* 1001 */
    {TRUE,  0x02, 0x01, 0x00}, /* 1010 */
    {TRUE,  0x01, 0x04, 0x02}, /* 1011 */
    {TRUE,  0x02, 0x01, 0x00}, /* 1100 */
    {TRUE,  0x01, 0x02, 0x01}, /* 1101 */
    {TRUE,  0x01, 0x01, 0x00}, /* 1110 */
    {FALSE, 0x00, 0x00, 0x00}, /* 1111 */
};

static const rm_hash_free_ent_t dw_ent_free_info [16] = {
    {TRUE,  0x04, 0x03, 0x00}, /* 0000 */
    {TRUE,  0x03, 0x0C, 0x02}, /* 0001 */
    {TRUE,  0x03, 0x0C, 0x02}, /* 0010 */
    {TRUE,  0x02, 0x0C, 0x02}, /* 0011 */
    {TRUE,  0x03, 0x03, 0x00}, /* 0100 */
    {FALSE, 0x02, 0x00, 0x00}, /* 0101 */
    {FALSE, 0x02, 0x00, 0x00}, /* 0110 */
    {FALSE, 0x01, 0x08, 0x03}, /* 0111 */
    {TRUE,  0x03, 0x03, 0x00}, /* 1000 */
    {FALSE, 0x02, 0x00, 0x00}, /* 1001 */
    {FALSE, 0x02, 0x00, 0x00}, /* 1010 */
    {FALSE, 0x01, 0x00, 0x00}, /* 1011 */
    {TRUE,  0x02, 0x03, 0x00}, /* 1100 */
    {FALSE, 0x01, 0x00, 0x00}, /* 1101 */
    {FALSE, 0x01, 0x00, 0x00}, /* 1110 */
    {FALSE, 0x00, 0x00, 0x00}, /* 1111 */
};

static const rm_hash_free_ent_t qw_ent_free_info [16] = {
    {TRUE,  0x04, 0x0F, 0x00}, /* 0000 */
    {FALSE, 0x03, 0x00, 0x00}, /* 0001 */
    {FALSE, 0x03, 0x00, 0x00}, /* 0010 */
    {FALSE, 0x02, 0x00, 0x00}, /* 0011 */
    {FALSE, 0x03, 0x00, 0x00}, /* 0100 */
    {FALSE, 0x02, 0x00, 0x00}, /* 0101 */
    {FALSE, 0x02, 0x00, 0x00}, /* 0110 */
    {FALSE, 0x01, 0x00, 0x00}, /* 0111 */
    {FALSE, 0x03, 0x00, 0x00}, /* 1000 */
    {FALSE, 0x02, 0x00, 0x00}, /* 1001 */
    {FALSE, 0x02, 0x00, 0x00}, /* 1010 */
    {FALSE, 0x01, 0x00, 0x00}, /* 1011 */
    {FALSE, 0x02, 0x00, 0x00}, /* 1100 */
    {FALSE, 0x01, 0x00, 0x00}, /* 1101 */
    {FALSE, 0x01, 0x00, 0x00}, /* 1110 */
    {FALSE, 0x00, 0x00, 0x00}, /* 1111 */
};

static const rm_hash_be_to_le_map_t bm1e_be_to_le_map [16] = {
    {FALSE, 0x00}, /* 0000 */
    {TRUE,  0x03}, /* 0001 */
    {TRUE,  0x03}, /* 0010 */
    {TRUE,  0x03}, /* 0011 */
    {TRUE,  0x0C}, /* 0100 */
    {FALSE, 0x00}, /* 0101 */
    {FALSE, 0x00}, /* 0110 */
    {FALSE, 0x00}, /* 0111 */
    {TRUE,  0x0C}, /* 1000 */
    {FALSE, 0x00}, /* 1001 */
    {FALSE, 0x00}, /* 1010 */
    {FALSE, 0x00}, /* 1011 */
    {TRUE,  0x0C}, /* 1100 */
    {FALSE, 0x00}, /* 1101 */
    {FALSE, 0x00}, /* 1110 */
    {TRUE,  0x0F}, /* 1111 */
};

static rm_hash_mgrp_slb_info_t mgrp_slb_info[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private Functions
 */


/*!
 * \brief Get the bitmap of used base entries in a base bucket.
 *
 * For a given software logical bucket and the specified base bucket in it,
 * this function will get the used base entry bitmap in the specified
 * base bucket. This function is applicable to shared hash scheme.
 *
 * \param [in] unit Unit number.
 * \param [in] slb_be_bmp Base entry bitmap in the software logical bucket.
 * \param [in] bb_bmp Base bucket bitmap in the software logical bucket.
 * \param [out] be_bmp Pointer to a buffer to contain the calculated bitmap.
 */
static int
rm_hash_bb_view_be_bmp_get(int unit,
                           uint16_t slb_be_bmp,
                           uint8_t bb_bmp,
                           uint8_t *be_bmp)
{
    uint16_t tmp = 0;

    SHR_FUNC_ENTER(unit);

    switch (bb_bmp) {
    case 0x0001:
        tmp = slb_be_bmp & 0x000F;
        *be_bmp = tmp;
        SHR_EXIT();
    case 0x0002:
        tmp = slb_be_bmp & 0x00F0;
        *be_bmp = tmp >> 4;
        SHR_EXIT();
    case 0x0004:
        tmp = slb_be_bmp & 0x0F00;
        *be_bmp = tmp >> 8;
        SHR_EXIT();
    case 0x0008:
        tmp = slb_be_bmp & 0xF000;
        *be_bmp = tmp >> 12;
        SHR_EXIT();
    default:
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the bitmap of logical entry in a base bucket.
 *
 * For a given software logical bucket and the specified base bucket in it,
 * this function will get the logical entry bitmap in the specified
 * base bucket. This function is applicable to shared hash scheme.
 *
 * \param [in] unit Unit number.
 * \param [in] slb_le_bmp Logical entry bitmap in the software logical bucket.
 * \param [in] bb_bmp Base bucket bitmap.
 * \param [out] le_bmp Pointer to a buffer to contain the calculated bitmap.
 */
static int
rm_hash_bb_view_le_bmp_get(int unit,
                           uint16_t slb_le_bmp,
                           uint8_t bb_bmp,
                           uint8_t *le_bmp)
{
    uint16_t tmp = 0;

    SHR_FUNC_ENTER(unit);

    switch (bb_bmp) {
    case 0x0001:
        tmp = slb_le_bmp & 0x000F;
        *le_bmp = tmp;
        SHR_EXIT();
    case 0x0002:
        tmp = slb_le_bmp & 0x00F0;
        *le_bmp = tmp >> 4;
        SHR_EXIT();
    case 0x0004:
        tmp = slb_le_bmp & 0x0F00;
        *le_bmp = tmp >> 8;
        SHR_EXIT();
    case 0x0008:
        tmp = slb_le_bmp & 0xF000;
        *le_bmp = tmp >> 12;
        SHR_EXIT();
    default:
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the bitmap of logical entry and actual used based entry
 *
 * For a given software logical bucket and the specified base bucket in it,
 * this function will get the used base entry and logical entry bitmap
 * in the specified base bucket. This function is applicable to shared hash
 * scheme.
 *
 * \param [in] unit Unit number.
 * \param [in] slb_e_bmp Used base entry and logical entry bitmap.
 * \param [in] bb_bmp Base bucket bitmap.
 * \param [in] bm Bucket mode.
 * \param [out] ue_bmp Pointer to a buffer to contain the calculated bitmap.
 */
static int
rm_hash_bb_view_ue_bmp_get(int unit,
                           rm_hash_slb_ent_bmp_t *slb_e_bmp,
                           uint8_t bb_bmp,
                           rm_hash_bm_t bm,
                           uint8_t *ue_bmp)
{
    uint8_t be_bmp = 0, le_bmp = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (rm_hash_bb_view_be_bmp_get(unit,
                                    slb_e_bmp->be_bmp,
                                    bb_bmp,
                                    &be_bmp));
    SHR_IF_ERR_EXIT
        (rm_hash_bb_view_le_bmp_get(unit,
                                    slb_e_bmp->le_bmp[bm],
                                    bb_bmp,
                                    &le_bmp));
    *ue_bmp = be_bmp | le_bmp;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get free base entry info
 *
 * Get free base entry info for the given bitmap.
 *
 * \param [in] unit Unit number.
 * \param [in] nibble Used base entry bitmap in the nibble.
 * \param [in] e_size Entry size.
 * \param [out] fe_info Pointer to an rm_hash_free_ent_t structure.
 */
static int
rm_hash_nibble_free_space_info_get(int unit,
                                   uint8_t nibble,
                                   uint8_t e_size,
                                   const rm_hash_free_ent_t **fe_info)
{

    SHR_FUNC_ENTER(unit);

    switch (e_size) {
    case 0x01:
        *fe_info = &sw_ent_free_info[nibble];
        SHR_EXIT();
    case 0x02:
        *fe_info = &dw_ent_free_info[nibble];
        SHR_EXIT();
    case 0x04:
        *fe_info = &qw_ent_free_info[nibble];
        SHR_EXIT();
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get free entry information.
 *
 * Get free base entry info for the given bitmap.
 *
 * \param [in] unit Unit number.
 * \param [in] hash_type Hash type of the underlining physical table.
 * \param [in] base_bkt_size Bucket size of the physical hash bucket.
 * \param [in] be_bmp Base entry bitmap.
 * \param [in] e_size Requested entry size.
 * \param [out] free_e_info Pointer to an rm_hash_free_ent_t structure.
 */
static int
rm_hash_bmn_free_space_info_get(int unit,
                                bcmptm_rm_hash_type_t hash_type,
                                uint8_t base_bkt_size,
                                uint8_t slb_be_bmp,
                                uint8_t e_size,
                                rm_hash_free_ent_t *free_e_info)
{
    const rm_hash_free_ent_t *fe_nibble_lo = NULL;
    const rm_hash_free_ent_t *fe_nibble_hi = NULL;

    SHR_FUNC_ENTER(unit);

    if ((hash_type == BCMPTM_RM_HASH_TYPE_DUAL) &&
        (base_bkt_size == RM_HASH_WIDE_DUAL_HASH_BKT_SIZE)) {
        /* Examine if low half of the SLB has free space. */
        SHR_IF_ERR_EXIT
            (rm_hash_nibble_free_space_info_get(unit,
                                                slb_be_bmp & 0x0F,
                                                e_size,
                                                &fe_nibble_lo));
        /* Examine if high half of the SLB has free space. */
        SHR_IF_ERR_EXIT
            (rm_hash_nibble_free_space_info_get(unit,
                                                (slb_be_bmp >> 0x04) & 0x0F,
                                                e_size,
                                                &fe_nibble_hi));
        free_e_info->avail = fe_nibble_lo->avail || fe_nibble_hi->avail;
        free_e_info->fbe_cnt = fe_nibble_lo->fbe_cnt + fe_nibble_hi->fbe_cnt;

        if (fe_nibble_lo->avail == TRUE) {
            free_e_info->bmp = fe_nibble_lo->bmp;
            free_e_info->offset = fe_nibble_lo->offset;
        } else if (fe_nibble_hi->avail == TRUE) {
            free_e_info->bmp = fe_nibble_hi->bmp << 4;
            free_e_info->offset = fe_nibble_hi->offset + 4;
        } else {
            free_e_info->bmp = 0x0;
            free_e_info->offset = 0x0;
        }
    } else {
        /*
         * For other cases, the effective bitmap in the software logical bucket
         * will be 4 bit length.
         */
        SHR_IF_ERR_EXIT
            (rm_hash_nibble_free_space_info_get(unit,
                                                slb_be_bmp,
                                                e_size,
                                                &fe_nibble_lo));
        *free_e_info = *fe_nibble_lo;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find free slot for an entry of BMN in a BMN bucket list.
 *
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank_list List of banks on which the slots will be searched.
 * \param [in] tbl_inst Table instance.
 * \param [in] slb_info_list Pointer to rm_hash_ent_slb_info_t structure.
 * \param [in] num_slb_info Number of software logical buckets.
 * \param [in] e_attr Pointer to rm_hash_req_ent_attr_t structure.
 * \param [out] result Pointer to rm_hash_free_slot_info_t structure.
 */
static int
rm_hash_bmn_srch_in_bmn_slb_list(int unit,
                                 rm_hash_pt_info_t *pt_info,
                                 uint8_t *rbank_list,
                                 int tbl_inst,
                                 rm_hash_ent_slb_info_t *slb_info_list,
                                 uint8_t num_slb_info,
                                 rm_hash_req_ent_attr_t *e_attr,
                                 rm_hash_free_slot_info_t *result)
{
    uint8_t fbe_cnt = 0, max_fbe_num = 0, idx = 0;
    rm_hash_slb_state_t *b_state = NULL;
    rm_hash_free_ent_t fe, qfe;
    uint8_t qslb_idx = 0, max_bkt_size, rbank, pipe;

    SHR_FUNC_ENTER(unit);

    max_bkt_size = RM_HASH_BASE_BKT_SIZE;

    sal_memset(&fe, 0, sizeof(fe));
    sal_memset(&qfe, 0, sizeof(qfe));
    pipe = (tbl_inst == -1)? 0 : tbl_inst;
    for (idx = 0; idx < num_slb_info; idx++) {
        rbank = rbank_list[idx];
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_ha_state_alloc(unit, pt_info, rbank, tbl_inst));
        if ((pt_info->hash_type[rbank] == BCMPTM_RM_HASH_TYPE_DUAL) &&
            (pt_info->base_bkt_size[rbank] == RM_HASH_WIDE_DUAL_HASH_BKT_SIZE)) {
            max_bkt_size = RM_HASH_BASE_BKT_SIZE * 2;
        }
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_slb_state_get(unit, pt_info, rbank, pipe,
                                          slb_info_list[idx].slb,
                                          &b_state));
        SHR_IF_ERR_EXIT
            (rm_hash_bmn_free_space_info_get(unit,
                                             pt_info->hash_type[rbank],
                                             pt_info->base_bkt_size[rbank],
                                             (uint8_t)b_state->e_bmp.be_bmp,
                                             e_attr->e_size,
                                             &fe));
        if (fe.avail == FALSE) {
            continue;
        }
        fbe_cnt = fe.fbe_cnt;
        if (fbe_cnt > max_fbe_num) {
            max_fbe_num = fbe_cnt;
            qfe = fe;
            qslb_idx = idx;
            if (max_fbe_num == max_bkt_size) {
                break;
            }
        }
    }
    if (qfe.avail == TRUE) {
        result->e_loc_list[0].bb_bmp = 0x01;
        result->e_loc_list[0].be_bmp = qfe.bmp;
        result->e_loc_list[0].le_bmp = qfe.bmp;
        result->slb_info_idx_list[0] = qslb_idx;
        result->num_inst = 1;
        result->found = TRUE;
    } else {
        result->found = FALSE;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find free slot for an entry of BMN in a BM1 bucket list.
 *
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank_list List of banks on which the slots will be searched.
 * \param [in] tbl_inst Table instance.
 * \param [in] slb_info_list Pointer to rm_hash_ent_slb_info_t structure.
 * \param [in] num_slb_info Number of software logical buckets.
 * \param [in] e_attr Pointer to rm_hash_req_ent_attr_t structure.
 * \param [out] result Pointer to rm_hash_free_slot_info_t structure.
 */
static int
rm_hash_bmn_srch_in_bm1_slb_list(int unit,
                                 rm_hash_pt_info_t *pt_info,
                                 uint8_t *rbank_list,
                                 int tbl_inst,
                                 rm_hash_ent_slb_info_t *slb_info_list,
                                 uint8_t num_slb_info,
                                 rm_hash_req_ent_attr_t *e_attr,
                                 rm_hash_free_slot_info_t *result)
{
    uint8_t fbe_cnt = 0, max_fbe_num = 0, idx = 0, vbb_bmp = 0;
    uint8_t bb_idx = 0, bb_bmp = 0, qbb_bmp = 0;
    uint8_t ue_bmp = 0;
    uint8_t qslb_idx = 0, pipe;
    rm_hash_slb_state_t *b_state = NULL;
    const rm_hash_free_ent_t *fe = NULL, *qfe = NULL;

    SHR_FUNC_ENTER(unit);

    pipe = (tbl_inst == -1)? 0 : tbl_inst;
    for (idx = 0; idx < num_slb_info; idx++) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_ha_state_alloc(unit, pt_info, rbank_list[idx],
                                           tbl_inst));
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_slb_state_get(unit, pt_info, rbank_list[idx],
                                          pipe,
                                          slb_info_list[idx].slb,
                                          &b_state));
        vbb_bmp = slb_info_list[idx].vbb_bmp;
        for (bb_idx = 0; bb_idx < RM_HASH_BM1_BASE_BKT_NUM; bb_idx++) {
            bb_bmp = 0x01 << bb_idx;
            if (bb_bmp & vbb_bmp) {
                SHR_IF_ERR_EXIT
                    (rm_hash_bb_view_ue_bmp_get(unit,
                                                &b_state->e_bmp,
                                                bb_bmp,
                                                RM_HASH_BM_0,
                                                &ue_bmp));
                SHR_IF_ERR_EXIT
                    (rm_hash_nibble_free_space_info_get(unit,
                                                        ue_bmp,
                                                        e_attr->e_size,
                                                        &fe));
                break;
            }
        }
        if ((fe == NULL) || (fe->avail == FALSE)) {
            continue;
        }
        fbe_cnt = fe->fbe_cnt;
        if (fbe_cnt > max_fbe_num) {
            max_fbe_num = fbe_cnt;
            qbb_bmp = bb_bmp;
            qslb_idx = idx;
            qfe = fe;
            if (max_fbe_num == RM_HASH_BASE_BKT_SIZE) {
                break;
            }
        }
    }
    if (qfe != NULL) {
        result->e_loc_list[0].bb_bmp = qbb_bmp;
        result->e_loc_list[0].be_bmp = qfe->bmp;
        result->e_loc_list[0].le_bmp = qfe->bmp;
        result->slb_info_idx_list[0] = qslb_idx;
        result->num_inst = 1;
        result->found = TRUE;
    } else {
        result->found = FALSE;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find free slot for an entry of BM0 in a BM0 bucket list.
 *
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank_list List of banks on which the slots will be searched.
 * \param [in] tbl_inst Table instance.
 * \param [in] slb_info_list Pointer to rm_hash_ent_slb_info_t structure.
 * \param [in] num_slb_info Number of software logical buckets.
 * \param [in] e_attr Pointer to rm_hash_req_ent_attr_t structure.
 * \param [out] result Pointer to rm_hash_free_slot_info_t structure.
 */
static int
rm_hash_bm0_srch_in_bm0_slb_list(int unit,
                                 rm_hash_pt_info_t *pt_info,
                                 uint8_t *rbank_list,
                                 int tbl_inst,
                                 rm_hash_ent_slb_info_t *slb_info_list,
                                 uint8_t num_slb_info,
                                 rm_hash_req_ent_attr_t *e_attr,
                                 rm_hash_free_slot_info_t *result)
{
    uint8_t fbe_cnt = 0, max_fbe_num = 0, idx = 0;
    rm_hash_slb_state_t *b_state = NULL;
    const rm_hash_free_ent_t *fe = NULL, *qfe = NULL;
    uint8_t qslb_idx = 0, pipe;

    SHR_FUNC_ENTER(unit);

    pipe = (tbl_inst == -1)? 0 : tbl_inst;
    for (idx = 0; idx < num_slb_info; idx++) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_ha_state_alloc(unit, pt_info, rbank_list[idx],
                                           tbl_inst));
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_slb_state_get(unit, pt_info, rbank_list[idx], pipe,
                                          slb_info_list[idx].slb,
                                          &b_state));
        SHR_IF_ERR_EXIT
            (rm_hash_nibble_free_space_info_get(unit,
                                                (uint8_t)b_state->e_bmp.be_bmp,
                                                e_attr->e_size,
                                                &fe));
        if (fe->avail == FALSE) {
            continue;
        }
        fbe_cnt = fe->fbe_cnt;
        if (fbe_cnt > max_fbe_num) {
            max_fbe_num = fbe_cnt;
            qfe = fe;
            qslb_idx = idx;
            if (max_fbe_num == RM_HASH_BASE_BKT_SIZE) {
                break;
            }
        }
    }
    if (qfe != NULL) {
        result->e_loc_list[0].bb_bmp = 0x01;
        result->e_loc_list[0].be_bmp = qfe->bmp;
        result->e_loc_list[0].le_bmp = qfe->bmp;
        result->slb_info_idx_list[0] = qslb_idx;
        result->num_inst = 1;
        result->found = TRUE;
    } else {
        result->found = FALSE;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find free slot for an entry of BM0 in a BM1 bucket list.
 *
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank_list List of banks on which the slots will be searched.
 * \param [in] tbl_inst Table instance.
 * \param [in] slb_info_list Pointer to rm_hash_ent_slb_info_t structure.
 * \param [in] num_slb_info Number of software logical buckets.
 * \param [in] e_attr Pointer to rm_hash_req_ent_attr_t structure.
 * \param [out] result Pointer to rm_hash_free_slot_info_t structure.
 */
static int
rm_hash_bm0_srch_in_bm1_slb_list(int unit,
                                 rm_hash_pt_info_t *pt_info,
                                 uint8_t *rbank_list,
                                 int tbl_inst,
                                 rm_hash_ent_slb_info_t *slb_info_list,
                                 uint8_t num_slb_info,
                                 rm_hash_req_ent_attr_t *e_attr,
                                 rm_hash_free_slot_info_t *result)
{
    uint8_t fbe_cnt = 0, max_fbe_num = 0, idx = 0, vbb_bmp = 0;
    uint8_t bb_idx = 0, bb_bmp = 0, qbb_bmp = 0;
    uint8_t ue_bmp = 0;
    uint8_t qslb_idx = 0, pipe;
    rm_hash_slb_state_t *b_state = NULL;
    const rm_hash_free_ent_t *fe = NULL, *qfe = NULL;

    SHR_FUNC_ENTER(unit);

    pipe = (tbl_inst == -1)? 0 : tbl_inst;
    for (idx = 0; idx < num_slb_info; idx++) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_ha_state_alloc(unit, pt_info, rbank_list[idx],
                                           tbl_inst));
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_slb_state_get(unit, pt_info, rbank_list[idx], pipe,
                                          slb_info_list[idx].slb,
                                          &b_state));
        vbb_bmp = slb_info_list[idx].vbb_bmp;
        for (bb_idx = 0; bb_idx < RM_HASH_BM1_BASE_BKT_NUM; bb_idx++) {
            bb_bmp = 0x01 << bb_idx;
            if (bb_bmp & vbb_bmp) {
                SHR_IF_ERR_EXIT
                    (rm_hash_bb_view_ue_bmp_get(unit,
                                                &b_state->e_bmp,
                                                bb_bmp,
                                                RM_HASH_BM_0,
                                                &ue_bmp));
                SHR_IF_ERR_EXIT
                    (rm_hash_nibble_free_space_info_get(unit,
                                                        ue_bmp,
                                                        e_attr->e_size,
                                                        &fe));
                break;
            }
        }
        if ((fe == NULL) || (fe->avail == FALSE)) {
            continue;
        }
        fbe_cnt = fe->fbe_cnt;
        if (fbe_cnt > max_fbe_num) {
            max_fbe_num = fbe_cnt;
            qbb_bmp = bb_bmp;
            qslb_idx = idx;
            qfe = fe;
            if (max_fbe_num == RM_HASH_BASE_BKT_SIZE) {
                break;
            }
        }
    }
    if (qfe != NULL) {
        result->e_loc_list[0].bb_bmp = qbb_bmp;
        result->e_loc_list[0].be_bmp = qfe->bmp;
        result->e_loc_list[0].le_bmp = qfe->bmp;
        result->slb_info_idx_list[0] = qslb_idx;
        result->num_inst = 1;
        result->found = TRUE;
    } else {
        result->found = FALSE;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find free slot for an entry of BM1 in a BM1 bucket list.
 *
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank_list List of banks on which the slots will be searched.
 * \param [in] tbl_inst Table instance.
 * \param [in] slb_info_list Pointer to rm_hash_ent_slb_info_t structure.
 * \param [in] num_slb_info Number of software logical buckets.
 * \param [in] e_attr Pointer to rm_hash_req_ent_attr_t structure.
 * \param [out] result Pointer to rm_hash_free_slot_info_t structure.
 */
static int
rm_hash_bm1_srch_in_bm1_slb_list(int unit,
                                 rm_hash_pt_info_t *pt_info,
                                 uint8_t *rbank_list,
                                 int tbl_inst,
                                 rm_hash_ent_slb_info_t *slb_info_list,
                                 uint8_t num_slb_info,
                                 rm_hash_req_ent_attr_t *e_attr,
                                 rm_hash_free_slot_info_t *result)
{
    uint8_t fbe_cnt = 0, max_fbe_num = 0;
    uint8_t hlb_fbe_cnt = 0, hlb_bb_bmp = 0, idx = 0;
    uint8_t bb_idx = 0, bb_bmp = 0, qual_bb_bmp = 0;
    uint8_t ue_bmp = 0;
    uint8_t qslb_idx = 0, pipe;
    rm_hash_slb_state_t *b_state = NULL;
    const rm_hash_free_ent_t *fe = NULL, *qfe = NULL, *hlb_qfe = NULL;

    SHR_FUNC_ENTER(unit);

    pipe = (tbl_inst == -1)? 0 : tbl_inst;
    for (idx = 0; idx < num_slb_info; idx++) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_ha_state_alloc(unit, pt_info, rbank_list[idx],
                                           tbl_inst));
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_slb_state_get(unit, pt_info, rbank_list[idx],
                                          pipe, slb_info_list[idx].slb,
                                          &b_state));
        hlb_fbe_cnt = 0;
        hlb_bb_bmp = 0;
        hlb_qfe = NULL;

        for (bb_idx = 0; bb_idx < RM_HASH_BM1_BASE_BKT_NUM; bb_idx++) {
            bb_bmp = 0x01 << bb_idx;
            SHR_IF_ERR_EXIT
                (rm_hash_bb_view_ue_bmp_get(unit,
                                            &b_state->e_bmp,
                                            bb_bmp,
                                            RM_HASH_BM_1,
                                            &ue_bmp));
            SHR_IF_ERR_EXIT
                (rm_hash_nibble_free_space_info_get(unit,
                                                    ue_bmp,
                                                    e_attr->e_size,
                                                    &fe));
            if ((fe->avail == TRUE) && (hlb_qfe == NULL)) {
                /*
                 * A free slot has been found in one of the base buckets
                 * of the hardware logical bucket. The corresponding
                 * base bucket bitmap and free entry info in this base
                 * bucket will be recorded.
                 */
                hlb_bb_bmp = bb_bmp;
                hlb_qfe = fe;
            }
            hlb_fbe_cnt += fe->fbe_cnt;
        }

        if (hlb_qfe == NULL) {
            /*
             * There is no free space in the hardware logical bucket
             * in this bank.
             */
            continue;
        }

        fbe_cnt = hlb_fbe_cnt;
        if (fbe_cnt > max_fbe_num) {
            max_fbe_num = fbe_cnt;
            qual_bb_bmp = hlb_bb_bmp;
            qfe = hlb_qfe;
            qslb_idx = idx;
            if (max_fbe_num == RM_HASH_BASE_BKT_SIZE * 2) {
                break;
            }
        }
    }
    if (qfe != NULL) {
        result->e_loc_list[0].bb_bmp = qual_bb_bmp;
        result->e_loc_list[0].be_bmp = qfe->bmp;
        result->e_loc_list[0].le_bmp = bm1e_be_to_le_map[qfe->bmp].le_bmp;
        result->slb_info_idx_list[0] = qslb_idx;
        result->num_inst = 1;
        result->found = TRUE;
    } else {
        result->found = FALSE;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the entry bitmap for an SLB.
 *
 * Try to get the entry bitmap for an SLB.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank Bank number.
 * \param [in] tbl_inst Table instance.
 * \param [in] slb Software logical bucket.
 * \param [out] slb_ent_bmp Entry bitmap of the SLB.
 */
static int
rm_hash_slb_ent_bmp_get(int unit,
                        rm_hash_pt_info_t *pt_info,
                        uint8_t rbank,
                        int tbl_inst,
                        uint32_t slb,
                        rm_hash_slb_ent_bmp_t *slb_ent_bmp)
{
    uint8_t pipe, idx;
    rm_hash_slb_state_t *b_state = NULL;
    int pipe_num;
    bcmdrd_sid_t sid;

    SHR_FUNC_ENTER(unit);

    if (pt_info->single_view_sid[rbank] == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    sid = *pt_info->single_view_sid[rbank];
    pipe_num = bcmdrd_pt_num_tbl_inst(unit, sid);
    if (tbl_inst == -1) {
        if (pt_info->bank_mode[rbank] == RM_HASH_BANK_MODE_MIXED) {
            /*
             * Physical table works in mixed mode, i.e. both global LT and
             * per pipe LT are mapped onto this bank.
             */
            for (pipe = 0; pipe < pipe_num; pipe++) {
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_slb_state_get(unit, pt_info, rbank, pipe, slb,
                                                  &b_state));
                slb_ent_bmp->be_bmp |= b_state->e_bmp.be_bmp;
                for (idx = 0; idx < RM_HASH_BM_NUM; idx++) {
                    slb_ent_bmp->le_bmp[idx] |= b_state->e_bmp.le_bmp[idx];
                }
            }
        } else if (pt_info->bank_mode[rbank] == RM_HASH_BANK_MODE_GLOBAL) {
            /* Physical table is in global mode. */
            pipe = 0;
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_slb_state_get(unit, pt_info, rbank, pipe, slb,
                                              &b_state));
            *slb_ent_bmp = b_state->e_bmp;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    } else if (tbl_inst >= 0) {
        pipe = tbl_inst;
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_slb_state_get(unit, pt_info, rbank, pipe, slb,
                                          &b_state));
        *slb_ent_bmp = b_state->e_bmp;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the narrow mode entry bitmap in a base entry.
 *
 * Try to get the narrow mode entry bitmap in a base entry.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank Bank number.
 * \param [in] tbl_inst Table instance.
 * \param [in] base_ent_offset Base entry offset
 * \param [out] nme_bmp Narrow mode entry bitmap of the base entry.
 */
static int
rm_hash_narrow_mode_ent_bmp_get(int unit,
                                rm_hash_pt_info_t *pt_info,
                                uint8_t rbank,
                                int tbl_inst,
                                uint32_t base_ent_offset,
                                rm_hash_nme_bmp_t *nme_bmp)
{
    uint8_t pipe;
    rm_hash_nme_bmp_t *tmp_ent_bmp = NULL;
    int pipe_num;
    bcmdrd_sid_t sid;

    SHR_FUNC_ENTER(unit);

    if (pt_info->single_view_sid[rbank] == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    sid = *pt_info->single_view_sid[rbank];
    pipe_num = bcmdrd_pt_num_tbl_inst(unit, sid);
    if (tbl_inst == -1) {
        if (pt_info->bank_mode[rbank] == RM_HASH_BANK_MODE_MIXED) {
            /*
             * Physical table works in mixed mode, i.e. both global LT and
             * per pipe LT are mapped onto this bank.
             */
            for (pipe = 0; pipe < pipe_num; pipe++) {
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_nme_bmp_get(unit, pt_info, rbank, pipe,
                                                base_ent_offset,
                                                &tmp_ent_bmp));
                nme_bmp->bmp |= tmp_ent_bmp->bmp;
            }
        } else if (pt_info->bank_mode[rbank] == RM_HASH_BANK_MODE_GLOBAL) {
            /* Physical table is in global mode. */
            pipe = 0;
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_nme_bmp_get(unit, pt_info, rbank, pipe,
                                            base_ent_offset,
                                            &tmp_ent_bmp));
            *nme_bmp = *tmp_ent_bmp;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    } else if (tbl_inst >= 0) {
        pipe = tbl_inst;
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_nme_bmp_get(unit, pt_info, rbank, pipe,
                                        base_ent_offset,
                                        &tmp_ent_bmp));
        *nme_bmp = *tmp_ent_bmp;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Search entry mode free slot in a base entry.
 *
 * Try to search entry mode free slot in a base entry.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank Bank number.
 * \param [in] tbl_inst Table instance.
 * \param [in] slb Software logical bucket.
 * \param [in] e_loc Pointer to rm_hash_ent_loc_t structure.
 * \param [in] e_nm Entry narrow mode.
 * \param [out] free_slot_avail Pointer to the result.
 * \param [out] ne_offset Pointer to the result of narrow entry offset.
 */
static int
rm_hash_nme_free_slot_search(int unit,
                             rm_hash_pt_info_t *pt_info,
                             uint8_t rbank,
                             int tbl_inst,
                             uint32_t slb,
                             rm_hash_ent_loc_t *e_loc,
                             rm_hash_entry_narrow_mode_t e_nm,
                             bool *free_slot_avail,
                             uint8_t *ne_offset)
{
    rm_hash_nme_bmp_t nme_bmp;
    uint32_t e_index = 0;
    const rm_hash_nm_free_ent_t *nm_fe = NULL;

    SHR_FUNC_ENTER(unit);

    /*
     * The requested entry is a narrow mode entry, further to examine
     * each base entry.
     */
    if ((e_nm != BCMPTM_RM_HASH_ENTRY_NARROW_MODE_HALF) &&
        (e_nm != BCMPTM_RM_HASH_ENTRY_NARROW_MODE_THIRD)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_ent_index_get(unit, pt_info, slb, e_loc, 1, &rbank,
                                      &e_index));
    /*
     * A base entry can contain two half entries or three third entries.
     * Using another bitmap to represent the usage of the base entry.
     */
    sal_memset(&nme_bmp, 0, sizeof(nme_bmp));
    SHR_IF_ERR_EXIT
        (rm_hash_narrow_mode_ent_bmp_get(unit, pt_info, rbank,
                                         tbl_inst, e_index,
                                         &nme_bmp));
    if (nme_bmp.bmp > 0x3F) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    if (e_nm == BCMPTM_RM_HASH_ENTRY_NARROW_MODE_HALF) {
        nm_fe = &half_ent_free_info[nme_bmp.bmp];
    } else {
        nm_fe = &third_ent_free_info[nme_bmp.bmp];
    }
    if (nm_fe->avail == TRUE) {
        /* There is free narrow entry space in this base entry. */
        *free_slot_avail = TRUE;
        *ne_offset = nm_fe->offset;
        SHR_EXIT();
    } else {
        *free_slot_avail = FALSE;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find free slot for an narrow entry in a full base bucket.
 *
 * Try to find free slot for an narrow entry in a full base bucket.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank Bank number.
 * \param [in] tbl_inst Table instance.
 * \param [in] slb_info rm_hash_ent_slb_info_t structure.
 * \param [in] e_attr Pointer to rm_hash_req_ent_attr_t structure.
 * \param [in] bb_bmp Base bucket bitmap.
 * \param [out] result_in_slb Pointer to rm_hash_slb_free_slot_info_t structure.
 */
static int
rm_hash_nme_free_search_in_fbb(int unit,
                               rm_hash_pt_info_t *pt_info,
                               uint8_t rbank,
                               int tbl_inst,
                               rm_hash_ent_slb_info_t slb_info,
                               rm_hash_req_ent_attr_t *e_attr,
                               uint8_t bb_bmp,
                               rm_hash_slb_free_slot_info_t *result_in_slb)
{
    uint8_t be_idx, ne_offset, base_bkt_size, be_offset = 0;
    rm_hash_ent_loc_t tmp_e_loc;
    bool free_slot_avail = FALSE;

    SHR_FUNC_ENTER(unit);

    /*
     * For narrow mode entry, still need to examine the free slot
     * in the base entry.
     */
    base_bkt_size = pt_info->base_bkt_size[rbank];

    for (be_idx = 0; be_idx < base_bkt_size; be_idx++) {
        sal_memset(&tmp_e_loc, 0, sizeof(tmp_e_loc));
        tmp_e_loc.bb_bmp = bb_bmp;
        tmp_e_loc.be_bmp = 0x01 << be_idx;
        SHR_IF_ERR_EXIT
            (rm_hash_nme_free_slot_search(unit, pt_info, rbank,
                                          tbl_inst, slb_info.slb,
                                          &tmp_e_loc, e_attr->e_nm,
                                          &free_slot_avail,
                                          &ne_offset));
        if (free_slot_avail == TRUE) {
            be_offset = be_idx;
            /*
             * There is some free space in this base bucket. But there is
             * no complete free base entry in this bucket.
             */
            result_in_slb->max_fbe_num = 0;
            result_in_slb->e_loc.bb_bmp = bb_bmp;
            result_in_slb->e_loc.be_bmp = 0x01 << be_offset;
            result_in_slb->e_loc.le_bmp = 0x01 << be_offset;
            result_in_slb->nm_offset = ne_offset;
            result_in_slb->e_loc_found = TRUE;
            SHR_EXIT();
        }
    }
    result_in_slb->e_loc_found = FALSE;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find free slot for an narrow entry in a non-full base bucket.
 *
 * Try to find free slot for an narrow entry in a non-full base bucket.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank Bank number.
 * \param [in] tbl_inst Table instance.
 * \param [in] slb_info rm_hash_ent_slb_info_t structure.
 * \param [in] e_attr Pointer to rm_hash_req_ent_attr_t structure.
 * \param [in] bb_bmp Base bucket bitmap.
 * \param [in] ue_bmp Used base entry bitmap.
 * \param [in] fe Pointer to rm_hash_free_ent_t structure.
 * \param [out] result_in_slb Pointer to rm_hash_slb_free_slot_info_t structure.
 */
static int
rm_hash_nme_free_search_in_non_fbb(int unit,
                                   rm_hash_pt_info_t *pt_info,
                                   uint8_t rbank,
                                   int tbl_inst,
                                   rm_hash_ent_slb_info_t slb_info,
                                   rm_hash_req_ent_attr_t *e_attr,
                                   uint8_t bb_bmp,
                                   uint8_t ue_bmp,
                                   const rm_hash_free_ent_t *fe,
                                   rm_hash_slb_free_slot_info_t *result_in_slb)
{
    uint8_t be_idx, ne_offset, be_bmp, base_bkt_size;
    rm_hash_ent_loc_t tmp_e_loc;
    bool free_slot_avail = FALSE;

    SHR_FUNC_ENTER(unit);

    base_bkt_size = pt_info->base_bkt_size[rbank];
    /*
     * For narrow mode entry, try to search the free slot
     * in the occupied base entry first.
     */
    for (be_idx = 0; be_idx < base_bkt_size; be_idx++) {
        be_bmp = 0x01 << be_idx;
        if (be_bmp & ue_bmp) {
            sal_memset(&tmp_e_loc, 0, sizeof(tmp_e_loc));
            tmp_e_loc.bb_bmp = bb_bmp;
            tmp_e_loc.be_bmp = be_bmp;
            SHR_IF_ERR_EXIT
                (rm_hash_nme_free_slot_search(unit, pt_info, rbank,
                                              tbl_inst, slb_info.slb,
                                              &tmp_e_loc, e_attr->e_nm,
                                              &free_slot_avail,
                                              &ne_offset));
            if (free_slot_avail == TRUE) {
                /*
                 * There is some free space in this base bucket.
                 * But there is no complete free base entry in this bucket.
                 */
                result_in_slb->max_fbe_num = fe->fbe_cnt;
                result_in_slb->e_loc.bb_bmp = bb_bmp;
                result_in_slb->e_loc.be_bmp = 0x01 << be_idx;
                result_in_slb->e_loc.le_bmp = 0x01 << be_idx;
                result_in_slb->nm_offset = ne_offset;
                result_in_slb->e_loc_found = TRUE;
                SHR_EXIT();
            }
        }
    }
    result_in_slb->e_loc_found = FALSE;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find free slot for an entry of BM0 in a BM2 bucket.
 *
 * Try to find free slot for an entry BM0 in a BM2 bucket.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank Bank number.
 * \param [in] tbl_inst Table instance.
 * \param [in] slb_info rm_hash_ent_slb_info_t structure.
 * \param [in] e_attr Pointer to rm_hash_req_ent_attr_t structure.
 * \param [out] result_in_slb Pointer to rm_hash_slb_free_slot_info_t structure.
 */
static int
rm_hash_bm0_srch_in_bm2_single_slb(int unit,
                                   rm_hash_pt_info_t *pt_info,
                                   uint8_t rbank,
                                   int tbl_inst,
                                   rm_hash_ent_slb_info_t slb_info,
                                   rm_hash_req_ent_attr_t *e_attr,
                                   rm_hash_slb_free_slot_info_t *result_in_slb)
{
    uint8_t bb_idx = 0, vbb_bmp = 0, bb_bmp = 0;
    uint8_t ue_bmp = 0;
    const rm_hash_free_ent_t *fe = NULL;
    rm_hash_slb_ent_bmp_t slb_ent_bmp;

    SHR_FUNC_ENTER(unit);

    sal_memset(&slb_ent_bmp, 0, sizeof(slb_ent_bmp));
    SHR_IF_ERR_EXIT
        (rm_hash_slb_ent_bmp_get(unit, pt_info, rbank, tbl_inst, slb_info.slb,
                                 &slb_ent_bmp));
    vbb_bmp = slb_info.vbb_bmp;

    for (bb_idx = 0; bb_idx < RM_HASH_BM2_BASE_BKT_NUM; bb_idx++) {
        bb_bmp = 0x01 << bb_idx;
        if (bb_bmp & vbb_bmp) {
            /* The only one valid base bucket has been identified */
            SHR_IF_ERR_EXIT
                (rm_hash_bb_view_ue_bmp_get(unit,
                                            &slb_ent_bmp,
                                            bb_bmp,
                                            RM_HASH_BM_0,
                                            &ue_bmp));
            SHR_IF_ERR_EXIT
                (rm_hash_nibble_free_space_info_get(unit,
                                                    ue_bmp,
                                                    e_attr->e_size,
                                                    &fe));
            /*
             * No matter if free space has been found,
             * terminate the iteration over base bucket.
             */
            break;
        }
    }
    if (bb_idx == RM_HASH_BM2_BASE_BKT_NUM) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    if (fe->avail == FALSE) {
        if (e_attr->e_nm == BCMPTM_RM_HASH_ENTRY_NARROW_MODE_NONE) {
            /* For non-narrow mode entry. */
            result_in_slb->e_loc_found = FALSE;
        } else if ((e_attr->e_nm == BCMPTM_RM_HASH_ENTRY_NARROW_MODE_HALF) ||
                   (e_attr->e_nm == BCMPTM_RM_HASH_ENTRY_NARROW_MODE_THIRD)) {
            SHR_IF_ERR_EXIT
                (rm_hash_nme_free_search_in_fbb(unit,
                                                pt_info,
                                                rbank,
                                                tbl_inst,
                                                slb_info,
                                                e_attr,
                                                bb_bmp,
                                                result_in_slb));
        } else {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        if (e_attr->e_nm == BCMPTM_RM_HASH_ENTRY_NARROW_MODE_NONE) {
            result_in_slb->max_fbe_num = fe->fbe_cnt;
            result_in_slb->e_loc.bb_bmp = bb_bmp;
            result_in_slb->e_loc.be_bmp = fe->bmp;
            result_in_slb->e_loc.le_bmp = fe->bmp;
            result_in_slb->e_loc_found = TRUE;
        } else if ((e_attr->e_nm == BCMPTM_RM_HASH_ENTRY_NARROW_MODE_HALF) ||
                   (e_attr->e_nm == BCMPTM_RM_HASH_ENTRY_NARROW_MODE_THIRD)) {
            SHR_IF_ERR_EXIT
                (rm_hash_nme_free_search_in_non_fbb(unit,
                                                    pt_info,
                                                    rbank,
                                                    tbl_inst,
                                                    slb_info,
                                                    e_attr,
                                                    bb_bmp,
                                                    ue_bmp,
                                                    fe,
                                                    result_in_slb));
            if (result_in_slb->e_loc_found == TRUE) {
                SHR_EXIT();
            } else {
                result_in_slb->max_fbe_num = fe->fbe_cnt;
                result_in_slb->e_loc.bb_bmp = bb_bmp;
                result_in_slb->e_loc.be_bmp = fe->bmp;
                result_in_slb->e_loc.le_bmp = fe->bmp;
                result_in_slb->nm_offset = 0;
                result_in_slb->e_loc_found = TRUE;
            }
        } else {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find free slot for an entry of BM1 in a BM2 bucket.
 *
 * Try to find free slot for an entry BM1 in a BM2 bucket.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank Bank number.
 * \param [in] tbl_inst Table instance.
 * \param [in] slb_info rm_hash_ent_slb_info_t structure.
 * \param [in] e_attr Pointer to rm_hash_req_ent_attr_t structure.
 * \param [out] result_in_slb Pointer to rm_hash_slb_free_slot_info_t structure.
 */
static int
rm_hash_bm1_srch_in_bm2_single_slb(int unit,
                                   rm_hash_pt_info_t *pt_info,
                                   uint8_t rbank,
                                   int tbl_inst,
                                   rm_hash_ent_slb_info_t slb_info,
                                   rm_hash_req_ent_attr_t *e_attr,
                                   rm_hash_slb_free_slot_info_t *result_in_slb)
{
    uint8_t hlb_fbe_cnt = 0, hlb_bb_bmp = 0;
    uint8_t bb_idx = 0, vbb_bmp = 0, bb_bmp = 0;
    uint8_t ue_bmp = 0;

    const rm_hash_free_ent_t *fe = NULL, *hlb_qual_fe = NULL;
    rm_hash_slb_ent_bmp_t slb_ent_bmp;

    SHR_FUNC_ENTER(unit);

    sal_memset(&slb_ent_bmp, 0, sizeof(slb_ent_bmp));
    SHR_IF_ERR_EXIT
        (rm_hash_slb_ent_bmp_get(unit, pt_info, rbank, tbl_inst, slb_info.slb,
                                 &slb_ent_bmp));
    vbb_bmp = slb_info.vbb_bmp;

    for (bb_idx = 0; bb_idx < RM_HASH_BM2_BASE_BKT_NUM; bb_idx++) {
        bb_bmp = 0x01 << bb_idx;
        if (bb_bmp & vbb_bmp) {
            /* One of valid base buckets has been determined */
            SHR_IF_ERR_EXIT
                (rm_hash_bb_view_ue_bmp_get(unit,
                                            &slb_ent_bmp,
                                            bb_bmp,
                                            RM_HASH_BM_1,
                                            &ue_bmp));
            SHR_IF_ERR_EXIT
                (rm_hash_nibble_free_space_info_get(unit,
                                                    ue_bmp,
                                                    e_attr->e_size,
                                                    &fe));
            if ((fe->avail == TRUE) && (hlb_qual_fe == NULL)) {
                /*
                 * A free slot has been found in one of the base buckets
                 * of this hardware logical buckets. The corresponding
                 * base bucket bitmap and free entry info in this base
                 * bucket will be recorded.
                 */
                hlb_bb_bmp = bb_bmp;
                hlb_qual_fe = fe;
            }
            hlb_fbe_cnt += fe->fbe_cnt;
        }
    }

    if (hlb_qual_fe == NULL) {
        /*
         * There is no free space in the hardware logical bucket
         * in this bank.
         */
        result_in_slb->e_loc_found = FALSE;
    } else {
        result_in_slb->e_loc.bb_bmp = hlb_bb_bmp;
        result_in_slb->e_loc.be_bmp = hlb_qual_fe->bmp;
        result_in_slb->e_loc.le_bmp = bm1e_be_to_le_map[hlb_qual_fe->bmp].le_bmp;
        result_in_slb->max_fbe_num = hlb_fbe_cnt;
        result_in_slb->e_loc_found = TRUE;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find free slot for an entry of BM2 in a BM2 bucket.
 *
 * Try to find free slot for an entry BM2 in a BM2 bucket.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank Bank number.
 * \param [in] tbl_inst Table instance.
 * \param [in] slb_info rm_hash_ent_slb_info_t structure.
 * \param [in] e_attr Pointer to rm_hash_req_ent_attr_t structure.
 * \param [out] result_in_slb Pointer to rm_hash_slb_free_slot_info_t structure.
 */
static int
rm_hash_bm2_srch_in_bm2_single_slb(int unit,
                                   rm_hash_pt_info_t *pt_info,
                                   uint8_t rbank,
                                   int tbl_inst,
                                   rm_hash_ent_slb_info_t slb_info,
                                   rm_hash_req_ent_attr_t *e_attr,
                                   rm_hash_slb_free_slot_info_t *result_in_slb)
{
    uint8_t hlb_fbe_cnt = 0, hlb_bb_bmp = 0;
    uint8_t bb_idx = 0, vbb_bmp = 0, bb_bmp = 0;
    uint8_t ue_bmp = 0;
    const rm_hash_free_ent_t *fe = NULL, *hlb_qual_fe = NULL;
    rm_hash_slb_ent_bmp_t slb_ent_bmp;

    SHR_FUNC_ENTER(unit);

    sal_memset(&slb_ent_bmp, 0, sizeof(slb_ent_bmp));
    SHR_IF_ERR_EXIT
        (rm_hash_slb_ent_bmp_get(unit, pt_info, rbank, tbl_inst, slb_info.slb,
                                 &slb_ent_bmp));
    vbb_bmp = slb_info.vbb_bmp;

    for (bb_idx = 0; bb_idx < RM_HASH_BM2_BASE_BKT_NUM; bb_idx++) {
        bb_bmp = 0x01 << bb_idx;
        if (bb_bmp & vbb_bmp) {
            /* One of valid base buckets has been determined */
            SHR_IF_ERR_EXIT
                (rm_hash_bb_view_ue_bmp_get(unit,
                                            &slb_ent_bmp,
                                            bb_bmp,
                                            RM_HASH_BM_2,
                                            &ue_bmp));
            SHR_IF_ERR_EXIT
                (rm_hash_nibble_free_space_info_get(unit,
                                                    ue_bmp,
                                                    e_attr->e_size,
                                                    &fe));
            if ((fe->avail == TRUE) && (hlb_qual_fe == NULL)) {
                /*
                 * A free slot has been found in one of the base buckets
                 * of this hardware logical buckets. The corresponding
                 * base bucket bitmap and free entry info in this base
                 * bucket will be recorded.
                 */
                hlb_bb_bmp = bb_bmp;
                hlb_qual_fe = fe;
            }
            hlb_fbe_cnt += fe->fbe_cnt;
        }
    }

    if (hlb_qual_fe == NULL) {
        /*
         * There is no free space in the hardware logical bucket
         * in this bank.
         */
        result_in_slb->e_loc_found = FALSE;
    } else {
        result_in_slb->e_loc.bb_bmp = hlb_bb_bmp;
        result_in_slb->e_loc.be_bmp = hlb_qual_fe->bmp;
        result_in_slb->e_loc.le_bmp = 0x0F;
        result_in_slb->max_fbe_num = hlb_fbe_cnt;
        result_in_slb->e_loc_found = TRUE;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find free slot for an entry of BM in a bucket list.
 *
 * Try to find free slot for an entry BM0 in a BM1 bucket.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank_list List of banks on which the slots will be searched.
 * \param [in] tbl_inst Table instance.
 * \param [in] slb_info_list Pointer to rm_hash_ent_slb_info_t structure.
 * \param [in] num_slb_info Number of software logical buckets.
 * \param [in] e_attr Pointer to rm_hash_req_ent_attr_t structure.
 * \param [out] result Pointer to rm_hash_free_slot_info_t structure.
 */
static int
rm_hash_ent_free_slot_srch_in_slb_list(int unit,
                                       rm_hash_pt_info_t *pt_info,
                                       uint8_t *rbank_list,
                                       int tbl_inst,
                                       rm_hash_ent_slb_info_t *slb_info_list,
                                       uint8_t num_slb_info,
                                       rm_hash_req_ent_attr_t *e_attr,
                                       rm_hash_free_slot_info_t *result)
{
    uint8_t idx = 0, grp = 0, inst = 0, tmp =0;
    uint8_t single_max_fbe_num = 0, multi_max_fbe_num = 0;
    uint8_t minst_qgrp = 0, sinst_qgrp = 0;
    bool multi_inst_grp = FALSE;
    bool minst_grp_free = FALSE, sinst_grp_free = FALSE;
    fn_ent_slb_srch_t func = NULL;
    rm_hash_bm_t e_bm = e_attr->e_bm;

    static fn_ent_slb_srch_t slb_srch[RM_HASH_BM_CNT] = {
        NULL,
        rm_hash_bm0_srch_in_bm2_single_slb,
        rm_hash_bm1_srch_in_bm2_single_slb,
        rm_hash_bm2_srch_in_bm2_single_slb
    };
    static uint8_t factor[RM_HASH_BM_CNT] = { 0, 1, 2, 4 };

    SHR_FUNC_ENTER(unit);

    func = slb_srch[e_bm];
    sal_memset(&mgrp_slb_info[unit], 0, sizeof(mgrp_slb_info[unit]));

    for (idx = 0; idx < num_slb_info; idx++) {
        grp  = slb_info_list[idx].map & 0x0F;
        multi_inst_grp = slb_info_list[idx].map & RM_HASH_MULTI_INST_GRP;
        inst = (slb_info_list[idx].map & RM_HASH_MGRP_INST_MASK) >> 0x04;
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_ha_state_alloc(unit, pt_info, rbank_list[idx],
                                           tbl_inst));
        SHR_IF_ERR_EXIT
            (func(unit,
                  pt_info,
                  rbank_list[idx],
                  tbl_inst,
                  slb_info_list[idx],
                  e_attr,
                  &mgrp_slb_info[unit].slb_fsi[grp][inst]));

        mgrp_slb_info[unit].slb_idx[grp][inst] = idx;
        if ((multi_inst_grp == FALSE) && (mgrp_slb_info[unit].slb_fsi[grp][0].e_loc_found == TRUE) &&
            (mgrp_slb_info[unit].slb_fsi[grp][0].nm_offset > 0)) {
            sinst_grp_free = TRUE;
            sinst_qgrp = grp;
            break;
        }

        /* Update the maximum free base entry number. */
        if ((multi_inst_grp == FALSE) && (mgrp_slb_info[unit].slb_fsi[grp][0].e_loc_found == TRUE) &&
            (mgrp_slb_info[unit].slb_fsi[grp][0].max_fbe_num > single_max_fbe_num)) {
            sinst_grp_free = TRUE;
            single_max_fbe_num = mgrp_slb_info[unit].slb_fsi[grp][0].max_fbe_num;
            sinst_qgrp = grp;
            if (single_max_fbe_num == RM_HASH_BASE_BKT_SIZE * factor[e_bm]) {
                break;
            }
        }

        if ((multi_inst_grp == TRUE) && (mgrp_slb_info[unit].slb_fsi[grp][0].e_loc_found == TRUE) &&
            (mgrp_slb_info[unit].slb_fsi[grp][1].e_loc_found == TRUE)) {
            minst_grp_free = TRUE;
            tmp = mgrp_slb_info[unit].slb_fsi[grp][0].max_fbe_num + mgrp_slb_info[unit].slb_fsi[grp][1].max_fbe_num;
            if (tmp > multi_max_fbe_num) {
                multi_max_fbe_num = tmp;
                minst_qgrp = grp;
                if (multi_max_fbe_num == RM_HASH_BASE_BKT_SIZE * factor[e_bm] * 2) {
                    break;
                }
            }
        }
    }
    if (minst_grp_free == TRUE) {
        /* Free slot has been found. */
        result->found = TRUE;
        for (idx = 0; idx < RM_HASH_MAX_GRP_INST_COUNT; idx++) {
            result->slb_info_idx_list[idx] = mgrp_slb_info[unit].slb_idx[minst_qgrp][idx];
            result->e_loc_list[idx] = mgrp_slb_info[unit].slb_fsi[minst_qgrp][idx].e_loc;
            result->nm_offset_list[idx] = mgrp_slb_info[unit].slb_fsi[minst_qgrp][idx].nm_offset;
        }
        result->num_inst = RM_HASH_MAX_GRP_INST_COUNT;
        SHR_EXIT();
    }
    if (sinst_grp_free == TRUE) {
        /* Free slot has been found. */
        result->found = TRUE;
        result->slb_info_idx_list[0] = mgrp_slb_info[unit].slb_idx[sinst_qgrp][0];
        result->e_loc_list[0] = mgrp_slb_info[unit].slb_fsi[sinst_qgrp][0].e_loc;
        result->nm_offset_list[0] = mgrp_slb_info[unit].slb_fsi[sinst_qgrp][0].nm_offset;
        result->num_inst = 1;
        SHR_EXIT();
    }

    result->found = FALSE;
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
int
bcmptm_rm_hash_bkt_free_ent_space_get(int unit,
                                      rm_hash_pt_info_t *pt_info,
                                      rm_hash_slb_ent_bmp_t *slb_e_bmp,
                                      rm_hash_req_ent_info_t *e_info,
                                      bool *accom,
                                      rm_hash_ent_loc_t *e_loc)
{
    uint8_t bb_bmp = 0, bb_idx = 0;
    uint8_t ue_bmp = 0;
    const rm_hash_free_ent_t *fe = NULL;

    SHR_FUNC_ENTER(unit);

    *accom = FALSE;
    for (bb_idx = 0; bb_idx < RM_HASH_BM2_BASE_BKT_NUM; bb_idx++) {
        bb_bmp = 0x01 << bb_idx;
        if (bb_bmp & e_info->vbb_bmp) {
            SHR_IF_ERR_EXIT
                (rm_hash_bb_view_ue_bmp_get(unit,
                                            slb_e_bmp,
                                            bb_bmp,
                                            e_info->e_bm,
                                            &ue_bmp));
            SHR_IF_ERR_EXIT
                (rm_hash_nibble_free_space_info_get(unit,
                                                    ue_bmp,
                                                    e_info->e_size,
                                                    &fe));
            if (fe->avail == TRUE) {
                e_loc->bb_bmp = bb_bmp;
                e_loc->be_bmp = fe->bmp;
                switch (e_info->e_bm) {
                case RM_HASH_BM_N:
                case RM_HASH_BM_0:
                    e_loc->le_bmp = e_loc->be_bmp;
                    break;
                case RM_HASH_BM_1:
                    e_loc->le_bmp = bm1e_be_to_le_map[e_loc->be_bmp].le_bmp;
                    break;
                case RM_HASH_BM_2:
                    e_loc->le_bmp = 0x0F;
                    break;
                case RM_HASH_BM_CNT:
                    SHR_ERR_EXIT(SHR_E_PARAM);
                default:
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                *accom = TRUE;
                break;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_free_ent_space_search(int unit,
                                     rm_hash_pt_info_t *pt_info,
                                     uint8_t *rbank_list,
                                     int tbl_inst,
                                     rm_hash_ent_slb_info_t *slb_info_list,
                                     uint8_t num_slb_info,
                                     rm_hash_req_ent_attr_t *e_attr,
                                     rm_hash_free_slot_info_t *result)
{
    fn_ent_free_slot_srch_t func = NULL;
    rm_hash_bm_t t_bm = pt_info->t_bm;
    rm_hash_bm_t e_bm = e_attr->e_bm;
    static fn_ent_free_slot_srch_t srch_func[RM_HASH_BM_CNT][RM_HASH_BM_CNT] = {
        {
         rm_hash_bmn_srch_in_bmn_slb_list,
         NULL,
         NULL,
         NULL
        },
        {
         NULL,
         rm_hash_bm0_srch_in_bm0_slb_list,
         NULL,
         NULL
        },
        {
         rm_hash_bmn_srch_in_bm1_slb_list,
         rm_hash_bm0_srch_in_bm1_slb_list,
         rm_hash_bm1_srch_in_bm1_slb_list,
         NULL
        },
        {
         NULL,
         rm_hash_ent_free_slot_srch_in_slb_list,
         rm_hash_ent_free_slot_srch_in_slb_list,
         rm_hash_ent_free_slot_srch_in_slb_list
        }
    };

    SHR_FUNC_ENTER(unit);

    if (num_slb_info == 0) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    func = srch_func[t_bm][e_bm];
    if (func != NULL) {
        SHR_IF_ERR_EXIT
            (func(unit, pt_info, rbank_list, tbl_inst, slb_info_list,
                  num_slb_info, e_attr, result));
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}
