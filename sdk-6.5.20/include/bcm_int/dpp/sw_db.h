/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
 */
#ifndef _BCM_DPP_SW_DB_H_
#define _BCM_DPP_SW_DB_H_

#include <bcm_int/dpp/vlan.h>
#include <bcm/switch.h>
#include <bcm/module.h>


#ifdef BCM_ARAD_SUPPORT
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#endif

#define _BCM_DPP_L3_NOF_FECS(__unit) (SOC_DPP_CONFIG(__unit)->l3.fec_db_size)

typedef struct dos_attack_info_s {
  uint8 tocpu;
  /* flags for all dos attack traps */
  uint8 sipequaldip;
  uint8 mintcphdrsize;
  uint8 v4firstfrag;
  uint8 tcpflags;
  uint8 l4port;
  uint8 tcpfrag;
  uint8 icmp;
  uint8 icmppktoversize;
  uint8 macsaequalmacda;
  uint8 icmpv6pingsize;
  uint8 icmpfragments;
  uint8 tcpoffset;
  uint8 udpportsequal;
  uint8 tcpportsequal;
  uint8 tcpflagssf;
  uint8 tcpflagsfup;
  uint8 tcphdrpartial;
  uint8 pingflood;
  uint8 synflood;
  uint8 tcpsmurf;
  uint8 tcpxmas;
  uint8 l3header;
} dos_attack_info_t;

typedef struct dpp_switch_sw_db_s {
  dos_attack_info_t dos_attack; 
} bcm_dpp_switch_sw_db_t;

typedef struct fec_to_ecmps_s {
  int *ecmps;  
  int *corresponding_fecs;
  int nof_ecmps;  
} fec_to_ecmps_t;

typedef struct fec_copy_info_s {
  int ecmp;  
  int real_fec;
} fec_copy_info_t;


typedef struct ecmp_size_s {
  int cur_size;
  int max_size;
} ecmp_size_t;

typedef struct multi_device_sync_flags_s {
  int eep;
  int inlif;  
} multi_device_sync_flags_t;


typedef struct bcm_dpp_l3_sw_db_s {
  fec_to_ecmps_t *fec_to_ecmp; /* for real-FEC store copy-FEC and their ECMPs */
  ecmp_size_t *ecmp_size; /* size of ECMP and max size */
  uint8 *ecmp_successive; /* [y] == 1 --> ECMP y is successsive*/
  uint8 *ecmp_protected; /* [y] == 1 --> ECMP y  is protected */
  fec_copy_info_t  *fec_copy_info; /* for copy-fec, store real FEC and my ECMP*/
} bcm_dpp_l3_sw_db_t;

typedef struct bcm_dpp_mirror_sw_db_s {
  int mirror_mode;
  uint8 egress_port_profile[SOC_PPC_MAX_NOF_LOCAL_PORTS_ARAD];
}bcm_dpp_mirror_sw_db_t;

extern int _bcm_dpp_init_finished_ok[BCM_MAX_NUM_UNITS];
extern int _cell_id_curr[BCM_MAX_NUM_UNITS];
extern bcm_dpp_l3_sw_db_t _l3_sw_db[BCM_MAX_NUM_UNITS];
extern bcm_dpp_switch_sw_db_t _switch_sw_db[BCM_MAX_NUM_UNITS];
extern bcm_dpp_mirror_sw_db_t _mirror_sw_db[BCM_MAX_NUM_UNITS];

int 
_bcm_sw_db_switch_urpf_mode_set(int unit, int urpf_mode);

int 
_bcm_sw_db_switch_urpf_mode_get(int unit, int *urpf_mode);

int 
_bcm_sw_db_switch_dos_attack_info_set(int unit, bcm_switch_control_t bcm_type, int enable);

int 
_bcm_sw_db_switch_dos_attack_info_get(int unit, bcm_switch_control_t bcm_type, int *enable);















int
bcm_sw_db_l3_fec_to_ecmps_get(int unit, int fec, fec_to_ecmps_t *fec_to_ecmps); 

/* given copy FEC return pointer to real FEC and used ECMP */
int
bcm_sw_db_l3_copy_fec_to_ecmps_get(int unit, int l3a_intf, fec_copy_info_t *fec_copy_info);

int
bcm_sw_db_l3_add_fec_mapping_to_ecmp(int unit,uint32 flags, int intf, int ecmp, int cor_fec);

int
bcm_sw_db_l3_fec_remove_fec(int unit, int intf);

int
bcm_sw_db_l3_set_ecmp_sizes(int unit, int mpintf, int max_size, int curr_size);

int
bcm_sw_db_l3_get_ecmp_sizes(int unit, int mpintf, int *max_size, int *curr_size);

int
bcm_sw_db_l3_set_ecmp_is_successive(int unit, int mpintf, uint8 successive);

int
bcm_sw_db_l3_get_ecmp_is_successive(int unit, int mpintf, uint8 *successive);

int
bcm_sw_db_l3_set_ecmp_is_protected(int unit, int mpintf, uint8 protected);

int
bcm_sw_db_l3_get_ecmp_is_protected(int unit, int mpintf, uint8 *protected);


int
bcm_sw_db_l3_dump_fec_to_ecmps(int unit, int start, int end);

int 
_bcm_sw_db_init(int unit);

int 
_bcm_sw_db_deinit(int unit);

int
_bcm_sw_db_cell_id_curr_get(int unit, int *cell_id);

int
_bcm_sw_db_cell_id_curr_set(int unit, int *cell_id);

int
_bcm_sw_db_petra_mirror_mode_set(int unit, int mode);

int
_bcm_sw_db_petra_mirror_mode_get(int unit, int *mode);

int
_bcm_sw_db_outbound_mirror_port_profile_set(int unit, int port, uint8 profile);

int
_bcm_sw_db_outbound_mirror_port_profile_get(int unit, int port, uint8 *profile);

#endif
