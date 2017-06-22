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

namespace Sisseminek {
    class funcs {
        public static double convertPixelsToDp(Activity a, float px) {
	        double dp = px / ((double)a.Resources.DisplayMetrics.DensityDpi / 160.0);
	        return Math.Round(dp);
        }
    }
}