#include "catch2/catch.hpp"

#include <algorithm>         // for max
#include <array>             // for array
#include <cstdint>           // for uint32_t, int32_t
#include <list>              // for list
#include <memory>            // for unique_ptr
#include <string>            // for basic_string, operator==, string
#include <type_traits>       // for integral_constant<>::value
#include <vector>            // for vector

#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#include "identifier.hpp"
#endif


/**
 * \file identifier.cpp Fixtures for classes in module 'identifier'
 */


TEST_CASE ( "toc::get_offsets", "[identifier]" )
{
	using arcstk::toc::get_offsets;

	using arcstk::details::TOCBuilder;

	SECTION ( "Returns correct offsets from TOC" )
	{
		// "Bach: Organ Concertos", Simon Preston, DGG
		auto toc0 = TOCBuilder::build(
			// track count
			15,
			// offsets
			std::vector<arcstk::lba_count_t>{ 33, 5225, 7390, 23380, 35608, 49820,
			69508, 87733, 106333, 139495, 157863, 198495, 213368, 225320,
			234103 },
			// leadout
			253038,
			// filenames
			std::vector<std::string>{ "file", "file", "file", "file", "file",
			"file", "file", "file", "file", "file",
			"file", "file", "file", "file", "file" }
		);

		auto offsets = get_offsets(*toc0);

		CHECK ( offsets.size() == 15 );

		CHECK ( offsets[ 0] ==     33 );
		CHECK ( offsets[ 1] ==   5225 );
		CHECK ( offsets[ 2] ==   7390 );
		CHECK ( offsets[ 3] ==  23380 );
		CHECK ( offsets[ 4] ==  35608 );
		CHECK ( offsets[ 5] ==  49820 );
		CHECK ( offsets[ 6] ==  69508 );
		CHECK ( offsets[ 7] ==  87733 );
		CHECK ( offsets[ 8] == 106333 );
		CHECK ( offsets[ 9] == 139495 );
		CHECK ( offsets[10] == 157863 );
		CHECK ( offsets[11] == 198495 );
		CHECK ( offsets[12] == 213368 );
		CHECK ( offsets[13] == 225320 );
		CHECK ( offsets[14] == 234103 );
	}
}


TEST_CASE ( "toc::get_filenames", "[identifier]" )
{
	using arcstk::toc::get_filenames;

	using arcstk::details::TOCBuilder;

	SECTION ( "Returns empty list when TOC does not contain filenames" )
	{
		// "Bach: Organ Concertos", Simon Preston, DGG
		auto toc0 = TOCBuilder::build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038
		);

		auto fnames = get_filenames(*toc0);

		CHECK ( fnames.empty() );
	}

	SECTION ( "Returns list of size track_count when TOC contains 1 filename" )
	{
		// "Bach: Organ Concertos", Simon Preston, DGG
		auto toc0 = TOCBuilder::build(
			// track count
			15,
			// offsets
			std::vector<arcstk::lba_count_t>{ 33, 5225, 7390, 23380, 35608, 49820,
			69508, 87733, 106333, 139495, 157863, 198495, 213368, 225320,
			234103 },
			// leadout
			253038,
			// filenames
			std::vector<std::string>{ "file", "file", "file", "file", "file",
			"file", "file", "file", "file", "file",
			"file", "file", "file", "file", "file" }
		);

		auto fnames = get_filenames(*toc0);

		CHECK ( fnames.size() == 15 );
	}

	SECTION ( "Returns list of size track_count when TOC contains"
			" multiple filenames" )
	{
		// "Bach: Organ Concertos", Simon Preston, DGG
		auto toc0 = TOCBuilder::build(
			// track count
			15,
			// offsets
			std::vector<arcstk::lba_count_t>{ 33, 5225, 7390, 23380, 35608, 49820,
			69508, 87733, 106333, 139495, 157863, 198495, 213368, 225320,
			234103 },
			// leadout
			253038,
			// filenames
			std::vector<std::string>{ "file1", "file2", "file3", "file4",
			"file5", "file6", "file7", "file8", "file9", "file10",
			"file11", "file12", "file13", "file14", "file15" }
		);

		auto fnames = get_filenames(*toc0);

		CHECK ( fnames.size() == 15 );
	}
}


TEST_CASE ( "get_track", "[identifier] [make_toc]" )
{
	using arcstk::details::get_track;

	auto container1 = std::vector<uint32_t>{ 0, 1, 2, 3, 4, 5 };
	auto container2 = std::list<uint32_t>  { 0, 1, 2, 3, 4, 5 };
	auto container3 = std::list<int32_t>   { 0, 1, 2, 3, 4, 5 };

	CHECK_THROWS ( get_track(container1, 0) );
	CHECK_THROWS ( get_track(container1, 7) );

	CHECK ( get_track(container1, 1) == 0 );
	CHECK ( get_track(container1, 2) == 1 );
	CHECK ( get_track(container1, 3) == 2 );
	CHECK ( get_track(container1, 4) == 3 );
	CHECK ( get_track(container1, 5) == 4 );
	CHECK ( get_track(container1, 6) == 5 );

	CHECK_THROWS ( get_track(container2, 0) );
	CHECK_THROWS ( get_track(container2, 7) );

	CHECK ( get_track(container2, 1) == 0 );
	CHECK ( get_track(container2, 2) == 1 );
	CHECK ( get_track(container2, 3) == 2 );
	CHECK ( get_track(container2, 4) == 3 );
	CHECK ( get_track(container2, 5) == 4 );
	CHECK ( get_track(container2, 6) == 5 );

	CHECK_THROWS ( get_track(container3, 0) );
	CHECK_THROWS ( get_track(container3, 7) );

	CHECK ( get_track(container3, 1) == 0 );
	CHECK ( get_track(container3, 2) == 1 );
	CHECK ( get_track(container3, 3) == 2 );
	CHECK ( get_track(container3, 4) == 3 );
	CHECK ( get_track(container3, 5) == 4 );
	CHECK ( get_track(container3, 6) == 5 );
}


TEST_CASE ( "calculate_leadout", "[identifier] [make_toc]" )
{
	using arcstk::details::calculate_leadout;

	auto offsets1 = std::vector<uint32_t>{ 33, 69163, 87321 };
	auto lengths1 = std::vector<uint32_t>{ 69130, 18158, 49123 };

	// identical to 1 except offset[0] is 0
	auto offsets2 = std::vector<uint32_t>{ 0, 69163, 87321 };
	auto lengths2 = std::vector<uint32_t>{ 69163, 18158, 49123 };

	// identical to 2 except length[2] is different
	auto offsets3 = std::list<uint32_t>{ 0, 69163, 87321 };
	auto lengths3 = std::list<uint32_t>{ 69163, 18158, 21002 };

	auto leadout1 = calculate_leadout(lengths1, offsets1);
	auto leadout2 = calculate_leadout(lengths2, offsets2);
	auto leadout3 = calculate_leadout(lengths3, offsets3);

	CHECK ( leadout1 == 136444 );
	CHECK ( leadout2 == 136444 );
	CHECK ( leadout3 == 108323 );
}


TEST_CASE ( "is_lba_container", "[identifier] [make_toc]" )
{
	using arcstk::details::has_lba_value_type;
	using arcstk::details::has_const_iterator;
	using arcstk::details::has_size;
	using arcstk::details::has_begin;
	using arcstk::details::has_end;
	using arcstk::details::is_lba_container;

	SECTION ( "has_lba_value_type for some std containers" )
	{
		CHECK ( has_lba_value_type<std::vector<int32_t>>::value  );
		CHECK ( has_lba_value_type<std::vector<uint32_t>>::value );
		CHECK ( has_lba_value_type<std::list<int32_t>>::value    );
		CHECK ( has_lba_value_type<std::list<uint32_t>>::value   );

		//CHECK ( has_lba_value_type<std::vector<char>>::value     );
	}

	SECTION ( "has_const_iterator for some std containers" )
	{
		CHECK ( has_const_iterator<std::vector<char>>::value     );
		CHECK ( has_const_iterator<std::vector<int32_t>>::value  );
		CHECK ( has_const_iterator<std::vector<uint32_t>>::value );
		CHECK ( has_const_iterator<std::list<int32_t>>::value    );
		CHECK ( has_const_iterator<std::list<uint32_t>>::value   );

		CHECK ( has_const_iterator<const std::vector<int32_t>>::value  );
	}

	SECTION ( "has_size for some std containers" )
	{
		CHECK ( has_size<std::vector<char>>::value     );
		CHECK ( has_size<std::vector<int32_t>>::value  );
		CHECK ( has_size<std::vector<uint32_t>>::value );
		CHECK ( has_size<std::list<int32_t>>::value    );
		CHECK ( has_size<std::list<uint32_t>>::value   );

		CHECK ( has_size<const std::vector<int32_t>>::value  );
	}

	SECTION ( "has_begin for some std containers" )
	{
		CHECK ( has_begin<std::vector<char>>::value     );
		CHECK ( has_begin<std::vector<int32_t>>::value  );
		CHECK ( has_begin<std::vector<uint32_t>>::value );
		CHECK ( has_begin<std::list<int32_t>>::value    );
		CHECK ( has_begin<std::list<uint32_t>>::value   );
	}

	SECTION ( "has_end for some std containers" )
	{
		CHECK ( has_end<std::vector<char>>::value     );
		CHECK ( has_end<std::vector<int32_t>>::value  );
		CHECK ( has_end<std::vector<uint32_t>>::value );
		CHECK ( has_end<std::list<int32_t>>::value    );
		CHECK ( has_end<std::list<uint32_t>>::value   );
	}

	SECTION ( "is_lba_container for non-refererence types" )
	{
		CHECK ( is_lba_container<std::vector<int32_t>>::value     );
		CHECK ( is_lba_container<std::vector<uint32_t>>::value    );
		CHECK ( is_lba_container<std::list<int32_t>>::value       );
		CHECK ( is_lba_container<std::list<uint32_t>>::value      );
		CHECK ( is_lba_container<std::array<int32_t,   1>>::value );
		CHECK ( is_lba_container<std::array<int32_t,  99>>::value );
		CHECK ( is_lba_container<std::array<uint32_t,  1>>::value );
		CHECK ( is_lba_container<std::array<uint32_t, 99>>::value );
		CHECK ( is_lba_container<std::list<int32_t>>::value       );
		CHECK ( is_lba_container<std::list<uint32_t>>::value      );

		//CHECK ( is_lba_container<std::vector<char>>::value        );
	}

	SECTION ( "is_lba_container for references" )
	{
		CHECK ( is_lba_container<std::vector<int32_t> &>::value     );
		CHECK ( is_lba_container<std::vector<uint32_t> &>::value    );
		CHECK ( is_lba_container<std::list<int32_t> &>::value       );
		CHECK ( is_lba_container<std::list<uint32_t> &>::value      );
		CHECK ( is_lba_container<std::array<int32_t,   1> &>::value );
		CHECK ( is_lba_container<std::array<int32_t,  99> &>::value );
		CHECK ( is_lba_container<std::array<uint32_t,  1> &>::value );
		CHECK ( is_lba_container<std::array<uint32_t, 99> &>::value );
		CHECK ( is_lba_container<std::list<int32_t> &>::value       );
		CHECK ( is_lba_container<std::list<uint32_t> &>::value      );

		//CHECK ( is_lba_container<std::vector<char> &>::value        );
	}
}


TEST_CASE ("is_filename_container", "[identifier] [make_toc]")
{
	using arcstk::details::is_filename_container;

	SECTION ( "is_filename_container for non-reference types" )
	{
		CHECK ( is_filename_container<std::vector<std::string>>::value     );
		CHECK ( is_filename_container<std::list<std::string>>::value       );
		CHECK ( is_filename_container<std::array<std::string,   1>>::value );
		CHECK ( is_filename_container<std::array<std::string,  99>>::value );
		CHECK ( is_filename_container<std::list<std::string>>::value       );

		//CHECK ( is_filename_container<std::vector<char>>::value        );
	}

	SECTION ( "is_filename_container for reference types" )
	{
		CHECK ( is_filename_container<std::vector<std::string> &>::value     );
		CHECK ( is_filename_container<std::list<std::string> &>::value       );
		CHECK ( is_filename_container<std::array<std::string,   1> &>::value );
		CHECK ( is_filename_container<std::array<std::string,  99> &>::value );
		CHECK ( is_filename_container<std::list<std::string> &>::value       );

		//CHECK ( is_filename_container<std::vector<char> &>::value        );
	}
}


// ARId


TEST_CASE ( "ARId", "[identifier] [arid]" )
{
	using arcstk::ARId;

	ARId id(10, 0x02c34fd0, 0x01f880cc, 0xbc55023f);


	SECTION ( "Constructor" )
	{
		CHECK ( id.track_count() == 10 );
		CHECK ( id.disc_id_1()   == 0x02c34fd0 );
		CHECK ( id.disc_id_2()   == 0x01f880cc );
		CHECK ( id.cddb_id()     == 0xbc55023f );

		CHECK ( id.url()         ==
				"http://www.accuraterip.com/accuraterip/0/d/f/dBAR-010-02c34fd0-01f880cc-bc55023f.bin" );
		CHECK ( id.filename()    ==
				"dBAR-010-02c34fd0-01f880cc-bc55023f.bin" );

		CHECK ( not id.empty() );
	}


	SECTION ( "Equality and inequality operators" )
	{
		ARId same_id(10, 0x02c34fd0, 0x01f880cc, 0xbc55023f);

		CHECK ( id == id );
		CHECK ( not (id != id) );

		CHECK ( id == same_id );
		CHECK ( not (id != same_id) );


		ARId id1(15, 0x001B9178, 0x014BE24E, 0xB40D2D0F);
		ARId id2(16, 0x001B9178, 0x014BE24E, 0xB40D2D0F); // different track
		ARId id3(15, 0x001B9179, 0x014BE24E, 0xB40D2D0F); // different id 1
		ARId id4(15, 0x001B9178, 0x014BE24D, 0xB40D2D0F); // different id 2
		ARId id5(15, 0x001B9178, 0x014BE24E, 0xC40D2D0F); // different cddb id

		CHECK ( id1 != id2 );
		CHECK ( not (id1 == id2) );

		CHECK ( id1 != id3 );
		CHECK ( not (id1 == id3) );

		CHECK ( id1 != id4 );
		CHECK ( not (id1 == id4) );

		CHECK ( id1 != id5 );
		CHECK ( not (id1 == id5) );

		CHECK ( id2 != id3 );
		CHECK ( not (id2 == id3) );

		CHECK ( id2 != id4 );
		CHECK ( not (id2 == id4) );

		CHECK ( id2 != id5 );
		CHECK ( not (id2 == id5) );

		CHECK ( id3 != id4 );
		CHECK ( not (id3 == id4) );

		CHECK ( id3 != id5 );
		CHECK ( not (id3 == id5) );

		CHECK ( id4 != id5 );
		CHECK ( not (id4 == id5) );
	}


	SECTION ( "Copy constructor" )
	{
		ARId copied_id(id);


		CHECK ( copied_id.track_count() == 10 );
		CHECK ( copied_id.disc_id_1()   == 0x02c34fd0 );
		CHECK ( copied_id.disc_id_2()   == 0x01f880cc );
		CHECK ( copied_id.cddb_id()     == 0xbc55023f );

		CHECK ( copied_id.url()         ==
				"http://www.accuraterip.com/accuraterip/0/d/f/dBAR-010-02c34fd0-01f880cc-bc55023f.bin" );
		CHECK ( copied_id.filename()    ==
				"dBAR-010-02c34fd0-01f880cc-bc55023f.bin" );

		CHECK ( not copied_id.empty() );
	}


	SECTION ( "Copy assignment operator" )
	{
		ARId other_id(11, 0x02c34fd0, 0x04e880bb, 0xbc55023f);
		id = other_id;

		CHECK ( id == other_id );

		CHECK ( id.track_count() == 11 );
		CHECK ( id.disc_id_1()   == 0x02c34fd0 );
		CHECK ( id.disc_id_2()   == 0x04e880bb );
		CHECK ( id.cddb_id()     == 0xbc55023f );

		CHECK ( id.url()         ==
				"http://www.accuraterip.com/accuraterip/0/d/f/dBAR-011-02c34fd0-04e880bb-bc55023f.bin" );
		CHECK ( id.filename()    ==
				"dBAR-011-02c34fd0-04e880bb-bc55023f.bin" );

		CHECK ( not id.empty() );
	}
}


// make_arid


TEST_CASE ( "make_arid builds valid ARIds", "[identifier] [aridbuilder]" )
{
	SECTION ( "ARId from track_count, offsets, leadout, example 1" )
	{
		// "Bach: Organ Concertos", Simon Preston, DGG

		std::unique_ptr<arcstk::ARId> id1 = arcstk::make_arid(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038
		);

		CHECK ( id1->track_count() == 15 );
		CHECK ( id1->disc_id_1()   == 0x001b9178 );
		CHECK ( id1->disc_id_2()   == 0x014be24e );
		CHECK ( id1->cddb_id()     == 0xb40d2d0f );

		CHECK ( id1->url()         ==
				"http://www.accuraterip.com/accuraterip/8/7/1/dBAR-015-001b9178-014be24e-b40d2d0f.bin" );
		CHECK ( id1->filename()    ==
				"dBAR-015-001b9178-014be24e-b40d2d0f.bin" );

		CHECK ( not id1->empty() );
	}


	SECTION ( "ARId from track_count, offsets, leadout, example 2" )
	{
		// "Saint-Saens: Symphony No. 3, Poulenc: Organ Concerto",
		// Berliner Sinfonie-Orchester, C.-P. Flor, ETERNA

		std::unique_ptr<arcstk::ARId> id2 = arcstk::make_arid(
			// track count
			3,
			// offsets
			{ 32, 96985, 166422 },
			// leadout
			264957
		);

		CHECK ( id2->track_count() == 3 );
		CHECK ( id2->disc_id_1()   == 0x0008100c );
		CHECK ( id2->disc_id_2()   == 0x001ac008 );
		CHECK ( id2->cddb_id()     == 0x190dcc03 );

		CHECK ( id2->url()         ==
				"http://www.accuraterip.com/accuraterip/c/0/0/dBAR-003-0008100c-001ac008-190dcc03.bin" );
		CHECK ( id2->filename()    ==
				"dBAR-003-0008100c-001ac008-190dcc03.bin" );

		CHECK ( not id2->empty() );
	}


	SECTION ( "ARId from track_count, offsets, leadout, example 3" )
	{
		// "Bach: Brandenburg Concertos 3,4 & 5",
		// Academy of St.-Martin-in-the-Fields, Sir Neville Marriner, Philips

		std::unique_ptr<arcstk::ARId> id3 = arcstk::make_arid(
			// track count
			9,
			// offsets
			{ 33, 34283, 49908, 71508, 97983, 111183, 126708, 161883, 187158 },
			// leadout
			210143
		);


		CHECK ( id3->track_count() == 9 );
		CHECK ( id3->disc_id_1()   == 0x001008a6 );
		CHECK ( id3->disc_id_2()   == 0x007469b8 );
		CHECK ( id3->cddb_id()     == 0x870af109 );

		CHECK ( id3->url()         ==
				"http://www.accuraterip.com/accuraterip/6/a/8/dBAR-009-001008a6-007469b8-870af109.bin" );
		CHECK ( id3->filename()    ==
				"dBAR-009-001008a6-007469b8-870af109.bin" );

		CHECK ( not id3->empty() );
	}


	SECTION ( "ARId from track_count, offsets, leadout, example 4" )
	{
		// Bent: "Programmed to Love"

		std::unique_ptr<arcstk::ARId> id4 = arcstk::make_arid(
			// track count
			18,
			// offsets
			{ 0, 29042, 53880, 58227, 84420, 94192, 119165, 123030, 147500,
				148267, 174602, 208125, 212705, 239890, 268705, 272055, 291720,
				319992 },
			// leadout
			332075
		);


		CHECK ( id4->track_count() == 18 );
		CHECK ( id4->disc_id_1()   == 0x00307c78 );
		CHECK ( id4->disc_id_2()   == 0x0281351d );
		CHECK ( id4->cddb_id()     == 0x27114b12 );

		CHECK ( id4->url()         ==
				"http://www.accuraterip.com/accuraterip/8/7/c/dBAR-018-00307c78-0281351d-27114b12.bin" );
		CHECK ( id4->filename()    ==
				"dBAR-018-00307c78-0281351d-27114b12.bin" );

		CHECK ( not id4->empty() );
	}


	SECTION ( "ARId from track_count, offsets, leadout, example 5" )
	{
		// "Wir entdecken Komponisten: Ludwig van Beethoven Vol. 1", DGG

		std::unique_ptr<arcstk::ARId> id5 = arcstk::make_arid(
			// track count
			1,
			// offsets
			{ 33 },
			// leadout
			233484
		);


		CHECK ( id5->track_count() == 1 );
		CHECK ( id5->disc_id_1()   == 0x0003902d );
		CHECK ( id5->disc_id_2()   == 0x00072039 );
		CHECK ( id5->cddb_id()     == 0x020c2901 );

		CHECK ( id5->url()         ==
				"http://www.accuraterip.com/accuraterip/d/2/0/dBAR-001-0003902d-00072039-020c2901.bin" );
		CHECK ( id5->filename()    ==
				"dBAR-001-0003902d-00072039-020c2901.bin" );

		CHECK ( not id5->empty() );
	}
}


TEST_CASE ( "make_arid refuses to build invalid ARIds",
		"[identifier] [aridbuilder]" )
{
	using arcstk::CDDA;

	SECTION ( "Build fails for inconsistent offsets" )
	{
		// one track too short (no minimal distance)
		// offset[1] has not minimal distance to offset[0]

		CHECK_THROWS ( arcstk::make_arid(
			// track count
			15,
			// offsets
			{ 33, 34 /* BOOM */, 7390, 23380, 35608, 49820, 69508, 87733,
				106333, 139495, 157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038
		));

		// one offset exceeds legal maximum
		// offset[14] exceeds maximal block address

		CHECK_THROWS ( arcstk::make_arid(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733,
				106333, 139495, 157863, 198495, 213368, 225320,
				CDDA.MAX_OFFSET + 1 /* BOOM */ },
			// leadout
			253038
		));

		// not ascending order
		// offsets[9] is smaller than offsets[8]

		CHECK_THROWS ( arcstk::make_arid(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733,
				106333, 106000 /* BOOM */, 157863, 198495, 213368, 225320,
				234103 },
			// leadout
			253038
		));

		// two offsets equal
		// offsets[9] is equal to offsets[8]

		CHECK_THROWS ( arcstk::make_arid(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733,
				106333, 106333 /* BOOM */, 157863, 198495, 213368, 225320,
				234103 },
			// leadout
			253038
		));
	}


	SECTION ( "Build fails for offsets and leadout inconsistent" )
	{
		// Leadout 0 is illegal (smaller than minimum)

		CHECK_THROWS ( arcstk::make_arid(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
			// leadout
			0 /* BOOM */
		));

		// Leadout exceeds maximal legal value

		CHECK_THROWS ( arcstk::make_arid(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
			// leadout
			CDDA.MAX_BLOCK_ADDRESS + 1 /* BOOM */
		));

		// Leadout is smaller than biggest offset

		CHECK_THROWS ( arcstk::make_arid(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
			// leadout
			234095 /* BOOM */
		));

		// Leadout is equal to biggest offset

		CHECK_THROWS ( arcstk::make_arid(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
			// leadout
			234103 /* BOOM */
		));

		// Leadout has not minimal distance to biggest offset

		CHECK_THROWS ( arcstk::make_arid(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
			// leadout
			234103 + CDDA.MIN_TRACK_LEN_FRAMES - 1 /* BOOM */
		));
	}


	SECTION ( "Build fails for offsets and trackcount inconsistent" )
	{
		// illegal track count: smaller than offsets count

		CHECK_THROWS ( arcstk::make_arid(
			// track count
			14, /* BOOM */
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038
		));

		CHECK_THROWS ( arcstk::make_arid(
			// track count
			8, /* BOOM */
			// offsets
			{ 33, 34283, 49908, 71508, 97983, 111183, 126708, 161883, 187158 },
			// leadout
			210143
		));

		// illegal track count: bigger than offsets count

		CHECK_THROWS ( arcstk::make_arid(
			// track count
			16, /* BOOM */
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038
		));

		CHECK_THROWS ( arcstk::make_arid(
			// track count
			9, /* BOOM */
			// offsets
			{ 33, 34283, 49908, 71508, 97983, 111183, 126708,
				161883/*, 187158 */ },
			// leadout
			210143
		));

		// illegal track count: smaller than minimum

		CHECK_THROWS ( arcstk::make_arid(
			// track count
			0, /* BOOM */
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038
		));

		// illegal track count: bigger than maximum

		CHECK_THROWS ( arcstk::make_arid(
			// track count
			CDDA.MAX_TRACKCOUNT+1, /* BOOM */
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038
		));
	}
}


TEST_CASE ( "make_arid builds empty ARIds", "[identifier] [aridbuilder]" )
{
	SECTION ( "Empty ARId" )
	{
		std::unique_ptr<arcstk::ARId> empty_id = arcstk::make_empty_arid();


		CHECK ( empty_id->track_count() == 0 );
		CHECK ( empty_id->disc_id_1()   == 0x00000000 );
		CHECK ( empty_id->disc_id_2()   == 0x00000000 );
		CHECK ( empty_id->cddb_id()     == 0x00000000 );

		CHECK ( empty_id->url()         ==
				"http://www.accuraterip.com/accuraterip/0/0/0/dBAR-000-00000000-00000000-00000000.bin" );
		CHECK ( empty_id->filename()    ==
				"dBAR-000-00000000-00000000-00000000.bin" );

		CHECK ( empty_id->empty() );
	}
}


// TOCValidator


TEST_CASE ( "TOCValidator", "[identifier]" )
{
	using arcstk::details::TOCValidator;
	using arcstk::CDDA;
	using arcstk::lba_count_t;


	SECTION ( "Validation succeeds for correct offsets" )
	{
		// some legal values
		CHECK_NOTHROW ( TOCValidator::validate_offsets({ 33, 5225, 7390, 23380,
				35608, 49820, 69508, 87733, 106333, 139495, 157863, 198495,
				213368, 225320, 234103 })
		);

		// some legal values
		CHECK_NOTHROW ( TOCValidator::validate_offsets(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 })
		);

		// some legal values
		CHECK_NOTHROW ( TOCValidator::validate(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038)
		);
	}


	SECTION ( "Validation fails for incorrect offsets" )
	{
		// offset[1] has less than minimal legal distance to offset[0]
		CHECK_THROWS ( TOCValidator::validate_offsets(
			{ 33, 34 /* BOOM */, 7390, 23380, 35608, 49820, 69508, 87733,
				106333, 139495, 157863, 198495, 213368, 225320, 234103 }
		));

		// offset[14] bigger than legal maximum
		CHECK_THROWS ( TOCValidator::validate_offsets(
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320,
				CDDA.MAX_OFFSET + 1 /* BOOM */ }
		));

		// offset[6] is greater than offset[7]
		CHECK_THROWS ( TOCValidator::validate_offsets(
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 69507 /* BOOM */,
				106333, 139495, 157863, 198495, 213368, 225320, 234103 }
		));

		// offset[7] and offset[8] are equal
		CHECK_THROWS ( TOCValidator::validate_offsets(
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 69508 /* BOOM */,
				106333, 139495, 157863, 198495, 213368, 225320, 234103 }
		));

		// track count bigger than legal maximum
		CHECK_THROWS ( TOCValidator::validate_offsets(
					std::vector<lba_count_t>(100)) );

		// track count smaller than legal minimum
		CHECK_THROWS ( TOCValidator::validate_offsets(
					std::vector<lba_count_t>()) );
	}


	SECTION ( "Validation succeeds for correct lengths" )
	{
		// complete correct lengths
		CHECK_NOTHROW ( TOCValidator::validate_lengths(
			{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075,
				18368, 40152, 14798, 11952, 8463, 18935 })
		);

		// incomplete correct lengths
		CHECK_NOTHROW ( TOCValidator::validate_lengths(
			{ 5192, 2165, 15885, -1 }) );
	}


	SECTION ( "Validation fails for incorrect lengths" )
	{
		// one length smaller than legal minimum
		CHECK_THROWS ( TOCValidator::validate_lengths(
			{ 5192, 2165, 15885,
				CDDA.MIN_TRACK_LEN_FRAMES - 1 /* BOOM */,
				5766 }
		));

		// sum of lengths greater than legal maximum of 99 min
		CHECK_THROWS ( TOCValidator::validate_lengths(
			{ 5192, 2165, 15885,
				360000 /* Redbook maximum */,
				100000 /* Exceeds maximum of 99 min */,
				-1 }
		));

		// track count bigger than legal maximum
		CHECK_THROWS ( TOCValidator::validate_lengths(
					std::vector<lba_count_t>(100)) );

		// last length smaller than legal minimum
		CHECK_THROWS ( TOCValidator::validate_lengths(
			{ 5192, 2165, 15885, CDDA.MIN_TRACK_LEN_FRAMES - 1 /* BOOM */ }
		));

		// track count smaller than legal minimum
		CHECK_THROWS (TOCValidator::validate_lengths(std::vector<lba_count_t>()));
	}


	SECTION ( "Validation succeeds for correct leadouts" )
	{
		// legal minimum
		CHECK_NOTHROW (
			TOCValidator::validate_leadout(CDDA.MIN_TRACK_OFFSET_DIST)
		);

		// some legal value
		CHECK_NOTHROW ( TOCValidator::validate_leadout(253038) );

		// legal maximum
		CHECK_NOTHROW ( TOCValidator::validate_leadout(CDDA.MAX_OFFSET)     );
		CHECK_THROWS  ( TOCValidator::validate_leadout(CDDA.MAX_OFFSET + 1) );

		// TODO more values
	}


	SECTION ( "Validation fails for non-standard leadouts" )
	{
		// legal maximum
		CHECK_THROWS ( TOCValidator::validate_leadout(CDDA.MAX_BLOCK_ADDRESS) );
	}


	SECTION ( "Validation fails for incorrect leadouts" )
	{
		// 0 (smaller than legal minimum)
		CHECK_THROWS ( TOCValidator::validate_leadout(0) );

		// greater than 0, but smaller than legal minimum
		CHECK_THROWS ( TOCValidator::validate_leadout(
			CDDA.MIN_TRACK_OFFSET_DIST - 1
		));

		// bigger than legal maximum
		CHECK_THROWS ( TOCValidator::validate_leadout(
			CDDA.MAX_BLOCK_ADDRESS + 1
		));
	}


	SECTION ( "Validation succeeds for correct trackcounts" )
	{
		// legal minimum
		CHECK_NOTHROW ( TOCValidator::validate_trackcount(1) );

		// legal values greater than minimum and smaller than maximum
		for (int i = 2; i < 99; ++i)
		{
			// 2 - 98
			CHECK_NOTHROW ( TOCValidator::validate_trackcount(i) );
		}

		// legal maximum
		CHECK_NOTHROW ( TOCValidator::validate_trackcount(99) );
	}


	SECTION ( "Validation fails for incorrect trackcounts" )
	{
		// smaller than legal minimum
		CHECK_THROWS ( TOCValidator::validate_trackcount(0) );

		// bigger than legal maximum
		CHECK_THROWS ( TOCValidator::validate_trackcount(100) );

		// bigger than legal maximum
		CHECK_THROWS ( TOCValidator::validate_trackcount(500) );

		// bigger than legal maximum
		CHECK_THROWS ( TOCValidator::validate_trackcount(999) );

		// bigger than legal maximum
		CHECK_THROWS ( TOCValidator::validate_trackcount(65535) );
	}
}


// TOCBuilder


TEST_CASE ( "TOCBuilder: build with leadout", "[identifier] [tocbuilder]" )
{
	using arcstk::details::TOCBuilder;


	SECTION ( "Build succeeds for correct trackcount, offsets, leadout" )
	{
		// "Bach: Organ Concertos", Simon Preston, DGG
		auto toc0 = TOCBuilder::build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038
		);


		CHECK ( toc0->track_count() == 15 );
		CHECK ( toc0->leadout()     == 253038 );

		CHECK_THROWS ( toc0->offset(0)  == 0 );

		CHECK ( toc0->offset(1)  ==     33 );
		CHECK ( toc0->offset(2)  ==   5225 );
		CHECK ( toc0->offset(3)  ==   7390 );
		CHECK ( toc0->offset(4)  ==  23380 );
		CHECK ( toc0->offset(5)  ==  35608 );
		CHECK ( toc0->offset(6)  ==  49820 );
		CHECK ( toc0->offset(7)  ==  69508 );
		CHECK ( toc0->offset(8)  ==  87733 );
		CHECK ( toc0->offset(9)  == 106333 );
		CHECK ( toc0->offset(10) == 139495 );
		CHECK ( toc0->offset(11) == 157863 );
		CHECK ( toc0->offset(12) == 198495 );
		CHECK ( toc0->offset(13) == 213368 );
		CHECK ( toc0->offset(14) == 225320 );
		CHECK ( toc0->offset(15) == 234103 );

		CHECK_THROWS ( toc0->offset(16) == 0 );
	}
}


TEST_CASE ( "TOCBuilder: build with lengths and files",
	"[identifier] [tocbuilder]" )
{
	using arcstk::details::TOCBuilder;


	SECTION ( "Build succeeds for correct trackcount, offsets, lengths" )
	{
		// "Bach: Organ Concertos", Simon Preston, DGG
		auto toc1 = TOCBuilder::build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// lengths
			{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075, 18368,
				40152, 14798, 11952, 8463, 18935 }
		);

		CHECK ( toc1->track_count() == 15 );
		CHECK ( toc1->leadout()     == 253038 );

		CHECK_THROWS ( toc1->offset(0)  ==      0 );

		CHECK ( toc1->offset(1)  ==     33 );
		CHECK ( toc1->offset(2)  ==   5225 );
		CHECK ( toc1->offset(3)  ==   7390 );
		CHECK ( toc1->offset(4)  ==  23380 );
		CHECK ( toc1->offset(5)  ==  35608 );
		CHECK ( toc1->offset(6)  ==  49820 );
		CHECK ( toc1->offset(7)  ==  69508 );
		CHECK ( toc1->offset(8)  ==  87733 );
		CHECK ( toc1->offset(9)  == 106333 );
		CHECK ( toc1->offset(10) == 139495 );
		CHECK ( toc1->offset(11) == 157863 );
		CHECK ( toc1->offset(12) == 198495 );
		CHECK ( toc1->offset(13) == 213368 );
		CHECK ( toc1->offset(14) == 225320 );
		CHECK ( toc1->offset(15) == 234103 );

		CHECK_THROWS ( toc1->offset(16) == 0 );

		CHECK_THROWS ( toc1->parsed_length(0) == 0 );

		CHECK ( toc1->parsed_length(1)  ==  5192 );
		CHECK ( toc1->parsed_length(2)  ==  2165 );
		CHECK ( toc1->parsed_length(3)  == 15885 );
		CHECK ( toc1->parsed_length(4)  == 12228 );
		CHECK ( toc1->parsed_length(5)  == 13925 );
		CHECK ( toc1->parsed_length(6)  == 19513 );
		CHECK ( toc1->parsed_length(7)  == 18155 );
		CHECK ( toc1->parsed_length(8)  == 18325 );
		CHECK ( toc1->parsed_length(9)  == 33075 );
		CHECK ( toc1->parsed_length(10) == 18368 );
		CHECK ( toc1->parsed_length(11) == 40152 );
		CHECK ( toc1->parsed_length(12) == 14798 );
		CHECK ( toc1->parsed_length(13) == 11952 );
		CHECK ( toc1->parsed_length(14) ==  8463 );
		CHECK ( toc1->parsed_length(15) == 18935 );

		CHECK_THROWS ( toc1->parsed_length(16) == 0 );
	}


	SECTION ( "Build succeeds for trackcount, offsets and incomplete lengths" )
	{
		auto toc2 = TOCBuilder::build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// lengths
			{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075,
				18368, 40152, 14798, 11952, 8463, -1 /* instead of 18935 */ }
		);


		CHECK ( toc2->track_count() == 15 );
		CHECK ( toc2->leadout()     == 0 ); // missing !

		CHECK_THROWS ( toc2->offset(0)  ==      0 );

		CHECK ( toc2->offset(1)  ==     33 );
		CHECK ( toc2->offset(2)  ==   5225 );
		CHECK ( toc2->offset(3)  ==   7390 );
		CHECK ( toc2->offset(4)  ==  23380 );
		CHECK ( toc2->offset(5)  ==  35608 );
		CHECK ( toc2->offset(6)  ==  49820 );
		CHECK ( toc2->offset(7)  ==  69508 );
		CHECK ( toc2->offset(8)  ==  87733 );
		CHECK ( toc2->offset(9)  == 106333 );
		CHECK ( toc2->offset(10) == 139495 );
		CHECK ( toc2->offset(11) == 157863 );
		CHECK ( toc2->offset(12) == 198495 );
		CHECK ( toc2->offset(13) == 213368 );
		CHECK ( toc2->offset(14) == 225320 );
		CHECK ( toc2->offset(15) == 234103 );

		CHECK_THROWS ( toc2->offset(16) == 0 );

		CHECK_THROWS ( toc2->parsed_length(0) == 0 ); // as defined!

		CHECK ( toc2->parsed_length(1)  ==  5192 );
		CHECK ( toc2->parsed_length(2)  ==  2165 );
		CHECK ( toc2->parsed_length(3)  == 15885 );
		CHECK ( toc2->parsed_length(4)  == 12228 );
		CHECK ( toc2->parsed_length(5)  == 13925 );
		CHECK ( toc2->parsed_length(6)  == 19513 );
		CHECK ( toc2->parsed_length(7)  == 18155 );
		CHECK ( toc2->parsed_length(8)  == 18325 );
		CHECK ( toc2->parsed_length(9)  == 33075 );
		CHECK ( toc2->parsed_length(10) == 18368 );
		CHECK ( toc2->parsed_length(11) == 40152 );
		CHECK ( toc2->parsed_length(12) == 14798 );
		CHECK ( toc2->parsed_length(13) == 11952 );
		CHECK ( toc2->parsed_length(14) ==  8463 );
		CHECK ( toc2->parsed_length(15) ==     0 );  // missing !

		CHECK_THROWS ( toc2->parsed_length(16) == 0 );
	}
}


TEST_CASE ( "TOCBuilder: build fails with illegal values",
	"[identifier] [tocbuilder]" )
{
	using arcstk::details::TOCBuilder;
	using arcstk::CDDA;
	using arcstk::lba_count_t;

	SECTION ( "Build fails for incorrect offsets" )
	{
		// no minimal distance: with leadout
		CHECK_THROWS ( TOCBuilder::build(
			// track count
			15,
			// offsets (offset[1] has not minimal distance to offset[0])
			{ 33, 34 /* BOOM */, 7390, 23380, 35608, 49820, 69508, 87733,
				106333, 139495, 157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038
			)
		);

		// no minimal distance: with lengths + files
		CHECK_THROWS ( TOCBuilder::build(
			// track count
			15,
			// offsets (offset[1] has not minimal distance to offset[0])
			{ 33, 34 /* BOOM */, 7390, 23380, 35608, 49820, 69508, 87733,
				106333, 139495, 157863, 198495, 213368, 225320, 234103 },
			// lengths
			{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075,
				18368, 40152, 14798, 11952, 8463, 18935 }
			)
		);

		// exeeds maximum: with leadout
		CHECK_THROWS ( TOCBuilder::build(
			// track count
			15,
			// offsets (offset[14] exceeds maximal block address)
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, CDDA.MAX_OFFSET + 1/* BOOM */ },
			// leadout
			253038
			)
		);

		// exeeds maximum: with lengths + files
		CHECK_THROWS ( TOCBuilder::build(
			// track count
			15,
			// offsets (offset[14] exceeds maximal block address)
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333,
				139495, 157863, 198495, 213368, 225320,
				CDDA.MAX_OFFSET + 1 /* BOOM */ },
			// lengths
			{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075,
				18368, 40152, 14798, 11952, 8463, 18935 }
			)
		);

		// not ascending order: with leadout
		CHECK_THROWS ( TOCBuilder::build(
			// track count
			15,
			// offsets (offsets[9] is smaller than offsets[8])
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333,
				106000 /* BOOM */, 157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038
			)
		);

		// not ascending order: with lengths + files
		CHECK_THROWS ( TOCBuilder::build(
			// track count
			15,
			// offsets (offsets[9] is smaller than offsets[8])
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333,
				106000 /* BOOM */, 157863, 198495, 213368, 225320, 234103 },
			// lengths
			{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075,
				18368, 40152, 14798, 11952, 8463, 18935 }
			)
		);

		// not ascending order: with leadout
		CHECK_THROWS ( TOCBuilder::build(
			// track count
			15,
			// offsets (offsets[9] is equal to offsets[8])
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333,
				106333 /* BOOM */, 157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038
			)
		);

		// not ascending order: with lengths + files
		CHECK_THROWS ( TOCBuilder::build(
			// track count
			15,
			// offsets (offsets[9] is equal to offsets[8])
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333,
				106333 /* BOOM */, 157863, 198495, 213368, 225320, 234103 },
			// lengths
			{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075,
				18368, 40152, 14798, 11952, 8463, 18935 }
			)
		);
	}


	SECTION ( "Build fails for inconsistent trackcount and offsets" )
	{
		// Track count 0 is illegal

		CHECK_THROWS ( TOCBuilder::build(
			// track count
			0, /* BOOM */
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038
		));

		CHECK_THROWS ( TOCBuilder::build(
			// track count
			0, /* BOOM */
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// lengths
			{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075,
				18368, 40152, 14798, 11952, 8463, -1 /* instead of 18935 */ }
		));


		// Track count is smaller than number of offsets

		CHECK_THROWS ( TOCBuilder::build(
			// track count
			5, /* BOOM */
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038
		));

		CHECK_THROWS ( TOCBuilder::build(
			// track count
			5, /* BOOM */
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// lengths
			{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075,
				18368, 40152, 14798, 11952, 8463, -1 /* instead of 18935 */ }
		));


		// Track count is bigger than number of offsets

		CHECK_THROWS ( TOCBuilder::build(
			// track count
			18, /* BOOM */
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038
		));

		CHECK_THROWS ( TOCBuilder::build(
			// track count
			18, /* BOOM */
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// lengths
			{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075,
				18368, 40152, 14798, 11952, 8463, -1 /* instead of 18935 */ }
		));


		// Track count is bigger than legal maximum

		CHECK_THROWS ( TOCBuilder::build(
			// track count
			CDDA.MAX_TRACKCOUNT + 1, /* BOOM */
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038
		));

		CHECK_THROWS ( TOCBuilder::build(
			// track count
			CDDA.MAX_TRACKCOUNT + 1, /* BOOM */
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// lengths
			{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075,
				18368, 40152, 14798, 11952, 8463, -1 /* instead of 18935 */ }
		));
	}


	SECTION ( "Build fails for inconsistent leadout and offsets" )
	{
		// Leadout 0 is illegal

		CHECK_THROWS ( TOCBuilder::build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
			// leadout
			0 /* BOOM */
		));

		// Leadout exceeds maximal legal value

		CHECK_THROWS ( TOCBuilder::build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
			// leadout
			CDDA.MAX_BLOCK_ADDRESS + 1 /* BOOM */
		));

		// Leadout has not minimal distance to last offset

		CHECK_THROWS ( TOCBuilder::build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
			// leadout
			234103 + CDDA.MIN_TRACK_LEN_FRAMES - 1 /* BOOM */
		));
	}


	SECTION ( "Build fails for inconsistent lengths" )
	{
		// length[4] is smaller than legal minimum

		CHECK_THROWS ( TOCBuilder::build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// lengths
			{ 5192, 2165, 15885, 12228, CDDA.MIN_TRACK_LEN_FRAMES - 1/* BOOM */,
				19513, 18155, 18325, 33075, 18368, 40152, 14798, 11952, 8463,
				18935 }
		));

		// sum of lengths exceeds legal maximum

		CHECK_THROWS ( TOCBuilder::build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// lengths
			{ 5192, 2165, 15885, 12228, CDDA.MAX_OFFSET /* BOOM */, 19513,
				18155, 18325, 33075, 18368, 40152, 14798, 11952, 8463, 18935 }
		));

		// more than 99 tracks + more lengths than offsets

		CHECK_THROWS ( TOCBuilder::build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// lengths
			std::vector<lba_count_t>(100) /* BOOM */
		));

		// no lengths

		CHECK_THROWS ( TOCBuilder::build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// lengths
			{ } /* BOOM */
		));
	}
}

