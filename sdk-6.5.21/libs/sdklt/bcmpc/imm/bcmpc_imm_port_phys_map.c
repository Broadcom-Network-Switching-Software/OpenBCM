/*! \file bcmpc_imm_port_phys_map.c
 *
 * PC_PORT_PHYS_MAPt IMM handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmimm/bcmimm_int_comp.h>

#include <bcmpc/bcmpc_util_internal.h>
#include <bcmpc/bcmpc_imm_internal.h>
#include <bcmpc/bcmpc_lport_internal.h>
#include <bcmpc/bcmpc_cfg_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_IMM

/*! Logical table ID for this handler. */
#define IMM_SID PC_PORT_PHYS_MAPt

/*! The data structure for PC_PORT_PHYS_MAPt entry. */
typedef struct pc_port_phys_map_entry_s {
    bcmpc_lport_t key;
    bcmpc_port_cfg_t data;
    bcmimm_entry_event_t event;
} pc_port_phys_map_entry_t;


/*******************************************************************************
 * Private Functions
 */

 /*!
  * \brief Initialize pc_port_phys_map_entry_t data buffer.
  *
  * \param [in] entry entry buffer.
  *
  * \return Nothing.
  */
static void
pc_port_phys_map_entry_t_init(pc_port_phys_map_entry_t *entry)
{
    entry->key = BCMPC_INVALID_LPORT;
    bcmpc_port_cfg_t_init(&entry->data);
}

/*!
 * \brief Fill PC structure according to the given LT field value.
 *
 * \param [in] unit Unit number.
 * \param [in] fid Field ID.
 * \param [in] fval Field value.
 * \param [in,out] entry Entry buffer.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
pc_port_phys_map_entry_set(int unit, uint32_t fid, uint64_t fval,
                           pc_port_phys_map_entry_t *entry)
{
    bcmpc_port_cfg_t *pmap = &entry->data;

    SHR_FUNC_ENTER(unit);

    switch (fid) {
        case PC_PORT_PHYS_MAPt_PORT_IDf:
            entry->key = fval;
            break;
        case PC_PORT_PHYS_MAPt_PC_PHYS_PORT_IDf:
            pmap->pport = fval;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief IMM input LT fields parsing.
 *
 * Parse the input LT fields and save the data to pc_port_phys_map_entry_t.
 *
 * \param [in] unit Unit number.
 * \param [in] key_flds IMM input key field array.
 * \param [in] data_flds IMM input data field array.
 * \param [out] entry Port config data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c in to \c pdata.
 */
static int
pc_lt_port_phys_map_fields_parse(int unit,
                                 const bcmltd_field_t *key_flds,
                                 const bcmltd_field_t *data_flds,
                                 pc_port_phys_map_entry_t *entry)
{
    const bcmltd_field_t *fld;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    /* Parse key fields. */
    fld = key_flds;
    while (fld) {
        fid = fld->id;
        fval = fld->data;

        SHR_IF_ERR_VERBOSE_EXIT
            (pc_port_phys_map_entry_set(unit, fid, fval, entry));

        fld = fld->next;
    }

    /* Parse data fields. */
    fld = data_flds;
    while (fld) {
        fid = fld->id;
        fval = fld->data;

        SHR_IF_ERR_VERBOSE_EXIT
            (pc_port_phys_map_entry_set(unit, fid, fval, entry));

        fld = fld->next;
    }

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * DB schema handler
 */

/*!
 * \brief Convert the LT fields to the PC data structure.
 *
 * This is the parse callback function of IMM DB schema handler, \ref
 * bcmpc_db_imm_schema_handler_t.
 *
 * Convert bcmltd_fields_t array \c flds to a bcmpc_lport_t \c key and a
 * bcmpc_port_cfg_t \c data.
 *
 * \param [in] unit Unit number.
 * \param [in] flds LT field array.
 * \param [in] key_size Key size.
 * \param [out] key Key value from \c flds.
 * \param [in] data_size Data size.
 * \param [out] data Data value from \c flds.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
pc_imm_port_phys_map_entry_parse(int unit, const bcmltd_fields_t *flds,
                                 size_t key_size, void *key,
                                 size_t data_size, void *data)
{
    size_t i;
    uint32_t fid;
    uint64_t fval;
    pc_port_phys_map_entry_t entry;

    SHR_FUNC_ENTER(unit);

    pc_port_phys_map_entry_t_init(&entry);

    for (i = 0; i < flds->count; i++) {
        fid = flds->field[i]->id;
        fval = flds->field[i]->data;
        SHR_IF_ERR_VERBOSE_EXIT
            (pc_port_phys_map_entry_set(unit, fid, fval, &entry));
    }

    if (key) {
        sal_memcpy(key, &entry.key, key_size);
    }

    if (data) {
        sal_memcpy(data, &entry.data, data_size);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Convert the PC data structure to the LT fields.
 *
 * This is the fill callback function of IMM DB schema handler, \ref
 * bcmpc_db_imm_schema_handler_t.
 *
 * Convert a bcmpc_lport_t \c key and a bcmpc_port_cfg_t \c data to
 * bcmltd_fields_t array \c flds.
 *
 * \param [in] unit Unit number.
 * \param [in] key_size Key size.
 * \param [in] key Key value.
 * \param [in] data_size Data size.
 * \param [in] data Data value.
 * \param [out] flds LT field array.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
pc_imm_port_phys_map_entry_fill(int unit,
                                size_t key_size, void *key,
                                size_t data_size, void *data,
                                bcmltd_fields_t *flds)
{
    size_t fcnt = 0;
    bcmpc_lport_t *lport = key;
    bcmpc_port_cfg_t *pmap = data;

    SHR_FUNC_ENTER(unit);

    if (key) {
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PORT_PHYS_MAPt_PORT_IDf, 0, *lport, fcnt);
    }

    if (data) {
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PORT_PHYS_MAPt_PC_PHYS_PORT_IDf, 0, pmap->pport, fcnt);
    }

    flds->count = fcnt;

    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse function for validating the logical to physical port
 *        configurations.
 *
 * This function is called for each logical port to physical port mapping
 * by traversing the logical port map table.
 *
 * \param [in] unit Unit number.
 * \param [in] key_size Key size, i.e. sizeof(bcmpc_lport_t).
 * \param [in] key The logical port number, bcmpc_lport_t.
 * \param [in] data_size Data size, i.e. sizeof(bcmpc_port_cfg_t).
 * \param [in] data The logical port configuration, bcmpc_port_cfg_t.
 * \param [in] cookie Logical to physical port map configurationi.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pc_port_phys_map_entry_validate(int unit, size_t key_size, void *key,
                                size_t data_size, void *data,
                                void *cookie)
{
    bcmpc_lport_t lport = *(bcmpc_lport_t *)key;
    bcmpc_pport_t pport = *(bcmpc_pport_t *)data;
    pc_port_phys_map_entry_t *entry = cookie;
    bcmpc_lport_t lport_curr = BCMPC_INVALID_LPORT;

    SHR_FUNC_ENTER(unit);

    /*
     * Compare the physical port of the entry configured and in the table.
     * Check if a physical port is already mapped to the
     * logical port. Return error if the port is already
     * active i.e if an entry exists in the PC_PORT for this
     * logical port.
     * Allow the LT Update to go through if not active.
     */

    if ((pport == entry->data.pport) && (lport != entry->key)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((pport != entry->data.pport) && (lport == entry->key)  &&
         (entry->event == BCMIMM_ENTRY_UPDATE)) {
        SHR_EXIT();
    }

    lport_curr = bcmpc_pport_to_lport(unit, pport);

    if ((lport_curr != lport) && (lport_curr != BCMPC_INVALID_LPORT) &&
            (entry->event == BCMIMM_ENTRY_UPDATE)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (((pport != entry->data.pport) && (lport != entry->key)) ||
        ((pport == entry->data.pport) && (lport == entry->key))) {
        SHR_EXIT();
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * IMM event handler
 */

/*!
 * \brief Stage callback function of IMM event handler (bcmimm_lt_cb_t).
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] event This is the reason for the entry event.
 * \param [in] key_flds This is a linked list of the key fields identifying
 *                        the entry.
 * \param [in] data_flds This is a linked list of the data fields in the
 *                        modified entry.
 * \param [in] context Is a pointer that was given during registration.
 *                     The callback can use this pointer to retrieve some
 *                     context.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
pc_port_phys_map_config_validate(int unit,
                                 bcmltd_sid_t sid,
                                 bcmimm_entry_event_t event,
                                 const bcmltd_field_t *key_flds,
                                 const bcmltd_field_t *data_flds,
                                 void *context)
{
    int rv = 0;
    pc_port_phys_map_entry_t entry;
    pc_port_phys_map_entry_t entry_lookup;
    bcmpc_port_cfg_t pcfg;

    SHR_FUNC_ENTER(unit);

    pc_port_phys_map_entry_t_init(&entry);
    SHR_IF_ERR_VERBOSE_EXIT
        (pc_lt_port_phys_map_fields_parse(unit, key_flds, NULL, &entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (pc_lt_port_phys_map_fields_parse(unit, NULL, data_flds,
                                          &entry));
    entry.event = event;

    switch (event) {
        case BCMIMM_ENTRY_UPDATE:
        sal_memset(&pcfg, 0, sizeof(pcfg));

        /*
         * Allow duplicate PC_PORT_PHYS_MAP entries
         * Lookup if there is a physical port corresponding to user-
         * entered logical port. If so, check if they have the same pport.
         * If so, allow update to go through.
         */

        rv = bcmpc_db_imm_entry_lookup(unit, PC_PORT_PHYS_MAPt,
                                       P(&entry.key), P(&entry_lookup.data.pport));
        if (rv != SHR_E_NOT_FOUND) {
            if (entry_lookup.data.pport == entry.data.pport) {
                SHR_EXIT();
            }
        }

        /*
         * Check if the PC_PORT LT has an entry for this logical port.
         * Update only if PC_PORTt has no corresponding entry
         * for this logical port.
         */

        rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt,
                                       P(&entry.key), P(&pcfg));
        if (rv != SHR_E_NOT_FOUND) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        case BCMIMM_ENTRY_INSERT:
        /* Traverse the existing logical ports to physical port map. */
        SHR_IF_ERR_EXIT
            (bcmpc_db_imm_entry_traverse(unit,
                                        PC_PORT_PHYS_MAPt,
                                        sizeof(bcmpc_lport_t),
                                        sizeof(bcmpc_pport_t),
                                        pc_port_phys_map_entry_validate,
                                        &entry));
        break;
        default:
            SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Stage callback function of IMM event handler (bcmimm_lt_cb_t).
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] event This is the reason for the entry event.
 * \param [in] key_flds This is a linked list of the key fields identifying
 *                        the entry.
 * \param [in] data_flds This is a linked list of the data fields in the
 *                        modified entry.
 * \param [in] context Is a pointer that was given during registration.
 *                     The callback can use this pointer to retrieve some
 *                     context.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
pc_port_phys_map_stage(int unit,
                       bcmltd_sid_t sid,
                       uint32_t trans_id,
                       bcmimm_entry_event_t event,
                       const bcmltd_field_t *key_flds,
                       const bcmltd_field_t *data_flds,
                       void *context,
                       bcmltd_fields_t *output_fields)
{
    pc_port_phys_map_entry_t entry, committed;
    bool port_based_cfg_enable = FALSE;
    int rv = SHR_E_NONE;
    bcmpc_drv_t *pc_drv = NULL;
    bool is_internal = FALSE;

    SHR_FUNC_ENTER(unit);

    pc_port_phys_map_entry_t_init(&entry);
    SHR_IF_ERR_VERBOSE_EXIT
        (pc_lt_port_phys_map_fields_parse(unit, key_flds, NULL, &entry));

    if (output_fields) {
        output_fields->count = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pc_port_based_config_get(unit, &port_based_cfg_enable));

    if (!port_based_cfg_enable) {
        SHR_EXIT();
    }

    /* Get device driver. */
    SHR_IF_ERR_EXIT(bcmpc_drv_get(unit, &pc_drv));

    switch (event) {
        case BCMIMM_ENTRY_INSERT:

            /*
             * Perform the PC_PORT lookup first, before parsing the data
             * fields from the user as the lookup overwrites the pport
             * information in the entry.data resulting in pport to be 0
             * which is reserved for CPU.
             */
            rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&entry.key),
                                           P(&entry.data));
            if (rv == SHR_E_NOT_FOUND) {
                SHR_EXIT();
            } else {
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            }

            entry.data.lport = entry.key;
            SHR_IF_ERR_VERBOSE_EXIT
                (pc_lt_port_phys_map_fields_parse(unit, NULL, data_flds,
                                                  &entry));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_internal_port_map_insert(unit, entry.key, &entry.data));
            break;
        case BCMIMM_ENTRY_UPDATE:
            /* If updating internal port, just return. */
            entry.data.pport = bcmpc_lport_to_pport(unit, entry.key);
            if (entry.data.pport != BCMPC_INVALID_PPORT) {
                SHR_IF_ERR_EXIT(pc_drv->is_internal_port(unit, entry.data.pport,
                                &is_internal));
                if (is_internal == true) {
                    SHR_ERR_EXIT(SHR_E_DISABLED);
                }
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_db_imm_entry_lookup(unit,
                                           IMM_SID,
                                           P(&entry.key), P(&entry.data)));
            sal_memcpy(&committed, &entry, sizeof(committed));
            SHR_IF_ERR_VERBOSE_EXIT
                (pc_lt_port_phys_map_fields_parse(unit, NULL, data_flds,
                                                  &entry));
            if (sal_memcmp(&entry, &committed, sizeof(committed)) == 0) {
                /* Do nothing if the config is not changed. */
                SHR_EXIT();
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_internal_port_map_update(unit, entry.key, &entry.data));
            break;
        case BCMIMM_ENTRY_DELETE:
            /* If deleting internal port, just return. */
            entry.data.pport = bcmpc_lport_to_pport(unit, entry.key);
            if (entry.data.pport != BCMPC_INVALID_PPORT) {
                SHR_IF_ERR_EXIT(pc_drv->is_internal_port(unit, entry.data.pport,
                                &is_internal));
                if (is_internal == true) {
                    SHR_ERR_EXIT(SHR_E_DISABLED);
                }
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_internal_port_map_delete(unit, entry.key));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (event == BCMIMM_ENTRY_INSERT || event == BCMIMM_ENTRY_UPDATE) {
        SHR_IF_ERR_EXIT
            (bcmpc_port_lt_entry_update(unit, entry.key));
    }

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions
 */

int
bcmpc_imm_port_phys_map_register(int unit)
{
    bcmpc_db_imm_schema_handler_t db_hdl;
    bcmimm_lt_cb_t event_hdl;

    SHR_FUNC_ENTER(unit);

    db_hdl.entry_parse = pc_imm_port_phys_map_entry_parse;
    db_hdl.entry_fill = pc_imm_port_phys_map_entry_fill;

    sal_memset(&event_hdl, 0, sizeof(event_hdl));
    event_hdl.stage = pc_port_phys_map_stage;
    event_hdl.validate = pc_port_phys_map_config_validate;

    /* Register the handlers to the table. */
    SHR_IF_ERR_EXIT
        (bcmpc_imm_tbl_register(unit, IMM_SID, &db_hdl, &event_hdl));

exit:
    SHR_FUNC_EXIT();
}
