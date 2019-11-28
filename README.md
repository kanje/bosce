# bosce
Bosce, or **bo**ost::**s**tate**c**hart **e**xtractor, is a command line tool to extract
information about <a href="https://www.boost.org/doc/libs/1_71_0/libs/statechart/doc/tutorial.html">
boost::statechart</a>s from an arbitrary binary with debug symbols and transform it to a
user-friendly form, e.g. UML diagrams.

## Demo
First, we need a binary with debug symbols to work on.

    $ g++ demo.cpp -o demo

Then, we get a list of state machine in this binary.

    $ bosce demo -l
    Available state-machines:
        StopWatch

Now, we generate a `StopWatch` state-machine representation in a <a href="http://plantuml.com/state-diagram">
PlantUML</a> format.

    $ bosce demo -s StopWatch > uml.pu

Finally, we use PlantUML to generate a PNG file and open it.

    $ plantuml uml.pu
    $ xdg-open uml.png

## Build Instructions
Bosce depends on `Qt5Core` and `boost::program_options` and requires a C++14 compatible compiler.

First, clone the repository.

    $ git clone https://github.com/kanje/bosce.git
    Cloning into 'bosce'...
    remote: Enumerating objects: 48, done.
    remote: Total 48 (delta 0), reused 0 (delta 0), pack-reused 48
    Unpacking objects: 100% (48/48), done.

Then, create a build directory and run `cmake` to configure the project.

    $ mkdir bosce-build
    $ cd bosce-build
    $ cmake -DCMAKE_BUILD_TYPE=Release ../bosce
    -- The CXX compiler identification is GNU 9.2.1
    ...
    -- Configuring done
    -- Generating done
    -- Build files have been written to: /.../bosce-build

Finally, build bosce.

    $ make -j10
    Scanning dependencies of target bosce
    [ 12%] Building CXX object CMakeFiles/bosce.dir/src/ScModel.cpp.o
    ...
    [100%] Built target bosce
