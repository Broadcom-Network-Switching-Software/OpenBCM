/*! \file shr_hash_str.h
 *
 * hash table for strings.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SHR_HASH_STR_H
#define SHR_HASH_STR_H

#include <shr/shr_types.h>

/*!
 * This is the header file of the hash dictionary API. None of the API
 * function is re-entrant. However, multiple hash tables can be manipulated
 * simultanously. It is only that multiple threads can not operate on the same
 * hash table simultanously. Taking this approach improves performance
 */

/*!
 * Maximal number of hash string elements
 */
#define BCM_SHASH_STR_MAX_ELEMENTS  0x80000     /* 0.5M*/

/*! Dictionary handle type. */
typedef struct shr_hash_str_t *shr_hash_str_hdl;

/*!
 * \brief Initialize hash string module.
 *
 * Call this function once upon initialization. Calling other API
 * functions prior to this will result in error.
 *
 * \return 0 on success otherwise failure to initialize
 */
extern int shr_hash_str_init(void);

/*!
 * \brief delete hash string module.
 *
 * Call this function once upon termination. Calling other functions
 * of this API after cleaning up the module may result in undesired
 * results
 *
 * \return none
 */
extern void shr_hash_str_delete(void);

/*!
 * \brief Allocate hash string dictionary.
 *
 * This API allocates resources for hash dictionary
 *
 * \param [in] num_of_elements dictionary size
 * \param [out] dict_hdl pointer where to place the dictionary handle
 *
 * \return 0 on success otherwise failure in allocating hash dictionary
 */
extern int shr_hash_str_dict_alloc(size_t num_of_elements,
                                   shr_hash_str_hdl *dict_hdl);

/*!
 * \brief create hash string dictionary.
 *
 * This API free all resources associated with the hash dictionary
 *
 * \param [in] dict_hdl the dictionary handle
 *
 * \return 0 on success otherwise failure in freeing hash dictionary
 */
extern int shr_hash_str_dict_free(shr_hash_str_hdl dict_hdl);

/*!
 * \brief Insert element in hash dictionary.
 *
 * This API insert an element into the hash directory
 *
 * \param [in] dict_hdl the dictionary handle
 * \param [in] key pointer to string used as key. The memory pointed by the
 *              key must remain under the control of the application as long
 *              as the entry is valid.
 * \param [in] value pointer to value.
 *
 * \return 0 success otherwise failure in finding an element
 */
extern int shr_hash_str_dict_insert(shr_hash_str_hdl dict_hdl,
                                    const char *key,
                                    const void *value);

/*!
 * \brief lookup element in hash dictionary.
 *
 * This API lookup for an element in the hash directory
 *
 * \param [in] dict_hdl the dictionary handle
 * \param [in] key pointer to string used as key. The memory pointed by the
 *              key must remain under the control of the application as long
 *              as the entry is valid.
 * \param [out] value is double pointer where the function will
 *              place the address of value
 *
 * \return 0 success otherwise failure in finding an element
 */
extern int shr_hash_str_dict_lookup(shr_hash_str_hdl dict_hdl,
                             const char *key,
                             void **value);

/*!
 * \brief delete element from hash dictionary.
 *
 * This API deletes an element from the hash directory
 *
 * \param [in] dict_hdl the dictionary handle
 * \param [in] key pointer to string used as key
 *
 * \return 0 on success, otherwise failure in deleting an element
 */
extern int shr_hash_str_dict_delete(shr_hash_str_hdl dict_hdl,
                 const char *key);


/*!
 * \brief start hash enumeration
 *
 * This API starts to enumerate all the entries in the hash.
 * It returns the first entry. To continue the enumeration
 * the application should call \ref shr_hash_str_get_next.
 *
 * \param [in] dict_hdl the dictionary handle
 * \param [out] key is a double pointer where the function will
 *              place the address of the key
 * \param [out] value is double pointer where the function will
 *              place the address of value
 *
 * \return 0 on success, otherwise failure in finding the first element
 */
extern int shr_hash_str_get_first(shr_hash_str_hdl dict_hdl,
                                  char **key,
                                  void **value);

/*!
 * \brief continue hash enumeration
 *
 * This API starts to enumerate all the entries in the hash.
 * It returns the first entry
 *
 * \param [in] dict_hdl the dictionary handle
 * \param [out] key is a double pointer where the function will
 *              place the address of the key
 * \param [out] value is double pointer where the function will
 *              place the address of value
 *
 * \return 0 on success, otherwise failure in finding the next element
 */
extern int shr_hash_str_get_next(shr_hash_str_hdl dict_hdl,
                                  char **key,
                                  void **value);
/*!
 * \brief Get statistics for hash dictionary.
 *
 * This API provides some statistics on the hash directory
 *
 * \param [in] dict_hdl the dictionary handle
 * \param [out] size pointer to the hash table size (can be NULL)
 * \param [out] element_count pointer to the number of elements in the
 *              hash table (can be NULL)
 * \param [out] empty_slots pointer to the number of empty slots in the
 *              hash table (can be NULL)
 *
 * \return 0 on success, otherwise failure in deleting an element
 */
extern int shr_hash_str_dict_info_get(shr_hash_str_hdl dict_hdl,
                 size_t *size,
                 size_t *element_count,
                 size_t *empty_slots);



#endif /* SHR_HASH_STR_H */
