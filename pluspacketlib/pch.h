#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#include <atomic>
#include <functional>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "MurmurHash2.h"
#include "zlib.h"
