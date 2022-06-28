
#include <functional>
#include <vector>

using namespace std;

enum GTestErrorState {
    g_success,
    g_failure,
};

void printOutput();

class GTest {
    public:

    class GTestCase {
        public:
            struct GTestCaseOutput {
                string text;
                GTestErrorState error_state;
            };
        private:
            string _name;
            function<void(ostream&)> _run_test_case;
        public:
            GTestCase(string, function<void(ostream&)>);
            GTestCaseOutput run();
    };

    private:
        string _name;
        vector<GTestCase> _test_cases;
        function<void(ostream&)> _before;
        function<void(ostream&)> _after;
        unsigned int _n_success;
    public:
        GTest(string title);
        GTestErrorState run();
        GTest& testCase(string title, function<void(ostream&)>);
        GTest& testCase(string title, function<void()>);
        GTest& before(function<void(ostream&)>);
        GTest& before(function<void()>);
        GTest& after(function<void(ostream&)>);
        GTest& after(function<void()>);
};

extern GTest 
    EncodedPhenotypesTest,
    DecodedGenotypesTest,
    ParserTest,
    EncodedGenotypesTest;