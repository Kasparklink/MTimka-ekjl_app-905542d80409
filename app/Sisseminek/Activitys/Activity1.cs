using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Json;
using System.Diagnostics;

using Android.App;
using Android.Content;
using Android.OS;
using Android.Runtime;
using Android.Views;
using Android.Widget;

using System.Net;
using System.Net.Sockets;
using System.Text.RegularExpressions;
using Android.Support.V4.Widget;
using Android.Util;
using System.ComponentModel;
using System.Threading.Tasks;

using Android.Support.V7.AppCompat;
using Android.Support.V7.App;

using Toolbar = Android.Support.V7.Widget.Toolbar;

namespace Sisseminek.Activitys {
    [Activity(Label = "Võistlused")]
    public class Activity1 : ActionBarActivity {
        List<Adapters.Items.Competition> tableItems = new List<Adapters.Items.Competition>();
        ListView listView;
        SwipeRefreshLayout mSwipeRefreshLayout;

        private void update() {
            tableItems.Clear();

            client.SendString("SZ_GET_COMPS");
            string res = client.ReceiveResponse().Trim();

            var json = JsonObject.Parse(res);
            var jarr = (JsonArray)json["items"];

            listView = FindViewById<ListView>(Resource.Id.List);

            // setting items
            for (int i = 0; i < jarr.Count; i++) {
                var item = (JsonObject)jarr[i];
                tableItems.Add(new Adapters.Items.Competition() { Id = (string)item["id"], Title = (string)item["name"] });
            }


            RunOnUiThread(() => {
                listView.Adapter = new Adapters.Competitions(this, tableItems);
            });
        }

        protected override void OnCreate(Bundle bundle)
        {
            base.OnCreate(bundle);
            SetContentView(Resource.Layout.layout1_main);

            var toolbar = (Toolbar)FindViewById(Resource.Id.toolbar);
            SetSupportActionBar(toolbar);
            SupportActionBar.Title = GetString(Resource.String.title_activity1);

            update();

            // events
            listView.ItemClick += OnListItemClick;

            FindViewById<ImageView>(Resource.Id.iv_ad1).Click += (s, e) => StartUrl("https://www.facebook.com/kergejoustik?sk=wall");
            FindViewById<ImageView>(Resource.Id.iv_ad2).Click += (s, e) => StartUrl("https://www.youtube.com/user/kergej6ustik");
            FindViewById<ImageView>(Resource.Id.iv_ad3).Click += (s, e) => StartUrl("http://sport.delfi.ee/news/kergejoustik/?l=tn_sport");
            FindViewById<ImageView>(Resource.Id.iv_ad4).Click += (s, e) => StartUrl("http://ekjl.ee/");

            mSwipeRefreshLayout = FindViewById<SwipeRefreshLayout>(Resource.Id.swipeLayout);
            mSwipeRefreshLayout.SetColorScheme(Android.Resource.Color.HoloBlueBright, Android.Resource.Color.HoloBlueDark, Android.Resource.Color.HoloGreenLight, Android.Resource.Color.HoloRedLight);
            mSwipeRefreshLayout.Refresh += mSwipeRefreshLayout_Refresh;
        }

        private void StartUrl(string url) {
            var uri = Android.Net.Uri.Parse(url);
            var intent = new Intent(Intent.ActionView, uri);
            StartActivity(intent);
        }

        private void mSwipeRefreshLayout_Refresh(object sender, EventArgs e) {
            (new Thread(new ThreadStart(_thread_update))).Start();
        }

        private void _thread_update() {
            update();

            RunOnUiThread(() => {
                mSwipeRefreshLayout.Refreshing = false;
            });
        }

        protected void OnListItemClick(object sender, Android.Widget.AdapterView.ItemClickEventArgs e) {
            var listView = sender as ListView;
            var t = tableItems[e.Position];
            globals.compId = t.Id;

            StartActivity(typeof(Activitys.Activity2_contests));
        }
    }
}