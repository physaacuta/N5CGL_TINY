using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace DBAccessN5CGL_TINY
{
    public class EnlargeJpeg
    {
        volatile int QueueCount = 0;
        System.Threading.EventWaitHandle wh = new System.Threading.EventWaitHandle(false, System.Threading.EventResetMode.AutoReset);


        class argument
        {
            public byte[] jpeg;
            public int ratio;
           public Action<SwdData.DefectImage> action;
            public argument(byte[] jpeg, int ratio, Action<SwdData.DefectImage> action) {
                this.jpeg = jpeg;
                this.ratio = ratio;
                this.action = action;
            }
        }

        public void WaitCompleteAll()
        {
            while (QueueCount != 0)
            {
                wh.WaitOne();
            }
        }

        public void Enque(byte[] jpeg, int ratio, Action<SwdData.DefectImage> a)
        {
            while (QueueCount > 10)
            {
                wh.WaitOne();
            }
            System.Threading.Interlocked.Increment(ref QueueCount);
            System.Threading.ThreadPool.QueueUserWorkItem(doit_async, new argument(jpeg, ratio, a));
        }

        private void doit_async(object state)
        {
            argument a = (argument)state;
            try
            {
                a.action(doit(a.jpeg, a.ratio));
            }
            finally
            {
                System.Threading.Interlocked.Decrement(ref QueueCount);
                wh.Set();
            }
        }

        static public SwdData.DefectImage doit(byte[] jpeg, int largeratio)
        {
            MemoryStream ms_src = new MemoryStream(jpeg, false);

            JpegBitmapDecoder decoder = new JpegBitmapDecoder(
                 ms_src, BitmapCreateOptions.IgnoreColorProfile, BitmapCacheOption.OnLoad);
            TransformedBitmap trans = new TransformedBitmap(decoder.Frames[0], new ScaleTransform(largeratio, largeratio));

            JpegBitmapEncoder encoder = new JpegBitmapEncoder();
            encoder.QualityLevel = 80;
            encoder.Frames.Add( BitmapFrame.Create(trans) );

            MemoryStream ms_dst = new MemoryStream();
            encoder.Save(ms_dst);
            ms_dst.Close();
            return new SwdData.DefectImage(ms_dst.ToArray());
        }
    }
}
