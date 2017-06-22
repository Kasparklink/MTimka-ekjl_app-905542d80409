using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Json;

using System.Net;
using System.Net.Sockets;
using System.IO;

namespace Sisseminek {
    class globals {
        public static TcpClient tcpclient;
        public static Stream stm;
        public static UTF8Encoding enco;
        
        public const string IP = "79.137.73.68";
        public const int PORT = 1001;

        public static string compId;
        public static string compName;
        public static string heatId;
        public static string heatName;
        public static JsonObject athlete;
        public static JsonArray attempt_names;
    }
}