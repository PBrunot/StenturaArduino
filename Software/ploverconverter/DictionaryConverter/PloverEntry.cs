using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DictionaryConverter
{
    class PloverEntry
    {
        public StrokeList Strokes { get; set; }
        public string Value { get; set; }

        public static List<PloverEntry> FromWinstenoEntry(WinStenoEntry wse)
        {
            List<PloverEntry> entries = new List<PloverEntry>();
            
            if (!String.IsNullOrEmpty(wse.Complete))
            {
                PloverEntry pe = new PloverEntry();
                pe.Strokes = wse.PloverStrokes;
                pe.Value = ConvertValue(wse.Complete);
                entries.Add(pe);
            }
            if (!String.IsNullOrEmpty(wse.Partial))
            {
                PloverEntry pe = new PloverEntry();
                pe.Strokes = wse.PloverStrokes;
                pe.Strokes.Last().RightStroke.Add('&');
                pe.Value = ConvertValue(wse.Partial + "&rb;");
                entries.Add(pe);
            }
            return entries;
        }
        private static string ConvertValue(String value)
        {
            Dictionary<String, String> replacements = new Dictionary<string, string>();
            replacements.Add("&sp;", " ");
            replacements.Add("&1uc;", "{-|}");
            replacements.Add("&amp;", "&");
            replacements.Add("&bs;", "{# BackSpace}");
            replacements.Add("&cr;", "{# Return}");
            replacements.Add("&uc;", "{MODE:CAPS}");
            replacements.Add("&lc;", "{MODE:LOWER}");
            replacements.Add("&dw;", "{# Control_L(BackSpace)}");
            replacements.Add("\\", "\\\\");
            replacements.Add("\"", "\\\"");
            foreach (string key in replacements.Keys.OrderByDescending(_k=>_k.Length))
            {
                value = value.Replace(key, replacements[key]);
            }
            if (value.Contains("&+i;"))
            {
                value = String.Format("{0}{{^}}", value.Replace("&+i;", ""));
            }
            if (value.Contains("&rb;"))
            {
                value = String.Format("{{^}}{0}", value.Replace("&rb;", ""));
            }
            return value.Trim();
        }
    }
}

