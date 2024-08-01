#include "ui/side_bar_widgets/blocks.hpp"

#include <QHeaderView>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMenu>
#include <QSplitter>
#include <QTreeWidget>
#include <QWidgetAction>

#include <QtNodes/DagGraphicsScene>
#include <QtNodes/NodeDelegateModelRegistry>

#include "data/block_manager.hpp"
#include "data/tab_manager.hpp"

Blocks::Blocks(std::shared_ptr<BlockManager> blockManager,
               std::shared_ptr<TabManager> tabManager,
               QWidget *parent)
    : QWidget(parent)
    , m_blockManager(blockManager)
    , m_tabManager(tabManager)
    , m_nodeId(QtNodes::InvalidNodeId)
    , m_splitter(new QSplitter(Qt::Vertical))
    , m_blockViewer(new QWidget())
    , m_viewerLabel(new QLabel())
    , m_library(new QWidget())
{
    initUi();
}

void Blocks::setNodeId(QtNodes::NodeId id)
{
    if (m_nodeId == id)
        return;
    m_nodeId = id;
    emit nodeIdChanged(id);
}

void Blocks::initUi()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);
    layout->addWidget(m_splitter);

    initViewer();
    initLibrary();

    m_splitter->addWidget(m_blockViewer);
    m_splitter->addWidget(m_library);
}

void Blocks::initViewer()
{
    auto layout = new QVBoxLayout(m_blockViewer);
    layout->setAlignment(Qt::AlignTop);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_viewerLabel);

    updateFields();
    connect(this, &Blocks::nodeIdChanged, this, &Blocks::updateFields);
}

void Blocks::initLibrary()
{
    auto layout = new QVBoxLayout(m_library);
    layout->setAlignment(Qt::AlignTop);
    layout->setContentsMargins(0, 0, 0, 0);

    QMenu *modelMenu = new QMenu();

    // Add filterbox to the context menu
    auto *txtBox = new QLineEdit(modelMenu);
    txtBox->setPlaceholderText(QStringLiteral("Filter"));
    txtBox->setClearButtonEnabled(true);

    auto *txtBoxAction = new QWidgetAction(modelMenu);
    txtBoxAction->setDefaultWidget(txtBox);

    // 1.
    modelMenu->addAction(txtBoxAction);

    // Add result treeview to the context menu
    QTreeWidget *treeView = new QTreeWidget(modelMenu);
    treeView->header()->close();

    auto *treeViewAction = new QWidgetAction(modelMenu);
    treeViewAction->setDefaultWidget(treeView);

    // 2.
    modelMenu->addAction(treeViewAction);

    auto registry = BlockManager::getRegistry();

    for (auto const &cat : registry->categories()) {
        auto item = new QTreeWidgetItem(treeView);
        item->setText(0, cat);
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    }

    for (auto const &assoc : registry->registeredModelsCategoryAssociation()) {
        QList<QTreeWidgetItem *> parent = treeView->findItems(assoc.second, Qt::MatchExactly);

        if (parent.count() <= 0)
            continue;

        auto item = new QTreeWidgetItem(parent.first());
        item->setText(0, assoc.first);
    }

    treeView->expandAll();

    connect(treeView, &QTreeWidget::itemClicked, [this](QTreeWidgetItem *item, int) {
        if (!(item->flags() & (Qt::ItemIsSelectable))) {
            return;
        }
        m_tabManager->getCurrentTab()->getScene()->createNodeAt(item->text(0), {0, 0});
    });

    //Setup filtering
    connect(txtBox, &QLineEdit::textChanged, [treeView](const QString &text) {
        QTreeWidgetItemIterator categoryIt(treeView, QTreeWidgetItemIterator::HasChildren);
        while (*categoryIt)
            (*categoryIt++)->setHidden(true);
        QTreeWidgetItemIterator it(treeView, QTreeWidgetItemIterator::NoChildren);
        while (*it) {
            auto modelName = (*it)->text(0);
            const bool match = (modelName.contains(text, Qt::CaseInsensitive));
            (*it)->setHidden(!match);
            if (match) {
                QTreeWidgetItem *parent = (*it)->parent();
                while (parent) {
                    parent->setHidden(false);
                    parent = parent->parent();
                }
            }
            ++it;
        }
    });

    layout->addWidget(modelMenu);
}

void Blocks::onNodeSelected(QtNodes::NodeId id)
{
    setNodeId(id);
}

void Blocks::updateFields()
{
    auto jsonObject = m_blockManager->getJson(m_nodeId);
    if (jsonObject.isEmpty())
        m_viewerLabel->setText("No block selected");
    else
        m_viewerLabel->setText(QJsonDocument(jsonObject).toJson(QJsonDocument::Indented));
}