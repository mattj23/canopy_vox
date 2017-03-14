#python3

"""
Plot the summary file in the simplest way possible
"""

import numpy
import argparse
import matplotlib.pyplot as plt

try:
    from sparsevox import SparseVoxels
except:
    from tools.sparsevox import SparseVoxels


def main():
    parser = argparse.ArgumentParser(description="Plots the top-down view of the voxel densities for a summary file")
    parser.add_argument("file", help="The .sparsevox file to process")
    parser.add_argument("output", help="The name of the png to output")
    args = parser.parse_args()

    voxels = SparseVoxels(file=args.file)

    i_values = set()
    j_values = set()

    for voxel in voxels:
        i_values.add(voxel.i)
        j_values.add(voxel.j)

    i0 = min(i_values)
    j0 = min(j_values)
    imax = max(i_values)
    jmax = max(j_values)

    print("i0, j0 = {}, {}".format(i0, j0))
    image = numpy.zeros(shape=(imax-i0+1, jmax-j0+1))

    for voxel in voxels:
        image[voxel.i-i0, voxel.j-j0] += voxel.v

    plt.imshow(image, vmin=0, vmax=image.mean() * 2, cmap="gray")
    plt.savefig(args.output)

if __name__ == '__main__':
    main()