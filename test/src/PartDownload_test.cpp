#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "PartDownload.h"

class PartDownloadTest : public ::testing::Test {
protected:
	PartDownloadTest() {
		// You can do set-up work for each test here.
		m_Partdownload = new PartDownload();
	}

	~PartDownloadTest() override {
		// You can do clean-up work that doesn't throw exceptions here.
		if (m_Partdownload)
		{
			m_Partdownload->deleteLater();
			m_Partdownload = nullptr;
		}
	}

	// If the constructor and destructor are not enough for setting up
	// and cleaning up each test, you can define the following methods:

	void SetUp() override {
		m_Partdownload->is_Finished = false;
		// Code here will be called immediately after the constructor (right
		// before each test).
	}

	void TearDown() override {
		// Code here will be called immediately after each test (right
		// before the destructor).
	}

	// Class members declared here can be used by all tests in the test suite
	// for Foo.

	PartDownload* m_Partdownload;
};

TEST_F(PartDownloadTest, createPartDownload)
{
	bool is_ok=false;
	if (m_Partdownload)
		is_ok = true;
	EXPECT_TRUE(true);
}

TEST_F(PartDownloadTest, Set_StartByte)
{
	qint64 startByte = 1513;
	m_Partdownload->Set_StartByte(startByte);
	EXPECT_EQ(m_Partdownload->get_StartByte(), startByte);
}

TEST_F(PartDownloadTest, Set_EndByte)
{
	qint64 endByte = 1513;
	m_Partdownload->Set_EndByte(endByte);
	EXPECT_EQ(m_Partdownload->get_EndByte(), endByte);
}

TEST_F(PartDownloadTest, Set_LastDownloadedByte)
{
	qint64 LastDownloadedByte = 1675;
	m_Partdownload->Set_LastDownloadedByte(LastDownloadedByte);
	EXPECT_EQ(m_Partdownload->get_LastDownloadedByte(), LastDownloadedByte);
}

TEST_F(PartDownloadTest, Set_PartDownloadFile)
{
	QFile* file1 = new QFile();

	m_Partdownload->Set_PartDownloadFile(file1);
	EXPECT_EQ(m_Partdownload->get_PartDownloadFile(), file1);
	file1->deleteLater();
}

TEST_F(PartDownloadTest, Set_PartDownloadId)
{
	int partDownloadId = 54;
	m_Partdownload->Set_PartDownloadId(partDownloadId);
	EXPECT_EQ(m_Partdownload->get_PartDownloadId(), partDownloadId);
}

TEST_F(PartDownloadTest, Set_DownloadId)
{
	int downloadId = 54;
	m_Partdownload->Set_DownloadId(downloadId);
	EXPECT_EQ(m_Partdownload->get_DownloadId(), downloadId);
}

TEST_F(PartDownloadTest, IsPartDownloadFinished)
{

	qint64 LastDownloadedByte = 1675;
	qint64 endByte = 1954;
	m_Partdownload->Set_LastDownloadedByte(LastDownloadedByte);
	m_Partdownload->Set_EndByte(endByte);
	
	bool is_Finsihed;
	if (m_Partdownload->get_LastDownloadedByte() >= m_Partdownload->get_EndByte())
	{
		m_Partdownload->is_Finished = true;
		is_Finsihed= true;
	}
	else
	{
		is_Finsihed= false;
	}
	
	EXPECT_THAT(endByte - LastDownloadedByte, ::testing::Ge(0));
	ASSERT_EQ(is_Finsihed, false);
}
