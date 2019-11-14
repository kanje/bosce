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
