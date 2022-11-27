using packetcache;
using System.Linq;

if (args.Length < 4)
{
    Console.WriteLine("Usage: packetstress <mem|pac> <server> <ttl second> <# threads> <test length seconds> <path to consume>");
    return;
}

string proto = args[0];
string server = args[1];

int cache_ttl_seconds;
if (!int.TryParse(args[2], out cache_ttl_seconds))
{
    Console.WriteLine("Invalid number of threads");
    return;
}

int thread_count;
if (!int.TryParse(args[3], out thread_count))
{
    Console.WriteLine("Invalid number of threads");
    return;
}

int seconds_to_run_for;
if (!int.TryParse(args[4], out seconds_to_run_for))
{
    Console.WriteLine("Invalid seconds to run for");
    return;
}

string path_to_consume = args[5];

Console.Write("Getting all subdirectories... ");
string[] music_sub_dirs = Directory.GetDirectories(path_to_consume, "*", SearchOption.AllDirectories);
Console.WriteLine("Subdirectories: " + music_sub_dirs.Length);

Console.Write("Getting subdirectories with files... ");
var dir_files = new Dictionary<string, string>(music_sub_dirs.Length);
foreach (string cur_dir in music_sub_dirs)
{
    string[] cur_dir_files = Directory.GetFiles(cur_dir, "*", SearchOption.TopDirectoryOnly);
    if (cur_dir_files.Length > 0)
    {
        string key = cur_dir.Substring(path_to_consume.Length).Trim(Path.DirectorySeparatorChar);
        string value = string.Join('\n', cur_dir_files.Select(path => Path.GetFileName(path)));
        if (value.Length > 200)
            value = value.Substring(0, 200);
        dir_files.Add(key, value);
    }
}
var dir_file_names = dir_files.Keys.ToArray();
Console.WriteLine("Dirs With Files: " + dir_files.Count);

var stats = new CacheStats();

var tasks = new Task[thread_count];
for (int t = 1; t <= thread_count; ++t)
    tasks[t - 1] = Task.Run(async () => await ProcessDictAsync());
Task.WaitAll(tasks);

Console.WriteLine($"hits: {stats.hits} - misses: {stats.misses} - total: {stats.total} - {stats.hitPercent}% - {stats.total / seconds_to_run_for} / sec");

async Task ProcessDictAsync()
{
    ICache cache = proto == "mem" ? new MemcacheClient(server, 11211) : new PacketCacheClient(server, 9914);
    if (cache == null)
    {
        Console.WriteLine("Unknown cache library: must be mem or pac");
        return;
    }

    DateTime start_time = DateTime.UtcNow;
    while (true)
    {
        int idx = Random.Shared.Next(0, dir_file_names.Length - 1);
        string dir_name = $"{idx}";
        string? gotten_filenames = await cache.GetAsync(dir_name);
        if (gotten_filenames == null)
        {
            lock (stats)
                stats.misses++;

            gotten_filenames = dir_files[dir_file_names[idx]];
            await cache.SetAsync(dir_name, gotten_filenames, cache_ttl_seconds);
        }
        else
        {
            lock (stats)
                stats.hits++;

            string actual_filenames = dir_files[dir_file_names[idx]];
            if (gotten_filenames != actual_filenames)
            {
                Console.WriteLine($"Cache get-put mismatch:\ngot:\n{gotten_filenames}\nshould be:\n{actual_filenames}");
                Environment.Exit(1);
            }
        }

        if ((DateTime.UtcNow - start_time).TotalSeconds >= seconds_to_run_for)
            break;
    }
}

class CacheStats
{
    public int hits, misses;
    public int total => hits + misses;
    public int hitPercent => (int)Math.Round(100.0 * (double)hits / total);
}
