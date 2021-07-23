						ONLINE BANKING MANAGEMENT SYSTEM

The project aims to develop a banking system that is user-friendly and multifunctional.

There are two types of USERS
*   Admin
*   Normal User

Admin is added in the database Statically
    username- admin
    password- admin
   
The functionalities of the admin
* Add new user
* Delete an existing user
* Search for a record
* Modify an account
* log out

   - Add new user: This is to add a new normal user where you admin.
   		    Admin will be asked for the username and the password.
   		    If the username is unique the data can be added else it will give error and will ask for the username again.
   		    Once added admin will get the Account number for the added username.
   - Delete an existing user: On choosing this option admin will get another two options:
					- to delete using username: Admin will be asked for the username to delete. 
								     Once entered that user if exists will be deleted.
								     If username does not exist, it will give error that user does not exist. 
					- to delete using account number: Admin will enter the account number.
									   Once entered it will check if that account number exists or not.
									   if exists it will delete the account else will give an error.
   - Search for a record: On choosing this option admin will get another two options:
					- to search using username: Admin will be asked for the username to search. 
								     Once entered it will check if that username exists or not.
								     If username does not exist, it will give error that user does not exist.
								     If exists it will give you the all the details of the account. 
					- to search using account number: Admin will enter the account number.
								   Once entered it will check if that account number exists or not.
								   If username does not exist, it will give error that user does not exist.
								   If exists it will give you the all the details of the account. 
   - Modify an account: This is used too change normal account to a joint account. 
			 It will ask for the account number which you want to change to a joint account.
			 If account exists and have only single user then it will ask for thr secong username and password.
			 If account will already have 2 active users then it will not let us change the account to a joint account.
   - Log out: It will exit the admin profile.
   
   
//Now you can login as a customer which admin have added to the database. And that user is active.
The functionalities of the Normal User
* Deposit
* Withdraw
* View Account Details
* Password Change
* Get Details of all transaction
* log out

 - Deposit: User can deposit the amount in his account. It will be added to the account. And will be updated in the database.
 - Withdraw: User can withdraw the amount from his account. It will be subtracted from the account. And will be updated in the database.
 - View Account Details: User can get all the details of his account using this option.
 - Password Change: User can change his/her password using this option.
 - Get Details of all transaction: This will give all the details of the transactions of the account.
 - Log out: It will exit the user's profile.

					
