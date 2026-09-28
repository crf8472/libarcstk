# AccurateRip Algorithm


## Motivation

 - Verify that rip is accurate.
 - Verify rips created with a mismatching drive or pressing offset.


## The Math

### Input: samples and tracks

A CDDA stereo sample consists of two numeric 16 bit values
\f$\langle s_l, s_r\rangle\f$, one for the left audio channel and one for the
right channel. The samples may or may not actually be of signed type but for
convenience, we consider them as unsigned 16 bit.

Two corresponding 16 bit stereo samples \f$ s_l\f$ and \f$ s_r\f$ can be
combined to an unsigned 32 bit value
\f$ cs := (s_r \ll\ 16)\, |\, s_l\quad \f$ and we call those
values "combined samples".

Combined samples are the expected input for calculating an AccurateRip
Checksum.

An AccurateRip checksum is calculated over an audio track
\f$ t := \langle cs_1,cs_2,...,cs_n\rangle \f$
consisting of a sequence of combined samples \f$ cs_i \f$
with \f$i: 1 <= i <= n \f$ being a 64 bit integer type.

### Flawed first definition

The actual checksum is the dot product of the index vector and the input vector
of combined samples. Intuitively, it is the lower bits of the sum of products of
each combined sample with its index.

\f[
	\textrm{ARCSv1}\bigl(\langle cs_1,cs_2,...,cs_n\rangle\bigr) %
	\; := \; %
	\sum_{i=1}^{n}\; %
	\bigl[ \bigl( i * cs_{i}\bigr) \:\textrm{mod}\: {2^{32}}\bigr]
\f]

Note that from a technical point of view all overflows are ignored. Neither the
overflow of \f$ i * cs_i \f$ is handled nor the guaranteed overflow while
accumulating the sum.

First 2939 samples of the first track are treated as zero, hence the first
product contributing to the sum is \f$ 2940 * cs_{2940} \f$. The last 2940
samples of the last track are ignored, hence the last product is \f$ (n-2940) *
cs_{n-2940} \f$.

\f[
	\textrm{ARCSv1}\bigl(\textrm{first}\bigr) \; := \; %
	\sum_{i=2940}^{n}\; %
	\bigl[ \bigl( i * cs_{i}\bigr) \:\textrm{mod}\: {2^{32}}\bigr]
\f]

\f[
	\textrm{ARCSv1}\bigl(\textrm{last}\bigr) \; := \; %
	\sum_{i=1}^{n - 2940}\; %
	\bigl[ \bigl( i * cs_{i}\bigr) \:\textrm{mod}\: {2^{32}}\bigr]
\f]

### Problems of the first definition

By this definition, a considerable amount of bits will never contribute to the
checksum. ARCSv1 is therefore considered flawed and is sometimes referred to as
"ARCF" (AccurateRip Checksum Flawed).

### Improved re-definition

\f[
	\textrm{ARCSv2}\bigl(\langle cs_1,cs_2,...,cs_n\rangle\bigr) %
	\; := \; %
	\sum_{i=1}^{n} %
		\bigl[ \bigl((i * cs_{i}) \:\textrm{mod}\: 2^{32}\bigr) %
			 + \bigl((i * cs_{i}) \:\textrm{div}\: 2^{32}\bigr)\bigr]
\f]

The product is originally considered 64 bits. It is decomposed into
its lower 32 bits \f$ (i * cs_i) \textrm{mod} 2^{32}\f$ and
higher bits \f$ (i * cs_i) \textrm{div} 2^{32}\f$, which are then summed
together. This is equivalent to treating the carry bits as additional
contributions rather than discarding them. Essentially, ARCSv2 is the sum of the
ARCSv1 over the input and its formerly ignored higher bits. It adds a 32bit
carry folding to the original definition, thereby removing the flaw.

The definitions for first and last track are corresponding to ARCSv1: the first
track skips the first 2939 samples while increasing the index. The last track
skips the last 2940 samples.

### Drive Offsets

Supporting drive offsets requires to adjust the weights of the sum by an integer
amount \f$ k\f$ without having to re-read the entire input. This is an index
shifting on the dot product.

Let's say we shift the checksum by an amount of 3: what was formerly sample 1 is
now sample 4, the former sample 2 is now sample 5 and so on. Every \f$ i\f$ is
increased by \f$ k\f$ which we assumed to be 3.

A property of the dot product, that is very helpful in this case, is its
algebraic linearity:

\f[
	\sum_{i=1}^{n} \bigl( (i+k) * cs_{i}\bigr) %
	= %
	\sum_{i=1}^{n} \bigl( i * cs_{i}\bigr) + k * \sum_{i=1}^{n} cs_{i}
\f]

In fact, this property holds for both ARCSv1 and ARCSv2 because both ignore
overflow bits — overflow is discarded consistently, not accumulated. Therefore,
the linearity is preserved even though neither the bit operation on ARCSv1 nor
the carry propagation of ARCSv2 are linear functions in the traditional sense.

Algebraic linearity entails that we can shift the checksum already calculated
without expensive re-reading or re-calculating if we have access to the simple
sum of all combined samples in the input. This sum is easy to cache while
reading the input. So that's good news.

Returning to our example with \f$ k == 3\f$ we recognize that the task would not
be completed by only shifting the checksum. First we have to remove the first
three samples of track \f$ t\f$ since its sample 4 is now to take index position
1. The removal can be easily done before shifting the remaining sum. Then, we
have to perform an index shift operation such that remainder part, whose first
sample is now the original sample 4, starts with index 1.

Conversely, we also have to add 3 samples after the original last position. If
\f$ t\f$ is not the last track, those 3 samples are the first samples in the
following track \f$ t+1\f$. Their original index starts with 1. The sample
portion to be added has therefore to be shifted by \f$ n-k\f$ to the "right" to
have their correct position:

\f[
	\textrm{shift}(t,k)
	:= %
	\underbrace{ %
			\sum_{i=k+1}^{n} \bigl( i * cs_{i,t}\bigr) %
			- k * \sum_{i=k+1}^{n} cs_{i,t} %
	}_{\text{Part of } t \text{ to carry over: shift left by } k} + %
	\underbrace{ %
		\Biggl(%
			\sum_{i=1}^{k} \bigl( i * cs_{i,t+1}\bigr) %
			+ (n-k) * \sum_{i=1}^{k} cs_{i,t+1} %
		\Biggr) %
	}_{\text{Part of } t+1 \text{ to add: shift right by } n-k} %
\f]

Note that the formula assumes \f$ k > 0\f$ which imposes to "shift right",
intuitively towards the following track. Furthermore it specifies the shifting
of a track, that is not the last track. For \f$ k < 0\f$ the index ranges and
the interaction with track boundaries differ.

Specifically, there are four cases of shifting to distinguish:

  1. For \f$ k > 0\f$ and non-last tracks, samples are added from the following
    track and must be shifted "right" by \f$ n-k\f$ while the carry-over part is
	shifted "left" by \f$ k\f$.
  2. For \f$ k > 0\f$, the shifting of the last track differs in that it adds a
	section of samples of its own trailing part that was skipped in the original
	checksum. This part has to be shifted "right" by \f$ k\f$.
  3. For \f$ k < 0\f$ and non-first tracks, \f$ \lvert k\rvert\f$ samples are
    added from the preceding track and must be shifted "left" by an amount of
	\f$ n-\lvert k\rvert\f$ with \f$ n\f$ denoting the total number of samples
	in the preceding track. The carry-over part is shifted "right" by
	\f$ \lvert k\rvert\f$.
  4. For \f$ k < 0\f$ and the first track, \f$ \lvert k\rvert\f$ samples of the
	leading section of the same track are added that were skipped in the
	original checksum. This part has to be shifted "left" by
	\f$ n-\lvert k\rvert\f$ where \f$ n\f$ denotes the total number of samples
	of the first track.

A complete "sliding window" shift of the dot product therefore implies three
operations thereby achieving an overlap-add:

  - remove the (either leading or trailing) section of input samples that is not
	included in the shifted dot product
  - shift the part that is present in the original as well as in the shifted
	dot product and then
  - add the shifted part that is only present in the shifted dot product
    for correction.

Shifting can obviously only be maximized by the amount skipped on the start and
the end. Hence, factor \f$k\f$ is only practically applicable for shifting if it
is in the range \f$[-2939,2940]\f$.

This gives 5880 different possible values for \f$ k\f$, including 0. When
reading the input sequence, we cache the weighted sum as well as the cumulative
simple sum for each \f$ k\f$ in the valid range. When performing a shift, we
have just to read the values from the corresponding array indices to be able to
shift. If we choose a signed type for \f$ k\f$, addition will automatically do
the right thing.

### Frame 450

The ARCS of the 0-based frame 450 (which is the 451. frame in total) is provided
in the AccurateRip answer of any request.


## Implementation

  - Combining the original samples is done by the iterator instance. This avoids
    the use of additional memory.
  - Using a Partitioner providing input respecting track bounds avoids
    if-comparisons within the calculation loop to check whether the end of the
	track is reached.
  - For shifting, the relevant 5880 subtotals of each track are cached. Drive
	offset detection is therefore performant. All possible 5880 values can be
	brute-forced.

