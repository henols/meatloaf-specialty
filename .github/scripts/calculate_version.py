#!/usr/bin/env python3
import datetime
import re
import subprocess
import os
import json
from datetime import timezone 
import sys


def get_header_version() :
    header_file = "include/version.h"


    rxs = ['^#define FN_VERSION_MAJOR (\w+)', '^#define FN_VERSION_MINOR (\w+)',
            '^(#define FN_VERSION_BUILD)', '^(#define FN_VERSION_DATE)', '^(#define FN_VERSION_FULL)', 
            '^(#define FW_VERSION)']

    ver_maj = ""
    ver_min = ""

    txt = [line for line in open(header_file)]


    for line in txt:

        for i in range(len(rxs)):
            m = re.match(rxs[i], line)
            if m is not None:
                break

        if m is not None:
            if i == 0:
                ver_maj = m.groups(0)[0]
            elif i == 1:
                ver_min = m.groups(0)[0]
    
    return (ver_maj, ver_min)
  
def calculate_version():

    major, minor, patch = sys.argv[1].split(".")

    ver_maj, ver_min = get_header_version()

    if int(ver_maj) > int(major):
        major = ver_maj
        minor = 0
        patch = 0
    elif int(ver_min) > int(minor):
        major = ver_maj
        minor = ver_min
        patch = 0
    else:
        patch = int(patch)+1

    date_time = datetime.datetime.now(timezone.utc)

    version_date = date_time.strftime("%Y-%m-%d %H:%M:%S")
    fw_date = date_time.now().strftime("%Y%m%d.%H")

    print(f"New versin created: {major}.{minor}.{patch}")
    print(f"FW version: {fw_date}")
    print(f"Date: {version_date}")
    with open(os.environ['GITHUB_OUTPUT'], 'a') as fh:
        print(f'fw_version={fw_date}', file=fh)
        print(f'date={version_date}', file=fh)
        print(f'major={major}', file=fh)
        print(f'minor={minor}', file=fh)
        print(f'patch={patch}', file=fh)

if __name__ == "__main__":
    calculate_version()