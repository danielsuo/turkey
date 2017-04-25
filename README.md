# Turkey

[TODO](https://docs.google.com/document/d/1c7FWr20b7X3C0umUbcWgD6-azkyj5Yhzmg3Ah-hwTuU/edit)

## Getting started
- Run ```./bin/install```. This will set up the ```TURKEY_HOME``` environment variable in your ```~/.bashrc```.
- Run ```./bin/install_deps``` if on Ubuntu with root acces. Otherwise, call home and complain.
- Run ```./bin/turkey compile``` to compile all converted applications
- Run ```./bin/turkey compile -a [APP]``` to compile the application you're working on
- Run ```./bin/turkey data -a [APP]``` to download the data for the application you're working on
- Run ```./bin/turkey one -a [APP] -c native -n 8``` to run a one-off of the application with configuration native and 8 threads

## How to convert an application (your mileage may vary...)
- TODO

## Application status
### PARSEC
| Application     | Compiled        | Turkey          | TBB             |
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
| vips            |                 |                 |                 |
| x264            |                 |                 |                 |

### SPLASH-2
| Application     | Compiled        | Turkey          | TBB             |
| --------------- | :-------------: | :-------------: | :-------------: |
| barnes          |                 |                 |                 |
| cholesky        |                 |                 |                 |
| fft             |                 |                 |                 |
| fmm             |                 |                 |                 |
| lu              |                 |                 |                 |
| ocean           |                 |                 |                 |
| radiosity       |                 |                 |                 |
| radix           |                 |                 |                 |
| raytrace        |                 |                 |                 |
| volrend         |                 |                 |                 |
| water           |                 |                 |                 |

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
