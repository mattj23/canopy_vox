"""
    Sparse voxel file representation

    A .sparsevox file is an ascii file that contains a sparse representation of a voxel set with a little bit of
    metadata information, the most relevant being the voxel spacing (length, width, and height of the voxel
    elements)

    Sample file contents:

        [info]
        spacing=0.1
        thinning=0.0013
        binning=0
        [voxels]
        -101,260,4,1
        58,-298,99,1
        58,-300,99,1
        58,-299,99,3
        57,-300,99,5
        70,-300,98,1
        71,-299,98,1
        70,-299,97,1
        70,-298,95,1
        70,-300,97,1
        71,-299,97,1
        71,-298,97,1
        66,-298,98,1
        66,-300,97,1
        65,-299,98,1


"""
import collections

# The Voxel tuple contains the i, j, k address of the voxel, plus the value (or intensity) of the voxel
Voxel = collections.namedtuple("Voxel", "i j k v")


class SparseVoxels:
    def __init__(self, **kwargs):
        self.voxels = []
        self.spacing = 1
        self.i_min = None
        self.j_min = None
        self.i_max = None
        self.j_max = None
        file_name = kwargs.get("file", None)

        if file_name is not None:
            self.load_from_file(file_name)

    def load_from_file(self, file_name):
        with open(file_name, "r") as handle:
            for line in handle:
                if line.strip().startswith("spacing="):
                    self.spacing = float(line.split("spacing=")[-1])

                parts = line.strip().split(",")
                if len(parts) == 4:
                    try:
                        values = [int(x) for x in parts]
                        vox = Voxel(*values)
                        self.voxels.append(vox)
                        if self.i_min is None or vox.i < self.i_min:
                            self.i_min = vox.i
                        if self.i_max is None or vox.i > self.i_max:
                            self.i_max = vox.i
                        if self.j_min is None or vox.j < self.j_min:
                            self.j_min = vox.j
                        if self.j_max is None or vox.j > self.j_max:
                            self.j_max = vox.j

                    except:
                        pass

    def __iter__(self):
        """

        :rtype: Voxel
        :return:
        """
        for x in self.voxels:
            yield x

    def __len__(self):
        return len(self.voxels)

    def __repr__(self):
        return "<SparseVoxels - n={}>".format(len(self.voxels))
