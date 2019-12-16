#include <climits>
#include "Arduino.h"
#include "catch.hpp"

#include "timer_override.h"

TEST_CASE( "Basic Timer", "[timer]" )
{
  TimerOverride timer;

  REQUIRE( timer.check(10) == true );
  timer.reset();
  REQUIRE( timer.check(10) == false );
  millis_fake.return_val = 1 << 10;
  REQUIRE( timer.check(10) == true );
}

//TEST_CASE( "Timer overflow", "[timer]" )
//{
//  TimerOverride timer;
//
//  millis_fake.return_val = ULLONG_MAX - 1;
//  timer.reset();
//  millis_fake.return_val = ULLONG_MAX ;
//  REQUIRE( timer.check( 10 ) == false );
//
//  millis_fake.return_val = 10 ;
//  REQUIRE( timer.check( 10 ) == true );
//
//  // Then check the case where we wait for whole timer to go all around over.
//  timer.reset();
//  REQUIRE( timer.check( 10 ) == false );
//  millis_fake.return_val = 0 ; // and the timer went overflow without any check in between
//  REQUIRE( timer.check( 10 ) == true );
//}
