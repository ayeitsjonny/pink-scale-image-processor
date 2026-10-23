# Benchmarks

Performance of `src/processor.c` on a Raspberry Pi under five GCC settings.

## Method

Each configuration was compiled, then run 1000 times on the same 256 × 256 input image using this bash helper. The **sys** time reported by `time` was recorded, along with the executable size in bytes.

```bash
benchmark() { time for i in {1..1000}; do eval "$@"; done; }
benchmark './processor < input_image.ppm > output_image.ppm'
```

## Results

| Setting | GCC flags | sys time (1000×, s) | Executable size (bytes) |
|---------|-----------|---------------------|-------------------------|
| Normal | *(none)* | 0.569 | 71008 |
| Optimise | `-O3` | 0.307 | 70992 |
| Static | `--static` | 0.530 | 779304 |
| CPU | `-O3 -mcpu=cortex-a76` | 0.305 | 70992 |
| Size | `-Oz -s` | 0.285 | 67672 |

## Observations

- **Optimisation roughly halves the runtime.** `-O3` cut sys time from 0.569 s to 0.307 s, mostly by optimising the per-pixel loop.
- **`-mcpu=cortex-a76` adds almost nothing over `-O3`.** The results (0.307 s vs 0.305 s) are within run-to-run noise for a simple loop like this.
- **Static linking produces by far the largest binary.** It is about 11× bigger and only slightly faster than the unoptimised build.
- **`-Oz -s` gives the smallest binary and the lowest time.** Stripping debug info shrinks the file, though timing fluctuates between runs, so the gap to `-O3` is small.
