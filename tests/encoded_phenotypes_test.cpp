#include "encoded_phenotype.hpp"
#include "genomus-core.hpp"
#include "species.hpp"
#include <iostream>

using namespace std;

namespace EncodedPhenotypesTest {
    void run() {
        auto my_event = Event({
            .parameter_types = {duration, pitch, intensity},
            .parameter_values = {0.5, 0.3, 1.2},
        });

        auto my_voice = Voice({
            .events = {
                Event({
                    .parameter_types = {duration, pitch, intensity},
                    .parameter_values = {0.5, 0.3, 1.2},
                }),
                Event({
                    .parameter_types = {duration, pitch, intensity},
                    .parameter_values = {0.1, 2.5, 1.3},
                }),
                Event({
                    .parameter_types = {duration, pitch, intensity},
                    .parameter_values = {0.5, 0.3, 1.2},
                }),
            }
        });

        auto my_score = Score({
            .voices = {
                Voice({
                    .events = {
                        Event({
                            .parameter_types = {duration, pitch, intensity, articulation},
                            .parameter_values = {0.5, 0.3, 1.2, 3.0},
                        }),
                        Event({
                            .parameter_types = {duration, pitch, intensity, articulation},
                            .parameter_values = {0.6, 0, 1.4, 2.0},
                        }),
                    }
                }),
                Voice({
                    .events = {
                        Event({
                            .parameter_types = {duration, pitch, intensity, articulation},
                            .parameter_values = {0.5, 0.3, 1.2, 3.0},
                        }),
                        Event({
                            .parameter_types = {duration, pitch, intensity, articulation},
                            .parameter_values = {0.5, 0, 1.4, 2.0},
                        }),
                        Event({
                            .parameter_types = {duration, pitch, intensity, articulation},
                            .parameter_values = {0.1, 0, 1.4, 2.0},
                        }),
                    }
                })
            }
        });

        // cout << "\nSAMPLE EVENT:" << endl;
        // cout << my_event.toString() << endl;

        // cout << "\nSAMPLE VOICE:" << endl;
        // cout << my_voice.toString(true) << endl;

        // cout << "\nSAMPLE SCORE:" << endl;
        // cout << my_score.toString(true) << endl;

        cout << CURRENT_SPECIES.toString() << endl;
    }
}
