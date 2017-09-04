# Turkey

[TODO](https://docs.google.com/document/d/1c7FWr20b7X3C0umUbcWgD6-azkyj5Yhzmg3Ah-hwTuU/edit)

## Getting started
- Run ```./bin/install```. This will set up the ```TURKEY_HOME``` environment variable in your ```~/.bashrc```.
- Run ```./bin/install_deps``` if on Ubuntu with root acces. Otherwise, call home and complain.
- Run ```source ~/.bashrc``` to get updated environment variables
- Run ```./bin/run.py build all``` to build all converted applications
- Run ```./bin/run.py build [APP]``` to build the application you're working on
- Run ```./bin/run.py data [APP]``` to download the data for the application you're working on. Use ```alll``` to download all applications
- Run ```./bin/run.py one [APP] -c native -n 8``` to run a one-off of the application with configuration native and 8 threads

## How to convert an application (your mileage may vary...)
- TODO

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

## Outstanding feature
- Modes (e.g., pthreads, tbb, turkey)

## Outstanding documentation
  - Job files
  - Conf files
  - Job outputs

## Outstanding clean-up
  - CMakeFiles.txt
  - Install scripts
  - Vendor dependencies
