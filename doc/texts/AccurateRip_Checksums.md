# AccurateRip Checksums


## Motivation

The ultimate motivation to manage AccurateRip checksums is to verify that a
specific rip is accurate, given that the AccurateRip service provides reference
values for this rip.

This requirement includes that rips created with an unknown or unintentional rip
offset (caused by either a drive offset or a pressing offset or a combination of
both) can also be verified. This entails that the rip offset has to be detected
locally given that reference checksums with sufficient confidence are
available.


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

### Flawed initial definition

The actual checksum is the dot product of the index vector and the input vector
of combined samples. Intuitively, it is the lower bits of the sum of products of
each combined sample with its index.

\f[
	\textrm{ARCSv1}\bigl(\langle cs_1,cs_2,...,cs_n\rangle\bigr) %
	\; := \; %
	\sum_{i=1}^{n}\; %
	\bigl[ \bigl( i * cs_{i}\bigr) \:\textrm{mod}\: {2^{32}}\bigr]
\f]

Note that from a all overflows are discarded. Neither the potential overflow of
\f$ i * cs_i \f$ is handled nor the guaranteed overflow while accumulating the
sum.

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
contributions rather than discarding them.

Essentially, ARCSv2 adds the previously discarded higher bits back to the ARCSv1
sum. It adds a 32bit carry folding to the original definition, thereby removing
the flaw.

The definitions for first and last track are corresponding to ARCSv1: the first
track skips the first 2939 samples while increasing the index. The last track
skips the last 2940 samples.

### Determining rip offsets

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

In fact, this property holds for both ARCSv1 and ARCSv2 because overflow bits
are discarded consistently, not accumulated. Therefore, the linearity is
preserved even though neither the bit operation on ARCSv1 nor the carry
propagation of ARCSv2 are linear functions in the traditional sense.

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
  4. For \f$ k < 0\f$ and the first track, \f$ \lvert k\rvert\f$ samples from
	the leading section (which were originally skipped) of the same track are
	added. This part has to be shifted "left" by \f$ n-\lvert k\rvert\f$ where
	\f$ n\f$ denotes the total number of samples of the first track.

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

#### Data structure to support offset detection

AccurateRip skips the first 2939 of the first track and the last 2940 samples of
the last track. Thus, the codomain for offset \f$k\f$ is thereby fixated to the
range [-2939,2940]. We can have minimal negative \f$k\f$ of -2939 since we
cannot shift the window before the first sample. For the equivalent reason, we
can have a maximal positive \f$k\f$ of 2940. With the additional case of
\f$k = 0\f$, the set of possible values has a cardinality of 5880.

We need four kinds of subtotals. For any \f$i\f$ in range [1,2940] we need the
weighted sum (which is just the ARCSv1 for any index up to the respective
\f$i\f$) and the simple sum (which is just \f$sum_{j=1}^i j\f$) up this respective
index position \f$i\f$ of every track. Let's call this range <em>prefix
range</em> and the samples therein <em>prefix samples</em>. Accordingly, we call
the range [n-2940,n] the <em>suffix range</em> of a track. We also need the
simple sum of every \f$j\f$ therein, which is accumulated "backwards":
\f$sum_{j=n}^{n-i} j\f$ and accordingly for the weighted sums. Those subtotals
can just be cached unmodified while looping over the relevant areas of the
track. They can be accumulated after having finished reading the track. (When
referring to "subtotals", I presuppose the accumulated values.)

For each track, we keep an array (or an array section) of size 5880 that
contains the respective subtotals for each \f$k \not= 0\f$. When shifting
"forward", which means \f$k > 0\f$ we subtract prefix samples from the current
track and add prefix samples of the next track. When shifting "backward", which
means \f$k < 0\f$ we add suffix samples of the previous track and subtract
suffix samples of the current track. This is the reason we have to cache 2940
instead of 2939 samples off the start of each track: we may have to add the
start of the track as the end of a +2940 offset. (Each track bound is in the
middle of a 5880-samples-wide window of index positions to cache.)

Accessing this cache via index should be convenient, so we store subtotals from
prefices and those of suffices in different orders.

Then, for a track \f$t\f$ with total samples \f$n\f$ we have the following
subtotals for the prefix samples with index \f$i\f$:

\f[
	\texttt{CACHE\_}\left[i\right] := (1+0) * cs_{1+0} \;+\; (1+1) * cs_{1+1} \;+\; \ldots \;+\; (1+i) * cs_{1+i}
\f]

Alike for suffices:

\f[
	\texttt{CACHE\_}\left[i\right] := (n-i) * cs_{n-i} \;+\; (n-(i-1)) * cs_{n-(i-1)} \;+\; \ldots \;+\; (n-1) * cs_{n-1)}
\f]

This comes down to:

\f{align*}{
\texttt{CACHE\_}\left[0\right] &:= (1+0) * cs_{1+0}\\
\texttt{CACHE\_}\left[1\right] &:= (1+0) * cs_{1+0} \;+\; (1+1) * cs_{1+1}\\
\texttt{CACHE\_}\left[2\right] &:= (1+0) * cs_{1+0} \;+\; (1+1) * cs_{1+1} \;+\; (1+2) * cs_{1+2}\\
 &\vdots\\
\texttt{CACHE\_}\left[2939\right] &:= (1+0) * cs_{1+0} \;+\; (1+1) * cs_{1+1} \;+\; \ldots \;+\; (1+2939) * cs_{1+2939}
\f}

With index 2940 we switch to the suffices:

\f{align*}{
\texttt{CACHE\_}\left[2940\right] &:= (n-2940) * cs_{n-2940} + (n-2939) * cs_{n-2939} + ... + (n-1) * cs_{n-1}\\
\texttt{CACHE\_}\left[2941\right] &:= (n-2939) * cs_{n-2939} + (n-2938) * cs_{n-2938} + ... + (n-1) * cs_{n-1}\\
 &\vdots\\
\texttt{CACHE\_}\left[5879\right] &:= (n-1) * cs_{n-1}\\
\f}

This cache provides the weighted sum for the i-th-first sample in the track as
\f$\texttt{CACHE\_}\left[i - 1\right]\f$ and for the i-th-last sample as
\f$\texttt{CACHE\_}\left[5880 - i\right]\f$.

We maintain two of those caches, one for the weighted sums and one for the
simple sums.

We read the samples sequentially and do not have random access to them. We
suppose that looping over the input (or in fact doing read-IO to get it in the
first place) is expensive, so we have to avoid re-reading and re-looping.

The caches can be populated in two steps: when reading the sample for the
according index, the sample value is assigned to this index position and when
having read the track completely, the first and the second half of the cache is
accumulated in their respective direction.

### Checksum of frame 450

The ARCS of the 0-based frame 450 (which is the 451st frame in the input) is
provided in the AccurateRip answer of any request as part of the dBAR format.
On some sites it is called "Offset CRC" or "Offset Finding CRC". This naming is
erroneous since the checksum for frame 450 is not a CRC in a mathematical sense.

The checksum for frame 450 as provided in dBAR is the ARCSv1 of the 588 samples
in frame 450 starting on an index of 1. This means:

\f[
	\textrm{ARF450}\bigl(\langle cs_1,cs_2,...,cs_{n}\rangle\bigr) %
	\; := \; %
	\sum_{i=1}^{588}\; %
	\bigl[ \bigl( i * cs_{450 * 588 + i}\bigr) \:\textrm{mod}\: {2^{32}}\bigr]
\f]

However, in the dBAR format, the checksum for frame 450 is not guaranteed to
(and in fact mostly does not) match the pressing offset the track checksums of
the same entry. It is therefore not reliable to derive the offset of the track
checksums from the accompanying frame 450 checksums. (This conjecture is based
on tests of several hundreds of discs.)

The historical motivation for frame 450 was to provide a normalized small
snippet of the track that can be iterated at low cost to derive the factual
pressing offset without looping over the entire track again. However, as shown
above, no additional iteration is ever required to apply or test any offset on
an AccurateRip checksum, hence the frame 450 checksum is not required for offset
detection and provides nearly no advantage over a complete enumeration of all
offset applications in the legal range. The reason is that applying an offset to
a checksum implies constant cost regardless whether over how many samples this
checksum was generated. Thus, the condition that motivated providing this
checksum is not given.


## Implementations under free and open-source licenses (incomplete list)

- https://github.com/leo-bogert/accuraterip-checksum
- https://github.com/gchudov/cuetools.net
- https://github.com/arcctgx/ARver
- https://github.com/neheb/cdverify
- https://github.com/whipper-team/whipper
- https://github.com/cyanreg/cyanrip
- https://github.com/spadev/cdrip-tools
- https://github.com/jonls/accuraterip-tools


## References

The AccurateRip checksums were described by Spoon in [1]. The flaw in ARCSv1 was
originally described by Steve Gabriel in 2008 in [2]. The approach for utilizing
linearity for offset detection and the data structure described above was
brought to my attention by several posts of Gregory S. Chudov, the author of
CUETools.NET, e.g. [3] and the source of CUETools.NET.

The AccurateRip DB is (c) by illustrate. Access requires permission. See [4].

[1] [Spoon: AccurateRip CRC Calculation](https://forum.dbpoweramp.com/forum/other-topics/developers-corner/20117-accuraterip-crc-calculation?20641=)

[2] [Steve Gabriel: Reply \#1](https://hydrogenaudio.org/index.php/topic,61468.msg548711.html#msg548711)

[3] [Gregory S. Chudov: Reply \#27](https://hydrogenaudio.org/index.php/topic,66233.msg592342.html#msg592342)

[4] https://www.accuraterip.com/3rdparty-access.htm

