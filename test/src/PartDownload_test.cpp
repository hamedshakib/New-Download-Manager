#include <gtest/gtest.h>
#include "PartDownload.h"

TEST(PartDownload_test, creatPartDownload)
{
	auto partdownload = new PartDownload();
	bool is_ok=false;
	if (partdownload)
		is_ok = true;
	EXPECT_TRUE(true);
}