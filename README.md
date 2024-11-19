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

## Authentication
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

## **Chatbot Documentation**

### **Overview**
This chatbot uses a combination of natural language processing techniques and a weighted scoring mechanism to determine appropriate responses to user input. The core functionality includes tokenizing input, calculating weights for matching contexts, and selecting an answer based on statistical measures like mean and standard deviation.

The chatbot leverages pre-stored context information and a trie-based search system to efficiently find relevant responses.

---

### **Function Descriptions**

#### **`returnTokenList(char* input, int* count)`**
##### Purpose:
Tokenizes a user input string into sanitized tokens.

##### Parameters:
- `char* input`: The input string provided by the user.
- `int* count`: Pointer to an integer to store the number of tokens generated.

##### Returns:
- A dynamically allocated array of sanitized tokens.

##### Workflow:
1. Splits the input string into tokens based on spaces.
2. Sanitizes each token using the `sanitize` function.
3. Returns the sanitized token list and updates `count` with the number of tokens.

---

#### **`mean(float* l, int le)`**
##### Purpose:
Calculates the mean of a list of floating-point numbers.

##### Parameters:
- `float* l`: Array of float numbers.
- `int le`: Number of elements in the array.

##### Returns:
- The mean value of the array.

---

#### **`stdDeviation(float* l, int le)`**
##### Purpose:
Computes the standard deviation of a list of floating-point numbers.

##### Parameters:
- `float* l`: Array of float numbers.
- `int le`: Number of elements in the array.

##### Returns:
- The standard deviation of the array.

##### Workflow:
1. Calculates the mean of the array.
2. Computes the squared differences from the mean.
3. Returns the square root of the average squared difference.

---

#### **`generateAnswer(char* input)`**
##### Purpose:
Generates a response to the user's input.

##### Parameters:
- `char* input`: The input query provided by the user.

##### Returns:
- A dynamically allocated string containing the chatbot's response.

##### Workflow:
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

### **How to Use**

1. **Setup**:
   - Ensure the necessary context and trie binary files (`context-test.bin` and `trie-test.bin`) are generated and accessible.

2. **Integrate with User Input**:
   - Use `generateAnswer` to process user queries.
   - Pass the user's query string to `generateAnswer` and display the returned response.

3. **Error Handling**:
   - Fallback response: If the chatbot cannot determine an answer, it responds with a default message: *"I'm sorry, but I do not know how to answer that!"*

---

### **Key Dependencies**
- **Trie-based Search**: Efficiently finds matching tokens in the context database.
- **Context Data**: Stores potential answers and their associated weights.
- **Utility Functions**:
  - `str_split`: Splits a string by a delimiter.
  - `sanitize`: Prepares tokens for comparison by removing unwanted characters.
  - `bubbleSortDescending`: Sorts arrays in descending order.
  - `inPlaceNormalize`: Normalizes weights.

---

### **Potential Improvements**
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

### **Example Usage**
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
## Library Utilities
### **Fuzzy Search**

This documentation provides an explanation of the **C code** implementing a fuzzy search algorithm. The code performs advanced, flexible searches in a dictionary database using various techniques like the Damerau-Levenshtein metric, Soundex hashing, and synonym matching. 

---

#### **Code Functionality Overview**

The code consists of the following primary components:

1. **`advanced_search()`**: The main entry function for performing advanced searches. It allows searching by title, author, or publisher using fuzzy matching.
2. **`fuzzy_search()`**: A helper function that conducts the fuzzy search for a specific category (title, author, or publisher).
3. **Utility Functions**:
   - **Damerau-Levenshtein Metric**: Used to compute the edit distance between strings, allowing correction of typographical errors.
   - **Soundex**: Identifies phonetically similar words.
   - **Synonyms Lookup**: Matches words with their synonyms using a thesaurus.
4. **Sorting Functionality**: Results are scored and sorted by relevance.

---

#### **How the Code Works**

##### **1. `advanced_search()`**
This function takes the following arguments:
- **`title`**: The search term for the book title.
- **`author`**: The search term for the book author.
- **`pub`**: The search term for the publisher.
- **`dict_file`**: Path to the dictionary file containing indexed data.

##### **Function Workflow**:
1. **Separate Searches**:
   - If a search term is provided for a category (e.g., title), it performs a fuzzy search using `fuzzy_search()`.
   - Results for each category are stored in arrays (`res_title`, `res_author`, and `res_pub`).

2. **Combine Results**:
   - Combines results from all search categories by matching common indices.
   - Calculates scores based on the relevance of results and their position in each category's list.

3. **Special Cases**:
   - If only one category has results (e.g., only the title was searched), returns the results for that category directly.
   - If no results are found for all categories, returns an empty array.

4. **Sorting**:
   - Scores are used to sort the combined results in descending order of relevance.

5. **Return**:
   - Returns an array of indices corresponding to the most relevant matches.

---

##### **2. `fuzzy_search()`**
This function performs a detailed fuzzy search for a given query and category.

##### **Inputs**:
- **`query`**: The search term.
- **`cat`**: The category to search in (`1` for title, `2` for author, `3` for publisher).
- **`dict_file`**: The dictionary file.

##### **Steps**:
1. **Sanitization**:
   - Prepares the query by stripping extra spaces, splitting it into terms, and sanitizing them to handle special characters.
   - Generates **Soundex hashes** for terms to handle phonetically similar words.
   
2. **Synonym Expansion**:
   - Uses a thesaurus (`thesaurus.csv`) to expand the search query with synonyms.

3. **Dictionary Lookup**:
   - Iterates through each row of the dictionary.
   - For each term:
     - Computes the **Damerau-Levenshtein Distance** for fuzzy matching.
     - Compares **Soundex hashes** for phonetically similar matches.
     - Matches terms with their synonyms.

4. **Scoring**:
   - Assigns scores to results based on:
     - Edit distance (smaller distance → higher score).
     - Soundex match quality.
     - Synonym penalty for less direct matches.
   - Combines results from all terms and calculates cumulative scores.

5. **Sorting**:
   - Results are sorted by scores in descending order for relevance.

6. **Return**:
   - Returns an array of indices representing matches, sorted by score.

---

#### **Key Algorithms and Techniques**

1. **Damerau-Levenshtein Distance**:
   - Measures the edit distance between two strings, accounting for operations like:
     - Insertion
     - Deletion
     - Substitution
     - Transposition
   - Allows matching terms even with typographical errors.

2. **Soundex Hashing**:
   - Encodes words into a phonetic representation.
   - Matches words that sound similar but may have different spellings.

3. **Synonyms Expansion**:
   - Uses a thesaurus to find synonyms for search terms.
   - Broadens the search to include related terms.

4. **Sorting by Scores**:
   - Combines scores from all matching methods (Damerau-Levenshtein, Soundex, and synonyms).
   - Higher relevance results are ranked at the top.

---

#### **How to Use**

##### **Input Requirements**
- A **dictionary file (`dict_file`)**:
  - Contains indexed data structured as:
    ```
    <token>,<type>,<indices>,<Soundex_hash>
    ```
    Example:
    ```
    "programming","title","1-2-3","P625"
    "science","author","4-5","S520"
    ```

- A **thesaurus file (`thesaurus.csv`)**:
  - Lists synonyms in a comma-separated format.
    Example:
    ```
    "book,volume,tome"
    "author,writer,novelist"
    ```

##### **Function Calls**
1. Include required headers and ensure linked implementations for:
   - Damerau-Levenshtein
   - Soundex
   - Thesaurus lookups

2. Call `advanced_search()` with appropriate parameters:
   ```c
   char* title = "science";
   char* author = "";
   char* publisher = "oxford";
   char* dict_file = "books-clean.csv";

   int* results = advanced_search(title, author, publisher, dict_file);

   for (int i = 0; results[i] != -1; ++i) {
       printf("Matched index: %d\n", results[i]);
   }
   free(results);
   ```

---

#### **Performance Considerations**
- **Memory Usage**:
  - Uses dynamically allocated memory for result arrays. Ensure all memory is freed after use.
- **Time Complexity**:
  - Depends on the size of the dictionary and the complexity of the search terms.
  - Sorting operations and multiple scans through the dictionary can be costly for large datasets.

---

#### **Limitations**
1. **Dictionary Structure**:
   - The dictionary file must follow the specified format strictly.
2. **Synonym Coverage**:
   - Effectiveness of synonyms depends on the quality of the thesaurus file.
3. **Optimization**:
   - Sorting and multiple list intersections could be optimized for performance.

---

#### **Conclusion**
This code provides a robust solution for implementing a fuzzy search with support for errors, synonyms, and phonetic matching. It is particularly suited for searching in large text databases like books, documents, or metadata collections.

**Search by ID:** To do this, we have implemented a binary search, in the `int idToIdx(int id)` function in `ui.c`. This returns the index of the book which has a matching ID. 

### Issuing and Returning Books
The users (faculty/student) can issue books from, or return their issued books to the library.
#### Issuing a book
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
#### Returning a book
To return a book, one can go to the `DUES` page, and see their issued books. Once they, trigger the return a book, the following functions are called in sequence.
1. `void returnPrompt(int i)`: This function takes the index of the issued book entry in the global `Dues` array, and then deletes it from the database.
2. `void loadDues()`: This loads the issued books into the global books array. This has been explained later in the UI section.

### Adding, Deleting, and Editing Books
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
The User Interface for this library management system has been written in the `ui.c`. It provides functionality for rendering different pages, managing user inputs, and interacting with backend modules.

### Overview

#### Key Features:

- Multi-page UI with views for book search, user management, dues, and chat functionalities.

- Command handling for efficient navigation and operations.

- Integration with other system components, such as RSA encryption and chatbots.



### Code Structure

#### Includes:

The following libraries are used:

- **Standard Libraries**: `stdio.h`, `stdlib.h`, `termios.h`, etc., for basic I/O and terminal interaction.

- **Custom Modules**: Headers such as `rsa.h`, `library.h`, and `chatbot.h` enable modular functionality.

#### Macros and Enums:

Key macros defined in the file include:

- `MSGTIMEOUT`: Timeout for displaying messages.

- `MAXCHARLIM`: Maximum character limit for command buffers.

- `CTRL_Key()`: This is a macro that gives us the ASCII value of the character when a key is pressed along with the `Ctrl` Key.

- `PAGES`: This enum stores the values for rendering different pages in the application, like the homepage, search results, book details, chat bot etc.

- `cursorKeys`: This is an enum storing the keycodes for different characters, which are then returned by the `readKeyPress` function.

The definitions for these macros is shown below.

```C
#define MSGTIMEOUT 3
#define MAXCHARLIM 5000

#define CTRL_Key(x) (x & 31)

enum cursorKeys {
    TAB_KEY = 9,
    BACKSPACE = 127,
    ARROW_LEFT = 1000,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
    PAGE_UP,
    PAGE_DOWN,
    HOME_KEY,
    END_KEY,
    DEL_KEY
};

enum PAGES {
    NORMAL=0,
    BOOK_VIEW,
    SEARCH,
    DUES,
    DUE_VIEW,
    USERS,
    CHAT
};
```

#### Data Structures:

- **`Due`**: Tracks book issuance details, such as username, book ID, issue date, and due date.

- **`Userd`**: Represents user information, including username and privilege level.

- **`Chat`**: Stores chatbot interactions (questions and answers).

- **`state`**: Maintains the current state of the UI, including cursor position, active page, and user information etc.

The definitions for these Structs are shown below:
```C
typedef struct {
    char* uname;
    int bookID;
    time_t issueDate;
    time_t dueDate;
} Due;

typedef struct {
    char* uname;
    int priv;
} Userd;

typedef struct {
    char* question;
    char* answer;
} Chat;

struct state {
    int cx, cy, screenrows, screencols, rowoff, numResults;
    struct termios orig_term;
    Book* books;
    int nbooks;
    char* username;
    int userPriv;
    int page;
    char commandBuf[MAXCHARLIM];
    time_t commandTime;
    int* sIdx;
    Due* dues;
    int nDues;
    Userd* users;
    int nUsers;
    Chat chat;
};
```
### Functions

#### `enableRawMode`
- This function turns off canonical mode in the terminal and turns it into raw mode.

#### `die`
- Kill the application if it throws an error and quit with an error message.

#### `disableRawMode`
- Reset the terminal.

#### `readKeyPress`
- Read key presses from the terminal stdin.

#### `freeBooks`
- Free the global books array.

#### `freeDues`
- Free the global dues array.

#### `quitApp`
- Function to run when the user sends the quit signal. Frees the global arrays and resets the terminal.

#### `searchByID`
- A function to search the book database by ID.

#### `idtoIdx`
- A binary search function that returns the index of the book when the ID of the book is passed into it.

#### `moveCursor`
- Move the cursor when the arrow keys are pressed.

#### `loadBooks`
- Loads the books when the app is started.

#### `loadDues`
- Loads the due books when the app is started.

#### `loadUsers`
- Load the users list for the admin.

#### `drawUsers`
- Draw the users table.

#### `changeUserPriv`
- Change the user type.

#### `returnPrompt`
- This function lets the user return a book.

#### `statusBar`
- Draws the bottom status bar.

#### `topBar`
- Draw the top bar.

#### `drawBooksTable`
- Draw the table of books on the homepage.

#### `chatPrompt`
- Prompt the user to ask questions to the chatbot.

#### `drawChat`
- Render the chatbot page.

#### `drawDues`
- Render the dues table.

#### `drawDueDeets`
- Render the details of a particular issued book record.

#### `drawSearchResults`
- Render the search results.

#### `drawCommand`
- Draw the command bar.

#### `setCommandMsg`
- Set the text in the command bar.

#### `drawHelp`
- Render the help bar.

#### `drawQuit`
- Draw the quit instruction.

#### `commandPrompt`
- Prompt the user for an input. Takes a FORMAT string as an input and returns a string (`char *`).

#### `deletePrompt`
- Prompt the user to delete a book and ask for confirmation.

#### `addPrompt`
- Prompt the user to add details about a new book.

#### `editPrompt`
- Edit book details.

#### `issuePrompt`
- Prompt the user to issue a book.

#### `searchPrompt`
- Prompt the user for a search.

#### `advancedSearchPrompt`
- Advanced search functionality.

#### `scroll`
- Scrolls the page by setting the global `E.rowoff` variable to set a row offset.

#### `goToxy`
- Sends the cursor to the x-th column and the y-th row.

#### `resetScreen`
- Resets the screen, clears it, and returns the cursor to `(0, 0)`.

#### `refreshScreen`
- Refreshes the screen and draws everything to the screen.

#### `init`
- Initializes the UI and logs the user in. Called at the beginning of the main function.

#### `getWindowSize`
- Gets dimensions of the terminal window in rows and columns and writes to the pointers passed as parameters.

#### `getCursorPosition`
- Gets the cursor position into the pointers passed as parameters.


### Dependencies
- Terminal support for ANSI escape codes (for UI rendering).

## Contributions
| Debayan Sarkar [(@TheSilyCoder)](https://github.com/TheSillyCoder)| Diptanuj Sarkar [(@CasioWave)](https://github.com/CasioWave)|
| --------| --------|
| User Interface | Chatbot |
| Database CRUD operations | Search | 
| Authentication | RSA |
## References
1. [The C Programming Language - By Brian W. Kernighan, Dennis M. Ritchie](https://ia903407.us.archive.org/35/items/the-ansi-c-programming-language-by-brian-w.-kernighan-dennis-m.-ritchie.org/The%20ANSI%20C%20Programming%20Language%20by%20Brian%20W.%20Kernighan%2C%20Dennis%20M.%20Ritchie.pdf)
2. [Video Diplay Terminal Information](https://vt100.net/)
