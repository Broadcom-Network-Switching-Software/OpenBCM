/*
 * $Id: sand_rhlist.h,v 1.0 2016/01/17 09:42:41 mfrank Exp
 * $
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 */
#ifndef UTILEX_RHLIST_H_INCLUDED
#define UTILEX_RHLIST_H_INCLUDED

#include <shared/utilex/utilex_str.h>
#include <shared/shrextend/shrextend_error.h>

/**
 * \brief Use this define if you don't want to bother with ID management
 */
#define    RHID_TO_BE_GENERATED    -1
/**
 * \brief type for entry handle
 */
typedef void *rhhandle_t;
/**
 * \brief type for entry id
 */
typedef int rhid_t;
/**
 * \brief Header structure for any elements that compose the list
 */
typedef struct _rhentry
{
  /**
   * Handle of the previous element in the list, when it is NULL - this element is the first
   */
    struct _rhentry *next;
  /**
   * Handle of the next element in the list, when it is NULL - this element is the last
   */
    struct _rhentry *prev;
  /**
   * Entry ID, may be assigned automatically, incrementing max_id of the list or explicitly by caller
   */
    rhid_t id;
  /**
   * Entry Name, should be provided by caller, may stay NULL. When sanity is enabled NULL is not allowed
   */
    char name[RHNAME_MAX_SIZE];
  /**
   * There may be number of users of the element, element may be freed only when users is zero
   */
    int users;
  /**
   * Kind of free variable that may be used for different purposes by different callers. E.g may testify on w/r/ permissions of the entry
   */
    int mode;
} rhentry_t;
/**
 * \brief double link list control structure
 */
typedef struct _rhlist
{
  /**
   * List itself may be member of list of lists, so it need to have this defintion as first in its control structure
   */
    rhentry_t entry;
  /**
   * Handle of the first element in the list, when it is NULL - no elements are in the list
   */
    rhentry_t *top;
  /**
   * Handle of the last element in the list, when it is NULL - no elements are in the list
   */
    rhentry_t *tail;
  /**
   * Number of elements in the list
   */
    int num;
  /**
   * Maximum ID of the element in the list, serves to allocates new number
   */
    int max_id;
  /**
   * Allocation size for list element - All elements must have the same size
   */
    int entry_size;
  /**
   * Flag that enforces sanity checks on inserting new element. It checks for double name and id
   */
    int sanity;
} rhlist_t;
/**
 * \brief Macro returning number of elements in the list
 */
#define RHLNUM(rhlist)          (rhlist)->num
/**
 * \brief Macro returning entry id
 */
#define RHID(rhentry)           ((rhentry_t *)rhentry)->id
/**
 * \brief Macro returning pointer to entry name
 */
#define RHNAME(rhentry)         ((rhentry_t *)rhentry)->name
/**
 * \brief Macro returning number of users for entry
 */
#define RHUSERS(rhentry)        ((rhentry_t *)rhentry)->users
/**
 * \brief Macro incrementing number of users
 */
#define RHUSER_ADD(rhentry)     (((rhentry_t *)rhentry)->users++)
/**
 * \brief Macro decrementing number of users
 */
#define RHUSER_DEL(rhentry)     (((rhentry_t *)rhentry)->users--)
/**
 * \brief Macro returning entry mode
 */
#define RHMODE(rhentry)         ((rhentry_t *)rhentry)->mode
/**
 * \brief Creates double link list and return its handle
 * \par DIRECT INPUT
 *   \param [in] name list may be assigned a name to facilitate debugging. Optional - may be NULL
 *   \param [in] entry_size size of list members - entries, serves for memory allocation, should be at least sizeof(rhentry_t)
 *   \param [in] sanity flag requesting sanity check on entry names and ID
 * \par DIRECT OUTPUT:
 *   \retval list handle, to be used in entry management
 */
rhlist_t *utilex_rhlist_create(
    char *name,
    int entry_size,
    int sanity);

/**
 * \brief Free memory for all entries in the list and free the list
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 */
void utilex_rhlist_free_all(
    rhlist_t * rhlist);

/**
 * \brief Free memory for all entries and reset the list to the new-born state
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 */
void utilex_rhlist_clean(
    rhlist_t * rhlist);

/**
 * \brief Go through all entries and update max id of the list to the maximum existing entry one
 * Serves to refresh ID allocation mechanism
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 */
void utilex_rhlist_refresh_max_id(
    rhlist_t * rhlist);

/**
 * \brief Allocate entry that will fit the list including sanity check, but not actually add it to the list
 * Useful when there is a need to first allocate the memory and then set some fields in the entry that will
 * influence the place where entry should be added
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 *   \param [in] name entry name, optional, may be NULL
 *   \param [in] id entry id, may be either specific number, or if auto allocation requested use RHID_TO_BE_GENERATED (-1)
 *   \param [in,out] rhentry_p pointer to the entry handle
 * \par DIRECT OUTPUT
 *   \retval _SHR_E_NONE for success
 *   \retval _SHR_E_PARAM problem with input parameters
 *   \retval _SHR_E_MEMORY memory allocation failure
 * \par INDIRECT OUTPUT
 *   *rhentry_p handle to new added top entry, undefined for failure
 */
shr_error_e utilex_rhlist_entry_alloc(
    rhlist_t * rhlist,
    char *name,
    rhid_t id,
    rhhandle_t * rhentry_p);

/**
 * \brief Allocate entry that will fit the list including sanity check and add it on top of the list, so it will be first
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 *   \param [in] name entry name, optional, may be NULL
 *   \param [in] id entry id, may be either specific number, or if auto allocation requested use RHID_TO_BE_GENERATED (-1)
 *   \param [in,out] rhentry_p pointer to the place where entry handle will be put in case of success
 * \par DIRECT OUTPUT
 *   \retval _SHR_E_NONE for success
 *   \retval _SHR_E_PARAM problem with input parameters
 *   \retval _SHR_E_MEMORY memory allocation failure
 * \par INDIRECT OUTPUT
 *   *rhentry_p handle to new added top entry, undefined for failure
 */
shr_error_e utilex_rhlist_entry_add_top(
    rhlist_t * rhlist,
    char *name,
    rhid_t id,
    rhhandle_t * rhentry_p);

/**
 * \brief Allocate entry that will fit the list including sanity check and add it to the tail of the list, so it will be last
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 *   \param [in] name entry name, optional, may be NULL
 *   \param [in] id entry id, may be either specific number, or if auto allocation requested use RHID_TO_BE_GENERATED (-1)
 *   \param [out] rhentry_p pointer to the place where entry handle will be put in case of success
 * \par DIRECT OUTPUT
 *   \retval _SHR_E_NONE for success
 *   \retval _SHR_E_PARAM problem with input parameters
 *   \retval _SHR_E_MEMORY memory allocation failure
 * \par INDIRECT OUTPUT
 *   *rhentry_p handle to new added top entry, undefined for failure
 */
shr_error_e utilex_rhlist_entry_add_tail(
    rhlist_t * rhlist,
    char *name,
    rhid_t id,
    rhhandle_t * rhentry_p);

/**
 * \brief Allocate entry that will fit the list including sanity check and insert sorted according to the name
 *              case insensitive
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 *   \param [in] name entry name, optional, may be NULL
 *   \param [in] id entry id, may be either specific number, or if auto allocation requested use RHID_TO_BE_GENERATED (-1)
 *   \param [out] rhentry_p pointer to the place where entry handle will be put in case of success
 * \par DIRECT OUTPUT
 *   \retval _SHR_E_NONE for success
 *   \retval _SHR_E_PARAM problem with input parameters
 *   \retval _SHR_E_MEMORY memory allocation failure
 * \par INDIRECT OUTPUT
 *   *rhentry_p handle to new added top entry, undefined for failure
 */
shr_error_e utilex_rhlist_entry_add_sorted(
    rhlist_t * rhlist,
    char *name,
    rhid_t id,
    rhhandle_t * rhentry_p);

/**
 * \brief Insert previously allocated entry before another entry in list
 * The major use - keep list sorted by some field or criteria
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 *   \param [in] rhentry entry to be inserted, must be valid entry handle
 *   \param [in] rhentry_next entry will be inserted before this one, if it is NULL, entry will be added to the tail
 * \par DIRECT OUTPUT
 *   \retval _SHR_E_NONE for success
 *   \retval _SHR_E_PARAM problem with input parameters
 */
shr_error_e utilex_rhlist_entry_insert_before(
    rhlist_t * rhlist,
    rhhandle_t rhentry,
    rhhandle_t rhentry_next);

/**
 * \brief Add previously allocated entry to the tail, making it last one
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 *   \param [in] rhentry entry to be inserted
 * \par DIRECT OUTPUT
 *   \retval _SHR_E_NONE for success
 *   \retval _SHR_E_PARAM problem with input parameters
 */
shr_error_e utilex_rhlist_entry_append(
    rhlist_t * rhlist,
    rhhandle_t rhentry);

/**
 * \brief Delete entry from the list and free the memory
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 *   \param [in] rhentry entry to be deleted
 * \par DIRECT OUTPUT
 *   \retval _SHR_E_NONE for success
 *   \retval _SHR_E_PARAM problem with input parameters
 */
shr_error_e utilex_rhlist_entry_del_free(
    rhlist_t * rhlist,
    rhhandle_t rhentry);

/**
 * \brief Remove entry from the list, but do not free the memory
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 *   \param [in] rhentry entry to be removed
 * \par DIRECT OUTPUT
 *   \retval _SHR_E_NONE for success
 *   \retval _SHR_E_PARAM problem with input parameters
 * \remark
 *   Used to utilize the entry independent from the list or to move it to another list
 */
shr_error_e utilex_rhlist_entry_remove(
    rhlist_t * rhlist,
    rhhandle_t rhentry);

/**
 * \brief Verify that entry exists in the list
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 *   \param [in] rhentry entry, which presence in list should be verified
 * \par DIRECT OUTPUT
 *   \retval _SHR_E_NONE for success
 *   \retval _SHR_E_PARAM problem with input parameters
 */
shr_error_e utilex_rhlist_entry_exists(
    rhlist_t * rhlist,
    rhhandle_t rhentry);

/**
 * \brief Get entry handle by its id
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 *   \param [in] id entry id that serves as search criteria
 * \par DIRECT OUTPUT
 *   \retval handle found
 *   \retval NULL entry not found or error
 */
rhhandle_t utilex_rhlist_entry_get_by_id(
    rhlist_t * rhlist,
    rhid_t id);

/**
 * \brief Get entry handle by its name
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 *   \param [in] name entry name that serves as search criteria
 * \par DIRECT OUTPUT
 *   \retval handle found
 *   \retval NULL entry not found or error
 */
rhhandle_t utilex_rhlist_entry_get_by_name(
    rhlist_t * rhlist,
    char *name);

/**
 * \brief Get the last entry with the specified name
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 *   \param [in] name entry name that serves as search criteria
 * \par DIRECT OUTPUT
 *   \retval handle found
 *   \retval NULL entry not found or error
 */
rhhandle_t utilex_rhlist_entry_get_last_by_name(
    rhlist_t * rhlist,
    char *name);

/**
 * \brief Get entry handle by first characters of its name
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 *   \param [in] name substring of the name starting from beginning
 * \par DIRECT OUTPUT
 *   \retval handle found
 *   \retval NULL entry not found or error
 */
rhhandle_t utilex_rhlist_entry_get_by_sub(
    rhlist_t * rhlist,
    char *name);

/**
 * \brief Get first entry(top) in the list
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 * \par DIRECT OUTPUT
 *   \retval entry handle for success
 *   \retval NULL is there are no entries or error
 */
rhhandle_t utilex_rhlist_entry_get_first(
    rhlist_t * rhlist);

/**
 * \brief Get last entry(tail) in the list
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 * \par DIRECT OUTPUT
 *   \retval entry handle for success
 *   \retval NULL is there are no entries or error
 */
rhhandle_t utilex_rhlist_entry_get_last(
    rhlist_t * rhlist);

/**
 * \brief Get next entry in the list, following the input one
 * \par DIRECT INPUT
 *   \param [in] rhentry entry that immediately preceding the one requested
 * \par DIRECT OUTPUT
 *   \retval entry handle for success
 *   \retval NULL is there are no entries or error
 */
rhhandle_t utilex_rhlist_entry_get_next(
    rhhandle_t rhentry);

/**
 * \brief Get previous entry in the list, preceding the input one
 * \par DIRECT INPUT
 *   \param [in] rhentry entry that immediately follows the one requested
 * \par DIRECT OUTPUT
 *   \retval entry handle for success
 *   \retval NULL is there are no entries or error
 */
rhhandle_t utilex_rhlist_entry_get_prev(
    rhhandle_t rhentry);

/**
 * \brief Get entry id given its name
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 *   \param [in] name entry name that serves as search criteria
 * \par DIRECT OUTPUT
 *   \retval entry id for success
 *   \retval negative for failure
 */
rhid_t utilex_rhlist_id_get_by_name(
    rhlist_t * rhlist,
    char *name);

/**
 * \brief Verify existence of entry given its name
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 *   \param [in] id entry id that serves as search criteria
 * \par DIRECT OUTPUT
 *   \retval valid name for success
 *   \retval NULL for failure
 */
char *utilex_rhlist_name_get_by_id(
    rhlist_t * rhlist,
    rhid_t id);

/**
 * \brief Verify existence of entry with certain id
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 *   \param [in] id entry id that serves as search criteria
 *   \param [in] rhentry_p - pointer to entry handle
 * \par DIRECT OUTPUT
 *   \retval _SHR_E_NONE for success
 *   \retval _SHR_E_PARAM problem with input parameters
 *   \retval _SHR_E_NOT_FOUND id the entry was not found
 * \par INDIRECT OUTPUT
 *   *rhentry_p - entry handle, that will be assigned in the case of success
 */
shr_error_e utilex_rhlist_id_exists(
    rhlist_t * rhlist,
    rhid_t id,
    rhhandle_t * rhentry_p);

/**
 * \brief Verify existence of entry with certain name
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 *   \param [in] name entry name that serves as search criteria
 *   \param [in,out] rhentry_p - pointer to entry handle, that will be assigned in the case of success, may be NULL is handle is not requested
 * \par DIRECT OUTPUT
 *   \retval _SHR_E_NONE for success
 *   \retval _SHR_E_PARAM problem with input parameters
 *   \retval _SHR_E_NOT_FOUND id the entry was not found
 * \par INDIRECT OUTPUT
 *   See rhentry_p above - pointer to entry handle, that will be assigned in the case of success
 */
shr_error_e utilex_rhlist_name_exists(
    rhlist_t * rhlist,
    char *name,
    rhhandle_t * rhentry_p);

/**
 * \brief Iterator through the elements of he list, starting from top
 * \par DIRECT INPUT
 *   \param [in]  rhlist handle to list
 *   \param [out] rhentry handle to the current entry fetched from the list
 * \par INDIRECT OUTPUT
 *   rhentry - entry handle, that is used inside the loop
 * \remark
 *   List must not be altered inside the iterator, the entries themselves may be altered, but not deleted or moved
 */
#define RHITERATOR(rhentry, rhlist)                \
        for(rhentry = utilex_rhlist_entry_get_first(rhlist); rhentry; rhentry = utilex_rhlist_entry_get_next(rhentry))

/**
 * \brief Iterator through the elements of he list, starting from tail
 * \par DIRECT INPUT
 *   \param [in]  rhlist handle to list
 *   \param [out] rhentry handle to the current entry fetched from the list
 * \par INDIRECT OUTPUT
 *   rhentry - entry handle, that is used inside the loop
 * \remark
 *   List must not be altered inside the iterator, the entries themselves may be altered, but not deleted or moved
 */
#define RHBACK_ITERATOR(rhentry, rhlist)                \
        for(rhentry = rhlist_get_last(rhlist); rhentry; rhentry = utilex_rhlist_entry_get_prev(rhentry))

/**
 * \brief Safe iterator through the elements of he list, starting from top
 * \par DIRECT INPUT
 *   \param [in]  rhlist handle to list
 *   \param [out] rhentry handle to the current entry fetched from the list
 * \par INDIRECT OUTPUT
 *   rhentry - entry handle, that is used inside the loop
 * \remark
 *   Entries may be deleted from the list through the iterator activity. Next is always the one taken when previous was valid
 *   Macro together with loop code should be wrapped in brackets due to the local variable declaration inside
 */
#define RHSAFE_ITERATOR(rhentry, rhlist)                                                                               \
        rhhandle_t rhentry_next;                                                                                       \
        for(rhentry = utilex_rhlist_entry_get_first(rhlist), rhentry_next = utilex_rhlist_entry_get_next(rhentry);     \
                       rhentry; rhentry = rhentry_next, rhentry_next = utilex_rhlist_entry_get_next(rhentry))

/**
 * \brief Safe iterator through the elements of he list, if entry is not NULL loop is limited only to this entry
 * \par DIRECT INPUT
 *   \param [in]  rhlist handle to list
 *   \param [in,out] rhentry handle to the current entry that may be input or fetched from the list
 * \par INDIRECT OUTPUT
 *   rhentry - entry handle, that is used inside the loop
 * \remark
 *   Macro that allows either to do the loop or to limit the run by the input rhentry only
 *   Entries may be deleted from the list through the iterator activity. Next is always the one taken when previous was valid
 *   Macro together with loop code should be wrapped in brackets due to the local variable declaration inside
 */
#define RHCOND_ITERATOR(rhentry, rhlist)                                                            \
        rhhandle_t rhentry_next = NULL;                                                             \
        if(rhentry == NULL) {                                                                       \
            rhentry = utilex_rhlist_entry_get_first(rhlist);                                        \
            rhentry_next = utilex_rhlist_entry_get_next(rhentry);                                   \
        }                                                                                           \
        for(;rhentry; rhentry = rhentry_next, rhentry_next = utilex_rhlist_entry_get_next(rhentry))

#endif /* UTILEX_RHLIST_H_INCLUDED */
