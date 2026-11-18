# ToDos


A list of ToDos, mainly for myself, do not expect consistency. Especially do not
expect that I really do anything of what I noted here any time soon.


## Features

- Multilanguage support. Would pull in dependency though and only affect
  logging + exceptions. Optional?
- Algorithm: EAC variant of CRC32? Would pull in dependency except implemented
  by hand. Choose between internal implementation and dep? Make feature
  optional?
- Algorithm: CTDB ids+checksums?

## API

- Public data classes in arcstk or its sub-namespaces are:
	ARId, Checksum, ChecksumSet, DBAR, DBARBlockHeader, AudioSize, ToC,
	ResultBits (Settings?)
	They should behave like int (== Stepanov regularity).
- Make all public data classes regular and _verify_ this by tests:
	- Copy+Move ctor
	- Copy+Move assignment
	- define operators == and !=
	- provide operator < (or specialize std::less if ordering is not natural)
	- specialize std::hash for class
	- Tipp: The moved-from state must be part of the classes invariant or the
		class isn't moveable
- Evaluation: noexcept ok? const ok?

## Implementation

- Improve test coverage

