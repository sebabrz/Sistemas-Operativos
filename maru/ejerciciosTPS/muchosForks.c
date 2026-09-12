#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
pid_t pid=
/* fork a child process */
pid=fork(); 
if(pid==0){
    execlp("/bin/ls", "ls", NULL);
    return 0;
}
/* fork another child process */
fork();
/* and fork another */
fork();
printf("Proceso PID=%d, hijo de PID=%d\n", getpid(), getppid());//esto me alcanza para ver el resultado porque todos lo van a ejecutar
return 0;
}
