#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>

void  Poor_Student_Process(int []);
void  Dear_Old_Dad_Process(int []);

int  main(int  argc, char *argv[])
{
  int    ShmID;
  int    *ShmPTR;
  pid_t  pid;
  int    status;
  
     ShmID = shmget(IPC_PRIVATE, 4*sizeof(int), IPC_CREAT | 0666);
     if (ShmID < 0) {
          printf("*** shmget error (server) ***\n");
          exit(1);
     }
     printf("Main has received a shared memory of four integers...\n");

     ShmPTR = (int *) shmat(ShmID, NULL, 0);
     if (*ShmPTR == -1) {
          printf("*** shmat error (server) ***\n");
          exit(1);
     }
     printf("Main has attached the shared memory...\n");

     printf("Server has filled %d %d %d %d in shared memory...\n",
            ShmPTR[0], ShmPTR[1], ShmPTR[2], ShmPTR[3]);

     ShmPTR[0] = 0; //BANKACCOUNT
     ShmPTR[1] = 0;    //TURN

     printf("Server is about to fork a child process...\n");
     pid = fork();
     if (pid < 0) {
          printf("*** fork error (main) ***\n");
          exit(1);
     }
     else if (pid == 0) { //child process --> poor student 
          Poor_Student_Process(ShmPTR);
          exit(0);
     }
     else{//Parent process --> old dad
       Dear_Old_Dad_Process(ShmPTR);
     }

     wait(&status);
     printf("main has detected the completion of its child...\n");
     shmdt((void *) ShmPTR);
     printf("main has detached its shared memory...\n");
     shmctl(ShmID, IPC_RMID, NULL);
     printf("main has removed its shared memory...\n");
     printf("main exits...\n");
     exit(0);
}

void  Poor_Student_Process(int  SharedMem[])
{
  int i;
  int account;
  int balance;
  
  srand(getpid()); //seed the rand num generator
  
  for(i = 0; i < 25; i++){
    //sleep from 0 to 5
    sleep(rand()%6); 
    
    account = SharedMem[0];
    
    while(SharedMem[1] != 1); //loop do nothing
    
    balance = rand()%50; // generate a number 0-100
    
    printf("Poor Student needs $%d\n", balance);
    
    if(balance > account){
        printf("Poor Student: Not Enough Cash ($%d)\n", account );
    }
    SharedMem[0] = account; //copy local account to bank
    SharedMem[1] = 0; //set turn = 1
    
  }   
}

void  Dear_Old_Dad_Process(int  SharedMem[])
{
  int i;
  int account;
  int randomVal;
  
  srand(getpid()); // seed the rand num generator
  for(i = 0; i < 25; i++){
    // sleep from 0 to 5
    sleep(rand()%6); 
    
    account = SharedMem[0];
    
    while(SharedMem[1] != 0); //loop do nothing
    
    if (account <= 100){ //Deposit Money;
      
      randomVal = rand()%10; // generate a number 0-100
      
      if(randomVal%2==0){//if even
        account += randomVal;
        printf("Dear old Dad: Deposits $%d / Balance = $%d\n", SharedMem[0], account);
      }
      else{
        printf("Dear old Dad: Doesn't have any money to give\n");
      }
      
      SharedMem[0] = account; //copy local account to bank
      SharedMem[1] = 1; //set turn = 1
    }
    else{
      printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", account);
    }
  }
  
  
}