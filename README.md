# FLAPPY BIRD GAME IN CPP

## Build and run (Windows + MSYS2 UCRT64)

1. Install dependencies in MSYS2 UCRT64 shell:

```bash
pacman -S --needed mingw-w64-ucrt-x86_64-toolchain mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-ninja mingw-w64-ucrt-x86_64-SDL3 mingw-w64-ucrt-x86_64-SDL3_image
```

2. Configure:

```bash
cmake -S . -B build -G Ninja -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++
```

3. Build:

```bash
cmake --build build
```

4. Run:

```bash
./build/game
```

## Build for web (touch + browser)

1. Install Emscripten and activate the environment:

```bash
emsdk install latest
emsdk activate latest
emsdk_env.bat
```

2. Configure and build:

```bash
emcmake cmake -S . -B build-web -G Ninja
cmake --build build-web
```

3. Test locally:

```bash
emrun --no_browser --port 8080 build-web/game.html
```

4. Upload these files from `build-web` to your website host:
- `game.html`
- `game.js`
- `game.wasm`
- `game.data`

## Publish on GitHub Pages (already prepared)

The project now includes `docs/` with:
- `docs/index.html`
- `docs/game.js`
- `docs/game.wasm`
- `docs/game.data`
- `docs/.nojekyll`

To publish:
1. Create a GitHub repo and push this project.
2. Open repository `Settings` -> `Pages`.
3. Set `Source` to `Deploy from a branch`.
4. Select branch `main` and folder `/docs`, then save.
5. Wait ~1-3 minutes and open your Pages URL.
