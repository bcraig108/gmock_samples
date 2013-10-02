/******************************************************************************
 *
 * @file sample1.cpp
 *
 * @version Sep 25, 2013
 *
 * @author Brian M. Craig
 *
 ******************************************************************************/

#include <gmock/gmock.h>			// include gmock and gtest
#include <gtest/gtest.h>

#include <string>

using ::testing::Return;

// @file tcInterface.h --------------------------------------------------------

/**
 * @brief This is the class that will be mocked
 */
class tcInterface {
public:
	tcInterface( void ) {};
	virtual ~tcInterface( void ) {};
	virtual bool Open( void ) = 0;
	virtual void Close( void ) = 0;
	virtual void Send( const void* apvData, unsigned anSize ) = 0;
	virtual bool Status( void ) const = 0;
};

// @file tcInterface_mock.h ---------------------------------------------------

/**
 * @brief This is the mock of the interface class
 */
class tcInterface_mock : public tcInterface {
public:

	// general format for mocks is MOCK_METHODn, where n is the number of args
	MOCK_METHOD0(Open, bool());		// no args, returns bool
	MOCK_METHOD0(Close, void());	// no args, returns void

	// the send method has 2 args and returns void
	MOCK_METHOD2(Send, void( const void* apvData, unsigned anSize ));

	// Status method is const - use MOCK_CONST_METHODn
	MOCK_CONST_METHOD0(Status, bool());
};

// @file tcProcessor.h --------------------------------------------------------

/**
 * @brief This is the class under test, which interacts with the interface class
 */
class tcProcessor {
public:
	tcProcessor( tcInterface& arcInterface ) : mrcInterface( arcInterface ) {};

	~tcProcessor( void ) {};

	bool SendMessage( std::string& arcMessage ) {
		if ( mrcInterface.Open() ) {
			mrcInterface.Send( arcMessage.c_str(), arcMessage.size() );
			mrcInterface.Close();
			return true;
		}
		else
		{
			return false;
		}
	}

	bool CheckInterface( void ) {
		return mrcInterface.Status();
	}

protected:
	tcInterface& mrcInterface;
};

// @file tcProcessor_test.cpp -------------------------------------------------

/**
 * @brief This is good test case the message should be sent
 */
TEST(tcProcessor,SendMessage_Good)
{
	tcInterface_mock lcInterface;
	tcProcessor      lcProcessor( lcInterface );

	std::string lcString( "Hello World!" );

	EXPECT_CALL( lcInterface, Open() )	// expect call to tcInterface::Open
		.Times(1)						// the number of calls to expect
		.WillOnce(Return(true));		// specify the return value

	EXPECT_CALL( lcInterface, Send( lcString.c_str(), lcString.size() ) )
		.Times(1);						// expect 1 call to tcInterface::Send

	EXPECT_CALL( lcInterface, Close())	// expect 1 call to tcInterface::Close
		.Times(1);

	// expect the call to return true
	EXPECT_TRUE( lcProcessor.SendMessage( lcString ) );
};

/**
 * @brief This is bad test case the message should not be sent
 */
TEST(tcProcessor,SendMessage_Bad)
{
	tcInterface_mock lcInterface;
	tcProcessor      lcProcessor( lcInterface );

	std::string lcString( "Hello World!" );

	EXPECT_CALL( lcInterface, Open() )	// expect call to tcInterface::Open
		.Times(1)						// the number of calls to expect
		.WillOnce(Return(false));		// specify the return value

	EXPECT_CALL( lcInterface, Send( lcString.c_str(), lcString.size() ) )
		.Times(0);						// expect no call to tcInterface::Send

	EXPECT_CALL( lcInterface, Close() )	// expect no call to tcInterface::Close
		.Times(0);

	// expect the call to return false
	EXPECT_FALSE( lcProcessor.SendMessage( lcString ) );
};

/**
 * @brief This is a test case for the status method
 */
TEST(tcProcessor,Status_Good)
{
	tcInterface_mock lcInterface;
	tcProcessor      lcProcessor( lcInterface );

	EXPECT_CALL( lcInterface, Status() )	// anytime Status() is called, it
		.WillRepeatedly( Return(true) );	// returns true

	// expect the call to return true
	EXPECT_TRUE( lcProcessor.CheckInterface() );
};


/**
 * @brief This is a test case for the status method, when the interface status
 *        is bad
 */
TEST(tcProcessor,Status_Bad)
{
	tcInterface_mock lcInterface;
	tcProcessor      lcProcessor( lcInterface );

	ON_CALL( lcInterface, Status() )		// anytime Status() is called, it
		.WillByDefault( Return(false) );	// returns false, but this is not
											// tested - warning will be
											// generated, but test will pass

	// expect the call to return false
	EXPECT_FALSE( lcProcessor.CheckInterface() );
};
