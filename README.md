# Hash Table Shootout

A series of benchmarks that run against several hash table implementations,
written by Nick Welch in 2010. Updated in 2015 by Jim Belton.

## Release Notes
### 2015-11-26
* Fixed a bug whereby all C++ implementations where hashing pointer values rather than strings
* Vastly sped up benchmarks from hours to minutes
* Simplified standard output from benchmarks
* Added `kjv-bench.py` which benchmarks each hash table implementation by counting the unique words in the King James version of the bible.

## How to Run the Benchmarks

First, some prerequisites are:

* make (~3.81)
* gcc/g++ (~4.8.2)
* python (hard-coded for 2.7 -- edit Makefile to use another version)
* ruby (hard-coded for 1.9.1 -- edit Makefile to use another version)

Install the hash libraries:

1. Install glib by running: `sudo apt-get install libglib2.0-dev`
2. Install boost by running: `sudo apt-get install libboost-all-dev`
3. Install google sparsehash: `sudo apt-get install libsparsehash-dev`
4. Install qt dev by running: `sudo apt-get install qtbase5-dev`
5. Install python dev; run: `sudo apt-get install python-dev`
6. Install ruby dev; run: `sudo apt-get install ruby-dev`

Now, run:

```
$ make
$ python bench.py # Note: This step takes many minutes.
$ python make_chart_data.py < output | python make_html.py
$ python kjv-bench.py # Runs the new KJV benchmark against all the implementations (fast)
```

Your charts are now in charts.html.

## Tweaks and Tips

You can tweak some of the values in bench.py to make it run faster at the
expense of less granular data, and you might need to tweak some of the *tickSize*
settings in `charts-template.html`.

To run the benchmark at the highest priority possible, do this:

```
$ sudo nice -n-20 ionice -c1 -n0 sudo -u $USER python bench.py
```

You might also want to disable any swap files/partitions so that swapping
doesn't influence performance.  The programs will die if they try to
allocate too much memory.

## Copyright Information

Originally written by Nick Welch in 2010. Updated and enhanced by Jim Belton in 2015
Copyright by Jim Belton. Licensed under the [MIT License](https://opensource.org/licenses/MIT)
