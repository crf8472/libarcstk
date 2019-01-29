# Versioning Policy


## Semantic versioning

Libarcs release versions consist of three non-negative numbers and an optional
suffix.

This versioning policy intends to obey the requirements of
[semantic versioning][1] while introducing the following fortifications:

- Development releases will always have a suffixed
- Stable releases will never have a suffix
- Suffixes are of the form ``-alpha.X``, ``-beta.X`` or ``-rc.X``
  with X being a non-negative unprefixed integer. This entails that
  for example ``0.1.0-alpha.1`` is a lower version than ``0.1.0-rc.1`` which is
  lower than ``0.1.0``.


## Version scheme

- The major version number indicates major API changes or significant changes on
  characteristic features.
- For any major version greater than 0, between any two releases with the same
  major version, API compatibility is kept. This assertion is not guaranteed for
  releases with major version 0.
- The minor version number starts on 0 for every new major version and counts
  the releases for this major version.
- The minor version changes when introducing minor new features - which can also
  include extending the API - or significant technical changes such as
  introducing availability for new platform, changing runtime dependencies or
  replacing some internal implementation with a completely different one.
- The patch level changes on any new release for the same minor version. It
  usually indicates bugfixes or other changes not characteristic for the
  behaviour of the minor version.
- Each development release version will furthermore have a version suffix unique
  for the combination of major, minor and patch version. This suffix qualifies
  the development release version as either alpha, beta or release candidate
  along with a non-prefixed counter starting with 1.
- It is not guaranteed that all suffixes will have been used in the development
  release history of every stable release.


## API changes before release 1.0.0

- Consider the API under development. This indicated by major version 0.
- While major version is 0, the API may change significantly, also between minor
  versions. Do not expect stability.
- While major version is 0, the API may become backwards incompatible at any
  time.
- The first stable release will have version 1.0.0. From then on, the guarantees
  above will hold.


## Development stages

A release for a new major or minor version starts in stage ``alpha`` and will
reach stage ``beta`` as soon as all tasks for the upcoming stable release are
implemented.

It reaches ``release candidate`` when no more significant changes to the last
beta version had to be made to correct bugs.

That means, stage ``alpha`` indicates that some implementations for the next
stable release are available, ``beta`` means feature completeness in regard of
the next stable release and ``release candidate`` means that all known bugs from
beta stage have been fixed.

Of course, stages may be skipped if it seems reasonable. However, any new major
release will be preceded by a release candidate.


## Current stage

Libarcs is in an early alpha development stage with version 0.1.0-alpha.1 being
the first development release of its history.

Well, it should actually be considered ``pre-alpha`` but I refuse to break the
equivalence of lexicographical and historical ordering of version suffixes by
introducing a tag like ``-pre-alpha``. Any filemanager would list such a tag
between the ``beta`` and ``rc`` releases, messing up the historical order of
development releases.

Libarcs will reach beta stage as soon as all features currently planned are
implemented and test coverage is ... well, good ... enough ... sort of.

[1]: https://semver.org

