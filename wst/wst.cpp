// wst.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include "node.h"
#include "btree.h"
#include "workingsettree.h"
#include <time.h>
using namespace std;

int insert_file(std::string filename, WorkingSetTree<int> &wst) {
	std::ifstream ifs;
	ifs.open(filename);

	// if file doesn't exist or cannot be read
	// print out error message and return errorlevel of 1
	if (!ifs.is_open()) {
		std::cout << filename << " cannot be opened for reading." << std::endl;
		return 1;
	}

	std::string line;

	// read the file line by line
	while (getline(ifs, line)) {
		wst.insert(stoi(line));
		//cout << "1";
	}
	//cout << endl;
	return 0;
}

int search_file(std::string filename, WorkingSetTree<int> &wst) {
	std::ifstream ifs;
	ifs.open(filename);

	// if file doesn't exist or cannot be read
	// print out error message and return errorlevel of 1
	if (!ifs.is_open()) {
		std::cout << filename << " cannot be opened for reading." << std::endl;
		return 1;
	}

	std::string line;

	// read the file line by line
	while (getline(ifs, line)) {
		wst.search(stoi(line));
	}

	return 0;
}

int insert_file_btree(std::string filename, BTree<int> &btree) {
	std::ifstream ifs;
	ifs.open(filename);

	// if file doesn't exist or cannot be read
	// print out error message and return errorlevel of 1
	if (!ifs.is_open()) {
		std::cout << filename << " cannot be opened for reading." << std::endl;
		return 1;
	}

	std::string line;
	int total_levels_traversed = 0;
	// read the file line by line
	while (getline(ifs, line)) {
		total_levels_traversed += btree.insert(stoi(line));
	}

	cout << "Insert complete. Total levels traversed: " << total_levels_traversed << endl;

	return 0;
}

int search_file_btree(std::string filename, BTree<int> &btree) {
	std::ifstream ifs;
	ifs.open(filename);

	// if file doesn't exist or cannot be read
	// print out error message and return errorlevel of 1
	if (!ifs.is_open()) {
		std::cout << filename << " cannot be opened for reading." << std::endl;
		return 1;
	}

	std::string line;

	// read the file line by line
	while (getline(ifs, line)) {
		btree.search(stoi(line));
	}

	return 0;
}

int delete_file_btree(std::string filename, BTree<int> &btree) {
	std::ifstream ifs;
	ifs.open(filename);

	// if file doesn't exist or cannot be read
	// print out error message and return errorlevel of 1
	if (!ifs.is_open()) {
		std::cout << filename << " cannot be opened for reading." << std::endl;
		return 1;
	}

	std::string line;

	// read the file line by line
	while (getline(ifs, line)) {
		btree.remove(stoi(line));
	}

	return 0;
}

void run_btree_command_line() {
	cout << "B-Tree: assumed to only hold int values for testing via command line" << endl;

	string operations_str = "\nPlease enter number (1) insert\t (2) search\t (3) remove\t (4) print\t";
	operations_str +=
		" (5) print in order of MRU to LRU\t (6) remove LRU\t (7) insert to back of linked list\t (8) quit\nfollowed by numbers to insert/delete";
	cout << "Please enter the minimum degree of the B-Tree" << endl;
	string input = "";
	getline(cin, input);
	BTree<int> btree(stoi(input));

	bool end_program = false;
	while (!end_program) {
		cout << operations_str << endl;

		getline(cin, input);
		istringstream iss(input);
		string option = "";
		if (iss >> option) {
			string num_str;
			switch (stoi(option)) {
			case 1: // insert elements
				while (iss >> num_str) {
					btree.insert(stoi(num_str));
				}
				break;
			case 2: // search for elements
				while (iss >> num_str) {
					pair<Node<int>&, int> node_index = btree.search(stoi(num_str));
					cout << num_str << ": ";
					if (node_index.second == -1) {
						cout << "element not found" << endl;
					}
					else {
						cout << "elemnt found at index " << node_index.second << " of the following node..."
							<< node_to_string(node_index.first) << endl;
					}
				}
				break;
			case 3: // delete elements
				while (iss >> num_str) {
					if (btree.remove(stoi(num_str))) {
						cout << num_str << ": deleted successfully" << endl;
					}
					else {
						cout << num_str << ": not found. Not deleted" << endl;
					}
				}
				break;
			case 4: // print current tree
				cout << "current height: " << btree.get_height() << "; max height: " << btree.get_max_height() << endl;
				cout << btree.to_string() << endl;
				break;
			case 5:
				cout << btree.print_ordered_mru() << endl;
				cout << btree.print_ordered_tail() << endl;
				break;
			case 6:
				cout << "removed LRU: " << to_string(btree.remove_lru()) << endl;
				break;
			case 7:
				while (iss >> num_str) {
					btree.insert_lru(stoi(num_str));
				}
				break;
			case 8: // quit program
				end_program = true;
				cout << "Bye" << endl;
				break;
			default:
				cout << "Please enter a valid option" << endl;
			}
		}
	}
}

void run_workingsettree_command_line() {
	cout << "Working Set Tree: assumed to only hold int values for testing via command line" << endl;

	string operations_str = "\nPlease enter number (1) insert\t (2) search\t (3) remove\t (4) print (5) print list\t";
	operations_str +=
		"(6) quit\t\tfollowed by numbers to insert/delete";
	cout << "Please enter the minimum degree of the b tree" << endl;
	string input = "";
	getline(cin, input);
	WorkingSetTree<int> wst(stoi(input));

	bool end_program = false;
	while (!end_program) {
		cout << operations_str << endl;

		getline(cin, input);
		istringstream iss(input);
		string option = "";
		if (iss >> option) {
			string num_str;
			switch (stoi(option)) {
			case 1: // insert elements
				while (iss >> num_str) {
					wst.insert(stoi(num_str));
				}
				break;
			/*case 2: // search for elements
				while (iss >> num_str) {
					pair<Node<int>&, int> node_index = wst.search(stoi(num_str));
					cout << num_str << ": ";
					if (node_index.second == -1) {
						cout << "element not found" << endl;
					}
					else {
						cout << "elemnt found at index " << node_index.second << " of the following node..."
							<< node_to_string(node_index.first) << endl;
					}
				}
				break;*/
				case 2: // search for elements
					while (iss >> num_str) {
						bool is_found = wst.search(stoi(num_str));
						cout << num_str << ": ";
						if (is_found) {
							cout << "element is found" << endl;
						}
						else {
							cout << "element is NOT found" << endl;
						}
					}
					break;
			case 3: // delete elements
				while (iss >> num_str) {
					if (wst.remove(stoi(num_str))) {
						cout << num_str << ": deleted successfully" << endl;
					}
					else {
						cout << num_str << ": not found. Not deleted" << endl;
					}
				}
				break;
			case 4: // print current tree
				cout << wst.to_string() << endl;
				break;
			case 5: // print list
				cout << wst.print_list() << endl;
				break;
			case 6: // quit
				end_program = true;
				cout << "Bye" << endl;
				break;
			default:
				cout << "Please enter a valid option" << endl;
			}
		}
	}
}

void time_btree_sec() {

	time_t start, end;
	double seconds;

	BTree<int> btree;

	time(&start);
	insert_file_btree("data\\original_unique_1_500000", btree);
	time(&end);
	seconds = difftime(end, start);
	cout << "Time taken to insert 500,000 elements into b-tree: " << seconds << endl;
	cout << "btree size: " << btree.size() << endl;
	//string file_string = "data\\unique\\unique";


	// searching
	time(&start);
	search_file_btree("data\\p1_p99_500_50000_0", btree);
	time(&end);

	seconds = difftime(end, start);

	cout << "Time taken to search 50,000 elements in b-tree: " << seconds << endl;


	// deleting
	time(&start);
	delete_file_btree("data\\p1_p99_500_50000_2", btree);
	time(&end);

	seconds = difftime(end, start);

	cout << "Time taken to delete 50,000 elements in b-tree: " << seconds << endl;


	// inserting
	time(&start);
	insert_file_btree("data\\p1_p99_500_50000_1", btree);
	time(&end);

	seconds = difftime(end, start);

	cout << "Time taken to insert 50,000 elements into the b-tree: " << seconds << endl;

}

void time_btree_ms() {

	clock_t t;

	BTree<int> btree;

	t = clock();
	insert_file_btree("data\\original_unique_1_500000", btree);
	t = clock() - t;
	cout << "Time taken to insert 500,000 elements into b-tree: " << t << endl;
	cout << "time: " << t << " miliseconds" << endl;
	cout << CLOCKS_PER_SEC << " clocks per second" << endl;
	cout << "time: " << t*1.0 / CLOCKS_PER_SEC << " seconds" << endl;
	cout << "btree size: " << btree.size() << endl;
	//string file_string = "data\\unique\\unique";


	// searching
	t = clock();
	search_file_btree("data\\p1_p99_500_50000_0", btree);
	t = clock() - t;

	cout << "Time taken to search 50,000 elements in b-tree: " << t << endl;
	cout << "time: " << t << " miliseconds" << endl;
	cout << CLOCKS_PER_SEC << " clocks per second" << endl;
	cout << "time: " << t*1.0 / CLOCKS_PER_SEC << " seconds" << endl;


	// deleting
	t = clock();
	delete_file_btree("data\\p1_p99_500_50000_2", btree);
	t = clock() - t;

	cout << "Time taken to delete 50,000 elements in b-tree: " << t << endl;
	cout << "time: " << t << " miliseconds" << endl;
	cout << CLOCKS_PER_SEC << " clocks per second" << endl;
	cout << "time: " << t*1.0 / CLOCKS_PER_SEC << " seconds" << endl;


	// inserting
	t = clock();
	insert_file_btree("data\\p1_p99_500_50000_1", btree);
	t = clock() - t;

	cout << "Time taken to insert 50,000 elements into the b-tree: " << t << endl;
	cout << "time: " << t << " miliseconds" << endl;
	cout << CLOCKS_PER_SEC << " clocks per second" << endl;
	cout << "time: " << t*1.0 / CLOCKS_PER_SEC << " seconds" << endl;

}

void time_wst_sec() {

	time_t start, end;
	double seconds;

	WorkingSetTree<int> wst;

	time(&start);
	//insert_file("data\\original_unique_1_500000", wst);
	insert_file("data\\original_unique_1_500000", wst);
	time(&end);
	seconds = difftime(end, start);
	cout << "Time taken to insert 500,000 elements: " << seconds << endl;
	cout << "insert completed. size: " << wst.size() << endl;

	//string file_string = "data\\unique\\unique0";

	time(&start);  // get current time; same as: now = time(NULL)
				   //search_file("data\\p1_p99_500", wst);
	search_file("data\\p1_p99_500_50000_0", wst);
	time(&end);

	seconds = difftime(end, start);

	//cout << "Time taken to search 500,000 elements: " << seconds << endl;
	cout << "Time taken to search 50,000 elements: " << seconds << endl;

}

void time_wst_ms() {

	clock_t t;

	WorkingSetTree<int> wst;

	t = clock();
	//insert_file("data\\original_unique_1_500000", wst);
	insert_file("data\\original_unique_1_500000", wst);
	t = clock() - t;
	cout << "Time taken to insert 500,000 elements: " << t << endl;
	cout << "insert completed. size: " << wst.size() << endl;
	cout << "time: " << t << " miliseconds" << endl;
	cout << CLOCKS_PER_SEC << " clocks per second" << endl;
	cout << "time: " << t*1.0 / CLOCKS_PER_SEC << " seconds" << endl;

	//string file_string = "data\\unique\\unique0";

	t = clock(); // get current time; same as: now = time(NULL)
				 //search_file("data\\p1_p99_500", wst);
	search_file("data\\p1_p99_500_50000_0", wst);
	t = clock() - t;

	//cout << "Time taken to search 500,000 elements: " << seconds << endl;
	cout << "Time taken to search 50,000 elements: " << t << endl;
	cout << "time: " << t << " miliseconds" << endl;
	cout << CLOCKS_PER_SEC << " clocks per second" << endl;
	cout << "time: " << t*1.0 / CLOCKS_PER_SEC << " seconds" << endl;

}

int main(int argc, const char * argv[]) {
	// insert code here...



	//run_btree_command_line();
	//run_workingsettree_command_line();
	//time_btree_sec();
	time_btree_ms();

	cout << "\n\n" << endl;

	//time_wst_sec();
	time_wst_ms();




	/*
	WorkingSetTree<int> wst;

	wst.insert(339853);
	wst.insert(91940);
	wst.insert(137048);
	wst.insert(34409);
	wst.insert(335644);
	wst.insert(242963);
	wst.insert(104272);
	wst.insert(426933);
	wst.insert(12591);
	wst.insert(483456);
	wst.insert(415629);
	wst.insert(155210);
	wst.insert(80310);
	wst.insert(204728);
	wst.insert(52599);
	wst.insert(385540);
	wst.insert(468405);
	wst.insert(282036);
	wst.insert(454246);
	wst.insert(323688);
	wst.insert(438550);
	wst.insert(24649);
	wst.insert(68079);
	wst.insert(96612);
	wst.insert(225404);
	wst.insert(101824);
	wst.insert(432084);
	wst.insert(57696);
	wst.insert(7584);
	wst.insert(398687);
	wst.insert(227980);
	wst.insert(428153);
	wst.insert(406972);
	wst.insert(121592);
	wst.insert(205397);
	wst.insert(216443);
	wst.insert(194603);
	wst.insert(164549);
	wst.insert(267071);
	wst.insert(104104);
	wst.insert(50066);
	wst.insert(35660);
	wst.insert(480015);
	wst.insert(182366);
	wst.insert(439287);
	wst.insert(380734);
	wst.insert(107155);
	wst.insert(199082);
	wst.insert(144429);
	wst.insert(271370);
	wst.insert(173308);
	wst.insert(486710);
	wst.insert(101446);
	wst.insert(440372);
	wst.insert(81766);
	wst.insert(109473);
	wst.insert(88511);
	wst.insert(282144);
	wst.insert(391297);
	wst.insert(455941);
	wst.insert(330088);
	wst.insert(379202);
	wst.insert(207021);
	wst.insert(404575);
	wst.insert(294992);
	wst.insert(202560);
	wst.insert(401811);
	wst.insert(469080);
	wst.insert(215875);
	wst.insert(242053);
	wst.insert(351228);
	wst.insert(78161);
	wst.insert(246133);
	wst.insert(480305);
	wst.insert(88255);
	wst.insert(355501);
	wst.insert(242543);
	wst.insert(392000);
	wst.insert(294397);
	wst.insert(275599);
	wst.insert(431041);
	wst.insert(137707);
	wst.insert(377496);
	wst.insert(227922);
	wst.insert(263684);
	wst.insert(300611);
	wst.insert(201369);
	wst.insert(145980);
	wst.insert(104854);
	wst.insert(276070);
	wst.insert(173660);
	wst.insert(497290);
	wst.insert(466722);
	wst.insert(274836);
	wst.insert(148067);
	wst.insert(56915);
	//wst.insert(58583);
	//wst.insert(239978);
	//wst.insert(500000);
	/*wst.insert(466164);
	wst.insert(415616);
	wst.insert(83524);
	wst.insert(476516);
	wst.insert(81189);
	wst.insert(434746);
	wst.insert(186412);
	wst.insert(429181);
	wst.insert(323914);
	wst.insert(264186);
	wst.insert(452701);
	wst.insert(35811);
	wst.insert(194479);
	wst.insert(127224);
	wst.insert(267006);
	cout << wst.to_string() << endl;
	//wst.insert(311400);
	//235832
	//271444

	///*

	string operations_str = "\nPlease enter number (1) insert\t (2) search\t (3) remove\t (4) print (5) print list\t";
	operations_str +=
	"(6) quit\t\t\tfollowed by numbers to insert/delete";
	//cout << "Please enter the minimum degree of the b tree" << endl;
	string input = "";
	//getline(cin, input);
	//WorkingSetTree<int> wst(stoi(input));

	bool end_program = false;
	while (!end_program) {
	cout << operations_str << endl;

	getline(cin, input);
	istringstream iss(input);
	string option = "";
	if (iss >> option) {
	string num_str;
	switch (stoi(option)) {
	case 1: // insert elements
	while (iss >> num_str) {
	wst.insert(stoi(num_str));
	}
	break;
	case 2: // search for elements
	while (iss >> num_str) {
	pair<Node<int>&, int> node_index = wst.search(stoi(num_str));
	cout << num_str << ": ";
	if (node_index.second == -1) {
	cout << "element not found" << endl;
	}
	else {
	cout << "elemnt found at index " << node_index.second << " of the following node..."
	<< node_to_string(node_index.first) << endl;
	}
	}
	break;
	case 3: // delete elements
	while (iss >> num_str) {
	if (wst.remove(stoi(num_str))) {
	cout << num_str << ": deleted successfully" << endl;
	}
	else {
	cout << num_str << ": not found. Not deleted" << endl;
	}
	}
	break;
	case 4: // print current tree
	cout << wst.to_string() << endl;
	break;
	case 5: // print list
	cout << wst.print_list() << endl;
	break;
	case 6: // quit
	end_program = true;
	cout << "Bye" << endl;
	break;
	default:
	cout << "Please enter a valid option" << endl;
	}
	}
	}



	cout << "finished" << endl;



	//*/


	return 0;
}
