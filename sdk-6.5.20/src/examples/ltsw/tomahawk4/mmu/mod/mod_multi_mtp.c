/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
cint_reset();
bcm_cosq_mod_control_t control;
int unit = 0;

/* Enable mirror-on-drop and config mirror-on-drop buffer pool size. */
bcm_cosq_mod_control_t_init(&control);
control.valid_flags = BCM_COSQ_MOD_CONTROL_POOL_LIMIT_VALID;
control.pool_limit = 65024; /* 256 Cells. */

print bcm_cosq_mod_control_set(unit, &control);
print bcm_cosq_mod_control_get(unit, &control);

/* Create mirror-on-drop profile. */
int profile_id, profile_id1;
int dest_port1 = 1;
bcm_gport_t gport;
uint8 dest_mac[6] = {0x00, 0x11, 0x12, 0x13, 0x14, 0x15};
uint8 src_mac[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
bcm_cosq_mod_profile_t profile, profile1;
bcm_mirror_source_t source, source2;

print bcm_cosq_mod_profile_create(unit, 0, &profile_id);

bcm_cosq_mod_profile_t_init(&profile);
profile.percent_0_25 = 800;
profile.percent_25_50 = 400;
profile.percent_50_75 = 200;
profile.percent_75_100 = 100;
profile.enable = 1;
print bcm_cosq_mod_profile_set(unit, profile_id, &profile);
print bcm_cosq_mod_profile_get(unit, profile_id, &profile);

bcm_mirror_destination_t mtp;
bcm_mirror_destination_t_init(&mtp);
BCM_GPORT_MODPORT_SET(gport, 0, dest_port1);
mtp.gport=gport;
mtp.flags2=BCM_MIRROR_DEST_FLAGS2_MOD_TUNNEL_PSAMP;
sal_memcpy(mtp.dst_mac, dest_mac, 6);
sal_memcpy(mtp.src_mac, src_mac, 6);
mtp.tpid=0x8100;
mtp.vlan_id=5;
mtp.version=4;
mtp.src_addr=0x021010a0;
mtp.dst_addr=0x111010a0;
mtp.ttl=2;
mtp.tos=2;
mtp.df=1;
mtp.cosq = 0; /*Only MC queue can be configured as MOD dest queue*/
print bcm_mirror_destination_create(unit,&mtp);

bcm_mirror_source_t_init(&source);
source.type = bcmMirrorSourceTypeCosqMod;
source.mod_profile_id = profile_id;
print bcm_mirror_source_dest_add(unit, &source, mtp.mirror_dest_id);

/* Add second destination, use the same MOD dest id */
print bcm_cosq_mod_profile_create(unit, 0, &profile_id1);

bcm_cosq_mod_profile_t_init(&profile1);
profile1.percent_0_25 = 400;
profile1.percent_25_50 = 200;
profile1.percent_50_75 = 100;
profile1.percent_75_100 = 50;
profile1.enable = 1;
print bcm_cosq_mod_profile_set(unit, profile_id1, &profile1);
print bcm_cosq_mod_profile_get(unit, profile_id1, &profile1);

bcm_mirror_source_t_init(&source2);
source2.type = bcmMirrorSourceTypeCosqMod;
source2.mod_profile_id = profile_id1;
print bcm_mirror_source_dest_add(unit, &source2, mtp.mirror_dest_id);


bcm_cosq_object_id_t id;
uint64 value1, value2, value3, value4, value5;

id.buffer = 0;
id.port = mtp.gport;
id.cosq = mtp.cosq;
print bcm_cosq_mod_stat_get(unit, &id, bcmCosqModStatDestQueueBytesCurrent, &value5);
print bcm_mirror_source_dest_delete(unit, &source, mtp.mirror_dest_id);
print bcm_mirror_source_dest_delete(unit, &source2, mtp.mirror_dest_id);



