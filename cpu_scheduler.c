#include <stdio.h>
#include <stdlib.h>
struct PCB{
	int pid;
	int priority;
	int arrival_Time;
	int burst_Time;
	int age;
	int finish;  //inform this process is finish
	int restBurstTime; //rest of burst time
	int notWorking;  //if notWorking is 1 then, this pcb is use more than once.
}pcb[10];
struct JOB_QUEUE{
	struct PCB *pcb;
	int check; //if check == 1 then this process is in ready queue or cpu or finish
}job_queue[10];
struct READY_QUEUE{
	struct PCB *pcb;
	struct READY_QUEUE *next;
}*start = NULL,*rear = NULL;
void RR(char*);
void FCFS();
void preemitive(char*);
int SETTING(int,int);
int Fcount = 0;
int cpuNull = 0;
struct READY_QUEUE *beforeRare =NULL;
FILE *out;
void main(int argc ,char *argv[])
{
	int i;
	FILE *process;
	process = fopen(argv[1],"r");
	out = fopen(argv[2],"w");
	if(out == NULL)
		exit(1);
	if(process == NULL)
		exit(1);
	for(i = 0; i < 10; i++)
	{
		fscanf(process,"%d %d %d %d",&pcb[i].pid,&pcb[i].priority,&pcb[i].arrival_Time,&pcb[i].burst_Time);
		pcb[i].age = 0;
		pcb[i].notWorking = 0;
		pcb[i].restBurstTime = NULL;
		job_queue[i].pcb = &pcb[i]; 
	}
	printf("\n-------------------   FCFS SCHEDULER  --------------------\n");
	fprintf(out,"\n\n-------------------   FCFS SCHEDULER  --------------------\n\n");
	FCFS();
	printf("\n-------------------   Round Robin SCHEDULER  --------------------\n\n");
	fprintf(out,"\n\n-------------------   Round Robin SCHEDULER  --------------------\n\n");
	RR(argv[3]);
	printf("\n-------------------   Preemitive SCHEDULER  --------------------\n\n");
	fprintf(out,"\n\n-------------------   Preemitive SCHEDULER  --------------------\n\n");
	preemitive(argv[4]);
	
}
void RR(char *timeQ)
{
	struct READY_QUEUE *ptr = NULL,*temp,*lock;
	int i,waitingTime = 0;
	int timeQuatum = atoi(timeQ);
	int turnaround = 0;
	int response = 0;
	int burstTime;
	int readyCheck = 1; //if 0 : scheduling is finsihed. if 2 : process is ready to get in CPU
	int cpuTime = 0;
	int compareToQuatum = 0;
	int cpu = 0; //if 1 : cpu is running. if 0 : cpu is idle.
	//start = NULL;
	//rear = NULL;
	while(readyCheck != 0)
	{
		readyCheck = SETTING(cpuTime,cpu);   
		if(readyCheck == 0)
			break;
		//if readyCheck 0 : finish, 1 : not come in CPU. 2 : come in CPU (arrival time == cpu time)
		if(ptr == NULL)
			ptr = start;
		if(cpu == 0){  //cpu is not running
			if(readyCheck == 1)   //���� cpu�� process�� �ȵ�� ����
			{
				printf("<time %d> ---- SYSTEM is idle -------\n",cpuTime);
				fprintf(out,"<time %d> ---- SYSTEM is idle -------\n",cpuTime);
				cpuNull++;
				cpuTime++;
			}
			else if(readyCheck == 2) //cpu�� ����
			{
				printf("<time %d> [new arrival] Process %d \n",cpuTime,ptr->pcb->pid);
				fprintf(out,"<time %d> [new arrival] Process %d \n",cpuTime,ptr->pcb->pid);
				cpu = 1;
				if(ptr->pcb->restBurstTime == NULL)
					burstTime = ptr->pcb->burst_Time;
				else
					burstTime = ptr->pcb->restBurstTime;
				lock = ptr;
				if(ptr->next != NULL){
					ptr= ptr->next;
					while(ptr){
						printf("<time %d> [new arrival] Process %d \n",cpuTime,ptr->pcb->pid);
						fprintf(out,"<time %d> [new arrival] Process %d \n",cpuTime,ptr->pcb->pid);
						ptr = ptr->next;
					}
					ptr = lock;
				}
				readyCheck = 3; // cpu�� ���� readycheck == 2 ��� ���ǿ� �Ȱɸ��� �ϱ� ���ؼ�.
			}
		}
		if(cpu == 1){ //cpu�� �� ��
			if(readyCheck == 2) //cpu�� ���µ� new process�� ���� ���.
			{
				lock = ptr;
				if(ptr->next != NULL){
					ptr= ptr->next;
					while(ptr){
						if(cpuTime == ptr->pcb->arrival_Time){
							printf("<time %d> [new arrival] Process %d \n",cpuTime,ptr->pcb->pid);
							fprintf(out,"<time %d> [new arrival] Process %d \n",cpuTime,ptr->pcb->pid);
						}
						ptr = ptr->next;
					}
					ptr = lock;
				}
			}
			if(ptr->pcb->finish != 1){  //if finish is 1 then, this process is finish

				if(compareToQuatum != 0 && compareToQuatum % timeQuatum == 0){           //rr time
					if(ptr->next != NULL){
						temp = ptr;
						ptr->pcb->restBurstTime = burstTime;
						if(rear != ptr->next){
							rear->next = ptr;
							rear = rear->next;
							ptr = ptr->next;
							temp->next = NULL;
						}
						else{
							rear->next = ptr;
							ptr = ptr->next;;
							//temp->next = NULL;
							rear = rear->next;
							temp->next = NULL;
							
						}
						if(ptr->pcb->notWorking == 0){
							response += ptr->pcb->age;
							ptr->pcb->notWorking = 1;
						}
						compareToQuatum = 0;
						printf("---------------------------Context Switch !!\n");
						fprintf(out,"---------------------------Context Switch !!\n");
						if(ptr->pcb->restBurstTime == NULL)
							burstTime = ptr->pcb->burst_Time;
						else
							burstTime = ptr->pcb->restBurstTime;
					}
					else{  //ready queue has only one process
						printf("---------------------------Context Switch !!!\n");
						fprintf(out,"---------------------------Context Switch !!!\n");
						ptr = ptr;
						compareToQuatum = 0;
					}
	
				}
				else{
					printf("<time %d> process %d is running \n",cpuTime,ptr->pcb->pid);
					fprintf(out,"<time %d> process %d is running \n",cpuTime,ptr->pcb->pid);
					lock = ptr;
					ptr = ptr->next;
					while(ptr){
						ptr->pcb->age ++;
						ptr = ptr->next;
					}
					ptr = lock;
					cpuTime++;
					compareToQuatum++;
					burstTime--;
					if(burstTime == 0){
						ptr->pcb->finish = 1;
					}
					continue;
				}
			}
			else if(ptr->pcb->finish == 1 ){
				printf("<time %d> process %d is finished\n",cpuTime,ptr->pcb->pid);
				fprintf(out,"<time %d> process %d is finished\n",cpuTime,ptr->pcb->pid);
				temp = ptr;
				compareToQuatum = 0;
				//compareToQuatum++;
				cpuTime++;
				if(ptr->next != NULL )  //�ڿ� ���� ���� process�� �ִٸ�.
				{
					printf("---------------------------Context Switch !!!!!\n");
					fprintf(out,"---------------------------Context Switch !!!!!\n");
					cpu = 1;
					ptr = ptr->next;
					if(ptr->pcb->notWorking == 0){
						response += ptr->pcb->age;
						ptr->pcb->notWorking = 1;
					}
					if(ptr->pcb->restBurstTime != NULL)
						burstTime = ptr->pcb->restBurstTime;
					else
						burstTime = ptr->pcb->burst_Time;
					free(temp);
					continue;
				}
				else{           //�ڿ� ���� ���� process�� ���ٸ�.
					free(temp);
					cpu = 0;    //cpu�� ��Եǰ�.
				}
			}

		}
	}
	printf("CPU USAGE : %f%%\n",(double)(cpuTime-cpuNull)/(double)cpuTime * 100);
	fprintf(out,"\nCPU USAGE : %f%%\n",(double)(cpuTime-cpuNull)/(double)cpuTime * 100);
	for(i = 0; i < 10; i++)
		waitingTime += pcb[i].age;
	printf("Average Waiting Time : %d \n",waitingTime/10);
	fprintf(out,"Average Waiting Time : %d \n",waitingTime/10);
	printf("Average of Response Time : %d \n",response/10);
	fprintf(out,"Average of Response Time : %d \n",response/10);
	for(i = 0; i < 10; i++)
		turnaround += pcb[i].age + pcb[i].burst_Time;
	printf("Average of TurnAround Time : %d \n",turnaround/10);
	fprintf(out,"Average of TurnAround Time : %d \n",turnaround/10);
	start = NULL;
	rear = NULL;
	for(i = 0; i < 10; i++){
		pcb[i].finish = 0;
		pcb[i].notWorking = 0;
		pcb[i].age = 0;
	}
	Fcount = 0;
	cpuNull = 0;
}
void FCFS()
{
	int i,waitingTime = 0;
	int response = 0;
	int turnaround = 0;
	struct READY_QUEUE *ptr = NULL,*temp,*lock;
	int burstTime;
	int readyCheck = 1; //if 0 : scheduling is finsihed. if 2 : process is ready to get in CPU
	int cpuTime = 0;
	int cpu = 0; //if 1 : cpu is running. if 0 : cpu is idle.
	//start = NULL;
	//rear = NULL;
	while(readyCheck != 0)
	{
		readyCheck = SETTING(cpuTime,cpu);   
		if(readyCheck == 0)
			break;
		//if readyCheck 0 : finish, 1 : not come in CPU. 2 : come in CPU (arrival time == cpu time)
		if(ptr == NULL)
			ptr = start;
		if(cpu == 0){  //cpu is not running
			if(readyCheck == 1)   //���� cpu�� process�� �ȵ�� ����
			{
				printf("<time %d> ---- SYSTEM is idle -------\n",cpuTime);
				fprintf(out,"<time %d> ---- SYSTEM is idle -------\n",cpuTime);
				cpuNull++;
				cpuTime++;
				continue;
			}
			else if(readyCheck == 2) //cpu�� ����
			{
				printf("<time %d> [new arrival] Process %d \n",cpuTime,ptr->pcb->pid);
				fprintf(out,"<time %d> [new arrival] Process %d \n",cpuTime,ptr->pcb->pid);
				cpu = 1;
				burstTime = ptr->pcb->burst_Time;
				lock = ptr;
				if(ptr->next != NULL){
					ptr= ptr->next;
					while(ptr){
						printf("<time %d> [new arrival] Process %d \n",cpuTime,ptr->pcb->pid);
						fprintf(out,"<time %d> [new arrival] Process %d \n",cpuTime,ptr->pcb->pid);

						ptr = ptr->next;
					}
					ptr = lock;
				}
				readyCheck = 3; // cpu�� ���� readycheck == 2 ��� ���ǿ� �Ȱɸ��� �ϱ� ���ؼ�.
			}
			
		}
		if(cpu == 1){ //cpu�� �� ��
			if(readyCheck == 2) //cpu�� ���µ� new process�� ���� ���.
			{
				lock = ptr;
				if(ptr->next != NULL){
					ptr= ptr->next;
					while(ptr){
						if(cpuTime == ptr->pcb->arrival_Time){
							printf("<time %d> [new arrival] Process %d \n",cpuTime,ptr->pcb->pid);
							fprintf(out,"<time %d> [new arrival] Process %d \n",cpuTime,ptr->pcb->pid);
						}
						ptr = ptr->next;
					}
					ptr = lock;
				}
			}
			if(ptr->pcb->finish != 1){  //if finish is 1 then, this process is finish
				lock = ptr;
				ptr = ptr->next;
				while(ptr){
					ptr->pcb->age ++;
					ptr = ptr->next;
				}
				ptr = lock;
				printf("<time %d> process %d is running \n",cpuTime,ptr->pcb->pid);
				fprintf(out,"<time %d> process %d is running \n",cpuTime,ptr->pcb->pid);
				cpuTime++;
				burstTime--;
				if(burstTime == 0){
					ptr->pcb->finish = 1;
				}
				continue;
			}
			else if(ptr->pcb->finish == 1 ){
				printf("<time %d> process %d is finished\n",cpuTime,ptr->pcb->pid);
				fprintf(out,"<time %d> process %d is finished\n",cpuTime,ptr->pcb->pid);
				cpuTime++;
				temp = ptr;
				if(ptr->next != NULL )  //�ڿ� ���� ���� process�� �ִٸ�.
				{
					printf("---------------------------Context Switch !\n");
					fprintf(out,"---------------------------Context Switch !\n");
					cpu = 1;
					ptr = ptr->next;
					response += ptr->pcb->age;
					burstTime = ptr->pcb->burst_Time;
					free(temp);
					continue;
				}
				else{           //�ڿ� ���� ���� process�� ���ٸ�.
					free(temp);
					cpu = 0;    //cpu�� ��Եǰ�.
				}
			}

		}
	}
	printf("\nCPU USAGE : %f%%\n",(double)(cpuTime-cpuNull)/(double)cpuTime * 100);
	fprintf(out,"CPU USAGE : %f%%\n",(double)(cpuTime-cpuNull)/(double)cpuTime * 100);
	for(i = 0; i < 10; i++)
		waitingTime += pcb[i].age;
	printf("Average Waiting Time : %d \n",waitingTime/10);
	fprintf(out,"Average Waiting Time : %d \n",waitingTime/10);
	printf("Average of Response Time : %d \n",response/10);
	fprintf(out,"Average of Response Time : %d \n",response/10);
	for(i = 0; i < 10; i++)
		turnaround += pcb[i].age + pcb[i].burst_Time;
	printf("Average of TurnAround Time : %d \n",turnaround/10);
	fprintf(out,"Average of TurnAround Time : %d \n",turnaround/10);
	start = NULL;
	rear = NULL;
	for(i = 0; i < 10; i++){
		pcb[i].finish = 0;
		pcb[i].notWorking = 0;
		pcb[i].age = 0;
	}
	Fcount = 0;
	cpuNull = 0;
}
void preemitive(char *alph)
{
	float alpha = atof(alph);
	int i,cpuNull = 0;
	int waitingTime = 0,responsetime = 0;
	int turnaround = 0;
	struct READY_QUEUE *ptr = NULL,*ttemp,*lock;
	struct READY_QUEUE *trail,*oldptr = NULL;
	int count = 0;
	struct PCB *temp;
	struct READY_QUEUE *max = NULL;
	int burstTime;
	int readyCheck = 1; //if 0 : scheduling is finsihed. if 2 : process is ready to get in CPU
	int cpuTime = 0;
	int cpu = 0; //if 1 : cpu is running. if 0 : cpu is idle.
	//start = NULL;
	//rear = NULL;
	while(readyCheck != 0)
	{
		readyCheck = SETTING(cpuTime,cpu);   
		if(readyCheck == 0)
			break;
		//if readyCheck 0 : finish, 1 : not come in CPU. 2 : come in CPU (arrival time == cpu time)
		if(ptr == NULL)
			ptr = start;
		if(cpu == 0){  //cpu is not running
			if(readyCheck == 1)   //���� cpu�� process�� �ȵ�� ����
			{
				printf("<time %d> ---- SYSTEM is idle -------\n",cpuTime);
				fprintf(out,"<time %d> ---- SYSTEM is idle -------\n",cpuTime);
				cpuNull++;
				cpuTime++;
				continue;
			}
			else if(readyCheck == 2) //cpu�� ����
			{
				printf("<time %d> [new arrival] Process %d \n",cpuTime,start->pcb->pid);
				fprintf(out,"<time %d> [new arrival] Process %d \n",cpuTime,start->pcb->pid);
				cpu = 1;
				burstTime = start->pcb->burst_Time;
				if(start->next != NULL){
					ptr = start->next;
					while(ptr){                        //���� �ð��� �������� process�� ���� �� ��������.
						printf("<time %d> [new arrival] Process %d \n",cpuTime,ptr->pcb->pid);
						fprintf(out,"<time %d> [new arrival] Process %d \n",cpuTime,ptr->pcb->pid);

						ptr = ptr->next;
					}
				}
				readyCheck = 3; // cpu�� ���� readycheck == 2 ��� ���ǿ� �Ȱɸ��� �ϱ� ���ؼ�.
			}
			
		}
		if(cpu == 1){ //cpu�� �� ��
			if(readyCheck == 2) //cpu�� ���µ� new process�� ���� ���.
			{
				start->pcb->restBurstTime = burstTime;
				ptr = start;
				max = start;
				if(ptr->next != NULL){
					ptr= ptr->next;
					while(ptr){
						if(cpuTime == ptr->pcb->arrival_Time){
							printf("<time %d> [new arrival] Process %d \n",cpuTime,ptr->pcb->pid);
							fprintf(out,"<time %d> [new arrival] Process %d \n",cpuTime,ptr->pcb->pid);
							if((max->pcb->age * alpha + max->pcb->priority) < (ptr->pcb->age *  alpha + ptr->pcb->priority)){ 
								//arrive �� process �� ���� ���� process ���� �켱 ������ ������ context switch ���ش�.
								if(count == 0){
									printf("--------------------------(context switch) \n");
									fprintf(out,"--------------------------(context switch) \n");
								}
								count++;
								max = ptr;
								trail = oldptr;
							}

						}
						oldptr = ptr;
						ptr = ptr->next;
					}
					count = 0;
					if(start != max){
						temp = start->pcb;
						start->pcb = max->pcb;
						max->pcb = temp;
						burstTime = start->pcb->burst_Time;
						if(max->next != NULL)
						{
							trail->next = max->next;
							rear->next = max;
							max->next = NULL;
							rear = max;
						}
						
					}
				}
				
			}
			if(start->pcb->finish != 1){  //if finish is 1 then, this process is finish

				max = start;
				ptr = start;
				while(ptr){
					if((max->pcb->age *  alpha + max->pcb->priority) < (ptr->pcb->age *  alpha + ptr->pcb->priority)){
						max = ptr;
						trail = oldptr;
					}
					oldptr = ptr;
					ptr = ptr->next;
				}
				if(max != start)
				{
					printf("--------------------------(context switch) \n");
					fprintf(out,"--------------------------(context switch) \n");
					temp = max->pcb;
					max->pcb = start->pcb;
					start->pcb = temp;
					if(start->pcb->notWorking != 1){
						responsetime += start->pcb->age;
						start->pcb->notWorking = 1;
					}
					if(max->next != NULL){
						trail->next = max->next;
						rear->next = max;
						max->next = NULL;
						rear = max;
					}
				}
				printf("<time %d> process %d is running \n",cpuTime,start->pcb->pid);
				fprintf(out,"<time %d> process %d is running \n",cpuTime,start->pcb->pid);
				cpuTime++;
				ptr = start->next;
				while(ptr){
					ptr->pcb->age ++;
					ptr = ptr->next;
				}
				burstTime--;
				if(burstTime == 1){
					start->pcb->finish = 1;
				}
				
			}
			else if(start->pcb->finish == 1 ){
				printf("<time %d> process %d is finished\n",cpuTime,start->pcb->pid);
				fprintf(out,"<time %d> process %d is finished\n",cpuTime,start->pcb->pid);
				cpuTime++;
				start->pcb->restBurstTime = burstTime;
				ttemp = start;
				if(start->next != NULL )  //�ڿ� ���� ���� process�� �ִٸ�.
				{
					cpu = 1;
					printf("--------------------------(context switch) \n");
					fprintf(out,"--------------------------(context switch) \n");
					start = start->next;
					max = start;
					ptr = start;
					while(ptr){
						if((max->pcb->age *  alpha + max->pcb->priority) < (ptr->pcb->age *  alpha + ptr->pcb->priority))
						{
							max = ptr;
							trail = oldptr;
						}
						oldptr = ptr;
						ptr = ptr->next;
					}
					if(max != start){
						temp = max->pcb;
						max->pcb = start->pcb;
						start->pcb = temp;
						if(start->pcb->notWorking != 1){
							responsetime += start->pcb->age;
							start->pcb->notWorking = 1;
						}
						if(max->next != NULL)
						{
							trail->next = max->next;
							rear->next = max;
							max->next = NULL;
							rear = max;
						}
					}
			
					//start = start->next;
					if(start->pcb->restBurstTime == NULL)
						burstTime = start->pcb->burst_Time;
					else
						burstTime = start->pcb->restBurstTime;
					free(ttemp);
					continue;
				}
				else{           //�ڿ� ���� ���� process�� ���ٸ�.
					start = NULL;
					free(ttemp);
					cpu = 0;    //cpu�� ��Եǰ�.
				}
			}

		}
	}
	printf("\nCPU USAGE : %f%%\n",(double)(cpuTime-cpuNull)/(double)cpuTime * 100);
	fprintf(out,"CPU USAGE : %f%%\n",(double)(cpuTime-cpuNull)/(double)cpuTime * 100);
	for(i = 0; i < 10; i++)
		waitingTime += pcb[i].age;
	printf("Average Waiting Time : %d \n",waitingTime/10);
	fprintf(out,"Average Waiting Time : %d \n",waitingTime/10);
	printf("Average of Response Time : %d \n",responsetime/10);
	fprintf(out,"Average of Response Time : %d \n",responsetime/10);
	for(i = 0; i < 10; i++)
		turnaround += pcb[i].age + pcb[i].burst_Time;
	printf("Average of TurnAround Time : %d \n",turnaround/10);
	fprintf(out,"Average of TurnAround Time : %d \n",turnaround/10);

	start = NULL;
	rear = NULL;
	for(i = 0; i < 10; i++){
		pcb[i].finish = 0;
		pcb[i].notWorking = 0;
		pcb[i].age = 0;
	}
	Fcount = 0;
}
int SETTING(int cputime,int cpu)
{
	struct READY_QUEUE *ptr;
	int num = 0;
	int result;       //result is check
	int pass = 0;  // 1 : �ּ� �Ѱ��� arrival time �� cpu time�� ���� ���.
	while(num < 10)
	{
		if(cputime == job_queue[num].pcb->arrival_Time)  
		{
			ptr = (struct READY_QUEUE*)malloc(sizeof(struct READY_QUEUE));
			if(ptr == NULL)  //defensive for malloc
				exit(1);
			ptr->pcb = job_queue[num].pcb;
			ptr->next = NULL;
			Fcount++;
			num++;
			result = 2;
			pass = 1;
			if(start != NULL){
				beforeRare = ptr;
				rear->next = ptr;
				rear = ptr;
			}
			else{
				start = ptr;
				rear = ptr;
			}
			continue;
		}
		else{
			if(pass == 1){
				num++;
			}
			else{
				num++;
				result = 1;
			}
		}
	}
	if(Fcount == 10)  //job queue�� ��� process�� ready queue�� �Ѿ.
	{
		if(cpu == 0)  //cpu time�� �� �������� arrival time ���� ũ�鼭, cpu�� �۵��� ��������.
			result = 0;
	}
	return result;
}	