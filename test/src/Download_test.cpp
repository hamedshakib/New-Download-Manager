#include <gtest/gtest.h>
#include "Download.h"

TEST(Download_test, Set_QueueId)
{
	auto n_download=new Download();
	n_download->Set_QueueId(10);
	EXPECT_EQ(n_download->get_QueueId(), 10);
}

TEST(Download_test, Set_downloadStatus)
{
	auto n_download = new Download();
	n_download->Set_downloadStatus(Download::DownloadStatusEnum::Completed);
	EXPECT_EQ(n_download->get_Status(), Download::DownloadStatusEnum::Completed);
}