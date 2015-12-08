using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Data;
using System.Data.OleDb;
using System.Globalization;

namespace DictionaryConverter
{
    class Converter
    {
        public static DataTable LoadMDB(FileInfo file, string tableName)
        {
            string myConnectionString = string.Format(@"Provider=Microsoft.Jet.OLEDB.4.0;" +
                               "Data Source={0};" +
                               "Persist Security Info=True;" +
                               "Jet OLEDB:Database Password=myPassword;", file.FullName);
            try
            {
                // Open OleDb Connection
                using (OleDbConnection myConnection = new OleDbConnection(myConnectionString))
                {
                    myConnection.Open();

                    // Execute Queries
                    using (OleDbCommand cmd = myConnection.CreateCommand())
                    {
                        cmd.CommandText = string.Format("SELECT * FROM `{0}`", tableName);
                        using (OleDbDataReader reader = cmd.ExecuteReader(CommandBehavior.CloseConnection))
                        {
                            // Load the result into a DataTable
                            DataTable myDataTable = new DataTable();
                            myDataTable.Load(reader);
                            return myDataTable;
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("OLEDB Connection FAILED: " + ex.Message);
                return null;
            }
        }

        
       
        public static IEnumerable<WinStenoEntry> Parse(FileInfo fi)
        {
            List<WinStenoEntry> output = new List<WinStenoEntry>();

            if (fi == null || !File.Exists(fi.FullName))
            {
                return output;
            }

            DataTable dt = LoadMDB(fi, "BatTab");

            foreach(var row in dt.AsEnumerable())
            {
                WinStenoEntry wse = new WinStenoEntry();
                wse.WinstenoStroke = row.Field<string>("Battuta");
                wse.Complete = row.Field<string>("Completa");
                wse.Partial = row.Field<string>("Semplice");
                wse.CapsFlag = row.Field<bool>("Completa_Iniziale");

                if (!String.IsNullOrEmpty(wse.Complete) || !String.IsNullOrEmpty(wse.Partial))
                {
                    output.Add(wse);
                }
            }

            return output;
        }
    }
}
