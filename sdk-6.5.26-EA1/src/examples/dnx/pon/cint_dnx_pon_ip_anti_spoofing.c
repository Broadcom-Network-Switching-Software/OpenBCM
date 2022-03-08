/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
*
* File: cint_dnx_pon_ip_anti_spoofing.c
* Purpose: An example of PON IP anti-spoofing:
    - Setting ports as NNI ports and PON ports
    - Creating PON 1:1 STC/DTC Model, N:1 STC/DTC Model
    - Enable PON Lif anti-spoofing
    - Setting PON LIF anti-spoofing entry
    - Creating PMF entry to handle packets which do not hit the entry
*
* Calling sequence:
*
* 1:1 STC service IP anti-spoofing test:
* Step 1. create 1:1 STC service: 
*         pon_1_1_service_stc_mode_init();pon_1_1_service
* Step 2. enable anti-spoofing based on gport:
*         pon_service_ip_anti_spoofing_enable(0,start_index,end_index,1,0)
* Step 3. add anti-spoofing entry:
*         DHCP entry:  pon_service_ip_anti_spoofing_set(0,start_index,end_index,1,1,0)
*         STATIC entry:  pon_service_ip_anti_spoofing_set(0,start_index,end_index,1,0,0)
* Step 4. create PMF to drop anti-spoofing non-hit packets, and forward hit packets: 
*         cint_sav_field_config_ipmf1(0,0,1);cint_sav_field_set_context_ipmf1(0,start_index,end_index,2,0);
* Step 5. send 1:1 servece STC upstream, if hit SAV entry, pass; or ,drop
*
* 1:1 DTC service IP anti-spoofing test:
* Step 1. create 1:1 DTC service: 
*         pon_1_1_service_mode_init();pon_1_1_service
* Step 2. enable anti-spoofing based on gport:
*         pon_service_ip_anti_spoofing_enable(0,start_index,end_index,1,0)
* Step 3. add anti-spoofing entry:
*         DHCP entry:  pon_service_ip_anti_spoofing_set(0,start_index,end_index,1,1,0)
*         STATIC entry:  pon_service_ip_anti_spoofing_set(0,start_index,end_index,1,0,0)
* Step 4. create PMF to drop anti-spoofing non-hit packets, and forward hit packets: 
*         cint_sav_field_config_ipmf1(0,0,1);cint_sav_field_set_context_ipmf1(0,start_index,end_index,2,0);
* Step 5. send 1:1 servece DTC upstream, if hit SAV entry, pass; or ,drop
*
* N:1 STC service IP anti-spoofing test:
* Step 1. create N:1 STC service: 
*         pon_n_1_service_stc_mode_init();pon_n_1_service
* Step 2. enable anti-spoofing based on gport:
*         pon_service_ip_anti_spoofing_enable(0,start_index,end_index,1,1)
* Step 3. add anti-spoofing entry:
*         DHCP entry:  pon_service_ip_anti_spoofing_set(0,start_index,end_index,1,1,1)
*         STATIC entry:  pon_service_ip_anti_spoofing_set(0,start_index,end_index,1,0,1)
* Step 4. create PMF to drop anti-spoofing non-hit packets, and forward hit packets: 
*         cint_sav_field_config_ipmf1(0,1,1);cint_sav_field_set_context_ipmf1(0,start_index,end_index,2,1);
* Step 5. send N:1 servece STC upstream, if hit SAV entry, pass; or ,drop
*
* N:1 DTC service IP anti-spoofing test:
* Step 1. create N:1 DTC service: 
*         pon_n_1_service_mode_init();pon_n_1_service
* Step 2. enable anti-spoofing based on gport:
*         pon_service_ip_anti_spoofing_enable(0,start_index,end_index,1,1)
* Step 3. add anti-spoofing entry:
*         DHCP entry:  pon_service_ip_anti_spoofing_set(0,start_index,end_index,1,1,1)
*         STATIC entry:  pon_service_ip_anti_spoofing_set(0,start_index,end_index,1,0,1)
* Step 4. create PMF to drop anti-spoofing non-hit packets, and forward hit packets: 
*         cint_sav_field_config_ipmf1(0,1,1);cint_sav_field_set_context_ipmf1(0,start_index,end_index,2,1);
* Step 5. send N:1 service DTC upstream, if hit SAV entry, pass; or ,drop
*
* Test Scenario 
*
* ./bcm.user
* cd ../../../../regress/bcm
* cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
* cint ../../src/./examples/sand/utility/cint_sand_utils_vlan.c
* cint ../../src/./examples/dnx/pon/cint_dnx_pon_utils.c
* cint ../../src/./examples/dnx/pon/cint_dnx_pon_application.c
* cint ../../src/./examples/dnx/pon/cint_dnx_pon_ip_anti_spoofing.c
* cint
* 1:1 STC service IP anti-spoofing test:
* pon_1_1_service_stc_mode_init(0,201,202);
* pon_1_1_service(0,0);
* exit;
*
* cint
* pon_service_ip_anti_spoofing_enable(0,0,2,1,0);
* pon_service_ip_anti_spoofing_set(0,0,2,1,0,0);
* pon_service_ip_anti_spoofing_set(0,0,2,1,1,0);
* cint_sav_field_config_ipmf1(0,0,1);
* cint_sav_field_set_context_ipmf1(0,0,2,2,0);
* exit;
*
*  Sending IPV4 STATIC hit packet,forward to nni port 
* tx 1 psrc=201 data=0x0000aaaa000c00020001810003e8080045000035000000008000f8abc01a03017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
*
*  Received packets on unit 0 should be: 
*  Data: 0x00000000aaaa000c000200019100400a080045000035000000008000f8abc01a03017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
*
*  Sending IPV4 STATIC non-hit packet to nni port, trap  
* tx 1 psrc=201 data=0x0000aaaa000c00020001810003e8080045000035000000008000f7e4c01a03c87fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
*
*  No  packets received on unit 0 
* 
* 1:1 DTC service IP anti-spoofing test:
* cint
* pon_1_1_service_init(0,201,202);
* pon_1_1_service(0,1);
* pon_service_ip_anti_spoofing_enable(0,0,2,1,0);
* pon_service_ip_anti_spoofing_set(0,0,2,1,0,0);
* pon_service_ip_anti_spoofing_enable(0,6,6,1,0);
* pon_service_ip_anti_spoofing_set(0,6,6,1,0,0);
* pon_service_ip_anti_spoofing_set(0,0,2,1,1,0);
* pon_service_ip_anti_spoofing_set(0,6,6,1,1,0);
* cint_sav_field_config_ipmf1(0,0,1);
* cint_sav_field_set_context_ipmf1(0,0,2,2,0);
* cint_sav_field_set_context_ipmf1(0,6,6,2,0);
* exit;
*
*  Sending IPV4 STATIC hit packet,forward to nni port 
* tx 1 psrc=201 data=0x0000aaaa000c00020001810003e8080045000035000000008000f8abc01a03017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
*
*  Received packets on unit 0 should be: 
*  Source port: 0, Destination port: 0 
*  Data: 0x00000000aaaa000c000200019100400a080045000035000000008000f8abc01a03017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
*
*  Sending IPV4 STATIC non-hit packet to nni port, trap  
* tx 1 psrc=201 data=0x0000aaaa000c00020001810003e8080045000035000000008000f7e4c01a03c87fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
*
* No  packets Received on unit 0.
*
*  Sending packet from PON port (untag) to NNI port (stag) 
* tx 1 psrc=201 data=0x0000aaaa000c000201dd810003e886dd600000000021068020010db800000000021122fffe3344550000000000000000123400000000ff13000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
*
*  Received packets on unit 0 should be: 
*  Data: 0x00000000aaaa000c000201dd9100600a86dd600000000021068020010db800000000021122fffe3344550000000000000000123400000000ff13000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20 
*
*  Sending packet from PON port (untag) to NNI port (stag) 
* tx 1 psrc=201 data=0x0000aaaa000c000201dd810003e886dd600000000021068020010db800000023021122fffe3344550000000000000000123400000000ff13000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
*
* No  packets Received on unit 0.
* 
* N:1 STC service IP anti-spoofing test:
* cint
* pon_n_1_service_stc_mode_init(0,201,202);
* pon_n_1_service(0,0);
* pon_service_ip_anti_spoofing_enable(0,0,2,1,1);
* pon_service_ip_anti_spoofing_set(0,0,2,1,0,1);
* pon_service_ip_anti_spoofing_set(0,0,2,1,1,1);
* cint_sav_field_config_ipmf1(0,1,1);
* cint_sav_field_set_context_ipmf1(0,0,2,2,1);
* exit;
*
*  Sending packet from PON port (untag) to NNI port (stag)
* tx 1 psrc=201 data=0x0000aaaa000c00020001810003e8080045000035000000008000f8abc01a03017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
*
*  Received packets on unit 0 should be: 
*  Source port: 0, Destination port: 0 
*  Data: 0x00000000aaaa000c00020001910043208100032a080045000035000000008000f8abc01a03017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
*
*  Sending packet from PON port (untag) to NNI port (stag)
* tx 1 psrc=201 data=0x0000aaaa000c00020001810003e8080045000035000000008000f7e4c01a03c87fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
*
*  No packet received  on unit 0
*
* N:1 DTC service IP anti-spoofing test:
* cint
* pon_n_1_service_init(0,201,202);
* pon_n_1_service(0,1);
* pon_service_ip_anti_spoofing_enable(0,0,2,1,1);
* pon_service_ip_anti_spoofing_set(0,0,2,1,0,1);
* pon_service_ip_anti_spoofing_set(0,0,2,1,1,1);
* cint_sav_field_config_ipmf1(0,1,1);
* cint_sav_field_set_context_ipmf1(0,0,2,2,1);
* exit;
*
*  Sending packet from PON port (untag) to NNI port (stag)
* tx 1 psrc=201 data=0x0000aaaa000c00020001810003e8080045000035000000008000f8abc01a03017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
*
*  Received packets on unit 0 should be: 
*  Source port: 0, Destination port: 0 
*  Data: 0x00000000aaaa000c00020001910043208100032a080045000035000000008000f8abc01a03017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
*  
*  Sending packet from PON port (untag) to NNI port (stag)
* tx 1 psrc=201 data=0x0000aaaa000c00020001810003e8080045000035000000008000f7e4c01a03c87fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
*
*  No packet received  on unit 0
*/

int is_sav_pmf_config = 0;
int in_lif_profile_sav = 0;

bcm_field_group_t sav_hit_fg_id = 0;
bcm_field_group_t sav_default_fg_id = 0;
int default_ent_id=0;
int trap_id = -1;
bcm_field_group_t sav_inlif_profile_cs_fg_id = BCM_FIELD_ID_INVALID;
bcm_field_entry_t sav_inlif_profile_cs_entry_id = BCM_FIELD_ID_INVALID;
bcm_field_context_t sav_inlif_profile_cs_context_id = BCM_FIELD_ID_INVALID;
bcm_field_presel_t  sav_inlif_profile_cs_presel_id = 50;

/*
 * Set up ip_anti_spoofing_set for PON service.
 * enable: 1  disable: 0
 */

int pon_service_ip_anti_spoofing_set(int unit,int start_index,int end_index,uint32 add,int is_dhcp,int is_n1)
{
    int rv;
    uint32 flags;
    bcm_gport_t pon_gport;
    bcm_l3_source_bind_t info;
    uint8 ip6_add[16]= { 0x20, 0x01, 0x0D, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, 0x22, 0xFF, 0xFE, 0x33, 0x44, 0x55 };    
    uint8 ip6_adr_mask[16] = { 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF };
    bcm_mac_t intf_in_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };  
    bcm_mac_t mac_all_0 = {0};
    int i = 0;
    uint32 index = 0;


    for (index = start_index; index <= end_index; index ++)
    {
        intf_in_mac_address[4] = index;
        ip6_add[1] = 0x1+index;
        ip6_add[6] = index;
        if (is_n1)
        {
            pon_gport = pon_n_1_service_info[index].pon_gport;
        }
        else
        {
            pon_gport = pon_1_1_service_info[index].pon_gport;
        }
        bcm_l3_source_bind_t_init(&info);

        info.ip = 0xc01a0301 + index;/*192.26.3.1*/
        info.ip_mask = 0xffffffff;

        sal_memcpy(info.ip6, ip6_add, 16);
        sal_memcpy(info.ip6_mask, ip6_adr_mask, 16);
        if(is_dhcp)
        {
            printf("intf_in_mac_address = :");
            for ( i=0; i<6; i++)
            {
                printf("%d = - ",intf_in_mac_address[i]);
            }
            printf("\n");

            sal_memcpy(info.mac, intf_in_mac_address, 6);
        }
        else
        {
            sal_memcpy(info.mac, mac_all_0, 6);
        }

        info.port = pon_gport;

        if (add)
        {
            /*ipv4*/
            rv = bcm_l3_source_bind_add(unit,&info);
            if (rv != BCM_E_NONE)
            {
                printf("bcm_l3_source_bind_add ipv4 for pon index %d failed, rv = %d!\n",index, rv);
            }
            /*ipv6*/
            info.flags = BCM_L3_SOURCE_BIND_IP6;
            rv = bcm_l3_source_bind_add(unit,&info);
            if (rv != BCM_E_NONE)
            {
                printf("bcm_l3_source_bind_add ipv6 for pon index %d failed, rv = %d!\n",index, rv);
            }
        }
        else
        {
            /*ipv4*/
            rv = bcm_l3_source_bind_delete(unit,&info);
            if (rv != BCM_E_NONE)
            {
                printf("bcm_l3_source_bind_delete ipv4 for pon index %d failed, rv = %d!\n",index, rv);
            }
            /*ipv6*/
            info.flags = BCM_L3_SOURCE_BIND_IP6;
            rv = bcm_l3_source_bind_delete(unit,&info);
            if (rv != BCM_E_NONE)
            {
                printf("bcm_l3_source_bind_delete ipv6 for pon index %d failed, rv = %d!\n",index, rv);
            }
        }
    }


    if(util_verbose) {
        printf("pon_service_ip_anti_spoofing_set:\n");
        print info;
    }
    return rv;
}

/*
 * Set up ip_anti_spoofing_set for PON service.
 * enable: 1  disable: 0
 */

int pon_service_ip_anti_spoofing_enable(int unit, int start_index, int end_index, uint32 enable,int is_n1)
{
    int rv;
    uint32 flags;
    bcm_gport_t pon_gport;
    bcm_vlan_port_t vlan_port;
    uint32 i = 0;

    printf("pon_service_ip_anti_spoofing_enable from index[%d - %d].\n", start_index, end_index);
    for(i = start_index; i <= end_index; i++)
    {
        printf("i=%d!\n",i);
        if (is_n1)
        {
            pon_gport = pon_n_1_service_info[i].pon_gport;
        }
        else
        {
            pon_gport = pon_1_1_service_info[i].pon_gport;
        }
        rv = bcm_l3_source_bind_enable_set(unit, pon_gport, enable);
        if (rv != BCM_E_NONE)
        {
            printf("pon_service_ip_anti_spoofing_set index=%d failed! %s\n", i,bcm_errmsg(rv));
        }
        if (is_n1)
        {
            pon_n_1_service_info[i].ip_anti_spoofing_enable = enable;
        }
        else
        {
            pon_1_1_service_info[i].ip_anti_spoofing_enable = enable;
        }

    }
    in_lif_profile_sav = 2;

    return rv;
}


int sav_field_data_clean(int unit, bcm_field_context_t context_id)
{
    int rv = 0;

    rv = bcm_field_entry_delete(unit, sav_default_fg_id, NULL, default_ent_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_entry_delete\n");
        return rv;
    }

    rv = bcm_field_group_context_detach(unit, sav_default_fg_id, context_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_context_detach\n");
        return rv;
    }
    rv = bcm_field_group_delete(unit, sav_default_fg_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_delete\n");
        return rv;
    }

    rv = cint_sav_field_inlif_profile_cs_ctx_destroy(unit);
    if (rv != BCM_E_NONE) {
        printf("Error in cint_sav_field_inlif_profile_cs_ctx_destroy\n");
        return rv;
    }


    is_sav_pmf_config=0;

    return rv;
}



int sav_wide_data_general_create_udf_qual(int unit, bcm_field_qualify_t *udf_qual_id, char *qual_info)
{
    int rv = BCM_E_NONE;
    char qual_info_tmp[100];
    bcm_field_qualifier_info_create_t udf_qual_info;
    void *dest_char;

    bcm_field_qualifier_info_create_t_init(&udf_qual_info);

    udf_qual_info.size = 8;
    snprintf(qual_info_tmp, 100, "UDF Qual(%s): %s", "8lsb", qual_info);

    dest_char = &udf_qual_info.name[0];
    sal_strncpy_s(dest_char, qual_info_tmp, sizeof(udf_qual_info.name));

    rv = bcm_field_qualifier_create(unit, 0, &udf_qual_info, udf_qual_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_field_qualifier_create for %s\n", qual_info);
        return rv;
    }
    return rv;
}

int
cint_sav_field_inlif_profile_cs_ctx_create(
    int unit,
    int profile)
{
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    void * dest_char;
    int rv = BCM_E_NONE;

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "antispoofing_inlif_profile", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &sav_inlif_profile_cs_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }
    printf("Created context (%d)  \n", sav_inlif_profile_cs_context_id);

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = sav_inlif_profile_cs_presel_id;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = sav_inlif_profile_cs_context_id;
    p_data.nof_qualifiers = 1;
    p_data.qual_data[0].qual_type = bcmFieldQualifyInVportClass;
    p_data.qual_data[0].qual_value = profile;
    p_data.qual_data[0].qual_mask = 0xf;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in dnx_field_presel_set \n", rv);
        return rv;
    }

    printf("Presel (%d) was configured for context(%d) \n", sav_inlif_profile_cs_presel_id, sav_inlif_profile_cs_context_id);

    return rv;
}


int
cint_sav_field_inlif_profile_cs_ctx_destroy(
    int unit)
{
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    void * dest_char;
    int rv = BCM_E_NONE;

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = sav_inlif_profile_cs_presel_id;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = FALSE;
    p_data.context_id = sav_inlif_profile_cs_context_id;
    p_data.nof_qualifiers = 0;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in dnx_field_presel_set \n", rv);
        return rv;
    }

    printf("Presel (%d) is cleared for context(%d) \n", sav_inlif_profile_cs_presel_id, sav_inlif_profile_cs_context_id);

    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, sav_inlif_profile_cs_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }
    printf("destroy context (%d)  \n", sav_inlif_profile_cs_context_id);

    return rv;
}


int cint_sav_field_set_context_ipmf1(int unit, int start_index, int end_index, int in_lif_profile,int is_n_1)
{
    bcm_gport_t pon_gport=0;
    int i=0;
    int rv = 0;
    for (i =start_index; i<=end_index; i++)
    {
        if(is_n_1)
        {
            pon_gport = pon_n_1_service_info[i].pon_gport;
        }
        else
        {
            pon_gport = pon_1_1_service_info[i].pon_gport;
        }

        rv = bcm_port_class_set(unit, pon_gport, bcmPortClassFieldIngressVport, in_lif_profile);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_port_class_set\n", rv);
            return rv;
        }
    }
    return rv;

}

int cint_sav_field_config_ipmf1(int unit, int is_n_1, int is_add)
{
    int rv = BCM_E_NONE;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    int nof_quals = 3;
    int nof_actions = 1;
    int ii = 0;
    void *dest_char;
    bcm_field_entry_info_t entry_info;
    bcm_field_entry_t entry_handle;
    bcm_field_context_info_t context_info;
    bcm_field_context_param_info_t param_info;
    bcm_gport_t trap_gport_drop;
    bcm_field_qualifier_info_get_t pwelif_qual_info_get;
    bcm_field_qualifier_info_get_t pcchit_qual_info_get;
    int trap_type = bcmRxTrapUserDefine;
    bcm_rx_trap_config_t trap_config;


    if(!is_add)
    {
        if (is_sav_pmf_config) {
            rv = sav_field_data_clean(unit, sav_inlif_profile_cs_context_id);
            if (rv != BCM_E_NONE) {
                printf("Error in sav_field_data_clean\n");
                return rv;
            }
        }
        return BCM_E_NONE;
    }

    if (is_sav_pmf_config) {
        rv = sav_field_data_clean(unit, sav_inlif_profile_cs_context_id);
        if (rv != BCM_E_NONE) {
            printf("Error in inlif_wide_data_general_pmf_clean\n");
            return rv;
        }
    }

    rv = cint_sav_field_inlif_profile_cs_ctx_create(unit, in_lif_profile_sav);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_sav_field_inlif_profile_cs_ctx_create\n", rv);
        return rv;
    }


    bcm_field_group_info_t_init(&fg_info);
    fg_info.stage = bcmFieldStageIngressPMF1;
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyL3SrcBindHit;
    fg_info.action_types[0] = bcmFieldActionTrap;
    fg_info.nof_actions = 1;

    dest_char = &fg_info.name[0];
    sal_strncpy_s(dest_char, "AntiSpoofingDefault", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, 0, &fg_info, &sav_default_fg_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_field_group_add\n");
        return rv;
    }
    printf("Create FG[%d] for default\n", sav_default_fg_id);




    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].input_arg = 0;

    attach_info.key_info.qual_info[0].offset = 0;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.nof_actions = fg_info.nof_actions;


    rv = bcm_field_group_context_attach(unit, 0, sav_default_fg_id, sav_inlif_profile_cs_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("\n sllb_def_dest_trap_set: bcm_rx_trap_type_create failed with rv %d\n", rv);
        return rv;
    }

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST;
    trap_config.dest_port = BCM_GPORT_BLACK_HOLE;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_NONE)
    {
        printf("\n sllb_def_dest_trap_set: bcm_rx_trap_set failed with rv %d\n", rv);
        return rv;
    }

    BCM_GPORT_TRAP_SET(trap_gport_drop, trap_id, 7, 0);

    bcm_field_entry_info_t_init(&entry_info);
    entry_info.priority = 3;
    entry_info.nof_entry_quals = 1;
    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = 0;
    entry_info.entry_qual[0].mask[0] = 0x1;
    entry_info.nof_entry_actions = 1;
    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = trap_gport_drop;

    rv = bcm_field_entry_add(unit, 0, sav_default_fg_id, &entry_info, &default_ent_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    printf("anti-spoofing default PMF create successfully!\n");

    is_sav_pmf_config=1;
    return rv;

}
