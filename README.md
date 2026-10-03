# BoundedReversiblePermutation

A bounded reversible permutation primitive for integer domains in portable C17.

## Overview

BRP maps every integer in `[0, N)` to exactly one integer in the same domain. `BRP16-V1` and `BRP32-V1` use deterministic seeded initialization and provide forward and exact inverse operations.

## Properties

- Bijective for every successfully initialized supported domain.
- Deterministic for a given algorithm version, domain, and seed.
- Supports domains from 1 through `UINT16_MAX` or `UINT32_MAX`, respectively.
- Fixed-size caller-owned context; no heap, domain-sized runtime tables, or mutable global state.
- The seed mixer and permutation are not cryptographic. This primitive is not intended for cryptographic use.

## API

Include `brp.h`. Initialize a `brp16_ctx_t` or `brp32_ctx_t` with `brp16_init_v1` or `brp32_init_v1`, then call the corresponding `brp*_forward_v1` and `brp*_inverse_v1` functions. Keep an initialized context immutable while using it. The header documents invalid-input fallback behavior.

## Example

[`examples/basic.c`](examples/basic.c) maps a value and applies the inverse. Build it as the `brp_basic` CMake target.

## Build

Requires CMake and a C17 compiler. CMake explicitly requests ISO C17 without compiler extensions.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Select a compiler with `-DCMAKE_C_COMPILER=gcc` or `clang` when configuring a fresh build directory.

## Test

```sh
ctest --test-dir build --output-on-failure
```

Tests cover basic behavior, exhaustive bijection and inverse properties for domains 1 through 4096 over three seeds, and the frozen V1 compatibility vectors in `tests/vectors_v1.csv`.

## Embedded characteristics

The production core is C17 and freestanding-compatible. It uses fixed-width integer types, caller-owned fixed-size contexts, no heap allocation, no runtime storage proportional to the domain, and no mutable global state. Context layout is an in-memory API detail, not a serialized format. A host object measurement with GCC 13.3 and Clang 18.1 on x86-64 Linux using `-Os -ffreestanding` reports `.text` sizes of 629 and 773 bytes, respectively, with `.data` and `.bss` both zero. These object-level figures are toolchain-specific and exclude caller-owned context storage.

## Algorithm notes

Each V1 round is a conditional reflection involution; inverse applies the six rounds in reverse order. The permutation is intentionally non-cryptographic.

## Compatibility / stability

`BRP16-V1` and `BRP32-V1` define deterministic compatibility behavior. Their frozen vectors are compatibility oracles. The repository version `0.1.0` is separate from the algorithm version macros.

## License

No license has been selected yet.
