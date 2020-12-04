/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        rx.c
 * Purpose:     XGS5 RX common driver.
 *
 * Notes:
 *      New RX APIs implementation.
 */

#include <shared/bsl.h>
#include <soc/defs.h>
#include <soc/drv.h>

#include <bcm_int/esw/xgs5.h>
#include <bcm_int/esw/rx.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/trunk.h>
#include <bcm_int/common/rx.h>
#include <bcm/error.h>


#if defined (BCM_EP_REDIRECT_VERSION_2)


#define BCMI_MAX_STRENGTH_VALUE (0x3E)

/* Packet drop reasons */
typedef enum bcmi_pkt_drop_reason_e {
    bcmiDropReasonEgressVxltMISSDrop = 0,
    bcmiDropReasonEgressTunnelSnapDrop,
    bcmiDropReasonEgressCfiDrop,
    bcmiDropReasonEgressTtlDrop,
    bcmiDropReasonEgressNotVlanMemberDrop,
    bcmiDropReasonEgressStgBlockDrop,
    bcmiDropReasonEgressIpmcL2SelfPort,
    bcmiDropReasonEgressPurgePkt,
    bcmiDropReasonEgressAgedPkt,
    bcmiDropReasonEgressBadTdip,
    bcmiDropReasonEgressProtectionDrop,
    bcmiDropReasonEgressVlanNotFound,
    bcmiDropReasonEgressStgDisableDrop,
    bcmiDropReasonEgressHigig2RsvdDrop,
    bcmiDropReasonEgressHigigRsvdHgi,
    bcmiDropReasonEgressL2MtuFailDrop,
    bcmiDropReasonEgressCompositeErrorDrop,
    bcmiDropReasonEgressIpmcL3SelfIntf,
    bcmiDropReasonEgressModidGrt31Drop,
    bcmiDropReasonEgressEfpDrop,
     /* The loction at 20 is not applicable */
    bcmiDropReasonEgressSplitHorizon = 21,
    bcmiDropReasonEgressSourceVirtualPortRemoval,
    bcmiDropReasonEgressNivPruneDrop,
    bcmiDropReasonEgressNonucastPruneDrop,
    bcmiDropReasonEgressTrillPktDrop,
    bcmiDropReasonEgressPacketTooSmall,
    bcmiDropReasonEgressNonTrillPktDrop,
    bcmiDropReasonEgressVplagPruning,
    bcmiDropReasonEgressInvalid1588PacketDrop,
    bcmiDropReasonEgressFcoePacketDrop,
    bcmiDropReasonEgressCnmPacketDrop,
    bcmiDropReasonEgressEpDropOrig
} bcmi_pkt_drop_reason_t;

/* Sync for copy-to-cpu config */
sal_mutex_t  _bcmi_rx_copy_to_cpu_config_mutex[BCM_MAX_NUM_UNITS];

/* Initialize the copy to cpu config data */
_bcmi_rx_CopyToCpu_config_t
    *(_bcmi_rx_egr_drop_copy_to_cpu_config_data[BCM_MAX_NUM_UNITS]) = { 0 };

SHR_BITDCL *_bcmi_rx_egr_drop_copy_to_cpu_loc_bitmap[BCM_MAX_NUM_UNITS] = { 0 };
/*
 * Function:
 *      bcmi_xgs5_rx_CopyToCpu_index_check
 * Purpose:
 *      Check if a index to the ctc table is valid
 * Parameters:
 *      unit             - (IN) device id.
 *      index_to_verify  - (IN) Index to verify.
 * Returns:
 *      BCM_E_*
 */
int bcmi_xgs5_rx_CopyToCpu_index_check(int unit, int index_to_verify)
{
    int start, end;
    soc_mem_t mem = EP_CTC_RES_TABLEm;

    /* Gather the table depth */
    start = soc_mem_index_min(unit, mem);
    end = soc_mem_index_max(unit, mem);

    if((index_to_verify < start) || (index_to_verify > end)) {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_rx_CopyToCpu_free_entry_get
 * Purpose:
 *      Get a free unused entry
 * Parameters:
 *      unit        - (IN) device id.
 *      free_index  - (OUT) Index of the free entry.
 * Returns:
 *      BCM_E_*
 */
int bcmi_xgs5_rx_CopyToCpu_free_entry_get(int unit, int *free_index)
{
    int index, start, end;
    soc_mem_t mem = EP_CTC_RES_TABLEm;

    if(free_index == NULL) {
        return BCM_E_PARAM;
    }

    /* Gather the table start and end indices */
    start = soc_mem_index_min(unit, mem);
    end = soc_mem_index_max(unit, mem);

    /* Look for a unused entry */
    for (index = start; index <= end; index++) {
        if(!SHR_BITGET(_bcmi_rx_egr_drop_copy_to_cpu_loc_bitmap[unit], index)) {
            *free_index = index;
            return BCM_E_NONE;
        }
    }

    /* If we end up here, no free entry found return error */
    return BCM_E_FULL;
}

/*
 * Function:
 *      bcmi_xgs5_rx_CopyToCpu_index_in_use_check
 * Purpose:
 *      Verify if a given index is in use
 * Parameters:
 *      unit             - (IN) device id.
 *      index_to_verify  - (IN) Index of the free entry.
 * Returns:
 *      BCM_E_*
 */
int bcmi_xgs5_rx_CopyToCpu_index_in_use_check(int unit, int index_to_check)
{
    if(SHR_BITGET(
        _bcmi_rx_egr_drop_copy_to_cpu_loc_bitmap[unit], index_to_check)) {
        /* Return 'bcm_e_exists' to indicate it is in use */
        return BCM_E_EXISTS;
    }

    /* Return not_found if the entry is not configured */
    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *      bcmi_rx_copyToCpu_drop_reason_hw_field_get
 * Purpose:
 *      Given a user facing drop reason, return its hardware field equivalent
 * Parameters:
 *      unit         - (IN) device id.
 *      drop_reason  - (IN) Drop reason field to get.
 *      drop_reason_field  - (OUT) Drop reason field.
 * Returns:
 *      BCM_E_*
 */
int bcmi_rx_copyToCpu_drop_reason_hw_field_get(int unit,
                                    bcm_pkt_drop_reason_t drop_reason,
                                    bcmi_pkt_drop_reason_t *drop_reason_field)
{
    /* Sanity check */
    if(drop_reason_field == NULL) {
        return BCM_E_PARAM;
    }

    switch (drop_reason) {
        case bcmPktDropReasonEgressVlanTranslateMiss:
            *drop_reason_field = bcmiDropReasonEgressVxltMISSDrop;
            break;
        case bcmPktDropReasonEgressTunnelSnap:
            *drop_reason_field = bcmiDropReasonEgressTunnelSnapDrop;
            break;
        case bcmPktDropReasonEgressColorSelect:
            *drop_reason_field = bcmiDropReasonEgressCfiDrop;
            break;
        case bcmPktDropReasonEgressTtlError:
            *drop_reason_field = bcmiDropReasonEgressTtlDrop;
            break;
        case bcmPktDropReasonEgressVlanFilter:
            *drop_reason_field = bcmiDropReasonEgressNotVlanMemberDrop;
            break;
        case bcmPktDropReasonEgressStgBlocked:
            *drop_reason_field = bcmiDropReasonEgressStgBlockDrop;
            break;
        case bcmPktDropReasonEgressIPMCSameVlanPrune:
            *drop_reason_field = bcmiDropReasonEgressIpmcL2SelfPort;
            break;
        case bcmPktDropReasonEgressMmuPurge:
            *drop_reason_field = bcmiDropReasonEgressPurgePkt;
            break;
        case bcmPktDropReasonEgressMmuAge:
            *drop_reason_field = bcmiDropReasonEgressAgedPkt;
            break;
        case bcmPktDropReasonEgressBadTdipDrop:
            *drop_reason_field = bcmiDropReasonEgressBadTdip;
            break;
        case bcmPktDropReasonEgressProtection:
            *drop_reason_field = bcmiDropReasonEgressProtectionDrop;
            break;
        case bcmPktDropReasonEgressUnknownVlan:
            *drop_reason_field = bcmiDropReasonEgressVlanNotFound;
            break;
        case bcmPktDropReasonEgressStgDisabled:
            *drop_reason_field = bcmiDropReasonEgressStgDisableDrop;
            break;
        case bcmPktDropReasonEgressHiGig2ReservedPPD:
            *drop_reason_field = bcmiDropReasonEgressHigig2RsvdDrop;
            break;
        case bcmPktDropReasonEgressHigigPlusUnknown:
            *drop_reason_field = bcmiDropReasonEgressHigigRsvdHgi;
            break;
        case bcmPktDropReasonEgressL2MtuExceeded:
            *drop_reason_field = bcmiDropReasonEgressL2MtuFailDrop;
            break;
        case bcmPktDropReasonEgressCompositeError:
            *drop_reason_field = bcmiDropReasonEgressCompositeErrorDrop;
            break;
        case bcmPktDropReasonEgressMulticastPruned:
            *drop_reason_field = bcmiDropReasonEgressIpmcL3SelfIntf;
            break;
        case bcmPktDropReasonEgressHigigPlusInvalidModuleId:
            *drop_reason_field = bcmiDropReasonEgressModidGrt31Drop;
            break;
        case bcmPktDropReasonEgressFieldAction:
            *drop_reason_field = bcmiDropReasonEgressEfpDrop;
            break;
        case bcmPktDropReasonEgressSplitHorizon:
            *drop_reason_field = bcmiDropReasonEgressSplitHorizon;
            break;
        case bcmPktDropReasonEgressSVPRemoval:
            *drop_reason_field = bcmiDropReasonEgressSourceVirtualPortRemoval;
            break;
        case bcmPktDropReasonEgressNivSrcKnockout:
            *drop_reason_field = bcmiDropReasonEgressNivPruneDrop;
            break;
        case bcmPktDropReasonEgressNonUCPrune:
            *drop_reason_field = bcmiDropReasonEgressNonucastPruneDrop;
            break;
        case bcmPktDropReasonEgressTrillPacket:
            *drop_reason_field = bcmiDropReasonEgressTrillPktDrop;
            break;
        case bcmPktDropReasonEgressPacketTooSmall:
            *drop_reason_field = bcmiDropReasonEgressPacketTooSmall;
            break;
        case bcmPktDropReasonEgressNonTrillPacket:
            *drop_reason_field = bcmiDropReasonEgressNonTrillPktDrop;
            break;
        case bcmPktDropReasonEgressVpLagPruned:
            *drop_reason_field = bcmiDropReasonEgressVplagPruning;
            break;
        case bcmPktDropReasonEgressInvalid1588Packet:
            *drop_reason_field = bcmiDropReasonEgressInvalid1588PacketDrop;
            break;
        case bcmPktDropReasonEgressInvalidFcoePacket:
            *drop_reason_field = bcmiDropReasonEgressFcoePacketDrop;
            break;
        case bcmPktDropReasonEgressInvalidCnmPacket:
            *drop_reason_field = bcmiDropReasonEgressCnmPacketDrop;
            break;
        case bcmPktDropReasonEgressOriginalPacket:
            *drop_reason_field = bcmiDropReasonEgressEpDropOrig;
            break;
        default:
            return BCM_E_PARAM;
            break;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_rx_CopyToCpu_config_add
 * Purpose:
 *      Add a copy-to-cpu entry
 * Parameters:
 *      unit              - (IN) device id.
 *      options           - (IN) options.
 *      copyToCpu_config  - (IN) Pointer to the copy-to-cpu config structure
 * Returns:
 *      BCM_E_*
 */

int bcmi_xgs5_rx_CopyToCpu_config_add(int unit, uint32 options,
                                bcm_rx_CopyToCpu_config_t *copyToCpu_config)
{
    ep_ctc_res_table_entry_t ep_ctc_res_table_entry;
    soc_mem_t mem = EP_CTC_RES_TABLEm;
    bcm_pkt_drop_reason_t drop_reason = 0;
    int table_index = 0;
    int buffer_priority = 0;
    bcmi_pkt_drop_reason_t drop_reason_field = 0;
    uint32 fldbuf[2];

    /* Sanity check */
    if(copyToCpu_config == NULL) {
        return BCM_E_PARAM;
    }

    /* Assign the index, incase we need to reuse it */
    table_index = copyToCpu_config->index;

    

    /* Check if the user has supplied the WITH_ID option, if has
       validate the same */
    if(options & BCM_RX_COPYTOCPU_WITH_ID) {
        BCM_IF_ERROR_RETURN(bcmi_xgs5_rx_CopyToCpu_index_check(unit,
                                                               table_index));
    }

    /* If this is a REPLACE operation, make sure the index is currently
       configured */
    if(options & BCM_RX_COPYTOCPU_REPLACE) {
        BCM_IF_ERROR_RETURN(bcmi_xgs5_rx_CopyToCpu_index_in_use_check(unit,
                                                          table_index));
    } else {
        /* For a add operation without Id supplied get a free index we
           need to configure */
        if(!(options & BCM_RX_COPYTOCPU_WITH_ID)) {
            BCM_IF_ERROR_RETURN(bcmi_xgs5_rx_CopyToCpu_free_entry_get(unit,
                                                              &table_index));
        }
    }

    /* Make sure the supplied drop_vector is non-null */
    if(BCM_PKT_DROP_REASON_IS_NULL(copyToCpu_config->drop_reason_vector)) {
        return BCM_E_PARAM;
    }

    /* Read the entry */
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                                     table_index,
                                     &ep_ctc_res_table_entry));

    /* Set the desired drop_vector */
    /* Extract the current drop_vector */
    soc_mem_field_get(unit, mem, (uint32 *)&ep_ctc_res_table_entry,
                      SELECT_BITMAPf, fldbuf);

    /* Run through the drop reasons and set/unset based on user choice */
    for (drop_reason = bcmPktDropReasonEgressVlanTranslateMiss;
          drop_reason < bcmPktDropReasonCount;
          drop_reason++) {

        /* Map the drop_reason to relevant field in hardware */
        BCM_IF_ERROR_RETURN(bcmi_rx_copyToCpu_drop_reason_hw_field_get(unit,
                                      drop_reason, &drop_reason_field));

        /* Depending on the user selection, set/clear the field */
        if (BCM_PKT_DROP_REASON_GET(
            copyToCpu_config->drop_reason_vector, drop_reason)) {
            /* Set the drop_reason in question */
            fldbuf[drop_reason_field / 32] |= (1 << (drop_reason_field % 32));
        } else {
            /* Unset the group in question */
            fldbuf[drop_reason_field / 32] &= ~(1 << (drop_reason_field % 32));
        }
    }

    /* Setup the updated drop vector */
    soc_mem_field_set(unit, mem, (uint32 *)&ep_ctc_res_table_entry,
                      SELECT_BITMAPf, fldbuf);

    /* Set the trigger strength */
    if((copyToCpu_config->strength >= 0) &&
       (copyToCpu_config->strength < BCMI_MAX_STRENGTH_VALUE)) {
        soc_mem_field32_set(unit, mem, (uint32 *)&ep_ctc_res_table_entry,
                  STRENGTHf, copyToCpu_config->strength);
    } else {
        return BCM_E_PARAM;
    }

    /* Set the buffer priority */
    if(copyToCpu_config->buffer_priority &
       BCM_RX_COPYTOCPU_BUFFER_PRIORITY_LOW) {
        buffer_priority = _BCM_RX_COPYTOCPU_BUFFER_PRIORITY_LOW;
    } else if(copyToCpu_config->buffer_priority &
       BCM_RX_COPYTOCPU_BUFFER_PRIORITY_MEDIUM) {
        buffer_priority = _BCM_RX_COPYTOCPU_BUFFER_PRIORITY_MEDIUM;
    } else if(copyToCpu_config->buffer_priority &
       BCM_RX_COPYTOCPU_BUFFER_PRIORITY_HIGH) {
        buffer_priority = _BCM_RX_COPYTOCPU_BUFFER_PRIORITY_HIGH;
    }

    soc_mem_field32_set(unit, mem, (uint32 *)&ep_ctc_res_table_entry,
              RDB_PRIORITYf, buffer_priority);

    /* Set the CPU CoS Queue */
    if(copyToCpu_config->cpu_cosq != -1) {
       if(copyToCpu_config->cpu_cosq < NUM_CPU_COSQ(unit)) {
        soc_mem_field32_set(unit, mem, (uint32 *)&ep_ctc_res_table_entry,
                  CPU_COSf, copyToCpu_config->cpu_cosq);
       } else {
           return BCM_E_PARAM;
       }
    }

    /* Set the truncate option, if user has chosen the same */
    if(copyToCpu_config->flags & BCM_PORT_REDIRECT_TRUNCATE) {
        soc_mem_field32_set(unit, mem, (uint32 *)&ep_ctc_res_table_entry,
                            FIRST_CELL_ONLYf, 1);
    }

    /* Write the entry to hardware */
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem,
                                      MEM_BLOCK_ALL, table_index,
                                      &ep_ctc_res_table_entry));

    /* Mark that this index is now in use */
    SHR_BITSET(_bcmi_rx_egr_drop_copy_to_cpu_loc_bitmap[unit], table_index);

    /* Update the local software copy */
    BCMI_GET_COPY_TOCPU_FLAGS_SET(unit, table_index,
                                  copyToCpu_config->flags);
    BCMI_GET_COPY_TOCPU_DROP_REASONS_SET(unit, table_index,
                                         copyToCpu_config->drop_reason_vector);
    BCMI_GET_COPY_TOCPU_STRENGTH_SET(unit, table_index,
                                     copyToCpu_config->strength);
    BCMI_GET_COPY_TOCPU_BUFFER_PRIORITY_SET(unit, table_index,
                                            copyToCpu_config->buffer_priority);
    BCMI_GET_COPY_TOCPU_COS_Q_SET(unit, table_index,
                                            copyToCpu_config->cpu_cosq);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_rx_CopyToCpu_config_get
 * Purpose:
 *      Get a copy-to-cpu entry
 * Parameters:
 *      unit              - (IN) device id.
 *      table_index       - (IN) Index of the entry.
 *      copyToCpu_config  - (OUT) Pointer to the copy-to-cpu config structure
 * Returns:
 *      BCM_E_*
 */

int bcmi_xgs5_rx_CopyToCpu_config_get(int unit, int table_index,
                           bcm_rx_CopyToCpu_config_t *copyToCpu_config)
{
    /* Sanity check */
    if(copyToCpu_config == NULL) {
        return BCM_E_PARAM;
    }

    /* Validate the index */
    BCM_IF_ERROR_RETURN(bcmi_xgs5_rx_CopyToCpu_index_check(unit,
                                                           table_index));

    /* Check if the entry is configured, it is an invalid index if it is not */
    if(bcmi_xgs5_rx_CopyToCpu_index_in_use_check(unit,
                          table_index) != BCM_E_EXISTS) {

        return BCM_E_PARAM;
    }

    /* Copy the software copy to send back to user */
    copyToCpu_config->index = table_index;
    copyToCpu_config->flags =
                      BCMI_GET_COPY_TOCPU_FLAGS_GET(unit, table_index);
    copyToCpu_config->drop_reason_vector =
                      BCMI_GET_COPY_TOCPU_DROP_REASONS_GET(unit, table_index);
    copyToCpu_config->strength =
                      BCMI_GET_COPY_TOCPU_STRENGTH_GET(unit, table_index);
    copyToCpu_config->buffer_priority =
                      BCMI_GET_COPY_TOCPU_BUFFER_PRIORITY_GET(unit, table_index);
    copyToCpu_config->cpu_cosq =
                      BCMI_GET_COPY_TOCPU_COS_Q_GET(unit, table_index);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_rx_CopyToCpu_config_delete
 * Purpose:
 *      Delete a copy-to-cpu entry
 * Parameters:
 *      unit   - (IN) device id.
 *      index  - (IN) Index of the entry to delete
 * Returns:
 *      BCM_E_*
 */

int bcmi_xgs5_rx_CopyToCpu_config_delete(int unit, int table_index)
{
    soc_mem_t mem = EP_CTC_RES_TABLEm;

    /* Validate the index */
    BCM_IF_ERROR_RETURN(bcmi_xgs5_rx_CopyToCpu_index_check(unit,
                                                           table_index));

    /* Check if the entry is configured */
    if(bcmi_xgs5_rx_CopyToCpu_index_in_use_check(unit,
                          table_index) != BCM_E_EXISTS) {
        return BCM_E_PARAM;
    }

    /* Reset the entry */
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ANY, table_index,
                        soc_mem_entry_null(unit, mem)));

    /* Mark that this index is now cleared */
    SHR_BITCLR(_bcmi_rx_egr_drop_copy_to_cpu_loc_bitmap[unit], table_index);

    /* Clear out the software copy */
    sal_memset((_bcmi_rx_egr_drop_copy_to_cpu_config_data[unit] + table_index), 0,
               sizeof(bcm_rx_CopyToCpu_config_t));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_rx_CopyToCpu_config_get_all
 * Purpose:
 *      Retrieve muultiple copy-to-cpu entries
 * Parameters:
 *      unit              - (IN) device id.
 *      entries_max       - (IN) Max entries to retrieve
 *      copyToCpu_config  - (OUT) Pointer to the copy-to-cpu config structure
 *      entries_count     - (OUT) Number of entries actually retrieved
 * Returns:
 *      BCM_E_*
 */

int bcmi_xgs5_rx_CopyToCpu_config_get_all(int unit, int entries_max,
                                 bcm_rx_CopyToCpu_config_t *copyToCpu_config,
                                 int *entries_count)
{
    int index = 0, max_index = 0;
    soc_mem_t mem = EP_CTC_RES_TABLEm;

    /* Sanity check */
    if(entries_count) {
        *entries_count = 0;
    } else {
        return BCM_E_PARAM;
    }

    /* Gather the table depth */
    max_index = soc_mem_index_count(unit, mem);

    /* If the 'entries_max' is 0 and the user provided 'copyToCpu_config'
       in NULL, the user only wants to get a count of configured entries.
       Run thru the list and count configured entries.

       If the user has set the 'entries_max' as 0 and the data holder
       'copyToCpu_config' is non-NULL, send back a 'param' error */
    if((entries_max == 0) && (copyToCpu_config == NULL)) {
        /* Iterate over configured entries */
        SHR_BIT_ITER(_bcmi_rx_egr_drop_copy_to_cpu_loc_bitmap[unit],
                     max_index,
                     index) {
            /* Keep incrementing the number of entries processed */
            (*entries_count)++;
        }

        return BCM_E_NONE;
    } else if((entries_max == 0) && (copyToCpu_config != NULL)) {
        return BCM_E_PARAM;
    }

    /* Run through the configured list and fill in needed data of configured
       entries */
    SHR_BIT_ITER(_bcmi_rx_egr_drop_copy_to_cpu_loc_bitmap[unit],
                 max_index,
                 index) {
        /* Fill in the modid data if the modid_max is non-zero and the
           data place holder is allocated */
        if(entries_max && copyToCpu_config) {
            copyToCpu_config[*entries_count].index = index;
            BCM_IF_ERROR_RETURN(bcmi_xgs5_rx_CopyToCpu_config_get(unit,
                                    index, &copyToCpu_config[*entries_count]));
            /* Keep incrementing the number of entries processed */
            (*entries_count)++;
            /* Keep decrementing the number of mods requested */
            if(entries_max > 0) {
                --entries_max;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_rx_CopyToCpu_config_delete_all
 * Purpose:
 *      Delete all copy-to-cpu entries configured
 * Parameters:
 *      unit              - (IN) device id.
 * Returns:
 *      BCM_E_*
 */

int bcmi_xgs5_rx_CopyToCpu_config_delete_all(int unit)
{
    int index = 0, max_index = 0;
    soc_mem_t mem = EP_CTC_RES_TABLEm;

    /* Gather the table depth */
    max_index = soc_mem_index_count(unit, mem);

    /* Run thru the list delete configured entries */
    SHR_BIT_ITER(_bcmi_rx_egr_drop_copy_to_cpu_loc_bitmap[unit], max_index, index) {
        BCM_IF_ERROR_RETURN(
            bcmi_xgs5_rx_CopyToCpu_config_delete(unit, index));
    }

    return BCM_E_NONE;
}

#endif /* BCM_EP_REDIRECT_VERSION_2*/
