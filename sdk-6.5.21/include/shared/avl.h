/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: 	avl.h
 * Purpose: 	Defines a generic AVL tree data structure.
 */

#ifndef _SHR_AVL_H
#define _SHR_AVL_H

/*
 * NOTE:
 *
 *   sizeof (shr_avl_datum_t) is actually datum_bytes.
 *   sizeof (shr_avl_entry_t) is also correspondingly larger.
 */

typedef int shr_avl_datum_t;

typedef int (*shr_avl_compare_fn)(void *user_data,
				  shr_avl_datum_t *datum1,
				  shr_avl_datum_t *datum2);

typedef int (*shr_avl_compare_fn_lkupdata)(void *user_data,
				  shr_avl_datum_t *datum1,
				  shr_avl_datum_t *datum2,
                                  void *lkupdata);

typedef int (*shr_avl_traverse_fn)(void *user_data,
				   shr_avl_datum_t *datum,
				   void *trav_data);

typedef int (*shr_avl_datum_copy_fn)(void *user_data,
				  shr_avl_datum_t *datum1,
                                     shr_avl_datum_t *datum2);

typedef struct shr_avl_entry_s {
    struct shr_avl_entry_s	*left;
    struct shr_avl_entry_s	*right;
    int				balance;
    shr_avl_datum_t		datum;	    /* NOTE: variable size field */
} shr_avl_entry_t;

typedef struct shr_avl_s {
    /* Static data configured on tree creation */
    void			*user_data;
    int				datum_bytes;
    int				datum_max;
    int				entry_bytes;

    /* Dynamic data */
    char			*datum_base;
    shr_avl_entry_t		*root;
    shr_avl_entry_t		*free_list;
    int				count;		/* Number entries in tree */
    shr_avl_datum_copy_fn       datum_copy_fn;
} shr_avl_t;

extern int shr_avl_create(shr_avl_t **avl_ptr,
			  void *user_data,
			  int datum_bytes,
			  int datum_max);

extern int shr_avl_destroy(shr_avl_t *avl);

extern int shr_avl_insert(shr_avl_t *avl,
			  shr_avl_compare_fn cmp_fn,
			  shr_avl_datum_t *datum);

extern int shr_avl_delete(shr_avl_t *avl,
			  shr_avl_compare_fn key_cmp_fn,
			  shr_avl_datum_t *datum);

extern int shr_avl_delete_all(shr_avl_t *avl);

extern int shr_avl_count(shr_avl_t *avl);

extern int shr_avl_lookup(shr_avl_t *avl,
			  shr_avl_compare_fn key_cmp_fn,
			  shr_avl_datum_t *datum);

extern int shr_avl_lookup_lkupdata(shr_avl_t *avl,            
                                   shr_avl_compare_fn_lkupdata key_cmp_fn,
                                   shr_avl_datum_t *datum,
                                   void *userdata);    

extern int shr_avl_lookup_min(shr_avl_t *avl,
			      shr_avl_datum_t *datum);

extern int shr_avl_lookup_max(shr_avl_t *avl,
			      shr_avl_datum_t *datum);

extern int shr_avl_traverse(shr_avl_t *avl,
			    shr_avl_traverse_fn trav_fn,
			    void *trav_data);

#ifdef BROADCOM_DEBUG
extern void shr_avl_dump(shr_avl_t *avl);
#endif /* BROADCOM_DEBUG */

#endif	/* !_SHR_AVL_H */
