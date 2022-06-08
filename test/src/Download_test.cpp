#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Download.h"


class DownloadTest : public ::testing::Test {
protected:
	DownloadTest() {
		// You can do set-up work for each test here.
		m_download = new Download();
	}

	~DownloadTest() override {
		// You can do clean-up work that doesn't throw exceptions here.
		if (m_download)
		{
			m_download->deleteLater();
			m_download = nullptr;
		}
	}

	// If the constructor and destructor are not enough for setting up
	// and cleaning up each test, you can define the following methods:

	void SetUp() override {
		// Code here will be called immediately after the constructor (right
		// before each test).
	}

	void TearDown() override {
		// Code here will be called immediately after each test (right
		// before the destructor).
	}

	// Class members declared here can be used by all tests in the test suite
	// for Foo.

	Download* m_download;
};

/*
TEST(Download_test, Set_Id)
{
	auto download = new Download();
	QUrl url = QUrl("https://google.com/chrome.zip");
	download->Set_Url(url);
	EXPECT_EQ(download->get_Url(), url);
}*/

TEST_F(DownloadTest, Set_Id)
{
	QUrl url = QUrl("https://google.com/chrome.zip");
	m_download->Set_Url(url);
	EXPECT_EQ(m_download->get_Url(), url);
}

TEST_F(DownloadTest, Set_QueueId)
{
	m_download->Set_QueueId(10);
	EXPECT_EQ(m_download->get_QueueId(), 10);
}


TEST_F(DownloadTest, Set_downloadStatus)
{
	m_download->Set_downloadStatus(Download::DownloadStatusEnum::Completed);
	EXPECT_EQ(m_download->get_Status(), Download::DownloadStatusEnum::Completed);

	m_download->Set_downloadStatus(Download::DownloadStatusEnum::Downloading);
	EXPECT_EQ(m_download->get_Status(), Download::DownloadStatusEnum::Downloading);

	m_download->Set_downloadStatus(Download::DownloadStatusEnum::NotStarted);
	EXPECT_EQ(m_download->get_Status(), Download::DownloadStatusEnum::NotStarted);

	m_download->Set_downloadStatus(Download::DownloadStatusEnum::Pause);
	EXPECT_EQ(m_download->get_Status(), Download::DownloadStatusEnum::Pause);
}

TEST_F(DownloadTest, Set_suffix)
{
	m_download->Set_Suffix("zip");
	EXPECT_EQ(m_download->get_Suffix(),"zip");

	m_download->Set_Suffix("exe");
	EXPECT_EQ(m_download->get_Suffix(),"exe");
}

TEST_F(DownloadTest, Set_FileName)
{
	m_download->Set_FileName("winrar");
	EXPECT_EQ(m_download->get_FileName(), "winrar");
}

TEST_F(DownloadTest, Set_FullFileName)
{
	m_download->Set_FullFileName("winrar.exe");
	EXPECT_EQ(m_download->get_FullFileName(), "winrar.exe");
}

TEST_F(DownloadTest, Set_MaxSpeed)
{
	qint64 maxSpeed = 150;
	m_download->Set_MaxSpeed(maxSpeed);
	EXPECT_EQ(m_download->get_MaxSpeed(),maxSpeed);
}

TEST_F(DownloadTest, Set_Description)
{
	m_download->Set_Description("a good download");
	EXPECT_STREQ(m_download->get_Description().toStdString().c_str(), "a good download");
}

TEST_F(DownloadTest, Set_SizeDownload)
{
	m_download->Set_SizeDownload(1044);
	EXPECT_EQ(m_download->get_SizeDownload(), 1044);

	m_download->Set_SizeDownload(55554);
	EXPECT_EQ(m_download->get_SizeDownload(), 55554);
}

TEST_F(DownloadTest, Set_DownloadedSize)
{
	m_download->Set_DownloadedSize(0);
	EXPECT_EQ(m_download->get_DownloadedSize(), 0);

	m_download->Set_DownloadedSize(18234);
	EXPECT_EQ(m_download->get_DownloadedSize(), 18234);
}

TEST_F(DownloadTest, Set_LastTryTime)
{
	QDateTime dateTime(QDate(2022, 12, 1), QTime(10, 21, 51));
	m_download->Set_LastTryTime(dateTime);
	EXPECT_EQ(m_download->get_LastTryTime(), dateTime);
}

TEST_F(DownloadTest, Set_Username)
{
	m_download->Set_Username("useruser");
	EXPECT_EQ(m_download->get_Username(), "useruser");
}

TEST_F(DownloadTest, Set_Password)
{
	m_download->Set_Password("pass1234");
	EXPECT_EQ(m_download->get_Password(), "pass1234");
}

TEST_F(DownloadTest, Set_ResumeCapability)
{
	m_download->Set_ResumeCapability(Download::ResumeCapabilityEnum::No);
	EXPECT_EQ(m_download->get_ResumeCapability(), Download::ResumeCapabilityEnum::No);

	m_download->Set_ResumeCapability(Download::ResumeCapabilityEnum::Yes);
	EXPECT_EQ(m_download->get_ResumeCapability(), Download::ResumeCapabilityEnum::Yes);

	m_download->Set_ResumeCapability(Download::ResumeCapabilityEnum::UnKnown);
	EXPECT_EQ(m_download->get_ResumeCapability(), Download::ResumeCapabilityEnum::UnKnown);
}
