#python3

help_string = """
Configuration script for the parallel MPI algorithm, this script helps generate
the configuration json file.

usage:
    ./config.py [json argument] [folder argument] > output_file.json

    json argument: a configuration json file to alter with the .pts files
                   specified in folder argument

    folder argument: a folder path that contains the pts files to add to the
                     configuration
"""

import os
import sys
import json
import re

def get_folder_argument():
    folders = [x for x in sys.argv if os.path.isdir(x)]
    if not folders:
        print("config.py: you need to specify a valid directory with point files")
        return None

    input_files = [os.path.join(folders[0], x) for x in os.listdir(folders[0]) if x.endswith(".pts")]
    return input_files

def get_json_argument():
    json_files = [x for x in sys.argv if x.endswith(".json")]
    if not json_files:
        print("config.py: you need to specify a .json file for the parallel algorithm configuration")
        return None
    return json_files[0]

def parse_json(json_target):
    if os.path.exists(json_target):
        with open(json_target, "r") as handle:
            file_contents = handle.read()
            input_string = re.sub(r"\\\n", "", file_contents)
            input_string = re.sub(r"//.*\n", "", input_string)

            return json.loads(input_string)
    else:
        print("config.py: the json configuration file doesn't exist")
        return None

def main():
    if len(sys.argv) < 2:
        print(help_string)

    json_target = get_json_argument()
    if json_target is None:
        return

    json_data = parse_json(json_target)
    if json_data is None:
        return

    input_files = get_folder_argument()
    if not input_files:
        print("config.py: no point files in directory")
        return

    json_data["input_files"] = input_files

    print(json.dumps(json_data, indent=4))
    

if __name__ == '__main__':
    main()
