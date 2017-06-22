using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Diagnostics;

namespace Sisseminek {
    public enum ClientState {
        Working,
        Idle
    }

    class client {
        public static ClientState State;

        public static void OpenClient() {
            globals.tcpclient = new TcpClient();
            globals.tcpclient.Connect(globals.IP, globals.PORT);
            globals.stm = globals.tcpclient.GetStream();
        }

        public static void CloseClient() {
            globals.tcpclient.Close();
            globals.stm.Close();
        }

        public static void SendString(string text) {
            State = ClientState.Working;

            text += ";end";
            byte[] send = globals.enco.GetBytes(text);
            globals.stm.Write(send, 0, send.Length);
            State = ClientState.Idle;
        }

        public static string ReceiveResponse() {
            const int messageSize = 2048;
            double readSoFar = 0;
            int read = 0;
            string all_msg = "";

            State = ClientState.Working;
            for (;;) {
                byte[] msg = new byte[messageSize];
                read = globals.stm.Read(msg, 0, messageSize);

                string str = globals.enco.GetString(msg, 0, read);

                if (str.EndsWith(";end"))
                {
                    read -= 4;
                    all_msg += str.Substring(0, str.Length - 4);
                    readSoFar += read;
                    break;
                }

                all_msg += str;
                readSoFar += read;
            }

            State = ClientState.Idle;

            return all_msg;
        }
    }
}