// demo_headless.cpp — run the cloth core without any renderer and print a
// stability report. Useful for profiling and for CI smoke checks; also shows
// the library is usable standalone (the WebGL demo is a port of this loop).

#include <cloth/cloth.hpp>

#include <cstdio>

int main() {
    using cloth::Cloth;
    using cloth::Vec3;

    constexpr int   kCols = 45, kRows = 31;
    constexpr float kDt   = 1.f / 120.f;

    Cloth c(kCols, kRows, 1.8f, 1.2f);
    for (int x = 0; x < kCols; x += 11) c.pin(c.indexOf(x, 0));
    c.pin(c.indexOf(kCols - 1, 0));

    const Vec3  sphereCenter{0.f, -0.85f, 0.15f};
    const float sphereRadius = 0.32f;

    std::printf("cloth %dx%d  particles=%d  constraints=%d  dt=1/120\n",
                kCols, kRows, c.particleCount(),
                static_cast<int>(c.constraints().size()));
    std::printf("%6s %14s %12s %10s\n", "step", "maxStretch", "meanSpeed", "minY");

    for (int step = 1; step <= 720; ++step) {
        c.applyWind({0.f, 0.f, 1.1f});
        c.step(kDt);
        c.collideSphere(sphereCenter, sphereRadius);

        if (step % 120 == 0) {
            float minY = 0.f;
            for (int i = 0; i < c.particleCount(); ++i)
                if (c.posOf(i).y < minY) minY = c.posOf(i).y;
            std::printf("%6d %13.1f%% %12.5f %10.3f\n", step,
                        (c.maxStretchRatio() - 1.f) * 100.f,
                        c.meanSpeed(kDt), minY);
        }
    }

    const bool stable = c.maxStretchRatio() < 1.25f;
    std::printf("\nresult: %s (max stretch %.1f%% %s 25%% budget)\n",
                stable ? "STABLE" : "UNSTABLE",
                (c.maxStretchRatio() - 1.f) * 100.f, stable ? "<" : ">=");
    return stable ? 0 : 1;
}
