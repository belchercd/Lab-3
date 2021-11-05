// C program to demonstrate use of fork() and pipe() 
#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 
  
int main() 
{ 
  // We use two pipes 
  // First pipe --> send input string from parent
  // Second pipe --> send concatenated string from child 
  int fd1[2];  // Used to store two ends of first pipe 
  int fd2[2];  // Used to store two ends of second pipe 
  
  char fixed_str[] = "howard.edu";
  char fixed_str2[] = "gobison.org";
  char input_str[100]; //www.geeks
  char input_str2[100]; //www.geekshoward.edu
  
  pid_t p; 
  
  if (pipe(fd1)==-1){
    fprintf(stderr, "Pipe Failed" );
    return 1;
  }
  if (pipe(fd2)==-1){
    fprintf(stderr, "Pipe Failed" );
    return 1;
  }
  
  printf("Enter a string to concatenate:");
  scanf("%s", input_str); //ex www.geeks
  
  p = fork();
  
  if (p < 0){
    fprintf(stderr, "fork Failed" );
    return 1;
  }
  
  // Parent process
  else if (p > 0){
    close(fd1[0]);  // Close reading end of first pipe
    
    // Write input string and close writing end of first pipe. 
    write(fd1[1], input_str, strlen(input_str)+1);  //writing to child 
        
    // Wait for child to send a string 
    wait(NULL); //jump to child process
    
    // Read a second string using second pipe 
    char concat_str2[100]; 
    read(fd2[0], concat_str2, 100); //reading www.geekshoward.edu
    
    // Concatenate fixed string 2 with second input string 
    int l = strlen(concat_str2); 
    int j;
    for (j=0; j<strlen(fixed_str2); j++){
      concat_str2[l++] = fixed_str2[j];
    }
    
    concat_str2[l] = '\0';   // string ends with '\0'
        
    printf("Parent: Concatenated string %s\n", concat_str2);
    
    // Close writing end of pipes
    close(fd2[1]);  
    close(fd1[1]);
  }
  
  // child process 
  else{
    close(fd1[1]);  // Close writing end of first pipes
      
    // Read a string using first pipe 
    char concat_str[100]; 
    read(fd1[0], concat_str, 100); //reading www.geeks
  
    // Concatenate a fixed string with it 
    int k = strlen(concat_str); 
    int i;
    for (i=0; i<strlen(fixed_str); i++){
      concat_str[k++] = fixed_str[i];
    }
    
    concat_str[k] = '\0';   // string ends with '\0'
    
    printf("Child: Concatenated string %s\n", concat_str);
    
    printf("Enter a string to concatenate:");
    scanf("%s", input_str2); //ex www.geekshoward.edu
    
    write(fd2[1], input_str2, strlen(input_str2)+1); //writing www.geekshoward.edu
    
    // Close both reading ends 
    close(fd1[0]); 
    close(fd2[0]); 

    exit(0); //once child exists it jumps back to parent 
  } 
} 