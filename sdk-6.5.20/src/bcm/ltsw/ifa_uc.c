/*! \file ifa_uc.c
 *
 * API interface for IFA embedded app.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/types.h>
#include <bcm_int/control.h>

#include <bcm_int/ltsw/property.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/collector.h>

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>

#define BSL_LOG_MODULE BSL_LS_BCM_IFA

/* IFA IPFIX template ID. Only a single fixed template is supported. */
#define IFA_TEMPLATE_ID 1

/* IFA EApp global data structure. */
typedef struct ifa_uc_s {
    /* Indicates if the EApp is running or not. */
    bool run;
} ifa_uc_t;

static ifa_uc_t ifa_uc[BCM_MAX_NUM_UNITS] = {{0}};

/******************************************************************************
 * Private functions
 */

/* Set the value of a field in the entry. */
static void
field_val_set(bcmi_lt_entry_t *entry, const char *fld_name,
              uint16_t idx, uint64_t val)
{
    int i;

    for (i = 0; i < entry->nfields; i++) {
        if ((sal_strcmp(entry->fields[i].fld_name, fld_name) == 0) &&
            entry->fields[i].idx == idx) {
            entry->fields[i].u.val = val;
            return;
        }
    }
}

/* Get the value of a field in the entry. */
static uint64_t
field_val_get(bcmi_lt_entry_t *entry, const char *fld_name, uint16_t idx)
{
    int i;

    for (i = 0; i < entry->nfields; i++) {
        if ((sal_strcmp(entry->fields[i].fld_name, fld_name) == 0) &&
            entry->fields[i].idx == idx) {
            return entry->fields[i].u.val;
        }
    }
    return 0;
}

/* Set the value of a symbol field in the entry. */
static void
field_sym_set(bcmi_lt_entry_t *entry, const char *fld_name,
              uint16_t idx, const char *sym)
{
    int i;

    for (i = 0; i < entry->nfields; i++) {
        if ((sal_strcmp(entry->fields[i].fld_name, fld_name) == 0) &&
            entry->fields[i].idx == idx) {
            entry->fields[i].u.sym_val = sym;
            return;
        }
    }
}

/* Get the value of a symbol field in the entry. */
static const char *
field_sym_get(bcmi_lt_entry_t *entry, const char *fld_name, uint16_t idx)
{
    int i;

    for (i = 0; i < entry->nfields; i++) {
        if ((sal_strcmp(entry->fields[i].fld_name, fld_name) == 0) &&
            entry->fields[i].idx == idx) {
            return entry->fields[i].u.sym_val;
        }
    }
    return NULL;
}

/* Set the same flag for all fields in the entry. */
static void
fields_flag_set(bcmi_lt_entry_t *entry, uint32_t flags)
{
    int i;

    for (i = 0; i < entry->nfields; i++) {
        entry->fields[i].flags |= flags;
    }
}

/* Oper state to error code translation table. */
typedef struct oper_state_xlate_s {
    /* LT name. */
    const char *tbl_name;

    struct {
        /* Oper state. */
        const char *state;

        /* Return code. */
        int rv;
    } s[32];
} oper_state_xlate_t;

oper_state_xlate_t oper_state_xlate[] = {
    {
        MON_INBAND_TELEMETRY_CONTROLs,
        {
            {SUCCESSs,                                  SHR_E_NONE},
            {APP_NOT_INITIALIZEDs,                      SHR_E_INIT},
            {COLLECTOR_NOT_EXISTSs,                     SHR_E_NOT_FOUND},
            {EXPORT_PROFILE_NOT_EXISTSs,                SHR_E_NOT_FOUND},
            {EXPORT_PROFILE_WIRE_FORMAT_NOT_SUPPORTEDs, SHR_E_PARAM},
            {EXPORT_PROFILE_INVALID_MAX_PKT_LENGTHs,    SHR_E_PARAM},
            {NULL,                                      SHR_E_NOT_FOUND},
        },
    },
    {

        MON_INBAND_TELEMETRY_IPFIX_EXPORTs,
        {
            {SUCCESSs,               SHR_E_NONE},
            {APP_NOT_INITIALIZEDs,   SHR_E_INIT},
            {NULL,                   SHR_E_NOT_FOUND},
        },
    }
};

/*
 * Get the oper state of a keyless table and convert to appropriate return code.
 */
static int
oper_state_check(int unit, const char *tbl_name)
{
    int i, j;
    const char *oper_state;
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        /* 0 */  {OPERATIONAL_STATEs, BCMI_LT_FIELD_F_SYMBOL |
                                      BCMI_LT_FIELD_F_GET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    bcmi_lt_entry_init(&entry, fields);
    entry.nfields = COUNTOF(fields);
    SHR_IF_ERR_VERBOSE_EXIT(bcmi_lt_entry_get(unit, tbl_name, &entry, NULL, NULL));
    oper_state = field_sym_get(&entry, OPERATIONAL_STATEs, 0);

    /* Convert the oper state to a SHR_E_XXX return code and return it. */
    for (i = 0; i < COUNTOF(oper_state_xlate); i++) {
        if (strcmp(tbl_name, oper_state_xlate[i].tbl_name) == 0) {
            for (j = 0; oper_state_xlate[i].s[j].state != NULL; j++) {
                if (strcmp(oper_state_xlate[i].s[j].state, oper_state) == 0) {
                    SHR_VERBOSE_EXIT(oper_state_xlate[i].s[j].rv);
                }
            }
        }
    }
    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

 exit:
    SHR_FUNC_EXIT();
}

typedef struct template_sym_map_s {
    /* SDK-6 API. */
    bcm_ifa_export_element_type_t element;

    /* Symbol to be used in API. */
    const char *sym;
} template_sym_map_t;

static template_sym_map_t tmpl_map[] = {
    {bcmIfaExportElementTypeIFAHeaders,      "IFA_HEADERS"},
    {bcmIfaExportElementTypeMetadataStack,   "IFA_METADATA_STACK"},
    {bcmIfaExportElementTypePktData,         "RX_PKT_NO_IFA"},
};
static const char *
tmpl_sym_get(bcm_ifa_export_element_type_t element)
{
    int i;

    for (i = 0; i < COUNTOF(tmpl_map); i++) {
        if (element == tmpl_map[i].element) {
            return tmpl_map[i].sym;
        }
    }

    assert(0);
    return NULL;
}

static bcm_ifa_export_element_type_t
tmpl_get(const char *sym)
{
    int i;

    for (i = 0; i < COUNTOF(tmpl_map); i++) {
        if (sal_strcmp(sym, tmpl_map[i].sym)) {
            return tmpl_map[i].element;
        }
    }

    assert(0);
    return 0;
}

/* Check if the template ID is valid and configured. */
static int
template_id_check(int unit, bcm_ifa_export_template_t template_id)
{
    bcmi_lt_entry_t entry;

    SHR_FUNC_ENTER(unit);

    /* Only a single fixed template is supported. */
    if (template_id != IFA_TEMPLATE_ID) {
        SHR_ERR_EXIT(SHR_E_BADID);
    }
    bcmi_lt_entry_init(&entry, NULL);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, MON_INBAND_TELEMETRY_IPFIX_EXPORTs,
                           &entry, NULL, NULL));

 exit:
    SHR_FUNC_EXIT();
}

/* Get the attached collector and export profile ID. */
static int
collector_attach_get(int unit, bcm_collector_t *collector_id,
                     int *export_profile_id)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        /* 0 */  {COLLECTOR_TYPEs,         BCMI_LT_FIELD_F_SYMBOL |
                                           BCMI_LT_FIELD_F_GET     , 0, {0}},
        /* 1 */  {MON_COLLECTOR_IPV4_IDs,  BCMI_LT_FIELD_F_GET     , 0, {0}},
        /* 2 */  {MON_COLLECTOR_IPV6_IDs,  BCMI_LT_FIELD_F_GET     , 0, {0}},
        /* 3 */  {MON_EXPORT_PROFILE_IDs,  BCMI_LT_FIELD_F_GET     , 0, {0}},
    };
    bcmlt_field_def_t fld_def;
    const char *coll_type;
    uint64_t collector_int_id;
    bcm_ltsw_collector_transport_type_t type;

    SHR_FUNC_ENTER(unit);

    bcmi_lt_entry_init(&entry, fields);
    entry.nfields = COUNTOF(fields);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, MON_INBAND_TELEMETRY_CONTROLs,
                           &entry, NULL, NULL));

    /*
     * Check if the export profile ID is the default value, i.e. not
     * configured.
     */
    *export_profile_id = field_val_get(&entry, MON_EXPORT_PROFILE_IDs, 0);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_def_get(unit, MON_INBAND_TELEMETRY_CONTROLs,
                               MON_EXPORT_PROFILE_IDs, &fld_def));
    if (*export_profile_id == (int) fld_def.def) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /*
     * Get the collector ID. No need to check if is default value or not, since
     * we have already checked it for export profile ID. Export profile ID and
     * collector ID are configured using the same API
     * (bcm_ifa_collector_attach()), so it is not possible for one to be
     * configured and not the other.
     */
    coll_type = field_sym_get(&entry, COLLECTOR_TYPEs, 0);
    if (sal_strcmp(coll_type, "IPV4") == 0) {
        type = LTSW_COLLECTOR_TYPE_IPV4_UDP;
        collector_int_id = field_val_get(&entry, MON_COLLECTOR_IPV4_IDs, 0);
    } else {
        type = LTSW_COLLECTOR_TYPE_IPV6_UDP;
        collector_int_id = field_val_get(&entry, MON_COLLECTOR_IPV6_IDs, 0);
    }

    /* Convert to external ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_collector_external_id_get(unit, collector_int_id, type,
                                             collector_id));

 exit:
    SHR_FUNC_EXIT();
}

/* Check if there is a collector attached. */
static int
collector_attach_check(int unit, bool *attach)
{
    bcm_collector_t collector_id;
    int export_profile_id;
    int rv;

    SHR_FUNC_ENTER(unit);

    rv = collector_attach_get(unit, &collector_id, &export_profile_id);
    if (rv == SHR_E_NONE) {
        *attach = true;
    } else if (rv == SHR_E_NOT_FOUND) {
        *attach = false;
    } else {
        SHR_ERR_EXIT(rv);
    }

 exit:
    SHR_FUNC_EXIT();
}
/******************************************************************************
 * Public Functions
 */

/*!
 * \brief Set the IFA EApp config info.
 *
 * Set the IFA EApp config info.
 *
 * \param [in] unit Unit number.
 * \param [in] cfg_info Configuration info.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_ifa_uc_config_info_set(int unit, bcm_ifa_config_info_t *cfg_info)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        /* 0 */ {DEVICE_IDENTIFIERs,        0, 0, {0}},
        /* 1 */ {RX_PKT_EXPORT_MAX_LENGTHs, 0, 0, {0}},
    };
    bcmi_lt_field_t *fields_old = NULL, *fields_new = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    /* Check if the app is running. */
    if (ifa_uc[unit].run == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    /* Allocate memories for set and get operation. */
    SHR_ALLOC(fields_old, sizeof(fields), "bcmIfaConfigInfoFields");
    SHR_ALLOC(fields_new, sizeof(fields), "bcmIfaConfigInfoFields");
    sal_memcpy(fields_old, fields, sizeof(fields));
    sal_memcpy(fields_new, fields, sizeof(fields));

    /* Get the current value of the field. */
    bcmi_lt_entry_init(&entry, fields_old);
    entry.nfields = COUNTOF(fields);
    fields_flag_set(&entry, BCMI_LT_FIELD_F_GET);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, MON_INBAND_TELEMETRY_CONTROLs,
                           &entry, NULL, NULL));

    /* Set the updated value in the LT. */
    bcmi_lt_entry_init(&entry, fields_new);
    entry.nfields = COUNTOF(fields);

    field_val_set(&entry, DEVICE_IDENTIFIERs, 0, cfg_info->device_id);
    field_val_set(&entry, RX_PKT_EXPORT_MAX_LENGTHs, 0,
                  cfg_info->rx_packet_export_max_length);
    fields_flag_set(&entry, BCMI_LT_FIELD_F_SET);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, MON_INBAND_TELEMETRY_CONTROLs,
                           &entry, NULL));

    /* Check the oper state to see if the commit has succeeded. */
    rv = oper_state_check(unit, MON_INBAND_TELEMETRY_CONTROLs);
    if (rv != SHR_E_NONE) {
        /* Operation failed, rollback the entry. */
        bcmi_lt_entry_init(&entry, fields_old);
        entry.nfields = COUNTOF(fields);
        fields_flag_set(&entry, BCMI_LT_FIELD_F_SET);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_set(unit, MON_INBAND_TELEMETRY_CONTROLs,
                               &entry, NULL));
        SHR_ERR_EXIT(rv);
    }

 exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief Get the IFA EApp config info.
 *
 * Get the IFA EApp config info.
 *
 * \param [in] unit Unit number.
 * \param [out] cfg_info Configuration info.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_ifa_uc_config_info_get(int unit, bcm_ifa_config_info_t *cfg_info)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        /* 0 */ {DEVICE_IDENTIFIERs,        BCMI_LT_FIELD_F_GET, 0, {0}},
        /* 1 */ {RX_PKT_EXPORT_MAX_LENGTHs, BCMI_LT_FIELD_F_GET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    /* Check if the app is running. */
    if (ifa_uc[unit].run == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    bcm_ifa_config_info_t_init(cfg_info);

    /* Setup the lt entry. */
    bcmi_lt_entry_init(&entry, fields);
    entry.nfields = COUNTOF(fields);

    /* Get the current value of the field. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, MON_INBAND_TELEMETRY_CONTROLs,
                           &entry, NULL, NULL));
    cfg_info->device_id = field_val_get(&entry, DEVICE_IDENTIFIERs, 0);
    cfg_info->rx_packet_export_max_length = field_val_get(&entry,
                                                          RX_PKT_EXPORT_MAX_LENGTHs, 0);

 exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief Set the IFA EApp stats.
 *
 * Set the IFA EApp stats.
 *
 * \param [in] unit Unit number.
 * \param [in] stats Statistics.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_ifa_uc_stats_set(int unit, bcm_ifa_stat_info_t *stats)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        /* 0 */  {RX_PKT_CNTs,                           BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 1 */  {TX_PKT_CNTs,                           BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 2 */  {TX_RECORD_CNTs,                        BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 3 */  {RX_PKT_NO_EXPORT_CONFIG_DISCARDs,      BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 4 */  {RX_PKT_NO_IPFIX_CONFIG_DISCARDs,       BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 5 */  {RX_PKT_CURRENT_LENGTH_EXCEED_DISCARDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 6 */  {RX_PKT_LENGTH_EXCEED_MAX_DISCARDs,     BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 7 */  {RX_PKT_PARSE_ERROR_DISCARDs,           BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 8 */  {RX_PKT_UNKNOWN_NAMESPACE_DISCARDs,     BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 9 */  {RX_PKT_EXCESS_RATE_DISCARDs,           BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 10 */ {RX_PKT_INCOMPLETE_METADATA_DISCARDs,   BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 11 */ {TX_PKT_FAILURE_CNTs,                   BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    /* Check if the app is running. */
    if (ifa_uc[unit].run == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }
    SHR_NULL_CHECK(stats, SHR_E_PARAM);


    /* Populate the stats. */
    bcmi_lt_entry_init(&entry, fields);
    entry.nfields = COUNTOF(fields);

    field_val_set(&entry, RX_PKT_CNTs, 0, stats->rx_pkt_cnt);
    field_val_set(&entry, TX_PKT_CNTs, 0, stats->tx_pkt_cnt);
    field_val_set(&entry, TX_RECORD_CNTs, 0, stats->tx_record_count);
    field_val_set(&entry, RX_PKT_NO_EXPORT_CONFIG_DISCARDs, 0,
                  stats->ifa_collector_not_present_drop);
    field_val_set(&entry, RX_PKT_NO_IPFIX_CONFIG_DISCARDs, 0,
                  stats->ifa_template_not_present_drop);
    field_val_set(&entry, RX_PKT_CURRENT_LENGTH_EXCEED_DISCARDs, 0,
                  stats->ifa_int_hdr_len_invalid_drop);
    field_val_set(&entry, RX_PKT_LENGTH_EXCEED_MAX_DISCARDs, 0,
                  stats->rx_pkt_length_exceed_max_drop_count);
    field_val_set(&entry, RX_PKT_PARSE_ERROR_DISCARDs, 0,
                  stats->rx_pkt_parse_error_drop_count);
    field_val_set(&entry, RX_PKT_UNKNOWN_NAMESPACE_DISCARDs, 0,
                  stats->rx_pkt_unknown_namespace_drop_count);
    field_val_set(&entry, RX_PKT_EXCESS_RATE_DISCARDs, 0,
                  stats->rx_pkt_excess_rate_drop_count);
    field_val_set(&entry, RX_PKT_INCOMPLETE_METADATA_DISCARDs, 0,
                  stats->ifa_incomplete_metadata_drop_count);
    field_val_set(&entry, TX_PKT_FAILURE_CNTs, 0,
                  stats->tx_pkt_failure_count);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, MON_INBAND_TELEMETRY_STATSs, &entry, NULL));

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the IFA EApp stats.
 *
 * Get the IFA EApp stats.
 *
 * \param [in] unit Unit number.
 * \param [out] stats Statistics.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_ifa_uc_stats_get(int unit, bcm_ifa_stat_info_t *stats)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        /* 0 */  {RX_PKT_CNTs,                           BCMI_LT_FIELD_F_GET, 0, {0}},
        /* 1 */  {TX_PKT_CNTs,                           BCMI_LT_FIELD_F_GET, 0, {0}},
        /* 2 */  {TX_RECORD_CNTs,                        BCMI_LT_FIELD_F_GET, 0, {0}},
        /* 3 */  {RX_PKT_NO_EXPORT_CONFIG_DISCARDs,      BCMI_LT_FIELD_F_GET, 0, {0}},
        /* 4 */  {RX_PKT_NO_IPFIX_CONFIG_DISCARDs,       BCMI_LT_FIELD_F_GET, 0, {0}},
        /* 5 */  {RX_PKT_CURRENT_LENGTH_EXCEED_DISCARDs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /* 6 */  {RX_PKT_LENGTH_EXCEED_MAX_DISCARDs,     BCMI_LT_FIELD_F_GET, 0, {0}},
        /* 7 */  {RX_PKT_PARSE_ERROR_DISCARDs,           BCMI_LT_FIELD_F_GET, 0, {0}},
        /* 8 */  {RX_PKT_UNKNOWN_NAMESPACE_DISCARDs,     BCMI_LT_FIELD_F_GET, 0, {0}},
        /* 9 */  {RX_PKT_EXCESS_RATE_DISCARDs,           BCMI_LT_FIELD_F_GET, 0, {0}},
        /* 10 */ {RX_PKT_INCOMPLETE_METADATA_DISCARDs,   BCMI_LT_FIELD_F_GET, 0, {0}},
        /* 11 */ {TX_PKT_FAILURE_CNTs,                   BCMI_LT_FIELD_F_GET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    /* Check if the app is running. */
    if (ifa_uc[unit].run == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }
    SHR_NULL_CHECK(stats, SHR_E_PARAM);

    /* Get the stats. */
    bcmi_lt_entry_init(&entry, fields);
    entry.nfields = COUNTOF(fields);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, MON_INBAND_TELEMETRY_STATSs,
                           &entry, NULL, NULL));


    /* Convert the stats to API form. */
    stats->rx_pkt_cnt = field_val_get(&entry, RX_PKT_CNTs, 0);
    stats->tx_pkt_cnt = field_val_get(&entry, TX_PKT_CNTs, 0);
    stats->tx_record_count = field_val_get(&entry, TX_RECORD_CNTs, 0);
    stats->ifa_collector_not_present_drop = field_val_get(&entry,
                                                          RX_PKT_NO_EXPORT_CONFIG_DISCARDs, 0);
    stats->ifa_template_not_present_drop = field_val_get(&entry,
                                                         RX_PKT_NO_IPFIX_CONFIG_DISCARDs, 0);
    stats->ifa_int_hdr_len_invalid_drop = field_val_get(&entry,
                                                        RX_PKT_CURRENT_LENGTH_EXCEED_DISCARDs, 0);
    stats->rx_pkt_length_exceed_max_drop_count = field_val_get(&entry,
                                                               RX_PKT_LENGTH_EXCEED_MAX_DISCARDs, 0);
    stats->rx_pkt_parse_error_drop_count = field_val_get(&entry,
                                                         RX_PKT_PARSE_ERROR_DISCARDs, 0);
    stats->rx_pkt_unknown_namespace_drop_count = field_val_get(&entry,
                                                               RX_PKT_UNKNOWN_NAMESPACE_DISCARDs, 0);
    stats->rx_pkt_excess_rate_drop_count = field_val_get(&entry,
                                                         RX_PKT_EXCESS_RATE_DISCARDs, 0);
    stats->ifa_incomplete_metadata_drop_count = field_val_get(&entry,
                                                              RX_PKT_INCOMPLETE_METADATA_DISCARDs, 0);
    stats->tx_pkt_failure_count = field_val_get(&entry,
                                                TX_PKT_FAILURE_CNTs, 0);

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create the IFA IPFIX template.
 *
 * Create the IFA IPFIX template.
 *
 * \param [in] unit Unit number.
 * \param [in] options Options.
 * \param [inout] Id Template Id.
 * \param [in] set_id IPFIX Set ID.
 * \param [in] num_export_elements Number of export elements.
 * \param [in] export_elements List of export elements.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_ifa_uc_template_create(int unit, uint32 options,
                                 bcm_ifa_export_template_t *id, uint16_t set_id,
                                 int num_export_elements,
                                 bcm_ifa_export_element_info_t *export_elements)
{
    int i;
    bcmi_lt_entry_t entry;
    bool delete = false;
    bcm_ifa_export_element_type_t fixed_tmpl[] = {
        bcmIfaExportElementTypeIFAHeaders,
        bcmIfaExportElementTypeMetadataStack,
        bcmIfaExportElementTypePktData
    };
    bcmi_lt_field_t fields[] = {
        /* 0 */  {SET_IDs,                         0,                         0, {0}},
        /* 1 */  {NUM_EXPORT_ELEMENTSs,            0,                         0, {0}},
        /* 2 */  {EXPORT_ELEMENTs,                 BCMI_LT_FIELD_F_ARRAY     |
                                                   BCMI_LT_FIELD_F_SYMBOL    |
                                                   BCMI_LT_FIELD_F_ELE_VALID, 0, {0}},
        /* 3 */  {EXPORT_ELEMENTs,                 BCMI_LT_FIELD_F_ARRAY     |
                                                   BCMI_LT_FIELD_F_SYMBOL    |
                                                   BCMI_LT_FIELD_F_ELE_VALID, 1, {0}},
        /* 4 */  {EXPORT_ELEMENTs,                 BCMI_LT_FIELD_F_ARRAY     |
                                                   BCMI_LT_FIELD_F_SYMBOL    |
                                                   BCMI_LT_FIELD_F_ELE_VALID, 2, {0}},
        /* 5 */  {ENTERPRISEs,                     BCMI_LT_FIELD_F_ARRAY     |
                                                   BCMI_LT_FIELD_F_ELE_VALID, 0, {0}},
        /* 6 */  {ENTERPRISEs,                     BCMI_LT_FIELD_F_ARRAY     |
                                                   BCMI_LT_FIELD_F_ELE_VALID, 1, {0}},
        /* 7 */  {ENTERPRISEs,                     BCMI_LT_FIELD_F_ARRAY     |
                                                   BCMI_LT_FIELD_F_ELE_VALID, 2, {0}},
        /* 8 */  {INFORMATION_ELEMENT_IDENTIFIERs, BCMI_LT_FIELD_F_ARRAY     |
                                                   BCMI_LT_FIELD_F_ELE_VALID, 0, {0}},
        /* 9 */  {INFORMATION_ELEMENT_IDENTIFIERs, BCMI_LT_FIELD_F_ARRAY     |
                                                   BCMI_LT_FIELD_F_ELE_VALID, 1, {0}},
        /* 10 */ {INFORMATION_ELEMENT_IDENTIFIERs, BCMI_LT_FIELD_F_ARRAY     |
                                                   BCMI_LT_FIELD_F_ELE_VALID, 2, {0}},
    };
    bcmi_lt_field_t *fields_old = NULL, *fields_new = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    /* Validations. */
    if (ifa_uc[unit].run == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }
    SHR_NULL_CHECK(export_elements, SHR_E_PARAM);

    /* Only a single fixed template is supported. */
    if (num_export_elements != (sizeof(fixed_tmpl) / sizeof(fixed_tmpl[0]))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    for (i = 0; i < num_export_elements; i++) {
        if (export_elements[i].element != fixed_tmpl[i]) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }
    if (options & BCM_IFA_EXPORT_TEMPLATE_WITH_ID) {
        if (*id != IFA_TEMPLATE_ID) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }
    *id = IFA_TEMPLATE_ID;

    /* Allocate memories for set and get operation. */
    SHR_ALLOC(fields_old, sizeof(fields), "bcmIfaTemplateFields");
    SHR_ALLOC(fields_new, sizeof(fields), "bcmIfaTemplateFields");
    sal_memcpy(fields_old, fields, sizeof(fields));
    sal_memcpy(fields_new, fields, sizeof(fields));

    /* Try to get the current state of the entry. */
    bcmi_lt_entry_init(&entry, fields_old);
    entry.nfields = COUNTOF(fields);
    fields_flag_set(&entry, BCMI_LT_FIELD_F_GET);
    rv = bcmi_lt_entry_get(unit, MON_INBAND_TELEMETRY_IPFIX_EXPORTs,
                           &entry, NULL, NULL);
    if (rv != SHR_E_NONE) {
        /* Cannot find the old entry, set rollback strategy to delete. */
        delete = true;
    }

    /* Update the new entry. */
    bcmi_lt_entry_init(&entry, fields_new);
    entry.nfields = COUNTOF(fields);
    fields_flag_set(&entry, BCMI_LT_FIELD_F_SET);
    field_val_set(&entry, SET_IDs, 0, set_id);
    field_val_set(&entry, NUM_EXPORT_ELEMENTSs, 0, num_export_elements);
    for (i = 0; i < num_export_elements; i++) {
        const char *sym = tmpl_sym_get(export_elements[i].element);

        field_sym_set(&entry, EXPORT_ELEMENTs, i, sym);
        if (export_elements[i].flags & BCM_IFA_EXPORT_ELEMENT_FLAGS_ENTERPRISE) {
            field_val_set(&entry, ENTERPRISEs, i, 1);
        } else {
            field_val_set(&entry, ENTERPRISEs, i, 0);
        }
        field_val_set(&entry, INFORMATION_ELEMENT_IDENTIFIERs, i,
                      export_elements[i].info_elem_id);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, MON_INBAND_TELEMETRY_IPFIX_EXPORTs,
                           &entry, NULL));

    /* Check the oper state to see if the commit has succeeded. */
    rv = oper_state_check(unit, MON_INBAND_TELEMETRY_IPFIX_EXPORTs);
    if (rv != SHR_E_NONE) {
        if (delete == true) {
            /* Delete the entry. */
            bcmi_lt_entry_init(&entry, NULL);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_delete(unit, MON_INBAND_TELEMETRY_IPFIX_EXPORTs,
                                      &entry, NULL));
        } else {
            /* Overwrite with old entry. */
            bcmi_lt_entry_init(&entry, fields_old);
            entry.nfields = COUNTOF(fields);
            fields_flag_set(&entry, BCMI_LT_FIELD_F_SET);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_set(unit, MON_INBAND_TELEMETRY_IPFIX_EXPORTs,
                                   &entry, NULL));
        }
        SHR_ERR_EXIT(rv);
    }

 exit:
    SHR_FREE(fields_old);
    SHR_FREE(fields_new);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the IFA IPFIX template.
 *
 * Get the IFA IPFIX template.
 *
 * \param [in] unit Unit number.
 * \param [in] Id Template Id.
 * \param [out] set_id IPFIX Set ID.
 * \param [in] max_size Size of the export_elements array.
 * \param [out] export_elements List of export elements.
 * \param [out] num_export_elements Number of export elements.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_ifa_uc_template_get(int unit, bcm_ifa_export_template_t id,
                              uint16_t *set_id, int max_export_elements,
                              bcm_ifa_export_element_info_t *export_elements,
                              int *num_export_elements)
{
    int i;
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        /* 0 */  {SET_IDs,                         BCMI_LT_FIELD_F_GET,       0, {0}},
        /* 1 */  {NUM_EXPORT_ELEMENTSs,            BCMI_LT_FIELD_F_GET,       0, {0}},
        /* 2 */  {EXPORT_ELEMENTs,                 BCMI_LT_FIELD_F_GET       |
                                                   BCMI_LT_FIELD_F_ARRAY     |
                                                   BCMI_LT_FIELD_F_SYMBOL    |
                                                   BCMI_LT_FIELD_F_ELE_VALID, 0, {0}},
        /* 3 */  {EXPORT_ELEMENTs,                 BCMI_LT_FIELD_F_GET       |
                                                   BCMI_LT_FIELD_F_ARRAY     |
                                                   BCMI_LT_FIELD_F_SYMBOL    |
                                                   BCMI_LT_FIELD_F_ELE_VALID, 1, {0}},
        /* 4 */  {EXPORT_ELEMENTs,                 BCMI_LT_FIELD_F_GET       |
                                                   BCMI_LT_FIELD_F_ARRAY     |
                                                   BCMI_LT_FIELD_F_SYMBOL    |
                                                   BCMI_LT_FIELD_F_ELE_VALID, 2, {0}},
        /* 5 */  {ENTERPRISEs,                     BCMI_LT_FIELD_F_GET       |
                                                   BCMI_LT_FIELD_F_ARRAY     |
                                                   BCMI_LT_FIELD_F_ELE_VALID, 0, {0}},
        /* 6 */  {ENTERPRISEs,                     BCMI_LT_FIELD_F_GET       |
                                                   BCMI_LT_FIELD_F_ARRAY     |
                                                   BCMI_LT_FIELD_F_ELE_VALID, 1, {0}},
        /* 7 */  {ENTERPRISEs,                     BCMI_LT_FIELD_F_GET       |
                                                   BCMI_LT_FIELD_F_ARRAY     |
                                                   BCMI_LT_FIELD_F_ELE_VALID, 2, {0}},
        /* 8 */  {INFORMATION_ELEMENT_IDENTIFIERs, BCMI_LT_FIELD_F_GET       |
                                                   BCMI_LT_FIELD_F_ARRAY     |
                                                   BCMI_LT_FIELD_F_ELE_VALID, 0, {0}},
        /* 9 */  {INFORMATION_ELEMENT_IDENTIFIERs, BCMI_LT_FIELD_F_GET       |
                                                   BCMI_LT_FIELD_F_ARRAY     |
                                                   BCMI_LT_FIELD_F_ELE_VALID, 1, {0}},
        /* 10 */ {INFORMATION_ELEMENT_IDENTIFIERs, BCMI_LT_FIELD_F_GET       |
                                                   BCMI_LT_FIELD_F_ARRAY     |
                                                   BCMI_LT_FIELD_F_ELE_VALID, 2, {0}},
    };

    SHR_FUNC_ENTER(unit);

    /* Validations. */
    if (ifa_uc[unit].run == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }
    SHR_NULL_CHECK(set_id, SHR_E_PARAM);
    SHR_NULL_CHECK(num_export_elements, SHR_E_PARAM);

    if (id != IFA_TEMPLATE_ID) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Get the template. */
    bcmi_lt_entry_init(&entry, fields);
    entry.nfields = COUNTOF(fields);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, MON_INBAND_TELEMETRY_IPFIX_EXPORTs,
                           &entry, NULL, NULL));


    *set_id = field_val_get(&entry, SET_IDs, 0);
    *num_export_elements = field_val_get(&entry, NUM_EXPORT_ELEMENTSs, 0);
    if (max_export_elements == 0) {
        /* Special case, user just wants the number of export elements. */
        SHR_EXIT();
    }

    SHR_NULL_CHECK(export_elements, SHR_E_PARAM);
    if (*num_export_elements > max_export_elements) {
        *num_export_elements = max_export_elements;
    }

    for (i = 0; i < *num_export_elements; i++) {
        const char *sym;
        uint64_t u64;

        sal_memset(&(export_elements[i]), 0, sizeof(export_elements[i]));

        sym = field_sym_get(&entry, EXPORT_ELEMENTs, i);
        export_elements[i].element = tmpl_get(sym);

        u64 = field_val_get(&entry, ENTERPRISEs, i);
        if (u64) {
            export_elements[i].flags |= BCM_IFA_EXPORT_ELEMENT_FLAGS_ENTERPRISE;
        }

        export_elements[i].info_elem_id = field_val_get(&entry,
                                                        INFORMATION_ELEMENT_IDENTIFIERs, i);
    }

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy the IFA IPFIX template.
 *
 * Destroy the IFA IPFIX template.
 *
 * \param [in] unit Unit number.
 * \param [in] Id Template Id.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_ifa_uc_template_destroy(int unit, bcm_ifa_export_template_t id)
{
    bcmi_lt_entry_t entry;
    bool coll_attach;

    SHR_FUNC_ENTER(unit);

    /* Validations. */
    if (ifa_uc[unit].run == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT(template_id_check(unit, id));

    /*
     * Prevent the deletion of template if the collector/export profile is still
     * attached. This is done because the API bcm_ifa_collector_detach() takes
     * template ID also as an IN parameter.
     */
    SHR_IF_ERR_VERBOSE_EXIT(collector_attach_check(unit, &coll_attach));
    if (coll_attach == true) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    bcmi_lt_entry_init(&entry, NULL);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_delete(unit, MON_INBAND_TELEMETRY_IPFIX_EXPORTs,
                              &entry, NULL));

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach the IFA collector.
 *
 * Attach the IFA IPFIX collector.
 *
 * \param [in] unit Unit number.
 * \param [in] collector_id Collector ID
 * \param [in] export_profile_id Export profile ID
 * \param [in] template_id Template Id.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_ifa_uc_collector_attach(int unit,
                                  bcm_collector_t collector_id,
                                  int export_profile_id,
                                  bcm_ifa_export_template_t template_id)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        /* 0 */  {COLLECTOR_TYPEs,         BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /* 1 */  {MON_COLLECTOR_IPV4_IDs,  0,                      0, {0}},
        /* 2 */  {MON_COLLECTOR_IPV6_IDs,  0,                      0, {0}},
        /* 3 */  {MON_EXPORT_PROFILE_IDs,  0,                      0, {0}},
    };
    bcmi_lt_field_t *fields_old = NULL, *fields_new = NULL;
    bool coll_attach;
    bcm_ltsw_collector_transport_type_t coll_type;
    uint32_t collector_int_id;
    int rv;

    SHR_FUNC_ENTER(unit);

    /* Validations. */
    if (ifa_uc[unit].run == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    /* Check if the collector is being used by anyone else. */
    if (bcmi_ltsw_collector_check_user_is_other(unit, collector_id,
                                                LTSW_COLLECTOR_EXPORT_APP_IFA_IPFIX) != 0) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Check if the template exists. */
    SHR_IF_ERR_VERBOSE_EXIT(template_id_check(unit, template_id));

    /* Check if there is a collector already attached. */
    SHR_IF_ERR_VERBOSE_EXIT(collector_attach_check(unit, &coll_attach));
    if (coll_attach == true) {
        SHR_ERR_EXIT(SHR_E_EXISTS);
    }

    /* Get the collector internal ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_collector_internal_id_get(unit, collector_id,
                                             &collector_int_id, &coll_type));

    /* Allocate memories for set and get operation. */
    SHR_ALLOC(fields_old, sizeof(fields), "bcmIfaCollectorFields");
    SHR_ALLOC(fields_new, sizeof(fields), "bcmIfaCollectorFields");
    sal_memcpy(fields_old, fields, sizeof(fields));
    sal_memcpy(fields_new, fields, sizeof(fields));

    /* Get the current state of the entry. */
    bcmi_lt_entry_init(&entry, fields_old);
    entry.nfields = COUNTOF(fields);
    fields_flag_set(&entry, BCMI_LT_FIELD_F_GET);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, MON_INBAND_TELEMETRY_CONTROLs,
                           &entry, NULL, NULL));


    /* Update the entry. */
    bcmi_lt_entry_init(&entry, fields_new);
    entry.nfields = COUNTOF(fields);
    fields_flag_set(&entry, BCMI_LT_FIELD_F_SET);

    field_val_set(&entry, MON_EXPORT_PROFILE_IDs, 0, export_profile_id);
    if (coll_type == LTSW_COLLECTOR_TYPE_IPV4_UDP) {
        field_sym_set(&entry, COLLECTOR_TYPEs, 0, "IPV4");
        field_val_set(&entry, MON_COLLECTOR_IPV4_IDs, 0, collector_int_id);
    } else if (coll_type == LTSW_COLLECTOR_TYPE_IPV6_UDP) {
        field_sym_set(&entry, COLLECTOR_TYPEs, 0, "IPV6");
        field_val_set(&entry, MON_COLLECTOR_IPV6_IDs, 0, collector_int_id);
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, MON_INBAND_TELEMETRY_CONTROLs,
                           &entry, NULL));

    /* Check if the update succeeded. */
    rv = oper_state_check(unit, MON_INBAND_TELEMETRY_CONTROLs);
    if (rv != SHR_E_NONE) {
        /* Roll back the entry. */
        bcmi_lt_entry_init(&entry, fields_old);
        entry.nfields = COUNTOF(fields);
        fields_flag_set(&entry, BCMI_LT_FIELD_F_SET);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_set(unit, MON_INBAND_TELEMETRY_CONTROLs,
                               &entry, NULL));
        SHR_ERR_EXIT(rv);
    }

    /* Update the reference counts and the user. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_collector_ref_count_update(unit, collector_id, 1));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_collector_export_profile_ref_count_update(unit,
                                                             export_profile_id, 1));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_collector_user_update(unit, collector_id,
                                         LTSW_COLLECTOR_EXPORT_APP_IFA_IPFIX));


 exit:
    SHR_FREE(fields_old);
    SHR_FREE(fields_new);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the IFA collector details.
 *
 * Get the attached IFA collector, export profile and IPFIX template.
 *
 * \param [in] unit Unit number.
 * \param [out] collector_id Collector ID
 * \param [out] export_profile_id Export profile ID
 * \param [out] template_id Template Id.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_ifa_uc_collector_attach_get(int unit,
                                      bcm_collector_t *collector_id,
                                      int *export_profile_id,
                                      bcm_ifa_export_template_t *template_id)
{
    SHR_FUNC_ENTER(unit);

    /* Validations. */
    if (ifa_uc[unit].run == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }
    SHR_NULL_CHECK(collector_id, SHR_E_PARAM);
    SHR_NULL_CHECK(export_profile_id, SHR_E_PARAM);
    SHR_NULL_CHECK(template_id, SHR_E_PARAM);


    SHR_IF_ERR_VERBOSE_EXIT
        (collector_attach_get(unit, collector_id, export_profile_id));

    /* Check if the template is created. */
    *template_id = IFA_TEMPLATE_ID;
    SHR_IF_ERR_VERBOSE_EXIT(template_id_check(unit, *template_id));

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach the IFA collector.
 *
 * Detach the IFA collector.
 *
 * \param [in] unit Unit number.
 * \param [out] collector_id Collector ID
 * \param [out] export_profile_id Export profile ID
 * \param [out] template_id Template Id.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_ifa_uc_collector_detach(int unit,
                                  bcm_collector_t collector_id,
                                  int export_profile_id,
                                  bcm_ifa_export_template_t template_id)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        /* 0 */  {COLLECTOR_TYPEs,         BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /* 1 */  {MON_COLLECTOR_IPV4_IDs,  0,                      0, {0}},
        /* 2 */  {MON_COLLECTOR_IPV6_IDs,  0,                      0, {0}},
        /* 3 */  {MON_EXPORT_PROFILE_IDs,  0,                      0, {0}},
    };
    bcmi_lt_field_t *fields_old = NULL, *fields_new = NULL;
    bcmlt_field_def_t fld_def;
    bcm_collector_t coll_id;
    int ep_id;
    int rv;

    SHR_FUNC_ENTER(unit);

    /* Validations. */
    if (ifa_uc[unit].run == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    /*
     * Check if the configured collector and export profile ID matches with the
     * values passed.
     */
    SHR_IF_ERR_VERBOSE_EXIT(collector_attach_get(unit, &coll_id, &ep_id));
    if ((coll_id != collector_id) || (ep_id != export_profile_id)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Allocate memories for set and get operation. */
    SHR_ALLOC(fields_old, sizeof(fields), "bcmIfaCollectorFields");
    SHR_ALLOC(fields_new, sizeof(fields), "bcmIfaCollectorFields");
    sal_memcpy(fields_old, fields, sizeof(fields));
    sal_memcpy(fields_new, fields, sizeof(fields));

    /* Get the collector details. */
    bcmi_lt_entry_init(&entry, fields_old);
    entry.nfields = COUNTOF(fields);
    fields_flag_set(&entry, BCMI_LT_FIELD_F_GET);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, MON_INBAND_TELEMETRY_CONTROLs,
                           &entry, NULL, NULL));

    /* Check if the template is created. */
    SHR_IF_ERR_VERBOSE_EXIT(template_id_check(unit, template_id));

    /* Write default values for the collector fields. */
    bcmi_lt_entry_init(&entry, fields_old);
    entry.nfields = COUNTOF(fields);
    fields_flag_set(&entry, BCMI_LT_FIELD_F_SET);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_def_get(unit, MON_INBAND_TELEMETRY_CONTROLs,
                               COLLECTOR_TYPEs, &fld_def));
    field_sym_set(&entry, COLLECTOR_TYPEs, 0, fld_def.sym_def);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_def_get(unit, MON_INBAND_TELEMETRY_CONTROLs,
                               MON_COLLECTOR_IPV4_IDs, &fld_def));
    field_val_set(&entry, MON_COLLECTOR_IPV4_IDs, 0, fld_def.def);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_def_get(unit, MON_INBAND_TELEMETRY_CONTROLs,
                               MON_COLLECTOR_IPV6_IDs, &fld_def));
    field_val_set(&entry, MON_COLLECTOR_IPV6_IDs, 0, fld_def.def);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_def_get(unit, MON_INBAND_TELEMETRY_CONTROLs,
                               MON_EXPORT_PROFILE_IDs, &fld_def));
    field_val_set(&entry, MON_EXPORT_PROFILE_IDs, 0, fld_def.def);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, MON_INBAND_TELEMETRY_CONTROLs,
                           &entry, NULL));

    /* Check if the initialization succeeded. */
    rv = oper_state_check(unit, MON_INBAND_TELEMETRY_CONTROLs);
    if (rv != SHR_E_NONE) {
        /* Rollback the operation. */
        bcmi_lt_entry_init(&entry, fields_old);
        entry.nfields = COUNTOF(fields);
        fields_flag_set(&entry, BCMI_LT_FIELD_F_SET);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_set(unit, MON_INBAND_TELEMETRY_CONTROLs,
                               &entry, NULL));
        SHR_ERR_EXIT(rv);
    }

    /* Update the reference counts and the user. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_collector_ref_count_update(unit, collector_id, -1));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_collector_export_profile_ref_count_update(unit,
                                                             export_profile_id, -1));

 exit:
    SHR_FREE(fields_old);
    SHR_FREE(fields_new);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the template transmit configuration.
 *
 * Set the template transmit configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] template_id Template ID
 * \param [in] collector_id Collector ID
 * \param [in] config Template transmit config.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_ifa_uc_template_transmit_config_set(int unit,
                                              bcm_ifa_export_template_t template_id,
                                              bcm_collector_t collector_id,
                                              bcm_ifa_template_transmit_config_t *config)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        /* 0 */  {TX_INTERVALs,            0, 0, {0}},
        /* 1 */  {INITIAL_BURSTs,          0, 0, {0}},
    };
    bcmi_lt_field_t *fields_old = NULL, *fields_new = NULL;
    bcm_collector_t coll_id;
    int unused1;
    int rv;

    SHR_FUNC_ENTER(unit);

    /* Validations. */
    if (ifa_uc[unit].run == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    /* Allocate memories for set and get operation. */
    SHR_ALLOC(fields_old, sizeof(fields), "bcmIfaTemplateXmitFields");
    SHR_ALLOC(fields_new, sizeof(fields), "bcmIfaTemplateXmitFields");
    sal_memcpy(fields_old, fields, sizeof(fields));
    sal_memcpy(fields_new, fields, sizeof(fields));

    /* Check if the template and collector ID is configured. */
    SHR_IF_ERR_VERBOSE_EXIT(template_id_check(unit, template_id));
    SHR_IF_ERR_VERBOSE_EXIT(collector_attach_get(unit, &coll_id, &unused1));
    if (coll_id != collector_id) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Get the current value of the fields. */
    bcmi_lt_entry_init(&entry, fields_old);
    entry.nfields = COUNTOF(fields);
    fields_flag_set(&entry, BCMI_LT_FIELD_F_GET);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, MON_INBAND_TELEMETRY_IPFIX_EXPORTs,
                           &entry, NULL, NULL));

    /* Update the entry with the new values. */
    bcmi_lt_entry_init(&entry, fields_old);
    entry.nfields = COUNTOF(fields);

    fields_flag_set(&entry, BCMI_LT_FIELD_F_SET);
    field_val_set(&entry, TX_INTERVALs, 0, config->retransmit_interval_secs);
    field_val_set(&entry, INITIAL_BURSTs, 0, config->initial_burst);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, MON_INBAND_TELEMETRY_IPFIX_EXPORTs,
                           &entry, NULL));

    /* Check if the update succeeded. */
    rv = oper_state_check(unit, MON_INBAND_TELEMETRY_IPFIX_EXPORTs);
    if (rv != SHR_E_NONE) {
        /* Rollback the operation. */
        bcmi_lt_entry_init(&entry, fields_old);
        entry.nfields = COUNTOF(fields);
        fields_flag_set(&entry, BCMI_LT_FIELD_F_SET);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_set(unit, MON_INBAND_TELEMETRY_IPFIX_EXPORTs,
                               &entry, NULL));
        SHR_ERR_EXIT(rv);
    }

 exit:
    SHR_FREE(fields_old);
    SHR_FREE(fields_new);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the template transmit configuration.
 *
 * Get the template transmit configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] template_id Template ID
 * \param [in] collector_id Collector ID
 * \param [out] config Template transmit config.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_ifa_uc_template_transmit_config_get(int unit,
                                              bcm_ifa_export_template_t template_id,
                                              bcm_collector_t collector_id,
                                              bcm_ifa_template_transmit_config_t *config)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        /* 0 */  {TX_INTERVALs,            BCMI_LT_FIELD_F_GET, 0, {0}},
        /* 1 */  {INITIAL_BURSTs,          BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    int unused1;
    bcm_collector_t coll_id;

    SHR_FUNC_ENTER(unit);

    /* Validations. */
    if (ifa_uc[unit].run == false) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    /* Check if the template and collector ID is configured. */
    SHR_IF_ERR_VERBOSE_EXIT(template_id_check(unit, template_id));
    SHR_IF_ERR_VERBOSE_EXIT(collector_attach_get(unit, &coll_id, &unused1));
    if (coll_id != collector_id) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Get the current value of the fields. */
    bcmi_lt_entry_init(&entry, fields);
    entry.nfields = COUNTOF(fields);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, MON_INBAND_TELEMETRY_IPFIX_EXPORTs,
                           &entry, NULL, NULL));

    /* Populate the API. */
    bcm_ifa_template_transmit_config_t_init(config);
    config->retransmit_interval_secs = field_val_get(&entry, TX_INTERVALs, 0);
    config->initial_burst = field_val_get(&entry, INITIAL_BURSTs, 0);


 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach the IFA EApp.
 *
 * Detach the  IFA EApp related logical tables if it is already initalized.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_ifa_uc_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (bcmi_warmboot_get(unit) == true) {
        /* Nothing to do for warmboot. */
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT(bcmi_lt_clear(unit, MON_INBAND_TELEMETRY_CONTROLs));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, MON_INBAND_TELEMETRY_IPFIX_EXPORTs));

    sal_memset(&(ifa_uc[unit]), 0, sizeof(ifa_uc[unit]));

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the IFA EApp.
 *
 * Initialize IFA EApp related logical tables.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_ifa_uc_init(int unit)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        /* 0 */ {INBAND_TELEMETRYs,        BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 1 */ {EXPORTs,                  BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 2 */ {MAX_EXPORT_PKT_LENGTHs,   BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 3 */ {MAX_RX_PKT_LENGTHs,       BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    uint64_t u64;
    int rv;

    SHR_FUNC_ENTER(unit);

    sal_memset(&(ifa_uc[unit]), 0, sizeof(ifa_uc_t));

    if (bcmi_warmboot_get(unit) == true) {
        rv = oper_state_check(unit, MON_INBAND_TELEMETRY_CONTROLs);
        if (rv == SHR_E_NONE) {
            ifa_uc[unit].run = true;
        }
        SHR_EXIT();
    }

    /* Get the config properties and initialize the app. */
    bcmi_lt_entry_init(&entry, fields);
    entry.nfields = COUNTOF(fields);

    field_val_set(&entry, INBAND_TELEMETRYs, 0, 1);
    field_val_set(&entry, EXPORTs, 0, 1);

    u64 = bcmi_ltsw_property_get(unit, BCMI_CPN_IFA_MAX_EXPORT_PKT_LENGTH, 1500);
    field_val_set(&entry, MAX_EXPORT_PKT_LENGTHs, 0, u64);

    u64 = bcmi_ltsw_property_get(unit, BCMI_CPN_IFA_MAX_RX_PKT_LENGTH, 1500);
    field_val_set(&entry, MAX_RX_PKT_LENGTHs, 0, u64);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, MON_INBAND_TELEMETRY_CONTROLs,
                           &entry, NULL));

    /* Check if the initialization succeeded. */
    rv = oper_state_check(unit, MON_INBAND_TELEMETRY_CONTROLs);
    if (rv == SHR_E_NONE) {
        ifa_uc[unit].run = true;
    }

 exit:
    SHR_FUNC_EXIT();
}
