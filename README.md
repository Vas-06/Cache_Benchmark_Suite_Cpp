# Cache Benchmark Suite (Learning Project)

A small C++ benchmarking tool that empirically measures the AoS vs. SoA cache-efficiency tradeoff, rather than just reasoning about it theoretically. Built as the closing project of the Data-Oriented Design phase, after already applying SoA in a pool allocator and a particle simulation — this project turns that intuition into actual measured evidence.

This is a **basic/skeletal version** — the goal was proving out the core tradeoff with real numbers, not building a general-purpose benchmarking framework.

## What it does

- Defines the same conceptual data three ways: an **AoS** version (`class` with multiple fields per object, stored in one `vector`), an **SoA** version (each field in its own separate `vector`), and an **AoSoA** version (fixed-size batches, each internally laid out as SoA — a `vector` of small structs, each struct holding fixed-size arrays per field).
- Times two different access patterns using `<chrono>`:
  - **Single-component access** — touching just one field across every element (e.g. incrementing one stat).
  - **Multi-component access** — touching several/all fields across every element.
- Sweeps across a wide range of dataset sizes (from a handful of elements up to 10 million) for each pattern, rather than relying on one fixed size.
- Prints per-size timings for each layout plus an overall average across the sweep, so the trend is visible rather than a single snapshot.

## Concepts used

- `std::chrono::high_resolution_clock` for timing code sections precisely.
- Why dataset size matters when benchmarking cache effects — small datasets fit entirely in L1/L2 regardless of layout, so no real difference can appear until the working set exceeds cache capacity.
- Measurement noise at small scales — sub-microsecond timings are dominated by clock/OS jitter, not real behavior, and shouldn't be read as a meaningful result.
- Matching starting conditions between comparison runs (an early run used different starting sizes for the two tests, which made their averages non-comparable — fixed by aligning the size sweep for both).
- The actual mechanism behind the AoS/SoA tradeoff: single-field access benefits from SoA because each cache line pulled in is entirely useful data; multi-field access benefits from AoS because one cache line already contains everything needed for that element, while SoA would need to pull separate cache lines from multiple arrays and stitch them back together.
- AoSoA (Array of Structs of Arrays) — grouping elements into small fixed-size batches (matched to typical SIMD register width, e.g. 8), each batch internally storing its fields as separate arrays — intended as a hybrid layout that keeps related fields close together (helping multi-field access) while still keeping per-field data contiguous within a batch (helping single-field access).
- The gap between a data layout being theoretically favorable and a benchmark actually proving it: AoSoA's real advantage depends on something (SIMD instructions, or compiler auto-vectorization) actually processing a full batch at once. A plain scalar loop over an AoSoA structure gets none of that benefit but still pays the cost of an extra level of indexing (`data[i].field[j]` instead of a flat `array[i]`) — so it can legitimately measure *worse* than both plain AoS and SoA despite being "theoretically best of both worlds."

## What this builds on

Follows directly from the pool allocator and particle simulation projects, both of which applied SoA/DOD principles on faith from earlier architecture study. This project exists to actually verify that faith with measured data, using the same struct-of-arrays layout pattern established in those projects.

## Key takeaways

- The initial instinct — "SoA is just better" — turned out to be incomplete. The real, more defensible conclusion from the data: **SoA wins for single-field access patterns; AoS wins (or ties) for multi-field access patterns.** Which layout is "correct" depends entirely on how the data is actually going to be accessed, not on DOD being universally superior to OOP-style layout.
- Both effects were small to moderate in magnitude at the struct size used here (a handful of ints) — reproduced consistently across two separate runs, but not dramatic. Effect size would likely grow with larger structs or more fields, since that increases how much irrelevant data gets dragged along per cache line in the losing case.
- Small dataset sizes produce noisy, meaningless comparisons — a result only becomes trustworthy once the working set is large enough to actually exceed cache capacity. An early attempt at 100 elements showed a "difference" that was pure timing noise, not a real cache effect.
- Reproducing a result before trusting it mattered here — the first multi-component run showed a fairly large AoS advantage; a second, more carefully controlled run (matched starting size) showed a smaller but consistent one. The smaller, reproduced number is the one worth reporting.
- The biggest single takeaway from adding AoSoA: it is not a free lunch. It underperformed both plain AoS and SoA at every size in this benchmark, because nothing in the test actually exploited the batching via SIMD — the layout alone doesn't deliver a speedup, it only enables one, conditional on the code processing a batch at once rather than element-by-element. "Best of both worlds" is a claim about *potential*, not a guarantee that just restructuring data automatically realizes it.

## Status

Complete for its purpose: a three-layout (AoS, SoA, AoSoA), two-pattern, multi-size benchmark producing clear, reproducible, correctly-nuanced results — including a result that complicated the initial expectation rather than just confirming it. Not extended with actual SIMD intrinsics or verified compiler auto-vectorization (flagged as a natural next step, but out of scope here), more exotic access patterns, alignment/padding experiments, or automated statistical analysis beyond the single-pass sweep and average.
