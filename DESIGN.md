# Design Principles													{#design}


Libarcs embraces "modern" C++, which means to choose the contemporary way of
doing things, not the way things were done back in the Nineties.

On the other hand, libarcs has a tendency to OOP-style design along with its
patterns but avoids deep inheritance levels. Inheritance is good in case it
helps to avoid repeating yourself, assists the intuition of the reader and
models a natural is-a relationship. However, genericity is considered as the
bigger achievement, so when in doubt, prefer the generic solution over OOP.

For example: it is tried not to bloat classes with members just to make
something a member. Always consider whether it is better to make it a non-member
non-friend.

The API is deliberately conservative to enable its use also by pre-C++11-code.

# Macros

- Avoid new macros whenever possible. Especially, avoid defining macros for data
  values, use constant variables instead.

# C-Style things

- Avoid using the C-API entirely wherever possible, do things C++style.
- Absolutely *never* use C-style casts, they are totally forbidden. Use
  only C++-casts. For conversions of arithmetic types prefer braced initializers
  (e.g. ``uint32_t{foo}``).
- If you absolutely must use the C-API for now, use it via its C++-headers
  whenever possible (e.g. ``cstdint`` instead of ``stdint.h``) to avoid
  polluting the global namespace.
- If you absolutely must use the C-API for now, document the very code site with
  a ``TODO`` tag and mark it with the string ``C-style stuff:`` followed by an
  explanation of the overwhelming excellent reasons why the use of the C-API was
  completely unavoidable. (For example: using libfoo imposes using C-style code
  but not using libfoo would reinvent the wheel.)

# Globals

- Avoid new globals whenever possible.
- Make it a member of a class, except for good reasons (e.g. in case of
  operators, service methods or if encapsulation is better supported by making
  it a non-member non-friend.).

# Types

- Use STL data types whenever possible: std::string instead of foo::myStr.
- Prefer C++14-style smart pointers (using ``std::make_*``) over raw pointers.
- Owning raw pointers are absolutely forbidden, use ``std::unique_ptr`` instead.
- Avoid non-owning raw pointers except for very good reasons (e.g. when
  protected accessors seem unavoidable).
- Prefer ``using A = foo:A`` over ``typedef``s.

# Classes

- Absolutely avoid class members that are ``public`` and non-const. Use
  accessors and mutators instead.
- Classes in exported header files do not have private members, except a single
  pointer to an instance whose type is initially forward declared in the same
  class. (In short: if it's declared in an exported header and happens to need
  private members, hide them by making it a ``Pimpl``.)
- A class declaration contains only declaration of its members, but never their
  inline implementation. (Inlining is no reason, static is no reason.)
- The definition ``= default`` has to be in the ``.cpp`` file not in the header.

# Linkage

- Libarcs does never ever put anything in the global namespace. Everything that
  is part of libarcs *must* reside in the ``arcs`` namespace or one of its
  contained namespaces.
- When it is arcs-global, it should have ``extern`` linkage to avoid unnecessary
  instances.
- What is declared and used only within a ``.cpp`` file must have internal
  linkage, usually by putting it an unnamed namespace or declaring it as static.

# Header files

- Any header file only declares symbols that are intentionally part of its API.
  Symbols in a header may not exist "by accident" or for "technical reasons".
  If you absolutely must provide a symbol in a header that is not considered
  part of the public API enclose it in a namespace ``details``.
- Of course forward declared implementation pointers are ok.

# Dependencies

- Do not introduce any new external dependencies. For runtime, prefer standard
  library whenever reasonably possible and for buildtime stick to the tools
  already involved (CMake, Catch2, git).

# Tests

- If it does anything non-trivial, add a unit test for it.
- Test filenames are of the scheme: ``<compilation_unit>-<classname>`` where the
  classname may be omitted if the test tests multiple entities of the
  compilation unit.

