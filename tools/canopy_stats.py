#python3

"""
    Compute the canopy statistics for the given file given the bounding limits

    Statistics:
        Canopy gap fraction (number of occupied columns vs number of clear ones)

"""
import os
import numpy
import argparse

from plotly.offline import download_plotlyjs, init_notebook_mode, plot
import plotly.graph_objs as go

import sys

from tools.sparsevox import SparseVoxels
from tools.lacunarity import Bounds, create_layer


def validate_bounds(bound_text):
    try:
        parts = bound_text.split(",")
        elements = [float(p) for p in parts]
        return elements[0], elements[1]
    except:
        return None


def quit_on_error():
    print("Exiting due to error")
    sys.exit()


def main():
    print("\nCanopy structural metrics processor")
    print("===========================================")
    print("  usage example: ")
    print("    $ python3 canopy_stats.py -x=-15,85 -y=21,31 path_to_input_file.sparsevox")
    print("")
    parser = argparse.ArgumentParser(description="Compute the canopy structural metrics for the given sparse voxel file")
    parser.add_argument("file", help="The .sparsevox file to process")
    parser.add_argument("-x", action="store", dest="x", help="The site's x bounds")
    parser.add_argument("-y", action="store", dest="y", help="The site's y bounds")
    args = parser.parse_args()

    # Bounding region validation
    x_bounds = validate_bounds(args.x)
    y_bounds = validate_bounds(args.y)
    bounds_validated = True
    if x_bounds is None:
        print("Error: incorrect format for x bounds '{}' (should be '-x=-20,10')".format(args.x))
        bounds_validated = False
    if y_bounds is None:
        print("Error: incorrect format for y bounds '{}' (should be '-y=-20,10')".format(args.y))
        bounds_validated = False

    if not bounds_validated:
        quit_on_error()

    x0 = min(x_bounds)
    x1 = max(x_bounds)
    y0 = min(y_bounds)
    y1 = max(y_bounds)

    print("Processing file {}".format(args.file))
    print(" -> X bounding = {:.2f}m to {:.2f}m".format(x0, x1))
    print(" -> Y bounding = {:.2f}m to {:.2f}m".format(y0, y1))
    print(" -> Loading voxels (this might take a while)")
    voxels = SparseVoxels(file=args.file)
    print(" -> Done loading")

    print("\nFile Summary")
    print(" -> Total nonzero cells in cloud = {}".format(len(voxels)))
    print(" -> Each cell represents a {:.2f}m cube".format(voxels.spacing))
    print(" -> Cloud i-index range = {} to {}".format(voxels.i_min, voxels.i_max))
    print(" -> Cloud j-index range = {} to {}".format(voxels.j_min, voxels.j_max))

    i0 = int(x0 / voxels.spacing)
    i1 = int(x1 / voxels.spacing)
    j0 = int(y0 / voxels.spacing)
    j1 = int(y1 / voxels.spacing)
    total_columns = (i1 - i0) * (j1 - j0)

    print(" -> I bounding = {} to {}".format(i0, i1))
    print(" -> J bounding = {} to {}".format(j0, j1))
    print(" -> Total columns in bounding region = {}".format(total_columns))

    print("\nIndexing cloud in bounding region...")

    # To index the voxel cloud in the bounded region we'll go through each voxel and if it's in the bounding region
    # we'll both add it to a column dictionary where the key is the i,j address of the voxel, and a layer dictionary
    # where the key is the z index of the voxel.  These will let us search quickly through the voxels later.
    count = 0
    voxel_columns = {}
    voxel_layers = {}
    image = numpy.zeros(shape=(j1 - j0 + 1, i1 - i0 + 1))

    for voxel in voxels:
        if i0 <= voxel.i <= i1 and j0 <= voxel.j <= j1:
            count += 1

            # Place this voxel in its column index
            column_key = (voxel.i, voxel.j)
            if column_key in voxel_columns:
                voxel_columns[column_key].append(voxel)
            else:
                voxel_columns[column_key] = [voxel]

            # Place this voxel in its layer index
            layer_key = voxel.k
            if layer_key in voxel_layers:
                voxel_layers[layer_key].append(voxel)
            else:
                voxel_layers[layer_key] = [voxel]

    print(" -> Total non-zero voxels in the bounds = {}".format(count))
    print(" -> Columns with non-zero values = {}".format(len(voxel_columns)))
    print(" -> Layers with non-zero values = {}".format(len(voxel_layers)))

    # Calculate gap fraction
    gap_fraction = 1 - (len(voxel_columns) / float(total_columns))

    # Calculate density per layer and z-layer statistics
    layer_densities = []
    layer_stats = []
    for z, cells in voxel_layers.items():
        layer_densities.append((z, len(cells) / float(total_columns)))
        a = numpy.array([v.v for v in cells])
        layer_stats.append((z, a.sum(), a.mean(), a.std()))

    layer_densities.sort()
    layer_stats.sort()
    with open('z-layer-density.csv', "w") as handle:
        handle.write("\n".join(["z layer index (k), z height, layer density"] +
                               ["{}, {:.3f} m, {:.6f}".format(z, z * voxels.spacing, density) for z, density in layer_densities]))

    stat_output = ["z layer index (k), z height, total points, mean points, sdev points"]
    for z, z_sum, z_mean, z_std in layer_stats:
        stat_output.append("{}, {:.3f} m, {}, {:.6f}, {:.6f}".format(z, z * voxels.spacing, z_sum, z_mean, z_std))
    with open('z-layer-stats.csv', 'w') as handle:
        handle.write("\n".join(stat_output))

    # Calculate lacunarity
    bounds = Bounds(i0=i0, i1=i1, j0=j0, j1=j1)
    layer = create_layer(bounds, voxel_layers)
    

    # Print canopy statistics
    print("\nCanopy structural metrics")
    print(" -> Gap fraction = {}".format(gap_fraction))
    print(" -> Density by z layer saved to 'z-layer-density.csv'")
    print(" -> Stats by z layer saved to 'z-layer-stats.csv'")


if __name__ == '__main__':
    main()