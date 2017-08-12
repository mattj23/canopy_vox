#python3

import numpy
try:
    from sparsevox import SparseVoxels
except:
    from tools.sparsevox import SparseVoxels


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
        return self.j_span, self.i_span

    def is_in_k_range(self, k):
        return (self.k0 is None or k >= self.k0) and (self.k1 is None or k <= self.k1)

    @property
    def i_span(self):
        return self.i1 - self.i0 + 1

    @property
    def j_span(self):
        return self.j1 - self.j0 + 1

    @property
    def window_sizes(self):
        """
        Calculate the window sizes as powers of 2 until we exceed the minimum dimension of the bounds
        :return: list of valid window sizes
        """
        min_dimension = min(self.array_shape)
        sizes = []
        size = 2
        while size < min_dimension:
            sizes.append(size)
            size = size * 2
        return sizes


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
    for k, voxels in voxel_layers.items():
        if bounds.is_in_k_range(k):
            for voxel in voxels:
                layer[voxel.j-bounds.j0, voxel.i-bounds.i0] = 1
    return layer


def lacunarity(window_size, layer):
    """

    :type layer: numpy.array
    :param window_size:
    :param layer:
    :return:
    """
    S = numpy.array(range(window_size*window_size + 1))
    nSr = numpy.zeros(shape=len(S)) # Distribution of occupied sites n[S,r]
    Nr = 0   # Total number of boxes examined N[r]
    for i in range(0, layer.shape[1] - window_size + 1):
        for j in range(0, layer.shape[0] - window_size + 1):
            slice = layer[j:j+window_size, i:i+window_size]
            count = numpy.count_nonzero(slice)
            nSr[count] += 1
            Nr += 1

    QSr = nSr / Nr   # Probability distribution Q(S,r)
    Z1 = (S * QSr).sum()
    Z2 = (S**2 * QSr).sum()
    return Z2 / (Z1**2)


def lacuniarity_quick(window_size, layer):
    """
    Does not yet work
    :type layer: numpy.array
    :param window_size:
    :param layer:
    :return:
    """

    Nr = (layer.shape[1] - window_size + 1) * (layer.shape[0] - window_size + 1)
    i = 0
    sites = numpy.zeros(shape=Nr)
    for i in range(0, layer.shape[1] - window_size + 1):
        for j in range(0, layer.shape[0] - window_size + 1):
            slice = layer[j:j+window_size, i:i+window_size]
            count = numpy.count_nonzero(slice)
            sites[i] = count
            i += 1
    return sites.var() / (sites.mean()**2) + 1


def lacunarity_curve(bounds, layer):
    """
    Calculate the lacunarity curve, where window sizes are in powers of 2 up until the minimum dimension of the layer
    and the layer is a 2d binary numpy array representing the region to process
    :type layer: numpy.array
    :type bounds: Bounds
    :rtype: list
    :param bounds: Bounds object which created the layer
    :param layer: binary numpy.array representing the region to process
    :return: a list of tuples where the first element is the window size and the second element is the lacunarity
    """
    results = []
    for window_size in bounds.window_sizes:
        results.append((window_size, lacunarity(window_size, layer)))
    return results

if __name__ == '__main__':
    test = """1010000011
0110011111
1110100111
1011011110
0101110000
0010101100
0111111100
1100011101
1100000011
0100001001"""

    x = numpy.array([[int(c) for c in line] for line in test.split("\n")])
    explicit_result = lacunarity(2, x)
    quick_result = lacuniarity_quick(2, x)
    assert abs(explicit_result - 1.256421981622191) < 0.001
    assert abs(quick_result - explicit_result) < 0.001