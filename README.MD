# Apollo
A C codebase that implements different data structures, 
algorithms, abstractions, in a STB Style Header.
Heavily relies on std c runtime, abstracts OS specific.

Idea inspired by @PixelRifts (GitHub) aka @VoxelRifts (Youtube).

### What is inside the toolbox
1. Common (used across other headers)
2. Simple Test Framework (STF)
3. Arena Allocator (arena)

### TODO:
* String Views
* Filesystem Interface
* Generic Linked List
* Cross Platform Sockets

### STF:
#### Simple Test Framework
A self contained testing framework
* Building the CLI:
```c
// tests/stf.c
#define STF_IMPL_CLI
#include "stf.h"
```
```sh
gcc ./tests/stf.c -o stf
```
* Building a test:
[Arena test written in STF](./tests/arena.c)
```sh
gcc ./tests/arena.c -shared -fPIC -o stf_arena.dll
```
* Using the CLI:
1. List all tests avilable in module
```sh
stf ./stf_arena.dll list
```
```txt  
[TEST]: "arena_init_test" -- inits an arena --
[TEST]: "arena_reserve_test" -- inits an arena, allocates a u64 and string of 64 chars --
[TEST]: "arena_remaining_test" -- inits an arena, reports it remaining space --
[TEST]: "arena_used_test" -- inits an arena, reports it used space --
[TEST]: "arena_reserve_no_space_left" -- tries to allocate but runs out of space --
[TEST]: "arena_reserve_null_arena" -- tries to allocate on an invalid arena --
```
2. Running a single test in a module:
```sh
stf ./stf_arena.dll arena_init_test
```
```txt
[INFO]: Executing case: "arena_init_test" (from ".\stf_arena.dll") -- inits an arena --
[INFO]: Finished case: "arena_init_test" (from ".\stf_arena.dll") -- succeeded (true) --
```
3. Running all tests in a module:
```sh
stf ./stf_arena.dll all
```
```txt
[INFO]: Executing case: "arena_init_test" (from ".\stf_arena.dll") -- inits an arena --
[INFO]: Finished case: "arena_init_test" (from ".\stf_arena.dll") -- succeeded (true) --

[INFO]: Executing case: "arena_reserve_test" (from ".\stf_arena.dll") -- inits an arena, allocates a u64 and string of 64 chars --
initializing an arena of 72 bytes
my_u64: 0000026ECD1BF7A0 -> 13907095937837018880
my_string: 0000026ECD1BF7A8 -> coffee is good_af
[INFO]: Finished case: "arena_reserve_test" (from ".\stf_arena.dll") -- succeeded (true) --

[INFO]: Executing case: "arena_remaining_test" (from ".\stf_arena.dll") -- inits an arena, reports it remaining space --
there are 16 bytes are left on the arena
allocating some memory
there are 4 bytes are left on the arena
[INFO]: Finished case: "arena_remaining_test" (from ".\stf_arena.dll") -- succeeded (true) --

[INFO]: Executing case: "arena_used_test" (from ".\stf_arena.dll") -- inits an arena, reports it used space --
there are 0 used bytes on the arena
allocating some memory
there are 12 used bytes on the arena
[INFO]: Finished case: "arena_used_test" (from ".\stf_arena.dll") -- succeeded (true) --

[INFO]: Executing case: "arena_reserve_no_space_left" (from ".\stf_arena.dll") -- tries to allocate but runs out of space --
[INFO]: Finished case: "arena_reserve_no_space_left" (from ".\stf_arena.dll") -- succeeded (true) --

[INFO]: Executing case: "arena_reserve_null_arena" (from ".\stf_arena.dll") -- tries to allocate on an invalid arena --
[INFO]: Finished case: "arena_reserve_null_arena" (from ".\stf_arena.dll") -- succeeded (true) --

[INFO]: Executed 6 tests, 6 succeeded (100%)
```