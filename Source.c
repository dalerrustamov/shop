#define WIN32_LEAN_AND_MEAN 
#define NOATOM
#define NOCLIPBOARD
#define NOCOMM
#define NOCTLMGR
#define NOCOLOR
#define NODEFERWINDOWPOS
#define NODESKTOP
#define NODRAWTEXT
#define NOEXTAPI
#define NOGDICAPMASKS
#define NOHELP
#define NOICONS
#define NOTIME
#define NOIMM
#define NOKANJI
#define NOKERNEL
#define NOKEYSTATES
#define NOMCX
#define NOMEMMGR
#define NOMENUS
#define NOMETAFILE
#define NOMSG
#define NONCMESSAGES
#define NOPROFILER
#define NORASTEROPS
#define NORESOURCE
#define NOSCROLL
//#define NOSERVICE		/* Windows NT Services */
#define NOSHOWWINDOW
#define NOSOUND
#define NOSYSCOMMANDS
#define NOSYSMETRICS
#define NOSYSPARAMS
#define NOTEXTMETRIC
#define NOVIRTUALKEYCODES
#define NOWH
#define NOWINDOWSTATION
#define NOWINMESSAGES
#define NOWINOFFSETS
#define NOWINSTYLES
#define OEMRESOURCE
#pragma warning(disable : 4996)

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <io.h>
#include <WinSock2.h>
#include <time.h>

#if !defined(_Wp64)
#define DWORD_PTR DWORD
#define LONG_PTR LONG
#define INT_PTR INT
#endif

/*
int sleeptime;
void  sleeptimein(){
printf("Input the time between orders: ");
scanf(sleeptime);
}
*/

typedef struct Bin{
	/*
	Bins:
	GuardBin(mutex)
	DonutReady(event)
	DonutsInBin
	TotalDonutServed
	DonutCapacity
	*/
	HANDLE GuardBin; //(mutex)
	HANDLE DonutsReady; //(event)
	
	int DonutsInBin;
	int TotalDonutsServed;
	int DonutCapacity;

	//the handles 
	HANDLE DonutsInBinHANDLE;
	HANDLE DonutCapacityHANDLE;

}Bin;

Bin Bins[4]; //4 bins=STATIC

typedef struct Baker{
	/*
	Baker:
	DonutsBaked
	BakeToday
	BakerPos (0 or 1)
	BakerHandle(Handle)
	*/
	int DonutsBaked;
	int BakeToday;
	int BakerPos;
	HANDLE BakerHandle;
}Baker;

Baker Bakers[2]; //2 is static

typedef struct Worker{
	/*
	Worker:
	WorkerGo(Event)
	DonutsServed
	WorkerPosition
	WorkerHandle(HANDLE)
	*/
	HANDLE WorkerGo; //(Event)
	int DonutsServed;
	int WorkerPosition;
	int TimeToServe; //holds the time that a worker should take to serve between orders
	HANDLE WorkerHandle;
}Worker;

Worker Workers[6]; //6 is static

typedef struct Queue{
	/*
	Queue:
	GuardQueue (Mutex)
	WorkerDone (Event)
	QueuePos
	QueueHandle (HANDLE)
	ManagerDone --??
	Front 0
	Back 0
	Count 0
	itemArray
	*/
	HANDLE GuardQueue; //(Mutex)
	HANDLE WorkerDone; //(Event)
	int QueuePos;
	HANDLE QueueHandle;
	BOOL ManagerDone[4];
	int Front;
	int Back;
	int Count;
	int itemArray[6];
}Queue;

Queue Managers[4]; //only 4 managers = static


void TodayGoal() { //First input: BakeToday
	for (int i = 0; i < 2; i++) {
		printf("Input the number of donuts the baker #%d will bake today: ", i+1);
		scanf("%d", &Bakers[i].BakeToday);
		//printf("\n");
	}
}

/*
void TodayGoal() { //First input: BakeToday
	printf("Input the number of donuts the bakers will bake today: ");
	scanf("%d", &Bakers->BakeToday);
}
*/

int TimeForOneDonut; //KEEP IT GLOBAL

void TimeToBake() { //second input: TimeForOneDonut
	printf("Input the time it takes to bake one donut(ms): ");
	scanf("%d", &TimeForOneDonut);
}

void DonutsFit() {  //Third input: DonutCapacity
	printf("Input how many donuts fit in a bin: ");
	scanf("%d", &Bins->DonutCapacity);

	/*
	int shopCap = Bins->DonutCapacity * 4;
	for (int i = 0; i <= 3; i++) {
		int shopAvailable = shopAvailable + Bakers[i].BakeToday;
	}
	*/
}

void TimeBerweenOrders() {
	printf("\n");
	for (int i = 0; i < 6; i++) {
		printf("Input the time between orders for worker #%d: ",i+1);
		scanf("%d", &Workers[i].TimeToServe);
	}
}

/*
void processshow() {
	int YESORNO;
	printf("Do you want to see the whole process or just results?: ");
	scanf("%d", &YESORNO);
}
*/

//THE STATUSES

BOOL AllBakersDone;
BOOL AllDonutsSold;
BOOL AllWorkersDone;
BOOL BakersBaking[2];
BOOL BinsDone[4];
BOOL WorkersDone[6];

//I use them later
int Temp = 0;
int Type_workThread; //1,2,3,4

//THE BAKER VARIABLES

//sorry for the hardcode. 
//I could easily create multidimentional array, but since 2,4,6 are static for this assignment, I preferred copypaste this time

int zero_0 = 0;
int zero_1 = 0;
int zero_2 = 0;
int zero_3 = 0;

int one_0 = 0;
int one_1 = 0;
int one_2 = 0;
int one_3 = 0;

//THE WORKER VARIABLES

int first_0 = 0;
int first_1 = 0;
int first_2 = 0;
int first_3 = 0;

int second_0 = 0;
int second_1 = 0;
int second_2 = 0;
int second_3 = 0;

int third_0 = 0;
int third_1 = 0;
int third_2 = 0;
int third_3 = 0;

int fourth_0 = 0;
int fourth_1 = 0;
int fourth_2 = 0;
int fourth_3 = 0;

int fifth_0 = 0;
int fifth_1 = 0;
int fifth_2 = 0;
int fifth_3 = 0;

int sixth_0 = 0;
int sixth_1 = 0;
int sixth_2 = 0;
int sixth_3 = 0;

//THE MUTEXES

HANDLE GuardBakerStatus;
HANDLE GuardDonutStatus;
HANDLE GuardWorkerStatus;

//THE THREADS

//*******************************
//I NEED 2
void WINAPI Bake(int who) //who is 1 or 2 passed by x from loop from main
{
	//Bakers[who].DonutsBaked = 0; //I initialized it with 0, otherwise it wouldnt enter the loop
	int Type; //0-3. I only need it in the loop, but declared it here so I dont have to declare it 200 times in the loop

	while (Bakers[who].DonutsBaked < Bakers[who].BakeToday) //baketoday is the goal entered by the user from main(), so it keeps entering the looop until it reaches the goal
	{
		Type = rand() % 4; //0,1,2,3 KEEP IT INSIDE THE WHILE

		if (WaitForSingleObject(Bins->DonutCapacityHANDLE, 1) != WAIT_TIMEOUT)
		{
			ReleaseSemaphore(Bins->DonutsInBin, 1, NULL); //releasing the semaphore
			//printf("%d", DonutType);

			Sleep(TimeForOneDonut); //TimeForOneDonut::::from input

			WaitForSingleObject(Bins->GuardBin, INFINITE);
			SetEvent(Bins->DonutsReady);

			Bins[Type].DonutsInBin++; //I dont use this line here, but I use it in my worker later
			Bakers[who].DonutsBaked++; //I only need it to get into the loop

			if (who == 0) {
				if (Type == 0) {

					zero_0++;
				}
				else if (Type == 1) {

					zero_1++;
				}
				else if (Type == 2) {

					zero_2++;
				}
				else {
					//if (Type == 3) {

					zero_3++;
				}
			}
			else {
				//if (who == 1) {
				if (Type == 0) {

					one_0++;
				}
				else if (Type == 1) {

					one_1++;
				}
				else if (Type == 2) {

					one_2++;
				}
				else {
					//if (Type == 3) {

					one_3++;
				}
			}

			ReleaseMutex(Bins->GuardBin); 


		}//if
	}//while

	BakersBaking[who] = FALSE;


	WaitForSingleObject(GuardBakerStatus, INFINITE);


	for (int i = 0; i <= 1; i++) {
		if (BakersBaking[i] == FALSE) {
			AllBakersDone = TRUE;
		}
	}

	ReleaseMutex(GuardBakerStatus);

	Sleep(10); //This is useless, but I need it just to make sure to get the results in a good order after releasing the mutex
	//for (int i = 0; i <= 1; i++) {

	//}
}

//*******************************
//I NEED 6
void WINAPI Work(int who)
{
	while (WorkersDone[who] != TRUE)
	{
		//printf("Input the time between orders: ");

		Sleep(Workers[who].TimeToServe); //For each worker input the time between orders

		WaitForSingleObject(GuardBakerStatus, INFINITE);
		{
			WaitForSingleObject(GuardDonutStatus, INFINITE);

			if (Bins->DonutsInBin == 0)//if bin is empty, then AllDonutsSold
			{
				AllDonutsSold = TRUE;

			}
			if (AllBakersDone == TRUE) //First Bakers bake 
			{
				if (AllDonutsSold == TRUE) { //And then workers sell
					//sleep(10);
					WorkersDone[who] = TRUE;  //If both are true, workers are ready to go home
				}
			}
			if (AllBakersDone != TRUE) //If this is not true, then AllDonutsSold is also not true
			{
				WorkersDone[who] = FALSE;
			}

			ReleaseMutex(GuardDonutStatus);
		}

		ReleaseMutex(GuardBakerStatus);

		Type_workThread = rand() % 4; //0,1,2,3
		WaitForSingleObject(Managers[who].GuardQueue, INFINITE);
		{
			if (Managers[Type_workThread].Count < 6)
			{
				Managers[Type_workThread].itemArray[Managers[Type_workThread].Back = who];
				Managers[Type_workThread].Back++;
				if (Managers[Type_workThread].Back == 6)
				{
					Managers[Type_workThread].Back = 0;
				}
			}
			Managers[Type_workThread].Count++;
		}

		//if (YESORNO == 1) {
		//	printf("Worker %d sold %d donuts today", who, Workers[who].DonutsServed);
		//}
		//else//do nothung
		ReleaseMutex(Managers[who].GuardQueue);
		WaitForSingleObject(Workers[who].WorkerGo, INFINITE);
		WaitForSingleObject(Bins[Type_workThread].GuardBin, INFINITE);

		{
			if (Bins[Type_workThread].DonutsInBin)
			{
				Workers[who].DonutsServed++; 
				Bins[Type_workThread].DonutsInBin--; //the donuts getting out of the bin, the space gets decremented
				 
				//wh0 => 0 to 5;

				if (who == 0) {//1
					if (Type_workThread == 0) {
						first_0++;
					}
					else if (Type_workThread == 1) {
						first_1++;
					}
					else if (Type_workThread == 2) {
						first_2++;
					}
					else {
						first_3++;
					}
				}
				else if (who == 1) {
					if (Type_workThread == 0) {
						second_0++;
					}
					else if (Type_workThread == 1) {
						second_1++;
					}
					else if (Type_workThread == 2) {
						second_2++;
					}
					else {
						second_3++;
					}
				}
				else if (who == 2) {
					if (Type_workThread == 0) {
						third_0++;
					}
					else if (Type_workThread == 1) {
						third_1++;
					}
					else if (Type_workThread == 2) {
						third_2++;
					}
					else {
						third_3++;
					}
				}
				else if (who == 3) {
					if (Type_workThread == 0) {
						fourth_0++;
					}
					else if (Type_workThread == 1) {
						fourth_1++;
					}
					else if (Type_workThread == 2) {
						fourth_2++;
					}
					else {
						fourth_3++;
					}
				}
				else if (who == 4) {
					if (Type_workThread == 0) {
						fifth_0++;
					}
					else if (Type_workThread == 1) {
						fifth_1++;
					}
					else if (Type_workThread == 2) {
						fifth_2++;
					}
					else {
						fifth_3++;
					}
				}
				else { //who==5
					if (Type_workThread == 0) {
						sixth_0++;
					}
					else if (Type_workThread == 1) {
						sixth_1++;
					}
					else if (Type_workThread == 2) {
						sixth_2++;
					}
					else {
						sixth_3++;
					}
				}


				WaitForSingleObject(GuardDonutStatus, INFINITE);
				if (Bins[Type_workThread].DonutsInBin == 0)
				{
					Bins[Type_workThread].DonutsReady = FALSE;
				}
				ReleaseMutex(GuardDonutStatus);
				SetEvent(WorkersDone[who] == TRUE); // tells the manager you're done
			}
		}
		ReleaseMutex(Bins[Type_workThread].GuardBin);
	}


	WaitForSingleObject(GuardWorkerStatus, INFINITE);
	{
		WorkersDone[who] = FALSE; // worker going home
		if (WorkersDone[0] == FALSE && WorkersDone[1] == FALSE)
		{
			AllWorkersDone = TRUE;
		}
	}
	ReleaseMutex(GuardWorkerStatus);
}


//*******************************
//I NEED 4
void WINAPI Manage(int who)
{
	while (AllBakersDone != TRUE && Managers->ManagerDone[who] != TRUE)
	{
		int temp = 0;

			WaitForSingleObject(Bins[who].DonutsReady, INFINITE); // HANDLE DonutsReady (event);
			{
				WaitForSingleObject(Managers[who].GuardQueue, INFINITE);
				//Decrement the count
				Managers[who].Count--;

				if (Managers[who].Count > 0)
				{
					Managers[who].Front = 0;
					Temp = Managers[who].itemArray[Managers[who].Front];
					SetEvent(Workers[Temp].WorkerGo);
					temp = 1;
					
				}

				Managers[who].Front++;
			}

			ReleaseMutex(Managers[who].GuardQueue);

			if (temp != 0) //if it made the: if (Managers[who].Count > 0)
			{
				ReleaseMutex(Bins[who].GuardBin);

				WaitForSingleObject(Managers[who].WorkerDone, INFINITE);

				if (AllBakersDone == TRUE)
				{
					SetEvent(Bins[who].DonutsReady) == TRUE;
				}
				if (Bins->DonutsInBin >= 1) //There is some donuts in the bin
				{
					SetEvent(Bins[who].DonutsReady) == TRUE;
				}
			}

			WaitForSingleObject(GuardWorkerStatus, INFINITE);

			if (AllWorkersDone == TRUE)
			{
				Managers->ManagerDone[who] = TRUE;
			}
			if ((AllWorkersDone == FALSE))
			{
				Managers->ManagerDone[who] = FALSE;
			}

			ReleaseMutex(GuardWorkerStatus);
	}
}


int _tmain(int argc, LPTSTR argv[])
{
	printf("There are 2 bakers, 4 bins, 6 workers \n");

	//inputs:

	TodayGoal();
	//sleeptimein();
	TimeToBake();
	DonutsFit();

	//processshow();
	TimeBerweenOrders();
	//starting the threads:

	//--BAKERS--//
	for (int x = 0; x < 2; x++) //I always need 2 Baker threads
	{
		Bakers[x].BakerHandle = (HANDLE)_beginthreadex(NULL, 0, Bake, x, 0, NULL); //x holds who
	}

	for (int x = 0; x < 2; x++) //wait infinite 
	{
		WaitForSingleObject(Bakers[x].BakerHandle, INFINITE);
	}

	//RESULTS: 

	//I could print it out from inside the thread, but there would be 2 copies of the same thing since there are 2 bakers
	//Or I could create another function, 
	//Or multidimentional array and printf with a loop at least, but I was just lazy


	//BAKERS_cout

	printf("\nBaker#%d\n", 1);
	printf("Made %d donuts\n", zero_0 + zero_1 + zero_2 + zero_3);

	printf("%d type #%d\n", zero_0, 1);
	printf("%d type #%d\n", zero_1, 2);
	printf("%d type #%d\n", zero_2, 3);
	printf("%d type #%d\n", zero_3, 4);

	printf("\nBaker#%d\n", 2);
	printf("Made %d donuts\n", one_0 + one_1 + one_2 + one_3);

	printf("%d type #%d\n", one_0, 1);
	printf("%d type #%d\n", one_1, 2);
	printf("%d type #%d\n", one_2, 3);
	printf("%d type #%d\n\n", one_3, 4);

	//BINS_cout

	printf("\nBin #%d\n%d Donuts\n", 1, zero_0 + one_0);
	printf("\nBin #%d\n%d Donuts\n", 2, zero_1 + one_1);
	printf("\nBin #%d\n%d Donuts\n", 3, zero_2 + one_2);
	printf("\nBin #%d\n%d Donuts\n", 4, zero_3 + one_3);

	//--WORKERS--//
	for (int x = 0; x < 6; x++) //I always need 2 Baker threads
	{
		Workers[x].WorkerHandle = (HANDLE)_beginthreadex(NULL, 0, Work, x, 0, NULL);
	}

	for (int x = 0; x < 6; x++)
	{
		WaitForSingleObject(Workers[x].WorkerHandle, INFINITE);
	}

	//WORKERS_cout
	printf("\n");
	printf("Worker #%d\n\nSold %d donuts\n\n", 1, first_0 + first_1 + first_2 + first_3);
	printf("%d type %d\n", first_0, 1);
	printf("%d type %d\n", first_1, 2);
	printf("%d type %d\n", first_2, 3);
	printf("%d type %d\n", first_3, 4);

	printf("\n");
	printf("Worker #%d\n\nSold %d donuts\n\n", 2, second_0 + second_1 + second_2 + second_3, 2);
	printf("%d type %d\n", second_0, 1);
	printf("%d type %d\n", second_1, 2);
	printf("%d type %d\n", second_2, 3);
	printf("%d type %d\n", second_3, 4);

	printf("\n");
	printf("Worker #%d\n\nSold %d donuts\n\n", 3, third_0 + third_1 + third_2 + third_3, 3);
	printf("%d type %d\n", third_0, 1);
	printf("%d type %d\n", third_1, 2);
	printf("%d type %d\n", third_2, 3);
	printf("%d type %d\n", third_3, 4);

	printf("\n");
	printf("Worker #%d\n\nSold %d donuts\n\n", 4, fourth_0 + fourth_1 + fourth_2 + fourth_3, 4);
	printf("%d type %d\n", fourth_0, 1);
	printf("%d type %d\n", fourth_1, 2);
	printf("%d type %d\n", fourth_2, 3);
	printf("%d type %d\n", fourth_3, 4);

	printf("\n");
	printf("Worker #%d\n\nSold %d donuts\n\n", 5, fifth_0 + fifth_1 + fifth_2 + fifth_3);
	printf("%d type %d\n", fifth_0, 1);
	printf("%d type %d\n", fifth_1, 2);
	printf("%d type %d\n", fifth_2, 3);
	printf("%d type %d\n", fifth_3, 4);

	printf("\n");
	printf("Worker #%d\n\nSold %d donuts\n\n", 6, sixth_0 + sixth_1 + sixth_2 + sixth_3);
	printf("%d type %d\n", sixth_0, 1);
	printf("%d type %d\n", sixth_1, 2);
	printf("%d type %d\n", sixth_2, 3);
	printf("%d type %d\n", sixth_3, 4);

	printf("\n");

	//JUST EXTRA THING
	printf("Total %d type %d was sold\n", first_0 + second_0 + third_0 + fourth_0 + fifth_0 + sixth_0, 1);
	printf("Total %d type %d was sold\n", first_1 + second_1 + third_1 + fourth_1 + fifth_1 + sixth_1, 2);
	printf("Total %d type %d was sold\n", first_2 + second_2 + third_2 + fourth_2 + fifth_2 + sixth_2, 3);
	printf("Total %d type %d was sold\n\n", first_3 + second_3 + third_3 + fourth_3 + fifth_3 + sixth_3, 4);

	//--MANAGERS--//
	for (int x = 0; x < 4; x++) //I always need 4 Queue manager threads
	{
		Managers[x].QueueHandle = (HANDLE)_beginthreadex(NULL, 0, Manage, x, 0, NULL);
	}

	for (int x = 0; x < 4; x++)
	{
		WaitForSingleObject(Managers[x].QueueHandle, INFINITE);
	}

	//no outputs from manager!!!!!!

	//KEEP OUTSIDE THE LOOP
	GuardDonutStatus = CreateMutex(NULL, FALSE, NULL);

	GuardBakerStatus = CreateMutex(NULL, FALSE, NULL);

	GuardWorkerStatus = CreateMutex(NULL, FALSE, NULL);
	

	for (int x = 0; x <= 3; x++) //0-3 managers
	{
		//managers mutexes
		Managers[x].GuardQueue = CreateMutex(NULL, FALSE, NULL);
		Managers[x].WorkerDone = CreateEvent(NULL, FALSE, FALSE, NULL);
		
		Bins[x].DonutsReady = CreateEvent(NULL, FALSE, FALSE, NULL);
		Bins[x].GuardBin = CreateMutex(NULL, FALSE, NULL);
		//bins semaphores
		Bins[x].DonutCapacityHANDLE = CreateSemaphore(NULL, &Bins->DonutCapacity, &Bins->DonutCapacity, NULL);
		//initial is the user input, max is the user input
		Bins[x].DonutsInBinHANDLE = CreateSemaphore(NULL, 0, &Bins->DonutCapacity, NULL); 
		//initial count is 0, max is the user input

		//Bins[0].GuardBin = CreateMutex(NULL, FALSE, NULL);
	}

	system("pause");
	return 0;
}
