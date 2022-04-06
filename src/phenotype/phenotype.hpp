#ifndef __GENOMUS_CORE_PHENOTYPE__
#define __GENOMUS_CORE_PHENOTYPE__

#include <string>
#include <vector>

using namespace std;

namespace Phenotype {
    enum DecodedPhenotypeFormats {
        MIDI,
        LILLYPOND,
        UNKNOWN,
    };

    class DecodedPhenotype {
        private:
            DecodedPhenotypeFormats format;
            char* buffer;
    };

    class Species {
        public:
            Species();
            int getNumberOfParameters();
    };

    template<class T>
    class Event {
        private:
            int param_n;
    };

    template<class T>
    class Voice {
        private:
            vector<Event<T>> events;
    };

    template<class T>
    class Score {
        private:
            vector<Voice<T>> voices;
    };

    template<class T>
    class EncodedPhenotype {
        private:
            Score<T> score;
        public:
            DecodedPhenotype decode(DecodedPhenotypeFormats format);
            string hello() {
                return "Hello from encoded phenotype";
            }
    };

    class Piano : public Species {
        public:
            Piano();   
    };
}

#endif 