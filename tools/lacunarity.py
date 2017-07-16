#python3

import numpy
from tools.sparsevox import Voxel


class Bounds:
    def __init__(self, **kwargs):
        self.i0 = kwargs.get("i0")
        self.i1 = kwargs.get("i1")
        self.j0 = kwargs.get("j0")
        self.j1 = kwargs.get("j1")
        self.k0 = kwargs.get("k0")
        self.k1 = kwargs.get("k1")

    @property
    def array_shape(self):
        return (self.j1 - self.j0 + 1, self.i1 - self.i0 + 1)

    def is_in_k_range(self, k):
        return (self.k0 is None or k >= self.k0) and (self.k1 is None or k <= self.k1)


def create_layer(bounds, voxel_layers):
    """
    Create a binary numpy array from the voxel layer dictionary supplied
    :type bounds: Bounds
    :param bounds: Bounds object specifying the region to use
    :type voxel_layers: dict
    :param voxel_layers: a dictionary where the key is the layer index and the value is a list of voxel named tuples
    that are already within the i/j bounds
    :return: binary numpy array of occupied voxels
    """

    layer = numpy.zeros(shape=bounds.array_shape)
    for k, voxels in voxel_layers:
        if bounds.is_in_k_range(k):
            for voxel in voxels:
                layer[voxel.j-bounds.j0, voxel.i-bounds.i0] = 1
    return layer