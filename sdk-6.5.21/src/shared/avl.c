/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Generic AVL Tree
 *
 * On creating a tree, the user specifies the size of the datum and the
 * FIXED maximum number of entries.  On insert/delete/lookup, the user
 * supplies the datum compare function.
 *
 * NOTE: These routines are NOT thread-safe.  If multiple threads could
 * access an AVL tree, external locks must be used to mutually exclude
 * each call to the shr_avl routines, including shr_avl_traverse().
 *
 * AVL tree code is derived from Paul Vixie's code (24-July-1987), which
 * used algorithms from "Algorithms & Data Structures," Niklaus Wirth,
 * Prentice-Hall 1986, ISBN 0-13-022005-1.
 */

#include <sal/types.h>
#include <shared/alloc.h>
#include <sal/core/libc.h>
#include <shared/avl.h>
#include <assert.h>

/*
 * Function:
 *	_shr_avl_entry_free
 * Purpose:
 *	Internal routine to put an entry back on the free list.
 *	The 'left' pointer is used as the free list 'next' pointer.
 */

STATIC INLINE void
_shr_avl_entry_free(shr_avl_t *avl, shr_avl_entry_t *t)
{
    assert(avl->count > 0);

    t->left = avl->free_list;
    avl->free_list = t;

    avl->count--;
}

/*
 * Function:
 *	_shr_avl_entry_alloc
 * Purpose:
 *	Internal routine to get an entry from free list.
 */

STATIC INLINE shr_avl_entry_t *
_shr_avl_entry_alloc(shr_avl_t *avl)
{
    shr_avl_entry_t		*t;

    t = avl->free_list;

    if (t != NULL) {
	avl->free_list = t->left;
	avl->count++;
    }

    return t;
}

/*
 * Function:
 *	_shr_avl_sprout
 * Purpose:
 *	Internal support routine
 */

STATIC int
_shr_avl_sprout(shr_avl_t *avl,
		shr_avl_entry_t **pt,
		shr_avl_compare_fn cmp_fn,
		shr_avl_datum_t *datum,
		int *balance)
{
    shr_avl_entry_t		*p1, *p2;
    int				i, rv;

    /*
     * Are we grounded?  If so, add the node "here",
     * set the rebalance flag, then exit.
     */

    if (*pt == NULL) {
	*pt = _shr_avl_entry_alloc(avl);

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
	rv = _shr_avl_sprout(avl, &(*pt)->left, cmp_fn, datum, balance);

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
	rv = _shr_avl_sprout(avl, &(*pt)->right, cmp_fn, datum, balance);

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
 * Function:
 *	_shr_avl_balance_l
 * Purpose:
 *	Internal support routine
 */

STATIC void
_shr_avl_balance_l(shr_avl_entry_t **pt, int *balance)
{
    shr_avl_entry_t		*p1, *p2;
    int				b1, b2;

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
	    p2->balance	= 0;
	}
    }
}

/*
 * Function:
 *	_shr_avl_balance_r
 * Purpose:
 *	Internal support routine
 */

STATIC void
_shr_avl_balance_r(shr_avl_entry_t **pt, int *balance)
{
    shr_avl_entry_t		*p1, *p2;
    int				b1, b2;

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
	    p2->balance	= 0;
	}
    }
}

/*
 * Function:
 *	_shr_avl_del
 * Purpose:
 *	Internal support routine
 */

STATIC void
_shr_avl_del(shr_avl_t *avl,
	     shr_avl_entry_t **pt,
	     int *balance,
	     shr_avl_entry_t **qqt)
{
    if ((*pt)->right != 0) {
	_shr_avl_del(avl, &(*pt)->right, balance, qqt);

	if (*balance) {
	    _shr_avl_balance_r(pt, balance);
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
 * Function:
 *	_shr_avl_delete
 * Purpose:
 *	Internal support routine
 */

STATIC int
_shr_avl_delete(shr_avl_t *avl,
		shr_avl_entry_t **pt,
		shr_avl_compare_fn cmp_fn,
		shr_avl_datum_t *datum,
		int *balance)
{
    shr_avl_entry_t		*t;
    int				ret;
    int				i;

    if (*pt == 0) {
	return 0;
    }

    i = (*cmp_fn)(avl->user_data, datum, &(*pt)->datum);

    if (i < 0) {
	ret = _shr_avl_delete(avl, &(*pt)->left, cmp_fn, datum, balance);

	if (*balance) {
	    _shr_avl_balance_l(pt, balance);
	}

	return ret;
    }

    if (i > 0) {
	ret = _shr_avl_delete(avl, &(*pt)->right, cmp_fn, datum, balance);

	if (*balance) {
	    _shr_avl_balance_r(pt, balance);
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
	_shr_avl_del(avl, &t->left, balance, &t);

	if (*balance) {
	    _shr_avl_balance_l(pt, balance);
	}
    }

    _shr_avl_entry_free(avl, t);

    return 1;
}

/*
 * Function:
 *	shr_avl_insert
 * Purpose:
 *	Insert a datum into the AVL tree.
 * Parameters:
 *	avl - AVL tree handle
 *	cmp_fn - Datum comparison function
 *	datum - Datum to insert
 * Returns:
 *	0	Success
 *	-1	Out of memory (tree full)
 * Notes:
 *	A duplicate key overwrites the old contents of the datum.
 */

int
shr_avl_insert(shr_avl_t *avl,
	       shr_avl_compare_fn cmp_fn,
	       shr_avl_datum_t *datum)
{
    int			rv, balance = 0;

    rv = _shr_avl_sprout(avl, &avl->root, cmp_fn, datum, &balance);

    return rv;
}

/*
 * Function:
 *	shr_avl_delete
 * Purpose:
 *	Delete a datum from the AVL tree.
 * Parameters:
 *	avl - AVL tree handle
 *	cmp_fn - Datum comparison function
 *	datum - Datum to delete; only key portion is used
 * Returns:
 *	0		Success, did not find datum
 *	1		Success, found and deleted datum
 */

int
shr_avl_delete(shr_avl_t *avl,
	       shr_avl_compare_fn key_cmp_fn,
	       shr_avl_datum_t *datum)
{
    int			rv, balance = 0;

    rv = _shr_avl_delete(avl, &avl->root, key_cmp_fn, datum, &balance);

    return rv;
}

/*
 * Function:
 *	_shr_avl_lookup
 * Purpose:
 *	Internal support routine
 */

STATIC int
_shr_avl_lookup(shr_avl_t *avl, shr_avl_entry_t *t,
		shr_avl_compare_fn key_cmp_fn,
		shr_avl_datum_t *datum)
{
    int			i;

    if (t == NULL) {
        return 0;
    }

    i = (*key_cmp_fn)(avl->user_data, datum, &t->datum);

    if (i < 0) {
	return _shr_avl_lookup(avl, t->left, key_cmp_fn, datum);
    }

    if (i > 0) {
	return _shr_avl_lookup(avl, t->right, key_cmp_fn, datum);
    }

    sal_memcpy(datum, &t->datum, avl->datum_bytes);

    return 1;
}

/*
 * Function:
 *	shr_avl_lookup
 * Purpose:
 *	Look up a datum in the AVL tree.
 * Parameters:
 *	avl - AVL tree handle
 *	datum - (IN) Datum in which key portion is valid
 *		(OUT) Datum in which key and data portion is valid if found
 *	key_cmp_fn - Compare function which should compare only the
 *		key portion of the datum.
 * Returns:
 *	0		Success, did not find datum
 *	1		Success, found key and updated rest of datum
 */

int
shr_avl_lookup(shr_avl_t *avl,
	       shr_avl_compare_fn key_cmp_fn,
	       shr_avl_datum_t *datum)
{
    if (avl->root == NULL) {
	return 0;
    }

    return _shr_avl_lookup(avl, avl->root, key_cmp_fn, datum);
}

/*
 * Function:
 *	_shr_avl_lookup_lkupdata
 * Purpose:
 *	Internal support routine
 */

STATIC int
_shr_avl_lookup_lkupdata(shr_avl_t *avl, shr_avl_entry_t *t,
                         shr_avl_compare_fn_lkupdata key_cmp_fn,
                         shr_avl_datum_t *datum,
                         void *lkupdata)
{
    int			i;

    if (t == NULL) {
        return 0;
    }

    i = (*key_cmp_fn)(avl->user_data, datum, &t->datum, lkupdata);

    if (i < 0) {
	return _shr_avl_lookup_lkupdata(avl, t->left, key_cmp_fn, datum, lkupdata);
    }

    if (i > 0) {
	return _shr_avl_lookup_lkupdata(avl, t->right, key_cmp_fn, datum, lkupdata);
    }

    sal_memcpy(datum, &t->datum, avl->datum_bytes);

    return 1;
}

/*
 * Function:
 *	shr_avl_lookup_lkupdata
 * Purpose:
 *	Look up a datum in the AVL tree. This function takes an additional argument
 *      lookup data. Users can for example capture the Pointer of the tree element inside
 *      compare function using user data. This facilitates return by address + value of
 *      tree element datum
 * Parameters:
 *	avl - AVL tree handle
 *	datum - (IN) Datum in which key portion is valid
 *		(OUT) Datum in which key and data portion is valid if found
 *	key_cmp_fn - Compare function which should compare only the
 *		key portion of the datum.
 * Returns:
 *	0		Success, did not find datum
 *	1		Success, found key and updated rest of datum
 */

int
shr_avl_lookup_lkupdata(shr_avl_t *avl,            
                        shr_avl_compare_fn_lkupdata key_cmp_fn,
                        shr_avl_datum_t *datum, /* KEY */
                        void *lkupdata)         /* User Data */   
{
    if (avl->root == NULL) {
	return 0;
    }

    return _shr_avl_lookup_lkupdata(avl, avl->root, key_cmp_fn, datum, lkupdata);
}

/*
 * Function:
 *	shr_avl_lookup_min
 * Purpose:
 *	Return the minimum element in the tree.
 * Parameters:
 *	avl - AVL tree
 *	datum - Datum to receive element.
 * Returns:
 *	0		Success, found and retrieved datum.
 *	-1		Tree empty.
 */

int shr_avl_lookup_min(shr_avl_t *avl,
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

/*
 * Function:
 *	shr_avl_lookup_max
 * Purpose:
 *	Return the maximum element in the tree.
 * Parameters:
 *	avl - AVL tree
 *	datum - Datum to receive element.
 * Returns:
 *	0		Success, found and retrieved datum.
 *	-1		Tree empty.
 */

int shr_avl_lookup_max(shr_avl_t *avl,
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

/*
 * Function:
 *      _shr_avl_delete_node
 * Purpose:
 *      Internal routine
 */

STATIC void
_shr_avl_delete_node(shr_avl_t *avl, shr_avl_entry_t **pt)
{
    if ((*pt) != NULL) {
        _shr_avl_delete_node(avl, &(*pt)->left);
        _shr_avl_delete_node(avl, &(*pt)->right);

        _shr_avl_entry_free(avl, (*pt));

        (*pt) = NULL;
    }
}

/*
 * Function:
 *	shr_avl_delete_all
 * Purpose:
 *	Reset an AVL tree to empty.
 *	Also used during tree creation to initialize free list.
 * Parameters:
 *	avl - AVL tree handle
 * Returns:
 *	0	Success
 */

int
shr_avl_delete_all(shr_avl_t *avl)
{
    _shr_avl_delete_node(avl, &avl->root);

    assert(avl->count == 0);

    return 0;
}

/*
 * Function:
 *	shr_avl_count
 * Purpose:
 *	Return the number of data in an AVL tree
 * Parameters:
 *	avl - AVL tree handle
 * Returns:
 *	Number of entries (0 if avl is NULL)
 */

int
shr_avl_count(shr_avl_t *avl)
{
    return ((avl == NULL) ? 0 : avl->count);
}

/*
 * Function:
 *	shr_avl_create
 * Purpose:
 *	Create an empty AVL tree
 * Parameters:
 *	avl_ptr - Return pointer (handle) to new AVL tree
 *	user_data - Arbitrary user data passed to compare routines
 *	datum_bytes - Size of datum being stored
 *	datum_max - Limit for number of data in table
 *	flags - Logical OR of SHR_AVL_FLAG_xxx
 * Returns:
 *	0	Success
 *	-1	Out of memory (system allocator)
 */

int
shr_avl_create(shr_avl_t **avl_ptr,
	       void *user_data,
	       int datum_bytes,
	       int datum_max)
{
    shr_avl_t		*avl;
    shr_avl_entry_t	*t;
    int			i;

    avl = sal_alloc(sizeof (shr_avl_t), "avl");

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

    avl->datum_base = sal_alloc(avl->entry_bytes * datum_max, "avl_ent");

    if (avl->datum_base == NULL) {
	sal_free(avl);
	return -1;
    }

    /* Initialize free list and put all entries on it */

    avl->free_list = NULL;
    avl->count = datum_max;

    for (i = 0; i < datum_max; i++) {
	t = (shr_avl_entry_t *) (avl->datum_base + i * avl->entry_bytes);
	_shr_avl_entry_free(avl, t);
    }

    assert(avl->count == 0);

    *avl_ptr = avl;

    return 0;
}

/*
 * Function:
 *	shr_avl_destroy
 * Purpose:
 *	Release the space occupied by AVL tree structure.
 * Parameters:
 *	avl - AVL tree handle
 */

int
shr_avl_destroy(shr_avl_t *avl)
{
    if (avl != NULL) {
	sal_free(avl->datum_base);
	sal_free(avl);
    }

    return 0;
}

/*
 * Function:
 *	_shr_avl_traverse
 * Purpose:
 *	Internal support routine
 * Notes:
 *	The callback routine may not modify the AVL tree being traversed.
 */

STATIC int
_shr_avl_traverse(shr_avl_t *avl,
		  shr_avl_entry_t *t,
		  shr_avl_traverse_fn trav_fn,
		  void *trav_data)
{
    int			rv;

    if (t != NULL) {
	if ((rv = _shr_avl_traverse(avl, t->left, trav_fn, trav_data)) < 0) {
	    return rv;
	}

	if ((rv = (*trav_fn)(avl->user_data, &t->datum, trav_data)) < 0) {
	    return rv;
	}

	if ((rv = _shr_avl_traverse(avl, t->right, trav_fn, trav_data)) < 0) {
	    return rv;
	}
    }

    return 0;
}

/*
 * Function:
 *	shr_avl_traverse
 * Purpose:
 *	Call a callback function for each datum in an AVL tree
 * Parameters:
 *	avl - AVL tree handle
 *	trav_fn - User callback function, called once per datum
 *	trav_data - Arbitrary user data passed to callback routine
 * Returns:
 *	If the user callback ever returns a negative value, the traversal
 *	is stopped and that value is returned.  Otherwise 0 is returned.
 */

int
shr_avl_traverse(shr_avl_t *avl,
		 shr_avl_traverse_fn trav_fn,
		 void *trav_data)
{
    if (avl == NULL) {
        return 0;
    }
    return _shr_avl_traverse(avl, avl->root, trav_fn, trav_data);
}
