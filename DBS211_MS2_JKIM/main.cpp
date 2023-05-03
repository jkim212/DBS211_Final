/*
*****************************************************************************
					DBS211 Project Final Submission
Full Name  : Juhan Kim
Student ID#: 126478221
Email      : jkim687@myseneca.ca
Section    : DBS211 NEE
Date       : Apr 1, 2023

Authenticity Declaration:
I declare this submission is the result of my own work and has not been
shared with any other student or 3rd party content provider. This submitted
piece of work is entirely of my own creation.
*****************************************************************************
*/

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <iomanip>
#include <string>
#include <occi.h>

using oracle::occi::Environment;
using oracle::occi::Connection;
using namespace oracle::occi;
using namespace std;

//Create struct Employee to store Employee info
struct Employee {
	int employeeNumber;
	char lastName[50];
	char firstName[50];
	char email[100];
	char phone[50];
	char extension[10];
	char reportsTo[100];
	char jobTitle[50];
	char city[50];
};


int main(void)
{
	/* OCCI Variables */
	Environment* env = nullptr;
	Connection* conn = nullptr;
	/* Used Variables */
	string user = "";
	string pass = "";
	string constr = "myoracle12c.senecacollege.ca:1521/oracle12c";
	try {
		env = Environment::createEnvironment(Environment::DEFAULT);
		conn = env->createConnection(user, pass, constr);
		std::cout << "DBS211 Project" << endl;
		std::cout << "Group Member: Juhan Kim       126478221      jkim687@myseneca.ca" << endl << endl;

		//Function declarations
		int menu(void);
		int findEmployee(Connection * conn, int employeeNumber, struct Employee* emp);
		void displayEmployee(Connection * conn, struct Employee emp);
		void displayAllEmployees(Connection * conn);
		void addEmployee(Connection * conn);
		void updateEmployee(Connection * conn, int employeeNumber);
		void deleteEmployee(Connection * conn, int employeeNumber);
		//Create Statement variable to excute query
		Statement* stmt = conn->createStatement();

		int option;                                                     //User to store option entered by user

		do {
			option = menu();                                            //Call menu function and store option to variable
			if (option == 1) {          //Case of option 1
				bool flag = true;                                       //Used to flag when user input is valid type (integer)
				bool digit = true;                                      //Used to check each char of user input
				char* test;                                             //Store user input to go through various tests
				int searchNum;                                          //Store user's wanted search number
				string search;                                          //Store raw user input


				do {
					std::cout << "Search by Employee Number: ";          //Prompt user for search
					getline(cin, search);
					test = new char[search.length() + 1];
					strcpy(test, search.c_str());                   //Copy into dynamic C-String test

					for (int i = 0; i < strlen(test); i++) {        //Go through each char in the input and check if every char is a digit
						if (!isdigit(test[i]) && digit) {           //If not a digit, flag variabable 'digit' is set to false
							digit = false;
						}
					}
					if (digit && search.length() > 0) {                                    //If all char are digits, exit loop by setting 'flag' to false
						flag = false;
						searchNum = atoi(test);                     //Change user input to integer using 'atoi' function and set it to searchNum var
					}
					else {                                          //If not all char are digits, print error
						std::cout << "**Please enter an integer**" << endl;
					}
					digit = true;
					delete[] test;                                  //Free dynamic memory
				} while (flag);

				struct Employee foundEmp;                           //Create struct Employee to store found data
				if (findEmployee(conn, searchNum, &foundEmp)) {     //Call findEmployee number and pass on search number and empty employee object
					displayEmployee(conn, foundEmp);                //Formatted display of found employee
				}
				else {                                              //If not found, print error
					std::cout << "Employee " << searchNum << " does not exist." << endl << endl;
				}
			}
			else if (option == 2) {                                 //Case of option 2, display all employees
				displayAllEmployees(conn);
			}
			else if (option == 3) {									//Case of option 3, addEmployee function is used to add an employee to list
				addEmployee(conn);
			}
			else if (option == 4) {									//Case of option 4, update employee extension
				int empNum;											//Variable to store employee number
				bool flag = true;
				struct Employee charlie;							//Temp employee required to pass on to findEmployee function
				cout << "Employee Number: ";
				do {												//Loop until the input is an integer
					cin >> empNum;
					if (cin.fail()) {
						cout << "Please enter a valid Employee Number\n> ";
						cin.clear();
						cin.ignore(1000, '\n');
					}
					else {
						flag = false;
						charlie.employeeNumber = empNum;
					}
				} while (flag);
				int found;
				found = findEmployee(conn, empNum, &charlie);		//Find employee index in the list that matches emp number
				if (found) {
					updateEmployee(conn, empNum);					//If found, update employee
				}
				else {												//Else, print error
					cout << "Employee number " << empNum << " does not exist." << endl;
				}

			}
			else if (option == 5) {									//Case of option 5, delete an employee from the list
				int empNum;											//Get user input of employee number
				bool flag = true;
				struct Employee charlie;
				cout << "Employee Number: ";
				do {
					cin >> empNum;
					if (cin.fail()) {
						cout << "Please enter a valid Employee Number\n> ";
						cin.clear();
						cin.ignore(1000, '\n');
					}
					else {
						flag = false;
						charlie.employeeNumber = empNum;
					}
				} while (flag);
				int found;
				found = findEmployee(conn, empNum, &charlie);		//Find employee
				if (found) {
					deleteEmployee(conn, empNum);					//If found in table, delete
				}
				else {
					cout << "Employee number " << empNum << " does not exist." << endl;
				}
			}
		} while (option != 0);                                      //Case of option 0, exit loop and end program
		std::cout << endl;
		std::cout << "Exiting Program..." << endl;
		std::cout << "Thank you!" << endl;


		conn->terminateStatement(stmt);                         //Terminate all open statements and connections
		env->terminateConnection(conn);
		Environment::terminateEnvironment(env);
	}
	catch (SQLException& sqlExcp) {
		std::cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
	}
	return 0;
}

//Function to display menu and return user option from the displayed menu
int menu(void) {
	bool flag = true;                           //Boolean used to flag when user types in valid option
	int opt;
	std::cout << "********************* HR Menu *********************" << std::endl << std::endl;
	std::cout << "1) Find Employee" << "\n"
		<< "2) Employees Report" << "\n"
		<< "3) Add Employee" << "\n"
		<< "4) Update Employee" << "\n"
		<< "5) Remove Employee" << "\n"
		<< "0) Exit" << std::endl;
	do {
		std::cout << ">";
		cin >> opt;                             //Read in option
		if (cin.fail()) {                       //Case of non-integer input
			std::cout << "Please enter a valid integer between 0 and 5: " << endl;
			cin.clear();                        //Clear error flags
		}
		else if (opt < 0 || opt > 5) {          //Case of out of bound integer input
			std::cout << "Please enter a valid integer between 0 and 5: " << endl;

		}
		else {
			flag = false;                       //Exit loop otherwise
		}
		cin.ignore(1000, '\n');                 //Clear input buffer

	} while (flag);

	return opt;                                 //Return user option
}

//Function used to search employee table by given search number and store to the employee object passed as parameter
int findEmployee(Connection* conn, int searchNum, struct Employee* emp) {
	int ret = 0;
	string query = "SELECT employeenumber, lastname, firstname, email, phone, extension, reportsTo, jobtitle, city "
		"FROM dbs211_employees e1 INNER JOIN dbs211_offices o1 ON e1.officecode = o1.officecode "
		"WHERE employeenumber = ";
	query += std::to_string(searchNum);                             //Concat 'searchNum' to the query to perform a specific search using 'WHERE' clause
	Statement* stmt = conn->createStatement();
	ResultSet* rs2;                                                 //Execute above query
	try {
		rs2 = stmt->executeQuery(query);

		if (!rs2->next()) {                                             //Case of no results
			ret = 0;
		}
		else {
			ret = 1;                                                            //If found, return 1
			do {                                                                //Read in data from the query
				emp->employeeNumber = rs2->getInt(1);
				strncpy(emp->lastName, rs2->getString(2).c_str(), 50);
				strncpy(emp->firstName, rs2->getString(3).c_str(), 50);
				strncpy(emp->email, rs2->getString(4).c_str(), 100);
				strncpy(emp->phone, rs2->getString(5).c_str(), 50);
				strncpy(emp->extension, rs2->getString(6).c_str(), 10);
				strncpy(emp->reportsTo, rs2->getString(7).c_str(), 100);
				strncpy(emp->jobTitle, rs2->getString(8).c_str(), 50);
				strncpy(emp->city, rs2->getString(9).c_str(), 50);
			} while (rs2->next());
		}

	}
	catch (SQLException& e) {
		std::cerr << "Error executing query: " << e.what() << std::endl;
	}


	return ret;
}

//Formatted print of a single employee
void displayEmployee(Connection* conn, struct Employee emp) {
	std::cout << "employeeNumber = " << emp.employeeNumber << "\n"
		<< "lastName = " << emp.lastName << "\n"
		<< "firstName = " << emp.firstName << "\n"
		<< "email = " << emp.email << "\n"
		<< "phone = " << emp.phone << "\n"
		<< "extension = " << emp.extension << "\n"
		<< "reportsTo = " << emp.reportsTo << "\n"
		<< "jobTitle = " << emp.jobTitle << "\n"
		<< "city = " << emp.city << "\n";

}

//Displays a list of all employees
void displayAllEmployees(Connection* conn) {
	Statement* stmt = conn->createStatement();
	//Display Employees
	//Format Table Header
	std::cout << std::setfill(' ') << std::setw(55) << "Employee List" << endl;
	std::cout << std::setw(111) << std::setfill('=') << "" << endl;
	std::cout << std::left << std::setfill(' ');
	std::cout << std::setw(5) << "E" << std::setw(20) << "Employee Name" << std::setw(35) << "Email" << std::setw(20) << "Phone"
		<< std::setw(10) << "Ext" << std::setw(20) << "Manager" << endl;
	std::cout << std::right << std::setfill('-') << std::setw(110) << "-" << endl;
	std::cout << std::setfill(' ') << std::left;

	try {   //Get data using query to select columns from employees table
		//Table given in the instructions was first ordered by the officecode then by employeenumber
		ResultSet* rs2 = stmt->executeQuery("SELECT e1.employeenumber, e1.lastname, e1.firstname, e1.email, o1.phone, "
			"e1.extension, e2.firstName, e2.lastName "
			"FROM dbs211_employees e1 LEFT JOIN dbs211_employees e2 ON e1.reportsTo = e2.employeenumber "
			"INNER JOIN dbs211_offices o1 ON e1.officecode = o1.officecode "
			"ORDER BY e1.officecode, e1.employeenumber");

		while (rs2->next()) {
			int empNum = rs2->getInt(1);                        //Set employee number to integer variable
			string lastN = rs2->getString(2);                   //Set last name to string variable
			string firstN = rs2->getString(3);                  //Set first name to string variable
			string email = rs2->getString(4);
			string phone = rs2->getString(5);
			string extens = rs2->getString(6);
			string managerF = rs2->getString(7);
			string managerL = rs2->getString(8);

			string fullName = firstN + " " + lastN;             //Concat first and last name
			string ManagerFull = managerF + " " + managerL;
			//Format print data
			std::cout << std::left
				<< std::setw(5) << empNum
				<< std::setw(20) << fullName
				<< std::setw(35) << email
				<< std::setw(20) << phone
				<< std::setw(10) << extens
				<< ManagerFull << endl;
		}

		std::cout << endl << endl;

	}
	catch (SQLException& e) {
		std::cerr << "Error executing query: " << e.what() << std::endl;
	}

}


void addEmployee(Connection* conn) {
	void insertEmployee(Connection * conn, struct Employee emp);
	struct Employee charlie;								//Temp employee created to pass on to insertEmployee function
	string input;											//Temp variables created to store user input
	int empNum;
	bool flag = true;
	cout << "Employee Number: ";							//Validate every input
	do {
		cin >> empNum;
		if (cin.fail()) {
			cout << "Please enter a valid Employee Number\n> ";
			cin.clear();
			cin.ignore(1000, '\n');
		}
		else {
			flag = false;
			charlie.employeeNumber = empNum;
			cin.clear();
			cin.ignore(1000, '\n');
		}
	} while (flag);
	flag = true;
	cout << "Last Name: ";
	do {
		std::getline(std::cin, input);
		if (input.length() >= 50 || input.length() < 1) {
			cout << "Please enter a valid Last Name\n >";
		}
		else {
			flag = false;
			strncpy(charlie.lastName, input.c_str(), 50);
			cin.clear();
		}
	} while (flag);
	flag = true;
	cout << "First Name: ";
	do {
		std::getline(std::cin, input);
		if (input.length() >= 50 || input.length() < 1) {
			cout << "Please enter a valid First Name\n >";
		}
		else {
			flag = false;
			strncpy(charlie.firstName, input.c_str(), 50);
			cin.clear();
		}
	} while (flag);
	flag = true;
	cout << "Email: ";
	do {
		std::getline(std::cin, input);
		if (input.length() >= 100 || input.length() < 1) {
			cout << "Please enter a valid Email\n >";
		}
		else {
			flag = false;
			strncpy(charlie.email, input.c_str(), 100);
			cin.clear();
		}
	} while (flag);
	flag = true;
	cout << "extension: ";
	do {
		std::getline(std::cin, input);
		if (input.length() >= 10 || input.length() < 1) {
			cout << "Please enter a valid Extension\n >";
		}
		else {
			flag = false;
			strncpy(charlie.extension, input.c_str(), 10);
			cin.clear();
		}
	} while (flag);
	flag = true;
	cin.clear();
	cout << "Job Title: ";
	do {
		std::getline(std::cin, input);
		if (input.length() >= 50 || input.length() < 1) {
			cout << "Please enter a valid Job Title\n >";
		}
		else {
			flag = false;
			strncpy(charlie.jobTitle, input.c_str(), 50);
			cin.clear();
		}
	} while (flag);
	cin.clear();
	flag = true;
	cout << "City: ";
	do {
		std::getline(std::cin, input);
		if (input.length() >= 50 || input.length() < 1) {
			cout << "Please enter a valid City\n >";
		}
		else {
			flag = false;
			strncpy(charlie.city, input.c_str(), 50);
			cin.clear();
		}
	} while (flag);
	cin.clear();
	flag = true;

	insertEmployee(conn, charlie);								//Pass on to inserEmployee function

}
void insertEmployee(Connection* conn, struct Employee emp) {
	struct Employee decoy;										//Created to pass on to find Employee
	if (findEmployee(conn, emp.employeeNumber, &decoy) != 0) {	//If emp number already exists in table, print error
		cout << "An employee with the same employee number exists.\n";
	}
	else {
		try {													//Use Insert query to add new employee
			string query = "INSERT INTO dbs211_employees (employeenumber, lastname, firstname, extension, email, officecode, reportsTo, jobtitle) \
\nVALUES (" + std::to_string(emp.employeeNumber) + ", '" +
std::string(emp.lastName) + "', '" +
std::string(emp.firstName) + "', '" +
std::string(emp.extension) + "', '" +
std::string(emp.email) + "', '1', 1002, '" +
std::string(emp.jobTitle) + "')";

			Statement* stmt = conn->createStatement();
			ResultSet* rs2 = stmt->executeQuery(query);

			cout << "The new employee is added successfully.\n";


		}
		catch (SQLException& e) {								//If query fails, print error
			std::cerr << "Error executing query: " << e.what() << std::endl;
		}

	}
}

void updateEmployee(Connection* conn, int employeeNumber) {
	std::string input;
	bool flag = true;
	cin.clear();
	cin.ignore(1000, '\n');
	cout << "New Extension: ";									//Validate extension length to match variable length
	do {
		getline(cin, input);
		if (input.length() > 10) {
			cout << "Extensions are valid up to 10 digits\n> ";
			cin.clear();
		}
		else {
			flag = false;
		}
	} while (flag);

																//Use UPDATE query to update employee extension
	string query = "UPDATE dbs211_employees SET extension = '" + input + "' WHERE employeenumber = " + std::to_string(employeeNumber);

	Statement* stmt = conn->createStatement();
	try {
		ResultSet* rs2 = stmt->executeQuery(query);

		cout << "Extension updated successfully.\n";

	}
	catch (SQLException& e) {
		std::cerr << "Error executing query: " << e.what() << std::endl;
	}


}

void deleteEmployee(Connection* conn, int employeeNumber) {

	//Use DELETE query to delete given employee number
	string query = "DELETE FROM dbs211_employees WHERE employeenumber = " + std::to_string(employeeNumber);

	Statement* stmt = conn->createStatement();
	try {
		ResultSet* rs2 = stmt->executeQuery(query);

		cout << "Employee Deleted successfully.\n";

	}
	catch (SQLException& e) {
		std::cerr << "Error executing query: " << e.what() << std::endl;
	}

}