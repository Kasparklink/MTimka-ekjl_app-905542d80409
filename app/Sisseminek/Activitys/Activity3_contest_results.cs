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
using System.Diagnostics;
using System.ComponentModel;
using Android.Support.V4.Widget;
using System.Threading;
using Android.Support.V7.App;

namespace Sisseminek.Activitys {

    [Activity(Label = "")]
    public class Activity3_contest_results : ActionBarActivity { 
        List<TableItem> tableItems = new List<TableItem>();
        ListView listView;
        SwipeRefreshLayout mSwipeRefreshLayout;
        JsonArray jarr = null;
        JsonArray aarr = null;

        private void update() {
            client.SendString("SZ_GET_RESULTS=" + globals.heatId);
            string res = client.ReceiveResponse();

            //System.Diagnostics.Debug.WriteLine(res);

            var json = JsonObject.Parse(res);
            jarr = (JsonArray)json["items"];
            aarr = (JsonArray)json["attempt_names"];

            // getting lowest rank
            List<string> ranks = new List<string>();
            for (int i = 0; i < jarr.Count; i++)
                ranks.Add(jarr[i]["rank"]);
            ranks.Sort(new SemiNumericComparer());

            string last_rank = "";
            int rank_forward = 0;
            int last_forward = 0;

            for (int i = 0; i < ranks.Count; i++)
            {
                for (int j = 0; j < jarr.Count; j++)
                {
                    if (jarr[j]["rank"] == ranks[i]) {
                        if (last_rank == jarr[j]["rank"] && rank_forward != 0)
                        {
                            last_forward++;
                            rank_forward--;
                        }
                        else
                        {
                            if (last_rank != jarr[j]["rank"])
                            {
                                last_forward = 0;
                            }

                            last_rank = jarr[j]["rank"];
                            rank_forward = last_forward + 1;

                            tableItems.Add(new TableItem() { Nimi = jarr[j]["first_name"] + " " + jarr[j]["last_name"], Klubi = jarr[j]["club"], Aeg = jarr[j]["best_result"], Koht = jarr[j]["rank"], Number = j });
                            break;
                        }
                    }
                }
            }


            listView.Adapter = new Proov(this, tableItems);    
        }

        protected override void OnCreate(Bundle bundle) {
            base.OnCreate(bundle);

            SetContentView(Resource.Layout.proovinfo);
            listView = FindViewById<ListView>(Resource.Id.List);
            Window.SetTitle(globals.heatName);

            update();

            // setting listview height
            listView.LayoutParameters.Height = 
                Convert.ToInt32(funcs.convertPixelsToDp(this, Resources.DisplayMetrics.HeightPixels));

            listView.ItemClick += OnListItemClick;

            //mSwipeRefreshLayout = FindViewById<SwipeRefreshLayout>(Resource.Id.swipeLayout);
            //mSwipeRefreshLayout.SetColorScheme(Android.Resource.Color.HoloBlueBright, Android.Resource.Color.HoloBlueDark, Android.Resource.Color.HoloGreenLight, Android.Resource.Color.HoloRedLight);
            //mSwipeRefreshLayout.Refresh += mSwipeRefreshLayout_Refresh;
        }

        void mSwipeRefreshLayout_Refresh(object sender, EventArgs f) {
            (new Thread(new ThreadStart(_thread_update))).Start();
        }

        private void _thread_update() {
            RunOnUiThread(() => {
                update();
                mSwipeRefreshLayout.Refreshing = false;
            });
        }

        protected void OnListItemClick(object sender, Android.Widget.AdapterView.ItemClickEventArgs e) {
            var listView = sender as ListView;
            var t = tableItems[e.Position];

            globals.athlete = (JsonObject)jarr[t.Number];
            globals.attempt_names = aarr;

            StartActivity(typeof(Activitys.Activity4_contest_result_info));
            //Android.Widget.Toast.MakeText(this, t.Nimi, Android.Widget.ToastLength.Short).Show();
            //Console.WriteLine("Clicked on " + t.Nimi);
            
        }
    }
}