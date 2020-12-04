/*! \file bcmcth_mon_flowtracker_flow_static.c
 *
 * This file contains Flowtracker custom handler fucntions
 * for handling insert, delete, update ,lookup and
 * validate of MON_FLOWTRACKER_FLOW_STATIC table.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_util_pack.h>
#include <bsl/bsl.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmbd/bcmbd.h>
#include <bcmbd/mcs_shared/mcs_mon_flowtracker_msg.h>
#include <bcmptm/bcmptm.h>
#include <bcmcth/bcmcth_util.h>
#include <bcmcth/bcmcth_mon_flowtracker_flow.h>
#include <bcmcth/bcmcth_mon_flowtracker_static.h>
#include <bcmcth/bcmcth_mon_flowtracker_int.h>

#define BSL_LOG_MODULE  BSL_LS_BCMCTH_MON

/*! Hash table handles for max number of units */
static shr_hash_str_hdl static_flow_db[BCMDRD_CONFIG_MAX_UNITS] = {NULL};

/*! Head ptr of hash entries for max number of units */
static bcmcth_mon_flowtracker_static_p
static_flow_db_entry_head[BCMDRD_CONFIG_MAX_UNITS] = {NULL};

/*! Hash entries for a given unit */
static bcmcth_mon_flowtracker_static_t *static_flow_db_entry = NULL;


/*!
 * \brief Prints variable into key character buffer.
 *
 * \param [in] _buf  Buffer pointer where data need to be printed.
 * \param [in] _var  Variable to print.
 * \param [in] _size Remaining size in buffer.
 *
 * \return Incremented buffer pointer.
 */
#define BCMCTH_MON_FT_PRINT_VAR_TO_KEY(_buf, _var, _size)  \
    do {                                                         \
        int tmp_sz = 0;                                          \
        tmp_sz = sal_snprintf(_buf, _size, "%x", _var);          \
        if (tmp_sz >= _size) {                                   \
            return;                                              \
        }                                                        \
        (_buf)  += tmp_sz;                                       \
        (_size) -= tmp_sz;                                       \
    } while (0)

#define BCMCTH_MON_FT_PRINT_64_VAR_TO_KEY(_buf, _var, _size)                  \
    do {                                                                      \
        int tmp_sz = 0;                                                       \
        tmp_sz = sal_snprintf(_buf, _size, "%llx", (unsigned long long)_var); \
        if (tmp_sz >= _size) {                                                \
            return;                                                           \
        }                                                                     \
        (_buf)  += tmp_sz;                                                    \
        (_size) -= tmp_sz;                                                    \
    } while (0)

static void
bcmcth_mon_flowtracker_static_flow_db_key_form
                  (int unit,
                   bcmcth_mon_flowtracker_static_t *entry)
{
    char *buf = NULL;
    int remaining_size = sizeof(entry->static_flow_db_key);
    int i;

    buf = &(entry->static_flow_db_key[0]);

    if (SHR_BITGET(entry->fbmp,
          MON_FLOWTRACKER_FLOW_STATICt_MON_FLOWTRACKER_GROUP_IDf)) {
        BCMCTH_MON_FT_PRINT_VAR_TO_KEY(buf, entry->group_id,
                                       remaining_size);
    }
    if (SHR_BITGET(entry->fbmp,
          MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_SRC_IPV4f )) {
        BCMCTH_MON_FT_PRINT_VAR_TO_KEY(buf, entry->src_ipv4,
                                       remaining_size);
    }
    if (SHR_BITGET(entry->fbmp,
          MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_DST_IPV4f)) {
        BCMCTH_MON_FT_PRINT_VAR_TO_KEY(buf, entry->dst_ipv4,
                                       remaining_size);
    }
    if (SHR_BITGET(entry->fbmp,
          MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_SRC_L4_PORTf)) {
        BCMCTH_MON_FT_PRINT_VAR_TO_KEY(buf, entry->src_l4_port,
                                       remaining_size);
    }
    if (SHR_BITGET(entry->fbmp,
          MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_DST_L4_PORTf)) {
        BCMCTH_MON_FT_PRINT_VAR_TO_KEY(buf, entry->dst_l4_port,
                                       remaining_size);
    }
    if (SHR_BITGET(entry->fbmp,
          MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_IP_PROTOf)) {
        BCMCTH_MON_FT_PRINT_VAR_TO_KEY(buf, entry->ip_proto,
                                       remaining_size);
    }
    if (SHR_BITGET(entry->fbmp,
          MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_INNER_SRC_IPV4f)) {
        BCMCTH_MON_FT_PRINT_VAR_TO_KEY(buf, entry->inner_src_ipv4,
                                       remaining_size);
    }
    if (SHR_BITGET(entry->fbmp,
          MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_INNER_DST_IPV4f)) {
        BCMCTH_MON_FT_PRINT_VAR_TO_KEY(buf, entry->inner_dst_ipv4,
                                       remaining_size);
    }
    if (SHR_BITGET(entry->fbmp,
          MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_INNER_SRC_L4_PORTf)) {
        BCMCTH_MON_FT_PRINT_VAR_TO_KEY(buf, entry->inner_src_l4_port,
                                       remaining_size);
    }
    if (SHR_BITGET(entry->fbmp,
          MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_INNER_DST_L4_PORTf)) {
        BCMCTH_MON_FT_PRINT_VAR_TO_KEY(buf, entry->inner_dst_l4_port,
                                       remaining_size);
    }
    if (SHR_BITGET(entry->fbmp,
          MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_INNER_IP_PROTOf)) {
        BCMCTH_MON_FT_PRINT_VAR_TO_KEY(buf, entry->inner_ip_proto,
                                       remaining_size);
    }
    if (SHR_BITGET(entry->fbmp,
          MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_VNIDf)) {
        BCMCTH_MON_FT_PRINT_VAR_TO_KEY(buf, entry->vnid,
                                       remaining_size);
    }
    if (SHR_BITGET(entry->fbmp,
          MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_CUSTOM_KEY_UPPERf)) {
        BCMCTH_MON_FT_PRINT_64_VAR_TO_KEY(buf, entry->custom_key_upper,
                                       remaining_size);
    }
    if (SHR_BITGET(entry->fbmp,
          MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_CUSTOM_KEY_LOWERf)) {
        BCMCTH_MON_FT_PRINT_64_VAR_TO_KEY(buf, entry->custom_key_lower,
                                       remaining_size);
    }
    if (SHR_BITGET(entry->fbmp,
          MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_ING_PORT_IDf)) {
        BCMCTH_MON_FT_PRINT_VAR_TO_KEY(buf, entry->ing_port_id,
                                       remaining_size);
    }
    if (SHR_BITGET(entry->fbmp,
                   MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_INNER_SRC_IPV6_UPPERf)) {
        for (i = 0; i < 8; i++) {
            BCMCTH_MON_FT_PRINT_VAR_TO_KEY(buf, entry->inner_src_ipv6[i],
                                           remaining_size);
        }
    }
    if (SHR_BITGET(entry->fbmp,
                   MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_INNER_SRC_IPV6_LOWERf)) {
        for (i = 8; i < 16; i++) {
            BCMCTH_MON_FT_PRINT_VAR_TO_KEY(buf, entry->inner_src_ipv6[i],
                                           remaining_size);
        }
    }
    if (SHR_BITGET(entry->fbmp,
                   MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_INNER_DST_IPV6_UPPERf)) {
        for (i = 0; i < 8; i++) {
            BCMCTH_MON_FT_PRINT_VAR_TO_KEY(buf, entry->inner_dst_ipv6[i],
                                           remaining_size);
        }
    }
    if (SHR_BITGET(entry->fbmp,
                   MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_INNER_DST_IPV6_LOWERf)) {
        for (i = 8; i < 16; i++) {
            BCMCTH_MON_FT_PRINT_VAR_TO_KEY(buf, entry->inner_dst_ipv6[i],
                                           remaining_size);
        }
    }
}

static bcmcth_mon_flowtracker_static_t *
bcmcth_mon_flowtracker_static_flow_db_free_entry_get(int unit)
{
    bcmcth_mon_flowtracker_static_t *entry_head = NULL;
    bcmlrd_sid_t sid = MON_FLOWTRACKER_FLOW_STATICt;
    int rv;
    uint32_t i;
    uint32_t max_flows = 0;

    entry_head = static_flow_db_entry_head[unit];
    if (entry_head == NULL) {
        return NULL;
    }

    rv = bcmlrd_map_table_attr_get(unit, sid,
            BCMLRD_MAP_TABLE_ATTRIBUTE_ENTRY_LIMIT,
            &max_flows);

    if (rv != SHR_E_NONE || max_flows == 0) {
        return NULL;
    }
    for (i = 0; i < max_flows; i++) {
        if (entry_head[i].is_used == false) {
            return (&(entry_head[i]));
        }
    }
    return NULL;
}

static int
bcmcth_mon_flowtracker_static_flow_db_insert
                            (int unit,
                             bcmcth_mon_flowtracker_static_t *entry)
{
    bcmcth_mon_flowtracker_static_t *data = NULL;
    SHR_FUNC_ENTER(unit);

    data = bcmcth_mon_flowtracker_static_flow_db_free_entry_get(unit);
    if (data == NULL) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }
    sal_memcpy(data, entry, sizeof(bcmcth_mon_flowtracker_static_t));

    /* Data is filled with ptr to entry structure */
    SHR_IF_ERR_VERBOSE_EXIT(shr_hash_str_dict_insert(static_flow_db[unit],
                                             &(data->static_flow_db_key[0]),
                                             data));
    /* Mark the entry used */
    data->is_used = true;
exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_flowtracker_static_flow_db_lookup
                            (int unit,
                             bcmcth_mon_flowtracker_static_t *entry)
{
    void *p_val = NULL;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(shr_hash_str_dict_lookup(static_flow_db[unit],
                                             &(entry->static_flow_db_key[0]),
                                             &p_val));
    sal_memcpy(entry, p_val, sizeof(bcmcth_mon_flowtracker_static_t));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_flowtracker_static_flow_db_delete
                            (int unit,
                             bcmcth_mon_flowtracker_static_t *entry)
{
    void *p_val = NULL;
    bcmcth_mon_flowtracker_static_t *ptr = NULL;
    SHR_FUNC_ENTER(unit);

    /* First look it up and get the pointer to entry */
    SHR_IF_ERR_VERBOSE_EXIT(shr_hash_str_dict_lookup(static_flow_db[unit],
                                             &(entry->static_flow_db_key[0]),
                                             &p_val));

    SHR_IF_ERR_VERBOSE_EXIT(shr_hash_str_dict_delete(static_flow_db[unit],
                                             &(entry->static_flow_db_key[0])));
    ptr = p_val;
    /* Mark it un-used */
    ptr->is_used = false;
exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_flowtracker_static_flow_db_get_first
                            (int unit,
                             bcmcth_mon_flowtracker_static_t *entry)
{
    void *p_val = NULL;
    char *flow_db_key;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(shr_hash_str_get_first(static_flow_db[unit],
                                             &flow_db_key,
                                             &p_val));
    sal_memcpy(entry, p_val, sizeof(bcmcth_mon_flowtracker_static_t));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_flowtracker_static_flow_db_get_next
                            (int unit,
                             bcmcth_mon_flowtracker_static_t *entry)
{
    void *p_val = NULL;
    char *flow_db_key;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(shr_hash_str_get_next(static_flow_db[unit],
                                          &flow_db_key,
                                          &p_val));
    sal_memcpy(entry, p_val, sizeof(bcmcth_mon_flowtracker_static_t));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_flowtracker_reinsert_used_entries(int unit)
{

    bcmcth_mon_flowtracker_static_t *entry_head = NULL;
    bcmcth_mon_flowtracker_static_t *data = NULL;
    bcmlrd_sid_t sid = MON_FLOWTRACKER_FLOW_STATICt;
    int rv;
    uint32_t i;
    uint32_t max_flows = 0;
    SHR_FUNC_ENTER(unit);

    entry_head = static_flow_db_entry_head[unit];
    if (entry_head == NULL) {
        SHR_EXIT();
    }

    rv = bcmlrd_map_table_attr_get(unit, sid,
            BCMLRD_MAP_TABLE_ATTRIBUTE_ENTRY_LIMIT,
            &max_flows);

    if (rv != SHR_E_NONE || max_flows == 0) {
        SHR_EXIT();
    }
    for (i = 0; i < max_flows; i++) {
        /* Reinsert used entries */
        if (entry_head[i].is_used == true) {
            data = &(entry_head[i]);
            /* Data is filled with ptr to entry structure */
            SHR_IF_ERR_VERBOSE_EXIT(
                shr_hash_str_dict_insert(static_flow_db[unit],
                                         &(data->static_flow_db_key[0]),
                                         data));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_flowtracker_static_fields_parse(int unit, const bcmltd_fields_t *in,
                                           bcmcth_mon_flowtracker_static_t *param)
{

    uint32_t idx, fid;
    uint64_t fval;
    bcmcth_mon_flowtracker_static_t *entry = param;
    SHR_FUNC_ENTER(unit);

    if ((in == NULL) || (entry == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    for (idx = 0; idx < in->count; idx++) {
        fid      = in->field[idx]->id;
        fval    = in->field[idx]->data;

        switch (fid) {
            case MON_FLOWTRACKER_FLOW_STATICt_MON_FLOWTRACKER_GROUP_IDf:
                entry->group_id = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_SRC_IPV4f:
                entry->src_ipv4 = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_DST_IPV4f:
                entry->dst_ipv4 = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_SRC_L4_PORTf:
                entry->src_l4_port = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_DST_L4_PORTf:
                entry->dst_l4_port = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_IP_PROTOf:
                entry->ip_proto = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_INNER_SRC_IPV4f:
                entry->inner_src_ipv4 = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_INNER_DST_IPV4f:
                entry->inner_dst_ipv4 = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_INNER_SRC_L4_PORTf:
                entry->inner_src_l4_port = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_INNER_DST_L4_PORTf:
                entry->inner_dst_l4_port = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_INNER_IP_PROTOf:
                entry->inner_ip_proto = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_VNIDf:
                entry->vnid = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_CUSTOM_KEY_UPPERf:
                entry->custom_key_upper = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_CUSTOM_KEY_LOWERf:
                entry->custom_key_lower = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_ING_PORT_IDf:
                entry->ing_port_id = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_INNER_SRC_IPV6_UPPERf:
                BCMCTH_IPV6_ADDR_UPPER_SET(entry->inner_src_ipv6, fval);
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_INNER_SRC_IPV6_LOWERf:
                BCMCTH_IPV6_ADDR_LOWER_SET(entry->inner_src_ipv6, fval);
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_INNER_DST_IPV6_UPPERf:
                BCMCTH_IPV6_ADDR_UPPER_SET(entry->inner_dst_ipv6, fval);
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_INNER_DST_IPV6_LOWERf:
                BCMCTH_IPV6_ADDR_LOWER_SET(entry->inner_dst_ipv6, fval);
                SHR_BITSET(entry->fbmp, fid);
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
                break;
        }
    }
    sal_memset(entry->static_flow_db_key, 0, sizeof(entry->static_flow_db_key));
    bcmcth_mon_flowtracker_static_flow_db_key_form(unit, entry);
exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_ft_flow_static_flow_fill(int unit,
                                    bcmcth_mon_flowtracker_static_t *entry,
                                    bcmltd_fields_t *out)
{
    size_t      num_fid = 0, count = 0;
    uint32_t    idx = 0, fid = 0;
    uint32_t    table_sz = 0;
    bcmlrd_fid_t   *fid_list = NULL;
    bcmltd_field_t *field = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit, MON_FLOWTRACKER_FLOW_STATICt, &num_fid));

    table_sz = (num_fid * sizeof(bcmlrd_fid_t));
    SHR_ALLOC(fid_list, table_sz, "bcmcthMonFtStatic");
    SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);
    sal_memset(fid_list, 0, table_sz);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_list_get(unit,
                               MON_FLOWTRACKER_FLOW_STATICt,
                               num_fid,
                               fid_list,
                               &count));
    out->count = count;

    for (idx = 0; idx < count; idx++) {
        fid = fid_list[idx];
        field = out->field[idx];
        field->id = fid;

        switch (fid) {
            case MON_FLOWTRACKER_FLOW_STATICt_MON_FLOWTRACKER_GROUP_IDf:
                field->data = entry->group_id;
                break;
            case MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_SRC_IPV4f:
                field->data = entry->src_ipv4;
                break;
            case MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_DST_IPV4f:
                field->data = entry->dst_ipv4;
                break;
            case MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_SRC_L4_PORTf:
                field->data = entry->src_l4_port;
                break;
            case MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_DST_L4_PORTf:
                field->data = entry->dst_l4_port;
                break;
            case MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_IP_PROTOf:
                field->data = entry->ip_proto;
                break;
            case MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_INNER_SRC_IPV4f:
                field->data = entry->inner_src_ipv4;
                break;
            case MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_INNER_DST_IPV4f:
                field->data = entry->inner_dst_ipv4;
                break;
            case MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_INNER_SRC_L4_PORTf:
                field->data = entry->inner_src_l4_port;
                break;
            case MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_INNER_DST_L4_PORTf:
                field->data = entry->inner_dst_l4_port;
                break;
            case MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_INNER_IP_PROTOf:
                field->data = entry->inner_ip_proto;
                break;
            case MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_VNIDf:
                field->data = entry->vnid;
                break;
            case MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_CUSTOM_KEY_UPPERf:
                field->data = entry->custom_key_upper;
                break;
            case MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_CUSTOM_KEY_LOWERf:
                field->data = entry->custom_key_lower;
                break;
            case MON_FLOWTRACKER_FLOW_STATICt_FLOW_KEYu_ING_PORT_IDf:
                field->data = entry->ing_port_id;
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_INNER_SRC_IPV6_UPPERf:
                field->data = BCMCTH_IPV6_ADDR_UPPER_GET(entry->inner_src_ipv6);
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_INNER_SRC_IPV6_LOWERf:
                field->data = BCMCTH_IPV6_ADDR_LOWER_GET(entry->inner_src_ipv6);
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_INNER_DST_IPV6_UPPERf:
                field->data = BCMCTH_IPV6_ADDR_UPPER_GET(entry->inner_dst_ipv6);
                break;
            case MON_FLOWTRACKER_FLOW_DATAt_FLOW_KEYu_INNER_DST_IPV6_LOWERf:
                field->data = BCMCTH_IPV6_ADDR_LOWER_GET(entry->inner_dst_ipv6);
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
                break;
        }
    }

exit:
    SHR_FREE(fid_list);
    SHR_FUNC_EXIT();

}

static void
bcmcth_mon_flowtracker_static_flow_set_msg_fill(
                 bcmcth_mon_flowtracker_static_t *entry,
                 mcs_ft_msg_ctrl_group_flow_data_set_t *msg)
{
    int lower_size, upper_size;

    msg->src_ipv4             = entry->src_ipv4;
    msg->dst_ipv4             = entry->dst_ipv4;
    msg->inner_src_ipv4       = entry->inner_src_ipv4;
    msg->inner_dst_ipv4       = entry->inner_dst_ipv4;
    msg->vnid                 = entry->vnid;
    msg->group_idx            = entry->group_id;
    msg->src_l4_port          = entry->src_l4_port;
    msg->dst_l4_port          = entry->dst_l4_port;
    msg->inner_src_l4_port    = entry->inner_src_l4_port;
    msg->inner_dst_l4_port    = entry->inner_dst_l4_port;
    msg->ip_proto             = entry->ip_proto;
    msg->inner_ip_proto       = entry->inner_ip_proto;
    msg->custom_length        = MCS_SHR_FT_CUSTOM_KEY_MAX_LENGTH;
    lower_size = upper_size   = MCS_SHR_FT_CUSTOM_KEY_MAX_LENGTH/2;
    sal_memcpy(&(msg->custom[0]), &entry->custom_key_lower, lower_size);
    sal_memcpy(&(msg->custom[lower_size]), &entry->custom_key_lower,
               upper_size);
    msg->in_port              =  entry->ing_port_id;
    sal_memcpy(msg->inner_src_ipv6, entry->inner_src_ipv6, sizeof(shr_ip6_t));
    sal_memcpy(msg->inner_dst_ipv6, entry->inner_dst_ipv6, sizeof(shr_ip6_t));
}

static int
bcmcth_mon_flowtracker_static_flow_set(int unit,
                                       bcmcth_mon_flowtracker_static_t *param)
{
    mcs_ft_msg_ctrl_static_flow_set_t send_msg = {0};

    SHR_FUNC_ENTER(unit);

    /* Fill up the send msg structure from static info */
    bcmcth_mon_flowtracker_static_flow_set_msg_fill(param, &send_msg);

    /* Send the static flow set msg to UKERNEL */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_ft_eapp_static_flow_set(unit, &send_msg));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_flowtracker_static_flow_delete(
                                   int unit,
                                   bcmcth_mon_flowtracker_static_t *param)
{
    mcs_ft_msg_ctrl_static_flow_set_t send_msg = {0};

    SHR_FUNC_ENTER(unit);

    /* Fill up the send msg structure from static info */
    bcmcth_mon_flowtracker_static_flow_set_msg_fill(param, &send_msg);

    /* Send the static flow delete msg to UKERNEL */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_ft_eapp_static_flow_delete(unit, &send_msg));

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

int
bcmcth_mon_flowtracker_static_validate(int unit,
                                       bcmlt_opcode_t opcode,
                                       const bcmltd_fields_t *in,
                                       const bcmltd_generic_arg_t *arg)
{
    bcmcth_mon_flowtracker_static_t param;
    bcmcth_mon_ft_info_t *info = NULL;
    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    info = bcmcth_mon_ft_info_get(unit);
    if (info == NULL) {
       SHR_ERR_EXIT(SHR_E_INIT);
    }

    /*
     * Validate if Hw learn is enabled. If yes, throw error.
     */
    if (FLOWTRACKER_HW_LEARN_ENABLED(info->ha->hw_learn)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /*
     * Validate if flow group exists.
     */
    sal_memset(&param, 0 , sizeof(bcmcth_mon_flowtracker_static_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_flowtracker_static_fields_parse(unit, in, &param));

    /* If UPDATE or DELETE, check if the entry exists */
    if ((opcode == BCMLT_OPCODE_UPDATE) ||
        (opcode == BCMLT_OPCODE_DELETE)) {
        if (bcmcth_mon_flowtracker_static_flow_db_lookup(unit, &param)
               != SHR_E_NONE) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_flowtracker_static_insert(int unit,
                                     const bcmltd_op_arg_t *op_arg,
                                     const bcmltd_fields_t *in,
                                     bcmltd_fields_t *out,
                                     const bcmltd_generic_arg_t *arg)
{
    bcmcth_mon_flowtracker_static_t param;
    bcmcth_mon_ft_info_t *info = NULL;

    SHR_FUNC_ENTER(unit);

    info = bcmcth_mon_ft_info_get(unit);
    if (info == NULL) {
       SHR_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&param, 0 , sizeof(bcmcth_mon_flowtracker_static_t));


    /* Parse the input fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_flowtracker_static_fields_parse(unit, in, &param));

    /* Insert the entry into static flow db */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_flowtracker_static_flow_db_insert(unit,
                                                                 &param));

    if (info->ha->run == true) {
        /* Set the static flow in UKERNEL by sending the msg */
        SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_flowtracker_static_flow_set(unit, &param));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_flowtracker_static_lookup(int unit,
                                     const bcmltd_op_arg_t *op_arg,
                                     const bcmltd_fields_t *in,
                                     bcmltd_fields_t *out,
                                     const bcmltd_generic_arg_t *arg)
{
    bcmcth_mon_flowtracker_static_t param;
    bcmcth_mon_ft_info_t *info = NULL;
    SHR_FUNC_ENTER(unit);

    info = bcmcth_mon_ft_info_get(unit);
    if (info == NULL) {
       SHR_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&param, 0 , sizeof(bcmcth_mon_flowtracker_static_t));

    /* Parse the input fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_flowtracker_static_fields_parse(unit, in, &param));

    /* Lookup the entry from static flow db */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_flowtracker_static_flow_db_lookup(unit,&param));

    /* Fill the out argument with appropriate info from the structure. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_flow_static_flow_fill(unit, &param, out));
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_flowtracker_static_delete(int unit,
                                     const bcmltd_op_arg_t *op_arg,
                                     const bcmltd_fields_t *in,
                                     bcmltd_fields_t *out,
                                     const bcmltd_generic_arg_t *arg)
{
    bcmcth_mon_flowtracker_static_t param;
    bcmcth_mon_ft_info_t *info = NULL;
    SHR_FUNC_ENTER(unit);

    info = bcmcth_mon_ft_info_get(unit);
    if (info == NULL) {
       SHR_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&param, 0 , sizeof(bcmcth_mon_flowtracker_static_t));

    /* Parse the input fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_flowtracker_static_fields_parse(unit, in, &param));

    /* Delete the entry from static flow db */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_flowtracker_static_flow_db_delete(unit,
                &param));

    if (info->ha->run == true) {
        /* Send static flow delete msg to UKERNEL */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_flowtracker_static_flow_delete(unit, &param));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_flowtracker_static_update(int unit,
                                     const bcmltd_op_arg_t *op_arg,
                                     const bcmltd_fields_t *in,
                                     bcmltd_fields_t *out,
                                     const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(op_arg);
    COMPILER_REFERENCE(in);
    COMPILER_REFERENCE(out);
    COMPILER_REFERENCE(arg);

    /*
     * This is a data less LT.
     * So there is nothing to update.
     */
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_flowtracker_static_first(int unit,
                                    const bcmltd_op_arg_t *op_arg,
                                    const bcmltd_fields_t *in,
                                    bcmltd_fields_t *out,
                                    const bcmltd_generic_arg_t *arg)
{
    bcmcth_mon_flowtracker_static_t param;
    SHR_FUNC_ENTER(unit);

    sal_memset(&param, 0 , sizeof(bcmcth_mon_flowtracker_static_t));

    /* Get the first static flow entry from db */
    SHR_IF_ERR_VERBOSE_EXIT(
                bcmcth_mon_flowtracker_static_flow_db_get_first(unit,
                &param));

    /* Fill the out argument with appropriate info from the structure. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_flow_static_flow_fill(unit, &param, out));
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_flowtracker_static_next(int unit,
                                   const bcmltd_op_arg_t *op_arg,
                                   const bcmltd_fields_t *in,
                                   bcmltd_fields_t *out,
                                   const bcmltd_generic_arg_t *arg)
{
    bcmcth_mon_flowtracker_static_t param;
    SHR_FUNC_ENTER(unit);

    sal_memset(&param, 0 , sizeof(bcmcth_mon_flowtracker_static_t));

    /* Parse the input fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_flowtracker_static_fields_parse(unit, in, &param));

    /* Get the next static flow entry from db */
    SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_mon_flowtracker_static_flow_db_get_next(unit,
                &param));

    /* Fill the out argument with appropriate info from the structure. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_flow_static_flow_fill(unit, &param, out));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_flowtracker_static_flows_db_init(int unit, bool warm)
{
    bcmlrd_sid_t sid = MON_FLOWTRACKER_FLOW_STATICt;
    uint32_t max_flows = 0;
    uint32_t ha_alloc_size = 0;
    SHR_FUNC_ENTER(unit);


    SHR_IF_ERR_VERBOSE_EXIT(bcmlrd_map_table_attr_get(unit, sid,
                BCMLRD_MAP_TABLE_ATTRIBUTE_ENTRY_LIMIT,
                &max_flows));

    if (max_flows == 0) {
        SHR_EXIT();
    }


    /* Allocate the static flow db hash table entries */
    ha_alloc_size = (max_flows * sizeof(bcmcth_mon_flowtracker_static_t));

    static_flow_db_entry = shr_ha_mem_alloc(unit, BCMMGMT_MON_COMP_ID,
                            BCMMON_FLOWTRACKER_STATIC_FLOW_ENTRY_SUB_COMP_ID,
                            "flowtrackerStaticFlowDb",
                            &ha_alloc_size);
    SHR_NULL_CHECK(static_flow_db_entry, SHR_E_MEMORY);
    if (!warm) {
        sal_memset(static_flow_db_entry, 0, ha_alloc_size);
    }

    /* Point the entry head to the static flow db entry's ptr */
    static_flow_db_entry_head[unit] = static_flow_db_entry;

    /*
     * Create/alloc the static flow database.
     * The string dictionary based on hash so limit the number of buckets to
     * save some memory which might increase the search time for heavily
     * loaded table
     */
    SHR_IF_ERR_VERBOSE_EXIT(
            shr_hash_str_dict_alloc(max_flows / 8, &(static_flow_db[unit])));

    if (warm) {
        /* Re-insert the hash entries into hash table for that unit */
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmcth_mon_flowtracker_reinsert_used_entries(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_flowtracker_static_flows_db_free(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(
            shr_hash_str_dict_free(static_flow_db[unit]));
exit:
    SHR_FUNC_EXIT();
}
