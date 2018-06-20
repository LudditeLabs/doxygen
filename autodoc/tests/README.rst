Autodoc tests
=============

This directory contains autodoc related tests.

Tests consist of two parts:

* c++ tests: implemented with Google Mock framework.
* python tests: implemented with ``pytest`` framework.

Layout
------

::

    CMakeLists-gtest.txt.in     - Template to integrate Google Mock.
    main.cpp                    - Custom Google Mock entry point.
    utils.cpp                   - Test utils.

    py/                         - Python tests.
    test_py.cpp                 - Test case to run python tests.
    visitormodule.cpp           - Python module provides access to autodoc.

    test_*.cpp                  - c++ tests.


C++ tests
---------

C++ tests depends on Google Mock framework which is downloads and compiles
by the ``CMakeLists-gtest.txt.in``.

Doxygen part is initialized and deinitialized by ``DoxygenEnv`` implemented
in the ``main.cpp``.


Python tests
------------

Python tests are implemented with ``pytest`` framework and runs autodoc parts
via extension ``visitor`` which is implemented in the ``visitormodule.cpp``.

Make sure ``pytest`` is installed for your python interpreter::

    $ wget https://bootstrap.pypa.io/get-pip.py
    $ sudo python3.5dm get-pip.py
    $ sudo python3.5dm -m pip install pytest
