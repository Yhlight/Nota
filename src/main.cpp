#include "Nota.h"

int main(int argc, char* argv[]) {
    if (argc > 2) {
        // In the future, we will have a proper command-line parser.
        return 64; // Usage error
    } else if (argc == 2) {
        nota::Nota::run_file(argv[1]);
    } else {
        // In the future, this will run a REPL.
    }
    return 0;
}
