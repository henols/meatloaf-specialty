#!/usr/bin/env python3
import sys
import configparser

def update_configuration():
    config = configparser.ConfigParser()
    config.read('platformio.ini.sample')

    for arg in sys.argv[1:]:
        name, value = arg.split("=")
        print(f"Updating meatloaf.{name} to {value}")
        config['meatloaf'][name] = value

    with open('platformio.ini', 'w') as configfile:
        config.write(configfile)

if __name__ == "__main__":
    update_configuration()