/*! \file bcmcfg_comp_scanner_traverse.c
 *
 * BCMCFG component scanner nodes traversal.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmcfg/bcmcfg_types.h>
#include <bcmcfg/bcmcfg_internal.h>
#include <bcmcfg/bcmcfg_comp_scanner_traverse.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMCFG_COMP

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief BCMCFG component scanner nodes traverse function.
 *
 * \param [in]  sc              Component scanner for traversing.
 * \param [in]  sc_data         Component configuration data.
 * \param [in]  node            Currnet Component node index in traversal.
 * \param [in]  array_offset    Array data offset of the component node.
 * \param [in]  array_idx       Array element index for the component node.
 *                              If the node is not an element of an array,
 *                              \c array_idx will be -1.
 * \param [in]  indent          Indentation level of the component node.
 * \param [in]  cb              Traverse callback function.
 * \param [in]  user_data       User data of the traverse callback function.
 */
static void
schema_node_traverse(const bcmcfg_comp_scanner_t *sc, uint32_t *sc_data,
                     uint32_t node, size_t array_offset,
                     int array_idx, int indent,
                     bcmcfg_comp_scanner_traverse_cb_f cb, void *user_data)
{
    uint32_t idx;
    const bcmcfg_comp_node_t *s;

    if (node == BCMCFG_NO_IDX || node >= sc->schema_count) {
        return;
    }

    s = &sc->schema[node];
    if (s->node == BCMCFG_COMP_MAP) {
        indent = cb(user_data, s->key, array_idx, indent, NULL, s->array);
        /* Expand the next level */
        if (node + 1 != s->next) {
            if (s->array > 0) {
                for (idx = 0; idx < s->array; idx++, array_offset += s->size) {
                    schema_node_traverse(sc, sc_data, node + 1, array_offset,
                                         0, indent + 1, cb, user_data);
                }
                array_offset -= s->size * s->array;
            } else {
                schema_node_traverse(sc, sc_data, node + 1, array_offset,
                                     -1, indent + 1, cb, user_data);
            }
        }
    } else if (s->node == BCMCFG_COMP_SCALAR) {
        uint32_t *data = sc_data + (s->offset + array_offset) / 4;

        indent = cb(user_data, s->key, array_idx, indent, data, s->array);
    }

    /* Expand the Sibling node */
    schema_node_traverse(sc, sc_data, s->next, array_offset,
                         array_idx < 0 ? -1 : array_idx + 1,
                         indent, cb, user_data);
}

/*******************************************************************************
 * Public Functions
 */

int
bcmcfg_comp_scanner_traverse(bcmcfg_comp_scanner_traverse_cb_f cb,
                             void *user_data)
{
    uint32_t idx;
    const bcmcfg_comp_scanner_conf_t *sc_conf = bcmcfg_component_conf;
    const bcmcfg_comp_scanner_t *sc;
    uint32_t *sc_data = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (!cb) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (!sc_conf) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    for (idx = 0; idx < sc_conf->count; idx++) {
        sc = sc_conf->scanner[idx];
        SHR_ALLOC(sc_data, sc->data_size, "bcmcfgCompTrvs");
        SHR_NULL_CHECK(sc_data, SHR_E_MEMORY);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcfg_comp_scanner_config_get(sc, sc->data_size, sc_data));
        schema_node_traverse(sc, sc_data, 0, 0, -1, 0, cb, user_data);
        SHR_FREE(sc_data);
    }

exit:
    SHR_FREE(sc_data);
    SHR_FUNC_EXIT();
}
