#ifndef __LIBARCSTK_MATCHERS_HPP__
#define __LIBARCSTK_MATCHERS_HPP__

/**
 * \internal
 *
 * \file
 *
 * \brief Internal API for Matcher algorithms.
 */

#ifndef __LIBARCSTK_MATCH_HPP__
#include "match.hpp"
#endif
#ifndef __LIBARCSTK_CALCULATE_HPP__
#include "calculate.hpp"
#endif
#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#include "identifier.hpp"
#endif
#ifndef __LIBARCSTK_LOGGING_HPP__
#include "logging.hpp"
#endif


namespace arcstk
{
inline namespace v_1_0_0
{

class ARResponse;

namespace details
{

class MatchOrder;

/**
 * \brief Defines the traversal method of the reference checksums.
 *
 * The traversal method can e.g. be implemented as an iteration over a single
 * block in the ChecksumSource. Alternatively, it could be implemented as a
 * traversal over the same track in every block.
 */
class MatchTraversal
{
	virtual Checksum do_get_reference(const ChecksumSource& ref_sums,
			const int current, const int counter) const
	= 0;

	virtual std::size_t do_size(const ChecksumSource& ref_sums,
			const int current) const
	= 0;

	virtual void do_traverse(Match& match, const Checksums &actual_sums,
		const ARId &actual_id, const ChecksumSource& ref_sums,
		const MatchOrder& order) const
	= 0;

public:

	/**
	 * \brief Virtual default destructor.
	 */
	virtual ~MatchTraversal() noexcept = default;

	/**
	 * \brief Provide a Checksum from some current index and some counter index.
	 *
	 * \param[in] ref_sums The source of checksums
	 * \param[in] current  Index of the current set
	 * \param[in] counter  Index of the checksum within the set indexed by \c current
	 *
	 * \return The checksum under the specified indices
	 */
	Checksum get_reference(const ChecksumSource& ref_sums, const int current,
			const int counter) const
	{
		return do_get_reference(ref_sums, current, counter);
	}

	/**
	 * \brief Provide the number of checksums under the index \ current.
	 *
	 * \param[in] ref_sums The source of checksums
	 * \param[in] current  Index of the current set
	 *
	 * \return The checksum under the specified index
	 */
	std::size_t size(const ChecksumSource& ref_sums, const int current) const
	{
		return do_size(ref_sums, current);
	}

	/**
	 * \brief
	 */
	void traverse(Match& match, const Checksums& actual_sums,
			const ARId &actual_id, const ChecksumSource& ref_sums,
			const MatchOrder& order) const
	{
		do_traverse(match, actual_sums, actual_id, ref_sums, order);
	}
};


/**
 * \brief
 */
class TraverseBlock final : public MatchTraversal
{
	virtual Checksum do_get_reference(const ChecksumSource& ref_sums,
			const int current, const int counter) const final;

	virtual std::size_t do_size(const ChecksumSource& ref_sums,
			const int current) const final;

	virtual void do_traverse(Match& match, const Checksums &actual_sums,
			const ARId &actual_id, const ChecksumSource& ref_sums,
			const MatchOrder& order) const final;

public:

	using MatchTraversal::MatchTraversal;
};


/**
 * \brief
 */
class TraverseTracks final : public MatchTraversal
{
	virtual Checksum do_get_reference(const ChecksumSource& ref_sums,
			const int current, const int counter) const final;

	virtual std::size_t do_size(const ChecksumSource& ref_sums,
			const int current) const final;

	virtual void do_traverse(Match& match, const Checksums &actual_sums,
			const ARId &actual_id, const ChecksumSource& ref_sums,
			const MatchOrder& order) const final;

public:

	using MatchTraversal::MatchTraversal;
};


class MatchPerformer;


/**
 * \brief Match items in a single loop.
 *
 * Apply the matches in any order.
 */
class MatchOrder
{
	virtual void do_perform(Match& match, const Checksums& actual_sums,
			const ChecksumSource& ref_sums,
			const MatchTraversal& t, int index) const
	= 0;

public:

	MatchOrder();

	virtual ~MatchOrder() noexcept = default;

	const MatchPerformer* performer() const noexcept;

	void set_performer(MatchPerformer* const performer) noexcept;

	void perform(Match& match, const Checksums& actual_sums,
			const ChecksumSource& ref_sums,
			const MatchTraversal& t, int index) const
	{
		do_perform(match, actual_sums, ref_sums, t, index);
	}

private:

	MatchPerformer* performer_;
};


/**
 * \brief
 */
class TrackOrder final : public MatchOrder
{
	using MatchOrder::MatchOrder;
	virtual void do_perform(Match& match, const Checksums& actual_sums,
			const ChecksumSource& ref_sums,
			const MatchTraversal& t, int index) const final;
};


/**
 * \brief
 */
class Cartesian final : public MatchOrder
{
	using MatchOrder::MatchOrder;
	virtual void do_perform(Match& match, const Checksums& actual_sums,
			const ChecksumSource& ref_sums,
			const MatchTraversal& t, int index) const final;
};


/**
 * \brief
 */
class MatchPerformerInterface
{
	virtual std::unique_ptr<Match> do_create_match_instance(
		const int blocks, const std::size_t tracks) const noexcept
	= 0;

	virtual bool do_matches(
		const ARId& actual, const ARId& reference) const noexcept
	= 0;

	virtual bool do_matches(
		const Checksum& actual, const Checksum& reference) const noexcept
	= 0;

protected:

	/**
	 * \brief
	 */
	std::unique_ptr<Match> create_match_instance(
		const int blocks, const std::size_t tracks) const noexcept
	{
		return do_create_match_instance(blocks, tracks);
	}

public:

	/**
	 * \brief
	 */
	virtual ~MatchPerformerInterface() noexcept = default;


	/**
	 * \brief Implement matching an actual ARId against a reference.
	 *
	 * The matching is implemented by calling operator == on the input
	 * ARId instances.
	 *
	 * \param[in] actual    The actual ARId to be matched
	 * \param[in] reference The reference ARId to match
	 *
	 * \return TRUE if the sums match, otherwise FALSE
	 */
	bool matches(
		const ARId& actual, const ARId& reference) const noexcept
	{
		return do_matches(actual, reference);
	}


	/**
	 * \brief Implement matching an actual Checksum against a reference.
	 *
	 * The matching is implemented by calling operator == on the input
	 * Checksum instances.
	 *
	 * \param[in] actual    The actual Checksum to be matched
	 * \param[in] reference The reference Checksum to match
	 *
	 * \return TRUE if the sums match, otherwise FALSE
	 */
	bool matches(
		const Checksum& actual, const Checksum& reference) const noexcept
	{
		return do_matches(actual, reference);
	}
};


/**
 * \internal
 * \brief Actual match performing Functor.
 */
class MatchPerformer final : public MatchPerformerInterface
{
	virtual std::unique_ptr<Match> do_create_match_instance(
		const int blocks, const std::size_t tracks) const noexcept final;

	virtual bool do_matches(
		const ARId& actual, const ARId& reference) const noexcept final;

	virtual bool do_matches(
		const Checksum& actual, const Checksum& reference) const noexcept final;

	//virtual std::unique_ptr<Match> do_perform(
	//		const Checksums& actual_sums, const ARId& actual_id,
	//		const ARResponse& ref_sums) const noexcept
	//= 0;

public:

	/**
	 * \brief The checksum types to verify.
	 */
	static constexpr std::array<checksum::type, 2> types {
		checksum::type::ARCS1,
		checksum::type::ARCS2
	};

	MatchPerformer(MatchTraversal* traversal, MatchOrder* order);

	virtual ~MatchPerformer() noexcept = default;

	const MatchTraversal* traversal() const;

	const MatchOrder* order() const;

	std::unique_ptr<Match> operator() (
			const Checksums& actual_sums, const ARId& actual_id,
			const ChecksumSource& ref_sums) const;

	//std::unique_ptr<Match> perform(const Checksums& actual_sums,
	//		const ARId &actual_id, const ARResponse& ref_sums) const noexcept
	//{
	//	return do_perform(actual_sums, actual_id, ref_sums);
	//}

private:

	MatchTraversal* const traversal_;
	MatchOrder* const order_;
};


/**
 * \internal
 * \brief Finds best matching block for tracks in order as passed.
 */
/*
class SingleBlockMatch final : public MatchPerformer
{
	std::unique_ptr<Match> do_perform(const Checksums &actual_sums,
			const ARId &actual_id, const ARResponse &ref_sums) const noexcept
		final;
};
*/


/**
 * \internal
 * \brief Finds best matching block for tracks in random order.
 */
/*
class AcrossBlocksMatch final : public MatchPerformer
{
	std::unique_ptr<Match> do_perform(const Checksums &actual_sums,
			const ARId &actual_id, const ARResponse &ref_sums) const noexcept
		final;
};
*/

} // namespace details

} // namespace v_1_0_0

} // namespace arcstk

#endif
