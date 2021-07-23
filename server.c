#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include<time.h>
#include "Customer.c"
#include "Account.c"
#include "Transaction.c"

#define PORT 9679
int main();
void mainMenu(int socket_desc);
void deposit(int socket_desc,struct Customer acc);
void choice_entered_by_admin(int socket_desc,struct Customer customer_login);
void valid_add(int socket_desc,struct Customer customer_login);
int checkAccountExists(char name[]);
void delete_user(int socket_desc,struct Customer customer_login);
void view_details(int socket_desc,struct Customer customer_login);
void view_details_with_accountnum(int socket_desc,struct Customer customer_login);
int choose_bw_name_and_account(int socket_desc);
int check_Accountnum_exists(long int acc_num);
void password_change(int socket_desc,struct Customer cust);
void delete_account(int socket_desc,struct Customer customer_login);
void verify_login_credentials(int socket_desc);
void choice_entered_by_normal_user(int socket_desc,struct Customer customer_login);
void view_cust_details(int socket_desc,struct Customer cust);




void initialize_db(){
    int fd1 = open("customer_db.txt", O_CREAT|O_EXCL|O_RDWR, 0755);
    int fd2 = open("account_db.txt", O_CREAT|O_EXCL|O_RDWR, 0755);
    int fd3 = open("transaction_db.txt", O_CREAT|O_EXCL|O_RDWR, 0755);
    if(fd1 != -1){
        struct Customer admin;
        strcpy(admin.username, "admin");
        strcpy(admin.password, "admin");
        //strcpy();
        //admin.account_number=0;
        admin.type = true;
        admin.status = true;
        write(fd1, &admin, sizeof(admin));
        close(fd1);close(fd2);close(fd3);
    }
    
}
void valid_add(int socket_desc,struct Customer customer_login){
    int fd_cust=open("customer_db.txt", O_RDONLY);
    struct Customer customer_add;
    struct Customer record;
    
    int invalid = 1;
    while(invalid){
        //printf("%d\n", invalid);
        read(socket_desc,&customer_add,sizeof(customer_add));
        printf("%s\n", customer_add.username);
        invalid = 0;
        while(read(fd_cust,&record,sizeof(record))){
            if(strcmp(customer_add.username,record.username) == 0){
                invalid = 1;
                write(socket_desc, &invalid,sizeof(int));// will send 1
                break;
            }
            else
            {
            	break;
            }
        }
        lseek(fd_cust, 0, SEEK_SET);
    }
    write(socket_desc, &invalid,sizeof(int));//will send 0
    close(fd_cust);

    char password[512]="";
    read(socket_desc,password,sizeof(password));
    //printf("%s\n", password);
    strcpy(customer_add.password, password);
    long int acc_num = customer_add.account_number = time(NULL);
    printf("%ld\n", acc_num);
    customer_add.type = false;
    customer_add.status = true;

    fd_cust=open("customer_db.txt", O_WRONLY|O_APPEND);

    write(fd_cust,&customer_add,sizeof(customer_add));
    close(fd_cust);

    struct Account account_initialize;
    int fd2=open("account_db.txt", O_WRONLY|O_APPEND);
    if(fd2!=-1){

        //while(read(fd1,))
        lseek(fd2, 0, SEEK_SET);                                     //Change this logic forjoint acc if accnum exists dont add
        account_initialize.account_number=acc_num;
        account_initialize.balance=0.0;
        account_initialize.status=true;
        write(fd2,&account_initialize,sizeof(account_initialize));
    }
    write(socket_desc,&customer_add,sizeof(customer_add));
    write(socket_desc,&account_initialize,sizeof(account_initialize));
    choice_entered_by_admin(socket_desc,customer_login);
}

int checkAccountExists(char name[])
{
    struct Customer customer;

    int fd_cust = open("customer_db.txt", O_RDONLY);
    
    while(read(fd_cust, &customer, sizeof(customer)))
    {

        if(!strcmp(customer.username, name) && customer.status!=false)
        {
            close(fd_cust);//exists
            return 1;
        }
    }
    close(fd_cust);
    return 0;    
}


void delete_user(int socket_desc,struct Customer customer_login){
    //printf("--------------\n");
    char name[512]="";
    read(socket_desc,name,sizeof(name));
    //printf("------%s\n",name);
    char msg[100]="";

    if(!checkAccountExists(name))
    {
        strcpy(msg, "Account does not exist");
        write(socket_desc, msg, sizeof(msg));
    }
    else
    {   
        int fd_cust=open("customer_db.txt", O_RDWR);
        struct  Customer customer;
        while(read(fd_cust, &customer, sizeof(customer))){
            if(!strcmp(customer.username, name))
            {
                customer.status=false; 
                lseek(fd_cust,-sizeof(customer),SEEK_CUR); 
                write(fd_cust,&customer, sizeof(customer));

                break;             
            }
        }
        printf("%s\n",customer.username);
        printf("%ld\n",customer.account_number);
        int fd_acc=open("account_db.txt",O_RDWR);
        struct Account acc;
        long int acc_num=customer.account_number;
        while(read(fd_acc,&acc,sizeof(acc))){
           if(acc_num==acc.account_number)
           {
            acc.status=false; 
            lseek(fd_acc,-sizeof(acc),SEEK_CUR); 
            write(fd_acc,&acc, sizeof(acc));
            break; 
            }
        }
        /*
            set .status=false;
        */
        close(fd_cust);
        close(fd_acc);
        strcpy(msg, "Account deleted Successfully");
        write(socket_desc, msg, sizeof(msg));
    }
choice_entered_by_admin(socket_desc,customer_login);
}

void view_details(int socket_desc,struct Customer customer_login){
    char name[512]="";
    read(socket_desc,name,sizeof(name));
    printf("%s\n",name);
    char msg[100]="";
    int fd_acc;
    if(!checkAccountExists(name))
    {
        strcpy(msg, "Account does not exist");
        write(socket_desc, msg, sizeof(msg));
        //choice_entered_by_admin(socket_desc,customer_login);
    }
    else
    {   
        strcpy(msg, "");
        write(socket_desc, msg, sizeof(msg));
        int fd_cust=open("customer_db.txt", O_RDONLY);
        struct  Customer customer;
        
        //lseek(fd_cust,0,SEEK_SET);
        while(read(fd_cust, &customer, sizeof(customer))){
            if(!strcmp(customer.username, name))
            {
                write(socket_desc,&customer, sizeof(customer));
                fd_acc=open("account_db.txt",O_RDONLY);
                struct Account acc;
                while(read(fd_acc,&acc,sizeof(acc))){
                    if(customer.account_number==acc.account_number){
                        write(socket_desc,&acc,sizeof(acc));
                        break;
                    }
                }
                close(fd_acc);
                break;             
            }
        }
        close(fd_cust);
        
    }
    choice_entered_by_admin(socket_desc,customer_login);
}


void view_details_with_accountnum(int socket_desc,struct Customer customer_login)
{
    long int acc_num;
    read(socket_desc,&acc_num,sizeof(acc_num));
    printf("%ld\n",acc_num);
    char msg[100]="";

    if(!check_Accountnum_exists(acc_num))
    {
        strcpy(msg, "Account does not exist");
        write(socket_desc, msg, sizeof(msg));
        //choice_entered_by_admin(socket_desc,customer_login);

    }
    else
    {   
        strcpy(msg, "");
        write(socket_desc, msg, sizeof(msg));
        int fd_cust=open("customer_db.txt", O_RDONLY);
        int fd_acc=open("account_db.txt", O_RDONLY);
        struct  Customer customer;
        struct Account acc;
        //lseek(fd_cust,0,SEEK_SET);
        bool flag=true;
        while(read(fd_cust, &customer, sizeof(customer))){
            if(acc_num==customer.account_number && customer.status==true)
            {
                write(socket_desc,&flag,sizeof(flag));
                write(socket_desc,&customer,sizeof(customer));
            }
        }
        close(fd_cust);
        flag=false;
        write(socket_desc,&flag,sizeof(flag));
        while(read(fd_acc,&acc,sizeof(acc))){
            if(acc_num==acc.account_number ){
                write(socket_desc,&acc,sizeof(acc));
                break;
            }
        }
        close(fd_acc);
    }
    choice_entered_by_admin(socket_desc,customer_login);
}

int choose_bw_name_and_account(int socket_desc){
    int n;

    read(socket_desc,&n,sizeof(n));
    printf("%d\n",n);
    return n;
}

int check_Accountnum_exists(long int acc_num)
{
   struct Customer customer;
   int fd_cust = open("customer_db.txt", O_RDONLY);

   while(read(fd_cust, &customer, sizeof(customer)))
   {
    if(acc_num==customer.account_number && customer.status!=false)
        {
            close(fd_cust);
            return 1;
        }
    }
close(fd_cust);
return 0; 
}

void delete_account(int socket_desc,struct Customer customer_login){
    long int acc_num;
    read(socket_desc,&acc_num,sizeof(acc_num));
    char msg[100]="";

    if(!check_Accountnum_exists(acc_num)){
        strcpy(msg, "Account does not exist");
        write(socket_desc, msg, sizeof(msg));
    }
    else{
        strcpy(msg, "");
        write(socket_desc, msg, sizeof(msg));
        int fd_cust=open("customer_db.txt", O_RDWR);
        int fd_acc=open("account_db.txt",O_RDWR);
        struct  Customer customer;
        struct Account acc;
        
        bool flag=true;
        while(read(fd_cust, &customer, sizeof(customer))){
            if(acc_num==customer.account_number)
            {
                write(socket_desc,&flag,sizeof(flag));
                customer.status=false; 
                lseek(fd_cust,-sizeof(customer),SEEK_CUR); 
                write(fd_cust,&customer, sizeof(customer));
            }
            
        close(fd_cust);
        flag=false;
        write(socket_desc,&flag,sizeof(flag));
        }
        while(read(fd_acc,&acc,sizeof(acc))){
           if(acc_num==acc.account_number)
           {
            acc.status=false; 
            lseek(fd_acc,-sizeof(acc),SEEK_CUR); 
            write(fd_acc,&acc, sizeof(acc));
            break; 
            }
        }
        close(fd_acc);
        strcpy(msg, "Account deleted Successfully");
        write(socket_desc, msg, sizeof(msg));
    }
choice_entered_by_admin(socket_desc,customer_login);
}
void modify_acc(int socket_desc,struct Customer cust)
{
    long int acc_num;
    read(socket_desc,&acc_num,sizeof(acc_num));
    struct Customer customer;
    int fd_cust=open("customer_db.txt",O_RDONLY);
    int count=0;
    struct Customer customer_add;
        struct Customer record;
         
    while(read(fd_cust,&customer,sizeof(customer)))
    {
        if(acc_num==customer.account_number && customer.status==true)
        {
            count++;
        }
    }
    close(fd_cust);
    if(count!=1)
    {
        char msg[100]="";
        strcpy(msg,"This account have 2 active users cannot add more");
        write(socket_desc,msg,sizeof(msg));
    }
    else
    {
        char msg[100]="";
        strcpy(msg,"This account can be modified");
        write(socket_desc,msg,sizeof(msg));
        
         fd_cust=open("customer_db.txt",O_RDONLY);
        int invalid = 1;
        while(invalid){
            //printf("%d\n", invalid);
            read(socket_desc,&customer_add,sizeof(customer_add));
            printf("%s\n", customer_add.username);
            invalid = 0;
            while(read(fd_cust,&record,sizeof(record))){
                if(strcmp(customer_add.username,record.username) == 0){
                    invalid = 1;
                    write(socket_desc, &invalid,sizeof(int));// will send 1
                    break;
                }
            }
            lseek(fd_cust, 0, SEEK_SET);
        }
    write(socket_desc, &invalid,sizeof(int));//will send 0
    close(fd_cust);

    char password[512]="";
    read(socket_desc,password,sizeof(password));
    //printf("%s\n", password);
    strcpy(customer_add.password, password);
    customer_add.account_number=acc_num;
    customer_add.type = false;
    customer_add.status = true;

    fd_cust=open("customer_db.txt", O_WRONLY|O_APPEND);
    write(fd_cust,&customer_add,sizeof(customer_add));
    close(fd_cust);

    }
    write(socket_desc,&customer_add,sizeof(customer_add));
    choice_entered_by_admin(socket_desc,customer_add);

}

void choice_entered_by_admin(int socket_desc,struct Customer customer_login)
{
    int read_choice;
    int n;
    read(socket_desc,&read_choice,sizeof(read_choice));
    printf("Entered choice =%d\n",read_choice );
    if(read_choice > 5 || read_choice < 1){
        //mainMenu(socket_desc);
        exit(0);
    }
    switch(read_choice)
    {
        case 1: valid_add(socket_desc,customer_login);
                break;
        case 2: n=choose_bw_name_and_account(socket_desc);
                switch(n)
                {
                    case 1: delete_user(socket_desc,customer_login); break;
                    case 2: delete_account(socket_desc,customer_login);break;
                }   
                break;
        case 3: n=choose_bw_name_and_account(socket_desc);
                switch(n)
                {
                    case 1: view_details(socket_desc,customer_login); break;
                    case 2: view_details_with_accountnum(socket_desc,customer_login);break;
                }   
                break;
        case 4: modify_acc(socket_desc,customer_login);break;
        default: mainMenu(socket_desc);break; 
    }
}

void deposit(int socket_desc, struct Customer cust)
{
    //printf("Hello %s\n",cust.username);
    double amount;
    double prev_bal,new_bal;

    printf("%ld\n",cust.account_number);
    read(socket_desc, &amount, sizeof(amount));

    struct Account account;
    struct Transaction transaction;

    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();
    time_t cur_time;
    struct tm *local;
    cur_time=time(NULL);

    local=localtime(&cur_time);
    //printf("%s\n",asctime(local));
    
    int fd_acc=open("account_db.txt",O_RDWR);
    int fd_trans=open("transaction_db.txt",O_RDWR|O_APPEND);
    int fcn_acc=fcntl(fd_acc, F_SETLKW, &lock);
    int fcn_trans=fcntl(fd_trans, F_SETLKW, &lock);
    if(fcn_acc<0||fcn_trans<0){perror("FCNTL::");}
    
    while(read(fd_acc,&account,sizeof(account))){
        if(cust.account_number==account.account_number)
        { 
            prev_bal=account.balance;
            write(socket_desc,&prev_bal,sizeof(prev_bal));
            new_bal=prev_bal+amount;
            write(socket_desc,&new_bal,sizeof(new_bal));
            account.balance=new_bal;
            lseek(fd_acc,-sizeof(account),SEEK_CUR);
            write(fd_acc,&account,sizeof(account));

            transaction.balance=new_bal;
            transaction.amount=amount;
            transaction.account_number=cust.account_number;
            strcpy(transaction.date,asctime(local));
            //transaction.date="";//int gettimeofday(struct timeval *tv, struct timezone *tz);
            write(fd_trans,&transaction,sizeof(transaction));
        }
    }
    sleep(20);
    
    lock.l_type = F_UNLCK;
    int fl=fcntl(fd_acc, F_SETLKW, &lock);
    int fl2=fcntl(fd_trans, F_SETLKW, &lock);
    if(fl<0 || fl2<0){perror("ERROR:");}
    close(fd_trans);
    close(fd_acc);
    choice_entered_by_normal_user(socket_desc,cust);
}
/////////////////////////////////////////////////////////////////////////////////////////////////
void withdraw(int socket_desc, struct Customer cust)
{
    //printf("Hello %s\n",cust.username);
    double amount;
    double prev_bal,new_bal;

    printf("%ld\n",cust.account_number);
    read(socket_desc, &amount, sizeof(amount));

    struct Account account;
    struct Transaction transaction;
    struct Transaction trans;

    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();
    time_t cur_time;
    struct tm *local;
    cur_time=time(NULL);

    local=localtime(&cur_time);
    //printf("%s\n",asctime(local));
    
    int fd_acc=open("account_db.txt",O_RDWR);
    int fd_trans=open("transaction_db.txt",O_RDWR|O_APPEND);
    int fcn_acc=fcntl(fd_acc, F_SETLKW, &lock);
    int fcn_trans=fcntl(fd_trans, F_SETLKW, &lock);
    if(fcn_acc<0||fcn_trans<0){perror("FCNTL::");}
    while(read(fd_acc,&account,sizeof(account))){
        if(cust.account_number==account.account_number)
        { 
            prev_bal=account.balance;
            write(socket_desc,&prev_bal,sizeof(prev_bal));
            if(prev_bal>=amount){
                new_bal=prev_bal-amount;
                write(socket_desc,&new_bal,sizeof(new_bal));
                account.balance=new_bal;
                lseek(fd_acc,-sizeof(account),SEEK_CUR);
                write(fd_acc,&account,sizeof(account));

                transaction.balance=new_bal;
                transaction.amount=amount;
                transaction.account_number=cust.account_number;
                strcpy(transaction.date,asctime(local));
                write(fd_trans,&transaction,sizeof(transaction));
            }
            
        }
    }
    sleep(20);
    
    lock.l_type = F_UNLCK;
    int fl=fcntl(fd_acc, F_SETLKW, &lock);
    int fl2=fcntl(fd_trans, F_SETLKW, &lock);
    if(fl<0 || fl2<0){perror("ERROR:");}
    close(fd_trans);
    close(fd_acc);
    choice_entered_by_normal_user(socket_desc,cust);
}
void view_cust_details(int socket_desc,struct Customer cust)
{
    //printf("Hello %s\n",cust.username);
    int fd_acc=open("account_db.txt",O_RDONLY);
    int fd_trans=open("transaction_db.txt",O_RDONLY);
    int fd_cust=open("customer_db.txt",O_RDONLY);
    struct Account account;
    //struct Transaction transaction;
    struct Customer customer;


    while(read(fd_acc,&account,sizeof(account))){
        if(cust.account_number==account.account_number)
        {
            write(socket_desc,&account,sizeof(account));
            break;
        }
    }
    while(read(fd_cust,&customer,sizeof(customer)))
    {
        if(!strcmp(cust.username,customer.username))
        {
            write(socket_desc,&customer,sizeof(customer));
            break;
        }
    }
    choice_entered_by_normal_user(socket_desc,cust);
}

void password_change(int socket_desc,struct Customer cust){

    char pass[512]="";
    char new_pass[512]="";
    printf("%s\n",cust.password);
    read(socket_desc,&pass,sizeof(pass));
    int n;
    

    if(!strcmp(pass,cust.password))
    {
        struct flock lock;
        lock.l_type = F_WRLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = 0;
        lock.l_len = 0;
        lock.l_pid = getpid();
        time_t cur_time;
        struct tm *local;
        cur_time=time(NULL);

        local=localtime(&cur_time);
        n=1;
        write(socket_desc,&n,sizeof(n));
        read(socket_desc,&new_pass,sizeof(new_pass));
        struct Customer customer;
        int fd_cust=open("customer_db.txt",O_RDWR);
        int fcn_cust=fcntl(fd_cust, F_SETLKW, &lock);
        while(read(fd_cust,&customer,sizeof(customer))){
            if(!strcmp(customer.username,cust.username))
            {
                lseek(fd_cust,-sizeof(customer),SEEK_CUR);
                strcpy(customer.password,new_pass);
                write(fd_cust,&customer,sizeof(customer));
                break;
            }
        }
        lock.l_type = F_UNLCK;
        int fl=fcntl(fd_cust, F_SETLKW, &lock);
    }
    else{
       write(socket_desc,&n,sizeof(n));

   }
   choice_entered_by_normal_user(socket_desc,cust);
}
void get_transactions(int socket_desc,struct Customer cust)
{
    printf("%ld\n",cust.account_number);
    struct Transaction trans;
    int fd_trans=open("transaction_db.txt",O_RDONLY);

    bool flag=true;
        while(read(fd_trans, &trans, sizeof(trans))){
            if(cust.account_number==trans.account_number)
            {
                write(socket_desc,&flag,sizeof(flag));
                write(socket_desc,&trans, sizeof(trans));
            }  
        }       
        flag=false;  
        write(socket_desc,&flag,sizeof(flag));
        close(fd_trans);
        choice_entered_by_normal_user(socket_desc,cust);        
}

void choice_entered_by_normal_user(int socket_desc,struct Customer customer_login){
    int read_choice;
    int n;
    long int acc_num;
    // printf("-----------------------------------------------%s\n",customer_login.username);
    struct Customer cust;
    struct Account acc;
    int fd_cust=open("customer_db.txt",O_RDONLY);
    while(read(fd_cust,&cust,sizeof(cust))){
        if(!strcmp(customer_login.username,cust.username))
        {
                //acc_num=cust.account_number;
                //printf("%ld\n",acc_num);
            write(fd_cust,&cust,sizeof(cust));
            break;
        }
    }
    //close(fd_acc);
    int fd_trans=open("transaction_db.txt",O_RDONLY);

    read(socket_desc,&read_choice,sizeof(read_choice));
    printf("Entered choice =%d\n",read_choice );
    switch(read_choice)
    {
        case 1: deposit(socket_desc,cust);break;
        case 2: withdraw(socket_desc,cust);break;
        case 3: view_cust_details(socket_desc,cust); break;
        case 4: password_change(socket_desc,cust);break;
        case 5: get_transactions(socket_desc,cust);break;
        default:mainMenu(socket_desc);break;
    }
}


void verify_login_credentials(int socket_desc){
    struct Customer customer_login;
    struct Customer record;
    read(socket_desc, &customer_login, sizeof(customer_login));
    //printf("%s\t%s\n", customer_login.username, customer_login.password);
    int fd = open("customer_db.txt", O_RDONLY);
    char message[512]="";
    
    while(read(fd, &record, sizeof(record))){
        //printf("%s\t%s\t%d\n", record.username, record.password, record.status);
        //printf("%d\t%d\n", strcmp(record.username, customer_login.username), 
          //  strcmp(record.password, customer_login.password));

        if(strcmp(record.username, customer_login.username) == 0 && 
            strcmp(record.password, customer_login.password) == 0 && record.status==true){

            printf("successful\n");
        strcpy(message, "Validation successful");
        write(socket_desc, message, sizeof(message));

        if(record.type==true){
            choice_entered_by_admin(socket_desc,customer_login);
            }
        else{
            printf("NORMAL USER\n");
            choice_entered_by_normal_user(socket_desc,customer_login);
	}
            
            
            return;
        }
    }
    printf("unsuccessful\n");
    strcpy(message, "Unknown user...\nExiting the execution\n");
    write(socket_desc, message, sizeof(message));

}
void mainMenu(int socket_desc)
{
    verify_login_credentials(socket_desc);
}


int main(){
    struct sockaddr_in server, client;
    int socket_desc, size_client, client_desc;
    socket_desc = socket(AF_INET,SOCK_STREAM,0);
    initialize_db();
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);
    int b=bind(socket_desc, (void *)(&server), sizeof(server));
    if(b<0){perror("Error at binding: ");}
    listen(socket_desc,3);//3 is max limit of clients,server can handle at once

    while(1){
        size_client = sizeof(client);
        client_desc = accept(socket_desc, (void *)(&client),&size_client);
        if(fork() == 0){
            mainMenu(client_desc);
            exit(0);
        }else{
            close(client_desc);
        }
    } 
    close(client_desc);
    close(socket_desc);
    return 0;
}
