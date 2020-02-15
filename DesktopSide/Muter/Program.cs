using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Muter
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main() {

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);


            Form1 form = new Form1();
            NetworkCommunication networkCommunication = new NetworkCommunication();
            networkCommunication.observer = form;

            Application.Run(form);
            networkCommunication.Stop = true;
        }   
    }
}
