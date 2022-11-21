#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#include <memory>
#include <string>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <vector>

#include "MurmurHash2.h"
#include "zlib.h"
