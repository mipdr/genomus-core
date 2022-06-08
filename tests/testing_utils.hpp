
#include <functional>
#include <vector>

using namespace std;

enum GTestErrorState {
    g_success,
    g_failure,
};

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
        unsigned int _n_success;
    public:
        GTest(string title);
        void run();
        GTest testCase(string title, function<void(ostream&)>);
};

extern GTest 
    EncodedPhenotypesTest;