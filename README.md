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

# **Chatbot Documentation**

## **Overview**
This chatbot uses a combination of natural language processing techniques and a weighted scoring mechanism to determine appropriate responses to user input. The core functionality includes tokenizing input, calculating weights for matching contexts, and selecting an answer based on statistical measures like mean and standard deviation.

The chatbot leverages pre-stored context information and a trie-based search system to efficiently find relevant responses.

---

## **Function Descriptions**

### **`returnTokenList(char* input, int* count)`**
#### Purpose:
Tokenizes a user input string into sanitized tokens.

#### Parameters:
- `char* input`: The input string provided by the user.
- `int* count`: Pointer to an integer to store the number of tokens generated.

#### Returns:
- A dynamically allocated array of sanitized tokens.

#### Workflow:
1. Splits the input string into tokens based on spaces.
2. Sanitizes each token using the `sanitize` function.
3. Returns the sanitized token list and updates `count` with the number of tokens.

---

### **`mean(float* l, int le)`**
#### Purpose:
Calculates the mean of a list of floating-point numbers.

#### Parameters:
- `float* l`: Array of float numbers.
- `int le`: Number of elements in the array.

#### Returns:
- The mean value of the array.

---

### **`stdDeviation(float* l, int le)`**
#### Purpose:
Computes the standard deviation of a list of floating-point numbers.

#### Parameters:
- `float* l`: Array of float numbers.
- `int le`: Number of elements in the array.

#### Returns:
- The standard deviation of the array.

#### Workflow:
1. Calculates the mean of the array.
2. Computes the squared differences from the mean.
3. Returns the square root of the average squared difference.

---

### **`generateAnswer(char* input)`**
#### Purpose:
Generates a response to the user's input.

#### Parameters:
- `char* input`: The input query provided by the user.

#### Returns:
- A dynamically allocated string containing the chatbot's response.

#### Workflow:
1. **Context and Trie Initialization**:
   - Reads context data from a binary file (`context-test.bin`).
   - Loads a trie structure from another binary file (`trie-test.bin`).

2. **Tokenization**:
   - Tokenizes and sanitizes the input using `returnTokenList`.

3. **Token Weight Calculation**:
   - Searches the trie for each token and retrieves corresponding weights.

4. **Weight Aggregation**:
   - Aggregates weights for each potential response.

5. **Normalization and Sorting**:
   - Normalizes the aggregated weights and sorts them in descending order using `bubbleSortDescending`.

6. **Answer Selection**:
   - Determines the top response based on statistical analysis of weights.
   - If the difference between the top weight and the mean is less than half the standard deviation or the weight is invalid (NaN), a fallback response is given.

7. **Memory Management**:
   - Frees dynamically allocated memory and returns the response.

---

## **How to Use**

1. **Setup**:
   - Ensure the necessary context and trie binary files (`context-test.bin` and `trie-test.bin`) are generated and accessible.

2. **Integrate with User Input**:
   - Use `generateAnswer` to process user queries.
   - Pass the user's query string to `generateAnswer` and display the returned response.

3. **Error Handling**:
   - Fallback response: If the chatbot cannot determine an answer, it responds with a default message: *"I'm sorry, but I do not know how to answer that!"*

---

## **Key Dependencies**
- **Trie-based Search**: Efficiently finds matching tokens in the context database.
- **Context Data**: Stores potential answers and their associated weights.
- **Utility Functions**:
  - `str_split`: Splits a string by a delimiter.
  - `sanitize`: Prepares tokens for comparison by removing unwanted characters.
  - `bubbleSortDescending`: Sorts arrays in descending order.
  - `inPlaceNormalize`: Normalizes weights.

---

## **Potential Improvements**
1. **Memory Management**:
   - Add checks to ensure all dynamically allocated memory is freed.
   - Use modern memory allocation patterns to avoid leaks.

2. **Error Handling**:
   - Validate input and files to handle missing or corrupted data gracefully.

3. **Optimization**:
   - Replace `bubbleSortDescending` with a more efficient sorting algorithm for larger datasets.

4. **Customizability**:
   - Allow dynamic updates to the context database without recompiling or replacing binary files.

---

## **Example Usage**
```c
#include <stdio.h>
#include "chatbot.h"

int main() {
    char query[256];
    printf("Ask me a question: ");
    fgets(query, 256, stdin);

    // Generate a response
    char* response = generateAnswer(query);
    printf("Chatbot: %s\n", response);

    free(response); // Free allocated response memory
    return 0;
}
```

This code integrates the chatbot into a simple terminal-based interface, allowing users to ask questions and receive answers.
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
The User Interface for this library management system has been written in the `ui.c`. This C file implements the user interface (UI) for a library management system. It provides functionality for rendering different pages, managing user inputs, and interacting with backend modules.

## Overview

### Key Features:

- Multi-page UI with views for book search, user management, dues, and chat functionalities.

- Command handling for efficient navigation and operations.

- Integration with other system components, such as RSA encryption and chatbots.



## Code Structure

### Includes:

The following libraries are used:

- **Standard Libraries**: `stdio.h`, `stdlib.h`, `termios.h`, etc., for basic I/O and terminal interaction.

- **Custom Modules**: Headers such as `rsa.h`, `library.h`, and `chatbot.h` enable modular functionality.

### Macros:

Key macros defined in the file include:

- `STUDLIM` and `FACLIM`: Limits for the number of students and faculty members.

- `MSGTIMEOUT`: Timeout for displaying messages.

- `MAXCHARLIM`: Maximum character limit for command buffers.

### Data Structures:

- **`Due`**: Tracks book issuance details, such as username, book ID, issue date, and due date.

- **`Userd`**: Represents user information, including username and privilege level.

- **`Chat`**: Stores chatbot interactions (questions and answers).

- **`state`**: Maintains the current state of the UI, including cursor position, active page, and user information.



## Functions

### Core Functionalities:

1. **Rendering Pages**:

   Functions responsible for displaying specific views, such as `NORMAL`, `BOOK_VIEW`, or `CHAT`.

2. **Command Handling**:

   Processes user inputs and updates the UI state accordingly.

3. **Integration**:

   Interfaces with backend modules for operations like user authentication, book search, and chatbot responses.

### Interaction Flow:

- The UI listens for user commands, processes inputs, and updates the terminal display.

- Modular design ensures seamless communication with other components, such as the library system and encryption module.

## Dependencies
- Terminal support for ANSI escape codes (for UI rendering).

- Backend modules (`rsa.h`, `library.h`, etc.) for full functionality.
## Contributions
| Debayan Sarkar [(@TheSilyCoder)](https://github.com/TheSillyCoder)| Diptanuj Sarkar [(@CasioWave)](https://github.com/CasioWave)|
| --------| --------|
| User Interface | Chatbot |
| Database CRUD operations | Search | 
| Authentication | RSA |
## References
1. k&r
