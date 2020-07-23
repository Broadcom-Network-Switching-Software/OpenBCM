/*! \file rm_alpm_ts.h
 *
 * Trouble shooting internal to RM ALPM
 *
 * This file contains trouble shooting which are internal to
 * ALPM Resource Manager
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef RM_ALPM_TS_H
#define RM_ALPM_TS_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <shr/shr_pb.h>
#include "rm_alpm.h"
#include "rm_alpm_leveln.h"

/*******************************************************************************
 * Defines
 */



/*******************************************************************************
 * Function prototypes
 */

/*!
 * \brief Dump Pivot info
 *
 * \param [in] u Device u.
 * \param [in] pb Print buffer.
 *
 * \return nothing.
 */
extern void
bcmptm_rm_alpm_l1_pivot_info_dump(int u, int m, uint8_t ldb, shr_pb_t *pb);

/*!
 * \brief Dump Pivot tries
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] w_vrf Weighted VRF
 * \param [in] ipv IP version
 * \param [in] pb Print buffer.
 *
 * \return nothing.
 */
extern void
bcmptm_rm_alpm_pivot_trie_dump(int u, int m, int db, int ln, int w_vrf,
                               int ipv, shr_pb_t *pb);

/*!
 * \brief Dump all groups following group list
 *
 * \param [in] u Device u
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] pb Print buffer
 * \param [in] verbose Verbose
 *
 * \return nothing.
 */
extern void
bcmptm_rm_alpm_buckets_dump(int u, int m, int db, int ln, shr_pb_t *pb, bool verbose);

/*!
 * \brief Dump all groups following group usage list
 *
 * \param [in] u Device u
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] pb Print buffer
 * \param [in] verbose Verbose
 *
 * \return nothing.
 */
extern void
bcmptm_rm_alpm_buckets_dump2(int u, int m, int db, int ln, shr_pb_t *pb, bool verbose);

/*!
 * \brief Dump a given group
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] group Rbkt group
 * \param [in] pb Print buffer.
 *
 * \return nothing.
 */
extern void
bcmptm_rm_alpm_bucket_dump(int u, int m, int db, int ln, rbkt_group_t group,
                               bool compact, shr_pb_t *pb);

/*!
 * \brief Dump bucket handle
 *
 * \param [in] u Device u.
 * \param [in] db Database
 * \param [in] ln Level
 * \param [in] pb Print buffer.
 *
 * \return nothing.
 */
extern void
bcmptm_rm_alpm_buckets_brief_dump(int u, int m, int db, int ln, shr_pb_t *pb, bool verbose);


/*!
 * \brief Dump level-1 info
 *
 * \param [in] u Device u.
 * \param [in] pb Print buffer.
 * \param [in] verbose Verbose.
 * \param [in] brief Brief message format.
 *
 * \return nothing.
 */
extern void
bcmptm_rm_alpm_l1_info_dump(int u, int m, int db, shr_pb_t *pb, int verbose, int brief);

/*!
 * \brief Dump level-n info
 *
 * \param [in] u Device u.
 * \param [in] pb Print buffer.
 * \param [in] verbose Verbose.
 * \param [in] brief Brief message format.
 *
 * \return nothing.
 */
extern void
bcmptm_rm_alpm_ln_info_dump(int u, int m, int db, shr_pb_t *pb, int verbose, int brief);


/*!
 * \brief Dump ALPM arg
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM arg
 * \param [in] compact Compact formart if true, else verbose format.
 * \param [in] pb Print buffer.
 *
 * \return nothing.
 */
extern void
bcmptm_rm_alpm_arg_dump(int u, int m, alpm_arg_t *arg, bool compact, shr_pb_t *pb);


extern void
bcmptm_rm_alpm_key_dump(key_tuple_t *t, shr_pb_t *pb);


/*!
 * \brief Dump PT entry
 *
 * \param [in] u Device u.
 * \param [in] sid Physical symbol id
 * \param [in] data Pt buffer
 *
 * \return nothing.
 */
extern void
bcmptm_rm_alpm_pt_dump(int u, int m, bcmdrd_sid_t sid, uint32_t *data);

/*!
 * \brief Dump PT entry
 *
 * \param [in] u Device u.
 * \param [in] name Physical symbol name
 * \param [in] data Pt buffer
 *
 * \return nothing.
 */
extern void
bcmptm_rm_alpm_pt_dump2(int u, int m, const char * name, uint32_t *data);

/*!
 * \brief Dump PT entry
 *
 * \param [in] u Device u.
 * \param [in] name Physical symbol name
 * \param [in] string Pt buffer string, in the way of 0xaa_00112233, msb -> lsb.
 *
 * \return nothing.
 */
extern void
bcmptm_rm_alpm_pt_dump3(int u, int m, const char * name, const char *string);

/*!
 * \brief Enable/disable dump to file
 *
 * \param [in] enable True if enable, otherwise disable.
 *
 * \return nothing.
 */
extern void
bcmptm_rm_alpm_dump_to_file_set(bool enable);

/*!
 * \brief Is dump to file enabled.
 *
 * \return Is enabled.
 */
extern bool
bcmptm_rm_alpm_dump_to_file_get(void);


#endif /* RM_ALPM_TS_H */
