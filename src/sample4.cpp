/******************************************************************************
 *
 * @file sample3.cpp
 *
 * @version Oct 1, 2013
 *
 * @author Brian M. Craig
 *
 ******************************************************************************/

#include <gmock/gmock.h>			// include gmock and gtest
#include <gtest/gtest.h>

using ::testing::InSequence;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::StrictMock;

// @file tcMutex.h ------------------------------------------------------------

class tcMutex
{
public:

	static const double WAIT_FOREVER = -1.0;
	static const double NO_WAIT      =  0.0;

	tcMutex( void ) {};
	virtual ~tcMutex( void ) {};

	virtual bool Take( double arTimeout = WAIT_FOREVER ) = 0;
	virtual void Give( void ) = 0;
};

// @file tcMutex_mock.h -------------------------------------------------------

class tcMutex_mock : public tcMutex
{
public:
	MOCK_METHOD1( Take, bool( double arTimeout ) );
	MOCK_METHOD0( Give, void() );
};

// @file tcScopeLock.h --------------------------------------------------------

class tcScopeLock
{
public:

	tcScopeLock( tcMutex& arcMutex ) : mrcMutex( arcMutex )
	{
		mrcMutex.Take( tcMutex::WAIT_FOREVER );
	}

	~tcScopeLock( void )
	{
		mrcMutex.Give();
	}

private:

	tcMutex& mrcMutex;
};

// @file tcScopeLock_test.cpp -------------------------------------------------

TEST(tcScopeLock,Normal)
{
	tcMutex_mock lcMutex;

	// No call to Take expected - will cause "uninteresting function" warning,
	// no error
	//	EXPECT_CALL( lcMutex, Take(tcMutex::WAIT_FOREVER) )
	//		.Times(1)
	//		.WillOnce(Return(true));

	EXPECT_CALL( lcMutex, Give() ).Times(1);

	{
		tcScopeLock lcScopeLock( lcMutex );
	}
}

TEST(tcScopeLock,Nice)
{
	NiceMock<tcMutex_mock> lcMutex;

	// No call to Take expected - no warnings because this is a NiceMock
	//	EXPECT_CALL( lcMutex, Take(tcMutex::WAIT_FOREVER) )
	//		.Times(1)
	//		.WillOnce(Return(true));

	EXPECT_CALL( lcMutex, Give() ).Times(1);

	{
		tcScopeLock lcScopeLock( lcMutex );
	}
}

TEST(tcScopeLock,Strict)
{
	StrictMock<tcMutex_mock> lcMutex;

	// No call to Take expected - will fail because this is a StrictMock
	//	EXPECT_CALL( lcMutex, Take(tcMutex::WAIT_FOREVER) )
	//		.Times(1)
	//		.WillOnce(Return(true));

	EXPECT_CALL( lcMutex, Give() ).Times(1);

	{
		tcScopeLock lcScopeLock( lcMutex );
	}
}

TEST(tcScopeLock,Order)
{
	StrictMock<tcMutex_mock> lcMutex;

	InSequence s;  // calls must occur in the order listed or the test will fail

	EXPECT_CALL( lcMutex, Take(tcMutex::WAIT_FOREVER) )
		.Times(1)
		.WillOnce(Return(true));
	EXPECT_CALL( lcMutex, Give() ).Times(1);

	{
		tcScopeLock lcScopeLock( lcMutex );
	}
}

TEST(tcScopeLock,OutOfOrder)
{
	StrictMock<tcMutex_mock> lcMutex;

	InSequence s;  // calls must occur in the order listed or the test will fail

	EXPECT_CALL( lcMutex, Give() ).Times(1);			// expect fail
	EXPECT_CALL( lcMutex, Take(tcMutex::WAIT_FOREVER) )
		.Times(1)
		.WillOnce(Return(true));

	{
		tcScopeLock lcScopeLock( lcMutex );
	}
}
