using Newtonsoft.Json.Linq;
using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace packetcache
{
    public class PacketCacheClient : ICache, IDisposable
    {
        public PacketCacheClient(string server, int port)
        {
            m_client = new UdpClient();
            m_client.Connect(server, port);

            m_client.Client.ReceiveTimeout = 10 * 1000;
        }

        public void Dispose()
        {
            m_client.Dispose();

            m_requestPacket.Dispose();
            m_responsePacket.Dispose();

            m_requestBuffer.Dispose();
            m_responseBuffer.Dispose();

            GC.SuppressFinalize(this);
        }

        public async Task<string?> GetAsync(string key)
        {
            StrToStream(key, m_requestPacket.Key);

            m_requestPacket.Op = CacheOp.Get;

            await ProcessPacketAsync();
            if (m_responsePacket.Op != CacheOp.Success)
                return null;
            else
                return StreamToStr(m_responsePacket.Value);
        }

        public async Task<bool> SetAsync(string key, string value, int cacheSeconds)
        {
            StrToStream(key, m_requestPacket.Key);
            StrToStream(value, m_requestPacket.Value);

            m_requestPacket.Op = CacheOp.Put;
            m_requestPacket.TtlSeconds = cacheSeconds;

            await ProcessPacketAsync();
            return m_responsePacket.Op == CacheOp.Success;
        }

        public async Task<bool> DelAsync(string key)
        {
            StrToStream(key, m_requestPacket.Key);

            m_requestPacket.Op = CacheOp.Delete;
            
            await ProcessPacketAsync();
            return m_responsePacket.Op == CacheOp.Success;
        }

        private async Task ProcessPacketAsync()
        {
            // Build out request packet
            Packet.Pack(m_requestPacket, m_requestBuffer);

            // Send the request
            int request_buffer_len = (int)m_requestBuffer.Length;
            if (await m_client.SendAsync(m_requestBuffer.GetBuffer(), request_buffer_len) != request_buffer_len)
                throw new PacketException("Not all data sent");

            // Recv the response
            byte[] received = (await m_client.ReceiveAsync()).Buffer;

            // Parse the buffer into a usable response packet
            Packet.Parse(received, m_responsePacket);
        }

        private static void StrToStream(string str, MemoryStream stream)
        {
            int byte_count = Encoding.UTF8.GetByteCount(str);

            stream.Seek(0, SeekOrigin.Begin);
            stream.SetLength(byte_count);

            Encoding.UTF8.GetBytes(str, 0, str.Length, stream.GetBuffer(), byte_count);
        }

        private static string StreamToStr(MemoryStream stream)
        {
            return Encoding.UTF8.GetString(stream.GetBuffer(), 0, (int)stream.Length);
        }

        private UdpClient m_client;

        private Packet m_requestPacket = new Packet();
        private Packet m_responsePacket = new Packet();

        private MemoryStream m_requestBuffer = new MemoryStream(Packet.MaxPacketSize);
        private MemoryStream m_responseBuffer = new MemoryStream(Packet.MaxPacketSize);
    }
}
