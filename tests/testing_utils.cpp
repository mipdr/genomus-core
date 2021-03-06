#include "testing_utils.hpp"
#include <functional>
#include <iostream>
#include <sstream>
#include <chrono>

using sclock = std::chrono::system_clock;
using sec = std::chrono::duration<double, std::milli>;

static int test_case_counter = 1;
static bool verbose = false;

GTest::GTestCase::GTestCase(string name, function<void(ostream&)> run) {
    this -> _name = name;
    this -> _run_test_case = run;
}

void printOutput(){ throw runtime_error("Dummy print error"); }

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
            regular_out << verbose_out.str() << endl;
        }
        regular_out << "\n" << e.what() << "\n";
        return { .text = regular_out.str(), .error_state = g_failure };
    } catch (bad_function_call bfc) {
        regular_out << "FAILED" << endl;
        if (verbose_out.str().length()) {
            regular_out << "\tTest output:" << endl;
            regular_out << verbose_out.str() << endl;
        }
        regular_out << "\n" << bfc.what() << "\n";
        return { .text = regular_out.str(), .error_state = g_failure };
    } catch (...) {
        regular_out << "FAILED" << endl;
        std::exception_ptr p = std::current_exception();
        regular_out << (p ? p.__cxa_exception_type() -> name() : "null") << endl;
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
    this -> _before = [](ostream&) { return; };
    this -> _after = [](ostream&) { return; };
}

GTest& GTest::testCase(string title, function<void(ostream &)> run_test_case) {
    this -> _test_cases.push_back(GTestCase(title, run_test_case));
    return *this;
}

GTest& GTest::testCase(string title, function<void()> run_test_case) {
    this -> _test_cases.push_back(GTestCase(title, [=](ostream&) { run_test_case(); }));
    return *this;
}

GTest& GTest::before(function<void(ostream &)> run) {
    this -> _before = run;
    return *this;
}

GTest& GTest::before(function<void()> run) {
    this -> _before = [=](ostream&) { run(); };
    return *this;
}

GTest& GTest::beforeEach(function<void()> run) {
    this -> _before_each = [=](ostream&) { run(); };
    return *this;
}

GTest& GTest::after(function<void(ostream &)> run) {
    this -> _after = run;
    return *this;
}

GTest& GTest::after(function<void()> run) {
    this -> _after = [=](ostream&) { run(); };
    return *this;
}

GTestErrorState GTest::run() {
    GTestErrorState suite_success = g_success;
    cout << " + TEST: " << this -> _name << endl;

    this -> _before(cout); 

    chrono::time_point<sclock> total_before, before;
    sec total_duration, duration;
    
    total_before = sclock::now();

    for (auto testCase : this -> _test_cases) {
        this -> _before_each(cout);

        before = sclock::now();
        auto result = testCase.run();
        if (result.error_state == g_success) {
            this -> _n_success++;
        } else {
            suite_success = g_failure;
        }
        duration = sclock::now() - total_before;
        cout << result.text << " after " << duration.count() << "ms" << endl;
        cout.flush();
    }

    total_duration = sclock::now() - total_before;

    test_case_counter = 1;

    this -> _after(cout);

    cout << " - " <<  this -> _n_success << "/" << this -> _test_cases.size() << " test cases were successful after " << total_duration.count() << "ms" << endl << endl;

    return suite_success;
}