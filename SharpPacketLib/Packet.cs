using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Net.Sockets;

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

        public CacheOp Op;
        public long ExpirationEpochSeconds;

        public MemoryStream Key = new MemoryStream(MaxPacketSize);
        public MemoryStream Value = new MemoryStream(MaxPacketSize);

        public void Dispose()
        {
            Key.Dispose();
            Value.Dispose();
            GC.SuppressFinalize(this);
        }

        public static void Pack(Packet p, MemoryStream output)
        {
            // Reset the output
            output.Seek(0, SeekOrigin.Begin);
            output.SetLength(0);

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
            output.Write()
            output.push_back(uint8_t(p.op));

            // Add the TTL
            uint32_t ttl_net = htonl(uint32_t(p.expiration));
            uint8_t* ttl_data = reinterpret_cast<uint8_t*>(&ttl_net);
            output.insert(output.end(), { ttl_data[0], ttl_data[1], ttl_data[2], ttl_data[3] });

            // Add the key len and data
            u_short key_len_net = htons(u_short(p.key.size()));
            uint8_t* key_len_data = reinterpret_cast<uint8_t*>(&key_len_net);
            output.insert(output.end(), { key_len_data[0], key_len_data[1] });
            output.insert(output.end(), p.key.begin(), p.key.end());

            // Add the value len and data
            u_short value_len_net = htons(u_short(p.value.size()));
            uint8_t* value_len_data = reinterpret_cast<uint8_t*>(&value_len_net);
            output.insert(output.end(), { value_len_data[0], value_len_data[1] });
            output.insert(output.end(), p.value.begin(), p.value.end());

            // CRC what we've got so far, and add to the output
            uint32_t crc_net = htonl(crc_bytes(output.data(), output.size()));
            uint8_t* crc_data = reinterpret_cast<uint8_t*>(&crc_net);
            output.insert(output.end(), { crc_data[0], crc_data[1], crc_data[2], crc_data[3] });
        }

        public static void Parse(MemoryStream input, Packet p)
        {

        }
    }
}
