import segyio

segy_file_path = "D:/Arquivos/seismic.sgy"
segy_output = segyio.open(segy_file_path, "r+")
segy_output.bin[3213] = 1
segy_output.bin[3215] = 0
segy_output.bin[3219] = 0
segy_output.bin[3223] = 0
segy_output.bin[3227] = 1
segy_output.bin[3229] = 4
segy_output.bin[3231] = 1

for i in range(len(segy_output.trace)):
    segy_output.header[i].update({
        segyio.TraceField.TraceNumber: i + 1,
        segyio.TraceField.TRACE_SEQUENCE_FILE: i + 1,
        segyio.TraceField.TraceIdentificationCode: 1,
        segyio.TraceField.DataUse: 1,
        segyio.TraceField.ElevationScalar: 69,
        segyio.TraceField.SourceGroupScalar: 71,
        segyio.TraceField.SourceType: 1
    })