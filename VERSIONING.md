# Versioning Policy


## Semantic versioning

Libarcstk release versions consist of three non-negative numbers and an optional
suffix.

This versioning policy intends to obey the requirements of
[semantic versioning][1] while introducing the following fortifications:

- Development releases will always have a "pre-release suffix"
- Stable releases will never have a suffix
- Suffixes are of the form ``-alpha.X``, ``-beta.X`` or ``-rc.X``
  with X being a positive unprefixed integer. This entails that
  for example ``0.1.0-alpha.1`` is a lower version than ``0.1.0-rc.1`` which is
  lower than ``0.1.0``.


## Development releases

- Suffix ``alpha`` means that some implementations for the next stable release
  are available but may contain bugs of any severity.
- Suffix ``beta`` means feature completeness in regard of the next stable
  release and tests for all new features have been provided.
- Suffix ``rc`` (release candidate) means that all known bugs from beta stage
  have been fixed.
- Of course, stages may be skipped if it seems reasonable. It is not guaranteed
  that all suffixes will have been used in the development release history of
  every stable release.


## API changes before release 1.0.0 (confer [SemVer points 4 and 5][2])

- Consider the API under development. This is indicated by still having major
  version 0.
- While major version is 0, the API may change significantly, at any time in any
  way, also between minor versions. Do not expect any stability.
- While major version is 0, the API may become backwards incompatible at any
  time.
- The first stable release will have version 1.0.0. From then on, as
  [SemVer point 8][3] defines, the guarantees for backwards compatibility will
  hold.


## Current stage

Libarcstk is in an early alpha development stage with version 0.1.0-alpha.1 being
the first development release of its history.

Well, it should actually be considered ``pre-alpha`` since it may in fact be
broken in every way you can think of, but I refuse to break the equivalence of
lexicographical and historical ordering of pre-release suffixes by introducing a
tag like ``-pre-alpha``. Any filemanager would list such a tag between the
``beta`` and ``rc`` releases, messing up the historical order of development
releases.

Libarcstk will reach beta stage as soon as all features currently planned are
implemented and test coverage is ... well, good ... enough ... sort of.

[1]: https://semver.org
[2]: https://semver.org#spec-item-4
[3]: https://semver.org#spec-item-8

