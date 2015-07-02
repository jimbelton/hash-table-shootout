all: build/glib_hash_table build/stl_unordered_map build/boost_unordered_map build/google_sparse_hash_map build/google_dense_hash_map build/qt_qhash build/python_dict build/ruby_hash

build/glib_hash_table: src/glib_hash_table.c Makefile src/template.c
	if [ ! -d build ]; then mkdir build; fi
	gcc -ggdb -O2 -lm `pkg-config --cflags --libs glib-2.0` src/glib_hash_table.c -o build/glib_hash_table

build/stl_unordered_map: src/stl_unordered_map.cc Makefile src/template.c
	if [ ! -d build ]; then mkdir build; fi
	g++ -O2 -lm src/stl_unordered_map.cc -o build/stl_unordered_map -std=c++0x

build/boost_unordered_map: src/boost_unordered_map.cc Makefile src/template.c
	if [ ! -d build ]; then mkdir build; fi
	g++ -O2 -lm src/boost_unordered_map.cc -o build/boost_unordered_map

build/google_sparse_hash_map: src/google_sparse_hash_map.cc Makefile src/template.c
	if [ ! -d build ]; then mkdir build; fi
	g++ -O2 -lm src/google_sparse_hash_map.cc -o build/google_sparse_hash_map

build/google_dense_hash_map: src/google_dense_hash_map.cc Makefile src/template.c
	if [ ! -d build ]; then mkdir build; fi
	g++ -O2 -lm src/google_dense_hash_map.cc -o build/google_dense_hash_map

build/qt_qhash: src/qt_qhash.cc Makefile src/template.c
	if [ ! -d build ]; then mkdir build; fi
# Replaced this because it didn't compile
#	g++ -O2 -lm `pkg-config --cflags --libs QtCore` src/qt_qhash.cc -o build/qt_qhash
	g++ -O2 -lm -fPIC -I/usr/include/qt5 -I/usr/include/qt5/QtCore -L /usr/lib/x86_64-linux-gnu -lQt5Core src/qt_qhash.cc -o build/qt_qhash

build/python_dict: src/python_dict.c Makefile src/template.c
	if [ ! -d build ]; then mkdir build; fi
	gcc -O2 -lm -I/usr/include/python2.7 -lpython2.7 src/python_dict.c -o build/python_dict

build/ruby_hash: src/ruby_hash.c Makefile src/template.c
	if [ ! -d build ]; then mkdir build; fi
	gcc -O2 -lm -I/usr/include/ruby-1.9.1 -I /usr/include/ruby-1.9.1/x86_64-linux -lruby-1.9.1 src/ruby_hash.c -o build/ruby_hash
