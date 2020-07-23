/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * System Snake
 *
 * This file runs a system-level snake test.  Packets are injected into
 * a set of closed loops running around the system and left to cycle for
 * a period of time.  At a certain interval, the counters are checked to
 * confirm that traffic continues to flow.  At the end of the test, the
 * packets are drained and verified against the transmitted packets.
 * The final counter values are displayed on success.
 *
 * Port speed used is default (set in rc.soc) unless overridden on cmd
 * line.  Full duplex is used for snake test.  Loopback modes are MAC
 * and PHY.  EXT mode uses cables (crossover or straight) between ports
 * 1&2, 3&4, 5&6, etc.  Exposed HiGig ports are connected externally.
 *
 * Details:
 *
 * Initialization includes placing all of the ports into the proper
 * forwarding and/or loopback state.  VLANs are configured to include
 * all ports.  On 5690, the L2 table is programmed to forward the
 * packets to the next destination.  On 5670, the UC table is used.  The
 * packets are drained with filters.  All packet I/O takes place on one
 * 5690 for simplicity.
 *
 * The implementation borrows significantly from the test/lb_util.c
 * library of loopback utilities.  The packet I/O and verification
 * routines, port interconnection functions, and counter monitoring
 * procedures are the most important pieces of recycling.
 *
 * The default paths implemented here are 12 closed loops corresponding
 * to a 5690 port number.  That is, for a given path, a packet travels
 * from port n on the first 5690 to port n on each succeeding 5690 (via
 * the Higig uplinks to 5670).  From the final 5690 in the system, the
 * packet travel to the 5670 and loops externally out the back panel
 * ports.  Thus, the back panel ports must be connected in pairs by
 * cable.  After traveling these external loops, the packet is returned
 * to the first 5690 unit and continues the cycle.  The path is broken
 * at the end of the test by means of a filter on the first 5690.
 *
 * The first section of this file contains a number of defines which
 * might be tweaked to alter the parameters of the test.  Changing the
 * number of paths will require reworking the path interconnection
 * declarations.  Also, the test currently expects all ports to be
 * involved.  Omitting some paths or ports will produce a test failure.
 *
 * Exceeding bandwidth rates will result in dropped packets and test
 * failure.  For the 12 paths defined below, the 10 packets used in each
 * path were experimentally determined.  They result in traffic near
 * line rate on the 5670; they also do not exceed the CPU's ability to
 * drain packets before the system drops packets.
 */

/*
 * This is the new implementation of the system snake test. It preserves
 * all the functionality of the original tests and uses the same
 * underlying functions to implement it.
 *
 * The new implementation goals were:
 *   1) to make it easy to add new platforms and snake configurations without 
 *      breaking the existing ones
 *   2) to overcome certain restrictions contained in the original code in
 *      order to allow for more complex system/snake configurations.
 *
 * The first goal is accomplished by replacing the complicated, platform-
 * dependent snake generation code with a simple generic function that
 * generates snakes according to specially constructed tables (which
 * are, indeed, platform-dependent). Now, adding a platform means adding
 * new snake definitions rather than changing the code, debugging it and
 * then testing that you haven't broken any other platforms.
 *
 * The new code also removes one of the major restrictions that the
 * original code. Specifically it allows various snake paths to co-exist
 * on the same platform. The code makes no assumption which chips is
 * used to inject packets into the snake or where to drain them from --
 * this information is implicitly encoded in the snake path
 * definition. The number of packets to be injected in the snake can
 * also be now specified on a per-snake basis allowing for snakes with
 * different speed/throughput to coexist.
 *
 * Adding a new platform:
 * ======================
 *
 * 1) Add a new member to the system names enum (SS_SYSID_XXX) right before 
 *    SS_SYSID_NUM element
 * 2) Add a platform name to the ss_sysid_str right before NULL. Make sure that
 *    the appropriate config variable is set on that box. For example, for
 *    the platform "foo" you should do "config add foo=1; config save"
 * 3) Specify the correct unit bitmap (it has '1's for all units present in 
 *    the box) in the ss_unit_bitmap array. For example, if your system has
 *    units 0, 1 and 2 the bitmap will be 0x7.
 * 4) Define the basic snake path(s) for the new platform.
 *    Specify the number of paths in the ss_num_paths array.
 *    Check that the number of snake "vertebrae" (the number of elements in
 *    the conn array) does not exceed SS_MAX_SNAKE_CONN.
 *    If it does, please update the constant.
 * 5) Create a snake list for the new platform. This list contains at least 1
 *    snake path, but is most handy when several snakes that differ only by
 *    port numbers should be created
 *    Check if the number of snakes for your platform exceeds SS_MAX_SNAKE. If 
 *    does, please
 *          6.1) Update the constant accordingly
 *          6.2) Add additional MAC addresses to ss_mac_src and ss_mac_dst 
 *               arrays.
 * 6) Add the platform-specific snake list to the ss_snake_table array
 *
 * Defining a snake
 * ================
 *
 * A snake describes a (looped) path that test packet(s) will take
 * through the the system. This path is system dependent and is designed
 * for each of the supported systems separately.
 *
 * A snake has a number of "vertebrae" or, more formally, a list of
 * connections.  Each connection (see system_snake_conn_t type
 * definition) defines where the packets that have been received on a
 * specific unit on a specific port with a specific module ID should go
 * and described by the following tuple:
 *     -- unit             The unit number on which that specific connection 
 *                         should be configured
 *     -- src_mod          The module ID of the arriving packet
 *     -- src_port         On which port the packet arrives
 *     -- dst_mod          What will be the new (rewritten) module ID
 *     -- dst_port         Through which port the packet should get out of the
 *                         chip (unit)
 *     -- flags            Which of {src,dst}_{mod,port} should be modified
 *                         (and how) based on the snake_port number on the 
 *                         snake list.
 *
 * Full snake definition includes:
 *    (a) the number of vertebrae,
 *    (b) the packet length increment that is applied as successive packets
 *        are transmitted,
 *    (c) the "return unit" which is the unit where the filter is set up
 *        to retrieve the packets at the end of the test, and 
 *    (d) list of connections (see system_snake_path_t definition).
 *
 * The number of packets to be injected into/drained from the snake is
 * definied rather implicitly by using packet_length_inc parameters, which
 * specifies how the test packet lengths should be computed: from
 * SS_PACKET_LENGTH_MIN to SS_PACKET_LENGTH_MAX with packet_length_inc
 * length increments.  Note that the packet length increment may be
 * specified on the command line which will override the hardcoded settings.
 *
 * The first connection in the snake definition is also used to
 * implicitly specify the unit that will be used to inject packets into
 * the snake and drain them from it at the end of the test, although (as
 * noted) the unit on which the filter is set up may differ for external
 * loopbacks.
 * 
 * Here is a definition of a snake for a Lancelot board with the
 * explanations:
 *
 * Line
 *   1  STATIC system_snake_path_t lancelot_snake_cw  = {
 *   2      8, 
 *   3      153,
 *   4      {
 *   5         {  1,  0,  0, 10,  0, SS_SP_PATH | SS_DP_PATH | SS_DM_PATH },
 *   6         {  0, 10,  7, 10,  6, SS_SM_PATH | SS_DM_PATH },
 *   7         {  0, 10,  5, 10,  4, SS_SM_PATH | SS_DM_PATH },
 *   8         {  0, 10,  3, 10,  2, SS_SM_PATH | SS_DM_PATH },
 *   9         {  4, 10, 12,  3,  0, SS_SM_PATH | SS_DP_PATH },
 *  10         {  4,  3,  0,  2,  0, SS_SP_PATH | SS_DP_PATH },
 *  11         {  3,  2,  0,  1,  0, SS_SP_PATH | SS_DP_PATH },
 *  12         {  2,  1,  0,  0,  0, SS_SP_PATH | SS_DP_PATH },
 *  13      }
 *  14  };
 * 
 * Line  1: Define a static initialized structure
 * Line  2: The snake has 8 connections (vertebrae)
 * Line  3: The packet length increment for the packets injected into the snake
 *          is 153, which will result in packet lengths SS_PACKET_LENGTH_MIN 
 *          (68 bytes), SS_PACKET_LENGTH_MIN+153 (221 bytes), etc.
 * Line  5: The snake originates on the chip 1 (the leftmost 5690). If a packet
 *          arrives on port 0 and has a module ID 0 (and belongs to the VLAN
 *          SS_VLAN_BASE + 0), then it is sent to the module with the modid 10
 *          port 0. This results in the packet being forwarded to the switch
 *          fabric via the HiGig interface. Flags indicate the for the packets
 *          starting from port N rather than 0, N should be added to the source
 *          port, destination port and destination module number. In addition, 
 *          in case an external loopback mode is chosen, the destination port's
 *          LSB will be toggled. (see ss_port_interconnect_define()) 
 * Line  6: On unit 0 (5670 switch fabric) a packet arriving on port 7 (a HiGig
 *          connected to the leftmost 5690) with modid 10 should be forwarded
 *          to port 6 (one of the stacking HiGigs connected via an external 
 *          loopbak cable to another external HiGig port (5))
 * Line  7: On unit 0 (5670 switch fabric) a packet arriving on port 5
 *          (an external  HiGig port connected via loopback to port 6)
 *          with modid 10 should be forwarded to port 4 (another stacking HiGig
 *          port connected via an external loopbak cable to external HiGig 
 *          port (3))
 * Line  8: On unit 0 (5670 switch fabric) a packet arriving on port 3
 *          (an external  HiGig port connected via loopback to port 4)
 *          with modid 10 should be forwarded to port 2 (a HiGig port
 *          connected to the unit 4 (modid 3) which is te rightmost 5690)
 * Line  9: On unit 4 (the right-most 5690), packets arriving via the HiGig
 *          port (12) with modid 10 should be forwarded to modid (3) port 0 
 *          (basically port 0 on the same chip which is looped back to itself
 *          in MAC or PHY mode or port1 in the EXTERNAL mode).
 * Line 10: On unit 4 (the right-most 5690), packets arriving on port 0 with 
 *          modid 3 should be forwarded to port 0 on modid 2. 
 * Line 11: On unit 3 (the second from the right 5690), packets arriving on 
 *          port 0 with modid 2 should be forwarded to port 0 on modid 1. 
 * Line 10: On unit 2 (the third from the right 5690), packets arriving on 
 *          port 0 with modid 1 should be forwarded to port 0 on modid 0, which
 *          is the leftmost 5690 (unit 1) and this closes the snake.
 *
 ******************************************************************************
 *  System Snake for Firebolt 48 port POE board
 ******************************************************************************
 *  Platform info:
 *
 *  Firebolt 48 POE has two Firebolt (BCM56504) devices. These 2 devices
 *  are connected to each other through highGig Port 2 and 3. Highgig Port
 *  0 and 1 come out of the board and 48 Gigabit ethernet ports come to the
 *  front panel. Ports highGig 0 and 1 are configured in 10 Gig Ethernet mode
 *  for system snake test.
 *
 *  System snake can be run as both internal mode and external mode.
 *
 *  In internal mode, there are 28 snakes:
 *  
 *  24 snakes each of which connect from port X of Firebolt device 0 to
 *  port X of firebolt device 1. ports 0-11 use highGig port 2 and 
 *  port 12-23 use highGig port 3.
 *  
 *  2 snakes on device 0, one on XGE Port 0 and other on XGE port 1.
 *  2 snakes on device 1, one on XGE Port 0 and other on XGE port 1.
 *
 *  In external mode, there are 28 snakes:
 *
 *  12 snakes each of which goes from port X on device 0 to Port X+1. Packets
 *  are looped on device 1 from Port X+1 to Port X through external loopback 
 *  cables to Port X+1 of device 0. These snakes apply for even port numbers
 *  for instance port 0, 2 ..
 *  For external loopback, each odd numbered port should be connected through 
 *  loopback cable to port x+1 i.e. port 1 to port 2, port 3 to port 4 and so
 *  on.
 *  
 *
 *  12 snakes each of which goes from port X+1 on device 0 to Port X1. Packets
 *  are looped on device 1 from Port X to Port X+1 through external loopback 
 *  cables to Port X of device 0. These snakes are applicable for odd numbered
 *  ports, eg port 1,3..
 *
 *  2 snakes on device 0 between XGE Port 0 and 1 and vice versa, i.e.
 *    one between XGE0 to XGE1 and another between XGE1 to XGE0.
 *  2 snakes on device 1 between XGE Port 0 and 1 and vice versa, i.e.
 *    one between XGE0 to XGE1 and another between XGE1 to XGE0.
 */

#ifdef INCLUDE_TEST

#include <sal/appl/sal.h>
#include <sal/appl/config.h>
#include <shared/bsl.h>
#include <soc/drv.h>

#include <bcm/error.h>
#include <bcm/tx.h>
#include <bcm/link.h>
#include <bcm/vlan.h>
#include <bcm/stat.h>

#include <appl/diag/system.h>
#include <appl/diag/test.h>
#include <appl/diag/dport.h>

#include <appl/test/loopback2.h>

#define SS_SOC_SCRIPT       "syssnake.soc"
 
/* defines */
#define SS_MAX_SNAKE                  28 /* max number of test snakes */
#define SS_MAX_SNAKE_CONN             12 /* maximum snake length      */

#define SS_PACKET_LENGTH_MIN          68
#define SS_PACKET_LENGTH_MAX          1516
#define SS_VLAN_BASE                  100
#define SS_COS                        0
#define SS_SEQUENCE_BASE              1
#define SS_PATTERN_BASE               0x11223344
#define SS_PATTERN_INC                0x01020304

/* Default settings (can be overridden on cmd line) */
#define SS_DURATION                   10
#define SS_INTERVAL                   5
#define SS_INTERPATH_WAIT             100000

#define SS_UNIT_ITER(sysid, unit)                               \
        for ((unit) = 0; (unit) < SOC_MAX_NUM_DEVICES; (unit)++)                 \
            if (ss_unit_bitmap[(sysid)] & (1 << (unit)))

#define SS_PACKETS_PER_PATH(path )                      \
        (((SS_PACKET_LENGTH_MAX - SS_PACKET_LENGTH_MIN) \
          / (path)->packet_length_inc) + 1)


/* enums */
enum {
    SS_SPEED_DEFAULT = 0,       /* ss_speed_str and ss_speed_mbits */
    SS_SPEED_MAX,               /* tables must match               */
    SS_SPEED_10,                
    SS_SPEED_100,
    SS_SPEED_1000,
    SS_SPEED_2500,
    SS_SPEED_3000,
    SS_SPEED_10G,
    SS_SPEED_NUM
};

enum {
    SS_MODE_MAC = 0,            /* ss_mode_str table must match */
    SS_MODE_PHY,
    SS_MODE_EXT,
    SS_MODE_NUM
};

/*
 * The system names enum
 */
enum {
    SS_SYSID_FIREBOLT,
    SS_SYSID_FBPOE,
    SS_SYSID_LM_FB48,
    SS_SYSID_LM_FELIX48P,
    SS_SYSID_ICS_EB,
    SS_SYSID_GE_HU48,
    SS_SYSID_FE_HU48,
    SS_SYSID_NUM
};

/*
 * Flags for system snake definitions:
 *    SS_SP_PATH   -- add snake number to the base source port
 *    SS_SM_PATH   -- add snake number to the base source modid
 *    SS_DP_PATH   -- add snake number to the base destination port
 *                    and toggle the LSB in the resulting number in 
 *                    case we use external loopback
 *    SS_DM_PATH   -- add snake number to the base destination port
 *    SS_DP_NOEXT  -- do not toggle the last bit of the destination port number
 *                    in the external loopback mode. This is used to implement
 *                    external loopbacks on chips like 5673 that have only 1
 *                    Ethernet (XG) port. For these chips external loopbacks
 *                    are done by connecting Rx to Tx on the same chip.
 */
enum {
    SS_SP_PATH =  0x00000001,
    SS_SM_PATH =  0x00000002,
    SS_DP_PATH =  0x00000004,
    SS_DM_PATH =  0x00000008,
    SS_DP_NOEXT = 0x00000010,
    SS_DP_FORCE = 0x00000020    /* Force toggling of DP LSB for all modes */
};

/* typedefs */
typedef struct system_snake_opts_s {
    int                 sysid;          /* SS_SYSID_xxx */
    int                 got_herc;       /* Hercules chip present */   
    int                 speed;          /* SS_SPEED_xxx */
    int                 mode;           /* SS_MODE_xxx */
    int                 verbose;        /* Print a lot of status info. */
    int                 duration;       /* Overall test duration in sec. */
    int                 interval;       /* Interval to check counters */
    int                 pli;            /* Packet length increment */
} system_snake_opts_t;

typedef struct system_snake_info_s {
    lb2s_port_connect_t snake_pi[SS_MAX_SNAKE][SS_MAX_SNAKE_CONN];
    lb2s_port_connect_t cmic_return_pi[SS_MAX_SNAKE];
    int                 install_count[SOC_MAX_NUM_DEVICES];
    loopback2_test_t    lw[SOC_MAX_NUM_DEVICES];
    int                 num_packets[SOC_MAX_NUM_DEVICES];
    int                 io_unit[SOC_MAX_NUM_DEVICES];
    int                 io_unit_num;
    lb2_port_stat_t     *stats[SOC_MAX_NUM_DEVICES];
} system_snake_info_t;

typedef struct system_snake_conn_s {
    int          unit;
    int          src_mod;
    bcm_port_t   src_port;
    int          dst_mod;
    bcm_port_t   dst_port;
    unsigned int flags;
} system_snake_conn_t;

typedef struct system_snake_path_s {
    int        conn_count;
    int        packet_length_inc;
    int        return_unit; /* For ext, return unit may differ from tx unit */
    system_snake_conn_t conn[SS_MAX_SNAKE_CONN];
} system_snake_path_t;

typedef struct system_snake_list_s {
    int                  snake_port;
    system_snake_path_t *snake_path;
} system_snake_list_t;

/* locals */
static const char *ss_speed_str[SS_SPEED_NUM + 1] = {
    "Default",          /* SS_SPEED_DEFAULT */
    "MAXimum",          /* SS_SPEED_MAX */
    "10fullduplex",     /* SS_SPEED_10 */
    "100fullduplex",    /* SS_SPEED_100 */
    "1000fullduplex",   /* SS_SPEED_1000 */
    "2500fullduplex",   /* SS_SPEED_2500 */
    "3000fullduplex",   /* SS_SPEED_3000 */
    "10Gfullduplex",    /* SS_SPEED_10G */
    NULL
};

static int ss_speed_mbits[SS_SPEED_NUM] = {
    0,                  /* Not used */  
    0,                  /* Not used */  
    10,                 /* 10Mbps */
    100,                /* 100Mbps */
    1000,               /* 1Gbps */
    2500,               /* 2.5 Gbps */
    3000,               /* 3.0 Gbps */
    10000               /* 10Gbps */
};

static int ss_speed_abil[SS_SPEED_NUM] = {
    0,                  /* Not used */  
    0,                  /* Not used */  
    BCM_PORT_ABIL_10MB_FD, 
    BCM_PORT_ABIL_100MB_FD, 
    BCM_PORT_ABIL_1000MB_FD, 
    BCM_PORT_ABIL_2500MB_FD,
    BCM_PORT_ABIL_3000MB_FD,
    BCM_PORT_ABIL_10GB_FD 
};


static const char *ss_mode_str[SS_MODE_NUM + 1] = {
    "MAC",              /* SS_MODE_MAC */
    "PHY",              /* SS_MODE_PHY */
    "EXT",              /* SS_MODE_EXT */
    NULL
};

static const char *ss_sysid_str[SS_SYSID_NUM + 1] = {
    "firebolt",         /* SS_SYSID_FIREBOLT  */
    "fbpoe",            /* SS_SYSID_FBPOE */
    "lmfb48",           /* SS_SYSID_LM_FB48 */
    "felix48",          /* SS_SYSID_LM_FELIX48P */
    "ics_eb",           /* SS_SYSID_ICS_EB */
    "ge_hu_48p",        /* SS_SYSID_GE_HU48 */
    "fe_hu_48p",        /* SS_SYSID_FE_HU48 */
    NULL
};

STATIC uint32 ss_unit_bitmap[SS_SYSID_NUM] = {
    0x01,       /* SS_SYSID_FIREBOLT */
    0x03,       /* SS_SYSID_FBPOE     */
    0x03,       /* SS_SYSID_LM_FB48   */
    0x03,       /* SS_SYSID_LM_FELIX48P */
    0x03,       /* SS_SYSID_ICS_EB */
    0x03,       /* SS_SYSID_GE_HU48 */
    0x03,       /* SS_SYSID_FE_HU48 */
};

STATIC uint32 ss_num_paths[SS_SYSID_NUM] = {
    28,         /* SS_SYSID_FIREBOLT */
    28,         /* SS_SYSID_FBPOE     */
    24,         /* SS_SYSID_LM_FB48   */
    27,         /* SS_SYSID_LM_FELIX48P */
    26,         /* SS_SYSID_ICS_EB */
    28,         /* SS_SYSID_GE_HU48 */
    26,         /* SS_SYSID_FE_HU48 */
};

STATIC char * ss_init_file[SS_SYSID_NUM] = {
    NULL,             /* SS_SYSID_FIREBOLT */
    SS_SOC_SCRIPT,    /* SS_SYSID_FBPOE     */
    SS_SOC_SCRIPT,    /* SS_SYSID_LM_FB48   */
    NULL,             /* SS_SYSID_LM_FELIX48P */
    SS_SOC_SCRIPT,    /* SS_SYSID_ICS_EB */
    SS_SOC_SCRIPT,    /* SS_SYSID_GE_HU48 */
    SS_SOC_SCRIPT,    /* SS_SYSID_FE_HU48 */
};

STATIC sal_mac_addr_t ss_mac_src[SS_MAX_SNAKE] = {
    {0x00, 0x11, 0x22, 0x33, 0x44, 0x55},
    {0x00, 0x11, 0x22, 0x33, 0x44, 0x66},
    {0x00, 0x11, 0x22, 0x33, 0x44, 0x77},
    {0x00, 0x11, 0x22, 0x33, 0x44, 0x88},
    {0x00, 0x11, 0x22, 0x33, 0x44, 0x99},
    {0x00, 0x11, 0x22, 0x33, 0x44, 0xaa},
    {0x00, 0x11, 0x22, 0x33, 0x44, 0xbb},
    {0x00, 0x11, 0x22, 0x33, 0x44, 0xcc},
    {0x00, 0x11, 0x22, 0x33, 0x44, 0xdd},
    {0x00, 0x11, 0x22, 0x33, 0x44, 0xee},
    {0x00, 0x11, 0x22, 0x33, 0x44, 0xff},
    {0x00, 0x11, 0x22, 0x33, 0x44, 0x00},
    {0x00, 0x11, 0x22, 0x33, 0x44, 0x11},
    {0x00, 0x11, 0x22, 0x33, 0x44, 0x12},
    {0x00, 0x11, 0x22, 0x33, 0x45, 0x55},
    {0x00, 0x11, 0x22, 0x33, 0x45, 0x66},
    {0x00, 0x11, 0x22, 0x33, 0x45, 0x77},
    {0x00, 0x11, 0x22, 0x33, 0x45, 0x88},
    {0x00, 0x11, 0x22, 0x33, 0x45, 0x99},
    {0x00, 0x11, 0x22, 0x33, 0x45, 0xaa},
    {0x00, 0x11, 0x22, 0x33, 0x45, 0xbb},
    {0x00, 0x11, 0x22, 0x33, 0x45, 0xcc},
    {0x00, 0x11, 0x22, 0x33, 0x45, 0xdd},
    {0x00, 0x11, 0x22, 0x33, 0x45, 0xee},
    {0x00, 0x11, 0x22, 0x33, 0x45, 0xff},
    {0x00, 0x11, 0x22, 0x33, 0x45, 0x00},
    {0x00, 0x11, 0x22, 0x33, 0x45, 0x11},
    {0x00, 0x11, 0x22, 0x33, 0x45, 0x12},
};

STATIC sal_mac_addr_t ss_mac_dst[SS_MAX_SNAKE] = {
    {0x66, 0x77, 0x88, 0x99, 0xaa, 0xff},
    {0x66, 0x77, 0x88, 0x99, 0xaa, 0x00},
    {0x66, 0x77, 0x88, 0x99, 0xaa, 0x11},
    {0x66, 0x77, 0x88, 0x99, 0xaa, 0x22},
    {0x66, 0x77, 0x88, 0x99, 0xaa, 0x33},
    {0x66, 0x77, 0x88, 0x99, 0xaa, 0x44},
    {0x66, 0x77, 0x88, 0x99, 0xaa, 0x55},
    {0x66, 0x77, 0x88, 0x99, 0xaa, 0x66},
    {0x66, 0x77, 0x88, 0x99, 0xaa, 0x77},
    {0x66, 0x77, 0x88, 0x99, 0xaa, 0x88},
    {0x66, 0x77, 0x88, 0x99, 0xaa, 0x99},
    {0x66, 0x77, 0x88, 0x99, 0xaa, 0xaa},
    {0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb},
    {0x66, 0x77, 0x88, 0x99, 0xaa, 0xcc},
    {0x66, 0x77, 0x88, 0x99, 0xab, 0xff},
    {0x66, 0x77, 0x88, 0x99, 0xab, 0x00},
    {0x66, 0x77, 0x88, 0x99, 0xab, 0x11},
    {0x66, 0x77, 0x88, 0x99, 0xab, 0x22},
    {0x66, 0x77, 0x88, 0x99, 0xab, 0x33},
    {0x66, 0x77, 0x88, 0x99, 0xab, 0x44},
    {0x66, 0x77, 0x88, 0x99, 0xab, 0x55},
    {0x66, 0x77, 0x88, 0x99, 0xab, 0x66},
    {0x66, 0x77, 0x88, 0x99, 0xab, 0x77},
    {0x66, 0x77, 0x88, 0x99, 0xab, 0x88},
    {0x66, 0x77, 0x88, 0x99, 0xab, 0x99},
    {0x66, 0x77, 0x88, 0x99, 0xab, 0xaa},
    {0x66, 0x77, 0x88, 0x99, 0xab, 0xbb},
    {0x66, 0x77, 0x88, 0x99, 0xab, 0xcc},
};


/*
 * Snake descriptions per board
 */

/*
 * Firebolt uses 28 snakes, one per GE/XE port on 56504.  
 */

STATIC system_snake_path_t firebolt_snake_cw = {
    1,
    480,
    0,
    { /*   u  sm  sp  dm  dp  flags */
        {  0,  0,    0, 0,   0, SS_DP_PATH | SS_DP_FORCE },
    }
};

STATIC system_snake_list_t firebolt_snake_test[] = {
    {   0, &firebolt_snake_cw },
    {   1, &firebolt_snake_cw },
    {   2, &firebolt_snake_cw },
    {   3, &firebolt_snake_cw },
    {   4, &firebolt_snake_cw },
    {   5, &firebolt_snake_cw },
    {   6, &firebolt_snake_cw },
    {   7, &firebolt_snake_cw },
    {   8, &firebolt_snake_cw },
    {   9, &firebolt_snake_cw },
    {  10, &firebolt_snake_cw },
    {  11, &firebolt_snake_cw },
    {  12, &firebolt_snake_cw },
    {  13, &firebolt_snake_cw },
    {  14, &firebolt_snake_cw },
    {  15, &firebolt_snake_cw },
    {  16, &firebolt_snake_cw },
    {  17, &firebolt_snake_cw },
    {  18, &firebolt_snake_cw },
    {  19, &firebolt_snake_cw },
    {  20, &firebolt_snake_cw },
    {  21, &firebolt_snake_cw },
    {  22, &firebolt_snake_cw },
    {  23, &firebolt_snake_cw },
    {  24, &firebolt_snake_cw },
    {  25, &firebolt_snake_cw },
    {  26, &firebolt_snake_cw },
    {  27, &firebolt_snake_cw },
};

/*
 * POE uses 2 snakes, one per GE port on 56504.
 * For even port numbers they go clockwise and for odd port 
 * numbers they go counter-clockwise. Also there are 4 snakes
 * one each XGE0-1 port on each device.
 */
STATIC system_snake_path_t fbpoe_snake_cw  = {
    2, 
    183,
    0,
    { /*   u  sm  sp  dm  dp  flags */
        {  0,  0,  0,  1,  0, SS_SP_PATH | SS_DP_PATH },
        {  1,  1,  0,  0,  0, SS_SP_PATH | SS_DP_PATH },
    }
};

STATIC system_snake_path_t fbpoe_snake_ccw = {
    2,
    183,
    0,
    { /*   u  sm  sp  dm  dp  flags */
        {  1,  1,  0,  0,  0, SS_SP_PATH | SS_DP_PATH },
        {  0,  0,  0,  1,  0, SS_SP_PATH | SS_DP_PATH },
    }              
};

STATIC system_snake_path_t fbpoe_10ge_u0_snake_cw  = {
    1, 
    480,
    0,
    { /*   u  sm  sp  dm  dp  flags */
        {  0,  0,  0,  0,  0, SS_SP_PATH | SS_DP_PATH },
    }
};

STATIC system_snake_path_t fbpoe_10ge_u1_snake_cw  = {
    1, 
    480,
    1,
    { /*   u  sm  sp  dm  dp  flags */
        {  1,  1,   0,  1,   0, SS_SP_PATH | SS_DP_PATH },
    }
};

STATIC system_snake_list_t fbpoe_snake_test[] = {
    {  0, &fbpoe_snake_cw },
    {  1, &fbpoe_snake_ccw },
    {  2, &fbpoe_snake_cw },
    {  3, &fbpoe_snake_ccw },
    {  4, &fbpoe_snake_cw },
    {  5, &fbpoe_snake_ccw },
    {  6, &fbpoe_snake_cw },
    {  7, &fbpoe_snake_ccw },
    {  8, &fbpoe_snake_cw },
    {  9, &fbpoe_snake_ccw },
    { 10, &fbpoe_snake_cw },
    { 11, &fbpoe_snake_ccw },
    { 12, &fbpoe_snake_cw },
    { 13, &fbpoe_snake_ccw },
    { 14, &fbpoe_snake_cw },
    { 15, &fbpoe_snake_ccw },
    { 16, &fbpoe_snake_cw },
    { 17, &fbpoe_snake_ccw },
    { 18, &fbpoe_snake_cw },
    { 19, &fbpoe_snake_ccw },
    { 20, &fbpoe_snake_cw },
    { 21, &fbpoe_snake_ccw },
    { 22, &fbpoe_snake_cw },
    { 23, &fbpoe_snake_ccw },
    { 24, &fbpoe_10ge_u0_snake_cw },
    { 25, &fbpoe_10ge_u0_snake_cw },
    { 24, &fbpoe_10ge_u1_snake_cw },
    { 25, &fbpoe_10ge_u1_snake_cw },
};

/*
 * LM_FB48 uses 2 snakes, one per GE port on 56504.
 * For even port numbers they go clockwise and for odd port 
 * numbers they go counter-clockwise.
 */
STATIC system_snake_path_t lm_fb48_snake_cw  = {
    2, 
    183,
    0,
    { /*   u  sm  sp  dm  dp  flags */
        {  0,  0,  0,  1,  0, SS_SP_PATH | SS_DP_PATH },
        {  1,  1,  0,  0,  0, SS_SP_PATH | SS_DP_PATH },
    }
};

STATIC system_snake_path_t lm_fb48_snake_ccw = {
    2,
    183,
    0,
    { /*   u  sm  sp  dm  dp  flags */
        {  1,  1,  0,  0,  0, SS_SP_PATH | SS_DP_PATH },
        {  0,  0,  0,  1,  0, SS_SP_PATH | SS_DP_PATH },
    }              
};

STATIC system_snake_list_t lmfb48_snake_test[] = {
    {  0, &lm_fb48_snake_cw },
    {  1, &lm_fb48_snake_ccw },
    {  2, &lm_fb48_snake_cw },
    {  3, &lm_fb48_snake_ccw },
    {  4, &lm_fb48_snake_cw },
    {  5, &lm_fb48_snake_ccw },
    {  6, &lm_fb48_snake_cw },
    {  7, &lm_fb48_snake_ccw },
    {  8, &lm_fb48_snake_cw },
    {  9, &lm_fb48_snake_ccw },
    { 10, &lm_fb48_snake_cw },
    { 11, &lm_fb48_snake_ccw },
    { 12, &lm_fb48_snake_cw },
    { 13, &lm_fb48_snake_ccw },
    { 14, &lm_fb48_snake_cw },
    { 15, &lm_fb48_snake_ccw },
    { 16, &lm_fb48_snake_cw },
    { 17, &lm_fb48_snake_ccw },
    { 18, &lm_fb48_snake_cw },
    { 19, &lm_fb48_snake_ccw },
    { 20, &lm_fb48_snake_cw },
    { 21, &lm_fb48_snake_ccw },
    { 22, &lm_fb48_snake_cw },
    { 23, &lm_fb48_snake_ccw },
};

/*
 * Felix 48 port uses 2 snakes per port. The platform has 2
 * felix devices (24 FE, 1 GE and 1 10GE port). The 2 devices are
 * connected through HG0 port.
 */
STATIC system_snake_path_t felix48p_snake_cw  = {
    2, 
    483,
    0,
    { /*   u  sm  sp  dm  dp  flags */
        {  0,  0,  0,  1,  0, SS_SP_PATH | SS_DP_PATH },
        {  1,  1,  0,  0,  0, SS_SP_PATH | SS_DP_PATH },
    }
};

STATIC system_snake_path_t felix48p_snake_ccw = {
    2,
    483,
    0,
    { /*   u  sm  sp  dm  dp  flags */
        {  1,  1,  0,  0,  0, SS_SP_PATH | SS_DP_PATH },
        {  0,  0,  0,  1,  0, SS_SP_PATH | SS_DP_PATH },
    }              
};

STATIC system_snake_path_t felix48p_snake_ge_cw  = {
    2, 
    183,
    0,
    { /*   u  sm  sp  dm  dp  flags */
        {  0,  0,  0,  1,  0, SS_SP_PATH | SS_DP_PATH },
        {  1,  1,  0,  0,  0, SS_SP_PATH | SS_DP_PATH },
    }
};

STATIC system_snake_path_t felix48p_snake_ge_ccw = {
    2,
    183,
    0,
    { /*   u  sm  sp  dm  dp  flags */
        {  1,  1,  0,  0,  0, SS_SP_PATH | SS_DP_PATH },
        {  0,  0,  0,  1,  0, SS_SP_PATH | SS_DP_PATH },
    }              
};

STATIC system_snake_path_t felix48p_10ge_snake_cw  = {
    2, 
    480,
    0,
    { /*   u  sm  sp  dm  dp  flags */
        {  0,  0,  27,  1,  27, SS_DP_NOEXT },
        {  1,  1,  27,  0,  27, SS_DP_NOEXT },
    }
};

STATIC system_snake_list_t felix48p_snake_test[] = {
    {  0, &felix48p_snake_cw },
    {  1, &felix48p_snake_ccw },
    {  2, &felix48p_snake_cw },
    {  3, &felix48p_snake_ccw },
    {  4, &felix48p_snake_cw },
    {  5, &felix48p_snake_ccw },
    {  6, &felix48p_snake_cw },
    {  7, &felix48p_snake_ccw },
    {  8, &felix48p_snake_cw },
    {  9, &felix48p_snake_ccw },
    { 10, &felix48p_snake_cw },
    { 11, &felix48p_snake_ccw },
    { 12, &felix48p_snake_cw },
    { 13, &felix48p_snake_ccw },
    { 14, &felix48p_snake_cw },
    { 15, &felix48p_snake_ccw },
    { 16, &felix48p_snake_cw },
    { 17, &felix48p_snake_ccw },
    { 18, &felix48p_snake_cw },
    { 19, &felix48p_snake_ccw },
    { 20, &felix48p_snake_cw },
    { 21, &felix48p_snake_ccw },
    { 22, &felix48p_snake_cw },
    { 23, &felix48p_snake_ccw },
    { 24, &felix48p_snake_ge_cw },
    { 25, &felix48p_snake_ge_ccw },
    { 27, &felix48p_10ge_snake_cw },
};

/*
 * Raptor/Raven cascase platform with slave connected over EB bus.
 * special config setting needed ss_ignore_pbmp=0x3e so that we 
 * dont mess around with the link ports as eb slave depends on that.
 */

STATIC system_snake_path_t raptor_eb_snake_cw  = {
    2, 
    483,
    0,
    { /*   u  sm  sp  dm  dp  flags */
        {  0,  0,  0,  1,  0, SS_SP_PATH | SS_DP_PATH },
        {  1,  1,  0,  0,  0, SS_SP_PATH | SS_DP_PATH },
    }
};

STATIC system_snake_path_t raptor_eb_ge1_snake_cw  = {
    2, 
    1600,
    1,
    { /*   u  sm  sp  dm  dp  flags */
        {  0,  0,  2,  1,  2, SS_DP_NOEXT },
        {  1,  1,  2,  0,  2, SS_DP_NOEXT },
    }
};
STATIC system_snake_path_t raptor_eb_ge4_snake_cw  = {
    2, 
    1600,
    1,
    { /*   u  sm  sp  dm  dp  flags */
        {  0,  0,  5,  1,  5, SS_DP_NOEXT },
        {  1,  1,  5,  0,  5, SS_DP_NOEXT },
    }
};

STATIC system_snake_list_t raptor_eb_snake_test[] = {
    {  6, &raptor_eb_snake_cw },
    {  7, &raptor_eb_snake_cw },
    {  8, &raptor_eb_snake_cw },
    {  9, &raptor_eb_snake_cw },
    { 10, &raptor_eb_snake_cw },
    { 11, &raptor_eb_snake_cw },
    { 12, &raptor_eb_snake_cw },
    { 13, &raptor_eb_snake_cw },
    { 14, &raptor_eb_snake_cw },
    { 15, &raptor_eb_snake_cw },
    { 16, &raptor_eb_snake_cw },
    { 17, &raptor_eb_snake_cw },
    { 18, &raptor_eb_snake_cw },
    { 19, &raptor_eb_snake_cw },
    { 20, &raptor_eb_snake_cw },
    { 21, &raptor_eb_snake_cw },
    { 22, &raptor_eb_snake_cw },
    { 23, &raptor_eb_snake_cw },
    { 24, &raptor_eb_snake_cw },
    { 25, &raptor_eb_snake_cw },
    { 26, &raptor_eb_snake_cw },
    { 27, &raptor_eb_snake_cw },
    { 28, &raptor_eb_snake_cw },
    { 29, &raptor_eb_snake_cw },
    {  2, &raptor_eb_ge1_snake_cw },
    {  5, &raptor_eb_ge4_snake_cw },
};

/*
 * In Hurricane, Port 1 is reserved port..
 * Hu uses 2 ports per snake - one per unit.
 * even ports go clockwise and odd ports counterclockwise
 * Ext snake uses one cw and one ccw
 * for SFP ports, In unit0 it is 26 & 27, in Unit 1 it is 28 & 29
 */
STATIC system_snake_path_t hu_48p_snake_cw  = {
    2, 
    183,
    0,
    { /*   u  sm  sp  dm  dp  flags */
        {  0,  0,  0,  1,  0, SS_SP_PATH | SS_DP_PATH },
        {  1,  1,  0,  0,  0, SS_SP_PATH | SS_DP_PATH },
    }
};

STATIC system_snake_path_t hu_48p_snake_ccw  = {
    2, 
    183,
    0,
    { /*   u  sm  sp  dm  dp  flags */
        {  1,  1,  0,  0,  0, SS_SP_PATH | SS_DP_PATH },
        {  0,  0,  0,  1,  0, SS_SP_PATH | SS_DP_PATH },
    }
};

STATIC system_snake_path_t hu_48p_snake_u0  = {
    1, 
    183,
    0,
    { /*   u  sm  sp  dm  dp  flags */
        {  0,  0,  0,  0,  0, SS_SP_PATH | SS_DP_PATH},
    }
};

STATIC system_snake_path_t hu_48p_snake_u1  = {
    1,
    183,
    1,
    { /*   u  sm  sp  dm  dp  flags */
        {  1,  1,  0,  1,  0, SS_SP_PATH | SS_DP_PATH},
    }
};

STATIC system_snake_list_t hurricane_ge_48p_snake_test[] = {
    {  2, &hu_48p_snake_cw },
    {  3, &hu_48p_snake_ccw },
    {  4, &hu_48p_snake_cw },
    {  5, &hu_48p_snake_ccw },
    {  6, &hu_48p_snake_cw },
    {  7, &hu_48p_snake_ccw },
    {  8, &hu_48p_snake_cw },
    {  9, &hu_48p_snake_ccw },
    {  10, &hu_48p_snake_cw },
    {  11, &hu_48p_snake_ccw },
    {  12, &hu_48p_snake_cw },
    {  13, &hu_48p_snake_ccw },
    {  14, &hu_48p_snake_cw },
    {  15, &hu_48p_snake_ccw },
    {  16, &hu_48p_snake_cw },
    {  17, &hu_48p_snake_ccw },
    {  18, &hu_48p_snake_cw },
    {  19, &hu_48p_snake_ccw },
    {  20, &hu_48p_snake_cw },
    {  21, &hu_48p_snake_ccw },
    {  22, &hu_48p_snake_cw },
    {  23, &hu_48p_snake_ccw },
    {  24, &hu_48p_snake_cw },
    {  25, &hu_48p_snake_ccw },
    {  26, &hu_48p_snake_u1 },
    {  27, &hu_48p_snake_u1 },
    {  28, &hu_48p_snake_u0 },
    {  29, &hu_48p_snake_u0 },
};

STATIC system_snake_list_t hurricane_fe_48p_snake_test[] = {
    {  2, &hu_48p_snake_cw },
    {  3, &hu_48p_snake_ccw },
    {  4, &hu_48p_snake_cw },
    {  5, &hu_48p_snake_ccw },
    {  6, &hu_48p_snake_cw },
    {  7, &hu_48p_snake_ccw },
    {  8, &hu_48p_snake_cw },
    {  9, &hu_48p_snake_ccw },
    {  10, &hu_48p_snake_cw },
    {  11, &hu_48p_snake_ccw },
    {  12, &hu_48p_snake_cw },
    {  13, &hu_48p_snake_ccw },
    {  14, &hu_48p_snake_cw },
    {  15, &hu_48p_snake_ccw },
    {  16, &hu_48p_snake_cw },
    {  17, &hu_48p_snake_ccw },
    {  18, &hu_48p_snake_cw },
    {  19, &hu_48p_snake_ccw },
    {  20, &hu_48p_snake_cw },
    {  21, &hu_48p_snake_ccw },
    {  22, &hu_48p_snake_cw },
    {  23, &hu_48p_snake_ccw },
    {  24, &hu_48p_snake_cw },
    {  25, &hu_48p_snake_ccw },
    {  26, &hu_48p_snake_u1 },
    {  27, &hu_48p_snake_u1 },
};

/*
 * Finally, all boards together
 */
STATIC system_snake_list_t *ss_snake_table[SS_SYSID_NUM] = {
    firebolt_snake_test,
    fbpoe_snake_test,
    lmfb48_snake_test,
    felix48p_snake_test,
    raptor_eb_snake_test,
    hurricane_ge_48p_snake_test,
    hurricane_fe_48p_snake_test,
};


STATIC system_snake_info_t ssi;

STATIC bcm_port_config_t pcfg[SOC_MAX_NUM_DEVICES];

STATIC int
ss_unit_setup(system_snake_opts_t *opts, int unit)
{
    int                   rv;
    bcm_port_t            port, dport;
    bcm_pbmp_t            vlan_pbm;
    bcm_vlan_t            vlan;
    pbmp_t                ss_pbm, ignore_pbm;

    if (opts->verbose) {
        cli_out("Configuring Ethernet ports on unit %d (%s)\n", 
                unit, SOC_UNIT_NAME(unit));
    }

    if (bcm_port_config_get(unit, &pcfg[unit]) != BCM_E_NONE) {
        cli_out("Error: bcm ports not initialized\n");
        return (-1);
    }

    if ((rv = bcm_vlan_destroy_all(unit)) < 0) {
        cli_out("Could not destroy existing VLANs: %s\n",
                bcm_errmsg(rv));
        return (-1);
    }

    BCM_PBMP_ASSIGN(vlan_pbm, pcfg[unit].all);
    ss_pbm = pcfg[unit].e;
    if (soc_property_get_str(unit, spn_SS_IGNORE_PBMP)) {
        ignore_pbm = soc_property_get_bcm_pbmp(unit, spn_SS_IGNORE_PBMP, 0);
        BCM_PBMP_REMOVE(ss_pbm, ignore_pbm);
        cli_out("Removed ignore_pbm from valid ports \n");
    }
    DPORT_BCM_PBMP_ITER(unit, ss_pbm, dport, port) {
        if (opts->verbose) {
            cli_out("\tConfiguring port %s\n", BCM_PORT_NAME(unit, port));
        }

        /* Note Ingress Etherport # = path # for now */
        vlan = SS_VLAN_BASE + port;
        if (((rv = bcm_vlan_create(unit, vlan)) < 0) &&
            (rv != BCM_E_EXISTS) ) {
            test_error(unit,
                       "Could not create VLAN %d: %s\n",
                       vlan, bcm_errmsg(rv));
            return (-1);
        }

        if ((rv = bcm_vlan_port_add(unit, vlan, vlan_pbm, vlan_pbm)) < 0) {
            test_error(unit,
                       "Could not add test ports to VLAN %d: %s\n",
                       vlan, bcm_errmsg(rv));
            return (-1);
        }
 
        if ((rv = bcm_port_untagged_vlan_set(unit, port, vlan)) < 0) {
            test_error(unit,
                       "Could not set port %s to tag with VLAN %d: %s\n",
                       BCM_PORT_NAME(unit, port), vlan, bcm_errmsg(rv));
            return (-1);
        }

        if (opts->mode == SS_MODE_MAC) {
            if ((rv = bcm_port_loopback_set(unit, port,
                                            BCM_PORT_LOOPBACK_MAC)) < 0) {
                test_error(unit,
                           "Port %s: Failed to set MAC loopback: %s\n",
                           BCM_PORT_NAME(unit, port), bcm_errmsg(rv));
                return(-1);                     /* In case they continue */
            }
        } else if (opts->mode == SS_MODE_PHY) {
            if ((rv = bcm_port_loopback_set(unit, port,
                                            BCM_PORT_LOOPBACK_PHY)) < 0) {
                test_error(unit,
                           "Port %s: Failed to set PHY loopback: %s\n",
                           BCM_PORT_NAME(unit, port), bcm_errmsg(rv));
                return(-1);                     /* In case they continue */
            }
        } else {
            /* Else normal port operation for SS_MODE_EXT, where  */
            /* ports are wired 1-2, 3-4, 5-6 etc.                 */
 
            assert(opts->mode == SS_MODE_EXT);
        }

        if ((rv = bcm_port_enable_set(unit, port, TRUE)) < 0) {
            test_error(unit,
                       "Port %s: Failed to enable ports: %s\n",
                       BCM_PORT_NAME(unit, port), bcm_errmsg(rv));
            return(-1);                         /* In case they continue */
        }

        if ((rv = bcm_port_learn_set(unit, port,
                                     BCM_PORT_LEARN_FWD)) < 0) {
            test_error(unit,
                       "Unable to set learn state: port %s: %s\n",
                       BCM_PORT_NAME(unit, port), bcm_errmsg(rv));
            return (-1);
        }

        /* Use speed set by rc unless overridden on cmd line. */
        if (!BCM_PBMP_MEMBER(pcfg[unit].xe, port)) {
            int speed = ss_speed_mbits[opts->speed];

            if (opts->speed == SS_SPEED_DEFAULT) {
                if ((rv = bcm_port_speed_max(unit, port, &speed)) < 0) {
                    test_error(unit,
                               "Port %s: Failed to get max port speed: %s\n",
                               BCM_PORT_NAME(unit, port), bcm_errmsg(rv));
                    return(-1);                     /* In case they continue */
                }
            }

            /*
             * Set speed in any case.  Otherwise, the internal PHY/MAC
             * will not necessarily be programmed correctly since linkscan
             * is no longer scanning the port after it is in loopback.
             */
            if ((rv = bcm_port_speed_set(unit, port, speed)) < 0) {
                test_error(unit,
                           "Port %s: Failed to set port speed: %s\n",
                           BCM_PORT_NAME(unit, port), bcm_errmsg(rv));
                return(-1);                     /* In case they continue */
            }
        }

        /* Always use full duplex for ss. */
        if ((rv = bcm_port_duplex_set(unit, port, 
                                      BCM_PORT_DUPLEX_FULL)) < 0) {
            test_error(unit,
                       "Port %s: Failed to set port duplex: %s\n",
                       BCM_PORT_NAME(unit, port), bcm_errmsg(rv));
            return(-1);                     /* In case they continue */
        }

        if (opts->mode == SS_MODE_EXT) {
            /* Always use full duplex for ss. */
            if ((rv = bcm_port_advert_set(unit, port, 
                                          BCM_PORT_ABIL_FD)) < 0) {
                test_error(unit,
                           "Port %s: Failed to set port duplex advert: %s\n",
                           BCM_PORT_NAME(unit, port), bcm_errmsg(rv));
                return(-1);                     /* In case they continue */
            }
            if (opts->speed != SS_SPEED_DEFAULT) {
                /* Use cmd line speed */
                if ((rv =
                     bcm_port_advert_set(unit, port, 
                                         ss_speed_abil[opts->speed])) < 0) {
                    test_error(unit,
                               "Port %s: "
                               "Failed to set port speed advert: %s\n",
                               BCM_PORT_NAME(unit, port), bcm_errmsg(rv));
                    return(-1);                     /* In case they continue */
                }
            }
            /* Restart autonegotiation */
            if ((rv = bcm_port_autoneg_set(unit, port, 
                                           TRUE)) < 0) {
                test_error(unit,
                           "Port %s: Failed to restart autonegotiation: %s\n",
                           BCM_PORT_NAME(unit, port), bcm_errmsg(rv));
                return(-1);                     /* In case they continue */
            }
        } /* SS_MODE_EXT advert and autoneg */

    } /* For each port */

    return (0);
}        

STATIC void
ss_set_packet_length_increment(system_snake_opts_t *opts)
{
    int                   snake;
    int                   sysid = opts->sysid;
    int                   snake_num  = ss_num_paths[sysid];
    system_snake_list_t  *snake_list = ss_snake_table[sysid];

    for (snake = 0; snake < snake_num; snake++) {
        snake_list[snake].snake_path->packet_length_inc = opts->pli;
    }
}

STATIC int
ss_port_interconnect_define(system_snake_opts_t *opts)
{
    lb2s_port_connect_t  *pi;
    int                   snake;
    system_snake_list_t  *snake_list;
    system_snake_path_t  *snake_path;
    int                   snake_num;
    int                   snake_len;
    int                   snake_port;
    int                   i;
    int                   sysid = opts->sysid;

    snake_list = ss_snake_table[sysid];
    snake_num  = ss_num_paths[sysid];
    
    for (snake = 0; snake < snake_num; snake++) {
        /*
         * Define snake interconnects
         */
        if (opts->verbose) {
            cli_out("Setting up snake %d\n", snake);
        }

        snake_path = snake_list[snake].snake_path;
        snake_port = snake_list[snake].snake_port;
        snake_len  = snake_path->conn_count;

        for (i = 0; i < snake_len; i++) {
            pi = &ssi.snake_pi[snake][i];
            sal_memset(pi, 0, sizeof(*pi));

            pi->src_modid  = snake_path->conn[i].src_mod;
            pi->this_port  = snake_path->conn[i].src_port;
            pi->dst_modid  = snake_path->conn[i].dst_mod;
            pi->to_port    = snake_path->conn[i].dst_port;
            pi->added_vlan = SS_VLAN_BASE + snake_port;
            ENET_SET_MACADDR(pi->src_mac, ss_mac_src[snake]);
            ENET_SET_MACADDR(pi->dst_mac, ss_mac_dst[snake]);

            if (snake_path->conn[i].flags & SS_SM_PATH) { 
                pi->src_modid += snake_port; 
            }

            if (snake_path->conn[i].flags & SS_SP_PATH) { 
                pi->this_port += snake_port; 
            }
            
            if (snake_path->conn[i].flags & SS_DM_PATH) { 
                pi->dst_modid += snake_port; 
            }

            if (snake_path->conn[i].flags & SS_DP_PATH) {
                pi->to_port += snake_port;
                if ((opts->mode == SS_MODE_EXT) ||
                    (snake_path->conn[i].flags & SS_DP_FORCE)) {
                    if (!(snake_path->conn[i].flags & SS_DP_NOEXT)) {
                        pi->to_port ^= 1;
                    }
                }
            }
            
            pi->valid_info = TRUE;

            if (opts->verbose) {
                cli_out("\t%02d.%02d -> %02d.%02d\n", 
                        pi->src_modid, pi->this_port,
                        pi->dst_modid, pi->to_port);
            }
        }

        /*
         * Define the CPU return path (for each snake it is the first unit 
         * in snake's definition that is used for cell injection and return)
         */
        pi = &ssi.cmic_return_pi[snake];
        sal_memset(pi, 0, sizeof(*pi));

        pi->src_modid  = snake_path->conn[0].src_mod;
        pi->this_port  = snake_port;
        pi->dst_modid  = snake_path->conn[0].src_mod;
        pi->to_port    = CMIC_PORT(snake_path->conn[0].unit);
        pi->added_vlan = SS_VLAN_BASE + snake_port;
        ENET_SET_MACADDR(pi->src_mac, ss_mac_src[snake]);
        ENET_SET_MACADDR(pi->dst_mac, ss_mac_dst[snake]);
        pi->valid_info = TRUE;

        if (opts->verbose) {
            cli_out("\tReturn path via unit %d\n", snake_path->conn[0].unit);
        }
    }

    return (0);
}

STATIC int
ss_port_interconnect_execute(system_snake_opts_t *opts)
{
    lb2s_port_connect_t  *pi;
    int                   snake;
    int                   snake_len;
    system_snake_path_t  *snake_path; 
    int                   i;
    int                   rv;
    int                   unit = 0;
    int                   sysid      = opts->sysid;
    system_snake_list_t  *snake_list = ss_snake_table[sysid];
    int                   snake_num  = ss_num_paths[sysid];
    
    for (snake = 0; snake < snake_num; snake++) {
        if (opts->verbose) {
            cli_out("Executing connections for snake %d\n", snake);
        }

        snake_path = snake_list[snake].snake_path;
        snake_len  = snake_path->conn_count;

        for (i = 0; i < snake_len; i++) {
            unit = snake_path->conn[i].unit;
            pi   = &ssi.snake_pi[snake][i];

            if (opts->verbose) {
                cli_out("\t%02d.%02d(%02d) -> %02d.%02d\n", 
                        pi->src_modid, pi->this_port,
                        pi->added_vlan - SS_VLAN_BASE,
                        pi->dst_modid, pi->to_port);
            }

            rv = lbu_connect_ports(unit, pi, TRUE);
            if (rv < 0) {
                test_error(unit,
                           "Port %s: Failed to connect to %02d.%02d: %s\n",
                           BCM_PORT_NAME(unit, pi->this_port),
                           pi->dst_modid, pi->to_port, bcm_errmsg(rv));
                return (-1);
            }
        }
    }

    return (0);
}

STATIC void
ss_io_units_find(system_snake_opts_t *opts)
{
    int                   unit;
    int                   snake;
    system_snake_path_t  *snake_path;
    int                   num_packets;
    int                   sysid = opts->sysid;
    system_snake_list_t  *snake_list = ss_snake_table[sysid];
    int                   snake_num  = ss_num_paths[sysid];

    SS_UNIT_ITER(sysid, unit) {
        num_packets = 0;
        for (snake = 0; snake < snake_num; snake++) {
            snake_path =  snake_list[snake].snake_path;
            /* Check if the snakes originates at the given unit */
            if (snake_path->conn[0].unit == unit) {
                num_packets += SS_PACKETS_PER_PATH(snake_path);
            }
        }

        ssi.num_packets[unit] = num_packets;
 
        if (num_packets == 0) {
            continue;
        }

        ssi.io_unit[ssi.io_unit_num++] = unit;
    }

    if (opts->verbose) {
        cli_out("%d units found\n", ssi.io_unit_num);
    }
}

STATIC int
ss_packet_alloc(system_snake_opts_t *opts, int unit)
{
    int num_packets = ssi.num_packets[unit];

    /* Allocate packet space */

    if (opts->verbose) {
        cli_out("%d Tx packets for unit %d\n", num_packets, unit);
    }
    
    bcm_pkt_blk_alloc(unit, num_packets,SS_PACKET_LENGTH_MAX,
                      BCM_TX_CRC_REGEN, &ssi.lw[unit].tx_pkts);
    if (!ssi.lw[unit].tx_pkts) {
        cli_out("Unable to allocate tx packet memory\n");
        return BCM_E_MEMORY;
    }
        
    if (opts->verbose) {
        cli_out("%d Rx packets for unit %d\n", num_packets, unit);
    }

    ssi.lw[unit].rx_pkts = sal_alloc(num_packets * sizeof(bcm_pkt_t), 
                                     "SS rx pkts");
    if (!ssi.lw[unit].rx_pkts) {
        bcm_pkt_blk_free(unit, ssi.lw[unit].tx_pkts, num_packets);
        cli_out("Unable to allocate rx packet memory\n");
        return BCM_E_MEMORY;
    }
        
    sal_memset(ssi.lw[unit].rx_pkts, 0, num_packets * sizeof(bcm_pkt_t));
        
    if (opts->verbose) {
        cli_out("%d Tx packet match markers for unit %d\n", num_packets, unit);
    }
        
    ssi.lw[unit].tx_pkt_match = sal_alloc(num_packets * sizeof(int), 
                                          "SS TX match markers");
    if (!ssi.lw[unit].tx_pkt_match) {
        bcm_pkt_blk_free(unit, ssi.lw[unit].tx_pkts, num_packets);
        sal_free(ssi.lw[unit].rx_pkts);
        return BCM_E_MEMORY;
    }
        
    sal_memset(ssi.lw[unit].tx_pkt_match, 0, num_packets * sizeof(int));
    
    return (0);
}

STATIC void
ss_tx_packet_fill(system_snake_opts_t *opts, int unit)
{
    int                   snake;
    lb2s_port_connect_t  *pi;
    int                   length;
    int                   cur_offset;
    uint8                *sequence_p;
    bcm_pkt_t            *pkt;
    enet_hdr_t           *eh;
    int                   pkt_idx    = 0;
    uint8                 seq_no     = SS_SEQUENCE_BASE;
    uint32                pattern    = SS_PATTERN_BASE;
    int                   sysid      = opts->sysid;
    system_snake_list_t  *snake_list = ss_snake_table[sysid];
    int                   snake_num  = ss_num_paths[sysid];
        
    for (snake = 0; snake < snake_num; snake++) {
        if (snake_list[snake].snake_path->conn[0].unit != unit) {
            continue;
        }
        pi = &(ssi.snake_pi[snake][0]);
        for (length = SS_PACKET_LENGTH_MIN; length <= SS_PACKET_LENGTH_MAX; 
             length += snake_list[snake].snake_path->packet_length_inc) {
            pkt = ssi.lw[unit].tx_pkts[pkt_idx];
            pkt->cos = SS_COS;
            /* For now, internal priority matches COS (or v.v.) */
            pkt->prio_int = SS_COS;
            pkt->opcode = BCM_HG_OPCODE_UC;
            pkt->pkt_data[0].len = length;
            BCM_PBMP_CLEAR(pkt->tx_pbmp);
            BCM_PBMP_PORT_ADD(pkt->tx_pbmp, pi->to_port);
            BCM_PBMP_CLEAR(pkt->tx_upbmp);
            BCM_PBMP_PORT_ADD(pkt->tx_upbmp, pi->to_port);

            eh = (enet_hdr_t *)BCM_PKT_DMAC(pkt);
                
            /* Write source/destination MAC address */
            ENET_SET_MACADDR(&eh->en_dhost, pi->dst_mac);
            ENET_SET_MACADDR(&eh->en_shost, pi->src_mac);
                
            /* Write VLAN tag (16 + 16 bits) and sequence number (32 bits) */
            packet_store((uint8 *)&eh->en_tag_tpid, 2, 0x8100 << 16, 0);
            packet_store((uint8 *)&eh->en_tag_ctrl, 2,
                         VLAN_CTRL(0, 0, pi->added_vlan) << 16, 0);
            packet_store((uint8*)eh + 16, 4, seq_no++, 0);

            /*
             * Fill data from after header & seq # to end of packet.  
             * Byte data starts with the pattern MSByte through LSByte 
             * and recycles.
             */
            cur_offset = 20;
            pattern = packet_store((uint8*)eh + cur_offset, 
                                   pkt->pkt_data[0].len - cur_offset,
                                   pattern, SS_PATTERN_INC);

            /* Store the sequence number of the packet */
            sequence_p = (uint8 *)eh + LB2_ID_POS;
            *sequence_p = seq_no;
                
            pkt_idx++;
        }
    }
}

STATIC int
ss_lbu_monitor_start(int unit)
{
    ssi.lw[unit].sema = sal_sem_create("ss-sema", sal_sem_BINARY, 0);
    if (ssi.lw[unit].sema == NULL) {
        return (-1);
    }

    ssi.lw[unit].sema_woke = FALSE;
        
    /* Now that all mems are safely created, fire off the handler */
    /* Let rx handler ignore packets until testing */
    ssi.lw[unit].expect_pkts = FALSE; 
    ssi.lw[unit].rx_pkt_cnt = 0;
    ssi.lw[unit].tx_ppt = ssi.num_packets[unit]; /* For use in callback */
    if (lbu_port_monitor_task(unit, lbu_rx_callback, &(ssi.lw[unit])) < 0) {
        return (-1);
    }
        
    /* Wiring up required data to fake out the lbu subsystem */
    ssi.lw[unit].current_test_type = LB2_TT_SNAKE;
    ssi.lw[unit].set_up = TRUE;
    ssi.lw[unit].cur_params = &(ssi.lw[unit].params[LB2_TT_SNAKE]);
    ssi.lw[unit].cur_params->loopback = LB2_MODE_PHY;

    return(0);
}

STATIC int
system_snake_init(system_snake_opts_t *opts)
{
    int                   unit = 0, rv;
    int                   i;
    bcm_pbmp_t            all_ethernets_pbm, ignore_pbm;
    int                   sysid = opts->sysid;
#ifndef NO_SAL_APPL
    char                 *init_soc_file;
#endif

    if (opts->verbose) {
        cli_out("Initializing system snake\n");
    }

    memset(&ssi, 0, sizeof(ssi));

    SS_UNIT_ITER(sysid, unit) {
        /*
         * Load system defaults from rc.soc
         */

        sh_swap_unit_vars(unit);

#ifndef NO_SAL_APPL
        if (diag_rc_load(unit) != CMD_OK) {
            cli_out("Test: ERROR: RC init script "
                    "for system snake test failed\n");
            return (-1);
        }

        /*
         * Load system snake soc dcript.
         */
        if ((init_soc_file = ss_init_file[sysid]) &&
            sh_rcload_file(unit, NULL, init_soc_file, FALSE) != CMD_OK) {
            cli_out("Test: ERROR: %s init script "
                    "for system snake test failed\n", SS_SOC_SCRIPT);
            return (-1);
        }
#endif

        ssi.stats[unit] =
            sal_alloc(SOC_MAX_NUM_PORTS * sizeof(lb2_port_stat_t),
                      "SS statistics");
        if (!ssi.stats[unit]) {
            cli_out("Unable to allocate statistics memory\n");
            return BCM_E_MEMORY;
        }
        
        /* 
         * Configuring chips for the test 
         */

        if (ss_unit_setup(opts, unit) < 0) {
            cli_out("%s: Unit setup failed\n", SOC_UNIT_NAME(unit));
            return (-1);
        }
    }

    /* 
     * Wait for restarted links to come up on all e-ports on each unit 
     */
    if (opts->mode == SS_MODE_EXT) {
        cli_out("Checking external links... ");
        SS_UNIT_ITER(sysid, unit) {
            BCM_PBMP_ASSIGN(all_ethernets_pbm, pcfg[unit].e); 
            if (soc_property_get_str(unit, spn_SS_IGNORE_PBMP)) {
                ignore_pbm = soc_property_get_bcm_pbmp(unit, spn_SS_IGNORE_PBMP, 0);
                BCM_PBMP_REMOVE(all_ethernets_pbm, ignore_pbm);
            }
            if ((rv = bcm_link_wait(unit,&all_ethernets_pbm, 20000000)) < 0) {
                test_error(unit,
                           "Unit %d: Failed to complete autonegotiation: %s\n"
                           "Check front panel cables\n",
                           unit, bcm_errmsg(rv));
                return(-1);                     /* In case they continue */
            }
        }
        cli_out("up\n");
    }

    /* 
     * Define port interconnections 
     */
    if ((rv = ss_port_interconnect_define(opts)) < 0) {
        test_error(unit,
                   "Failed to define port interconnects: %s\n",
                   bcm_errmsg(rv));
        return(-1);
    }

   /*
    * Execute port interconnections
    */
    if ((rv = ss_port_interconnect_execute(opts)) < 0) {
        test_error(unit,
                   "Failed to execute port interconnects: %s\n",
                   bcm_errmsg(rv));
        return(-1);
    }

    /*
     * Find the units that will be used to inject and extract packets
     */
    ss_io_units_find(opts);

    for (i = 0; i < ssi.io_unit_num; i++) {
        unit = ssi.io_unit[i];

        /*
         * Allocate memory for Tx packets, Rx packets and packet match markers
         */
        if ((rv = ss_packet_alloc(opts, unit)) < 0) {
            test_error(unit,
                       "Failed to allocate Tx or Rx packets: %s\n",
                       bcm_errmsg(rv));
            return (-1);
        }

        /*
         * Fill individual Tx packets 
         */
        ss_tx_packet_fill(opts, unit);

        /*
         * Start loopback monitors
         */
        if ((rv = ss_lbu_monitor_start(unit)) < 0) {
            test_error(unit,
                       "Failed to start loopback monitor thread: %s\n",
                       bcm_errmsg(rv));
            return (-1);
        }
    }

    return 0;
}

static int
system_snake_stats(int unit, bcm_pbmp_t bitmap, lb2_port_stat_t stats[])
{
    /* 
     * NB: We choose "SOC_IS_XGS(unit)" for the third argument so
     * that traffic checking is disabled on fabrics.  This is to
     * prevent complaint on the external loopbacks.
     */

    return lbu_snake_stats(unit, bitmap,
                           (SOC_IS_XGS(unit)) ? LB2_MODE_EXT : LB2_MODE_PHY,
                           stats);
}

int
system_snake_run(system_snake_opts_t *opts)
{
    int                   unit, rv, ix, io_unit;
    int                   i, port;
    int                   timer, sleep_time;
    int                   no_packets;
    lb2s_port_connect_t  *pi;
    int                   snake;
    int                   num_packets;
    int                   sysid = opts->sysid;
    system_snake_list_t  *snake_list = ss_snake_table[sysid];
    int                   snake_num  = ss_num_paths[sysid];

    /* Counter records init */
    SS_UNIT_ITER(sysid, unit) {
        for (ix = 0; ix < SOC_MAX_NUM_PORTS; ix++) {
            ssi.stats[unit][ix].initialized = FALSE;
        }

        BCM_PBMP_ITER(pcfg[unit].port, port) {
            if ((rv = bcm_stat_clear(unit, port)) < 0) {
                test_error(unit,
                           "Could not clear counters: %s\n",
                           bcm_errmsg(rv));
                return -1;
            }
        }

        /* This is the init call */
        if (system_snake_stats(unit, pcfg[unit].port,
                               ssi.stats[unit]) < 0) {
            lbu_snake_dump_stats(unit, pcfg[unit].port, ssi.stats[unit]);
            return (-1);
        }
    }

    /* TX packets */
    if (opts->verbose) {
        cli_out("Transmitting packets\n");
    }
    
    for (i = 0; i < ssi.io_unit_num; i++) {
        io_unit = ssi.io_unit[i];

        num_packets = ssi.num_packets[io_unit];

        if (opts->verbose) {
            cli_out("\t%d packets to unit %d\n", num_packets, io_unit);
        }

        for (ix = 0; ix < num_packets; ix++ ) {
            if ((rv = bcm_tx(io_unit,
                             ssi.lw[io_unit].tx_pkts[ix], NULL)) < 0) {
                cli_out("System snake TX pkt %d: ERROR: bcm_tx: %s\n", 
                        ix, bcm_errmsg(rv));
            }
        }
    }

    cli_out("Running (%d seconds): Speed=%s Mode=%s\n",
            opts->duration, 
            ss_speed_str[opts->speed],
            ss_mode_str[opts->mode]);

    /* Wait, monitor counters */
    timer = 0;
    no_packets = 0;
    while (timer < opts->duration) {
        if ((opts->duration - timer) < opts->interval) 
            sleep_time = opts->duration - timer;
        else
            sleep_time = opts->interval;

        sal_sleep(sleep_time);
        timer += sleep_time;
        cli_out("Time elapsed:  %d seconds %s\n", timer,
                (timer>=opts->duration)?"(end of test)":"");

        SS_UNIT_ITER(sysid, unit) {
            if (system_snake_stats(unit,
                             pcfg[unit].port, ssi.stats[unit]) < 0) {
                no_packets = 1;
                break;
            }
        }
        
        if (no_packets) {
            break;
        }
    }

    if (no_packets) {
        SS_UNIT_ITER(sysid, unit) {
            cli_out("Counters for unit %d (%s)\n", unit, SOC_UNIT_NAME(unit));
            system_snake_stats(unit, pcfg[unit].port, ssi.stats[unit]);
            lbu_snake_dump_stats(unit, pcfg[unit].port, ssi.stats[unit]);
        }

        cli_out("\nSystem snake test FAILED\n");
        
        return (-1);
    }
        
    /* Let the packets out */
    for (snake = 0 ; snake < snake_num; snake++) {
        int return_unit;

        return_unit = io_unit = snake_list[snake].snake_path->conn[0].unit;
        /*
         * For external loopback, we may need to set up the filter on
         * a unit that is different than the one on which the packet was
         * transmitted.
         */
        if (opts->mode == SS_MODE_EXT) {
            return_unit = snake_list[snake].snake_path->return_unit;
        }

        ssi.lw[io_unit].unit = io_unit;
        ssi.lw[io_unit].expect_pkts = TRUE;

        pi = &(ssi.cmic_return_pi[snake]);
        if (opts->verbose) {
            cli_out("Redirect %d to CPU\n", pi->added_vlan - SS_VLAN_BASE);
        }
        if ((rv = lbu_connect_ports(return_unit, pi, TRUE)) < 0) {
            test_error(io_unit,
                       "CPU redirect failure on snake %d: %s\n",
                       snake, bcm_errmsg(rv));
            return -1;
        }

        sal_usleep(SS_INTERPATH_WAIT); /* To let packets drain */
    }
    
    /* Retrieve packets */
    for (i = 0; i < ssi.io_unit_num; i++) {
        io_unit = ssi.io_unit[i];

        if (opts->verbose) {
            cli_out("Retrieving packets from unit %d\n", io_unit);
        }

        if (sal_sem_take(ssi.lw[io_unit].sema, 
                         soc_property_get(io_unit, 
                                          spn_DIAG_LB_PACKET_TIMEOUT, 
                                          5) * SECOND_USEC)) {
            cli_out("\nTime-out waiting for snake completion on unit %d\n",
                    io_unit);
            cli_out("Receive count is %d; expecting %d pkts.\n\n",
                    ssi.lw[io_unit].rx_pkt_cnt, ssi.num_packets[io_unit]);
            
            SS_UNIT_ITER(sysid, unit) {
                system_snake_stats(unit, pcfg[unit].port,
                                   ssi.stats[unit]);
                lbu_snake_dump_stats(unit, pcfg[unit].port,
                                     ssi.stats[unit]);
            }

            ssi.lw[io_unit].sema_woke = FALSE;

            return (-1);
        }
            
        ssi.lw[io_unit].sema_woke = FALSE;    
        ssi.lw[io_unit].expect_pkts = FALSE; /* Let RX handler ignore pkts */
        
        /* Check counters and packets */
        if (opts->verbose) {
            cli_out("Checking packets received from unit %d\n", io_unit);
        }

        if (lbu_snake_analysis(&(ssi.lw[io_unit])) < 0) {
            SS_UNIT_ITER(sysid, unit) {
                if (system_snake_stats(unit, pcfg[unit].port,
                                       ssi.stats[unit]) < 0) {
                    
                }
                lbu_snake_dump_stats(unit, pcfg[unit].port,
                                     ssi.stats[unit]);
            }
            return (-1);
        }
        
        for (ix = 0; ix < ssi.num_packets[io_unit]; ix++) {
            if (ssi.lw[io_unit].tx_pkt_match[ix] == 0) {
                cli_out("\nTX packet %d did not return to CPU\n", (ix + 1));
            }
        }
    }
     
    /*
     * Do the final stats
     */
    if (opts->verbose) {
        SS_UNIT_ITER(sysid, unit) {
            cli_out("Counters for unit %d (%s)\n", unit, SOC_UNIT_NAME(unit));
            system_snake_stats(unit, pcfg[unit].port, ssi.stats[unit]);
            lbu_snake_dump_stats(unit, pcfg[unit].port, ssi.stats[unit]);
        }
    }

    cli_out("Passed\n");

    return 0;
}

STATIC int
system_snake_done(system_snake_opts_t *opts)
{
    int                   unit, ix, i, rv;

    /* Unregister the callback */
    for (ix = 0; ix < ssi.io_unit_num; ix++) {
        unit = ssi.io_unit[ix];

        if (opts->verbose) {
            cli_out("Unregistering Rx callback on unit %d\n", unit);
        }

        if ((rv = bcm_rx_unregister(unit, lbu_rx_callback,
                                    BCM_RX_PRIO_MAX)) < 0) {
            cli_out("Failed to unregister RX handler.\n");
        }

        if (opts->verbose) {
            cli_out("Stopping Rx on unit %d\n", unit);
        }

        if ((rv = bcm_rx_stop(unit, NULL)) < 0) {
            cli_out("system_snake_done: could not stop packet driver: %s\n",
                    bcm_errmsg(rv));
        }

        /* Flush out tx packets */
        if (ssi.lw[unit].tx_pkts) {
            bcm_pkt_blk_free(unit, ssi.lw[unit].tx_pkts, 
                             ssi.num_packets[unit]);
            ssi.lw[unit].tx_pkts = NULL;
        }

        if (ssi.lw[unit].tx_pkt_match) {
            sal_free(ssi.lw[unit].tx_pkt_match);
            ssi.lw[unit].tx_pkt_match = NULL;
        }

        /* Flush out rx packets */
        if (ssi.lw[unit].rx_pkts) {
            for (i = 0; i < ssi.num_packets[unit]; i++) {
                if (ssi.lw[unit].rx_pkts[i]._pkt_data.data) {
                    bcm_rx_free(unit, ssi.lw[unit].rx_pkts[i]._pkt_data.data);
                }
            }
            sal_free(ssi.lw[unit].rx_pkts);
            ssi.lw[unit].rx_pkts = NULL;
        }
        
        if (ssi.lw[unit].sema) {
            sal_sem_destroy(ssi.lw[unit].sema);
            ssi.lw[unit].sema = NULL;
        }

        if (ssi.stats[unit]) {
            sal_free(ssi.stats[unit]);
            ssi.stats[unit] = NULL;
        }
    }

    return 0;
}

int
system_snake_test(system_snake_opts_t *opts)
{
    int           rv, d_rv;

    /* Configure system */
    if ((rv = system_snake_init(opts)) < 0) {
        goto cleanup;
    }

    /* Transmit packets, wait, check */
    rv = system_snake_run(opts);

 cleanup:
    /* Clean up data structures */
    if ((d_rv = system_snake_done(opts)) < 0) {
        return d_rv;
    }

    return (rv < 0) ? rv : d_rv;
}

char cmd_ss_usage[] =
"ss [Duration=<secs>] [CheckInterval=<secs>]\n\t"
"   [Mode=MAC|PHY|EXT] [Speed=MAX|10|100|1000|2500|10G]\n\t"
"Test system with snaked packets.\n\t"
"This test is supported only for certain platforms.\n\t"
"HiGig cables must be looped back in pairs.\n\t"
"Defaults: Duration=10, CheckInterval=5, Mode=PHY, "
    "Speed=DEFAULT(from rc.soc).\n" ;

cmd_result_t
cmd_ss(int unit, args_t *args)
{
    system_snake_opts_t opts;
    parse_table_t       pt;
    int                 sysid, rv;

    /*
     * Make sure that our current hardware is in the
     * list of platforms that support the ss command.
     */

    for (sysid = 0; sysid < SS_SYSID_NUM; sysid++) {
        if (soc_cm_config_var_get(unit, ss_sysid_str[sysid])) {
            break;
        }
    }

    if (sysid == SS_SYSID_NUM) {
        cli_out("This test only runs on certain platforms.\n");
        cli_out("One of the following config variables must be\n");
        cli_out("set to indicate the platform:\n");
        for (sysid = 0; sysid < SS_SYSID_NUM; sysid++) {
            cli_out("   %s\n", ss_sysid_str[sysid]);
        }

        return CMD_FAIL;
    }

    /* cli_out("%s baseboard detected\n",ss_sysid_str[sysid]); */

    opts.got_herc = TRUE;
    opts.sysid = sysid;

    /* Default values */
    opts.duration = SS_DURATION;
    opts.interval = SS_INTERVAL;
    opts.speed = SS_SPEED_DEFAULT;
    if (sysid ==  SS_SYSID_FIREBOLT) {
        opts.mode = SS_MODE_EXT;
    } else {
        opts.mode = SS_MODE_PHY;
    }
    opts.verbose = FALSE;
    opts.pli = -1;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Duration", PQ_INT|PQ_DFL, 0,
                    &opts.duration, NULL);
    parse_table_add(&pt, "CheckInterval", PQ_INT|PQ_DFL, 0,
                    &opts.interval, NULL);
    parse_table_add(&pt, "Speed", PQ_MULTI|PQ_DFL, 0,
                    &opts.speed, ss_speed_str);
    parse_table_add(&pt, "Mode", PQ_MULTI|PQ_DFL, 0,
                    &opts.mode, ss_mode_str);
    parse_table_add(&pt, "Verbose", PQ_BOOL|PQ_DFL, 0,
                    &opts.verbose, NULL);
    parse_table_add(&pt, "LengthIncrement", PQ_INT|PQ_DFL, 0,
                    &opts.pli, NULL);

    if (parse_arg_eq(args, &pt) < 0) {
        parse_arg_eq_done(&pt);
        return(CMD_USAGE);
    }

    if (ARG_CNT(args) > 0) {
        cli_out("%s: Unrecognized extra argument: %s\n",
                ARG_CMD(args), ARG_CUR(args));
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    parse_arg_eq_done(&pt);

    /*
     * Police any cmd line parameters
     */

    if (sysid ==  SS_SYSID_FIREBOLT) {
        if (opts.mode != SS_MODE_EXT) {
            cli_out("%s Mode not supported on %s platform. Use tr 39 instead.\n",
                    ss_mode_str[opts.mode], ss_sysid_str[sysid]); 
            return CMD_FAIL;
        }
    }

    if (opts.duration < opts.interval) {
        opts.interval = opts.duration;
    }

    if (opts.speed == SS_SPEED_MAX) {
        opts.speed = SS_SPEED_DEFAULT;
    }

    if (opts.pli > 0) {
        ss_set_packet_length_increment(&opts);
    }

    rv = system_snake_test(&opts);

    /*
     * Restore current unit since system_snake_test() changes it.
     */

    sh_swap_unit_vars(unit);

    return rv;
}

#endif  /* INCLUDE_TEST */

typedef int _diag_system_snake_not_empty; /* Make ISO compilers happy. */
