cmake_minimum_required(VERSION 3.20)

project(DownloadManager VERSION 0.01 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# مسیرهای سورس‌کد، هدرها و ترجمه
set(MY_SOURCE_PATH "src")
set(MY_HeaderAndUi_PATH "HeaderAndUi")
set(MY_TRANSLATION_PATH "translations")

# اضافه کردن پوشه‌ها به Include Path جهت پیدا شدن هدرها در کدهای C++
include_directories(${MY_HeaderAndUi_PATH} ${MY_SOURCE_PATH})

# تنظیمات خودکار Qt برای MOC, UIC و RCC
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# یافتن بسته‌های Qt و GTest
# مسیر Qt از CMAKE_PREFIX_PATH خوانده می‌شود که در CMakePresets.json از متغیر
# محیطی QT_ROOT گرفته می‌شود (پس روی هر کامپیوتری مستقل از تنظیمات شخصی بیلد می‌شود).
find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Core Sql Network Quick LinguistTools)
if(NOT QT_FOUND)
    message(FATAL_ERROR
        "Qt پیدا نشد. لطفاً در سیستم خودتان متغیر محیطی QT_ROOT را روی مسیر پوشه Qt تنظیم کنید "
        "(مثلاً C:/Qt/6.11.1/msvc2022_64) و دوباره پیکربندی کنید. یا با پارامتر "
        "cmake -DCMAKE_PREFIX_PATH=<مسیر Qt> آن را بدهید.")
endif()
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Core Sql Network Quick LinguistTools REQUIRED)
find_package(GTest QUIET)

# فایل‌های ترجمه (فایل‌های TS source)
set(TS_FILES
    ${MY_TRANSLATION_PATH}/app_Persian.ts
)

# لیست کامل سورس‌ها، هدرها و فرم‌های UI
# نکته: ${TS_FILES} از این لیست حذف شد تا تداخلی با بیلد اصلی ایجاد نکند
set(PROJECT_SOURCES
    main.qrc
    appicon.rc
    ${MY_SOURCE_PATH}/main.cpp

    # MainWindow
    ${MY_HeaderAndUi_PATH}/MainWindow.ui
    ${MY_HeaderAndUi_PATH}/MainWindow.h
    ${MY_SOURCE_PATH}/MainWindow.cpp

    # DownloadManager
    ${MY_HeaderAndUi_PATH}/DownloadManager.h
    ${MY_SOURCE_PATH}/DownloadManager.cpp

    # Download
    ${MY_HeaderAndUi_PATH}/Download.h
    ${MY_SOURCE_PATH}/Download.cpp

    # DownloadControl
    ${MY_HeaderAndUi_PATH}/DownloadControl.h
    ${MY_SOURCE_PATH}/DownloadControl.cpp

    # PartDownloader
    ${MY_HeaderAndUi_PATH}/PartDownload.h
    ${MY_SOURCE_PATH}/PartDownload.cpp
    ${MY_HeaderAndUi_PATH}/PartDownloader.h
    ${MY_SOURCE_PATH}/PartDownloader.cpp

    # Database
    ${MY_HeaderAndUi_PATH}/DatabaseReader.h
    ${MY_SOURCE_PATH}/DatabaseReader.cpp
    ${MY_HeaderAndUi_PATH}/SettingUpDatabase.h
    ${MY_SOURCE_PATH}/SettingUpDatabase.cpp
    ${MY_HeaderAndUi_PATH}/DatabaseQueryPreparer.h
    ${MY_SOURCE_PATH}/DatabaseQueryPreparer.cpp
    ${MY_HeaderAndUi_PATH}/DatabaseInteract.h
    ${MY_SOURCE_PATH}/DatabaseInteract.cpp
    ${MY_HeaderAndUi_PATH}/ProcessDatabaseOutput.h
    ${MY_SOURCE_PATH}/ProcessDatabaseOutput.cpp
    ${MY_HeaderAndUi_PATH}/DatabaseManager.h
    ${MY_SOURCE_PATH}/DatabaseManager.cpp

    # Core & UI Features
    ${MY_HeaderAndUi_PATH}/ProcessEnum.h
    ${MY_SOURCE_PATH}/ProcessEnum.cpp
    ${MY_HeaderAndUi_PATH}/DownloadFileWriter.h
    ${MY_SOURCE_PATH}/DownloadFileWriter.cpp
    ${MY_HeaderAndUi_PATH}/DownloadCategory.h
    ${MY_SOURCE_PATH}/DownloadCategory.cpp
    ${MY_HeaderAndUi_PATH}/NewDownloadCreater.h
    ${MY_SOURCE_PATH}/NewDownloadCreater.cpp
    ${MY_HeaderAndUi_PATH}/DeterminerDownloadFileType.h
    ${MY_SOURCE_PATH}/DeterminerDownloadFileType.cpp
    ${MY_HeaderAndUi_PATH}/SettingInteract.h
    ${MY_SOURCE_PATH}/SettingInteract.cpp
    ${MY_HeaderAndUi_PATH}/NewDownloadUrlWidget.ui
    ${MY_HeaderAndUi_PATH}/NewDownloadUrlWidget.h
    ${MY_SOURCE_PATH}/NewDownloadUrlWidget.cpp
    ${MY_HeaderAndUi_PATH}/NewDownloadComplitedInformationWidget.ui
    ${MY_HeaderAndUi_PATH}/NewDownloadComplitedInformationWidget.h
    ${MY_SOURCE_PATH}/NewDownloadComplitedInformationWidget.cpp
    ${MY_HeaderAndUi_PATH}/ConverterSizeToSuitableString.h
    ${MY_SOURCE_PATH}/ConverterSizeToSuitableString.cpp
    ${MY_HeaderAndUi_PATH}/CalculatorDownload.h
    ${MY_SOURCE_PATH}/CalculatorDownload.cpp
    ${MY_HeaderAndUi_PATH}/TableViewController.h
    ${MY_SOURCE_PATH}/TableViewController.cpp
    ${MY_HeaderAndUi_PATH}/ApplicationManager.h
    ${MY_SOURCE_PATH}/ApplicationManager.cpp
    ${MY_HeaderAndUi_PATH}/TableViewRowCreater.h
    ${MY_SOURCE_PATH}/TableViewRowCreater.cpp
    ${MY_HeaderAndUi_PATH}/OpenFileForUser.h
    ${MY_SOURCE_PATH}/OpenFileForUser.cpp
    ${MY_HeaderAndUi_PATH}/ShowDownloadWidget.ui
    ${MY_HeaderAndUi_PATH}/ShowDownloadWidget.h
    ${MY_SOURCE_PATH}/ShowDownloadWidget.cpp
    ${MY_HeaderAndUi_PATH}/DateTimeManager.h
    ${MY_SOURCE_PATH}/DateTimeManager.cpp
    ${MY_HeaderAndUi_PATH}/ShowDownloadProperties.ui
    ${MY_HeaderAndUi_PATH}/ShowDownloadProperties.h
    ${MY_SOURCE_PATH}/ShowDownloadProperties.cpp
    ${MY_HeaderAndUi_PATH}/OptionsWidget.ui
    ${MY_HeaderAndUi_PATH}/OptionsWidget.h
    ${MY_SOURCE_PATH}/OptionsWidget.cpp
    ${MY_HeaderAndUi_PATH}/ProxyManager.h
    ${MY_SOURCE_PATH}/ProxyManager.cpp
    ${MY_HeaderAndUi_PATH}/Queue.h
    ${MY_SOURCE_PATH}/Queue.cpp
    ${MY_HeaderAndUi_PATH}/QueueManager.h
    ${MY_SOURCE_PATH}/QueueManager.cpp
    ${MY_HeaderAndUi_PATH}/ShowSchedule.ui
    ${MY_HeaderAndUi_PATH}/ShowSchedule.h
    ${MY_SOURCE_PATH}/ShowSchedule.cpp
    ${MY_HeaderAndUi_PATH}/MainTreeViewController.h
    ${MY_SOURCE_PATH}/MainTreeViewController.cpp
    ${MY_HeaderAndUi_PATH}/TranslationManager.h
    ${MY_SOURCE_PATH}/TranslationManager.cpp
    ${MY_HeaderAndUi_PATH}/RunGuard.h
    ${MY_SOURCE_PATH}/RunGuard.cpp
    ${MY_HeaderAndUi_PATH}/QueueTimeManager.h
    ${MY_SOURCE_PATH}/QueueTimeManager.cpp
    ${MY_HeaderAndUi_PATH}/ConverterQueueTime.h
    ${MY_SOURCE_PATH}/ConverterQueueTime.cpp
    ${MY_HeaderAndUi_PATH}/CompleteDownloadDialog.h
    ${MY_SOURCE_PATH}/CompleteDownloadDialog.cpp
    ${MY_HeaderAndUi_PATH}/BatchDownloadCreatorWidget.h
    ${MY_SOURCE_PATH}/BatchDownloadCreatorWidget.cpp
    ${MY_HeaderAndUi_PATH}/MainTableViewController.h
    ${MY_SOURCE_PATH}/MainTableViewController.cpp
    ${MY_HeaderAndUi_PATH}/ScheduleTreeWidgetTabController.h
    ${MY_SOURCE_PATH}/ScheduleTreeWidgetTabController.cpp
    ${MY_HeaderAndUi_PATH}/MainTreeViewQueueItem.h
    ${MY_SOURCE_PATH}/MainTreeViewQueueItem.cpp
    ${MY_HeaderAndUi_PATH}/SelectColumnsForMainTableView.h
    ${MY_SOURCE_PATH}/SelectColumnsForMainTableView.cpp
)

# ۱. ساخت Executable 
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )

    # ۲. کامپایل فایل‌های .ts به .qm و قرار دادن آن‌ها در پوشه translations کنار فایل اجرا
    qt_add_translations(${PROJECT_NAME}
        TS_FILES ${TS_FILES}
        QM_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/translations"
    )
else()
    if(ANDROID)
        add_library(${PROJECT_NAME} SHARED ${PROJECT_SOURCES})
    else()
        add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
    endif()

    # برای Qt 5
    qt5_add_translation(QM_FILES ${TS_FILES})
    add_custom_target(translations ALL DEPENDS ${QM_FILES})
    
    # کپی کردن به پوشه translations پس از بیلد در Qt 5
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/translations"
        COMMAND ${CMAKE_COMMAND} -E copy ${QM_FILES} "${CMAKE_CURRENT_BINARY_DIR}/translations/"
    )
endif()

if(MSVC)
    # غیرفعال کردن سخت‌گیری اضافی MSVC روی C++20 برای هدرهای سیستم و Qt
    target_compile_options(${PROJECT_NAME} PRIVATE /permissive)
endif()

# تعریف متغیرهای کامپایل
target_compile_definitions(${PROJECT_NAME}
    PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>
)

# لینک کردن کتابخانه‌های Qt
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Sql
        Qt${QT_VERSION_MAJOR}::Network
        Qt${QT_VERSION_MAJOR}::Quick
)

# تنظیم مشخصات سیستم‌عامل
set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

# کپی کردن خودکار فایل‌های اولیه دیتابیس و تنظیمات
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Database/DM.db" AND NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/DM.db")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/Database/DM.db"
         DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Ini file/OptionAndSettings.ini" AND NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/OptionAndSettings.ini")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/Ini file/OptionAndSettings.ini"
         DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
endif()

# نهایی‌سازی ساختار فایل اجرایی برای Qt 6
if(QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(${PROJECT_NAME})
    qt_finalize_executable(${PROJECT_NAME})
endif()