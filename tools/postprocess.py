#python3

"""
    Does column statistic postprocessing for a .sparsevox file
"""

import argparse
import sys
import statistics

class Processor:

    def __init__(self, file_name):
        self.file_name = file_name
        self.voxels = {}
        self.columns = {}
        self.limits = {"i":None, "j":None, "k":None}
        self.output = []

    def _dimension_span(self, dim):
        return self.limits[dim][1] - self.limits[dim][0]

    @property
    def di(self):
        return self._dimension_span("i")

    @property
    def dj(self):
        return self._dimension_span("j")

    @property
    def dk(self):
        return self._dimension_span("k")

    def run(self):
        # Load the data from the file
        with open(self.file_name, "r") as handle:
            for line in handle:
                self._process_line(line)

        # Process the various columns
        for col, counts in self.columns.items():
            if len(counts) < 3:
                st = "n/a"
            else:
                st = statistics.stdev(counts)
            stats = [col[0], col[1], sum(counts), statistics.mean(counts), st]
            self.output.append(",".join([str(x) for x in stats]))

        self.output.sort()

    def _check_limit(self, key, value):
        if self.limits[key] is None:
            self.limits[key] = [value, value]
        else:
            if value < self.limits[key][0]:
                self.limits[key][0] = value
            if value > self.limits[key][1]:
                self.limits[key][1] = value

    def _process_line(self, line):
        pieces = line.strip().split(",")
        i, j, k, count = map(int, pieces)
        self._check_limit("i", i)
        self._check_limit("j", j)
        self._check_limit("k", k)
        self.voxels[(i, j, k)] = count

        if (i, j) not in self.columns:
            self.columns[(i, j)] = []
        self.columns[(i, j)].append(count)


def main():
    parser = argparse.ArgumentParser(description="Column statistic postprocessing for a .sparsevox file")
    parser.add_argument("input_file", help="Input .sparsevox file for processing")
    args = parser.parse_args()

    processor = Processor(args.input_file)
    processor.run()

    # print("Total voxel space:    {}".format(processor.di * processor.dj * processor.dk))
    # print("Total nonzero voxels: {}".format(len(processor.voxels)))

    print("\n".join(processor.output))

if __name__ == '__main__':
    main()