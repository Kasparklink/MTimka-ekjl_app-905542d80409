using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Android.App;
using Android.Content;
using Android.OS;
using Android.Runtime;
using Android.Views;
using Android.Widget;

namespace Sisseminek
{
    public class TableItem
    {
        //Info Display Asjad
        public string Nimi { get; set; }
        public string Klubi { get; set; }
        public int Number { get; set; }
        public string Aeg { get; set; }
        public string Koht { get; set; }
        // public int ImageResourceId { get; set; }

        //Maini Asjad
        public string Id { get; set; }
        public string Võistlus { get; set; }

        //Lisainfo Asjad 
        public string Aeg1 { get; set; }
        public string Aeg2 { get; set; }
        public string Aeg3 { get; set; }
        public string Aeg4 { get; set; }
        public string Aeg5 { get; set; }
        public string Aeg6 { get; set; }

    }
}
