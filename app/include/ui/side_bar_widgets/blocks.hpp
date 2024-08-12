#pragma once

#include <QWidget>

class QLabel;
class QSplitter;
class QTreeWidgetItem;
class BlockManager;
class TabManager;

namespace QtUtility {
namespace widgets {
class QCollapsibleWidget;
} // namespace widgets
} // namespace QtUtility

#include <QtNodes/Definitions>

class Blocks : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QtNodes::NodeId nodeId READ nodeId WRITE setNodeId NOTIFY nodeIdChanged)
public:
    Blocks(std::shared_ptr<BlockManager> blockManager,
           std::shared_ptr<TabManager> tabManager,
           QWidget *parent = nullptr);

    QtNodes::NodeId nodeId() const { return m_nodeId; }
    void setNodeId(QtNodes::NodeId id);

signals:
    void nodeIdChanged(QtNodes::NodeId id);

public slots:
    void onNodeSelected(QtNodes::NodeId id);

private slots:
    void updateFields();
    void onLibraryItemClicked(QTreeWidgetItem *item);

private:
    void initUi();
    void initEditor();
    void initLibrary();

    std::shared_ptr<BlockManager> m_blockManager;
    std::shared_ptr<TabManager> m_tabManager;
    QtNodes::NodeId m_nodeId;

    QSplitter *m_splitter;
    QtUtility::widgets::QCollapsibleWidget *m_blockEditor;
    QLabel *m_viewerLabel;
    QtUtility::widgets::QCollapsibleWidget *m_library;
};
