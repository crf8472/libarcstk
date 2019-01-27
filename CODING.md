# Coding Style														{#coding}


I describe the coding style I used to use in the project. It's main purpose is a
documentation for myself. It's not comprehensive but enumerates the points where
I decided to reduce the wiggle room for stylistic creativity.


# Names

- Variable naming style is lower_snake_case: ``sample_seq``,
  ``first_idx``, etc.
- Class names are UpperCamelCase: ``FileReader``, ``SampleBuffer``, etc.
- Non-public class member variables are suffixed with an underscore:
  ``config_``, ``handler_``, etc.
- Namespace names are lower case ASCII-letters (``[a-z]``)
- Macro names are upper case ASCII-letters and may have underscores (``[A-Z_]``)


# Headers and Declarations

- Header file names end with ``.hpp`` (not ``.h`` or anything else).
- Headers do not contain definitions, except when ``inline``


# Formatting

- Use spaces, not tabs
- 1 indent step is 4 spaces.
- Put opening and closing curly braces (``{`` and ``}``) on the next line.
- There is no space after the opening parenthesis ``(`` and no space before the
  closing parenthesis ``)``.
- There is a space before and after any operator.
- Constructor initialization lists are formatted with one assignment per line,
  with leading ``:`` or ``,`` and indented one step to the constructor name.
- Prefer ``and``, ``or`` and ``not`` to ``||``, ``&&`` and ``~``.
- Put the ``= 0;`` of a pure virtual method declaration on a separate line.


# Compiler feedback

- Eliminate *all* compiler warnings from your code. (``CMakeLists.txt`` will
  define for you what to warn about when compiling.)


# Documentation Comments

- Every namespace, every function, every constant, every type, every class and
  every class member must have a documentation comment (regardless whether it's
  part of a public API, an internal API or any API at all).
- If something involves a value, make the unit of the value maximally clear
  (bad: "size"; good: "number of samples", "size in bytes").
- If something involves an index, make maximally clear whether the index is
  1-based or 0-based or whatever its base may be.
- Use doxygens ``\\todo`` tags only for issues that are about to change the API
  or the behaviour (or both), not for mere implementation issues.


# Non-documentation Comments

- The following constructions must have a comment: every improvable code, every
  code that is outcommented and every not improvable hack that has to
  document its indispensability.
- If you have to comment out code or place a commented code snippet, include the
  reasons to do that in the comment.
- If a comment is indicated, prefer commenting blocks to commenting single
  lines. If you comment a block, the comment is preferably as short as possible
  and just summons up what the block is achieving.
- Don't use ``/*`` ``*/`` for commenting out multiple lines. Use ``//`` on
  every line instead.
- If something is wrong but can't be corrected immediately, describe the bug
  leaving a FIXME tag explaining the need at its very site in the code.
- Use ``TODO`` tags within the code to mark sites were non-bug, non-API issues
  are to do.


# Character encoding and newlines

- Libarcs project files have unix line endings (== line feed,``0x0A``). Whatever
  your git is configured to checkout, ensure that your git commits unix line
  endings!
- Ensure that every source file you modify has a unix line ending at the end of
  file. Every line ends with a unix line ending and the end of file must have
  a unix line ending too.
- Libarcs project files are encoded UTF-8. Whatever your git is configured to
  checkout, ensure that your git commits UTF-8 encoded characters.

