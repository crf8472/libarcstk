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
libarcstk is compiled as C++17. Versions before 0.3 were compiled as C++14.

On the other hand, libarcstk has a tendency to OOP-style design along with some
of its patterns but avoids deep inheritance levels. Inheritance is good in case
it helps to avoid repeating yourself, assists the intuition of the reader, helps
decoupling and models a natural is-a relationship. However, in some cases,
templates seemed the more natural or elegant solution.

The API is deliberately conservative to enable its use also by pre-C++11-code
and perhaps by other languages at some point.

