# Hosting plan — GitHub Pages

Same convention as the rest of the site: **one repo per project, served at `leeyunhome.github.io/<repo>/`**, hub links out to each.

| Piece | URL |
|---|---|
| Case study (this repo's `index.html`) | `https://leeyunhome.github.io/cloth-simulation/` |
| Interactive demo | `https://leeyunhome.github.io/cloth-simulation/demo/` |
| Sibling project (3DGS) | `https://leeyunhome.github.io/splatting-viewer/` |

## One-time setup

1. Create the GitHub repo `leeyunhome/cloth-simulation`, push `main`
2. **Settings → Pages** → Source: *Deploy from a branch* → `main` / `(root)` → Save
3. Live in 1–2 minutes

No build step — everything is static. The demo is a single self-contained HTML file with **zero CDN dependencies** (unlike splatting-viewer, it doesn't even need an importmap), so it works offline and over `file://`.

## Deploy

```bash
git push origin main   # Pages redeploys automatically
```

## Size

Total repo ≈ 1.6 MB (biggest file: vendored `catch.hpp`, 657 KB; screenshots ≈ 900 KB). No large-file concerns.

## Hub link

After the first deploy, add a card to `leeyunhome.github.io` under 🤖/🛠 sections pointing at `https://leeyunhome.github.io/cloth-simulation/` (remember: hub repo is pushed from multiple environments — `git fetch` + rebase before pushing).
