# Cloth Lab — a cloth simulation from scratch

A mass-spring cloth simulation built from the ground up: position Verlet integration with a constraint-relaxation solver (Jakobsen), a **C++17 core verified by 290 unit-test assertions**, and an **interactive raw-WebGL2 demo** with a hand-written procedural weave shader. No physics or rendering libraries.

**Live:** https://leeyunhome.github.io/cloth-simulation/ · **Demo:** https://leeyunhome.github.io/cloth-simulation/demo/

한국어 안내는 라이브 페이지의 **KO/EN 토글**을 사용하세요 (페이지가 한·영 병기).

---

## What's here

```
├── index.html            # bilingual case study (the story, numbers, bugs the tests caught)
├── demo/index.html       # interactive WebGL2 demo — single self-contained file
├── cpp/
│   ├── include/cloth/cloth.hpp   # the reference implementation (header-only, C++17)
│   ├── tests/test_cloth.cpp      # 7 Catch2 test cases · 290 assertions
│   ├── src/demo_headless.cpp     # no-renderer stability report (CI smoke check)
│   ├── third_party/catch2/       # vendored Catch2 v2 single header (offline builds)
│   └── CMakeLists.txt
└── .github/workflows/ci.yml      # Ubuntu · Windows · macOS build + ctest
```

The physics was implemented in C++ first and pinned down by tests, then ported line-for-line to JavaScript for the browser demo. Two real bugs were caught by the tests along the way — a NaN-producing division by zero on degenerate grids, and an ordering bug where a tear-exempt bend constraint could hide overstretch from the tearing check. Both stories are in the case study.

## The model

- 45×31 particle grid → **1,395 particles**
- **7,992 distance constraints**: 2,714 structural (stretch) + 2,640 shear + 2,638 bend
- Position Verlet at a fixed 120 Hz substep; stiffness controlled by relaxation iteration count
- Aerodynamic per-triangle wind (`n·w` along the normal — that's what makes it billow)
- Sphere + floor collision by projection, tearing as a pre-solve pass, mouse grabbing via camera-basis ray picking

## Building the C++ core

```bash
cmake -S cpp -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Or without CMake:

```bash
g++ -std=c++17 -O2 -Icpp/include -Icpp/third_party/catch2 cpp/tests/test_cloth.cpp -o test_cloth
./test_cloth    # All tests passed (290 assertions in 7 test cases)
```

## Running the demo locally

`demo/index.html` is a single self-contained file (no ES modules, no CDN dependencies) — it opens directly in a browser, or serve the repo root:

```bash
python -m http.server 8000   # → http://localhost:8000/
```

## References

Jakobsen, *Advanced Character Physics* (GDC 2001) · Provot, *Deformation constraints in a mass-spring model* (1995). All code in this repo written from scratch.
