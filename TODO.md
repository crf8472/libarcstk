# ToDos


A list of ToDos, mainly for myself, do not expect consistency. Especially do not
expect that I really do anything of what I noted here any time soon.


## API

- Public data classes in arcstk or its sub-namespaces are:
	ARId, Checksum, ChecksumSet, DBAR, DBARBlockHeader, AudioSize, ToC,
	ResultBits (Settings?)
	They should behave like int (Stepanov regularity).
- Make all data classes regular:
	- Copy+Move ctor
	- Copy+Move assignment
	- define operators == and !=
	- provide operator < (or specialize std::less if ordering is not natural)
	- specialize std::hash for class
	- Tipp: The moved-from state must be part of the classes invariant or the
		class isn't moveable
- Multilanguage support. Would pull in dependency though.


## Features

- Offset correction i.e. fast re-computation of ARCSs for specified read offsets
like in CUE.Tools (not sure whether this is needed)

