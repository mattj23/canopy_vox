#python3

"""
Configuration script for the parallel MPI algorithm, this script helps generate
the configuration json file.
"""

import os
import sys
import json
import argparse

def get_json_argument():
    json_files = [x for x in sys.argv if x.endswith(".json")]
    if not json_files:
        print("config.py: you need to specify a .json file for the parallel algorithm configuration")
        return None
    return json_files[0]

def parse_json(json_target):
    if os.path.exists(json_target):

        pass
    else:
        print("config.py: the json configuration file doesn't exist")

def main():


    json_target = get_json_argument()
    if json_target is None:
        return


    pass


if __name__ == '__main__':
    main()
