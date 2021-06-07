#include <time.h>
#include <sys/types.h>
#include <signal.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h> 
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <stdlib.h>

#include <limits.h>

#define BLUE  "\x1B[34m"
#define WHITE "\033[0m"

int fd[2];

void add_null_term(char *txt)
{
for(int i=0;i<100;i++)
    if(txt[i]=='\n') {txt[i+1]=0;break;}
}

int overridemode=0;
void myfct(int y)
    {
    dup2(fd[0],STDIN_FILENO); //Overwrite userinput
    overridemode=1;
    }

char *depthfirstsearch(DIR *dir,int string, char* arg, char* ending, int f, int s, char* report, char* currentpath, int* messagelength)
{
    struct dirent *entry;
    char resultstring[1000];
    char nextpath[1000];
    int increment, answer, counter;
    FILE *fp;
    char buffer;
    char infile[30] =" in file ";
    if(dir == NULL)
    {
        printf("something went wrong\n");
        fflush(0);
        return "something";
    }
    if(string == 1)
    {
        strncpy(resultstring, "string ", 7);

    }
    else
    {
        strncpy(resultstring, "file ", 5);
    }

//case 1 - looking for a file name, in all sub-directories, file ending is irrelevant
    if(string != 1 && s == 1 && f != 1)
    {
        for(entry = readdir(dir); entry!=NULL; entry = readdir(dir))
        {
            if((strncmp(entry->d_name, "..", 2) ==0 && strlen(entry->d_name) == 2) || (strncmp(entry->d_name, ".", 1) == 0 && strlen(entry->d_name) == 1))
            {
                continue;
            }
            else if(entry->d_type == 4)
            {
                DIR *newdir = opendir(entry->d_name);

                strncpy(nextpath, currentpath, 1000);
                strcat(nextpath, "/");
                strcat(nextpath, entry->d_name);
                depthfirstsearch(newdir, string, arg, ending, f, s, report, nextpath, messagelength);
                closedir(newdir);
                continue;
            }
            if(strncmp((entry->d_name),arg,strlen(entry->d_name)) == 0 && entry->d_type != 4)
            {
                strcat(resultstring, entry->d_name);
                *messagelength += strlen(entry->d_name);

                strcat(resultstring, " found in directory ");
                *messagelength += 20;

                strcat(resultstring, currentpath);
                *messagelength += strlen(currentpath) + 1;

                strcat(resultstring, "\n");

                strcat(report, resultstring);

                memset(resultstring, '\0', sizeof(resultstring));
                if(string == 1)
                {
                    strncpy(resultstring, "string ", 7);
                    *messagelength += 7;    
                }
                else
                {
                    strncpy(resultstring, "file ", 5);
                    *messagelength += 5;
                }
            }
        }
    }

//case 2 - looking for a file name, in just the current directory, file ending is irrelevant
    else if(string != 1 && s != 1 && f != 1)
    {
        for(entry = readdir(dir); entry!=NULL; entry = readdir(dir))
        {
            if(strncmp((entry->d_name),arg,strlen(entry->d_name)) == 0 && entry->d_type != 4)
            {
                strcat(resultstring, entry->d_name);
                *messagelength += strlen(entry->d_name);

                strcat(resultstring, " found in directory ");
                *messagelength += 20;

                strcat(resultstring, currentpath);
                *messagelength += strlen(currentpath) + 1;
                strcat(resultstring, "\n");

                strcat(report, resultstring);

                memset(resultstring, '\0', sizeof(resultstring));
                if(string == 1)
                {
                    strncpy(resultstring, "string ", 7);
                    *messagelength += 7;    
                }
                else
                {
                    strncpy(resultstring, "file ", 5);
                    *messagelength += 5;
                }
            }
        }
    }

//case 3 - looking for a file name, in current directory, with the specified file ending
    else if(string != 1 && s != 1 && f == 1)
    {
        counter = 1;
        for(entry = readdir(dir); entry!=NULL; entry = readdir(dir))
        {
            if(strncmp((entry->d_name),arg,strlen(entry->d_name)) == 0 && entry->d_type != 4)
            {
                //check file ending
                answer = 0;
                if (strlen(entry->d_name) >= strlen(ending) + 1 && (entry->d_name)[strlen(entry->d_name) - strlen(ending) - 1] == '.')
                {
                    answer = 1;
                    counter = 0;
                    for(increment = strlen(entry->d_name) - strlen(ending); increment < strlen(entry->d_name); increment++)
                    {
                        if((entry->d_name)[increment] != ending[counter])
                        {
                            answer = 0;
                            break;
                        }
                        counter++;
                    }
                }

                if(strlen(ending) == 0)
                {
                    counter = 0;
                    for(increment = 0; increment < strlen(entry->d_name); increment++)
                    {
                        if((entry->d_name)[increment] == '.')
                        {
                            counter ++;
                            break;
                        }
                    }
                }
                if(counter == 0)
                {
                    answer = 1;
                }
                if(answer == 1)
                {
                    strcat(resultstring, entry->d_name);
                    *messagelength += strlen(entry->d_name);

                    strcat(resultstring, " found in directory ");
                    *messagelength += 20;

                    strcat(resultstring, currentpath);
                    *messagelength += strlen(currentpath) + 1;
                    strcat(resultstring, "\n");

                    strcat(report, resultstring);

                    memset(resultstring, '\0', sizeof(resultstring));
                    if(string == 1)
                    {
                        strncpy(resultstring, "string ", 7);
                        *messagelength += 7;

                    }
                    else
                    {
                        strncpy(resultstring, "file ", 5);
                        *messagelength += 5;
                    }
                }
            }
        }
    }

//case 4 - looking for a file name, in all sub-directories, with the specified file ending
    if(string != 1 && s == 1 && f == 1)
    {
        counter = 1;
        for(entry = readdir(dir); entry!=NULL; entry = readdir(dir))
        {
            if((strncmp(entry->d_name, "..", 2) == 0 && strlen(entry->d_name) == 2) || (strncmp(entry->d_name, ".", 1) == 0 && strlen(entry->d_name) == 1))
            {
                continue;
            }
            else if(entry->d_type == 4)
            {
                DIR *newdir = opendir(entry->d_name);
                strncpy(nextpath, currentpath, 1000);
                strcat(nextpath, "/");
                strcat(nextpath, entry->d_name);
                depthfirstsearch(newdir, string, arg, ending, f, s, report, nextpath, messagelength);
                closedir(newdir);
                continue;
            }
            if(strncmp((entry->d_name),arg,strlen(entry->d_name)) == 0 && entry->d_type != 4)
            {
                answer = 0;
                if (strlen(entry->d_name) >= strlen(ending) + 1 && (entry->d_name)[strlen(entry->d_name) - strlen(ending) - 1] == '.')
                {
                    answer = 1;
                    counter = 0;
                    for(increment = strlen(entry->d_name) - strlen(ending); increment < strlen(entry->d_name); increment++)
                    {
                        if((entry->d_name)[increment] != ending[counter])
                        {
                            answer = 0;
                            break;
                        }
                        counter++;
                    }
                }

                if(strlen(ending) == 0)
                {
                    counter = 0;
                    for(increment = 0; increment < strlen(entry->d_name); increment++)
                    {
                        if((entry->d_name)[increment] == '.')
                        {
                            counter ++;
                            break;
                        }
                    }
                }
                if(counter == 0)
                {
                    answer = 1;
                }


                if(answer == 1)
                {
                    strcat(resultstring, entry->d_name);
                    *messagelength += strlen(entry->d_name);

                    strcat(resultstring, " found in directory ");
                    *messagelength += 20;

                    strcat(resultstring, currentpath);
                    *messagelength += strlen(currentpath) + 1;
                    strcat(resultstring, "\n");

                    strcat(report, resultstring);
                    
                    memset(resultstring, '\0', sizeof(resultstring));
                    if(string == 1)
                    {
                        strncpy(resultstring, "string ", 7);
                        *messagelength += 7;

                    }
                    else
                    {
                        strncpy(resultstring, "file ", 5);
                        *messagelength += 5;
                    }
                }
            }
        }
    }

//case 5 - looking for a string inside a file, in current directory, file ending is irrelevant
    else if(string == 1 && s != 1 && f != 1)
    {
        for(entry = readdir(dir); entry!=NULL; entry = readdir(dir))
        {
            //read thru file and check if the string is in there
            if(entry->d_type != 4)
            {
                answer = 0;
                fp = fopen(entry->d_name, "r");
                if(fp == NULL)
                {
                    printf("file %s permission denied\n", entry->d_name);
                    fflush(0);
                    continue;
                }
                while(fread(&buffer, 1, 1, fp) == 1)
                {
                    if(arg[0] == buffer)
                    {
                        for(increment = 1; increment < strlen(arg); increment++)
                        {
                            if(fread(&buffer, 1, 1, fp) != 1)
                            {
                                answer = 0;
                                break;
                            }
                            if(arg[increment] != buffer)
                            {
                                answer = 0;
                                increment = (increment * -1) + 1;
                                fseek(fp, increment, SEEK_CUR);
                                break;
                            }
                            answer = 1;
                        }
                    }
                }
            }
            if(answer == 1)
            {
                strcat(resultstring, arg);
                *messagelength += strlen(arg);

                strcat(resultstring, " found in directory ");
                *messagelength += 20;

                strcat(resultstring, currentpath);
                *messagelength += strlen(currentpath) + 1;

                strcat(resultstring, infile);
                *messagelength += strlen(infile);

                strcat(resultstring, entry->d_name);
                *messagelength += strlen(entry->d_name);

                strcat(resultstring, "\n");

                strcat(report, resultstring);

                memset(resultstring, '\0', sizeof(resultstring));
                if(string == 1)
                {
                    strncpy(resultstring, "string ", 7);
                    *messagelength += 7;    
                }
                else
                {
                    strncpy(resultstring, "file ", 5);
                    *messagelength += 5;
                }
            }
        }
    }

//case 6 - looking for a string inside a file, in current directory, with specified file ending
    else if(string == 1 && s != 1 && f == 1)
    {
        counter = 1;
        for(entry = readdir(dir); entry!=NULL; entry = readdir(dir))
        {
            //read thru file and check if the string is in there
            if(entry->d_type != 4)
            {
                answer = 0;
                if (strlen(entry->d_name) >= strlen(ending) + 1 && (entry->d_name)[strlen(entry->d_name) - strlen(ending) - 1] == '.')
                {
                    answer = 1;
                    counter = 0;
                    for(increment = strlen(entry->d_name) - strlen(ending); increment < strlen(entry->d_name); increment++)
                    {
                        if((entry->d_name)[increment] != ending[counter])
                        {
                            answer = 0;
                            break;
                        }
                        counter++;
                    }
                }

                if(strlen(ending) == 0)
                {
                    counter = 0;
                    for(increment = 0; increment < strlen(entry->d_name); increment++)
                    {
                        if((entry->d_name)[increment] == '.')
                        {
                            counter ++;
                            break;
                        }
                    }
                }
                if(counter == 0)
                {
                    answer = 1;
                }


                if(answer == 1)
                {
                    answer = 0;
                    fp = fopen(entry->d_name, "r");
                    if(fp == NULL)
                    {
                        printf("file %s permission denied\n", entry->d_name);
                        fflush(0);
                        continue;
                    }
                    while(fread(&buffer, 1, 1, fp) == 1)
                    {
                        if(arg[0] == buffer)
                        {
                            for(increment = 1; increment < strlen(arg); increment++)
                            {
                                if(fread(&buffer, 1, 1, fp) != 1)
                                {
                                    answer = 0;
                                    break;
                                }
                                if(arg[increment] != buffer)
                                {
                                    answer = 0;
                                    increment = (increment * -1) + 1;
                                    fseek(fp, increment, SEEK_CUR);
                                    break;
                                }
                                answer = 1;
                            }
                        }
                    }
                }
            }

            if(answer == 1)
            {
                strcat(resultstring, arg);
                *messagelength += strlen(arg);

                strcat(resultstring, " found in directory ");
                *messagelength += 20;

                strcat(resultstring, currentpath);
                *messagelength += strlen(currentpath) + 1;

                strcat(resultstring, infile);
                *messagelength += strlen(infile);

                strcat(resultstring, entry->d_name);
                *messagelength += strlen(entry->d_name);

                strcat(resultstring, "\n");

                strcat(report, resultstring);

                memset(resultstring, '\0', sizeof(resultstring));
                if(string == 1)
                {
                    strncpy(resultstring, "string ", 7);
                    *messagelength += 7;    
                }
                else
                {
                    strncpy(resultstring, "file ", 5);
                    *messagelength += 5;
                }
            }
        }
    }

//case 7 - looking for a string inside a file, in all sub-directories, file ending is irrelevant
    else if(string == 1 && s == 1 && f != 1)
    {
        for(entry = readdir(dir); entry!=NULL; entry = readdir(dir))
        {
            //read thru file and check if the string is in there
            if((strncmp(entry->d_name, "..", 2) ==0 && strlen(entry->d_name) == 2) || (strncmp(entry->d_name, ".", 1) == 0 && strlen(entry->d_name) == 1))
            {
                continue;
            }
            else if(entry->d_type == 4) 
            {
                DIR *newdir = opendir(entry->d_name);
                strncpy(nextpath, currentpath, 1000);
                strcat(nextpath, "/");
                strcat(nextpath, entry->d_name);
                depthfirstsearch(newdir, string, arg, ending, f, s, report, nextpath, messagelength);
                closedir(newdir);
                continue;
            }
            if(entry->d_type != 4)
            {
                answer = 0;
                fp = fopen(entry->d_name, "r");
                if(fp == NULL)
                {
                    printf("file %s permission denied\n", entry->d_name);
                    fflush(0);
                    continue;
                }
                while(fread(&buffer, 1, 1, fp) == 1)
                {
                    if(arg[0] == buffer)
                    {
                        for(increment = 1; increment < strlen(arg); increment++)
                        {
                            if(fread(&buffer, 1, 1, fp) != 1)
                            {
                                answer = 0;
                                break;
                            }
                            if(arg[increment] != buffer)
                            {
                                answer = 0;
                                increment = (increment * -1) + 1;
                                fseek(fp, increment, SEEK_CUR);
                                break;
                            }
                            answer = 1;
                        }
                    }
                }
            }
            if(answer == 1)
            {
                strcat(resultstring, arg);
                *messagelength += strlen(arg);

                strcat(resultstring, " found in directory ");
                *messagelength += 20;

                strcat(resultstring, currentpath);
                *messagelength += strlen(currentpath) + 1;

                strcat(resultstring, infile);
                *messagelength += strlen(infile);

                strcat(resultstring, entry->d_name);
                *messagelength += strlen(entry->d_name);

                strcat(resultstring, "\n");

                strcat(report, resultstring);                

                memset(resultstring, '\0', sizeof(resultstring));
                if(string == 1)
                {
                    strncpy(resultstring, "string ", 7);
                    *messagelength += 7;    
                }
                else
                {
                    strncpy(resultstring, "file ", 5);
                    *messagelength += 5;
                }
            }
        }
    }

//case 8 - looking for a string inside a file, in all sub-directory, with specified file ending
    else if(string == 1 && s == 1 && f == 1)
    {
        counter = 1;
        for(entry = readdir(dir); entry!=NULL; entry = readdir(dir))
        {
            //read thru file and check if the string is in there
            if((strncmp(entry->d_name, "..", 2) ==0 && strlen(entry->d_name) == 2) || (strncmp(entry->d_name, ".", 1) == 0 && strlen(entry->d_name) == 1))
            {
                continue;
            }
            else if(entry->d_type == 4)
            {
                DIR *newdir = opendir(entry->d_name);
                strncpy(nextpath, currentpath, 1000);
                strcat(nextpath, "/");
                strcat(nextpath, entry->d_name);
                depthfirstsearch(newdir, string, arg, ending, f, s, report, nextpath, messagelength);
                closedir(newdir);
                continue;
            }
            if(entry->d_type != 4)
            {
                answer = 0;
                if (strlen(entry->d_name) >= strlen(ending) + 1 && (entry->d_name)[strlen(entry->d_name) - strlen(ending) - 1] == '.')
                {
                    answer = 1;
                    counter = 0;
                    for(increment = strlen(entry->d_name) - strlen(ending); increment < strlen(entry->d_name); increment++)
                    {
                        if((entry->d_name)[increment] != ending[counter])
                        {
                            answer = 0;
                            break;
                        }
                        counter++;
                    }
                }

                if(strlen(ending) == 0)
                {
                    counter = 0;
                    for(increment = 0; increment < strlen(entry->d_name); increment++)
                    {
                        if((entry->d_name)[increment] == '.')
                        {
                            counter ++;
                            break;
                        }
                    }
                }
                if(counter == 0)
                {
                    answer = 1;
                }

                if(answer == 1)
                {
                    answer = 0;
                    fp = fopen(entry->d_name, "r");
                    if(fp == NULL)
                    {
                        printf("file %s permission denied\n", entry->d_name);
                        fflush(0);
                        continue;
                    }
                    while(fread(&buffer, 1, 1, fp) == 1)
                    {
                        if(arg[0] == buffer)
                        {
                            for(increment = 1; increment < strlen(arg); increment++)
                            {
                                if(fread(&buffer, 1, 1, fp) != 1)
                                {
                                    answer = 0;
                                    break;
                                }
                                if(arg[increment] != buffer)
                                {
                                    answer = 0;
                                    increment = (increment * -1) + 1;
                                    fseek(fp, increment, SEEK_CUR);
                                    break;
                                }
                                answer = 1;
                            }
                        }
                    }
                }
            }
            if(answer == 1)
            {
                strcat(resultstring, arg);
                *messagelength += strlen(arg);

                strcat(resultstring, " found in directory ");
                *messagelength += 20;

                strcat(resultstring, currentpath);
                *messagelength += strlen(currentpath) + 1;

                strcat(resultstring, infile);
                *messagelength += strlen(infile);

                strcat(resultstring, entry->d_name);
                *messagelength += strlen(entry->d_name);

                strcat(resultstring, "\n");


                strcat(report, resultstring);

                memset(resultstring, '\0', sizeof(resultstring));
                if(string == 1)
                {
                    strncpy(resultstring, "string ", 7);
                    *messagelength += 7;    
                }
                else
                {
                    strncpy(resultstring, "file ", 5);
                    *messagelength += 5;
                }
            }
        }
    }
    return report;
}

int main()
{
    int counter;
    /* therse strings will be used to construct a formatted string for output */
    char thefile[30] = "the file ";
    char thestring[30] = "the string ";
    char currentdirect[30] = " in the current directory ";
    char alldirect[30] =" in all sub-directories ";
    char fileend[40] = "but only in files that end with ";

    
    int *childpids = mmap(0,sizeof(int)*10,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
    char *childmessages = mmap(0,sizeof(char)*10000,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
    int *messagelength = mmap(0,sizeof(int),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
    for(int i=0;i<10;i++) childpids[i]=0;

    signal(SIGUSR1,myfct);
    char input[1000];
    int parentPid = getpid();
    pipe(fd);
    int save_stdin = dup(STDIN_FILENO);
    while (1)
    {
        printf("%sfindstuff%s$ ", BLUE, WHITE);
        fflush(0);
        dup2(save_stdin,STDIN_FILENO);
        overridemode=0;
        read(STDIN_FILENO,input,1000);
        if(overridemode==0)
            add_null_term(input);//to get a NULL at the end of the string in case of a user input

        if (strncmp(input,"find",4) == 0)
        {
            //int sleepcount = input[5]-48; //ASCII conversion
            if (fork() == 0) //child process
            {
                char doingwhat[1000] ="Trying to find ";
                char arg[1000];
                char findending[100];
                char *currentdir = (char *)malloc(sizeof(char) * 1000);

                char* childreport = (char*)malloc(sizeof(char) * 10000);
                //search for an empty spot in the child list
                int kidnum=0;

                int s = 0;
                int f = 0; //initialize both tags as 0 (falsey)

                int searchstring = 0;

                int done = 0;
                int parencount = 0;


                for(int i=0;i<10;i++) 
                {
                    if(childpids[i]==0) 
                    {
                        childpids[i]=getpid();
                        break;
                    }
                    kidnum++;
                }
                if(kidnum == 10)
                {
                    printf("The limit of 10 kids has already been reached\n");
                    fflush(0);
                    return 0;
                }

                /* figure out if its a file or a string*/
                if(input[5] == '"')
                {
                    searchstring = 1;
                }

                /* check for -s and -f */
                counter = 5;
                while(input[counter] != 0 && input[counter] != 10)
                {
                    if (input[counter] == '-')
                    {
                        if(input[counter + 1] == 's')
                        {
                            s = 1;
                            done = 1;
                        }
                        else if(input[counter + 1] == 'f')
                        {
                            f = 1;
                            done = 1;
                            int x = counter + 3;
                            int y = 0;
                            while(input[x] != '-' && input[x] != 0 && input[x] != ' ' && input[x] != '\n')
                            {
                                //add input[x] to the end of findending[] 
                                findending[y] = input[x];
                                x++;
                                y++;
                                findending[y] = 0; //add null
                            }
                            
                        }
                        else
                        {
                            //printf("your flag algorithm (-s & -f) is messin up or the argument is misleading\n");
                        }
                    }
                    if(done == 0 && /*input[counter] != ' ' &&*/ input[counter] != '"')
                    {
                        arg[counter - 5 - parencount] = input[counter];
                    }
                    if(parencount == 0 && input[5] == '"')
                    {
                        parencount++;
                        counter++;
                        continue;
                    }
                    //on the first paren, we want to skip it, the second one we want a null char
                    counter++;
                    arg[counter - 5] = 0; //add null char
                }
            
                //get the doing what string put together here
                /* (the file/ the string) ___ (in the current directory/ in all sub-directories) (but only in files that end with ___)"*/
                if(searchstring == 1){strcat(doingwhat, thestring);}
                else{strcat(doingwhat, thefile);}

                strcat(doingwhat, arg);

                if(s == 1){strcat(doingwhat, alldirect);}
                else{strcat(doingwhat, currentdirect);}

                if(f == 1){strcat(doingwhat, fileend); strcat(doingwhat, findending);}

                strcat(doingwhat, "\n");

                //copy doingwhat onto the mmap childmessages
                strcpy(&childmessages[kidnum*1000], doingwhat);

                //printf("kid %d sleeps for %d seconds to indicate a search\n",kidnum,sleepcount);
                
                //sleep(sleepcount);
                sleep(2);

                //finding stuff here...

                DIR *dir;
                dir = opendir(".");
                getcwd(currentdir, 1000);

                depthfirstsearch(dir, searchstring, arg, findending, f, s, childreport, currentdir, messagelength);
        
                closedir(dir);
                //finding done.
                counter = 0;
                int flag = 0;
                if(strlen(childreport) == 0)
                {
                    flag = 1;
                    if(searchstring == 1)
                    {
                        strncpy(childreport, "string ", 7);
                        counter += 7;
                    }
                    else
                    {
                        strncpy(childreport, "file ", 5);
                        counter += 5;
                    }
                    strcat(childreport, arg);
                    counter += strlen(arg);

                    strcat(childreport, " not found.\n\0");
                    counter += 13;
                }

                close(fd[0]); //close read     

                strcat(childreport,"\0");//null terminator is important, because pipe
                if(flag == 1)
                {
                    write(fd[1],childreport,counter); //this works, not sure its a perfect solution
                }
                else
                {
                    write(fd[1],childreport,*messagelength);
                }
                
                close(fd[1]); //close write  
                kill(parentPid,SIGUSR1);
               
                childpids[kidnum] = 0;
                childmessages[kidnum * 1000] = '\0';

                //clear/free unneccesary memory
                free(currentdir);
                free(childreport);

                return 0;
            }
           
        }
        else if (strncmp(input,"list",4) == 0)
        {
            //Go thru all child processes
            counter =0;
            for(int i=0;i<10;i++) 
            {
                if(childpids[i]!=0)       
                {
                    counter++;
                    //list all child process, what they are trying to do and their serial number (PID?)
                    printf("-----------------------------------\n");
                    fflush(0);
                    printf("Child %d (PID = %d) Current Action:\n", i, childpids[i]);
                    fflush(0);
                    printf("%s", &childmessages[1000*i]);
                    fflush(0);
                    printf("-----------------------------------\n");
                    fflush(0);
                }
            }
            if(counter == 0)
            {
                printf("There are no kids to speak of...\n");
                fflush(0);
            }
        }

        else if (strncmp(input,"q",1) == 0 || strncmp(input,"quit",4) == 0)
        {
            for(int i=0;i<10;i++) 
            {
                if(childpids[i]!=0)       
                {
                    kill(childpids[i], SIGKILL);
                    childpids[i] = 0; /* not really neccessary */
                }
                kill(parentPid, SIGKILL);
            }
        }

        else if (strncmp(input,"kill",4) == 0)
        {
            //get the input and kill that process
            char str_number[50];
            counter = 5;
            while(input[counter] != 0)
            {
                str_number[counter - 5] = input[counter];
                counter++;
            }
            int killpid = atoi(str_number);
            printf("attempting to kill kid with pid %d\n",killpid);
            fflush(0);
            if(killpid != 0) // you dont want to actually be able to kill the parent here
            {
                kill(killpid, SIGKILL);
            }
            counter = 0;
            for(int i=0;i<10;i++) 
            {
                if(childpids[i] == killpid)       
                {
                    childpids[i] = 0;
                    counter++;
                }
            }
            if(counter > 0)
            {
                printf("kid killed\n");
                fflush(0);
            }
            else
            {
                printf("failed to kill\n");
                fflush(0);
            }

        }

        //killing the kid for good:
        for(int i=0;i<10;i++) 
        {
            if(childpids[i]!=0)       
            {
                waitpid(childpids[i],0,WNOHANG);
            }
        }
        printf("input check: %s",input);
    }
    munmap(childpids, 10*sizeof(int));
    munmap(childmessages, sizeof(char)*10000);
    munmap(messagelength, sizeof(int));
    return 0;
}
