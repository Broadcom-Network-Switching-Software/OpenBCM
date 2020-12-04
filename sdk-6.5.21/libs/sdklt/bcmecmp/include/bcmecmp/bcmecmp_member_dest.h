/*! \file bcmecmp_member_dest.h
 *
 * This file contains ECMP custom table handler data structures definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMECMP_MEMBER_DEST_H
#define BCMECMP_MEMBER_DEST_H

#include <shr/shr_types.h>
#include <shr/shr_bitop.h>
#include <shr/shr_itbm.h>
#include <sal/sal_mutex.h>
#include <bcmdrd_config.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmecmp/generated/bcmecmp_member_dest_ha.h>
#include <bcmimm/bcmimm_int_comp.h>


/*! \brief The max number NextHop paths. */
#define BCMECMP_MEMBER_DEST_ARRAY_SIZE (4096)

/*!
 * \brief Structure for ECMP logical table fields information.
 */
typedef struct bcmecmp_member_dest_lt_field_attrs_s {
    /*! Minimum ECMP Group Identifier value supported. */
    int min_ecmp_id;

    /*! Maximum ECMP Group Identifier value supported. */
    int max_ecmp_id;

    /*! Minimum value for configured ECMP paths allowed for groups. */
    uint32_t min_npaths;

    /*! Maximum value for configured ECMP paths allowed for groups. */
    uint32_t max_npaths;

} bcmecmp_member_dest_lt_field_attrs_t;


/*!
 * \brief ECMP group array base pointer.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns ECMP group array base pointer.
 */
#define BCMECMP_MEMBER0_GRP_INFO_PTR(_u) \
        ((BCMECMP_INFO_PTR(_u))->dest_info0.grp_member_dest_arr)

/*!
 * \brief Minimum value supported for ECMP ID field of the logical table.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns Minimum ECMP ID value supported by the logical table.
 */
#define BCMECMP_MEMBER0_LT_MIN_ECMP_ID(_u) \
            (BCMECMP_MEMBER0_LT_FIELD_ATTRS_PTR(_u)->min_ecmp_id)

/*!
 * \brief Maximum value supported for ECMP ID field of the logical table.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns Maximum ECMP ID value supported by the logical table.
 */
#define BCMECMP_MEMBER0_LT_MAX_ECMP_ID(_u) \
            (BCMECMP_MEMBER0_LT_FIELD_ATTRS_PTR(_u)->max_ecmp_id)

/*!
 * \brief Minimum value supported for NUM_PATHS field of the logical table.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns Minimum value supported for MAX_PATHS field of the logical table.
 */
#define BCMECMP_MEMBER0_LT_MIN_NPATHS(_u) \
            (BCMECMP_MEMBER0_LT_FIELD_ATTRS_PTR(_u)->min_npaths)

/*!
 * \brief Maximum value supported for NUM_PATHS field of the logical table.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns Maximum value supported for MAX_PATHS field of the logical table.
 */
#define BCMECMP_MEMBER0_LT_MAX_NPATHS(_u) \
            (BCMECMP_MEMBER0_LT_FIELD_ATTRS_PTR(_u)->max_npaths)

/*!
 * \brief Get ECMP information structure for given BCM unit.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns ECMP information structure of the unit.
 */
#define BCMECMP_MEMBER0_INFO(_u) (BCMECMP_INFO_PTR(_u)->dest_info0)

/*!
 * \brief Set/Get the LTD symbol identifiers of fields in a given table.
 *
 * \param [in] _u BCM unit number.
 */
#define BCMECMP_MEMBER0_INFO_FIDS(_u) \
            (BCMECMP_INFO_PTR(_u)->dest_info0.ltd_fids)

/*!
 * \brief ECMP group logical table field attributes structure pointer.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns Pointer to ECMP Group logical table field attributes structure.
 */
#define BCMECMP_MEMBER0_LT_FIELD_ATTRS_PTR(_u) \
            ((bcmecmp_member_dest_lt_field_attrs_t*)\
            ((BCMECMP_MEMBER0_INFO(_u)).lt_fattrs))


/*!
 * \brief Get ECMP information structure pointer for given BCM unit.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns ECMP information structure pointer of the unit.
 */
#define BCMECMP_MEMBER0_INFO_PTR(_u) (&(BCMECMP_INFO_PTR(_u)->dest_info0))

/*!
 * \brief Set/Get the LTD symbol identifiers of fields in a given table.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns LTD symbol identifier values of fields in the table.
 */
#define BCMECMP_MEMBER0_TBL_LTD_FIELDS(_u) \
            ((BCMECMP_MEMBER0_INFO(_u)).ltd_fids)


/*!
 * \brief Set/Get the maximum entry ID value supported by a given table.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns Maximum entry ID value supported by the table.
 */

#define BCMECMP_MEMBER0_TBL_SIZE(_u) \
             (BCMECMP_TBL_SIZE(unit, member, BCMECMP_GRP_TYPE_LEVEL0) + \
            + BCMECMP_TBL_SIZE(unit, member2, BCMECMP_GRP_TYPE_LEVEL0))


/*!
 * \brief Get ECMP entries in WAL information.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns ECMP entries in WAL details.
 */
#define BCMECMP_MEMBER0_WALI_PTR(_u) \
           ((BCMECMP_MEMBER0_INFO_PTR(_u))->wali)

/*!
 * \brief Number of ECMP Paths configured for a given ECMP group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Number of ECMP paths configured for the group.
 */
#define BCMECMP_MEMBER0_GRP_NUM_PATHS(_u, _id) \
            ((BCMECMP_MEMBER0_GRP_INFO_PTR(_u))[(_id)].num_paths)

/*!
 * \brief ECMP group entries backup array base pointer.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns ECMP group high availability data array base pointer.
 */
#define BCMECMP_MEMBER0_GRP_INFO_BK_PTR(_u) \
            ((BCMECMP_MEMBER0_INFO_PTR(_u))->grp_member_dest_bk_arr)


/*!
 * \brief ECMP group logical table transaction identifier value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns ECMP group logical table transaction identifier value.
 */
#define BCMECMP_MEMBER0_GRP_TRANS_ID(_u, _id) \
            ((BCMECMP_MEMBER0_GRP_INFO_PTR(_u))[(_id)].trans_id)

/*!
 * \brief ECMP group logical table transaction identifier value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns ECMP group logical table transaction identifier value.
 */
#define BCMECMP_MEMBER0_GRP_TRANS_ID_BK(_u, _id) \
            ((BCMECMP_MEMBER0_GRP_INFO_BK_PTR(_u))[(_id)].trans_id)
/*!
 * \brief ECMP group logical table source identifier value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns ECMP group logical table source identifier value.
 */
#define BCMECMP_MEMBER0_GRP_LT_SID_BK(_u, _id) \
            ((BCMECMP_MEMBER0_GRP_INFO_BK_PTR(_u))[(_id)].glt_sid)

/*!
 * \brief ECMP group logical table source identifier value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns ECMP group logical table source identifier value.
 */
#define BCMECMP_MEMBER0_GRP_LT_SID(_u, _id) \
            ((BCMECMP_MEMBER0_GRP_INFO_PTR(_u))[(_id)].glt_sid)


/*!
 * \brief Number of ECMP Paths configured for a given ECMP group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Number of ECMP paths configured for the group.
 */
#define BCMECMP_MEMBER0_GRP_NUM_PATHS_BK(_u, _id) \
            ((BCMECMP_MEMBER0_GRP_INFO_BK_PTR(_u))[(_id)].num_paths)

/*!
 * \brief Start index of a given group in ECMP member table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Start index of the ECMP group in ECMP member table.
 */
#define BCMECMP_MEMBER0_GRP_MEMB_TBL_START_IDX_BK(_u, _id) \
            ((BCMECMP_MEMBER0_GRP_INFO_BK_PTR(_u))[(_id)].mstart_idx)

/*!
 * \brief Start index of a given group in ECMP member table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Start index of the ECMP group in ECMP member table.
 */
#define BCMECMP_MEMBER0_GRP_MEMB_TBL_START_IDX(_u, _id) \
            ((BCMECMP_MEMBER0_GRP_INFO_PTR(_u))[(_id)].mstart_idx)

/*!
 * \brief Start index of a given group in ECMP member table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Start index of the ECMP group in ECMP member table.
 */
#define BCMECMP_MEMBER0_GRP_MEMB_TBL_START_AUTO_BK(_u, _id) \
            ((BCMECMP_MEMBER0_GRP_INFO_BK_PTR(_u))[(_id)].mstart_auto)

/*!
 * \brief Start index of a given group in ECMP member table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Start index of the ECMP group in ECMP member table.
 */
#define BCMECMP_MEMBER0_GRP_MEMB_TBL_START_AUTO(_u, _id) \
            ((BCMECMP_MEMBER0_GRP_INFO_PTR(_u))[(_id)].mstart_auto)

/*!
 * \brief ECMP group array pointer for a specific entry.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 * \param [in] _fld field identifier.
 *
 * \returns ECMP group array base pointer.
 */
#define BCMECMP_MEMBER0_GRP_INFO_BK_FLD(_u, _id, _fld) \
            ((BCMECMP_MEMBER0_GRP_INFO_BK_PTR(_u))[(_id)]. ##_fld)

/*!
 * \brief ECMP group logical table source identifier value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns ECMP group logical table source identifier value.
 */
#define BCMECMP_MEMBER0_GRP_LT_SID_BK(_u, _id) \
            ((BCMECMP_MEMBER0_GRP_INFO_BK_PTR(_u))[(_id)].glt_sid)

/*!
 * \brief ECMP group logical table source identifier value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns ECMP group logical table source identifier value.
 */
#define BCMECMP_MEMBER0_GRP_LT_SID(_u, _id) \
            ((BCMECMP_MEMBER0_GRP_INFO_PTR(_u))[(_id)].glt_sid)

/*!
 * \brief Start index of a given group in ECMP member table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Start index of the ECMP group in ECMP member table.
 */
#define BCMECMP_MEMBER0_GRP_MEMB_TBL_START_IDX_BK(_u, _id) \
            ((BCMECMP_MEMBER0_GRP_INFO_BK_PTR(_u))[(_id)].mstart_idx)


/*! \brief The max number of groups. */
#define BCMECMP_MEMBER_DEST_GRP_TBL_SIZE (8)

/*!
 * \brief Structure for ECMP group table LTD fields information.
 */
typedef struct bcmecmp_member_dest_grp_ltd_fields_t_s {

    /*! ECMP group identifier LTD field identifier. */
    bcmecmp_ltd_fid_t ecmp_id_fid;

    /*! Number of ECMP paths configured LTD field identifier. */
    bcmecmp_ltd_fid_t num_paths_fid;

    /*! base index of the group. */
    bcmecmp_ltd_fid_t base_index_fid;

    /*! base index of the group. */
    bcmecmp_ltd_fid_t base_index_auto_fid;

    /*! oper base index of the group. */
    bcmecmp_ltd_fid_t base_index_oper_fid;

} bcmecmp_member_dest_ltd_fields_t;

/*!
 * ECMP overlay LT IMM register function.
 */
extern int
bcmecmp_member0_imm_register(int unit, bcmlrd_sid_t sid);

/*!
 * ECMP overlay LT IMM register function.
 */
extern int
bcmecmp_member1_imm_register(int unit, bcmlrd_sid_t sid);


/*!
 * \brief Get ECMP LT Entry buffer pointer.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns ECMP LT Entry buffer pointer.
 */
#define BCMECMP_MEMBER0_LT_ENT_PTR(_u) \
          ((BCMECMP_MEMBER0_INFO_PTR(_u))->lt_ent)

/*!
 * \brief Get ECMP current LT Entry buffer pointer.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns ECMP LT Entry buffer pointer.
 */
#define BCMECMP_MEMBER0_CURRENT_LT_ENT_PTR(_u) \
          ((BCMECMP_MEMBER0_INFO_PTR(_u))->current_lt_ent)

/*!
 * \brief ECMP group logical table field attributes structure void * pointer.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns Void * pointer to logical table field attributes structure.
 */
#define BCMECMP_MEMBER0_LT_FIELD_ATTRS_VOID_PTR(_u) \
            ((BCMECMP_MEMBER0_INFO(_u)).lt_fattrs)

/*! Invalid Index or ID. */
#define BCMECMP_MEMBER_DEST_INVALID (0xffff)

/*!
 * \brief Get the ITBM_LIST corresponding to the given table.
 *
 * \param [in] _iptr Table info pointer.
 *
 * \returns Pointer to the ECMP table structure.
 */
  #define BCMECMP_MEMBER0_TBL_ITBM_LIST(_iptr)    \
            (((_iptr)->dest_info0.list_hdl))

/*!
 * \brief Get ECMP table structure pointer for a given table name.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var Table name.
 *
 * \returns Pointer to the ECMP table structure.
 */
#define BCMECMP_MEMBER0_TBL_PTR(_u, _var) \
            (&(BCMECMP_INFO(_u)._var[BCMECMP_GRP_TYPE_LEVEL0]))

/*!
 * \brief Set the entry staged status.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * set the entry staged status.
 *
 * \param [in] _iptr Table info pointer.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \returns None.
 * table.
 */
#define BCMECMP_MEMBER0_TBL_ENT_STAGED_SET(_iptr, _i)    \
            (((_iptr)->dest_info0).flags |= (BCMECMP_ENTRY_STAGED))

/*!
 * \brief Clear the entry staged status.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * clear the entry staged status.
 *
 * \param [in] _iptr Table base pointer.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \returns None.
 * table.
 */
#define BCMECMP_MEMBER0_TBL_ENT_STAGED_CLEAR(_iptr, _i)    \
            (((_iptr)->dest_info0).flags &= (~BCMECMP_ENTRY_STAGED))

/*!
 * \brief Get entry staged status.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * get the entry staged status.
 *
 * \param [in] _iptr Table base pointer.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \returns Entry staged status.
 * table.
 */
#define BCMECMP_MEMBER0_TBL_ENT_STAGED_GET(_iptr, _i)    \
            (((_iptr)->dest_info0).flags & (BCMECMP_ENTRY_STAGED))

/*!
 * \brief Set the entry staged status.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * set the entry staged status.
 *
 * \param [in] _iptr Table base pointer.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \returns None.
 * table.
 */
#define BCMECMP_MEMBER0_TBL_BK_ENT_STAGED_SET(_iptr, _i)    \
            (((_iptr)->dest_info0).bk_flags |= (BCMECMP_ENTRY_STAGED))

/*!
 * \brief Clear the entry staged status.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * clear the entry staged status.
 *
 * \param [in] _iptr Table base pointer.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \returns None.
 * table.
 */
#define BCMECMP_MEMBER0_TBL_BK_ENT_STAGED_CLEAR(_iptr, _i)    \
            (((_iptr)->dest_info0).bk_flags &= (~BCMECMP_ENTRY_STAGED))

/*!
 * \brief Get entry staged status.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * get the entry staged status.
 *
 * \param [in] _iptr Table base pointer.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \returns Entry staged status.
 * table.
 */
#define BCMECMP_MEMBER0_TBL_BK_ENT_STAGED_GET(_iptr, _i)    \
            (((_iptr)->dest_info0).bk_flags & (BCMECMP_ENTRY_STAGED))


/*!
 * \brief Checks whether the given group is in use and returns the status.
 *
 * \param [in] unit Unit no.
 * \param [in] grp_id Group ID.
 * \param [out] in_use specifies whether the group is in use.
 *
 * \returns SHR_E_NONE if successful.
 */
extern int
bcmecmp_member0_grp_in_use(int unit, uint32_t grp_id, bool *in_use);

/*!
 * \brief Adds the given group and programs HW tables.
 *
 * \param [in] unit Unit no.
 * \param [in] lt_entry pointer to lt_entry structure.
 *
 * \returns SHR_E_NONE if successful, or an appropriate return code.
 */
extern int
bcmecmp_member0_add(int unit,
                  bcmecmp_flex_lt_entry_t *lt_entry);
/*!
 * \brief Deletes the given group and programs HW tables.
 *
 * \param [in] unit Unit no.
 * \param [in] lt_entry pointer to lt_entry structure.
 *
 * \returns SHR_E_NONE if successful, or an appropriate return code.
 */
extern int
bcmecmp_member0_delete(int unit,
                     bcmecmp_flex_lt_entry_t *lt_entry);
/*!
 * \brief Updates the given group and programs HW tables.
 *
 * \param [in] unit Unit no.
 * \param [in] lt_entry pointer to lt_entry structure.
 *
 * \returns SHR_E_NONE if successful, or an appropriate return code.
 */
extern int
bcmecmp_member0_update(int unit,
                     bcmecmp_flex_lt_entry_t *lt_entry);

/*!
 * \brief Updates the given group and programs HW tables.
 *
 * \param [in] unit Unit no.
 * \param [in] sid sid of the table.
 * \param [in] lt_entry pointer to lt_entry structure.
 *
 * \returns SHR_E_NONE if successful, or an appropriate return code.
 */
extern int
bcmecmp_member0_lt_entry_t_init(int unit,
                                    bcmltd_sid_t sid,
                                    bcmecmp_flex_lt_entry_t *lt_entry);
/*!
 * \brief Updates the given group and programs HW tables.
 *
 * \param [in] unit Unit no.
 * \param [in] sid  sid of the table.
 * \param [in] trans_id  Transaction ID.
 * \param [in] context  pointer to context structure.
 * \param [in] lkup_type  Specifies whether HW lookup or local.
 * \param [in] in list of input fields.
 * \param [out] out list of output fields.
 *
 * \returns SHR_E_NONE if successful, or an appropriate return code.
 */
extern int
bcmecmp_member0_get(int unit,
                              bcmltd_sid_t sid,
                              uint32_t trans_id,
                              void *context,
                              bcmimm_lookup_type_t lkup_type,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out);
/*!
 * \brief ECMP group array base pointer.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns ECMP group array base pointer.
 */
#define BCMECMP_MEMBER1_GRP_INFO_PTR(_u) \
        ((BCMECMP_INFO_PTR(_u))->dest_info1.grp_member_dest_arr)

/*!
 * \brief Minimum value supported for ECMP ID field of the logical table.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns Minimum ECMP ID value supported by the logical table.
 */
#define BCMECMP_MEMBER1_LT_MIN_ECMP_ID(_u) \
            (BCMECMP_MEMBER1_LT_FIELD_ATTRS_PTR(_u)->min_ecmp_id)

/*!
 * \brief Maximum value supported for ECMP ID field of the logical table.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns Maximum ECMP ID value supported by the logical table.
 */
#define BCMECMP_MEMBER1_LT_MAX_ECMP_ID(_u) \
            (BCMECMP_MEMBER1_LT_FIELD_ATTRS_PTR(_u)->max_ecmp_id)

/*!
 * \brief Minimum value supported for NUM_PATHS field of the logical table.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns Minimum value supported for MAX_PATHS field of the logical table.
 */
#define BCMECMP_MEMBER1_LT_MIN_NPATHS(_u) \
            (BCMECMP_MEMBER1_LT_FIELD_ATTRS_PTR(_u)->min_npaths)

/*!
 * \brief Maximum value supported for NUM_PATHS field of the logical table.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns Maximum value supported for MAX_PATHS field of the logical table.
 */
#define BCMECMP_MEMBER1_LT_MAX_NPATHS(_u) \
            (BCMECMP_MEMBER1_LT_FIELD_ATTRS_PTR(_u)->max_npaths)

/*!
 * \brief Get ECMP information structure for given BCM unit.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns ECMP information structure of the unit.
 */
#define BCMECMP_MEMBER1_INFO(_u) (BCMECMP_INFO_PTR(_u)->dest_info1)

/*!
 * \brief Set/Get the LTD symbol identifiers of fields in a given table.
 *
 * \param [in] _u BCM unit number.
 */
#define BCMECMP_MEMBER1_INFO_FIDS(_u) \
            (BCMECMP_INFO_PTR(_u)->dest_info1.ltd_fids)

/*!
 * \brief ECMP group logical table field attributes structure pointer.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns Pointer to ECMP Group logical table field attributes structure.
 */
#define BCMECMP_MEMBER1_LT_FIELD_ATTRS_PTR(_u) \
            ((bcmecmp_member_dest_lt_field_attrs_t*)\
            ((BCMECMP_MEMBER1_INFO(_u)).lt_fattrs))


/*!
 * \brief Get ECMP information structure pointer for given BCM unit.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns ECMP information structure pointer of the unit.
 */
#define BCMECMP_MEMBER1_INFO_PTR(_u) (&(BCMECMP_INFO_PTR(_u)->dest_info1))

/*!
 * \brief Set/Get the LTD symbol identifiers of fields in a given table.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns LTD symbol identifier values of fields in the table.
 */
#define BCMECMP_MEMBER1_TBL_LTD_FIELDS(_u) \
            ((BCMECMP_MEMBER1_INFO(_u)).ltd_fids)


/*!
 * \brief Set/Get the maximum entry ID value supported by a given table.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns Maximum entry ID value supported by the table.
 */

#define BCMECMP_MEMBER1_TBL_SIZE(_u) \
             (BCMECMP_TBL_SIZE(unit, member, BCMECMP_GRP_TYPE_LEVEL0) + \
            + BCMECMP_TBL_SIZE(unit, member2, BCMECMP_GRP_TYPE_LEVEL0))


/*!
 * \brief Get ECMP entries in WAL information.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns ECMP entries in WAL details.
 */
#define BCMECMP_MEMBER1_WALI_PTR(_u) \
           ((BCMECMP_MEMBER1_INFO_PTR(_u))->wali)

/*!
 * \brief Number of ECMP Paths configured for a given ECMP group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Number of ECMP paths configured for the group.
 */
#define BCMECMP_MEMBER1_GRP_NUM_PATHS(_u, _id) \
            ((BCMECMP_MEMBER1_GRP_INFO_PTR(_u))[(_id)].num_paths)

/*!
 * \brief ECMP group entries backup array base pointer.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns ECMP group high availability data array base pointer.
 */
#define BCMECMP_MEMBER1_GRP_INFO_BK_PTR(_u) \
            ((BCMECMP_MEMBER1_INFO_PTR(_u))->grp_member_dest_bk_arr)


/*!
 * \brief ECMP group logical table transaction identifier value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns ECMP group logical table transaction identifier value.
 */
#define BCMECMP_MEMBER1_GRP_TRANS_ID(_u, _id) \
            ((BCMECMP_MEMBER1_GRP_INFO_PTR(_u))[(_id)].trans_id)

/*!
 * \brief ECMP group logical table transaction identifier value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns ECMP group logical table transaction identifier value.
 */
#define BCMECMP_MEMBER1_GRP_TRANS_ID_BK(_u, _id) \
            ((BCMECMP_MEMBER1_GRP_INFO_BK_PTR(_u))[(_id)].trans_id)
/*!
 * \brief ECMP group logical table source identifier value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns ECMP group logical table source identifier value.
 */
#define BCMECMP_MEMBER1_GRP_LT_SID_BK(_u, _id) \
            ((BCMECMP_MEMBER1_GRP_INFO_BK_PTR(_u))[(_id)].glt_sid)

/*!
 * \brief ECMP group logical table source identifier value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns ECMP group logical table source identifier value.
 */
#define BCMECMP_MEMBER1_GRP_LT_SID(_u, _id) \
            ((BCMECMP_MEMBER1_GRP_INFO_PTR(_u))[(_id)].glt_sid)


/*!
 * \brief Number of ECMP Paths configured for a given ECMP group.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Number of ECMP paths configured for the group.
 */
#define BCMECMP_MEMBER1_GRP_NUM_PATHS_BK(_u, _id) \
            ((BCMECMP_MEMBER1_GRP_INFO_BK_PTR(_u))[(_id)].num_paths)

/*!
 * \brief Start index of a given group in ECMP member table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Start index of the ECMP group in ECMP member table.
 */
#define BCMECMP_MEMBER1_GRP_MEMB_TBL_START_IDX_BK(_u, _id) \
            ((BCMECMP_MEMBER1_GRP_INFO_BK_PTR(_u))[(_id)].mstart_idx)

/*!
 * \brief Start index of a given group in ECMP member table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Start index of the ECMP group in ECMP member table.
 */
#define BCMECMP_MEMBER1_GRP_MEMB_TBL_START_IDX(_u, _id) \
            ((BCMECMP_MEMBER1_GRP_INFO_PTR(_u))[(_id)].mstart_idx)

/*!
 * \brief Start index of a given group in ECMP member table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Start index of the ECMP group in ECMP member table.
 */
#define BCMECMP_MEMBER1_GRP_MEMB_TBL_START_AUTO_BK(_u, _id) \
            ((BCMECMP_MEMBER1_GRP_INFO_BK_PTR(_u))[(_id)].mstart_auto)

/*!
 * \brief Start index of a given group in ECMP member table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Start index of the ECMP group in ECMP member table.
 */
#define BCMECMP_MEMBER1_GRP_MEMB_TBL_START_AUTO(_u, _id) \
            ((BCMECMP_MEMBER1_GRP_INFO_PTR(_u))[(_id)].mstart_auto)

/*!
 * \brief ECMP group array pointer for a specific entry.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 * \param [in] _fld field identifier.
 *
 * \returns ECMP group array base pointer.
 */
#define BCMECMP_MEMBER1_GRP_INFO_BK_FLD(_u, _id, _fld) \
            ((BCMECMP_MEMBER1_GRP_INFO_BK_PTR(_u))[(_id)]. ##_fld)

/*!
 * \brief ECMP group logical table source identifier value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns ECMP group logical table source identifier value.
 */
#define BCMECMP_MEMBER1_GRP_LT_SID_BK(_u, _id) \
            ((BCMECMP_MEMBER1_GRP_INFO_BK_PTR(_u))[(_id)].glt_sid)

/*!
 * \brief ECMP group logical table source identifier value.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns ECMP group logical table source identifier value.
 */
#define BCMECMP_MEMBER1_GRP_LT_SID(_u, _id) \
            ((BCMECMP_MEMBER1_GRP_INFO_PTR(_u))[(_id)].glt_sid)

/*!
 * \brief Start index of a given group in ECMP member table.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _id ECMP group identifier.
 *
 * \returns Start index of the ECMP group in ECMP member table.
 */
#define BCMECMP_MEMBER1_GRP_MEMB_TBL_START_IDX_BK(_u, _id) \
            ((BCMECMP_MEMBER1_GRP_INFO_BK_PTR(_u))[(_id)].mstart_idx)


/*! \brief The max number of groups. */
#define BCMECMP_MEMBER_DEST_GRP_TBL_SIZE (8)


/*!
 * ECMP overlay LT IMM register function.
 */
extern int
bcmecmp_member0_imm_register(int unit, bcmlrd_sid_t sid);



/*!
 * \brief Get ECMP LT Entry buffer pointer.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns ECMP LT Entry buffer pointer.
 */
#define BCMECMP_MEMBER1_LT_ENT_PTR(_u) \
          ((BCMECMP_MEMBER1_INFO_PTR(_u))->lt_ent)

/*!
 * \brief Get ECMP current LT Entry buffer pointer.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns ECMP LT Entry buffer pointer.
 */
#define BCMECMP_MEMBER1_CURRENT_LT_ENT_PTR(_u) \
          ((BCMECMP_MEMBER1_INFO_PTR(_u))->current_lt_ent)

/*!
 * \brief ECMP group logical table field attributes structure void * pointer.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns Void * pointer to logical table field attributes structure.
 */
#define BCMECMP_MEMBER1_LT_FIELD_ATTRS_VOID_PTR(_u) \
            ((BCMECMP_MEMBER1_INFO(_u)).lt_fattrs)

/*!
 * \brief Minimum value supported for ECMP ID field of the logical table.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns Minimum ECMP ID value supported by the logical table.
 */
#define BCMECMP_MEMBER1_LT_MIN_GRP_ID(_u) (0)

/*!
 * \brief Maximum value supported for ECMP ID field of the logical table.
 *
 * \param [in] _u BCM unit number.
 *
 * \returns Maximum ECMP ID value supported by the logical table.
 */
#define BCMECMP_MEMBER1_LT_MAX_GRP_ID(_u) (8)

/*! Invalid Index or ID. */
#define BCMECMP_MEMBER_DEST_INVALID (0xffff)

/*!
 * \brief Get the ITBM_LIST corresponding to the given table.
 *
 * \param [in] _iptr Table info pointer.
 *
 * \returns Pointer to the ECMP table structure.
 */
  #define BCMECMP_MEMBER1_TBL_ITBM_LIST(_iptr)    \
            (((_iptr)->dest_info1.list_hdl))

/*!
 * \brief Get ECMP table structure pointer for a given table name.
 *
 * \param [in] _u BCM unit number.
 * \param [in] _var Table name.
 *
 * \returns Pointer to the ECMP table structure.
 */
#define BCMECMP_MEMBER1_TBL_PTR(_u, _var) \
            (&(BCMECMP_INFO(_u)._var[BCMECMP_GRP_TYPE_LEVEL0]))

/*!
 * \brief Set the entry staged status.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * set the entry staged status.
 *
 * \param [in] _iptr Table info pointer.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \returns None.
 * table.
 */
#define BCMECMP_MEMBER1_TBL_ENT_STAGED_SET(_iptr, _i)    \
            (((_iptr)->dest_info1).flags |= (BCMECMP_ENTRY_STAGED))

/*!
 * \brief Clear the entry staged status.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * clear the entry staged status.
 *
 * \param [in] _iptr Table base pointer.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \returns None.
 * table.
 */
#define BCMECMP_MEMBER1_TBL_ENT_STAGED_CLEAR(_iptr, _i)    \
            (((_iptr)->dest_info1).flags &= (~BCMECMP_ENTRY_STAGED))

/*!
 * \brief Get entry staged status.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * get the entry staged status.
 *
 * \param [in] _iptr Table base pointer.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \returns Entry staged status.
 * table.
 */
#define BCMECMP_MEMBER1_TBL_ENT_STAGED_GET(_iptr, _i)    \
            (((_iptr)->dest_info1).flags & (BCMECMP_ENTRY_STAGED))

/*!
 * \brief Set the entry staged status.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * set the entry staged status.
 *
 * \param [in] _iptr Table base pointer.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \returns None.
 * table.
 */
#define BCMECMP_MEMBER1_TBL_BK_ENT_STAGED_SET(_iptr, _i)    \
            (((_iptr)->dest_info1).bk_flags |= (BCMECMP_ENTRY_STAGED))

/*!
 * \brief Clear the entry staged status.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * clear the entry staged status.
 *
 * \param [in] _iptr Table base pointer.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \returns None.
 * table.
 */
#define BCMECMP_MEMBER1_TBL_BK_ENT_STAGED_CLEAR(_iptr, _i)    \
            (((_iptr)->dest_info1).bk_flags &= (~BCMECMP_ENTRY_STAGED))

/*!
 * \brief Get entry staged status.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * get the entry staged status.
 *
 * \param [in] _iptr Table base pointer.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \returns Entry staged status.
 * table.
 */
#define BCMECMP_MEMBER1_TBL_BK_ENT_STAGED_GET(_iptr, _i)    \
            (((_iptr)->dest_info1).bk_flags & (BCMECMP_ENTRY_STAGED))


/*!
 * \brief Checks whether the given group is in use and returns the status.
 *
 * \param [in] unit Unit no.
 * \param [in] grp_id Group ID.
 * \param [out] in_use specifies whether the group is in use.
 *
 * \returns SHR_E_NONE if successful.
 */
extern int
bcmecmp_member1_grp_in_use(int unit, uint32_t grp_id, bool *in_use);

/*!
 * \brief Adds the given group and programs HW tables.
 *
 * \param [in] unit Unit no.
 * \param [in] lt_entry pointer to lt_entry structure.
 *
 * \returns SHR_E_NONE if successful, or an appropriate return code.
 */
extern int
bcmecmp_member1_add(int unit,
                  bcmecmp_flex_lt_entry_t *lt_entry);
/*!
 * \brief Deletes the given group and programs HW tables.
 *
 * \param [in] unit Unit no.
 * \param [in] lt_entry pointer to lt_entry structure.
 *
 * \returns SHR_E_NONE if successful, or an appropriate return code.
 */
extern int
bcmecmp_member1_delete(int unit,
                     bcmecmp_flex_lt_entry_t *lt_entry);
/*!
 * \brief Updates the given group and programs HW tables.
 *
 * \param [in] unit Unit no.
 * \param [in] lt_entry pointer to lt_entry structure.
 *
 * \returns SHR_E_NONE if successful, or an appropriate return code.
 */
extern int
bcmecmp_member1_update(int unit,
                     bcmecmp_flex_lt_entry_t *lt_entry);

/*!
 * \brief Updates the given group and programs HW tables.
 *
 * \param [in] unit Unit no.
 * \param [in] sid sid of the table.
 * \param [in] lt_entry pointer to lt_entry structure.
 *
 * \returns SHR_E_NONE if successful, or an appropriate return code.
 */
extern int
bcmecmp_member1_lt_entry_t_init(int unit,
                                    bcmltd_sid_t sid,
                                    bcmecmp_flex_lt_entry_t *lt_entry);
/*!
 * \brief Updates the given group and programs HW tables.
 *
 * \param [in] unit Unit no.
 * \param [in] sid  sid of the table.
 * \param [in] trans_id  Transaction ID.
 * \param [in] context  pointer to context structure.
 * \param [in] lkup_type  Specifies whether HW lookup or local.
 * \param [in] in list of input fields.
 * \param [out] out list of output fields.
 *
 * \returns SHR_E_NONE if successful, or an appropriate return code.
 */
extern int
bcmecmp_member1_get(int unit,
                              bcmltd_sid_t sid,
                              uint32_t trans_id,
                              void *context,
                              bcmimm_lookup_type_t lkup_type,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out);
#endif /* BCMECMP_MEMBER_DEST_H */
