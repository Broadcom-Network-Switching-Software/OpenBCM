/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RX LOS
 */

#include <shared/bsl.h>
#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT))
#include <appl/dcmn/rx_los/rx_los_db.h>
#include <appl/dcmn/rx_los/rx_los.h>
#endif /* BCM_PETRA_SUPPORT || BCM_DFE_SUPPORT */
#include <sal/core/time.h>
#include <bcm/types.h>
#include <soc/types.h>
#include <bcm/error.h>
#include <sal/core/alloc.h>
#include <sal/appl/io.h>
#include <sal/core/libc.h>
#include <soc/maxdef.h>
#include <bcm/debug.h>
#include <soc/debug.h>

typedef struct rx_los_link_info_s {
    rx_los_state_t current_state;
    uint32 current_retries;
    sal_usecs_t timeStamp;
    sal_usecs_t link_down_cycle_start;
    uint32 link_down_cycle_count;
    sal_usecs_t rx_seq_change_cycle_start;
    uint32 rx_seq_change_cycle_count;
} rx_los_link_info_t;

typedef struct rx_los_unit_info_s{
    rx_los_link_info_t *links;
    pbmp_t supported_links;
    pbmp_t not_steady_state;
    rx_los_callback_t callback;
} rx_los_unit_info_t;

static char* rx_los_state_name[] =  {
    "rx_los_state_ideal_state",
    "rx_los_state_no_signal",
    "rx_los_state_no_signal_active", 
    "rx_los_state_rx_seq_change",
    "rx_los_state_sleep_one", 
    "rx_los_state_restart", 
    "rx_los_state_sleep_two", 
    "rx_los_state_link_up_check",
    "rx_los_state_long_sleep", 
    "ERROR state"
};

typedef struct rx_los_db_s
{
    rx_los_unit_info_t* rx_los_units[SOC_MAX_NUM_DEVICES];
    soc_pbmp_t rx_los_units_pbmp;
    uint32 rx_los_long_sleep;
    uint32 rx_los_short_sleep;
    uint32 rx_los_active_sleep;
    uint32 rx_los_allowed_retries;
    uint32 rx_los_priority;
    uint32 rx_los_link_down_cycle_time;
    uint32 rx_los_link_down_cycle_count;
    uint32 rx_los_rx_seq_change_cycle_time;
    uint32 rx_los_rx_seq_change_cycle_count;
} rx_los_db_t;


/* Database: */

static rx_los_db_t rx_los_db = {
    {0}, 
    {{0}},
    RX_LOS_LONG_SLEEP_DEFAULT, 
    RX_LOS_SHORT_SLEEP_DEFAULT, 
    0, 
    RX_LOS_ALLOWED_RETRIES_DEFAULT,
    RX_LOS_THREAD_PRIORITY_DEFAULT, 
    RX_LOS_LINK_DOWN_CYCLE_TIME_DEFAULT, 
    RX_LOS_LINK_DOWN_CYCLE_COUNT_DEFAULT, 
    RX_LOS_RX_SEQ_CHANGE_CYCLE_TIME_DEFAULT,
    RX_LOS_RX_SEQ_CHANGE_CYCLE_COUNT_DEFAULT
};

int rx_los_db_state_name_get(rx_los_state_t state, char **name)
{
    if ((state<0)||(state>rx_los_states_count)) {
        *name = rx_los_state_name[rx_los_states_count];
        return BCM_E_FAIL;
    }

    *name = rx_los_state_name[state];
    return  BCM_E_NONE;
}

int rx_los_db_set_config(uint32 short_sleep_usec, uint32 long_sleep_usec, uint32 allowed_retries, uint32 priority, int link_down_count_max, int link_down_cycle_time)
{
    rx_los_db.rx_los_long_sleep = long_sleep_usec;
    rx_los_db.rx_los_short_sleep = short_sleep_usec;
    rx_los_db.rx_los_allowed_retries = allowed_retries;
    rx_los_db.rx_los_priority = priority;
    rx_los_db.rx_los_link_down_cycle_count = link_down_count_max;
    rx_los_db.rx_los_link_down_cycle_time = link_down_cycle_time;

    return BCM_E_NONE;
}

int rx_los_db_set_active_sleep_config(uint32 active_sleep_usec)
{
    rx_los_db.rx_los_active_sleep = active_sleep_usec;
	
	return BCM_E_NONE;
}

uint32 rx_los_db_long_sleep_get(void)
{
    return rx_los_db.rx_los_long_sleep;
}

uint32 rx_los_db_short_sleep_get(void)
{
    return rx_los_db.rx_los_short_sleep;
}

uint32 rx_los_db_active_sleep_get(void)
{
    return rx_los_db.rx_los_active_sleep;
}

uint32 rx_los_db_allowed_retries_get(void)
{
    return rx_los_db.rx_los_allowed_retries;
}

uint32 rx_los_db_thread_priority_get(void)
{
    return rx_los_db.rx_los_priority;
}



/* the next functions require database to be locked when called */

int rx_los_db_units_pbmp_get(pbmp_t *pbmp)
{
    BCM_PBMP_ASSIGN(*pbmp, rx_los_db.rx_los_units_pbmp);

    return BCM_E_NONE;
}

int rx_los_db_move_link_to_support(int unit, int port)
{
    if (rx_los_db.rx_los_units[unit]==NULL) {
        return BCM_E_FAIL;
    }

    BCM_PBMP_PORT_ADD(rx_los_db.rx_los_units[unit]->supported_links, port);

    return BCM_E_NONE;
}

int rx_los_db_remove_link_from_support(int unit, int port)
{
    if (rx_los_db.rx_los_units[unit]==NULL) {
        return BCM_E_FAIL;
    }
    BCM_PBMP_PORT_REMOVE(rx_los_db.rx_los_units[unit]->supported_links, port);
    
    return BCM_E_NONE;
}

int rx_los_db_remove_link_from_steady(int unit, int port)
{
    if (rx_los_db.rx_los_units[unit] == NULL){
        return BCM_E_FAIL;
    }
    BCM_PBMP_PORT_ADD(rx_los_db.rx_los_units[unit]->not_steady_state,port);

    return BCM_E_NONE;
}

int rx_los_db_move_link_to_steady(int unit, int port)
{
    if (rx_los_db.rx_los_units[unit] == NULL){
        return BCM_E_FAIL;
    }
    BCM_PBMP_PORT_REMOVE(rx_los_db.rx_los_units[unit]->not_steady_state,port);

    return BCM_E_NONE;
}


/* the next functions require database to be locked when called,
   and require the function rx_los_db_unit_null_check to be called and checked before they are used */


int rx_los_db_not_steady_state_pbmp_get(int unit, pbmp_t *pbmp)
{
    BCM_PBMP_ASSIGN(*pbmp, rx_los_db.rx_los_units[unit]->not_steady_state);

    return BCM_E_NONE;
}

int rx_los_db_supported_pbmp_get(int unit, pbmp_t *pbmp)
{
    BCM_PBMP_ASSIGN(*pbmp, rx_los_db.rx_los_units[unit]->supported_links);

    return BCM_E_NONE;
}

rx_los_state_t rx_los_db_state_get(int unit, int port)
{
    rx_los_state_t state;
    state = rx_los_db.rx_los_units[unit]->links[port].current_state;

    return state;

}

int rx_los_db_state_set(int unit,int port,rx_los_state_t state)
{
    int rv;
    char *old_name = NULL, *new_name = NULL;

    rv = rx_los_db_state_name_get(rx_los_db.rx_los_units[unit]->links[port].current_state, &old_name);
    if(rv != BCM_E_NONE) {
        return rv;
    }
    rv = rx_los_db_state_name_get(state,&new_name);
    if(rv != BCM_E_NONE) {
        return rv;
    }
    LOG_DEBUG(BSL_LS_APPL_PORT,
              (BSL_META_U(unit,
                          "rx_los_db_state_set unit %d link %d pre_state: %s new_state: %s\n"),
                          unit, port, old_name, new_name));
    rx_los_db.rx_los_units[unit]->links[port].current_state = state;
    
    return rv;
}

sal_usecs_t rx_los_db_timestamp_get(int unit, int port)
{
    sal_usecs_t timeStamp;
    timeStamp = rx_los_db.rx_los_units[unit]->links[port].timeStamp;

     return timeStamp;
}

void rx_los_db_timestamp_update(int unit, int port)
{
    rx_los_db.rx_los_units[unit]->links[port].timeStamp = sal_time_usecs();
}

int rx_los_db_tries_get(int unit, int port)
{
    int rv;
    rv = rx_los_db.rx_los_units[unit]->links[port].current_retries;

    return rv;
}

void rx_los_db_tries_initialize(int unit, int port)
{
    rx_los_db.rx_los_units[unit]->links[port].current_retries = 0;

}

void rx_los_db_tries_increment(int unit, int port)
{
    rx_los_db.rx_los_units[unit]->links[port].current_retries++;
}

int rx_los_db_unit_null_check(int unit)
{
    if ((rx_los_db.rx_los_units[unit] == NULL)||(rx_los_db.rx_los_units[unit]->links == NULL)) {
        return BCM_E_NOT_FOUND;
    }
    return BCM_E_NONE;
}

/*first init of DB*/
void
rx_los_db_init(void) {

    BCM_PBMP_CLEAR(rx_los_db.rx_los_units_pbmp);
    sal_memset(rx_los_db.rx_los_units, 0, (sizeof(rx_los_unit_info_t*))*SOC_MAX_NUM_DEVICES);
}

/* to be called only when db is locked */
int
rx_los_db_unit_init(int unit, pbmp_t links_pbmp) {
    int i, rv = BCM_E_NONE;
    rx_los_unit_info_t *unit_info;

    if (rx_los_db.rx_los_units[unit]!= NULL) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_db_unit_init  - unit is not null in db\n")));
        return BCM_E_FAIL;
    }

    /* set unit info */
    unit_info = (rx_los_unit_info_t*)sal_alloc(sizeof(rx_los_unit_info_t),"rx_los_unit_t");
    if (unit_info == NULL) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "rx_los_db_init_unit failed to allocate unit_info\n")));
        return BCM_E_FAIL;
    }

    /*initilaize callback*/
    unit_info->callback = NULL;

    /* set links to start mode */
    unit_info->links = (rx_los_link_info_t*)sal_alloc(SOC_MAX_NUM_PORTS * sizeof(rx_los_link_info_t), "rx los links");
    if (unit_info->links == NULL) {
        sal_free(unit_info);
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "rx_los_db_init_unit failed to allocate links\n")));
        return BCM_E_FAIL;
    }

    for (i = 0; i < SOC_MAX_NUM_PORTS; i++) {
        unit_info->links[i].current_retries = 0;
        unit_info->links[i].current_state = rx_los_state_no_signal;
        unit_info->links[i].link_down_cycle_count = 0;
        unit_info->links[i].link_down_cycle_start = sal_time_usecs();
        unit_info->links[i].rx_seq_change_cycle_count = 0;
        unit_info->links[i].rx_seq_change_cycle_start = sal_time_usecs();
    }

    BCM_PBMP_ASSIGN(unit_info->supported_links, links_pbmp);
    BCM_PBMP_CLEAR(unit_info->not_steady_state);

    rx_los_db.rx_los_units[unit] = unit_info;
    BCM_PBMP_PORT_ADD(rx_los_db.rx_los_units_pbmp, unit);

    return rv;
}

/* to be called only when db is locked*/
int
rx_los_db_unit_deinit(int unit) {

    rx_los_unit_info_t *unit_info;

    unit_info = rx_los_db.rx_los_units[unit];
    rx_los_db.rx_los_units[unit] = NULL;
    BCM_PBMP_PORT_REMOVE(rx_los_db.rx_los_units_pbmp, unit);

    if (unit_info != NULL) {
        sal_free(unit_info->links);
        sal_free(unit_info);
    }
    return BCM_E_NONE;
}

int
rx_los_db_dump_port(int unit, bcm_port_t port) {
    int rv;
    char *state_name;

    rv = rx_los_db_unit_null_check(unit);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_db_dump - rx_los_db_unit_null_check FAILED\n")));
        return rv;
    }

    if (!BCM_PBMP_MEMBER(rx_los_db.rx_los_units[unit]->supported_links, port)) {
        return BCM_E_PORT;
    }

    rv = rx_los_db_state_name_get(rx_los_db.rx_los_units[unit]->links[port].current_state, &state_name);

    cli_out("Port %d: state %s, current retries %d, timestamp %u link_down_count %u rx_seq_change_count %u \n", port, 
            state_name,
            rx_los_db.rx_los_units[unit]->links[port].current_retries,
            rx_los_db.rx_los_units[unit]->links[port].timeStamp,
            rx_los_db.rx_los_units[unit]->links[port].link_down_cycle_count,
            rx_los_db.rx_los_units[unit]->links[port].rx_seq_change_cycle_count);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META_U(unit,
                              "ERROR: link %u - state number is %d\n"), port, rx_los_db.rx_los_units[unit]->links[port].current_state));
        return rv;
    }

    return BCM_E_NONE;
}

int
rx_los_db_dump(int unit) {

    char buf[1000];
    uint32 port;
    int rv;

    cli_out("RX LOS application dump.\n");
    cli_out("------------------------\n");

    rv = rx_los_db_unit_null_check(unit);
    if (BCM_FAILURE(rv)) {
        cli_out("rx_los_dump - Disabled\n");
        return rv;
    }


    cli_out("rx_los_allowed_retries = %d.\n", rx_los_db.rx_los_allowed_retries);
    cli_out("rx_los_long_sleep_usec = %d.\n", rx_los_db.rx_los_long_sleep);
    cli_out("rx_los_short_sleep_usec = %d.\n", rx_los_db.rx_los_short_sleep);
    cli_out("rx_los_thread_priority = %d.\n", rx_los_db.rx_los_priority);
    cli_out("Not Steady Links: %s\n", SOC_PBMP_FMT(rx_los_db.rx_los_units[unit]->not_steady_state, buf));
    cli_out("Supported: Links %s\n", SOC_PBMP_FMT(rx_los_db.rx_los_units[unit]->supported_links, buf));
    BCM_PBMP_ITER(rx_los_db.rx_los_units[unit]->supported_links, port) {
        rx_los_db_dump_port(unit, port);
    }

    return BCM_E_NONE;

}

/*
 * Link Down recurring
 */
int
rx_los_db_link_down_cycle_get(int unit, bcm_port_t port, sal_usecs_t *cycle_start, uint32 *cycle_count)
{
    *cycle_start = rx_los_db.rx_los_units[unit]->links[port].link_down_cycle_start;
    *cycle_count = rx_los_db.rx_los_units[unit]->links[port].link_down_cycle_count;

    return BCM_E_NONE;

}

int
rx_los_db_link_down_cycle_set(int unit, bcm_port_t port, sal_usecs_t cycle_start, uint32 cycle_count)
{
    rx_los_db.rx_los_units[unit]->links[port].link_down_cycle_start = cycle_start;
    rx_los_db.rx_los_units[unit]->links[port].link_down_cycle_count = cycle_count;

    return BCM_E_NONE;

}

int
rx_los_db_link_down_cycle_config_get(int unit, bcm_port_t port, sal_usecs_t *cycle_time, uint32 *cycle_max_count)
{
    *cycle_time = rx_los_db.rx_los_link_down_cycle_time;
    *cycle_max_count = rx_los_db.rx_los_link_down_cycle_count;

    return BCM_E_NONE;

}

int
rx_los_db_link_down_cycle_config_set(int unit, bcm_port_t port, sal_usecs_t cycle_time, uint32 cycle_max_count)
{
    rx_los_db.rx_los_link_down_cycle_time = cycle_time;
    rx_los_db.rx_los_link_down_cycle_count = cycle_max_count;

    return BCM_E_NONE;

}

/*
 * Rx Seq change recurring
 */
int
rx_los_db_rx_seq_change_cycle_get(int unit, bcm_port_t port, sal_usecs_t *cycle_start, uint32 *cycle_count)
{
    *cycle_start = rx_los_db.rx_los_units[unit]->links[port].rx_seq_change_cycle_start;
    *cycle_count = rx_los_db.rx_los_units[unit]->links[port].rx_seq_change_cycle_count;

    return BCM_E_NONE;

}

int
rx_los_db_rx_seq_change_cycle_set(int unit, bcm_port_t port, sal_usecs_t cycle_start, uint32 cycle_count)
{
    rx_los_db.rx_los_units[unit]->links[port].rx_seq_change_cycle_start = cycle_start;
    rx_los_db.rx_los_units[unit]->links[port].rx_seq_change_cycle_count = cycle_count;

    return BCM_E_NONE;

}

int
rx_los_db_rx_seq_change_cycle_config_get(int unit, bcm_port_t port, sal_usecs_t *cycle_time, uint32 *cycle_max_count)
{
    *cycle_time = rx_los_db.rx_los_rx_seq_change_cycle_time;
    *cycle_max_count = rx_los_db.rx_los_rx_seq_change_cycle_count;

    return BCM_E_NONE;

}

int
rx_los_db_rx_seq_change_cycle_config_set(int unit, bcm_port_t port, sal_usecs_t cycle_time, uint32 cycle_max_count)
{
    rx_los_db.rx_los_rx_seq_change_cycle_time = cycle_time;
    rx_los_db.rx_los_rx_seq_change_cycle_count = cycle_max_count;

    return BCM_E_NONE;

}

int
rx_los_db_register(int unit, rx_los_callback_t callback)
{
    rx_los_db.rx_los_units[unit]->callback = callback;

    return BCM_E_NONE;

}

int
rx_los_db_callback_get(int unit, rx_los_callback_t *callback)
{
    *callback = rx_los_db.rx_los_units[unit]->callback;

    return BCM_E_NONE;

}
