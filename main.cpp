#include "fsm.h"

int main() {
    fsm::Fsm<int> fsm({0, 1, 2, 3}, {'a', 'b'}, {{{1, 2}, {2}}, {{2}, {3}}, {{1, 2}, {3}}, {{}, {}}}, {0}, {3});
    fsm.inspect();
    std::cout << "\n";
    fsm.min().inspect();

    return 0;
}
