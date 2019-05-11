using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

using System.IO;
using System.Diagnostics;
using Microsoft.Win32.SafeHandles;
namespace ConsoleTest
{
    class Program
    {
       

        static void Main(string[] args)
        {
            try
            {
               
                ProcessStartInfo processStartInfo = new ProcessStartInfo(@"C:\Users\Jon\Documents\Visual Studio 2017\Projects\DummyStats\DummyStats\bin\Debug\DummyStats.exe");

                processStartInfo.EnvironmentVariables["Cor_Enable_Profiling"] = "1";
                

                processStartInfo.EnvironmentVariables["COR_PROFILER"] = "{D1EEF7F2-7F41-4C97-98B1-8E483D0CE3E6}";
                processStartInfo.EnvironmentVariables["COR_PROFILER_PATH"] = @"G:\CodeLaptop\CallProfiler\x64\Debug\CallProfiler.dll";
                
                processStartInfo.UseShellExecute = false;

                processStartInfo.RedirectStandardOutput = true;

            
                Process profiledProcess = new Process();
                profiledProcess.StartInfo = processStartInfo;
                
                profiledProcess.OutputDataReceived += ProfiledProcess_OutputDataReceived1;
                profiledProcess.Start();
                profiledProcess.BeginOutputReadLine();
                
                
                while (!profiledProcess.HasExited)
                {
                    System.Threading.Thread.Sleep(1000);
                }
                Console.WriteLine("ProfiledProcess exited");
                Console.ReadKey();

            }
            catch (Exception exc)
            {
                Console.WriteLine("Failed:" + exc);
            }
            Console.WriteLine("Finished");
            Console.ReadKey();
        }

        private static void ProfiledProcess_OutputDataReceived1(object sender, DataReceivedEventArgs e)
        {
            Console.WriteLine("StandardOutput:: " + e.Data);
        }        

        private static void ProfiledProcess_OutputDataReceived(object sender, DataReceivedEventArgs e)
        {
            Console.WriteLine("StandardOutput:: " + e.Data);
        }
    }
}
