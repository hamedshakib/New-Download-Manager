cmake_minimum_required (VERSION 3.20)

project (DownloadManager VERSION 0.01 LANGUAGES CXX)


# Include sub-projects.
#add_subdirectory ("")


set(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(MY_SOURCE_PATH "src")
SET(MY_HeaderAndUi_PATH "HeaderAndUi")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Core,Gui,Widgets,Network,NetworkAuth,Sql,Multimedia,MultimediaWidgets,Charts,Concurrent

set(CMAKE_PREFIX_PATH "C:/Programs/QT/6.2.4/msvc2019_64")

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Core Sql Network Quick REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Core Sql Network Quick REQUIRED)



set(PROJECT_SOURCES
	    main.qrc
        appicon.rc
        ${MY_SOURCE_PATH}/main.cpp

        #MainWindow
        ${MY_HeaderAndUi_PATH}/MainWindow.ui
        ${MY_HeaderAndUi_PATH}/MainWindow.h
        ${MY_SOURCE_PATH}/MainWindow.cpp


        #DownloadManager
        ${MY_HeaderAndUi_PATH}/DownloadManager.h
        ${MY_SOURCE_PATH}/DownloadManager.cpp

        #Download
        ${MY_HeaderAndUi_PATH}/Download.h
        ${MY_SOURCE_PATH}/Download.cpp

        #Downloader
        ${MY_HeaderAndUi_PATH}/Downloader.h
        ${MY_SOURCE_PATH}/Downloader.cpp

        #PartDownloader
        ${MY_HeaderAndUi_PATH}/PartDownload.h
        ${MY_SOURCE_PATH}/PartDownload.cpp

        #PartDownloader
        ${MY_HeaderAndUi_PATH}/PartDownloader.h
        ${MY_SOURCE_PATH}/PartDownloader.cpp


        #DatabaseReader
        ${MY_HeaderAndUi_PATH}/DatabaseReader.h
        ${MY_SOURCE_PATH}/DatabaseReader.cpp


        #SettingUpDatabase
        ${MY_HeaderAndUi_PATH}/SettingUpDatabase.h
        ${MY_SOURCE_PATH}/SettingUpDatabase.cpp

        #DatabaseQueryPreparer
        ${MY_HeaderAndUi_PATH}/DatabaseQueryPreparer.h
        ${MY_SOURCE_PATH}/DatabaseQueryPreparer.cpp


        #DatabaseInteract
        ${MY_HeaderAndUi_PATH}/DatabaseInteract.h
        ${MY_SOURCE_PATH}/DatabaseInteract.cpp


        #ProcessDatabaseOutput
        ${MY_HeaderAndUi_PATH}/ProcessDatabaseOutput.h
        ${MY_SOURCE_PATH}/ProcessDatabaseOutput.cpp

        
        #DatabaseManager
        ${MY_HeaderAndUi_PATH}/DatabaseManager.h
        ${MY_SOURCE_PATH}/DatabaseManager.cpp


        #ProcessEnum
        ${MY_HeaderAndUi_PATH}/ProcessEnum.h
        ${MY_SOURCE_PATH}/ProcessEnum.cpp

        #DownloadFileWriter
        ${MY_HeaderAndUi_PATH}/DownloadFileWriter.h
        ${MY_SOURCE_PATH}/DownloadFileWriter.cpp


        #DownloadCategory
        ${MY_HeaderAndUi_PATH}/DownloadCategory.h
        ${MY_SOURCE_PATH}/DownloadCategory.cpp
        
        #NewDownloadCreater
        ${MY_HeaderAndUi_PATH}/NewDownloadCreater.h
        ${MY_SOURCE_PATH}/NewDownloadCreater.cpp

        #DeterminerDownloadFileType
        ${MY_HeaderAndUi_PATH}/DeterminerDownloadFileType.h
        ${MY_SOURCE_PATH}/DeterminerDownloadFileType.cpp

        #SettingInteract
        ${MY_HeaderAndUi_PATH}/SettingInteract.h
        ${MY_SOURCE_PATH}/SettingInteract.cpp

        #NewDownloadUrlWidget
        ${MY_HeaderAndUi_PATH}/NewDownloadUrlWidget.ui
        ${MY_HeaderAndUi_PATH}/NewDownloadUrlWidget.h
        ${MY_SOURCE_PATH}/NewDownloadUrlWidget.cpp

        #NewDownloadUrlWidget
        ${MY_HeaderAndUi_PATH}/NewDownloadComplitedInformationWidget.ui
        ${MY_HeaderAndUi_PATH}/NewDownloadComplitedInformationWidget.h
        ${MY_SOURCE_PATH}/NewDownloadComplitedInformationWidget.cpp

        #ConverterSizeToSuitableString
        ${MY_HeaderAndUi_PATH}/ConverterSizeToSuitableString.h
        ${MY_SOURCE_PATH}/ConverterSizeToSuitableString.cpp

        #CalculatorDownload
        ${MY_HeaderAndUi_PATH}/CalculatorDownload.h
        ${MY_SOURCE_PATH}/CalculatorDownload.cpp

        #TableViewController
        ${MY_HeaderAndUi_PATH}/TableViewController.h
        ${MY_SOURCE_PATH}/TableViewController.cpp

        #ApplicationManager
        ${MY_HeaderAndUi_PATH}/ApplicationManager.h
        ${MY_SOURCE_PATH}/ApplicationManager.cpp

        #TableViewRowCreater
        ${MY_HeaderAndUi_PATH}/TableViewRowCreater.h
        ${MY_SOURCE_PATH}/TableViewRowCreater.cpp

        #OpenFileForUser
        ${MY_HeaderAndUi_PATH}/OpenFileForUser.h
        ${MY_SOURCE_PATH}/OpenFileForUser.cpp

        #ShowDownloadWidget
        ${MY_HeaderAndUi_PATH}/ShowDownloadWidget.ui
        ${MY_HeaderAndUi_PATH}/ShowDownloadWidget.h
        ${MY_SOURCE_PATH}/ShowDownloadWidget.cpp

        #DateTimeManager
        ${MY_HeaderAndUi_PATH}/DateTimeManager.h
        ${MY_SOURCE_PATH}/DateTimeManager.cpp

        #ShowDownloadProperties
        ${MY_HeaderAndUi_PATH}/ShowDownloadProperties.ui
        ${MY_HeaderAndUi_PATH}/ShowDownloadProperties.h
        ${MY_SOURCE_PATH}/ShowDownloadProperties.cpp





)

#qt_wrap_ui(PROJECT_SOURCES downloader.ui)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES} 
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET andr APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(${PROJECT_NAME} SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(${PROJECT_NAME}
          ${PROJECT_SOURCES}
        )
    endif()
endif()


target_compile_definitions(${PROJECT_NAME}
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

target_link_libraries(${PROJECT_NAME}
  PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Sql Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::Quick
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)



if(QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(${PROJECT_NAME})
    qt_finalize_executable(${PROJECT_NAME})
endif()
