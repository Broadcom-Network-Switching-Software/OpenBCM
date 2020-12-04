/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        chipsim.h
 * Purpose:     
 */

#ifndef   _CHIPSIM_H_
#define   _CHIPSIM_H_



typedef struct event_s event_t;

typedef void (*event_handler_t)(event_t *ev);

struct event_s {
    sal_usecs_t		abs_time;
    event_handler_t	handler;
    event_t		*next;
    pcid_info_t         *pcid_info;
};

extern void pcid_counter_activity(event_t *ev);
extern void pcid_check_packet_input(event_t *ev);
extern void event_enqueue(pcid_info_t *pcid_info, event_handler_t handler,
                          sal_usecs_t abs_time);
extern void event_init(void);
extern event_t *event_peek(void);
extern void event_process_through(sal_usecs_t abs_time);




#endif /* _CHIPSIM_H_ */
