

#ifndef INCLUDED_KAPS_JR1_TRIENODE_H
#define INCLUDED_KAPS_JR1_TRIENODE_H

#include "kaps_jr1_fib_cmn_pfxbundle.h"
#include "kaps_jr1_portable.h"
#include "kaps_jr1_fib_cmn_nstblrqt.h"
#include "kaps_jr1_device.h"

#include "kaps_jr1_externcstart.h"

#define NLMNSTRIE_MAXCHILD 2
#define NLMNSTRIE_LCHILD 0
#define NLMNSTRIE_RCHILD 1

#define NLM_INVALID_RPT_UUID 0xFFFF


typedef enum NlmNsTrieNodeType
{
    NLMNSTRIENODE_REGULAR = 0,
    NLMNSTRIENODE_LP
} NlmNsTrieNodeType;

typedef struct NlmNsDownStreamRptNodes
{
    struct kaps_jr1_trie_node *rptNode;
    struct NlmNsDownStreamRptNodes *next;
} NlmNsDownStreamRptNodes;


typedef struct kaps_jr1_trie_node
{
    struct kaps_jr1_trie_node *m_parent_p;
    struct kaps_jr1_trie_node *m_child_p[NLMNSTRIE_MAXCHILD];
    struct kaps_jr1_lpm_trie *m_trie_p;
    struct kaps_jr1_lsn_mc *m_lsn_p;

    struct kaps_jr1_trie_node *m_rptParent_p;

    kaps_jr1_pfx_bundle *m_lp_prefix_p;     
    kaps_jr1_pfx_bundle *m_rpt_prefix_p;

    
    kaps_jr1_pfx_bundle *m_aptLmpsofarPfx_p;

    
    kaps_jr1_pfx_bundle *m_rptAptLmpsofarPfx;

    
    kaps_jr1_pfx_bundle *m_iitLmpsofarPfx_p;

    NlmNsDownStreamRptNodes *m_downstreamRptNodes;

    uint16_t m_depth;           

    
    uint16_t m_numIptEntriesInSubtrie;

    
    uint16_t m_numAptEntriesInSubtrie;

    uint32_t m_poolId;
    uint32_t m_trienodeId;

    uint16_t m_rptId;
    uint16_t m_rpt_uuid;        

    uint32_t m_node_type:1;
    uint32_t m_numRptSplits:12;
    uint32_t m_child_id:1;
    uint32_t m_isRptNode:1;
    uint32_t m_isReserved160bTrig:1;
    uint32_t m_numReserved160bTrig:16;

} kaps_jr1_trie_node;


typedef struct NlmNsTrieNode__TraverseArgs
{
    void *arg1;                 
    void *arg2;                 
    void *arg3;                 
    void *arg4;                 
    void *arg5;                 
} NlmNsTrieNode__TraverseArgs;


typedef enum kaps_jr1_recursion_status_t
{
    KAPS_JR1_RECURSION_STOP,
    KAPS_JR1_RECURSION_ERROR,
    KAPS_JR1_RECURSION_CONTINUE
} kaps_jr1_recursion_status;


typedef enum kaps_jr1_trie_node_processing_status_t
{
    KAPS_JR1_UNTRAVERSED_NODE,
    KAPS_JR1_LEFT_CHILD_EXPANDED,
    KAPS_JR1_RIGHT_CHILD_EXPANDED
} kaps_jr1_trie_node_processing_status;



typedef int32_t (
    *kaps_jr1_trie_node_traverse_dfs_visitor_t) (
    kaps_jr1_trie_node * node,
    NlmNsTrieNode__TraverseArgs * data);


typedef int (
    *kaps_jr1_trie_node_traverse_dfs_visitor_for_remove_t) (
    kaps_jr1_trie_node * node,
    NlmNsTrieNode__TraverseArgs * data);




kaps_jr1_trie_node *kaps_jr1_trie_node_create_root_node(
    struct kaps_jr1_lpm_trie *trie,
    uint32_t child);


void kaps_jr1_trie_node_destroy(
    kaps_jr1_trie_node * self,
    kaps_jr1_nlm_allocator * alloc,
    int32_t delPfxes);


extern struct kaps_jr1_lsn_mc *kaps_jr1_trie_node_pvt_recreate_lsn(
    kaps_jr1_trie_node * self);


extern struct kaps_jr1_lsn_mc *kaps_jr1_trie_node_pvt_create_lsn(
    kaps_jr1_trie_node * self);


void kaps_jr1_trie_node_remove_child_path(
    kaps_jr1_trie_node * self);


kaps_jr1_trie_node *NlmNsTrieNode__GiveOut(
    kaps_jr1_trie_node * self,
    kaps_jr1_trie_node * target_node);


void kaps_jr1_trie_node_set_pfx_bundle(
    kaps_jr1_trie_node * self,
    kaps_jr1_pfx_bundle * pfx);


NlmErrNum_t kaps_jr1_trie_node_update_iit(
    kaps_jr1_trie_node * self,
    NlmReasonCode * o_reason);


void kaps_jr1_trie_node_pvt_expand_trie(
    kaps_jr1_trie_node * self,
    uint32_t depth);



void kaps_jr1_trie_node_print(
    kaps_jr1_trie_node * self,
    FILE * fp);

kaps_jr1_trie_node *kaps_jr1_trie_node_get_path(
    kaps_jr1_trie_node * self,
    const uint8_t * pfxdata,
    uint32_t pfxlen);

kaps_jr1_trie_node *kaps_jr1_trie_node_insertPathFromPrefix(
    kaps_jr1_trie_node * self,
    kaps_jr1_prefix * prefix,
    uint32_t depth,
    uint32_t upto);

kaps_jr1_trie_node *kaps_jr1_trie_node_insertPathFromPrefixBundle(
    kaps_jr1_trie_node * self,
    kaps_jr1_pfx_bundle * pfxBundle);

void kaps_jr1_trie_node_update_iitLmpsofar(
    kaps_jr1_trie_node * self,
    kaps_jr1_trie_node * ancestorIptNode,
    kaps_jr1_pfx_bundle ** upStreamPfx_pp,
    kaps_jr1_pfx_bundle * rqtPfx,
    NlmTblRqtCode rqtCode);

void
kaps_jr1_trie_node_traverse_dfs_preorder(
    kaps_jr1_trie_node * node,
    kaps_jr1_trie_node_traverse_dfs_visitor_t func,
    NlmNsTrieNode__TraverseArgs * args);


void
kaps_jr1_trie_node_traverse_dfs_postorder(
    kaps_jr1_trie_node * node,
    kaps_jr1_trie_node_traverse_dfs_visitor_t func,
    NlmNsTrieNode__TraverseArgs * args);




int32_t kaps_jr1_trienode_wb_save_rpt(
    kaps_jr1_trie_node * rpt_node,
    uint32_t * nv_offset);

#endif 
