# Versioning Policy												{#versioning}


# Semantic versioning

Libarcs release versions consist of three non-negative numbers and an optional
suffix.

The major version number indicates major API changes or significant changes on
characteristic features.

For any major version greater than 0, between any two releases with the same
major version, API compatibility is kept. This assertion is not guaranteed for
releases with major version 0.

The minor version number starts on 0 for every new major version and counts the
releases for this major version.

The minor version changes when introducing minor new features - which can also
include extending the API - or significant technical changes such as introducing
availability for new platform, changing runtime dependencies or replacing some
internal implementation with a completely different one.

The patch level changes on any new release for the same minor version. It
usually indicates bugfixes or other changes not characteristic for the behaviour
of the minor version.

Each development release version will furthermore have a version suffix unique
for the combination of major, minor and patch version. This suffix qualifies the
development release version as either alpha, beta or release candidate along
with a non-prefixed counter starting with 1.

This versioning policy intends to obey the requirements of semantic versioning
(cf. [here][1]) while introducing the following fortifications:

- Stable releases will never have a suffix while development releases will
  always be suffixed.
- The only suffices used will be ``-alpha.<X>``, ``-beta.<X>`` or ``-rc.<X>``
  with X being a non-prefixed integer counter starting with 1. This entails that
  for example ``0.1.0-alpha.1`` is a lower version than ``0.1.0-rc.1`` which is
  lower than ``0.1.0``.

It is not guaranteed that all suffixes will have been used in the development
release history of every stable release.


# API changes before release 1.0.0

Libarcs is currently in an early alpha development stage with version
0.1.0-alpha.0 being the first development release of its history. (Well, it
should actually be considered ``pre-alpha`` but I refuse to break the
equivalence of lexicographical and historical ordering of version suffixes by
introducing a tag like ``-pre-alpha`` that any filemanager would list between
the ``beta`` and ``rc`` releases, messing up the historical order of development
releases.)

Libarcs will reach beta stage as soon as all features currently planned are
implemented.

- Any release with a major version 0 is considered experimental and unstable,
  also when non-suffixed.
- This entails that the upcoming release 0.1.0 will be an unstable release
  insofar as the API may change when upgrading to 0.2.0 (in fact when upgrading
  to 0.2.0-alpha.1). API changes breaking backwards compatibility without
  changing the major version number will be excluded for future major versions
  greater than 0.
- The major version will remain 0 while the API of libarcs is expected to change
  significantly between two releases and libarcs is therefore considered
  unstable.
- The first stable release will have version 1.0.0.


# Development stages

A release for a new major or minor version starts in stage ``alpha`` and will
reach stage ``beta`` as soon as all tasks for the upcoming stable release are
implemented.

It reaches ``release candidate`` when no more significant changes to the last
beta version had to be made to correct bugs.

That means, stage ``alpha`` indicates that some implementations for the next
stable release are available, ``beta`` means feature completeness in regard of
the next stable release and ``release candidate`` means that all known bugs from
beta stage have been fixed.

For major and minor versions, the alpha stage may as well be skipped if all
requirements for a beta release are fulfilled with a new commit.

For patch versions, the stages ``alpha`` as well as ``beta`` may be skipped.

A release candidate for a new major or minor release is only released after a
beta version, meaning we do not intend to skip beta releases for major or minor
version upgrades.

[1]: https://semver.org

