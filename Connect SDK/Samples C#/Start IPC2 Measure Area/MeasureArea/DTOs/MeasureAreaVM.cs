using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;


namespace MeasureArea.DTOs
{
    public class MeasureAreaVM
    {
        public string Name { get; set; } 

        public int Shape { get; set; }

        public int Mode { get; set; }

        public bool BindToTempProfile { get; set; }

        public bool UseEmissivity { get; set; }

        public float Emissivity { get; set; }

        public bool ShowInDigDispGroup { get; set; }

        public float distMin { get; set; }

        public float distMax { get; set; }

        public POINT Location { get; set; }

        public SIZE Size { get; set; }

        public bool IsHotSpot { get; set; }

        public bool IsColdSpot { get; set; }

        #region MetaData
        
        public int Index { get; set; }
        public List<POINT> PolygonPoints { get; set; }

        #endregion

    }

   

   

}
