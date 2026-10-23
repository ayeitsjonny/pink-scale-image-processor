# Pink Scale Image Processor (Raspberry Pi, C)

A small C program that reads a binary PPM (P6) image from `stdin`, converts it to a pink/purple "pink scale" version, and writes the result to `stdout`. Written for the INF1102 *Programming the Pi* project and benchmarked on a Raspberry Pi with several GCC optimisation settings.

## How It Works

For every pixel, a weighted sum of the original R, G, B values is computed:

```
new = 0.299 * old_R + 0.587 * old_G + 0.114 * old_B
```

The output channels are then set as:

```
new_R = new
new_G = new / 2
new_B = new
```

Since R and B are equal and G is half as bright, the image comes out in shades of pink/purple.

## Implementation Notes

- **Data type:** all pixel values and intermediate calculations use `double`. Each input byte is cast with `(double)` and each result is cast back with `(uint8_t)` when written.
- **Pass by reference:** images are passed to the processing function as pointers. The input is `const` (read-only) and the output buffer is written by the function:

```c
static void do_processing(const uint8_t *im_in, uint8_t *im_out,
                          int width, int height);
```

- **Indexing:** a pixel's R byte is at `idx = (y * width + x) * 3`, with G at `idx + 1` and B at `idx + 2`.
- **Clamping:** the weighted sum is clamped to 255.0 to guard against rounding.
- **I/O:** the image comes in on `stdin` and goes out on `stdout`, so diagnostic messages are printed to `stderr`.
- **Config:** image dimensions come from a generated `inf1102_config.h` (not included in this repo).

## Build and Run

```bash
gcc -o processor src/processor.c
./processor < input_image.ppm > output_image.ppm
```

The input must be a P6 PPM matching the dimensions in the config header (256 × 256 in this project).

If the terminal fills with junk, the output wasn't redirected. Press Ctrl-C and type `reset`.

## Benchmarks

The program was timed over 1000 runs on a Raspberry Pi with five GCC settings (normal, `-O3`, `--static`, `-O3 -mcpu=cortex-a76`, `-Oz -s`). The short version: `-O3` roughly halves the runtime, static linking gives a much larger binary, and `-Oz -s` is both smallest and fastest.

Full results and discussion are in [docs/benchmarks.md](docs/benchmarks.md).

## Repository Layout

```
.
├── README.md
├── .gitignore
├── src/
│   └── processor.c
└── docs/
    └── benchmarks.md
```

Project-specific generated files (config header, input/output images, submission package) are intentionally not included.
