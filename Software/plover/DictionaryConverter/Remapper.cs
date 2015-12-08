using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DictionaryConverter
{
    class Remapper
    {
        /// <summary>
        /// To get the mapping, I compared the layout here:
        /// https://github.com/Skaaal/StenturaArduino/blob/master/Stentura%20200%20SRT/Italian%20version/italian%20layout.JPG
        /// with the standard layout here:
        /// https://github.com/balthamos/steno-arduino/blob/master/steno-arduino.ino
        /// </summary>
        static List<char> ENGLISH_L = new List<char>() { 'S', 'T', 'K', 'P', 'W', 'H', 'R', 'A', 'O', '*', 'E', 'U'  };
        static List<char> ITALIAN_L = new List<char>() { 'S', 'P', 'C', 'T', 'H', 'V', 'R', 'I', 'A', '*', 'E', 'O' };

        static List<char> ENGLISH_R = new List<char>() { 'F', 'R', 'P', 'B', 'L', 'G', 'T', 'S', 'D', 'Z', '#' };
        static List<char> ITALIAN_R = new List<char>() { 'C', 'S', 'T', 'H', 'P', 'R', 'I', 'E', 'A', 'O', '$' };

        /// <summary>
        /// Translate a single italian stenokey to the corresponding english stenokey
        /// </summary>
        /// <param name="italian"></param>
        /// <returns>English character</returns>
        public static char RemapLeft(char italian)
        {
            int idx = ITALIAN_L.IndexOf(italian);
            if (idx < 0)
            {
                throw new InvalidOperationException(string.Format("Invalid character in italian file, left stroke : {0}", italian));
            }
            return ENGLISH_L[idx];
        }
        /// <summary>
        /// Translate a single italian stenokey to the corresponding english stenokey
        /// </summary>
        /// <param name="italian"></param>
        /// <returns>English characteur</returns>
        public static char RemapRight(char italian)
        {
            int idx = ITALIAN_R.IndexOf(italian);
            if (idx < 0)
            {
                throw new InvalidOperationException(string.Format("Invalid character in italian file, right stroke : {0}", italian));
            }
            return ENGLISH_R[idx];
        }
    }
}
