# DS2HW5

This project provides a menu-driven C++ program for:
- external merge sorting binary `Record` data files
- building a primary index from the sorted output

## 1) Program Features

- Reads binary files named `pairs<fileNo>.bin` (for example: `pairs501.bin`).
- Uses a fixed in-memory buffer size of 300 records.
- Performs two-phase sorting:
  - **Internal sort phase**: sorts each 300-record chunk and writes temporary runs.
  - **External merge phase**: repeatedly merges runs until one final sorted file remains.
- Produces sorted output as `order<fileNo>.bin`.
- Builds and prints a **primary index** from the sorted file as `(weight, offset)` pairs.
- Prints timing metrics for internal sort, external merge, and total execution time.
- Supports repeated runs through an interactive menu until the user quits.

## 2) Whole Program Flow

1. Program starts and shows the menu.
2. User enters a file number:
   - `0` exits.
   - otherwise the program validates that `pairs<fileNo>.bin` exists.
3. **Task 1 (External merge sort)** runs:
   - reads records from `pairs<fileNo>.bin`
   - groups records in chunks of 300
   - stable-sorts each chunk by `weight` descending
   - writes each sorted chunk into a temporary run file
   - repeatedly merges run files pairwise into larger sorted runs
   - renames the final run to `order<fileNo>.bin`
   - reports execution times
4. **Task 2 (Primary index construction)** runs:
   - reads `order<fileNo>.bin` sequentially
   - stores the first occurrence offset for each distinct `weight`
   - prints the index list
5. Program asks whether to continue or quit, then loops.

## 3) Algorithms Used in Task Implementation

### Task 1: External Merge Sort
- **Chunking strategy**: fixed-size blocks of 300 records (buffer-limited sorting).
- **Internal sorting**: `stable_sort` on each block, ordered by descending `weight`.
- **Run generation**: each sorted block is written as a temporary binary run.
- **K-way approach used**: repeated **2-way merge passes** over run files until one run remains.
- **Merge rule**: compare current heads of two runs and write the larger `weight` first.
- **Complexity pattern**:
  - internal per-run sort: `O(M log M)` for run size `M=300`
  - external merging over all records: approximately `O(N log R)` where `R` is number of initial runs

### Task 2: Primary Index Construction
- **Single sequential scan** over the sorted file.
- Because data is already sorted by `weight` descending, each new distinct `weight` appears in one contiguous region.
- The algorithm records `(weight, first_record_offset)` whenever `weight` changes.
- Time complexity is `O(N)` with linear pass and `O(U)` index storage (`U` = unique weights).

## 4) Recent `main` Branch Commits

Snapshot from `origin/main`:

- `54b7876` — `6/5 10:21 (pass test)`
- `c912bbc` — `Merge pull request #1 from meatballguy/kitakaaki`
- `a72f7e7` — `6/5 0:11 update`
- `d7687b7` — `add gitignore`
- `b49fc90` — `Merge branch 'master' of https://github.com/meatballguy/DS2HW5`
- `95a16b8` — `task1 maybe okay`
- `367be46` — `Initial commit`