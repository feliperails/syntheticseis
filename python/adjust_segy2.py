import segyio

segy_input_file_path = "D:/Arquivos/seismic.sgy"
segy_input = segyio.open(segy_input_file_path, "r")

segy_output_file_path = "D:/Arquivos/seismic_output.sgy"
segy_output = segyio.open(segy_input_file_path, "r+")

segy_output.bin[3217] = 1
segy_output.trace = segy_input.trace

length = len(segy_output.trace)
print(length)
for i in range(length):
    segy_output.header[i].update({
        segyio.TraceField.TRACE_SAMPLE_INTERVAL: 1,
        segyio.TraceField.SourceX: segy_output.header[i][segyio.TraceField.SourceX],
        segyio.TraceField.SourceY: segy_output.header[i][segyio.TraceField.SourceY],
        segyio.TraceField.GroupX: segy_output.header[i][segyio.TraceField.GroupX],
        segyio.TraceField.GroupY: segy_output.header[i][segyio.TraceField.GroupY]
    })