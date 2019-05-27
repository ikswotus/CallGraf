using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Text.RegularExpressions;

namespace LogReader
{
    class Program
    {
        public static Regex _logRegex = new Regex(@"(?<func_dir>[\w\W]+?)\s(?<ticks>\d{1,})\s(?<thread_id>\d{1,})\s(?<function>[\w\W]+)", RegexOptions.Compiled | RegexOptions.IgnoreCase);


        public class ShadowStack
        {
            public ShadowStack()
            {
                CallStack = new Stack<TimingInfo>(20);
            }
            public Stack<TimingInfo> CallStack {get;set;}
        }
           
        public class TimingInfo
        {
            public string Name { get; set; }
            public ulong EnterTicks { get; set; }
            public ulong ExitTicks { get; set; }
        }


        static void Main(string[] args)
        {
            try
            {
                string[] lines = File.ReadAllLines(@"E:\Code\test.txt");


                Dictionary<ulong, ShadowStack> stax = new Dictionary<ulong, ShadowStack>();

                Dictionary<ulong, Dictionary<string, List<TimingInfo>>> completedByThread = new Dictionary<ulong, Dictionary<string, List<TimingInfo>>>();

                // Dictionary<string, List<TimingInfo>> completed = new Dictionary<string, List<TimingInfo>>();
                // List<TimingInfo> completed = new List<TimingInfo>();

                foreach (string line in lines)
                {
                    Match m = _logRegex.Match(line);
                    if (m.Success)
                    {
                        ulong tid = ulong.Parse(m.Groups["thread_id"].Value);
                        if (!stax.ContainsKey(tid))
                        {
                            stax[tid] = new ShadowStack();
                        }
                        string method = m.Groups["func_dir"].Value;
                        ulong ticks = ulong.Parse(m.Groups["ticks"].Value);
                        string fname = m.Groups["function"].Value;
                        if (String.Equals(method, "ENTERMethod", StringComparison.OrdinalIgnoreCase))
                        {
                            stax[tid].CallStack.Push(new TimingInfo() { EnterTicks = ticks, Name = fname });
                        }
                        else // assume leave/tail since those are the only 2 options...
                        {
                            if (stax[tid].CallStack.Count == 0)
                            {
                                Console.WriteLine("Invalid stack depth for thread id: " + tid);
                            }
                            else if (!string.Equals(stax[tid].CallStack.Peek().Name, fname, StringComparison.OrdinalIgnoreCase))
                            {
                                Console.WriteLine("Invalid stack for thread id: " + tid);
                            }
                            else
                            {
                                TimingInfo paired = stax[tid].CallStack.Pop();
                                paired.ExitTicks = ticks;

                                if (!completedByThread.ContainsKey(tid))
                                    completedByThread[tid] = new Dictionary<string, List<TimingInfo>>();
                                Dictionary<string, List<TimingInfo>> completed = completedByThread[tid];
                                if (!completed.ContainsKey(fname))
                                    completed[fname] = new List<TimingInfo>();
                                completed[fname].Add(paired);
                            }
                        }
                    }
                }

                foreach (ulong tid in completedByThread.Keys)
                {
                    Dictionary<string, List<TimingInfo>> completed = completedByThread[tid];

                    foreach (string key in completed.Keys)
                    {
                        float avg = 0;
                        List<float> points = new List<float>();
                        foreach (TimingInfo ti in completed[key])
                        {
                            points.Add(ti.ExitTicks - ti.EnterTicks);
                        }
                        if (points.Count > 0)
                        {
                            float sum = points.Sum(i => i);
                            if (sum > 0)
                            {
                                avg = sum / points.Count();
                                Console.WriteLine("{2} {0}::{1}", key, avg, tid);
                            }
                        }


                        // Console.WriteLine("Tracking {0} values for key {1}", completed[key].Count ,key );
                    }
                }
            }
            catch(Exception exc)
            {
                Console.WriteLine("Failure: " + exc);
                Console.ReadKey();
            }
            Console.ReadKey();

        }
    }
}
