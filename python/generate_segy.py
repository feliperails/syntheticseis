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

cellSizeInZ = int(round(data.attrs['cellSizeInZ'][0]))
segyio.tools.from_array3D(segy_file_path, data, dt=cellSizeInZ)
segy_output = segyio.open(segy_file_path, "r+")

origin_x = data.attrs['geometryOriginX'][0]
origin_y = data.attrs['geometryOriginY'][0]
inline_end_x = data.attrs['geometryInlineEndX'][0]
inline_end_y = data.attrs['geometryInlineEndY'][0]
crossline_end_x = data.attrs['geometryCrosslineEndX'][0]
crossline_end_y = data.attrs['geometryCrosslineEndY'][0]

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
            segyio.TraceField.SourceX: x,
            segyio.TraceField.SourceY: y,
            segyio.TraceField.GroupX: x,
            segyio.TraceField.GroupY: y,
            segyio.TraceField.TRACE_SAMPLE_INTERVAL: cellSizeInZ
        })