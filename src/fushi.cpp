#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <iterator>
#include <fstream>
#include <cstring>

// we define these constants to make our lives easier in show_items()
#define BUY 1
#define SELL 2
#define DISPLAY 3

using namespace std;

struct Item{
	string name;
	int buy, sell;
	Item(string, int, int);
	Item(istream &i);
	friend ostream& operator << (ostream &o, Item &i);
};

// (c++) this is called an initializer list. whenever we have a constructor that just assigns the parameters to a bunch of the data members we can use this concise notation instead of typing out
/*		name = n;
		buy = b;
		sell = s;
*/
// you can also add more code in the body but there was none needed so it's just blank
Item::Item(string n, int b, int s) : name(n), buy(b), sell(s) {
}
// this constructor takes an input file (.list in our case) and reads a single item
Item::Item(istream &i){
	string buystr, sellstr;
	getline(i, name, ','); // the third argument of getline() is normally implictly defined as '\n'. however, for our purposes we define it as ',' instead so we can read all the string data until it hits a ',' into the second variable
	getline(i, buystr, ','); // by the way, filereading/getline are "additive" so each call to it progresses through the file without losing its place and starting over. it probably uses some "pointer" which is a static variable to accomplish this.
	getline(i, sellstr, ',');
	buy = atoi(buystr.c_str()); // since the data from getline() ends up being a string, we need to convert this to an int. (c++) in c++11 there's a better way to do this via stoi(). we called the string::c_str() method because atoi() takes a char*, not string
	sell = atoi(sellstr.c_str());
}

// (c++) this friend function (not method!!) allows you to use ostreams with our Item class, it's pretty useful to know
// with this method we can do something like
//
// cout << item << endl;       where item is of type Item
//
// the idea is you overload ostream's << operator so it knows what to do if it approaches an operand of type Item
// i put it as a friend because typically, you wouldn't have access to the private data in a class (however Item is a struct so everything is public so it was a bit redundant)
ostream& operator<<(ostream &o, Item &i){
	o << i.name << "\t" << i.buy << "\t" << i.sell;
	return o;
}

// this TypeList class represents a set of items tied to what type they are and also if they are chargeable
// essentially it's the data representation of .list files
struct TypeList{
	string type;
	int chargeable;
	vector<Item> items;
	TypeList(char*);
	friend ostream& operator << (ostream &o, TypeList &tl);
};

// fn is the filename that will be read
TypeList::TypeList(char *fn) {
	ifstream f(fn);
	getline(f, type, ',');
	string cc;
	getline(f, cc, ',');
	chargeable = (cc.find('c') != string::npos); // this checks if the charge parameter contains a c. (c++) string::npos means it's nonexistent. i didn't use == because i didn't feel like stripping the whitespace of the parameter
	Item *ii = NULL; // this Item object is a buffer we can read into
	while(f.good()){ // this keeps reading the file until the end
		ii = new Item(f); // read into the buffer and...
		items.push_back(*ii); // ...push it into the vector
	}
	delete ii; // garbage collection!
}

// (c++) this one writes a TypeList to an ostream so you can do something like:
// cout << typelist; ( see earlier comments about ostreams and operator overloading )
ostream&  operator << (ostream &o, TypeList &tl){
	// (c++) this loop is the ideal way to iterate through a vector or any STL container
	/*
		(c++/java/c#) particularly, there's this notion of an "iterator". an iterator's purpose is to allow you to traverse through any container, without actually having to worry about the actual details of how to actually perform said iteration. [tangent sidenote::] of course, it's important to know how to actually do this but when you're trying to solve other problems, mundane code minutia might actually get in the way.
						anyway, an iterator can be thought of as a generalization of the idea of a pointer or an index. you have a starting iterator and an ending iterator and you can sequentially go from the start to the end. note start does not necessarily mean index 0 and end does not necessarily mean index size-1, it can even be backwards! (size-1 start and 0 end would be a reverse iterator)
	*/
	for(vector<Item>::iterator it = tl.items.begin(); it != tl.items.end(); ++it){ 
		cout << *it  << "\t" << tl.type << endl;
	}
	return o;
}

void show_items(vector<TypeList> stuff, int act, int tt){
	bool cc = stuff[tt].chargeable; // i assigned this to a variable because i knew i would be referring to this a lot and typing this bs out is not only tedious but this has to index the array and subtract 1 everytime. minutia optimization tho
	int val, price, input, cmax = cc ? 100 : 1;
	cout << "Price: ";
	cin >> input;
	cout << endl;
	
	// calculate price
	price = input;
	for(vector<Item>::iterator it = stuff[tt].items.begin(); it != stuff[tt].items.end(); ++it){
		val = (act == BUY) ? it->buy : it->sell; // this is where that constant i defined earlier came in handy, i could have also just done act == 1 but wtf does 1 mean?? this is actually more concise and clear because it's saying if the act is BUY
		for(int c = 0; c < cmax; ++c){
			price = input - (val * c / 10);
			if (val == price) {
				cout << it->name;
				if (cc) cout << c;
			}
		}
	}	
}

int main(int argc, char **argv) {
	// the command line arguments are all the data files containing the lists of items so if you don't specify any, then it will just exit (you can make a batch/shell script to make inputting these easier)
	if(argc < 2){
		cout << "usage: fushi <datafiles>" << endl;
		return -1;
	}
	
	// load all the datafiles from the cmd arguments
	vector<TypeList> stuff; // this is a vector containing every single item, each element in it is a TypeList
	TypeList *tl = NULL; // we initialize a buffer TypeList to read the data into
	for(int i = 1; i < argc; ++i){ // this loop initializes all the files from the arguments
		tl = new TypeList(argv[i]);
		stuff.push_back(*tl);
	}
	delete tl;	// clean up your poop crap....
	
	int type_selection = 0;
	int action_selection = 0;


	cout << "Fushigi no Dungeon 2 - Fuurai no Shiren:" << endl
		 << "Shop ID Tool" << endl << endl;
	
	while(1){
		do{
			cout << "Which type?" << endl;
			int i = 0;
			
			// list all the types
			for(vector<TypeList>::iterator it = stuff.begin(); it != stuff.end(); ++it){
				cout << "[" << ++i << "]" << it->type << endl; 
			}
			cin >> type_selection;
			cout << endl;
		} while (type_selection < 1 || type_selection > stuff.size()); // stuff.size() is maximum selection you can make
		--type_selection;
		do{
			cout << "[1]Buying" << endl
				 << "[2]Selling" << endl
				 << "[3]Display Chart" << endl
				 << endl;
			cin >> action_selection;
			cout << endl;
		} while (action_selection < 1 || action_selection > 3);
		
		
		switch(action_selection){ // switches are usually better for menu-like things because it maps to a jumptable in asm while if statements are a bunch of comparisons+branches, but if you want to be a showoff, you could use an array of function pointers
			// calculate
			case BUY: // buy
			case SELL: // sell
				show_items(stuff, action_selection, type_selection); // essentially how i reduced this into a function was observing that there were two fragments of code that were functionally the same but had a couple different variables between the repeats. so i threw all the commonalities into a function and mapped the differences to parameters. this is somewhat of a standard way to break some code into a function. it also lets the actual calculation get abstracted here which is useful because we only need to twiddle with the math in one place instead of 5
				break;
				
			// display (this is a little sloppy and i will probably fix it)
			case DISPLAY:
				for(vector<Item>::iterator it = stuff[type_selection].items.begin(); it != stuff[type_selection].items.end(); ++it){
					cout << (*it);
				}
		}
		cout << endl << endl;
	}

	return 0;
}