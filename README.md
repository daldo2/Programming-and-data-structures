# Programming and Data Structures

A collection of C programming projects focusing on algorithms, graphics logic, and robust memory management.

## Projects

### Banking System

A file-based account management system utilizing binary data storage and record linking.

* **Data Persistence**: Uses `data.bin` to store `Account` structures, including names, addresses, and validated PESEL numbers.
* **Relational Linking**: Links `InsuranceRecord` entries to specific accounts using account numbers as foreign keys.
* **Binary File I/O**: High-performance data access using `fseek`, `ftell`, and `rewind` for precise record updates.

### Octal Text Processor
A utility for processing and summing large octal numbers from standard input.
* **Memory Safety**: Implements manual heap management with `malloc`, `realloc`, and `free`.
* **Error Injection**: Uses a custom `rand_malloc` wrapper to simulate and handle memory allocation failures.
* **Input Handling**: Features whitespace trimming and dynamic line buffering.

### Graphics Projects
Interactive projects developed using the `primlib` library:
* **Tetris**: Game state management, piece rotation, and line clearing logic.
* **Rotating Polygon**: Matrix-based transformations for 2D shapes.
* **Tower of Hanoi**: Recursive algorithm visualization.

---

## Setup & Compilation

### Dependencies
* Graphics projects require **SDL2** and **SDL2_gfx**.
* Standard projects require a **GCC** compiler.

### Compiling Banking System

Navigate to `/banking-system` and execute:

gcc banking_system.c -o banking_system


### Compiling Octal Text Processor
Navigate to `/octal-processor` and execute:
gcc main.c rand_malloc.c -o octal_processor

### Compiling Graphics Projects
Navigate to the specific project directory and execute:
gcc main.c ../primlib/primlib.c -o program_name -lSDL2 -lSDL2_gfx -lm

<img width="1401" height="818" alt="image" src="https://github.com/user-attachments/assets/4594463f-4d62-471a-bd27-103554e26030" />

<img width="1484" height="836" alt="image" src="https://github.com/user-attachments/assets/8a11d0c2-52a5-4e40-b451-b2bc602b8809" />

<img width="1192" height="689" alt="image" src="https://github.com/user-attachments/assets/9e788ef5-af80-4432-8a51-52da52a9cf63" />
