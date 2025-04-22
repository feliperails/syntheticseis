import sys
import segyio
import h5py

argv = sys.argv
hdf5_file_path = argv[1]
segy_file_path = argv[2]

hdf5_input = h5py.File(hdf5_file_path, 'r')
data = hdf5_input['data']

dimensions = data.shape
dim_i = dimensions[0]
dim_j = dimensions[1]

cellSizeInZ = data.attrs['cellSizeInZ'][0] * 1000
geometryOriginX = data.attrs['geometryOriginX'][0] * 100
geometryOriginY = data.attrs['geometryOriginY'][0] * 100
zBottom = data.attrs['zBottom'][0] * 1000
geometryInlineEndX = data.attrs['geometryInlineEndX'][0] * 100
geometryInlineEndY = data.attrs['geometryInlineEndY'][0] * 100
geometryCrosslineEndX = data.attrs['geometryCrosslineEndX'][0] * 100
geometryCrosslineEndY = data.attrs['geometryCrosslineEndY'][0] * 100

str_cell_size_z = str(cellSizeInZ)

cellSizeInZ = max(round(cellSizeInZ), 1)
segyio.tools.from_array3D(segy_file_path, data, dt=cellSizeInZ)
segy_output = segyio.open(segy_file_path, "r+")

header = "Processed by SyntheticSeis.".ljust(80, " ")
header += "https://github.com/feliperails/syntheticseis/".ljust(80, " ")
header += ("Original sample interval: " + str_cell_size_z).ljust(80, " ")
header += "WARNING: SEG-Y files do not support floating sample intervals.".ljust(80, " ")
header += "We recommend adjusting the sample interval manually when ".ljust(80, " ")
header += " importing this file into a visualization program,".ljust(80, " ")
header += "such as Petrel or Aspen SKUA.".ljust(80, " ")

formatted_text = segyio.tools.wrap(header)

segy_output.text[0] = formatted_text

origin_x = geometryOriginX
origin_y = geometryOriginY
origin_z = int(zBottom)
inline_end_x = geometryInlineEndX
inline_end_y = geometryInlineEndY
crossline_end_x = geometryCrosslineEndX
crossline_end_y = geometryCrosslineEndY

inline_step_x = (inline_end_x - origin_x) / dim_j
inline_step_y = (inline_end_y - origin_y) / dim_j
crossline_step_x = (crossline_end_x - origin_x) / dim_i
crossline_step_y = (crossline_end_y - origin_y) / dim_i

for j in range(dim_j):
    inline_start_x = origin_x + inline_step_x * j
    inline_start_y = origin_y + inline_step_y * j
    for i in range(dim_i):
        x = int(inline_start_x + crossline_step_x * i)
        y = int(inline_start_y + crossline_step_y * i)

        trace_no = i * dim_j + j

        segy_output.header[trace_no].update({
            segyio.TraceField.TRACE_SEQUENCE_LINE: trace_no + 1,
            segyio.TraceField.TRACE_SEQUENCE_FILE: trace_no + 1,
            segyio.TraceField.CDP: i + 1,
            segyio.TraceField.SourceSurfaceElevation: origin_z,
            segyio.TraceField.SourceDepth: origin_z,
            segyio.TraceField.GroupWaterDepth: origin_z,
            segyio.TraceField.SourceWaterDepth: origin_z,
            segyio.TraceField.SourceX: x,
            segyio.TraceField.SourceY: y,
            segyio.TraceField.GroupX: x,
            segyio.TraceField.GroupY: y,
            segyio.TraceField.TRACE_SAMPLE_INTERVAL: cellSizeInZ
        })