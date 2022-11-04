/*  Feature  : Hurricane4 MACsec Encryption and decryption
 *
 *  Usage    : BCM.0> cint macsec_encrypt_decrypt_4_sa_multi_vlans_cos.c
 *
 *  config   : hr4_sdk6_macsec_config.bcm
 *
 *  Log file : macsec_encrypt_decrypt_4_sa_multi_vlans_cos_log.txt
 *
 *  Test Topology :
 *
 *                   +----------------------------+
 *                   |                            |
 *                   |                            +
 *                   |                            |
 *    ingress_port   |                            |
 *   ----------------+            HR4             +----------------
 *                   |                            |  egress_port1
 *                   |                            |
 *                   |                            +
 *                   |                            |
 *                   |                            |
 *                   +----------------------------+
 *
 *  Summary:
 *  ========
 *    This CINT configures macsec encryption/decryption for a single port with multiple vlans
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      1.1) Configure IFP rules and EFP rules for MACsec
 *           1.1.1) Install an IFP rule to redirect MACsec packets received from
 *                  MACsec port to one of MACsec decrypt engine ports (decrypt-1st pass)
 *           1.1.2) Install an EFP rule to modify SVTAG in the packet (decrypt-1st pass)
 *           1.1.3) Install VFP/IFP rules to map int_pri/cng in SVTAG to encrypt-2nd pass internal priority/cng.
 *           1.1.4) Install an IFP rule to assign Modidbase for the encrypted
 *                  packet from MACsec engine (encrypt-2nd pass).
 *
 *      1.2) Configure MACsec on macsec-enabled port (egress_port1)
 *           1.2.1) Enable MACsec on the port
 *           1.2.2) Add a flex flow-based L2 entry to map SVTAG.DEST_PORT to DGPP (encrypt-2nd pass IPIPE).
 *           1.2.3) Set the port class to 1 (to be used as a qualifier for the IFP rule in 1.1.1)
 *           1.2.4) Configure a L3_IPMC egress object with L3_FLAGS3_L2_TAG_ONLY
 *           1.2.5) Creat a per-port egress SC with encryption disabled
 *
 *      1.3) Configure vlan 10, vlan 11, and vlan 12 with 2 ports (ingress_port and egress_port1)
 *           1.3.1) For each vlan, create an ipmc remap group and map the original ipmc group of the vlan to the corresponding remap group.
 *                  Ipmc remap group is the same as the original ipmc group except that for each macsec-enabled port (port x) in the vlan
 *                  (a) port x is not in the L2 bitmap
 *                  (b) L3_IPMC egress object created in 1.2.4 for port x will be chained under port 65/66.
 *
 *      1.4) For each vlan with MACsec-enabled port (egress_port1) and MACsec processing enabled on <vlan, port>
 *           1.4.1) For decryption direction
 *                  1.4.1.1) Configure a decryption SC and 4 SA's
 *                  1.4.1.2) Configure a decryption policy
 *                  1.4.1.3) Configure a decryption flow
 *           1.4.2) For encryption direction:
 *                  1.4.2.1) Configure an encryption SC and 4 SA's
 *                  1.4.2.2) Install EFP rules to modify SVTAG of the packet (encrypt-1st pass).
 *                           The subport id is passed in to MACsec engine through SVTAG.
 *                           Packet Cng (2 bits) are passed in SVTAG also.
 *      1.5) For each vlan with MACsec-enabled port (egress_port1) and MACsec processing disabled on <vlan, port>
 *           1.5.1) For decryption direction
 *                  1.5.1.1) Configure a flow for <vlan, port> and mark it as management flow
 *           1.5.2) For encryption direction,
 *                  1.5.2.1) Install EFP rules to modify SVTAG of the packet (encrypt-1st pass).
 *                           The subport id is passed in to MACsec engine through SVTAG.
 *                           Packet Cng (2 bits) are passed in SVTAG also.
 *                           The egress SC created in 1.2.5 should be used
 *
 *      1.6) Add mapping from internal priority (4 bits) to SVTAG.PRI (encrypt-1st pass EPIPE)
 *
 *    2) Step2 - Configuration (Null)
 *    ======================================================
 *      Do nothing in configuration step.
 *
 *    3) Step3 - Verification (Done in test_verify())
 *    ======================================================
 *      3.1) Test vlan 10 traffic:
 *           3.1.1) Test unicast forwarding to a MACsec-enabled port
 *                  3.1.1.1) Add L2 entries for unicast L2 switching to egress_port1
 *                  3.1.1.2) Unicast packet is received from ingress port
 *                           Expected result:
 *                           The packet is encrypted with the egress SC for <vlan 10, egress_port1> and forwarded to egress_port1
 *           3.1.2) Test unicast MACsec decryption
 *                  3.1.2.1) Add L2 entries for unicast L2 switching to ingress_port
 *                  3.1.2.2) Unicast packet encrypted for ingress SC for <vlan, egress_port1> is received from egress_port1
 *                           Expected result:
 *                           The packet is decrypted and forwarded to ingress_port
 *           3.1.3) Remove L2 entry added in 3.1.1)/3.1.2) and repeat for multicast packets for encryption and decryption
 *
 *           3.1.4) Test manual switching to a new egress SA and repeat 3.1.1
 *                  Expected result:
 *                  The packet is encrypted with a new AN and forwarded to egress_port1
 *           3.1.5) Repeat 3.1.2 using packet encrypted with the next ingress SA
 *                  Expected result:
 *                  The packet is decrypted and forwarded to ingress_port
 *      3.2) Repeat the same test as 3.1 for vlan 11 traffic
 *      3.3) Test vlan 12 traffics
 *           Expected result:
 *           3.3.1) Encrypt direction:
 *                  <vlan 12, egress_port1) egress packets will not be encrypted
 *           3.3.2) Decryption direction:
 *                  <vlan 12, egress_port1) ingress packets will be treated as management packet and bypass MACsec.
 */

/* Reset C interpreter*/
cint_reset();

int mod_id = 2;

bcm_field_group_t macsec_efp_group;

/**** global variables ****/
int ifp_encrypt_stat_id;
int ifp_decrypt_stat_id[2];
int efp_decrypt_stat_id;

static int isec_sp_tcam_index = 0;
static int isec_sc_tcam_index = 0;

uint8 ing_pkt_pri = 0;
uint8 egr_pkt_pri = 0;

uint8 int_pri = 11;
bcm_color_t pkt_color = bcmColorGreen;
uint8 svtag_pri = int_pri >> 1;
uint8 svtag_cng = int_pri & 0x1;

/**** Data Structures ****/

int MANAGEMENT_FLOW_FLAG = 0x01;

struct macsec_decrypt_flow_s {
    bcm_mac_t   src_mac;            /* Match on Packet src_mac */
    bcm_mac_t   src_mac_mask;
    bcm_mac_t   dst_mac;            /* Match on Packet dst_mac */
    bcm_mac_t   dst_mac_mask;
    bcm_port_t  src_port;           /* Match on Packet ingress port */
    uint32      src_port_mask;
    bcm_vlan_t  vlan;
    bcm_vlan_t  vlan_mask;
    bcm_xflow_macsec_flow_id_t   flow_id;   /* Returned flow id */
    int         pri;
    int         flags;
};

struct macsec_port_s {
    bcm_port_t      port;
    uint32          macsec_port_class;
    uint8           encrypt_port_offset;
    bcm_if_t        egr_obj_id;         /* for ipmc remap */
    bcm_xflow_macsec_secure_chan_id_t encrypt_disabled_sc_id;
};

int MAX_PORTS_PER_VLAN=72;

struct macsec_vlan_s {
    bcm_vlan_t      vid;
    bcm_port_t      port[MAX_PORTS_PER_VLAN];
    int             num_of_ports;
    bcm_multicast_t vlan_ipmc_group;
    bcm_multicast_t remapped_ipmc_group;
};

/* internal flags */
int VLAN_PORT_MACSEC_DISABLE_FLAG = 0x01;
int VLAN_PORT_MACSEC_NO_SCI_P2P_FLAG = 0x02;
int VLAN_PORT_MACSEC_NO_SCI_ES_FLAG = 0x04;

/* Note that multiple decrypt SC's will exist when the MACsec CA is not P2P */
/* For simplicity, it is omitted in the test */
struct macsec_vlan_port_s {
    int macsec_port_index;
    int macsec_vlan_index;
    bcm_xflow_macsec_secure_chan_id_t   encrypt_sc_id;
    bcm_xflow_macsec_secure_assoc_id_t  encrypt_sa_id[4];
    uint64                              encrypt_sci;
    int efp_encrypt_stat_id[3];

    bcm_xflow_macsec_secure_chan_id_t   decrypt_sc_id;
    bcm_xflow_macsec_secure_assoc_id_t  decrypt_sa_id[4];
    uint64                              decrypt_sci;
    macsec_decrypt_flow_s               decrypt_flow;
    int flags;                          /* internal flags */
};



/**** Internal Sub-functions ****/

void
macsec_decrypt_flow_s_init(macsec_decrypt_flow_s *decrypt_flow)
{
    if (decrypt_flow != NULL) {
        sal_memset(decrypt_flow, 0, sizeof (*decrypt_flow));
    }
    return;
}

void
macsec_port_s_init(macsec_port_s *macsec_port)
{
    if (macsec_port != NULL) {
        sal_memset(macsec_port, 0, sizeof (*macsec_port));
    }
    return;
}

void
macsec_vlan_s_init(macsec_vlan_s *macsec_vlan)
{
    if (macsec_vlan != NULL) {
        sal_memset(macsec_vlan, 0, sizeof (*macsec_vlan));
    }
    return;
}

void
macsec_vlan_port_s_init(macsec_vlan_port_s *macsec_vlan_port)
{
    if (macsec_vlan_port != NULL) {
        sal_memset(macsec_vlan_port, 0, sizeof (*macsec_vlan_port));
    }
    return;
}


/************************** MACsec Sub-Routines ****************************/

/* Create vlan and add port to vlan */
bcm_error_t
vlan_create_add_port(int unit, int vid, int port, int untagged)
{
    bcm_pbmp_t pbmp, upbmp;
    bcm_error_t rv = BCM_E_NONE;

    rv = bcm_vlan_create(unit, vid);
    if((rv != BCM_E_NONE) && (rv != BCM_E_EXISTS)) {
        printf("\nError in creating vlan: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    if (untagged) {
        BCM_PBMP_PORT_ADD(upbmp, port);
    }
    rv = bcm_vlan_port_add(unit, vid, pbmp, upbmp);
    if (BCM_FAILURE(rv)) {
        printf("\nError in adding vlan port: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Create VLAN with IPMC group since macsec-enabled port required the IPMC logic */
bcm_error_t
macsec_vlan_create(int unit, int vid,
                   bcm_multicast_t *vlan_ipmc_group)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_vlan_control_vlan_t vlan_ctrl;

    rv = bcm_vlan_create(unit, vid);
    if((rv != BCM_E_NONE) && (rv != BCM_E_EXISTS)) {
        printf("\nError in creating vlan: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_IF_ERROR_RETURN(bcm_multicast_create(unit, BCM_MULTICAST_TYPE_VLAN, vlan_ipmc_group));
    BCM_IF_ERROR_RETURN(bcm_vlan_control_vlan_get(unit, vid, &vlan_ctrl));
    vlan_ctrl.broadcast_group           = *vlan_ipmc_group;
    vlan_ctrl.unknown_multicast_group   = *vlan_ipmc_group;
    vlan_ctrl.unknown_unicast_group     = *vlan_ipmc_group;

    /* Enables virtual port replications to this VLAN using replication indices */
    vlan_ctrl.vp_mc_ctrl                = bcmVlanVPMcControlEnable;

    rv = bcm_vlan_control_vlan_set(unit, vid, vlan_ctrl);
    if (BCM_FAILURE(rv)) {
        printf("\nError in creating vlan multicast group: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Add L2 Entry */
bcm_error_t
l2_add(int unit, bcm_mac_t mac, bcm_vlan_t vid, int port)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_l2_addr_t l2_addr;
    int my_modid;

    BCM_IF_ERROR_RETURN(bcm_stk_modid_get(unit, &my_modid));
    bcm_l2_addr_t_init(&l2_addr, mac, vid);
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.modid = my_modid;
    l2_addr.port  = port;
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (BCM_FAILURE(rv)) {
        printf("\nError in adding static L2 entry: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    return rv;
}

bcm_error_t
l3_intf_create(int unit, bcm_vlan_t vlan, bcm_mac_t mac_addr, uint32 flags, bcm_if_t * intf_id)
{
    bcm_l3_intf_t l3_interface;

    bcm_l3_intf_t_init(&l3_interface);
    sal_memcpy(l3_interface.l3a_mac_addr, mac_addr, sizeof(l3_interface.l3a_mac_addr));
    l3_interface.l3a_vid = vlan;
    l3_interface.l3a_flags = flags;
    BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &l3_interface));
    *intf_id = l3_interface.l3a_intf_id;

    return BCM_E_NONE;
}

/* L3 egress object, BCM_L3_IPMC */
bcm_error_t
l3_egress_create(int unit, uint32 flags, uint32 flags3, bcm_port_t port,
                            bcm_vlan_t vlan, bcm_mac_t mac_addr, bcm_if_t intf_id,
                 bcm_if_t *egr_obj_id)
{
    bcm_l3_egress_t l3_egress;
    int my_modid;

    bcm_l3_egress_t_init(&l3_egress);

    BCM_IF_ERROR_RETURN(bcm_stk_modid_get(unit, &my_modid));
    l3_egress.flags     = flags;
    l3_egress.intf      = intf_id;
    sal_memcpy(l3_egress.mac_addr, mac_addr, sizeof(l3_egress.mac_addr));
    l3_egress.vlan      = vlan;
    l3_egress.module    = my_modid;
    l3_egress.port      = port;
    l3_egress.flags3    = flags3; /* L2MC: use SD tag view (flag BCM_L3_FLAGS3_L2_TAG_ONLY) */
    BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, flags, &l3_egress, egr_obj_id));
    return BCM_E_NONE;
}

bcm_error_t
multicast_port_add(int unit, bcm_multicast_t mc_group,
                   bcm_port_t port, bcm_if_t egr_obj_id)
{
    bcm_if_t    encap_id = -1;
    bcm_gport_t gport;
    int my_modid;

    BCM_IF_ERROR_RETURN(bcm_stk_modid_get(unit, &my_modid));
    BCM_GPORT_MODPORT_SET(gport, my_modid, port);
    if (egr_obj_id) {
        BCM_IF_ERROR_RETURN(bcm_multicast_egress_object_encap_get (unit, mc_group, egr_obj_id, &encap_id));
    }
    /* SDK will convert the port to the corresponding MACsec engine port to add to L3_bitmap */
    BCM_IF_ERROR_RETURN(bcm_multicast_egress_add (unit, mc_group, gport, encap_id));

    return BCM_E_NONE;
}

bcm_error_t
multicast_port_del(int unit, bcm_multicast_t mc_group,
                   bcm_port_t port, bcm_if_t egr_obj_id)
{
    bcm_if_t            encap_id = -1;
    bcm_gport_t gport;
    int my_modid;

    BCM_IF_ERROR_RETURN(bcm_stk_modid_get(unit, &my_modid));
    BCM_GPORT_MODPORT_SET(gport, my_modid, port);
    if (egr_obj_id) {
        BCM_IF_ERROR_RETURN(bcm_multicast_egress_object_encap_get (unit, mc_group, egr_obj_id, &encap_id));
    }
    BCM_IF_ERROR_RETURN(bcm_multicast_egress_delete (unit, mc_group, gport, encap_id));

   return BCM_E_NONE;
}


/* Add l2 flex flow. Used by MACSEC_ENCRYPT_TX */
bcm_error_t
macsec_encrypt_l2_flex_add(int unit, int port)
{
    bcm_l2_addr_t           l2_addr;
    bcm_flow_handle_t       handle;
    bcm_flow_option_id_t    option_id;
    bcm_flow_field_id_t     field_id;
    bcm_flow_logical_field_t logical_fields;
    int my_modid;
    int i;

    /* This is used for Encryption 2nd pass */
    BCM_IF_ERROR_RETURN(bcm_stk_modid_get(unit, &my_modid));
    BCM_IF_ERROR_RETURN( bcm_flow_handle_get(unit, "MACSEC_ENCRYPT_TX", &handle));
    BCM_IF_ERROR_RETURN( bcm_flow_option_id_get(unit, handle, "LOOKUP_SUBPORT_NUMBER_ASSIGN_LOCAL_DGPP", &option_id));
    BCM_IF_ERROR_RETURN( bcm_flow_logical_field_id_get(unit, handle, "SUBPORT_NUMBER", &field_id));
    logical_fields.id       = field_id;

    /* The current Cancun uses <SVTAG.pri, port> as key to map to DGPP */
    /* It could have used <port> alone as key instead */
    for (i = 0; i < 8; i++) {
        logical_fields.value = i << 13 | port; /* subport number field from SVTAG */
        sal_memset(&l2_addr, 0, sizeof (l2_addr));
        l2_addr.flow_handle     = handle;
        l2_addr.flow_option_handle = option_id;
        l2_addr.port            = port; /* Destination port number */
        l2_addr.modid           = my_modid;
        l2_addr.cos_dst = 0;   /* second pass int_pri to be determined by VFP/IFP */
        l2_addr.logical_fields[0] = logical_fields;
        l2_addr.num_of_fields   = 1;
        /* l2_addr.flags = BCM_L2_SETPRI; */
        BCM_IF_ERROR_RETURN( bcm_l2_addr_add(unit, &l2_addr));
    }
    return BCM_E_NONE;
}


/* Add egr vlan xlate entry. Used by MACSEC_ENCRYPT_RX */
bcm_error_t
macsec_encrypt_flow_encap_add(int unit, bcm_port_t dest_port, int int_pri, bcm_xflow_macsec_secure_chan_id_t sc_id)
{
    /* This is used for 1st pass */
    bcm_flow_handle_t       handle;
    bcm_flow_option_id_t    option_id;
    bcm_flow_logical_field_t logical_fields;
    bcm_flow_encap_config_t encap_config;
    bcm_xflow_macsec_subport_id_t subport_id;
    int subport_num;
    int my_modid;

    BCM_IF_ERROR_RETURN(bcm_stk_modid_get(unit, &my_modid));
    BCM_IF_ERROR_RETURN(bcm_xflow_macsec_subport_id_get(unit, sc_id, &subport_id));

    BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "MACSEC_ENCRYPT_RX", &handle));
    BCM_IF_ERROR_RETURN(bcm_flow_option_id_get(unit, handle, "LOOKUP_DGPP_INTPRI_ASSIGN_CLASS_ID", &option_id));
    subport_num = BCM_XFLOW_MACSEC_SUBPORT_NUM_GET(subport_id);
    bcm_flow_encap_config_t_init(&encap_config);
    encap_config.flow_handle    = handle;
    encap_config.flow_option    = option_id;
    BCM_GPORT_MODPORT_SET(encap_config.flow_port, my_modid, dest_port);
    encap_config.pri            = int_pri;
    encap_config.class_id       = subport_num;
    encap_config.criteria       = BCM_FLOW_ENCAP_CRITERIA_DGPP_INT_PRI;
    encap_config.valid_elements = BCM_FLOW_ENCAP_FLOW_PORT_VALID | BCM_FLOW_ENCAP_INT_PRI_VALID | BCM_FLOW_ENCAP_CLASS_ID_VALID;
    BCM_IF_ERROR_RETURN(bcm_flow_encap_add(unit, &encap_config, 0, &logical_fields));
    return BCM_E_NONE;
}


bcm_error_t
macsec_secure_chan_create(int unit, int flags, uint64 sci, int int_flags, int pri, bcm_port_t dest_port,
                          bcm_xflow_macsec_secure_chan_id_t *sc_id)
{
    int rv          = BCM_E_NONE;
    /* SC vars */
    int conf_off    = 0;
    int mtu_sel     = bcmXflowMacsecMtu0;
    int act_an      = 0;

    int sectag_off  = 16;
    int crypto      = bcmXflowMacsecCryptoAes128Gcm;
    int sc_info_flags = BCM_XFLOW_MACSEC_SECURE_CHAN_INFO_CONTROLLED_PORT;
    xflow_macsec_sectag_ethertype_t etype_sel = bcmXflowMacsecSecTagEtype0; /* 0x88E5 */
    uint64 sci_mask;
    bcm_xflow_macsec_subport_id_t subport_id;
    int subport_num;
    uint8 tci;

    COMPILER_64_SET(sci_mask, 0xffffffff, 0xffffffff);

    /* Configure security channel */
    bcm_xflow_macsec_secure_chan_info_t sc_info;
    bcm_xflow_macsec_secure_chan_info_t_init(&sc_info);
    sc_info.sci         = sci;
    sc_info.crypto      = crypto;
    sc_info.confidentiality_offset = conf_off;
    sc_info.active_an   = act_an;
    if (flags & BCM_XFLOW_MACSEC_ENCRYPT) {
        if (flags & BCM_XFLOW_MACSEC_SECURE_CHAN_INFO_ENCRYPT_DISABLE) {
            sc_info_flags |= BCM_XFLOW_MACSEC_SECURE_CHAN_INFO_ENCRYPT_DISABLE;
            COMPILER_64_SET(sc_info.sci, 0x0, 0x0);
            sc_info.crypto      = 0;
            sc_info.confidentiality_offset = 0;
            sc_info.active_an   = 0;
            sc_info.dest_port   = dest_port;
        }
        else {
            tci = 0x0B; /* SC=1, E=1, C=1 */
            sc_info_flags |= BCM_XFLOW_MACSEC_SECURE_CHAN_INFO_INCLUDE_SCI;

            if (int_flags & (VLAN_PORT_MACSEC_NO_SCI_P2P_FLAG | VLAN_PORT_MACSEC_NO_SCI_ES_FLAG)) {
                tci = tci &= ~0x8;     /* SC=0 */
                sc_info_flags &= ~BCM_XFLOW_MACSEC_SECURE_CHAN_INFO_INCLUDE_SCI;
            }
            if (int_flags & VLAN_PORT_MACSEC_NO_SCI_ES_FLAG) {
                tci = tci | 0x10;      /* ES=1 */
            }
            sc_info.tci         = tci;
            sc_info.mtu_sel     = mtu_sel;
            sc_info.sectag_etype_sel = etype_sel;
            sc_info.sectag_offset = sectag_off;
            sc_info.dest_port   = dest_port; /* SVTAG.Dest_port sent from MACSEC engine */
            if (dest_port == 0) {
                printf("\r\nDest port is required for MACSEC_ENCRYPT_TX!\n");
                return BCM_E_PARAM;
            }
        }
    }
    else {
        sc_info.sci_mask    = sci_mask;
        sc_info.replay_protect_window_size = 0; /* 0: strict replay check */
        sc_info_flags |= BCM_XFLOW_MACSEC_SECURE_CHAN_INFO_REPLAY_PROTECT_ENABLE;
    }
    sc_info.flags       = sc_info_flags;
    rv = bcm_xflow_macsec_secure_chan_create(unit, flags, 0, sc_info, pri, sc_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in creating security channel: %s.\n", bcm_errmsg(rv));
        print sc_info;
        return rv;
    }
    printf("\nCreated securty channel with id: 0x%x\n", *sc_id);
    if (flags == BCM_XFLOW_MACSEC_ENCRYPT) {
        print bcm_xflow_macsec_subport_id_get(unit, *sc_id, &subport_id);
        printf ("Subport_id 0x%x for the sc 0x%x\n",subport_id, *sc_id);
        subport_num = BCM_XFLOW_MACSEC_SUBPORT_NUM_GET(subport_id);
        printf ("Subport_num 0x%x for the sc 0x%x\n",subport_num, *sc_id);
    }

    /* Enable security channel*/
    BCM_IF_ERROR_RETURN(bcm_xflow_macsec_secure_chan_enable_set (unit, *sc_id, 1));
    printf("\r\nEnabled secure channel with id: 0x%x\n", *sc_id);
    return BCM_E_NONE;
}



bcm_error_t
macsec_secure_assoc_create(int unit, int flags, bcm_xflow_macsec_secure_chan_id_t sc_id,
                           bcm_xflow_macsec_secure_assoc_id_t *sa_id, int asso_num)
{
    /* SA vars */
    int next_pkt_num    = 5;
    bcm_xflow_macsec_crypto_aes128_gcm_t aes_k;
    int an_control = bcmXflowMacsecSecureAssocAnNormal;
    int sa_info_flags   = BCM_XFLOW_MACSEC_SECURE_ASSOC_INFO_SET_NEXT_PKT_NUM;

    /*Configure security association*/
    bcm_xflow_macsec_secure_assoc_info_t sa_info;
    bcm_xflow_macsec_crypto_aes128_gcm_t key;
    bcm_xflow_macsec_secure_assoc_info_t_init(&sa_info);
    aes_k.key[0] = 0xef;
    aes_k.key[1] = 0xcd;
    aes_k.key[2] = 0xab;
    aes_k.key[3] = 0x89;
    aes_k.key[4] = 0x67;
    aes_k.key[5] = 0x45;
    aes_k.key[6] = 0x23;
    aes_k.key[7] = 0x01;
    aes_k.key[8] = 0xec;
    aes_k.key[9] = 0xff;
    aes_k.key[10] = 0x00;
    aes_k.key[11] = 0x00;
    aes_k.key[12] = 0x00;
    aes_k.key[13] = 0x00;
    aes_k.key[14] = 0x00;
    aes_k.key[15] = 0x00 + asso_num;
    sa_info.an = asso_num;
    sa_info.aes = aes_k;
    if (flags == BCM_XFLOW_MACSEC_ENCRYPT) {
        sa_info.an_control = an_control;
    }
    sa_info.next_pkt_num = next_pkt_num;
    sa_info.flags = sa_info_flags;

    BCM_IF_ERROR_RETURN( bcm_xflow_macsec_secure_assoc_create (unit, flags, sc_id ,sa_info, sa_id));
    printf("\r\nCreated security association with id:0x%x\n", *sa_id);

    return BCM_E_NONE;
}


bcm_error_t
macsec_macsec_port_enable(int unit, int port, uint8 encrypt_port_offset)
{
    /* Enable MacSec on the ports which transmit/receive the MACsec packets.
       - configure MODPORT_MAP_SUBPORT to map DGPP to the internal MACsec lb port */
    bcm_xflow_macsec_port_info_t port_info;
    port_info.enable = 1;
    /* port offset 0 or 1, which connects to the MACsec Engine */
    port_info.encrypt_port_offset = encrypt_port_offset;
    BCM_IF_ERROR_RETURN(bcm_xflow_macsec_port_info_set(unit, port, &port_info));
    printf("Enable MACsec on the port %d\n", port);
    return BCM_E_NONE;
}



bcm_error_t
macsec_decrypt_policy_create(int unit, uint64 sci, uint32 pol_info_flags,
                             uint32 sectag_offset, int mtu_sel,
                             bcm_xflow_macsec_policy_id_t *pol_id)
{
    uint32 policy_flags = 0;

    if (COMPILER_64_HI(sci) || COMPILER_64_LO(sci))
        pol_info_flags |= BCM_XFLOW_MACSEC_DECRYPT_POLICY_POINT_TO_POINT_ENABLE;

    /* Configure decrypt policy */
    bcm_xflow_macsec_decrypt_policy_info_t dec_policy;
    bcm_xflow_macsec_decrypt_policy_info_t_init(&dec_policy);
    dec_policy.flags = pol_info_flags;
    dec_policy.sci = sci;
    dec_policy.sectag_offset = sectag_offset;
    dec_policy.mtu_sel = mtu_sel;

    BCM_IF_ERROR_RETURN( bcm_xflow_macsec_decrypt_policy_create(unit, policy_flags, 0, &dec_policy, pol_id));
    printf("\nCreated decrypt policy with id:0x%x\n", *pol_id);
    return BCM_E_NONE;
}


bcm_error_t
macsec_secure_chan_decrypt_policy_bind(int unit, bcm_xflow_macsec_secure_chan_id_t sc_id,
                                       bcm_xflow_macsec_policy_id_t pol_id)
{
    /*  Get and update secure channel with policy id */
    bcm_xflow_macsec_secure_chan_info_t sc_info_get;
    bcm_xflow_macsec_secure_chan_info_t_init(&sc_info_get);
    int pri_get = 0;
    BCM_IF_ERROR_RETURN( bcm_xflow_macsec_secure_chan_get(unit, sc_id, &sc_info_get, &pri_get));
    sc_info_get.policy_id = pol_id;
    BCM_IF_ERROR_RETURN( bcm_xflow_macsec_secure_chan_set(unit, BCM_XFLOW_MACSEC_DECRYPT, sc_id, &sc_info_get, pri_get));
    printf("\nBind secure channel 0x%x to decrypt policy 0x%x, pri=%d\n", sc_id, pol_id, pri_get);
    return BCM_E_NONE;
}


bcm_error_t
macsec_decrypt_flow_create(int unit, macsec_decrypt_flow_s *decrypt_flow, bcm_xflow_macsec_policy_id_t policy_id)
{
    /* Decrypt flow vars */
    xflow_macsec_flow_pkt_type_t pkt_type = bcmXflowMacsecDecryptFlowMacSec;
    uint32 tpid = BCM_XFLOW_MACSEC_FLOW_TPID_SEL_0;
    uint32 tpid_mask = 0x0F;
    /* xflow_macsec_vlan_mpls_tag_status_t vlan_mpls_tag_status = bcmXflowMacsecTagAny; */
    xflow_macsec_vlan_mpls_tag_status_t vlan_mpls_tag_status = bcmXflowMacsecTagSingleVlan;
    xflow_macsec_flow_etype_t etype_sel = bcmXflowMacsecFlowEtypeEII;
    bcm_xflow_macsec_flow_id_t flow_id;

    /* Configure decrypt flows */
    bcm_xflow_macsec_decrypt_flow_info_t dec_flow;
    bcm_xflow_macsec_decrypt_flow_info_t_init(&dec_flow);

    if (decrypt_flow->flags & MANAGEMENT_FLOW_FLAG) {
        /* all packets in the flow are treated as management packets */
        dec_flow.set_management_pkt = 1;
        pkt_type = bcmXflowMacsecDecryptFlowNonMacsec;
    }
    dec_flow.policy_id  = policy_id;
    dec_flow.pkt_type = pkt_type;
    dec_flow.tpid = tpid;
    dec_flow.tpid_mask = tpid_mask;
    dec_flow.vlan_mpls_tag_status = vlan_mpls_tag_status;
    dec_flow.etype_sel = etype_sel;
    sal_memcpy(dec_flow.src_mac, decrypt_flow->src_mac, sizeof(dec_flow.src_mac));
    sal_memcpy(dec_flow.src_mac_mask, decrypt_flow->src_mac_mask, sizeof(dec_flow.src_mac));
    sal_memcpy(dec_flow.dst_mac, decrypt_flow->dst_mac, sizeof(dec_flow.src_mac));
    sal_memcpy(dec_flow.dst_mac_mask, decrypt_flow->dst_mac_mask, sizeof(dec_flow.src_mac));
    dec_flow.src_port   = decrypt_flow->src_port;
    dec_flow.src_port_mask = decrypt_flow->src_port_mask;
    dec_flow.udf.first_vlan = decrypt_flow->vlan;
    dec_flow.udf_mask.first_vlan = decrypt_flow->vlan_mask;

    print bcm_xflow_macsec_decrypt_flow_create(unit, BCM_XFLOW_MACSEC_DECRYPT, 0, &dec_flow, decrypt_flow->pri, &flow_id);
    printf("\nCreated decrypt flow with id:0x%x\n", flow_id);
    print bcm_xflow_macsec_decrypt_flow_enable_set(unit, flow_id, 1);
    printf("\nEnabled decrypt flow with id:0x%x\n", flow_id);
    /* get the decrypt flow */
    decrypt_flow->flow_id = flow_id;
    return BCM_E_NONE;
}


int non_macsec_port_class = 0;
int macsec_port_class1 = 1;
int macsec_port_class2 = 2;


bcm_error_t
assign_egress_sc_by_efp(int unit, bcm_port_t port, bcm_vlan_t vlan_id, bcm_xflow_macsec_secure_chan_id_t sc_id,
                        bcm_color_t pkt_color, bcm_field_group_t group, int *stat_id)
{
    int qset_macsec_flow;
    int aset_pkt_type;
    bcm_field_stat_t stat_type[]  = { bcmFieldStatBytes, bcmFieldStatPackets };
    bcm_field_entry_t entry;
    bcm_xflow_macsec_subport_id_t subport;
    char str[512];
    int my_modid;
    uint8 data;

    BCM_IF_ERROR_RETURN(bcm_stk_modid_get(unit, &my_modid));

    snprintf(str, 512,"%s","> EFP rule to modify encrypt SVTAG installed.\n");
    qset_macsec_flow = bcmFieldMacSecFlowForEncrypt;
    aset_pkt_type = bcmFieldActionMacSecEncryptPktType;
    BCM_IF_ERROR_RETURN(bcm_xflow_macsec_subport_id_get(unit, sc_id, &subport));

    /* EFP rule matches MACsec flow to modify SVTAG */
    BCM_IF_ERROR_RETURN( bcm_field_stat_create(unit, group, sizeof(stat_type) / sizeof(stat_type[0]), stat_type, stat_id));

    BCM_IF_ERROR_RETURN( bcm_field_entry_create(unit, group, &entry));
    BCM_IF_ERROR_RETURN( bcm_field_qualify_MacSecFlow(unit, entry, qset_macsec_flow));
    BCM_IF_ERROR_RETURN( bcm_field_qualify_DstPort(unit, entry, my_modid, 0xff, port, 0xff));
    BCM_IF_ERROR_RETURN( bcm_field_qualify_OuterVlanId(unit, entry, vlan_id, 0xFFF));
    switch (pkt_color) {
    case bcmColorGreen:
    data = BCM_FIELD_COLOR_GREEN;
    BCM_IF_ERROR_RETURN( bcm_field_qualify_Color(unit, entry, data));
    BCM_IF_ERROR_RETURN( bcm_field_action_add(unit, entry, bcmFieldActionMacSecSvtagColor, BCM_FIELD_COLOR_GREEN, 0));
    break;
    case bcmColorYellow:
    data = BCM_FIELD_COLOR_YELLOW;
    BCM_IF_ERROR_RETURN( bcm_field_qualify_Color(unit, entry, data));
    BCM_IF_ERROR_RETURN( bcm_field_action_add(unit, entry, bcmFieldActionMacSecSvtagColor, BCM_FIELD_COLOR_YELLOW, 0));
    break;
    case bcmColorRed:
    data = BCM_FIELD_COLOR_RED;
    BCM_IF_ERROR_RETURN( bcm_field_qualify_Color(unit, entry, data));
    BCM_IF_ERROR_RETURN( bcm_field_action_add(unit, entry, bcmFieldActionMacSecSvtagColor, BCM_FIELD_COLOR_RED, 0));
    break;
    }
    BCM_IF_ERROR_RETURN( bcm_field_entry_stat_attach(unit, entry, *stat_id));
    BCM_IF_ERROR_RETURN( bcm_field_action_add(unit, entry, aset_pkt_type, bcmFieldMacSecEncryptDecryptPktData, 0));

    /* param0 is subport_id, not subport_num */
    BCM_IF_ERROR_RETURN( bcm_field_action_add(unit, entry, bcmFieldActionMacSecSubPortNumAdd, subport, 0x0));
    BCM_IF_ERROR_RETURN( bcm_field_entry_install(unit, entry));
    printf("%s\n", str);

    return BCM_E_NONE;
}


bcm_if_t dummy_intf_id;
bcm_vlan_t dummy_vid = 4095;
bcm_mac_t dummy_mac = "00:00:00:00:00:01";


bcm_error_t
macsec_port_config(int unit, macsec_port_s *macsec_port)
{
    bcm_port_t  port = macsec_port->port;
    printf("\n-------------------------------------------\n");
    printf("Start macsec_port_config() for port-%d\n", port);
    printf("---------------------------------------------\n");
    BCM_IF_ERROR_RETURN(macsec_macsec_port_enable(unit, port, macsec_port->encrypt_port_offset));

    /* macsec port: class 1 or 2 */
    BCM_IF_ERROR_RETURN(bcm_port_class_set(unit, port, bcmPortClassFieldIngress, macsec_port->macsec_port_class));

    BCM_IF_ERROR_RETURN(macsec_encrypt_l2_flex_add(unit, port));

    uint32 flags3 = BCM_L3_FLAGS3_L2_TAG_ONLY;
    BCM_IF_ERROR_RETURN(l3_egress_create(unit, BCM_L3_IPMC, flags3, macsec_port->port, dummy_vid, dummy_mac, dummy_intf_id, &(macsec_port->egr_obj_id)));

    uint64 zero_sci;
    COMPILER_64_SET(zero_sci, 0x0, 0x0);
    printf("Configure per port disabled egress SC\n");
    /* disabled egress SC has to be per port as MACsec engine will check SC.DGPP */
    BCM_IF_ERROR_RETURN(macsec_secure_chan_create(unit, BCM_XFLOW_MACSEC_ENCRYPT | BCM_XFLOW_MACSEC_SECURE_CHAN_INFO_ENCRYPT_DISABLE,
                                                  zero_sci, 0, 0, port, &macsec_port->encrypt_disabled_sc_id));
    return BCM_E_NONE;
}



bcm_error_t
macsec_vlan_config(int unit, macsec_vlan_s *macsec_vlan)
{
    bcm_vlan_t  vlan = macsec_vlan->vid;
    int         untagged = 0;
    bcm_gport_t gport_array[MAX_PORTS];
    bcm_if_t encap_id_array[MAX_PORTS];
    int port_cnt;
    int i;

    printf("\n-------------------------------------------\n");
    printf("Start macsec_vlan_config() for vlan-%d\n", vlan);
    printf("---------------------------------------------\n");

    BCM_IF_ERROR_RETURN(macsec_vlan_create(unit, vlan, &macsec_vlan->vlan_ipmc_group));

    for (i = 0; i < macsec_vlan->num_of_ports; i++) {
        BCM_IF_ERROR_RETURN(vlan_create_add_port(unit, vlan, macsec_vlan->port[i], untagged));
    }

    /* create remapped_ipmc_group */
    BCM_IF_ERROR_RETURN(bcm_multicast_create(unit, BCM_MULTICAST_TYPE_L3, &macsec_vlan->remapped_ipmc_group));
    int remapped_group = macsec_vlan->remapped_ipmc_group;

    BCM_IF_ERROR_RETURN(bcm_multicast_egress_get(unit, macsec_vlan->vlan_ipmc_group, MAX_PORTS, gport_array, encap_id_array, &port_cnt));

    for (i = 0; i < port_cnt; i++) {
        bcm_port_t port;
        int mod_id = 0;
        if (BCM_GPORT_IS_LOCAL(gport_array[i]))
            port = BCM_GPORT_LOCAL_GET(gport_array[i]);
        else {
            if (BCM_GPORT_IS_MODPORT(gport_array[i])) {
                mod_id = BCM_GPORT_MODPORT_MODID_GET(gport_array[i]);
                port = BCM_GPORT_MODPORT_PORT_GET(gport_array[i]);
            }
        }
        bcm_xflow_macsec_port_info_t port_info;
        BCM_IF_ERROR_RETURN(bcm_xflow_macsec_port_info_get(unit, port, &port_info));
        if (port_info.enable) {
            int j;
            for (j = 0; j < MAX_MACSEC_PORT; j++) {
                if (macsec_port[j].port == port)
                    break;
            }
            if (j == MAX_MACSEC_PORT)
                return BCM_E_NOT_FOUND;

            bcm_if_t    encap_id = -1;
            if (macsec_port[j].egr_obj_id) {
                BCM_IF_ERROR_RETURN(bcm_multicast_egress_object_encap_get(unit, macsec_vlan->vlan_ipmc_group, macsec_port[j].egr_obj_id, &encap_id));
            }
            else
                return BCM_E_NOT_FOUND;

            /* SDK will convert the port to the corresponding MACsec engine port to add to L3_bitmap */
            BCM_IF_ERROR_RETURN(bcm_multicast_egress_add(unit, remapped_group, gport_array[i], encap_id));
        }
        else {
            BCM_IF_ERROR_RETURN(bcm_multicast_egress_add(unit, remapped_group, gport_array[i], encap_id_array[i]));
        }
    }

    printf("Call bcm_multicast_control_set() to remap group 0x%x to 0x%x\n", macsec_vlan->vlan_ipmc_group, remapped_group);
    /* enable remap */
    BCM_IF_ERROR_RETURN(bcm_multicast_control_set(unit, macsec_vlan->vlan_ipmc_group, bcmMulticastRemapXflowMacsec, 1));
    BCM_IF_ERROR_RETURN(bcm_multicast_control_set(unit, macsec_vlan->vlan_ipmc_group, bcmMulticastRemapGroup, remapped_group));

    return BCM_E_NONE;
}


bcm_error_t
macsec_vlan_port_config_sc_sa_setup(int unit, macsec_vlan_port_s *macsec_vlan_port)
{
    int  port_idx = macsec_vlan_port->macsec_port_index;
    int  vlan_idx = macsec_vlan_port->macsec_vlan_index;
    bcm_port_t port = macsec_port[port_idx].port;
    bcm_vlan_t vlan = macsec_vlan[vlan_idx].vid;
    int  pri = isec_sc_tcam_index++;
    int  i;

    printf("\n--------------------------------------------------\n");
    printf("Start macsec_vlan_port_config() for vlan=%d, port=%d\n", vlan, port);
    printf("----------------------------------------------------\n");

    if (macsec_vlan_port->flags & VLAN_PORT_MACSEC_DISABLE_FLAG) {
        macsec_vlan_port->encrypt_sc_id = macsec_port[port_idx].encrypt_disabled_sc_id;
    }
    else {
        BCM_IF_ERROR_RETURN(macsec_secure_chan_create(unit, BCM_XFLOW_MACSEC_DECRYPT, macsec_vlan_port->decrypt_sci, macsec_vlan_port->flags, pri, 0, &macsec_vlan_port->decrypt_sc_id));
        for (i=0; i < 4; i++) {
            BCM_IF_ERROR_RETURN(macsec_secure_assoc_create(unit, BCM_XFLOW_MACSEC_DECRYPT, macsec_vlan_port->decrypt_sc_id, &macsec_vlan_port->decrypt_sa_id[i], i));
        }

        BCM_IF_ERROR_RETURN(macsec_secure_chan_create(unit, BCM_XFLOW_MACSEC_ENCRYPT,
                                                      macsec_vlan_port->encrypt_sci, macsec_vlan_port->flags, 0, port, &macsec_vlan_port->encrypt_sc_id));
        for (i=0; i < 4; i++){
            BCM_IF_ERROR_RETURN(macsec_secure_assoc_create(unit, BCM_XFLOW_MACSEC_ENCRYPT, macsec_vlan_port->encrypt_sc_id, &macsec_vlan_port->encrypt_sa_id[i], i));
        }
    }

    BCM_IF_ERROR_RETURN(assign_egress_sc_by_efp(unit, port, vlan, macsec_vlan_port->encrypt_sc_id, bcmColorGreen, macsec_efp_group, &macsec_vlan_port->efp_encrypt_stat_id[0]));
    BCM_IF_ERROR_RETURN(assign_egress_sc_by_efp(unit, port, vlan, macsec_vlan_port->encrypt_sc_id, bcmColorYellow, macsec_efp_group, &macsec_vlan_port->efp_encrypt_stat_id[1]));
    BCM_IF_ERROR_RETURN(assign_egress_sc_by_efp(unit, port, vlan, macsec_vlan_port->encrypt_sc_id, bcmColorRed, macsec_efp_group, &macsec_vlan_port->efp_encrypt_stat_id[2]));

    return BCM_E_NONE;
}

bcm_error_t
macsec_vlan_port_config_flow_setup(int unit, macsec_vlan_port_s *macsec_vlan_port, bcm_xflow_macsec_policy_id_t policy_id)
{
    int  port_idx = macsec_vlan_port->macsec_port_index;
    int  vlan_idx = macsec_vlan_port->macsec_vlan_index;
    bcm_port_t port = macsec_port[port_idx].port;
    bcm_vlan_t vlan = macsec_vlan[vlan_idx].vid;
    macsec_decrypt_flow_s *flow = &macsec_vlan_port->decrypt_flow;
    int  pri = isec_sp_tcam_index++;

    printf("\n--------------------------------------------------\n");
    printf("Start macsec_vlan_port_config() for vlan=%d, port=%d\n", vlan, port);
    printf("----------------------------------------------------\n");
    /* decryption: per <port,vlan> SP_TCAM flow */
    flow->src_port = port;
    flow->src_port_mask = 0x7f;
    flow->vlan = vlan;
    flow->vlan_mask = 0xfff;
    flow->pri = pri;
    if (macsec_vlan_port->flags & VLAN_PORT_MACSEC_DISABLE_FLAG)
        flow->flags = MANAGEMENT_FLOW_FLAG;
    BCM_IF_ERROR_RETURN(macsec_decrypt_flow_create(unit, flow, policy_id));

    return BCM_E_NONE;
}


/* Create a UDF to match on 2 bytes chunk */
/* offset in bits from the start of L2 header */
bcm_error_t
l2_udf_create(int unit, int offset, bcm_udf_id_t *udf_id)
{
    int rv;
    bcm_udf_chunk_info_t    info;
    bcm_udf_abstract_pkt_format_info_t pkt_format_info;
    int i;
    uint32 bmpChunk=0x01;
    uint32 not_allowed_chunk_bmap;

    rv = bcm_udf_abstract_pkt_format_info_get(unit, bcmUdfAbstractPktFormatL2,
                                              &pkt_format_info);
    if (BCM_SUCCESS(rv))
        printf("pkt_format_info_get succeeded\n");
    else {
        printf("pkt_format_info_get failed\n");
        return rv;
    }
    not_allowed_chunk_bmap = pkt_format_info.chunk_bmap_used |
                             pkt_format_info.unavail_chunk_bmap;

    printf("not_allowed_chunk_bitmap = 0x%x\n", not_allowed_chunk_bmap);
    /* We only need one 2-bytes chunk */
    for (i = 0; i < 16; i++) {
        if ((not_allowed_chunk_bmap & bmpChunk) == 0)
            break;
        else
            bmpChunk = bmpChunk << 1;
    }
    if (i == 16)
        return BCM_E_RESOURCE;
    else
        printf("bmpChunk: 0x%08X\n", bmpChunk);

    bcm_udf_chunk_info_t_init(&info);
    /* 2 bytes after L2 */
    info.offset = offset; /* in bits */
    info.width  = 16; /* in bits */
    info.chunk_bmap = bmpChunk;
    info.abstract_pkt_format = bcmUdfAbstractPktFormatL2;

    rv = bcm_udf_chunk_create(0, NULL, &info, udf_id);
    if (BCM_SUCCESS(rv)) {
        printf("UDF created Successfully: udf_id=%d\n", *udf_id);
    } else {
        printf("UDF Failed to be created: rv:%d\n", rv);
    }
    return rv;
}

bcm_error_t
vfp_udf_setup(int unit)
{
    bcm_field_group_config_t group_cfg;
    bcm_field_entry_t entry;
    bcm_field_stat_t stat_type[]  = { bcmFieldStatBytes, bcmFieldStatPackets };
    bcm_udf_id_t udf_id;
    int stat_id;
    int i;

    l2_udf_create(unit, 112, &udf_id);

    bcm_field_group_config_t_init(&group_cfg);

    group_cfg.priority = 1;  /* different slice */

    BCM_FIELD_QSET_INIT(group_cfg.qset);
    BCM_FIELD_ASET_INIT(group_cfg.aset);
    BCM_FIELD_QSET_ADD(group_cfg.qset, bcmFieldQualifyStageLookup);
    BCM_FIELD_QSET_ADD(group_cfg.qset, bcmFieldQualifyMacSecFlow);
    BCM_IF_ERROR_RETURN(bcm_field_qset_id_multi_set(unit, bcmFieldQualifyUdf, 1,
                                                    udf_id,
                                                    &group_cfg.qset));
    BCM_FIELD_ASET_ADD(group_cfg.aset, bcmFieldActionStatGroup);
    BCM_FIELD_ASET_ADD(group_cfg.aset, bcmFieldActionPrioIntNew);  /* int_pri */
    BCM_FIELD_ASET_ADD(group_cfg.aset, bcmFieldActionDropPrecedence);  /* color */
    group_cfg.mode = bcmFieldGroupModeAuto;
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_cfg));

    /* This is for 2nd pass encryption:
       map SVTAG.pri + SVTAG.cng to int_pri (4 bits to 4 bits direct mapping)
       Also extract pkt_color from the 2-bit field in Svtag
    */
    uint8 data[2];
    uint8 mask[2] = {0xFC, 0x00};  /* 4-bit internal priority and 2-bit color */
    uint8 intpri;
    uint8 color;
    for (i = 0; i < 64; i++) {
        intpri = i >> 2;
        color = i & 0x3;
        if (color == 3)
            continue;
        BCM_IF_ERROR_RETURN(bcm_field_stat_create(unit, group_cfg.group,
                                                  sizeof(stat_type) / sizeof(stat_type[0]), stat_type, &stat_id));
        BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_cfg.group, &entry));
        BCM_IF_ERROR_RETURN( bcm_field_qualify_MacSecFlow(unit, entry, bcmFieldMacSecFlowAfterEncrypt));

        data[0] = i << 2;
        data[1] = 0x00;
        BCM_IF_ERROR_RETURN(bcm_field_qualify_udf(unit, entry, udf_id,
                                                  2, data, mask));
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionPrioIntNew, intpri, 0));
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDropPrecedence, color+1, 0)); /* BCM_FIELD_COLOR_XXX */
        BCM_IF_ERROR_RETURN(bcm_field_entry_stat_attach(unit, entry, stat_id));
        BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit,entry));
    }

    /* No need to set up IFP for 2nd pass decryption
       Because the int_pri will be determined like the decrypted packet is received from the port
    */
    return BCM_E_NONE;
}


bcm_error_t
ifp_udf_setup(int unit)
{
    bcm_field_group_config_t group_cfg;
    bcm_field_entry_t entry;
    bcm_field_stat_t stat_type[]  = { bcmFieldStatBytes, bcmFieldStatPackets };
    bcm_udf_id_t udf_id;
    int stat_id;
    int i;

    l2_udf_create(unit, 112, &udf_id);

    bcm_field_group_config_t_init(&group_cfg);

    group_cfg.priority = 1;  /* different slice */

    BCM_FIELD_QSET_INIT(group_cfg.qset);
    BCM_FIELD_ASET_INIT(group_cfg.aset);
    BCM_FIELD_QSET_ADD(group_cfg.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_cfg.qset, bcmFieldQualifyMacSecFlow);
    BCM_IF_ERROR_RETURN(bcm_field_qset_id_multi_set(unit, bcmFieldQualifyUdf, 1,
                                                    udf_id,
                                                    &group_cfg.qset));
    BCM_FIELD_ASET_ADD(group_cfg.aset, bcmFieldActionStatGroup);
    BCM_FIELD_ASET_ADD(group_cfg.aset, bcmFieldActionPrioIntNew);      /* int_pri */
    BCM_FIELD_ASET_ADD(group_cfg.aset, bcmFieldActionDropPrecedence);  /* color */
    group_cfg.mode = bcmFieldGroupModeAuto;
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_cfg));

    /* This is for 2nd pass encryption:
       map SVTAG.pri + SVTAG.cng to int_pri (4 bits to 4 bits direct mapping)
       Also extract pkt_color from the 2-bit field in Svtag
    */
    uint8 data[2];
    uint8 mask[2] = {0xFC, 0x00};  /* 4-bit internal priority and 2-bit color */
    uint8 intpri;
    uint8 color;
    for (i = 0; i < 64; i++) {
        intpri = i >> 2;
        color = i & 0x3;
        if (color == 3)
            continue;
        BCM_IF_ERROR_RETURN(bcm_field_stat_create(unit, group_cfg.group,
                                                  sizeof(stat_type) / sizeof(stat_type[0]), stat_type, &stat_id));
        BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_cfg.group, &entry));
        BCM_IF_ERROR_RETURN( bcm_field_qualify_MacSecFlow(unit, entry, bcmFieldMacSecFlowAfterEncrypt));

        data[0] = i << 2;
        data[1] = 0x00;
        BCM_IF_ERROR_RETURN(bcm_field_qualify_udf(unit, entry, udf_id,
                                                  2, data, mask));
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionPrioIntNew, intpri, 0));
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDropPrecedence, color+1, 0)); /* BCM_FIELD_COLOR_XXX */
        BCM_IF_ERROR_RETURN( bcm_field_entry_stat_attach(unit, entry, stat_id));
        BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit,entry));
    }

    /* No need to set up IFP for 2nd pass decryption
       Because the int_pri will be determined like the decrypted packet is received from the port
    */
    return BCM_E_NONE;
}


bcm_error_t
ifp_setup(int unit, int dir, int *stat_id)
{
    bcm_field_group_config_t group_cfg;
    bcm_field_entry_t entry;
    bcm_field_stat_t stat_type[]  = { bcmFieldStatBytes, bcmFieldStatPackets };
    int macsec_decrypt_lb_port_offset0 = 67;
    int macsec_decrypt_lb_port_offset1 = 68;
    int my_modid;
    bcm_gport_t gport;
    char str[512];

    bcm_field_group_config_t_init(&group_cfg);

    group_cfg.priority = 2;

    BCM_FIELD_QSET_INIT(group_cfg.qset);
    BCM_FIELD_ASET_INIT(group_cfg.aset);

    BCM_FIELD_QSET_ADD(group_cfg.qset, bcmFieldQualifyStageIngress);

    BCM_FIELD_ASET_ADD(group_cfg.aset, bcmFieldActionStatGroup);
    if (dir == BCM_XFLOW_MACSEC_ENCRYPT) {
        BCM_FIELD_QSET_ADD(group_cfg.qset, bcmFieldQualifyMacSecFlow);
        BCM_FIELD_ASET_ADD(group_cfg.aset, bcmFieldActionMacSecModidBase);
        snprintf(str, 512,"%s","> IFP rule to map DGPP to final DGPP installed (encrypt-2nd pass).");
    } else {
        BCM_FIELD_QSET_ADD(group_cfg.qset, bcmFieldQualifyInterfaceClassPort);
        BCM_FIELD_QSET_ADD(group_cfg.qset, bcmFieldQualifyDrop);
        BCM_FIELD_QSET_ADD(group_cfg.qset, bcmFieldQualifyMacSecFlow);
        BCM_FIELD_ASET_ADD(group_cfg.aset, bcmFieldActionUnmodifiedPacketRedirectPort);
        snprintf(str, 512,"%s","> IFP rule to send MACsec packets to MACsec lb port installed (decrypt-1st pass).");
    }
    BCM_IF_ERROR_RETURN( bcm_field_group_config_create(unit, &group_cfg));

    if (dir == BCM_XFLOW_MACSEC_ENCRYPT) {
        BCM_IF_ERROR_RETURN( bcm_field_stat_create(unit, group_cfg.group,
                                                   sizeof(stat_type)/sizeof(stat_type[0]), stat_type, stat_id));
        BCM_IF_ERROR_RETURN( bcm_field_entry_create(unit, group_cfg.group, &entry));
        /* IFP rule matches encrypted flow and assign the ModidBase to map DGPP (looked up from FDB) to real/final DGPP */
        BCM_IF_ERROR_RETURN( bcm_field_qualify_MacSecFlow(unit, entry, bcmFieldMacSecFlowAfterEncrypt));
        BCM_IF_ERROR_RETURN( bcm_field_action_add(unit, entry, bcmFieldActionMacSecModidBase, 0, 0));
        BCM_IF_ERROR_RETURN( bcm_field_entry_stat_attach(unit, entry, *stat_id));
        BCM_IF_ERROR_RETURN( bcm_field_entry_install(unit, entry));
    } else {
        /* create IFP entry to avoid redirect for 2nd pass decryption */
        BCM_IF_ERROR_RETURN( bcm_field_entry_create(unit, group_cfg.group, &entry));
        BCM_IF_ERROR_RETURN( bcm_field_qualify_MacSecFlow(unit, entry, bcmFieldMacSecFlowAfterDecrypt));
        BCM_IF_ERROR_RETURN( bcm_field_entry_prio_set(unit, entry, 2));  /* higher priority */
        BCM_IF_ERROR_RETURN( bcm_field_entry_install(unit, entry));

        /* Do not match on 0x88e5 to allow for clear text packet pass-through */
        /* Create 1st decryption IFP entry to redirect to port 67 */
        BCM_IF_ERROR_RETURN(bcm_stk_modid_get(unit, &my_modid));
        BCM_GPORT_MODPORT_SET(gport, my_modid, 67);
        BCM_IF_ERROR_RETURN( bcm_field_stat_create(unit, group_cfg.group,
                                                   sizeof(stat_type)/sizeof(stat_type[0]), stat_type, stat_id));
        BCM_IF_ERROR_RETURN( bcm_field_entry_create(unit, group_cfg.group, &entry));

        BCM_IF_ERROR_RETURN( bcm_field_qualify_InterfaceClassPort(unit, entry, macsec_port_class1, BCM_FIELD_EXACT_MATCH_MASK));
        /* not dropped */
        BCM_IF_ERROR_RETURN( bcm_field_qualify_Drop(unit, entry, 0, 1));

        BCM_IF_ERROR_RETURN( bcm_field_action_add(unit, entry, bcmFieldActionUnmodifiedPacketRedirectPort, gport, macsec_decrypt_lb_port_offset0));
        BCM_IF_ERROR_RETURN( bcm_field_entry_stat_attach(unit, entry, *stat_id));
        BCM_IF_ERROR_RETURN( bcm_field_entry_prio_set(unit, entry, 1)); /* lower priority */
        BCM_IF_ERROR_RETURN( bcm_field_entry_install(unit, entry));

        /* create 2nd decryption IFP entry to redirect to port 68 */
        BCM_GPORT_MODPORT_SET(gport, my_modid, 68);
        BCM_IF_ERROR_RETURN( bcm_field_stat_create(unit, group_cfg.group,
                                                   sizeof(stat_type)/sizeof(stat_type[0]), stat_type, stat_id+1));
        BCM_IF_ERROR_RETURN( bcm_field_entry_create(unit, group_cfg.group, &entry));

        BCM_IF_ERROR_RETURN( bcm_field_qualify_InterfaceClassPort(unit, entry, macsec_port_class2, BCM_FIELD_EXACT_MATCH_MASK));
        /* not dropped */
        BCM_IF_ERROR_RETURN( bcm_field_qualify_Drop(unit, entry, 0, 1));

        BCM_IF_ERROR_RETURN( bcm_field_action_add(unit, entry, bcmFieldActionUnmodifiedPacketRedirectPort, gport, macsec_decrypt_lb_port_offset1));
        BCM_IF_ERROR_RETURN( bcm_field_entry_stat_attach(unit, entry, *(stat_id+1)));
        BCM_IF_ERROR_RETURN( bcm_field_entry_prio_set(unit, entry, 1)); /* lower priority */
        BCM_IF_ERROR_RETURN( bcm_field_entry_install(unit, entry));
    }

    printf("%s\n", str);
    return BCM_E_NONE;
}


bcm_error_t
create_efp_group(int unit, bcm_field_group_t *group)
{
    bcm_field_group_config_t group_cfg;
    char str[512];

    /* EFP rule matches MACsec flow to add SVTAG */
    bcm_field_group_config_t_init(&group_cfg);
    BCM_FIELD_QSET_INIT(group_cfg.qset);
    BCM_FIELD_ASET_INIT(group_cfg.aset);

    BCM_FIELD_QSET_ADD(group_cfg.qset, bcmFieldQualifyStageEgress);
    BCM_FIELD_QSET_ADD(group_cfg.qset, bcmFieldQualifyMacSecFlow);
    BCM_FIELD_QSET_ADD(group_cfg.qset, bcmFieldQualifyDstPort);
    BCM_FIELD_QSET_ADD(group_cfg.qset, bcmFieldQualifyOuterVlanId);
    BCM_FIELD_QSET_ADD(group_cfg.qset, bcmFieldQualifyColor);

    BCM_FIELD_ASET_ADD(group_cfg.aset, bcmFieldActionStatGroup);
    BCM_FIELD_ASET_ADD(group_cfg.aset, bcmFieldActionMacSecEncryptPktType);
    BCM_FIELD_ASET_ADD(group_cfg.aset, bcmFieldActionMacSecDecryptPktType);
    BCM_FIELD_ASET_ADD(group_cfg.aset, bcmFieldActionMacSecSubPortNumAdd);
    BCM_FIELD_ASET_ADD(group_cfg.aset, bcmFieldActionMacSecSvtagColor);

    group_cfg.priority = 1;

    BCM_IF_ERROR_RETURN( bcm_field_group_config_create(unit, &group_cfg));
    *group = group_cfg.group;
    printf("efp_group: 0x%x created\n", *group);

    return BCM_E_NONE;
}



bcm_error_t
efp_decryption_setup(int unit, bcm_field_group_t group)
{
    bcm_field_entry_t entry;
    bcm_field_stat_t stat_type[]  = { bcmFieldStatBytes, bcmFieldStatPackets };
    int qset_macsec_flow;
    int aset_pkt_type;
    bcm_xflow_macsec_subport_id_t subport;
    int stat_id;

    BCM_IF_ERROR_RETURN( bcm_field_stat_create(unit, group, sizeof(stat_type) / sizeof(stat_type[0]), stat_type, &stat_id));
    BCM_IF_ERROR_RETURN( bcm_field_entry_create(unit, group, &entry));
    BCM_IF_ERROR_RETURN( bcm_field_qualify_MacSecFlow(unit, entry, bcmFieldMacSecFlowForDecrypt));
    BCM_IF_ERROR_RETURN( bcm_field_action_add(unit, entry, bcmFieldActionMacSecDecryptPktType, bcmFieldMacSecEncryptDecryptPktData, 0));
    BCM_IF_ERROR_RETURN( bcm_field_entry_stat_attach(unit, entry, stat_id));
    BCM_IF_ERROR_RETURN( bcm_field_entry_install(unit, entry));
    printf("efp_decryption_setup completed\n");

    return BCM_E_NONE;
}


bcm_error_t
macsec_init(int unit)
{
    /* IPMC logic required by non-UC traffic */
    /* Enable L3 Egress Mode */
    BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1));
    /* Enable L3IngressMode */
    BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit, bcmSwitchL3IngressMode, 1));
    /* Enable switch ingress intf map set */
    BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit, bcmSwitchL3IngressInterfaceMapSet, 1));

    /* set SW2_IFP_DST_ACTION_CONTROL.SRC_REMOVAL_EN = 1 && HG_SRC_REMOVAL_EN = 1
       to enforce src pruning after IFP redirect */
    BCM_IF_ERROR_RETURN( bcm_field_control_set(unit, bcmFieldControlRedirectExcludeSrcPort, 1));

    /* Enable ipmc */
    BCM_IF_ERROR_RETURN( bcm_ipmc_enable(unit, 1));
    return BCM_E_NONE;
}

int MAX_MACSEC_PORT = 2;
int MAX_MACSEC_VLAN = 3;
int MAX_MACSEC_VLAN_PORT = MAX_MACSEC_PORT*MAX_MACSEC_VLAN;
int MAX_MACSEC_POLICY = 32;

bcm_port_t ingress_port;
bcm_port_t egress_port1;    /* macsec 1 */
bcm_gport_t egress_gport1;

macsec_port_s macsec_port[MAX_MACSEC_PORT];
macsec_vlan_s macsec_vlan[MAX_MACSEC_VLAN];
macsec_vlan_port_s macsec_vlan_port[MAX_MACSEC_VLAN_PORT];
bcm_xflow_macsec_policy_id_t policy_id[MAX_MACSEC_POLICY];


/* Glabal variable definition */
int CPU_PORT = 0;
int MAX_PORTS = 72;
int MAX_QUEUES = 32;
int MAX_CPU_QUEUES = 64;
int MAX_SCH_NODES = 64;
int prev_port = -1;
int next_ucast_index = 0;
int next_mcast_index = 0;
int next_sched_index = 0;
int valid_port_number = 0;

bcm_gport_t ucast_q_gport[MAX_PORTS][MAX_QUEUES];
bcm_gport_t mcast_q_gport[MAX_PORTS][MAX_QUEUES];
bcm_gport_t sched_gport[MAX_PORTS][MAX_SCH_NODES];
bcm_gport_t cpu_mcast_q_gport[MAX_CPU_QUEUES];
bcm_gport_t cpu_sched_gport[MAX_SCH_NODES];

/* TC (internal priority) to Q mapping */
int MAX_TC = 16;
int newUcTcToQMap[] = {7, 6, 5, 4, 3, 2, 1, 0, 7, 6, 5, 4, 3, 2, 1, 0};
int newMcTcToQMap[] = {0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7};

/* Callback function for cosq gport traversal */
static bcm_error_t
cosq_gport_traverse_callback(int unit, bcm_gport_t port,
                             int numq, uint32 flags,
                             bcm_gport_t gport, void *user_data)
{
     int local_port = 0;

     switch (flags) {
         case BCM_COSQ_GPORT_UCAST_QUEUE_GROUP:
             local_port = BCM_GPORT_UCAST_QUEUE_GROUP_SYSPORTID_GET(gport);
             if (prev_port != local_port) {
                 next_ucast_index = 0;
                 next_mcast_index = 0;
                 next_sched_index = 0;
                 prev_port = local_port;
             }
             if (CPU_PORT == local_port) {
                 return BCM_E_PARAM;
             }
             ucast_q_gport[local_port][next_ucast_index] = gport;
             next_ucast_index++;
             break;

         case BCM_COSQ_GPORT_MCAST_QUEUE_GROUP:
             local_port = BCM_GPORT_MCAST_QUEUE_GROUP_SYSPORTID_GET(gport);
             if (prev_port != local_port) {
                 next_ucast_index = 0;
                 next_mcast_index = 0;
                 next_sched_index = 0;
                 prev_port = local_port;
             }
             if (CPU_PORT == local_port) {
                 cpu_mcast_q_gport[next_mcast_index] = gport;
             } else {
                 mcast_q_gport[local_port][next_mcast_index] = gport;
             }
             next_mcast_index++;
             break;

         case BCM_COSQ_GPORT_SCHEDULER:
             if (CPU_PORT == prev_port) {
                 cpu_sched_gport[next_sched_index] = gport;
             } else {
                 sched_gport[local_port][next_sched_index] = gport;
             }
             next_sched_index++;
             break;
         default:
             return BCM_E_PARAM;
     }
    return 1;
}

/*
 * Build queue list for all cosq gports
 */
static bcm_error_t
cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb, void *user_data)
{
    int ret_val = 0;

    ret_val = bcm_cosq_gport_traverse(unit, cb, NULL);
    if (ret_val  < BCM_E_NONE) {
        printf("ERR(%d): bcm_cosq_gport_traverse function unit = %d\n", ret_val, unit);
        return -1;
    }

    next_ucast_index = 0;
    next_mcast_index = 0;
    next_sched_index = 0;
    prev_port = -1;
    return 1;
}


/*
 * Set the TC to queue map using the gport for one port using bcm_cosq_gport_mapping_set()
 */
int port_tc_to_q_mapping_setup(int unit, int port)
{
    int rv = BCM_E_NONE;
    int tci;
    int queue_id = 0;
    int queue_gport = 0;

    /* Tc to Unicast Q mapping set */
    for(tci = 0; tci < MAX_TC; tci++) {
        queue_id = newUcTcToQMap[tci];
        queue_gport = ucast_q_gport[port][queue_id];
        rv = bcm_cosq_gport_mapping_set(unit, port, tci,
                                        BCM_COSQ_GPORT_UCAST_QUEUE_GROUP,
                                        queue_gport, 0);
        if (rv != BCM_E_NONE) {
            printf("Unicast: Unable to map TC to Egress queue (P, TC, Q) = (%d, %d, %x) - %s\n",
                   port, tci, queue_id, bcm_errmsg(rv));
            return rv;
        }
        /* printf("Unicast: Map TC to Egress queue (P, TC, Q) = (%d, %d, %x)\n",
           port, tci, queue_id); */
    }

    /* Tc to Multicast Q mapping set */
    for(tci = 0; tci < MAX_TC; tci++) {
        queue_id = newMcTcToQMap[tci];
        queue_gport = mcast_q_gport[port][queue_id];
        rv = bcm_cosq_gport_mapping_set(unit, port, tci,
                                        BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
                                        queue_gport, 0);
        if (rv != BCM_E_NONE) {
            printf("Multicast: Unable to map TC to Egress queue (P, TC, Q) = (%d, %d, %x) - %s\n",
                   port, tci, queue_id, bcm_errmsg(rv));
            return rv;
        }
        /* printf("Multicast: map TC to Egress queue (P, TC, Q) = (%d, %d, %x)\n",
           port, tci, queue_id); */
    }
    return rv;
}

/*
 * Set the TC to queue map for ingress port
 */
int tc_to_q_mapping_setup(int unit)
{
    int rv = BCM_E_NONE;
    int i = 0;

    rv = port_tc_to_q_mapping_setup(unit, ingress_port);
    if (rv != BCM_E_NONE) {
        printf("port_tc_to_q_mapping_setup() failed on port %d\r\n", ingress_port);
        return rv;
    }

    rv = port_tc_to_q_mapping_setup(unit, 65);
    if (rv != BCM_E_NONE) {
        printf("port_tc_to_q_mapping_setup() failed on port %d\r\n", 65);
        return rv;
    }
    rv = port_tc_to_q_mapping_setup(unit, 66);
    if (rv != BCM_E_NONE) {
        printf("port_tc_to_q_mapping_setup() failed on port %d\r\n", 66);
        return rv;
    }

    rv = port_tc_to_q_mapping_setup(unit, egress_port1);
    if (rv != BCM_E_NONE) {
        printf("port_tc_to_q_mapping_setup() failed on port %d\r\n",egress_port1);
        return rv;
    }

    rv = port_tc_to_q_mapping_setup(unit, 67);
    if (rv != BCM_E_NONE) {
        printf("port_tc_to_q_mapping_setup() failed on port %d\r\n", 67);
        return rv;
    }
    rv = port_tc_to_q_mapping_setup(unit, 68);
    if (rv != BCM_E_NONE) {
        printf("port_tc_to_q_mapping_setup() failed on port %d\r\n", 68);
        return rv;
    }

    return rv;
}


/*
 * Get the TC to queue gport map for one ingress port
 */
int port_tc_to_q_mapping_get(int unit, int port)
{
    int rv = BCM_E_NONE;
    int tc = 0;
    int cosq = -1;
    int queue_gport = 0;

    /* Tc to Unicast Q mapping get */
    for(tc = 0; tc < MAX_TC; tc ++) {
        rv = bcm_cosq_gport_mapping_get(unit, port, tc,
                                        BCM_COSQ_GPORT_UCAST_QUEUE_GROUP,
                                        &queue_gport, &cosq);
        if (rv != BCM_E_NONE) {
            printf("Unicast: Unable to retrieve the cosq of map TC %d of port %d\n",
                   tc, port, bcm_errmsg(rv));
            return rv;
        }
        printf("Unicast: TC %d of port %d was mapped to Egress queue %d\n",
               tc, port, cosq, bcm_errmsg(rv));
    }

    /* Tc to Multicast Q mapping get */
    for(tc = 0; tc < MAX_TC; tc ++) {
        rv = bcm_cosq_gport_mapping_get(unit, port, tc,
                                        BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
                                        &queue_gport, &cosq);
        if (rv != BCM_E_NONE) {
            printf("Multicast: Unable to retrieve the cosq of map TC %d of port %d\n",
                   tc, port, bcm_errmsg(rv));
            return rv;
        }
        printf("Multicast: TC %d of port %d was mapped to Egress queue %d\n",
               tc, port, cosq, bcm_errmsg(rv));
    }
    return rv;
}

/*
 * Get the TC to queue gport map
 */
int tc_to_q_mapping_get(int unit)
{
  int rv = BCM_E_NONE;
  int i = 0;

  printf("==> tc_to_q_mapping_get:\n");

  rv = port_tc_to_q_mapping_get(unit, ingress_port);
  if (rv != BCM_E_NONE) {
      printf("Unable to port_tc_to_q_mapping_get on port %d\r\n", ing_port);
      return rv;
  }
  rv = port_tc_to_q_mapping_get(unit, 65);
  if (rv != BCM_E_NONE) {
      printf("Unable to port_tc_to_q_mapping_get on port %d\r\n", 65);
      return rv;
  }
  return rv;
}


int print_cosq_stat(int unit, bcm_port_t port, int stat, int unicast)
{
    int cos;
    int queue_gport;
    uint64 outpkt;

    for(cos = 0; cos < 8; cos++) {
        if (unicast)
            queue_gport = ucast_q_gport[port][cos];
        else
            queue_gport = mcast_q_gport[port][cos];
        bcm_cosq_stat_get(unit, queue_gport, cos, stat, &outpkt);
        printf("queue gport=0x%x, cos=%d, ", queue_gport, cos);
        print outpkt;
    }
    return BCM_E_NONE;
}


/*
 *  Create a qos_map (if the qMapId is zero) and add a entry to the map
 */
bcm_error_t
qos_map_setup(int unit, unsigned int flags, uint8 int_pri, bcm_color_t clr, uint8 priority, uint8 cfi, int *qMapId)
{
    bcm_qos_map_t qMap;
    int rv;

    /* Reserve hardware resources for QoS mapping */
    if (*qMapId == 0) {
        BCM_IF_ERROR_RETURN(bcm_qos_map_create(unit, flags, qMapId));
        printf("qMapId = 0x%x\n", *qMapId);
    }

    sal_memset(&qMap, 0, sizeof (qMap));
    qMap.int_pri = int_pri;
    qMap.color   = clr;
    qMap.pkt_pri = priority;
    qMap.pkt_cfi = cfi;
    BCM_IF_ERROR_RETURN(bcm_qos_map_add(unit, flags, qMap, *qMapId));
    return BCM_E_NONE;
}


/* Setup L2 qos mapping */
bcm_error_t
qos_port_setup(int unit, bcm_port_t port, unsigned int flags, uint8 int_pri, bcm_color_t clr, uint8 priority, uint8 cfi, int *qMapId)
{
    bcm_gport_t gport;
    int rv;
    int my_modid;

    BCM_IF_ERROR_RETURN(bcm_stk_modid_get(unit, &my_modid));
    rv = qos_map_setup(unit, flags, int_pri, clr, priority, cfi, qMapId);
    if (BCM_FAILURE(rv)) {
        printf("\nError in qos_map_setup: %s\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_GPORT_MODPORT_SET(gport, my_modid, port);

    if(flags & BCM_QOS_MAP_INGRESS)
        BCM_IF_ERROR_RETURN(bcm_qos_port_map_set(unit, gport, *qMapId, -1));
    else
        BCM_IF_ERROR_RETURN(bcm_qos_port_map_set(unit, gport, -1, *qMapId));
    return BCM_E_NONE;
}

/* set up mapping from int_pri to svtag.pri && svtag.cng */
/* Packet Cng is passed to SVTAG by EFP rules */
bcm_error_t
intpri_to_svtag_pri_cng_mapping_setup(int unit)
{
    bcm_qos_map_t qMap;
    bcm_color_t color[3] = {bcmColorGreen, bcmColorYellow, bcmColorRed};
    uint8 int_pri;
    uint8 svtag_pri, svtag_cng;
    int i;

    for (int_pri = 0; int_pri < 16; int_pri++) {
        svtag_pri = int_pri >> 1;
        svtag_cng = int_pri & 0x1;
        /* mapping is independent of color */
        for (i = 0; i < 3; i++) {
            bcm_qos_map_t_init(&qMap);
            qMap.int_pri = int_pri;
            qMap.color = color[i];
            qMap.etag_pcp = svtag_pri;
            qMap.etag_de = svtag_cng;
            BCM_IF_ERROR_RETURN(bcm_qos_map_add(unit, BCM_QOS_MAP_SUBPORT, &qMap, 0));
        }
    }
    return BCM_E_NONE;
}



int vfp_for_2nd_pass_encrypt = 1;

bcm_error_t
config_macsec(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_vlan_t  vid1 = 10, vid2 = 11, vid3 = 12;
    bcm_multicast_t vlan_ipmc_group;

    BCM_IF_ERROR_RETURN(macsec_init(unit));

    /* IFP rule for Decryption */
    BCM_IF_ERROR_RETURN(ifp_setup(unit, BCM_XFLOW_MACSEC_DECRYPT, ifp_decrypt_stat_id));

    /* IFP rule for Encryption */
    BCM_IF_ERROR_RETURN(ifp_setup(unit, BCM_XFLOW_MACSEC_ENCRYPT, &ifp_encrypt_stat_id));

    if (vfp_for_2nd_pass_encrypt)
        BCM_IF_ERROR_RETURN(vfp_udf_setup(unit));
    else
        BCM_IF_ERROR_RETURN(ifp_udf_setup(unit));

    uint64 tmp;
    COMPILER_64_SET(tmp, 0x0, 0x1);
    /* Drop failed MACsec packets */
    BCM_IF_ERROR_RETURN(bcm_xflow_macsec_control_set(unit, BCM_XFLOW_MACSEC_ENCRYPT, 0, bcmXflowMacsecControlEncryptFailDrop, tmp));
    BCM_IF_ERROR_RETURN(bcm_xflow_macsec_control_set(unit, BCM_XFLOW_MACSEC_DECRYPT, 0, bcmXflowMacsecControlDecryptFailDrop, tmp));

    bcm_xflow_macsec_mtu_t mtu_sel;
    mtu_sel = bcmXflowMacsecMtu0;
    BCM_IF_ERROR_RETURN(bcm_xflow_macsec_mtu_set(
                            unit, BCM_XFLOW_MACSEC_ENCRYPT | BCM_XFLOW_MACSEC_MTU_WITH_ID,
                            0, 1514, &mtu_sel));
    mtu_sel = bcmXflowMacsecMtu0;
    BCM_IF_ERROR_RETURN(bcm_xflow_macsec_mtu_set(
                            unit, BCM_XFLOW_MACSEC_DECRYPT | BCM_XFLOW_MACSEC_MTU_WITH_ID,
                            0, 1514, &mtu_sel));

    BCM_IF_ERROR_RETURN(l3_intf_create(unit, dummy_vid, dummy_mac, 0, &dummy_intf_id));

    /* Create MACsec efp group */
    BCM_IF_ERROR_RETURN(create_efp_group(unit, &macsec_efp_group));

    /* Create one EFP rule for decryption */
    BCM_IF_ERROR_RETURN(efp_decryption_setup(unit, macsec_efp_group));

    if (BCM_FAILURE((rv = intpri_to_svtag_pri_cng_mapping_setup(unit)))) {
        printf("MACsec Setup Failed %s.\n", bcm_errmsg(rv));
        return -1;
    }

    /* initialize data structures */
    printf("Configure macsec_port[0]\n");
    macsec_port_s_init(&macsec_port[0]);
    macsec_port[0].port = egress_port1;
    macsec_port[0].macsec_port_class = macsec_port_class1;
    macsec_port[0].encrypt_port_offset = 0;

    BCM_IF_ERROR_RETURN(macsec_port_config(unit, &macsec_port[0]));

    printf("Configure macsec_vlan[0]\n");
    macsec_vlan_s_init(&macsec_vlan[0]);
    macsec_vlan[0].vid = vid1;
    macsec_vlan[0].port[0] = ingress_port;
    macsec_vlan[0].port[1] = egress_port1;
    macsec_vlan[0].num_of_ports = 2;
    BCM_IF_ERROR_RETURN(macsec_vlan_config(unit, &macsec_vlan[0]));

    printf("Configure macsec_vlan[1]\n");
    macsec_vlan_s_init(&macsec_vlan[1]);
    macsec_vlan[1].vid = vid2;
    macsec_vlan[1].port[0] = ingress_port;
    macsec_vlan[1].port[1] = egress_port1;
    macsec_vlan[1].num_of_ports = 2;
    BCM_IF_ERROR_RETURN(macsec_vlan_config(unit, &macsec_vlan[1]));

    printf("Configure macsec_vlan[2]\n");
    macsec_vlan_s_init(&macsec_vlan[2]);
    macsec_vlan[2].vid = vid3;
    macsec_vlan[2].port[0] = ingress_port;
    macsec_vlan[2].port[1] = egress_port1;
    macsec_vlan[2].num_of_ports = 2;
    BCM_IF_ERROR_RETURN(macsec_vlan_config(unit, &macsec_vlan[2]));

    uint64 zero_sci;
    COMPILER_64_SET(zero_sci, 0x0, 0x0);
    /* Configure macsec_policy_id */
    BCM_IF_ERROR_RETURN(macsec_decrypt_policy_create(unit, zero_sci,
                                                     BCM_XFLOW_MACSEC_DECRYPT_POLICY_TAGGED_CONTROL_PORT_ENABLE |
                                                     BCM_XFLOW_MACSEC_DECRYPT_POLICY_UNTAGGED_CONTROL_PORT_ENABLE,
                                                     0, bcmXflowMacsecMtu0, &policy_id[0]));
    /*
       Note that we use loopback to test MACsec decryption.
       Therefore, decrypt_sci equals encrypt_sci.
    */
    printf("Configure SC/SA for macsec_vlan_port[0]\n");
    uint64 encrypt_sci;
    uint64 decrypt_sci;
    COMPILER_64_SET(encrypt_sci, 0x0, 0x8);
    COMPILER_64_SET(decrypt_sci, 0x0, 0x8);
    macsec_vlan_port_s_init(&macsec_vlan_port[0]);
    macsec_vlan_port[0].macsec_port_index = 0;    /* macsec_port[0] */
    macsec_vlan_port[0].macsec_vlan_index = 0;    /* macsec_vlan[0] */
    macsec_vlan_port[0].encrypt_sci = encrypt_sci;
    macsec_vlan_port[0].decrypt_sci = decrypt_sci;
    BCM_IF_ERROR_RETURN(macsec_vlan_port_config_sc_sa_setup(unit, &macsec_vlan_port[0]));

    /*  Bind MACsec <policy, sci> to SC */
    bcm_xflow_macsec_secure_chan_id_t sc_id;
    sc_id = macsec_vlan_port[0].decrypt_sc_id;
    BCM_IF_ERROR_RETURN(macsec_secure_chan_decrypt_policy_bind(unit, sc_id, policy_id[0]));

    printf("Configure decrypt flow for macsec_vlan_port[0]\n");
    BCM_IF_ERROR_RETURN(macsec_vlan_port_config_flow_setup(unit, &macsec_vlan_port[0], policy_id[0]));

    printf("Configure SC/SA for macsec_vlan_port[1]\n");
    COMPILER_64_SET(encrypt_sci, 0x0, 0x9);
    COMPILER_64_SET(decrypt_sci, 0x0, 0x9);
    macsec_vlan_port_s_init(&macsec_vlan_port[1]);
    macsec_vlan_port[1].macsec_port_index = 0;     /* macsec_port[0] */
    macsec_vlan_port[1].macsec_vlan_index = 1;     /* macsec_vlan[1] */
    macsec_vlan_port[1].encrypt_sci = encrypt_sci;
    macsec_vlan_port[1].decrypt_sci = decrypt_sci;
    BCM_IF_ERROR_RETURN(macsec_vlan_port_config_sc_sa_setup(unit, &macsec_vlan_port[1]));

    /*  Bind MACsec <policy, sci> to SC */
    sc_id = macsec_vlan_port[1].decrypt_sc_id;
    BCM_IF_ERROR_RETURN(macsec_secure_chan_decrypt_policy_bind(unit, sc_id, policy_id[0]));

    printf("Configure decrypt flow for macsec_vlan_port[1]\n");
    BCM_IF_ERROR_RETURN(macsec_vlan_port_config_flow_setup(unit, &macsec_vlan_port[1], policy_id[0]));

    /* Configure vlan 12, port 1 as MACsec disabled */
    printf("Configure SC/SA for macsec_vlan_port[2]\n");
    COMPILER_64_SET(encrypt_sci, 0x0, 0x0);
    COMPILER_64_SET(decrypt_sci, 0x0, 0x0);
    macsec_vlan_port_s_init(&macsec_vlan_port[2]);
    macsec_vlan_port[2].macsec_port_index = 0;     /* macsec_port[0] */
    macsec_vlan_port[2].macsec_vlan_index = 2;     /* macsec_vlan[2] */
    macsec_vlan_port[2].encrypt_sci = encrypt_sci;
    macsec_vlan_port[2].decrypt_sci = decrypt_sci;
    macsec_vlan_port[2].flags = VLAN_PORT_MACSEC_DISABLE_FLAG;
    BCM_IF_ERROR_RETURN(macsec_vlan_port_config_sc_sa_setup(unit, &macsec_vlan_port[2]));

    printf("Configure management flow for macsec_vlan_port[2]\n");
    /* SDK will check policy_id even for Management flow */
    BCM_IF_ERROR_RETURN(macsec_vlan_port_config_flow_setup(unit, &macsec_vlan_port[2], policy_id[0]));

    return rv;
}


/************************* Sub-routines for verification ********************************/

/*
 * This function is written so that hardcoding of port
 * numbers in Cint scripts is removed. This function gives
 * required number of ports
 */
bcm_error_t
portNumbersGet(int unit, int *port_list, int num_ports)
{
    int i = 0, port = 0, rv = 0;
    bcm_port_config_t configP;
    bcm_pbmp_t ports_pbmp;

    rv = bcm_port_config_get(unit, &configP);
    if (BCM_FAILURE(rv)) {
        printf("\nError in retrieving port configuration: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    ports_pbmp = configP.e;
    for (i= 1; i < BCM_PBMP_PORT_MAX; i++) {
        if (BCM_PBMP_MEMBER(&ports_pbmp, i) && (port < num_ports)) {
            port_list[port] = i;
            port++;
        }
    }

    if ((0 == port) || (port != num_ports)) {
        printf("portNumbersGet() failed \n");
        return -1;
    }

    return BCM_E_NONE;
}

/* FP Group/Rule Creation/Destruction
*/

bcm_field_group_config_t    _group_config;

bcm_error_t
create_vfp_group (int unit, bcm_field_group_config_t *group_config)
{
    bcm_error_t rv = BCM_E_NONE;

    /* FP group configuration and creation */
    bcm_field_group_config_t_init(group_config);

    BCM_FIELD_QSET_INIT(group_config->qset);
    BCM_FIELD_QSET_ADD(group_config->qset, bcmFieldQualifyStageLookup);
    BCM_FIELD_QSET_ADD(group_config->qset, bcmFieldQualifyInPort);

    group_config->mode = bcmFieldGroupModeAuto;

    rv = bcm_field_group_config_create(unit, group_config);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_field_group_config_create() : %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

bcm_error_t
create_vfp_to_cpu_rule (int unit, bcm_field_group_t gid, bcm_field_entry_t *eid, bcm_port_t port)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_port_t port_mask = 0xffffffff;

    /* FP entry configuration and creation */
    rv = bcm_field_entry_create(unit, gid, eid);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_field_entry_create() : %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_qualify_InPort(unit, *eid, port, port_mask);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_field_qualify_InPort() : %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* FP entry actions configuration */
    rv = bcm_field_action_add(unit, *eid, bcmFieldActionDrop, 0, 0);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_field_action_add() : %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_action_add(unit, *eid, bcmFieldActionCopyToCpu, 0, 0);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_field_action_add() : %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Installing FP entry to FP TCAM */
    rv = bcm_field_entry_install(unit, *eid);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_field_entry_install() : %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

bcm_error_t
destroy_vfp_to_cpu_rule (int unit, bcm_field_entry_t eid)
{
    bcm_error_t rv = BCM_E_NONE;

    /* Destroying FP entry */
    rv = bcm_field_entry_destroy(unit, eid);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_field_entry_destroy() : %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

void
port_discard(int unit, int port, int discard)
{
    int mode = BCM_PORT_DISCARD_NONE;
    int rv;
    if (discard) {
        mode = BCM_PORT_DISCARD_ALL;
    }
    rv = bcm_port_discard_set(unit, port, mode);
    if (BCM_FAILURE(rv)) {
        print "Cannot set port discard!";
    }
}


int rx_count;   /* Global received packet count */
int test_failed; /*Final result will be stored here */

uint32 packet_patterns;
unsigned char *expected_packets[2];
bcm_pbmp_t expected_pbmp[2];

/*
 * Use a priority above 100 to be able to co-exist with diagnostic
 * shell packetWatcher.
 */
const int   rxcb_priority = 101;

/* Rx callback function */
bcm_rx_t
rxCallback(int unit, bcm_pkt_t *packet, void *cookie)
{
    int                 i, *count = (auto) cookie;
    uint8               buffer[1024];;
    uint32              length;
    bcm_port_t          src_port;

    src_port = packet->src_port;
    length = packet->pkt_len;
    printf("RxCallback: received from port 0x%x, length=0x%x\n", src_port, length);

    sal_memcpy(buffer, packet->pkt_data->data, length);
    for (i = 0; i < packet_patterns; i++) {
        if (sal_memcmp(buffer, expected_packets[i], length) == 0) {

            if (BCM_PBMP_MEMBER(expected_pbmp[i], src_port)) {
                (*count)++; /* bump up the rx count */
                printf("RxCallback: rx_count = 0x%x\n", *count);

                /* remove from bitmap to make sure no duplicates */
                BCM_PBMP_PORT_REMOVE(expected_pbmp[i], src_port);
            }
            else
                test_failed = 1;  /* dupicate or unexpected egress ports */
            break;
        }
    }
    if (i == packet_patterns)
        test_failed = 1;   /* an unexpected packet */

    return BCM_RX_NOT_HANDLED;
}


/* Register callback function for received packets */
bcm_error_t
registerRxCallback(int unit)
{
    int         rv;
    /* Receive packets from all CPU queues. */
    const int   flags = BCM_RCO_F_ALL_COS;

    if (!bcm_rx_active(unit)) {
        /*
         * If necessary, initialize and start the receive task.
         */
        rv = bcm_rx_init(unit);
        if(BCM_FAILURE(rv)) {
            printf("\nError in bcm_rx_init(): %s.\n",bcm_errmsg(rv));
            return rv;
        }

        rv = bcm_rx_start(unit, NULL);
        if(BCM_FAILURE(rv)) {
            printf("\nError in bcm_rx_start(): %s.\n",bcm_errmsg(rv));
            return rv;
        }
        printf("Started the Rx task\n");
    }

    rv = bcm_rx_register(unit, "rxCallback", rxCallback, rxcb_priority, &rx_count, flags);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_rx_register(): %s.\n",bcm_errmsg(rv));
        return rv;
    }
    printf("Registered Packet callback\n");

    return BCM_E_NONE;
}

/*
 * Test Setup:
 * This functions gets the port numbers and sets up ingress and
 * egress ports.
 */
bcm_error_t test_setup(int unit)
{
    int i = 0;
    int port_num = 2;
    int port[port_num];
    bcm_error_t rv;
    int ing_map = 0;
    int egr_map = 0;

    BCM_IF_ERROR_RETURN(bcm_stk_modid_set(unit, mod_id));

    if (BCM_E_NONE != portNumbersGet(unit, port, port_num)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    /* Put all egress ports into loopback */
    for (i = 0; i < port_num; i++) {
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port[i], BCM_PORT_LOOPBACK_MAC));
    }

    /* Create FP group */
/*
    BCM_IF_ERROR_RETURN(create_vfp_group(unit, &_group_config));
*/
    /* Start the packet watcher (quiet mode) */
    bshell(unit, "pw start report +raw");

    ingress_port = port[0];
    egress_port1 = port[1];

    printf("ingress_port =%d\n",ingress_port);
    printf("egress_port1 =%d\n",egress_port1);

    BCM_GPORT_MODPORT_SET(egress_gport1, mod_id, egress_port1);

    /* 9-bit interface class id will mark ingress traffic eligible for the IFP rule */
    /* non-MACsec ports: class 0
       MACsec ports: class 1
    */
    for (i = 0; i < port_num; i++) {
        rv = bcm_port_class_set(unit, port[i], bcmPortClassFieldIngress,
                                non_macsec_port_class);
        if (BCM_FAILURE(rv)) {
            printf("bcm_port_class_set() failed. %s\n", bcm_errmsg(rv));
            return rv;
        }
    }

    /* Add ingress priority mapping:
       map {packet priority + CFI} to {int_pri, Green}
       and assign the map to ingress_port
    */
    if (BCM_E_NONE != qos_port_setup(unit, ingress_port, BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_L2,
                                     int_pri, pkt_color, ing_pkt_pri, 0, &ing_map)) {
        printf("qos_port_setup() failed for port %d\n", ingress_port);
        return -1;
    }

    if (BCM_E_NONE != qos_port_setup(unit, egress_port1, BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_L2,
                                     int_pri, pkt_color, ing_pkt_pri, 0, &ing_map)) {
        printf("qos_port_setup() failed for port %d\n", ingress_port);
        return -1;
    }

    /* Add egress priority mapping:
       map {int_pri, Green} to {egress packet priority + CFI}
       and assign the map to egress_port
       BCM_QOS_MAP_L2 flag will set up packet dot1p priority
       BCM_QOS_MAP_SUBPORT flag will set up SVTAG.PRI
     */
    if (BCM_E_NONE != qos_port_setup(unit, egress_port1, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_L2,
                                     int_pri, pkt_color, egr_pkt_pri, 0, &egr_map)) {
        printf("qos_port_setup() failed for port %d\n", egress_port1);
        return -1;
    }

    if (BCM_E_NONE != qos_port_setup(unit, ingress_port, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_L2,
                                     int_pri, pkt_color, egr_pkt_pri, 0, &egr_map)) {
        printf("qos_port_setup() failed for port %d\n", egress_port1);
        return -1;
    }

    /* Build queue list for all cosq gports */
    rv = cosq_gport_traverse(unit, cosq_gport_traverse_callback, NULL);
    if (BCM_FAILURE(rv)) {
        printf("cosq_gport_traverse() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Set the mapping from TC to queue via bcm_cosq_gport_mapping_set */
    rv = tc_to_q_mapping_setup(unit);
    if (BCM_FAILURE(rv)) {
       printf("tc_to_q_mapping_setup() failed.\n");
       return rv;
    }

    if (BCM_FAILURE((rv = config_macsec(unit)))) {
        printf("MACsec Setup Failed %s.\n", bcm_errmsg(rv));
        return -1;
    }

    if (BCM_E_NONE != registerRxCallback(unit)) {
        printf("registerRxCallback() FAILED\n");
        return -1;
    }

    return BCM_E_NONE;
}


bcm_error_t
macsec_get_stats(int unit, int dir, bcm_xflow_macsec_id_t id,
                 bcm_xflow_macsec_stat_type_t stat_type, uint64 *val)
{
    BCM_IF_ERROR_RETURN(bcm_xflow_macsec_stat_get(unit,
                                                  dir, id, stat_type, val));
    return BCM_E_NONE;
}


/* Expected Egress packets */
unsigned char expected_encrypted_packet1[512] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x81, 0x00,
    0x00, 0x0a, 0x88, 0xe5, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x08, 0x8b, 0x94, 0x0d, 0x0a, 0x42, 0x3e, 0xf6, 0x7e, 0x03, 0x92,
    0x30, 0xce, 0x0b, 0xbf, 0x27, 0xaa, 0x54, 0x42, 0xde, 0x93, 0x9c, 0x35, 0xad, 0x51,
    0x0d, 0x5c, 0x0b, 0x0d, 0xfb, 0x18, 0x97, 0x11, 0x7e, 0x6d, 0x6f, 0xe5, 0x75, 0xe1,
    0x21, 0x2b, 0x0a, 0x46, 0x74, 0x54, 0x4e, 0xb0, 0xec, 0xab, 0x8d, 0x54, 0xd2, 0xc0,
    0xbe, 0x7a, 0x35, 0x27, 0xf5, 0x6e, 0x8c, 0x63, 0x34, 0x5b, 0x8a, 0x02, 0xf1, 0x35,
    0xe7, 0x3d, 0x89, 0x9e, 0xa4, 0x23};

unsigned char expected_encrypted_packet2[512] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x81, 0x00,
    0x00, 0x0a, 0x88, 0xe5, 0x2d, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x08, 0x8b, 0x6e, 0x39, 0x5f, 0xd9, 0x2c, 0x63, 0x68, 0x35, 0xb7,
    0x98, 0x9a, 0x1e, 0x2b, 0x36, 0x57, 0x66, 0x0a, 0x3a, 0x0b, 0x33, 0x84, 0xad, 0x0a,
    0x39, 0x2f, 0xf9, 0x97, 0x4c, 0xb9, 0xd1, 0xdd, 0x89, 0x13, 0x30, 0xc6, 0xe7, 0x38,
    0x8d, 0xda, 0x8b, 0x87, 0x95, 0xde, 0x3b, 0x21, 0xd2, 0xc6, 0xfe, 0x30, 0x2c, 0x8e,
    0x79, 0xd7, 0x48, 0x84, 0xf2, 0xc7, 0x6b, 0x06, 0x6f, 0xa2, 0xa3, 0x2e, 0x9c, 0xaf,
    0xf7, 0xb5, 0x0b, 0x91, 0x12, 0x32};

unsigned char expected_encrypted_packet3[512] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x81, 0x00,
    0x00, 0x0b, 0x88, 0xe5, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x09, 0x79, 0x9d, 0x9f, 0x71, 0xc9, 0x56, 0x27, 0x59, 0xa0, 0x07,
    0xa1, 0xc7, 0x3e, 0xa1, 0x4e, 0x27, 0xe6, 0xae, 0x44, 0x92, 0xe6, 0x59, 0x45, 0x38,
    0xe8, 0x18, 0x65, 0x2d, 0xe6, 0x69, 0x94, 0xa7, 0xdf, 0x18, 0x39, 0xab, 0xf1, 0xa1,
    0xee, 0x9f, 0x96, 0x68, 0xa9, 0xae, 0x26, 0x50, 0x56, 0xc3, 0x7b, 0x06, 0xcb, 0xee,
    0xd0, 0x26, 0x07, 0xb1, 0xb6, 0x3f, 0x11, 0x48, 0x5c, 0x45, 0x7d, 0x98, 0x8a, 0xe5,
    0xaa, 0x17, 0x6b, 0x43, 0xd6, 0xa6};

unsigned char expected_encrypted_mc_packet1[512] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x81, 0x00,
    0x00, 0x0a, 0x88, 0xe5, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x08, 0x21, 0xb3, 0xe4, 0x22, 0x8c, 0x2e, 0xb5, 0xd4, 0x18, 0x56,
    0xb8, 0xc1, 0x54, 0x27, 0xa3, 0x33, 0x2e, 0xda, 0x04, 0x44, 0xe2, 0xc9, 0x1b, 0x1f,
    0xb6, 0xcb, 0x25, 0x71, 0x23, 0x31, 0x00, 0xa1, 0x8f, 0x9f, 0x36, 0x96, 0x2c, 0xdb,
    0xa6, 0xb1, 0x4a, 0x9a, 0x13, 0x4c, 0xd3, 0xb0, 0xab, 0x37, 0x38, 0x70, 0x9f, 0x0c,
    0x5c, 0x48, 0xa6, 0x82, 0xad, 0x64, 0xb5, 0xa2, 0x12, 0xcb, 0xe5, 0x9f, 0xc4, 0x1a,
    0xc5, 0x61, 0x09, 0x9b, 0xcf, 0x08};

unsigned char expected_decrypted_packet1[512] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x81, 0x00,
    0x00, 0x0a, 0xff, 0xff, 0x45, 0x78, 0x00, 0x2e, 0x00, 0x00, 0x00, 0x00, 0x40, 0xff,
    0x84, 0x57, 0x14, 0x00, 0x00, 0x01, 0xe1, 0x00, 0x00, 0x01, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25,
    0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2d, 0x0a, 0x03, 0xab, 0x09, 0x11,
    0x56, 0xa1};

unsigned char expected_decrypted_packet2[512] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x81, 0x00,
    0x00, 0x0b, 0xff, 0xff, 0x45, 0x78, 0x00, 0x2e, 0x00, 0x00, 0x00, 0x00, 0x40, 0xff,
    0x84, 0x57, 0x14, 0x00, 0x00, 0x01, 0xe1, 0x00, 0x00, 0x01, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25,
    0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2d, 0x0a, 0x03, 0xab, 0x48, 0xa1,
    0x8e, 0xdf};

unsigned char expected_egress_passthrough_packet1[512] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x81, 0x00,
    0x00, 0x0c, 0xff, 0xff, 0x45, 0x78, 0x00, 0x2e, 0x00, 0x00, 0x00, 0x00, 0x40, 0xff,
    0x84, 0x57, 0x14, 0x00, 0x00, 0x01, 0xe1, 0x00, 0x00, 0x01, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25,
    0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2d, 0x0a, 0x03, 0xab, 0xce, 0xb6,
    0xf4, 0x7c};

unsigned char expected_ingress_passthrough_packet1[512] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x81, 0x00,
    0x00, 0x0c, 0xff, 0xff, 0x45, 0x78, 0x00, 0x2e, 0x00, 0x00, 0x00, 0x00, 0x40, 0xff,
    0x84, 0x57, 0x14, 0x00, 0x00, 0x01, 0xe1, 0x00, 0x00, 0x01, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25,
    0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2d, 0x0a, 0x03, 0xab, 0xce, 0xb6,
    0xf4, 0x7c};

/*
 *
 * Verify behavior by sending packets from CPU -> ingress port(loopback) -> egress_port(loopback) -> CPU.
 *
 */
bcm_error_t
test_verify(int unit)
{
    char uc_pkt[512];
    char uc_enc_pkt_1[512];
    char uc_enc_pkt_2[512];
    char uc_enc_pkt_3[512];
    char mc_pkt[512];
    bcm_field_entry_t   eid1;
    int expected_rx_count;
    int pass1, pass2, pass3, pass4, pass5, pass6, pass7, pass8, pass9;
    char str[512];

    snprintf(uc_pkt, 512,"%s","0000000000220000000000018100000Affff4578002E0000000040FF845714000001E10000011415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2D0A03AB");
    snprintf(uc_enc_pkt_1, 512,"%s","0000000000220000000000018100000a88e52c000000000500000000000000088b940d0a423ef67e039230ce0bbf27aa5442de939c35ad510d5c0b0dfb1897117e6d6fe575e1212b0a4674544eb0ecab8d54d2c0be7a3527f56e8c63345b8a02f135e73d");

    snprintf(uc_enc_pkt_2, 512,"%s","0000000000220000000000018100000a88e52d000000000500000000000000088b6e395fd92c636835b7989a1e2b3657660a3a0b3384ad0a392ff9974cb9d1dd891330c6e7388dda8b8795de3b21d2c6fe302c8e79d74884f2c76b066fa2a32e9caff7b5");

    snprintf(uc_enc_pkt_3, 512,"%s","0000000000220000000000018100000b88e52c00000000050000000000000009799d9f71c9562759a007a1c73ea14e27e6ae4492e6594538e818652de66994a7df1839abf1a1ee9f9668a9ae265056c37b06cbeed02607b1b63f11485c457d988ae5aa17");

    bshell(unit, "e; vlan show");
    bshell(unit, "e; l2 show; e");

    print "==========  Test1: MACsec Encryption - Unicast ==================";
    port_discard(unit, ingress_port, 0);
    port_discard(unit, egress_port1, 1);
    snprintf(str, 512, "l2 add module=%d port=%d mac=00:00:00:00:00:22 vlan=10 ReplacePriority=f static=t", mod_id, egress_port1);
    bshell(unit, str);
    printf(">>> Input Unicast packet @ port-%d:\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x%s; sleep quiet 1", ingress_port, uc_pkt);

    printf(">>> Output MACsec encrypted packet with SCI-x @ port-%d:\n", egress_port1);
    bshell(0,"dmirror 2 mode=ingress dp=cpu0");

    test_failed = 0;
    rx_count = 0;
    expected_rx_count = 1;
    packet_patterns = 1;
    expected_packets[0]  = expected_encrypted_packet1;
    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port1);
    bshell(unit, str);
    bshell(0,"dmirror 2 mode=off");

    bshell(unit, "sleep quiet 1");
    pass1 = (test_failed == 0) && (rx_count == expected_rx_count);
    printf("\n----------------------------------------------------------- \n");
    printf("Test1 = [%s]", pass1? "PASS":"FAIL");
    printf("\n----------------------------------------------------------- \n");

    uint64 val;
    macsec_get_stats(unit, BCM_XFLOW_MACSEC_ENCRYPT, macsec_vlan_port[0].encrypt_sc_id,
                     bcmXflowMacsecSecyTxSCStatsEncryptedPkts, &val);
    printf("Encrypted pkt count:\n");
    print val;
    printf("\n");
    macsec_get_stats(unit, BCM_XFLOW_MACSEC_DECRYPT, macsec_vlan_port[0].decrypt_sc_id,
                     bcmXflowMacsecSecyRxSCStatsOKPkts, &val);
    printf("Decrypted pkt count:\n");
    print val;
    printf("\n");

    /*Initalize the mc_pkt */
    snprintf(mc_pkt, 512,"%s","FFFFFFFFFFFF0000000000018100000Affff4578002E0000000040FF845714000001E10000011415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2D0A03AB");
    print "==========  Test2: MACsec Encryption - BC/DLF ==================";
    port_discard(unit, ingress_port, 0);
    port_discard(unit, egress_port1, 1);
    printf(">>> Input BC/DLF packet @ port-%d:\n", ingress_port);
    printf(">>> Output MACsec encrypted packet with SCI-x @ port-%d:\n", egress_port1);
    bshell(0,"dmirror 2 mode=ingress dp=cpu0");
    snprintf(str, 512, "tx 1 pbm=%d data=0x%s; sleep quiet 1", ingress_port, mc_pkt);

    test_failed = 0;
    rx_count = 0;
    expected_rx_count = 1;
    packet_patterns = 1;
    expected_packets[0]  = expected_encrypted_mc_packet1;
    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port1);
    bshell(unit, str);
    bshell(0,"dmirror 2 mode=off");
    bshell(unit, "sleep quiet 1");
    pass2 = (test_failed == 0) && (rx_count == expected_rx_count);
    printf("\n----------------------------------------------------------- \n");
    printf("Test2 = [%s]", pass2? "PASS":"FAIL");
    printf("\n----------------------------------------------------------- \n");

    macsec_get_stats(unit, BCM_XFLOW_MACSEC_ENCRYPT, macsec_vlan_port[0].encrypt_sc_id,
                     bcmXflowMacsecSecyTxSCStatsEncryptedPkts, &val);
    printf("Encrypted pkt count:\n");
    print val;
    printf("\n");
    macsec_get_stats(unit, BCM_XFLOW_MACSEC_DECRYPT, macsec_vlan_port[0].decrypt_sc_id,
                     bcmXflowMacsecSecyRxSCStatsOKPkts, &val);
    printf("Decrypted pkt count:\n");
    print val;
    printf("\n");

    /* Test manual switch to a new egress SA */
    bcm_xflow_macsec_secure_chan_info_t chan_info;
    int priority;
    bcm_xflow_macsec_secure_chan_info_t_init(&chan_info);
    BCM_IF_ERROR_RETURN(bcm_xflow_macsec_secure_chan_get(unit, macsec_vlan_port[0].encrypt_sc_id, &chan_info, &priority));
    chan_info.active_an++;
    BCM_IF_ERROR_RETURN(bcm_xflow_macsec_secure_chan_set(unit, BCM_XFLOW_MACSEC_ENCRYPT, macsec_vlan_port[0].encrypt_sc_id, &chan_info, priority));

    print "==========  Test3: MACsec Encryption - Unicast (after manual SA switch) ==================";
    port_discard(unit, ingress_port, 0);
    port_discard(unit, egress_port1, 1);
    printf(">>> Input Unicast packet @ port-%d:\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x%s; sleep quiet 1", ingress_port, uc_pkt);

    printf(">>> Output MACsec encrypted packet with SCI-x @ port-%d:\n", egress_port1);
    bshell(0,"dmirror 2 mode=ingress dp=cpu0");
    test_failed = 0;
    rx_count = 0;
    expected_rx_count = 1;
    packet_patterns = 1;
    expected_packets[0]  = expected_encrypted_packet2;
    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port1);
    bshell(unit, str);

    bshell(0,"dmirror 2 mode=off");
    bshell(unit, "sleep quiet 1");
    pass3 = (test_failed == 0) && (rx_count == expected_rx_count);
    printf("\n----------------------------------------------------------- \n");
    printf("Test3 = [%s]", pass3? "PASS":"FAIL");
    printf("\n----------------------------------------------------------- \n");

    macsec_get_stats(unit, BCM_XFLOW_MACSEC_ENCRYPT, macsec_vlan_port[0].encrypt_sc_id,
                     bcmXflowMacsecSecyTxSCStatsEncryptedPkts, &val);
    printf("Encrypted pkt count:\n");
    print val;
    printf("\n");
    macsec_get_stats(unit, BCM_XFLOW_MACSEC_DECRYPT, macsec_vlan_port[0].decrypt_sc_id,
                     bcmXflowMacsecSecyRxSCStatsOKPkts, &val);
    printf("Decrypted pkt count:\n");
    print val;
    printf("\n");

    /* Manually delete the old SA */
    BCM_IF_ERROR_RETURN(bcm_xflow_macsec_secure_assoc_destroy(unit, macsec_vlan_port[0].encrypt_sa_id[0]));

    print "==========  Test4: MACsec decryption - Unicast ==================";
    port_discard(unit, ingress_port, 1);
    port_discard(unit, egress_port1, 0);
    snprintf(str, 512, "l2 add module=%d port=%d mac=00:00:00:00:00:22 vlan=10 ReplacePriority=f static=t", mod_id, ingress_port);
    bshell(unit, str);
    printf(">>> Input Unicast packet @ port-%d:\n", egress_port1);
    snprintf(str, 512, "tx 1 pbm=%d data=0x%s; sleep quiet 1", egress_port1, uc_enc_pkt_1);

    printf(">>> Output decrypted pkt @ port-%d:\n", ingress_port);
    bshell(0,"dmirror 1 mode=ingress dp=cpu0");
    test_failed = 0;
    rx_count = 0;
    expected_rx_count = 1;
    packet_patterns = 1;
    expected_packets[0]  = expected_decrypted_packet1;
    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , ingress_port);
    bshell(unit, str);
    bshell(0,"dmirror 1 mode=off");
    bshell(unit, "sleep quiet 1");
    pass4 = (test_failed == 0) && (rx_count == expected_rx_count);
    printf("\n----------------------------------------------------------- \n");
    printf("Test4 = [%s]", pass4? "PASS":"FAIL");
    printf("\n----------------------------------------------------------- \n");

    macsec_get_stats(unit, BCM_XFLOW_MACSEC_ENCRYPT, macsec_vlan_port[0].encrypt_sc_id,
                     bcmXflowMacsecSecyTxSCStatsEncryptedPkts, &val);
    printf("Encrypted pkt count:\n");
    print val;
    printf("\n");
    macsec_get_stats(unit, BCM_XFLOW_MACSEC_DECRYPT, macsec_vlan_port[0].decrypt_sc_id,
                     bcmXflowMacsecSecyRxSCStatsOKPkts, &val);
    printf("Decrypted pkt count:\n");
    print val;
    printf("\n");

    print "==========  Test5: MACsec decryption - Unicast (through the 2nd ingress SA */==================";
    port_discard(unit, ingress_port, 1);
    port_discard(unit, egress_port1, 0);
    snprintf(str, 512, "l2 add module=%d port=%d mac=00:00:00:00:00:22 vlan=10 ReplacePriority=f static=t", mod_id, ingress_port);
    bshell(unit, str);
    printf(">>> Input Unicast packet @ port-%d:\n", egress_port1);
    snprintf(str, 512, "tx 1 pbm=%d data=0x%s; sleep quiet 1", egress_port1, uc_enc_pkt_2);

    printf(">>> Output decrypted pkt @ port-%d:\n", ingress_port);
    bshell(0,"dmirror 1 mode=ingress dp=cpu0");
    test_failed = 0;
    rx_count = 0;
    expected_rx_count = 1;
    packet_patterns = 1;
    expected_packets[0]  = expected_decrypted_packet1;
    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , ingress_port);
    bshell(unit, str);
    bshell(0,"dmirror 1 mode=off");
    bshell(unit, "sleep quiet 1");
    pass5 = (test_failed == 0) && (rx_count == expected_rx_count);
    printf("\n----------------------------------------------------------- \n");
    printf("Test5 = [%s]", pass5? "PASS":"FAIL");
    printf("\n----------------------------------------------------------- \n");

    macsec_get_stats(unit, BCM_XFLOW_MACSEC_ENCRYPT, macsec_vlan_port[0].encrypt_sc_id,
                     bcmXflowMacsecSecyTxSCStatsEncryptedPkts, &val);
    printf("Encrypted pkt count:\n");
    print val;
    printf("\n");
    macsec_get_stats(unit, BCM_XFLOW_MACSEC_DECRYPT, macsec_vlan_port[0].decrypt_sc_id,
                     bcmXflowMacsecSecyRxSCStatsOKPkts, &val);
    printf("Decrypted pkt count:\n");
    print val;
    printf("\n");

    /* Manually delete the old SA */
    BCM_IF_ERROR_RETURN(bcm_xflow_macsec_secure_assoc_destroy(unit, macsec_vlan_port[0].decrypt_sa_id[0]));

    print "==========  Test6: MACsec Encryption - Unicast ==================";
    port_discard(unit, ingress_port, 0);
    port_discard(unit, egress_port1, 1);
    snprintf(str, 512, "l2 add module=%d port=%d mac=00:00:00:00:00:22 vlan=11 ReplacePriority=f static=t", mod_id, egress_port1);
    bshell(unit, str);
    snprintf(uc_pkt, 512,"%s","0000000000220000000000018100000Bffff4578002E0000000040FF845714000001E10000011415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2D0A03AB");

    printf(">>> Input Unicast packet @ port-%d:\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x%s; sleep quiet 1", ingress_port, uc_pkt);

    printf(">>> Output MACsec encrypted packet with SCI-x @ port-%d:\n", egress_port1);
    bshell(0,"dmirror 2 mode=ingress dp=cpu0");

    test_failed = 0;
    rx_count = 0;
    expected_rx_count = 1;
    packet_patterns = 1;
    expected_packets[0]  = expected_encrypted_packet3;
    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port1);
    bshell(unit, str);
    bshell(0,"dmirror 2 mode=off");
    bshell(unit, "sleep quiet 1");
    pass6 = (test_failed == 0) && (rx_count == expected_rx_count);
    printf("\n----------------------------------------------------------- \n");
    printf("Test6 = [%s]", pass6? "PASS":"FAIL");
    printf("\n----------------------------------------------------------- \n");

    macsec_get_stats(unit, BCM_XFLOW_MACSEC_ENCRYPT, macsec_vlan_port[1].encrypt_sc_id,
                     bcmXflowMacsecSecyTxSCStatsEncryptedPkts, &val);
    printf("Encrypted pkt count:\n");
    print val;
    printf("\n");
    macsec_get_stats(unit, BCM_XFLOW_MACSEC_DECRYPT, macsec_vlan_port[1].decrypt_sc_id,
                     bcmXflowMacsecSecyRxSCStatsOKPkts, &val);
    printf("Decrypted pkt count:\n");
    print val;
    printf("\n");

    print "==========  Test7: MACsec decryption - Unicast ==================";
    port_discard(unit, ingress_port, 1);
    port_discard(unit, egress_port1, 0);
    snprintf(str, 512, "l2 add module=%d port=%d mac=00:00:00:00:00:22 vlan=11 ReplacePriority=f static=t", mod_id, ingress_port);
    bshell(unit, str);
    printf(">>> Input Unicast packet @ port-%d:\n", egress_port1);
    snprintf(str, 512, "tx 1 pbm=%d data=0x%s; sleep quiet 1", egress_port1, uc_enc_pkt_3);

    printf(">>> Output decrypted pkt @ port-%d:\n", ingress_port);
    bshell(0,"dmirror 1 mode=ingress dp=cpu0");

    test_failed = 0;
    rx_count = 0;
    expected_rx_count = 1;
    packet_patterns = 1;
    expected_packets[0]  = expected_decrypted_packet2;
    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , ingress_port);
    bshell(unit, str);
    bshell(0,"dmirror 1 mode=off");
    bshell(unit, "sleep quiet 1");
    pass7 = (test_failed == 0) && (rx_count == expected_rx_count);
    printf("\n----------------------------------------------------------- \n");
    printf("Test7 = [%s]", pass7? "PASS":"FAIL");
    printf("\n----------------------------------------------------------- \n");

    macsec_get_stats(unit, BCM_XFLOW_MACSEC_ENCRYPT, macsec_vlan_port[1].encrypt_sc_id,
                     bcmXflowMacsecSecyTxSCStatsEncryptedPkts, &val);
    printf("Encrypted pkt count:\n");
    print val;
    printf("\n");
    macsec_get_stats(unit, BCM_XFLOW_MACSEC_DECRYPT, macsec_vlan_port[1].decrypt_sc_id,
                     bcmXflowMacsecSecyRxSCStatsOKPkts, &val);
    printf("Decrypted pkt count:\n");
    print val;
    printf("\n");

    print "==========  Test8: MACsec Egress passthrough - Unicast ==================";
    port_discard(unit, ingress_port, 0);
    port_discard(unit, egress_port1, 1);
    snprintf(str, 512, "l2 add module=%d port=%d mac=00:00:00:00:00:22 vlan=12 ReplacePriority=f static=t", mod_id, egress_port1);
    bshell(unit, str);
    snprintf(uc_pkt, 512,"%s","0000000000220000000000018100000Cffff4578002E0000000040FF845714000001E10000011415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2D0A03AB");

    printf(">>> Input Unicast packet @ port-%d:\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x%s; sleep quiet 1", ingress_port, uc_pkt);

    printf(">>> Output cleartext packet @ port-%d:\n", egress_port1);
    bshell(0,"dmirror 2 mode=ingress dp=cpu0");

    test_failed = 0;
    rx_count = 0;
    expected_rx_count = 1;
    packet_patterns = 1;
    expected_packets[0]  = expected_egress_passthrough_packet1;
    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port1);
    bshell(unit, str);
    bshell(0,"dmirror 2 mode=off");
    bshell(unit, "sleep quiet 1");
    pass8 = (test_failed == 0) && (rx_count == expected_rx_count);
    printf("\n----------------------------------------------------------- \n");
    printf("Test8 = [%s]", pass8? "PASS":"FAIL");
    printf("\n----------------------------------------------------------- \n");


    print "==========  Test9: MACsec Ingress passthrough - Unicast ==================";
    port_discard(unit, ingress_port, 1);
    port_discard(unit, egress_port1, 0);
    snprintf(str, 512, "l2 add module=%d port=%d mac=00:00:00:00:00:22 vlan=12 ReplacePriority=f static=t", mod_id, ingress_port);
    bshell(unit, str);
    snprintf(uc_pkt, 512,"%s","0000000000220000000000018100000Cffff4578002E0000000040FF845714000001E10000011415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2D0A03AB");

    printf(">>> Input Unicast packet @ port-%d:\n", egress_port1);
    snprintf(str, 512, "tx 1 pbm=%d data=0x%s; sleep quiet 1", egress_port1, uc_pkt);

    printf(">>> Output cleartext packet with SCI-x @ port-%d:\n", ingress_port);
    bshell(0,"dmirror 1 mode=ingress dp=cpu0");

    test_failed = 0;
    rx_count = 0;
    expected_rx_count = 1;
    packet_patterns = 1;
    expected_packets[0]  = expected_ingress_passthrough_packet1;
    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , ingress_port);
    bshell(unit, str);
    bshell(0,"dmirror 1 mode=off");
    bshell(unit, "sleep quiet 1");
    pass9 = (test_failed == 0) && (rx_count == expected_rx_count);
    printf("\n----------------------------------------------------------- \n");
    printf("Test9 = [%s]", pass9? "PASS":"FAIL");
    printf("\n----------------------------------------------------------- \n");


    if (pass1 && pass2 && pass3 && pass4 && pass5 && pass6 && pass7 && pass8 && pass9)
        return BCM_E_NONE;
    else
        return -1;
}

/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration
 *  c) demonstrates the functionality(done in test_verify()).
 */
bcm_error_t
execute(void)
{
    bcm_error_t rv;
    int unit = 0;

    bshell(unit, "init all");

    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed %s.\n", bcm_errmsg(rv));
        return -1;
    }

    if (BCM_FAILURE((rv = test_verify(unit)))) {
        printf("test_verify() [FAIL]\n");
    }
    else
        printf("test_verify() [PASS]\n");

    /* Unregister Rx Callback */
    rv = bcm_rx_unregister(unit, rxCallback, rxcb_priority);
    if (rv != BCM_E_NONE) {
        printf("Rx Unregister failed: %s\n", bcm_errmsg(rv));
    }

/*
    port_tc_to_q_mapping_get(unit, ingress_port);
    port_tc_to_q_mapping_get(unit, 65);
    port_tc_to_q_mapping_get(unit, 66);

    port_tc_to_q_mapping_get(unit, egress_port1);
    port_tc_to_q_mapping_get(unit, 67);
    port_tc_to_q_mapping_get(unit, 68);
*/
    printf("--------------------------------\n");
    printf("port 65 Unicast TX Stats\n");
    printf("--------------------------------\n");
    print_cosq_stat(unit, 65, bcmCosqStatOutPackets, 1);

    printf("--------------------------------\n");
    printf("port 66 Unicast TX Stats\n");
    printf("--------------------------------\n");
    print_cosq_stat(unit, 66, bcmCosqStatOutPackets, 1);

    printf("--------------------------------\n");
    printf("port %d Unicast TX Stats\n", egress_port1);
    printf("--------------------------------\n");
    print_cosq_stat(unit, egress_port1, bcmCosqStatOutPackets, 1);

    printf("--------------------------------\n");
    printf("Port 65 Multicast TX Stats\n");
    printf("--------------------------------\n");
    print_cosq_stat(unit, 65, bcmCosqStatOutPackets, 0);

    printf("--------------------------------\n");
    printf("Port 66 Multicast TX Stats\n");
    printf("--------------------------------\n");
    print_cosq_stat(unit, 66, bcmCosqStatOutPackets, 0);

    printf("--------------------------------\n");
    printf("Port %d Multicast TX Stats\n", egress_port1);
    printf("--------------------------------\n");
    print_cosq_stat(unit, egress_port1, bcmCosqStatOutPackets, 0);

    printf("===========================================================================\n");

    printf("--------------------------------\n");
    printf("port 67 Unicast TX Stats\n");
    printf("--------------------------------\n");
    print_cosq_stat(unit, 67, bcmCosqStatOutPackets, 1);

    printf("--------------------------------\n");
    printf("port 68 Unicast TX Stats\n");
    printf("--------------------------------\n");
    print_cosq_stat(unit, 68, bcmCosqStatOutPackets, 1);

    printf("--------------------------------\n");
    printf("port %d Unicast TX Stats\n", ingress_port);
    printf("--------------------------------\n");
    print_cosq_stat(unit, ingress_port, bcmCosqStatOutPackets, 1);

    printf("--------------------------------\n");
    printf("Port 67 Multicast TX Stats\n");
    printf("--------------------------------\n");
    print_cosq_stat(unit, 67, bcmCosqStatOutPackets, 0);

    printf("--------------------------------\n");
    printf("Port 68 Multicast TX Stats\n");
    printf("--------------------------------\n");
    print_cosq_stat(unit, 68, bcmCosqStatOutPackets, 0);

    printf("--------------------------------\n");
    printf("Port %d Multicast TX Stats\n", ingress_port);
    printf("--------------------------------\n");
    print_cosq_stat(unit, ingress_port, bcmCosqStatOutPackets, 0);

    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
    print "execute(): Start";
    print execute();
    print "execute(): End";
}
