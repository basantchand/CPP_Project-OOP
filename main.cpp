/*
 * Student Record Management System
 * A C++ Console-Based Academic Records Application
 *
 * Implements the design shown in the project proposal:
 *   - Student class          (attributes + grade computation)
 *   - StudentManagementSystem class (enroll/update/delete/search/report + persistence)
 *   - Data persisted to students.txt (pipe-delimited flat file)
 *
 * Build:  g++ -std=c++17 -O2 -o srms srms.cpp
 * Run:    ./srms
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <limits>

const int NUM_SUBJECTS = 5;
const std::string SUBJECT_NAMES[NUM_SUBJECTS] = {
    "Mathematics", "Science", "English", "Social Studies", "Computer Science"};
const std::string DATA_FILE = "students.txt";

// ----------------------------------------------------------------------
// Student
// ----------------------------------------------------------------------
class Student
{
private:
    int rollNumber;
    std::string name;
    int age;
    std::string department;
    double marks[NUM_SUBJECTS];

public:
    Student()
        : rollNumber(0), name(""), age(0), department(""), marks{0, 0, 0, 0, 0} {}

    Student(int rollNumber, const std::string &name, int age,
            const std::string &department, const double marksIn[NUM_SUBJECTS])
        : rollNumber(rollNumber), name(name), age(age), department(department)
    {
        for (int i = 0; i < NUM_SUBJECTS; ++i)
            marks[i] = marksIn[i];
    }

    // ---- Getters ----
    int getRollNumber() const { return rollNumber; }
    std::string getName() const { return name; }
    int getAge() const { return age; }
    std::string getDepartment() const { return department; }
    double getMark(int index) const { return marks[index]; }

    // ---- Computed values ----
    double getTotal() const
    {
        double total = 0.0;
        for (int i = 0; i < NUM_SUBJECTS; ++i)
            total += marks[i];
        return total;
    }

    double getAverage() const
    {
        return getTotal() / NUM_SUBJECTS;
    }

    double getPercentage() const
    {
        return getAverage(); // marks are already out of 100 each
    }

    std::string getGrade() const
    {
        double pct = getPercentage();
        if (pct >= 90)
            return "A+";
        if (pct >= 80)
            return "A";
        if (pct >= 70)
            return "B+";
        if (pct >= 60)
            return "B";
        if (pct >= 50)
            return "C";
        if (pct >= 40)
            return "D";
        return "F";
    }

     bool isPassing() const {
        for (int i = 0; i < NUM_SUBJECTS; ++i)
            if (marks[i] < 40.0) return false; // must pass every subject
        return true;
    }
 
    // ---- Persistence ----
    // rollNumber|name|age|department|m0|m1|m2|m3|m4
    std::string toFileLine() const {
        std::ostringstream oss;
        oss << rollNumber << "|" << name << "|" << age << "|" << department;
        for (int i = 0; i < NUM_SUBJECTS; ++i) oss << "|" << marks[i];
        return oss.str();
    }
 
    static bool fromFileLine(const std::string& line, Student& out) {
        std::vector<std::string> tokens;
        std::stringstream ss(line);
        std::string token;
        while (std::getline(ss, token, '|')) tokens.push_back(token);
        if (tokens.size() != static_cast<size_t>(4 + NUM_SUBJECTS)) return false;
 
        try {
            int roll = std::stoi(tokens[0]);
            std::string nm = tokens[1];
            int ag = std::stoi(tokens[2]);
            std::string dept = tokens[3];
            double m[NUM_SUBJECTS];
            for (int i = 0; i < NUM_SUBJECTS; ++i) m[i] = std::stod(tokens[4 + i]);
            out = Student(roll, nm, ag, dept, m);
            return true;
        } catch (...) {
            return false;
        }
    }
 
    // ---- Display ----
    void printReportCard() const {
        std::cout << "\n---------------------------------------------\n";
        std::cout << "               STUDENT REPORT CARD\n";
        std::cout << "---------------------------------------------\n";
        std::cout << "Roll Number : " << rollNumber << "\n";
        std::cout << "Name        : " << name << "\n";
        std::cout << "Age         : " << age << "\n";
        std::cout << "Department  : " << department << "\n";
        std::cout << "-----------------------------------------------\n";
        std::cout << std::left << std::setw(20) << "Subject" << "Marks\n";
        std::cout << "-----------------------------------------------\n";
        for (int i = 0; i < NUM_SUBJECTS; ++i) {
            std::cout << std::left << std::setw(20) << SUBJECT_NAMES[i]
                       << std::fixed << std::setprecision(2) << marks[i] << "\n";
        }
        std::cout << "-----------------------------------------------\n";
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Total       : " << getTotal() << " / " << (NUM_SUBJECTS * 100) << "\n";
        std::cout << "Average     : " << getAverage() << "\n";
        std::cout << "Percentage  : " << getPercentage() << "%\n";
        std::cout << "Grade       : " << getGrade() << "\n";
        std::cout << "Status      : " << (isPassing() ? "PASS" : "FAIL") << "\n";
        std::cout << "-----------------------------------------------\n";
    }
 
    void printSummaryRow() const {
        std::cout << std::left << std::setw(6) << rollNumber
                   << std::setw(20) << name
                   << std::setw(16) << department
                   << std::fixed << std::setprecision(2) << std::setw(10) << getTotal()
                   << std::setw(10) << getAverage()
                   << std::setw(6) << getGrade()
                   << (isPassing() ? "PASS" : "FAIL") << "\n";
    }
};
 
// ----------------------------------------------------------------------
// StudentManagementSystem
// ----------------------------------------------------------------------
class StudentManagementSystem {
private:
    std::vector<Student> students;
    int nextRollNumber;
 
    // Locate a student by roll number; returns pointer or nullptr
    Student* findByRollNumber(int roll) {
        for (auto& s : students)
            if (s.getRollNumber() == roll) return &s;
        return nullptr;
    }
 
    static void pause() {
        std::cout << "\nPress Enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
    }
 
    static int readInt(const std::string& prompt) {
        int value;
        while (true) {
            std::cout << prompt;
            std::cin >> value;
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid input. Please enter a number.\n";
                continue;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
    }
 
    static double readMark(const std::string& subject) {
        double value;
        while (true) {
            std::cout << "  " << subject << " (0-100): ";
            std::cin >> value;
            if (std::cin.fail() || value < 0.0 || value > 100.0) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "  Invalid mark. Enter a value between 0 and 100.\n";
                continue;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
    }
 
    static std::string readLine(const std::string& prompt) {
        std::string line;
        std::cout << prompt;
        std::getline(std::cin, line);
        return line;
    }
 
public:
    StudentManagementSystem() : nextRollNumber(1) {
        loadFromFile();
    }

     // ---- Persistence ----
    void loadFromFile() {
        std::ifstream file(DATA_FILE);
        if (!file.is_open()) return; // no existing file yet — start fresh
 
        students.clear();
        std::string line;
        int maxRoll = 0;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            Student s;
            if (Student::fromFileLine(line, s)) {
                students.push_back(s);
                maxRoll = std::max(maxRoll, s.getRollNumber());
            }
        }
        file.close();
        nextRollNumber = maxRoll + 1;
    }
 
    void saveToFile() const {
        std::ofstream file(DATA_FILE, std::ios::trunc);
        if (!file.is_open()) {
            std::cout << "Error: could not open " << DATA_FILE << " for writing.\n";
            return;
        }
        for (const auto& s : students) file << s.toFileLine() << "\n";
        file.close();
    }
 
    // ---- Core operations ----
    void enrollStudent() {
        std::cout << "\n--- Enroll New Student ---\n";
        std::string name = readLine("Name: ");
        int age = readInt("Age: ");
        std::string department = readLine("Department: ");
 
        double marks[NUM_SUBJECTS];
        std::cout << "Enter marks for each subject:\n";
        for (int i = 0; i < NUM_SUBJECTS; ++i)
            marks[i] = readMark(SUBJECT_NAMES[i]);
 
        Student s(nextRollNumber, name, age, department, marks);
        students.push_back(s);
        std::cout << "\nStudent enrolled successfully with Roll Number "
                   << nextRollNumber << ".\n";
        nextRollNumber++;
        saveToFile();
    }
 
    void updateStudent() {
        std::cout << "\n--- Update Student Record ---\n";
        int roll = readInt("Enter Roll Number to update: ");
        Student* s = findByRollNumber(roll);
        if (!s) {
            std::cout << "No student found with Roll Number " << roll << ".\n";
            return;
        }
 
        std::cout << "Leave a field blank to keep its current value.\n";
        std::string name = readLine("New Name [" + s->getName() + "]: ");
        std::string department = readLine("New Department [" + s->getDepartment() + "]: ");
 
        double marks[NUM_SUBJECTS];
        for (int i = 0; i < NUM_SUBJECTS; ++i) marks[i] = s->getMark(i);
 
        std::cout << "Update marks? (y/n): ";
        std::string choice;
        std::getline(std::cin, choice);
        if (!choice.empty() && (choice[0] == 'y' || choice[0] == 'Y')) {
            for (int i = 0; i < NUM_SUBJECTS; ++i)
                marks[i] = readMark(SUBJECT_NAMES[i]);
        }
 
        int age = s->getAge();
        *s = Student(roll,
                      name.empty() ? s->getName() : name,
                      age,
                      department.empty() ? s->getDepartment() : department,
                      marks);
        std::cout << "Record updated successfully.\n";
        saveToFile();
    }
 
    void deleteStudent() {
        std::cout << "\n--- Delete Student Record ---\n";
        int roll = readInt("Enter Roll Number to delete: ");
        auto it = std::remove_if(students.begin(), students.end(),
            [roll](const Student& s) { return s.getRollNumber() == roll; });
 
        if (it == students.end()) {
            std::cout << "No student found with Roll Number " << roll << ".\n";
            return;
        }
        students.erase(it, students.end());
        std::cout << "Student record deleted successfully.\n";
        saveToFile();
    }
 
    void searchStudent() {
        std::cout << "\n--- Search Student ---\n";
        std::cout << "1. Search by Roll Number\n2. Search by Name\n";
        int choice = readInt("Choose an option: ");
 
        if (choice == 1) {
            int roll = readInt("Enter Roll Number: ");
            Student* s = findByRollNumber(roll);
            if (s) s->printReportCard();
            else std::cout << "No student found with Roll Number " << roll << ".\n";
        } else if (choice == 2) {
            std::string name = readLine("Enter Name (or part of it): ");
            std::string lowerName = name;
            std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
            bool found = false;
            for (const auto& s : students) {
                std::string sn = s.getName();
                std::transform(sn.begin(), sn.end(), sn.begin(), ::tolower);
                if (sn.find(lowerName) != std::string::npos) {
                    s.printReportCard();
                    found = true;
                }
            }
            if (!found) std::cout << "No matching student found.\n";
        } else {
            std::cout << "Invalid option.\n";
        }
    }
 
    void listAll() const {
        std::cout << "\n--- All Enrolled Students ---\n";
        if (students.empty()) {
            std::cout << "No students enrolled yet.\n";
            return;
        }
        std::cout << std::left << std::setw(6) << "Roll" << std::setw(20) << "Name"
                   << std::setw(16) << "Department" << std::setw(10) << "Total"
                   << std::setw(10) << "Average" << std::setw(6) << "Grade"
                   << "Status\n";
        std::cout << std::string(78, '-') << "\n";
        for (const auto& s : students) s.printSummaryRow();
    }
 
    void generateClassReport() const {
        std::cout << "\n===================== CLASS REPORT =====================\n";
        if (students.empty()) {
            std::cout << "No students enrolled yet.\n";
            return;
        }
 
        // Rank students by total marks, descending
        std::vector<Student> ranked = students;
        std::sort(ranked.begin(), ranked.end(),
            [](const Student& a, const Student& b) { return a.getTotal() > b.getTotal(); });
 
        std::cout << std::left << std::setw(6) << "Rank" << std::setw(6) << "Roll"
                   << std::setw(20) << "Name" << std::setw(10) << "Total"
                   << std::setw(10) << "Average" << std::setw(6) << "Grade"
                   << "Status\n";
        std::cout << std::string(78, '-') << "\n";
 
        int rank = 1;
        double sumAverage = 0.0;
        int passCount = 0;
        for (const auto& s : ranked) {
            std::cout << std::left << std::setw(6) << rank++;
            s.printSummaryRow();
            sumAverage += s.getAverage();
            if (s.isPassing()) passCount++;
        }
 
        double classAverage = sumAverage / ranked.size();
        double passRate = (100.0 * passCount) / ranked.size();
 
        std::cout << std::string(78, '-') << "\n";
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Class Average   : " << classAverage << "\n";
        std::cout << "Highest Score   : " << ranked.front().getName()
                   << " (" << ranked.front().getTotal() << ")\n";
        std::cout << "Lowest Score    : " << ranked.back().getName()
                   << " (" << ranked.back().getTotal() << ")\n";
        std::cout << "Pass Rate       : " << passRate << "% (" << passCount
                   << "/" << ranked.size() << " students)\n";
        std::cout << "==========================================================\n";
    }