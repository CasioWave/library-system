#pragma once


typedef struct {
    int d, m, y;
} Date;
typedef struct  {
    int id;
    char title[255];
    char** authors;
    char publisher[100];
    Date pubDate;
    int pages;
    int qty;
} Book;

Book* fetchBooks(char*);
void displayBooks();
