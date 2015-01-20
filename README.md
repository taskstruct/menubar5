# menubar5
Unofficial port of Menubar applet for Plasma 5

# build
On Kubuntu
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_PREFIX_PATH=/usr -DKDE_INSTALL_USE_QT_SYS_PATHS=ON -DDATA_INSTALL_DIR=/usr/share -DQML_IMPORT_PATH=/usr/lib/x86_64-linux-gnu/qt5/qml/ ..