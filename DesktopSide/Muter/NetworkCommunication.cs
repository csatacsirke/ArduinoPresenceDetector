using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Muter
{
    interface INetworkCommunicationObserver
    {
        void OnMessage(String message);
        void OnError(String message);
    }


    class NetworkCommunication
    {
        public int ArudinoToPcPort = 42100;
        public int PcToArduinoPort = 42110;
        public volatile bool Stop = false;
        public INetworkCommunicationObserver observer;


        public NetworkCommunication() {
            Thread workerThread = new Thread(new ThreadStart(DoWork));
            workerThread.Start();
        }

        

        void BroadcastQuery() {

            try {

                // This constructor arbitrarily assigns the local port number.
                UdpClient senderUdpClient = new UdpClient();
                senderUdpClient.EnableBroadcast = true;

                UdpClient receiverUdpClient = new UdpClient(ArudinoToPcPort);
                receiverUdpClient.Client.ReceiveTimeout = 5000;


                // Sends a message to the host to which you have connected.
                Byte[] sendBytes = Encoding.ASCII.GetBytes("query");

                // Sends a message to a different host using optional hostname and port parameters.
                //UdpClient udpClientB = new UdpClient();

                Task<UdpReceiveResult> task_ReceiveAsync = receiverUdpClient.ReceiveAsync();

                senderUdpClient.Send(sendBytes, sendBytes.Length, new IPEndPoint(IPAddress.Broadcast, PcToArduinoPort));

                //IPEndPoint object will allow us to read datagrams sent from any source.
                //IPEndPoint RemoteIpEndPoint = new IPEndPoint(IPAddress.Any, ArudinoToPcPort);

                // Blocks until a message returns on this socket from a remote host.
                UdpReceiveResult udpReceiveResult = task_ReceiveAsync.Result;

                Byte[] receivedBytes = udpReceiveResult.Buffer;
                IPEndPoint RemoteIpEndPoint = udpReceiveResult.RemoteEndPoint;

                string returnData = Encoding.ASCII.GetString(receivedBytes);

                // Uses the IPEndPoint object to determine which of these two hosts responded.
                Console.WriteLine("This is the message you received " +
                                             returnData.ToString());
                Console.WriteLine("This message was sent from " +
                                            RemoteIpEndPoint.Address.ToString() +
                                            " on their port number " +
                                            RemoteIpEndPoint.Port.ToString());

                //udpClient.Close();
                //udpClientB.Close();
                senderUdpClient.Close();
                receiverUdpClient.Close();

                observer?.OnMessage(returnData.ToString());
            } catch (SocketException e) { 
                if(e.SocketErrorCode == SocketError.TimedOut) {
                    observer?.OnError("Timeout.");
                } else {
                    Console.WriteLine(e.ToString());
                    observer?.OnError(e.ToString());
                }
            } catch (Exception e) {
                Console.WriteLine(e.ToString());
                observer?.OnError(e.ToString());
            } 

        }

        void DoWork() {

            while (!Stop) {
                BroadcastQuery();
                Thread.Sleep(1000);
            }

        }
    }
}
