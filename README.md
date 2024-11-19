# Library Management System C

This repository contains the source code for the semester 5 project assigned to us for the course **CS3101: Programming and Data Structures I**

## Compiling Instructions

To compile the source, first download it, by either cloning it, or downloading it as a zip from github. Then, run `make`. That should create an executable called `lms` in your `PWD`.

An example has been provided below (for cloning the repo using git-cli and then compiling it).

```bash
git clone https://github.com/TheSillyCoder/library-system.git
cd library-system
make
./lms
```


## Features

This application has ~ 3000 books in the database, all of which are stored in the `books-clean.csv` file. This application let's a user (student/faculty) issue or return books,
while the admins can add, delete, and edit books. It also has a search functionality, including a free-text/fuzzy search, an field-wise/advanced search, and a search by ID. The users 
are classified using a user priviledge parameter, and the users are split into 3 seperate classes, "admins", "student", and "faculty". It also has a chatbot `Clippy`, which can answer your questions. 
The admin can change a user's priviledge, by demoting or promoting them. 

We have talked about this features in a little more detail below.

### Authentication
When the application is opened it prompts the user for their username and password. If the username is new, the application asks the user if they want to sign up, and if confirmed,
its signs the user up, and logs them in. Or otherwise, if the username and password match one record in the database, it logs that user in. Otherwise it quits the application. Saying 
*"Login Failed!!!"*

The functions being called in this process are as follows:
1. `int login(int* priv, char** uname)`: It prompts for a login and returns `LOGIN_FAILIURE`, if the login fails, and `LOGIN_SUCCESS` otherwise.
2. `User* fetchUsers(char* filename)`: This reads the user data from the user and stores them in an array of `User` structs.
3. `void registerUser(char* username, char* password)`: In case its a new user, this function writes the new user into the users database.

The return values of `login` are here 

```C
#define LOGIN_SUCCESS 0 
#define LOGIN_FAILURE -1
```
The priviledges have the following values:
```C
enum PRIV {
    ADMIN=1,
    FACULTY=2,
    STUDENT=4
};
```
The forementioned `User` struct has been defined as follows.
```C
typedef struct {
    char* username;
    char* password;
    int priv;
} User;
```

### Library Utilities
#### Search
TODO

**Search by ID:** To do this, we have implemented a binary search, in the `int idToIdx(int id)` function in `ui.c`. This returns the index of the book which has a matching ID. 

#### Issuing and Returning Books
The users (faculty/student) can issue books from, or return their issued books to the library.
##### Issuing a book
When you trigger the command to issue a book, the function calls that are executed have been explained below.
1. `void issuePrompt(int i)`: This function takes a variable `int i` as one of the parameters, which corresponds to the index of the the book in the global `E.Books` array. This then is passed to the next function.
2. `void issueBook(char* username, int bookID, int nweeks)`: This function is defined in `library.c` it adds a record in the `transanctions.csv`. The structure of this database has been mentioned
below.

**Note:** If a book has less than 3 copies, only faculty can issue the book. Also, there's a upper bound on the number of books that can be issued by the users, namely `STUDILIM` and `FACILIM`. Also
the default number of weeks for which a book can be issued is defined as `STUDLIM` and `FACLIM`.
Here are the definitions for these limits defined in `ui.c`.
```C
#define STUDLIM 8
#define FACLIM 16

#define STUDILIM 5
#define FACILIM 10
```
If the number of copies of the book being issued is less than 5, the following formula is used to calculate the number of weeks for which the book will be issued.
```C
nWeeks = nWeeks - (5 - copies);
```
##### Returning a book
To return a book, one can go to the `DUES` page, and see their issued books. Once they, trigger the return a book, the following functions are called in sequence.
1. `void returnPrompt(int i)`: This function takes the index of the issued book entry in the global `Dues` array, and then deletes it from the database.
2. `void loadDues()`: This loads the issued books into the global books array. This has been explained later in the UI section.

#### Adding, Deleting, and Editing Books
Any admin who has admin priviledges can add, delete or edit books. Whenever these are triggered, the following functions are called respectively.
1. `void addPrompt()`: This prompts the admin, to enter the details of the new book to be added to the data base. And then it calls `updateBooks()` to update the database.
2. `void deletePrompt(int i)`: This prompts the admin for confirmation whether they *really* want to delete the record. If confirmed, it deletes the book from the global books array and calls `updateBooks()` to update the database.
3. `void editPrompt(int i)`: This prompts the admin to enter what field they want to edit. And then it edits that book in the global books array and then calls `updateBooks()` to update the database.

All of these functions call the function `updateBooks()`. This function has been explained below.

`void updateBooks(Book* books, int nbooks)`: This function takes an array of books and the number of books in that array as parameters and writes it to the database.

The `Book` struct was defined like this:
```C
typedef struct  {
    int id;
    char* title;
    char* authors;
    char* publisher;
    char* pubDate;
    int pages;
    int qty;
} Book;
```

## Databases
All the databases used in this application are in CSV format. 
### Books
The books are stored in `books-clean.csv` file. This dataset was downloaded from kaggle. It has the following fields:
1. book ID
2. Book title
3. Book authors
4. Number of Pages in the book
5. Publication date
6. Publisher
7. Copies Available
### Users
The users are stored in `users.csv` file. This file has the following fields:
1. Username
2. Password
3. Userfield

### Issued Books
The issued books are stored in `transanctions.csv` file. This file has the following fields:
1. Username
2. Book ID
3. Issue Date
4. Due Date

## UI
OMAGO
## Refrences
1. k&r
