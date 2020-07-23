/*! \file bcmtm_mc_group.h
 *
 * APIs, definitions for BCMTM MC Group.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMTM_MC_GROUP_H
#define BCMTM_MC_GROUP_H

#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <shr/shr_bitop.h>
#include <bcmltd/chip/bcmltd_tm_constants.h>
#include <bcmtm/generated/bcmtm_mc_ha.h>

/*****************************************************************************
    Defines
 */

/*! Maximum port aggregate bitmaps. */
#define MAX_PORT_AGG_BMP 272

/*! Maximum number of replication list across devices. */
#define MAX_REPL_LIST_COUNT 147456

/*! Maximum multicast next hops supported across device. */
#define MAX_MC_NHOP 32768


/*! Iterate to clear the bit range. */
#define SHR_BIT_ITER_UNSET(_a, _max, _b)            \
    for ((_b) = 0; (_b) < (_max); (_b)++)           \
        if ((_a)[(_b) / SHR_BITWID] == 0xffffffff)  \
            (_b) += (SHR_BITWID - 1);               \
        else if (!SHR_BITGET((_a), (_b)))
/*****************************************************************************
    Structures
 */

/*! Multicast group configuration.
 *
 * It saves the port list, number of port in port list, and port bitmap for
 * a given mutlicast group ID.
 */
typedef struct mc_group_cfg_s {
    /*! MC Group ID */
    int     mc_group_id;

    /*! MC port list ID */
    int     port_list_id;

    /*! Number of consecutive port list entries. */
    int     port_list_num_entries;

    /*! L3 Port bitmap */
    SHR_BITDCLNAME(l3_bitmap, BCMDRD_CONFIG_MAX_PORTS);

    /*! L2 Port bitmap */
    SHR_BITDCLNAME(l2_bitmap, BCMDRD_CONFIG_MAX_PORTS);

    /*! L2mc disable. */
    bool    l2_mc_disable;
} mc_group_cfg_t;

/*!
 * Multicast port list configutation.
 *
 * It saves infromation regarding the number of mc nhops, port aggregate ID,
 * infomation regarding replication list.
 */
typedef struct mc_port_list_cfg_s {
    /*! Port list ID */
    int      port_list_id;

    /*! Port Aggregate ID */
    int      port_agg_id;

    /*! Count */
    int      num_mc_nhop;

    /*! Multicast next hop indices configured */
    int      *mc_nhop;

    /*! Enables when replication list is shared across repl_head. */
    bool     shared;

    /*! Number of replication list used for a replication head. */
    int      num_list_used;
} mc_port_list_cfg_t;



/*!
 * Multicast replication group information.
 *
 * This contains base pointer for the replication head table, Number of entries
 * in the replication head table and if RQE flush is needed for the hardware.
 */
typedef struct {
    /*! Bitmap of Port Aggregate IDs */
    SHR_BITDCLNAME(port_agg_bmp, MAX_PORT_AGG_BMP);

    /*! Replication Head Base Ptr */
    int base_ptr;

    /*! Num entries in Head Tbl */
    int num_entries;

    /*! Issue RQE flush for the hardware. */
    bool rqe_flush;

} bcmtm_mc_repl_group_info_t;


/*! Multicast IPMC infromation table.
 *
 * This has information for a multicast group, if the group is valid, cut-through
 * support and bitmaps for L2 and L3 replications.
 */
typedef struct {
    /*! MC Group valid */
    bool valid;

    /*! Do not cut through */
    bool do_not_cut_through;

    /*! L3 Port bitmap */
    SHR_BITDCLNAME(l3_bitmap, BCMDRD_CONFIG_MAX_PORTS);

    /*! L2 Port bitmap */
    SHR_BITDCLNAME(l2_bitmap, BCMDRD_CONFIG_MAX_PORTS);

    /*! L2mc disable */
    bool l2_mc_disable;

} bcmtm_mc_ipmc_info_t;


/*! Replication list chain ID */
typedef uint32_t bcmtm_repl_list_chain_id_t;
/*****************************************************************************
    Function Pointers
 */
/*!
 * \brief Set multicast replication group entry.
 * This configures MMU_REPL_GROUP_INFO_TBLm for a given multicast index.
 *
 * \param [in] unit Logical device id.
 * \param [in] mc_group_id Multicast group ID.
 * \param [in] mc_group_info Mutlicast group configurations.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
typedef int
(*bcmtm_mc_repl_group_entry_set_t)(int unit,
                                   uint32_t mc_group_id,
                                   bcmtm_mc_repl_group_info_t *mc_group_info);

/*!
 * \brief Get multicast replication group entry.
 * This gets the configured MMU_REPL_GROUP_INFO_TBLm for a given multicast index.
 *
 * \param [in] unit Logical device id.
 * \param [in] mc_group_id Multicast group ID.
 * \param [out] mc_group_info Mutlicast group configurations.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
typedef int
(*bcmtm_mc_repl_group_entry_get_t)(int unit,
                                   uint32_t mc_group_id,
                                   bcmtm_mc_repl_group_info_t *mc_group_info);
/*!
 * \brief Set replication list entry to the multicast head ID.
 *
 * \param [in] unit Logical device id.
 * \param [in] mc_head_id Multicast head ID.
 * \param [in] start_ptr Pointer to the first replication list entry in
 * replication ID linked list.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
typedef int
(*bcmtm_mc_repl_head_entry_set_t)(int unit,
                                  uint32_t mc_head_id,
                                  uint32_t start_ptr);

/*!
 * \brief Get replication list.
 * This gives the start pointer to the replication list for a given
 * multicast head ID.
 *
 * \param [in] unit Logical device id.
 * \param [in] mc_head_id Multicast head ID.
 * \param [out] start_ptr Pointer to the first replication list entry in
 * replication ID linked list.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
typedef int
(*bcmtm_mc_repl_head_entry_get_t)(int unit,
                                  uint32_t mc_head_id,
                                  uint32_t *start_ptr);
/*!
 * \brief Get multicast replication list.
 *
 * \param [in] unit Logical device id.
 * \param [out] mc_list_entry Multicast replication list.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
typedef int
(*bcmtm_mc_repl_list_entry_get_t)(int unit,
                                  bcmtm_mc_repl_list_entry_t *mc_list_info);

/*!
 * \brief Set multicast replication list.
 * Programs both the ITMs for the replication list.
 *
 * \param [in] unit Logical device id.
 * \param [in] mc_list_entry Multicast replication list.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
typedef int
(*bcmtm_mc_repl_list_entry_set_t)(int unit,
                                   bcmtm_mc_repl_list_entry_t *mc_list_info);

/*!
 * Check for if the replication list is in use or free.
 *
 * \param [in] unit Logical devide ID.
 * \param [in] repl_entry_ptr Replication list.
 * \param [out] in_use Status of the replication list.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
typedef int
(*bcmtm_mc_repl_list_entry_inuse_get_t)(int unit,
                                        int repl_entry_ptr,
                                        bool *in_use);

/*!
 * \brief Mutlicast initial copy count.
 *
 * \param [in] unit Unit number.
 * \param [in] mc_head_id Multicast head ID.
 * \param [in] repl_count Replication count. This denotes number of next hop
 * for a given multicast head ID.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
typedef int
(*bcmtm_mc_repl_icc_set_t) (int unit,
                            int mc_head_id,
                            int repl_count);
/*!
 * \brief IP multicast replication config set.
 *
 * \param [in] unit Logical device id.
 * \param [in] mc_group_id Multicast group ID.
 * \param [in] ipmc_info IPMC config.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
typedef int
(*bcmtm_mc_ipmc_set_t) (int unit,
                        int mc_group_id,
                        bcmtm_mc_ipmc_info_t *ipmc_info);

/*!
 * \brief IP multicast replication config get.
 *
 * \param [in] unit Logical device id.
 * \param [in] mc_group_id Multicast group ID.
 * \param [out] ipmc_info IPMC config.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
typedef int
(*bcmtm_mc_ipmc_get_t) (int unit,
                        int mc_group_id,
                        bcmtm_mc_ipmc_info_t *ipmc_info);

/*!
 * \brief Get the number of replication head table count.
 * \param [in] unit Logical device id.
 * \param [out] num_index Number of entries.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM num_index is null.
 */
typedef int
(*bcmtm_mc_repl_head_index_count_get_t)(int unit, int *index_count);

/*!
 * \brief Get the maximum number of next hops.
 * \param [in] unit Logical device id.
 * \param [out] num_index Number of next hops.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM num_index is null.
 */
typedef int
(*bcmtm_mc_nhop_index_count_get_t)(int unit, int *index_count);

/*!
 * \brief Get the maximum number of packet replications.
 * \param [in] unit Logical device id.
 * \param [out] num_index Maximum number of replication for a multicast group ID.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM num_index is null.
 */
typedef int
(*bcmtm_mc_max_packet_replication_get_t)(int unit, int *index_count);

/*!
 * \brief Get the number of next hops per replication index in sparse mode.
 * \param [in] unit Logical device id.
 * \param [out] num_index Number of next hops.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM num_index is null.
 */
typedef int
(*bcmtm_mc_nhops_per_repl_index_sparse_mode_get_t)(int unit, int *index_count);

/*!
 * \brief Dynamic replication list update.
 * This function implements the sequence needed for the dynamic updation
 * of the replication list.
 *
 * \param [in] unit Logical device id.
 * \param [in] repl_head_info Replication head
 * \param [in] start_ptr New replication list.
 * \param [in] repl_count Replication count. This denotes icc count for the
 * replication list.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
typedef int
(*bcmtm_mc_repl_list_update) (int unit,
                              bcmtm_mc_repl_head_info_t *repl_head_info,
                              uint32_t start_ptr,
                              int repl_count);

/*!
 * \brief Dynamic replication list update cleanup.
 * This function implements the sequence for clean up once dynamic replication
 * list is updated.
 *
 * \param [in] unit Logical device id.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
typedef int
(*bcmtm_mc_repl_list_update_cleanup)(int unit);

/*! \brief Get the multicast replication list free count.
 *
 * \param [in] unit Logical unit number.
 * \param [out] free Number of free replication list.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM num_index is null.
 */
typedef int
(*bcmtm_mc_repl_list_free_count) (int unit, uint64_t *free_resource);


/*! BCMTM multicast driver function. */
typedef struct bcmtm_mc_drv_s {

    /*! Multicast replication group entry set. */
    bcmtm_mc_repl_group_entry_set_t mc_repl_group_entry_set;
    /*! Multicast replication group entry get. */
    bcmtm_mc_repl_group_entry_get_t mc_repl_group_entry_get;

    /*! Multicast replication head table entry set. */
    bcmtm_mc_repl_head_entry_set_t mc_repl_head_entry_set;
    /*! Multicast replication head table entry get. */
    bcmtm_mc_repl_head_entry_get_t mc_repl_head_entry_get;

    /*! Mutlicast replication list entry set. */
    bcmtm_mc_repl_list_entry_set_t mc_repl_list_entry_set;
    /*! Mutlicast replication list entry get. */
    bcmtm_mc_repl_list_entry_get_t mc_repl_list_entry_get;

    /*! Replication list entru use count get. */
    bcmtm_mc_repl_list_entry_inuse_get_t mc_repl_list_in_use_get;

    /*! Replication list Initial Copy Counet set. */
    bcmtm_mc_repl_icc_set_t mc_repl_icc_set;

    /*! Replication multicast group set. */
    bcmtm_mc_ipmc_set_t mc_ipmc_set;
    /*! Replication multicast group get. */
    bcmtm_mc_ipmc_get_t mc_ipmc_get;

    /*! Replication head Index count get. */
    bcmtm_mc_repl_head_index_count_get_t mc_repl_head_index_count_get;

    /*! Multicast next HOP Index count get. */
    bcmtm_mc_nhop_index_count_get_t mc_nhop_index_count_get;

    /*! Multicast next HOP Index count get sparse mode. */
    bcmtm_mc_nhops_per_repl_index_sparse_mode_get_t  mc_nhops_per_repl_index_sparse_mode_get;

    /*! Multicast maximum packet replication count get. */
    bcmtm_mc_max_packet_replication_get_t mc_max_packet_replication_get;

    /*! Multicast replication list update. */
    bcmtm_mc_repl_list_update mc_repl_list_update;
    /*! Multicast replication list clean up. */
    bcmtm_mc_repl_list_update_cleanup mc_repl_list_update_cleanup;

    /*! Multicast replication list free entries count. */
    bcmtm_mc_repl_list_free_count mc_repl_list_free_count;
} bcmtm_mc_drv_t;

#endif /* BCMTM_MC_GROUP_H */
