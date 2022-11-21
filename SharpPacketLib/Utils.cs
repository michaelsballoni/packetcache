using System.IO.Hashing;
using System.Text;

namespace packetcache
{
    internal static class Utils
    {
        public static byte[] Crc32(byte[] bytes, int byteCount)
        {
            var crc = new Crc32();
            crc.Append(new ReadOnlySpan<byte>(bytes, 0, byteCount));
            var hash_bytes = crc.GetCurrentHash();
            return hash_bytes;
        }

        public static string StreamToStr(MemoryStream stream)
        {
            return Encoding.UTF8.GetString(stream.GetBuffer(), 0, (int)stream.Length);
        }

        public static void StrToStream(string str, MemoryStream stream)
        {
            int byte_count = Encoding.UTF8.GetByteCount(str);

            stream.Seek(0, SeekOrigin.Begin);
            stream.SetLength(byte_count);

            Encoding.UTF8.GetBytes(str, 0, str.Length, stream.GetBuffer(), byte_count);
        }
    }
}
