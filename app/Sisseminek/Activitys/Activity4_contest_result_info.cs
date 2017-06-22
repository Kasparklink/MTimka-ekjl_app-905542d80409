using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Json;

using Android.App;
using Android.Content;
using Android.OS;
using Android.Runtime;
using Android.Views;
using Android.Widget;
using System.Text.RegularExpressions;
using Java.Lang;
using System.ComponentModel;
using Android.Support.V4.Widget;
using Android.Support.V7.App;

namespace Sisseminek.Activitys {

    [Activity(Label = "")]
    public class Activity4_contest_result_info : ActionBarActivity
    {
        List<TableItem> tableItems = new List<TableItem>();
        List<TableItem> tableItems_more = new List<TableItem>();
        ListView listView;
        SwipeRefreshLayout mSwipeRefreshLayout;
        ListView listView_more;

        protected override void OnCreate(Bundle bundle)
        {
            base.OnCreate(bundle);

            SetContentView(Resource.Layout.lisa);
            listView = FindViewById<ListView>(Resource.Id.List2);
            listView_more = FindViewById<ListView>(Resource.Id.List1);

            JsonObject jarr = globals.athlete;
            JsonArray aarr = (JsonArray)jarr["results"];

            string rank = "",
                   name = "",
                   club = "",
                   best = "";

            rank += jarr["rank"];
            name += jarr["first_name"] + " " + jarr["last_name"];
            club += jarr["club"];
            best += jarr["best_result"];

            TableItem tt = new TableItem() { Koht = rank, Nimi = name, Klubi = club, Aeg = best };
            for (int i = 0; i < aarr.Count; i++) {
                if (i == 0) tt.Aeg1 = aarr[i]["result_string"];
                else if (i == 1) tt.Aeg2 = aarr[i]["result_string"];
                else if (i == 2) tt.Aeg3 = aarr[i]["result_string"];
                else if (i == 3) tt.Aeg4 = aarr[i]["result_string"];
                else if (i == 4) tt.Aeg5 = aarr[i]["result_string"];
                else if (i == 5) tt.Aeg6 = aarr[i]["result_string"];
            }

            // adding results
            for (int i = 0; i < aarr.Count; i++)
            {
                tableItems_more.Add(new TableItem() { Võistlus = globals.attempt_names[i]["item"]+"    "+aarr[i]["result_string"] });
            }

            tableItems.Add(tt);

            listView.Adapter = new Lisa(this, tableItems);
            //listView_more.Adapter = new MainDisain(this, tableItems_more);

            //listView.ItemClick += OnListItemClick;

            //mSwipeRefreshLayout = FindViewById<SwipeRefreshLayout>(Resource.Id.swipeLayout);
            //mSwipeRefreshLayout.SetColorScheme(Android.Resource.Color.HoloBlueBright, Android.Resource.Color.HoloBlueDark, Android.Resource.Color.HoloGreenLight, Android.Resource.Color.HoloRedLight);
            //mSwipeRefreshLayout.Refresh += mSwipeRefreshLayout_Refresh;
        }

        //void mSwipeRefreshLayout_Refresh(object sender, EventArgs f)
        //{
        //    BackgroundWorker worker = new BackgroundWorker();
        //    worker.DoWork += worker_DoWork;
        //    worker.RunWorkerCompleted += worker_RunWorkerCompleted;
        //    worker.RunWorkerAsync();
        //}

        //void worker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs f)
        //{
        //    RunOnUiThread(() => { mSwipeRefreshLayout.Refreshing = false; });
        //}

        //void worker_DoWork(object sender, DoWorkEventArgs f)
        //{
        //    //Will run on separate thread
        //    Thread.Sleep(2000);

        //}

        protected void OnListItemClick(object sender, Android.Widget.AdapterView.ItemClickEventArgs e)
        {
            var listView = sender as ListView;
            var t = tableItems[e.Position];
            Android.Widget.Toast.MakeText(this, t.Nimi, Android.Widget.ToastLength.Short).Show();
            Console.WriteLine("Clicked on " + t.Nimi);
        }
    }
}