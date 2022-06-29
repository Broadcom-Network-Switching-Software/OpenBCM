# Install requests and joblib python packages if missing
# On Unix:
#    sudo pip install requests
#    sudo pip install joblib
#    sudo pip install pyyaml
# On Windows:
#    pip install requests
#    pip install joblib
#    pip install pyyaml
#
# This script demonstrates how to authenticate access to portal and then
# calls API GET /api/csg/csg using bearer token, which results in a license
# file download

import os, sys, getopt
import requests
import json,yaml
from datetime import datetime
import time

long_opt=['help','apikey=','lic=','devidfile=','devid=']

def represent_unicode(dumper, data):
    return dumper.represent_scalar('tag:yaml.org,2002:str', data)

yaml.add_representer(str, represent_unicode)

def authorize(session, apiKey, api_base):
  # gets the token using your apiKey
  # replace apiKey value below using your 32 digit API Key from your
  # user account's API key at CSG portal site
  resp = session.post(api_base + 'Token?apiKey=' + apiKey)
  if not resp.ok:
      print((resp.text))
      sys.exit()

  token = json.loads(resp.text)
  return 'Bearer ' + token['token']

def embellish(text,char='-'):
    """ Embellish a string within 80 column width with -- """
    r = (text.__len__() % 2 != 0) and 1 or 0
    l = (80-text.__len__())/2
    r += l
    print("".join([char for i in range(l)])+text+\
          "".join([char for i in range(r)]))

def showUsage(cmd,detail=False):
    embellish('USAGE')
    print(cmd)
    for opt in long_opt:
        if opt == 'help': print('\t[-h or --'+opt+']')
        if opt == 'apikey=': print('\t[-a or --'+opt+'<api key>]')
        if opt == 'lic=': print('\t[-l or --'+opt+'<license id>]')
        if opt == 'devidfile=': print('\t[-f or --'+opt+'<devid list file>]')
        if opt == 'devid=': print('\t[-d or --'+opt+'<devid>]')
    if detail :
        embellish('OPTION-LIST')
        for opt in long_opt:
            if opt == 'help':
                print("  %-28s: Prints the usage" % ('-h or --'+opt,))
            if opt == 'apikey=':
                print("  %-28s: User API Key" % ('-a or --'+opt+'<api key>',))
            if opt == 'lic=':
                print("  %-28s: User License Id" % ('-l or --'+opt+'<license id>',))
            if opt == 'devidfile=':
                print("  %-28s: Device Id list in yaml" % ('-f or --'+opt+'<devid list file>',))
            if opt == 'devid=':
                print("  %-28s: Device Id" % ('-d or --'+opt+'<devid>',))

if __name__ == "__main__" :
  try:
      opts, args = getopt.getopt(sys.argv[1:], "ha:l:f:d:", long_opt)
  except getopt.GetoptError as err:
      print(str(err))
      sys.exit(2)

  lic=[]
  devlist={}
  devidfile='devicelist.yml'
  apiKey=None
  for o, a in opts:
      if o in ('-h', '--help'):
          showUsage(sys.argv[0],detail=True)
          sys.exit(0)
      if o in ('-a', '--apikey'): apiKey=a
      if o in ('-l', '--lic'): lic.append(a)
      if o in ('-f', '--devidfile'):
        devidfile=a
        with open(a,'r') as f: devlist=yaml.load(f,yaml.BaseLoader)
      if o in ('-d', '--devid'): devlist[a]=''

  if not (devlist and lic and apiKey) :
      print ("Insufficient parameters...")
      showUsage(sys.argv[0])
      sys.exit(0)

  api_base = 'https://bfl.broadcom.com/api/v1/'
  session = requests.session()

  print(str(datetime.now()) + " Script starting...\n")

  # call your GET /api/csg/csg API using bearer_token below
  provision=False
  auth=False
  for otpId in devlist:
    print(("%s Check provisioning for %s"%(str(datetime.now()),otpId)))

    if len(devlist[otpId]) > 0:
      print("\t...skipping...")
      continue
    else:
      opts={'lic': lic, 'otpId': otpId.upper()}

      if not auth:
        bearer_token = authorize(session,apiKey,api_base)
        head = {'Content-Encoding': 'gzip', 'Authorization': bearer_token}

      resp=session.get(api_base + 'csg/ccf',params=opts, headers=head, stream=True)
      if not resp.ok:
        if resp.reason == 'Unauthorized' :
          bearer_token = authorize(session,apiKey,api_base)
          head = {'Content-Encoding': 'gzip', 'Authorization': bearer_token}
          resp=session.get(api_base + 'csg/ccf',params=opts, headers=head, stream=True)
          if not resp.ok:
            print("Provision Error:" + resp.reason)
            print("Completing license retreival for provisioned devices...")
            break
        else:
          print("Provision Error:" + resp.reason)
          print("Completing license retreival for provisioned devices...")
          break
      else:
        auth=True
        devlist[otpId]=int(resp.text)
        provision=True
        time.sleep(1)

  if not provision:
    print("Nothing to provision, exiting...")
    sys.exit()

  status=False
  print('\n' + str(datetime.now()) + " provisioning request completed.")
  retries=0
  sleeptime=60
  while not status:
    if retries < 3:
      time.sleep(sleeptime)
      if sleeptime < 300: sleeptime*=1.2
      retries+=1
      print(("Download License... attempt: %d"%retries))
    else: break
    bearer_token = authorize(session,apiKey,api_base)
    head = {'Content-Encoding': 'gzip', 'Authorization': bearer_token}
    for otpId in devlist:
      if type(devlist[otpId]) is not int: continue
      opts={'provisionId': str(devlist[otpId])}
      resp=session.get(api_base + 'csg/BinFile',params=opts, headers=head, stream=True)
      status=resp.ok
      print(("%s Retrieving license file for provision id %s"%(str(datetime.now()),opts['provisionId'])))

      if not status:
        print("Error:" + resp.reason)
        break
      for k in resp.headers['Content-Disposition'].split(';'):
        f=k.split('=')
        if (len(f) > 1) and f[0].strip()=='filename':
          fileName=f[1].strip()[1:-1].replace(' ','_')
          print(fileName)
          break

      with open(fileName, 'wb') as f:
        for chunk in resp.iter_content(chunk_size=1024):
          if chunk: # filter out keep-alive new chunks
            f.write(chunk)
        print(str(datetime.now()) + " Succeeded: Downloaded " + fileName)
      devlist[otpId]=fileName
      retries=0
      if otpId == list(devlist.keys())[-1]:
        status=True
        break

  with open(devidfile,'wb') as f: f.write(yaml.dump(devlist, encoding='utf-8', allow_unicode=True, default_flow_style=False, explicit_start=True))
  print('\n' + str(datetime.now()) + " Script completed.")
