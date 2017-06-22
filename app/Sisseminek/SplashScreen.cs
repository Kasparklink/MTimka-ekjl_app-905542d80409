using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

using Android.App;
using Android.Content;
using Android.OS;
using Android.Runtime;
using Android.Views;
using Android.Widget;

namespace Sisseminek
{
    [Activity(Label = "EKJL", MainLauncher = true )]
    public class SplashScreen : Activity
    {
        protected override void OnCreate(Bundle bundle)
        {
            base.OnCreate(bundle);
            //Näitab splash screeni 4 sec
            //Thread.Sleep(2000);
            //Start Activity1 Activity

            // setup client
            globals.enco = new UTF8Encoding();
            client.OpenClient();

            // ryn new activity
            StartActivity(typeof(Activitys.Activity1));
        }
    }
}