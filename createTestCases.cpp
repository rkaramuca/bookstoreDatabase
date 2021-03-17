#include<cstdlib>
#include<iostream>
#include<fstream>
#include<sstream>
#include<climits>
#include<iomanip>
#include<map>
#include<sstream>
#include<cstring>

using namespace std;

enum TransactionType {Add, Delete, ChangeOnhand, ChangePrice};

typedef char String[25];
struct BookRec {
    unsigned int isbn;
        // err 1: isbn less than 1. write error message to cerr and IGNORE record
        // err 2: isbn is less than or equal to the previous isbn. write error message to cerr and write record to cout. add record to output file
    String name;
    String author;
    int onhand;
    float price;
    String type;
};

struct TransactionRec {
    TransactionType ToDo;
    BookRec B;
};

struct BookRec;
struct TransactionRec;

int main(int argc, char** argv) {
    // vars
    TransactionRec tRec;
    BookRec B;
    fstream outFile("t.out", ios::out|ios::binary);

    // TOM SAWYER: DUPLICATE TEST CASE
    B.isbn = 123766891;
    strcpy(B.name, "Tom Sawyer"); 
    strcpy(B.author, "Twain, Mark");
    B.onhand = 2;
    B.price = 2.2;
    strcpy(B.type, "fiction");
    tRec.ToDo = Add;
    tRec.B = B;
    outFile.write((char*) &tRec, sizeof(TransactionRec));

    // LEAVES OF GRASS: NEGATIVE ONHAND
    B.isbn = 239678325;
    strcpy(B.name, "Leaves of Grass"); 
    strcpy(B.author, "Whitman, Walt");
    B.onhand = -9;
    B.price = 29.99;
    strcpy(B.type, "poetry");
    tRec.ToDo = ChangeOnhand;
    tRec.B = B;
    outFile.write((char*) &tRec, sizeof(TransactionRec));

    // THE HUNGER GAMES: ADD
    B.isbn = 4087954326;
    strcpy(B.name, "The Hunger Games"); 
    strcpy(B.author, "Collins, Suzanne");
    B.onhand = 5;
    B.price = 18.99;
    strcpy(B.type, "fiction");
    tRec.ToDo = Add;
    tRec.B = B;
    outFile.write((char*) &tRec, sizeof(TransactionRec));

    // HEART OF DARKNESS: DELETE
    B.isbn = 3391679915;
    strcpy(B.name, "Heart of Darkness"); 
    strcpy(B.author, "Conrad, Joseph");
    B.onhand = 12;
    B.price = 11.99;
    strcpy(B.type, "drama");
    tRec.ToDo = Delete;
    tRec.B = B;
    outFile.write((char*) &tRec, sizeof(TransactionRec));

    // HEART OF DARKNESS: ADD
    B.isbn = 3391679915;
    strcpy(B.name, "Heart of Darkness"); 
    strcpy(B.author, "Conrad, Joseph");
    B.onhand = 12;
    B.price = 11.99;
    strcpy(B.type, "drama");
    tRec.ToDo = Add;
    tRec.B = B;
    outFile.write((char*) &tRec, sizeof(TransactionRec));

    // HEART OF DARKNESS: DELETE
    B.isbn = 3391679915;
    strcpy(B.name, "Heart of Darkness"); 
    strcpy(B.author, "Conrad, Joseph");
    B.onhand = 12;
    B.price = 11.99;
    strcpy(B.type, "drama");
    tRec.ToDo = Delete;
    tRec.B = B;
    outFile.write((char*) &tRec, sizeof(TransactionRec));

    outFile.close();

    return 0;
}
