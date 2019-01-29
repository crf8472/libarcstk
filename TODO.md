# ToDos


A list of ToDos, mainly for myself, do not expect consistency. Especially do not
expect that I really do anything of what I noted here any time soon.


## API

- Make all concrete classes regular, whenever possible i.e.:
	- Copy+Move ctor
	- Copy+Move assignment
	- define operators == and !=
	- specialize std::less for class (or provide operator <)
	- specialize std::hash for class
	- Tipp: implement check and write a test for it
	- Tipp: The moved-from state must be part of the classes invariant or the
		class isn't moveable
- Implement non-member non-friend functions as function objects


## Technical

- Make project compileable on Windows
- Ruby bindings (e.g. using rice)
- Python bindings


## Features

- Offset correction i.e. fast re-computation of ARCSs for specified read offsets
like in CUE.Tools (not sure whether this is needed)

