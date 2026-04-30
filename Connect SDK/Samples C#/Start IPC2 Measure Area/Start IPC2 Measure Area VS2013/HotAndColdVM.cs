using MeasureArea.DTOs;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MeasureArea
{
    public class HotAndColdVM
    {
        public HotAndColdVM()
        {
            HotAreaIndex = ColdAreaIndex = null;
        }
        //public int HotAreaIndex { get; set; }
        //public int ColdAreaIndex { get; set; }

        public MeasureAreaVM HotAreaIndex { get; set; }
        public MeasureAreaVM ColdAreaIndex { get; set; }
    }
}
