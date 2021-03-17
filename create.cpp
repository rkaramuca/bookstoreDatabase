/*
 * Ronald Karamuca
 * CSC 310 Project 1: Bookstore Database Part 2
 * Use fstream to save records to a binary file, 
 * then display the contents of said file.
 */

#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
using namespace std;

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

void parseData(string file);
void printInfo(BookRec buff);

int main(int argc, char *argv[])
{
	// check if the user included a file, then check if the file is valid
	string fileName;
	if( argv[1] != NULL )
		fileName = argv[1];
	else {
		cout << "Bad input, include a file name.\nTry again." << endl;
		return 1;
	}

	// call main logic of program
	parseData(fileName);

	return 0;
}

void parseData(string file) 
{
	// sets the output formatting restraints
	cout.setf(ios::fixed, ios::floatfield);
	cout.setf(ios::showpoint);
	cout.precision(2);

	// open the input and output files
	fstream infile("library.dat", ios::in);
	fstream outfile("library.out", ios::out | ios::binary);

	BookRec buff;

	// get file input into the buffer
	long isbn;
	int line = 1;
	long prevIsbn = 0;
	while( infile >> isbn ) {
		infile.ignore( 1, '|' );
		infile.getline( buff.name, 25, '|' );
		infile.getline( buff.author, 25, '|' );
		infile >> buff.onhand; infile.ignore( 1, '|' );
		infile >> buff.price; infile.ignore( 1, '|' );
		infile.getline( buff.type, 25, '\n' );

		// check if isbn is less than 1
		if( isbn < 1 ) {
			cerr << "> Illegal ISBN number encountered on line 13 of data file - record ignored." << endl;
		}
		// check if previous isbn is larger than the current one
		else if( prevIsbn > isbn ) {
			cerr << "> ISBN number out of sequence on line " << line << " of data." << endl;
			buff.isbn = isbn;
			outfile.write((char *) &buff, sizeof(BookRec));
			printInfo(buff);
		}
		// check if the onhand value is negative
		else if( buff.onhand < 0 ) {
			cerr << "> Negative amount onhand on line " << line << " of data file - record ignored." << endl;
			buff.isbn = isbn;
			printInfo(buff);
		}
		// otherwise push the data to the file
		else {
			buff.isbn = isbn;
			outfile.write((char *) &buff, sizeof(BookRec));
		}

		// inc. line count and update previous isbn value
		line++;
		prevIsbn = isbn;
	}
	// close the file streams
	infile.close();
	outfile.close();

	// print the binary file contents
	fstream bin("library.out", ios::in | ios::binary);
	while(bin.read((char *) &buff, sizeof(buff)))
		printInfo(buff);

	bin.close();
}

// output the buffer contents
void printInfo(BookRec buff) 
{
	cout<<setw(10)<<setfill('0')<<buff.isbn
	<<setw(25)<<setfill(' ')<<buff.name
	<<setw(25)<<buff.author
	<<setw(5)<<buff.onhand
	<<setw(7)<<buff.price
	<<setw(10)<<buff.type<<endl;
}
