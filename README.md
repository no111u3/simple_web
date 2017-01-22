This is simple web server for static content with thread pool

Complile

* cmake .
* make (use -j option to faster compile)

Usage

#Run#
* $ ./simple_web {Flags}

#Flags#
* -n -- not daemon
* -d -- work directory (default is current run directory)
* -p -- port (default is 12345)
* -h -- host (default is 127.0.0.1 know as localhost)
