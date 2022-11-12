using Enyim.Caching;
using Enyim.Caching.Configuration;
using Enyim.Caching.Memcached;

namespace packetcache
{
    public class MemcacheClient : ICache, IDisposable
    {
        public MemcacheClient(string server, int port)
        {
            var options = new MemcachedClientOptions();
            options.AddServer(server, port);
            options.Protocol = MemcachedProtocol.Binary;

            var logger = new Microsoft.Extensions.Logging.Abstractions.NullLoggerFactory();
            var config = new MemcachedClientConfiguration(logger, options);

            m_client = new MemcachedClient(logger, config);
        }

        public void Dispose()
        {
            m_client.Dispose();
            GC.SuppressFinalize(this);
        }

        public async Task<T?> GetAsync<T>(string key) where T : class
        {
            var result = await m_client.GetAsync<T>(key);
            return result.HasValue ? result.Value : null;
        }

        public async Task<bool> SetAsync(string key, object value, double cacheSeconds)
        {
            return await m_client.StoreAsync(StoreMode.Set, key, value, TimeSpan.FromSeconds(cacheSeconds));
        }

        public async Task<bool> DelAsync(string key)
        {
            return await m_client.RemoveAsync(key);
        }

        private MemcachedClient m_client;
    }
}