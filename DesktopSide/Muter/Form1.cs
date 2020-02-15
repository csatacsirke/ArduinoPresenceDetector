using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Muter
{
    public partial class Form1 : Form, INetworkCommunicationObserver
    {
        public Form1() {
            InitializeComponent();
        }

        public void OnMessage(string message) {
            if (!IsHandleCreated) {
                return;
            }
            Invoke(new Action(() => {
                string timestamp = DateTime.Now.ToString("[HH:mm:ss]");
                consoleTextBox.AppendText($"{timestamp} {message} {Environment.NewLine}");
            }));
        }

        public void OnError(string message) {
            if(!IsHandleCreated) {
                return;
            }
            Invoke(new Action(() => {
                string timestamp = DateTime.Now.ToString("[HH:mm:ss]");
                consoleTextBox.AppendText($"{timestamp} (!) {message} {Environment.NewLine}");
            }));
        }
    }
}
