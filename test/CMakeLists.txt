cmake_minimum_required(VERSION 3.20)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Core Sql Network Quick LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Core Sql Network Quick LinguistTools REQUIRED)
enable_testing()
add_subdirectory(googletest)




set(headers_Path ./../HeaderAndUi)
set(sources_Path ./../src)

set(MY_Sources
	#Download
	${headers_Path}/Download.h
	${sources_Path}/Download.cpp
	src/Download_test.cpp
	
	#PartDownload
	${headers_Path}/PartDownload.h
	${sources_Path}/PartDownload.cpp
	src/PartDownload_test.cpp
	
	
	
	
	
)

add_executable(DownloadManagertest ${MY_Sources})
target_link_libraries(DownloadManagertest GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main 
 Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Sql Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::Quick
)
target_include_directories(DownloadManagertest PRIVATE ./../HeaderAndUi ./../src ./..)
#add_test(NAME DownloadManagertest COMMAND DownloadManagertest)

add_test(NAME test COMMAND DownloadManagertest)