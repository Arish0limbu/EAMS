/*
 * Employment Attendance Management System (EAMS)
 * Main Program File
 * gg
 * A complete console-based employee attendance management system
 */

#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <vector>
#include <map>
#include <set>
#include <climits>

#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
#else
    #include <termios.h>
    #include <unistd.h>
#endif

using namespace std;

// Cross-platform getch function
int getch_custom() {
    #ifdef _WIN32
        return _getch();
    #else
        struct termios oldt, newt;
        int ch;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    #endif
}

// Sleep function
void sleep_ms(int milliseconds) {
    #ifdef _WIN32
        Sleep(milliseconds);
    #else
        usleep(milliseconds * 1000);
    #endif
}

// ==================== UTILITY CLASS ====================
class Utility {
private:
    static int employeeCount;
    static int leaveCount;
    static int salaryCount;
    static int attendanceCount;

public:
    // Get current date as string
    static string getCurrentDate() {
        time_t now = time(0);
        tm* ltm = localtime(&now);
        char buffer[11];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d", ltm);
        return string(buffer);
    }

    // Get current time as string
    static string getCurrentTime() {
        time_t now = time(0);
        tm* ltm = localtime(&now);
        char buffer[9];
        strftime(buffer, sizeof(buffer), "%H:%M:%S", ltm);
        return string(buffer);
    }

    // Get current month as string
    static string getCurrentMonth() {
        time_t now = time(0);
        tm* ltm = localtime(&now);
        char buffer[8];
        strftime(buffer, sizeof(buffer), "%Y-%m", ltm);
        return string(buffer);
    }

    // Validate name (letters only)
    static bool isValidName(const string& name) {
        if (name.empty()) return false;
        for (size_t i = 0; i < name.length(); i++) {
            if (!isalpha(name[i]) && name[i] != ' ') return false;
        }
        return true;
    }

    // Validate phone number (exactly 10 digits)
    static bool isValidPhone(const string& phone) {
        if (phone.length() != 10) return false;
        for (size_t i = 0; i < phone.length(); i++) {
            if (!isdigit(phone[i])) return false;
        }
        return true;
    }

    // Validate email format
    static bool isValidEmail(const string& email) {
        size_t atPos = email.find('@');
        size_t dotPos = email.rfind('.');
        if (atPos == string::npos || dotPos == string::npos) return false;
        if (atPos > dotPos) return false;
        if (atPos == 0 || dotPos == email.length() - 1) return false;
        return true;
    }

    // Validate age (18-65)
    static bool isValidAge(int age) {
        return age >= 18 && age <= 65;
    }

    // Validate salary (positive)
    static bool isValidSalary(double salary) {
        return salary > 0;
    }

    // Validate password (minimum 8 characters)
    static bool isValidPassword(const string& password) {
        return password.length() >= 8;
    }

    // Check if string is not empty
    static bool isNotEmpty(const string& str) {
        return !str.empty();
    }

    // Validate date format (YYYY-MM-DD)
    static bool isValidDate(const string& date) {
        if (date.length() != 10) return false;
        if (date[4] != '-' || date[7] != '-') return false;
        for (int i = 0; i < 10; i++) {
            if (i == 4 || i == 7) continue;
            if (!isdigit(date[i])) return false;
        }
        return true;
    }

    // Calculate days between two dates
    static int calculateDays(const string& startDate, const string& endDate) {
        try {
            int startYear = stoi(startDate.substr(0, 4));
            int startMonth = stoi(startDate.substr(5, 2));
            int startDay = stoi(startDate.substr(8, 2));

            int endYear = stoi(endDate.substr(0, 4));
            int endMonth = stoi(endDate.substr(5, 2));
            int endDay = stoi(endDate.substr(8, 2));

            tm startTm = {0};
            startTm.tm_year = startYear - 1900;
            startTm.tm_mon = startMonth - 1;
            startTm.tm_mday = startDay;

            tm endTm = {0};
            endTm.tm_year = endYear - 1900;
            endTm.tm_mon = endMonth - 1;
            endTm.tm_mday = endDay;

            time_t start = mktime(&startTm);
            time_t end = mktime(&endTm);

            return static_cast<int>(difftime(end, start) / (60 * 60 * 24));
        } catch (...) {
            return 0;
        }
    }

    // Calculate working hours between two times
    static double calculateWorkingHours(const string& checkIn, const string& checkOut) {
        try {
            int inHour = stoi(checkIn.substr(0, 2));
            int inMin = stoi(checkIn.substr(3, 2));
            int inSec = stoi(checkIn.substr(6, 2));

            int outHour = stoi(checkOut.substr(0, 2));
            int outMin = stoi(checkOut.substr(3, 2));
            int outSec = stoi(checkOut.substr(6, 2));

            double inTotal = inHour + inMin / 60.0 + inSec / 3600.0;
            double outTotal = outHour + outMin / 60.0 + outSec / 3600.0;

            return outTotal - inTotal;
        } catch (...) {
            return 0.0;
        }
    }

    // Generate employee ID
    static string generateEmployeeID() {
        employeeCount++;
        return "EMP" + to_string(employeeCount);
    }

    // Generate leave ID
    static string generateLeaveID() {
        leaveCount++;
        return "LV" + to_string(leaveCount);
    }

    // Generate salary ID
    static string generateSalaryID() {
        salaryCount++;
        return "SAL" + to_string(salaryCount);
    }

    // Set employee count (for loading from file)
    static void setEmployeeCount(int count) {
        employeeCount = count;
    }

    // Set leave count (for loading from file)
    static void setLeaveCount(int count) {
        leaveCount = count;
    }

    // Set salary count (for loading from file)
    static void setSalaryCount(int count) {
        salaryCount = count;
    }

    // Get employee count
    static int getEmployeeCount() {
        return employeeCount;
    }

    // Get leave count
    static int getLeaveCount() {
        return leaveCount;
    }

    // Get salary count
    static int getSalaryCount() {
        return salaryCount;
    }

    // Set attendance count (for loading from file)
    static void setAttendanceCount(int count) {
        attendanceCount = count;
    }

    // Get attendance count
    static int getAttendanceCount() {
        return attendanceCount;
    }

    // Generate attendance ID
    static string generateAttendanceID() {
        attendanceCount++;
        return "ATT" + to_string(attendanceCount);
    }

    // Clear input buffer
    static void clearInputBuffer() {
        cin.clear();
        if (cin.rdbuf()->in_avail() > 0) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    // Pause and wait for user input
    static void pause() {
        cout << "\nPress Enter to continue...";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    // Clear screen
    static void clearScreen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }

    // Print centered text
    static void printCentered(const string& text, int width = 80) {
        int padding = (width - text.length()) / 2;
        if (padding > 0) {
            cout << setw(padding) << "" << text << endl;
        } else {
            cout << text << endl;
        }
    }

    // Print box border
    static void printBox(int width = 80) {
        cout << "+";
        for (int i = 0; i < width - 2; i++) {
            cout << "-";
        }
        cout << "+" << endl;
    }

    // Print line
    static void printLine(int width = 80) {
        for (int i = 0; i < width; i++) {
            cout << "-";
        }
        cout << endl;
    }

    // Print header
    static void printHeader(const string& title) {
        printBox();
        printCentered(title);
        printBox();
    }

    // Print loading animation
    static void printLoading(const string& message) {
        cout << message;
        for (int i = 0; i < 3; i++) {
            cout << ".";
            cout.flush();
            sleep_ms(500);
        }
        cout << endl;
    }

    // Mask password input
    static string getPasswordInput() {
        string password;
        char ch;
        while ((ch = getch_custom()) != '\r') {
            if (ch == '\b') {
                if (!password.empty()) {
                    password.pop_back();
                    cout << "\b \b";
                }
            } else {
                password += ch;
                cout << '*';
            }
        }
        cout << endl;
        return password;
    }

    // Get integer input with validation
    static int getIntInput(const string& prompt, int min = -999999, int max = 999999) {
        int value;
        while (true) {
            cout << prompt;
            if (cin >> value) {
                if (value >= min && value <= max) {
                    clearInputBuffer();
                    return value;
                } else {
                    cout << "Input must be between " << min << " and " << max << endl;
                }
            } else {
                cout << "Invalid input. Please enter a number." << endl;
                cin.clear();
                clearInputBuffer();
            }
        }
    }

    // Get double input with validation
    static double getDoubleInput(const string& prompt, double min = -1e308, double max = 1e308) {
        double value;
        while (true) {
            cout << prompt;
            if (cin >> value) {
                if (value >= min && value <= max) {
                    clearInputBuffer();
                    return value;
                } else {
                    cout << "Input must be between " << min << " and " << max << endl;
                }
            } else {
                cout << "Invalid input. Please enter a number." << endl;
                cin.clear();
                clearInputBuffer();
            }
        }
    }

    // Get string input with validation
    static string getStringInput(const string& prompt, bool (*validator)(const string&) = nullptr) {
        string value;
        while (true) {
            cout << prompt;
            getline(cin, value);
            if (validator == nullptr || validator(value)) {
                return value;
            } else {
                cout << "Invalid input. Please try again." << endl;
            }
        }
    }

    // Convert string to lowercase
    static string toLower(string str) {
        transform(str.begin(), str.end(), str.begin(), [](unsigned char c){ return tolower(c); });
        return str;
    }

    // Convert string to uppercase
    static string toUpper(string str) {
        transform(str.begin(), str.end(), str.begin(), [](unsigned char c){ return toupper(c); });
        return str;
    }

    // Trim whitespace from string
    static string trim(string str) {
        size_t start = str.find_first_not_of(" \t\n\r");
        if (start == string::npos) return "";
        size_t end = str.find_last_not_of(" \t\n\r");
        if (end < start) return "";
        return str.substr(start, end - start + 1);
    }
};

// Initialize static members
int Utility::employeeCount = 0;
int Utility::leaveCount = 0;
int Utility::salaryCount = 0;
int Utility::attendanceCount = 0;

// ==================== FILE MANAGER CLASS ====================
class FileManager {
public:
    // Save data to binary file
    template<typename T>
    static bool saveToFile(const string& filename, const vector<T>& data) {
        ofstream outFile(filename, ios::binary);
        if (!outFile) {
            cerr << "Error opening file: " << filename << endl;
            return false;
        }

        size_t size = data.size();
        outFile.write(reinterpret_cast<char*>(&size), sizeof(size));

        if (!outFile) {
            outFile.close();
            return false;
        }

        for (const auto& item : data) {
            outFile.write(reinterpret_cast<const char*>(&item), sizeof(T));
            if (!outFile) {
                outFile.close();
                return false;
            }
        }

        outFile.close();
        return true;
    }

    // Load data from binary file
    template<typename T>
    static vector<T> loadFromFile(const string& filename) {
        vector<T> data;
        ifstream inFile(filename, ios::binary);
        if (!inFile) {
            return data;
        }

        size_t size;
        inFile.read(reinterpret_cast<char*>(&size), sizeof(size));
        
        if (!inFile) {
            inFile.close();
            return data;
        }

        for (size_t i = 0; i < size; i++) {
            T item;
            inFile.read(reinterpret_cast<char*>(&item), sizeof(T));
            if (!inFile) {
                break;
            }
            data.push_back(item);
        }

        inFile.close();
        return data;
    }

    // Check if file exists
    static bool fileExists(const string& filename) {
        ifstream file(filename);
        return file.good();
    }

    // Create backup
    static bool createBackup(const string& sourceFile, const string& backupFile) {
        ifstream source(sourceFile, ios::binary);
        ofstream backup(backupFile, ios::binary);

        if (!source || !backup) {
            return false;
        }

        backup << source.rdbuf();
        source.close();
        backup.close();
        return true;
    }

    // Restore from backup
    static bool restoreBackup(const string& backupFile, const string& targetFile) {
        ifstream backup(backupFile, ios::binary);
        ofstream target(targetFile, ios::binary);

        if (!backup || !target) {
            return false;
        }

        target << backup.rdbuf();
        backup.close();
        target.close();
        return true;
    }
};

// ==================== DEPARTMENT CLASS ====================
class Department {
private:
    string departmentId;
    string departmentName;
    string description;
    string headOfDepartment;

public:
    // Default constructor
    Department() {
        departmentId = "";
        departmentName = "";
        description = "";
        headOfDepartment = "";
    }

    // Parameterized constructor
    Department(string id, string name, string desc, string head) {
        departmentId = id;
        departmentName = name;
        description = desc;
        headOfDepartment = head;
    }

    // Getters
    string getDepartmentId() const { return departmentId; }
    string getDepartmentName() const { return departmentName; }
    string getDescription() const { return description; }
    string getHeadOfDepartment() const { return headOfDepartment; }

    // Setters
    void setDepartmentId(string id) { departmentId = id; }
    void setDepartmentName(string name) { departmentName = name; }
    void setDescription(string desc) { description = desc; }
    void setHeadOfDepartment(string head) { headOfDepartment = head; }

    // Display department information
    void display() const {
        cout << left << setw(15) << departmentId
             << setw(25) << departmentName
             << setw(30) << description
             << setw(25) << headOfDepartment << endl;
    }

    // Operator overloading for comparison
    bool operator==(const Department& other) const {
        return departmentId == other.departmentId;
    }
};

// ==================== EMPLOYEE CLASS ====================
class Employee {
private:
    string employeeId;
    string fullName;
    string gender;
    int age;
    string dateOfBirth;
    string citizenshipNumber;
    string phoneNumber;
    string email;
    string address;
    string department;
    string position;
    string joiningDate;
    double basicSalary;
    string username;
    string password;
    string status; // Active/Inactive

public:
    // Default constructor
    Employee() {
        employeeId = "";
        fullName = "";
        gender = "";
        age = 0;
        dateOfBirth = "";
        citizenshipNumber = "";
        phoneNumber = "";
        email = "";
        address = "";
        department = "";
        position = "";
        joiningDate = "";
        basicSalary = 0.0;
        username = "";
        password = "";
        status = "Active";
    }

    // Parameterized constructor
    Employee(string id, string name, string gen, int ag, string dob, string citNum,
             string phone, string mail, string addr, string dept, string pos,
             string joinDate, double salary, string user, string pass, string stat) {
        employeeId = id;
        fullName = name;
        gender = gen;
        age = ag;
        dateOfBirth = dob;
        citizenshipNumber = citNum;
        phoneNumber = phone;
        email = mail;
        address = addr;
        department = dept;
        position = pos;
        joiningDate = joinDate;
        basicSalary = salary;
        username = user;
        password = pass;
        status = stat;
    }

    // Getters
    string getEmployeeId() const { return employeeId; }
    string getFullName() const { return fullName; }
    string getGender() const { return gender; }
    int getAge() const { return age; }
    string getDateOfBirth() const { return dateOfBirth; }
    string getCitizenshipNumber() const { return citizenshipNumber; }
    string getPhoneNumber() const { return phoneNumber; }
    string getEmail() const { return email; }
    string getAddress() const { return address; }
    string getDepartment() const { return department; }
    string getPosition() const { return position; }
    string getJoiningDate() const { return joiningDate; }
    double getBasicSalary() const { return basicSalary; }
    string getUsername() const { return username; }
    string getPassword() const { return password; }
    string getStatus() const { return status; }

    // Setters
    void setEmployeeId(string id) { employeeId = id; }
    void setFullName(string name) { fullName = name; }
    void setGender(string gen) { gender = gen; }
    void setAge(int ag) { age = ag; }
    void setDateOfBirth(string dob) { dateOfBirth = dob; }
    void setCitizenshipNumber(string citNum) { citizenshipNumber = citNum; }
    void setPhoneNumber(string phone) { phoneNumber = phone; }
    void setEmail(string mail) { email = mail; }
    void setAddress(string addr) { address = addr; }
    void setDepartment(string dept) { department = dept; }
    void setPosition(string pos) { position = pos; }
    void setJoiningDate(string joinDate) { joiningDate = joinDate; }
    void setBasicSalary(double salary) { basicSalary = salary; }
    void setUsername(string user) { username = user; }
    void setPassword(string pass) { password = pass; }
    void setStatus(string stat) { status = stat; }

    // Display employee information
    void display() const {
        cout << left << setw(12) << employeeId
             << setw(25) << fullName
             << setw(10) << gender
             << setw(5) << age
             << setw(15) << department
             << setw(20) << position
             << setw(12) << phoneNumber
             << setw(10) << status << endl;
    }

    // Display detailed employee information
    void displayDetails() const {
        cout << "\n========== EMPLOYEE DETAILS ==========" << endl;
        cout << "Employee ID: " << employeeId << endl;
        cout << "Full Name: " << fullName << endl;
        cout << "Gender: " << gender << endl;
        cout << "Age: " << age << endl;
        cout << "Date of Birth: " << dateOfBirth << endl;
        cout << "Citizenship Number: " << citizenshipNumber << endl;
        cout << "Phone Number: " << phoneNumber << endl;
        cout << "Email: " << email << endl;
        cout << "Address: " << address << endl;
        cout << "Department: " << department << endl;
        cout << "Position: " << position << endl;
        cout << "Joining Date: " << joiningDate << endl;
        cout << "Basic Salary: $" << basicSalary << endl;
        cout << "Username: " << username << endl;
        cout << "Status: " << status << endl;
        cout << "=====================================" << endl;
    }

    // Validate employee data
    bool validate() const {
        if (!Utility::isValidName(fullName)) return false;
        if (!Utility::isValidPhone(phoneNumber)) return false;
        if (!Utility::isValidEmail(email)) return false;
        if (!Utility::isValidAge(age)) return false;
        if (!Utility::isValidSalary(basicSalary)) return false;
        if (!Utility::isValidPassword(password)) return false;
        if (!Utility::isValidDate(dateOfBirth)) return false;
        if (!Utility::isValidDate(joiningDate)) return false;
        return true;
    }

    // Operator overloading for comparison
    bool operator==(const Employee& other) const {
        return employeeId == other.employeeId;
    }

    // Operator overloading for input
    friend istream& operator>>(istream& is, Employee& emp) {
        cout << "Enter Full Name: ";
        is >> ws;
        getline(is, emp.fullName);

        cout << "Enter Gender (Male/Female): ";
        is >> ws;
        getline(is, emp.gender);
        while (emp.gender != "Male" && emp.gender != "Female" && emp.gender != "male" && emp.gender != "female") {
            cout << "Invalid gender! Please enter Male or Female: ";
            is >> ws;
            getline(is, emp.gender);
        }
        // Capitalize first letter
        if (emp.gender[0] >= 'a' && emp.gender[0] <= 'z') {
            emp.gender[0] = toupper(emp.gender[0]);
        }

        cout << "Enter Age: ";
        is >> emp.age;
        while (!Utility::isValidAge(emp.age)) {
            cout << "Invalid age! Age must be between 18 and 65. Please enter again: ";
            is >> emp.age;
        }
        Utility::clearInputBuffer();

        cout << "Enter Date of Birth (YYYY-MM-DD): ";
        is >> ws;
        getline(is, emp.dateOfBirth);
        while (!Utility::isValidDate(emp.dateOfBirth)) {
            cout << "Invalid date format! Please enter in YYYY-MM-DD format: ";
            is >> ws;
            getline(is, emp.dateOfBirth);
        }

        cout << "Enter Citizenship Number: ";
        is >> ws;
        getline(is, emp.citizenshipNumber);

        cout << "Enter Phone Number (10 digits): ";
        is >> ws;
        getline(is, emp.phoneNumber);
        while (!Utility::isValidPhone(emp.phoneNumber)) {
            cout << "Invalid phone number! Please enter exactly 10 digits: ";
            is >> ws;
            getline(is, emp.phoneNumber);
        }

        cout << "Enter Email: ";
        is >> ws;
        getline(is, emp.email);
        while (!Utility::isValidEmail(emp.email)) {
            cout << "Invalid email format! Please enter a valid email: ";
            is >> ws;
            getline(is, emp.email);
        }

        cout << "Enter Address: ";
        is >> ws;
        getline(is, emp.address);

        cout << "Enter Department: ";
        is >> ws;
        getline(is, emp.department);

        cout << "Enter Position: ";
        is >> ws;
        getline(is, emp.position);

        cout << "Enter Joining Date (YYYY-MM-DD): ";
        is >> ws;
        getline(is, emp.joiningDate);
        while (!Utility::isValidDate(emp.joiningDate)) {
            cout << "Invalid date format! Please enter in YYYY-MM-DD format: ";
            is >> ws;
            getline(is, emp.joiningDate);
        }

        cout << "Enter Basic Salary: ";
        is >> emp.basicSalary;
        while (!Utility::isValidSalary(emp.basicSalary)) {
            cout << "Invalid salary! Salary must be positive. Please enter again: ";
            is >> emp.basicSalary;
        }
        Utility::clearInputBuffer();

        cout << "Enter Username: ";
        is >> ws;
        getline(is, emp.username);
        while (emp.username.empty()) {
            cout << "Username cannot be empty! Please enter again: ";
            is >> ws;
            getline(is, emp.username);
        }

        cout << "Enter Password: ";
        emp.password = Utility::getPasswordInput();
        while (!Utility::isValidPassword(emp.password)) {
            cout << "Password must be at least 8 characters! Please enter again: ";
            emp.password = Utility::getPasswordInput();
        }

        emp.status = "Active";
        emp.employeeId = Utility::generateEmployeeID();

        return is;
    }
};

// ==================== ATTENDANCE CLASS ====================
class Attendance {
private:
    string attendanceId;
    string employeeId;
    string employeeName;
    string date;
    string checkInTime;
    string checkOutTime;
    double workingHours;
    double overtimeHours;
    string status; // Present/Absent/Late

public:
    // Default constructor
    Attendance() {
        attendanceId = "";
        employeeId = "";
        employeeName = "";
        date = "";
        checkInTime = "";
        checkOutTime = "";
        workingHours = 0.0;
        overtimeHours = 0.0;
        status = "Absent";
    }

    // Parameterized constructor
    Attendance(string id, string empId, string empName, string dt,
               string checkIn, string checkOut, double workHours, double overHours, string stat) {
        attendanceId = id;
        employeeId = empId;
        employeeName = empName;
        date = dt;
        checkInTime = checkIn;
        checkOutTime = checkOut;
        workingHours = workHours;
        overtimeHours = overHours;
        status = stat;
    }

    // Getters
    string getAttendanceId() const { return attendanceId; }
    string getEmployeeId() const { return employeeId; }
    string getEmployeeName() const { return employeeName; }
    string getDate() const { return date; }
    string getCheckInTime() const { return checkInTime; }
    string getCheckOutTime() const { return checkOutTime; }
    double getWorkingHours() const { return workingHours; }
    double getOvertimeHours() const { return overtimeHours; }
    string getStatus() const { return status; }

    // Setters
    void setAttendanceId(string id) { attendanceId = id; }
    void setEmployeeId(string empId) { employeeId = empId; }
    void setEmployeeName(string empName) { employeeName = empName; }
    void setDate(string dt) { date = dt; }
    void setCheckInTime(string checkIn) { checkInTime = checkIn; }
    void setCheckOutTime(string checkOut) { checkOutTime = checkOut; }
    void setWorkingHours(double hours) { workingHours = hours; }
    void setOvertimeHours(double hours) { overtimeHours = hours; }
    void setStatus(string stat) { status = stat; }

    // Display attendance information
    void display() const {
        cout << left << setw(15) << attendanceId
             << setw(15) << employeeId
             << setw(25) << employeeName
             << setw(12) << date
             << setw(10) << checkInTime
             << setw(10) << checkOutTime
             << setw(12) << fixed << setprecision(2) << workingHours
             << setw(10) << fixed << setprecision(2) << overtimeHours
             << setw(10) << status << endl;
    }

    // Check if already checked in
    bool isCheckedIn() const {
        return !checkInTime.empty();
    }

    // Check if already checked out
    bool isCheckedOut() const {
        return !checkOutTime.empty();
    }

    // Check if late arrival (after 9:00 AM)
    bool isLateArrival() const {
        if (checkInTime.empty()) return false;
        try {
            int hour = stoi(checkInTime.substr(0, 2));
            return hour >= 9;
        } catch (...) {
            return false;
        }
    }

    // Check if early departure (before 5:00 PM)
    bool isEarlyDeparture() const {
        if (checkOutTime.empty()) return false;
        try {
            int hour = stoi(checkOutTime.substr(0, 2));
            return hour < 17;
        } catch (...) {
            return false;
        }
    }

    // Operator overloading for comparison
    bool operator==(const Attendance& other) const {
        return attendanceId == other.attendanceId;
    }
};

// ==================== LEAVE CLASS ====================
class Leave {
private:
    string leaveId;
    string employeeId;
    string employeeName;
    string leaveType; // Sick, Casual, Annual, Emergency
    string startDate;
    string endDate;
    int totalDays;
    string reason;
    string status; // Pending, Approved, Rejected

public:
    // Default constructor
    Leave() {
        leaveId = "";
        employeeId = "";
        employeeName = "";
        leaveType = "";
        startDate = "";
        endDate = "";
        totalDays = 0;
        reason = "";
        status = "Pending";
    }

    // Parameterized constructor
    Leave(string id, string empId, string empName, string type,
          string start, string end, int days, string rsn, string stat) {
        leaveId = id;
        employeeId = empId;
        employeeName = empName;
        leaveType = type;
        startDate = start;
        endDate = end;
        totalDays = days;
        reason = rsn;
        status = stat;
    }

    // Getters
    string getLeaveId() const { return leaveId; }
    string getEmployeeId() const { return employeeId; }
    string getEmployeeName() const { return employeeName; }
    string getLeaveType() const { return leaveType; }
    string getStartDate() const { return startDate; }
    string getEndDate() const { return endDate; }
    int getTotalDays() const { return totalDays; }
    string getReason() const { return reason; }
    string getStatus() const { return status; }

    // Setters
    void setLeaveId(string id) { leaveId = id; }
    void setEmployeeId(string empId) { employeeId = empId; }
    void setEmployeeName(string empName) { employeeName = empName; }
    void setLeaveType(string type) { leaveType = type; }
    void setStartDate(string start) { startDate = start; }
    void setEndDate(string end) { endDate = end; }
    void setTotalDays(int days) { totalDays = days; }
    void setReason(string rsn) { reason = rsn; }
    void setStatus(string stat) { status = stat; }

    // Display leave information
    void display() const {
        cout << left << setw(12) << leaveId
             << setw(15) << employeeId
             << setw(25) << employeeName
             << setw(15) << leaveType
             << setw(12) << startDate
             << setw(12) << endDate
             << setw(8) << totalDays
             << setw(20) << (reason.length() > 20 ? reason.substr(0, 20) : reason)
             << setw(12) << status << endl;
    }

    // Display detailed leave information
    void displayDetails() const {
        cout << "\n========== LEAVE DETAILS ==========" << endl;
        cout << "Leave ID: " << leaveId << endl;
        cout << "Employee ID: " << employeeId << endl;
        cout << "Employee Name: " << employeeName << endl;
        cout << "Leave Type: " << leaveType << endl;
        cout << "Start Date: " << startDate << endl;
        cout << "End Date: " << endDate << endl;
        cout << "Total Days: " << totalDays << endl;
        cout << "Reason: " << reason << endl;
        cout << "Status: " << status << endl;
        cout << "===================================" << endl;
    }

    // Check if leave is approved
    bool isApproved() const {
        return status == "Approved";
    }

    // Check if leave is pending
    bool isPending() const {
        return status == "Pending";
    }

    // Operator overloading for comparison
    bool operator==(const Leave& other) const {
        return leaveId == other.leaveId;
    }
};

// ==================== SALARY CLASS ====================
class Salary {
private:
    string salaryId;
    string employeeId;
    string employeeName;
    string department;
    string month;
    int workingDays;
    int presentDays;
    int absentDays;
    int leaveDays;
    double overtimeHours;
    double basicSalary;
    double overtimePay;
    double bonus;
    double tax;
    double deduction;
    double grossSalary;
    double netSalary;

public:
    // Default constructor
    Salary() {
        salaryId = "";
        employeeId = "";
        employeeName = "";
        department = "";
        month = "";
        workingDays = 0;
        presentDays = 0;
        absentDays = 0;
        leaveDays = 0;
        overtimeHours = 0.0;
        basicSalary = 0.0;
        overtimePay = 0.0;
        bonus = 0.0;
        tax = 0.0;
        deduction = 0.0;
        grossSalary = 0.0;
        netSalary = 0.0;
    }

    // Parameterized constructor
    Salary(string id, string empId, string empName, string dept, string mnth,
           int workDays, int presDays, int absDays, int lvDays, double overHours,
           double basicSal, double overPay, double bon, double tx, double ded,
           double gross, double net) {
        salaryId = id;
        employeeId = empId;
        employeeName = empName;
        department = dept;
        month = mnth;
        workingDays = workDays;
        presentDays = presDays;
        absentDays = absDays;
        leaveDays = lvDays;
        overtimeHours = overHours;
        basicSalary = basicSal;
        overtimePay = overPay;
        bonus = bon;
        tax = tx;
        deduction = ded;
        grossSalary = gross;
        netSalary = net;
    }

    // Getters
    string getSalaryId() const { return salaryId; }
    string getEmployeeId() const { return employeeId; }
    string getEmployeeName() const { return employeeName; }
    string getDepartment() const { return department; }
    string getMonth() const { return month; }
    int getWorkingDays() const { return workingDays; }
    int getPresentDays() const { return presentDays; }
    int getAbsentDays() const { return absentDays; }
    int getLeaveDays() const { return leaveDays; }
    double getOvertimeHours() const { return overtimeHours; }
    double getBasicSalary() const { return basicSalary; }
    double getOvertimePay() const { return overtimePay; }
    double getBonus() const { return bonus; }
    double getTax() const { return tax; }
    double getDeduction() const { return deduction; }
    double getGrossSalary() const { return grossSalary; }
    double getNetSalary() const { return netSalary; }

    // Setters
    void setSalaryId(string id) { salaryId = id; }
    void setEmployeeId(string empId) { employeeId = empId; }
    void setEmployeeName(string empName) { employeeName = empName; }
    void setDepartment(string dept) { department = dept; }
    void setMonth(string mnth) { month = mnth; }
    void setWorkingDays(int days) { workingDays = days; }
    void setPresentDays(int days) { presentDays = days; }
    void setAbsentDays(int days) { absentDays = days; }
    void setLeaveDays(int days) { leaveDays = days; }
    void setOvertimeHours(double hours) { overtimeHours = hours; }
    void setBasicSalary(double salary) { basicSalary = salary; }
    void setOvertimePay(double pay) { overtimePay = pay; }
    void setBonus(double bon) { bonus = bon; }
    void setTax(double tx) { tax = tx; }
    void setDeduction(double ded) { deduction = ded; }
    void setGrossSalary(double gross) { grossSalary = gross; }
    void setNetSalary(double net) { netSalary = net; }

    // Calculate salary
    void calculateSalary() {
        // Calculate overtime pay (assuming $20 per overtime hour)
        overtimePay = overtimeHours * 20.0;

        // Calculate gross salary
        grossSalary = basicSalary + overtimePay + bonus;

        // Calculate tax (10% of gross salary)
        tax = grossSalary * 0.10;

        // Calculate deduction (for absent days)
        double dailyRate = basicSalary / workingDays;
        deduction = (absentDays * dailyRate) + tax;

        // Calculate net salary
        netSalary = grossSalary - deduction;
    }

    // Display salary information
    void display() const {
        cout << left << setw(12) << salaryId
             << setw(15) << employeeId
             << setw(25) << employeeName
             << setw(15) << department
             << setw(10) << month
             << setw(8) << presentDays
             << setw(8) << leaveDays
             << setw(10) << fixed << setprecision(2) << overtimeHours
             << setw(12) << fixed << setprecision(2) << grossSalary
             << setw(12) << fixed << setprecision(2) << netSalary << endl;
    }

    // Display salary slip
    void displaySalarySlip() const {
        cout << "\n============================================" << endl;
        cout << "              SALARY SLIP                    " << endl;
        cout << "============================================" << endl;
        cout << "Salary ID: " << salaryId << endl;
        cout << "Employee Name: " << employeeName << endl;
        cout << "Employee ID: " << employeeId << endl;
        cout << "Department: " << department << endl;
        cout << "Month: " << month << endl;
        cout << "--------------------------------------------" << endl;
        cout << "Working Days: " << workingDays << endl;
        cout << "Present Days: " << presentDays << endl;
        cout << "Absent Days: " << absentDays << endl;
        cout << "Leave Days: " << leaveDays << endl;
        cout << "Overtime Hours: " << fixed << setprecision(2) << overtimeHours << endl;
        cout << "--------------------------------------------" << endl;
        cout << "Basic Salary: $" << fixed << setprecision(2) << basicSalary << endl;
        cout << "Overtime Pay: $" << fixed << setprecision(2) << overtimePay << endl;
        cout << "Bonus: $" << fixed << setprecision(2) << bonus << endl;
        cout << "Gross Salary: $" << fixed << setprecision(2) << grossSalary << endl;
        cout << "Tax: $" << fixed << setprecision(2) << tax << endl;
        cout << "Deduction: $" << fixed << setprecision(2) << deduction << endl;
        cout << "--------------------------------------------" << endl;
        cout << "NET SALARY: $" << fixed << setprecision(2) << netSalary << endl;
        cout << "============================================" << endl;
    }

    // Operator overloading for comparison
    bool operator==(const Salary& other) const {
        return salaryId == other.salaryId;
    }
};

// ==================== ADMIN CLASS ====================
class Admin {
private:
    string username;
    string password;

public:
    // Default constructor
    Admin() {
        username = "admin";
        password = "admin123";
    }

    // Parameterized constructor
    Admin(string user, string pass) {
        username = user;
        password = pass;
    }

    // Getters
    string getUsername() const { return username; }
    string getPassword() const { return password; }

    // Setters
    void setUsername(string user) { username = user; }
    void setPassword(string pass) { password = pass; }

    // Verify login
    bool verifyLogin(string user, string pass) const {
        return username == user && password == pass;
    }

    // Change password
    bool changePassword(string oldPass, string newPass) {
        if (password == oldPass) {
            if (Utility::isValidPassword(newPass)) {
                password = newPass;
                return true;
            }
        }
        return false;
    }
};

// ==================== REPORT CLASS ====================
class Report {
public:
    // Generate daily attendance report
    static void generateDailyAttendanceReport(const vector<Attendance>& attendanceList, const string& date) {
        cout << "\n========== DAILY ATTENDANCE REPORT ==========" << endl;
        cout << "Date: " << date << endl;
        cout << "--------------------------------------------" << endl;

        cout << left << setw(15) << "Attendance ID"
             << setw(15) << "Employee ID"
             << setw(25) << "Employee Name"
             << setw(10) << "Check In"
             << setw(10) << "Check Out"
             << setw(12) << "Work Hours"
             << setw(10) << "Status" << endl;
        cout << "--------------------------------------------" << endl;

        int presentCount = 0;
        int absentCount = 0;
        int lateCount = 0;

        for (const auto& att : attendanceList) {
            if (att.getDate() == date) {
                att.display();
                if (att.getStatus() == "Present") presentCount++;
                else if (att.getStatus() == "Absent") absentCount++;
                if (att.isLateArrival()) lateCount++;
            }
        }

        cout << "--------------------------------------------" << endl;
        cout << "Total Present: " << presentCount << endl;
        cout << "Total Absent: " << absentCount << endl;
        cout << "Late Arrivals: " << lateCount << endl;
        cout << "============================================" << endl;
    }

    // Generate monthly attendance report
    static void generateMonthlyAttendanceReport(const vector<Attendance>& attendanceList, const string& month) {
        cout << "\n========== MONTHLY ATTENDANCE REPORT ==========" << endl;
        cout << "Month: " << month << endl;
        cout << "--------------------------------------------" << endl;

        cout << left << setw(15) << "Attendance ID"
             << setw(15) << "Employee ID"
             << setw(25) << "Employee Name"
             << setw(12) << "Date"
             << setw(10) << "Status" << endl;
        cout << "--------------------------------------------" << endl;

        int presentCount = 0;
        int absentCount = 0;

        for (const auto& att : attendanceList) {
            if (att.getDate().length() >= 7 && att.getDate().substr(0, 7) == month) {
                cout << left << setw(15) << att.getAttendanceId()
                     << setw(15) << att.getEmployeeId()
                     << setw(25) << att.getEmployeeName()
                     << setw(12) << att.getDate()
                     << setw(10) << att.getStatus() << endl;

                if (att.getStatus() == "Present") presentCount++;
                else if (att.getStatus() == "Absent") absentCount++;
            }
        }

        cout << "--------------------------------------------" << endl;
        cout << "Total Present: " << presentCount << endl;
        cout << "Total Absent: " << absentCount << endl;
        cout << "============================================" << endl;
    }

    // Generate employee list report
    static void generateEmployeeListReport(const vector<Employee>& employeeList) {
        cout << "\n========== EMPLOYEE LIST REPORT ==========" << endl;
        cout << "--------------------------------------------" << endl;

        cout << left << setw(12) << "Employee ID"
             << setw(25) << "Full Name"
             << setw(15) << "Department"
             << setw(20) << "Position"
             << setw(12) << "Phone"
             << setw(10) << "Status" << endl;
        cout << "--------------------------------------------" << endl;

        int activeCount = 0;
        int inactiveCount = 0;

        for (const auto& emp : employeeList) {
            emp.display();
            if (emp.getStatus() == "Active") activeCount++;
            else inactiveCount++;
        }

        cout << "--------------------------------------------" << endl;
        cout << "Total Active: " << activeCount << endl;
        cout << "Total Inactive: " << inactiveCount << endl;
        cout << "Total Employees: " << employeeList.size() << endl;
        cout << "============================================" << endl;
    }

    // Generate salary report
    static void generateSalaryReport(const vector<Salary>& salaryList, const string& month) {
        cout << "\n========== SALARY REPORT ==========" << endl;
        cout << "Month: " << month << endl;
        cout << "--------------------------------------------" << endl;

        cout << left << setw(15) << "Employee ID"
             << setw(25) << "Employee Name"
             << setw(15) << "Department"
             << setw(8) << "Present"
             << setw(8) << "Leave"
             << setw(10) << "Overtime"
             << setw(12) << "Gross"
             << setw(12) << "Net Salary" << endl;
        cout << "--------------------------------------------" << endl;

        double totalGross = 0.0;
        double totalNet = 0.0;

        for (const auto& sal : salaryList) {
            if (sal.getMonth() == month) {
                cout << left << setw(15) << sal.getEmployeeId()
                     << setw(25) << sal.getEmployeeName()
                     << setw(15) << sal.getDepartment()
                     << setw(8) << sal.getPresentDays()
                     << setw(8) << sal.getLeaveDays()
                     << setw(10) << fixed << setprecision(2) << sal.getOvertimeHours()
                     << setw(12) << fixed << setprecision(2) << sal.getGrossSalary()
                     << setw(12) << fixed << setprecision(2) << sal.getNetSalary() << endl;

                totalGross += sal.getGrossSalary();
                totalNet += sal.getNetSalary();
            }
        }

        cout << "--------------------------------------------" << endl;
        cout << "Total Gross Salary: $" << fixed << setprecision(2) << totalGross << endl;
        cout << "Total Net Salary: $" << fixed << setprecision(2) << totalNet << endl;
        cout << "============================================" << endl;
    }

    // Generate leave report
    static void generateLeaveReport(const vector<Leave>& leaveList) {
        cout << "\n========== LEAVE REPORT ==========" << endl;
        cout << "--------------------------------------------" << endl;

        cout << left << setw(12) << "Leave ID"
             << setw(15) << "Employee ID"
             << setw(25) << "Employee Name"
             << setw(15) << "Leave Type"
             << setw(12) << "Start Date"
             << setw(12) << "End Date"
             << setw(8) << "Days"
             << setw(12) << "Status" << endl;
        cout << "--------------------------------------------" << endl;

        int approvedCount = 0;
        int rejectedCount = 0;
        int pendingCount = 0;

        for (const auto& lv : leaveList) {
            lv.display();
            if (lv.getStatus() == "Approved") approvedCount++;
            else if (lv.getStatus() == "Rejected") rejectedCount++;
            else if (lv.getStatus() == "Pending") pendingCount++;
        }

        cout << "--------------------------------------------" << endl;
        cout << "Total Approved: " << approvedCount << endl;
        cout << "Total Rejected: " << rejectedCount << endl;
        cout << "Total Pending: " << pendingCount << endl;
        cout << "============================================" << endl;
    }

    // Generate department report
    static void generateDepartmentReport(const vector<Employee>& employeeList, const vector<Department>& departmentList) {
        cout << "\n========== DEPARTMENT REPORT ==========" << endl;
        cout << "--------------------------------------------" << endl;

        for (const auto& dept : departmentList) {
            int count = 0;
            for (const auto& emp : employeeList) {
                if (emp.getDepartment() == dept.getDepartmentName() && emp.getStatus() == "Active") {
                    count++;
                }
            }
            cout << left << setw(25) << dept.getDepartmentName()
                 << "Active Employees: " << count << endl;
        }

        cout << "============================================" << endl;
    }

    // Find highest attendance employee
    static void findHighestAttendance(const vector<Attendance>& attendanceList, const vector<Employee>& employeeList) {
        cout << "\n========== HIGHEST ATTENDANCE EMPLOYEE ==========" << endl;

        map<string, int> attendanceCount;
        for (const auto& att : attendanceList) {
            if (att.getStatus() == "Present") {
                attendanceCount[att.getEmployeeId()]++;
            }
        }

        if (attendanceCount.empty()) {
            cout << "No attendance records found." << endl;
            return;
        }

        string highestEmpId;
        int maxCount = 0;
        for (const auto& pair : attendanceCount) {
            if (pair.second > maxCount) {
                maxCount = pair.second;
                highestEmpId = pair.first;
            }
        }

        for (const auto& emp : employeeList) {
            if (emp.getEmployeeId() == highestEmpId) {
                cout << "Employee Name: " << emp.getFullName() << endl;
                cout << "Employee ID: " << emp.getEmployeeId() << endl;
                cout << "Department: " << emp.getDepartment() << endl;
                cout << "Present Days: " << maxCount << endl;
                break;
            }
        }

        cout << "================================================" << endl;
    }

    // Find lowest attendance employee
    static void findLowestAttendance(const vector<Attendance>& attendanceList, const vector<Employee>& employeeList) {
        cout << "\n========== LOWEST ATTENDANCE EMPLOYEE ==========" << endl;

        map<string, int> attendanceCount;
        for (const auto& att : attendanceList) {
            if (att.getStatus() == "Present") {
                attendanceCount[att.getEmployeeId()]++;
            }
        }

        if (attendanceCount.empty()) {
            cout << "No attendance records found." << endl;
            return;
        }

        string lowestEmpId;
        int minCount = 999999;
        for (const auto& pair : attendanceCount) {
            if (pair.second < minCount) {
                minCount = pair.second;
                lowestEmpId = pair.first;
            }
        }

        for (const auto& emp : employeeList) {
            if (emp.getEmployeeId() == lowestEmpId) {
                cout << "Employee Name: " << emp.getFullName() << endl;
                cout << "Employee ID: " << emp.getEmployeeId() << endl;
                cout << "Department: " << emp.getDepartment() << endl;
                cout << "Present Days: " << minCount << endl;
                break;
            }
        }

        cout << "===============================================" << endl;
    }

    // Find top overtime employees
    static void findTopOvertimeEmployees(const vector<Attendance>& attendanceList, const vector<Employee>& employeeList, int topN = 5) {
        cout << "\n========== TOP OVERTIME EMPLOYEES ==========" << endl;

        map<string, double> overtimeMap;
        for (const auto& att : attendanceList) {
            overtimeMap[att.getEmployeeId()] += att.getOvertimeHours();
        }

        if (overtimeMap.empty()) {
            cout << "No overtime records found." << endl;
            return;
        }

        vector<pair<string, double>> overtimeList(overtimeMap.begin(), overtimeMap.end());
        sort(overtimeList.begin(), overtimeList.end(),
             [](const pair<string, double>& a, const pair<string, double>& b) {
                 return a.second > b.second;
             });

        cout << left << setw(25) << "Employee Name"
             << setw(15) << "Employee ID"
             << setw(15) << "Overtime Hours" << endl;
        cout << "--------------------------------------------" << endl;

        int count = 0;
        for (const auto& pair : overtimeList) {
            if (count >= topN) break;

            for (const auto& emp : employeeList) {
                if (emp.getEmployeeId() == pair.first) {
                    cout << left << setw(25) << emp.getFullName()
                         << setw(15) << emp.getEmployeeId()
                         << setw(15) << fixed << setprecision(2) << pair.second << endl;
                    break;
                }
            }
            count++;
        }

        cout << "============================================" << endl;
    }

    // Find absent employees
    static void findAbsentEmployees(const vector<Attendance>& attendanceList, const vector<Employee>& employeeList, const string& date) {
        cout << "\n========== ABSENT EMPLOYEES ==========" << endl;
        cout << "Date: " << date << endl;
        cout << "--------------------------------------------" << endl;

        set<string> presentIds;
        for (const auto& att : attendanceList) {
            if (att.getDate() == date && att.getStatus() == "Present") {
                presentIds.insert(att.getEmployeeId());
            }
        }

        cout << left << setw(15) << "Employee ID"
             << setw(25) << "Employee Name"
             << setw(15) << "Department" << endl;
        cout << "--------------------------------------------" << endl;

        int absentCount = 0;
        for (const auto& emp : employeeList) {
            if (emp.getStatus() == "Active" && presentIds.find(emp.getEmployeeId()) == presentIds.end()) {
                cout << left << setw(15) << emp.getEmployeeId()
                     << setw(25) << emp.getFullName()
                     << setw(15) << emp.getDepartment() << endl;
                absentCount++;
            }
        }

        cout << "--------------------------------------------" << endl;
        cout << "Total Absent: " << absentCount << endl;
        cout << "============================================" << endl;
    }
};

// ==================== MENU CLASS ====================
class Menu {
private:
    Admin admin;
    vector<Employee> employees;
    vector<Attendance> attendanceList;
    vector<Leave> leaveList;
    vector<Salary> salaryList;
    vector<Department> departments;

    // Load all data from files
    void loadAllData() {
        employees = FileManager::loadFromFile<Employee>("employees.dat");
        attendanceList = FileManager::loadFromFile<Attendance>("attendance.dat");
        leaveList = FileManager::loadFromFile<Leave>("leave.dat");
        salaryList = FileManager::loadFromFile<Salary>("salary.dat");
        departments = FileManager::loadFromFile<Department>("department.dat");

        // Update counters based on actual IDs in data
        int maxEmpId = 0;
        for (const auto& emp : employees) {
            string id = emp.getEmployeeId();
            if (id.length() >= 4 && id.substr(0, 3) == "EMP") {
                try {
                    int num = stoi(id.substr(3));
                    if (num > maxEmpId) maxEmpId = num;
                } catch (...) {}
            }
        }
        Utility::setEmployeeCount(maxEmpId);

        int maxLeaveId = 0;
        for (const auto& lv : leaveList) {
            string id = lv.getLeaveId();
            if (id.length() >= 3 && id.substr(0, 2) == "LV") {
                try {
                    int num = stoi(id.substr(2));
                    if (num > maxLeaveId) maxLeaveId = num;
                } catch (...) {}
            }
        }
        Utility::setLeaveCount(maxLeaveId);

        int maxSalaryId = 0;
        for (const auto& sal : salaryList) {
            string id = sal.getSalaryId();
            if (id.length() >= 4 && id.substr(0, 3) == "SAL") {
                try {
                    int num = stoi(id.substr(3));
                    if (num > maxSalaryId) maxSalaryId = num;
                } catch (...) {}
            }
        }
        Utility::setSalaryCount(maxSalaryId);

        int maxAttId = 0;
        for (const auto& att : attendanceList) {
            string id = att.getAttendanceId();
            if (id.length() >= 4) {
                try {
                    int num = stoi(id.substr(3));
                    if (num > maxAttId) maxAttId = num;
                } catch (...) {}
            }
        }
        Utility::setAttendanceCount(maxAttId);
    }

    // Save all data to files
    void saveAllData() {
        FileManager::saveToFile("employees.dat", employees);
        FileManager::saveToFile("attendance.dat", attendanceList);
        FileManager::saveToFile("leave.dat", leaveList);
        FileManager::saveToFile("salary.dat", salaryList);
        FileManager::saveToFile("department.dat", departments);
    }

    // Display welcome screen
    void displayWelcomeScreen() {
        Utility::clearScreen();
        Utility::printBox();
        Utility::printCentered("EMPLOYMENT ATTENDANCE MANAGEMENT SYSTEM");
        Utility::printCentered("(EAMS)");
        Utility::printBox();
        cout << endl;
        Utility::printLoading("Loading system");
        Utility::clearScreen();
    }

    // Display main menu
    void displayMainMenu() {
        Utility::printHeader("MAIN MENU");
        cout << "1. Admin Login" << endl;
        cout << "2. Employee Login" << endl;
        cout << "3. Exit" << endl;
        Utility::printLine();
    }

    // Display admin menu
    void displayAdminMenu() {
        Utility::printHeader("ADMIN MENU");
        cout << "1. Add Employee" << endl;
        cout << "2. Edit Employee" << endl;
        cout << "3. Delete Employee" << endl;
        cout << "4. Search Employee" << endl;
        cout << "5. Display All Employees" << endl;
        cout << "6. Employee Attendance" << endl;
        cout << "7. Leave Management" << endl;
        cout << "8. Salary Management" << endl;
        cout << "9. Department Management" << endl;
        cout << "10. Reports" << endl;
        cout << "11. Backup Data" << endl;
        cout << "12. Restore Data" << endl;
        cout << "13. Change Password" << endl;
        cout << "14. Logout" << endl;
        Utility::printLine();
    }

    // Display employee menu
    void displayEmployeeMenu() {
        Utility::printHeader("EMPLOYEE MENU");
        cout << "1. View Profile" << endl;
        cout << "2. Mark Attendance" << endl;
        cout << "3. View Attendance History" << endl;
        cout << "4. Apply Leave" << endl;
        cout << "5. View Leave Status" << endl;
        cout << "6. View Salary Slip" << endl;
        cout << "7. Change Password" << endl;
        cout << "8. Logout" << endl;
        Utility::printLine();
    }

    // Display department menu
    void displayDepartmentMenu() {
        Utility::printHeader("DEPARTMENT MANAGEMENT");
        cout << "1. Add Department" << endl;
        cout << "2. Edit Department" << endl;
        cout << "3. Delete Department" << endl;
        cout << "4. Search Department" << endl;
        cout << "5. Display All Departments" << endl;
        cout << "6. Back to Admin Menu" << endl;
        Utility::printLine();
    }

    // Display reports menu
    void displayReportsMenu() {
        Utility::printHeader("REPORTS");
        cout << "1. Daily Attendance Report" << endl;
        cout << "2. Monthly Attendance Report" << endl;
        cout << "3. Employee List Report" << endl;
        cout << "4. Salary Report" << endl;
        cout << "5. Leave Report" << endl;
        cout << "6. Department Report" << endl;
        cout << "7. Highest Attendance Employee" << endl;
        cout << "8. Lowest Attendance Employee" << endl;
        cout << "9. Top Overtime Employees" << endl;
        cout << "10. Absent Employees" << endl;
        cout << "11. Back to Admin Menu" << endl;
        Utility::printLine();
    }

    // Admin login
    bool adminLogin() {
        Utility::clearScreen();
        Utility::printHeader("ADMIN LOGIN");

        string username, password;
        cout << "Username: ";
        cin >> username;
        cout << "Password: ";
        password = Utility::getPasswordInput();

        if (admin.verifyLogin(username, password)) {
            cout << "\nLogin Successful!" << endl;
            Utility::pause();
            return true;
        } else {
            cout << "\nInvalid username or password!" << endl;
            Utility::pause();
            return false;
        }
    }

    // Employee login
    Employee* employeeLogin() {
        Utility::clearScreen();
        Utility::printHeader("EMPLOYEE LOGIN");

        string empId, password;
        cout << "Employee ID: ";
        cin >> empId;
        cout << "Password: ";
        password = Utility::getPasswordInput();

        for (auto& emp : employees) {
            if (emp.getEmployeeId() == empId && emp.getPassword() == password && emp.getStatus() == "Active") {
                cout << "\nLogin Successful!" << endl;
                Utility::pause();
                return &emp;
            }
        }

        cout << "\nInvalid employee ID or password!" << endl;
        Utility::pause();
        return nullptr;
    }

    // Add employee
    void addEmployee() {
        Utility::clearScreen();
        Utility::printHeader("ADD EMPLOYEE");

        Employee emp;
        cin >> emp;

        if (!emp.validate()) {
            cout << "\nInvalid employee data! Please check all fields." << endl;
            Utility::pause();
            return;
        }

        // Check for duplicate username
        for (const auto& e : employees) {
            if (e.getUsername() == emp.getUsername()) {
                cout << "\nUsername already exists!" << endl;
                Utility::pause();
                return;
            }
        }

        employees.push_back(emp);
        saveAllData();

        cout << "\nEmployee added successfully!" << endl;
        cout << "Employee ID: " << emp.getEmployeeId() << endl;
        Utility::pause();
    }

    // Edit employee
    void editEmployee() {
        Utility::clearScreen();
        Utility::printHeader("EDIT EMPLOYEE");

        string empId;
        cout << "Enter Employee ID to edit: ";
        cin >> empId;

        for (auto& emp : employees) {
            if (emp.getEmployeeId() == empId) {
                cout << "\nCurrent Details:" << endl;
                emp.displayDetails();

                cout << "\nEnter new details (leave blank to keep current):" << endl;

                string input;
                cout << "Full Name [" << emp.getFullName() << "]: ";
                Utility::clearInputBuffer();
                getline(cin, input);
                if (!input.empty()) emp.setFullName(input);

                cout << "Phone Number [" << emp.getPhoneNumber() << "]: ";
                getline(cin, input);
                if (!input.empty() && Utility::isValidPhone(input)) emp.setPhoneNumber(input);

                cout << "Email [" << emp.getEmail() << "]: ";
                getline(cin, input);
                if (!input.empty() && Utility::isValidEmail(input)) emp.setEmail(input);

                cout << "Address [" << emp.getAddress() << "]: ";
                getline(cin, input);
                if (!input.empty()) emp.setAddress(input);

                cout << "Department [" << emp.getDepartment() << "]: ";
                getline(cin, input);
                if (!input.empty()) emp.setDepartment(input);

                cout << "Position [" << emp.getPosition() << "]: ";
                getline(cin, input);
                if (!input.empty()) emp.setPosition(input);

                double salary;
                cout << "Basic Salary [" << emp.getBasicSalary() << "]: ";
                cin >> salary;
                if (salary > 0) emp.setBasicSalary(salary);

                cout << "Status [Active/Inactive] [" << emp.getStatus() << "]: ";
                cin >> input;
                if (input == "Active" || input == "Inactive") emp.setStatus(input);

                saveAllData();
                cout << "\nEmployee updated successfully!" << endl;
                Utility::pause();
                return;
            }
        }

        cout << "\nEmployee not found!" << endl;
        Utility::pause();
    }

    // Delete employee
    void deleteEmployee() {
        Utility::clearScreen();
        Utility::printHeader("DELETE EMPLOYEE");

        string empId;
        cout << "Enter Employee ID to delete: ";
        cin >> empId;

        for (auto it = employees.begin(); it != employees.end(); ++it) {
            if (it->getEmployeeId() == empId) {
                cout << "\nEmployee Details:" << endl;
                it->displayDetails();

                char confirm;
                cout << "\nAre you sure you want to delete this employee? (y/n): ";
                cin >> confirm;

                if (confirm == 'y' || confirm == 'Y') {
                    employees.erase(it);
                    saveAllData();
                    cout << "\nEmployee deleted successfully!" << endl;
                } else {
                    cout << "\nDeletion cancelled." << endl;
                }
                Utility::pause();
                return;
            }
        }

        cout << "\nEmployee not found!" << endl;
        Utility::pause();
    }

    // Search employee
    void searchEmployee() {
        Utility::clearScreen();
        Utility::printHeader("SEARCH EMPLOYEE");

        cout << "Search by:" << endl;
        cout << "1. Employee ID" << endl;
        cout << "2. Employee Name" << endl;
        cout << "3. Department" << endl;
        cout << "4. Phone Number" << endl;
        cout << "5. Position" << endl;
        cout << "Enter choice: ";

        int choice;
        cin >> choice;

        string searchTerm;
        cout << "Enter search term: ";
        Utility::clearInputBuffer();
        getline(cin, searchTerm);

        bool found = false;

        cout << "\nSearch Results:" << endl;
        cout << left << setw(12) << "Employee ID"
             << setw(25) << "Full Name"
             << setw(10) << "Gender"
             << setw(5) << "Age"
             << setw(15) << "Department"
             << setw(20) << "Position"
             << setw(12) << "Phone"
             << setw(10) << "Status" << endl;
        Utility::printLine();

        for (const auto& emp : employees) {
            bool match = false;
            switch (choice) {
                case 1: match = emp.getEmployeeId() == searchTerm; break;
                case 2: match = Utility::toLower(emp.getFullName()).find(Utility::toLower(searchTerm)) != string::npos; break;
                case 3: match = Utility::toLower(emp.getDepartment()).find(Utility::toLower(searchTerm)) != string::npos; break;
                case 4: match = emp.getPhoneNumber() == searchTerm; break;
                case 5: match = Utility::toLower(emp.getPosition()).find(Utility::toLower(searchTerm)) != string::npos; break;
            }

            if (match) {
                emp.display();
                found = true;
            }
        }

        if (!found) {
            cout << "No matching employees found." << endl;
        }

        Utility::pause();
    }

    // Display all employees
    void displayAllEmployees() {
        Utility::clearScreen();
        Utility::printHeader("ALL EMPLOYEES");

        if (employees.empty()) {
            cout << "No employees found." << endl;
            Utility::pause();
            return;
        }

        cout << left << setw(12) << "Employee ID"
             << setw(25) << "Full Name"
             << setw(10) << "Gender"
             << setw(5) << "Age"
             << setw(15) << "Department"
             << setw(20) << "Position"
             << setw(12) << "Phone"
             << setw(10) << "Status" << endl;
        Utility::printLine();

        for (const auto& emp : employees) {
            emp.display();
        }

        cout << "\nTotal Employees: " << employees.size() << endl;
        Utility::pause();
    }

    // Sort employees
    void sortEmployees() {
        Utility::clearScreen();
        Utility::printHeader("SORT EMPLOYEES");

        cout << "Sort by:" << endl;
        cout << "1. Employee ID" << endl;
        cout << "2. Name" << endl;
        cout << "3. Salary" << endl;
        cout << "4. Department" << endl;
        cout << "5. Joining Date" << endl;
        cout << "Enter choice: ";

        int choice;
        cin >> choice;

        cout << "Order:" << endl;
        cout << "1. Ascending" << endl;
        cout << "2. Descending" << endl;
        cout << "Enter choice: ";

        int order;
        cin >> order;

        bool ascending = (order == 1);

        switch (choice) {
            case 1:
                sort(employees.begin(), employees.end(),
                    [ascending](const Employee& a, const Employee& b) {
                        return ascending ? a.getEmployeeId() < b.getEmployeeId() : a.getEmployeeId() > b.getEmployeeId();
                    });
                break;
            case 2:
                sort(employees.begin(), employees.end(),
                    [ascending](const Employee& a, const Employee& b) {
                        return ascending ? a.getFullName() < b.getFullName() : a.getFullName() > b.getFullName();
                    });
                break;
            case 3:
                sort(employees.begin(), employees.end(),
                    [ascending](const Employee& a, const Employee& b) {
                        return ascending ? a.getBasicSalary() < b.getBasicSalary() : a.getBasicSalary() > b.getBasicSalary();
                    });
                break;
            case 4:
                sort(employees.begin(), employees.end(),
                    [ascending](const Employee& a, const Employee& b) {
                        return ascending ? a.getDepartment() < b.getDepartment() : a.getDepartment() > b.getDepartment();
                    });
                break;
            case 5:
                sort(employees.begin(), employees.end(),
                    [ascending](const Employee& a, const Employee& b) {
                        return ascending ? a.getJoiningDate() < b.getJoiningDate() : a.getJoiningDate() > b.getJoiningDate();
                    });
                break;
        }

        cout << "\nEmployees sorted successfully!" << endl;
        displayAllEmployees();
    }

    // Employee attendance management
    void employeeAttendanceManagement() {
        Utility::clearScreen();
        Utility::printHeader("EMPLOYEE ATTENDANCE");

        cout << "1. View Today's Attendance" << endl;
        cout << "2. View Attendance by Date" << endl;
        cout << "3. View Employee Attendance History" << endl;
        cout << "4. Back to Admin Menu" << endl;
        cout << "Enter choice: ";

        int choice;
        cin >> choice;

        switch (choice) {
            case 1:
                viewTodaysAttendance();
                break;
            case 2:
                viewAttendanceByDate();
                break;
            case 3:
                viewEmployeeAttendanceHistory();
                break;
            case 4:
                return;
        }
    }

    // View today's attendance
    void viewTodaysAttendance() {
        Utility::clearScreen();
        Utility::printHeader("TODAY'S ATTENDANCE");

        string today = Utility::getCurrentDate();

        cout << "Date: " << today << endl;
        cout << left << setw(15) << "Attendance ID"
             << setw(15) << "Employee ID"
             << setw(25) << "Employee Name"
             << setw(10) << "Check In"
             << setw(10) << "Check Out"
             << setw(12) << "Work Hours"
             << setw(10) << "Status" << endl;
        Utility::printLine();

        bool found = false;
        for (const auto& att : attendanceList) {
            if (att.getDate() == today) {
                att.display();
                found = true;
            }
        }

        if (!found) {
            cout << "No attendance records for today." << endl;
        }

        Utility::pause();
    }

    // View attendance by date
    void viewAttendanceByDate() {
        Utility::clearScreen();
        Utility::printHeader("ATTENDANCE BY DATE");

        string date;
        cout << "Enter date (YYYY-MM-DD): ";
        cin >> date;

        if (!Utility::isValidDate(date)) {
            cout << "Invalid date format!" << endl;
            Utility::pause();
            return;
        }

        cout << "\nDate: " << date << endl;
        cout << left << setw(15) << "Attendance ID"
             << setw(15) << "Employee ID"
             << setw(25) << "Employee Name"
             << setw(10) << "Check In"
             << setw(10) << "Check Out"
             << setw(12) << "Work Hours"
             << setw(10) << "Status" << endl;
        Utility::printLine();

        bool found = false;
        for (const auto& att : attendanceList) {
            if (att.getDate() == date) {
                att.display();
                found = true;
            }
        }

        if (!found) {
            cout << "No attendance records for this date." << endl;
        }

        Utility::pause();
    }

    // View employee attendance history
    void viewEmployeeAttendanceHistory() {
        Utility::clearScreen();
        Utility::printHeader("EMPLOYEE ATTENDANCE HISTORY");

        string empId;
        cout << "Enter Employee ID: ";
        cin >> empId;

        cout << left << setw(15) << "Attendance ID"
             << setw(15) << "Employee ID"
             << setw(25) << "Employee Name"
             << setw(12) << "Date"
             << setw(10) << "Check In"
             << setw(10) << "Check Out"
             << setw(12) << "Work Hours"
             << setw(10) << "Status" << endl;
        Utility::printLine();

        bool found = false;
        for (const auto& att : attendanceList) {
            if (att.getEmployeeId() == empId) {
                att.display();
                found = true;
            }
        }

        if (!found) {
            cout << "No attendance records found for this employee." << endl;
        }

        Utility::pause();
    }

    // Leave management
    void leaveManagement() {
        Utility::clearScreen();
        Utility::printHeader("LEAVE MANAGEMENT");

        cout << "1. View Pending Leave Requests" << endl;
        cout << "2. Approve Leave Request" << endl;
        cout << "3. Reject Leave Request" << endl;
        cout << "4. Delete Leave Request" << endl;
        cout << "5. View All Leave Requests" << endl;
        cout << "6. Back to Admin Menu" << endl;
        cout << "Enter choice: ";

        int choice;
        cin >> choice;

        switch (choice) {
            case 1:
                viewPendingLeaveRequests();
                break;
            case 2:
                approveLeaveRequest();
                break;
            case 3:
                rejectLeaveRequest();
                break;
            case 4:
                deleteLeaveRequest();
                break;
            case 5:
                viewAllLeaveRequests();
                break;
            case 6:
                return;
        }
    }

    // View pending leave requests
    void viewPendingLeaveRequests() {
        Utility::clearScreen();
        Utility::printHeader("PENDING LEAVE REQUESTS");

        cout << left << setw(12) << "Leave ID"
             << setw(15) << "Employee ID"
             << setw(25) << "Employee Name"
             << setw(15) << "Leave Type"
             << setw(12) << "Start Date"
             << setw(12) << "End Date"
             << setw(8) << "Days"
             << setw(20) << "Reason"
             << setw(12) << "Status" << endl;
        Utility::printLine();

        bool found = false;
        for (const auto& lv : leaveList) {
            if (lv.isPending()) {
                lv.display();
                found = true;
            }
        }

        if (!found) {
            cout << "No pending leave requests." << endl;
        }

        Utility::pause();
    }

    // Approve leave request
    void approveLeaveRequest() {
        Utility::clearScreen();
        Utility::printHeader("APPROVE LEAVE REQUEST");

        string leaveId;
        cout << "Enter Leave ID to approve: ";
        cin >> leaveId;

        for (auto& lv : leaveList) {
            if (lv.getLeaveId() == leaveId && lv.isPending()) {
                lv.setStatus("Approved");
                saveAllData();
                cout << "\nLeave request approved successfully!" << endl;
                Utility::pause();
                return;
            }
        }

        cout << "\nLeave request not found or already processed." << endl;
        Utility::pause();
    }

    // Reject leave request
    void rejectLeaveRequest() {
        Utility::clearScreen();
        Utility::printHeader("REJECT LEAVE REQUEST");

        string leaveId;
        cout << "Enter Leave ID to reject: ";
        cin >> leaveId;

        for (auto& lv : leaveList) {
            if (lv.getLeaveId() == leaveId && lv.isPending()) {
                lv.setStatus("Rejected");
                saveAllData();
                cout << "\nLeave request rejected successfully!" << endl;
                Utility::pause();
                return;
            }
        }

        cout << "\nLeave request not found or already processed." << endl;
        Utility::pause();
    }

    // Delete leave request
    void deleteLeaveRequest() {
        Utility::clearScreen();
        Utility::printHeader("DELETE LEAVE REQUEST");

        string leaveId;
        cout << "Enter Leave ID to delete: ";
        cin >> leaveId;

        for (auto it = leaveList.begin(); it != leaveList.end(); ++it) {
            if (it->getLeaveId() == leaveId) {
                char confirm;
                cout << "Are you sure you want to delete this leave request? (y/n): ";
                cin >> confirm;

                if (confirm == 'y' || confirm == 'Y') {
                    leaveList.erase(it);
                    saveAllData();
                    cout << "\nLeave request deleted successfully!" << endl;
                } else {
                    cout << "\nDeletion cancelled." << endl;
                }
                Utility::pause();
                return;
            }
        }

        cout << "\nLeave request not found." << endl;
        Utility::pause();
    }

    // View all leave requests
    void viewAllLeaveRequests() {
        Utility::clearScreen();
        Utility::printHeader("ALL LEAVE REQUESTS");

        if (leaveList.empty()) {
            cout << "No leave requests found." << endl;
            Utility::pause();
            return;
        }

        cout << left << setw(12) << "Leave ID"
             << setw(15) << "Employee ID"
             << setw(25) << "Employee Name"
             << setw(15) << "Leave Type"
             << setw(12) << "Start Date"
             << setw(12) << "End Date"
             << setw(8) << "Days"
             << setw(20) << "Reason"
             << setw(12) << "Status" << endl;
        Utility::printLine();

        for (const auto& lv : leaveList) {
            lv.display();
        }

        cout << "\nTotal Leave Requests: " << leaveList.size() << endl;
        Utility::pause();
    }

    // Salary management
    void salaryManagement() {
        Utility::clearScreen();
        Utility::printHeader("SALARY MANAGEMENT");

        cout << "1. Generate Salary" << endl;
        cout << "2. View Salary Records" << endl;
        cout << "3. Back to Admin Menu" << endl;
        cout << "Enter choice: ";

        int choice;
        cin >> choice;

        switch (choice) {
            case 1:
                generateSalary();
                break;
            case 2:
                viewSalaryRecords();
                break;
            case 3:
                return;
        }
    }

    // Generate salary
    void generateSalary() {
        Utility::clearScreen();
        Utility::printHeader("GENERATE SALARY");

        string month;
        cout << "Enter month (YYYY-MM): ";
        cin >> month;

        for (const auto& emp : employees) {
            if (emp.getStatus() != "Active") continue;

            // Calculate working days (assuming 22 working days per month)
            int workingDays = 22;

            // Calculate present days
            int presentDays = 0;
            double overtimeHours = 0.0;

            for (const auto& att : attendanceList) {
                if (att.getEmployeeId() == emp.getEmployeeId() && att.getDate().length() >= 7 && att.getDate().substr(0, 7) == month) {
                    if (att.getStatus() == "Present") {
                        presentDays++;
                        overtimeHours += att.getOvertimeHours();
                    }
                }
            }

            // Calculate leave days
            int leaveDays = 0;
            for (const auto& lv : leaveList) {
                if (lv.getEmployeeId() == emp.getEmployeeId() && lv.isApproved()) {
                    if (lv.getStartDate().length() >= 7 && lv.getStartDate().substr(0, 7) == month) {
                        leaveDays += lv.getTotalDays();
                    }
                }
            }

            // Calculate absent days
            int absentDays = workingDays - presentDays - leaveDays;
            if (absentDays < 0) absentDays = 0;

            // Create salary record
            Salary salary;
            salary.setSalaryId(Utility::generateSalaryID());
            salary.setEmployeeId(emp.getEmployeeId());
            salary.setEmployeeName(emp.getFullName());
            salary.setDepartment(emp.getDepartment());
            salary.setMonth(month);
            salary.setWorkingDays(workingDays);
            salary.setPresentDays(presentDays);
            salary.setAbsentDays(absentDays);
            salary.setLeaveDays(leaveDays);
            salary.setOvertimeHours(overtimeHours);
            salary.setBasicSalary(emp.getBasicSalary());
            salary.setBonus(0.0); // Can be added later
            salary.calculateSalary();

            // Check if salary already exists for this month
            bool exists = false;
            for (auto& sal : salaryList) {
                if (sal.getEmployeeId() == emp.getEmployeeId() && sal.getMonth() == month) {
                    sal = salary;
                    exists = true;
                    break;
                }
            }

            if (!exists) {
                salaryList.push_back(salary);
            }
        }

        saveAllData();
        cout << "\nSalary generated successfully for month: " << month << endl;
        Utility::pause();
    }

    // View salary records
    void viewSalaryRecords() {
        Utility::clearScreen();
        Utility::printHeader("SALARY RECORDS");

        if (salaryList.empty()) {
            cout << "No salary records found." << endl;
            Utility::pause();
            return;
        }

        cout << left << setw(12) << "Salary ID"
             << setw(15) << "Employee ID"
             << setw(25) << "Employee Name"
             << setw(15) << "Department"
             << setw(10) << "Month"
             << setw(8) << "Present"
             << setw(8) << "Leave"
             << setw(10) << "Overtime"
             << setw(12) << "Gross"
             << setw(12) << "Net Salary" << endl;
        Utility::printLine();

        for (const auto& sal : salaryList) {
            sal.display();
        }

        cout << "\nTotal Salary Records: " << salaryList.size() << endl;
        Utility::pause();
    }

    // Department management
    void departmentManagement() {
        while (true) {
            Utility::clearScreen();
            displayDepartmentMenu();

            int choice;
            cout << "Enter choice: ";
            cin >> choice;

            switch (choice) {
                case 1:
                    addDepartment();
                    break;
                case 2:
                    editDepartment();
                    break;
                case 3:
                    deleteDepartment();
                    break;
                case 4:
                    searchDepartment();
                    break;
                case 5:
                    displayAllDepartments();
                    break;
                case 6:
                    return;
                default:
                    cout << "Invalid choice!" << endl;
                    Utility::pause();
            }
        }
    }

    // Add department
    void addDepartment() {
        Utility::clearScreen();
        Utility::printHeader("ADD DEPARTMENT");

        Department dept;
        string deptId = "DEPT" + to_string(departments.size() + 1);
        dept.setDepartmentId(deptId);

        string name, description, head;

        cout << "Department ID: " << deptId << endl;
        cout << "Department Name: ";
        Utility::clearInputBuffer();
        getline(cin, name);
        dept.setDepartmentName(name);

        cout << "Description: ";
        getline(cin, description);
        dept.setDescription(description);

        cout << "Head of Department: ";
        getline(cin, head);
        dept.setHeadOfDepartment(head);

        departments.push_back(dept);
        saveAllData();

        cout << "\nDepartment added successfully!" << endl;
        Utility::pause();
    }

    // Edit department
    void editDepartment() {
        Utility::clearScreen();
        Utility::printHeader("EDIT DEPARTMENT");

        string deptId;
        cout << "Enter Department ID to edit: ";
        cin >> deptId;

        for (auto& dept : departments) {
            if (dept.getDepartmentId() == deptId) {
                cout << "\nCurrent Details:" << endl;
                cout << "Department ID: " << dept.getDepartmentId() << endl;
                cout << "Department Name: " << dept.getDepartmentName() << endl;
                cout << "Description: " << dept.getDescription() << endl;
                cout << "Head of Department: " << dept.getHeadOfDepartment() << endl;

                string input;
                cout << "\nEnter new details (leave blank to keep current):" << endl;

                cout << "Department Name [" << dept.getDepartmentName() << "]: ";
                Utility::clearInputBuffer();
                getline(cin, input);
                if (!input.empty()) dept.setDepartmentName(input);

                cout << "Description [" << dept.getDescription() << "]: ";
                getline(cin, input);
                if (!input.empty()) dept.setDescription(input);

                cout << "Head of Department [" << dept.getHeadOfDepartment() << "]: ";
                getline(cin, input);
                if (!input.empty()) dept.setHeadOfDepartment(input);

                saveAllData();
                cout << "\nDepartment updated successfully!" << endl;
                Utility::pause();
                return;
            }
        }

        cout << "\nDepartment not found!" << endl;
        Utility::pause();
    }

    // Delete department
    void deleteDepartment() {
        Utility::clearScreen();
        Utility::printHeader("DELETE DEPARTMENT");

        string deptId;
        cout << "Enter Department ID to delete: ";
        cin >> deptId;

        for (auto it = departments.begin(); it != departments.end(); ++it) {
            if (it->getDepartmentId() == deptId) {
                cout << "\nDepartment Details:" << endl;
                cout << "Department ID: " << it->getDepartmentId() << endl;
                cout << "Department Name: " << it->getDepartmentName() << endl;

                char confirm;
                cout << "\nAre you sure you want to delete this department? (y/n): ";
                cin >> confirm;

                if (confirm == 'y' || confirm == 'Y') {
                    departments.erase(it);
                    saveAllData();
                    cout << "\nDepartment deleted successfully!" << endl;
                } else {
                    cout << "\nDeletion cancelled." << endl;
                }
                Utility::pause();
                return;
            }
        }

        cout << "\nDepartment not found!" << endl;
        Utility::pause();
    }

    // Search department
    void searchDepartment() {
        Utility::clearScreen();
        Utility::printHeader("SEARCH DEPARTMENT");

        string searchTerm;
        cout << "Enter department name or ID: ";
        Utility::clearInputBuffer();
        getline(cin, searchTerm);

        bool found = false;

        cout << left << setw(15) << "Department ID"
             << setw(25) << "Department Name"
             << setw(30) << "Description"
             << setw(25) << "Head of Department" << endl;
        Utility::printLine();

        for (const auto& dept : departments) {
            if (Utility::toLower(dept.getDepartmentName()).find(Utility::toLower(searchTerm)) != string::npos ||
                dept.getDepartmentId() == searchTerm) {
                dept.display();
                found = true;
            }
        }

        if (!found) {
            cout << "No matching departments found." << endl;
        }

        Utility::pause();
    }

    // Display all departments
    void displayAllDepartments() {
        Utility::clearScreen();
        Utility::printHeader("ALL DEPARTMENTS");

        if (departments.empty()) {
            cout << "No departments found." << endl;
            Utility::pause();
            return;
        }

        cout << left << setw(15) << "Department ID"
             << setw(25) << "Department Name"
             << setw(30) << "Description"
             << setw(25) << "Head of Department" << endl;
        Utility::printLine();

        for (const auto& dept : departments) {
            dept.display();
        }

        cout << "\nTotal Departments: " << departments.size() << endl;
        Utility::pause();
    }

    // Reports
    void reports() {
        while (true) {
            Utility::clearScreen();
            displayReportsMenu();

            int choice;
            cout << "Enter choice: ";
            cin >> choice;

            switch (choice) {
                case 1: {
                    string date = Utility::getCurrentDate();
                    Report::generateDailyAttendanceReport(attendanceList, date);
                    Utility::pause();
                    break;
                }
                case 2: {
                    string month = Utility::getCurrentMonth();
                    Report::generateMonthlyAttendanceReport(attendanceList, month);
                    Utility::pause();
                    break;
                }
                case 3:
                    Report::generateEmployeeListReport(employees);
                    Utility::pause();
                    break;
                case 4: {
                    string month = Utility::getCurrentMonth();
                    Report::generateSalaryReport(salaryList, month);
                    Utility::pause();
                    break;
                }
                case 5:
                    Report::generateLeaveReport(leaveList);
                    Utility::pause();
                    break;
                case 6:
                    Report::generateDepartmentReport(employees, departments);
                    Utility::pause();
                    break;
                case 7:
                    Report::findHighestAttendance(attendanceList, employees);
                    Utility::pause();
                    break;
                case 8:
                    Report::findLowestAttendance(attendanceList, employees);
                    Utility::pause();
                    break;
                case 9:
                    Report::findTopOvertimeEmployees(attendanceList, employees);
                    Utility::pause();
                    break;
                case 10: {
                    string date = Utility::getCurrentDate();
                    Report::findAbsentEmployees(attendanceList, employees, date);
                    Utility::pause();
                    break;
                }
                case 11:
                    return;
                default:
                    cout << "Invalid choice!" << endl;
                    Utility::pause();
            }
        }
    }

    // Backup data
    void backupData() {
        Utility::clearScreen();
        Utility::printHeader("BACKUP DATA");

        string backupFile = "backup_" + Utility::getCurrentDate() + ".dat";

        if (FileManager::createBackup("employees.dat", backupFile + "_emp") &&
            FileManager::createBackup("attendance.dat", backupFile + "_att") &&
            FileManager::createBackup("leave.dat", backupFile + "_lv") &&
            FileManager::createBackup("salary.dat", backupFile + "_sal") &&
            FileManager::createBackup("department.dat", backupFile + "_dept")) {
            cout << "Data backed up successfully!" << endl;
        } else {
            cout << "Backup failed!" << endl;
        }

        Utility::pause();
    }

    // Restore data
    void restoreData() {
        Utility::clearScreen();
        Utility::printHeader("RESTORE DATA");

        string backupFile;
        cout << "Enter backup file name (without extension): ";
        cin >> backupFile;

        if (FileManager::restoreBackup(backupFile + "_emp", "employees.dat") &&
            FileManager::restoreBackup(backupFile + "_att", "attendance.dat") &&
            FileManager::restoreBackup(backupFile + "_lv", "leave.dat") &&
            FileManager::restoreBackup(backupFile + "_sal", "salary.dat") &&
            FileManager::restoreBackup(backupFile + "_dept", "department.dat")) {
            loadAllData();
            cout << "Data restored successfully!" << endl;
        } else {
            cout << "Restore failed!" << endl;
        }

        Utility::pause();
    }

    // Change admin password
    void changeAdminPassword() {
        Utility::clearScreen();
        Utility::printHeader("CHANGE PASSWORD");

        string oldPassword, newPassword;
        cout << "Enter current password: ";
        oldPassword = Utility::getPasswordInput();

        cout << "Enter new password: ";
        newPassword = Utility::getPasswordInput();

        if (admin.changePassword(oldPassword, newPassword)) {
            cout << "\nPassword changed successfully!" << endl;
        } else {
            cout << "\nPassword change failed!" << endl;
        }

        Utility::pause();
    }

    // Employee view profile
    void viewProfile(Employee* emp) {
        Utility::clearScreen();
        Utility::printHeader("MY PROFILE");
        emp->displayDetails();
        Utility::pause();
    }

    // Employee mark attendance
    void markAttendance(Employee* emp) {
        Utility::clearScreen();
        Utility::printHeader("MARK ATTENDANCE");

        string today = Utility::getCurrentDate();
        string currentTime = Utility::getCurrentTime();

        // Check if already checked in today
        for (auto& att : attendanceList) {
            if (att.getEmployeeId() == emp->getEmployeeId() && att.getDate() == today) {
                if (att.isCheckedIn() && !att.isCheckedOut()) {
                    // Check out
                    att.setCheckOutTime(currentTime);
                    double workHours = Utility::calculateWorkingHours(att.getCheckInTime(), att.getCheckOutTime());
                    att.setWorkingHours(workHours);

                    // Calculate overtime (more than 8 hours)
                    if (workHours > 8.0) {
                        att.setOvertimeHours(workHours - 8.0);
                    }

                    att.setStatus("Present");
                    saveAllData();

                    cout << "Checked out successfully!" << endl;
                    cout << "Check In Time: " << att.getCheckInTime() << endl;
                    cout << "Check Out Time: " << att.getCheckOutTime() << endl;
                    cout << "Working Hours: " << fixed << setprecision(2) << workHours << endl;
                    if (att.getOvertimeHours() > 0) {
                        cout << "Overtime Hours: " << fixed << setprecision(2) << att.getOvertimeHours() << endl;
                    }
                    Utility::pause();
                    return;
                } else if (att.isCheckedOut()) {
                    cout << "You have already checked out today!" << endl;
                    Utility::pause();
                    return;
                }
            }
        }

        // Check in
        Attendance att;
        att.setAttendanceId(Utility::generateAttendanceID());
        att.setEmployeeId(emp->getEmployeeId());
        att.setEmployeeName(emp->getFullName());
        att.setDate(today);
        att.setCheckInTime(currentTime);
        att.setStatus("Present");

        attendanceList.push_back(att);
        saveAllData();

        cout << "Checked in successfully!" << endl;
        cout << "Check In Time: " << currentTime << endl;
        Utility::pause();
    }

    // Employee view attendance history
    void viewAttendanceHistory(Employee* emp) {
        Utility::clearScreen();
        Utility::printHeader("MY ATTENDANCE HISTORY");

        cout << left << setw(15) << "Attendance ID"
             << setw(15) << "Employee ID"
             << setw(25) << "Employee Name"
             << setw(12) << "Date"
             << setw(10) << "Check In"
             << setw(10) << "Check Out"
             << setw(12) << "Work Hours"
             << setw(10) << "Status" << endl;
        Utility::printLine();

        bool found = false;
        for (const auto& att : attendanceList) {
            if (att.getEmployeeId() == emp->getEmployeeId()) {
                att.display();
                found = true;
            }
        }

        if (!found) {
            cout << "No attendance records found." << endl;
        }

        Utility::pause();
    }

    // Employee apply leave
    void applyLeave(Employee* emp) {
        Utility::clearScreen();
        Utility::printHeader("APPLY LEAVE");

        Leave leave;
        leave.setLeaveId(Utility::generateLeaveID());
        leave.setEmployeeId(emp->getEmployeeId());
        leave.setEmployeeName(emp->getFullName());

        cout << "Leave Types:" << endl;
        cout << "1. Sick Leave" << endl;
        cout << "2. Casual Leave" << endl;
        cout << "3. Annual Leave" << endl;
        cout << "4. Emergency Leave" << endl;
        cout << "Enter choice: ";

        int choice;
        cin >> choice;

        switch (choice) {
            case 1: leave.setLeaveType("Sick Leave"); break;
            case 2: leave.setLeaveType("Casual Leave"); break;
            case 3: leave.setLeaveType("Annual Leave"); break;
            case 4: leave.setLeaveType("Emergency Leave"); break;
            default:
                cout << "Invalid choice!" << endl;
                Utility::pause();
                return;
        }

        string startDate, endDate;
        cout << "Start Date (YYYY-MM-DD): ";
        cin >> startDate;
        cout << "End Date (YYYY-MM-DD): ";
        cin >> endDate;

        if (!Utility::isValidDate(startDate) || !Utility::isValidDate(endDate)) {
            cout << "Invalid date format!" << endl;
            Utility::pause();
            return;
        }

        leave.setStartDate(startDate);
        leave.setEndDate(endDate);
        leave.setTotalDays(Utility::calculateDays(startDate, endDate) + 1);

        cout << "Reason: ";
        Utility::clearInputBuffer();
        string reason;
        getline(cin, reason);
        leave.setReason(reason);

        leave.setStatus("Pending");

        leaveList.push_back(leave);
        saveAllData();

        cout << "\nLeave application submitted successfully!" << endl;
        cout << "Leave ID: " << leave.getLeaveId() << endl;
        Utility::pause();
    }

    // Employee view leave status
    void viewLeaveStatus(Employee* emp) {
        Utility::clearScreen();
        Utility::printHeader("MY LEAVE STATUS");

        cout << left << setw(12) << "Leave ID"
             << setw(15) << "Leave Type"
             << setw(12) << "Start Date"
             << setw(12) << "End Date"
             << setw(8) << "Days"
             << setw(20) << "Reason"
             << setw(12) << "Status" << endl;
        Utility::printLine();

        bool found = false;
        for (const auto& lv : leaveList) {
            if (lv.getEmployeeId() == emp->getEmployeeId()) {
                cout << left << setw(12) << lv.getLeaveId()
                     << setw(15) << lv.getLeaveType()
                     << setw(12) << lv.getStartDate()
                     << setw(12) << lv.getEndDate()
                     << setw(8) << lv.getTotalDays()
                     << setw(20) << (lv.getReason().length() > 20 ? lv.getReason().substr(0, 20) : lv.getReason())
                     << setw(12) << lv.getStatus() << endl;
                found = true;
            }
        }

        if (!found) {
            cout << "No leave applications found." << endl;
        }

        Utility::pause();
    }

    // Employee view salary slip
    void viewSalarySlip(Employee* emp) {
        Utility::clearScreen();
        Utility::printHeader("MY SALARY SLIP");

        string month = Utility::getCurrentMonth();
        bool found = false;

        for (const auto& sal : salaryList) {
            if (sal.getEmployeeId() == emp->getEmployeeId() && sal.getMonth() == month) {
                sal.displaySalarySlip();
                found = true;
                break;
            }
        }

        if (!found) {
            cout << "No salary slip found for current month." << endl;
        }

        Utility::pause();
    }

    // Employee change password
    void changeEmployeePassword(Employee* emp) {
        Utility::clearScreen();
        Utility::printHeader("CHANGE PASSWORD");

        string oldPassword, newPassword;
        cout << "Enter current password: ";
        oldPassword = Utility::getPasswordInput();

        if (emp->getPassword() != oldPassword) {
            cout << "\nCurrent password is incorrect!" << endl;
            Utility::pause();
            return;
        }

        cout << "Enter new password: ";
        newPassword = Utility::getPasswordInput();

        if (!Utility::isValidPassword(newPassword)) {
            cout << "\nPassword must be at least 8 characters!" << endl;
            Utility::pause();
            return;
        }

        emp->setPassword(newPassword);
        saveAllData();

        cout << "\nPassword changed successfully!" << endl;
        Utility::pause();
    }

    // Admin menu handler
    void adminMenuHandler() {
        while (true) {
            Utility::clearScreen();
            displayAdminMenu();

            int choice;
            cout << "Enter choice: ";
            cin >> choice;

            switch (choice) {
                case 1:
                    addEmployee();
                    break;
                case 2:
                    editEmployee();
                    break;
                case 3:
                    deleteEmployee();
                    break;
                case 4:
                    searchEmployee();
                    break;
                case 5:
                    displayAllEmployees();
                    break;
                case 6:
                    employeeAttendanceManagement();
                    break;
                case 7:
                    leaveManagement();
                    break;
                case 8:
                    salaryManagement();
                    break;
                case 9:
                    departmentManagement();
                    break;
                case 10:
                    reports();
                    break;
                case 11:
                    backupData();
                    break;
                case 12:
                    restoreData();
                    break;
                case 13:
                    changeAdminPassword();
                    break;
                case 14:
                    return;
                default:
                    cout << "Invalid choice!" << endl;
                    Utility::pause();
            }
        }
    }

    // Employee menu handler
    void employeeMenuHandler(Employee* emp) {
        while (true) {
            Utility::clearScreen();
            displayEmployeeMenu();

            int choice;
            cout << "Enter choice: ";
            cin >> choice;

            switch (choice) {
                case 1:
                    viewProfile(emp);
                    break;
                case 2:
                    markAttendance(emp);
                    break;
                case 3:
                    viewAttendanceHistory(emp);
                    break;
                case 4:
                    applyLeave(emp);
                    break;
                case 5:
                    viewLeaveStatus(emp);
                    break;
                case 6:
                    viewSalarySlip(emp);
                    break;
                case 7:
                    changeEmployeePassword(emp);
                    break;
                case 8:
                    return;
                default:
                    cout << "Invalid choice!" << endl;
                    Utility::pause();
            }
        }
    }

public:
    // Constructor
    Menu() {
        loadAllData();
    }

    // Destructor
    ~Menu() {
        saveAllData();
    }

    // Run the main menu
    void run() {
        displayWelcomeScreen();

        while (true) {
            Utility::clearScreen();
            displayMainMenu();

            int choice;
            cout << "Enter choice: ";
            cin >> choice;

            switch (choice) {
                case 1:
                    if (adminLogin()) {
                        adminMenuHandler();
                    }
                    break;
                case 2: {
                    Employee* emp = employeeLogin();
                    if (emp != nullptr) {
                        employeeMenuHandler(emp);
                    }
                    break;
                }
                case 3:
                    Utility::clearScreen();
                    Utility::printHeader("THANK YOU");
                    Utility::printCentered("Goodbye!");
                    Utility::printBox();
                    return;
                default:
                    cout << "Invalid choice!" << endl;
                    Utility::pause();
            }
        }
    }
};

// ==================== MAIN FUNCTION ====================
int main() {
    Menu menu;
    menu.run();

    return 0;
}
