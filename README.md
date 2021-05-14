# Dependencies 

* _libevent_ (or _libev_) for the primary I/O backend.
* _openssl_ for SSL transport.
* _CMake_ version 2.8.9 or greater (for building)
* _gcc_ C compiler


# Steps to build libcouchbase



$ git clone git://github.com/couchbase/libcouchbase.git
$ cd libcouchbase && mkdir build && cd build
$ ../cmake/configure
$ make
$ ctest

# Steps to get and install JSON-C library 

$ git clone https://github.com/json-c/json-c.git
$ mkdir json-c-build
$ cd json-c-build
$ cmake ../json-c  

$ make
$ make install

## Linking to libjson-c 
If your system has pkgconfig, then you can just add this to your makefile:

CFLAGS += $(shell pkg-config --cflags json-c)
LDFLAGS += $(shell pkg-config --libs json-c)

# Running the example 

Install Couchbase 7.0 build with a test bucket with a single document to start with 

test1::
{
  "a": 1,
  "b": 2,
  "c": "xyz"
}

Compile using options (the path points to the install location of the json-c library)
gcc -I /usr/local/include -L /usr/local/lib -lcouchbase n1ql.c -o n1ql -ljson-c
 
Run with options couchbase://<IP>/<bucket_name> username password 
 ./n1ql couchbase://localhost/test Administrator password
