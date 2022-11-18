using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace packetcache
{
    public class PacketCacheClient : ICache
    {
        public PacketCacheClient(string server, int port)
        {

        }

        public Task<string?> GetAsync(string key)
        {
            // build request packet
            // send request packet
            // recv response packet
            // return result
            throw new NotImplementedException();
        }

        public Task<bool> SetAsync(string key, string value, int cacheSeconds)
        {
            throw new NotImplementedException();
        }

        public Task<bool> DelAsync(string key)
        {
            throw new NotImplementedException();
        }
    }
}
