#!/usr/bin/env python3
import re
import sys


def update_version(major, minor, patch, date, fw_date):
    """Update the version number in the header file."""
    header_file = "include/version.h"


    rxs = ['^(#define FN_VERSION_MAJOR)', 
            '^(#define FN_VERSION_MINOR)',
            '^(#define FN_VERSION_BUILD)', 
            '^(#define FN_VERSION_DATE)', 
            '^(#define FN_VERSION_FULL)', 
            '^(#define FW_VERSION)']

    txt = [line for line in open(header_file)]

    fout = open(header_file, "w")

    for line in txt:

        for i in range(len(rxs)):
            m = re.match(rxs[i], line)
            if m is not None:
                break

        if m is not None:
            if i == 0:
                line = m.groups(0)[0] + f" \"{major}\"\n"
                fout.write(line)
            elif i == 1:
                line = m.groups(0)[0] + f" \"{minor}\"\n"
                fout.write(line)
            elif i == 2:
                line = m.groups(0)[0] + f" \"{patch}\"\n"
                fout.write(line)
            elif i == 3:
                line = m.groups(0)[0] + f" \"{date}\"\n"
                fout.write(line)
            elif i == 4:
                line = m.groups(0)[0] + f" \"{major}.{minor}.{patch}\"\n"
                fout.write(line)
            elif i == 5:
                line = m.groups(0)[0] + f" \"{fw_date}\"\n"
                fout.write(line)
        else:
            fout.write(line)

    fout.close()
    print(f"New version created: {major}.{minor}.{patch}")
    print(f"Date: {date}")
    print(f"FW date: {fw_date}")

if __name__ == "__main__":
    update_version(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5])