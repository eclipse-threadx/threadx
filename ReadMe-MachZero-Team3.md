# MACHZERO — ThreadX Issue #585

**Repository:** [eclipse-threadx/threadx](https://github.com/eclipse-threadx/threadx)  
**Issue:** [#585 — Module converter utilities misparse ELF files when built for a 64-bit host](https://github.com/eclipse-threadx/threadx/issues/585)  

## ROUND 1
## 1. ISSUE

In the original converter, ELF32 fields were represented with unsigned long, whose size can differ by host data model (commonly 4 bytes on ILP32 and 8 bytes on LP64). ELF32 itself defines fixed-width fields; the host-side C structure did not reliably match those widths.

The raw file is directly copied onto C structures without accounting for padding, which leads to mismatch in the file layout (different offsets on different systems).

Since this does not raise an error, garbage data is propagated upstream which might cause undefined behaviour.

The failure occurs on the developer host system while interpreting the ELF input. It is not a scheduler defect and does not require hardware to reproduce the structure-size mismatch.

## 2. ARCHITECTURE

The data flow is:

```text
ELF32 module file
      ↓
Host converter parses ELF metadata and finds code sections
      ↓
module_to_binary writes binary   OR   module_to_c_array writes C source code
      ↓
Firmware packages the generated module bytes
      ↓
ThreadX module manager loads the module on the target
```

## ROUND 2
## 3. PLANNED SOLUTION

We planned to eliminate compiler dependency by passing ELF file as raw bytes to the system instead of copying into the C structures. 
We wanted to validate the ELF fields by checking 5
a) Magic Bytes - Valid ELF file or not (0x7F E L F)
b) Class - If its 32 or 64 bit ELF
c) Data Encoding - Little/ big endian (the converters support ELF32 little-endian input; explicit decoding makes this independent of the host byte order)
d) Version - ELF Format 
e) Machine - Target architecture (planned validation; the current implementation decodes e_machine but does not reject files based on its value)

We planned to change the variable lengths by decoding using fixed width values (uint8_t, uint16_t, uint32_t) using <stdint.h> library (Given as a suggested fix by the maintainer) to ensure its the same across all systems.

Converter converts it to a binary or C array output as before. We are not modifying the core functionality of the project, just fixing the compatibility issue, making it host independent.

We wanted to test if the code functionality remained unchanged using a few test cases:
a) Valid cases (Perfect input, ELF with gaps)
b) Invalid (Class: ELF 32/64 bit, Data Encoding: Little/ Big Endian, Table offsets, whether its a valid ELF file or not)

## ROUND 3
## 4. SOLUTION IMPLEMENTATION
The issue recommended changes in module_to_binary.c and module_to_c_array.c 

Our changes are in the two converter files, a shared ELF32 decoding header, the module-manager CMake test registration, and a Python regression test file. Here are some changes we implemented:

a) ELF format fields changed to fixed width types
Unsigned char, int, long (host dependant) changed to uint8_t, uint16_t, uint32_t (host independent)

b) Checking structure size during compile time
Use typedef checking for size of ELF fields (ELF_HEADER: 52 bytes, ELF_SECTION_HEADER: 40 bytes, ELF_PROGRAM_HEADER : 32 bytes, ELF_SYMBOL_TABLE_ENTRY: 16 bytes)

c) Little endian decoding
Reconstruct values from individual bytes

d) ELF Header Decoder 
Validates ELF information fields (magic bytes, class, data encoding, identification version, ELF header version, and expected record sizes). It decodes the machine field but does not validate its value.

e) Section Header Processing
Instead of copying the layout of ELF file to C structures, we follow the planned solution

f) Program Header Allocation 
Using ELF32 (not dependent on host)

g) Improved Input Safety
Record input file size and validate every request (offset and length must remain within the file)

h) Updated Cleanup Routine
Release additional raw byte buffer introduced by our parsing changes


## CODE CHANGES — OLD AND UPDATED

The examples below show the relevant old pattern beside the implemented pattern. The old examples are from the original converter approach; the updated examples are shortened excerpts from the current code so the main difference is easy to see.

### 1. ELF fields: host-dependent widths changed to fixed-width types

**Old code pattern:**

```c
typedef struct ELF_HEADER_STRUCT
{
    unsigned char   id[16];
    unsigned short  file_type;
    unsigned short  machine_type;
    unsigned long   version;
    unsigned long   entry_address;
    unsigned long   program_header_offset;
    unsigned long   section_header_offset;
    /* More unsigned long and unsigned short fields. */
} ELF_HEADER;
```

**Updated code pattern in `module_converter_elf32.h`:**

```c
typedef struct ELF_HEADER_STRUCT
{
    uint8_t     elf_header_id_string[16];
    uint16_t    elf_header_file_type;
    uint16_t    elf_header_machinge_type;
    uint32_t    elf_header_version;
    uint32_t    elf_header_entry_address;
    uint32_t    elf_header_program_header_offset;
    uint32_t    elf_header_section_header_offset;
    uint32_t    elf_header_processor_flags;
    uint16_t    elf_header_size;
    uint16_t    elf_header_program_header_size;
    uint16_t    elf_header_program_header_entries;
    uint16_t    elf_header_section_header_size;
    uint16_t    elf_header_section_header_entries;
    uint16_t    elf_header_section_string_index;
} ELF_HEADER;
```

The same fixed-width approach is used for program headers, section headers, and symbol-table entries. The size checks are compile-time guards against accidentally changing those decoded-value structures:

```c
typedef char module_elf32_header_size_check[
    (sizeof(ELF_HEADER) == MODULE_ELF32_HEADER_SIZE) ? 1 : -1];
typedef char module_elf32_program_header_size_check[
    (sizeof(ELF_PROGRAM_HEADER) == MODULE_ELF32_PROGRAM_HEADER_SIZE) ? 1 : -1];
```

### 2. Raw file bytes: direct structure reads changed to explicit decoding

**Old code pattern:**

```c
/* Treat bytes read from the file as an ELF_HEADER object. */
elf_object_read(0, &header, sizeof(header));
```

This relied on the host structure's field sizes, padding, and byte order matching the on-disk ELF layout.

**Updated code pattern:**

```c
unsigned char raw_header[MODULE_ELF32_HEADER_SIZE];

if ((elf_object_read(0, raw_header, sizeof(raw_header)) != 0) ||
    (module_elf32_decode_header(raw_header, sizeof(raw_header), &header) != 0))
{
    /* Report invalid or unsupported ELF input and clean up. */
}
```

The decoder checks ELF magic, ELF32, little-endian encoding, identification version, header version, and expected structure sizes. It then reads each field from its specified byte offset. The `e_machine` value is decoded into the host record, but its value is not currently checked against a particular target architecture.

### 3. Little-endian numbers: host byte interpretation changed to explicit assembly

**Old code pattern:**

```c
/* A multi-byte field was obtained through the host structure layout. */
header.elf_header_entry_address = raw_header_field;
```

**Updated code pattern:**

```c
static uint32_t module_elf32_read_u32_le(const uint8_t *bytes)
{
    return(((uint32_t)bytes[0]) |
           ((uint32_t)bytes[1] << 8) |
           ((uint32_t)bytes[2] << 16) |
           ((uint32_t)bytes[3] << 24));
}

header->elf_header_entry_address = module_elf32_read_u32_le(&bytes[24]);
```

For bytes `78 56 34 12`, the function produces the numeric value `0x12345678`. This gives the same decoded value whether the converter itself runs on a little-endian or big-endian host.

### 4. Reads: unchecked seek/read changed to file-range checked reads

**Old code pattern:**

```c
fseek(source_file, offset, SEEK_SET);
for (i = 0; i < object_size; i++)
{
    alpha = fgetc(source_file);
    if (alpha == EOF)
        return(1);
    buffer[i] = (unsigned char)alpha;
}
```

**Updated code pattern in both converter files:**

```c
if ((source_file_size < 0) || (offset > (unsigned long)LONG_MAX) ||
    (offset > (unsigned long)source_file_size))
{
    return(1);
}

remaining_bytes = (unsigned long)source_file_size - offset;
if (object_size > remaining_bytes)
{
    return(1);
}

if (fseek(source_file, (long)offset, SEEK_SET) != 0)
{
    return(1);
}
```

The range check happens before seeking and reading. Subtracting the offset from the known file size first avoids relying on an unchecked `offset + length` calculation that could overflow. A zero-length read is accepted so empty tables can be handled without treating them as an error.

### 5. Section table: raw table overlay changed to entry-by-entry decode

**Old code pattern:**

```c
section_header = malloc(sizeof(ELF_SECTION_HEADER) * entry_count);
elf_object_read(table_offset, section_header,
                sizeof(ELF_SECTION_HEADER) * entry_count);
```

**Updated code pattern:**

```c
raw_section_header_table = malloc(raw_table_size);
elf_object_read(table_offset, raw_section_header_table, raw_table_size);

for (table_index = 0; table_index < entry_count; table_index++)
{
    raw_section_entry = raw_section_header_table +
                        (table_index * MODULE_ELF32_SECTION_HEADER_SIZE);
    module_elf32_decode_section_header(raw_section_entry,
                                       MODULE_ELF32_SECTION_HEADER_SIZE,
                                       &section_header[table_index]);
}
```

Each record is decoded from its ELF-defined 40-byte representation. The converter's later logic still receives `ELF_SECTION_HEADER` values and continues sorting/selecting sections and producing the same output format.

### 6. Output and cleanup: validate input before creating output

The converters now read and validate the input header before opening the output file. This prevents an invalid header from leaving a newly created empty output file. A shared cleanup routine closes any file that was opened and frees any allocated buffers; it safely handles pointers that are still `NULL`. The same parsing and cleanup approach is present in both `module_to_binary.c` and `module_to_c_array.c`.

## VALIDATION RUNS

### Valid 32-bit ELF conversion on two host builds

this link contains the output screenshots of initial (failure) and fixed code (success)
https://drive.google.com/drive/folders/1-717-yVBiul7tpWoPQbeeNmXZexUx02P?usp=sharing

this file contains the instructions to reproduce the test cases obtained above
https://docs.google.com/document/d/19U9uNcRZ8nxyY5OsScYqIZUblecu7yh-KiVe43n9f9Y/edit?usp=sharing

`test32.o` was the input where the old 64-bit converter failed while reading program headers. The updated converter succeeds, and its output matches the known-good output from the 32-bit build byte for byte.

```text
$ ./new64 test32.o a64.bin
-> exit=0  output: 103 bytes
$ ./new32 test32.o a32.bin
-> exit=0  output: 103 bytes
$ cmp a64.bin a32.bin && echo SAME
SAME
$ cmp a64.bin out32.bin && echo "SAME as the old known-good 32-bit output"
SAME as the old known-good 32-bit output
```

### Building the 32-bit ELF fixture

The fixture source contains a small `add` function. It is compiled as a 32-bit object file without linking, then used as the converter input.

```text
$ printf 'int add(int a,int b){return a+b;}\n' > test.c
$ gcc -m32 -c test.c -o test32.o
```

### Unsupported and malformed input rejection

The ELF64 object, text file, truncated ELF header, and empty file are each rejected. The converter exits with status 6 and does not create the output file. The text, truncated, and empty inputs are shorter than the 52-byte ELF32 header; the ELF64 input is rejected as an unsupported class.

```text
$ ./new64 test64.o c.bin
**** Error: invalid or unsupported ELF32 little-endian header ****
-> exit=6  output: not created bytes
$ ./new64 notelf.txt c.bin
**** Error: invalid or unsupported ELF32 little-endian header ****
-> exit=6  output: not created bytes
$ ./new64 trunc_hdr.elf c.bin
**** Error: invalid or unsupported ELF32 little-endian header ****
-> exit=6  output: not created bytes
$ ./new64 empty.elf c.bin
**** Error: invalid or unsupported ELF32 little-endian header ****
-> exit=6  output: not created bytes
```
The four files fail for different reasons but print the same message. test64.o fails because it’s ELF64. The other three (24, 30 and 0 bytes) are shorter than the 52-byte header and fail the size check before the decoder runs.



ENVIRONMENT SPECIFICATIONS: Linux MacBookPro 6.17.0-42-generic #42-Ubuntu SMP PREEMPT_DYNAMIC x86_64 x86_64 x86_64 GNU/Linux
