/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        board.h
 * Purpose:     Definitions for the board type.
 */

#ifndef   _SOC_BOARD_H_
#define   _SOC_BOARD_H_

#include <sal/types.h>

#include <soc/defs.h>
#include <soc/drv.h>

/*
 * SOC_BOARD_CXN_MAX:  This is the maximum connections allowed on
 * one board interconnecting units.  We count each connection twice
 * (to support simplex connections and make other things easier.)
 * This is based on Hercules w/ 8 ports.  Draco could conceivably
 * connect to 13 devices on one board......
 *
 * SOC_BOARD_DEV_MAX:  Max number of SOC units controlled by one
 * CPU.  
 */

#define SOC_BOARD_CXN_MAX      16
#define SOC_BOARD_DEV_MAX      0

/*
 * Typedef:  soc_board_cxn_t
 * Indicate an on board connection with the following data type:
 *    src_port:          Port on unit                       
 *    tx_unit, tx_port:  Where does TX of src_port go?      
 *    rx_unit, rx_port:  Where does RX of src_port come from? 
 *    reachable:         Which units on this board are
 *                       reachable thru tx.  This is a BITMAP
 *                       indexed by SOC unit number.
 */

typedef struct soc_board_cxn_s {
    int                  src_port; /* Port on unit */
    int                  tx_unit;  /* Where does TX of src_port go? */
    int                  tx_port;
    int                  rx_unit;  /* Where does RX of src_port come from? */
    int                  rx_port;
    uint32               reachable; /* Which units are reachable thru tx */
} soc_board_cxn_t;

/*
 * Typedef:  soc_unit_info_t
 * Describe a unit and its connections on a board.
 *    dev_id:           Id value from devids.h
 *    num_cxns:          How many stack connections to
 *                       other chips on this board.
 *    cxn:               List of connections.  See above.
 */
typedef struct soc_unit_static_s {
    int                  dev_id;
    soc_driver_t         *soc_driver;
    int                  max_port;   /* Max physical port number */
} soc_unit_static_t;

typedef struct soc_unit_info_s {
    soc_unit_static_t    *static_info;
    int                  num_cxns;
    soc_board_cxn_t      cxn[SOC_BOARD_CXN_MAX];
} soc_unit_info_t;

/*
 * Typedef:  soc_board_t
 *    Describes a board
 *    board_id:          Value from above enum
 *    num_units:         How many devices on this board
 *    unit_info:         Array of unit descripts from above.
 *    max_vlans:         How many VLANs does the board support
 *    max_stg:           How many Spanning Tree Groups does it spt
 *    max_trunks:        How many trunks does the board support
 *    max_ports_per_trunk:   How many ports can be trunked together
 */

typedef struct soc_board_s {
    soc_board_ids_t      board_id;
    int                  num_units;
    soc_unit_info_t      unit_info[SOC_BOARD_DEV_MAX];
    /* Some parameters related to capabilities */
    int                  max_vlans;
    int                  max_stg;
    int                  max_trunks;
    int                  max_ports_per_trunk;
    char                 *board_name;
} soc_board_t;

/* Variable:
 *     known_boards:   The list of descriptions of known boards
 */

extern soc_board_t soc_known_boards[SOC_BOARD_NUM_BOARDS];

#endif /* _SOC_BOARD_H_ */
