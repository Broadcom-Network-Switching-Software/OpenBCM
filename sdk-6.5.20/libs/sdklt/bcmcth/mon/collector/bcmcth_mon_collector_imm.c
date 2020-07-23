/*! \file bcmcth_mon_collector_imm.c
 *
 * BCMCTH Monitor collector IMM handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_libc.h>

#include <bcmltd/chip/bcmltd_id.h>
#include <bcmcth/bcmcth_mon_collector.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmdrd_config.h>
#include <bcmcth/bcmcth_util.h>
#include <bcmcth/bcmcth_mon_drv.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_MON

/*******************************************************************************
 * Local definitions
 */

/*******************************************************************************
 * Private Functions
 */

static int
collector_fields_free(int unit, int num_fields, bcmltd_fields_t *fields)
{
    int idx;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(fields, SHR_E_PARAM);

    if (fields->field) {
        for (idx = 0; idx < num_fields; idx++) {
            if (fields->field[idx]) {
                shr_fmm_free(fields->field[idx]);
            }
        }
        SHR_FREE(fields->field);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
collector_fields_alloc(int unit, size_t num_fields, bcmltd_fields_t *fields)
{
    size_t idx;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(fields, SHR_E_PARAM);

    /* Allocate fields buffers */
    SHR_ALLOC(fields->field,
              sizeof(bcmltd_field_t *) * num_fields,
              "bcmcthMonIntFields");

    SHR_NULL_CHECK(fields->field, SHR_E_MEMORY);

    sal_memset(fields->field, 0, sizeof(bcmltd_field_t *) * num_fields);

    fields->count = num_fields;

    for (idx = 0; idx < num_fields; idx++) {
        fields->field[idx] = shr_fmm_alloc();
        SHR_NULL_CHECK(fields->field[idx], SHR_E_MEMORY);
        sal_memset(fields->field[idx], 0, sizeof(bcmltd_field_t));
    }

exit:
    if (SHR_FUNC_ERR()) {
        collector_fields_free(unit, num_fields, fields);
    }
    SHR_FUNC_EXIT();
}

static void
field_arr_to_ll(int count, bcmltd_field_t **arr, bcmltd_field_t **ll)
{
    int idx;

    if (count == 0) {
        *ll = NULL;
        return;
    }

    *ll = arr[0];
    for (idx = 1; idx < count; idx++) {
        arr[idx - 1]->next = arr[idx];
    }

    arr[idx - 1]->next = NULL;
}

/*!
 * \brief imm MON_COLLECTOR_IPV4t  input fields parsing.
 *
 * Parse imm MON_COLLECTOR_IPV4t input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key IMM input key field array.
 * \param [in] data IMM input data field array.
 * \param [out] export profile info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c key \c data to \c ltcfg.
 */
static int
mon_collector_ipv4_lt_fields_parse(
    int unit,
    const bcmltd_field_t *key,
    const bcmltd_field_t *data,
    bcmcth_mon_collector_ipv4_t *entry)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;
    uint32_t fval32[2] = {0};
    uint32_t idx;
    SHR_FUNC_ENTER(unit);

    sal_memset(entry, 0, sizeof(*entry));

    gen_field = key;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
            case MON_COLLECTOR_IPV4t_MON_COLLECTOR_IPV4_IDf:
                entry->collector_ipv4_id = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;
        idx = gen_field->idx;

        switch (fid) {
            case MON_COLLECTOR_IPV4t_TAG_STRUCTUREf:
                entry->tag_type = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;

            case  MON_COLLECTOR_IPV4t_DST_MACf:
                fval32[0] = (uint32_t)fval;
                fval32[1] = (uint32_t)(fval >> 32);
                BCMCTH_MAC_ADDR_FROM_UINT32(entry->dst_mac,fval32);
                SHR_BITSET(entry->fbmp, fid);
                break;

            case  MON_COLLECTOR_IPV4t_SRC_MACf:
                fval32[0] = (uint32_t)fval;
                fval32[1] = (uint32_t)(fval >> 32);
                BCMCTH_MAC_ADDR_FROM_UINT32(entry->src_mac,fval32);
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_COLLECTOR_IPV4t_VLAN_IDf:
                entry->outer_vlan_tag.tci.vid = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;

            case MON_COLLECTOR_IPV4t_CFIf:
                entry->outer_vlan_tag.tci.cfi = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_COLLECTOR_IPV4t_PRIf:
                entry->outer_vlan_tag.tci.prio = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;

            case MON_COLLECTOR_IPV4t_INNER_VLAN_IDf:
                entry->inner_vlan_tag.tci.vid = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;

            case MON_COLLECTOR_IPV4t_INNER_CFIf:
                entry->inner_vlan_tag.tci.cfi = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_COLLECTOR_IPV4t_INNER_PRIf:
                entry->inner_vlan_tag.tci.prio = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;

            case MON_COLLECTOR_IPV4t_TPIDf:
                entry->outer_vlan_tag.tpid = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_COLLECTOR_IPV4t_INNER_TPIDf:
                entry->inner_vlan_tag.tpid = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_COLLECTOR_IPV4t_DST_IPV4f:
                entry->dst_ip = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_COLLECTOR_IPV4t_SRC_IPV4f:
                entry->src_ip = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_COLLECTOR_IPV4t_TOSf:
                entry->tos = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_COLLECTOR_IPV4t_TTLf:
                entry->ttl = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_COLLECTOR_IPV4t_DST_L4_UDP_PORTf:
                entry->dst_port = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_COLLECTOR_IPV4t_SRC_L4_UDP_PORTf:
                entry->src_port = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_COLLECTOR_IPV4t_UDP_CHKSUMf:
                entry->udp_chksum = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_COLLECTOR_IPV4t_SYSTEM_ID_LENf:
                entry->system_id_len = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_COLLECTOR_IPV4t_SYSTEM_IDf:
                entry->system_id[idx] = (uint8_t)fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_COLLECTOR_IPV4t_COMPONENT_IDf:
                entry->component_id = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_COLLECTOR_IPV4t_OBSERVATION_DOMAINf:
                entry->observation_domain = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_COLLECTOR_IPV4t_IPFIX_ENTERPRISE_NUMBERf:
                entry->enterprise_number = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief imm MON_COLLECTOR_IPV6t  input fields parsing.
 *
 * Parse imm MON_COLLECTOR_IPV6t input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key IMM input key field array.
 * \param [in] data IMM input data field array.
 * \param [out] export profile info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c key \c data to \c ltcfg.
 */
static int
mon_collector_ipv6_lt_fields_parse(
    int unit,
    const bcmltd_field_t *key,
    const bcmltd_field_t *data,
    bcmcth_mon_collector_ipv6_t *entry)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;
    uint32_t fval32[2] = {0};
    uint32_t idx;
    SHR_FUNC_ENTER(unit);

    sal_memset(entry, 0, sizeof(*entry));

    gen_field = key;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
            case MON_COLLECTOR_IPV6t_MON_COLLECTOR_IPV6_IDf:
                entry->collector_ipv6_id = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;
        idx = gen_field->idx;

        switch (fid) {
            case MON_COLLECTOR_IPV6t_TAG_STRUCTUREf:
                entry->tag_type = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;

            case  MON_COLLECTOR_IPV6t_DST_MACf:
                fval32[0] = (uint32_t)fval;
                fval32[1] = (uint32_t)(fval >> 32);
                BCMCTH_MAC_ADDR_FROM_UINT32(entry->dst_mac,fval32);
                SHR_BITSET(entry->fbmp, fid);
                break;

            case  MON_COLLECTOR_IPV6t_SRC_MACf:
                fval32[0] = (uint32_t)fval;
                fval32[1] = (uint32_t)(fval >> 32);
                BCMCTH_MAC_ADDR_FROM_UINT32(entry->src_mac,fval32);
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_COLLECTOR_IPV6t_VLAN_IDf:
                entry->outer_vlan_tag.tci.vid = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;

            case MON_COLLECTOR_IPV6t_CFIf:
                entry->outer_vlan_tag.tci.cfi = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_COLLECTOR_IPV6t_PRIf:
                entry->outer_vlan_tag.tci.prio = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;

            case MON_COLLECTOR_IPV6t_INNER_VLAN_IDf:
                entry->inner_vlan_tag.tci.vid = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;

            case MON_COLLECTOR_IPV6t_INNER_CFIf:
                entry->inner_vlan_tag.tci.cfi = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_COLLECTOR_IPV6t_INNER_PRIf:
                entry->inner_vlan_tag.tci.prio = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;

            case MON_COLLECTOR_IPV6t_TPIDf:
                entry->outer_vlan_tag.tpid = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_COLLECTOR_IPV6t_INNER_TPIDf:
                entry->inner_vlan_tag.tpid = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_COLLECTOR_IPV6t_DST_IPV6u_UPPERf:
                BCMCTH_IPV6_ADDR_UPPER_SET(entry->dst_ip6_addr, fval);
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_COLLECTOR_IPV6t_DST_IPV6u_LOWERf:
                BCMCTH_IPV6_ADDR_LOWER_SET(entry->dst_ip6_addr, fval);
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_COLLECTOR_IPV6t_SRC_IPV6u_UPPERf:
                BCMCTH_IPV6_ADDR_UPPER_SET(entry->src_ip6_addr, fval);
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_COLLECTOR_IPV6t_SRC_IPV6u_LOWERf:
                BCMCTH_IPV6_ADDR_LOWER_SET(entry->src_ip6_addr, fval);
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_COLLECTOR_IPV6t_TOSf:
                entry->tos = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_COLLECTOR_IPV6t_TTLf:
                entry->ttl = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_COLLECTOR_IPV6t_DST_L4_UDP_PORTf:
                entry->dst_port = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_COLLECTOR_IPV6t_SRC_L4_UDP_PORTf:
                entry->src_port = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_COLLECTOR_IPV6t_UDP_CHKSUMf:
                entry->udp_chksum = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;

            case MON_COLLECTOR_IPV6t_FLOW_LABELf:
                entry->flow_label = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;

            case MON_COLLECTOR_IPV6t_SYSTEM_ID_LENf:
                entry->system_id_len = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_COLLECTOR_IPV6t_SYSTEM_IDf:
                entry->system_id[idx] = (uint8_t)fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_COLLECTOR_IPV6t_COMPONENT_IDf:
                entry->component_id = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_COLLECTOR_IPV6t_OBSERVATION_DOMAINf:
                entry->observation_domain = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_COLLECTOR_IPV6t_IPFIX_ENTERPRISE_NUMBERf:
                entry->enterprise_number = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief imm MON_EXPORT_PROFILEt  input fields parsing.
 *
 * Parse imm MON_EXPORT_PROFILEt input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key IMM input key field array.
 * \param [in] data IMM input data field array.
 * \param [out] export profile info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c key \c data to \c ltcfg.
 */
static int
mon_export_profile_lt_fields_parse(
    int unit,
    const bcmltd_field_t *key,
    const bcmltd_field_t *data,
    bcmcth_mon_export_profile_t *entry)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    sal_memset(entry, 0, sizeof(*entry));

    gen_field = key;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
            case MON_EXPORT_PROFILEt_MON_EXPORT_PROFILE_IDf:
                entry->export_profile_id = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
            case MON_EXPORT_PROFILEt_WIRE_FORMATf:
                entry->wire_format = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_EXPORT_PROFILEt_INTERVALf:
                entry->export_interval = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_EXPORT_PROFILEt_PACKET_LEN_INDICATORf:
                entry->packet_len_indicator = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_EXPORT_PROFILEt_MAX_PKT_LENGTHf:
                entry->max_packet_length = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_EXPORT_PROFILEt_NUM_RECORDSf:
                entry->num_records = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
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
 * \param [in] key This is a linked list of the key fields in the entry.
 * \param [in] data This is a linked list of the data fields in the entry.
 * \param [in] context Is a pointer that was given during registration.
 *                     The callback can use this pointer to retrieve some
 *                     context.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
mon_collector_ipv4_stage(
    int unit,
    bcmltd_sid_t sid,
    uint32_t trans_id,
    bcmimm_entry_event_t event,
    const bcmltd_field_t *key,
    const bcmltd_field_t *data,
    void *context,
    bcmltd_fields_t *output_fields)
{
    bcmcth_mon_collector_ipv4_t entry;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
                (mon_collector_ipv4_lt_fields_parse(unit, key, data, &entry));

    if (output_fields) {
        output_fields->count = 0;
    }

    /*Telemetry Apps have to register for the call backs */
    switch (event) {
        case BCMIMM_ENTRY_INSERT:
        case BCMIMM_ENTRY_UPDATE:

            break;
        case BCMIMM_ENTRY_DELETE:

            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Handle the different events for INT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_int_imm_collector_ipv4_handler(unit, event, &entry));

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
 * \param [in] key This is a linked list of the key fields in the entry.
 * \param [in] dat This is a linked list of the data fields in the entry.
 * \param [in] context Is a pointer that was given during registration.
 *                     The callback can use this pointer to retrieve some
 *                     context.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
mon_collector_ipv6_stage(
    int unit,
    bcmltd_sid_t sid,
    uint32_t trans_id,
    bcmimm_entry_event_t event,
    const bcmltd_field_t *key,
    const bcmltd_field_t *data,
    void *context,
    bcmltd_fields_t *output_fields)
{
    bcmcth_mon_collector_ipv6_t entry;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
                (mon_collector_ipv6_lt_fields_parse(unit, key, data, &entry));

    if (output_fields) {
        output_fields->count = 0;
    }
    /*Telemetry Apps have to register for the call backs */
    switch (event) {
        case BCMIMM_ENTRY_INSERT:
        case BCMIMM_ENTRY_UPDATE:

            break;
        case BCMIMM_ENTRY_DELETE:

            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Handle the different events for INT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_int_imm_collector_ipv6_handler(unit, event, &entry));

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
 * \param [in] key This is a linked list of the key fields in the entry.
 * \param [in] data This is a linked list of the data fields in the entry.
 * \param [in] context Is a pointer that was given during registration.
 *                     The callback can use this pointer to retrieve some
 *                     context.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
mon_export_profile_stage(
    int unit,
    bcmltd_sid_t sid,
    uint32_t trans_id,
    bcmimm_entry_event_t event,
    const bcmltd_field_t *key,
    const bcmltd_field_t *data,
    void *context,
    bcmltd_fields_t *output_fields)
{
    bcmcth_mon_export_profile_t entry;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
                (mon_export_profile_lt_fields_parse(unit, key, data, &entry));

    if (output_fields) {
        output_fields->count = 0;
    }
    /*Telemetry Apps have to register for the call backs */
    switch (event) {
        case BCMIMM_ENTRY_INSERT:
        case BCMIMM_ENTRY_UPDATE:

            break;
        case BCMIMM_ENTRY_DELETE:

            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Handle the different events for INT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_int_imm_export_profile_handler(unit, event, &entry));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief MON_COLLECTOR_IPV4 In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to MON_COLLECTOR_IPV4 logical table entry commit stages.
 */
static bcmimm_lt_cb_t collector_ipv4_event_hdl = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = mon_collector_ipv4_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};


/*!
 * \brief MON_COLLECTOR_IPV6 In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to MON_COLLECTOR_IPV6 logical table entry commit stages.
 */
static bcmimm_lt_cb_t collector_ipv6_event_hdl = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = mon_collector_ipv6_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/*!
 * \brief MON_EXPORT_PROFILE In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to MON_EXPORT_PROFILE logical table entry commit stages.
 */
static bcmimm_lt_cb_t export_profile_event_hdl = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = mon_export_profile_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/*******************************************************************************
 * Public Functions
 */

int
bcmcth_mon_collector_ipv4_entry_get(int unit,
                                    uint32_t collector_id,
                                    bcmcth_mon_collector_ipv4_t *entry)
{
    bcmltd_fields_t in = {0};
    bcmltd_fields_t out = {0};
    bcmltd_field_t *in_head, *out_head;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (collector_fields_alloc(unit, 1, &in));
    in.field[0]->id = MON_COLLECTOR_IPV4t_MON_COLLECTOR_IPV4_IDf;
    in.field[0]->data = collector_id;

    SHR_IF_ERR_VERBOSE_EXIT
        (collector_fields_alloc(unit,
                                MON_COLLECTOR_IPV4t_FIELD_COUNT + PROTOBUF_SYSTEM_ID_MAX_LENGTH,
                                &out));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_lookup(unit, MON_COLLECTOR_IPV4t,
                             &in, &out));

    /* The out fields are organized as an array of pointers. Convert it to a LL
     * since that is what the parse function expects.
     */
    field_arr_to_ll(in.count, in.field, &in_head);
    field_arr_to_ll(out.count, out.field, &out_head);

    SHR_IF_ERR_VERBOSE_EXIT
        (mon_collector_ipv4_lt_fields_parse(unit,
                                            in_head, out_head,
                                            entry));

exit:
    collector_fields_free(unit, 1, &in);
    collector_fields_free(unit,
                          MON_COLLECTOR_IPV4t_FIELD_COUNT + PROTOBUF_SYSTEM_ID_MAX_LENGTH, &out);
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_collector_ipv6_entry_get(int unit,
                                    uint32_t collector_id,
                                    bcmcth_mon_collector_ipv6_t *entry)
{
    bcmltd_fields_t in = {0};
    bcmltd_fields_t out = {0};
    bcmltd_field_t *in_head, *out_head;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (collector_fields_alloc(unit, 1, &in));
    in.field[0]->id = MON_COLLECTOR_IPV6t_MON_COLLECTOR_IPV6_IDf;
    in.field[0]->data = collector_id;

    SHR_IF_ERR_VERBOSE_EXIT
        (collector_fields_alloc(unit,
                                MON_COLLECTOR_IPV6t_FIELD_COUNT + PROTOBUF_SYSTEM_ID_MAX_LENGTH,
                                &out));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_lookup(unit, MON_COLLECTOR_IPV6t,
                             &in, &out));

    /* The out fields are organized as an array of pointers. Convert it to a LL
     * since that is what the parse function expects.
     */
    field_arr_to_ll(in.count, in.field, &in_head);
    field_arr_to_ll(out.count, out.field, &out_head);

    SHR_IF_ERR_VERBOSE_EXIT
        (mon_collector_ipv6_lt_fields_parse(unit,
                                            in_head, out_head,
                                            entry));

exit:
    collector_fields_free(unit, 1, &in);
    collector_fields_free(unit,
                          MON_COLLECTOR_IPV6t_FIELD_COUNT + PROTOBUF_SYSTEM_ID_MAX_LENGTH, &out);
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_export_profile_entry_get(int unit,
                                    uint32_t export_profile_id,
                                    bcmcth_mon_export_profile_t *entry)
{
    bcmltd_fields_t in = {0};
    bcmltd_fields_t out = {0};
    bcmltd_field_t *in_head, *out_head;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (collector_fields_alloc(unit, 1, &in));
    in.field[0]->id = MON_EXPORT_PROFILEt_MON_EXPORT_PROFILE_IDf;
    in.field[0]->data = export_profile_id;

    SHR_IF_ERR_VERBOSE_EXIT
        (collector_fields_alloc(unit,
                                MON_EXPORT_PROFILEt_FIELD_COUNT,
                                &out));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_lookup(unit, MON_EXPORT_PROFILEt,
                             &in, &out));

    /* The out fields are organized as an array of pointers. Convert it to a LL
     * since that is what the parse function expects.
     */
    field_arr_to_ll(in.count, in.field, &in_head);
    field_arr_to_ll(out.count, out.field, &out_head);

    SHR_IF_ERR_VERBOSE_EXIT
        (mon_export_profile_lt_fields_parse(unit,
                                            in_head, out_head,
                                            entry));

exit:
    collector_fields_free(unit, 1, &in);
    collector_fields_free(unit, MON_EXPORT_PROFILEt_FIELD_COUNT, &out);
    SHR_FUNC_EXIT();
}

static bool
bcmcth_mon_collector_imm_mapped(int unit, const bcmdrd_sid_t sid)
{
    int rv;
    const bcmlrd_map_t *map = NULL;

    rv = bcmlrd_map_get(unit, sid, &map);
    if (SHR_SUCCESS(rv) &&
        map &&
        map->group &&
        map->group[0].dest.kind == BCMLRD_MAP_CUSTOM) {
        return TRUE;
    }
    return FALSE;
}


int
bcmcth_mon_collector_imm_register(int unit)
{
    SHR_FUNC_ENTER(unit);

    /*
     * To register callback for MON COLLECTOR LTs here.
     */
    if (bcmcth_mon_collector_imm_mapped(unit, MON_COLLECTOR_IPV4t)) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit, MON_COLLECTOR_IPV4t,
                                 &collector_ipv4_event_hdl, NULL));
    }
    if (bcmcth_mon_collector_imm_mapped(unit, MON_COLLECTOR_IPV6t)) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit, MON_COLLECTOR_IPV6t,
                                 &collector_ipv6_event_hdl, NULL));
    }
    if (bcmcth_mon_collector_imm_mapped(unit, MON_EXPORT_PROFILEt)) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit, MON_EXPORT_PROFILEt,
                                 &export_profile_event_hdl, NULL));
    }
exit:
    SHR_FUNC_EXIT();
}
