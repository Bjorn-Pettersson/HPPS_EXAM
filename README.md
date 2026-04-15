# HPPS Exam (Exam 40, 01-2025)

This repository contains my High Performance Programming exam implementation in C (OpenMP), along with the written report and supporting utilities.

The project implements:
- Integer histograms from sample files
- Distance histograms from 2D point files
- Sequential and parallel algorithms for benchmarking and analysis
- Utility programs for data generation and format conversion

## Repository Contents

- `histogram.c` / `histogram.h`: Counting histogram algorithms (`sequential`, `parallel_bins`, `parallel_samples`)
- `dist-histogram.c` / `dist-histogram.h`: Distance histogram algorithms (`sequential`, `parallel`)
- `run-histogram.c`: Benchmark runner for counting histograms
- `run-dist-histogram.c`: Benchmark runner for distance histograms
- `gen-ints.c`: Generate integer datasets
- `gen-points.c`: Generate 2D point datasets
- `ints2text.c`, `points2text.c`: Convert binary files to text output for inspection
- `strings2mapping.c`, `strings2ints.c`: Mapping/string conversion tools
- `util.c` / `util.h`: Shared I/O and timing utilities
- `Exam-hpps.pdf`: Full exam report

## Build

Requirements:
- C compiler with OpenMP support (tested with GCC/Clang)
- `make`

Build all programs:

```bash
make
```

Clean build artifacts:

```bash
make clean
```

## Command-Line Usage

From the project root:

```text
gen-ints            N K STRIDE OUTFILE
gen-points          N STRIDE OUTFILE
ints2text           INTSFILE
points2text         POINTSFILE
strings2mapping     STRINGSFILE MAPPINGFILE
strings2ints        MAPPINGFILE STRINGSFILE INTSFILE
run-histogram       RUNS ALGORITHM K SAMPLESFILE HISTOGRAMFILE
run-dist-histogram  RUNS ALGORITHM K SAMPLESFILE HISTOGRAMFILE
```

Supported algorithms:
- `run-histogram`: `sequential`, `parallel_bins`, `parallel_samples`
- `run-dist-histogram`: `sequential`, `parallel`

## Quick Start

### 1) Counting histogram

Generate integer samples, run a histogram, and print results:

```bash
./gen-ints 1000000 86 7 ints.ints
OMP_NUM_THREADS=8 ./run-histogram 5 parallel_samples 86 ints.ints hist.ints
./ints2text hist.ints | head
```

### 2) Distance histogram

Generate points, run distance histogram, and print results:

```bash
./gen-points 100000 1.4 points.pts
OMP_NUM_THREADS=8 ./run-dist-histogram 5 parallel 100 points.pts dist.ints
./ints2text dist.ints | head
```

## Notes on the Solution

- For distance histograms, pair distances are computed with an upper-triangular loop (`j = i + 1`) and then double-counted. This matches the interpretation used by the provided reference outputs.
- All provided test cases pass for this submission.
- No memory leaks were detected with valgrind during testing.

## Performance Discussion Summary

Hardware used for benchmarking:
- Intel Core i7-7700HQ @ 2.80GHz
- 4 physical cores / 8 hardware threads
- Cache: L1 256 KiB (data, total), L2 1 MiB, L3 6 MiB shared

### Counting histogram (`histogram.c`)

Observations:
- `parallel_samples` scales better than `parallel_bins` in these experiments.
- `parallel_bins` repeats a full scan of the sample array per thread, which limits speedup.
- Data distribution can affect runtime through locality and contention.

Representative results from the report:

| Threads | Strong (bins) | Strong (samples) | Weak (bins) | Weak (samples) |
|---:|---:|---:|---:|---:|
| 1 | 1.000 | 1.000 | 1.000 | 1.000 |
| 2 | 1.005 | 0.965 | 1.551 | 2.535 |
| 4 | 0.878 | 0.815 | 1.825 | 2.261 |
| 8 | 0.900 | 0.385 | 1.845 | 1.994 |

| Threads | Strong (bins) | Strong (samples) | Weak (bins) | Weak (samples) |
|---:|---:|---:|---:|---:|
| 1 | 1.000 | 1.000 | 1.000 | 1.000 |
| 2 | 0.839 | 1.361 | 0.892 | 1.241 |
| 4 | 0.741 | 2.087 | 0.737 | 1.772 |
| 8 | 0.461 | 2.796 | 0.491 | 2.955 |

### Distance histogram (`dist-histogram.c`)

Observations:
- Distance histogram locality is generally worse than counting histogram locality due to all-pairs work (`O(n^2)`) and more scattered bin updates.
- Parallelization uses thread-local histograms plus reduction to avoid races and reduce contention.
- Dynamic scheduling gave the best practical tradeoff in this implementation.

Representative speedup figures from the report:

| Threads | Strong | Weak |
|---:|---:|---:|
| 1 | 1.000 | 1.000 |
| 2 | 1.837 | 1.308 |
| 4 | 3.098 | 1.536 |
| 8 | 3.994 | 1.403 |

## Reproducing Benchmarks

General benchmark pattern:

```bash
OMP_NUM_THREADS=<t> ./run-histogram <runs> <algorithm> <k> <samplesfile> <outfile>
OMP_NUM_THREADS=<t> ./run-dist-histogram <runs> <algorithm> <k> <pointsfile> <outfile>
```

Suggested process:
1. Generate datasets with `gen-ints` or `gen-points`.
2. Sweep thread counts (`1, 2, 4, 8`) using `OMP_NUM_THREADS`.
3. Run multiple repetitions and compare mean runtime.
4. Compute speedup/throughput externally (spreadsheet or script).

## Known Limitations and Open Questions

- Weak scaling for distance histograms is not perfect in this submission.
- The workload definition for weak scaling in an `O(n^2)` all-pairs problem is subtle and may impact interpretation.
- The chosen distance counting convention (double counting) follows the provided examples.

## License

No explicit license is currently provided.
If you plan to make this public, add a license file (for example, MIT or BSD-3-Clause).
