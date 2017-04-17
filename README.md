# Turkey

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

| Application     | Compiled        | Turkey          | TBB             |
| --------------- | :-------------: | :-------------: | :-------------: |
| blackscholes    | X               |                 | X               |
| bodytrack       | X               |                 | X               |
| canneal         | X               |                 |                 |
| dedup           | X               |                 |                 |
| facesim         |                 |                 |                 |
| ferret          |                 |                 |                 |
| fluidanimate    |                 |                 | X               |
| freqmine        | X               |                 |                 |
| raytrace        |                 |                 |                 |
| streamcluster   | X               |                 | X               |
| swaptions       |                 |                 | X               |
| vips            |                 |                 |                 |
| x264            |                 |                 |                 |

## Outstanding documentation
- Job files
- Conf files
- Job outputs
