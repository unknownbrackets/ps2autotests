Description
===========

A set of test programs run on a PS2, with accompanying results.

Readers of these tests can become more familiar with how to write homebrew and
other software, and can become more familiar with the operation of the hardware.

These tests can also be used, in an automated fashion, for software intended to
mimic the behavior of PS2 hardware.


Running tests on hardware
=========================

To execute a test on hardware, and to develop tests, the following is required:

 * A PS2 capable of running homebrew
 * ps2link.elf
 * Network connectivity to your PS2
 * A working ps2sdk setup

Individual tests can be run using a command like this:

    ./gentest.py cpu/ee/alu

This will automatically send the test to ps2link, and save its results to an
expected file.


Creating tests
==============

Tests can be created quite simply by including a Makefile template.

Pull requests are accepted.  Please take care not to submit copyrighted material
without proper licensing and permission.


Automating tests
================

Tests will either end in `.elf` or `.irx`.  The latter are tests meant to be
run on the IOP (R3000) processor, rather than the EE (R5900).

Tests will then send data to stdout.  A test that has run accurately will
produce the same data as contained in the `.expected` file.  On Windows, take
care to normalize newlines when making the comparison.


Acknowledgements
================

PS2 is a registered trademark of Sony Computer Entertainment Inc.

These tests would not be possible without the work done in [ps2sdk] (https://github.com/ps2dev/ps2sdk).
