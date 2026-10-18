# SPSC tests

This directory contains the unit tests based on GTest.

The files with names ending with `_test` contains the actual tests or instantiate the parametrized tests, the rests are the test fixtures and helpers. In particular the `vN_queue_test.cpp` files instantiates the parametrized test suite for each version of the queue - the files separated to improve the compilation time of the tests. Most of the fixtures are defined in `queue_test_common.hpp`.
