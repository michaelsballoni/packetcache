using packetcache;
using System.Linq;

if (args.Length < 4)
{
    Console.WriteLine("Usage: packetapp <mem|pac> <server> <ttl second> <# threads> <path to consume>");
    return;
}

string proto = args[0];
string server = args[1];

double cache_ttl_seconds;
if (!double.TryParse(args[2], out cache_ttl_seconds))
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

string path_to_consume = args[4];

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
        dir_files.Add(key, string.Join('\n', cur_dir_files));
    }
}
var dir_file_names = dir_files.Keys.ToArray();
Console.WriteLine("Dirs With Files: " + dir_files.Count);

var stats = new CacheStats();

var tasks = new Task[thread_count];
for (int t = 1; t <= thread_count; ++t)
    tasks[t - 1] = Task.Run(async () => await ProcessDictAsync());
Task.WaitAll(tasks);

Console.WriteLine($"hits: {stats.hits} - misses: {stats.misses} - total: {stats.total}");

async Task ProcessDictAsync()
{
    var cache = proto == "mem" ? new MemcacheClient(server, 11211) : null;
    if (cache == null)
    {
        Console.WriteLine("Unknown cache library: must be mem or pac");
        return;
    }

    DateTime start_time = DateTime.UtcNow;
    while (true)
    {
        int idx = Random.Shared.Next(0, dir_file_names.Length - 1);
        string dir_name = "" + idx;
        string? filenames = await cache.GetAsync<string>(dir_name);
        if (filenames == null)
        {
            lock (stats)
                stats.misses++;

            filenames = dir_files[dir_file_names[idx]];
            await cache.SetAsync(dir_name, filenames, cache_ttl_seconds);
        }
        else
        {
            lock (stats)
                stats.hits++;
        }

        if ((DateTime.UtcNow - start_time).TotalSeconds >= 5)
            break;
    }
}

class CacheStats
{
    public int hits, misses;
    public int total => hits + misses;
}

