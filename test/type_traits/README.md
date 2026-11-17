# Type traits tests

Verify design properties of any class in public or non-public headers:

  - Constructing methods and destructibility of the type.
  - Verify whether class is abstract, final or without members.
  - Verify whether class is swappable, comparable or supports to_string
	functionality.

### TEST_CASE 1: 'traits'

  - Is it a class?
  - Is it abstract?
  - Is it final?
  - Does it have state, a.k.a. non-static members?

### TEST_CASE 2: 'construction traits'

  - Is it default constructable?
  - Is it copyable?
  - Is it moveable?
  - Is it destructable?

### TEST_CASE 3: 'value traits'

  - Is it swappable?
  - Is it comparable?
  - Is it convertible to bool? (Or to any other base type?)
  - Is it convertible to string?

