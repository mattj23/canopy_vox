#python3

"""
Plot the summary file in the simplest way possible
"""

import numpy
import argparse
import matplotlib.pyplot as plt

def main():
    parser = argparse.ArgumentParser(description="Plots the top-down view of the voxel densities for a summary file")
    parser.add_argument("file", help="The .summary file to process")
    args = parser.parse_args()

    columns = {}
    i_values = []
    j_values = []

    with open(args.file, "r") as handle:
        for line in handle:
            pieces = line.split(",")
            i, j, c = map(int, pieces[:3])
            columns[(i, j)] = c
            i_values.append(i)
            j_values.append(j)

    i0 = min(i_values)
    j0 = min(j_values)
    imax = max(i_values)
    jmax = max(j_values)

    print(imax - i0, i0)
    print(jmax - j0, j0)

    image = numpy.zeros(shape=(imax-i0+1, jmax-j0+1))

    for k, v in columns.items():
        i, j = k
        image[i-i0, j-j0] = v

    plt.imshow(image, vmin=0, vmax=500, cmap="gray")
    plt.show()

if __name__ == '__main__':
    main()