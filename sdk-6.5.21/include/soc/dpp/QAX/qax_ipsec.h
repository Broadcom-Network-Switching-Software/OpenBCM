/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    qax_ipsec.h
 * Purpose: Manages IPSec block
 */
#ifndef _QAX_IPSEC_H
#define _QAX_IPSEC_H


#include <bcm/ipsec.h>
#include <soc/types.h>


#define BCM_IPSEC_MAX_NUM_OF_IPSEC_TUNNEL       (384)
#define BCM_IPSEC_MAX_NUM_OF_SA                 (384)
#define BCM_IPSEC_MAX_NUM_OF_BFD                (768)

#define IPSEC_TUNNEL_ID_INVALID                 (0xFFFFFFFF)
#define IPSEC_SA_ID_INVALID                     (0xFFFFFFFF)


#define SOC_QAX_IPSEC_ID_IN_RANGE_CHECK(id)     if (((id) < (0)) || ((id) > (BCM_IPSEC_MAX_NUM_OF_SA)))  return SOC_E_LIMIT;



#define IPSEC_ACCESS  sw_state_access[unit].dpp.soc.qax.ipsec


typedef enum soc_qax_ipsec_program_type_e {
    Ipsec_Bypass                                 = 0,      
    Ipsec_IPV4_Encryption_AES128_Auth_Sha1       = 1,
    Ipsec_IPV4_Decryption_AES128_Auth_Sha1       = 2,
    Ipsec_IPV4_Encryption_AES256_Auth_Sha2       = 3,
    Ipsec_IPV4_Decryption_AES256_Auth_Sha2       = 4,
    Ipsec_Program_Type_Count                     = 5       
} soc_qax_ipsec_program_type_t;

#define soc_qax_ipsec_program_type_name { \
    "Bypass", \
    "Encryption_IPV4_AES128_Auth_Sha1",   \
    "Decryption_IPV4_AES128_Auth_Sha1",   \
    "Encryption_IPV4_AES256_Auth_Sha256", \
    "Decryption_IPV4_AES256_Auth_Sha256"  \
}


#define soc_qax_ipsec_algo_type_name { \
    "NONE",    \
    "AES128",  \
    "AES256"   \
}


#define soc_qax_ipsec_auth_type_name { \
    "NONE",    \
    "SHA1",    \
    "SHA256"   \
}


#define soc_qax_ipsec_direction { \
    "ENCRYPTION", \
    "DECRYPTION"  \
}


typedef struct soc_qax_sw_state_ipsec
{
    soc_port_t                  ipsec_port;
    uint32                      num_of_ipsec_channels;
    uint32                      num_of_bfd_channels;
    uint32                      sa_to_tunnel_map[BCM_IPSEC_MAX_NUM_OF_SA];
    bcm_ipsec_sa_info_t         sa[BCM_IPSEC_MAX_NUM_OF_SA];
    bcm_ipsec_tunnel_info_t     tunnel[BCM_IPSEC_MAX_NUM_OF_IPSEC_TUNNEL];
}soc_qax_sw_state_ipsec_t;



int soc_qax_ipsec_init(int unit);
int soc_qax_ipsec_config_set( int unit, bcm_ipsec_config_t *config);
int soc_qax_ipsec_tunnel_create( int unit, uint32 flags, uint32 *tunnel_id, const bcm_ipsec_tunnel_info_t *tunnel_info);
int soc_qax_ipsec_tunnel_delete( int unit, uint32 flags, uint32 tunnel_id);
int soc_qax_ipsec_sa_create(  int unit, uint32 flags, uint32 *spi_id, bcm_ipsec_sa_info_t *sa);
int soc_qax_ipsec_sa_delete(  int unit, uint32 flags, uint32 sa_id);
int soc_qax_ipsec_sa_key_update( int unit, uint32 flags, uint32 spi_id, bcm_ipsec_sa_keys_t *sa_keys);
int soc_qax_ipsec_sa_to_tunnel_map_set(int unit, uint32 tunnel_id, uint32 sa_id);
int soc_qax_ipsec_sa_to_tunnel_map_get(int unit, uint32 sa_id, uint32 *tunnel_id);
int soc_qax_ipsec_diag_sa( int unit, uint32 sa_id);
int soc_qax_ipsec_diag_traffic_info( int unit);

#endif 
