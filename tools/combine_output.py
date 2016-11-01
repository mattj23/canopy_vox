#python3

"""

This is a temporary tool to combine all of the .sparsevox files in a directory into a single file.  Could probably
be currently done with some clever bash-fu, but may eventually need to change the file format.

"""

import argparse
import json
import os

description = """This tool takes the output of the parallel voxelization and thinning and combines it into a single
sparse representation output file."""


def process_file(file_name, output_handle):
    with open(file_name, "r") as input_handle:
        for line in input_handle:
            output_handle.write(line)

def main():
    # Parse the command line arguments
    parser = argparse.ArgumentParser(description=description)
    parser.add_argument("config", help="Configuration JSON file for the run that we're aggregating" )
    parser.add_argument("--output", "-o", action="store", dest="output", help="File to output the aggregated data to")
    args = parser.parse_args()

    # Load the configuration file to figure out where the output directory is
    try:
        with open(args.config, "r") as handle:
            cleaned = "".join([x for x in handle if not x.strip().startswith("//")])
            config_data = json.loads(cleaned)
    except ValueError:
        print("The configuration file could not be parsed by the json deserializer")
        return
    except IOError:
        print("Could not read the file {}".format(args.config))
        return

    # Discover the contents of the output directory
    try:
        working_path = config_data['output_directory']
    except KeyError:
        print("Could not extract the 'output_directory' field from the config file")
        return

    all_files = [os.path.join(working_path, x) for x in os.listdir(working_path) if x.endswith(".sparsevox")]

    output_path = "combined.sparsevox" if args.output is None else args.output
    if os.path.exists(output_path):
        os.remove(output_path)

    with open(output_path, "a") as handle:
        for input_file in all_files:
            print("processing {}".format(input_file))
            process_file(input_file, handle)

    print("done processing, all data written to {}".format(output_path))


if __name__ == '__main__':
    main()
