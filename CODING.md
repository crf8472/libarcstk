# Coding Style


I describe the coding style I used to use in the project. It's main purpose is a
documentation for myself. It's not comprehensive but enumerates the points where
I decided to reduce the wiggle room for stylistic creativity.

Beware: the following contains my opinion. You might not like it. It does *not*
claim to be the truth to obey for everybody, it is just the current setting for
libarcstk.



## Character encoding and newlines

- Libarcstk project files have unix line endings (== line feed,``0x0A``).
  Whatever your git is configured to checkout, ensure that your git commits unix
  line endings.
- Libarcstk project files are encoded UTF-8. Whatever your git is configured to
  checkout, ensure that your git commits UTF-8 encoded characters.
- Ensure that every source file you modify has a unix line ending (a blank line)
  at the end of the file.


## Formatting

- Use tabs not spaces for indenting
- 1 indent step is 1 tab = 4 spaces.
- Put opening and closing curly braces of blocks (``{`` and ``}``) on the next
  line.
- There is no space after the opening parenthesis ``(`` and no space before the
  closing parenthesis ``)``.
- Braced initializers: There is one space before and after the opening brace
  ``{`` and one space before the closing brace ``}``.
- There is a space before and after any operator.
- Put the ``= 0;`` of a pure virtual method declaration on a separate line.
- Constructor initialization lists are formatted with one assignment per line,
  with leading ``:`` or ``,`` and indented one step to the constructor name.


## Compiler feedback

- Eliminate *all* compiler warnings for *all* supported compilers from your
  code. (Root ``CMakeLists.txt`` declares the compilers to support and the
  switches to generate feedback.)


## Headers, Declarations, Definitions

- Header file names end with ``.hpp`` (not ``.h``,``.hh`` or anything else).
- Headers do not contain definitions, except for template (member) functions or
  for functions the compiler must be able to inline (as SampleInputIterator and
  the functions in the logging API).
- Avoid inline definitions in the body of non-template classes.
- For templates, use ``typename`` for sites where base types are allowed and use
  ``class`` for sites where own class definitions are required.


## Documentation

- Everything declared in a header, i.e. every namespace, every function,
  every constant, every type, every class and every class member must have a
  documentation comment.
- Every declaration in a source file except implementation classes (i.e.
  subclasses that are ``final`` and not declared in a header) must have a
  documentation comment.
- Implementation classes and their members may or may not have documentation
  comments.
- Use Doxygens ``\todo`` tags sparingly and only for issues that are about to
  change the API, not for mere implementation issues.
- If something involves an index, document whether the index is 1-based or
  0-based or whatever its start value may be.


## Non-documentation Comments

- The following constructions must have a comment: every improvable code, every
  code that is outcommented and every not improvable hack that has to
  document its indispensability.
- If you have to comment out code or place a commented code snippet, include the
  reasons to do that in the comment.
- If a comment is indicated, prefer commenting blocks over commenting single
  lines. If you comment a block, the comment is preferably as short as possible
  and just summons up what the block is achieving.
- Don't use ``/*`` ``*/`` for commenting out multiple lines. Use ``//`` on
  every line instead.
- If something is wrong but can't be corrected immediately, describe the bug
  leaving a FIXME tag explaining the need at its very site in the code.
- Use ``TODO`` tags within the code generously to mark sites were non-bug,
  non-API issues are to do. For bugs, use ``FIXME``. If unsure, use ``XXX``.


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


## Names

- Variable naming style is lower_snake_case: ``sample_seq``,
  ``first_idx``, etc.
- Class names are UpperCamelCase: ``AudioSize``, ``ContentHandler``, etc.
- Non-public class member variables are suffixed with an underscore:
  ``config_``, ``handler_``, etc.
- Namespace names consist of lower case ASCII-letters (``[a-z]``)
- Macro names are upper case ASCII-letters and may contain underscores
  (``[A-Z_]``)
- If something involves a value, make the unit of the value maximally clear
  (good: ``bytes_per_frame``, ``total_samples`` - ``bad: ``frame_size``,
  ``samples``).
- For input, you may also use ``using`` declarations to hint on further
  restrictions of types, e.g. TrackNo transports a hint that values from 1-99
  are actually valid input while underlying type may accept more values.


## Globals

- Avoid new globals whenever possible. (Providing a constant is usually not a
  reason for a global: this can also be a static member of a class/struct or a
  function just returning the constant.)


## Types

- Owning raw pointers are absolutely forbidden, use ``std::unique_ptr`` instead.
- Use STL data types whenever possible: ``std::string`` instead of
  ``foo::myStr``.
- Prefer smart pointers over raw pointers. Prefer to use C++14's
  ``std::make_unique`` if possible.
- Use non-owning raw pointers sparingly, except for very good reasons.
- Prefer ``using``-declaratives over classical ``typedef``'s:
  ``using A = foo::A`` instead of ``typedef foo::A A``.
- Prefer choosing the minimal possible scope for a ``using`` declarative.
  Declaratives of type ``using namespace`` in any non-leaf namespace are
  considered a bug.
- Prefer the form ``auto foo { expr }`` for auto-typed and ``auto foo = type
  {expr}`` for fixed-type variables (see [Gotw94][1]).
- Prefer braced initialization, also in constructor lists.


## Classes

- Absolutely avoid class members that are ``public`` and non-const. Use
  accessors and mutators instead. Also trivial accessors and mutators are ok.
- Classes in exported header files should be Pimpls if they hold private
  members. (It could be ok to use non-Pimpl layout for classes without private
  members but private member functions.) The forward declaration and the opaque
  pointer in the Pimpl class are ``private``. The pointer to the impl class is
  always a ``unique_ptr``.
- Any non-template class declaration contains only declaration of its members,
  but not their inline implementation. (Inlining is no reason, static is no
  reason.)
- The definition ``= delete`` has to be in the header not in the source file
  since it is part of the API. The definition ``= default`` is preferred to be
  in the source file not in the header file.


## Linkage

- Libarcstk does never ever put anything in the global namespace. Period.
  Everything that is part of libarcstk *must* reside in the ``arcstk``
  namespace or one of its contained namespaces.
- When it is arcstk-global, it should have ``extern`` linkage to avoid
  unnecessary instances.
- What is declared and used only within a ``.cpp`` file must have internal
  linkage, usually by putting it in an unnamed namespace. Avoid the ``static``
  qualifier for only defining linkage. Avoid declarations in the source file
  whenever possible.


## Header files

- Any header file only declares symbols that are intentionally part of its API.
  Symbols in a header may not exist "by accident" or for "technical reasons".
  If you absolutely must provide a symbol in a header that is not considered
  part of the public API enclose it in the namespace ``arcstk::details``.
  Of course forward declared implementation pointers are ok.
- If it is not part of the public API but needs to be tested, move it to a
  separate header in the source directory and include that by the test class.
- Consider to reuse the filename of the public header and append ``_details``
  to its basename to make clear which source file depends on that header's
  contents.


## Dependencies

- Do not introduce any new external dependencies. For runtime, prefer standard
  library whenever reasonably possible and for buildtime stick to the tools
  already involved (CMake, Catch2).
- For documentation stick to the tools already involved (doxygen, m.css). Any
  additional buildtime dependencies must be deactivated by default and only
  optionally activated by a CMake buildstep-switch.


## Tests

- If it does anything non-trivial, add a unit test for it.
- Keep one testcase file per TU since compiling tests is expensive.

[1]: https://herbsutter.com/2013/08/12/gotw-94-solution-aaa-style-almost-always-auto/

