

#ifndef INCLUDED_KAPS_JR1_LSNMC_HW_H
#define INCLUDED_KAPS_JR1_LSNMC_HW_H

#include "kaps_jr1_fib_lsnmc.h"

#include "kaps_jr1_externcstart.h"

NlmErrNum_t kaps_jr1_lsn_mc_write_entry_to_hw(
    kaps_jr1_lsn_mc * self,
    uint16_t lpuGran,
    kaps_jr1_pfx_bundle * pfx,
    uint32_t oldix,
    uint32_t newix,
    uint32_t is_ad_update,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_jr1_lsn_mc_delete_entry_in_hw(
    kaps_jr1_lsn_mc * self,
    int ix,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_jr1_lsn_mc_commit(
    kaps_jr1_lsn_mc * self,
    int32_t isSbcPossible,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_jr1_lsn_mc_commit_brick(
    kaps_jr1_lsn_mc * self,
    int32_t brick_index,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_jr1_lsn_mc_clear_lpu_for_grow(
    kaps_jr1_lsn_mc * self,
    int32_t isGrowUp,
    uint32_t numLpusToClear,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_jr1_lsn_mc_update_iit(
    kaps_jr1_lsn_mc * self,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_jr1_lsn_mc_commitIIT(
    kaps_jr1_lsn_mc * self,
    uint32_t index,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_jr1_lsn_mc_get_hit_bits_for_lsn_from_hw(
    kaps_jr1_lsn_mc * curLsn);

void kaps_jr1_lsn_mc_prepare_mlp_data(
    uint8_t * pfxData,
    uint16_t pfxLen,
    uint16_t lopoffLen,
    uint16_t gran,
    uint8_t * o_data);

NlmErrNum_t kaps_jr1_format_map_get_gran(
    kaps_jr1_lsn_mc_settings * settings,
    struct kaps_jr1_ads *kaps_jr1_it,
    uint32_t cur_lpu_nr,
    uint32_t * brick_gran_ix);

uint8_t *kaps_jr1_get_bkt_data(
    kaps_jr1_lsn_mc * self,
    struct kaps_jr1_device *device,
    uint32_t bkt_nr,
    uint32_t row_nr);

uint32_t kaps_jr1_get_old_pfx_hb_index(
    kaps_jr1_lsn_mc * curLsn,
    uint32_t oldPfxIndex);

kaps_jr1_status kaps_jr1_fib_update_it_callback(
    struct uda_mem_chunk *uda_chunk);

kaps_jr1_status kaps_jr1_fib_uda_sub_block_copy_callback(
    struct uda_mem_chunk *uda_chunk,
    int32_t to_region_id,
    int32_t to_offset);

void kaps_jr1_fib_update_lsn_size(
    struct kaps_jr1_db *db);

uint32_t
kaps_jr1_get_daisy_chain_id(
    struct kaps_jr1_device *device,
    struct kaps_jr1_lsn_mc *self,
    struct uda_mem_chunk *uda_chunk,
    uint32_t brickNum);


#include <kaps_jr1_externcend.h>

#endif
