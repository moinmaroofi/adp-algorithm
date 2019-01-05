using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.Timers;
using System.Diagnostics;

namespace adp_algorithm
{
    class Program
    {
        static Stopwatch stw = new Stopwatch();
        static int n;          //number of users
        static double n0;       //noise
        static double[,] h;     //gain
        static double[] y;       //gama
        static double pmax;    //maximum of p for each transmiter
        static double[] p;       //power of transmiters
        static double[] pr;  //price
        static double[] u;      //utilization function
         const double B = 128;//bandwidth
        static Thread[] t;
        static double[] converg; //convergense check
        static Random r = new Random();
        const double threshold = 0.0001;



        static void Main(string[] args)
        {
            Console.WriteLine("\n*****************************************************");
            Init();
            YInit();
            Console.WriteLine("\n\npress any key to start adp.......");
            Console.Read();
            stw.Start();
            
            Despatch();
            Console.ReadLine();
            Console.ReadLine();
        }
        static void Init()        /*Initialization*/
        {
            //get number of nodes and noise ammaunt from user

            Console.Write( "enter the number of nodes : ");
            n = int.Parse(Console.ReadLine());
            Console.Write("enter the amount of noise : ");
            n0 = double.Parse(Console.ReadLine());

            pmax = n0 * 10000; //   Pmax/n0 = 40db

            double[] transx = new double[n]; //transmitter position
            double[] transy = new double[n];
            double[] recx = new double[n];   //reciver position
            double[] recy = new double[n];

            p = new double[n];
            y = new double[n];
            u = new double[n];
            converg = new double[n];
            pr = new double[n];
            
            for (int i = 0; i < n; i++)
            {

                

                /*random positioning*/
                /*random positioning in (0,10) for transmiters*/
                transx[i] = r.NextDouble() * 10;
                transy[i] = r.NextDouble() * 10;

                /*random positioning for recivers
                        ((rand(0,1) - 0.5) * 6) => rand(-3,3) */

                recx[i] = transx[i] + ((r.NextDouble() - 0.5) * 6);
                recy[i] = transy[i] + ((r.NextDouble() - 0.5) * 6);

                /*position output*/
                Console.Write( "transmiter [" + i + "] position is : (" + transx[i] + " , " + transy[i]
                    + ")\n reciver [" + i + "] position is : (" + recx[i] + " , " + recy[i] + ") \n");

                //random price and power
                p[i] = r.NextDouble() * pmax;
                pr[i] = (1 / (n0 * B)) * r.NextDouble();
                //converge percent
                converg[i] = 1;
            }
            /*make 2d arrey h*/
            h = new double[n,n];
            //for (int i = 0; i < n; i++)
            //    h[i] = new double[n];

            for (int i = 0; i < n; i++)
            {
                for (int j = 0; j < n; j++)
                {

                    /*destanse of trasmiter i and reciver j*/
                    double d = Math.Pow((Math.Pow((transx[i] - recx[j]), 2) + Math.Pow((transy[i] - recy[j]), 2)), 0.5);
                    /*gain of trasmiter i and reciver j*/
                    h[i,j] = Math.Pow(d, -4);
                    //Console.WriteLine("h" + i + j + " = " + h[i, j]);
                }
            }
        }

        static void YInit()
        {
            
            for (int i = 0; i < n; i++)
            {
                y[i] = Y(i);
            }
        }
        static double Y(int i)
        {
            double sigI = 0;

            for (int j = 0; j < n; j++)
            {
                sigI += p[j] * h[j,i];
            }
            return ((p[i] * h[i,i]) / (n0 + (1 / B) * sigI));
        }
        static void AdpStart(int i)
        {
            double maxcon = 1;
            while (maxcon > threshold)
            {/*each thread (transmiter) wait for a random time between (0,1)second
                   (for simulate the real asynchronous nodes)*/
                Thread.Sleep((int)(r.NextDouble() * 1000));
                maxcon = Finish();

                
                

                //sum of price effect
                double sigPr = 0;

                //sum of Interference effect
                double sigI = 0;

                for (int j = 0; j < n; j++)
                {

                    if (i != j)
                    {
                        sigPr += pr[i] * h[j,i];
                        sigI += p[j] * h[i,j];
                    }

                }

                /*use this formula for update power for transmiter i
                           p = 1/sigma(price(i) * h(j,i)) */
                double tempP = 1 / sigPr;
                //Console.WriteLine("\nprocess " + i + " : local power = " + tempP);
                if (tempP <= pmax && tempP >= 0)
                {
                    //deferences between 2 power iteration
                    converg[i] = (Math.Abs(tempP - p[i])) / p[i];
                    p[i] = tempP;
                }
                else if (tempP > pmax)
                {
                    converg[i] = (Math.Abs(pmax - p[i])) / p[i];
                    p[i] = pmax;
                }

                else
                {
                    converg[i] = 1;
                    p[i] = 0;
                }


                /*use this formula for update price for transmiter i
                               price(i) = 1/(B*n0+sigma(p(j)*h(i,j)) */
                pr[i] = 1 / (B * n0 + sigI);
                

                // new gama calculation
                y[i] = Y(i);

                //new utilization calculation
                u[i] = Math.Log(y[i]);


            }


        }
        static void Out(){
	         double finutil;
	         while (true)
	         {
                Thread.Sleep(100);
                double util = 0;
                double maxcon = 0;
		         for (int i = 0; i<n; i++)
		         {
			         //calculate sum of utilization
			         util += u[i]; 
			         if (maxcon<converg[i])

                         maxcon = converg[i];
		         }
		         /*max deferences between powers is lower than threshold??*/
		         if (maxcon < threshold)
		         {
			         finutil = util;
                    stw.Stop();
			         break;
		         }

        Console.Write(  "\n u("+(stw.ElapsedMilliseconds)+") = " + util);
		         /*make output each 100ms*/
		         //std::this_thread::sleep_for(std::chrono::milliseconds(100));
	         }
	         Console.Write(  "\n\n\n\t\t\t    solution\n ==============================================================\n the final utilization ("+ (stw.ElapsedMilliseconds)+"ms) is : " + finutil + "\n\n p :");
	         for (int i = 0; i<n; i++)
	         {
		         Console.Write(  "\n p["+i+"] = " + p[i]);
	         }
	         Console.Write( "\n\n **************************finished****************************\n\n");
            Console.ReadLine();
        }
        static void Despatch()
        {
            //using std::thread;
            // make arrey of thread by size n (1 thread for each transmiter)
            t = new Thread[n];
            //one thread for make output
            Thread tn = new Thread(() => Program.Out());

            for (int i = 0; i < n; i++)
            {
                // define each thread to run adpStart()
                int x = i;
                t[i] = new Thread(() => Program.AdpStart(x));
            }
            tn.Start();
            //tn.Join();
            for (int i = 0; i < n; i++)
            {
                //start threads
                t[i].Start();
            }
            for (int i = 0; i < n; i++)
            {
                //start threads
                t[i].Join();
            }
            //if (tn.joinable())
            //start output thread

            Console.ReadLine();
        }

        static double Finish()
        {
            double maxcon = 0;
            for (int j = 0; j < n; j++)
            {

                //calculate sum of utilization
                if (maxcon < converg[j])

                    maxcon = converg[j];
            }
            /*max deferences between powers is lower than threshold??*/
            return maxcon;
        }
    }
}
