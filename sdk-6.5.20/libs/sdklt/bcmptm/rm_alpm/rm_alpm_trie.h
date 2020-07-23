/*! \file rm_alpm_trie.h
 *
 * Trie routines internal to RM ALPM
 *
 * This file contains trie routines which are internal to
 * ALPM Resource Manager
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef RM_ALPM_TRIE_H
#define RM_ALPM_TRIE_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <shr/shr_lmem_mgr.h>
#include <shr/shr_dq.h>

/*******************************************************************************
 * Defines
 */
/*
 * This macro is a tidy way of performing subtraction to move from a
 * pointer within an object to a pointer to the object.
 *
 * Arguments are:
 *    type of object to recover
 *    pointer to object from which to recover element pointer
 *    pointer to an object of type t
 *    name of the trie node field in t through which the object is linked on
 *    trie
 * Returns:
 *    a pointer to the object, of type t
 */
#define TRIE_ELEMENT(t, p, ep, f) \
    ((t) (((char *) (p)) - (((char *) &((ep)->f)) - ((char *) (ep)))))

/*
 * TRIE_ELEMENT_GET performs the same function as TRIE_ELEMENT, but does not
 * require a pointer of type (t).  This form is preferred as TRIE_ELEMENT
 * typically generate Coverity errors, and the (ep) argument is unnecessary.
 *
 * Arguments are:
 *    type of object to recover
 *    pointer to object from which to recover element pointer
 *    name of the trie node field in t through which the object is linked on
 *    trie
 * Returns:
 *    a pointer to the object, of type t
 */
#define TRIE_ELEMENT_GET(t, p, f) \
    ((t) (((char *) (p)) - (((char *) &(((t)(0))->f)))))


#define CLONE_TRIE_NODE(dest, src) \
    sal_memcpy((dest), (src), sizeof(rm_alpm_trie_node_t))

#define    INTERNAL     0
#define    PAYLOAD      1
#define    MAX          2



/*******************************************************************************
 * Typedefs
 */
 typedef enum trie_propagate_op_e {
    PROPOGATE_INSERT = 0,
    PROPOGATE_DELETE = 1,
    PROPOGATE_UPDATE = 2,
    PROPOGATE_HIT = 3,
    PROPOGATE_DONT_CARE = 4
} trie_propagate_op_t;


typedef struct rm_alpm_trie_node_s rm_alpm_trie_node_t;
typedef uint8_t rm_alpm_node_type_e_t;

typedef struct child_node_s {
    rm_alpm_trie_node_t *child_node;
} rm_alpm_child_node_t;

struct rm_alpm_trie_node_s {
    /* rm_alpm_trie_node_t     *trie_node; */
#define MAX_CHILD     (2)
    rm_alpm_child_node_t    child[MAX_CHILD];
    rm_alpm_node_type_e_t   type;
    uint16_t        skip_len;
    uint32_t        skip_addr;
    uint32_t        count;     /* number of payload node counts */
    uint32_t        bpm;       /* best prefix match bit map - 32 bits */
};

typedef struct rm_alpm_trie_s {
    rm_alpm_trie_node_t *trie;     /* trie root pointer */
    uint32_t    mkl;       /* max_key_len*/
} rm_alpm_trie_t;

typedef struct rm_alpm_trie_bpm_cb_info_s {
    uint32_t        *pfx;     /* prefix buffer pointer from caller space */
    uint32_t        len;      /* prefix length */
    void            *user_data;
} rm_alpm_trie_bpm_cb_info_t;

typedef enum rm_alpm_trie_traverse_order_e_s {
    TRAVERSE_PREORDER,  /* root, left, right */
    TRAVERSE_INORDER,   /* left, root, right */
    TRAVERSE_POSTORDER, /* left, right, root */
    TRAVERSE_BFS,       /* Level by level */
    TRAVERSE_ORDER_MAX
} rm_alpm_trie_traverse_order_e_t;

typedef enum rm_alpm_trie_traverse_states_e_s {
    TRAVERSE_STATE_NONE,
    TRAVERSE_STATE_DELETED,
    TRAVERSE_STATE_DONE,
    TRAVERSE_STATE_MAX
} rm_alpm_trie_traverse_states_e_t;

typedef int (*rm_alpm_trie_callback_f)(rm_alpm_trie_node_t *trie, void *datum);

typedef int (*rm_alpm_trie_propagate_cb_f)(rm_alpm_trie_node_t *trie,
                                           rm_alpm_trie_bpm_cb_info_t *info);

typedef int (*rm_alpm_trie_callback_ext_f)(rm_alpm_trie_node_t *ptrie,
                                           rm_alpm_trie_node_t *trie,
                                           rm_alpm_trie_traverse_states_e_t *state,
                                           void *info);
typedef int (*rm_alpm_trie_clone_callback_f)(rm_alpm_trie_node_t *trie,
                                             rm_alpm_trie_node_t **clone_trie,
                                             void *info);

typedef int (*rm_alpm_trie_repart_callback_f)(rm_alpm_trie_node_t *ptrie,
                                              rm_alpm_trie_node_t *trie,
                                              rm_alpm_trie_traverse_states_e_t *state,
                                              void *info,
                                              rm_alpm_trie_node_t **new_ptrie);

/*******************************************************************************
 * Global variables
 */


/*******************************************************************************
 * Function prototypes
 */


/*
 * Function:
 *     bcmptm_rm_alpm_trie_create
 * Purpose:
 *     allocates a trie & initializes it
 */
extern int bcmptm_rm_alpm_trie_create(int u, int m, uint32_t       max_key_len,
                                      rm_alpm_trie_t **ptrie);

/*
 * Function:
 *     bcmptm_rm_alpm_trie_destroy
 * Purpose:
 *     destroys a trie
 */
extern int bcmptm_rm_alpm_trie_destroy(int u, int m, rm_alpm_trie_t *trie);

/*
 * Function:
 *     bcmptm_rm_alpm_trie_insert
 * Purpose:
 *     Inserts provided prefix/length in to the trie
 */
extern int bcmptm_rm_alpm_trie_insert(int u, int m,
                                      rm_alpm_trie_t      *trie,
                                      uint32_t            *key,
                                      uint32_t            *bpm,
                                      uint32_t            length,
                                      rm_alpm_trie_node_t *payload);

/*
 * Function:
 *     bcmptm_rm_alpm_trie_delete
 * Purpose:
 *     Deletes provided prefix/length in to the trie
 */
extern int bcmptm_rm_alpm_trie_delete(int u, int m,
                                      rm_alpm_trie_t      *trie,
                                      uint32_t            *key,
                                      uint32_t            length,
                                      rm_alpm_trie_node_t **payload);

/*
 * Function:
 *     bcmptm_rm_alpm_trie_search
 * Purpose:
 *     Search the given trie for provided prefix/length
 */
extern int bcmptm_rm_alpm_trie_search(rm_alpm_trie_t      *trie,
                                      uint32_t            *key,
                                      uint32_t            length,
                                      rm_alpm_trie_node_t **payload);

/*
 * Function:
 *     bcmptm_rm_alpm_trie_search_verbose
 * Purpose:
 *     Search the given trie for provided prefix/length,
 *     return the matched prefix/length
 */
extern int bcmptm_rm_alpm_trie_search_verbose(rm_alpm_trie_t      *trie,
                                              uint32_t            *key,
                                              uint32_t            length,
                                              rm_alpm_trie_node_t **payload,
                                              uint32_t            *result_key,
                                              uint32_t            *result_len);
/*
 * Function:
 *     bcmptm_rm_alpm_trie_dump
 * Purpose:
 *     Dumps the trie pre-order [root|left|child]
 */
extern int bcmptm_rm_alpm_trie_dump(rm_alpm_trie_t          *trie,
                                    rm_alpm_trie_callback_f cb,
                                    void                    *user_data);

/*
 * Function:
 *     bcmptm_rm_alpm_trie_dump2
 * Purpose:
 *     Dumps the trie pre-order [root|left|child]
 */
extern int bcmptm_rm_alpm_trie_dump2(rm_alpm_trie_node_t *trie,
                                     rm_alpm_trie_callback_f cb,
                                     void *user_data,
                                     uint32_t mkl);

/*
 * Function:
 *     bcmptm_rm_alpm_trie_find_lpm
 * Purpose:
 *     Find the longest prefix matched with given prefix
 */
extern int bcmptm_rm_alpm_trie_find_lpm(rm_alpm_trie_t      *trie,
                                        uint32_t            *key,
                                        uint32_t            length,
                                        rm_alpm_trie_node_t **payload,
                                        uint32_t            *lpm_pfx,
                                        uint32_t            *lpm_len);
/*
 * Function:
 *     bcmptm_rm_alpm_trie_find_lpm2
 * Purpose:
 *     Find the longest prefix matched with given prefix, exclude itself.
 */
extern int bcmptm_rm_alpm_trie_find_lpm2(rm_alpm_trie_t      *trie,
                                         uint32_t            *key,
                                         uint32_t            length,
                                         rm_alpm_trie_node_t **payload);

/*
 * Function:
 *     bcmptm_rm_alpm_trie_find_pm
 * Purpose:
 *     Find the prefix matched nodes with given prefix and callback
 *     with specified callback funtion and user data
 */
extern int bcmptm_rm_alpm_trie_find_pm(rm_alpm_trie_t          *trie,
                                       uint32_t                *key,
                                       uint32_t                length,
                                       rm_alpm_trie_callback_f cb,
                                       void                    *user_data);

/*
 * Function:
 *     bcmptm_rm_alpm_trie_find_prefix_bpm
 * Purpose:
 *    Given a key/length return the Best prefix match length
 *    key/bpm_pfx_len will be the BPM for the key/length
 */
extern int bcmptm_rm_alpm_trie_find_prefix_bpm(rm_alpm_trie_t *trie,
                                               uint32_t       *key,
                                               uint32_t       length,
                                               uint32_t       *bpm_pfx_len);

/*
 * Function:
 *     bcmptm_rm_alpm_trie_bpm_mask_get
 * Purpose:
 *     Get the bpm mask of target key. This key is already in the trie.
 */
extern int bcmptm_rm_alpm_trie_bpm_mask_get(rm_alpm_trie_t *trie,
                                            uint32_t       *key,
                                            uint32_t       length,
                                            uint32_t       *bpm_mask);
/*
 * Function:
 *     bcmptm_rm_alpm_trie_split
 * Purpose:
 *     Split the trie into 2 based on optimum pivot
 */
extern int bcmptm_rm_alpm_trie_split(int u, int m,
                                     rm_alpm_trie_t      *trie,
                                     const uint32_t      max_split_len,
                                     uint32_t            *pivot,
                                     uint32_t            *length,
                                     rm_alpm_trie_node_t **split_trie_root,
                                     rm_alpm_trie_node_t **split_trie_node,
                                     uint32_t            *bpm,
                                     /* if set split will strictly split only on
                                      * payload nodes, if not set splits at
                                      * optimal point on the trie
                                      */
                                     uint32_t            payload_node_split,
                                     int                 max_split_count);

/*
 * Function:
 *     bcmptm_rm_alpm_trie_merge
 * Purpose:
 *     unsplit or fuse the child trie with parent trie
 */
extern int bcmptm_rm_alpm_trie_merge(int u, int m,
                                     rm_alpm_trie_t      *parent_trie,
                                     rm_alpm_trie_node_t *child_trie,
                                     uint32_t            *child_pivot,
                                     uint32_t            length);
/*
 * Function:
 *     bcmptm_rm_alpm_trie_split2
 * Purpose:
 *     Split the trie such that the new sub trie covers given prefix/length.
 *     Basically this is a reverse of bcmptm_rm_alpm_trie_merge.
 */
extern int bcmptm_rm_alpm_trie_split2(int u, int m,
                                      rm_alpm_trie_t      *trie,
                                      uint32_t            *key,
                                      uint32_t            key_len,
                                      uint32_t            *pivot,
                                      uint32_t            *pivot_len,
                                      rm_alpm_trie_node_t **split_trie_root);

/*!
 * \brief Clone a new trie
 *
 * \param [in] trie_src Trie cloned from
 * \param [in] clone_cb Payload clone callback
 * \param [in] user_data User data
 * \param [out] trie_dst Trie cloned to
 * \param [out] clone_hdl Local memory handle
 *
 * \return SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_trie_clone_create(int u, int m,
                                 rm_alpm_trie_t *trie_src,
                                 rm_alpm_trie_clone_callback_f clone_cb,
                                 void *user_data,
                                 rm_alpm_trie_t **trie_dst,
                                 shr_lmm_hdl_t *clone_hdl);

/*!
 * \brief Destroy a cloned trie
 *
 * \param [in] trie Trie to be destroyed
 * \param [in] clone_hdl Local memory handle
 *
 * \return nothing
 */
extern void
bcmptm_rm_alpm_trie_clone_destroy(int u, int m, rm_alpm_trie_t *trie,
                                  shr_lmm_hdl_t clone_hdl);

/*!
 * \brief Return payload count of a trie
 *
 * \param [in] trie Trie
 *
 * \return Trie count
 */
extern int
bcmptm_rm_alpm_trie_count(rm_alpm_trie_t *trie);

extern int bcmptm_rm_alpm_trie_compare(rm_alpm_trie_t *trie_src,
                                       rm_alpm_trie_t *trie_dst,
                                       int            *equal);

/*
 * Function:
 *     bcmptm_rm_alpm_trie_traverse
 * Purpose:
 *     Traverse the trie & call the application callback with user data
 */
extern int bcmptm_rm_alpm_trie_traverse(int                             u, int m,
                                        rm_alpm_trie_node_t             *trie,
                                        rm_alpm_trie_callback_f         cb,
                                        void                            *user_data,
                                        rm_alpm_trie_traverse_order_e_t order);

extern int bcmptm_rm_alpm_trie_traverse_get_next(int u, int m, bool get_first,
                                                 rm_alpm_trie_node_t *trie,
                                                 shr_dq_t *queue,
                                                 rm_alpm_trie_callback_f cb,
                                                 void *user_data,
                                                 void **payload);

/*
 * Function:
 *     bcmptm_rm_alpm_trie_traverse2
 * Purpose:
 *     Traverse the trie (PAYLOAD) & call the extended application callback
 *     which has current node's PAYLOAD parent node with user data.
 */
extern int bcmptm_rm_alpm_trie_traverse2(rm_alpm_trie_node_t             *trie,
                                         rm_alpm_trie_callback_ext_f     cb,
                                         void                            *user_data,
                                         rm_alpm_trie_traverse_order_e_t order);

/*
 * Function:
 *     bcmptm_rm_alpm_trie_repartition
 * Purpose:
 *     Traverse the trie & call the extended application callback
 *     which has current node's parent node with user data.
 */
extern int bcmptm_rm_alpm_trie_repartition(rm_alpm_trie_t *trie,
                                           rm_alpm_trie_repart_callback_f cb,
                                           void *user_data,
                                           rm_alpm_trie_traverse_order_e_t order);


/*
 * Function:
 *     bcmptm_rm_alpm_trie_pivot_propagate_prefix
 * Purpose:
 *   Propogate prefix BPM from a given pivot.
 */
extern int bcmptm_rm_alpm_trie_propagate(rm_alpm_trie_t              *trie,
                                         uint32_t                    *pfx,
                                         uint32_t                    len,
                                         trie_propagate_op_t         op,
                                         rm_alpm_trie_propagate_cb_f cb,
                                         rm_alpm_trie_bpm_cb_info_t  *cb_info,
                                         bool                        bpm_aware);

extern int bcmptm_rm_alpm_trie_pivot_propagate(uint32_t mkl,
                                               rm_alpm_trie_node_t *pivot,
                                               uint32_t pivot_len,
                                               uint32_t *pfx,
                                               uint32_t len,
                                               trie_propagate_op_t op,
                                               rm_alpm_trie_propagate_cb_f cb,
                                               rm_alpm_trie_bpm_cb_info_t *cb_info,
                                               bool bpm_aware);

/*
 * Function:
 *     bcmptm_rm_alpm_trie_iter_get_first
 * Purpose:
 *     Traverse the trie & return pointer to first payload node
 */
extern int bcmptm_rm_alpm_trie_iter_get_first(rm_alpm_trie_t      *trie,
                                              rm_alpm_trie_node_t **payload);


extern int bcmptm_rm_alpm_trie_iter_get_first2(rm_alpm_trie_node_t *trie,
                                               rm_alpm_trie_node_t **payload);

extern int bcmptm_rm_alpm_trie_swap(rm_alpm_trie_t *trie_e, /* to be replaced */
                                    rm_alpm_trie_t *trie_n);

/*!
 * \brief Initiliaze ALPM trie sub module.
 *
 * \param [in]  multi_thread is a boolean indicating true if multi threads
 *              can allocate/free elements from this memory and false otherwise
 *
 * \return SHR_E_NONE/SHR_E_MEMORY.
 */

extern int bcmptm_rm_alpm_trie_init(int u, int m, bool multi_thread);


extern void
bcmptm_rm_alpm_trie_traverse_cache_cleanup(int u, int m);

/*!
 * \brief Cleanup ALPM trie sub module.
 *
 * \return nothing.
 */
extern void bcmptm_rm_alpm_trie_cleanup(int u, int m);

#endif /* RM_ALPM_TRIE_H */

