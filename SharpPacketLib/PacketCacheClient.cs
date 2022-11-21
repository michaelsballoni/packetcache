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

            GC.SuppressFinalize(this);
        }

        public async Task<string?> GetAsync(string key)
        {
            Utils.StrToStream(key, m_requestPacket.Key);

            m_requestPacket.Op = CacheOp.Get;

            await ProcessPacketAsync();
            if (m_responsePacket.Op != CacheOp.Success)
                return null;
            else
                return Utils.StreamToStr(m_responsePacket.Value);
        }

        public async Task<bool> SetAsync(string key, string value, int cacheSeconds)
        {
            Utils.StrToStream(key, m_requestPacket.Key);
            Utils.StrToStream(value, m_requestPacket.Value);

            m_requestPacket.Op = CacheOp.Put;
            m_requestPacket.TtlSeconds = cacheSeconds;

            await ProcessPacketAsync();
            return m_responsePacket.Op == CacheOp.Success;
        }

        public async Task<bool> DelAsync(string key)
        {
            Utils.StrToStream(key, m_requestPacket.Key);

            m_requestPacket.Op = CacheOp.Delete;
            
            await ProcessPacketAsync();
            return m_responsePacket.Op == CacheOp.Success;
        }

        private async Task ProcessPacketAsync()
        {
            // Build the request
#if DEBUG
            Console.WriteLine("request: " + m_requestPacket);
#endif
            int request_len;
            Packet.Pack(m_requestPacket, m_requestBuffer, out request_len);
#if DEBUG
            Console.WriteLine("ProcessPacketAsync: request_len = " + request_len);
#endif
            // Send the request
            if (await m_client.SendAsync(m_requestBuffer, request_len) != request_len)
                throw new PacketException("Not all data sent");

            // Recv the response
            byte[] response = (await m_client.ReceiveAsync()).Buffer;
#if DEBUG
            Console.WriteLine("ProcessPacketAsync: response_len = " + response.Length);
#endif
            // Parse the buffer into a usable response packet
            Packet.Parse(response, m_responsePacket);
#if DEBUG
            Console.WriteLine("ProcessPacketAsync: response: " + m_responsePacket);
#endif
        }

        private UdpClient m_client;

        private Packet m_requestPacket = new Packet();
        private Packet m_responsePacket = new Packet();

        private byte[] m_requestBuffer = new byte[Packet.MaxPacketSize];
    }
}
