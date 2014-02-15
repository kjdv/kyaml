#include "tokenizer.hh"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

TEST(EmptySkeleton, Failing)
{
  EXPECT_TRUE(false) << "empty skeleton\n";
}
