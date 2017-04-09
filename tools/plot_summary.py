#python3

"""
Plot the summary file in the simplest way possible
"""

import os
import numpy
import argparse
import matplotlib.pyplot as plt

try:
    from sparsevox import SparseVoxels
except:
    from tools.sparsevox import SparseVoxels


def load_plot_file(file_name):
    points = []
    with open(file_name, "r") as handle:
        for line in handle:
            try:
                x, y, *_ = line.split()
                x = float(x)
                y = float(y)
                points.append([x, y])
            except:
                continue
    return zip(*points)

def main():
    parser = argparse.ArgumentParser(description="Plots the top-down view of the voxel densities for a summary file")
    parser.add_argument("file", help="The .sparsevox file to process")
    parser.add_argument("output", help="The name of the png to output")
    parser.add_argument("--plot-green", action="store", dest="plot_green", help="A file to plot green circles at")
    args = parser.parse_args()

    # Load the voxels from the file
    print("Loading voxels from {}".format(args.file))
    voxels = SparseVoxels(file=args.file)
    print("Done")

    # Determine the entire collection of all i and j values
    i_values = set()
    j_values = set()

    for voxel in voxels:
        i_values.add(voxel.i)
        j_values.add(voxel.j)

    # The maximum and minimum values of i and j will determine what the  bounding space of the voxel field is,
    # as well as the offset values i_0 and j_0 which will be necessary to transform from voxel space into image
    # space (by taking a coordinate in voxel space and subtracting (i_0, j_0))
    i_0 = min(i_values)
    j_0 = min(j_values)
    i_max = max(i_values)
    j_max = max(j_values)

    print("Voxel cloud summary")
    print("i_0, i_max = {}, {} ({:.2f}m to {:.2f}m)".format(i_0, i_max, i_0 * voxels.spacing, i_max * voxels.spacing))
    print("j_0, j_max = {}, {} ({:.2f}m to {:.2f}m)".format(j_0, j_max, j_0 * voxels.spacing, j_max * voxels.spacing))

    image = numpy.zeros(shape=(i_max-i_0+1, j_max-j_0+1))

    for voxel in voxels:
        image[voxel.i-i_0, voxel.j-j_0] += voxel.v

    intensity_max = (image.max() * 0.5)
    plt.imshow(image, vmin=0, vmax=intensity_max, cmap="gray")

    # Plot any files
    if os.path.exists(args.plot_green):
        green_x, green_y = load_plot_file(args.plot_green)
        g_j = [(x / voxels.spacing) + j_0 for x in green_x]
        g_i = [(y / voxels.spacing) + i_0 for y in green_y]
        plt.scatter(x=g_i, y=g_j, c="g", s=40)

    plt.show()
    plt.savefig(args.output)

if __name__ == '__main__':
    main()