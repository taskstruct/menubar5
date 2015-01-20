project(menubarplugin)

#add_definitions(-DQT_QML_DEBUG)

set(menubar_SRCS
    menubarplugin.cpp
    menubaritem.cpp
    menucloner.cpp
    menuutils.cpp
    importer_interface.cpp
    kappmenuimporter.cpp
    windowmenumanager.cpp
)

qt5_add_dbus_adaptor(menubar_SRCS com.canonical.AppMenu.Renderer.xml
    menubaritem.h MenuBarItem)

add_library(menubarplugin SHARED ${menubar_SRCS})

target_link_libraries(menubarplugin
    Qt5::X11Extras
    KF5::Plasma KF5::PlasmaQuick KF5::ConfigCore
    KF5::ConfigGui
    KF5::Service
    KF5::ConfigWidgets
    KF5::WindowSystem
    KF5::IconThemes
    KF5::I18n
    KF5::XmlGui
    dbusmenu-qt5
)

message(STATUS "Default is ${QML_INSTALL_DIR}")
message(STATUS "KDE is ${KDE_INSTALL_QMLDIR}")
#message(STATUS ${PLASMA_DATA_INSTALL_DIR})
#message(STATUS ${SERVICES_INSTALL_DIR})
#message(STATUS ${DATA_INSTALL_DIR})


install(TARGETS menubarplugin DESTINATION ${QML_INSTALL_DIR}/org/kde/plasma/private/menubar)

install(FILES qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/plasma/private/menubar)
