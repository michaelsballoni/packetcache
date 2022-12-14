using System.IO.Hashing;
using System.Text;

namespace packetcache
{
    internal static class Utils
    {
        /// <summary>
        /// Compute the CRC32 checksum of some bytes
        /// </summary>
        public static byte[] Crc32(byte[] bytes, int byteCount)
        {
            var crc = new Crc32();
            crc.Append(new ReadOnlySpan<byte>(bytes, 0, byteCount));
            var hash_bytes = crc.GetCurrentHash();
            return hash_bytes;
        }

        /// <summary>
        /// Convert a stream into a string
        /// </summary>
        /// <param name="stream"></param>
        /// <returns></returns>
        public static string StreamToStr(MemoryStream stream)
        {
            return Encoding.UTF8.GetString(stream.GetBuffer(), 0, (int)stream.Length);
        }

        /// <summary>
        /// Covert a string into a stream
        /// </summary>
        public static void StrToStream(string str, MemoryStream stream)
        {
            int byte_count = Encoding.UTF8.GetByteCount(str);

            stream.Seek(0, SeekOrigin.Begin);
            stream.SetLength(byte_count);

            Encoding.UTF8.GetBytes(str, 0, str.Length, stream.GetBuffer(), 0);
        }
    }
}
