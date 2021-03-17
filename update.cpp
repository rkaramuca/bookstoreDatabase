/* 
 * Ronald Karamuca
 * CSC 310 Project 1: Bookstore Database Part 3
 * Use fstream to update and modify binary files
 * that simulate a Bookstore inventory database.
 * After all modifying is done, print out the new record.
 */

#include <iostream>
#include <fstream>
#include <map>
#include <iomanip>
using namespace std;

enum TransactionType {Add, Delete, ChangeOnhand, ChangePrice};

// create the outline for a book record
typedef char String[25];
struct BookRec
{
	unsigned int isbn;
	String name;
	String author;
	int onhand;
	float price;
	String type;
};

// Transactions will have a task and a record to affect/add
struct TransactionRec
{
    TransactionType ToDo;
    BookRec B;
};

// function prototypes
int fileChecking(int argc, char** argv);
long getOffsets(char* fileName, map<unsigned int, long> &offsets);
void transactions(map<unsigned int, long> &offsetsMap, long &offset, char** argv);
void printRecord(char* file);
void printInfo(BookRec buff); 
string addZeros(unsigned int &key);

int main(int argc, char** argv) {
    // file checking
    // if error occurred, kill program
    int argCheck = fileChecking(argc, argv);
    if(argCheck == 1)
        return 1;

    // create the map of offsets based on ISBNs
    char* master = argv[1];
    map<unsigned int, long> offsetsMap;
    long offset = getOffsets(master, offsetsMap);

    // main logic, calls all other transaction functions
    transactions(offsetsMap, offset, argv);
    return 0;
}

// check validity of file names
int fileChecking(int argc, char** argv) 
{
    // if there isn't a master file, transaction file, and output file, 
    // kill the program
    if(argc != 4) {
        cerr << "Error: Incorrect input format.\n";
        cout << "Enter: ./database masterFile transactionFile newFile\n";
        return 1;
    }

    string mast = argv[1];
    string transact = argv[2];
    string newFile = argv[3];

    // tests if master file is valid
    fstream test(mast);
    if(!test) {
        cerr << "Provide a valid master file: '" << argv[1] << "' does not exist.\n";
        return 1;
    }
    test.close();

    // tests if transaction file is valid
    fstream test2(transact);
    if(!test2) {
        cerr << "Provide a valid transaction file: '" << argv[2] << "' does not exist.\n";
        return 1;
    }
    test2.close();

    // check that all files are uniquely named
    if(mast == transact) {
        cerr << "Transaction file cannot have the same name as the Master file. Try again.\n";
        return 1;
    }
    else if(newFile == mast) {
        cerr << "New file cannot have the same name as the Master file. Try again.\n";
        return 1;
    }
    else if(newFile == transact) {
        cerr << "New file cannot have the same name as the Transaction file. Try again.\n";
        return 1;
    }

    return 0;
}

// gets byte offset of file and populates map with ISBN -> offset pairs
long getOffsets(char* fileName, map<unsigned int, long> &offsets) 
{
    // create file stream with the master file,
    // create a temp BookRecord,
    // and create a variable to track byte offset
    BookRec *buff = new BookRec;
    long curr = 0;

    // logic from "Working with Binary Files.docx" on Canvas
    fstream infile(fileName, ios::in | ios::binary);
    while(infile.read((char *) buff, sizeof(BookRec))) {
        // offset map @ the value of isbn is = BookRec size multuplied by the current offset + 1, to avoid collision
        offsets[buff -> isbn] = sizeof(BookRec) * curr++;
    }
    infile.close();

    return curr;
}

// does the main logic of the program, calls other functions for each transaction
void transactions(map<unsigned int, long> &offsetsMap, long &offset, char** argv) 
{
    // get all file names
    string mFile = argv[1];
    char* transact = argv[2];
    char* outfile = argv[3];

    // given by Dr. Digh. 
    // make a copy as to not alter the master file
    system(("cp " + mFile + " copy.out").c_str());

    // tempfile must be readable, writable, binary, and appendable as to 
    // not accidentally overwrite data
    // also, it is a copy of the master so we can edit data without 
    // affecting the master record
    fstream infile(transact, ios::in | ios::binary);
    fstream tempfile("copy.out", ios::in | ios::out | ios::binary | ios:: app);
    fstream error("ERRORS", ios::out);

    // get all transactions from the transaction file
    TransactionRec *transaction = new TransactionRec;
    int counter = 1; // transaction number tracker
    while(infile.read((char *) transaction, sizeof(TransactionRec))) {
        // add contition
        if(transaction -> ToDo == 0) {
            // if it's a duplicate (found in map), let the user know
            if(offsetsMap.find(transaction -> B.isbn) != offsetsMap.end()) {
                error << "Error in transaction number " << counter << ": cannot add---duplicate key " << addZeros(transaction -> B.isbn) << "\n";
            }
            // add the new key to the correct offset position in the map 
            // and update the binary temp file
            else {
                offsetsMap[transaction -> B.isbn] = sizeof(BookRec) * offset++;
                tempfile.write((char *) &(transaction -> B), sizeof(BookRec));
            }
        }
        // delete condition
        else if(transaction -> ToDo == 1) {
            // use same logic as above to search the map for the key value
            // if not found, we cannot delete it... let the user know
            if(offsetsMap.find(transaction -> B.isbn) == offsetsMap.end()) {
                error << "Error in transaction number " << counter << ": cannot delete---no such key " << addZeros(transaction -> B.isbn) << "\n";
            }
            // delete the key/value pair via iterator index from offsets.find 
            // so it doesn't get added to the final file
            else {
                offsetsMap.erase(offsetsMap.find(transaction -> B.isbn));
            }
        }
        // change onhand condition
        else if(transaction -> ToDo == 2) {
            if(offsetsMap.find(transaction -> B.isbn) == offsetsMap.end()) {
                error << "Error in transaction number " << counter << ": cannot change count---no such key " << addZeros(transaction -> B.isbn) << "\n";
            }
            else {
                BookRec curr;
                // move to the offset of the current-needed record, then
                // increase/decrease the onhand amount as needed
                tempfile.seekg(offsetsMap[transaction -> B.isbn], ios::beg);
                tempfile.read((char *) &curr, sizeof(BookRec));
                transaction -> B.onhand += curr.onhand;

                // if the onhand change makes it negative, that is an error
                // we cannot have negative books... make it 0 now
                if(transaction -> B.onhand < 0) {
                    error << "Error in transaction number " << counter << ": count = " << transaction -> B.onhand << " for key " << addZeros(transaction -> B.isbn) << "\n";
                    transaction -> B.onhand = 0;
                }
                // add the new onhand count to the record by going to the 
                // correct record offset and changing that line
                offsetsMap[transaction -> B.isbn] = sizeof(BookRec) * offset++;
                tempfile.write((char *) &(transaction -> B), sizeof(BookRec));
            }
        }
        // change price condition
        else if(transaction -> ToDo == 3) {
            // if the record is not in the map, alert the user
            // otherwise, change the price
            if(offsetsMap.find(transaction -> B.isbn) == offsetsMap.end()) {
                error << "Error in transaction number " << counter << ": cannot change price---no such key " << addZeros(transaction -> B.isbn) << "\n";
            }
            else {
                // same logic as above, go to the correct offset for the record
                // and change the price for the record at that offset,
                // then overwrite it
                BookRec curr;
                tempfile.seekg(offsetsMap[transaction -> B.isbn], ios::beg);
                tempfile.read((char *) &curr, sizeof(BookRec));
                transaction -> B.price += curr.price;

                // write out the new price to the temp file 
                offsetsMap[transaction -> B.isbn] = sizeof(BookRec) * offset++;
                tempfile.write((char *) &(transaction -> B), sizeof(BookRec));
            }
        }
        // increment the transaction counter to know what transaction we are on
        counter++;
    }
    // no more transaction errors can be encountered, close the file
    error.close();

    
    // create new file with updated info
    fstream newfile(outfile, ios::out | ios::binary);
    // logic for <for-loop> from 
    // "https://stackoverflow.com/questions/1443793/iterate-keys-in-a-c-map"
    for(map<unsigned int, long>::iterator it = offsetsMap.begin(); it != offsetsMap.end(); ++it) {
        BookRec curr;
        // first is key, second is value
        // seek to the record, then read it into the buffer
        tempfile.seekg(it -> second);
        tempfile.read((char *) &curr, sizeof(BookRec));
        // write the now-correct records to the end-new file from the buffer
        newfile.write((char *) &curr, sizeof(BookRec));
    }
    // delete the copy file 
    system(("rm copy.out"));
    // no more things to be added, close it
    newfile.close();

    // print the new record
    printRecord(outfile);

    // close remaining opened files
    infile.close();
    tempfile.close();
}

// print the entire file (from my create.cpp)
void printRecord(char* file) 
{
    // clear terminal
    system("clear");
    // create a temp BookRec buffer
    BookRec buff;
    fstream bin(file, ios::in | ios::binary);
 
    // print errors to the terminal 
    fstream errors("ERRORS");
    if(errors) 
        cout << "Transaction Errors:\n" << errors.rdbuf() << endl;
    errors.close();

    // print the contents of the new file
    cout << "Contents of " << file << ":\n" << "--------------------------------------------------------------------------------" << endl;

	while(bin.read((char *) &buff, sizeof(buff))) {
            printInfo(buff);
    }
    bin.close();

    cout << "--------------------------------------------------------------------------------" << endl;
}

// output the line contents for a BookRec buffer line
void printInfo(BookRec buff) 
{ 
    // code from Dr. Digh.
    // sets the output formatting restraints
	cout.setf(ios::fixed, ios::floatfield);
	cout.setf(ios::showpoint);
	cout.precision(2);
   
	cout<<setw(10)<<setfill('0')<<buff.isbn
	<<setw(25)<<setfill(' ')<<buff.name
	<<setw(25)<<buff.author
	<<setw(3)<<buff.onhand
	<<setw(7)<<buff.price
	<<setw(10)<<buff.type<<endl;
}

// append extra '0' to front of ISBNs for correct length
string addZeros(unsigned int &isbn) {
    string toReturn = to_string(isbn);
    while(toReturn.length() < 10) {
        toReturn = "0" + toReturn;
    }
    return toReturn;
}
