# Versioning

We would like to follow the rules of the [Semantic Versioning](http://semver.org). 

It is pseudo-auto implemented as follows in InstrumentAll.

## Core
 * In the main branches (not in forks), [VERSION](/VERSION) is defined, e.g. *2.0-rc.1*
 * At compile time, with the help of CMake, a version number is created by using the content of the VERSION file, and appending two numbers, corresponding to:
     1. the count of commits since the last VERSION modification
     2. the count of local builds since the last commit

Then, we get a generated version number that would be (based on our example) *2.0-rc.1.5.3* where they were 5 commits since VERSION modifications, and already 3 builds while being in the current commit. 

## Forks
The core version is displayed, and the fork version  is added, generated the same way that the core version is: `<`[fork_VERSION](/fork_VERSION)`>.<count of commits>.<count of builds> `