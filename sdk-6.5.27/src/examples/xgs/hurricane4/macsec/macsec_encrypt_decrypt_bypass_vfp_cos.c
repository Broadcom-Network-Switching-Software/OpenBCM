/*  Feature  : Hurricane4 MACsec Encryption and decryption
 *
 *  Usage    : BCM.0> cint macsec_encrypt_decrypt_bypass_vfp_cos.c
 *
 *  config   : hr4_sdk6_macsec_config.bcm
 *
 *  Log file : macsec_encrypt_decrypt_bypass_vfp_cos_log.txt
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
 *    This CINT configures macsec encryption/decryption for a single port with MACsec bypass for various control protocol packets
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
 *      1.2) Configure VFP rules to match various control protocols in etherII, LLC, and SNAP format,
 *           and set opaque object id1=0x1111
 *           1.2.1) DstIp6 is used as the container for packet content after ethertype/length field
 *                  128 bits of be filled in the container if the packet is LLC/SNAP or the ethertype is not IPv4/IPv6/ARP/RARP
 *           1.2.2) Hint for DstIpv6 is used to reduce the length of the IFP control flow rules
 *
 *      1.3) Configure 2 IFP rules
 *           1.3.1) For control packets from ingress_port, match on opaque object id1=0x1111 and set EditCtrlID=16
 *           1.3.2) For control packets from egress_port1, match on decrypt 2nd pass, opaque object id1=0x1111 and set EditCtrlID=16
 *                  Note that control packets from one MACsec-enabled port may be forwarded as control packets to another MACsec-enabled port (not tested)
 *
 *      1.4) Configure vlan 10 with 2 ports
 *
 *      1.5) Configure MACsec on egress port 1
 *           1.5.1) Enable MACsec on the egress port
 *           1.5.2) For decryption direction
 *                  1.5.2.1) Configure a decryption SC/SA
 *                  1.5.2.2) Configure a decryption policy with control port enabled and configure mapping from <ms_subport, SCI> to SC
 *                  1.5.2.3) Configure a flow for SecTag packets and bind to the decryption policy in 1.5.2.2)
 *                  1.5.2.4) Configure a policy with untag control port enabled
 *                  1.5.2.5) Configure various control flows for clear text control packets and bind to the policy in 1.5.2.4)
 *           1.5.3) For encryption direction:
 *                  1.5.3.1) Configure an encryption SC/SA
 *                  1.5.3.2) Install EFP rules to modify SVTAG of the packet (encrypt-1st pass).
 *                           The subport id is passed in to MACsec engine through SVTAG.
 *                           Packet Cng (2 bits) are passed in SVTAG also.
 *                  1.5.3.3) For control packets (EditCtrlID=16), set SVTAG.CtrlBit = 1
 *                           Otherwise, set SVTAG.CtrlBit = 0
 *                  1.5.3.4) Add a flex flow-based L2 entry to map
 *                           SVTAG.DEST_PORT to DGPP (encrypt-2nd pass IPIPE).
 *           1.5.4) Set the port class to 1 (to be used as a qualifier for the
 *                  IFP rule in 1.1.1)
 *
 *      1.6) Add mapping from internal priority (4 bits) to SVTAG.PRI (encrypt-1st pass EPIPE)
 *
 *      1.7) Set up L3 IPMC L2 forwarding for BUM traffics
 *           1.7.1) Configure a dummy egress l3_intf
 *           1.7.2) For each MACsec-enabled port,
 *                  1.7.2.1) Configure a L3_IPMC egress object with L3_FLAGS3_L2_TAG_ONLY
 *           1.7.3) For each vlan with MACsec-enabled port (egress_port1),
 *                  1.7.3.1) remove egress_port1 from L2 bitmap of the vlan's ipmc group
 *                  1.7.3.2) add MACsec engine port into the L3 bitmap of the vlan's ipmc group
 *                           using the egress object created in 1.7.2.1)
 *
 *    2) Step2 - Configuration (Null)
 *    ======================================================
 *      Do nothing in configuration step.
 *
 *    3) Step3 - Verification (Done in test_verify())
 *    ======================================================
 *     3.1) Test control packets received from ingress_port and forwarded to egress_port
 *          Expected result: control paxket is forwarded to egress_port without encryption
 *     3.2) Test control packets in clear text received from egress_portq and forwarded to ingress_port
 *          Expected result: control paxket in clear text is forwarded to egress_port
*/

/* Reset C interpreter*/
cint_reset();

bcm_vlan_t  vlan_id = 10;
int mod_id = 2;

bcm_udf_id_t udf_id_l2;

bcm_field_group_t macsec_efp_group;
bcm_field_group_t macsec_encrypt_ifp_group;
bcm_field_group_t macsec_decrypt_ifp_group;
bcm_field_group_t l2user_ifp_group;
bcm_field_group_t macsec_vfp_group;  /* dmac, ipv6-dip (128 bits after ethertype/length) for non-ipv4/non-ipv6 packets */

uint8 eapol_dst[6] = {0x01,0x80,0xc2,0x00,0x00,0x03};
uint8 eapol_dst_mask[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
uint16 eapol_etype = 0x888e;

uint8 bpdu_dst[6] = {0x01,0x80,0xc2,0x00,0x00,0x00};
uint8 bpdu_dst_mask[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
uint16 bpdu_dsap_ssap = 0x4242;
uint8 bpdu_ctrl = 0x03;

uint8 cdp_dst[6] = {0x01,0x00,0x0c,0xcc,0xcc,0xcc};
uint8 cdp_dst_mask[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
uint16 cdp_dsap_ssap = 0xAAAA;
uint8 cdp_ctrl = 0x03;
uint32 cdp_org_code = 0x00000C;
uint16 cdp_type = 0x2000;

uint8 slow_protocol_dst[6] = {0x01,0x80,0xc2,0x00,0x00,0x02};
uint8 slow_protocol_dst_mask[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
uint16 slow_protocol_etype = 0x8809;
uint8 slow_protocol_subtype_lacp = 0x1;

uint16 control_flow_opaque_obj = 0x1111;
uint16 control_flow_opaque_obj_mask = 0xFFFF;

/**** global variables ****/
int vfp_stat_id_eapol;
int vfp_stat_id_bpdu;
int vfp_stat_id_cdp;
int vfp_stat_id_lacp;
int ifp_encrypt_stat_id;
int ifp_decrypt_stat_id[2];
int ifp_control_flow_encrypt_stat_id;
int ifp_control_flow_decrypt_stat_id;
int efp_decrypt_stat_id;

static int isec_sc_tcam_index = 0;
static int isec_sp_tcam_index = 0;

uint8 ing_pkt_pri = 0;
uint8 egr_pkt_pri = 0;
bcm_color_t pkt_color = bcmColorGreen;
uint8 int_pri = 11;
uint8 svtag_pri = int_pri >> 1;
uint8 svtag_cng = int_pri & 0x1;


/**** Data Structures ****/
int MAX_POLICY_NUM = 32;
bcm_xflow_macsec_policy_id_t policy_id[MAX_POLICY_NUM];

int MACSEC_FLOW_CLEARTEXT_FLAG  = 0x1;
int MACSEC_FLOW_MANAGEMENT_FLAG = 0x2;
int MACSEC_FLOW_LLC_FLAG = 0x4;
/* MACsec engine will only consider 0xAAAA03000000 (0x000000 OUI) as SNAP */
int MACSEC_FLOW_SNAP_FLAG = 0x8;
int MACSEC_FLOW_VLAN_UNTAG = 0x10;

struct macsec_flow_s {
    bcm_mac_t   src_mac;            /* Match on Packet src_mac */
    bcm_mac_t   src_mac_mask;
    bcm_mac_t   dst_mac;            /* Match on Packet dst_mac */
    bcm_mac_t   dst_mac_mask;
    bcm_port_t  src_port;           /* Match on Packet ingress port */
    uint32      src_port_mask;
    bcm_xflow_macsec_flow_udf_param_t udf;
    bcm_xflow_macsec_flow_udf_param_t udf_mask;
    bcm_xflow_macsec_policy_id_t policy_id; /* The policy index that this flow points to. */
    bcm_xflow_macsec_flow_id_t   flow_id;   /* Returned flow id */
    uint32      flags;
};


/* internal flags */
int PORT_MACSEC_DISABLE_FLAG = 0x01;
int PORT_MACSEC_NO_SCI_P2P_FLAG = 0x02;
int PORT_MACSEC_NO_SCI_ES_FLAG = 0x04;

int MAX_BYPASS_FLOW_NUM = 0x8;

/* Note that multiple decrypt SC's will exist when the MACsec CA is not P2P */
/* For simplicity, it is omitted in the test */
struct macsec_port_s {
    bcm_port_t                          port;
    bcm_xflow_macsec_secure_chan_id_t   encrypt_sc_id;
    bcm_xflow_macsec_secure_assoc_id_t  encrypt_sa_id;
    uint64                              encrypt_sci;
    int                                 efp_encrypt_stat_id[6];
    bcm_xflow_macsec_secure_chan_id_t   decrypt_sc_id;
    bcm_xflow_macsec_secure_assoc_id_t  decrypt_sa_id;
    uint64                              decrypt_sci;
    macsec_flow_s                       decrypt_flow;
    macsec_flow_s                       cleartext_flow;
    macsec_flow_s                       bypass_flow[MAX_BYPASS_FLOW_NUM];
    uint32                              flags; /* internal flags */
    uint32                              macsec_port_class;
    uint8                               encrypt_port_offset;
};


int CTRL_FLOW_FMT_ETYPE_SUBTYPE_VALID = 0x1;
int CTRL_FLOW_FMT_SNAP_NON_ZERO_OUI_VALID = 0x2;

struct ctrl_flow_format_s {
    bcm_field_L2Format_t format;
    uint16 etype_data;
    uint16 etype_mask;
    uint8  etype_subtype;
    bcm_field_llc_header_t llc_data;
    bcm_field_llc_header_t llc_mask;
    uint8  snap_nonzero_oui_etype[5];    /* 3 bytes of OUI and 2 bytes of etype */
    bcm_field_snap_header_t snap_data;
    bcm_field_snap_header_t snap_mask;
    uint32                  flags;   /* XXX_VALID flag */
};


int non_macsec_port_class = 0;
int macsec_port_class1 = 1;
int macsec_port_class2 = 2;
int default_vfp_port_class = 0;
/*
   Vfp port class help reducing the number of VFP entries
   Ports with the same control packet handling can be assigned the same vfp port class
 */
int vfp_port_class_ingress_port = 1;
int vfp_port_class_egress_port1 = 2;


/**** Internal Sub-functions ****/

void
macsec_flow_s_init(macsec_flow_s *macsec_flow)
{
    if (macsec_flow != NULL) {
        sal_memset(macsec_flow, 0, sizeof (*macsec_flow));
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
ctrl_flow_format_s_init(ctrl_flow_format_s *ctrl_flow_fmt)
{
    if (ctrl_flow_fmt != NULL) {
        sal_memset(ctrl_flow_fmt, 0, sizeof (*ctrl_flow_fmt));
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

            if (int_flags & (PORT_MACSEC_NO_SCI_P2P_FLAG | PORT_MACSEC_NO_SCI_ES_FLAG)) {
                tci = tci &= ~0x8;     /* SC=0 */
                sc_info_flags &= ~BCM_XFLOW_MACSEC_SECURE_CHAN_INFO_INCLUDE_SCI;
            }
            if (int_flags & PORT_MACSEC_NO_SCI_ES_FLAG) {
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
                           bcm_xflow_macsec_secure_assoc_id_t *sa_id)
{
    /* SA vars */
    int asso_num        = 0;
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
    aes_k.key[15] = 0x00;
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
macsec_flow_create(int unit, int pri, macsec_flow_s *macsec_flow)
{
    /* Decrypt flow vars */
    uint32 flow_flags = BCM_XFLOW_MACSEC_DECRYPT;
    uint32 flow_enable = 1;
    xflow_macsec_flow_pkt_type_t pkt_type;
    uint32 tpid = BCM_XFLOW_MACSEC_FLOW_TPID_SEL_0;
    uint32 tpid_mask = 0x0F;
    xflow_macsec_vlan_mpls_tag_status_t vlan_mpls_tag_status;
    bcm_xflow_macsec_flow_id_t flow_id;

    bcm_xflow_macsec_decrypt_flow_info_t dec_flow;
    bcm_xflow_macsec_decrypt_flow_info_t_init(&dec_flow);

    if (macsec_flow->flags & MACSEC_FLOW_CLEARTEXT_FLAG)
        pkt_type = bcmXflowMacsecDecryptFlowNonMacsec;
    else
        pkt_type = bcmXflowMacsecDecryptFlowMacSec;

    if (macsec_flow->flags & MACSEC_FLOW_MANAGEMENT_FLAG) {
        /* all packets in the flow are treated as management packets */
        dec_flow.set_management_pkt = 1;
    }

    if (macsec_flow->flags & MACSEC_FLOW_LLC_FLAG)
        dec_flow.etype_sel = bcmXflowMacsecFlowEtypeLlc;
    else {
        if (macsec_flow->flags & MACSEC_FLOW_SNAP_FLAG)
            dec_flow.etype_sel = bcmXflowMacsecFlowEtypeSnap;
        else
            dec_flow.etype_sel = bcmXflowMacsecFlowEtypeEII;
    }

    if (macsec_flow->flags & MACSEC_FLOW_VLAN_UNTAG) {
        vlan_mpls_tag_status = bcmXflowMacsecTagUntaggedVlan;
        tpid = 0;
    }
    else
        vlan_mpls_tag_status = bcmXflowMacsecTagSingleVlan;

    /* Configure flow */
    dec_flow.policy_id  = macsec_flow->policy_id;
    dec_flow.pkt_type = pkt_type;
    dec_flow.tpid = tpid;
    dec_flow.tpid_mask = tpid_mask;
    dec_flow.vlan_mpls_tag_status = vlan_mpls_tag_status;
    sal_memcpy(dec_flow.src_mac, macsec_flow->src_mac, sizeof(dec_flow.src_mac));
    sal_memcpy(dec_flow.src_mac_mask, macsec_flow->src_mac_mask, sizeof(dec_flow.src_mac));
    sal_memcpy(dec_flow.dst_mac, macsec_flow->dst_mac, sizeof(dec_flow.src_mac));
    sal_memcpy(dec_flow.dst_mac_mask, macsec_flow->dst_mac_mask, sizeof(dec_flow.src_mac));
    dec_flow.src_port   = macsec_flow->src_port;
    dec_flow.src_port_mask = macsec_flow->src_port_mask;
    dec_flow.udf = macsec_flow->udf;  /* ethertype/llc is configured as part of udf */
    dec_flow.udf_mask = macsec_flow->udf_mask;

    BCM_IF_ERROR_RETURN(bcm_xflow_macsec_decrypt_flow_create(unit, flow_flags, 0, &dec_flow, pri, &flow_id));
    printf("\nCreated decrypt flow with id:0x%x\n", flow_id);
    BCM_IF_ERROR_RETURN(bcm_xflow_macsec_decrypt_flow_enable_set(unit, flow_id, flow_enable));
    printf("\nEnabled decrypt flow with id:0x%x\n", flow_id);

    /* get the decrypt flow */
    bcm_xflow_macsec_decrypt_flow_info_t dec_flow_get;
    bcm_xflow_macsec_decrypt_flow_info_t_init(&dec_flow_get);
    int pri_fget = 0;
    BCM_IF_ERROR_RETURN(bcm_xflow_macsec_decrypt_flow_get(unit, flow_id, &dec_flow_get, &pri_fget));
    printf("\nPriority received in bcm_xflow_macsec_decrypt_flow_get:%d\n",pri_fget);
    macsec_flow->flow_id = flow_id;
    return BCM_E_NONE;
}



bcm_error_t
macsec_flow_policy_bind(int unit, macsec_flow_s *macsec_flow, bcm_xflow_macsec_policy_id_t pol_id)
{
    bcm_xflow_macsec_decrypt_flow_info_t dec_flow_get;
    int pri_fget = 0;
    bcm_xflow_macsec_decrypt_flow_info_t_init(&dec_flow_get);
    BCM_IF_ERROR_RETURN(bcm_xflow_macsec_decrypt_flow_get(unit, macsec_flow->flow_id, &dec_flow_get, &pri_fget));

    /* bind flow to new policy */
    dec_flow_get.policy_id = pol_id;
    BCM_IF_ERROR_RETURN(bcm_xflow_macsec_decrypt_flow_set(unit, macsec_flow->flow_id, &dec_flow_get, pri_fget));
    macsec_flow->policy_id = pol_id;
    printf("flow_id 0x%x bound to policy_id 0x%x\n", macsec_flow->flow_id, pol_id);
    return BCM_E_NONE;
}


bcm_error_t
assign_egress_sc_by_efp(int unit, bcm_port_t port, bcm_xflow_macsec_secure_chan_id_t sc_id, bcm_color_t pkt_color,
                        bcm_field_group_t group, int *stat_id)
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

    /* EFP rule matches MACsec flow to modify SVTAG with control = 1 */
    BCM_IF_ERROR_RETURN( bcm_field_stat_create(unit, group, sizeof(stat_type) / sizeof(stat_type[0]), stat_type, stat_id));
    BCM_IF_ERROR_RETURN( bcm_field_entry_create(unit, group, &entry));
    BCM_IF_ERROR_RETURN( bcm_field_qualify_MacSecFlow(unit, entry, qset_macsec_flow));
    BCM_IF_ERROR_RETURN( bcm_field_qualify_DstPort(unit, entry, my_modid, 0xff, port, 0xff));
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
    /* Match on ifp EditCtrlId */
    BCM_IF_ERROR_RETURN( bcm_field_qualify_MacSecControlPkt(unit, entry, bcmFieldMacSecEncryptDecryptPktControl));
    BCM_IF_ERROR_RETURN( bcm_field_action_add(unit, entry, aset_pkt_type, bcmFieldMacSecEncryptDecryptPktControl, 0));
    /* param0 is subport_id, not subport_num */
    BCM_IF_ERROR_RETURN( bcm_field_action_add(unit, entry, bcmFieldActionMacSecSubPortNumAdd, subport, 0x0));
    BCM_IF_ERROR_RETURN( bcm_field_entry_stat_attach(unit, entry, *stat_id));
    BCM_IF_ERROR_RETURN( bcm_field_entry_prio_set(unit, entry, 2));  /* higher priority */
    BCM_IF_ERROR_RETURN( bcm_field_entry_install(unit, entry));

    /* EFP rule matches MACsec flow to add SVTAG with control = 0 */
    BCM_IF_ERROR_RETURN( bcm_field_stat_create(unit, group, sizeof(stat_type) / sizeof(stat_type[0]), stat_type, stat_id+1));
    BCM_IF_ERROR_RETURN( bcm_field_entry_create(unit, group, &entry));
    BCM_IF_ERROR_RETURN( bcm_field_qualify_MacSecFlow(unit, entry, qset_macsec_flow));
    BCM_IF_ERROR_RETURN( bcm_field_qualify_DstPort(unit, entry, my_modid, 0xff, port, 0xff));
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
    BCM_IF_ERROR_RETURN( bcm_field_action_add(unit, entry, aset_pkt_type, bcmFieldMacSecEncryptDecryptPktData, 0));
    /* param0 is subport_id, not subport_num */
    BCM_IF_ERROR_RETURN( bcm_field_action_add(unit, entry, bcmFieldActionMacSecSubPortNumAdd, subport, 0x0));
    BCM_IF_ERROR_RETURN( bcm_field_entry_stat_attach(unit, entry, *(stat_id+1)));
    BCM_IF_ERROR_RETURN( bcm_field_entry_prio_set(unit, entry, 1));  /* lower priority */
    BCM_IF_ERROR_RETURN( bcm_field_entry_install(unit, entry));

    printf("%s\n", str);

    return BCM_E_NONE;
}


/* Create a macsec-enabled port with one SC and one SA */
bcm_error_t
macsec_port_create(int unit, bcm_port_t port, macsec_port_s *macsec_port)
{
    int     decrypt_flags = BCM_XFLOW_MACSEC_DECRYPT;
    int     encrypt_flags = BCM_XFLOW_MACSEC_ENCRYPT;

    printf("\n-------------------------------------------\n");
    printf("Start macsec_port_create() for port-%d\n", port);
    printf("-------------------------------------------\n");
    BCM_IF_ERROR_RETURN(macsec_macsec_port_enable(unit, port, macsec_port->encrypt_port_offset));

    /* decryption for packets coming from the port */
    BCM_IF_ERROR_RETURN(macsec_secure_chan_create(unit, decrypt_flags, macsec_port->decrypt_sci, macsec_port->flags, isec_sc_tcam_index++, 0, &macsec_port->decrypt_sc_id));
    BCM_IF_ERROR_RETURN(macsec_secure_assoc_create(unit, decrypt_flags, macsec_port->decrypt_sc_id, &macsec_port->decrypt_sa_id));

    /* Bind SC to decrypt policy */
    BCM_IF_ERROR_RETURN(macsec_secure_chan_decrypt_policy_bind(unit, macsec_port->decrypt_sc_id,
                                                               policy_id[2]));
    /* decryption: per port SP_TCAM flow */
    macsec_port->decrypt_flow.src_port = port;
    macsec_port->decrypt_flow.src_port_mask = 0x7f;
    macsec_port->decrypt_flow.policy_id = policy_id[2];
    BCM_IF_ERROR_RETURN(macsec_flow_create(unit, isec_sp_tcam_index++, &(macsec_port->decrypt_flow)));

    macsec_port->cleartext_flow.src_port = port;
    macsec_port->cleartext_flow.src_port_mask = 0x7f;
    macsec_port->cleartext_flow.policy_id = policy_id[2];
    macsec_port->cleartext_flow.flags = MACSEC_FLOW_CLEARTEXT_FLAG;
    BCM_IF_ERROR_RETURN(macsec_flow_create(unit, isec_sp_tcam_index++, &(macsec_port->cleartext_flow)));

    /* Create bypass flows */
    /* higher priority than cleartext flow */
    macsec_flow_s *bypass_flow;

    /*******************************************************************************/
    /* EAPoL */
    bypass_flow = &(macsec_port->bypass_flow[0]);
    macsec_flow_s_init(bypass_flow);
    sal_memcpy(bypass_flow->dst_mac, eapol_dst, sizeof(eapol_dst));
    sal_memcpy(bypass_flow->dst_mac_mask, eapol_dst_mask, sizeof(eapol_dst_mask));
    bypass_flow->src_port = port;
    bypass_flow->src_port_mask = 0x7f;
    bypass_flow->policy_id = policy_id[1];

    bypass_flow->flags = MACSEC_FLOW_CLEARTEXT_FLAG |
                         MACSEC_FLOW_MANAGEMENT_FLAG;
    /* EAPoL ethertype */
    bypass_flow->udf.ethertype = 0x888e;
    bypass_flow->udf_mask.ethertype = 0xFFFF;
    BCM_IF_ERROR_RETURN(macsec_flow_create(unit, isec_sp_tcam_index++, bypass_flow));

    /*******************************************************************************/
    /* Ethernet/LLC cleartext flow */
    bypass_flow = &(macsec_port->bypass_flow[1]);
    macsec_flow_s_init(bypass_flow);
    bypass_flow->src_port = port;
    bypass_flow->src_port_mask = 0x7f;
    bypass_flow->policy_id = policy_id[2];
    bypass_flow->flags = MACSEC_FLOW_CLEARTEXT_FLAG;

    bypass_flow->udf.first_vlan = 0x00a;
    bypass_flow->udf_mask.first_vlan = 0xfff;

    /* payload following ethertype/LLC in packet */
    bypass_flow->udf.payload[0]=0x12;
    bypass_flow->udf.payload[1]=0x34;
    bypass_flow->udf_mask.payload[0]=0xFF;
    bypass_flow->udf_mask.payload[1]=0xFF;
    BCM_IF_ERROR_RETURN(macsec_flow_create(unit, isec_sp_tcam_index++, bypass_flow));

    /*******************************************************************************/
    /* BPDU */
    bypass_flow = &(macsec_port->bypass_flow[2]);
    macsec_flow_s_init(bypass_flow);
    sal_memcpy(bypass_flow->dst_mac, bpdu_dst, sizeof(bpdu_dst));
    sal_memcpy(bypass_flow->dst_mac_mask, bpdu_dst_mask, sizeof(bpdu_dst_mask));
    bypass_flow->src_port = port;
    bypass_flow->src_port_mask = 0x7f;
    bypass_flow->policy_id = policy_id[1];

    bypass_flow->flags = MACSEC_FLOW_CLEARTEXT_FLAG |
                         MACSEC_FLOW_MANAGEMENT_FLAG |
                         MACSEC_FLOW_LLC_FLAG;
    /* BPDU LLC */
    bypass_flow->udf.ethertype = bpdu_dsap_ssap;  /* LLC */
    bypass_flow->udf_mask.ethertype = 0xFFFF;
    /* SNAP following LLC in packet */
    bypass_flow->udf.payload[0] = bpdu_ctrl; /* ctrl */
    bypass_flow->udf_mask.payload[0]=0xFF;
    BCM_IF_ERROR_RETURN(macsec_flow_create(unit, isec_sp_tcam_index++, bypass_flow));

    /*******************************************************************************/
    bypass_flow = &(macsec_port->bypass_flow[3]);
    macsec_flow_s_init(bypass_flow);
    sal_memcpy(bypass_flow->dst_mac, cdp_dst, sizeof(bpdu_dst));
    sal_memcpy(bypass_flow->dst_mac_mask, cdp_dst_mask, sizeof(bpdu_dst_mask));
    bypass_flow->src_port = port;
    bypass_flow->src_port_mask = 0x7f;
    bypass_flow->policy_id = policy_id[1];

    bypass_flow->flags = MACSEC_FLOW_CLEARTEXT_FLAG |
                         MACSEC_FLOW_MANAGEMENT_FLAG |
                         MACSEC_FLOW_LLC_FLAG;
    /* Note that MACsec engine will only consider 0xAAAA03000000 as SNAP */
    /* CDP LLC */
    bypass_flow->udf.ethertype = cdp_dsap_ssap;  /* LLC */
    bypass_flow->udf_mask.ethertype = 0xFFFF;
    bypass_flow->udf.payload[0] = cdp_ctrl; /* ctrl */
    bypass_flow->udf_mask.payload[0] = 0xFF;
    /* ORG_CODE+Type following LLC in packet */
    bypass_flow->udf.payload[1] = cdp_org_code >> 16;   /* org_code */
    bypass_flow->udf_mask.payload[1] = 0xFF;
    bypass_flow->udf.payload[2] = (cdp_org_code >> 8) & 0xFF ;
    bypass_flow->udf_mask.payload[2] = 0xFF;
    bypass_flow->udf.payload[3] = cdp_org_code & 0xFF;
    bypass_flow->udf_mask.payload[3] = 0xFF;
    bypass_flow->udf.payload[4] = cdp_type >> 8;        /* type */
    bypass_flow->udf_mask.payload[4] = 0xFF;
    bypass_flow->udf.payload[5] = cdp_type & 0xFF;
    bypass_flow->udf_mask.payload[5]=0xFF;
    BCM_IF_ERROR_RETURN(macsec_flow_create(unit, isec_sp_tcam_index++, bypass_flow));

    /*******************************************************************************/
    /* LACP */
    bypass_flow = &(macsec_port->bypass_flow[4]);
    macsec_flow_s_init(bypass_flow);
    sal_memcpy(bypass_flow->dst_mac, slow_protocol_dst, sizeof(slow_protocol_dst));
    sal_memcpy(bypass_flow->dst_mac_mask, slow_protocol_dst_mask, sizeof(slow_protocol_dst_mask));
    bypass_flow->src_port = port;
    bypass_flow->src_port_mask = 0x7f;
    bypass_flow->policy_id = policy_id[1];

    bypass_flow->flags = MACSEC_FLOW_CLEARTEXT_FLAG |
                         MACSEC_FLOW_MANAGEMENT_FLAG;
    bypass_flow->udf.ethertype = slow_protocol_etype;   /* slow protocols */
    bypass_flow->udf_mask.ethertype = 0xFFFF;
    bypass_flow->udf.payload[0] = slow_protocol_subtype_lacp; /* LACP */
    bypass_flow->udf_mask.payload[0]=0xFF;
    BCM_IF_ERROR_RETURN(macsec_flow_create(unit, isec_sp_tcam_index++, bypass_flow));


    /*******************************************************************************/
    /* encryption for packets going to the port */
    BCM_IF_ERROR_RETURN(macsec_secure_chan_create(unit, encrypt_flags, macsec_port->encrypt_sci, macsec_port->flags, 0, port, &macsec_port->encrypt_sc_id));
    BCM_IF_ERROR_RETURN(macsec_secure_assoc_create(unit, encrypt_flags, macsec_port->encrypt_sc_id, &macsec_port->encrypt_sa_id));
    BCM_IF_ERROR_RETURN(macsec_encrypt_l2_flex_add(unit, port));

/*    BCM_IF_ERROR_RETURN(macsec_encrypt_flow_encap_add(unit, port, int_pri, macsec_port->encrypt_sc_id));
 */
    BCM_IF_ERROR_RETURN(assign_egress_sc_by_efp(unit, port, macsec_port->encrypt_sc_id, bcmColorGreen, macsec_efp_group, &macsec_port->efp_encrypt_stat_id[0]));
    BCM_IF_ERROR_RETURN(assign_egress_sc_by_efp(unit, port, macsec_port->encrypt_sc_id, bcmColorYellow, macsec_efp_group, &macsec_port->efp_encrypt_stat_id[2]));
    BCM_IF_ERROR_RETURN(assign_egress_sc_by_efp(unit, port, macsec_port->encrypt_sc_id, bcmColorRed, macsec_efp_group, &macsec_port->efp_encrypt_stat_id[4]));

    /* macsec port: class 1 or 2 */
    BCM_IF_ERROR_RETURN(bcm_port_class_set(unit, port, bcmPortClassFieldIngress, macsec_port->macsec_port_class));
    return BCM_E_NONE;
}

/* Create a UDF to match on chunks */
/* offset in bits from the start of L2 header */
bcm_error_t
udf_create_l2(int unit, int offset, int length, bcm_udf_id_t *udf_id)
{
    int rv;
    bcm_udf_chunk_info_t    info;
    bcm_udf_abstract_pkt_format_info_t pkt_format_info;
    int i;
    uint32 bmpChunk=0x01;
    uint32 not_allowed_chunk_bmap;
    int num_of_chunks;

    rv = bcm_udf_abstract_pkt_format_info_get(unit, bcmUdfAbstractPktFormatL2,
                                              &pkt_format_info);
    if (BCM_SUCCESS(rv))
        printf("pkt_format_info_get succeeded\n");
    else {
        printf("pkt_format_info_get failed\n");
        return rv;
    }

    num_of_chunks = (length + 15)/16;  /* 16 bits per check */
    bmpChunk = (0x1 << num_of_chunks) - 1;
    not_allowed_chunk_bmap = pkt_format_info.chunk_bmap_used |
                             pkt_format_info.unavail_chunk_bmap;

    printf("not_allowed_chunk_bitmap = 0x%x\n", not_allowed_chunk_bmap);
    /* We only need one 2-bytes chunk */
    for (i = 0; i < 16 - num_of_chunks + 1; i++) {
        if ((not_allowed_chunk_bmap & bmpChunk) == 0)
            break;
        else
            bmpChunk = bmpChunk << 1;
    }
    if (i == (16 - num_of_chunks + 1))
        return BCM_E_RESOURCE;
    else
        printf("bmpChunk: 0x%08X\n", bmpChunk);

    bcm_udf_chunk_info_t_init(&info);
    info.offset = offset; /* in bits */
    info.width  = length; /* in bits */
    info.chunk_bmap = bmpChunk;
    info.abstract_pkt_format = bcmUdfAbstractPktFormatL2;

    rv = bcm_udf_chunk_create(0, NULL, &info, udf_id);
    if (BCM_SUCCESS(rv)) {
        printf("L2_UDF created Successfully: udf_id=%d\n", *udf_id);
    } else {
        printf("L2_UDF Failed to be created: rv:%d\n", rv);
    }
    return rv;
}


bcm_error_t
vfp_udf_setup(int unit)
{
    bcm_field_group_config_t group_cfg;
    bcm_field_entry_t entry;
    bcm_field_stat_t stat_type[]  = { bcmFieldStatBytes, bcmFieldStatPackets };
    int stat_id;
    int i;

    bcm_field_group_config_t_init(&group_cfg);

    group_cfg.priority = 1;  /* different slice */

    BCM_FIELD_QSET_INIT(group_cfg.qset);
    BCM_FIELD_ASET_INIT(group_cfg.aset);
    BCM_FIELD_QSET_ADD(group_cfg.qset, bcmFieldQualifyStageLookup);
    BCM_FIELD_QSET_ADD(group_cfg.qset, bcmFieldQualifyMacSecFlow);
    BCM_IF_ERROR_RETURN(bcm_field_qset_id_multi_set(unit, bcmFieldQualifyUdf, 1,
                                                    udf_id_l2,
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
        BCM_IF_ERROR_RETURN(bcm_field_qualify_udf(unit, entry, udf_id_l2,
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
    int stat_id;
    int i;

    bcm_field_group_config_t_init(&group_cfg);

    group_cfg.priority = 1;  /* different slice */

    BCM_FIELD_QSET_INIT(group_cfg.qset);
    BCM_FIELD_ASET_INIT(group_cfg.aset);
    BCM_FIELD_QSET_ADD(group_cfg.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_cfg.qset, bcmFieldQualifyMacSecFlow);
    BCM_IF_ERROR_RETURN(bcm_field_qset_id_multi_set(unit, bcmFieldQualifyUdf, 1,
                                                    udf_id_l2,
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
        BCM_IF_ERROR_RETURN(bcm_field_qualify_udf(unit, entry, udf_id_l2,
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
create_vfp_group(int unit, bcm_field_group_t *group)
{
    bcm_field_group_config_t group_cfg;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_ASET_INIT(aset);

    /* For matching control packets */
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageLookup);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyMacSecFlow);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInterfaceClassPort);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstMac);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyVlanFormat);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyOuterVlanId);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInnerVlanId);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyL2Format);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyEtherType);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstIp6);

    BCM_FIELD_ASET_ADD(aset, bcmFieldActionAssignOpaqueObject1);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionStatGroup);

    bcm_field_group_config_t_init(&group_cfg);
    group_cfg.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE;

    group_cfg.mode = bcmFieldGroupModeAuto;
    group_cfg.qset = qset;
    group_cfg.aset = aset;

    BCM_IF_ERROR_RETURN( bcm_field_group_config_create(unit, &group_cfg));
    *group = group_cfg.group;
    printf("vfp_group: 0x%x created\n", *group);
    return BCM_E_NONE;
}


bcm_error_t
control_flow_qualify_vlan(int unit, bcm_field_entry_t entry, bcm_vlan_t ovid, bcm_vlan_t ivid,
                          uint8 vformat_data, uint8 vformat_mask)
{
    BCM_IF_ERROR_RETURN( bcm_field_qualify_VlanFormat(unit, entry, vformat_data, vformat_mask));

    /* Only consider untagged, outer tagged, inner tagged, and double tagged here */
    /* Additional cases can be added */
    uint8 vfmt = vformat_data & vformat_mask;
    switch(vfmt) {
    case BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED | BCM_FIELD_VLAN_FORMAT_INNER_TAGGED:
        BCM_IF_ERROR_RETURN( bcm_field_qualify_OuterVlanId(unit, entry, ovid, 0xfff));
        BCM_IF_ERROR_RETURN( bcm_field_qualify_InnerVlanId(unit, entry, ivid, 0xfff));
        break;
    case BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED:
        BCM_IF_ERROR_RETURN( bcm_field_qualify_OuterVlanId(unit, entry, ovid, 0xfff));
        break;

    case BCM_FIELD_VLAN_FORMAT_INNER_TAGGED:
        BCM_IF_ERROR_RETURN( bcm_field_qualify_InnerVlanId(unit, entry, ivid, 0xfff));
        break;

    case 0:
        break;

    default:
        return (BCM_E_PARAM);
    };
    return BCM_E_NONE;
}



bcm_error_t
vfp_control_flow_add_ethii(int unit, uint8 *dstmac, uint8 *dstmac_mask, bcm_vlan_t ovid, bcm_vlan_t ivid, uint8 vformat_data, uint8 vformat_mask,
                           int vfp_port_class, int macsec_port, ctrl_flow_format_s *ctrl_flow_fmt, int *stat_id)
{
    bcm_field_entry_t entry;
    bcm_field_stat_t stat_type[]  = { bcmFieldStatBytes, bcmFieldStatPackets };
    bcm_mac_t mac;
    bcm_mac_t mac_mask;
    bcm_error_t rv;
    bcm_field_llc_header_t llc_header;
    bcm_field_llc_header_t llc_header_mask;
    bcm_ip6_t data;
    bcm_ip6_t mask;

    sal_memcpy(mac, dstmac, 6);
    sal_memcpy(mac_mask, dstmac_mask, 6);

    BCM_IF_ERROR_RETURN( bcm_field_stat_create(unit, macsec_vfp_group,
                                               sizeof(stat_type)/sizeof(stat_type[0]), stat_type, stat_id));
    rv = bcm_field_entry_create(unit, macsec_vfp_group, &entry);
    if (rv != BCM_E_NONE)
        goto error1;

    rv = bcm_field_qualify_InterfaceClassPort(unit, entry, vfp_port_class, BCM_FIELD_EXACT_MATCH_MASK);
    if (rv != BCM_E_NONE)
        goto error;
`   rv = bcm_field_qualify_DstMac(unit, entry, mac, mac_mask);
    if (rv != BCM_E_NONE)
        goto error;

    /* macsec_port is used to indicate if the ingress port is a MACsec-enabled port */
    if (macsec_port) {
        rv = bcm_field_qualify_MacSecFlow(unit, entry, bcmFieldMacSecFlowAfterDecrypt);
        if (rv != BCM_E_NONE)
            goto error;
    }
    rv = bcm_field_qualify_DstMac(unit, entry, mac, mac_mask);
    if (rv != BCM_E_NONE)
        goto error;

    rv = bcm_field_qualify_L2Format(unit, entry, bcmFieldL2FormatEthII);
    if (rv != BCM_E_NONE)
        goto error;

    rv = bcm_field_qualify_EtherType(unit, entry, ctrl_flow_fmt->etype_data, ctrl_flow_fmt->etype_mask);
    if (rv != BCM_E_NONE)
        goto error;

    rv = control_flow_qualify_vlan(unit, entry, ovid, ivid, vformat_data, vformat_mask);
    if (rv != BCM_E_NONE)
        goto error;

    if (ctrl_flow_fmt->flags & CTRL_FLOW_FMT_ETYPE_SUBTYPE_VALID) {
        sal_memset(&data, 0, sizeof (data));
        sal_memset(&mask, 0, sizeof (mask));
        data[0] = ctrl_flow_fmt->etype_subtype;
        mask[0] = 0xFF;
        /* IPV6_DIP container is filled with 16 bytes of data after ethertype/length field if the packet is not IPv4/IPv6 packet */
        rv = bcm_field_qualify_DstIp6(unit, entry, data, mask);
        if (rv != BCM_E_NONE)
            goto error;
    }

    rv = bcm_field_action_add(unit, entry, bcmFieldActionAssignOpaqueObject1, control_flow_opaque_obj, 0);
    if (rv != BCM_E_NONE)
        goto error;

    rv = bcm_field_entry_stat_attach(unit, entry, *stat_id);
    if (rv != BCM_E_NONE)
        goto error;

    rv = bcm_field_entry_install(unit, entry);
    if (rv != BCM_E_NONE)
        goto error;

    printf("VFP ethii rule to assign opaqueObject1=0x%x added \n", control_flow_opaque_obj);
    return BCM_E_NONE;

error:
    BCM_IF_ERROR_RETURN(bcm_field_entry_destroy(unit, entry));
error1:
    BCM_IF_ERROR_RETURN(bcm_field_stat_destroy(unit, *stat_id));
    return (rv);
}



bcm_error_t
vfp_control_flow_add_llc(int unit, uint8 *dstmac, uint8 *dstmac_mask, bcm_vlan_t ovid, bcm_vlan_t ivid, uint8 vformat_data, uint8 vformat_mask,
                         int vfp_port_class, int macsec_port, ctrl_flow_format_s *ctrl_flow_fmt, int *stat_id)
{
    bcm_field_entry_t entry;
    bcm_field_stat_t stat_type[]  = { bcmFieldStatBytes, bcmFieldStatPackets };
    bcm_mac_t mac;
    bcm_mac_t mac_mask;
    bcm_error_t rv;
    bcm_ip6_t data;
    bcm_ip6_t mask;

    sal_memcpy(mac, dstmac, 6);
    sal_memcpy(mac_mask, dstmac_mask, 6);

    BCM_IF_ERROR_RETURN( bcm_field_stat_create(unit, macsec_vfp_group,
                                               sizeof(stat_type)/sizeof(stat_type[0]), stat_type, stat_id));
    rv = bcm_field_entry_create(unit, macsec_vfp_group, &entry);
    if (rv != BCM_E_NONE)
        goto error1;

    rv = bcm_field_qualify_InterfaceClassPort(unit, entry, vfp_port_class, BCM_FIELD_EXACT_MATCH_MASK);
    if (rv != BCM_E_NONE)
        goto error1;
;
    /* macsec_port is used to indicate if the ingress port is a MACsec-enabled port */
    if (macsec_port) {
        rv = bcm_field_qualify_MacSecFlow(unit, entry, bcmFieldMacSecFlowAfterDecrypt);
        if (rv != BCM_E_NONE)
            goto error1;
    }
    rv = bcm_field_qualify_DstMac(unit, entry, mac, mac_mask);
    if (rv != BCM_E_NONE)
        goto error;

    rv = bcm_field_qualify_L2Format(unit, entry, bcmFieldL2FormatLlc);
    if (rv != BCM_E_NONE)
        goto error;
    rv = control_flow_qualify_vlan(unit, entry, ovid, ivid, vformat_data, vformat_mask);
    if (rv != BCM_E_NONE)
        goto error;

    sal_memset(&data, 0, sizeof (data));
    sal_memset(&mask, 0, sizeof (mask));
    data[0] = ctrl_flow_fmt->llc_data.dsap;
    data[1] = ctrl_flow_fmt->llc_data.ssap;
    data[2] = ctrl_flow_fmt->llc_data.control;
    mask[0] = ctrl_flow_fmt->llc_mask.dsap;
    mask[1] = ctrl_flow_fmt->llc_mask.ssap;
    mask[2] = ctrl_flow_fmt->llc_mask.control;

    /* IPV6_DIP container is filled with 16 bytes of data after ethertype/length field if the packet is not IPv4/IPv6 packet */
    rv = bcm_field_qualify_DstIp6(unit, entry, data, mask);
    if (rv != BCM_E_NONE)
        goto error;

    rv = bcm_field_action_add(unit, entry, bcmFieldActionAssignOpaqueObject1, control_flow_opaque_obj, 0);
    if (rv != BCM_E_NONE)
        goto error;

    rv = bcm_field_entry_stat_attach(unit, entry, *stat_id);
    if (rv != BCM_E_NONE)
        goto error;

    rv = bcm_field_entry_install(unit, entry);
    if (rv != BCM_E_NONE)
        goto error;

    printf("VFP llc rule to assign opaqueObject1=0x%x added \n", control_flow_opaque_obj);
    return BCM_E_NONE;

error:
    BCM_IF_ERROR_RETURN(bcm_field_entry_destroy(unit, entry));
error1:
    BCM_IF_ERROR_RETURN(bcm_field_stat_destroy(unit, *stat_id));
    return (rv);
}


bcm_error_t
vfp_control_flow_add_snap(int unit, uint8 *dstmac, uint8 *dstmac_mask, bcm_vlan_t ovid, bcm_vlan_t ivid, uint8 vformat_data, uint8 vformat_mask,
                          int vfp_port_class, int macsec_port, ctrl_flow_format_s *ctrl_flow_fmt, int *stat_id)
{
    bcm_field_entry_t entry;
    bcm_field_stat_t stat_type[]  = { bcmFieldStatBytes, bcmFieldStatPackets };
    bcm_mac_t mac;
    bcm_mac_t mac_mask;
    bcm_error_t rv;
    uint32 org_code = ctrl_flow_fmt->snap_data.org_code;
    uint16 type = ctrl_flow_fmt->snap_data.type ;
    uint32 org_code_mask = ctrl_flow_fmt->snap_mask.org_code;
    uint16 type_mask = ctrl_flow_fmt->snap_mask.type ;
    bcm_ip6_t data;
    bcm_ip6_t mask;

    sal_memcpy(mac, dstmac, 6);
    sal_memcpy(mac_mask, dstmac_mask, 6);

    BCM_IF_ERROR_RETURN( bcm_field_stat_create(unit, macsec_vfp_group,
                                               sizeof(stat_type)/sizeof(stat_type[0]), stat_type, stat_id));
    rv = bcm_field_entry_create(unit, macsec_vfp_group, &entry);
    if (rv != BCM_E_NONE)
        goto error1;

`   rv = bcm_field_qualify_DstMac(unit, entry, mac, mac_mask);
    if (rv != BCM_E_NONE)
        goto error;

    rv = bcm_field_qualify_InterfaceClassPort(unit, entry, vfp_port_class, BCM_FIELD_EXACT_MATCH_MASK);
    if (rv != BCM_E_NONE)
        goto error;
    /* macsec_port is used to indicate if the ingress port is a MACsec-enabled port */
    if (macsec_port) {
        rv = bcm_field_qualify_MacSecFlow(unit, entry, bcmFieldMacSecFlowAfterDecrypt);
        if (rv != BCM_E_NONE)
            goto error;
    }
    rv = bcm_field_qualify_DstMac(unit, entry, mac, mac_mask);
    if (rv != BCM_E_NONE)
        goto error;

    /* Only 0xAAAA03000000 DSAP/SSAP/Ctrl/OUI is considered as SNAP in the XGS pipeline */
    if (org_code == 0)
        rv = bcm_field_qualify_L2Format(unit, entry, bcmFieldL2FormatSnap);
    else
        rv = bcm_field_qualify_L2Format(unit, entry, bcmFieldL2FormatLlc);
    if (rv != BCM_E_NONE)
        goto error;
    rv = control_flow_qualify_vlan(unit, entry, ovid, ivid, vformat_data, vformat_mask);
    if (rv != BCM_E_NONE)
        goto error;

    sal_memset(&data, 0, sizeof (data));
    sal_memset(&mask, 0, sizeof (mask));
    data[0] = 0xAA;
    data[1] = 0xAA;
    data[2] = 0x03;
    data[3] = (org_code >> 16) & 0xFF;
    data[4] = (org_code >> 8) & 0xFF;
    data[5] = org_code & 0xFF;
    data[6] = (type >> 8) & 0xFF;
    data[7] = type & 0xFF;
    mask[0] = 0xFF;
    mask[1] = 0xFF;
    mask[2] = 0xFF;
    mask[3] = (org_code_mask >> 16) & 0xFF;
    mask[4] = (org_code_mask >> 8) & 0xFF;
    mask[5] = org_code_mask & 0xFF;
    mask[6] = (type_mask >> 8) & 0xFF;
    mask[7] = type_mask & 0xFF;

    /* IPV6_DIP container is filled with 16 bytes of data after ethertype/length field if the packet is not IPv4/IPv6 packet */
    rv = bcm_field_qualify_DstIp6(unit, entry, data, mask);
    if (rv != BCM_E_NONE)
        goto error;

    rv = bcm_field_action_add(unit, entry, bcmFieldActionAssignOpaqueObject1, control_flow_opaque_obj, 0);
    if (rv != BCM_E_NONE)
        goto error;

    rv = bcm_field_entry_stat_attach(unit, entry, *stat_id);
    if (rv != BCM_E_NONE)
        goto error;

    rv = bcm_field_entry_install(unit, entry);
    if (rv != BCM_E_NONE)
        goto error;

    printf("VFP snap rule to assign opaqueObject1=0x%x added \n", control_flow_opaque_obj);
    return BCM_E_NONE;

error:
    BCM_IF_ERROR_RETURN(bcm_field_entry_destroy(unit, entry));
error1:
    BCM_IF_ERROR_RETURN(bcm_field_stat_destroy(unit, *stat_id));
    return (rv);
}





bcm_error_t
vfp_control_flow_add(int unit, uint8 *dstmac, uint8 *dstmac_mask, bcm_vlan_t ovid, bcm_vlan_t ivid, uint8 vformat_data, uint8 vformat_mask,
                     int vfp_port_class, int macsec_port, ctrl_flow_format_s *ctrl_flow_fmt, int *stat_id)
{

    switch (ctrl_flow_fmt->format) {
    case bcmFieldL2FormatEthII:
        BCM_IF_ERROR_RETURN(vfp_control_flow_add_ethii(unit, dstmac, dstmac_mask, ovid, ivid, vformat_data, vformat_mask,
                                                       vfp_port_class, macsec_port, ctrl_flow_fmt, stat_id));
        break;

    case bcmFieldL2FormatLlc:
        BCM_IF_ERROR_RETURN(vfp_control_flow_add_llc(unit, dstmac, dstmac_mask, ovid, ivid, vformat_data, vformat_mask,
                                                     vfp_port_class, macsec_port, ctrl_flow_fmt, stat_id));
        break;

    case bcmFieldL2FormatSnap:
        BCM_IF_ERROR_RETURN(vfp_control_flow_add_snap(unit, dstmac, dstmac_mask, ovid, ivid, vformat_data, vformat_mask,
                                                      vfp_port_class, macsec_port, ctrl_flow_fmt, stat_id));
        break;

    default:
        BCM_IF_ERROR_RETURN(bcm_field_stat_destroy(unit, *stat_id));
        BCM_IF_ERROR_RETURN(bcm_field_entry_destroy(unit, entry));
        return (BCM_E_PARAM);
    }

    return BCM_E_NONE;
}


bcm_error_t
ifp_control_flow_add(int unit, int dir, uint16 data, uint16 mask, int *stat_id)
{
    bcm_field_stat_t stat_type[]  = { bcmFieldStatBytes, bcmFieldStatPackets };
    bcm_field_group_t group;
    bcm_field_entry_t entry;

    if (dir == BCM_XFLOW_MACSEC_ENCRYPT)
        group = macsec_encrypt_ifp_group;
    else
        group = macsec_decrypt_ifp_group;

    BCM_IF_ERROR_RETURN( bcm_field_stat_create(unit, group,
                                               sizeof(stat_type)/sizeof(stat_type[0]), stat_type, stat_id));
    BCM_IF_ERROR_RETURN( bcm_field_entry_create(unit, group, &entry));

    if (dir == BCM_XFLOW_MACSEC_ENCRYPT) {
        BCM_IF_ERROR_RETURN( bcm_field_entry_prio_set(unit, entry, 1)); /* lower priority compared to entry created in ifp_encryption_default_setup() */
    }
    else {
        BCM_IF_ERROR_RETURN( bcm_field_qualify_MacSecFlow(unit, entry, bcmFieldMacSecFlowAfterDecrypt));
        BCM_IF_ERROR_RETURN( bcm_field_entry_prio_set(unit, entry, 3)); /* highest priority compared to entries created in ifp_decryption_default_setup() */
    }
    BCM_IF_ERROR_RETURN(bcm_field_qualify_OpaqueObject1(unit, entry, data, mask));
    /* Need both data and mask */
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionAssignEditCtrlId, 16, 0xFF));

    BCM_IF_ERROR_RETURN( bcm_field_entry_stat_attach(unit, entry, *stat_id));
    BCM_IF_ERROR_RETURN( bcm_field_entry_install(unit, entry));

    if (dir == BCM_XFLOW_MACSEC_ENCRYPT)
        printf("IFP rule to assign EditCtrlId=16 (for encryption 1st pass) added \n");
    else
        printf("IFP rule to assign EditCtrlId=16 (for decryption 2nd pass) added \n");

    return BCM_E_NONE;
}



bcm_error_t
create_ifp_group(int unit, int dir, bcm_field_group_t *group)
{
    bcm_field_group_config_t group_cfg;
    char str[512];

    bcm_field_group_config_t_init(&group_cfg);

    group_cfg.priority = 2;

    BCM_FIELD_QSET_INIT(group_cfg.qset);
    BCM_FIELD_ASET_INIT(group_cfg.aset);

    BCM_FIELD_QSET_ADD(group_cfg.qset, bcmFieldQualifyStageIngress);

    BCM_FIELD_ASET_ADD(group_cfg.aset, bcmFieldActionStatGroup);
    if (dir == BCM_XFLOW_MACSEC_ENCRYPT) {
        /* For encryption 2nd pass */
        BCM_FIELD_QSET_ADD(group_cfg.qset, bcmFieldQualifyMacSecFlow);
        BCM_FIELD_ASET_ADD(group_cfg.aset, bcmFieldActionMacSecModidBase);
        snprintf(str, 512,"%s","> IFP group to map DGPP to final DGPP installed (encrypt-2nd pass).");
    } else {
        BCM_FIELD_QSET_ADD(group_cfg.qset, bcmFieldQualifyInterfaceClassPort);
        BCM_FIELD_QSET_ADD(group_cfg.qset, bcmFieldQualifyDrop);
        BCM_FIELD_QSET_ADD(group_cfg.qset, bcmFieldQualifyMacSecFlow);
        BCM_FIELD_ASET_ADD(group_cfg.aset, bcmFieldActionUnmodifiedPacketRedirectPort);
        snprintf(str, 512,"%s","> IFP group to send MACsec packets to MACsec engine port installed (decrypt-1st pass).");
    }

    /* For matching control packets */
    BCM_FIELD_QSET_ADD(group_cfg.qset, bcmFieldQualifyOpaqueObject1);
    BCM_FIELD_ASET_ADD(group_cfg.aset, bcmFieldActionAssignEditCtrlId);

    BCM_IF_ERROR_RETURN( bcm_field_group_config_create(unit, &group_cfg));
    *group = group_cfg.group;
    printf("%s\n", str);
    return BCM_E_NONE;
}


bcm_error_t
ifp_decryption_default_setup(int unit, bcm_field_group_t group, int *stat_id)
{
    bcm_field_entry_t entry;
    bcm_field_stat_t stat_type[]  = { bcmFieldStatBytes, bcmFieldStatPackets };
    int macsec_decrypt_lb_port_offset0 = 67;
    int macsec_decrypt_lb_port_offset1 = 68;
    int my_modid;
    bcm_gport_t gport;
    int tmp_stat_id;

    /* Create IFP entry to avoid redirect for 2nd pass decryption */
    BCM_IF_ERROR_RETURN( bcm_field_stat_create(unit, group,
                                               sizeof(stat_type)/sizeof(stat_type[0]), stat_type, &tmp_stat_id));
    BCM_IF_ERROR_RETURN( bcm_field_entry_create(unit, group, &entry));
    BCM_IF_ERROR_RETURN( bcm_field_qualify_MacSecFlow(unit, entry, bcmFieldMacSecFlowAfterDecrypt));
    BCM_IF_ERROR_RETURN( bcm_field_entry_stat_attach(unit, entry, tmp_stat_id));
    BCM_IF_ERROR_RETURN( bcm_field_entry_prio_set(unit, entry, 2));  /* higher priority */
    BCM_IF_ERROR_RETURN( bcm_field_entry_install(unit, entry));
    printf("%s","> IFP rule to avoid redirect for 2nd pass decryption installed (decrypt-2nd pass).");

    /* Do not match on 0x88e5 to allow for clear text packet pass-through */
    /* Create 1st decryption IFP entry to redirect to port 67 */
    /* Entry 2:
       Packet from macsec_port_class1
       action: UnmodifiedPacketRedirectPort to macsec decryption port 67
    */
    BCM_IF_ERROR_RETURN(bcm_stk_modid_get(unit, &my_modid));
    BCM_GPORT_MODPORT_SET(gport, my_modid, 67);
    BCM_IF_ERROR_RETURN( bcm_field_stat_create(unit, group,
                                               sizeof(stat_type)/sizeof(stat_type[0]), stat_type, stat_id));
    BCM_IF_ERROR_RETURN( bcm_field_entry_create(unit, group, &entry));

    BCM_IF_ERROR_RETURN( bcm_field_qualify_InterfaceClassPort(unit, entry, macsec_port_class1, BCM_FIELD_EXACT_MATCH_MASK));
    /* not dropped */
    BCM_IF_ERROR_RETURN( bcm_field_qualify_Drop(unit, entry, 0, 1));

    BCM_IF_ERROR_RETURN( bcm_field_action_add(unit, entry, bcmFieldActionUnmodifiedPacketRedirectPort, gport, macsec_decrypt_lb_port_offset0));
    BCM_IF_ERROR_RETURN( bcm_field_entry_stat_attach(unit, entry, *stat_id));
    BCM_IF_ERROR_RETURN( bcm_field_entry_prio_set(unit, entry, 1)); /* lower priority */
    BCM_IF_ERROR_RETURN( bcm_field_entry_install(unit, entry));
    printf("%s","> IFP rule to send MACsec packets to MACsec engine port 67 installed (decrypt-1st pass).");

    /* create 2nd decryption IFP entry to redirect to port 68 */
    /* Entry 3:
       Packet from macsec_port_class2
       action: UnmodifiedPacketRedirectPort to macsec decryption port 68
    */
    BCM_GPORT_MODPORT_SET(gport, my_modid, 68);
    BCM_IF_ERROR_RETURN( bcm_field_stat_create(unit, group,
                                               sizeof(stat_type)/sizeof(stat_type[0]), stat_type, stat_id+1));
    BCM_IF_ERROR_RETURN( bcm_field_entry_create(unit, group, &entry));

    BCM_IF_ERROR_RETURN( bcm_field_qualify_InterfaceClassPort(unit, entry, macsec_port_class2, BCM_FIELD_EXACT_MATCH_MASK));
    /* not dropped */
    BCM_IF_ERROR_RETURN( bcm_field_qualify_Drop(unit, entry, 0, 1));

    BCM_IF_ERROR_RETURN( bcm_field_action_add(unit, entry, bcmFieldActionUnmodifiedPacketRedirectPort, gport, macsec_decrypt_lb_port_offset1));
    BCM_IF_ERROR_RETURN( bcm_field_entry_stat_attach(unit, entry, *(stat_id+1)));
    BCM_IF_ERROR_RETURN( bcm_field_entry_prio_set(unit, entry, 1)); /* lower priority */
    BCM_IF_ERROR_RETURN( bcm_field_entry_install(unit, entry));
    printf("%s","> IFP rule to send MACsec packets to MACsec engine port 68 installed (decrypt-1st pass).");

    return BCM_E_NONE;
}


bcm_error_t
ifp_encryption_default_setup(int unit, bcm_field_group_t group, int *stat_id)
{
    bcm_field_entry_t entry;
    bcm_field_stat_t stat_type[]  = { bcmFieldStatBytes, bcmFieldStatPackets };

    BCM_IF_ERROR_RETURN( bcm_field_stat_create(unit, group,
                                               sizeof(stat_type)/sizeof(stat_type[0]), stat_type, stat_id));
    BCM_IF_ERROR_RETURN( bcm_field_entry_create(unit, group, &entry));
    BCM_IF_ERROR_RETURN( bcm_field_qualify_MacSecFlow(unit, entry, bcmFieldMacSecFlowAfterEncrypt));
    BCM_IF_ERROR_RETURN( bcm_field_action_add(unit, entry, bcmFieldActionMacSecModidBase, 0, 0));
    BCM_IF_ERROR_RETURN( bcm_field_entry_stat_attach(unit, entry, *stat_id));
    BCM_IF_ERROR_RETURN( bcm_field_entry_prio_set(unit, entry, 2));  /* higher priority */
    BCM_IF_ERROR_RETURN( bcm_field_entry_install(unit, entry));
    printf("%s","> IFP rule to use IFP modidBase (encrypt-2nd pass).");
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
    BCM_FIELD_QSET_ADD(group_cfg.qset, bcmFieldQualifyMacSecControlPkt);
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
efp_decryption_setup(int unit, bcm_field_group_t group, int *stat_id)
{
    bcm_field_entry_t entry;
    bcm_field_stat_t stat_type[]  = { bcmFieldStatBytes, bcmFieldStatPackets };
    int qset_macsec_flow;
    int aset_pkt_type;
    bcm_xflow_macsec_subport_id_t subport;

    BCM_IF_ERROR_RETURN( bcm_field_stat_create(unit, group, sizeof(stat_type) / sizeof(stat_type[0]), stat_type, stat_id));
    BCM_IF_ERROR_RETURN( bcm_field_entry_create(unit, group, &entry));
    BCM_IF_ERROR_RETURN( bcm_field_qualify_MacSecFlow(unit, entry, bcmFieldMacSecFlowForDecrypt));
    BCM_IF_ERROR_RETURN( bcm_field_action_add(unit, entry, bcmFieldActionMacSecDecryptPktType, bcmFieldMacSecEncryptDecryptPktData, 0));
    BCM_IF_ERROR_RETURN( bcm_field_entry_stat_attach(unit, entry, *stat_id));
    BCM_IF_ERROR_RETURN( bcm_field_entry_install(unit, entry));
    printf("efp_decryption_setup completed\n");

    return BCM_E_NONE;
}



bcm_error_t
create_l2user_ifp_group(int unit, bcm_field_group_t *group)
{
    bcm_field_group_config_t group_cfg;

    bcm_field_group_config_t_init(&group_cfg);

    group_cfg.priority = 1;
    BCM_FIELD_QSET_INIT(group_cfg.qset);
    BCM_FIELD_ASET_INIT(group_cfg.aset);

    BCM_FIELD_QSET_ADD(group_cfg.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_cfg.qset, bcmFieldQualifyInterfaceClassPort);
    BCM_FIELD_QSET_ADD(group_cfg.qset, bcmFieldQualifyMacSecFlow);
    BCM_FIELD_QSET_ADD(group_cfg.qset, bcmFieldQualifyDstMac);
    BCM_FIELD_QSET_ADD(group_cfg.qset, bcmFieldQualifyOuterVlanId);

    BCM_FIELD_ASET_ADD(group_cfg.aset, bcmFieldActionStatGroup);
    BCM_FIELD_ASET_ADD(group_cfg.aset, bcmFieldActionCopyToCpu);
    BCM_FIELD_ASET_ADD(group_cfg.aset, bcmFieldActionDrop);
    BCM_FIELD_ASET_ADD(group_cfg.aset, bcmFieldActionRedirectPbmp);
    BCM_FIELD_ASET_ADD(group_cfg.aset, bcmFieldActionStatGroup);
    BCM_IF_ERROR_RETURN( bcm_field_group_config_create(unit, &group_cfg));

    *group = group_cfg.group;
    printf("> IFP group to redirect control packets created (decrypt-1st pass), group = 0x%x\n", *group);
    return BCM_E_NONE;
}


int mask_leng(uint8 *mask, int size)
{
    int i;
    uint8 bit;
    uint8 byte_index = 0;
    uint8 byte;

    for (i = 0; i < size; i++)
    {
        byte_index = i / 8;
        byte = mask[byte_index];

        if ((i % 8) == 0)
            bit = 0x80;

        if (byte & bit)
            bit >>= 1;
        else
            break;
    }
    return i;
}


bcm_error_t
create_ifp_to_redirect_decrypted_flow(int unit, uint8 *dstmac, uint8 *dstmac_mask,
                                      bcm_vlan_t vid, bcm_vlan_t vid_mask, bcm_pbmp_t pbmp,
                                      bcm_field_entry_t *ret_entry, int *ret_stat_id)
{
    bcm_field_entry_t entry;
    bcm_field_stat_t stat_type[]  = { bcmFieldStatBytes, bcmFieldStatPackets };
    int stat_id;
    bcm_mac_t mac;
    bcm_mac_t mac_mask;
    int mask_leng;
    uint8 tmp_mask[4];

    tmp_mask[0] = (vid_mask & 0xFF000000) >> 24;
    tmp_mask[1] = (vid_mask & 0x00FF0000) >> 16;
    tmp_mask[2] = (vid_mask & 0x0000FF00) >> 8;
    tmp_mask[3] = (vid_mask & 0x000000FF);

    /*
       Determine total mask length.
       Longer mask length has higher priority
       vid_mask should be either 0 or 0xFFFFFFFF
    */
    mask_leng = mask_leng(dstmac_mask, 48);
    mask_leng += mask_leng(tmp_mask, 32);

    sal_memcpy(mac, dstmac, 6);
    sal_memcpy(mac_mask, dstmac_mask, 6);

    BCM_IF_ERROR_RETURN(bcm_field_stat_create(unit, l2user_ifp_group,
                                              sizeof(stat_type)/sizeof(stat_type[0]),
                                              stat_type, &stat_id));
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, l2user_ifp_group, &entry));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_MacSecFlow(unit, entry, bcmFieldMacSecFlowAfterDecrypt));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_DstMac(unit, entry, mac, mac_mask));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_OuterVlanId(unit, entry, vid, vid_mask));
    BCM_IF_ERROR_RETURN(bcm_field_entry_prio_set(unit, entry, 3*mask_leng+2));  /* higher priority */

    /* forward to ports in pbmp */
    BCM_IF_ERROR_RETURN(bcm_field_action_ports_add(unit, entry, bcmFieldActionRedirectPbmp, pbmp));
    BCM_IF_ERROR_RETURN(bcm_field_entry_stat_attach(unit, entry, stat_id));
    printf("l2_user IFP: fp stat get statid=%d type=packets\n", stat_id);
    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
    *ret_entry = entry;
    *ret_stat_id = stat_id;
    return BCM_E_NONE;
};


bcm_error_t
create_ifp_for_bypass_flow(int unit, uint8 *dstmac, uint8 *dstmac_mask,
                           bcm_vlan_t vid, bcm_vlan_t vid_mask, bcm_pbmp_t pbmp, int four_rules)
{
    bcm_field_entry_t entry;
    bcm_field_stat_t stat_type[]  = { bcmFieldStatBytes, bcmFieldStatPackets };
    int stat_id;
    bcm_mac_t mac;
    bcm_mac_t mac_mask;
    int mask_leng;
    uint8 tmp_mask[4];

    tmp_mask[0] = (vid_mask & 0xFF000000) >> 24;
    tmp_mask[1] = (vid_mask & 0x00FF0000) >> 16;
    tmp_mask[2] = (vid_mask & 0x0000FF00) >> 8;
    tmp_mask[3] = (vid_mask & 0x000000FF);

    /*
       Determine total mask length.
       Longer mask length has higher priority
       vid_mask should be either 0 or 0xFFFFFFFF
    */
    mask_leng = mask_leng(dstmac_mask, 48);
    mask_leng += mask_leng(tmp_mask, 32);

    sal_memcpy(mac, dstmac, 6);
    sal_memcpy(mac_mask, dstmac_mask, 6);

    /* Create 4 IFP entries */
    /* Entry 1:
       Control packet from decrypt macsec engine
       action: redirect to pbmp
    */
    BCM_IF_ERROR_RETURN(bcm_field_stat_create(unit, l2user_ifp_group,
                                              sizeof(stat_type)/sizeof(stat_type[0]),
                                              stat_type, &stat_id));
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, l2user_ifp_group, &entry));
    BCM_IF_ERROR_RETURN(bcm_field_entry_prio_set(unit, entry, 3*mask_leng+2));  /* higher priority */
    BCM_IF_ERROR_RETURN(bcm_field_qualify_DstMac(unit, entry, mac, mac_mask));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_OuterVlanId(unit, entry, vid, vid_mask));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_MacSecFlow(unit, entry, bcmFieldMacSecFlowAfterDecrypt));

    /* forward to ports in pbmp */
    BCM_IF_ERROR_RETURN(bcm_field_action_ports_add(unit, entry, bcmFieldActionRedirectPbmp, pbmp));
    BCM_IF_ERROR_RETURN(bcm_field_entry_stat_attach(unit, entry, stat_id));
    printf("l2_user IFP: fp stat get statid=%d type=packets\n", stat_id);
    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

    if (four_rules)
    {
        /* Entry 2:
           Control packet from portClass 1
           action: do nothing
        */
        BCM_IF_ERROR_RETURN(bcm_field_stat_create(unit, l2user_ifp_group,
                                                  sizeof(stat_type)/sizeof(stat_type[0]),
                                                  stat_type, &stat_id));
        BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, l2user_ifp_group, &entry));
        BCM_IF_ERROR_RETURN(bcm_field_entry_prio_set(unit, entry, 3*mask_leng+1));  /* lower priority */
        BCM_IF_ERROR_RETURN(bcm_field_qualify_DstMac(unit, entry, mac, mac_mask));
        BCM_IF_ERROR_RETURN(bcm_field_qualify_OuterVlanId(unit, entry, vid, vid_mask));
        BCM_IF_ERROR_RETURN(bcm_field_qualify_InterfaceClassPort(unit, entry, macsec_port_class1, BCM_FIELD_EXACT_MATCH_MASK));
        bcm_field_entry_stat_attach(unit, entry, stat_id);
        printf("l2_user IFP: fp stat get statid=%d type=packets\n", stat_id);
        BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

        /* Entry 3:
           Control packet from portClass 2
           action: do nothing
        */
        BCM_IF_ERROR_RETURN(bcm_field_stat_create(unit, l2user_ifp_group,
                                                  sizeof(stat_type)/sizeof(stat_type[0]),
                                                  stat_type, &stat_id));
        BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, l2user_ifp_group, &entry));
        BCM_IF_ERROR_RETURN(bcm_field_entry_prio_set(unit, entry, 3*mask_leng+1));  /* lower priority */
        BCM_IF_ERROR_RETURN(bcm_field_qualify_DstMac(unit, entry, mac, mac_mask));
        BCM_IF_ERROR_RETURN(bcm_field_qualify_OuterVlanId(unit, entry, vid, vid_mask));
        BCM_IF_ERROR_RETURN(bcm_field_qualify_InterfaceClassPort(unit, entry, macsec_port_class2, BCM_FIELD_EXACT_MATCH_MASK));
        BCM_IF_ERROR_RETURN(bcm_field_entry_stat_attach(unit, entry, stat_id));
        printf("l2_user IFP: fp stat get statid=%d type=packets\n", stat_id);
        BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

        /* Entry 4:
           Control packet in all other cases
           action: copy to CPU and drop (replace deleted L2_user_entry)
        */
        BCM_IF_ERROR_RETURN(bcm_field_stat_create(unit, l2user_ifp_group,
                                                  sizeof(stat_type)/sizeof(stat_type[0]),
                                                  stat_type, &stat_id));
        BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, l2user_ifp_group, &entry));
        BCM_IF_ERROR_RETURN(bcm_field_entry_prio_set(unit, entry, 3*mask_leng));  /* lowest priority */
        BCM_IF_ERROR_RETURN(bcm_field_qualify_DstMac(unit, entry, mac, mac_mask));
        BCM_IF_ERROR_RETURN(bcm_field_qualify_OuterVlanId(unit, entry, vid, vid_mask));

        /* forward to CPU */
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));
        BCM_IF_ERROR_RETURN(bcm_field_entry_stat_attach(unit, entry, stat_id));
        printf("l2_user IFP: fp stat get statid=%d type=packets\n", stat_id);
        BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
    }
    return BCM_E_NONE;
};


/* IFP entries in this group must be in different slice from the IFP entries
   used for packet redirect to decryption MACsec engine
   Both may be hit.
*/
bcm_error_t
move_l2user_to_ifp(int unit, uint8 *dstmac, uint8 *dstmac_mask,
                   bcm_vlan_t vid, bcm_vlan_t vid_mask, bcm_pbmp_t pbmp)
{
    int i, index, cache_size = 0;
    bcm_l2_cache_addr_t l2_addr;
    int match_cnt = 0;

    bcm_l2_cache_size_get(unit, &cache_size);
    for (index = 0; index < cache_size; index++) {
        bcm_l2_cache_addr_t_init(&l2_addr);
        if (bcm_l2_cache_get(unit, index, &l2_addr) == BCM_E_NONE) {
            /* Fix some garbage bits in l2_add.vlan */
            l2_addr.vlan &= l2_addr.vlan_mask;

            if (((vid_mask & l2_addr.vlan_mask) <= vid_mask) &&
                ((l2_addr.vlan & l2_addr.vlan_mask) == (vid & l2_addr.vlan_mask)))
            {
                for (i = 0; i < 6; i++) {
                    if ((dstmac_mask[i] & l2_addr.mac_mask[i]) != l2_addr.mac_mask[i])
                        break;
                }
                if (i != 6)
                    continue;

                for (i = 0; i < 6; i++) {
                    if ((dstmac[i] & l2_addr.mac_mask[i]) != (l2_addr.mac[i] & l2_addr.mac_mask[i]))
                        break;
                }
                if (i != 6)
                    continue;

                if (l2_addr.flags & BCM_L2_CACHE_CPU) {
                    match_cnt++;
                    /* Create IFP rules for the deleted L2_user_entry */
                    BCM_IF_ERROR_RETURN(create_ifp_for_bypass_flow(unit, l2_addr.mac, l2_addr.mac_mask,
                                                                   l2_addr.vlan, l2_addr.vlan_mask, pbmp, 1));
                    printf("delete L2_USER_ENTRY\n");
                    BCM_IF_ERROR_RETURN(bcm_l2_cache_delete(unit, index));
                }
            }
        }
    }
    /* Create one ifp entry using the input value/mask when there is no corresponding L2_user_entry */
    if (match_cnt == 0)
        BCM_IF_ERROR_RETURN(create_ifp_for_bypass_flow(unit, dstmac, dstmac_mask,
                                                       vid, vid_mask, pbmp, 0));
    return BCM_E_NONE;
}


bcm_error_t
remove_l2user(int unit, uint8 *dstmac, uint8 *dstmac_mask,
              bcm_vlan_t vid, bcm_vlan_t vid_mask)
{
    int i, index, cache_size = 0;
    bcm_l2_cache_addr_t l2_addr;

    bcm_l2_cache_size_get(unit, &cache_size);
    for (index = 0; index < cache_size; index++) {
        bcm_l2_cache_addr_t_init(&l2_addr);
        if (bcm_l2_cache_get(unit, index, &l2_addr) == BCM_E_NONE) {
            /* Fix some garbage bits in l2_add.vlan */
            l2_addr.vlan &= l2_addr.vlan_mask;

            if (((vid_mask & l2_addr.vlan_mask) <= vid_mask) &&
                ((l2_addr.vlan & l2_addr.vlan_mask) == (vid & l2_addr.vlan_mask)))
            {
                for (i = 0; i < 6; i++) {
                    if ((dstmac_mask[i] & l2_addr.mac_mask[i]) != l2_addr.mac_mask[i])
                        break;
                }
                if (i != 6)
                    continue;

                for (i = 0; i < 6; i++) {
                    if ((dstmac[i] & l2_addr.mac_mask[i]) != (l2_addr.mac[i] & l2_addr.mac_mask[i]))
                        break;
                }
                if (i != 6)
                    continue;

                printf("delete L2_USER_ENTRY\n");
                BCM_IF_ERROR_RETURN(bcm_l2_cache_delete(unit, index));
            }
        }
    }
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


bcm_port_t ingress_port;
bcm_port_t egress_port1;    /* macsec 1 */
bcm_gport_t egress_gport1;
bcm_gport_t ingress_gport;
macsec_port_s macsec_port1; /* egress_port1 */


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
        /*
        printf("Unicast: Map TC to Egress queue (P, TC, Q) = (%d, %d, %x)\n",
                port, tci, queue_id);
        */
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
        /*
        printf("Multicast: map TC to Egress queue (P, TC, Q) = (%d, %d, %x)\n",
                port, tci, queue_id);
        */

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


int vfp_for_2nd_pass_encrypt = 1;

bcm_error_t
config_macsec(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_vlan_t  vid = 10;
    int         untagged = 0;
    bcm_multicast_t vlan_ipmc_group;

    BCM_IF_ERROR_RETURN(macsec_init(unit));

    uint64 tmp;
    COMPILER_64_SET(tmp, 0x0, 0x1);
    /* Drop failed MACsec packets */
    BCM_IF_ERROR_RETURN(bcm_xflow_macsec_control_set(unit, BCM_XFLOW_MACSEC_ENCRYPT, 0, bcmXflowMacsecControlEncryptFailDrop, tmp));
    BCM_IF_ERROR_RETURN(bcm_xflow_macsec_control_set(unit, BCM_XFLOW_MACSEC_DECRYPT, 0, bcmXflowMacsecControlDecryptFailDrop, tmp));

    /* Configure MTU0 in MACsec engine */
    bcm_xflow_macsec_mtu_t mtu_sel;
    mtu_sel = bcmXflowMacsecMtu0;
    BCM_IF_ERROR_RETURN(bcm_xflow_macsec_mtu_set(
                            unit, BCM_XFLOW_MACSEC_ENCRYPT | BCM_XFLOW_MACSEC_MTU_WITH_ID,
                            0, 1514, &mtu_sel));
    mtu_sel = bcmXflowMacsecMtu0;
    BCM_IF_ERROR_RETURN(bcm_xflow_macsec_mtu_set(
                            unit, BCM_XFLOW_MACSEC_DECRYPT | BCM_XFLOW_MACSEC_MTU_WITH_ID,
                            0, 1514, &mtu_sel));

    BCM_IF_ERROR_RETURN(macsec_vlan_create(unit, vid, &vlan_ipmc_group));
    BCM_IF_ERROR_RETURN(vlan_create_add_port(unit, vid, ingress_port, untagged));
    BCM_IF_ERROR_RETURN(vlan_create_add_port(unit, vid, egress_port1, untagged));

    bcm_gport_t gport;
    int my_modid;
    BCM_IF_ERROR_RETURN(bcm_stk_modid_get(unit, &my_modid));
    BCM_GPORT_MODPORT_SET(gport, my_modid, 0);
    /* Set EGR_LPORT_PROFILE.EN_EFILTER = 0 for CPU port
       This is to make sure rediect to CPU control packets will not be dropped
     */
    BCM_IF_ERROR_RETURN(bcm_port_vlan_member_set(unit, gport, 0));

    /* Set port default vlan id for untagged packets */
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, ingress_port, vid));
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port1, vid));

    BCM_IF_ERROR_RETURN(create_ifp_group(unit, BCM_XFLOW_MACSEC_DECRYPT, &macsec_decrypt_ifp_group));
    BCM_IF_ERROR_RETURN(create_ifp_group(unit, BCM_XFLOW_MACSEC_ENCRYPT, &macsec_encrypt_ifp_group));

    /* FP rules for Decryption */
    BCM_IF_ERROR_RETURN(ifp_decryption_default_setup(unit, macsec_decrypt_ifp_group, ifp_decrypt_stat_id));
    /* FP rule for Encryption */
    BCM_IF_ERROR_RETURN(ifp_encryption_default_setup(unit, macsec_encrypt_ifp_group, &(ifp_encrypt_stat_id)));

    /* Used in IFP for int_pri derivation */
    BCM_IF_ERROR_RETURN(udf_create_l2(unit, 112, 16, &udf_id_l2));

    if (vfp_for_2nd_pass_encrypt)
        BCM_IF_ERROR_RETURN(vfp_udf_setup(unit));
    else
        BCM_IF_ERROR_RETURN(ifp_udf_setup(unit));

    BCM_IF_ERROR_RETURN(create_l2user_ifp_group(unit, &l2user_ifp_group));

    /* Create MACsec efp group */
    BCM_IF_ERROR_RETURN(create_efp_group(unit, &macsec_efp_group));

    /* Create one EFP rule for decryption */
    BCM_IF_ERROR_RETURN(efp_decryption_setup(unit, macsec_efp_group, &efp_decrypt_stat_id));

    /* Create MACsec vfp group */
    BCM_IF_ERROR_RETURN(create_vfp_group(unit, &macsec_vfp_group));

    /* Second pass encryption is SOBMH-like flow.  Will not hit VFP */

    /***************************************************************************************************/
    /* In decryption direction, control packet identification is performed in the macsec engine        */
    /* We still have to handle 2nd pass decryption in case it will be forwarded to another MACsec port */
    /***************************************************************************************************/

    /* VFP rule to match EAPoL control packet format from ingress_port */
    ctrl_flow_format_s ctrl_flow_fmt;
    ctrl_flow_format_s_init(&ctrl_flow_fmt);
    ctrl_flow_fmt.format = bcmFieldL2FormatEthII;
    ctrl_flow_fmt.etype_data = eapol_etype;
    ctrl_flow_fmt.etype_mask = 0xFFFF;
    BCM_IF_ERROR_RETURN(vfp_control_flow_add(unit, eapol_dst, eapol_dst_mask, vlan_id, 0, BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED,
                                             BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED | BCM_FIELD_VLAN_FORMAT_INNER_TAGGED,
                                             vfp_port_class_ingress_port, 0, &ctrl_flow_fmt, &vfp_stat_id_eapol));

    /* VFP rule to match BPDU control packet format from ingress_port */
    ctrl_flow_format_s_init(&ctrl_flow_fmt);
    ctrl_flow_fmt.format = bcmFieldL2FormatLlc;
    ctrl_flow_fmt.llc_data.dsap = bpdu_dsap_ssap >> 8;
    ctrl_flow_fmt.llc_data.ssap = bpdu_dsap_ssap & 0xFF;
    ctrl_flow_fmt.llc_data.control = bpdu_ctrl;
    ctrl_flow_fmt.llc_mask.dsap = 0xFF;
    ctrl_flow_fmt.llc_mask.ssap = 0xFF;
    ctrl_flow_fmt.llc_mask.control = 0xFF;
    BCM_IF_ERROR_RETURN(vfp_control_flow_add(unit, bpdu_dst, bpdu_dst_mask, vlan_id, 0, BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED,
                                             BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED | BCM_FIELD_VLAN_FORMAT_INNER_TAGGED,
                                             vfp_port_class_ingress_port, 0, &ctrl_flow_fmt, &vfp_stat_id_bpdu));

    /* VFP rule to match CDP control packet format from ingress_port */
    ctrl_flow_format_s_init(&ctrl_flow_fmt);
    ctrl_flow_fmt.format = bcmFieldL2FormatSnap;
    ctrl_flow_fmt.snap_data.org_code = cdp_org_code;
    ctrl_flow_fmt.snap_data.type = cdp_type;
    ctrl_flow_fmt.snap_mask.org_code = 0xFFFFFF;
    ctrl_flow_fmt.snap_mask.type = 0xFFFF;
    BCM_IF_ERROR_RETURN(vfp_control_flow_add(unit, cdp_dst, cdp_dst_mask, vlan_id, 0, BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED,
                                             BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED | BCM_FIELD_VLAN_FORMAT_INNER_TAGGED,
                                             vfp_port_class_ingress_port, 0, &ctrl_flow_fmt, &vfp_stat_id_cdp));

    /* VFP rule to match LACP control packet format from ingress_port */
    ctrl_flow_format_s_init(&ctrl_flow_fmt);
    ctrl_flow_fmt.format = bcmFieldL2FormatEthII;
    ctrl_flow_fmt.etype_data = slow_protocol_etype;
    ctrl_flow_fmt.etype_mask = 0xFFFF;
    ctrl_flow_fmt.etype_subtype = slow_protocol_subtype_lacp;
    ctrl_flow_fmt.flags = CTRL_FLOW_FMT_ETYPE_SUBTYPE_VALID;
    BCM_IF_ERROR_RETURN(vfp_control_flow_add(unit, slow_protocol_dst, slow_protocol_dst_mask, vlan_id, 0, BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED,
                                             BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED | BCM_FIELD_VLAN_FORMAT_INNER_TAGGED,
                                             vfp_port_class_ingress_port, 0, &ctrl_flow_fmt, &vfp_stat_id_lacp));

    /* IFP rule to match control packets to assign EditCtrlId*/
    BCM_IF_ERROR_RETURN(ifp_control_flow_add(unit, BCM_XFLOW_MACSEC_ENCRYPT, control_flow_opaque_obj,
                                             control_flow_opaque_obj_mask, &ifp_control_flow_encrypt_stat_id));
    BCM_IF_ERROR_RETURN(ifp_control_flow_add(unit, BCM_XFLOW_MACSEC_DECRYPT, control_flow_opaque_obj,
                                             control_flow_opaque_obj_mask, &ifp_control_flow_decrypt_stat_id));
    uint64 zero_sci;
    COMPILER_64_SET(zero_sci, 0x0, 0x0);
    /* Configure UNTAGGED_CONTROL_PORT_ENABLE=0 for policy_id[0] */
    BCM_IF_ERROR_RETURN(macsec_decrypt_policy_create(unit, zero_sci,
                                                     0,
                                                     0, bcmXflowMacsecMtu0, &policy_id[0]));
/*    BCM_IF_ERROR_RETURN(macsec_decrypt_policy_create(unit, zero_sci,
                                                     BCM_XFLOW_MACSEC_DECRYPT_POLICY_UNTAGGED_FRAME_DENY,
                                                     0, bcmXflowMacsecMtu0, &policy_id[0]));
*/
    /* Configure UNTAGGED_CONTROL_PORT_ENABLE=1 for policy_id[1] */
    BCM_IF_ERROR_RETURN(macsec_decrypt_policy_create(unit, zero_sci,
                                                     BCM_XFLOW_MACSEC_DECRYPT_POLICY_UNTAGGED_CONTROL_PORT_ENABLE,
                                                     0, bcmXflowMacsecMtu0, &policy_id[1]));
    /* Configure default macsec_policy_id */
    BCM_IF_ERROR_RETURN(macsec_decrypt_policy_create(unit, zero_sci,
                                                     BCM_XFLOW_MACSEC_DECRYPT_POLICY_TAGGED_CONTROL_PORT_ENABLE |
                                                     BCM_XFLOW_MACSEC_DECRYPT_POLICY_UNTAGGED_CONTROL_PORT_ENABLE,
                                                     0, bcmXflowMacsecMtu0, &policy_id[2]));
    /*
       Note that we use loopback to test MACsec decryption.
       Therefore, decrypt_sci equals encrypt_sci.
    */
    macsec_port_s_init(&macsec_port1);
    COMPILER_64_SET(macsec_port1.decrypt_sci, 0, 0x08);
    COMPILER_64_SET(macsec_port1.encrypt_sci, 0, 0x08);
    macsec_port1.encrypt_port_offset = 0;
    /* assign class 1 or 2 */
    macsec_port1.macsec_port_class = macsec_port_class1;
    macsec_port1.port = egress_port1;
    BCM_IF_ERROR_RETURN(macsec_port_create(unit, egress_port1, &macsec_port1));

    bcm_color_t color[3] = {bcmColorGreen, bcmColorYellow, bcmColorRed};
    int i;
    /* set up mapping from int_pri to svtag.pri && svtag.cng */
    bcm_qos_map_t qMap;
    for (i = 0; i < 3; i++) {
        bcm_qos_map_t_init(&qMap);
        qMap.int_pri = int_pri;
        qMap.color = color[i];
        qMap.etag_pcp = svtag_pri;
        qMap.etag_de = svtag_cng;
        print bcm_qos_map_add(unit, BCM_QOS_MAP_SUBPORT, &qMap, 0);
    }
    bcm_if_t intf_id;
    bcm_if_t egr_obj_id1;
    bcm_vlan_t dummy_vid = 4095;
    bcm_mac_t dummy_mac = "00:00:00:00:00:01";

    uint32 flags3 = BCM_L3_FLAGS3_L2_TAG_ONLY;
    BCM_IF_ERROR_RETURN(l3_intf_create(unit, dummy_vid, dummy_mac, 0, &intf_id));
    BCM_IF_ERROR_RETURN(l3_egress_create(unit, BCM_L3_IPMC, flags3, macsec_port1.port, dummy_vid, dummy_mac, intf_id, &egr_obj_id1));

    BCM_IF_ERROR_RETURN(multicast_port_del(unit, vlan_ipmc_group, macsec_port1.port, 0)); /* remove from L2_BITMAP */
    BCM_IF_ERROR_RETURN(multicast_port_add(unit, vlan_ipmc_group, macsec_port1.port, egr_obj_id1)); /* add to L3_BITMAP for MACSEC */

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
create_ifp_to_cpu_group (int unit, bcm_field_group_config_t *group_config)
{
    bcm_error_t rv = BCM_E_NONE;

    /* FP group configuration and creation */
    bcm_field_group_config_t_init(group_config);

    BCM_FIELD_QSET_INIT(group_config->qset);
    BCM_FIELD_QSET_ADD(group_config->qset, bcmFieldQualifyStageIngress);
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
create_ifp_to_cpu_rule (int unit, bcm_field_group_t gid, bcm_field_entry_t *eid, bcm_port_t port)
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
destroy_ifp_to_cpu_rule (int unit, bcm_field_entry_t eid)
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
            printf("match pattern %d\n", i);
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
    BCM_IF_ERROR_RETURN(create_ifp_to_cpu_group(unit, &_group_config));

    /* Start the packet watcher (quiet mode) */
    bshell(unit, "pw start report +raw");

    ingress_port = port[0];
    egress_port1 = port[1];

    printf("ingress_port =%d\n",ingress_port);
    printf("egress_port1 =%d\n",egress_port1);

    BCM_GPORT_MODPORT_SET(egress_gport1, mod_id, egress_port1);
    BCM_GPORT_MODPORT_SET(ingress_gport, mod_id, ingress_port);

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
        rv = bcm_port_class_set(unit, port[i], bcmPortClassFieldLookup,
                                default_vfp_port_class);
        if (BCM_FAILURE(rv)) {
            printf("bcm_port_class_set() failed. %s\n", bcm_errmsg(rv));
            return rv;
        }
    }

    /* Assign vfp_port_class for ports under test */
    /* Use modport gport as the ingress port can be at a different device */
    rv = bcm_port_class_set(unit, ingress_gport, bcmPortClassFieldLookup,
                            vfp_port_class_ingress_port);
    if (BCM_FAILURE(rv)) {
        printf("bcm_port_class_set() failed. %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_port_class_set(unit, egress_port1, bcmPortClassFieldLookup,
                            vfp_port_class_egress_port1);
    if (BCM_FAILURE(rv)) {
        printf("bcm_port_class_set() failed. %s\n", bcm_errmsg(rv));
        return rv;
    }


    /* Add ingress priority mapping:
       map {packet priority + CFI} to {int_pri, Green}
       and assign the map to an ingress port
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
       and assign the map to an egress port
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

    /* For control packets redirected to CPU */
    if (BCM_E_NONE != qos_port_setup(unit, 0, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_L2,
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



bcm_error_t
dump_fp_stats(int unit)
{
    uint64 val;

    bcm_field_stat_get(unit, vfp_stat_id_eapol, bcmFieldStatPackets, &val);
    printf("vfp_stat_id_eapol:\n");
    print val;

    bcm_field_stat_get(unit, vfp_stat_id_bpdu, bcmFieldStatPackets, &val);
    printf("vfp_stat_id_bpdu:\n");
    print val;

    bcm_field_stat_get(unit, vfp_stat_id_cdp, bcmFieldStatPackets, &val);
    printf("vfp_stat_id_cdp:\n");
    print val;

    bcm_field_stat_get(unit, vfp_stat_id_lacp, bcmFieldStatPackets, &val);
    printf("vfp_stat_id_lacp:\n");
    print val;

    bcm_field_stat_get(unit, ifp_encrypt_stat_id, bcmFieldStatPackets, &val);
    printf("ifp_encrypt_stat_id:\n");
    print val;

    bcm_field_stat_get(unit, macsec_port1.efp_encrypt_stat_id[0], bcmFieldStatPackets, &val);
    printf("efp_encrypt_stat_id[0]:\n");
    print val;
    bcm_field_stat_get(unit, macsec_port1.efp_encrypt_stat_id[1], bcmFieldStatPackets, &val);
    printf("efp_encrypt_stat_id[1]:\n");
    print val;
    bcm_field_stat_get(unit, macsec_port1.efp_encrypt_stat_id[2], bcmFieldStatPackets, &val);
    printf("efp_encrypt_stat_id[2]:\n");
    print val;
    bcm_field_stat_get(unit, macsec_port1.efp_encrypt_stat_id[3], bcmFieldStatPackets, &val);
    printf("efp_encrypt_stat_id[3]:\n");
    print val;
    bcm_field_stat_get(unit, macsec_port1.efp_encrypt_stat_id[4], bcmFieldStatPackets, &val);
    printf("efp_encrypt_stat_id[4]:\n");
    print val;
    bcm_field_stat_get(unit, macsec_port1.efp_encrypt_stat_id[5], bcmFieldStatPackets, &val);
    printf("efp_encrypt_stat_id[5]:\n");
    print val;

    bcm_field_stat_get(unit, ifp_decrypt_stat_id[0], bcmFieldStatPackets, &val);
    printf("ifp_decrypt_stat_id[0]:\n");
    print val;

    bcm_field_stat_get(unit, ifp_decrypt_stat_id[1], bcmFieldStatPackets, &val);
    printf("ifp_decrypt_stat_id[1]:\n");
    print val;

    bcm_field_stat_get(unit, ifp_control_flow_encrypt_stat_id, bcmFieldStatPackets, &val);
    printf("ifp_control_flow_encrypt_stat_id:\n");
    print val;

    bcm_field_stat_get(unit, ifp_control_flow_decrypt_stat_id, bcmFieldStatPackets, &val);
    printf("ifp_control_flow_decrypt_stat_id:\n");
    print val;

    bcm_field_stat_get(unit, efp_decrypt_stat_id, bcmFieldStatPackets, &val);
    printf("efp_decrypt_stat_id:\n");
    print val;
    printf("\n");
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

unsigned char expected_encrypted_mc_packet1[512] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x81, 0x00,
    0x00, 0x0a, 0x88, 0xe5, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x08, 0x21, 0xb3, 0xe4, 0x22, 0x8c, 0x2e, 0xb5, 0xd4, 0x18, 0x56,
    0xb8, 0xc1, 0x54, 0x27, 0xa3, 0x33, 0x2e, 0xda, 0x04, 0x44, 0xe2, 0xc9, 0x1b, 0x1f,
    0xb6, 0xcb, 0x25, 0x71, 0x23, 0x31, 0x00, 0xa1, 0x8f, 0x9f, 0x36, 0x96, 0x2c, 0xdb,
    0xa6, 0xb1, 0x4a, 0x9a, 0x13, 0x4c, 0xd3, 0xb0, 0xab, 0x37, 0x38, 0x70, 0x9f, 0x0c,
    0x5c, 0x48, 0xa6, 0x82, 0xad, 0x64, 0xb5, 0xa2, 0x12, 0xcb, 0xe5, 0x9f, 0xc4, 0x1a,
    0xc5, 0x61, 0x09, 0x9b, 0xcf, 0x08};

unsigned char expected_decrypted_packet2[512] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x81, 0x00,
    0x00, 0x0a, 0xff, 0xff, 0x45, 0x78, 0x00, 0x2e, 0x00, 0x00, 0x00, 0x00, 0x40, 0xff,
    0x84, 0x57, 0x14, 0x00, 0x00, 0x01, 0xe1, 0x00, 0x00, 0x01, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25,
    0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2d, 0x0a, 0x03, 0xab, 0x09, 0x11,
    0x56, 0xa1};

unsigned char expected_eapol_packet[512] = {
    0x01, 0x80, 0xc2, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x81, 0x00, 0x00, 0x0a,
    0x88, 0x8e, 0x01, 0x00, 0x00, 0x05, 0x01, 0x01, 0x00, 0x05, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00};

unsigned char expected_eapol_packet_crc[512] = {
    0x01, 0x80, 0xc2, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x81, 0x00, 0x00, 0x0a,
    0x88, 0x8e, 0x01, 0x00, 0x00, 0x05, 0x01, 0x01, 0x00, 0x05, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xfe, 0x79, 0x8e, 0xf9};

unsigned char expected_bpdu_packet[256] = {
    0x01, 0x80, 0xc2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x81, 0x00,
    0x00, 0x0a, 0x00, 0x79, 0x42, 0x42, 0x03, 0x00, 0x00, 0x03, 0x02, 0x7c, 0x80, 0x00,
    0x00, 0x0c, 0x30, 0x5d, 0xd1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x0c,
    0x30, 0x5d, 0xd1, 0x00, 0x80, 0x05, 0x00, 0x00, 0x14, 0x00, 0x02, 0x00, 0x0f, 0x00,
    0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0xbf, 0x4e, 0x8a,
    0x44, 0xb2, 0x5d, 0x44, 0x28, 0x68, 0x54, 0x9c, 0x1b, 0xf7, 0x72, 0x0f, 0x00, 0x03,
    0x0d, 0x40, 0x80, 0x00, 0x00, 0x1a, 0xa1, 0x97, 0xd1, 0x80, 0x13, 0x7c, 0x80, 0x05,
    0x00, 0x0c, 0x30, 0x5d, 0xd1, 0x00, 0x00, 0x03, 0x0d, 0x40, 0x80, 0x80, 0x13, 0x00,
    0x00, 0x00, 0x00};

unsigned char expected_cdp_packet[1024] = {
    0x01, 0x00, 0x0c, 0xcc, 0xcc, 0xcc, 0x00, 0x18, 0xba, 0x98, 0x68, 0x8f, 0x81, 0x00, 0x00, 0x0a,
    0x01, 0x76, 0xaa, 0xaa,
    0x03, 0x00, 0x00, 0x0c, 0x20, 0x00, 0x02, 0xb4, 0x0b, 0xea, 0x00, 0x01, 0x00, 0x06, 0x53, 0x31,
    0x00, 0x05, 0x00, 0xc2, 0x43, 0x69, 0x73, 0x63, 0x6f, 0x20, 0x49, 0x4f, 0x53, 0x20, 0x53, 0x6f,
    0x66, 0x74, 0x77, 0x61, 0x72, 0x65, 0x2c, 0x20, 0x43, 0x33, 0x35, 0x36, 0x30, 0x20, 0x53, 0x6f,
    0x66, 0x74, 0x77, 0x61, 0x72, 0x65, 0x20, 0x28, 0x43, 0x33, 0x35, 0x36, 0x30, 0x2d, 0x41, 0x44,
    0x56, 0x49, 0x50, 0x53, 0x45, 0x52, 0x56, 0x49, 0x43, 0x45, 0x53, 0x4b, 0x39, 0x2d, 0x4d, 0x29,
    0x2c, 0x20, 0x56, 0x65, 0x72, 0x73, 0x69, 0x6f, 0x6e, 0x20, 0x31, 0x32, 0x2e, 0x32, 0x28, 0x34,
    0x34, 0x29, 0x53, 0x45, 0x2c, 0x20, 0x52, 0x45, 0x4c, 0x45, 0x41, 0x53, 0x45, 0x20, 0x53, 0x4f,
    0x46, 0x54, 0x57, 0x41, 0x52, 0x45, 0x20, 0x28, 0x66, 0x63, 0x31, 0x29, 0x0a, 0x43, 0x6f, 0x70,
    0x79, 0x72, 0x69, 0x67, 0x68, 0x74, 0x20, 0x28, 0x63, 0x29, 0x20, 0x31, 0x39, 0x38, 0x36, 0x2d,
    0x32, 0x30, 0x30, 0x38, 0x20, 0x62, 0x79, 0x20, 0x43, 0x69, 0x73, 0x63, 0x6f, 0x20, 0x53, 0x79,
    0x73, 0x74, 0x65, 0x6d, 0x73, 0x2c, 0x20, 0x49, 0x6e, 0x63, 0x2e, 0x0a, 0x43, 0x6f, 0x6d, 0x70,
    0x69, 0x6c, 0x65, 0x64, 0x20, 0x53, 0x61, 0x74, 0x20, 0x30, 0x35, 0x2d, 0x4a, 0x61, 0x6e, 0x2d,
    0x30, 0x38, 0x20, 0x30, 0x30, 0x3a, 0x31, 0x35, 0x20, 0x62, 0x79, 0x20, 0x77, 0x65, 0x69, 0x6c,
    0x69, 0x75, 0x00, 0x06, 0x00, 0x17, 0x63, 0x69, 0x73, 0x63, 0x6f, 0x20, 0x57, 0x53, 0x2d, 0x43,
    0x33, 0x35, 0x36, 0x30, 0x2d, 0x32, 0x34, 0x54, 0x53, 0x00, 0x02, 0x00, 0x11, 0x00, 0x00, 0x00,
    0x01, 0x01, 0x01, 0xcc, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x14, 0x46, 0x61,
    0x73, 0x74, 0x45, 0x74, 0x68, 0x65, 0x72, 0x6e, 0x65, 0x74, 0x30, 0x2f, 0x31, 0x33, 0x00, 0x04,
    0x00, 0x08, 0x00, 0x00, 0x00, 0x28, 0x00, 0x08, 0x00, 0x24, 0x00, 0x00, 0x0c, 0x01, 0x12, 0x00,
    0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02, 0x20, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x18, 0xba, 0x98, 0x68, 0x80, 0xff, 0x00, 0x00, 0x00, 0x09, 0x00, 0x04, 0x00, 0x0a,
    0x00, 0x06, 0x00, 0x01, 0x00, 0x0b, 0x00, 0x05, 0x01, 0x00, 0x12, 0x00, 0x05, 0x00, 0x00, 0x13,
    0x00, 0x05, 0x00, 0x00, 0x16, 0x00, 0x11, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0xcc, 0x00, 0x04,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x00, 0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00};

unsigned char expected_lacp_packet[512] = {
    0x01, 0x80, 0xc2, 0x00, 0x00, 0x02, 0x00, 0x13, 0xc4, 0x12, 0x0f, 0x0d, 0x81, 0x00, 0x00, 0x0a,
    0x88, 0x09, 0x01, 0x01,
    0x01, 0x14, 0x80, 0x00, 0x00, 0x13, 0xc4, 0x12, 0x0f, 0x00, 0x00, 0x0d, 0x80, 0x00, 0x00, 0x16,
    0x85, 0x00, 0x00, 0x00, 0x02, 0x14, 0x80, 0x00, 0x00, 0x0e, 0x83, 0x16, 0xf5, 0x00, 0x00, 0x0d,
    0x80, 0x00, 0x00, 0x19, 0x36, 0x00, 0x00, 0x00, 0x03, 0x10, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

unsigned char expected_bypass_packet1[512] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x81, 0x00,
    0x00, 0x0a, 0x08, 0x00, 0x12, 0x34, 0x45, 0x78, 0x00, 0x2e, 0x00, 0x00, 0x00, 0x00,
    0x40, 0xff, 0x84, 0x57, 0x14, 0x00, 0x00, 0x01, 0xe1, 0x00, 0x00, 0x01, 0x14, 0x15,
    0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23,
    0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2d, 0x0a, 0x03, 0xab,
    0x3c, 0x9b, 0x49, 0xed};

/*
 *
 * Verify behavior by sending packets from CPU -> ingress port(loopback) -> egress_port(loopback) -> CPU.
 *
 */
bcm_error_t
test_verify(int unit)
{
    char uc_pkt[512];
    char uc_enc_pkt[512];
    char mc_pkt[512];
    char eapol_pkt[512];
    char bpdu_pkt[512];
    char cdp_pkt[1024];
    char lacp_pkt[1024];
    char bypass_uc_pkt[512];
    bcm_field_entry_t   eid1;
    int expected_rx_count;
    int pass1, pass2, pass3, pass4, pass5, pass6, pass7, pass8, pass9, pass10, pass11, pass12, pass13;
    char str[1024];
    uint64 val;

    bcm_pbmp_t pbmp;
    bcm_vlan_t vid_tmp = 0;
    bcm_vlan_t vidmask_tmp = 0;
    bcm_field_entry_t ret_entry;
    int ret_stat_id;

    snprintf(uc_pkt, 512,"%s","0000000000220000000000018100000Affff4578002E0000000040FF845714000001E10000011415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2D0A03AB");
    snprintf(uc_enc_pkt, 512,"%s","0000000000220000000000018100000a88e52C000000000500000000000000088b940d0a423ef67e039230ce0bbf27aa5442de939c35ad510d5c0b0dfb1897117e6d6fe575e1212b0a4674544eb0ecab8d54d2c0be7a3527f56e8c63345b8a02f135e73d");

    snprintf(eapol_pkt, 512,"%s","0180c20000030000000000018100000A888e01000005010100050100000000000000000000000000000000000000000000000000000000000000000000000000");

    snprintf(bpdu_pkt, 512,"%s", "0180c20000000000000000018100000A0079424203000003027c8000000c305dd100000000008000000c305dd10080050000140002000f00000050000000000000000000000000000000000000000000000000000000000000000000000055bf4e8a44b25d442868549c1bf7720f00030d408000001aa197d180137c8005000c305dd10000030d40808013");

    snprintf(cdp_pkt, 1024, "%s", "01000ccccccc0018ba98688f8100000A0176aaaa0300000c200002b40bea000100065331000500c2436973636f20494f5320536f6674776172652c20433335363020536f667477617265202843333536302d414456495053455256494345534b392d4d292c2056657273696f6e2031322e322834342953452c2052454c4541534520534f4654574152452028666331290a436f707972696768742028632920313938362d3230303820627920436973636f2053797374656d732c20496e632e0a436f6d70696c6564205361742030352d4a616e2d30382030303a3135206279207765696c697500060017636973636f2057532d43333536302d3234545300020011000000010101cc000400000000000300144661737445746865726e6574302f313300040008000000280008002400000c011200000000ffffffff010220ff0000000000000018ba986880ff000000090004000a00060001000b0005010012000500001300050000160011000000010101cc000400000000001a00100000000100000000ffffffff");

    snprintf(lacp_pkt, 512, "%s", "0180c20000020013c4120f0d8100000A88090101011480000013c4120f00000d800000168500000002148000000e8316f500000d80000019360000000310800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");

    snprintf(bypass_uc_pkt, 512,"%s","0000000000220000000000018100000A080012344578002E0000000040FF845714000001E10000011415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2D0A03AB");

    bshell(unit, "e; vlan show");
    bshell(unit, "e; l2 show; e");

    print "==========  Test1: MACsec Encryption - Unicast ==================";
    port_discard(unit, ingress_port, 0);
    port_discard(unit, egress_port1, 1);
    snprintf(str, 512, "l2 add module=%d port=%d mac=00:00:00:00:00:22 vlan=10 ReplacePriority=0 static=t", mod_id, egress_port1);
    bshell(unit, str);
    printf(">>> Input Unicast packet @ port-%d:\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x%s; sleep quiet 1", ingress_port, uc_pkt);

    printf(">>> Output MACsec encrypted packet with SCI-x @ port-%d:\n", egress_port1);
    create_ifp_to_cpu_rule(unit, _group_config.group, &eid1, egress_port1);

    test_failed = 0;
    rx_count = 0;
    expected_rx_count = 1;
    packet_patterns = 1;
    expected_packets[0]  = expected_encrypted_packet1;
    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port1);
    bshell(unit, str);
    destroy_ifp_to_cpu_rule (unit, eid1);
    bshell(unit, "sleep quiet 1");
    pass1 = (test_failed == 0) && (rx_count == expected_rx_count);
    printf("\n----------------------------------------------------------- \n");
    printf("Test1 = [%s]", pass1? "PASS":"FAIL");
    printf("\n----------------------------------------------------------- \n");

    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_ENCRYPT, macsec_port1.encrypt_sc_id,
                           bcmXflowMacsecSecyTxSCStatsEncryptedPkts, &val);
    printf("Encrypted pkt count:\n");
    print val;
    printf("\n");
    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_DECRYPT, macsec_port1.decrypt_sc_id,
                           bcmXflowMacsecSecyRxSCStatsOKPkts, &val);
    printf("Decrypted pkt count:\n");
    print val;
    printf("\n");

    dump_fp_stats(unit);

    print "==========  Test2: MACsec Encryption - BC/DLF ==================";
    snprintf(mc_pkt, 512,"%s","FFFFFFFFFFFF0000000000018100000Affff4578002E0000000040FF845714000001E10000011415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2D0A03AB");
    port_discard(unit, ingress_port, 0);
    port_discard(unit, egress_port1, 1);
    printf(">>> Input BC/DLF packet @ port-%d:\n", ingress_port);
    printf(">>> Output MACsec encrypted packet with SCI-x @ port-%d:\n", egress_port1);
    create_ifp_to_cpu_rule(unit, _group_config.group, &eid1, egress_port1);
    snprintf(str, 512, "tx 1 pbm=%d data=0x%s; sleep quiet 1", ingress_port, mc_pkt);

    test_failed = 0;
    rx_count = 0;
    expected_rx_count = 1;
    packet_patterns = 1;
    expected_packets[0]  = expected_encrypted_mc_packet1;
    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , egress_port1);
    bshell(unit, str);
    destroy_ifp_to_cpu_rule (unit, eid1);
    bshell(unit, "sleep quiet 1");
    pass2 = (test_failed == 0) && (rx_count == expected_rx_count);
    printf("\n----------------------------------------------------------- \n");
    printf("Test2 = [%s]", pass2? "PASS":"FAIL");
    printf("\n----------------------------------------------------------- \n");

    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_ENCRYPT, macsec_port1.encrypt_sc_id,
                           bcmXflowMacsecSecyTxSCStatsEncryptedPkts, &val);
    printf("Encrypted pkt count:\n");
    print val;
    printf("\n");
    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_DECRYPT, macsec_port1.decrypt_sc_id,
                           bcmXflowMacsecSecyRxSCStatsOKPkts, &val);
    printf("Decrypted pkt count:\n");
    print val;
    printf("\n");

    dump_fp_stats(unit);

    print "==========  Test3: MACsec decryption - Unicast ==================";
    port_discard(unit, ingress_port, 1);
    port_discard(unit, egress_port1, 0);
    snprintf(str, 512, "l2 add module=%d port=%d mac=00:00:00:00:00:22 vlan=10 ReplacePriority=0 static=t", mod_id, ingress_port);
    bshell(unit, str);

    printf(">>> Input Unicast packet @ port-%d:\n", egress_port1);
    snprintf(str, 512, "tx 1 pbm=%d data=0x%s ; sleep quiet 1", egress_port1, uc_enc_pkt);

    printf(">>> Output decrypted pkt @ port-%d:\n", ingress_port);

    create_ifp_to_cpu_rule(unit, _group_config.group, &eid1, ingress_port);

    test_failed = 0;
    rx_count = 0;
    expected_rx_count = 1;
    packet_patterns = 1;
    expected_packets[0]  = expected_decrypted_packet2;
    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , ingress_port);
    bshell(unit, str);
    destroy_ifp_to_cpu_rule (unit, eid1);

    bshell(unit, "sleep quiet 1");
    pass3 = (test_failed == 0) && (rx_count == expected_rx_count);
    printf("\n----------------------------------------------------------- \n");
    printf("Test3 = [%s]", pass3? "PASS":"FAIL");
    printf("\n----------------------------------------------------------- \n");

    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_ENCRYPT, macsec_port1.encrypt_sc_id,
                           bcmXflowMacsecSecyTxSCStatsEncryptedPkts, &val);
    printf("Encrypted pkt count:\n");
    print val;
    printf("\n");
    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_DECRYPT, macsec_port1.decrypt_sc_id,
                           bcmXflowMacsecSecyRxSCStatsOKPkts, &val);
    printf("Decrypted pkt count:\n");
    print val;
    printf("\n");

    dump_fp_stats(unit);

    print "==========  Test4: MACsec decryption (drop_untag/pass_untag) - Unicast ==================";
    int pass4_1, pass4_2, pass4_3;
    port_discard(unit, ingress_port, 1);
    port_discard(unit, egress_port1, 0);
    snprintf(str, 512, "l2 add module=%d port=%d mac=00:00:00:00:00:22 vlan=10 ReplacePriority=0 static=t", mod_id, ingress_port);
    bshell(unit, str);

    printf(">>> Input Unicast packet @ port-%d:\n", egress_port1);
    snprintf(str, 512, "tx 1 pbm=%d data=0x%s ; sleep quiet 1", egress_port1, uc_pkt);

    printf(">>> Output decrypted pkt @ port-%d:\n", ingress_port);

    create_ifp_to_cpu_rule(unit, _group_config.group, &eid1, ingress_port);

    test_failed = 0;
    rx_count = 0;
    expected_rx_count = 1;
    packet_patterns = 1;
    expected_packets[0]  = expected_decrypted_packet2;
    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , ingress_port);
    bshell(unit, str);
    destroy_ifp_to_cpu_rule (unit, eid1);

    bshell(unit, "sleep quiet 1");
    pass4_1 = (test_failed == 0) && (rx_count == expected_rx_count);
    printf("\n----------------------------------------------------------- \n");
    printf("Test4_1 = [%s]", pass4_1? "PASS":"FAIL");
    printf("\n----------------------------------------------------------- \n");

    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_ENCRYPT, macsec_port1.encrypt_sc_id,
                           bcmXflowMacsecSecyTxSCStatsEncryptedPkts, &val);
    printf("Encrypted pkt count:\n");
    print val;
    printf("\n");
    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_DECRYPT, macsec_port1.decrypt_sc_id,
                           bcmXflowMacsecSecyRxSCStatsOKPkts, &val);
    printf("Decrypted pkt count:\n");
    print val;
    printf("\n");

    dump_fp_stats(unit);

    /* bind to policy_id[0] which is set to UNTAGGED_CONTROL_PORT_ENABLE=0 */
    BCM_IF_ERROR_RETURN(macsec_flow_policy_bind(unit, &macsec_port1.cleartext_flow, policy_id[0]));

    create_ifp_to_cpu_rule(unit, _group_config.group, &eid1, ingress_port);
    printf(">>> Input Unicast secTag untagged packet @ port-%d:\n", egress_port1);
    snprintf(str, 512, "tx 1 pbm=%d data=0x%s ; sleep quiet 1", egress_port1, uc_pkt);

    test_failed = 0;
    rx_count = 0;
    expected_rx_count = 0;
    packet_patterns = 1;
    expected_packets[0]  = expected_decrypted_packet2;
    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , ingress_port);
    bshell(unit, str);
    destroy_ifp_to_cpu_rule (unit, eid1);

    bshell(unit, "sleep quiet 1");
    pass4_2 = (test_failed == 0) && (rx_count == expected_rx_count);
    printf("\n----------------------------------------------------------- \n");
    printf("Test4_2 = [%s]", pass4_2? "PASS":"FAIL");
    printf("\n----------------------------------------------------------- \n");

    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_ENCRYPT, macsec_port1.encrypt_sc_id,
                           bcmXflowMacsecSecyTxSCStatsEncryptedPkts, &val);
    printf("Encrypted pkt count:\n");
    print val;
    printf("\n");
    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_DECRYPT, macsec_port1.decrypt_sc_id,
                           bcmXflowMacsecSecyRxSCStatsOKPkts, &val);
    printf("Decrypted pkt count:\n");
    print val;
    printf("\n");

    dump_fp_stats(unit);

    /* bind to policy_id[1] which is set to UNTAGGED_CONTROL_PORT_ENABLE=1 */
    BCM_IF_ERROR_RETURN(macsec_flow_policy_bind(unit, &macsec_port1.cleartext_flow, policy_id[1]));

    create_ifp_to_cpu_rule(unit, _group_config.group, &eid1, ingress_port);
    printf(">>> Input Unicast secTag untagged packet @ port-%d:\n", egress_port1);
    snprintf(str, 512, "tx 1 pbm=%d data=0x%s ; sleep quiet 1", egress_port1, uc_pkt);

    test_failed = 0;
    rx_count = 0;
    expected_rx_count = 1;
    packet_patterns = 1;
    expected_packets[0]  = expected_decrypted_packet2;
    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , ingress_port);
    bshell(unit, str);
    destroy_ifp_to_cpu_rule (unit, eid1);

    bshell(unit, "sleep quiet 1");
    pass4_3 = (test_failed == 0) && (rx_count == expected_rx_count);
    printf("\n----------------------------------------------------------- \n");
    printf("Test4_3 = [%s]", pass4_3? "PASS":"FAIL");
    printf("\n----------------------------------------------------------- \n");

    dump_fp_stats(unit);

    pass4 = pass4_1 && pass4_2 && pass4_3;
    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_ENCRYPT, macsec_port1.encrypt_sc_id,
                           bcmXflowMacsecSecyTxSCStatsEncryptedPkts, &val);
    printf("Encrypted pkt count:\n");
    print val;
    printf("\n");
    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_DECRYPT, macsec_port1.decrypt_sc_id,
                           bcmXflowMacsecSecyRxSCStatsOKPkts, &val);
    printf("Decrypted pkt count:\n");
    print val;
    printf("\n");

    print "==========  Test5: MACsec decryption (bypass EAPoL)  ==================";
    /* Forwarding for EAPoL packets received from egress_port1 to CPU and ingress_port */
    /* Note that packets looped back from ingress_port will also hit control packet VFP filter */
    uint8 eapol_dst[6] = {0x01,0x80,0xc2,0x00,0x00,0x03};
    uint8 eapol_dst_mask[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
    BCM_PBMP_CLEAR(pbmp);
    /* forward to CPU and port 1 */
    BCM_PBMP_PORT_ADD(pbmp, 0);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port);

    BCM_IF_ERROR_RETURN(remove_l2user(unit, eapol_dst, eapol_dst_mask, vid_tmp, vidmask_tmp));
    /* Redirct to CPU after 2nd pass MACsec decrytion */

    BCM_IF_ERROR_RETURN(create_ifp_to_redirect_decrypted_flow(unit, eapol_dst, eapol_dst_mask,
                                                              vid_tmp, vidmask_tmp, pbmp, &ret_entry, &ret_stat_id));
    port_discard(unit, ingress_port, 1);
    port_discard(unit, egress_port1, 0);

    /* bind to policy_id[0] which is set to UNTAGGED_CONTROL_PORT_ENABLE=0 */
    BCM_IF_ERROR_RETURN(macsec_flow_policy_bind(unit, &macsec_port1.cleartext_flow, policy_id[0]));

    /* bind to policy_id[1] which is set to UNTAGGED_CONTROL_PORT_ENABLE=1 */
    BCM_IF_ERROR_RETURN(macsec_flow_policy_bind(unit, &macsec_port1.bypass_flow[0], policy_id[1]));

    create_ifp_to_cpu_rule(unit, _group_config.group, &eid1, ingress_port);
    printf(">>> Input Unicast secTag untagged packet @ port-%d:\n", egress_port1);
    snprintf(str, 512, "tx 1 pbm=%d data=0x%s ; sleep quiet 1", egress_port1, eapol_pkt);

    test_failed = 0;
    rx_count = 0;
    expected_rx_count = 2;
    packet_patterns = 2;
    expected_packets[0]  = expected_eapol_packet;
    expected_packets[1]  = expected_eapol_packet_crc;
    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0], egress_port1);
    BCM_PBMP_CLEAR(expected_pbmp[1]);
    BCM_PBMP_PORT_ADD(expected_pbmp[1], ingress_port);

    bshell(unit, str);
    destroy_ifp_to_cpu_rule (unit, eid1);

    BCM_IF_ERROR_RETURN(bcm_field_entry_stat_detach(unit, ret_entry, ret_stat_id));
    BCM_IF_ERROR_RETURN(bcm_field_stat_destroy(unit, ret_stat_id));
    BCM_IF_ERROR_RETURN(bcm_field_entry_destroy(unit, ret_entry));

    bshell(unit, "sleep quiet 1");
    pass5 = (test_failed == 0) && (rx_count == expected_rx_count);
    printf("\n----------------------------------------------------------- \n");
    printf("Test5 = [%s]", pass5? "PASS":"FAIL");
    printf("\n----------------------------------------------------------- \n");

    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_ENCRYPT, macsec_port1.encrypt_sc_id,
                           bcmXflowMacsecSecyTxSCStatsEncryptedPkts, &val);
    printf("Encrypted pkt count:\n");
    print val;
    printf("\n");

    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_DECRYPT, macsec_port1.decrypt_sc_id,
                           bcmXflowMacsecSecyRxSCStatsOKPkts, &val);
    printf("Decrypted pkt count:\n");
    print val;
    printf("\n");

    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_DECRYPT, macsec_port1.bypass_flow[0].policy_id,
                           bcmXflowMacsecUnctrlPortInMulticastPkts, &val);
    printf("In Management pkt count:\n");
    print val;
    printf("\n");

    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_ENCRYPT, macsec_port1.encrypt_sc_id,
                           bcmXflowMacsecUnctrlPortOutMulticastPkts, &val);
    printf("Out management pkt count:\n");
    print val;
    printf("\n");

    dump_fp_stats(unit);

    print "==========  Test6: MACsec decryption (bypass BPDU)  ==================";
    /* Forwarding for BPDU packets received from egress_port1 to CPU only */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, 0); /* forward to CPU only */

    BCM_IF_ERROR_RETURN(remove_l2user(unit, bpdu_dst, bpdu_dst_mask, vid_tmp, vidmask_tmp));
    /* Redirct to CPU after 2nd pass MACsec decrytion */
    BCM_IF_ERROR_RETURN(create_ifp_to_redirect_decrypted_flow(unit, bpdu_dst, bpdu_dst_mask,
                                                              vid_tmp, vidmask_tmp, pbmp, &ret_entry, &ret_stat_id));
    port_discard(unit, ingress_port, 1);
    port_discard(unit, egress_port1, 0);

    /* bind to policy_id[0] which is set to UNTAGGED_CONTROL_PORT_ENABLE=0 */
    BCM_IF_ERROR_RETURN(macsec_flow_policy_bind(unit, &macsec_port1.cleartext_flow, policy_id[0]));

    /* bind to policy_id[1] which is set to UNTAGGED_CONTROL_PORT_ENABLE=1 */
    BCM_IF_ERROR_RETURN(macsec_flow_policy_bind(unit, &macsec_port1.bypass_flow[2], policy_id[1]));

    create_ifp_to_cpu_rule(unit, _group_config.group, &eid1, ingress_port);
    printf(">>> Input Unicast secTag untagged packet @ port-%d:\n", egress_port1);
    snprintf(str, 512, "tx 1 pbm=%d data=0x%s ; sleep quiet 1", egress_port1, bpdu_pkt);

    test_failed = 0;
    rx_count = 0;
    expected_rx_count = 1;
    packet_patterns = 1;
    expected_packets[0]  = expected_bpdu_packet;
    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0], egress_port1);
    bshell(unit, str);
    destroy_ifp_to_cpu_rule (unit, eid1);
    BCM_IF_ERROR_RETURN(bcm_field_entry_stat_detach(unit, ret_entry, ret_stat_id));
    BCM_IF_ERROR_RETURN(bcm_field_stat_destroy(unit, ret_stat_id));
    BCM_IF_ERROR_RETURN(bcm_field_entry_destroy(unit, ret_entry));

    bshell(unit, "sleep quiet 1");
    pass6 = (test_failed == 0) && (rx_count == expected_rx_count);
    printf("\n----------------------------------------------------------- \n");
    printf("Test6 = [%s]", pass6? "PASS":"FAIL");
    printf("\n----------------------------------------------------------- \n");

    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_ENCRYPT, macsec_port1.encrypt_sc_id,
                           bcmXflowMacsecSecyTxSCStatsEncryptedPkts, &val);
    printf("Encrypted pkt count:\n");
    print val;
    printf("\n");
    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_DECRYPT, macsec_port1.decrypt_sc_id,
                           bcmXflowMacsecSecyRxSCStatsOKPkts, &val);
    printf("Decrypted pkt count:\n");
    print val;
    printf("\n");

    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_DECRYPT, macsec_port1.bypass_flow[2].policy_id,
                           bcmXflowMacsecUnctrlPortInMulticastPkts, &val);
    printf("In Management pkt count:\n");
    print val;
    printf("\n");

    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_ENCRYPT, macsec_port1.encrypt_sc_id,
                           bcmXflowMacsecUnctrlPortOutMulticastPkts, &val);
    printf("Out management pkt count:\n");
    print val;
    printf("\n");

    dump_fp_stats(unit);

    print "==========  Test7: MACsec decryption (bypass CDP)  ==================";
    /* Forwarding for BPDU packets received from egress_port1 to CPU only */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, 0);  /* forward to CPU only */

    BCM_IF_ERROR_RETURN(remove_l2user(unit, cdp_dst, cdp_dst_mask, vid_tmp, vidmask_tmp));
    /* Redirct to CPU after 2nd pass MACsec decrytion */
    BCM_IF_ERROR_RETURN(create_ifp_to_redirect_decrypted_flow(unit, cdp_dst, cdp_dst_mask,
                                                              vid_tmp, vidmask_tmp, pbmp, &ret_entry, &ret_stat_id));
    port_discard(unit, ingress_port, 1);
    port_discard(unit, egress_port1, 0);

    /* bind to policy_id[0] which is set to UNTAGGED_CONTROL_PORT_ENABLE=0 */
    BCM_IF_ERROR_RETURN(macsec_flow_policy_bind(unit, &macsec_port1.cleartext_flow, policy_id[0]));

    /* bind to policy_id[1] which is set to UNTAGGED_CONTROL_PORT_ENABLE=1 */
    BCM_IF_ERROR_RETURN(macsec_flow_policy_bind(unit, &macsec_port1.bypass_flow[3], policy_id[1]));

    create_ifp_to_cpu_rule(unit, _group_config.group, &eid1, ingress_port);
    printf(">>> Input Unicast secTag untagged packet @ port-%d:\n", egress_port1);
    snprintf(str, 1024, "tx 1 pbm=%d data=0x%s ; sleep quiet 1", egress_port1, cdp_pkt);

    test_failed = 0;
    rx_count = 0;
    expected_rx_count = 1;
    packet_patterns = 1;
    expected_packets[0]  = expected_cdp_packet;
    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0], egress_port1);
    bshell(unit, str);
    destroy_ifp_to_cpu_rule (unit, eid1);
    BCM_IF_ERROR_RETURN(bcm_field_entry_stat_detach(unit, ret_entry, ret_stat_id));
    BCM_IF_ERROR_RETURN(bcm_field_stat_destroy(unit, ret_stat_id));
    BCM_IF_ERROR_RETURN(bcm_field_entry_destroy(unit, ret_entry));

    bshell(unit, "sleep quiet 1");
    pass7 = (test_failed == 0) && (rx_count == expected_rx_count);
    printf("\n----------------------------------------------------------- \n");
    printf("Test7 = [%s]", pass7? "PASS":"FAIL");
    printf("\n----------------------------------------------------------- \n");

    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_ENCRYPT, macsec_port1.encrypt_sc_id,
                           bcmXflowMacsecSecyTxSCStatsEncryptedPkts, &val);
    printf("Encrypted pkt count:\n");
    print val;
    printf("\n");
    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_DECRYPT, macsec_port1.decrypt_sc_id,
                           bcmXflowMacsecSecyRxSCStatsOKPkts, &val);
    printf("Decrypted pkt count:\n");
    print val;
    printf("\n");

    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_DECRYPT, macsec_port1.bypass_flow[3].policy_id,
                           bcmXflowMacsecUnctrlPortInMulticastPkts, &val);
    printf("In Management pkt count:\n");
    print val;
    printf("\n");

    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_ENCRYPT, macsec_port1.encrypt_sc_id,
                           bcmXflowMacsecUnctrlPortOutMulticastPkts, &val);
    printf("Out management pkt count:\n");
    print val;
    printf("\n");

    dump_fp_stats(unit);

    print "==========  Test8: MACsec decryption (bypass LACP)  ==================";
    /* Forwarding LACP packets received from egress_port1 to CPU only */
    BCM_PBMP_CLEAR(pbmp);
    /* forward to CPU */
    BCM_PBMP_PORT_ADD(pbmp, 0);

    BCM_IF_ERROR_RETURN(remove_l2user(unit, slow_protocol_dst, slow_protocol_dst_mask, vid_tmp, vidmask_tmp));
    /* Redirct to CPU after 2nd pass MACsec decrytion */
    BCM_IF_ERROR_RETURN(create_ifp_to_redirect_decrypted_flow(unit, slow_protocol_dst, slow_protocol_dst_mask,
                                                              vid_tmp, vidmask_tmp, pbmp, &ret_entry, &ret_stat_id));
    port_discard(unit, ingress_port, 1);
    port_discard(unit, egress_port1, 0);

    /* bind to policy_id[0] which is set to UNTAGGED_CONTROL_PORT_ENABLE=0 */
    BCM_IF_ERROR_RETURN(macsec_flow_policy_bind(unit, &macsec_port1.cleartext_flow, policy_id[0]));

    /* bind to policy_id[1] which is set to UNTAGGED_CONTROL_PORT_ENABLE=1 */
    BCM_IF_ERROR_RETURN(macsec_flow_policy_bind(unit, &macsec_port1.bypass_flow[4], policy_id[1]));

    create_ifp_to_cpu_rule(unit, _group_config.group, &eid1, ingress_port);
    printf(">>> Input Unicast secTag untagged packet @ port-%d:\n", egress_port1);
    snprintf(str, 512, "tx 1 pbm=%d data=0x%s ; sleep quiet 1", egress_port1, lacp_pkt);

    test_failed = 0;
    rx_count = 0;
    expected_rx_count = 1;
    packet_patterns = 1;
    expected_packets[0]  = expected_lacp_packet;
    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0], egress_port1);

    bshell(unit, str);
    destroy_ifp_to_cpu_rule (unit, eid1);
    BCM_IF_ERROR_RETURN(bcm_field_entry_stat_detach(unit, ret_entry, ret_stat_id));
    BCM_IF_ERROR_RETURN(bcm_field_stat_destroy(unit, ret_stat_id));
    BCM_IF_ERROR_RETURN(bcm_field_entry_destroy(unit, ret_entry));

    bshell(unit, "sleep quiet 1");
    pass8 = (test_failed == 0) && (rx_count == expected_rx_count);
    printf("\n----------------------------------------------------------- \n");
    printf("Test8 = [%s]", pass8? "PASS":"FAIL");
    printf("\n----------------------------------------------------------- \n");

    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_ENCRYPT, macsec_port1.encrypt_sc_id,
                           bcmXflowMacsecSecyTxSCStatsEncryptedPkts, &val);
    printf("Encrypted pkt count:\n");
    print val;
    printf("\n");
    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_DECRYPT, macsec_port1.decrypt_sc_id,
                           bcmXflowMacsecSecyRxSCStatsOKPkts, &val);
    printf("Decrypted pkt count:\n");
    print val;
    printf("\n");

    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_DECRYPT, macsec_port1.bypass_flow[4].policy_id,
                           bcmXflowMacsecUnctrlPortInMulticastPkts, &val);
    printf("In Management pkt count:\n");
    print val;
    printf("\n");

    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_ENCRYPT, macsec_port1.encrypt_sc_id,
                           bcmXflowMacsecUnctrlPortOutMulticastPkts, &val);
    printf("Out management pkt count:\n");
    print val;
    printf("\n");

    dump_fp_stats(unit);

    print "==========  Test9: MACsec decryption (bypass) - Unicast ==================";
    port_discard(unit, ingress_port, 1);
    port_discard(unit, egress_port1, 0);

    /* bind to policy_id[0] which is set to UNTAGGED_CONTROL_PORT_ENABLE=0 */
    BCM_IF_ERROR_RETURN(macsec_flow_policy_bind(unit, &macsec_port1.cleartext_flow, policy_id[0]));

    /* bind to policy_id[1] which is set to UNTAGGED_CONTROL_PORT_ENABLE=1 */
    BCM_IF_ERROR_RETURN(macsec_flow_policy_bind(unit, &macsec_port1.bypass_flow[1], policy_id[1]));

    create_ifp_to_cpu_rule(unit, _group_config.group, &eid1, ingress_port);
    printf(">>> Input Unicast secTag untagged packet @ port-%d:\n", egress_port1);
    snprintf(str, 512, "tx 1 pbm=%d data=0x%s ; sleep quiet 1", egress_port1, bypass_uc_pkt);

    test_failed = 0;
    rx_count = 0;
    expected_rx_count = 1;
    packet_patterns = 1;
    expected_packets[0]  = expected_bypass_packet1;
    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0] , ingress_port);
    bshell(unit, str);
    destroy_ifp_to_cpu_rule (unit, eid1);

    bshell(unit, "sleep quiet 1");
    pass9 = (test_failed == 0) && (rx_count == expected_rx_count);
    printf("\n----------------------------------------------------------- \n");
    printf("Test9 = [%s]", pass9? "PASS":"FAIL");
    printf("\n----------------------------------------------------------- \n");

    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_ENCRYPT, macsec_port1.encrypt_sc_id,
                           bcmXflowMacsecSecyTxSCStatsEncryptedPkts, &val);
    printf("Encrypted pkt count:\n");
    print val;
    printf("\n");
    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_DECRYPT, macsec_port1.decrypt_sc_id,
                           bcmXflowMacsecSecyRxSCStatsOKPkts, &val);
    printf("Decrypted pkt count:\n");
    print val;
    printf("\n");

    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_DECRYPT, macsec_port1.bypass_flow[1].policy_id,
                           bcmXflowMacsecUnctrlPortInMulticastPkts, &val);
    printf("In Management pkt count:\n");
    print val;
    printf("\n");

    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_ENCRYPT, macsec_port1.encrypt_sc_id,
                           bcmXflowMacsecUnctrlPortOutMulticastPkts, &val);
    printf("Out management pkt count:\n");
    print val;
    printf("\n");

    dump_fp_stats(unit);

    print "==========  Test10: MACsec encryption+decryption (bypass EAPoL in both directions)  ==================";
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, 0);

    BCM_IF_ERROR_RETURN(remove_l2user(unit, eapol_dst, eapol_dst_mask, vid_tmp, vidmask_tmp));
    /* Redirct to CPU after 2nd pass MACsec decrytion */
    BCM_IF_ERROR_RETURN(create_ifp_to_redirect_decrypted_flow(unit, eapol_dst, eapol_dst_mask,
                                                              vid_tmp, vidmask_tmp, pbmp, &ret_entry, &ret_stat_id));
    port_discard(unit, ingress_port, 0);
    port_discard(unit, egress_port1, 0);

    /* bind to policy_id[0] which is set to UNTAGGED_CONTROL_PORT_ENABLE=0 */
    BCM_IF_ERROR_RETURN(macsec_flow_policy_bind(unit, &macsec_port1.cleartext_flow, policy_id[0]));

    /* bind to policy_id[1] which is set to UNTAGGED_CONTROL_PORT_ENABLE=1 */
    BCM_IF_ERROR_RETURN(macsec_flow_policy_bind(unit, &macsec_port1.bypass_flow[0], policy_id[1]));

    printf(">>> Input Unicast secTag untagged packet @ port-%d:\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x%s ; sleep quiet 1", ingress_port, eapol_pkt);
    test_failed = 0;
    rx_count = 0;
    expected_rx_count = 1;
    packet_patterns = 1;
    expected_packets[0]  = expected_eapol_packet;
    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0], egress_port1);

    bshell(unit, str);

    bshell(unit, "sleep quiet 1");

    pass10 = (test_failed == 0) && (rx_count == expected_rx_count);
    printf("\n----------------------------------------------------------- \n");
    printf("Test10 = [%s]", pass10? "PASS":"FAIL");
    printf("\n----------------------------------------------------------- \n");

    BCM_IF_ERROR_RETURN(bcm_field_entry_stat_detach(unit, ret_entry, ret_stat_id));
    BCM_IF_ERROR_RETURN(bcm_field_stat_destroy(unit, ret_stat_id));
    BCM_IF_ERROR_RETURN(bcm_field_entry_destroy(unit, ret_entry));

    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_ENCRYPT, macsec_port1.encrypt_sc_id,
                           bcmXflowMacsecSecyTxSCStatsEncryptedPkts, &val);
    printf("Encrypted pkt count:\n");
    print val;
    printf("\n");
    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_DECRYPT, macsec_port1.decrypt_sc_id,
                           bcmXflowMacsecSecyRxSCStatsOKPkts, &val);
    printf("Decrypted pkt count:\n");
    print val;
    printf("\n");

    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_DECRYPT, macsec_port1.bypass_flow[0].policy_id,
                           bcmXflowMacsecUnctrlPortInMulticastPkts, &val);
    printf("In Management pkt count:\n");
    print val;
    printf("\n");

    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_ENCRYPT, macsec_port1.encrypt_sc_id,
                           bcmXflowMacsecUnctrlPortOutMulticastPkts, &val);
    printf("Out management pkt count:\n");
    print val;
    printf("\n");

    dump_fp_stats(unit);

    print "==========  Test11: MACsec encryption+decryption (bypass BPDU in both directions)  ==================";
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, 0);

    BCM_IF_ERROR_RETURN(remove_l2user(unit, bpdu_dst, bpdu_dst_mask, vid_tmp, vidmask_tmp));
    /* Redirct to CPU after 2nd pass MACsec decrytion */
    BCM_IF_ERROR_RETURN(create_ifp_to_redirect_decrypted_flow(unit, bpdu_dst, bpdu_dst_mask,
                                                              vid_tmp, vidmask_tmp, pbmp, &ret_entry, &ret_stat_id));
    port_discard(unit, ingress_port, 0);
    port_discard(unit, egress_port1, 0);

    /* bind to policy_id[0] which is set to UNTAGGED_CONTROL_PORT_ENABLE=0 */
    BCM_IF_ERROR_RETURN(macsec_flow_policy_bind(unit, &macsec_port1.cleartext_flow, policy_id[0]));

    /* bind to policy_id[1] which is set to UNTAGGED_CONTROL_PORT_ENABLE=1 */
    BCM_IF_ERROR_RETURN(macsec_flow_policy_bind(unit, &macsec_port1.bypass_flow[2], policy_id[1]));

    printf(">>> Input Unicast secTag untagged packet @ port-%d:\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x%s ; sleep quiet 1", ingress_port, bpdu_pkt);
    test_failed = 0;
    rx_count = 0;
    expected_rx_count = 1;
    packet_patterns = 1;
    expected_packets[0]  = expected_bpdu_packet;
    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0], egress_port1);

    bshell(unit, str);

    bshell(unit, "sleep quiet 1");

    pass11 = (test_failed == 0) && (rx_count == expected_rx_count);
    printf("\n----------------------------------------------------------- \n");
    printf("Test11 = [%s]", pass11? "PASS":"FAIL");
    printf("\n----------------------------------------------------------- \n");

    BCM_IF_ERROR_RETURN(bcm_field_entry_stat_detach(unit, ret_entry, ret_stat_id));
    BCM_IF_ERROR_RETURN(bcm_field_stat_destroy(unit, ret_stat_id));
    BCM_IF_ERROR_RETURN(bcm_field_entry_destroy(unit, ret_entry));

    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_ENCRYPT, macsec_port1.encrypt_sc_id,
                           bcmXflowMacsecSecyTxSCStatsEncryptedPkts, &val);
    printf("Encrypted pkt count:\n");
    print val;
    printf("\n");
    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_DECRYPT, macsec_port1.decrypt_sc_id,
                           bcmXflowMacsecSecyRxSCStatsOKPkts, &val);
    printf("Decrypted pkt count:\n");
    print val;
    printf("\n");

    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_DECRYPT, macsec_port1.bypass_flow[2].policy_id,
                           bcmXflowMacsecUnctrlPortInMulticastPkts, &val);
    printf("In Management pkt count:\n");
    print val;
    printf("\n");

    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_ENCRYPT, macsec_port1.encrypt_sc_id,
                           bcmXflowMacsecUnctrlPortOutMulticastPkts, &val);
    printf("Out management pkt count:\n");
    print val;
    printf("\n");

    dump_fp_stats(unit);

    print "==========  Test12: MACsec encryption+decryption (bypass CDP in both directions)  ==================";
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, 0);

    BCM_IF_ERROR_RETURN(remove_l2user(unit, cdp_dst, cdp_dst_mask, vid_tmp, vidmask_tmp));
    BCM_IF_ERROR_RETURN(create_ifp_to_redirect_decrypted_flow(unit, cdp_dst, cdp_dst_mask,
                                                              vid_tmp, vidmask_tmp, pbmp, &ret_entry, &ret_stat_id));
    port_discard(unit, ingress_port, 0);
    port_discard(unit, egress_port1, 0);

    BCM_IF_ERROR_RETURN(macsec_flow_policy_bind(unit, &macsec_port1.cleartext_flow, policy_id[0]));

    BCM_IF_ERROR_RETURN(macsec_flow_policy_bind(unit, &macsec_port1.bypass_flow[3], policy_id[1]));

    printf(">>> Input Multicast cleartext packet @ port-%d:\n", ingress_port);
    snprintf(str, 1024, "tx 1 pbm=%d data=0x%s ; sleep quiet 1", ingress_port, cdp_pkt);
    test_failed = 0;
    rx_count = 0;
    expected_rx_count = 1;
    packet_patterns = 1;
    expected_packets[0]  = expected_cdp_packet;
    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0], egress_port1);
    bshell(unit, str);

    bshell(unit, "sleep quiet 1");

    pass12 = (test_failed == 0) && (rx_count == expected_rx_count);
    printf("\n----------------------------------------------------------- \n");
    printf("Test12 = [%s]", pass12? "PASS":"FAIL");
    printf("\n----------------------------------------------------------- \n");

    BCM_IF_ERROR_RETURN(bcm_field_entry_stat_detach(unit, ret_entry, ret_stat_id));
    BCM_IF_ERROR_RETURN(bcm_field_stat_destroy(unit, ret_stat_id));
    BCM_IF_ERROR_RETURN(bcm_field_entry_destroy(unit, ret_entry));

    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_ENCRYPT, macsec_port1.encrypt_sc_id,
                           bcmXflowMacsecSecyTxSCStatsEncryptedPkts, &val);
    printf("Encrypted pkt count:\n");
    print val;
    printf("\n");
    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_DECRYPT, macsec_port1.decrypt_sc_id,
                           bcmXflowMacsecSecyRxSCStatsOKPkts, &val);
    printf("Decrypted pkt count:\n");
    print val;
    printf("\n");

    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_DECRYPT, macsec_port1.bypass_flow[3].policy_id,
                           bcmXflowMacsecUnctrlPortInMulticastPkts, &val);
    printf("In Management pkt count:\n");
    print val;
    printf("\n");

    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_ENCRYPT, macsec_port1.encrypt_sc_id,
                           bcmXflowMacsecUnctrlPortOutMulticastPkts, &val);
    printf("Out management pkt count:\n");
    print val;
    printf("\n");

    dump_fp_stats(unit);

    print "==========  Test13: MACsec encryption+decryption (bypass LACP in both directions)  ==================";
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, 0);

    BCM_IF_ERROR_RETURN(remove_l2user(unit, slow_protocol_dst, slow_protocol_dst_mask, vid_tmp, vidmask_tmp));
    BCM_IF_ERROR_RETURN(create_ifp_to_redirect_decrypted_flow(unit, slow_protocol_dst, slow_protocol_dst_mask,
                                                              vid_tmp, vidmask_tmp, pbmp, &ret_entry, &ret_stat_id));
    port_discard(unit, ingress_port, 0);
    port_discard(unit, egress_port1, 0);

    BCM_IF_ERROR_RETURN(macsec_flow_policy_bind(unit, &macsec_port1.cleartext_flow, policy_id[0]));

    BCM_IF_ERROR_RETURN(macsec_flow_policy_bind(unit, &macsec_port1.bypass_flow[3], policy_id[1]));

    printf(">>> Input Multicast cleartext packet @ port-%d:\n", ingress_port);
    snprintf(str, 1024, "tx 1 pbm=%d data=0x%s ; sleep quiet 1", ingress_port, lacp_pkt);
    test_failed = 0;
    rx_count = 0;
    expected_rx_count = 1;
    packet_patterns = 1;
    expected_packets[0]  = expected_lacp_packet;
    BCM_PBMP_CLEAR(expected_pbmp[0]);
    BCM_PBMP_PORT_ADD(expected_pbmp[0], egress_port1);
    bshell(unit, str);

    bshell(unit, "sleep quiet 1");

    pass13 = (test_failed == 0) && (rx_count == expected_rx_count);
    printf("\n----------------------------------------------------------- \n");
    printf("Test13 = [%s]", pass13? "PASS":"FAIL");
    printf("\n----------------------------------------------------------- \n");

    BCM_IF_ERROR_RETURN(bcm_field_entry_stat_detach(unit, ret_entry, ret_stat_id));
    BCM_IF_ERROR_RETURN(bcm_field_stat_destroy(unit, ret_stat_id));
    BCM_IF_ERROR_RETURN(bcm_field_entry_destroy(unit, ret_entry));

    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_ENCRYPT, macsec_port1.encrypt_sc_id,
                           bcmXflowMacsecSecyTxSCStatsEncryptedPkts, &val);
    printf("Encrypted pkt count:\n");
    print val;
    printf("\n");
    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_DECRYPT, macsec_port1.decrypt_sc_id,
                           bcmXflowMacsecSecyRxSCStatsOKPkts, &val);
    printf("Decrypted pkt count:\n");
    print val;
    printf("\n");

    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_DECRYPT, macsec_port1.bypass_flow[0].policy_id,
                           bcmXflowMacsecUnctrlPortInMulticastPkts, &val);
    printf("In Management pkt count:\n");
    print val;
    printf("\n");

    print macsec_get_stats(unit, BCM_XFLOW_MACSEC_ENCRYPT, macsec_port1.encrypt_sc_id,
                           bcmXflowMacsecUnctrlPortOutMulticastPkts, &val);
    printf("Out management pkt count:\n");
    print val;
    printf("\n");

    dump_fp_stats(unit);

    bshell(unit, "sleep quiet 2");
    if (pass1 && pass2 && pass3 && pass4 && pass5 && pass6 && pass7 && pass8 && pass9 && pass10 && pass11 && pass12 && pass13)
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

/*    port_tc_to_q_mapping_get(unit, ingress_port);
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

