// test_cloth.cpp — physics correctness tests for the cloth core.
//
// These are not "does it compile" tests: each one checks a property the
// simulation must satisfy for the on-screen result to be trustworthy —
// integrator accuracy, solver convergence, collision invariants, stability.

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <cloth/cloth.hpp>

using cloth::Cloth;
using cloth::SpringType;
using cloth::Vec3;

static constexpr float kDt = 1.f / 120.f;

TEST_CASE("grid topology: particle and constraint counts match the formulas") {
    const int C = 45, R = 31;
    Cloth c(C, R, 1.8f, 1.2f);

    REQUIRE(c.particleCount() == C * R);

    // structural: horizontal (C-1)*R + vertical C*(R-1)
    // shear:      2 per cell = 2*(C-1)*(R-1)
    // bend:       (C-2)*R + C*(R-2)
    const int structural = (C - 1) * R + C * (R - 1);
    const int shear      = 2 * (C - 1) * (R - 1);
    const int bend       = (C - 2) * R + C * (R - 2);
    REQUIRE(static_cast<int>(c.constraints().size()) == structural + shear + bend);

    // two triangles per cell
    REQUIRE(static_cast<int>(c.triangles().size()) == (C - 1) * (R - 1) * 6);
}

TEST_CASE("pinned particles do not move under gravity") {
    Cloth c(10, 8, 1.f, 0.8f);
    c.pin(c.indexOf(0, 0));
    c.pin(c.indexOf(9, 0));
    const Vec3 p0 = c.posOf(c.indexOf(0, 0));

    for (int i = 0; i < 240; ++i) c.step(kDt);

    const Vec3 p1 = c.posOf(c.indexOf(0, 0));
    REQUIRE((p1 - p0).length() == Approx(0.f).margin(1e-6f));
}

TEST_CASE("Verlet free fall tracks the analytic 1/2·g·t^2 trajectory") {
    // Two particles falling together: the constraint stays at rest length,
    // so the trajectory is pure integration — compare against physics.
    Cloth c(2, 1, 0.1f, 0.1f);
    c.params.damping    = 1.f; // undamped for the analytic comparison
    c.params.iterations = 1;

    const float y0 = c.posOf(0).y;
    const int   n  = 240;             // 2 seconds at dt = 1/120
    for (int i = 0; i < n; ++i) c.step(kDt);

    const float t        = n * kDt;
    const float analytic = y0 + 0.5f * c.params.gravity.y * t * t;
    // Discrete Verlet from rest accumulates n(n+1)/2 · g·dt² vs n²/2 · g·dt²,
    // an O(1/n) difference — allow 1.5 %.
    REQUIRE(c.posOf(0).y == Approx(analytic).epsilon(0.015f));
}

TEST_CASE("relaxation solver restores rest length after a large displacement") {
    Cloth c(2, 1, 0.5f, 0.5f);
    c.params.gravity    = {0.f, 0.f, 0.f};
    c.params.iterations = 10;

    const float rest = c.constraints()[0].rest;
    c.setPos(1, c.posOf(0) + Vec3{rest * 4.f, 0.f, 0.f}); // stretch 400 %
    c.satisfy();

    const float len = (c.posOf(1) - c.posOf(0)).length();
    REQUIRE(len == Approx(rest).epsilon(1e-3f));
}

TEST_CASE("sphere collision keeps every particle outside the sphere") {
    Cloth c(20, 14, 1.2f, 0.8f);
    const Vec3  center{0.f, -0.9f, 0.f};
    const float radius = 0.35f;

    for (int i = 0; i < 300; ++i) { // cloth free-falls onto the sphere
        c.step(kDt);
        c.collideSphere(center, radius);
    }
    for (int i = 0; i < c.particleCount(); ++i) {
        REQUIRE((c.posOf(i) - center).length() >= radius - 1e-4f);
    }
}

TEST_CASE("overstretched structural constraints tear; bend constraints never do") {
    Cloth c(3, 1, 0.4f, 0.4f);
    c.params.gravity    = {0.f, 0.f, 0.f};
    c.params.tearFactor = 1.5f;
    c.pin(0);

    c.setPos(2, c.posOf(2) + Vec3{2.f, 0.f, 0.f}); // rip the right end away
    c.satisfy();

    int aliveStructural = 0, aliveBend = 0;
    for (const auto& con : c.constraints()) {
        if (con.type == SpringType::Structural && con.alive) ++aliveStructural;
        if (con.type == SpringType::Bend       && con.alive) ++aliveBend;
    }
    REQUIRE(aliveStructural < 2); // at least one structural spring tore
    REQUIRE(aliveBend == 1);      // bend springs are exempt from tearing
}

TEST_CASE("a damped hanging cloth settles instead of oscillating forever") {
    Cloth c(24, 16, 1.2f, 0.8f);
    for (int x = 0; x < 24; x += 4) c.pin(c.indexOf(x, 0));
    c.pin(c.indexOf(23, 0));

    for (int i = 0; i < 900; ++i) c.step(kDt); // 7.5 simulated seconds

    REQUIRE(c.meanSpeed(kDt) < 0.02f);    // effectively at rest
    REQUIRE(c.maxStretchRatio() < 1.15f); // and not overstretched (< 15 %)
}
