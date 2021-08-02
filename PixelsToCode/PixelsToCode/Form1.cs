using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Windows.Forms;

namespace PixelsToCode
{
    public partial class Form1 : Form
    {
        List<Button> cells = new List<Button>();
        int gridDim = 24;
        GenerateForm generateForm = new GenerateForm();
        MergeForm mergeForm = new MergeForm();
        public Form1()
        {
            InitializeComponent();
            CreateGrid();
        }

        private void CreateGrid()
        {
            int offset = 25;
            int btnDim = 25;

            for( int y = 0; y < gridDim; y++ )
            {
                for( int x = 0; x < gridDim; x++ )
                {
                    Button btn = new Button();
                    btn.Width = btnDim;
                    btn.Height = btnDim;
                    btn.FlatStyle = FlatStyle.Flat;
                    btn.Location = new Point(offset + x * btnDim, offset + y * btnDim);
                    btn.BackColor = Color.Black;
                    btn.ForeColor = Color.DarkGray;
                    btn.Click += ColorCellClick;
                    Controls.Add( btn );
                    cells.Add( btn );
                }
            }
        }

        private void btnExport_Click( object sender, EventArgs e )
        {
            SaveFileDialog dialog = new SaveFileDialog();
            dialog.DefaultExt = "txt";

            if( dialog.ShowDialog() == DialogResult.OK )
            {
                Save(dialog.FileName);
            }
        }

        private void btnColor_Click( object sender, EventArgs e )
        {
            ColorDialog dialog = new ColorDialog();
            dialog.Color = btnColor.BackColor;

            if( dialog.ShowDialog() == DialogResult.OK )
                btnColor.BackColor = dialog.Color;
        }

        private void Save(string fileName)
        {
            string text = "const long Frame[] PROGMEM =\n" +
                "{\n";

            for( int i = 0; i < gridDim; i++ )
            {
                for( int j = 0; j < gridDim; j++ )
                {
                    int index = i * gridDim + j;
                    Color c = cells[index].BackColor;

                    text += "0x" + c.R.ToString( "X2" ) + 
                        c.G.ToString( "X2" ) + 
                        c.B.ToString( "X2" );

                    if( index < (cells.Count - 1) )
                    {
                        text += ",";
                    }
                }
                
                text += "\n";
            }

            text += "};";

            using( var writer = new StreamWriter( fileName ) )
            {
                writer.Write( text );
            }
        }

        private void ColorCellClick( object sender, EventArgs e )
        {
            ( sender as Button ).BackColor = btnColor.BackColor;
        }

        private void btnLoad_Click( object sender, EventArgs e )
        {
            OpenFileDialog dialog = new OpenFileDialog();

            if( dialog.ShowDialog() == DialogResult.OK )
            {
                LoadFile( dialog.FileName );
            }
        }

        private void LoadFile(string fileName)
        {
            var rgbText = File.ReadAllText(fileName).Split('{')[1].Split('}')[0].Split(',');

            for( int i = 0; i < cells.Count; i++ )
            {
                Color c = ColorTranslator.FromHtml( rgbText[i].Replace( "0x", "#" ) );
                cells[i].BackColor = c;
            }
        }

        private void btnGenerate_Click( object sender, EventArgs e )
        {
            if( generateForm.ShowDialog() == DialogResult.OK )
            { 
                for(int i = 0; i < generateForm.Colors.Length; i++ )
                {
                    cells[i].BackColor = generateForm.Colors[i];
                }
            }
        }

        private void btnUp_Click( object sender, EventArgs e )
        {
            for( int i = 24; i < cells.Count; i++ )
            {
                cells[i - 24].BackColor = cells[i].BackColor;
            }
            for(int i = cells.Count - 24; i < cells.Count; i++ )
            {
                cells[i].BackColor = Color.Black;
            }
        }

        private void btnLeft_Click( object sender, EventArgs e )
        {
            for(int i = 0; i < cells.Count - 1; i++ )
            {
                cells[i].BackColor = cells[i + 1].BackColor;
            }

            cells[cells.Count - 1].BackColor = Color.Black;
        }

        private void btnDown_Click( object sender, EventArgs e )
        {
            for( int i = cells.Count - 1; i > 23; i-- )
            {
                cells[i].BackColor = cells[i - 24].BackColor;
            }

            for( int i = 0; i < 24; i++ )
            {
                cells[i].BackColor = Color.Black;
            }
        }

        private void btnRight_Click( object sender, EventArgs e )
        {
            for( int i = cells.Count - 1; i > 0; i-- )
            {
                cells[i].BackColor = cells[i - 1].BackColor;
            }

            cells[0].BackColor = Color.Black;
        }

        private void numericUpDown1_ValueChanged( object sender, EventArgs e )
        {
            generateForm.xOffset = (int)numericUpDown1.Value;
            generateForm.GenerateSprite();

            for( int i = 0; i < generateForm.Colors.Length; i++ )
            {
                cells[i].BackColor = generateForm.Colors[i];
            }
        }

        private void numericUpDown2_ValueChanged( object sender, EventArgs e )
        {
            generateForm.yOffset = ( int )numericUpDown2.Value;
            generateForm.GenerateSprite();

            for( int i = 0; i < generateForm.Colors.Length; i++ )
            {
                cells[i].BackColor = generateForm.Colors[i];
            }
        }

        private void button1_Click( object sender, EventArgs e )
        {
            if( mergeForm.ShowDialog() == DialogResult.OK )
            {
                string output = "const long " + mergeForm.AnimationName + "[] PROGMEM =\n{\n";
                for(int i = 0; i < mergeForm.fileNames.Count; i++ )
                {
                    var content = File.ReadAllLines(mergeForm.fileNames[i]).ToList();
                    for( int j = 2; j < content.Count - 1; j++ )
                    {
                        output += content[j];
                        if(j < content.Count - 2)
                        {
                            output += "\n";
                        }
                    }

                    if(i < mergeForm.fileNames.Count - 1)
                    {
                        output += ",\n\n";
                    }
                    else
                    {
                        output += "\n";
                    }
                }
                output += "};";

                if(output.Length > 0)
                {
                    string path = Path.GetDirectoryName( mergeForm.fileNames[0] );
                    File.WriteAllText( path + "\\" + mergeForm.AnimationName + ".txt", output );
                }
            }
        }
    }
}
