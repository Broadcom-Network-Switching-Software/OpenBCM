/*! \file bcmtm_imm_mc_port_agg_list.c
 *
 * BCMTM MC Port Aggregate List in-memory table.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <shr/shr_crc.h>
#include <shr/shr_ha.h>
#include <bcmbd/bcmbd.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/chip/bcmltd_tm_constants.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/multicast/bcmtm_mc_group.h>
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_ha_internal.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmissu/issu_api.h>

/*******************************************************************************
 * Local definitions
 */
#define REPL_LIST_RESERVED 0
#define REPL_CHAIN_BLOCK_SIZE 2048

#define BSL_LOG_MODULE  BSL_LS_BCMTM_IMM

static bcmtm_repl_list_chain_t *bcmtm_repl_list[BCMDRD_CONFIG_MAX_UNITS];
/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Getting multicast next hop. Nhop_id should be an array of 64.
 *
 * \param [in] unit Unit number.
 * \param [in] repl_list_entry Replication list.
 * \param [in] nhop_id array of next hop ID.
 * \param [in] nhop_id_count Number of elements in nhop_id array.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcmtm_mc_nh_get(int unit,
                bcmtm_mc_repl_list_entry_t *repl_list_entry,
                int *nhop_id, int *nhop_id_count)
{
    int iter, count = 0;
    bcmtm_mc_dev_info_t *mc_dev;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmtm_mc_dev_info_get(unit, &mc_dev));

    SHR_NULL_CHECK(repl_list_entry, SHR_E_PARAM);
    SHR_NULL_CHECK(nhop_id, SHR_E_PARAM);
    SHR_NULL_CHECK(nhop_id_count, SHR_E_PARAM);

    if (repl_list_entry->sparse_mode) {
        for (iter = 0; iter < mc_dev->nhops_per_repl_index; iter++) {
            if (repl_list_entry->mode_bitmap & (1 << iter)) {
                nhop_id[count] = repl_list_entry->nhop[iter];
                count++;
            }
        }
    } else {
        SHR_BIT_ITER(repl_list_entry->nhop, 64, iter) {
            nhop_id[count] = repl_list_entry->msb * 64 + iter;
            count++;
        }
    }
    *nhop_id_count = count;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Link the replication lists.
 *
 * If head is null then it creates new list with the elem and if the head is
 * not null elem points to the next of the existing list.
 *
 * \param [in] unit Unit number.
 * \param [in] head Head of the link list.
 * \param [in] elem New replication list to be added to head.
 */
static void
bcmtm_repl_chain_link(int unit,
                      bcmtm_repl_list_chain_id_t *head,
                      bcmtm_repl_list_chain_id_t elem)
{
    bcmtm_repl_list_chain_id_t t, h;
    bcmtm_repl_list_chain_t *mc_chain_ptr;
    bcmtm_repl_list_chain_t *repl_chain_block;
    h = *head;

    repl_chain_block = bcmtm_repl_list[unit];
    if (h) {

        mc_chain_ptr = repl_chain_block + (uint32_t)h;
        t = mc_chain_ptr->prev;
        mc_chain_ptr->prev = elem;

        mc_chain_ptr = repl_chain_block + (uint32_t)t;
        mc_chain_ptr->next = elem;

        mc_chain_ptr = repl_chain_block + (uint32_t)elem;
        mc_chain_ptr->next = h;
        mc_chain_ptr->prev = t;
    } else {
        *head = elem;
        mc_chain_ptr = repl_chain_block + (uint32_t)elem;
        mc_chain_ptr->next = mc_chain_ptr->prev = elem;
    }
}

/*!
 * \brief Replication chain unlink
 *
 * It unlinks the replication list chain id form the replication chain.
 *
 * \param [in] unit Unit number.
 * \param [in] chain Chain to be unlinked.
 */
static void
bcmtm_repl_chain_unlink(int unit,
                        bcmtm_repl_list_chain_id_t *chain)
{
    bcmtm_repl_list_chain_id_t p, n, c;
    bcmtm_repl_list_chain_t *mc_chain_ptr;
    bcmtm_repl_list_chain_t *repl_chain_block;

    repl_chain_block = bcmtm_repl_list[unit];
    c = *chain;
    n = 0;
    if (c) {
        mc_chain_ptr = repl_chain_block + (uint32_t)c;
        p = mc_chain_ptr->prev;
        n = mc_chain_ptr->next;

        mc_chain_ptr->prev = mc_chain_ptr->next = 0;
        if (n == c) {
            *chain = 0;
            return;
        }
        mc_chain_ptr = repl_chain_block + (uint32_t)p;
        mc_chain_ptr->next = n;

        mc_chain_ptr = repl_chain_block + (uint32_t)n;
        mc_chain_ptr->prev = p;
    }
    *chain = n;
}

/*!
 * \brief Get multicast replication list.
 *
 * \param [in] unit Unit number.
 * \param [in] head_chain Replication list head.
 * \param [in] max_nhop Maximum number of next hops.
 * \param [in] mc_nhop Multicast next hop bitmap.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcmtm_mc_repl_list_get(int unit,
                       bcmtm_repl_list_chain_t *head_chain,
                       int max_nhop,
                       SHR_BITDCL *mc_nhop)
{
    bcmtm_mc_repl_list_entry_t repl_list_info;
    int i, start_ptr, nhop_id[64], nhop_count;
    bcmtm_drv_t *drv;
    bcmtm_mc_drv_t mc_drv;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mc_nhop, SHR_E_PARAM);
    SHR_NULL_CHECK(head_chain, SHR_E_NOT_FOUND);

    sal_memset(&mc_drv, 0, sizeof(bcmtm_mc_drv_t));
    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->mc_drv_get, SHR_E_UNAVAIL);
    SHR_IF_ERR_EXIT(drv->mc_drv_get(unit, &mc_drv));

    start_ptr = head_chain->start_ptr;
    if (start_ptr == REPL_LIST_RESERVED) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    while (1) {
        sal_memset(&repl_list_info, 0, sizeof(bcmtm_mc_repl_list_entry_t));
        repl_list_info.entry_ptr = start_ptr;
        SHR_IF_ERR_VERBOSE_EXIT
            (mc_drv.mc_repl_list_entry_get(unit, &repl_list_info));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmtm_mc_nh_get(unit, &repl_list_info, nhop_id, &nhop_count));
        for (i = 0; i < nhop_count; i++) {
            SHR_BITSET(mc_nhop, nhop_id[i]);
        }

        if (repl_list_info.next_ptr == repl_list_info.entry_ptr) {
            break;
        }
        start_ptr = repl_list_info.next_ptr;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Replication list CRC32 hash compute.
 *
 * \param [in] unit Unit number.
 * \param [in] max_count Next hop max count.
 * \param [in] mc_nhop Bitmap for multicast next hop.
 * \param [out] hash CRC32 hash.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM hash or mc_nhop is NULL.
 */
static int
bcmtm_mc_repl_list_hash_compute(int unit,
                                int max_count,
                                SHR_BITDCL *mc_nhop,
                                uint32_t *hash)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(hash, SHR_E_PARAM);
    SHR_NULL_CHECK(mc_nhop, SHR_E_PARAM);
    *hash = shr_crc32(0, (uint8_t *)mc_nhop, max_count / 8);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the multicast replication list.
 *
 * It takes the head chain and the uses mc_nhop to get the replication list.
 *
 * \param [in] unit Unit number.
 * \param [in] head_chain Head chain for the replication list.
 * \param [in] mc_nhop Multicast nhop bitmap.
 * \param [in] max_count Maximum number of next hops
 * \param [out] repl_chain Replication list chain.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcmtm_mc_repl_list_find(int unit,
                        bcmtm_repl_list_chain_id_t head_chain,
                        SHR_BITDCL *mc_nhop, int max_count,
                        bcmtm_repl_list_chain_id_t *repl_chain)
{
    uint32_t hash;
    int rv, size = 0;
    bcmtm_repl_list_chain_id_t next_chain_id;
    bcmtm_repl_list_chain_t *next_chain;
    SHR_BITDCL *mc_nhop_get = NULL;
    bcmtm_mc_dev_info_t *mc_dev;
    bcmtm_repl_list_chain_t *repl_chain_block;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmtm_mc_dev_info_get(unit, &mc_dev));

    SHR_NULL_CHECK(repl_chain, SHR_E_PARAM);

    size = sizeof(SHR_BITDCL) * SHRi_BITDCLSIZE(mc_dev->max_mc_nhop);
    mc_nhop_get = sal_alloc(size, "bcmtmMulticastNhopGet");

    SHR_NULL_CHECK(mc_nhop_get, SHR_E_MEMORY);
    sal_memset(mc_nhop_get, 0, size);
    if (head_chain == 0) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmtm_mc_repl_list_hash_compute(unit, max_count,
                                            mc_nhop, &hash));
    next_chain_id = 0;
    while (next_chain_id != head_chain) {
        repl_chain_block = bcmtm_repl_list[unit];
        if (next_chain_id == 0) {
            next_chain_id = head_chain;
        }
        next_chain = repl_chain_block + (uint32_t)next_chain_id;
        if (next_chain->hash == hash) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmtm_mc_repl_list_get(unit, next_chain, max_count,
                                    mc_nhop_get));
            rv = sal_memcmp(mc_nhop, mc_nhop_get, SHRi_BITDCLSIZE(max_count));
            if (rv != 0) {
                next_chain_id = next_chain->next;
                continue;
            }
            *repl_chain = next_chain_id;
            SHR_EXIT();
        }
        next_chain_id = next_chain->next;
    }
    SHR_ERR_EXIT(SHR_E_NOT_FOUND);
exit:
    if (mc_nhop_get) {
        sal_free(mc_nhop_get);
    }
    SHR_FUNC_EXIT();
}

static int
bcmtm_mc_bit_pos_get(int unit, uint32_t ls_bits[2], uint32_t *ls_bit_pos)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ls_bit_pos, SHR_E_PARAM);
    *ls_bit_pos = -1;
    if (ls_bits[0] != 0) {
        SHR_BIT_ITER(&ls_bits[0], 32, *ls_bit_pos) {
            break;
        }
    }
    else if (ls_bits[1] != 0) {
        SHR_BIT_ITER(&ls_bits[1], 32, *ls_bit_pos) {
            break;
        }
        *ls_bit_pos += 32;
    } else {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmtm_mc_repl_chain_in_use(int unit,
                           bcmtm_repl_list_chain_t *recycle_chain,
                           bool *in_use)
{
    bcmtm_mc_repl_list_entry_t repl_list_info;
    bcmtm_drv_t *drv;
    bcmtm_mc_drv_t mc_drv;

    SHR_FUNC_ENTER(unit);
    sal_memset(&mc_drv, 0, sizeof(bcmtm_mc_drv_t));

    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->mc_drv_get, SHR_E_UNAVAIL);
    SHR_IF_ERR_EXIT(drv->mc_drv_get(unit, &mc_drv));

    sal_memset(&repl_list_info, 0, sizeof(bcmtm_mc_repl_list_entry_t));
    repl_list_info.entry_ptr = recycle_chain->start_ptr;
    *in_use = FALSE;
    while (1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mc_drv.mc_repl_list_in_use_get(unit,
                                repl_list_info.entry_ptr, in_use));
        if (*in_use) {
            break;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (mc_drv.mc_repl_list_entry_get(unit, &repl_list_info));

        if (repl_list_info.entry_ptr == repl_list_info.next_ptr) {
            break;
        }
        repl_list_info.entry_ptr = repl_list_info.next_ptr;
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmtm_mc_repl_list_entry_free(int unit, int repl_entry_ptr)
{
    bcmtm_mc_dev_info_t *mc_dev;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmtm_mc_dev_info_get(unit, &mc_dev));

    if (!(SHR_BITGET(mc_dev->used_list_bmp, repl_entry_ptr))) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_BITCLR(mc_dev->used_list_bmp, repl_entry_ptr);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief MC Port list IMM cleanup function.
 *
 * \param [in] unit Unit number.
 * \param [in] chain_id Replication list chain ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Memory allocation failed.
 */
static int
bcmtm_imm_mc_repl_chain_free(int unit,
                             bcmtm_repl_list_chain_id_t chain_id)
{
    bcmtm_mc_dev_info_t *mc_dev;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_mc_dev_info_get(unit, &mc_dev));

    bcmtm_repl_chain_link(unit, &(mc_dev->free_list_chain), chain_id);
exit:
    SHR_FUNC_EXIT();
}


static int
bcmtm_mc_repl_list_recycle(int unit, int *entry_ptr)
{
    bcmtm_repl_list_chain_id_t recycle_chain_id, chain_id;
    bcmtm_repl_list_chain_t *recycle_chain;
    bool in_use, found = FALSE;
    bcmtm_mc_repl_list_entry_t repl_list_info;
    bcmtm_mc_dev_info_t *mc_dev;
    bcmtm_drv_t *drv;
    bcmtm_mc_drv_t mc_drv;
    uint32_t recycle_list_chain;
    bcmtm_repl_list_chain_t *repl_chain_block;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_mc_dev_info_get(unit, &mc_dev));

    sal_memset(&mc_drv, 0, sizeof(bcmtm_mc_drv_t));
    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->mc_drv_get, SHR_E_UNAVAIL);
    SHR_IF_ERR_EXIT(drv->mc_drv_get(unit, &mc_drv));

    SHR_NULL_CHECK(entry_ptr, SHR_E_PARAM);
    recycle_list_chain = mc_dev->recycle_list_chain;

    if (recycle_list_chain == 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    recycle_chain_id = 0;
    while (recycle_chain_id != recycle_list_chain) {
        repl_chain_block = bcmtm_repl_list[unit];
        if (recycle_chain_id == 0) {
            recycle_chain_id = recycle_list_chain;
        }
        recycle_chain = repl_chain_block +(uint32_t)recycle_chain_id;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmtm_mc_repl_chain_in_use(unit, recycle_chain, &in_use));
        if ((recycle_chain->start_ptr != REPL_LIST_RESERVED) &&
            !in_use) {
            chain_id = recycle_chain_id;
            bcmtm_repl_chain_unlink(unit, &chain_id);
            if (recycle_chain_id == recycle_list_chain) {
                mc_dev->recycle_list_chain = recycle_list_chain = chain_id;
            }

            sal_memset(&repl_list_info, 0, sizeof(bcmtm_mc_repl_list_entry_t));

            repl_list_info.entry_ptr = recycle_chain->start_ptr;
            while (1) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (mc_drv.mc_repl_list_entry_get(unit, &repl_list_info));

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmtm_mc_repl_list_entry_free(unit,
                                repl_list_info.entry_ptr));
                if (repl_list_info.entry_ptr == repl_list_info.next_ptr) {
                    found = TRUE;
                    break;
                }
                repl_list_info.entry_ptr = repl_list_info.next_ptr;
            }
            *entry_ptr = recycle_chain->start_ptr;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmtm_imm_mc_repl_chain_free(unit, recycle_chain_id));
            (mc_dev->recycle_list_size)--;
            break;
        }
        recycle_chain = repl_chain_block +(uint32_t)recycle_chain_id;
        recycle_chain_id = recycle_chain->next;
    }
    if (!found) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmtm_mc_repl_list_entry_alloc(int unit, int *repl_entry_ptr)
{
#define MC_MAX_RECYCLE_POOL_SIZE 18
    int rv, iter, recycle_list_size;
    bcmtm_mc_dev_info_t *mc_dev;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_mc_dev_info_get(unit, &mc_dev));

    recycle_list_size = mc_dev->recycle_list_size;
    if (recycle_list_size > MC_MAX_RECYCLE_POOL_SIZE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmtm_mc_repl_list_recycle(unit, repl_entry_ptr));
        SHR_BITSET(mc_dev->used_list_bmp, *repl_entry_ptr);
        SHR_EXIT();
    }
    SHR_BIT_ITER_UNSET(mc_dev->used_list_bmp, mc_dev->max_repl_head, iter) {
        if (iter == REPL_LIST_RESERVED) {
            continue;
        }
        *repl_entry_ptr = iter;
        SHR_BITSET(mc_dev->used_list_bmp, *repl_entry_ptr);
        SHR_EXIT();
    }

    if (recycle_list_size > 0) {
        rv = bcmtm_mc_repl_list_recycle(unit, repl_entry_ptr);
        if (SHR_FAILURE(rv)) {
            *repl_entry_ptr = -1;
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        } else {
            SHR_BITSET(mc_dev->used_list_bmp, *repl_entry_ptr);
            SHR_EXIT();
        }
    }
    *repl_entry_ptr = -1;
    SHR_ERR_EXIT(SHR_E_RESOURCE);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Free the multicast replication list chain.
 *
 * \param [in] unit Logical device id.
 * \param [in] start_ptr Replication list start pointer.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcmtm_mc_free_nhop_chain(int unit, int start_ptr)
{
    bcmtm_drv_t *drv;
    bcmtm_mc_drv_t mc_drv;
    bcmtm_mc_repl_list_entry_t repl_list_info;

    SHR_FUNC_ENTER(unit);
    sal_memset(&mc_drv, 0, sizeof(bcmtm_mc_drv_t));
    sal_memset(&repl_list_info, 0, sizeof(bcmtm_mc_repl_list_entry_t));

    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->mc_drv_get, SHR_E_UNAVAIL);
    SHR_IF_ERR_EXIT(drv->mc_drv_get(unit, &mc_drv));

    repl_list_info.entry_ptr = start_ptr;
    while (1) {
           SHR_IF_ERR_VERBOSE_EXIT
              (mc_drv.mc_repl_list_entry_get(unit, &repl_list_info));

           SHR_IF_ERR_VERBOSE_EXIT
                (bcmtm_mc_repl_list_entry_free(unit,
                            repl_list_info.entry_ptr));
           if (repl_list_info.next_ptr == repl_list_info.entry_ptr ) {
               break;
           } else {
               repl_list_info.entry_ptr = repl_list_info.next_ptr;
           }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief  Update replication list entry in the physical table.
 *
 * \param [in] unit Logical device id.
 * \param [in] max_count Maximum number of multicast next hop id.
 * \param [in] mc_nhop Bitmap for multicast next hops.
 * \param [in] remaining_count Remaining count.
 * \param [out] start_ptr Start replication list pointer.
 * \param [out] num_list_used Number of list in use for the replication chain.
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcmtm_mc_repl_list_write_entries(int unit, int max_count,
                        SHR_BITDCL *mc_nhop, int remaining_count,
                        int *start_ptr, int *num_list_used)
{
    int i, msb, msb_max, rv;
    int repl_entry_ptr = -1, prev_repl_entry_ptr;
    bool no_more_free_repl_entries  = FALSE;
    bcmtm_mc_repl_list_entry_t repl_list_entry;
    uint32_t ls_bits[2], mode_bitmap = 0, ls_bit_pos;
    int bits_count0, bits_count1;
    int nhop_array_count = 0;
    int nhop_id[64];
    bcmtm_mc_dev_info_t *mc_dev;
    bcmtm_drv_t *drv;
    bcmtm_mc_drv_t mc_drv;

    SHR_FUNC_ENTER(unit);

    sal_memset(&mc_drv, 0, sizeof(bcmtm_mc_drv_t));
    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->mc_drv_get, SHR_E_UNAVAIL);
    SHR_IF_ERR_EXIT(drv->mc_drv_get(unit, &mc_drv));

    *num_list_used = 0;

    SHR_IF_ERR_EXIT
        (bcmtm_mc_dev_info_get(unit, &mc_dev));
    prev_repl_entry_ptr = -1;

    /* (MAX nhop index/(32*2)) array iterations. */
    msb_max = (mc_dev->max_mc_nhop/ (2 * 32));

    for (msb = 0; msb < msb_max; msb++) {
        ls_bits[0] = mc_nhop[2 * msb + 0];
        ls_bits[1] = mc_nhop[2 * msb + 1];
        if ((ls_bits[0] != 0) || (ls_bits[1] != 0)) {
            rv = bcmtm_mc_repl_list_entry_alloc(unit,
                                        &repl_entry_ptr);
            if (rv == SHR_E_RESOURCE) {
                no_more_free_repl_entries = TRUE;
            } else if (SHR_FAILURE(rv)) {
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            } else {
                no_more_free_repl_entries = FALSE;
            }
            if (prev_repl_entry_ptr == -1) {
                if (no_more_free_repl_entries) {
                    SHR_ERR_EXIT(SHR_E_RESOURCE);
                }
                *start_ptr = repl_entry_ptr;
            } else {
                if (no_more_free_repl_entries) {
                    repl_list_entry.next_ptr = prev_repl_entry_ptr;
                } else {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmtm_mc_nh_get(unit, &repl_list_entry,
                                            nhop_id, &nhop_array_count));
                    if (nhop_array_count < mc_dev->nhops_per_repl_index) {
                        mode_bitmap = 0;
                        for (i = 0; i < nhop_array_count; i++) {
                            mode_bitmap |= (1U << i);
                            repl_list_entry.nhop[i] = nhop_id[i];
                        }
                        for (i = nhop_array_count;
                                i < mc_dev->nhops_per_repl_index; i++) {
                            rv = bcmtm_mc_bit_pos_get(unit, ls_bits,
                                                      &ls_bit_pos);
                            if (rv == SHR_E_NOT_FOUND) {
                                break;
                            }
                            repl_list_entry.nhop[i] = msb * 64 + ls_bit_pos;
                            mode_bitmap |= (1U << i);
                            ls_bits[ls_bit_pos / 32] &=
                                            ~(1U << (ls_bit_pos % 32));
                        }
                        repl_list_entry.sparse_mode = TRUE;
                        repl_list_entry.mode_bitmap = mode_bitmap;
                        remaining_count -= (i - nhop_array_count);
                        repl_list_entry.remaining_reps = remaining_count;
                        if (remaining_count > 31) {
                            repl_list_entry.remaining_reps = 0;
                        } else if (remaining_count <= 0) {
                            break;
                        }
                        if (i < mc_dev->nhops_per_repl_index) {
                            bcmtm_mc_repl_list_entry_free(unit,
                                                repl_entry_ptr);
                            repl_entry_ptr = -1;
                            continue;
                        }
                    }
                    repl_list_entry.next_ptr = repl_entry_ptr;
                }
                (*num_list_used)++;
                SHR_IF_ERR_EXIT(mc_drv.mc_repl_list_entry_set
                                        (unit, &repl_list_entry));
                if (no_more_free_repl_entries) {
                    SHR_ERR_EXIT(SHR_E_RESOURCE);
                }
            }
            prev_repl_entry_ptr = repl_entry_ptr;
            sal_memset(&repl_list_entry, 0, sizeof(bcmtm_mc_repl_list_entry_t));
            SHR_BITCOUNT_RANGE(&ls_bits[0], bits_count0, 0, 32);
            SHR_BITCOUNT_RANGE(&ls_bits[1], bits_count1, 0, 32);
            repl_list_entry.sparse_mode = FALSE;
            repl_list_entry.entry_ptr = repl_entry_ptr;
            repl_list_entry.nhop[0] = ls_bits[0];
            repl_list_entry.nhop[1] = ls_bits[1];
            repl_list_entry.msb = msb;

            remaining_count -= (bits_count0 + bits_count1);
            repl_entry_ptr = -1;
            if (remaining_count > 31) {
                repl_list_entry.remaining_reps = 0;
            } else if (remaining_count > 0) {
                repl_list_entry.remaining_reps = remaining_count;
            } else {
                break;
            }
        }
    }
    if (prev_repl_entry_ptr != -1) {
        repl_list_entry.next_ptr = repl_list_entry.entry_ptr;
        SHR_IF_ERR_EXIT
            (mc_drv.mc_repl_list_entry_set(unit, &repl_list_entry));

        (*num_list_used)++;
    }

exit:
    if (repl_entry_ptr != -1 || no_more_free_repl_entries ) {
           bcmtm_mc_free_nhop_chain(unit, repl_entry_ptr);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief MC Port list IMM allocation.
 *
 * \param [in] unit Unit number.
 * \param [in] chain_id Replication list chain.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Memory allocation failed.
 */
static int
bcmtm_imm_mc_repl_chain_alloc(int unit,
                              bcmtm_repl_list_chain_id_t *chain_id)
{
    bcmtm_mc_dev_info_t *mc_dev;
    uint32_t req_size;
    uint32_t *free_list_chain;
    bcmtm_repl_list_chain_t *repl_chain_block = NULL;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(chain_id, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmtm_mc_dev_info_get(unit, &mc_dev));
    free_list_chain = &(mc_dev->free_list_chain);
    /*
     * 1. Allocate from unused entries in the repl chain block.
     * 2. If all entries are used, allocate from free list.
     * 3. Neither (1) nor (2).
     *
     * Since number of entries in repl chain block is same as
     *      REPL_HEAD, (3) should not occur.
     */
    if (mc_dev->repl_chain_initial_use_count  <= mc_dev->max_repl_head) {
        if (mc_dev->repl_chain_initial_use_count >=
                            mc_dev->repl_chain_alloc_entries) {
            mc_dev->repl_chain_alloc_entries += REPL_CHAIN_BLOCK_SIZE;
            req_size = mc_dev->repl_chain_alloc_entries;
            req_size *= sizeof(bcmtm_repl_list_chain_t);
            repl_chain_block = bcmtm_repl_list[unit];
            repl_chain_block = shr_ha_mem_realloc(unit,
                                            repl_chain_block,
                                            req_size);
            SHR_NULL_CHECK(repl_chain_block, SHR_E_MEMORY);
            bcmtm_repl_list[unit] = repl_chain_block;
        }
        *chain_id = mc_dev->repl_chain_initial_use_count;
        mc_dev->repl_chain_initial_use_count++;
    } else if (free_list_chain) {
        *chain_id = *free_list_chain;
        bcmtm_repl_chain_unlink(unit, free_list_chain);
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Multicast replication list.
 * Creates multiplication replication list if not available. If available
 * reuse the list and updates the reference count. This updated the ICC count
 * for the multicast replication head id.
 *
 * \param [in] unit Logical device id.
 * \param [in] mc_nhop Bitmap for multicast next hop.
 * \param [in] max_count Maximum number of elements in mc_nhop.
 * \param [in] repl_head_id Replication Head ID.
 * \param [out] prev_shared_head Previous head ID having same replication list.
 * \param [out] repl_count Replication count (ICC count).
 * \param [out] new_repl_chain Replication list ID.
 * \param [out] num_list_used Number of replication list used for the head.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcmtm_mc_repl_list_set(int unit,
                       SHR_BITDCL *mc_nhop,
                       int max_count,
                       int repl_head_id,
                       int *prev_shared_head,
                       int *repl_count,
                       bcmtm_repl_list_chain_id_t *new_repl_chain,
                       int *num_list_used)
{
    bcmtm_repl_list_chain_id_t active_chain, repl_chain_id = 0;
    bcmtm_repl_list_chain_t *chain;
    int rv, start_ptr = 0, remaining_count;
    uint32_t hash;
    bcmtm_mc_dev_info_t *mc_dev;
    bcmtm_repl_list_chain_t *repl_chain_block;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_mc_dev_info_get(unit, &mc_dev));
    SHR_NULL_CHECK(new_repl_chain, SHR_E_PARAM);
    active_chain = mc_dev->active_list_chain;

    SHR_BITCOUNT_RANGE(mc_nhop, remaining_count, 0, max_count);
    /* replication count more than 31 then nhop count needs to be 0. */
    if (remaining_count > 31) {
        *repl_count = 0;
    } else {
        *repl_count = remaining_count;
    }

    rv = bcmtm_mc_repl_list_find(unit, active_chain,
                        mc_nhop, max_count, new_repl_chain);
    /*
     * Search for existing replication chains (REPL_HEAD) for
     * exact list of NHOP entires.
     * If found, increment ref_count and return.
     *
     * Shared count updates:
     *    In case of add if the replication list mapped to the replication
     * head is exaclty same to an existing list then we just update the previous
     * head id to the new head id.
     */
    if ((rv == SHR_E_NONE) &&
        (*new_repl_chain != 0)) {
        repl_chain_block = bcmtm_repl_list[unit];
        chain = repl_chain_block + (uint32_t)(*new_repl_chain);

        /* prev_shared_head always points to the last head.*/
        *prev_shared_head = chain->prev_shared_head;
        /* updating the previous shared head id to newly added head id.*/
        chain->prev_shared_head = repl_head_id;

        *num_list_used = chain->num_list_used;
        chain->ref_count++;
        SHR_ERR_EXIT(SHR_E_EXISTS);
    } else if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmtm_mc_repl_list_write_entries(unit, max_count,
                        mc_nhop, remaining_count, &start_ptr, num_list_used));
    if (start_ptr == 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmtm_mc_repl_list_hash_compute(unit, max_count,
                                            mc_nhop, &hash));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmtm_imm_mc_repl_chain_alloc(unit, &repl_chain_id));
    repl_chain_block = bcmtm_repl_list[unit];
    chain = repl_chain_block + (uint32_t)repl_chain_id;
    sal_memset(chain, 0, sizeof(bcmtm_repl_list_chain_t));

    chain->start_ptr = start_ptr;
    chain->prev_shared_head = repl_head_id;
    chain->ref_count = 1;
    chain->hash = hash;
    chain->num_list_used = *num_list_used;

    /* Shared count updates:
     * This is first replication list added. No replication head is sharing this
     * replication list.
     */
    *prev_shared_head = -1;
    bcmtm_repl_chain_link(unit, &(mc_dev->active_list_chain), repl_chain_id);
    *new_repl_chain = repl_chain_id;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Updated IMM for TM_MC_PORT_AGG_LIST logical table.
 *
 * \param [in] unit Logical unit.
 * \param [in] lt_cfg Logical table configuration.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcmtm_mc_port_agg_list_imm_update (int unit,
                                   mc_port_list_cfg_t *lt_cfg)
{
    bcmltd_fields_t in;
    bcmltd_sid_t ltid = TM_MC_PORT_AGG_LISTt;
    bcmltd_fid_t fid;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 2, &in));

    in.count = 0;

    /* TM_MC_PORT_AGG_LISTt_TM_MC_PORT_AGG_LIST_IDf */
    fid = TM_MC_PORT_AGG_LISTt_TM_MC_PORT_AGG_LIST_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, lt_cfg->port_list_id));

    /* TM_MC_PORT_AGG_LISTt_SHARED_REPL_RESOURCEf */
    fid = TM_MC_PORT_AGG_LISTt_SHARED_REPL_RESOURCEf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, lt_cfg->shared));

    SHR_IF_ERR_EXIT
        (bcmimm_entry_update(unit, 0, ltid, &in));

exit:
    bcmtm_field_list_free(&in);
    SHR_FUNC_EXIT();
}

/*!
 * \brief  Delete the replication chain
 * This deleted the replication chain from the active list and
 * add to the recycle list.
 * This API should not be called for recycle list chains.
 *
 * \param [in] unit Logical device id.
 * \param [in] repl_chain_id Replication chain to be deleted.
 * \param [out] shared True if list is shared across head id.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcmtm_mc_repl_chain_delete (int unit,
                            bcmtm_repl_list_chain_id_t repl_chain_id,
                            int *shared)
{
    bcmtm_repl_list_chain_t *repl_chain;
    bcmtm_repl_list_chain_id_t chain_id;
    bcmtm_mc_dev_info_t *mc_dev;
    bcmtm_repl_list_chain_t *repl_chain_block;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_mc_dev_info_get(unit, &mc_dev));
    *shared = 1;

    if (repl_chain_id == 0) {
        SHR_EXIT();
    }
    repl_chain_block = bcmtm_repl_list[unit];

    repl_chain = repl_chain_block + (uint32_t)repl_chain_id;
    if (repl_chain->ref_count == 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    repl_chain->ref_count--;
    if (repl_chain->ref_count != 0) {
        if (repl_chain->ref_count == 1) {
            *shared = 0;
        }
        SHR_EXIT();
    }
    chain_id = repl_chain_id;
    *shared = 0;
    bcmtm_repl_chain_unlink(unit, &chain_id);
    if (mc_dev->active_list_chain == repl_chain_id) {
        mc_dev->active_list_chain = chain_id;
    }

    bcmtm_repl_chain_link(unit, &(mc_dev->recycle_list_chain),
                      repl_chain_id);
    (mc_dev->recycle_list_size)++;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Multicast replication head shared list update
 * This function removes the repl_head from the shared list that it is
 * a part of.
 *
 * \param [in] unit Logical device id.
 * \param [in] repl_head_info Multicast replication head info pointer.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcmtm_mc_shared_repl_head_update (int unit,
                    bcmtm_mc_repl_head_info_t *repl_head_info)
{
    bcmtm_repl_list_chain_id_t repl_chain_id;
    bcmtm_repl_list_chain_t *repl_chain;
    bcmtm_mc_dev_info_t *mc_dev;
    int repl_head_id;
    int prev_shared_head = -1, next_shared_head = -1, old_shared_head = -1;
    mc_port_list_cfg_t prev_cfg = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_mc_dev_info_get(unit, &mc_dev));
    repl_head_id = repl_head_info->index;

    prev_shared_head = repl_head_info->prev_shared_head;
    next_shared_head = repl_head_info->next_shared_head;
    repl_chain_id = repl_head_info->repl_chain;
    repl_chain = (bcmtm_repl_list[unit]) + (uint32_t)repl_chain_id;

    /*
     * Remove the shared entry from the list.
     * When next and previous are equal it is then the only entry left.
     */
    if (next_shared_head != prev_shared_head) {

        /*
         * For first entry prev_shared_head is same as head id and last entry
         * has next_shared_head is same as head id.
         */
        if (next_shared_head == repl_head_id) {
            /* last entry in the list to be deleted. */
            mc_dev->head_info_list[prev_shared_head].next_shared_head =
                prev_shared_head;
            /*
             * Since repl_chain->prev_shared_head always points to the
             * last in the list, update it only for this case.
             */
            repl_chain->prev_shared_head = prev_shared_head;
            old_shared_head = prev_shared_head;
        } else if (prev_shared_head == repl_head_id) {
            /* first entry to be  deleted from the list. */
            mc_dev->head_info_list[next_shared_head].prev_shared_head =
                next_shared_head;
            old_shared_head = next_shared_head;
        } else {
            /* entries in between the list. */
            mc_dev->head_info_list[prev_shared_head].next_shared_head =
                next_shared_head;
            mc_dev->head_info_list[next_shared_head].prev_shared_head =
                prev_shared_head;
        }
    }

    /*
     * Update shared bit to 0 for the remaining head in the shared list.
     * The assumption is that the ref_count is already updated.
     */
    if ((old_shared_head != -1) &&
        (repl_chain->ref_count == 1)) {
        prev_cfg.shared = 0;
        prev_cfg.port_list_id = old_shared_head;

        if (prev_cfg.port_list_id != repl_head_id) {
            SHR_IF_ERR_EXIT
                (bcmtm_mc_port_agg_list_imm_update(unit, &prev_cfg));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Multicast replication information.
 * This creates replication list based on the nexthop information provided by
 * the application.
 *
 * \param [in] unit Logical device id.
 * \param [in] repl_head_id Multicast replication head table ID.
 * \param [in] entry_count Number of entries in the mc_nhop array.
 * \param [in] mc_nhop Array for multicast next hops.
 * \param [in] repl_count Replication count for the list.
 * \param [in] lt_cfg Logical table configuration for mc_replication.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcmtm_mc_repl_info_add (int unit,
                        int repl_head_id,
                        int entry_count,
                        int *mc_nhop,
                        int *repl_count,
                        mc_port_list_cfg_t *lt_cfg)
{
    bcmtm_mc_repl_head_info_t *repl_head_info;
    bcmtm_mc_repl_head_info_t old_repl_head_info;
    bcmtm_repl_list_chain_id_t old_repl_chain,
                               new_repl_chain = 0;
    SHR_BITDCL *nhop_id_bmp = NULL;
    int i, rv, prev_shared_head = -1, num_list_used = 0;
    uint32_t start_ptr, update = 0;
    int  size, max_mc_nhop, shared;
    bcmtm_mc_dev_info_t *mc_dev;
    bcmtm_repl_list_chain_t *repl_chain_block;
    mc_port_list_cfg_t lt_cfg_old;
    bcmtm_drv_t *drv;
    bcmtm_mc_drv_t mc_drv;

    SHR_FUNC_ENTER(unit);
    sal_memset(&mc_drv, 0, sizeof(bcmtm_mc_drv_t));

    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->mc_drv_get, SHR_E_UNAVAIL);
    SHR_IF_ERR_EXIT(drv->mc_drv_get(unit, &mc_drv));

    SHR_IF_ERR_EXIT
        (bcmtm_mc_dev_info_get(unit, &mc_dev));
    max_mc_nhop = mc_dev->max_mc_nhop;

    sal_memset(&lt_cfg_old, 0, sizeof(mc_port_list_cfg_t));
    size = sizeof(SHR_BITDCL) * SHRi_BITDCLSIZE(max_mc_nhop);
    repl_head_info = &(mc_dev->head_info_list[repl_head_id]);

    /* Shared count update:
     * Keeping a copy of the replication head info needed for update operations.
     * Where we share a list and the new list is different from the old one.
     */
    old_repl_head_info = *repl_head_info;
    nhop_id_bmp = sal_alloc(size, "bcmtmMulticastNhopBmp");
    SHR_NULL_CHECK(nhop_id_bmp, SHR_E_MEMORY);
    sal_memset(nhop_id_bmp, 0, size);

    for (i = 0; i < entry_count; i++) {
        if ((mc_nhop[i] != -1) &&
            (mc_nhop[i] < max_mc_nhop)) {
            SHR_BITSET(nhop_id_bmp, mc_nhop[i]);
        } else if (mc_nhop[i] == -1) {
            SHR_BITSET(nhop_id_bmp, 0);
        }
    }

    lt_cfg->shared = 0;
    /* creating new replication chain. */
    rv = bcmtm_mc_repl_list_set(unit, nhop_id_bmp, max_mc_nhop,
                                repl_head_id, &prev_shared_head, repl_count,
                                &new_repl_chain, &num_list_used);

    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_EXISTS);

    if (rv == SHR_E_EXISTS && prev_shared_head != -1) {

        /* Shared count updates:
         * Replication list is shared only if bcmtm_mc_repl_list_set returns
         * SHR_E_EXISTS.
         * Updating the old replication head id once new replication head id
         * is added with same replication list. For the first entry previous
         * shared head id is same as the replication head id.
         * update the old head entry with shared values as 1. (applicable when
         * list is shared for first time i.e. number of replication head sharing
         * the list is 2.
         *
         * Update the next_shared_head for the previous shared head.
         */
        if (prev_shared_head ==
                mc_dev->head_info_list[prev_shared_head].prev_shared_head) {
            /* updated shared_head as shared = 1. Will be updated first time. */
            lt_cfg_old.shared = 1;
            lt_cfg_old.port_list_id = prev_shared_head;
        }
        mc_dev->head_info_list[prev_shared_head].next_shared_head = repl_head_id;
        lt_cfg->shared = 1;
    }
    repl_chain_block = bcmtm_repl_list[unit];
    old_repl_chain = repl_head_info->repl_chain;
    repl_head_info->repl_chain = new_repl_chain;

    /* Shared count updates:
     * Update the previous_shared_head for the current replication head.
     *
     * For new replication list previous shared head id is same as the replication
     * head ID. For sharing one update it to the previous head id using the same
     * replication list.
     */
    if (prev_shared_head == -1) {
        repl_head_info->prev_shared_head = repl_head_id;
    } else {
        repl_head_info->prev_shared_head = prev_shared_head;
    }
    repl_head_info->next_shared_head = repl_head_id;
    lt_cfg->num_list_used = num_list_used;

    /* calling chip specific update function if defined. */
    if (new_repl_chain)
    {
        start_ptr = (repl_chain_block + (uint32_t)new_repl_chain)->start_ptr;

        if (mc_drv.mc_repl_list_update) {
            if (repl_head_info->mc_group_valid &&
                repl_head_info->port_agg_valid) {
                update = 1;
                mc_drv.mc_repl_list_update(unit, repl_head_info,
                                                start_ptr, *repl_count);
            }
        }
        mc_drv.mc_repl_head_entry_set(unit, repl_head_id, start_ptr);
        mc_drv.mc_repl_icc_set(unit, repl_head_id, *repl_count);

    }
    if ((old_repl_chain != 0) &&
        ((repl_chain_block + (uint32_t)old_repl_chain)->start_ptr !=
                         REPL_LIST_RESERVED)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmtm_mc_repl_chain_delete(unit, old_repl_chain, &shared));
    }

    /* Update the old shared head info only for UPDATE operation.
     * This case is equivalent to delete with update operation. */
    if (old_repl_head_info.port_agg_valid) {
        SHR_IF_ERR_EXIT
            (bcmtm_mc_shared_repl_head_update(unit, &old_repl_head_info));
    }


    /*! updating at the end once everything is verified. */
    if (lt_cfg_old.port_list_id) {
        SHR_IF_ERR_EXIT
            (bcmtm_mc_port_agg_list_imm_update(unit, &lt_cfg_old));
    }
exit:
    if (nhop_id_bmp) {
        sal_free(nhop_id_bmp);
    }
    if (update) {
        /* clean up sequence for update chip specific function call. */
        if (mc_drv.mc_repl_list_update_cleanup) {
            mc_drv.mc_repl_list_update_cleanup(unit);
            update = 0;
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create an entry in the TM_MC_PORT_AGG_LIST LT.
 *
 * \param [in] unit Logical device id.
 * \param [in] lt_cfg Logical table information to be deleted.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcmtm_mc_port_list_create (int unit, mc_port_list_cfg_t *lt_cfg)
{
    bcmtm_mc_repl_head_info_t *repl_head_info;
    bcmtm_mc_repl_group_info_t repl_group_info;
    bcmtm_mc_ipmc_info_t ipmc_info;
    int mc_group_id, repl_count = 0;
    bcmtm_mc_dev_info_t *mc_dev;
    int diff = 0;
    bcmtm_drv_t *drv;
    bcmtm_mc_drv_t mc_drv;

    SHR_FUNC_ENTER(unit);

    sal_memset(&mc_drv, 0, sizeof(bcmtm_mc_drv_t));

    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->mc_drv_get, SHR_E_UNAVAIL);
    SHR_IF_ERR_EXIT(drv->mc_drv_get(unit, &mc_drv));

    SHR_NULL_CHECK(lt_cfg, SHR_E_PARAM);
    if (lt_cfg->port_list_id == 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if ((lt_cfg->num_mc_nhop == -1) ||
        (lt_cfg->num_mc_nhop == 0)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmtm_mc_dev_info_get(unit, &mc_dev));
    repl_head_info = &(mc_dev->head_info_list[lt_cfg->port_list_id]);
    if (repl_head_info->mc_group_valid ) {
        diff =  lt_cfg->num_mc_nhop - repl_head_info->num_cfg_repl;
        if ((mc_dev->repl_count_mc_grp[repl_head_info->mc_group_id] + diff) >
            mc_dev->max_packet_replications) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmtm_mc_repl_info_add(unit, lt_cfg->port_list_id,
                                lt_cfg->num_mc_nhop, lt_cfg->mc_nhop,
                                &repl_count, lt_cfg));
    repl_head_info->index = lt_cfg->port_list_id;
    repl_head_info->repl_count = repl_count;
    repl_head_info->num_cfg_repl = lt_cfg->num_mc_nhop;
    if (lt_cfg->port_agg_id != -1) {
        repl_head_info->port_agg_id = lt_cfg->port_agg_id;
        repl_head_info->port_agg_valid = 1;
    }
    if (repl_head_info->mc_group_valid ) {
        mc_dev->repl_count_mc_grp[repl_head_info->mc_group_id] +=
              diff;
    }
    if (repl_head_info->mc_group_valid &&
        repl_head_info->port_agg_valid) {
        mc_group_id = repl_head_info->mc_group_id;
        mc_drv.mc_repl_group_entry_get(unit, mc_group_id, &repl_group_info);


        if (repl_group_info.base_ptr) {
            SHR_BITSET(repl_group_info.port_agg_bmp,
                        repl_head_info->port_agg_id);
            mc_drv.mc_repl_group_entry_set
                        (unit, mc_group_id, &repl_group_info);
        }
        if (repl_head_info->repl_count > 1) {
            mc_drv.mc_ipmc_get(unit, mc_group_id, &ipmc_info);
            ipmc_info.do_not_cut_through = 1;
            mc_drv.mc_ipmc_set(unit, mc_group_id, &ipmc_info);

        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an entry in the TM_MC_PORT_AGG_LIST LT.
 *
 * \param [in] unit Logical device id.
 * \param [in] lt_cfg Logical table information to be deleted.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcmtm_mc_port_list_delete (int unit, mc_port_list_cfg_t *lt_cfg)
{
    bcmtm_mc_repl_head_info_t *repl_head_info;
    bcmtm_mc_repl_group_info_t repl_group_info;
    int mc_group_id, repl_head_id, shared;
    bcmtm_mc_dev_info_t *mc_dev;
    bcmtm_repl_list_chain_id_t repl_chain_id;
    bcmtm_drv_t *drv;
    bcmtm_mc_drv_t mc_drv;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_cfg, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmtm_mc_dev_info_get(unit, &mc_dev));
    sal_memset(&mc_drv, 0, sizeof(bcmtm_mc_drv_t));

    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->mc_drv_get, SHR_E_UNAVAIL);
    SHR_IF_ERR_EXIT(drv->mc_drv_get(unit, &mc_drv));

    SHR_NULL_CHECK(mc_dev, SHR_E_INIT);
    repl_head_id = lt_cfg->port_list_id;
    repl_head_info = &(mc_dev->head_info_list[repl_head_id]);
    if (repl_head_info->mc_group_valid) {
        mc_group_id = repl_head_info->mc_group_id;
        mc_drv.mc_repl_group_entry_get(unit, mc_group_id, &repl_group_info);

        if (repl_group_info.base_ptr &&
            (repl_head_info->port_agg_valid)) {
            SHR_BITCLR(repl_group_info.port_agg_bmp,
                        repl_head_info->port_agg_id);
            mc_drv.mc_repl_group_entry_set(unit, mc_group_id, &repl_group_info);
        }
    }
    repl_head_info->port_agg_valid = 0;

    repl_chain_id = repl_head_info->repl_chain;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmtm_mc_repl_chain_delete(unit, repl_chain_id, &shared));

    /* Clear old shared dependencies before delete. */
    SHR_IF_ERR_EXIT
        (bcmtm_mc_shared_repl_head_update(unit, repl_head_info));

    repl_head_info->repl_chain = 0;
    if (repl_head_info->mc_group_valid) {
        mc_dev->repl_count_mc_grp[repl_head_info->mc_group_id] -=
             repl_head_info->num_cfg_repl;
    }
    repl_head_info->num_cfg_repl = 0;
    repl_head_info->repl_count = 0;


    mc_drv.mc_repl_head_entry_set(unit, repl_head_info->index, 0);
    mc_drv.mc_repl_icc_set(unit, repl_head_info->index, 0);

    repl_head_info->prev_shared_head = -1;
    repl_head_info->next_shared_head = -1;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get previous multicast port list configuration.
 *
 * \param [in] unit Logical device id.
 * \param [in] port_list_id Multicast port aggregate list id.
 * \param [out] ltcfg Old configuration for multicast port aggregate list id.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcmtm_mc_port_list_old_cfg_get(int unit, int port_list_id,
                               mc_port_list_cfg_t *ltcfg)
{
    bcmltd_fields_t in;
    bcmltd_fields_t out;
    bcmltd_field_t *data_field;
    size_t num_fid;
    uint64_t fval;
    uint32_t i, idx_count;
    int rv = 0;
    bcmlrd_fid_t fid = 0;
    bcmlrd_sid_t sid;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));
    SHR_NULL_CHECK(ltcfg, SHR_E_PARAM);

    sid = TM_MC_PORT_AGG_LISTt;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit, TM_MC_PORT_AGG_LISTt, &num_fid));
    fid = TM_MC_PORT_AGG_LISTt_AGG_LIST_MEMBERf;
    idx_count = bcmlrd_field_idx_count_get(unit, sid,
                                        fid);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &in));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmtm_field_list_alloc(unit, num_fid + idx_count - 1, &out));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmtm_default_entry_array_construct(unit,
                                sid, &in));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmtm_field_list_set(unit, &in,
                TM_MC_PORT_AGG_LISTt_TM_MC_PORT_AGG_LIST_IDf, 0, port_list_id ));
    rv = bcmimm_entry_lookup(unit, sid, &in, &out);
    if (rv == SHR_E_NOT_FOUND) {
        out.count = 0;
    } else {
        SHR_IF_ERR_EXIT(rv);

        fid = TM_MC_PORT_AGG_LISTt_NUM_AGG_LIST_MEMBERf;
        rv = bcmtm_field_list_get(unit, &out,
                    fid, 0, &fval);
        if (SHR_SUCCESS(rv)) {
            ltcfg->num_mc_nhop = (uint32_t)fval;
        }
        rv = bcmtm_field_list_get(unit, &out,
                    TM_MC_PORT_AGG_LISTt_TM_MC_PORT_AGG_IDf, 0, &fval);
        if (SHR_SUCCESS(rv)) {
            ltcfg->port_agg_id = (uint32_t)fval;
        }
    }
    for (i = 0; i < out.count; i++) {
        data_field = out.field[i];
        fid = TM_MC_PORT_AGG_LISTt_AGG_LIST_MEMBERf;
        if (data_field->id == fid) {
            ltcfg->mc_nhop[data_field->idx] = (int)data_field->data;
        }
    }
exit:
    bcmtm_field_list_free(&in);
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Parse the key and data fields
 *
 * \param [in] unit Unit number.
 * \param [in] key Array of Key fields
 * \param [in] Data Array of Data fields
 * \param [out] ltcfg Logical table configurations.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcmtm_mc_port_list_lt_fields_parse(int unit,
                       const bcmltd_field_t *key,
                       const bcmltd_field_t *data,
                       mc_port_list_cfg_t *ltcfg)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid, idx;
    uint64_t fval;
    int rv;
    bcmtm_mc_dev_info_t *mc_dev;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_mc_dev_info_get(unit, &mc_dev));

    ltcfg->num_list_used = 0;
    ltcfg->shared = 0;
    /* Parse key field */
    gen_field = key;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
        case TM_MC_PORT_AGG_LISTt_TM_MC_PORT_AGG_LIST_IDf:
            ltcfg->port_list_id = fval;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

    rv = bcmtm_mc_port_list_old_cfg_get(unit, ltcfg->port_list_id, ltcfg);

    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* Parse data field */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;
        idx = gen_field->idx;

        switch (fid) {
        case TM_MC_PORT_AGG_LISTt_TM_MC_PORT_AGG_IDf:
            ltcfg->port_agg_id = fval;
            break;
        case TM_MC_PORT_AGG_LISTt_NUM_AGG_LIST_MEMBERf:
            ltcfg->num_mc_nhop = fval;
            break;
        case TM_MC_PORT_AGG_LISTt_AGG_LIST_MEMBERf:
            ltcfg->mc_nhop[idx] = (int)fval;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        gen_field = gen_field->next;
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Multicast port list callback function for staging.
 *
 * Handle the MC port list creation/modification parameters
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] action This is the desired action for the entry.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL MC Group failure.
 */
static int
bcmtm_mc_port_list_stage_callback(int unit,
                             bcmltd_sid_t sid,
                             uint32_t trans_id,
                             bcmimm_entry_event_t event_reason,
                             const bcmltd_field_t *key,
                             const bcmltd_field_t *data,
                             void *context,
                             bcmltd_fields_t *output_fields)
{
    mc_port_list_cfg_t lt_cfg;
    bcmltd_fid_t fid;

    SHR_FUNC_ENTER(unit);
    sal_memset(&lt_cfg, -1, sizeof(mc_port_list_cfg_t));
    lt_cfg.mc_nhop = sal_alloc(sizeof(uint32_t) * MAX_AGG_LIST_MEMBER,
                                "bcmtmMulticastNhopConfig");
    SHR_NULL_CHECK(lt_cfg.mc_nhop, SHR_E_MEMORY);
    sal_memset(lt_cfg.mc_nhop, -1, sizeof(uint32_t) * MAX_AGG_LIST_MEMBER);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmtm_mc_port_list_lt_fields_parse (unit, key, data, &lt_cfg));

    switch (event_reason) {
    case BCMIMM_ENTRY_INSERT:
    case BCMIMM_ENTRY_UPDATE:
        SHR_NULL_CHECK(output_fields, SHR_E_INTERNAL);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmtm_mc_port_list_create(unit, &lt_cfg));
        output_fields->count = 0;
        fid = TM_MC_PORT_AGG_LISTt_NUM_REPL_RESOURCE_IN_USEf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, output_fields,
                                  fid, 0, lt_cfg.num_list_used));
        fid = TM_MC_PORT_AGG_LISTt_SHARED_REPL_RESOURCEf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, output_fields, fid, 0, lt_cfg.shared));
        break;
    case BCMIMM_ENTRY_DELETE:
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmtm_mc_port_list_delete(unit, &lt_cfg));
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    if (lt_cfg.mc_nhop != NULL) {
        sal_free(lt_cfg.mc_nhop);
    }
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int
bcmtm_imm_mc_port_list_register(int unit)
{
    const bcmlrd_map_t *map = NULL;
    int rv;
    /*!
     * \brief TM port list callback structure.
     *
     * This structure contains callback functions that will be conresponding
     * to TM_MC_PORT_AGG_LIST logical table entry commit stages.
 */
    static bcmimm_lt_cb_t bcmtm_mc_port_list_imm_callback = {
        /*! Staging function. */
        .stage = bcmtm_mc_port_list_stage_callback,
    };

    SHR_FUNC_ENTER(unit);

    /*
     * To register callback for TM_MC_PORT_AGG_LIST LT.
     */
    rv = bcmlrd_map_get(unit, TM_MC_PORT_AGG_LISTt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 TM_MC_PORT_AGG_LISTt,
                                 &bcmtm_mc_port_list_imm_callback,
                                 NULL));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_imm_mc_port_agg_list_init(int unit, bool warm)
{
    uint32_t data_sz, array_sz, alloc_sz;
    bcmtm_mc_dev_info_t *mc_dev;
    bcmtm_repl_list_chain_t *repl_chain_block = NULL;
    bcmtm_drv_t *drv;
    bcmtm_mc_drv_t mc_drv;
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, TM_MC_GROUPt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmtm_mc_dev_info_get(unit, &mc_dev));

    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->mc_drv_get, SHR_E_UNAVAIL);
    SHR_IF_ERR_EXIT(drv->mc_drv_get(unit, &mc_drv));

    SHR_IF_ERR_VERBOSE_EXIT
        (mc_drv.mc_nhop_index_count_get(unit, &mc_dev->max_mc_nhop));

    SHR_IF_ERR_VERBOSE_EXIT
        (mc_drv.mc_nhops_per_repl_index_sparse_mode_get
                                (unit, &mc_dev->nhops_per_repl_index));
    SHR_IF_ERR_VERBOSE_EXIT
        (mc_drv.mc_repl_head_index_count_get(unit, &mc_dev->max_repl_head));

    SHR_IF_ERR_VERBOSE_EXIT
        (mc_drv.mc_max_packet_replication_get
                                (unit, &mc_dev->max_packet_replications));
    array_sz = (mc_dev->max_repl_head > REPL_CHAIN_BLOCK_SIZE) ?
                                REPL_CHAIN_BLOCK_SIZE:
                                mc_dev->max_repl_head;
    if (!warm) {
        mc_dev->repl_chain_alloc_entries = array_sz;
    } else {
        array_sz = mc_dev->repl_chain_alloc_entries;
    }

    data_sz = sizeof(bcmtm_repl_list_chain_t);
    alloc_sz = data_sz * array_sz;
    repl_chain_block = shr_ha_mem_alloc(unit,
                                    BCMMGMT_TM_COMP_ID,
                                    BCMTM_HA_DEV_MC_REPL_LIST_INFO,
                                    "bcmtmMulticastReplListInfo",
                                    &alloc_sz);
    SHR_NULL_CHECK(repl_chain_block, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT
        ((alloc_sz < (data_sz * array_sz)) ? SHR_E_MEMORY : SHR_E_NONE);
    if (!warm) {
        sal_memset(repl_chain_block, 0, alloc_sz);
        /* Index 0 is reserved. */
        mc_dev->repl_chain_initial_use_count = 1;

        bcmissu_struct_info_report(unit,
                                   BCMMGMT_TM_COMP_ID,
                                   BCMTM_HA_DEV_MC_REPL_LIST_INFO, 0,
                                   data_sz,
                                   array_sz,
                                   BCMTM_REPL_LIST_CHAIN_T_ID);
    }
    bcmtm_repl_list[unit] = repl_chain_block;
exit:
    SHR_FUNC_EXIT();
}

