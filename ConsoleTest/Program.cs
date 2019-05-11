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
        struct SECURITY_ATTRIBUTES
        {
            public uint nLength;
            public IntPtr lpSecurityDescriptor;
            public int bInheritHandle;
        };

        [DllImport("Kernel32.dll", CharSet = CharSet.Auto)]
        private static extern SafeFileHandle CreateNamedPipe(
            string lpName,         // pointer to pipe name
            uint dwOpenMode,       // pipe open mode
            uint dwPipeMode,       // pipe-specific modes
            uint nMaxInstances,    // maximum number of instances
            uint nOutBufferSize,   // output buffer size, in bytes
            uint nInBufferSize,    // input buffer size, in bytes
            uint nDefaultTimeOut,  // time-out time, in milliseconds
            ref SECURITY_ATTRIBUTES lpSecurityAttributes  // pointer to security attributes
            );
        [DllImport("Advapi32.dll")]
        private static extern bool ConvertStringSecurityDescriptorToSecurityDescriptor(
           string StringSecurityDescriptor,
           uint StringSDRevision,
           out IntPtr SecurityDescriptor,
           IntPtr SecurityDescriptorSize
           );

        [DllImport("Kernel32.dll")]
        private static extern bool LocalFree(IntPtr ptr);

        private static bool CreatePipe(string pipeName, bool blockingPipe, ref SafeFileHandle pipeHandle, ref FileStream pipe)
        {
            SECURITY_ATTRIBUTES sa;
            sa.nLength = 12;
            sa.bInheritHandle = 0;
            if (!ConvertStringSecurityDescriptorToSecurityDescriptor("D: (A;OICI;GRGW;;;AU)", 1, out sa.lpSecurityDescriptor, IntPtr.Zero))
                return false;
            uint flags = 4 | 2 | 0;

            if (!blockingPipe)
                flags |= 1;
            pipeHandle = CreateNamedPipe(pipeName, 3, flags, 1, 512, 512, 1000, ref sa);
            LocalFree(sa.lpSecurityDescriptor);
            if (pipeHandle.IsInvalid)
                return false;
            pipe = new FileStream(pipeHandle, FileAccess.ReadWrite, 512, false);
            return true;
        }

        [DllImport("Kernel32.dll")]
        private static extern bool ConnectNamedPipe(
          SafeFileHandle hNamedPipe,  // handle to named pipe to connect
          IntPtr lpOverlapped         // pointer to overlapped structure
          );

        static void Main(string[] args)
        {
            try
            {
                //SafeFileHandle loggingPipeHandle = null;
                //FileStream loggingPipe = null;

                //CreatePipe(@"\\.\pipe\OMV_LOGGING_PIPE", false, ref loggingPipeHandle, ref loggingPipe);

                ProcessStartInfo processStartInfo = new ProcessStartInfo(@"C:\Users\Jon\Documents\Visual Studio 2017\Projects\DummyStats\DummyStats\bin\Debug\DummyStats.exe");

                processStartInfo.EnvironmentVariables["Cor_Enable_Profiling"] = "1";
                processStartInfo.EnvironmentVariables["COR_PROFILER"] = "{D1EEF7F2-7F41-4C97-98B1-8E483D0CE3E6}";
                processStartInfo.EnvironmentVariables["COR_PROFILER_PATH"] = @"C:\Users\Jon\Documents\Visual Studio 2017\Projects\CallProfiler\x64\Debug\CallProfiler.dll";

                //processStartInfo.EnvironmentVariables["OMV_USAGE"] = "trace";
                //processStartInfo.EnvironmentVariables["OMV_SKIP"] = "0";
                //processStartInfo.EnvironmentVariables["OMV_PATH"] = @"E:\TempData";
                //processStartInfo.EnvironmentVariables["OMV_STACK"] = "1";
                //processStartInfo.EnvironmentVariables["OMV_FORMAT"] = "v2";
                //processStartInfo.EnvironmentVariables["OMV_DynamicObjectTracking"] = "0x1";
                ////  processStartInfo.EnvironmentVariables["OMV_FORCE_GC_ON_COMMENT"] = gcOnLogFileComments ? "1" : "0";
                //processStartInfo.EnvironmentVariables["OMV_INITIAL_SETTING"] = "0x3";
                //processStartInfo.EnvironmentVariables["OMV_TargetCLRVersion"] = "v4";
                processStartInfo.UseShellExecute = false;

                processStartInfo.RedirectStandardOutput = true;


                


                Process profiledProcess = new Process();
                profiledProcess.StartInfo = processStartInfo;
                //profiledProcess.StartInfo.RedirectStandardOutput = true;
                profiledProcess.OutputDataReceived += ProfiledProcess_OutputDataReceived1;
                profiledProcess.Start();
                profiledProcess.BeginOutputReadLine();
                

                //ConnectNamedPipe(loggingPipeHandle, IntPtr.Zero);

                //const int maxloggingBufferSize = 512;
                //byte[] loggingBuffer = new byte[maxloggingBufferSize];
                //int loggingReadBytes = 0;

                while (!profiledProcess.HasExited)
                {
                    
                    //try
                    //{
                    //    loggingReadBytes += loggingPipe.Read(loggingBuffer, loggingReadBytes, maxloggingBufferSize - loggingReadBytes);
                    //    Console.WriteLine("Read: " + loggingReadBytes);
                    //}
                    //catch (System.IO.IOException ioe)
                    //{
                    //    Console.WriteLine("failure reading log:" + ioe.ToString());
                    //}

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
