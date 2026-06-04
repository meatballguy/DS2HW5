# DS2HW5

## Newest Feature: Program Flow of the Main Program

The main program consists of two main missions that operate in a loop until the user decides to quit:

1. **Mission 1: External Merge Sort**
   - The user inputs a file number (e.g., `501`), and the program reads binary data from `pairs<fileNo>.bin`.
   - **Internal Sort**: Records are read into an internal buffer (maximum 300 records), sorted by `weight` in descending order, and saved as temporary run files.
   - **External Sort**: The temporary files are merged two-by-two in passes until a single sorted output file `order<fileNo>.bin` is produced.
   - The execution time for both the internal sort and external merge steps is displayed.

2. **Mission 2: Build the Primary Index**
   - The program reads the sorted `order<fileNo>.bin` file.
   - It constructs a primary index mapping each unique `weight` value to its initial record index (offset) in the file.
   - The primary index is then printed to the console.

After completing both missions, the program prompts the user to either quit or continue with another file.