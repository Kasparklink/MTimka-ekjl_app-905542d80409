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

namespace Sisseminek.Adapters {
    public class Competitions : BaseAdapter<Items.Competition>
    {
        List<Items.Competition> items;
        Activity context;


        public Competitions(Activity context, List<Items.Competition> items) : base() {
            this.context = context;
            this.items = items;
        }

        public override Items.Competition this[int position] {
            get { return items[position]; }
        }

        public override long GetItemId(int position) {
            return position;
        }

        public override int Count {
            get { return items.Count; }
        }

        public override View GetView(int position, View convertView, ViewGroup parent) {
            // variables this function uses
            View view;
            Items.Competition item;

            // fill variables
            item = items[position];
            view = convertView;

            if (view == null)
                view = context.LayoutInflater.Inflate(Resource.Layout.MainDisain, null);


            view.FindViewById<TextView>(Resource.Id.Text1).Text = item.Title;

            return view;
        }

    }
}