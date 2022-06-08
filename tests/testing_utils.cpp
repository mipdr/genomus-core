#include "testing_utils.hpp"
#include <iostream>
#include <regex>
#include <sstream>

static int test_case_counter = 0;
static bool verbose = false;

GTest::GTestCase::GTestCase(string name, function<void(ostream&)> run) {
    this -> _name = name;
    this -> _run_test_case = run;
}

GTest::GTestCase::GTestCaseOutput GTest::GTestCase::run() {
    stringstream regular_out, verbose_out;
    regular_out << "\t - Test case #" << test_case_counter << ": " << this -> _name << " -> ";
    test_case_counter++;

    try {
        this -> _run_test_case(verbose_out);
    } catch (runtime_error e) {
        regular_out << "FAILED" << endl;
        if (verbose_out.str().length()) {
            regular_out << "\tTest output:" << endl;
            regular_out << regex_replace(verbose_out.str(), regex("\n"), "\n\t") << endl;
        }
        regular_out << "\n" << e.what() << "\n";
        return { .text = regular_out.str(), .error_state = g_failure };
    }
    regular_out << "OK"; 
    if (verbose) {
        regular_out << "\n" << verbose_out.str() << endl;
    }
    return { .text = regular_out.str(), .error_state = g_success }; 
}

GTest::GTest(string title) {
    this -> _name = title;
    this -> _test_cases = vector<GTestCase>();
    this -> _n_success = 0;
}

GTest GTest::testCase(string title, function<void (ostream &)> run) {
    this -> _test_cases.push_back(GTestCase(title, run));
    return *this;
}

void GTest::run() {
    cout << " + TEST: " << this -> _name << endl;
    for (auto testCase : this -> _test_cases) {
        auto result = testCase.run();
        if (result.error_state == g_success) {
            this -> _n_success++;
        }
        cout << result.text << endl;
        cout.flush();
    }

    cout << this -> _n_success << "/" << this -> _test_cases.size() << " test cases were successful" << endl;
}