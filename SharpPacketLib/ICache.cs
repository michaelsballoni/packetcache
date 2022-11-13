﻿namespace packetcache
{
    public interface ICache
    {
        Task<T?> GetAsync<T>(string key) where T : class;
        Task<bool> SetAsync(string key, object value, int cacheSeconds);
        Task<bool> DelAsync(string key);
    }
}
