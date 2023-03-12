QT += widgets
requires(qtConfig(filedialog))

HEADERS       = \
	src/view/mainwindow/mainwindow.h \
	src/view/mycodetext/mycodetext.h \
	src/view/myconsole/myconsole.h \
	src/view/myhighlighter/myhighlighter.h \
	src/view/mylinenum/mylinenum.h \
	src/view/mytable/mytable.h \
	src/view/mytextfield/mytextfield.h \
	src/view/myhelpwindow/myhelpwindow.h \
	src/process/myerror/myerror.h \
	src/process/managers/buffermanager/buffermanager.h \
	src/process/managers/catalogmanager/catalogmanager.h \
	src/process/managers/indexmanager/indexmanager.h \
	src/process/interpreter/interpreter.h \
	src/process/managers/recordmanager/recordmanager.h \
	src/process/api/api.h \
	src/process/statement/statement.h \
	src/process/statement/createTable/createTable.h \
	src/process/statement/createIndex/createIndex.h \
    src/process/statement/delete/delete.h \
	src/process/statement/dropTable/dropTable.h \
	src/process/statement/dropIndex/dropIndex.h \
    src/process/statement/insert/insert.h \
	src/process/statement/select/select.h \
	src/process/statement/quit/quit.h \
	src/process/statement/execfile/execfile.h \
    src/process/condition/condition.h \
    src/process/attribute/attribute.h \
    src/process/blocks/block.h \
    src/process/blocks/tableBlock/tableBlock.h \
    src/process/table/table.h \
    src/process/blocks/catalogBlock/catalogBlock.h \
    src/view/myTableView/myTableView.h \
    src/process/managers/managers.h \
    src/process/statement/statements.h \
    src/process/blocks/blocks.h \
    src/process/BPlusTree/BPlusTree.h \
    src/process/blocks/leafBlock/leafBlock.h \
    src/process/blocks/nonleafBlock/nonleafBlock.h \
    src/process/blocks/nodeBlock/nodeBlock.h \
    src/process/blocks/indexCatalogBlock/indexCatalogBlock.h
SOURCES       = \
	src/view/mainwindow/mainwindow.cpp \
	src/view/mycodetext/mycodetext.cpp \
	src/view/myconsole/myconsole.cpp \
	src/view/myhighlighter/myhighlighter.cpp \
	src/view/mylinenum/mylinenum.cpp \
	src/view/mytable/mytable.cpp \
	src/main.cpp \
	src/view/mytextfield/mytextfield.cpp \
	src/view/mainwindow/mainwindowEvents.cpp \
	src/view/mainwindow/mainwindowFileProcess.cpp \
	src/view/mainwindow/mainwindowMenu.cpp \
	src/view/myhelpwindow/myhelpwindow.cpp \
	src/process/myerror/myerror.cpp \
	src/process/managers/buffermanager/buffermanager.cpp \
	src/process/managers/catalogmanager/catalogmanager.cpp \
	src/process/managers/indexmanager/indexmanager.cpp \
	src/process/interpreter/interpreter.cpp \
	src/process/managers/recordmanager/recordmanager.cpp \
	src/process/api/api.cpp \
	src/process/statement/createTable/createTable.cpp \
	src/process/statement/createIndex/createIndex.cpp \
    src/process/statement/delete/delete.cpp \
	src/process/statement/dropTable/dropTable.cpp \
	src/process/statement/dropIndex/dropIndex.cpp \
    src/process/statement/insert/insert.cpp \
    src/process/statement/select/select.cpp \
	src/process/statement/quit/quit.cpp \
	src/process/statement/execfile/execfile.cpp \
    src/process/statement/statement.cpp \
    src/process/attribute/attribute.cpp \
    src/process/blocks/block.cpp \
    src/process/blocks/tableBlock/tableBlock.cpp \
    src/process/table/table.cpp \
    src/process/blocks/catalogBlock/catalogBlock.cpp \
    src/view/myTableView/myTableView.cpp \
    src/process/condition/condition.cpp \
    src/process/BPlusTree/BPlusTree.cpp \
    src/process/blocks/leafBlock/leafBlock.cpp \
    src/process/blocks/nonleafBlock/nonleafBlock.cpp \
    src/process/blocks/nodeBlock/nodeBlock.cpp \
    src/process/blocks/indexCatalogBlock/indexCatalogBlock.cpp
#! [0]
RESOURCES     = \
	src/resources/resources.qrc
#! [0]

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/mainwindows/application
INSTALLS += target

FORMS += \
	src/view/mainwindow/mainwindow.ui





