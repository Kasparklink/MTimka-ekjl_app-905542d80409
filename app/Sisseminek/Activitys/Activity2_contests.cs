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
using Android.Support.V7.App;
using System.Text.RegularExpressions;
using System.ComponentModel;
using Android.Support.V4.Widget;
using Java.Lang;

namespace Sisseminek.Activitys {

    [Activity(Label = "")]
    public class Activity2_contests : ActionBarActivity {
        ExpandableListAdapter mAdapter;
        ExpandableListView expandableListView;
        SwipeRefreshLayout mSwipeRefreshLayout;
        List<string> group = new List<string>();
        Dictionary<string, List<string>> dicMyMap = new Dictionary<string, List<string>>();
        JsonArray heats;

        protected override void OnCreate(Bundle bundle) {
            base.OnCreate(bundle);

            SetContentView(Resource.Layout.layout2_contests);


            expandableListView = FindViewById<ExpandableListView>(Resource.Id.expandableListView);

            //Set Data
            SetData(out mAdapter);
            expandableListView.SetAdapter(mAdapter);

            expandableListView.ChildClick += (s, e) => {
                globals.heatId = get_group_children_id(e.GroupPosition, e.ChildPosition, out globals.heatName);
                StartActivity(typeof(Activity3_contest_results));
            };
        }


        private string[] get_timetable_groups() {
            List<string> groups = new List<string>();

            for (int i = 0; i < heats.Count; i++) {
                bool group_exists = false;
                var item = (JsonObject)heats[i];

                for (int j = 0; j < groups.Count; j++) {
                    if (groups[j] == ((string)item["starting_time"]).Split(' ')[0]) {
                        group_exists = true;
                        break;
                    }
                }

                if (group_exists == false) {
                    groups.Add(((string)item["starting_time"]).Split(' ')[0]);
                }
            }

            return groups.ToArray();
        }

        private string[] get_group_children(string group) {
            List<string> children = new List<string>();

            for (int i = 0; i < heats.Count; i++) {
                var item = (JsonObject)heats[i];

                if (group == ((string)item["starting_time"]).Split(' ')[0])
                    children.Add((string)item["long_name"]);
            }

            return children.ToArray();
        }

        private string get_group_children_id(int group_index, int index, out string name) {
            int indexx = 0;
            string group = get_timetable_groups()[group_index];

            for (int i = 0; i < heats.Count; i++) {
                var item = (JsonObject)heats[i];

                if (group == ((string)item["starting_time"]).Split(' ')[0]) {
                    if (indexx == index) {
                        name = (string)item["long_name"];
                        return (string)item["id"];
                    }
                    indexx++;
                }
            }

            name = "";
            return "";
        }

        private void SetData(out ExpandableListAdapter mAdapter) {
            client.SendString("SZ_GET_TIMETABLE=" + globals.compId);
            string res = client.ReceiveResponse();

            var json = JsonObject.Parse(res);
            heats = (JsonArray)json["items"];

            string[] groups = get_timetable_groups();

            for (int i = 0; i < groups.Length; i++) {
                string[] groupA = get_group_children(groups[i]);

                group.Add(groups[i]);
                dicMyMap.Add(group[i], groupA.ToList());
            }

            mAdapter = new ExpandableListAdapter(this, group, dicMyMap);
        }
    }
}