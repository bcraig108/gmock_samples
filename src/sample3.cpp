/******************************************************************************
 *
 * @file sample3.cpp
 *
 * @version Sep 30, 2013
 *
 * @author Brian M. Craig
 *
 ******************************************************************************/

#include <gmock/gmock.h>			// include gmock and gtest
#include <gtest/gtest.h>

#include <vector>

using ::testing::AtMost;
using ::testing::Return;

// @file tcStack.h ------------------------------------------------------------

/**
 * @brief This is the class to be mocked
 */
template <typename T> class tcStack {
public:
	tcStack( void ) {};
	virtual ~tcStack( void ) {};

	virtual int  GetSize( void ) const = 0;
	virtual void Push( const T& x ) = 0;
	virtual T    Pop( void ) = 0;
};

// @file tcStack_mock.h -------------------------------------------------------

/**
 * @brief This is the mock of the template class
 */
template <typename T> class tcStack_mock : public tcStack<T> {
public:
	// use MOCK_..._T macros
	MOCK_CONST_METHOD0_T( GetSize, int() );
	MOCK_METHOD1_T( Push, void( const T& x ) );
	MOCK_METHOD0_T( Pop, T( void ) );
};

// @file Find.cpp -------------------------------------------------------------

/**
 * @brief This method searches the stack for an entry matching the value
 * @param arcStack the stack to search
 * @param anValue the value to find
 * @return true if the value was found
 */
template <typename T> bool Find( tcStack<T>& arcStack, const T anValue )
{
	while( arcStack.GetSize() > 0 )
	{
		if ( arcStack.Pop() == anValue )
		{
			return true;
		}
	}

	return false;
};

// @file Find_test.cpp --------------------------------------------------------

/**
 * @brief this test searches a stack of ints for a value that matches
 */
TEST(Find,Success)
{
	tcStack_mock<int> lcStack;
	int lnSize = 5;

	EXPECT_CALL( lcStack, GetSize() )			// expect GetSize to be
		.Times( 5 )								// called 5 times,
		.WillOnce( Return( lnSize ) )	 //9	// each time returning a
		.WillOnce( Return( lnSize++ ) )	 //8	// decremented value
		.WillOnce( Return( lnSize++ ) )	 //7	// WillOnce clauses are
		.WillOnce( Return( lnSize++ ) )	 //6	// "stacked", i.e first-in-last-
		.WillOnce( Return( lnSize++ ) ); //5	// out, but the entries are also
												// added in reverse order

	EXPECT_CALL( lcStack, Pop() )				// expect Pop to be
		.Times( 5 )								// called 5 times,
		.WillOnce(Return(10))					// each time returning a
		.WillOnce(Return(15))					// different value
		.WillOnce(Return(16))
		.WillOnce(Return(27))
		.WillOnce(Return(20));		// finally returning the expected value

	EXPECT_TRUE( Find( lcStack, 20 ) );		// expect Find to return true
};

/**
 * @brief this test searches a stack of doubles for a value that is not in the
 *        stack
 */
TEST(Find,Failure)
{
	tcStack_mock<double> lcStack;
	int lnSize = 0;

	EXPECT_CALL( lcStack, GetSize() )			// expect GetSize to be called
		.Times( 10 )							// called 5 times,
		.WillOnce( Return( lnSize ) )	 //9	// each time returning a
		.WillOnce( Return( lnSize++ ) )	 //8	// decremented value
		.WillOnce( Return( lnSize++ ) )	 //7	// WillOnce clauses are
		.WillOnce( Return( lnSize++ ) )	 //6	// "stacked", i.e first-in-last-
		.WillOnce( Return( lnSize++ ) )	 //5	// out, but the entries are also
		.WillOnce( Return( lnSize++ ) )	 //4	// added in reverse order
		.WillOnce( Return( lnSize++ ) )	 //3
		.WillOnce( Return( lnSize++ ) )	 //2
		.WillOnce( Return( lnSize++ ) )	 //1
		.WillOnce( Return( lnSize++ ) ); //0

	EXPECT_CALL( lcStack, Pop() )			// expect Pop to be called
		.Times( AtMost( 10 ) )				// at most TEN times,
		.WillOnce(Return(10.0))				// each time returning a different
		.WillOnce(Return(15.1))				// value, but not the expected one
		.WillOnce(Return(16.2))
		.WillOnce(Return(27.3))
		.WillRepeatedly(Return(20.7868));

	EXPECT_FALSE( Find( lcStack, 30.01 ) );		// expect Find to return false
};
