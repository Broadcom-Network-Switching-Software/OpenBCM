/*! \file avl.c
 *
 * Generic AVL Tree
 *
 * AVL tree code is derived from Paul Vixie's code (24-July-1987), which
 * used algorithms from "Algorithms & Data Structures," Niklaus Wirth,
 * Prentice-Hall 1986, ISBN 0-13-022005-1.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <sal/sal_alloc.h>

#include <shr/shr_avl.h>

/*
 * Internal routine to put an entry back on the free list.  The 'left'
 * pointer is used as the free list 'next' pointer.
 */
static inline void
avl_entry_free(shr_avl_t *avl, shr_avl_entry_t *t)
{
    assert(avl->count > 0);

    t->left = avl->free_list;
    avl->free_list = t;

    avl->count--;
}

/*
 * Internal routine to get an entry from free list.
 */
static inline  shr_avl_entry_t *
avl_entry_alloc(shr_avl_t *avl)
{
    shr_avl_entry_t *t;

    t = avl->free_list;

    if (t != NULL) {
        avl->free_list = t->left;
        avl->count++;
    }

    return t;
}

/*
 * Internal support routine for inerting new nodes.
 */
static int
avl_sprout(shr_avl_t *avl,
           shr_avl_entry_t **pt,
           shr_avl_compare_fn cmp_fn,
           shr_avl_datum_t *datum,
           int *balance)
{
    shr_avl_entry_t *p1, *p2;
    int i, rv;

    /*
     * Are we grounded?  If so, add the node "here",
     * set the rebalance flag, then exit.
     */

    if (*pt == NULL) {
        *pt = avl_entry_alloc(avl);

        if (*pt == NULL) {
            return -1;
        }

        (*pt)->left = NULL;
        (*pt)->right = NULL;
        (*pt)->balance = 0;

        sal_memcpy(&(*pt)->datum, datum, avl->datum_bytes);

        *balance = 1;

        return 0;
    }

    /*
     * If less, prepare to move to the left.
     */

    i = (*cmp_fn)(avl->user_data, datum, &(*pt)->datum);

    if (i < 0) {
        rv = avl_sprout(avl, &(*pt)->left, cmp_fn, datum, balance);

        if (rv < 0) {
            return rv;
        }

        if (*balance == 0) {
            return 0;
        }

        /* Left branch has grown longer */

        switch ((*pt)->balance) {
        case 1: /* Right branch was longer; balance is ok now */
            (*pt)->balance = 0;
            *balance = 0;
            break;
        case 0: /* Balance was okay; now left branch longer */
            (*pt)->balance = -1;
            break;
        case -1:
            /* Left branch was already too long. rebalance */
            p1 = (*pt)->left;

            if (p1->balance == -1) { /* LL */
                (*pt)->left = p1->right;
                p1->right = *pt;
                (*pt)->balance = 0;
                *pt = p1;
            } else {   /* Double LR */
                p2 = p1->right;
                p1->right = p2->left;
                p2->left = p1;

                (*pt)->left = p2->right;
                p2->right = *pt;

                if (p2->balance == -1) {
                    (*pt)->balance = 1;
                } else {
                    (*pt)->balance = 0;
                }

                if (p2->balance == 1) {
                    p1->balance = -1;
                } else {
                    p1->balance = 0;
                }

                *pt = p2;
            }

            (*pt)->balance = 0;
            *balance = 0;
        }

        return 0;
    }

    /*
     * If more, prepare to move to the right.
     */

    if (i > 0) {
        rv = avl_sprout(avl, &(*pt)->right, cmp_fn, datum, balance);

        if (rv < 0) {
            return rv;
        }

        if (*balance == 0) {
            return 0;
        }

        /* Right branch has grown longer */

        switch ((*pt)->balance) {
        case -1:
            (*pt)->balance = 0;
            *balance = 0;
            break;
        case 0:
            (*pt)->balance = 1;
            break;
        case 1:
            p1 = (*pt)->right;

            if (p1->balance == 1) { /* RR */
                (*pt)->right = p1->left;
                p1->left = *pt;
                (*pt)->balance = 0;
                *pt = p1;
            } else {   /* Double RL */
                p2 = p1->left;
                p1->left = p2->right;
                p2->right = p1;

                (*pt)->right = p2->left;
                p2->left = *pt;

                if (p2->balance == 1) {
                    (*pt)->balance = -1;
                } else {
                    (*pt)->balance = 0;
                }

                if (p2->balance == -1) {
                    p1->balance = 1;
                } else {
                    p1->balance = 0;
                }

                *pt = p2;
            }

            (*pt)->balance = 0;
            *balance = 0;
        }

        return 0;
    }

    /*
     * If same, just gets replaced.
     */

    *balance = 0;

    sal_memcpy(&(*pt)->datum, datum, avl->datum_bytes);

    return 0;
}

/*
 * Internal support routine for balancing tree (left).
 */
static void
avl_balance_l(shr_avl_entry_t **pt, int *balance)
{
    shr_avl_entry_t *p1, *p2;
    int b1, b2;

    switch ((*pt)->balance) {
    case -1:
        (*pt)->balance = 0;
        break;
    case 0:
        (*pt)->balance = 1;
        *balance = 0;
        break;
    case 1:
        p1 = (*pt)->right;
        b1 = p1->balance;

        if (b1 >= 0) {
            (*pt)->right = p1->left;
            p1->left = *pt;

            if (b1 == 0) {
                (*pt)->balance = 1;
                p1->balance = -1;
                *balance = 0;
            } else {
                (*pt)->balance = 0;
                p1->balance = 0;
            }

            *pt = p1;
        } else {
            p2 = p1->left;
            b2 = p2->balance;
            p1->left = p2->right;
            p2->right = p1;
            (*pt)->right = p2->left;
            p2->left = *pt;

            if (b2 == 1) {
                (*pt)->balance = -1;
            } else {
                (*pt)->balance = 0;
            }

            if (b2 == -1) {
                p1->balance = 1;
            } else {
                p1->balance = 0;
            }

            *pt = p2;
            p2->balance = 0;
        }
    }
}

/*
 * Internal support routine for balancing tree (right).
 */
static void
avl_balance_r(shr_avl_entry_t **pt, int *balance)
{
    shr_avl_entry_t *p1, *p2;
    int b1, b2;

    switch ((*pt)->balance) {
    case 1:
        (*pt)->balance = 0;
        break;
    case 0:
        (*pt)->balance = -1;
        *balance = 0;
        break;
    case -1:
        p1 = (*pt)->left;
        b1 = p1->balance;

        if (b1 <= 0) {
            (*pt)->left = p1->right;
            p1->right = *pt;

            if (b1 == 0) {
                (*pt)->balance = -1;
                p1->balance = 1;
                *balance = 0;
            } else {
                (*pt)->balance = 0;
                p1->balance = 0;
            }

            *pt = p1;
        } else {
            p2 = p1->right;
            b2 = p2->balance;
            p1->right = p2->left;
            p2->left = p1;
            (*pt)->left = p2->right;
            p2->right = *pt;

            if (b2 == -1) {
                (*pt)->balance = 1;
            } else {
                (*pt)->balance = 0;
            }

            if (b2 == 1) {
                p1->balance = -1;
            } else {
                p1->balance = 0;
            }

            *pt = p2;
            p2->balance = 0;
        }
    }
}

/*
 * Internal support routine for node deletion.
 */
static void
avl_del(shr_avl_t *avl,
        shr_avl_entry_t **pt,
        int *balance,
        shr_avl_entry_t **qqt)
{
    if ((*pt)->right != 0) {
        avl_del(avl, &(*pt)->right, balance, qqt);

        if (*balance) {
            avl_balance_r(pt, balance);
        }
    } else {
        if(avl->datum_copy_fn) {
            avl->datum_copy_fn(avl->user_data, &(*qqt)->datum,  &(*pt)->datum);
        } else {
            sal_memcpy(&(*qqt)->datum, &(*pt)->datum, avl->datum_bytes);
        }
        *qqt = *pt;
        *pt = (*pt)->left;
        *balance = 1;
    }
}

/*
 * Internal support routine for shr_avl_delete.
 */
static int
avl_delete(shr_avl_t *avl,
           shr_avl_entry_t **pt,
           shr_avl_compare_fn cmp_fn,
           shr_avl_datum_t *datum,
           int *balance)
{
    shr_avl_entry_t *t;
    int ret;
    int i;

    if (*pt == 0) {
        return 0;
    }

    i = (*cmp_fn)(avl->user_data, datum, &(*pt)->datum);

    if (i < 0) {
        ret = avl_delete(avl, &(*pt)->left, cmp_fn, datum, balance);

        if (*balance) {
            avl_balance_l(pt, balance);
        }

        return ret;
    }

    if (i > 0) {
        ret = avl_delete(avl, &(*pt)->right, cmp_fn, datum, balance);

        if (*balance) {
            avl_balance_r(pt, balance);
        }

        return ret;
    }

    t = *pt;

    if (t->right == NULL) {
        *pt = t->left;
        *balance = 1;
    } else if (t->left == NULL) {
        *pt = t->right;
        *balance = 1;
    } else {
        avl_del(avl, &t->left, balance, &t);

        if (*balance) {
            avl_balance_l(pt, balance);
        }
    }

    avl_entry_free(avl, t);

    return 1;
}

/*
 * Internal support routine for deleting a single node.
 */
static void
avl_delete_node(shr_avl_t *avl, shr_avl_entry_t **pt)
{
    if ((*pt) != NULL) {
        avl_delete_node(avl, &(*pt)->left);
        avl_delete_node(avl, &(*pt)->right);

        avl_entry_free(avl, (*pt));

        (*pt) = NULL;
    }
}

/*
 * Internal support routine for shr_avl_lookup.
 */
static int
avl_lookup(shr_avl_t *avl, shr_avl_entry_t *t,
           shr_avl_compare_fn key_cmp_fn,
           shr_avl_datum_t *datum)
{
    int i;

    if (t == NULL) {
        return 0;
    }

    i = (*key_cmp_fn)(avl->user_data, datum, &t->datum);

    if (i < 0) {
        return avl_lookup(avl, t->left, key_cmp_fn, datum);
    }

    if (i > 0) {
        return avl_lookup(avl, t->right, key_cmp_fn, datum);
    }

    sal_memcpy(datum, &t->datum, avl->datum_bytes);

    return 1;
}

/*
 * Internal support routine for shr_avl_lookup_lkupdata.
 */
static int
avl_lookup_lkupdata(shr_avl_t *avl, shr_avl_entry_t *t,
                    shr_avl_compare_fn_lkupdata key_cmp_fn,
                    shr_avl_datum_t *datum,
                    void *lkupdata)
{
    int i;

    if (t == NULL) {
        return 0;
    }

    i = (*key_cmp_fn)(avl->user_data, datum, &t->datum, lkupdata);

    if (i < 0) {
        return avl_lookup_lkupdata(avl, t->left, key_cmp_fn, datum, lkupdata);
    }

    if (i > 0) {
        return avl_lookup_lkupdata(avl, t->right, key_cmp_fn, datum, lkupdata);
    }

    sal_memcpy(datum, &t->datum, avl->datum_bytes);

    return 1;
}

/*
 * Internal support routine for shr_avl_traverse.
 */
static int
avl_traverse(shr_avl_t *avl,
             shr_avl_entry_t *t,
             shr_avl_traverse_fn trav_fn,
             void *trav_data)
{
    int rv;

    if (t != NULL) {
        if ((rv = avl_traverse(avl, t->left, trav_fn, trav_data)) < 0) {
            return rv;
        }

        if ((rv = (*trav_fn)(avl->user_data, &t->datum, trav_data)) < 0) {
            return rv;
        }

        if ((rv = avl_traverse(avl, t->right, trav_fn, trav_data)) < 0) {
            return rv;
        }
    }

    return 0;
}

int
shr_avl_create(shr_avl_t **avl_ptr,
               void *user_data,
               int datum_bytes,
               int datum_max)
{
    shr_avl_t *avl;
    shr_avl_entry_t *t;
    int i;

    avl = sal_alloc(sizeof (shr_avl_t), "shrAvl");

    if (avl == NULL) {
        return -1;
    }

    sal_memset(avl, 0, sizeof (*avl));

    avl->user_data = user_data;
    avl->datum_bytes = datum_bytes;
    avl->datum_max = datum_max;
    avl->entry_bytes = (sizeof (shr_avl_entry_t) -
                        sizeof (shr_avl_datum_t) + datum_bytes);
    avl->root = NULL;
    avl->datum_copy_fn = NULL;

    /* Pre-allocate maximum number of entries and put on free list. */

    avl->datum_base = sal_alloc(avl->entry_bytes * datum_max, "shrAvlEnt");

    if (avl->datum_base == NULL) {
        sal_free(avl);
        return -1;
    }

    /* Initialize free list and put all entries on it */

    avl->free_list = NULL;
    avl->count = datum_max;

    for (i = 0; i < datum_max; i++) {
        t = (shr_avl_entry_t *) (avl->datum_base + i * avl->entry_bytes);
        avl_entry_free(avl, t);
    }

    assert(avl->count == 0);

    *avl_ptr = avl;

    return 0;
}

int
shr_avl_destroy(shr_avl_t *avl)
{
    if (avl != NULL) {
        sal_free(avl->datum_base);
        sal_free(avl);
    }

    return 0;
}

int
shr_avl_insert(shr_avl_t *avl,
               shr_avl_compare_fn cmp_fn,
               shr_avl_datum_t *datum)
{
    int rv;
    int balance = 0;

    rv = avl_sprout(avl, &avl->root, cmp_fn, datum, &balance);

    return rv;
}

int
shr_avl_delete(shr_avl_t *avl,
               shr_avl_compare_fn key_cmp_fn,
               shr_avl_datum_t *datum)
{
    int rv;
    int balance = 0;

    rv = avl_delete(avl, &avl->root, key_cmp_fn, datum, &balance);

    return rv;
}

int
shr_avl_delete_all(shr_avl_t *avl)
{
    avl_delete_node(avl, &avl->root);

    assert(avl->count == 0);

    return 0;
}

int
shr_avl_count(shr_avl_t *avl)
{
    return ((avl == NULL) ? 0 : avl->count);
}

int
shr_avl_lookup(shr_avl_t *avl,
               shr_avl_compare_fn key_cmp_fn,
               shr_avl_datum_t *datum)
{
    if (avl->root == NULL) {
        return 0;
    }

    return avl_lookup(avl, avl->root, key_cmp_fn, datum);
}

int
shr_avl_lookup_lkupdata(shr_avl_t *avl,
                        shr_avl_compare_fn_lkupdata key_cmp_fn,
                        shr_avl_datum_t *datum,
                        void *lkupdata)
{
    if (avl->root == NULL) {
        return 0;
    }

    return avl_lookup_lkupdata(avl, avl->root, key_cmp_fn, datum, lkupdata);
}

int
shr_avl_lookup_min(shr_avl_t *avl,
                   shr_avl_datum_t *datum)
{
    shr_avl_entry_t *t;

    if (avl->root == NULL) {
        return -1;
    }

    for (t = avl->root; t->left != NULL; t = t->left)
        ;

    sal_memcpy(datum, &t->datum, avl->datum_bytes);

    return 0;
}

int
shr_avl_lookup_max(shr_avl_t *avl,
                   shr_avl_datum_t *datum)
{
    shr_avl_entry_t *t;

    if (avl->root == NULL) {
        return -1;
    }

    for (t = avl->root; t->right != NULL; t = t->right)
        ;

    sal_memcpy(datum, &t->datum, avl->datum_bytes);

    return 0;
}

int
shr_avl_traverse(shr_avl_t *avl,
                 shr_avl_traverse_fn trav_fn,
                 void *trav_data)
{
    if (avl == NULL) {
        return 0;
    }
    return avl_traverse(avl, avl->root, trav_fn, trav_data);
}
