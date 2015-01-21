cmake_minimum_required(VERSION 2.8.12)

project(menubar5)

set(REQUIRED_QT_VERSION 5.3.0)
set(KF5_MIN_VERSION 5.3.0)

find_package(ECM 1.0.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

# Find the required libraries
find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Core Widgets DBus X11Extras Quick Qml)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS Plasma PlasmaQuick Config Service ConfigWidgets WindowSystem IconThemes I18n XmlGui)
find_package(dbusmenu-qt5 0.9 REQUIRED)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(ECMPackageConfigHelpers)
include(ECMMarkNonGuiExecutable)
include(CMakePackageConfigHelpers)
include(WriteBasicConfigVersionFile)
include(CheckIncludeFiles)
include(FeatureSummary)

add_subdirectory(plugin)

plasma_install_package(plasmoid org.kde.plasma.menubar)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
