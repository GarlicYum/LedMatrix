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
    public partial class MergeForm : Form
    {
        public List<string> fileNames 
        {
            get
            {
                List<string> items = new List<string>();
                foreach(string item in listBox1.Items)
                {
                    items.Add( item );
                }

                return items;
            }
        }

        public string AnimationName { get { return textBox1.Text; } }

        public MergeForm()
        {
            InitializeComponent();
            listBox1.AllowDrop = true;
        }

        private void listBox1_DragEnter( object sender, DragEventArgs e )
        {
            if( DragContainsValidData( e ) )
            {
                e.Effect = DragDropEffects.Copy;
            }
            else
            { 
                e.Effect = DragDropEffects.None;
            }
        }

        private void listBox1_DragDrop( object sender, DragEventArgs e )
        {
            if(DragContainsValidData(e))
            {
                listBox1.Items.Clear();
                GetFilenames( e );
            }
        }

        private void GetFilenames( DragEventArgs e )
        {
            if( ( e.AllowedEffect & DragDropEffects.Copy ) == DragDropEffects.Copy )
            {
                Array data = ((IDataObject)e.Data).GetData("FileDrop") as Array;
                if( data != null )
                {
                    List<string> files = new List<string>();
                    for( int i = 0; i < data.Length; i++ )
                    { 
                        if(data.GetValue(i) is String)
                        {
                            string filename = ( ( string[] )data )[i];
                            if ( Path.GetExtension( filename ).ToLower() == ".txt")
                            {
                                files.Add(filename);
                            }
                        }
                    }

                    int maxlen = files.Max(x => x.Length);
                    files = files.OrderBy( x => x.PadLeft( maxlen, '0' ) ).ToList();
                    listBox1.Items.Clear();
                    foreach(string file in files)
                    {
                        listBox1.Items.Add( file );
                    }
                }
            }
        }

        private bool DragContainsValidData( DragEventArgs e )
        {
            if( ( e.AllowedEffect & DragDropEffects.Copy ) == DragDropEffects.Copy )
            {
                Array data = ((IDataObject)e.Data).GetData("FileDrop") as Array;
                if( data != null )
                {

                    for( int i = 0; i < data.Length; i++ )
                    {
                        if( data.GetValue( i ) is String )
                        {
                            string filename = ( ( string[] )data )[0];
                            if( Path.GetExtension( filename ).ToLower() == ".txt" )
                            {
                                return true;
                            }
                        }
                    }
                }
            }

            return false;
        }
    }
}
