using System.Net;

namespace packetcache
{
    internal class PacketException : Exception
    {
        public PacketException(string msg) : base(msg) { }
        public PacketException(string msg, Exception? inner) : base(msg, inner) { }
    }

    internal enum CacheOp
    {
		Failure,
        Success,

        Clear,

		Get,
		Put,
		Delete,

		GetStats,
		ClearStats,
	};

    internal class Packet : IDisposable
    {
        // https://stackoverflow.com/questions/1098897/what-is-the-largest-safe-udp-packet-size-on-the-internet
        public const int MaxPacketSize = 508;

        public const int MinPacketSize =
            1 + // op 
            4 + // ttl
            2 + // key len
            2 + // val len
            4;  // crc

        public CacheOp Op;
        public long TtlSeconds;

        public MemoryStream Key = new MemoryStream(MaxPacketSize);
        public MemoryStream Value = new MemoryStream(MaxPacketSize);

        public void Reset()
        {
            Op = CacheOp.Failure;
            TtlSeconds = 0;

            Key.Seek(0, SeekOrigin.Begin);
            Key.SetLength(0);

            Value.Seek(0, SeekOrigin.Begin);
            Value.SetLength(0);
        }

        public void Dispose()
        {
            Key.Dispose();
            Value.Dispose();
            GC.SuppressFinalize(this);
        }

        public override string ToString()
        {
            return $"{Op} - ttl: {TtlSeconds} - key: {Utils.StreamToStr(Key)} - value: {Utils.StreamToStr(Value)}";
        }

        public static void Pack(Packet p, byte[] output, out int idx)
        {
            // Make sure it will all fit
            if
            (
                1 +  // op
                4 + // ttl
                2 + p.Key.Length +
                2 + p.Value.Length +
                4   // crc
                > MaxPacketSize
            )
            {
                throw new PacketException("Too much data");
            }

            // Add the op
            idx = 0;
            output[idx++] = (byte)p.Op;

            // Add the TTL
            var ttl_bytes = BitConverter.GetBytes(IPAddress.HostToNetworkOrder((int)p.TtlSeconds));
            output[idx++] = ttl_bytes[0];
            output[idx++] = ttl_bytes[1];
            output[idx++] = ttl_bytes[2];
            output[idx++] = ttl_bytes[3];

            // Add the key len and data
            short key_len = (short)p.Key.Length;
            var key_len_bytes = BitConverter.GetBytes(IPAddress.HostToNetworkOrder(key_len));
            output[idx++] = key_len_bytes[0];
            output[idx++] = key_len_bytes[1];
            Buffer.BlockCopy(p.Key.GetBuffer(), 0, output, idx, key_len);
            idx += key_len;

            // Add the value len and data
            short value_len = (short)p.Value.Length;
            var value_len_bytes = BitConverter.GetBytes(IPAddress.HostToNetworkOrder(value_len));
            output[idx++] = value_len_bytes[0];
            output[idx++] = value_len_bytes[1];
            Buffer.BlockCopy(p.Value.GetBuffer(), 0, output, idx, value_len);
            idx += value_len;

            // CRC what we've got so far
            var crc_bytes = Utils.Crc32(output, idx);
            // DEBUG
            //uint crc_value = BitConverter.ToUInt32(crc_bytes);
            //Console.WriteLine($"Pack crc_value = {crc_value}"); 
            output[idx++] = crc_bytes[3];
            output[idx++] = crc_bytes[2];
            output[idx++] = crc_bytes[1];
            output[idx++] = crc_bytes[0];
        }

        public static void Parse(byte[] input, Packet p)
        {
            // Reset the the output
            p.Reset();

            // Validate data length
            short input_len = (short)input.Length;
            if (input_len < MinPacketSize)
                throw new PacketException("Not enough data");
            else if (input_len > MaxPacketSize)
                throw new PacketException("Too much data");

            // Validate the CRC
            byte[] crc_computed_bytes = Utils.Crc32(input, input_len - 4);
            // DEBUG
            //uint crc_computed_value = BitConverter.ToUInt32(crc_computed_bytes);
            //Console.WriteLine($"Parse crc_computed_value = {crc_computed_value}"); 
            if
            (
                crc_computed_bytes[0] != input[input_len - 1]
                ||
                crc_computed_bytes[1] != input[input_len - 2]
                ||
                crc_computed_bytes[2] != input[input_len - 3]
                ||
                crc_computed_bytes[3] != input[input_len - 4]
            )
            {
                throw new PacketException("Checksum mismath");
            }

            // Extract op
            int idx = 0;
            p.Op = (CacheOp)input[idx++];

            // Extract ttl
            p.TtlSeconds = IPAddress.HostToNetworkOrder(BitConverter.ToInt32(input, idx));
            idx += 4;

            // Extract key
            short key_len = IPAddress.HostToNetworkOrder(BitConverter.ToInt16(input, idx));
            idx += 2;
            if (key_len > 0)
            {
                if (idx + key_len >= input_len)
                    throw new PacketException("Invalid key length");

                p.Key.Write(input, idx, key_len);
                p.Key.Seek(0, SeekOrigin.Begin);

                idx += key_len;
            }

            // Extract value
            short value_len = IPAddress.HostToNetworkOrder(BitConverter.ToInt16(input, idx));
            idx += 2;
            if (value_len > 0)
            {
                if (idx + value_len >= input_len)
                    throw new PacketException("Invalid value length");

                p.Value.Write(input, idx, value_len);
                p.Value.Seek(0, SeekOrigin.Begin);

                idx += value_len;
            }

            // Validate input size
            if (idx != input_len - 4)
                throw new PacketException("Invalid packet format");
        }
    }
}
