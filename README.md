# EECS 370 — Computer Organization

C and LC-2K assembly: assemblers, linker, instruction simulation, pipelining, and cache simulation.

Coursework portfolio of **Han Sun ([shufubianyibo](https://github.com/shufubianyibo))**. This repository collects source snapshots from local projects so they can be browsed directly from my personal GitHub profile.

## Main projects

### [LC-2K cache simulator — Project 4](starter_4/)

A configurable cache layer for LC-2K memory accesses, with adjustable block size, set count, and associativity. The implementation tracks valid and dirty blocks, uses LRU replacement, fills blocks on misses, and writes dirty victims back to memory. Assembly test cases target alignment, conflicts, replacement order, and write-back behavior.

Read [`cache.c`](starter_4/cache.c) alongside the `.as` tests. This component links against the course simulator interface; the Makefile identifies the external object dependency required to run it.

### [Cycle-by-cycle pipeline simulator — Project 3](starter_3/)

A C implementation of an LC-2K instruction pipeline with explicit stage registers and separate current/next machine states. It models instruction movement through the pipeline, data forwarding, load-use stalls, and branch handling, making the timing consequences of instruction dependencies visible in the execution trace.

The implementation is [`simulator.c`](starter_3/simulator.c); the surrounding assembly programs exercise dependent instructions and control flow.

### [Assembler and linker toolchain — Projects 1 and 2](starter_2a/)

The [initial assembler](starter_1a/) and [instruction simulator](starter_1s/) establish instruction encoding and execution. The [object-file assembler](starter_2a/assembler.c) and [linker](starter_2l/linker.c) extend the workflow with symbol tables, relocation records, and references across separately assembled files. Test inputs include undefined symbols, duplicate labels, offsets, and multi-file references.

## All projects

| Project | Source files |
| --- | ---: |
| [starter_1a](starter_1a/) | 21 |
| [starter_1s](starter_1s/) | 12 |
| [starter_2a](starter_2a/) | 21 |
| [starter_2l](starter_2l/) | 20 |
| [starter_3](starter_3/) | 17 |
| [starter_4](starter_4/) | 21 |
| [assembly-exercises](assembly-exercises/) | 2 |

## Running the projects

Each directory is independent. Read its README, build file, tests, and `PORTFOLIO.md` before running. Original course infrastructure, data, or toolchains may be required. This publication checks source packaging; it does not certify that every historical project builds on a current machine.

## Attribution

These are individual and collaborative course projects. Course starter code, supplied tests, and third-party dependencies are included where needed. Original credits and licenses are retained. Each `PORTFOLIO.md` links to the source repository and records its base commit and commit authors when available. Source-file counts include tests and scaffolding and are not a measure of individually authored work. Snapshots were collected on 2026-11-17; original working directories were left unchanged.
