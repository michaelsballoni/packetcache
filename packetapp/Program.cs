using packetcache;

if (args.Length < 2)
{
    Console.WriteLine("Usage: packetapp <mem|pac> <server>");
    return;
}

string proto = args[0];
string server = args[1];

while (true)
{
    try
    {
        ICache cache = proto == "mem" ? new MemcacheClient(server, 11211) : new PacketCacheClient(server, 9914);
        if (cache == null)
        {
            Console.WriteLine("Unknown cache library: must be mem or pac");
            return;
        }

        while (true)
        {
            Console.Write("\nget <key> OR set <key> <value> <ttl seconds> OR del <key>\n> ");
            string? line = Console.ReadLine();
            if (line == null || string.IsNullOrWhiteSpace(line))
                continue;

            var tokens = line.Split(' ');
            if (tokens.Length < 2)
                continue;

            var verb = tokens[0];
            var key = tokens[1];

            if (verb == "get")
            {
                var value = cache.GetAsync(key).Result;
                Console.WriteLine("value: " + (value == null ? "(null)" : value));
            }
            else if (verb == "del")
            {
                bool success = cache.DelAsync(key).Result;
                if (!success)
                    Console.WriteLine("FAILED!\n");
            }
            else if (verb == "set")
            {
                if (tokens.Length != 4)
                {
                    Console.WriteLine("set takes key, value, and TTL seconds");
                    continue;
                }

                var value = tokens[2];

                int ttl_seconds;
                if (!int.TryParse(tokens[3], out ttl_seconds))
                {
                    Console.WriteLine("set TTL seconds is invalid");
                    continue;
                }

                bool success = cache.SetAsync(key, value, ttl_seconds).Result;
                if (!success)
                    Console.WriteLine("FAILED!\n");
            }
            else
            {
                Console.WriteLine("Unknown command: get, set, or del");
                continue;
            }
        }
    }
    catch (Exception exp)
    {
        while (exp.InnerException != null)
            exp = exp.InnerException;

        Console.WriteLine($"ERROR: {exp.GetType().FullName}: {exp.Message}");
        Console.WriteLine("\nHit [Enter] to reconnect and continue: ");
        Console.ReadLine();
    }
}
