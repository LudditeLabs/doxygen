Autodoc tests
=============

This directory contains autodoc related tests.

Tests consist of two parts:

* c++ tests: implemented with Google Mock framework.
* python tests: implemented with ``pytest`` framework.

Layout
------

::

    pymodule/                   - Autodoc python module for pyhton tests.
    pytests/                    - Python tests.
    CMakeLists-gtest.txt.in     - Template to integrate Google Mock.
    main.cpp                    - Custom Google Mock entry point.
    utils.cpp                   - Test utils.
    test_py.cpp                 - Test case to run python tests.
    test_*.cpp                  - C++ tests.

C++ tests
---------

C++ tests depends on Google Mock framework which is downloads and compiles
by the ``CMakeLists-gtest.txt.in``.

Doxygen part is initialized and deinitialized by ``DoxygenEnv`` implemented
in the ``main.cpp``.

Python tests
------------

Python tests are implemented with ``pytest`` framework and runs autodoc parts
via extension ``visitor`` which is implemented in the ``pymodule/``.

Make sure ``pytest`` is installed for your python interpreter::

    $ wget https://bootstrap.pypa.io/get-pip.py
    $ sudo python3.5dm get-pip.py
    $ sudo python3.5dm -m pip install pytest
