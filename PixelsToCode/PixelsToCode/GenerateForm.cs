using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace PixelsToCode
{
    public partial class GenerateForm : Form
    {
        protected bool validData;

        string path;
        protected Image image;
        protected Thread getImageThread;

        public Color[] Colors { get; private set; } = new Color[24 * 24];

        public GenerateForm()
        {
            InitializeComponent();
            pictureBox1.AllowDrop = true;
        }

        private void pictureBox1_DragEnter( object sender, DragEventArgs e )
        {
            string filename;
            validData = GetFilename( out filename, e );
            if( validData )
            {
                path = filename;
                getImageThread = new Thread( new ThreadStart( LoadImage ) );
                getImageThread.Start();
                e.Effect = DragDropEffects.Copy;
            }
            else
                e.Effect = DragDropEffects.None;
        }

        private void pictureBox1_DragDrop( object sender, DragEventArgs e )
        {
            if( validData )
            {
                while( getImageThread.IsAlive )
                {
                    Application.DoEvents();
                    Thread.Sleep( 0 );
                }
                pictureBox1.Image = image;
            }
        }

        private bool GetFilename( out string filename, DragEventArgs e )
        {
            bool ret = false;
            filename = String.Empty;
            if( ( e.AllowedEffect & DragDropEffects.Copy ) == DragDropEffects.Copy )
            {
                Array data = ((IDataObject)e.Data).GetData("FileDrop") as Array;
                if( data != null )
                {
                    if( ( data.Length == 1 ) && ( data.GetValue( 0 ) is String ) )
                    {
                        filename = ( ( string[] )data )[0];
                        string ext = Path.GetExtension(filename).ToLower();
                        if( ( ext == ".jpg" ) || ( ext == ".png" ) || ( ext == ".bmp" ) )
                        {
                            ret = true;
                        }
                    }
                }
            }
            return ret;
        }

        protected void LoadImage()
        {
            image = new Bitmap( path );
        }

        private void btnOK_Click( object sender, EventArgs e )
        {
            if(image != null)
            {
                double pixelSize = GetPixelSize();
                int minY = GetMinY();
                int minX = GetMinX();

                if(pixelSize != -1)
                {
                    double yStart = minY + (pixelSize / 2);
                    double xStart = minX + (pixelSize / 2);
                    Bitmap img = new Bitmap(image);

                    for( int y = 0; y < 24; y++ )
                    {
                        int sampleY = (int)(yStart + y * pixelSize);
                        for(int x = 0; x < 24; x++ )
                        {
                            int sampleX = (int)(xStart + x * pixelSize);

                            if(sampleX >= image.Width || sampleY >= image.Height ||
                                x < MinXIndex(pixelSize, xStart, yStart, y, img) ||
                                x > MaxXIndex( pixelSize, xStart, yStart, y, img ))
                            {
                                Colors[y * 24 + x] = Color.Black;
                            }
                            else
                            {
                                Colors[y * 24 + x] = img.GetPixel( sampleX, sampleY );
                            }
                        }
                    }
                }
            }
        }

        private int MinXIndex(double pixelSize, double xStart, double yStart, int y, Bitmap img)
        {
            for(int x = 0; x < 24; x++ )
            {
                int sampleX = (int)(xStart + x * pixelSize);
                int sampleY = (int)(yStart + y * pixelSize);

                if(sampleX < img.Width && sampleY < img.Height)
                {
                    Color c = img.GetPixel(sampleX, sampleY);
                    if( c.ToArgb() != Color.White.ToArgb() && c.ToArgb() != Color.Black.ToArgb() )
                    {
                        return x;
                    }
                }
            }

            return -1;
        }

        private int MaxXIndex( double pixelSize, double xStart, double yStart, int y, Bitmap img )
        {
            for( int x = 23; x >= 0; x-- )
            {
                int sampleX = (int)(xStart + x * pixelSize);
                int sampleY = (int)(yStart + y * pixelSize);

                if( sampleX < img.Width && sampleY < img.Height )
                {
                    Color c = img.GetPixel(sampleX, sampleY);
                    if( c.ToArgb() != Color.White.ToArgb() && c.ToArgb() != Color.Black.ToArgb() )
                    {
                        return x;
                    }
                }
            }

            return -1;
        }

        private int GetMinY()
        {
            Bitmap img = new Bitmap(image);

            for( int y = 0; y < image.Height; y++ )
            {
                for( int x = 0; x < image.Width; x++ )
                {
                    Color c = img.GetPixel(x, y);
                    if( c.ToArgb() != Color.White.ToArgb() && c.ToArgb() != Color.Black.ToArgb() )
                    {
                        return y;
                    }
                }
            }

            return -1;
        }

        private int GetMinX()
        {
            Bitmap img = new Bitmap(image);

            for( int x = 0; x < image.Width; x++ )
            {
                for( int y = 0; y < image.Height; y++ )
                {
                    Color c = img.GetPixel(x, y);
                    if( c.ToArgb() != Color.White.ToArgb() && c.ToArgb() != Color.Black.ToArgb() )
                    {
                        return x;
                    }
                }
            }

            return -1;
        }

        private int GetMaxY()
        {
            Bitmap img = new Bitmap(image);

            for( int y = image.Height - 1; y >= 0; y-- )
            {
                for( int x = 0; x < image.Width; x++ )
                {
                    Color c = img.GetPixel(x, y);
                    if( c.ToArgb() != Color.White.ToArgb() && c.ToArgb() != Color.Black.ToArgb() )
                    {
                        return y;
                    }
                }
            }

            return -1;
        }

        private int GetMaxX()
        {
            Bitmap img = new Bitmap(image);

            for( int x = image.Width - 1; x >= 0; x-- )
            {
                for( int y = 0; y < image.Height; y++ )
                {
                    Color c = img.GetPixel(x, y);
                    if( c.ToArgb() != Color.White.ToArgb() && c.ToArgb() != Color.Black.ToArgb() )
                    {
                        return x;
                    }
                }
            }

            return -1;
        }

        private double GetPixelSize()
        {
            Bitmap img = new Bitmap(image);

            int minY = GetMinY();
            int maxY = GetMaxY();

            if(minY == -1 || maxY == -1)
            {
                return -1;
            }

            return ( maxY - minY ) / (double)numPixels.Value;
        }
    }
}
