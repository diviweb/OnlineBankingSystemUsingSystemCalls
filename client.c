#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdbool.h>
#include <string.h>
#include<ctype.h>
#include<unistd.h>
#include <arpa/inet.h>
#include "Customer.c"
#include "Account.c"
#include "Transaction.c"

#define PORT 9679

void adminOptions(int admin_desc,struct Customer customer_login);
int choose_bw_name_and_account(int socket_desc);
void normal_user_options(int socket_desc,struct Customer customer_login);
int main();
void main_menu(int socket_desc);

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void add_new_customer(int add_desc,struct Customer customer_login){
    char buffer[512]="";
    int buf_cust,buf_acc;
    struct Customer customer;
    printf("-----------------------ADD A NEW CUSTOMER-----------------------\n");
    //while
    int invalid = 1;
    while(invalid){
        printf("Enter the name: ");
        scanf("%s",customer.username);
        write(add_desc,&customer,sizeof(customer));
        read(add_desc,&invalid,sizeof(int));
        //printf("%d\n", invalid);
        if(invalid==1)
        {
            printf("\n\n------------------------------------------------------------\n");
            printf("NAME ALREADY EXISTS IN THE DATABASE....\n--------------------PLEASE TRY AGAIN--------------------\n\n\n");
        }
    }

    char password[512]="";
    printf("Enter the password: ");
    scanf("%s", password);
    
    write(add_desc,password,sizeof(password));
    
    read(add_desc,&customer,sizeof(customer));
    struct Account acc;
    read(add_desc,&acc,sizeof(acc));
    printf("User added successfully...\n");
    printf("Welcome %s,\tYour account number = %ld,\tand the balance= %.2f\n",customer.username, acc.account_number,acc.balance);
    sleep(2);
    adminOptions(add_desc,customer_login);
}

void delete_user(int socket_desc,struct Customer customer_login)
{
    char name[512]="";
    char buf[100]="";

    printf("Enter the name of the customer to delete the account: ");
    scanf("%s",name);
    write(socket_desc,name,sizeof(name));

    read(socket_desc,buf,sizeof(buf));
    printf("%s\n",buf);
    sleep(2);
    adminOptions(socket_desc,customer_login);
    
}
void delete_account(int socket_desc,struct Customer customer_login){
    long int acc_num;
    
    struct Customer customer;
    printf("Enter the account number you want to delete: ");
    scanf("%ld",&acc_num);

    write(socket_desc,&acc_num,sizeof(acc_num));

    char msg[100]="";
    read(socket_desc,&msg,sizeof(msg));
    
    if(!strcmp(msg,"Account does not exist")){
        printf("Sorry No Account with Account number %ld\n\n",acc_num);
        sleep(2);
        //adminOptions(socket_desc,customer_login);
    }
    else{
            bool flag;
            read(socket_desc,&flag,sizeof(flag));
            
            while(1){
                
                read(socket_desc,&flag,sizeof(flag));
                printf("%d\n",flag);
                if(!flag){
                    break;
                }

            }
        printf("Account with account number: %ld deleted successfully\n",acc_num);
        char buf[100]="";
        read(socket_desc,&buf,sizeof(buf));
        printf("%s\n",buf);
        sleep(2);
        
    }
    adminOptions(socket_desc,customer_login);    
}
int choose_bw_name_and_account(int socket_desc){

    int n;
    printf("what to want to proceed with\nPress 1 for username \nPress 2 for Account number\n\n ::: ");
    scanf("%d",&n);
    if(n==1 || n==2){
        printf("Entered choice = %d\n",n);
        write(socket_desc,&n,sizeof(n));
        return n;
    }
}
void view_details(int socket_desc,struct Customer customer_login)
{
    char name[512]="";
    struct Customer customer;
    struct Account acc;
    //search using name or account number(for joint account)
    printf("Enter the name of the user to view account details: ");
    scanf("%s",name);

    write(socket_desc,name,sizeof(name));//sending name to the server

    char msg[100]="";
    read(socket_desc,&msg,sizeof(msg));
    printf("%s\n",msg);
    if(!strcmp(msg,"Account does not exist")){
        printf("Sorry No Account with username %s\n\n",name);
        sleep(2);
        adminOptions(socket_desc,customer_login);
    }
    else{
        read(socket_desc,&customer,sizeof(customer));
        printf("The details of the customer named %s\n",customer.username);
        printf("Account Number= %ld\n",customer.account_number);
        printf("Type= %d\n",customer.type);
        printf("Status=%d\n",customer.status);
        
        read(socket_desc,&acc,sizeof(acc));
        printf("Balance in acount= %.2f\n",acc.balance); 
        
        //printf("Account Number= %ld\n",acc.account_number);
        sleep(2);
        adminOptions(socket_desc,customer_login);
    }

}
void view_details_with_accountnum(int socket_desc,struct Customer customer_login){
    long int acc_num;
    struct Customer customer;
    struct Account acc;
    printf("Enter the account number you want to view: ");
    scanf("%ld",&acc_num);

    write(socket_desc,&acc_num,sizeof(acc_num));
    char msg[100]="";
    read(socket_desc,&msg,sizeof(msg));
    if(!strcmp(msg,"Account does not exist")){
        printf("Sorry No Account with Account number %ld\n\n",acc_num);
        sleep(2);
        //adminOptions(socket_desc,customer_login);
    }
    else{
            bool flag;
            read(socket_desc,&flag,sizeof(flag));
            while(1){
                read(socket_desc,&customer,sizeof(customer));
                printf("The details of the customer named %s\n",customer.username);
                printf("Account Number= %ld\n",customer.account_number);
                printf("Type= %d\n",customer.type);
                printf("Status=%d\n",customer.status);
                read(socket_desc,&flag,sizeof(flag));
                if(!flag){
                    break;
                }
            }
            read(socket_desc,&acc,sizeof(acc));
            printf("Balance in acount= %.2f\n",acc.balance);
    }
    adminOptions(socket_desc,customer_login);
}
void deposit_amt(int socket_desc,struct Customer customer_login)
{
    double amount;
    double prev_bal,new_bal;
    printf("\nEnter amount to deposit: ");
    scanf("%lf",&amount);

    write(socket_desc, &amount, sizeof(amount));
    read(socket_desc, &prev_bal, sizeof(prev_bal));
    printf("Previous balance = %.2lf\n",prev_bal);
    read(socket_desc, &new_bal, sizeof(new_bal));
    printf("Updated balance = %.2lf\n",new_bal);

    //printf("New Balance: %d",amount);
    
    normal_user_options(socket_desc,customer_login);
}
void withdraw(int socket_desc,struct Customer customer_login)
{
    double amount;
    double prev_bal,new_bal;
    printf("\nEnter amount to withdraw: ");
    scanf("%lf",&amount);

    write(socket_desc, &amount, sizeof(amount));
    read(socket_desc, &prev_bal, sizeof(prev_bal));
    printf("Previous balance = %.2lf\n",prev_bal);
    if(prev_bal<amount)
    {
        printf("Account does not have sufficient balance to withdraw\n\n");
        //normal_user_options(socket_desc,customer_login);
    }
    else{
        read(socket_desc, &new_bal, sizeof(new_bal));
        printf("Updated balance = %.2lf\n",new_bal);
    }
    //printf("New Balance: %d",amount);
    sleep(2);
    normal_user_options(socket_desc,customer_login);
}
void view_cust_details(int socket_desc,struct Customer customer_login)
{
    struct Account account;
    struct Customer customer;
    read(socket_desc,&account,sizeof(account));
    read(socket_desc,&customer,sizeof(customer));
    printf("Username:                     %s\n",customer.username);
    printf("Password:                     %s\n",customer.password);
    printf("Status:                       ACTIVE\n");
    printf("Account number:               %ld\n",account.account_number);
    printf("Current Balance:              %.2lf\n",account.balance);

    sleep(2);
    normal_user_options(socket_desc,customer_login);

}
void password_change(int socket_desc,struct Customer cust){
    printf("Are you sure you want to change your password\nPress 1 to continue\n");
    int n;
    scanf("%d",&n);
    char pass[512]="";
    char new_pass[512]="";
    char new_pass_confirm[512]="";
    if(n==1)
    {
        printf("Enter current password: ");
        scanf("%s",pass);
        write(socket_desc,&pass,sizeof(pass));
        int n;
        read(socket_desc,&n,sizeof(n));
        if(n==0)
        {
            printf("You entered a wrong current password\n\n\n");
            //normal_user_options(socket_desc,cust);
        }
        else if(n==1)
        {
            printf("Current password verified\n");
            printf("Enter your new Password: ");
            scanf("%s",new_pass);
            printf("Renter your new Password: ");
            scanf("%s",new_pass_confirm);
            if(!strcmp(new_pass_confirm,new_pass))
            {
                write(socket_desc,&new_pass,sizeof(new_pass));
                sleep(3);
                printf("Your password has been updated successful!!\n\n");
                
            }
            else{
                printf("Both the entered passwords didn't match\n\n\n");
                
            }
            //normal_user_options(socket_desc,cust);
        }        
    }
    else{
        printf("Sorry!!! INVALID input\n\n\n");
    }
    sleep(3);
    normal_user_options(socket_desc,cust);
}
void get_transactions(int socket_desc,struct Customer cust)
{
   // printf("%ld\n",cust.account_number);
    bool flag;
    struct Transaction trans;
    read(socket_desc,&flag,sizeof(flag));
            
    while(1){
        //read(socket_desc,&customer,sizeof(customer));
        read(socket_desc,&trans,sizeof(trans));
        printf("%s\t",trans.date);
        printf("%ld\t\t", trans.account_number);
        printf("%lf\t", trans.amount);
        printf("%lf\t\n",trans.balance);

        read(socket_desc,&flag,sizeof(flag));
        if(!flag){
            break;
        }

    }
    printf("You got all the transaction\n");
    sleep(3);
    normal_user_options(socket_desc,cust);
}

void normal_user_options(int socket_desc,struct Customer customer_login)
{
    int choice;
    printf("----------------------------------------------------\n");
    printf("-------------------WELCOME %s-------------------\n\n\n",customer_login.username);
    printf("SELECT AN OPTION\n\n");
    printf("PRESS 1::Deposit\n");
    printf("PRESS 2::Withdraw\n");
    printf("PRESS 3::View Account Details \n");
    printf("PRESS 4::Password Change\n");
    printf("Press 5::Get Details of all transaction\n");
    printf("Press 6::To log out...\n");

    printf("-----------------Enter Your Choice-----------------\n");
    scanf("%d", &choice);
    while(choice > 6 || choice < 1)
        {   int t=3;
            while(--t){
                printf("Invalid Choice!\n");
                printf("-----------------Enter Your Choice-----------------\n");
                scanf("%d", &choice);}
                printf("\n\n So many wrong choices.... BYE BYE.... LOGGING YOU OUT\n\n\n");
                sleep(2);
                main_menu(socket_desc);
            }
            write(socket_desc, &choice, sizeof(choice));
            int n;
    switch(choice){
        case 1: deposit_amt(socket_desc,customer_login);break;
        case 2: withdraw(socket_desc,customer_login);break;
        case 3: view_cust_details(socket_desc,customer_login);break;
        case 4: password_change(socket_desc,customer_login);break;
        case 5: get_transactions(socket_desc,customer_login);break;
        case 6: printf("Thank you for choosing online banking... Good Bye!!!\n\n\n");
        main_menu(socket_desc);break;
    }


 }
 void modify_acc(int socket_desc,struct Customer cust)
 {
    int n;
    printf("Are you sure you want to modify the account\nPress 1 for YES\nPress 0 for NO\n");
    scanf("%d",&n);
    if(n==1)
    {
        long int acc_num;    
        printf("Enter the account number of the account you want to edit: ");
        scanf("%ld",&acc_num);

        write(socket_desc,&acc_num,sizeof(acc_num));

        char buf[100]="";
        read(socket_desc,&buf,sizeof(buf));

        if(!strcmp(buf,"This account can be modified"))
        {
            //char name[512];
            //char pass[512];
            struct Customer customer;
            //printf("Enter the name of new user");
            //scanf("%s",name);

            int invalid = 1;
            while(invalid){
                printf("Enter the name: ");
                scanf("%s",customer.username);
                write(socket_desc,&customer,sizeof(customer));
                read(socket_desc,&invalid,sizeof(int));
                //printf("%d\n", invalid);
                if(invalid==1)
                {
                    printf("\n\n------------------------------------------------------------\n");
                    printf("NAME ALREADY EXISTS IN THE DATABASE....\n--------------------PLEASE TRY AGAIN--------------------\n\n\n");
                }
            }
            char password[512]="";
            printf("Enter the password: ");
            scanf("%s", password);
            write(socket_desc,password,sizeof(password));

            read(socket_desc,&customer,sizeof(customer));
            printf("User added successfully...\n");
            printf("Welcome %s,\tYour account number = %ld\n",customer.username, acc_num);
            sleep(2);
        }
        else{
            printf("Either Account have 2 active members cannot be added more\nor there is no account with the account number\n\n");
        }

        adminOptions(socket_desc,cust);
    }
    else if(n==0)
    {
        printf("\n\nYou chose not to modify.\n\n\n");
        adminOptions(socket_desc,cust);

    }
    else
    {
        printf("Invalid choice... Sorry");
        modify_acc(socket_desc,cust);
    }
 }

void adminOptions(int admin_desc,struct Customer customer_login)
{
    int choice;
    printf("----------------------------------------------------\n");
    printf("-------------------WELCOME %s-------------------\n\n\n",customer_login.username);
    printf("SELECT AN OPTION\n\n");
    printf("PRESS 1::Add new user\n");
    printf("PRESS 2::Delete an existing user\n");
    printf("PRESS 3::Search for a record\n");
    printf("PRESS 4::Modify an account\n");
    printf("Press 5::To log out...\n");

    printf("-----------------Enter Your Choice-----------------\n");
    scanf("%d", &choice);

    while(choice > 5 || choice < 1)
    {   
            printf("Invalid Choice!\n");
            printf(".... BYE BYE.... LOGGING YOU OUT\n\n\n");
            sleep(2);
            exit(0);
            //main_menu(admin_desc);
     }
            write(admin_desc, &choice, sizeof(choice));
            int n;
            switch(choice)
            {
                case 1: add_new_customer(admin_desc,customer_login);
                break;
                case 2: n=choose_bw_name_and_account(admin_desc);
                        switch(n)
                        {
                            case 1: delete_user(admin_desc,customer_login); break;
                            case 2: delete_account(admin_desc,customer_login);break;
                        }
                        break;
                case 3: n=choose_bw_name_and_account(admin_desc);
                        switch(n)
                        {
                            case 1: view_details(admin_desc,customer_login); break;
                            case 2: view_details_with_accountnum(admin_desc,customer_login);break;
                        }                
                        break;
                case 4: modify_acc(admin_desc,customer_login);break;
                case 5: printf("Thank you for choosing online banking... Good Bye!!!\n\n\n");
                main_menu(admin_desc);
                default: break;
            }
        }

void main_menu(int socket_desc)
{   

        struct Customer customer;

        
        printf("-----------------Welcome to online banking-----------------\n");
        printf("Enter username: ");
        scanf("%s", customer.username);
        printf("Enter password: ");
        scanf("%s", customer.password);

        write(socket_desc, &customer, sizeof(customer));
        
        char buff[512]="";
        int valid_login=read(socket_desc, buff, sizeof(buff));
        printf("%s\n", buff);
        //read(socket_desc,&valid_login,sizeof(valid_login));
        if(!strcmp(buff,"Validation successful")){
            if(valid_login>=0 && strcmp(customer.username,"admin")==0 )
            {
                adminOptions(socket_desc,customer);
            }
            else
            {
                normal_user_options(socket_desc,customer);
        //Normal customer
            }
        }
}
int main()
{
        struct sockaddr_in server;
        int socket_desc;
        socket_desc = socket(AF_INET, SOCK_STREAM, 0);
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = INADDR_ANY;
        server.sin_port = htons(PORT);
        connect(socket_desc, (void *)(&server), sizeof(server));
        main_menu(socket_desc);
        close(socket_desc);
	return 0;
}
