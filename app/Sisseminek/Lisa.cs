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
    [Activity(Label = "Lisa")]
    public class Lisa : BaseAdapter<TableItem>
    {
        List<TableItem> items;
        Activity context;


        public Lisa(Activity context, List<TableItem> items) : base()
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
                view = context.LayoutInflater.Inflate(Resource.Layout.lisainfo, null);
            view.FindViewById<TextView>(Resource.Id.Text1).Text = item.Koht;
            view.FindViewById<TextView>(Resource.Id.Text2).Text = item.Nimi;
            //view.FindViewById<TextView>(Resource.Id.Text3).Text = item.Koht;
            view.FindViewById<TextView>(Resource.Id.Text4).Text = item.Klubi;
            view.FindViewById<TextView>(Resource.Id.Text5).Text = item.Aeg1;
            view.FindViewById<TextView>(Resource.Id.Text6).Text = item.Aeg2;
            view.FindViewById<TextView>(Resource.Id.Text7).Text = item.Aeg3;
            view.FindViewById<TextView>(Resource.Id.Text8).Text = item.Aeg4;
            view.FindViewById<TextView>(Resource.Id.Text9).Text = item.Aeg5;
            view.FindViewById<TextView>(Resource.Id.Text10).Text = item.Aeg6;
            view.FindViewById<TextView>(Resource.Id.Text11).Text = item.Aeg;
            // view.FindViewById<ImageView>(Resource.Id.image).SetImageResource(item.ImageResourceId);
            return view;
        }

    }
}