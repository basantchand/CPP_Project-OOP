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