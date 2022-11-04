

#ifndef __KAPS_JR1_LSNMC_RECOVER_H
#define __KAPS_JR1_LSNMC_RECOVER_H

#include "kaps_jr1_fib_lsnmc.h"
#include "kaps_jr1_lpm_algo.h"

#include "kaps_jr1_externcstart.h"

NlmErrNum_t kaps_jr1_lsn_mc_recover_construct_pfx_bundle(
    kaps_jr1_lsn_mc_settings * pSettings,
    uint8_t * commonBits,
    uint32_t commonLength_1,
    uint8_t * suffix,
    int32_t suffixLength_1,
    uint32_t ix,
    kaps_jr1_pfx_bundle ** pfxBundle_pp,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_jr1_lsn_mc_recover_parse_pfx(
    uint8_t * brickData,
    uint32_t brickLength_1,
    uint32_t pfxStartPosInBrick,
    uint32_t gran,
    uint8_t * suffix,
    int32_t * suffixLength_1,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_jr1_lsn_mc_recover_rebuild_brick(
    kaps_jr1_lsn_mc * lsn_p,
    kaps_jr1_lpm_lpu_brick * curBrick,
    kaps_jr1_prefix * commonPfx,
    uint32_t curBrickIter,
    uint32_t curLpu,
    uint32_t curRow,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_jr1_lsn_mc_recover_rebuild_lsn(
    struct kaps_jr1_wb_lsn_info *lsn_info,
    kaps_jr1_lsn_mc * lsn_p,
    kaps_jr1_prefix * commonPfx,
    NlmReasonCode * o_reason);

#include <kaps_jr1_externcend.h>

#endif
