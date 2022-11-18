namespace packetcache
{
    public interface ICache
    {
        Task<string?> GetAsync(string key);
        Task<bool> SetAsync(string key, string value, int cacheSeconds);
        Task<bool> DelAsync(string key);
    }
}
