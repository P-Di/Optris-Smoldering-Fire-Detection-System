using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace MeasureArea.DTOs
{
    public class POINT
    {
        public Int32 x;
        public Int32 y;

        #region Metadata
        public bool Ischanged=false;  // detection for need update point or not!
        #endregion

    }
}
