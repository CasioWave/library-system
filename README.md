# Library Management System C

This repository contains the source code for the semester 5 project assigned to us for the course **CS3101: Programming and Data Structures I**

## Compiling Instructions

To compile the source, first download it, by either cloning it, or downloading it as a zip from github. Then, run `make`. That should create an executable called `lms` in your `PWD`.

An example has been provided below (for cloning the repo using git-cli and then compiling it).

```
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

### Library Utilities
#### Search
TODO

**Search by ID:** To do this, we have implemented a binary search, in the `int idToIdx(int id)` function in `ui.c`. This returns the index of the book which has a matching ID. 

#### Issuing and Returning Books
TODO
#### Adding, Deleting, and Editing Books
TODO
### Authentication
DODO
## Databases
DODO PAAKHI

## UI
OMAGO
## Refrences
1. k&r
