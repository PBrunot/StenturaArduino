using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DictionaryConverter
{
    class WinStenoEntry
    {
        public string WinstenoStroke { get; set; }
        public string Complete { get; set; }
        public string Partial { get; set; }
        public bool CapsFlag { get; set; }

        /// <summary>
        /// Returns the english plover strokes, converting italian WinstenoKeystroke
        /// </summary>
        public StrokeList PloverStrokes { get { return StrokeConverter.Parse(WinstenoStroke); } }
    }
}
