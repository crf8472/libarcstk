# Design Principles


I used the project to improve my C++ knowledge while achieving the solution of a
real task. When it eventually grew to a certain point, I felt that I could
share it. This decision motivated me to reflect my API design. I noted the
following points mainly as a log for myself. Libarcstk is at least intended to
obey these requirements.

Beware: the following contains decisions based on my opinion. You might not like
it.

Libarcstk embraces "modern" C++, which means to choose the contemporary way of
doing things, not the way things were done back in the Nineties. Currently,
libarcstk is compiled as C++17. Versions before 0.3 were compiled with C++14.

On the other hand, libarcstk has a tendency to OOP-style design along with some
of its patterns but avoids deep inheritance levels. Inheritance is good in case
it helps to avoid repeating yourself, assists the intuition of the reader, helps
decoupling and models a natural is-a relationship. However, in some cases,
templates seemed the more natural or elegant solution.

The API is deliberately conservative to enable its use also by pre-C++11-code
and perhaps by other languages at some point.


## Macros

- Avoid new macros whenever possible. Macros are the natural solution for
  inclusion guards. Avoid them for any other use. Using a macro when you could
  use a constant is considered a bug.


## C-Style things

- Absolutely *never* use C-style casts, they are totally forbidden. Use
  only C++-casts. For conversions of arithmetic types prefer braced initializers
  (e.g. ``uint32_t { foo }``).
- Avoid using the C-API entirely wherever possible, do things C++style.
- If you absolutely must use the C-API for now, use it via its C++-headers
  whenever possible (e.g. ``cstdint`` instead of ``stdint.h``) to avoid
  polluting the global namespace.
- If you absolutely must use the C-API for now, document the very code site with
  a ``TODO`` tag and mark it with the string ``C-style stuff:`` followed by an
  explanation of the overwhelming excellent reasons why the use of the C-API was
  completely unavoidable. (For example: using libfoo imposes using C-style code
  but not using libfoo would reinvent the wheel.)


## Globals

- Avoid new globals whenever possible. (Providing a constant is usually not a
  reason for a global: this can also be a static member of a class/struct or a
  function just returning the constant.)


## Types

- Owning raw pointers are absolutely forbidden, use ``std::unique_ptr`` instead.
- Use STL data types whenever possible: std::string instead of foo::myStr.
- Prefer smart pointers over raw pointers. Prefer to use ``std::make_unique``
  if possible.
- Use non-owning raw pointers sparingly, except for very good reasons.
- Prefer ``using``-declaratives over classical ``typedef``'s:
  ``using A = foo::A`` instead of ``typedef foo::A A``.
- Prefer choosing the minimal possible scope for a using declarative. Avoid
  any declarative of the form ``using namespace foo`` except for good reasons.
- Prefer the form ``auto foo { expr }`` for auto-typed and ``auto foo = type
  {expr}`` for fixed-type variables (see [Gotw94][1]).
- Prefer braced initialization, also in constructor lists.


## Classes

- Absolutely avoid class members that are ``public`` and non-const. Use
  accessors and mutators instead. Also trivial accessors and mutators are ok.
- Classes in exported header files should be Pimpls. The forward declaration
  and the opaque pointer in the Pimpl class are ``private``.
- A non-template class declaration contains only declaration of its members, but
  not their inline implementation. (Inlining is no reason, static is no reason.)
- The definition ``= delete`` has to be in the header not in the source file
  since it is part of the API.


## Linkage

- Libarcstk does never ever put anything in the global namespace. Everything
  that is part of libarcstk *must* reside in the ``arcstk`` namespace or one of
  its contained namespaces.
- When it is arcstk-global, it should have ``extern`` linkage to avoid
  unnecessary instances.
- What is declared and used only within a ``.cpp`` file must have internal
  linkage, usually by putting it in an unnamed namespace. Avoid the ``static``
  qualifier for only defining linkage.


## Header files

- Any header file only declares symbols that are intentionally part of its API.
  Symbols in a header may not exist "by accident" or for "technical reasons".
  If you absolutely must provide a symbol in a header that is not considered
  part of the public API enclose it in the namespace ``arcstk::details``.
  Of course forward declared implementation pointers are ok.
- Non-public files (such as .tpp files with template implementations) reside
  in a directory ``details`` within the top-level include directory.
- If it is not part of the public API but needs to be tested, move it to a
  separate header in the source directory and include that by the test class.
  Prefix the header filename with the name of the doxygen module the header
  contributes to followed by an underscore ``_``.


## Dependencies

- Do not introduce any new external dependencies. For runtime, prefer standard
  library whenever reasonably possible and for buildtime stick to the tools
  already involved (CMake, Catch2).


## Tests

- If it does anything non-trivial, add a unit test for it.
- Keep one testcase file per module, since the modules are of moderate size and
  compiling tests is expensive.

[1]: https://herbsutter.com/2013/08/12/gotw-94-solution-aaa-style-almost-always-auto/
