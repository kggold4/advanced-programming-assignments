#include <stdio.h>
#include <utmp.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define SHOWHOST
void show_time(long time){
    char *cp;
    cp = ctime(&time);
    printf("%16.16s", cp);
}

void show_info(struct utmp *curr_rec){
    printf("%-8.8s", curr_rec->ut_user);
    printf("        ");
    if (strcmp(curr_rec->ut_user, "reboot")==0){
        printf("system boot ");
        printf("   ");
    }
    else{
        printf("%-8.8s", curr_rec->ut_line);
        printf("       ");
    }
    if(strcmp(curr_rec->ut_host, ":0") == 0 || strcmp(curr_rec->ut_host, ":1") == 0){
        printf("%s                   ", curr_rec->ut_host);
    }
    else{
        printf("%16.16s", curr_rec->ut_host);
        printf("     ");
    }
    show_time(curr_rec->ut_tv.tv_sec);
    printf("\n");
}

int main(int argc, char **argv){
    if (argc==1){
        printf("Please Insert Argument\n");
        exit(1);
    }
    int sum = atoi(argv[1]);
    struct utmp current_record;
    int wtmpfd;
    int reclen=sizeof(current_record);
    if ((wtmpfd=open(WTMP_FILE, O_RDONLY))==-1){
        perror(WTMP_FILE);
        exit(1);
    }
    int totalRec=0;
    while (read(wtmpfd, &current_record, reclen)==reclen){
        if(strcmp(current_record.ut_user,"runlevel")!= 0 && strcmp(current_record.ut_user,"shutdown")!=0 && strcmp(current_record.ut_user, "")!=0 && strcmp(current_record.ut_user, "LOGIN")!=0)
            totalRec+=1;
    }
    if(sum>totalRec | sum==0)
        sum=totalRec;
    int count=0;
    int numJumpBack=1;
    lseek(wtmpfd, -reclen, SEEK_END);
    while (read(wtmpfd, &current_record, reclen)==reclen&&count<sum){
        if(strcmp(current_record.ut_user,"runlevel")!= 0 && strcmp(current_record.ut_user,"shutdown")!=0 && strcmp(current_record.ut_user, "")!=0 && strcmp(current_record.ut_user, "LOGIN")!=0){
            show_info(&current_record);
            count++;
        }
        numJumpBack++;
        lseek(wtmpfd, -numJumpBack*reclen, SEEK_END);
    }
    close(wtmpfd);
    return 0;  
}
