#
# \file patch_id_gen.py
#
# Generating new patch ID.
#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
import sys
import os
import os.path
import time
import getopt
import hashlib    # support md5 hashing

def usage():
    print ('python patch_id_gen.py -u <user_id>  -c <component> | -h')
    print ('Where <user_id> is the user ID of the person that responsible for the patch')
    print ('      <component> is the component that owns the patch\n')

def main(argv):
    user_id = None
    component = None
    patch_id = None

    # Check if there are any input parameters
    try:
        opts, args = getopt.getopt(argv, "hu:c:", ["help"])
    except getopt.GetoptError:
        usage()
        sys.exit(-1)

    for opt, arg in opts:
        if opt in ("-h", "--help"):
            usage()
            sys.exit(0);
        elif opt == '-u':
            user_id = arg
        elif opt == '-c':
            component = arg
        else:
            usage()
            sys.exit(-1)

    # Verify that all the inputs are available
    if (not user_id or not component):
        usage()
        sys.exit(-1)

    m = hashlib.md5()
    m.update(str(time.gmtime()))
    m.update(user_id + component)
    hex_val = m.hexdigest()[:16]
    val = int(hex_val[:8],16) ^ int(hex_val[8:16],16)
    print ('object ID = %s' % hex(val))


if __name__ == "__main__":
    if sys.version_info[0] < 2 or \
       (sys.version_info[0] == 2 and sys.version_info[1] < 7):
        print('Error: Unsupported python version, min version 2.7.x')
    else:
        main(sys.argv[1:])
