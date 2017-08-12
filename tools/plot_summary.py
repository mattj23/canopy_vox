#python3

"""
Plot the summary file in the simplest way possible
"""

import os
import numpy
import argparse
from plotly.offline import download_plotlyjs, init_notebook_mode, plot
import plotly.graph_objs as go

try:
    from sparsevox import SparseVoxels
except:
    from tools.sparsevox import SparseVoxels


def load_plot_file(file_name):
    points = []
    with open(file_name, "r") as handle:
        for line in handle:
            try:
                name, x, y, *_ = line.split()
                x = float(x)
                y = float(y)
                points.append([x, y])
            except:
                continue
    return zip(*points)


def validate_bounds(bound_text):
    try:
        parts = bound_text.split(",")
        elements = [float(p) for p in parts]
        return elements[0], elements[1]
    except:
        return None


def main():
    parser = argparse.ArgumentParser(description="Plots the top-down view of the voxel densities for a summary file")
    parser.add_argument("file", help="The .sparsevox file to process")
    parser.add_argument("output", help="The name of the png to output")
    parser.add_argument("--gages", action="store", dest="gages", help="A file to plot green circles at")
    parser.add_argument("-x", action="store", dest="x", help="The site's x bounds")
    parser.add_argument("-y", action="store", dest="y", help="The site's y bounds")

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

    # Build the image array
    image = numpy.zeros(shape=(j_max-j_0+1, i_max-i_0+1))
    for voxel in voxels:
        image[voxel.j-j_0, voxel.i-i_0] += voxel.v
    image = image.clip(0, 5000)

    x_axis = [(i_0 + i) * voxels.spacing for i in range(i_max - i_0)]
    y_axis = [(j_0 + i) * voxels.spacing for i in range(j_max - j_0)]

    # Plot the data
    color_scale = [ [0, 'rgb(255,255,255)'], [0.5, 'rgb(50, 50, 50)'], [1.0, 'rgb(0,0,0)'] ]
    heat_trace = go.Heatmap(z=image, x=x_axis, y=y_axis, colorscale=color_scale)

    # Check if we want to plot gages
    if args.gages is not None and os.path.exists(args.gages):
        gage_y, gage_x = load_plot_file(args.gages)
        gage_trace = go.Scatter(x=gage_x, y=gage_y, mode="markers")
        plot_data = [heat_trace, gage_trace]
    else:
        plot_data = [heat_trace]

    # Check if we want to plot bounds
    x_bounds = validate_bounds(args.x)
    y_bounds = validate_bounds(args.y)
    if x_bounds and y_bounds:
        site_boundary = {"type": "rect", "x0": x_bounds[0], "x1": x_bounds[1], "y0": y_bounds[0], "y1": y_bounds[1], "line":{"color": "rgba(128, 0, 128, 1)"}}
        shapes = [site_boundary]
    else:
        shapes = []
    layout = go.Layout(title="Voxel Field", xaxis={"showgrid": True}, yaxis={"showgrid": True}, shapes=shapes)
    plot(go.Figure(data=plot_data, layout=layout), args.output)
    #
    # # Plot any files
    # if os.path.exists(args.plot_green):
    #     green_x, green_y = load_plot_file(args.plot_green)
    #     g_j = [(x / voxels.spacing) + j_0 for x in green_x]
    #     g_i = [(y / voxels.spacing) + i_0 for y in green_y]
    #     plt.scatter(x=g_i, y=g_j, c="g", s=40)


if __name__ == '__main__':
    main()