# How to verify AccurateRip checksums


## Have a look at the code example

Locate the example ``albumverify`` in the top-level folder ``examples``. You may
find a compileable code example for verifying AccurateRip checksums on a
concrete album image.


## Verifying an Album

### Prerequisites

Consult the module @ref verify for the part of the API you want to use.

Insert

@code{.cpp}
	#include <arcstk/dbar.hpp>
	#include <arcstk/identifier.hpp>
	#include <arcstk/calculate.hpp>
	#include <arcstk/verify.hpp>
@endcode

in the file where you intend to place your code.

### Verify

You need
	- AccurateRip id of the album to verify
	- Reference Checksums provided by AccurateRip for this particular id
	- The audio input you wish to verify


