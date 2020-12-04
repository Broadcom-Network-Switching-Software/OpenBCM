/*! \file shr_avl.h
 *
 * Defines a generic AVL tree data structure.
 *
 * On creating a tree, the user specifies the size of the datum and the
 * FIXED maximum number of entries.  On insert/delete/lookup, the user
 * supplies the datum compare function.
 *
 * NOTE: These routines are NOT thread-safe.  If multiple threads could
 * access an AVL tree, external locks must be used to mutually exclude
 * each call to the shr_avl routines, including shr_avl_traverse().
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SHR_AVL_H
#define SHR_AVL_H

/*! Opaque type for raw AVL datum (actual datum size is variable) */
typedef int shr_avl_datum_t;

/*!
 * \brief Comparison function for AVL tree operations.
 *
 * <long-description>
 *
 * \param [in] user_data Application-provided context.
 * \param [in] datum1 First datum of comparison.
 * \param [in] datum2 Second datum of comparison.
 *
 * \retval 1 datum1 is greater than datum2
 * \retval 0 datum1 is equal to datum2
 * \retval -1 datum1 is lesser than datum2
 */
typedef int (*shr_avl_compare_fn)(void *user_data,
                                  shr_avl_datum_t *datum1,
                                  shr_avl_datum_t *datum2);

/*!
 * \brief Comparison function for AVL lookup operations.
 *
 * <long-description>
 *
 * \param [in] user_data Application-provided context for AVL tree.
 * \param [in] datum1 First datum of comparison.
 * \param [in] datum2 Second datum of comparison.
 *
 * \retval 1 datum1 is greater than datum2
 * \retval 0 datum1 is equal to datum2
 * \retval -1 datum1 is lesser than datum2
 */
typedef int (*shr_avl_compare_fn_lkupdata)(void *user_data,
                                           shr_avl_datum_t *datum1,
                                           shr_avl_datum_t *datum2,
                                           void *lkupdata);

/*!
 * \brief Call-back function for AVL tree traverse operation.
 *
 * The application-provided traverse function will be called for each
 * datum in the AVL tree upon request.
 *
 * \param [in] user_data Application-provided context for AVL tree.
 * \param [in] datum Current traversed datum.
 * \param [in] trav_data Application-provided context for traverse.
 */
typedef int (*shr_avl_traverse_fn)(void *user_data,
                                   shr_avl_datum_t *datum,
                                   void *trav_data);

/*!
 * \brief Call-back function for copying AVL entries.
 *
 * The application-provided traverse function will be called for all
 * entries in the AVL tree upon request.
 *
 * \param [in] user_data Application-provided context for AVL tree.
 * \param [in] datum1 Destination datum.
 * \param [in] trav_data Source datum.
 */
typedef int (*shr_avl_datum_copy_fn)(void *user_data,
                                     shr_avl_datum_t *datum1,
                                     shr_avl_datum_t *datum2);

/*!
 * \brief AVL tree node structure.
 *
 * In addition to the raw AVL datum, this structure contains all
 * necessary control information for the entry.
 */
typedef struct shr_avl_entry_s {

    /*! Pointer to node's left subtree. */
    struct shr_avl_entry_s *left;

    /*! Pointer to node's right subtree. */
    struct shr_avl_entry_s *right;

    /*! Current AVL balance factor. */
    int balance;

    /*! Raw AVL datum (opaque type - real size is variable). */
    shr_avl_datum_t datum;

} shr_avl_entry_t;

/*!
 * \brief AVL tree data structure.
 *
 * This structure is used for all AVL operations.
 */
typedef struct shr_avl_s {

    /*! Application-provided context for call-backs. */
    void *user_data;

    /*! Size of each datum stored in the tree. */
    int datum_bytes;

    /*! Maximum number of entries in AVL tree. */
    int datum_max;

    /*! Size of AVL entry, i.e. raw datum + control information */
    int entry_bytes;

    /*! Allocated memory block for storing AVL entries. */
    char *datum_base;

    /*! Root of AVL tree. */
    shr_avl_entry_t *root;

    /*! List of unused AVL entries. */
    shr_avl_entry_t *free_list;

    /*! Number entries in tree */
    int count;

    /*! Function for copying AVL data */
    shr_avl_datum_copy_fn datum_copy_fn;

} shr_avl_t;

/*!
 * \brief Create an empty AVL tree.
 *
 * Use this function to create a new AVL tree.
 *
 * \param [out] avl_ptr Return pointer (handle) to new AVL tree.
 * \param [in] user_data Arbitrary user data passed to compare routines.
 * \param [in] datum_bytes Size of datum being stored.
 * \param [in] datum_max Maximum number of nodes in tree.
 *
 * \retval 0 No errors.
 * \retval -1 Memory allocation error.
 */
extern int
shr_avl_create(shr_avl_t **avl_ptr,
               void *user_data,
               int datum_bytes,
               int datum_max);

/*!
 * \brief Release the space occupied by AVL tree structure.
 *
 * \param [in] avl AVL tree handle.
 *
 * \retval 0 No errors.
 */
extern int
shr_avl_destroy(shr_avl_t *avl);

/*!
 * \brief Insert a datum into the AVL tree.
 *
 * A duplicate key overwrites the old contents of the datum.
 *
 * \param [in] avl AVL tree handle.
 * \param [in] cmp_fn Datum comparison function.
 * \param [in] datum Datum to insert.
 *
 * \retval 0 No errors.
 * \retval -1 Tree is full.
 */
extern int
shr_avl_insert(shr_avl_t *avl,
               shr_avl_compare_fn cmp_fn,
               shr_avl_datum_t *datum);

/*!
 * \brief Delete an AVL tree entry.
 *
 * \param [in] avl AVL tree handle
 * \param [in] key_cmp_fn Call-back function for entry comparison
 * \param [in] datum Datum of AVL entry to be deleted
 *
 * \retval 0 Success, did not find datum.
 * \retval 1 Success, found and deleted datum.
 */
extern int
shr_avl_delete(shr_avl_t *avl,
               shr_avl_compare_fn key_cmp_fn,
               shr_avl_datum_t *datum);

/*!
 * \brief Reset an AVL tree to empty.
 *
 * Also used during tree creation to initialize free list.
 *
 * \param [in] avl AVL tree handle.
 *
 * \retval 0 No errors.
 */
extern int
shr_avl_delete_all(shr_avl_t *avl);

/*!
 * \brief Return the number of data in an AVL tree.
 *
 * \param [in] avl AVL tree handle
 *
 * \return Number of entries in AVL tree (0 if avl is NULL).
 */
extern int
shr_avl_count(shr_avl_t *avl);

/*!
 * \brief Look up a datum in the AVL tree.
 *
 * \param [in] avl AVL tree handle
 * \param [in] key_cmp_fn Compare function which should compare only
 *		          the key portion of the datum.
 * \param [in,out] datum As input, datum in which key portion is
 *                       valid. As output, datum in which key and data
 *                       portion is valid if found
 *
 * \retval 0 Success, did not find datum.
 * \retval 1 Success, found key and updated rest of datum.
 */
extern int
shr_avl_lookup(shr_avl_t *avl,
               shr_avl_compare_fn key_cmp_fn,
               shr_avl_datum_t *datum);

/*!
 * \brief <brief-description>
 *
 * <long-description>
 *
 * \param [in] avl AVL tree handle.
 * \param [in] key_cmp_fn Compare function which should compare only
 *                        the key portion of the datum.
 * \param [in,out] datum As input, datum in which key portion is
 *                       valid. As output, datum in which key and data
 *                       portion is valid if found.
 * \param [in] lkupdata Application-provided data passed to the
 *                      comparison function
 *
 * \retval 0 Success, did not find datum.
 * \retval 1 Success, found key and updated rest of datum.
 */
extern int
shr_avl_lookup_lkupdata(shr_avl_t *avl,
                        shr_avl_compare_fn_lkupdata key_cmp_fn,
                        shr_avl_datum_t *datum,
                        void *lkupdata);

/*!
 * \brief Return the minimum element in the tree.
 *
 * Return the minimum elelment as per application-provided comparison
 * function during AVL tree creation.
 *
 * \param [in] avl AVL tree handle.
 * \param [out] datum Datum to receive element.
 *
 * \retval 0 Success, found and retrieved datum.
 * \retval -1 Tree empty.
 */
extern int
shr_avl_lookup_min(shr_avl_t *avl,
                   shr_avl_datum_t *datum);

/*!
 * \brief Return the maximum element in the tree.
 *
 * Return the maximum elelment as per application-provided comparison
 * function during AVL tree creation.
 *
 * \param [in] avl AVL tree handle.
 * \param [out] datum Datum to receive element.
 *
 * \retval 0 Success, found and retrieved datum.
 * \retval -1 Tree empty.
 */
extern int
shr_avl_lookup_max(shr_avl_t *avl,
                   shr_avl_datum_t *datum);

/*!
 * \brief Call a callback function for each datum in an AVL tree.
 *
 * The callback routine may not modify the AVL tree being traversed.
 *
 * \param [in] avl AVL tree handle.
 * \param [in] trav_fn User callback function, called once per datum.
 * \param [in] trav_data Arbitrary user data passed to callback routine.
 *
 * \return If the user callback ever returns a negative value, the
 *	   traversal is stopped and that value is returned.  Otherwise
 *	   0 is returned.
 */
extern int
shr_avl_traverse(shr_avl_t *avl,
                 shr_avl_traverse_fn trav_fn,
                 void *trav_data);

#endif  /* SHR_AVL_H */
