#pragma once


typedef struct {
    int d, m, y;
} Date;
typedef struct  {
    int id;
    char* title;
    char** authors;
    char* publisher;
    Date pubDate;
    int pages;
    int qty;
} Book;

Book* fetchBooks(char*);
void displayBooks(Book *);
