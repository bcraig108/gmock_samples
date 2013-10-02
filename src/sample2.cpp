/******************************************************************************
 *
 * @file sample2.cpp
 *
 * @version Sep 26, 2013
 *
 * @author Brian M. Craig
 *
 ******************************************************************************/

#include <gmock/gmock.h>			// include gmock and gtest
#include <gtest/gtest.h>

#include <vector>

using ::testing::Return;
using ::testing::_;

// @file tcAccumulator.h ------------------------------------------------------

/**
 * @brief This is the class to be mocked
 */
class tcAccumulator
{
public:
	tcAccumulator( void ) {};
	virtual ~tcAccumulator( void ) {};

	// overloaded methods
	virtual void Add( int anNum ) = 0;
	virtual void Add( int anNum1, int anNum2 ) = 0;
	virtual void Add( const std::vector<int>& arcNums ) = 0;

	virtual int GetValue( void ) const = 0;
};

// @file tcAccumulator_mock.h -------------------------------------------------

/**
 * @brief the class mock
 */
class tcAccumulator_mock : public tcAccumulator
{
public:

	// overloaded methods
	MOCK_METHOD1( Add, void( int anNum ) );
	MOCK_METHOD2( Add, void( int anNum1, int anNum2 ) );
	MOCK_METHOD1( Add, void( const std::vector<int>& arcNums ) );

	MOCK_CONST_METHOD0( GetValue, int() );
};

// @file Accumulate.cpp -------------------------------------------------------

/**
 * @brief This method makes calls to the accumulator class
 * @param arcAccumulator a reference to the accumulator
 * @param arcNumbers a vector of numbers to use
 * @return the accumulated values
 */
int Accumulate( tcAccumulator&          arcAccumulator,
		         const std::vector<int>& arcNumbers )
{
	if ( arcNumbers.size() == 1 )
	{
		arcAccumulator.Add( arcNumbers[0] );
	}
	else if ( arcNumbers.size() == 2 )
	{
		arcAccumulator.Add( arcNumbers[0], arcNumbers[1] );
	}
	else
	{
		arcAccumulator.Add( arcNumbers );
	}

	return arcAccumulator.GetValue();
};

// @file Accumulate_test.cpp --------------------------------------------------

TEST(Accumulate,OneNumber)
{
	tcAccumulator_mock lcAccumulator;

	// accumulate one number
	std::vector<int> lcNumbers;
	lcNumbers.clear();
	lcNumbers.push_back( 2 );

	// expect a call to Add with one arg - set to 2
	EXPECT_CALL( lcAccumulator, Add( 2 ) ).Times( 1 );

	// expect a call to GetValue returning 2
	EXPECT_CALL( lcAccumulator, GetValue() ).Times( 1 ).WillOnce(Return(2));

	// expect Accumulate to return 2
	EXPECT_EQ( Accumulate( lcAccumulator, lcNumbers ), 2 );
};

TEST(Accumulate,TwoNumbers)
{
	tcAccumulator_mock lcAccumulator;

	// accumulate two numbers
	std::vector<int> lcNumbers;
	lcNumbers.clear();
	lcNumbers.push_back( 2 );
	lcNumbers.push_back( 3 );

	// expect a call to Add with 2 args - don't care what the values are
	EXPECT_CALL( lcAccumulator, Add( _, _ ) ).Times( 1 );

	// expect a call to GetValue returning 5
	EXPECT_CALL( lcAccumulator, GetValue() ).Times( 1 ).WillOnce(Return(5));

	// expect Accumulate to return 5
	EXPECT_EQ( Accumulate( lcAccumulator, lcNumbers ), 5 );
};

TEST(Accumulate,ThreeNumbers)
{
	tcAccumulator_mock lcAccumulator;

	// accumulate three numbers
	std::vector<int> lcNumbers;
	lcNumbers.clear();
	lcNumbers.push_back( 2 );
	lcNumbers.push_back( 3 );
	lcNumbers.push_back( 4 );

	// expect a call to Add with vector arg
	EXPECT_CALL( lcAccumulator, Add( lcNumbers ) ).Times( 1 );
	EXPECT_CALL( lcAccumulator, GetValue() ).Times( 1 ).WillOnce(Return(9));

	EXPECT_EQ( Accumulate( lcAccumulator, lcNumbers ), 9 );
};


TEST(Accumulate,ZeroNumbers)
{
	tcAccumulator_mock lcAccumulator;

	// accumulate zero numbers
	std::vector<int> lcNumbers;
	lcNumbers.clear();

	EXPECT_CALL( lcAccumulator, Add( lcNumbers ) ).Times( 1 );
	EXPECT_CALL( lcAccumulator, GetValue() ).Times( 1 ).WillOnce(Return(0));

	EXPECT_EQ( Accumulate( lcAccumulator, lcNumbers ), 0 );
};

