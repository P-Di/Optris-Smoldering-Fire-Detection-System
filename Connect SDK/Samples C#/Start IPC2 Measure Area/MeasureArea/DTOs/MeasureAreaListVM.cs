using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;


namespace MeasureArea.DTOs
{
    public class MeasureAreaListVM
    {
        public MeasureAreaListVM()
        {
            Areas = new List<MeasureAreaVM>();
        }
        public List<MeasureAreaVM> Areas { get; set; }


    }
}
