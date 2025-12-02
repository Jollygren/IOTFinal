using HiveMQtt;
using HiveMQtt.Client;
using HiveMQtt.Client.Options;
using HiveMQtt.Client.Results;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Navigation;
using MQTTnet;
using MQTTnet.Internal;
using MQTTnet.Packets;
using System;
using System.Collections.Generic;
using System.Data;
using System.IO;
using System.Linq;
using System.Net;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Security;
using System.Security.Cryptography.X509Certificates;
using System.Text;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Foundation.Collections;
using static System.Net.Mime.MediaTypeNames;
using static System.Runtime.InteropServices.JavaScript.JSType;


// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace Control_Pannel
{
    /// <summary>
    /// An empty window that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainWindow : Window
    {
        private HiveMQClient MqttClient;
        private const string broker = "p07da41d.ala.us-east-1.emqxsl.com";
        private const int port = 8883;
        private string password = "2743";
        private const string username = "Jkaufman2743";
        private const string defaultSubscribtion = "SENG3030/";
        private const string certificate = @"-----BEGIN CERTIFICATE-----
MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD
QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j
b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG
9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB
CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97
nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt
43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P
T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4
gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO
BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR
TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw
DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr
hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg
06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF
PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls
YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk
CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=
-----END CERTIFICATE-----";

        private string userInput = "";
        private string topicPublish = "SENG3030/Thursday/Jkaufman2743/subscribe/";

        public MainWindow()
        {
            InitializeComponent();
            _ = ConnectToMqtt();
           
        }

        private async Task ConnectToMqtt()
        {

            SecureString securePassword = new NetworkCredential("", password).SecurePassword;
            X509Certificate2 certif = new X509Certificate2(Encoding.UTF8.GetBytes(certificate));

            var clientOptions = new HiveMQClientOptionsBuilder()
                .WithBroker(broker)
                .WithPort(port)
                .WithUserName(username)
                .WithPassword(securePassword)
                .WithUseTls(true)
                .WithClientCertificate(certif)
                .Build();

            MqttClient = new HiveMQClient(clientOptions);

            MqttClient.OnMessageReceived += MqttClient_OnMessageReceived;
            MqttClient.AfterConnect += MqttClient_AfterConnect;


           await MqttClient.ConnectAsync();

        }

       

       

        private void MqttClient_AfterDisconnect(object? sender, HiveMQtt.Client.Events.AfterDisconnectEventArgs e)
        {
            AddTextToConnectionStatus("DISCONNECTED");
        }

        private void MqttClient_AfterConnect(object? sender, HiveMQtt.Client.Events.AfterConnectEventArgs e)
        {
            AddTextToConnectionStatus("CONNECTED");
        }

        private void MqttClient_OnMessageReceived(object? sender, HiveMQtt.Client.Events.OnMessageReceivedEventArgs e)
        {
            var payload = e.PublishMessage;
            if (payload.Topic != null)
            {
                string message = payload.Topic.ToString() + " " + payload.PayloadAsString;
                AddTextToLogBox(message);
            }
        }

        private void SubscribeToDefualt(object sender, RoutedEventArgs e)
        {
           MqttClient.SubscribeAsync(defaultSubscribtion).ConfigureAwait(false);
            if(MqttClient.Subscriptions.Count() > 0)
            {
                AddTextToConnectionStatus("SUBSCRIBED");
            }
            
        }

        


       
        private void AddTextToConnectionStatus(string text)
        {
            this.DispatcherQueue.TryEnqueue(() =>
            {
                connectionStatus.Text = text;
            });
        }

        private void AddTextToLogBox(string text)
        {
            this.DispatcherQueue.TryEnqueue(() =>
            {
               logTextBox.Text = text;
            });
        }

        private void SubscribeToInputAsync()
        {
            
            this.DispatcherQueue.TryEnqueue(() =>
            {
               // userInput = subscribeInput.Text;
            });
            userInput = subscribeInput.Text;
            var result = MqttClient.SubscribeAsync(userInput).ConfigureAwait(false);

           
            if (MqttClient.Subscriptions.Count < 1)
            {
                AddTextToConnectionStatus("FAILED");
            }
            else
            {
                AddTextToConnectionStatus("SUCCESS");
                MqttClient.UnsubscribeAsync(userInput).ConfigureAwait(false);
                PublishTopicAsync();
            }
            
        }

        private void PublishTopicAsync()
        { 
            MqttClient.PublishAsync(topicPublish, userInput).ConfigureAwait(false);
           
        }

        private void SubscribeButton_Click(object sender, RoutedEventArgs e)
        {
           SubscribeToInputAsync();
        }
    }
}
