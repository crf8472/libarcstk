# Legacy Modules for finding dependencies

The find modules in this directory are only required for finding the
dependencies of the examples on plattforms for which the github workflow
build-examples.yml runs.

Although libsndfile is packaged with appropriate cmake files, those files are
missing in the ubuntu packages. Therefore FindSndFile.cmake exists which makes
the finding strategy equivalent on all required platforms.

However, the Modules are legacy from a cmake point of view and will therefore be
removed once cmake can safely find libsndfile and libcue without it.

