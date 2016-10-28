# Performs a comparison between default_output and the output bin
import os

parallel_out = "/home/matt/voxel/output"

def merge_dicts(*dict_args):
    '''
    Given any number of dicts, shallow copy and merge into a new dict,
    precedence goes to key value pairs in latter dicts.
    '''
    result = {}
    for dictionary in dict_args:
        result.update(dictionary)
    return result

def read_file(filename):
    voxels = {}
    with open(filename, "r") as handle:
        for line in handle:
            chunks = line.strip().split(",")
            i, j, k = map(int, chunks[:3])
            c = float(chunks[-1])
            voxels[(i, j, k)] = c
    return voxels

def sum_of(d):
    return sum(d.values())

def main():
    expected = read_file("default_output.asc")

    files = [os.path.join(parallel_out, x) for x in os.listdir(parallel_out)]
    voxels = map(read_file, files)
    produced = merge_dicts(*voxels)

    print(sum_of(expected))
    print(sum_of(produced))

if __name__ == '__main__':
    main()
