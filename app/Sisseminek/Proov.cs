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
    [Activity(Label = "")]
    public class Proov : BaseAdapter<TableItem>
    {
        List<TableItem> items;
        Activity context;


        public Proov(Activity context, List<TableItem> items) : base()
        {
            this.context = context;
            this.items = items;
        }
        public override TableItem this[int position]
        {
            get
            {
                return items[position];
            }
        }
        public override long GetItemId(int position)
        {
            return position;
        }


        public override int Count
        {
            get { return items.Count; }
        }
        public override View GetView(int position, View convertView, ViewGroup parent)
        {
            var item = items[position];
            View view = convertView; // re-use an existing view, if one is available
            if (view == null) // otherwise create a new one
                view = context.LayoutInflater.Inflate(Resource.Layout.proov, null);
            view.FindViewById<TextView>(Resource.Id.Text1).Text = item.Nimi;
            view.FindViewById<TextView>(Resource.Id.Text2).Text = item.Klubi;
            //view.FindViewById<TextView>(Resource.Id.Text3).Text = item.Koht;
            view.FindViewById<TextView>(Resource.Id.Text4).Text = item.Aeg;
            view.FindViewById<TextView>(Resource.Id.Text5).Text = item.Koht;
            //view.FindViewById<ImageView>(Resource.Id.image).SetImageResource(item.ImageResourceId);
            return view;
        }
       
    }
}