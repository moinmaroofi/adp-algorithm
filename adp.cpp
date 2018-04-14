// adp.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <iostream>
#include <thread>
#include <stdio.h>
#include <math.h>
#include <cmath>
#include <conio.h>
#include <stdlib.h>
#include <chrono>

using namespace std;




class adp {
public:
	void Initialization();		
	void GamaInitialization();		
	double GamaCalculation(int);		
	double UtilizationCalculation(int);		
	void ThreadingNodes();	
	//void adpStart(int);
};


/*variables*/
int numberOfNodes;			
double noise;		 
double** Gain;		
double* gama;		 
double maxOfPower;	
double* currentPower;		 //current power of transmiters
double* price;	 
double* utilization;		
const double bandWidth  = 128;
thread* threads;
double* powerDifference; 
const double stopThreshold = 0.00001;




int main()
{
	adp a;
	a.Initialization();
	a.GamaInitialization();
	a.ThreadingNodes();

	getch();
    return 0;
}




 void adp::Initialization()		
{
	 
	 
	cout << "enter the number of nodesn : ";
	cin >> numberOfNodes;
	cout << "enter the amount of noise : ";
	cin >> noise;

	maxOfPower = noise* 10000; //  

	double* Xtransmiter = new double[numberOfNodes];	
	double* Ytransmiter = new double[numberOfNodes];
	double* Xreciver = new double[numberOfNodes];	
	double* Yreciver = new double[numberOfNodes];

	currentPower = new double[numberOfNodes]; 
	gama = new double[numberOfNodes];
	utilization = new double[numberOfNodes];
	powerDifference = new double[numberOfNodes];
	price = new double[numberOfNodes];
	
	for (int i = 0; i < numberOfNodes; i++) {

		/*((double)rand() / (RAND_MAX)) is a number between 0,1*/

	/*random positioning*/
		/*random positioning in (0,10) for transmiters*/
		Xtransmiter[i] = ((double)rand() / (RAND_MAX)) * 10; 
		Ytransmiter[i] = ((double)rand() / (RAND_MAX)) * 10;

		/*random positioning for recivers
				((rand(0,1) - 0.5) * 6) => rand(-3,3) */

		Xreciver[i] = Xtransmiter[i] + (((double)rand() / (RAND_MAX)- 0.5) * 6);
		Yreciver[i] = Ytransmiter[i] + (((double)rand() / (RAND_MAX)- 0.5) * 6);

		
		cout <<"transmiter ["<<i<<"] position is : ("<< Xtransmiter[i] <<" , " << Ytransmiter[i] 
			<< ")\n reciver ["<<i<<"] position is : (" << Xreciver[i] << " , " << Yreciver[i] << ") \n";

		
		currentPower[i] = ((double)rand() / (RAND_MAX)) * maxOfPower;
		price[i] = (1/(noise*bandWidth))*((double)rand() / (RAND_MAX));
		
		powerDifference[i] = 1;
	}
	
	Gain = new double*[numberOfNodes];
	for (int i = 0; i < numberOfNodes; i++) {
		
		Gain[i] = new double[numberOfNodes];
		
	}

	for (int i = 0; i < numberOfNodes; i++)
	{
		for (int j = 0; j < numberOfNodes; j++)
		{
			
			/*calcute destanse between trasmiter (i) and reciver (j)*/
			double distance = pow((pow((Xtransmiter[i]- Xreciver[j]),2) + pow((Ytransmiter[i] - Yreciver[j]),2 )),0.5);
			/*gain of trasmiter i and reciver j*/
			Gain[i][j] = pow(distance, -4);
		}
	}	
}

 void adp::GamaInitialization()
 {
	 adp a;
	 for (int i = 0; i < numberOfNodes; i++)
	 {
		 gama[i] = a.GamaCalculation(i);
	 }
 }

 double adp::UtilizationCalculation(int i)
 {
	 return( log(gama[i]));
 }

 
 double adp::GamaCalculation(int i) {
	 double sigI = 0;
	 
		 for (int j = 0; j < numberOfNodes; j++)
		 {
			 sigI += currentPower[j] * Gain[j][i];
		 }
		 return((currentPower[i] * Gain[i][i]) / (noise + (1 / bandWidth)*sigI));
 }



 void ADPNode(int i)
 {
	 
	 while (true)
	 {
		 double maxDiffr = 0;
		 for (int i = 0; i < numberOfNodes; i++)
		 {
			
			 if (maxDiffr < powerDifference[i])
				 maxDiffr = powerDifference[i];
		 }
		 /*max deferences between powers is lower than threshold??*/
		 if (maxDiffr < stopThreshold)
		 {
			 break;
		 }
		 
		 /*each thread (transmiter) wait for a random miliseconds 
			(for simulate the asynchronous defination)*/
		 std::this_thread::sleep_for(std::chrono::milliseconds((rand()) % 1000));

		 
		 double someOfPrice = 0;	

		 
		 double someOfInterference = 0;	

		 for (int j = 0; j < numberOfNodes; j++)
		 {
			
			 if (i != j)
			 {
				 someOfPrice += price[i] * Gain[j][i];
				 someOfInterference += currentPower[j] * Gain[i][j];
			 }
			 
		 }

		 
		 double newPower = 1/someOfPrice; 
		 if (newPower <= maxOfPower && newPower >= 0)
		 {
			
			 powerDifference[i] = (abs(newPower - currentPower[i])) / currentPower[i];
			 currentPower[i] = newPower;
		 }
		 else if (newPower > maxOfPower)
		 {
			 powerDifference[i] = (abs(maxOfPower - currentPower[i])) / currentPower[i];
			 currentPower[i] = maxOfPower;
		 }
			 
		 else
		 {
			 powerDifference[i] = 1;
			 currentPower[i] = 0;
		 }
			 

		 
		 price[i] = 1/ (bandWidth*noise +someOfInterference);
		 adp a;

		 
		 gama[i] = a.GamaCalculation(i);

		 
		 utilization[i] = a.UtilizationCalculation(i);
		 
		 
	 }

	 
 }

 void outputTask()
 {
	 double finalUtilization;
	 while (true)
	 {
		 
		 double correntUtilization = 0;
		 double maxDeffr = 0;
		 for (int i = 0; i < numberOfNodes; i++)
		 {
			 
			 correntUtilization += utilization[i]; 
			 if (maxDeffr < powerDifference[i])
				 maxDeffr = powerDifference[i];
		 }
		 
		 if (maxDeffr < stopThreshold)
		 {
			 finalUtilization = correntUtilization;
			 break;
		 }
			 
		 cout << "\n utilization = " << correntUtilization;// << " & max defference in is : " << maxcon;
		 /*make output each 100ms*/
		 std::this_thread::sleep_for(std::chrono::milliseconds(100));
	 }
	 cout << "\n\n\n\t\t\t the final utilization is : "
		 << finalUtilization << "\n\n ";
	 for (int i = 0; i < numberOfNodes; i++)
	 {
		 cout << "\n power["<<i<<"] = " << currentPower[i];
	 }
	 cout << "\n\n\n finished\n\n";
 }

 void adp::ThreadingNodes()
 {
	 using std::thread;
	 
	 threads = new thread[numberOfNodes]; 
	
	 thread outputThread(outputTask); 
	 
	 for (int i = 0; i < numberOfNodes; i++)
	 {
		 // define each thread to run ADPNode(i)
		 threads[i] = thread(ADPNode, i);
	 }
	 for (int i = 0; i < numberOfNodes; i++)
	 {
		 //start threads
		 threads[i].join(); 
	 }
	 
		 outputThread.join();
	 
 }

