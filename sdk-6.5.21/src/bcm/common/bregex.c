/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       bregex.c
 * Purpose:    Regex common APIs
 */

#include <soc/drv.h>
#include <bcm/bregex.h>

#if defined(INCLUDE_REGEX)

void
bcm_regex_config_t_init(bcm_regex_config_t *config)
{
    if (NULL != config) {
        sal_memset(config, 0, sizeof(*config));
        config->max_flows = -1;
        config->payload_depth = 1500;
        config->inspect_num_pkt = 1;
        config->report_buffer_size = 1024;
    }

    return;
}

void
bcm_regex_engine_config_t_init(bcm_regex_engine_config_t *config)
{
    if (NULL != config) {
        sal_memset(config, 0, sizeof(bcm_regex_engine_config_t));
    }
    return;
}

void
bcm_regex_match_t_init(bcm_regex_match_t *match)
{
    if (NULL != match) {
        sal_memset(match, 0, sizeof(bcm_regex_match_t));
        match->payload_length_max = -1;
        match->payload_length_min = -1;
        match->policer_id = -1;
        match->requires = -1;
        match->provides = -1;
        match->sequence = -1;
        match->policy_id = -1;
    }
    return;
}


void
bcm_regex_report_t_init(bcm_regex_report_t *report)
{
    if (NULL != report) {
        sal_memset(report, 0, sizeof(bcm_regex_report_t));
    }
    return;
}

/* Initialize regex session key structure. */
void bcm_regex_session_key_t_init(
    bcm_regex_session_key_t *session_key)
{
    if (NULL != session_key) {
        sal_memset(session_key, 0, sizeof(*session_key));
    }
}

/* Initialize regex session structure. */
void bcm_regex_session_t_init(
    bcm_regex_session_t *session)
{
    if (NULL != session) {
        sal_memset(session, 0, sizeof(*session));
    }
}

#endif /* INCLUDE_REGEX */

