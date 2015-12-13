using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DictionaryConverter
{
    class Program
    {
        static void Main(string[] args)
        {
            var sourceFile = new System.IO.FileInfo(@"C:\Users\pasca\Desktop\voc-it.mdb");
            var stenoEntries = Converter.Parse(sourceFile);
            var filtered = stenoEntries.GroupBy(_s => _s.WinstenoStroke).Select(_g => _g.First());
            Console.WriteLine("Loaded {0} entries from {1}, {2} after duplicates removal.", stenoEntries.Count(), sourceFile.Name, filtered.Count());
            foreach (var duplicates in stenoEntries.GroupBy(_s => _s.WinstenoStroke).Where(_g => _g.Count() > 1))
            {
                Console.WriteLine("Duplicate : {0}", duplicates.Key);
                foreach(var dup in duplicates)
                {
                    Console.WriteLine("\t{0} - {1}", dup.Complete, dup.Partial);
                }
            }
            var ploverEntries = filtered.SelectMany(_se => PloverEntry.FromWinstenoEntry(_se)).Where(_pe=>_pe != null);
            var plovedFiltered = ploverEntries.GroupBy(_s => _s.Strokes.ToString()).Select(_g => _g.First());
           
            var outFile = new FileInfo(String.Format("{0}.json", Path.GetFileNameWithoutExtension(sourceFile.FullName)));
            outFile.Delete();
            using (var outstream = outFile.AppendText())
            {
                outstream.Write("{\n");
                foreach (var entry in plovedFiltered)
                {
                    outstream.Write("\"{0}\": \"{1}\", \n", entry.Strokes, entry.Value);
                }
                outstream.Write("\"\": \"\"\n");
                outstream.Write("}");
            }
            Console.WriteLine("Written {0} entries in {1}", plovedFiltered.Count(), outFile.FullName);
        }
    }
}
