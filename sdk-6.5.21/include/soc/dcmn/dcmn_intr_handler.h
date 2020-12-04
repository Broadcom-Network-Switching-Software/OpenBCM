/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:    Implement header for soc interrupt handler.
 */

#ifndef _DCMN_INTR_HANDLER_H_
#define _DCMN_INTR_HANDLER_H_


#include <soc/defs.h>
#include <soc/intr.h>
#include <soc/dcmn/error.h>


typedef enum dcmn_intr_reset_callback_e
{
    ASIC_RESET_NONE,
    ASIC_PON_RESET,
    ASIC_HARD_RESET,
    ASIC_SOFT_RESET_BLOCKS,
    ASIC_SOFT_RESET_BLOCKS_FABRIC
} dcmn_intr_reset_callback_t;

int dcmn_num_of_ints(int unit);

void dcmn_intr_switch_event_cb(
    int unit, 
    soc_switch_event_t event, 
    uint32 arg1, 
    uint32 arg2, 
    uint32 arg3, 
    void *userdata);
int dcmn_intr_add_handler(int unit, int en_inter, int occurrences, int timeCycle, soc_handle_interrupt_func inter_action, soc_handle_interrupt_func inter_recurring_action);
int dcmn_intr_add_handler_ext(int unit, int en_inter, int occurrences, int timeCycle, soc_handle_interrupt_func inter_action, soc_handle_interrupt_func inter_recurring_action,char *msg);
int dcmn_intr_add_clear_ext(int unit,int en_inter,clear_func inter_action);
int dcmn_intr_handler_deinit(int unit);
int dcmn_intr_handler_short_init(int unit);
int dcmn_intr_handler_init(int unit);

int dcmn_get_ser_entry_from_cache(int unit,  soc_mem_t mem, int copyno, int array_index, int index, uint32 *data_entr);
int dcmn_int_name_to_id(int unit, char *name);
int dcmn_fdt_unreachdest_interrupt_clear(int unit, int block_instance, int interrupt_id);
int dcmn_ips_emptysdqcqwrite_interrupt_clear(int unit, int block_instance, int interrupt_id);
int dcmn_ips_emptyddqcqwrite_interrupt_clear(int unit, int block_instance, int interrupt_id);
int dcmn_ips_queueentereddel_interrupt_clear(int unit, int block_instance, int interrupt_id);
int dcmn_ips_creditlost_interrupt_clear(int unit, int block_instance, int interrupt_id);
int dcmn_ips_creditoverflow_interrupt_clear(int unit, int block_instance, int interrupt_id);
int dcmn_ips_sdqcqoverflow_interrupt_clear(int unit, int block_instance, int interrupt_id);
int dcmn_ips_ddqcqoverflow_interrupt_clear(int unit, int block_instance, int interrupt_id);
int dcmn_ips_pushqueueactive_interrupt_clear(int unit, int block_instance, int interrupt_id);

int dcmn_interrupt_handle_IHBInvalidDestinationValid(int unit,int block_instance,uint32 en_interrupt,char * msg);
int dcmn_interrupt_handle_IHBKapsDirectLookupErr(int unit,int block_instance,uint32 en_interrupt,char * msg);
int dcmn_interrupt_handle_IHBIsemErrorTableCoherency(int unit,int block_instance,uint32 en_interrupt,char * msg);
int dcmn_interrupt_handle_IHBIsemErrorCamTableFull(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IHBIsemErrorDeleteUnknownKey(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IHBIsemErrorReachedMaxEntryLimit(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IHBIsemWarningInsertedExisting(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IHBIsemManagementUnitFailureValid(int unit,int block_instance,uint32 en_interrupt,char * msg);
int dcmn_interrupt_handle_MRPSMcdaWrap(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_MRPSMcdbWrap(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_MTRPS_EMMcdaWrap(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_MTRPS_EMMcdbWrap(int unit, int block_instance, uint32 en_interrupt, char *msg);

int dcmn_interrupt_handle_SCHActFlowBadParams(int unit,int block_instance,uint32 en_interrupt,char * msg);
int dcmn_interrupt_handle_SCHShpFlowBadParams(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_SCHRestartFlowEvent(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_SCHSmpThrowSclMsg(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_SCHSclGroupChanged(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_SCHSmpFabricMsgsFifoFull(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_SCHSmpBadMsg(int unit, int block_instance, uint32 en_interrupt, char *msg);

int dcmn_interrupt_handle_FDTBurstTooLarge(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_FMACLOS_Int(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_FMACRxLostOfSync(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_FMACOOF_Int(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_FMACDecErr_Int(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_FMACTransmitErr_Int(int unit, int block_instance, uint32 en_interrupt, char *msg);

int dcmn_interrupt_handle_PPDB_BMactErrorTableCoherency_Int(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_PPDB_BMactManagementUnitFailureValid_Int(int unit, int block_instance, uint32 en_interrupt, char *msg);

int dcmn_interrupt_handle_FCRCpuCntCellFne(int unit,int block_instance,uint32 en_interrupt,char * msg);

int dcmn_interrupt_handle_IPTCrcErrPkt_Int(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IPTCrcDeletedBuffersFifoFull_Int(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IPTCrcDeletedBuffersFifoNotEmpty_Int(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IPTOcbOnlyDataOverflow_Int(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IPTOcbMix0DataOverflow_Int(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IPTOcbMix1DataOverflow_Int(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_OAMPRxStatsDone(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_OAMPRmapemManagementUnitFailureValid(int unit, int block_instance, uint32 en_interrupt, char *msg);

int dcmn_interrupt_handle_IDRErrorMiniMulticast0ExtOverflow(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IDRErrorFullMulticast0ExtOverflow(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IDRErrorMiniMulticast1ExtOverflow(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IDRErrorFullMulticast1ExtOverflow(int unit, int block_instance, uint32 en_interrupt, char *msg);

int dcmn_interrupt_handle_IRRErrIsMaxReplication(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IRRErrIsReplicationEmpty(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IRRErrMaxReplication(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IRRErrReplicationEmpty(int unit, int block_instance, uint32 en_interrupt, char *msg);

int dcmn_interrupt_handle_IQMFreeBdbOvf(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IQMFreeBdbUnf(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IQMDeqComEmptyQ(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IQMFullUscntOvf(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IQMMiniUscntOvf(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IQMUpdtFifoOvf(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IQMIngressReset(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IQMFreeBdbProtErr(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_IngressSoftReset(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_FDRIFMFifoOverflow_Int(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_fsrd_fsrd_SyncStatusChanged(int unit, int block_instance, uint32 en_interrupt, char *msg);
int dcmn_interrupt_handle_fsrd_fsrd_TxpllLockChanged(int unit, int block_instance, uint32 en_interrupt, char *msg);
#endif 
