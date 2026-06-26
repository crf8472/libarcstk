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
  - API change: ChecksumSet::length() will return AudioSize
  - API change: ChecksumSet::set_length() will accept only AudioSize
  - API change: ChecksumSet::get() will return std::pair with a success flag
  - API change: Checksum::empty() is replaced by Checksum::zero()
  - API change: ARId::track_count() is renamed to ARId::total_tracks()
  - API change: make_arid() will produce ARId instead of std::unique_ptr<ARId>
  - API change: make_toc() will produce ToC instead of std::unique_ptr<ToC>
  - API change: new DBAR parser events: start_triplets + end_triplets
  - API change: remove is_valid_arcs() and is_valid_confidence()

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

