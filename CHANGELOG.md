# Changelog

All notable changes to this project will be documented in this file.

## [0.9.0]

### Added
  - API: provide class CalculationSet for multi-algorithm calculations
  - API: provide include-all header libarcstk.hpp
  - API: Provide class CheckingDBARBuilder
  - API: DBAR and DBARBlock have now empty()
  - API: DBAR and DBARBlock are now reverse iterable (issues #9, #10)
  - API: ToC and ARId have now operator <<
  - API: manage global request url via ACCURATERIP
  - API: ARId::bool() provided, TRUE iff ARId is empty()
  - API: added some member swap()s, equals()s etc.
  - Build: Add build switch WITH_EXAMPLES (ON pulls dependencies!)
  - Portability: DBAR parsing has experimental support for big endian plattforms

### Changed
  - API: no more type erasure on iterators, use template function instead
  - API: Use Updater<A> instead of Calculation with Algorithm A
  - API: Calculation and Algorithm interfaces are no longer updateable
  - API: No converting constructors in public API, (Settings, Checksum,...)
  - API: checksum::type_name(checksum::type) renamed to checksum::name()
  - API: ChecksumSet::length() will return AudioSize
  - API: ChecksumSet::set_length() will accept only AudioSize
  - API: ChecksumSet::get() will return std::pair with a success flag
  - API: Checksum::empty() is replaced by Checksum::zero()
  - API: ARId::track_count() is renamed to ARId::total_tracks()
  - API: make_arid() will produce ARId instead of std::unique_ptr<ARId>
  - API: make_toc() will produce ToC instead of std::unique_ptr<ToC>
  - API: new DBAR parser events: start_triplets + end_triplets
  - API: remove is_valid_arcs() and is_valid_confidence()

### Deprecated
  - nothing

### Removed
  - API: SampleInputIterator was removed in favor of template functions
  - Build: Remove deprecated build switch USE_MCSS

### Fixed
  - Hidden out-of-bounds in get_partitioning()
  - Stream handling of parse_dbar_stream() fixed and improved
  - Postfix operator ++ for DBAR and DBARBlock does now work as expected
  - parse_dbar_file() does now a single read of file not bigger than 8 MiB
  - Respect 3 namespaces in documentation that were missing before

### Security
  - Added commit triggered workflows for build & test, ASAN/UBSAN and clang-tidy


## [0.3.1-alpha.1] - 2025-12-30

### Changed
  - Build: Deprecate configure switches -DUSE_* in favour of -DUSE_DOC_TOOL=*
  - Build: Default-deactivate LaTeX on target 'doc'
  - Build: Improve doxygen postprocessing, fix some annoyances in output
  - Build: Some fixes in build management
  - Build: Add experimental support for doxygen-awesome-css
  - Build: Add GraphViz options for debugging cmake targets
  - Documentation: Start to write some HowTos (work-in-progress)
  - Documentation: Add BUILD.md to doxygen documentation

### Fixed
  - Minor cleaning in code


## [0.3.0-alpha.1] - 2025-03-02

### Changed
  - Compileable as C++17. Support for C++14 is dropped in main branch. (Use
    branch 0.2 if you require C++14.)
  - Client code using version 0.2* will not be compileable anymore with version
    0.3 and above.
  - API: Rewrite of all classes and functions in modules 'id' and 'calc'.
  - API: Interfaces of classes Calculation, TOC and AudioSize have changed
    fundamentally.
  - API: TOC is replaced by ToC which does not hold lengths at the moment.
  - Class Checksums is a mere typedef.
  - The logs will be make use of more log levels and be more compact in general.

### Removed
  - Class CalcContext is removed.
  - Most of the template magic in calculate.hpp will be removed as well as the
    templated versions of make_toc().
  - Public headers in arcstk/details and the contained classes ARIdBuilder and
    TOCBuilder
  - Forced validation of ToC objects is dropped. Validation of toc data is
    completely rewritten and fully optional.


## [0.2.1-alpha.1] - 2022-02-22

### Fixed
  - Fix pkgconfig file
  - Fix cmake warning about wrong usage of PRE_BUILD when configuring
  - Minor fixes when using include-what-you-use
  - Ignore Session.vim


## [0.2.0-alpha.1] - 2024-04-04

### Changed
  - API breaking: Module Verify replaces module 'match'
  - API breaking: Module DBAR replaces module 'parse'
  - Use branch "0.1" for API 0.1.x

### Deprecated
  - API 0.1.x: unmaintained, do not expect further development


## [0.1.1-beta.2] - 2023-08-01

### Changed
  - Minor changes in logging when parsing an ARResponse
  - Update Catch2 to 3.4.0
  - Determine minimum CMake version to 3.6


## [0.1.1-beta.1] - 2023-03-12

### Changed
  - InvalidMetadataException and NonstandardMetadataException are now
    runtime_error

### Fixed
  - Bugfix: Functions involved while updating the audiosize are no longer
    noexcept
  - Add build config to be cloned as a submodule
  - Several bugfixes


## [0.1.0] - 2023-01-07

### Removed
  - Remove confusing messages during build
  - Remove unnecessary output on example code

### Fixed
  - Better support for compiling as a submodule


## [0.1.0-rc.4] - 2022-10-02

### Changed
  - Adjust warnings flags for sources and tests
  - Upgrade to Catch2 v3.1.0

### Fixed
  - Force unsigned hex literals in version test to avoid compile warnings
  - Compile warnings for calculate.hpp and samples.hpp


## [0.1.0-rc.3] - 2022-01-03

### Changed
  - API: Move version info from .hpp to .cpp file
  - Recreate version.cpp on every build

### Fixed
  - Link examples to locally compiled SO when building
  - Build: recreate documentation output directory after removing
  - Build: remove overflow warnings when compiling with g++11


## [0.1.0-rc.2] - 2021-03-11

### Changed
  - API: made CDDA a struct with static members, so CDDA.x becomes CDDA::x
  - API: AudioSize::set_pcm_byte_count() is renamed to
    AudioSize::set_pcm_total_bytes()
  - API: SampleSequence::wrap() is renamed to SampleSequence::wrap_int_buffer()
  - API: SampleSequence::wrap_bytes() is renamed to
    SampleSequence::wrap_byte_buffer()

### Fixed
  - Internal fixes
  - Update documentation


## [0.1.0-rc.1] - 2020-05-03

### Changed
  - API improvements (little changes, but too many to enumerate)
  - Add operator == and swap() to many more classes
  - Add more unit tests
  - Coding style: prefer braced initializations
  - Represent amounts of samples or frames by signed types

### Removed
  - Remove broken static build option (#5)
  - Remove inheritance from ChecksumSet and hide details consequently (#3)

### Fixed
  - ARStreamParser no longer throws an exception on errors if an ErrorHandler is
    present
  - Remove "type leaks" by iterators in ARBlock and ARRresponse (#7)


## [0.1.0-beta.2] - 2020-02-23

### Added
  - Improve documentation (more work needed)

### Changed
  - API: class names, function signatures
  - API: tried to make things more sound and complete (e.g. operators)
  - API: use custom types for numeric ranges (sample_type, lba_count)
  - Templatized builders and validators for TOC and ARId
  - Major reorganizations in cmake files
  - "Minimized" Doxyfiles


## [0.1.0-beta.1] - 2019-11-03

### Added
  - Specify compilation warnings for clang and g++

### Changed
  - Reorganize compilation settings
  - Adjust some libarcstk warning messages

### Fixed
  - Minor fixes


## [0.1.0-alpha.3] - 2019-06-02

### Added
  - Install cmake config files (making release a cmake package)

### Removed
  - Remove duplicate output: Do not log exceptions, just throw them

### Fixed
  - Many bugfixes in cmake's install instructions
  - Fix building the m.css based documentation
  - Use CHECK instead of REQUIRE in all tests


## [0.1.0-alpha.2] - 2019-05-01

### Fixed
  - Installation of public headers


## [0.1.0-alpha.1] - 2019-04-27

### Added
  - Initial pre-release

