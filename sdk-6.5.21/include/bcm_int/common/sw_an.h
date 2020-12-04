/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef SW_AUTONEG_SUPPORT

 #ifndef __BCM_SW_AN_H__
 #define __BCM_SW_AN_H__

#include <phymod/phymod.h>
#include <bcm/types.h>

 typedef enum bcm_port_sw_an_event_e {
 	BCM_PORT_SW_AN_EVENT_NONE = 0,
 	BCM_PORT_SW_AN_EVENT_AN_START,
 	BCM_PORT_SW_AN_EVENT_AN_IN_PROCESS,
 	BCM_PORT_SW_AN_EVENT_AN_RESTART,
 	BCM_PORT_SW_AN_EVENT_AN_DOWN,
 	BCM_PORT_SW_AN_EVENT_AN_STOP,
 	BCM_PORT_SW_AN_EVENT_AN_LINK_DOWN,
 	BCM_PORT_SW_AN_EVENT_AN_MAX
 } bcm_port_sw_an_event_t;

 typedef enum bcm_sw_an_fsm_event_e {
    BCM_FSM_SW_AN_EVT_NONE = 0,
    BCM_FSM_SW_AN_EVT_WAIT, /* wait for event */
    BCM_FSM_SW_AN_EVT_START, /* SW state to kick off the state machine */
    BCM_FSM_SW_AN_EVT_SEQ_START, /* HW event to restart AN from base page exchange */
    BCM_FSM_SW_AN_EVT_LD_PAGE,  /* HW req to load the new page */
    BCM_FSM_SW_AN_EVT_LP_PAGE_RDY, /* HW event indicating LP page rcvd */
    BCM_FSM_SW_AN_EVT_PAGE_EX_COMPLETE,
    BCM_FSM_SW_AN_EVT_SEND_ACK,
    BCM_FSM_SW_AN_EVT_SEND_RESTART, /* Restart request from external events like link down */
    BCM_FSM_SW_AN_EVT_MAX
} bcm_sw_an_fsm_event_t;

#define SW_AN_MODE_CL73_MSA     0x3
#define SW_AN_MODE_MSA_ONLY     0x4

#define SW_AN_MSA_OUI_13to23 0x353
#define SW_AN_MSA_OUI_2to12  0x4DF
#define SW_AN_MSA_OUI_0to1   0x1

/* pause capability not related to PHY */
#define SW_AN_BASE0_PAGE_CAP_MASK   0x3
#define SW_AN_BASE0_PAGE_CAP_OFFSET 0xA

/* Next Page bit postion */
#define SW_AN_BASE0_PAGE_NP_MASK   0x1
#define SW_AN_BASE0_PAGE_NP_OFFSET 0xF

#define SW_AN_BASE_TECH_ABILITY_10GKR_MASK   0x1
#define SW_AN_BASE_TECH_ABILITY_10GKR_OFFSET 0x7

#define SW_AN_BASE_TECH_ABILITY_25GKRS_MASK   0x1
#define SW_AN_BASE_TECH_ABILITY_25GKRS_OFFSET 0xE

#define SW_AN_BASE_TECH_ABILITY_25GKR_MASK   0x1
#define SW_AN_BASE_TECH_ABILITY_25GKR_OFFSET 0xF

#define SW_AN_BASE_CL74_ABILITY_SUP_MASK   0x1
#define SW_AN_BASE_CL74_ABILITY_SUP_OFFSET 0xE

#define SW_AN_BASE_CL74_ABILITY_REQ_MASK   0x1
#define SW_AN_BASE_CL74_ABILITY_REQ_OFFSET 0xF

#define SW_AN_BASE_CL91_ABILITY_REQ_MASK   0x1
#define SW_AN_BASE_CL91_ABILITY_REQ_OFFSET 0xC

#define SW_AN_BASE_CL74_25GKRS_REQ_MASK   0x1
#define SW_AN_BASE_CL74_25GKRS_REQ_OFFSET 0xD

/* Message Page definitions */

#define SW_AN_MSG_PAGE0_MSG_CODE_MASK   0x3FF
#define SW_AN_MSG_PAGE0_MSG_CODE_OFFSET 0x0

#define SW_AN_MSG_PAGE1_OUI_13to23_MASK 0x7FF
#define SW_AN_MSG_PAGE1_OUI_13to23_OFFSET 0x0

#define SW_AN_MSG_PAGE2_OUI_2to12_MASK   0x7FF
#define SW_AN_MSG_PAGE2_OUI_2to12_OFFSET 0x0

/* Unformatted Page for MSA */
#define SW_AN_UF_PAGE0_OUI_MASK   0x3
#define SW_AN_UF_PAGE0_OUI_OFFSET 0x9

#define SW_AN_UF_PAGE1_25G_KR1_ABILITY_MASK   0x1
#define SW_AN_UF_PAGE1_25G_KR1_ABILITY_OFFSET 0x4

#define SW_AN_UF_PAGE1_25G_CR1_ABILITY_MASK   0x1
#define SW_AN_UF_PAGE1_25G_CR1_ABILITY_OFFSET 0x5

#define SW_AN_UF_PAGE1_50G_KR2_ABILITY_MASK   0x1
#define SW_AN_UF_PAGE1_50G_KR2_ABILITY_OFFSET 0x8

#define SW_AN_UF_PAGE1_50G_CR2_ABILITY_MASK   0x1
#define SW_AN_UF_PAGE1_50G_CR2_ABILITY_OFFSET 0x9

#define SW_AN_UF_PAGE2_CL91_SUPPORT_MASK   0x1
#define SW_AN_UF_PAGE2_CL91_SUPPORT_OFFSET 0x8

#define SW_AN_UF_PAGE2_CL74_SUPPORT_MASK   0x1
#define SW_AN_UF_PAGE2_CL74_SUPPORT_OFFSET 0x9

#define SW_AN_UF_PAGE2_CL91_REQ_MASK   0x1
#define SW_AN_UF_PAGE2_CL91_REQ_OFFSET 0xA

#define SW_AN_UF_PAGE2_CL74_REQ_MASK   0x1
#define SW_AN_UF_PAGE2_CL74_REQ_OFFSET 0xB

/* initialize the SW AN module */
extern int bcm_sw_an_module_init(int unit);

/* deInit the SW AN module */
extern int bcm_sw_an_module_deinit(int unit);

/* Start/Stop the SW AN thread for the unit */
extern int bcm_sw_an_enable_set(int unit, int enable); 

extern int bcm_sw_an_enable_get(int unit, int *enable);

/* add the port to SW AN */
extern int bcm_sw_an_port_register(int unit, bcm_port_t port);

/* remove the port from SW AN */
extern int bcm_sw_an_port_unregister(int unit, bcm_port_t port);

/* notify the SW AN thread of any AN event */
extern int bcm_sw_an_post_event(int unit, bcm_port_t port, bcm_port_sw_an_event_t event);

/* sw an advert set */
extern int bcm_sw_an_advert_set(int unit, bcm_port_t port, phymod_autoneg_ability_t *ability);

extern int bcm_sw_an_ability_advert_set(int unit, soc_port_t port, soc_port_ability_t *ability);

/* sw an advert/remote ability get */
extern int bcm_sw_an_ability_remote_get(int unit, soc_port_t port, soc_port_ability_t *portAbility);
extern int bcm_sw_an_ability_advert_get(int unit, soc_port_t port, soc_port_ability_t *portAbility);

/* debug function to dump an context for the port */
extern int bcm_sw_an_port_diag(int unit, int port);


#endif /* __BCM_SW_AN_H__ */
#endif /* SW_AUTONEG_SUPPORT */

