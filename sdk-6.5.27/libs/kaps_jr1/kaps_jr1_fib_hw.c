

#include "kaps_jr1_fib_hw.h"
#include "kaps_jr1_xpt.h"
#include "kaps_jr1_utility.h"
#include "kaps_jr1_ab.h"
#include "kaps_jr1_algo_hw.h"
#include "kaps_jr1_fib_lsnmc.h"
#include "kaps_jr1_fib_lsnmc_hw.h"
#include "kaps_jr1_fib_triedata.h"

void
kaps_jr1_write_first_half_of_fmap(
    uint8_t * write_buf,
    uint32_t buf_len_in_bytes,
    uint32_t cur_bit_pos,
    struct kaps_jr1_ads *write_ads)
{
    KapsJr1WriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_00);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_01);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_02);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_03);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_04);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_05);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_06);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_07);

}

void
kaps_jr1_write_second_half_of_fmap(
    uint8_t * write_buf,
    uint32_t buf_len_in_bytes,
    uint32_t cur_bit_pos,
    struct kaps_jr1_ads *write_ads)
{
    KapsJr1WriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_08);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_09);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_10);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_11);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_12);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_13);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_14);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_15);

}

static NlmErrNum_t
kaps_jr1_jericho_iit_write(
    kaps_jr1_fib_tbl_mgr * fibTblMgr,
    uint32_t dev_num,
    struct kaps_jr1_ab_info *start_ab,
    struct kaps_jr1_ads *write_ads,
    uint32_t it_addr,
    NlmReasonCode * o_reason)
{
    struct NlmFibStats *fibStats = &fibTblMgr->m_curFibTbl->m_fibStats;
    kaps_jr1_status status = KAPS_JR1_OK;
    uint8_t write_buf[16] = { 0 };
    struct kaps_jr1_device *device = fibTblMgr->m_devMgr_p;
    uint32_t cur_bit_pos, num_bytes;
    struct kaps_jr1_ab_info *cur_dup_ab;
    struct kaps_jr1_db *db = fibTblMgr->m_curFibTbl->m_db;

    num_bytes = device->hw_res->ads_width_8;

    KapsJr1WriteBitsInArray(write_buf, num_bytes, 19, 0, write_ads->bpm_ad);
    KapsJr1WriteBitsInArray(write_buf, num_bytes, 27, 20, write_ads->bpm_len);
    KapsJr1WriteBitsInArray(write_buf, num_bytes, 31, 28, write_ads->row_offset);

    cur_bit_pos = 32;

    kaps_jr1_write_first_half_of_fmap(write_buf, num_bytes, cur_bit_pos, write_ads);
    cur_bit_pos += 32;

    kaps_jr1_write_second_half_of_fmap(write_buf, num_bytes, cur_bit_pos, write_ads);
    cur_bit_pos += 32;

    if (device->id == KAPS_JR1_JERICHO_PLUS_DEVICE_ID)
    {
        KapsJr1WriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 11, cur_bit_pos, write_ads->bkt_row);
        cur_bit_pos += 14;
    }
    else if (device->id == KAPS_JR1_QUX_DEVICE_ID)
    {
        KapsJr1WriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 7, cur_bit_pos, write_ads->bkt_row);
        cur_bit_pos += 8;
    }
    else
    {
        KapsJr1WriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 9, cur_bit_pos, write_ads->bkt_row);
        cur_bit_pos += 11;
    }

    KapsJr1WriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 4, cur_bit_pos, write_ads->reserved);
    cur_bit_pos += 5;

    KapsJr1WriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 7, cur_bit_pos, write_ads->key_shift);
    cur_bit_pos += 8;

    if (device->id != KAPS_JR1_JERICHO_PLUS_DEVICE_ID)
    {
        KapsJr1WriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 7, cur_bit_pos, write_ads->ecc);
        cur_bit_pos += 8;
    }

    
    cur_dup_ab = start_ab;
    while (cur_dup_ab)
    {

        status = kaps_jr1_dm_iit_write(device, db, cur_dup_ab->ab_num, it_addr, num_bytes, write_buf);
        fibStats->numIITWrites++;

        if (status != KAPS_JR1_OK)
        {
            *o_reason = NLMRSC_XPT_FAILED;
            return NLMERR_FAIL;
        }

        cur_dup_ab = cur_dup_ab->dup_ab;
    }

    return NLMERR_OK;
}



NlmErrNum_t
kaps_jr1_iit_write(
    kaps_jr1_fib_tbl_mgr * fibTblMgr,
    uint32_t dev_num,
    struct kaps_jr1_ab_info * start_ab,
    struct kaps_jr1_ads * write_ads,
    uint32_t it_addr,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t errNum = NLMERR_OK;
    
    errNum = kaps_jr1_jericho_iit_write(fibTblMgr, dev_num, start_ab, write_ads, it_addr, o_reason);

    return errNum;
}




static NlmErrNum_t
kaps_jr1_write_to_tcam(
    kaps_jr1_fib_tbl_mgr * fibTblMgr,
    uint8_t devNum,
    uint16_t blkWidthInBits,
    struct kaps_jr1_ab_info *start_blk,
    uint32_t logicalLoc,
    uint8_t rptId,
    uint16_t startBit,
    uint16_t endBit,
    uint8_t * data,
    uint16_t dataLen,
    uint32_t byte_offset,
    NlmReasonCode * o_reason)
{
    kaps_jr1_status status = KAPS_JR1_OK;
    uint8_t ab_data[KAPS_JR1_RPB_WIDTH_8];
    uint8_t ab_mask[KAPS_JR1_RPB_WIDTH_8];
    uint32_t num_bytes;
    struct kaps_jr1_ab_info *cur_dup_ab;
    struct kaps_jr1_device *device = fibTblMgr->m_devMgr_p;

    kaps_jr1_sassert(blkWidthInBits == KAPS_JR1_RPB_WIDTH_1);

    num_bytes = (dataLen + 7) / 8;
    kaps_jr1_memcpy(ab_data, data, num_bytes);

    if (num_bytes < KAPS_JR1_RPB_WIDTH_8)
        kaps_jr1_memset(&ab_data[num_bytes], 0, KAPS_JR1_RPB_WIDTH_8 - num_bytes);

    kaps_jr1_memset(ab_mask, 0xFF, KAPS_JR1_RPB_WIDTH_8);
    if (dataLen > 0)
        kaps_jr1_FillZeroes(ab_mask, KAPS_JR1_RPB_WIDTH_8, startBit, startBit - dataLen + 1);

    if (device->kaps_jr1_shadow && device->nv_ptr)
    {
        device->kaps_jr1_shadow->rpb_blks[start_blk->ab_num].rpb_rows[logicalLoc].rpb_tbl_id =
            fibTblMgr->m_curFibTbl->m_tblId;
    }

    cur_dup_ab = start_blk;
    while (cur_dup_ab)
    {
        if (device->kaps_jr1_shadow && device->nv_ptr)
        {
            device->kaps_jr1_shadow->rpb_blks[cur_dup_ab->ab_num].rpb_rows[logicalLoc].rpb_tbl_id =
                fibTblMgr->m_curFibTbl->m_tblId;
        }
        
        status = kaps_jr1_dm_rpb_write(device, cur_dup_ab->ab_num, logicalLoc, ab_data, ab_mask, 3);

        if (status != KAPS_JR1_OK)
        {
            *o_reason = NLMRSC_XPT_FAILED;
            return NLMERR_FAIL;
        }

        cur_dup_ab = cur_dup_ab->dup_ab;
    }

    return NLMERR_OK;
}

static NlmErrNum_t
kaps_jr1_delete_in_tcam(
    kaps_jr1_fib_tbl_mgr * fibTblMgr,
    uint8_t devNum,
    uint16_t blkWidthInBits,
    struct kaps_jr1_ab_info *start_blk,
    uint32_t logicalLoc,
    NlmReasonCode * o_reason)
{
    kaps_jr1_status status = KAPS_JR1_OK;
    struct kaps_jr1_dba_entry entry;
    struct kaps_jr1_ab_info *cur_dup_ab;

    kaps_jr1_sassert(blkWidthInBits == KAPS_JR1_RPB_WIDTH_1);

    
    cur_dup_ab = start_blk;
    while (cur_dup_ab)
    {

        status =
            kaps_jr1_dm_rpb_write(fibTblMgr->m_devMgr_p, cur_dup_ab->ab_num, logicalLoc, (uint8_t *) & entry,
                                   (uint8_t *) & entry, 0);

        if (status != KAPS_JR1_OK)
        {
            *o_reason = NLMRSC_XPT_FAILED;
            return NLMERR_FAIL;
        }

        cur_dup_ab = cur_dup_ab->dup_ab;
    }
    return NLMERR_OK;
}



NlmErrNum_t
kaps_jr1_write_ab_data(
    kaps_jr1_fib_tbl_mgr * fibTblMgr,
    uint8_t devNum,
    uint16_t blkWidthInBits,
    struct kaps_jr1_ab_info * start_blk,
    uint32_t logicalLoc,
    uint8_t rptId,
    uint16_t startBit,
    uint16_t endBit,
    uint8_t * data,
    uint16_t dataLen,
    uint32_t byte_offset,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t errNum;

    errNum = kaps_jr1_write_to_tcam(fibTblMgr, devNum, blkWidthInBits, start_blk, logicalLoc, rptId, startBit,
                                endBit, data, dataLen, byte_offset, o_reason);


    return errNum;
}

NlmErrNum_t
kaps_jr1_delete_ab_data(
    kaps_jr1_fib_tbl_mgr * fibTblMgr,
    uint8_t devNum,
    uint16_t blkWidthInBits,
    struct kaps_jr1_ab_info * start_blk,
    uint32_t logicalLoc,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t errNum;

    errNum = kaps_jr1_delete_in_tcam(fibTblMgr, devNum, blkWidthInBits, start_blk, logicalLoc, o_reason);

    return errNum;
}

NlmErrNum_t
kaps_jr1_fib_rpt_write(
    kaps_jr1_fib_tbl_mgr * fibTblMgr,
    uint32_t dev_num,
    kaps_jr1_dev_rpt_entry * entry, 
    int32_t isDeleteOper,
    uint16_t addr,              
    NlmReasonCode * o_reason_p)
{
    NlmErrNum_t errNum = NLMERR_OK;
    kaps_jr1_status status;
    struct kaps_jr1_device *device = fibTblMgr->m_devMgr_p;
    struct NlmFibStats *fibStats = NULL;
    struct kaps_jr1_lpm_mgr *lpm_mgr = fibTblMgr->m_lpm_mgr;
    struct kaps_jr1_db *res_db = NULL;
    uint64_t rptMap;
    uint32_t rptNr;

    {
        res_db = lpm_mgr->resource_db;
        rptMap = res_db->hw_res.db_res->rpt_map[0];
        if (res_db->common_info->is_cascaded)
        {
            rptMap = res_db->hw_res.db_res->rpt_map[device->core_id];
        }
    }

    if (isDeleteOper == 1)
    {
        for (rptNr = 0; rptNr < HW_MAX_PCM_BLOCKS; ++rptNr)
        {
            if (rptMap & (1ULL << rptNr))
            {
                status = kaps_jr1_algo_hw_delete_rpt_data(fibTblMgr->m_devMgr_p, rptNr, addr);

                if (status != KAPS_JR1_OK)
                {
                    *o_reason_p = NLMRSC_INTERNAL_ERROR;
                    return NLMERR_FAIL;
                }
            }
        }

        return NLMERR_OK;
    }

    for (rptNr = 0; rptNr < HW_MAX_PCM_BLOCKS; ++rptNr)
    {
        if (rptMap & (1ULL << rptNr))
        {
            status = kaps_jr1_algo_hw_write_rpt_data(device, rptNr, entry->m_data, entry->m_mask, addr );

            if (status != KAPS_JR1_OK)
            {
                *o_reason_p = NLMRSC_INTERNAL_ERROR;
                return NLMERR_FAIL;
            }
        }
    }

    if (fibTblMgr->m_curFibTbl)
    {
        fibStats = &fibTblMgr->m_curFibTbl->m_fibStats;
        fibStats->numRPTWrites++;
        if (device->type != KAPS_JR1_DEVICE)
        {
            fibStats->numPIOWrites++;
        }
    }

    return errNum;
}

NlmErrNum_t
kaps_jr1_fib_rit_write(
    kaps_jr1_fib_tbl_mgr * fibTblMgr,
    uint32_t dev_num,
    struct kaps_jr1_pct * entry,
    uint16_t addr,              
    NlmReasonCode * o_reason_p)
{
    kaps_jr1_status status;
    struct NlmFibStats *fibStats = NULL;
    struct kaps_jr1_lpm_mgr *lpm_mgr = fibTblMgr->m_lpm_mgr;
    struct kaps_jr1_db *res_db = NULL;
    uint64_t rptMap;
    uint32_t rptNr, i, pct_block_num;
    struct kaps_jr1_device *device = fibTblMgr->m_devMgr_p;
    NlmErrNum_t errNum = NLMERR_OK;

    if (device->issu_in_progress)
        return NLMERR_OK;

    {
        res_db = lpm_mgr->resource_db;
        rptMap = res_db->hw_res.db_res->rpt_map[0];
        if (res_db->common_info->is_cascaded)
        {
            rptMap = res_db->hw_res.db_res->rpt_map[device->core_id];
        }
    }

    i = 0;

    for (rptNr = 0; rptNr < HW_MAX_PCM_BLOCKS; ++rptNr)
    {
        if (rptMap & (1ULL << rptNr))
        {

            if (device->type == KAPS_JR1_DEVICE && (rptNr % 4 == 2 || rptNr % 4 == 3))
                continue;

            pct_block_num = rptNr;
            if (device->type == KAPS_JR1_DEVICE)
            {
                pct_block_num = (rptNr % 2);
                pct_block_num += 2 * (rptNr / 4);
            }

            status = kaps_jr1_algo_hw_write_pct(fibTblMgr->m_devMgr_p, &entry[i], pct_block_num, addr);

            if (status != KAPS_JR1_OK)
            {
                *o_reason_p = NLMRSC_INTERNAL_ERROR;
                return NLMERR_FAIL;
            }

            if (device->type != KAPS_JR1_DEVICE)
                ++i;
        }
    }

    fibStats = &fibTblMgr->m_curFibTbl->m_fibStats;
    fibStats->numRITWrites++;
    if (device->type != KAPS_JR1_DEVICE)
    {
        fibStats->numPIOWrites++;
    }

    if (device->type == KAPS_JR1_DEVICE)
    {
        if (entry[0].u.kaps_jr1.bpm_len)
        {
            errNum = kaps_jr1_iit_write(fibTblMgr, 0, NULL, &entry[1].u.kaps_jr1, entry[0].u.kaps_jr1.bpm_ad, o_reason_p);
        }

    }

    return errNum;
}
