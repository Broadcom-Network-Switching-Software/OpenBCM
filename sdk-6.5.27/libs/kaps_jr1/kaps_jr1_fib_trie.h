



#ifndef INCLUDED_KAPS_JR1_TRIE_H
#define INCLUDED_KAPS_JR1_TRIE_H

#include <kaps_jr1_fib_cmn_pfxbundle.h>
#include <kaps_jr1_portable.h>
#include <kaps_jr1_fib_triedata.h>
#include <kaps_jr1_fib_trienode.h>
#include <kaps_jr1_fib_lsnmc.h>
#include "kaps_jr1_fibmgr.h"
#include "kaps_jr1_fibmgr_xk.h"
#include "kaps_jr1_db.h"

#include <kaps_jr1_externcstart.h>


extern kaps_jr1_trie_global *kaps_jr1_trie_two_level_fib_module_init(
    kaps_jr1_nlm_allocator * alloc,
    kaps_jr1_fib_tbl_mgr * fibTblMgr,
    NlmReasonCode * o_reason);

extern void kaps_jr1_trie_module_destroy(
    void *trieGlobal);

extern kaps_jr1_lpm_trie *kaps_jr1_trie_create(
    void *trieGlobal,
    void *fibTbl,
    NlmReasonCode * o_reason);

extern void kaps_jr1_trie_destroy(
    kaps_jr1_lpm_trie * self);

extern NlmErrNum_t kaps_jr1_trie_submit_rqt(
    kaps_jr1_lpm_trie * self,
    NlmTblRqtCode rqtCode,
    struct kaps_jr1_lpm_entry *entry,
    NlmReasonCode * o_reason);

extern kaps_jr1_pfx_bundle *kaps_jr1_trie_locate_lpm(
    kaps_jr1_lpm_trie * self,
    uint8_t * pfxData,
    uint32_t pfxLen,
    uint32_t * o_lpmHitIndex_p,
    NlmReasonCode * o_reason);

extern uint32_t kaps_jr1_trie_locate_exact(
    kaps_jr1_lpm_trie * self,
    uint8_t * pfxData_p,
    uint16_t inUse,
    kaps_jr1_pfx_bundle ** o_pfxBundle_pp,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_jr1_trie_get_prefix_location(
    kaps_jr1_lpm_trie * trie_p,
    struct kaps_jr1_lpm_entry *lpm_entry,
    uint32_t * abs_udc,
    uint32_t * abs_row);

void kaps_jr1_trie_get_resource_usage(
    kaps_jr1_fib_tbl * fibTbl,
    kaps_jr1_fib_resource_usage * rxcUsed_p,
    uint32_t log_info);

NlmErrNum_t kaps_jr1_trie_process_hit_bits_iit_lmpsofar(
    kaps_jr1_lpm_trie * trie_p);


kaps_jr1_status kaps_jr1_trie_wb_save(
    kaps_jr1_lpm_trie * trie_p,
    uint32_t * nv_offset);


kaps_jr1_status kaps_jr1_trie_wb_restore(
    kaps_jr1_lpm_trie * trie_p,
    uint32_t * nv_offset);


NlmErrNum_t kaps_jr1_trie_wb_save_pools(
    kaps_jr1_trie_global * trie_g,
    struct kaps_jr1_wb_cb_functions *wb_fun);


NlmErrNum_t kaps_jr1_trie_wb_restore_pools(
    kaps_jr1_trie_global * trie_g,
    struct kaps_jr1_wb_cb_functions *wb_fun);






kaps_jr1_status kaps_jr1_trie_wb_traverse_and_print_trie(
    kaps_jr1_lpm_trie * trie,
    FILE * fp);


kaps_jr1_status kaps_jr1_trie_wb_verify_trie(
    kaps_jr1_lpm_trie * trie,
    FILE * fp);

void kaps_jr1_trie_calc_trie_lsn_stats(
    kaps_jr1_trie_node * node_p,
    uint32_t find_terminating_search_path, 
    struct NlmFibStats *stats_p);





kaps_jr1_status kaps_jr1_trie_cr_restore(
    kaps_jr1_lpm_trie * trie_p,
    uint32_t * nv_offset);


NlmErrNum_t kaps_jr1_trie_cr_restore_pools(
    kaps_jr1_lpm_trie * trie,
    struct kaps_jr1_wb_cb_functions *wb_fun);

void kaps_jr1_trie_cr_save_default_rpt_entry(
    kaps_jr1_trie_global * trie_g,
    kaps_jr1_lpm_trie * trie,
    struct kaps_jr1_wb_cb_functions *wb_fun);

NlmErrNum_t kaps_jr1_trie_cr_restore_default_rpt_entry(
    kaps_jr1_trie_global * trie_g,
    kaps_jr1_lpm_trie * trie,
    struct kaps_jr1_wb_cb_functions *wb_fun);

void kaps_jr1_trie_wb_restore_iit_lmpsofar(
    kaps_jr1_trie_node * node_p,
    kaps_jr1_trie_node * ancestorIptNode_p);

void kaps_jr1_trie_cr_save_pools(
    kaps_jr1_trie_global * trie_g,
    struct kaps_jr1_wb_cb_functions *wb_fun);

kaps_jr1_trie_node *kaps_jr1_trie_find_dest_lp_node(
    kaps_jr1_lpm_trie * self,
    kaps_jr1_trie_node * start_node,
    const uint8_t * pfxdata,
    uint32_t pfxlen,
    kaps_jr1_trie_node ** last_seen_node,
    int32_t isPfxInsertion);

NlmErrNum_t
kaps_jr1_trie_process_iit_lmpsofar(
    kaps_jr1_lpm_trie * trie_p,
    kaps_jr1_trie_node * destnode,
    struct kaps_jr1_lpm_entry * entry,
    NlmTblRqtCode rqtCode,
    NlmReasonCode * o_reason);


void kaps_jr1_trie_verify_iit_lmpsofar(
    kaps_jr1_trie_node * node_p,
    kaps_jr1_trie_node * ancestorIptNode_p);
#include <kaps_jr1_externcend.h>

#endif 
