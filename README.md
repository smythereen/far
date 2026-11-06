# far

**File Access Routines** — a C library for path/file I/O, text parsing,
directory walks, and simple TCP/UDP helpers, built on `sys`.

**Requires the [base](https://github.com/smythereen/base) repository** to be
installed first (project root with `BASE`, `rules.mak`, `targets.mak`, and the
`repo` unit). Clone base, then fetch this unit with `repo add far` or via the
project starter list.

## What it provides

Public API: `far.h` (installed as `~/cmn/far.h`). Library: `~/lib/libfar.a`.

### Files and paths

- `File_Open` / `FILE_Open` — create path as needed and open fd or `FILE*`.
- `File_Close` / `FILE_Close`, `Is_File_Open`, `File_Statistics`, type helpers
  (`File_Is_Directory`, `File_Is_FIFO`, `File_Type`).
- `File_Read` / `File_Write`, `File_Size` / `FILE_Size`.
- `File_Link`, `Verify_Path`, `Access_Dir` / `Build_Dir`, `Delete` (recursive),
  `ScanDir`.
- `File_To_FILE` / `FILE_To_File` — convert between fd and stream.

### Text and records

- Line readers: `Read_Line`, `File_Gets` / `FILE_Gets`, `Get_All_Lines` /
  `Read_All_Lines`, and write helpers `Write_Line` / `Write_LineAP`.
- Field/word splitting: `Get_Fields` / `Read_Fields`, `Get_Words` /
  `Read_Words`, `Find_Word_Values`.
- Fixed records: `Read_Record` / `Write_Record`.
- Comments: `Is_Comment`, `Set_Comment_Characters`.
- In-buffer argv builders: `Declare_Vector_Buffer`, `Add_To_Vector`,
  `Vector_Buffer`, `Sort_List`.

### Sockets (lightweight)

- `Connect2Host`, `Listen4Host`, `InitUDP2Host`, plus `Accept_*` globals.

### Misc

- `File_Test_For_Read_Data` (select with timeout), `CreateKillScript` /
  `AddToKillScript`.

Internal table: `File_Table[MAX_FD]` tracks open descriptors (`fardata.c`).

## Makefile / locations

```
far/makefile          # LIBRARIES := -lsys
far/UNIT.deps         # base 1; require sys
far/far.h farlocal.h  # public vs internal header
```

```sh
cd ~/far && make header && make library
# needs ~/lib/libsys.a and ~/cmn/common.h already installed
```

## System prerequisites

- GNU make, gcc, Linux sockets/`select`, POSIX file APIs.
- Direct link dependency on **sys** (`-lsys`).

## Direct public-unit dependencies

| Unit | Kind | Evidence |
|------|------|----------|
| [sys](https://github.com/smythereen/sys) | headers + `libsys.a` | `#include <common.h>` in `far.h`; makefile `LIBRARIES := -lsys`; `UNIT.deps` `require sys`; `nm libfar.a` undefineds `SysPrt`, `LocalName`, `Trim`, `Add_Line`, `EventLog`, `FatalError`, `File_Path`, … resolved only in `libsys.a` |

Linking `-lfar` without `-lsys` fails (undefined `SysPrt` / `LocalName` /
`TraceLevel` / `TracePointer`). Removing `libsys.a` while still passing
`-lsys` fails with `cannot find -lsys`.

## Indirect dependencies

```
far ──► sys ──(headers)──► atomic
```

- **atomic** — via sys (`sysprt.c` includes `atomic_32.h`). Far itself does
  not include atomic headers.
- No other public unit libraries are required for `libfar.a`.

## Usage examples (adapted from far test programs, verified)

Word split + path check:

```c
#include <fcntl.h>
#include <far.h>
#include <stdio.h>

int main(void) {
	char buf[256];
	char **w;
	LocalName = "use_far";
	snprintf(buf, sizeof buf, "one two three");
	w = Get_Words(" ", buf, sizeof buf);
	for (; w && *w; w++) printf("[%s]", *w);
	printf("\nverify=/tmp -> %d\n", Verify_Path("/tmp", O_RDONLY, 0));
	return 0;
}
```

```sh
gcc -o use_far use_far.c -I$HOME/cmn -L$HOME/lib -lfar -lsys
./use_far
# [one][two][three]
# verify=/tmp -> 1
```

CSV-ish “last word group” reader (from the historical `lastword` test):

```c
#include <far.h>
#include <stdio.h>
#include <string.h>

static void print_it(char **x) {
	int i;
	for (i = 0; x[i]; i++) printf(i ? ",%s" : "%s", x[i]);
	putchar('\n');
}

int main(int ac, char *av[]) {
	char **w, **l = 0, *buf, buf1[4096], buf2[4096];
	if (ac != 2) return 1;
	LocalName = av[0];
	buf = buf1;
	while ((w = Read_Words(av[1], ",", buf, sizeof buf1)) != 0) {
		if (l && strcmp(w[0], l[0]) != 0) print_it(l);
		buf = (buf == buf1) ? buf2 : buf1;
		l = w;
	}
	if (l) print_it(l);
	return 0;
}
```

```sh
printf 'a,1\na,2\nb,3\n' > words.txt
gcc -o lastword lastword.c -I$HOME/cmn -L$HOME/lib -lfar -lsys
./lastword words.txt
# a,2
# b,3
```

## Version

Release tag: `v0.2.0` (documentation update; version number unchanged).

## Using this unit from C++

This unit can be used from **C and C++**. Keep the unit's `.c` implementation
files compiled as C with `gcc`/`CC`; compile your C++ client with `g++`/`CXX`
and use `g++` for the final mixed-language link. There is no need to rebuild
the C library as C++.

Base's shared rules recognize C++ product sources named `.cc` and compile
them with `CXX` and `CXXFLAGS` (for example `make CXX=g++`). `CPP` names
the preprocessor, not the C++ compiler. The commands below assume the
headers and libraries have already been installed into `$HOME/cmn` and
`$HOME/lib`. GNU C++17 on Linux x86-64 is the example target; GNU extensions
in the shared types/headers are intentional.

`far.h` and its nested `common.h` already guard their declarations with
`extern "C"`; include the public header directly, not `farlocal.h`.
Many APIs retain `char *` parameters, so pass writable arrays for paths,
buffers and format strings instead of C++ string literals. `std::string`
objects are not C strings or FAR records; manage their storage/lifetime in
your client and respect each function's mutation contract. The header's
legacy inline tracing calls can emit string-literal-to-`char *` warnings;
base currently uses `-Wno-write-strings` in `CXXFLAGS` for this legacy API.

Save as `use_far.cc`:

```cpp
#include <far.h>
int main() {
    char text[] = "# comment";
    return Is_Comment(text) ? 0 : 1;
}
```

```sh
g++ -std=gnu++17 -I"$HOME/cmn" use_far.cc -L"$HOME/lib" -lfar -lsys -o use_far
./use_far
```

The C++ example above was compiled, linked and run on Linux x86-64 using
Clang 22.1.8 via `zig c++ -std=gnu++17 -nostdlib++`, with GCC 14.2-built C
objects where needed. It uses no C++ standard-library runtime. The `g++`
command is the GNU-toolchain form; that driver was not available for this
smoke check. This is not an exhaustive API or concurrency test.

## License

The original files are offered under `Apache-2.0 OR GPL-2.0-or-later`;
you choose either license. See `LICENSE` and `LICENSES/`.

Copyright notices are retained.
