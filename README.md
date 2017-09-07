# Turkey

## Getting started

### Installing
- Run ```./bin/install```. This will set up the ```TURKEY_HOME``` environment variable in your ```~/.bashrc```.
- Run ```./bin/install_deps``` if on Ubuntu with root acces. Otherwise, call home and complain.
- Run ```source ~/.bashrc``` to get updated environment variables
- Run ```./bin/run.py build all``` to build all converted applications
- Run ```./bin/run.py data [APP]``` to download the data for the application you're working on. Use ```all``` to download all applications

NOTE: You may need to specify a cmake executable during build
```bash
./bin/run.py build all -e cmake3
```

### Installing on the Intel vLab cluster
Add the following to your ```~/.bashrc```
```bash
alias cmake=cmake3
source scl_source enable devtoolset-6
```

NOTE: On CentOS, you may need to upgrade gcc
```bash
# Install newer version of gcc
sudo yum install devtoolset-6*

# Set gcc to newer version in current terminal
scl enable devtoolset-6 bash
```

### Building
- Run ```./bin/run.py build [APP]``` to build the application you're working on

### Running
- Run ```./bin/run.py one [APP] -c native -n 8``` to run a one-off of the application with configuration native and 8 threads
- Run ```./bin/run.py gen jobs/example.params``` to generate a list of tasks according to a params file (parsed by ```turkey/generator.py```)
- Run ```./bin/run.py job jobs/example.jobs``` to run the jobs generated in the above step (job file has the same name and location as params file)
- Run ```./bin/run.py parse [PARAMS] [JOB] [OUTPUT_DIR]``` to generate csv file of task-level stats
- Run ```./bin/run.py viz [PARAMS] [JOB] [OUTPUT_DIR]``` to generate graphs from the job run

### Running on vLab
- Useful guide [link](http://qcd.phys.cmu.edu/QCDcluster/pbs/run_serial.html)

### Statistics
- Task-level stats are parsed via the ```parse``` sub-command (see above)
- System-level stats are stored in ```[OUTPUT_DIR]/stats.csv```

## Application status
### PARSEC
| Application     | compiled        | Turkey          | TBB             |
| --------------- | :-------------: | :-------------: | :-------------: |
| blackscholes    | X               | X               | X               |
| bodytrack       | X               |                 | X               |
| canneal         | X               | X               |                 |
| dedup           | X               |                 |                 |
| facesim         | X               |                 |                 |
| ferret          | X               |                 |                 |
| fluidanimate    | X               | X               | X               |
| freqmine        | X               |                 |                 |
| streamcluster   | X               |                 | X               |
| swaptions       | X               |                 | X               |
| vips            | X               |                 |                 |
| x264            | X               |                 |                 |

### SPLASH-2
| Application     | compiled        | Turkey          | TBB             |
| --------------- | :-------------: | :-------------: | :-------------: |
| barnes          | X               |                 |                 |
| cholesky        |                 |                 |                 |
| fft             |                 |                 |                 |
| fmm             |                 |                 |                 |
| lu_cb           |                 |                 |                 |
| lu_ncb          |                 |                 |                 |
| ocean_cp        |                 |                 |                 |
| ocean_ncp       |                 |                 |                 |
| radiosity       |                 |                 |                 |
| radix           |                 |                 |                 |
| raytrace        |                 |                 |                 |
| volrend         |                 |                 |                 |
| water_nsquared  |                 |                 |                 |
| water_spatial   |                 |                 |                 |

