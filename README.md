# packetcache
Cache all the news that fits...in a packet...

packetcache is a single-threaded UDP server that is kind of memcache's kid brother.  You can't cache much, the key and value together can't be more than around 500 bytes.  But maybe that's all you need?

Initial performance tests show that packetcache performance is on par with memcache, albeit for a much smaller clientel.

# Project Layout
## C++
### pluspacketlib
C++ static library with complete server and POC client implementation.  Relies on smhasher and zlib to be installed next to the solution directory to build.
### pluspacktests
C++ unit tests that hit the high points of pluspacketlib.
### packetserver
C++ server application, a thin wrapper around pluspacketlib's server.
## C#
### SharpPacketLib
C# class library that uses Enyim to implement a simple memcache client, and implements packet processing to be a likewise simple packetcache client.
### packetapp
Simple POC for smoke testing C# client library.
### packetstress
Stress test program for evaluating throughput of memcache vs. packetcache.  You give it some testing parameters, and a directory to process into a "database" of directory path -> list of filenames, up to 200 characters.  An arbitrary yet handy source of test data.  

Tests all-on-my-PC showed packetcache capable of 60K operations / second.  Using separate t3.small EC2's the throughput returned to earth at 20K / second.  Similar throughput was observed using a similar ElastiCache memcache, so the bottleneck may be with the testing system rather than with the server systems.

A Linux build of packetcache installed on comparable hardware as ElastiCache uses would make for an interesting comparison.
