/*! \file bcmcfg_comp_scanner_traverse.h
 *
 * BCMCFG component scanner nodes traversal.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCFG_COMP_SCANNER_TRAVERSE_H
#define BCMCFG_COMP_SCANNER_TRAVERSE_H

/*!
 * \brief Traversal callback function.
 *
 * Prototype for callback function of traversing component scanner nodes.
 *
 * \param [in]  user_data       User callback data.
 * \param [in]  key             Key string for this node.
 * \param [in]  array_idx       Array element index for the component node.
 *                              If the node is not an element of an array,
 *                              \c array_idx will be -1.
 * \param [in]  indent          Indentation level for this node.
 * \param [in]  data            Data of this node. NULL if this is a map node.
 * \param [in]  array           Array size of this node if greater than 0.
 *
 * \return The indentation level of the node used in the callback function.
 *         Return \c indnet if the indentation is not changed.
 */
typedef int (*bcmcfg_comp_scanner_traverse_cb_f)(void *user_data,
                                                 const char *key,
                                                 int array_idx,
                                                 int indent,
                                                 uint32_t *data,
                                                 uint32_t array);

/*!
 * \brief Traversal function for component scanner nodes.
 *
 * This function will call \c cb for every traversing component scanner nodes.
 *
 * \param [in]  cb              Traverse callback function.
 * \param [in]  user_data       User data for traverse callback function.
 *
 * \retval SHR_E_NONE           Operation is successful.
 * \retval SHR_E_PARAM          Parameter error.
 * \retval SHR_E_INTERNAL       Internal error.
 * \retval SHR_E_MEMORY         Insufficient dynamically allocated memory.
 */
extern int
bcmcfg_comp_scanner_traverse(bcmcfg_comp_scanner_traverse_cb_f cb,
                             void *user_data);

#endif /* BCMCFG_COMP_SCANNER_TRAVERSE_H */
