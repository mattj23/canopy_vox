#python3

"""
    Compute the gap fraction for the given voxel file
"""

import numpy
import argparse
import matplotlib.pyplot as plt

try:
    from sparsevox import SparseVoxels
except:
    from tools.sparsevox import SparseVoxels


def validate_bounds(bound_text):
    try:
        parts = bound_text.split(",")
        elements = [float(p) for p in parts]
        return elements[0], elements[1]
    except:
        return None

def main():
    parser = argparse.ArgumentParser(description="Compute the gap fraction for the given sparse voxel file")
    parser.add_argument("file", help="The .sparsevox file to process")
    parser.add_argument("-x", action="store", dest="x", help="The site's x bounds")
    parser.add_argument("-y", action="store", dest="y", help="The site's y bounds")
    args = parser.parse_args()

    # voxels = SparseVoxels(file=args.file)
    x_bounds = validate_bounds(args.x)
    y_bounds = validate_bounds(args.y)

    if x_bounds is None:
        print("Incorrect format for x bounds '{}' (should be '-x=-20,10')".format(args.x))
    if y_bounds is None:
        print("Incorrect format for y bounds '{}' (should be '-y=-20,10')".format(args.y))

    x0 = min(x_bounds)
    x1 = max(x_bounds)
    y0 = min(y_bounds)
    y1 = max(y_bounds)

    print("Processing file {}".format(args.file))
    print("X bounding = {:.2f}m to {:.2f}m".format(x0, x1))
    print("Y bounding = {:.2f}m to {:.2f}m".format(y0, y1))

    voxels = SparseVoxels(file=args.file)

    print("There are {} total nonzero cells in the voxel cloud".format(len(voxels)))
    print("Each cell represents a {:.2f}m cube".format(voxels.spacing))

    i0 = int(x0 / voxels.spacing)
    i1 = int(x1 / voxels.spacing)
    j0 = int(y0 / voxels.spacing)
    j1 = int(y1 / voxels.spacing)
    total_columns = (i1 - i0) * (j1 - j0)

    print("I bounding = {} to {}".format(i0, i1))
    print("J bounding = {} to {}".format(j0, j1))
    print("Total columns = {}".format(total_columns))

    count = 0
    columns = {}
    # Construct the columns
    for voxel in voxels:
        if voxel.i >= i0 and voxel.i <= i1 and voxel.j >= j0 and voxel.j <= j1:
            key = (voxel.i, voxel.j)
            if key in columns:
                columns[key].append(voxel.v)
            else:
                columns[key] = [voxel.v]

    column_means = []
    for k, v in columns.items():
        a = numpy.array(v)
        column_means.append(a.mean())
    cmean = numpy.array(column_means).mean()
    print("Column mean value = {}".format(cmean))


    print("There are {} total voxels within the bounds".format(count))
    print("Total non-empty columns = {}".format(len(column_means)))
    print("Gap fraction = {}".format(1-(len(column_means) / float(total_columns))))


if __name__ == '__main__':
    main()