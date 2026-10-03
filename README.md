# Bounded Reversible Permutation

Native C17 implementation of BRP32-V1 and BRP16-V1, mapping `[0, N)` to itself. V1 uses six deterministic conditional reflection rounds; each round is an involution and inverse applies rounds in reverse order. The 32-bit seed expansion and mixer are deterministic, non-cryptographic compatibility behavior. Contexts must be initialized and then kept immutable. See `include/brp.h` for the API and invalid-input behavior.

Build and run tests with CMake and CTest. The core uses fixed-width integers and no dynamic allocation or mutable global state.
