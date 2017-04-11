"""
Parsing vmstat output.

vmstat.py
"""


import os


# TODO: should have a start/top marker and add metadata to time_file
def split_run(working_dir, vmstat_file, time_file):
    """
    Split vmstat output into files based on timestamp boundaries.

    vmstat_file: vmstat output decorated with $(date) via ./bin/vmstat
    time_file: time boundaries to split by
    """
    vmstats = open(os.path.join(working_dir, vmstat_file), 'r')
    timestamps = open(os.path.join(working_dir, time_file), 'r')

    stamp_len = len('Tue Apr 11 10:17:57 EDT 2017')

    markers = []
    for timestamp in timestamps:
        args = timestamp[stamp_len + 1:].strip().split(' ')
        timestamp = timestamp[:stamp_len]
        markers.append({
            'args': args,
            'timestamp': timestamp
        })

    marker_counter = 0
    output = None
    for vmstat in vmstats:

        args = vmstat[stamp_len + 1:].strip().split()
        timestamp = vmstat[:stamp_len]

        if marker_counter < len(markers) and timestamp in markers[marker_counter]['timestamp']:
            margs = markers[marker_counter]['args']
            if output:
                output.close()
            output = open(os.path.join(working_dir, '_'.join(margs) + '.out'), 'w')
            marker_counter += 1
        if output:
            output.write(','.join(args) + '\n')

    vmstats.close()
    timestamps.close()
