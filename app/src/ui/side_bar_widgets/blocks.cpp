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

#include <QtUtility/widgets/qcollapsible_widget.hpp>

#include "data/block_manager.hpp"
#include "data/tab_manager.hpp"

using QCollapsibleWidget = QtUtility::widgets::QCollapsibleWidget;

Blocks::Blocks(std::shared_ptr<BlockManager> blockManager,
               std::shared_ptr<TabManager> tabManager,
               QWidget *parent)
    : QWidget(parent)
    , m_blockManager(blockManager)
    , m_tabManager(tabManager)
    , m_nodeId(QtNodes::InvalidNodeId)
    , m_splitter(new QSplitter(Qt::Vertical))
    , m_blockEditor(new QCollapsibleWidget("Editor"))
    , m_viewerLabel(new QLabel())
    , m_library(new QCollapsibleWidget("Library"))
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
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_splitter);

    initEditor();
    initLibrary();
    m_splitter->addWidget(m_blockEditor);
    m_splitter->addWidget(m_library);
    m_splitter->setStretchFactor(1, 1);
    m_splitter->setChildrenCollapsible(false);

    connect(m_blockEditor, &QCollapsibleWidget::contentSizeChanged, this, [this]() {
        m_splitter->setSizes({1, 1});
    });
}

void Blocks::initEditor()
{
    auto layout = new QVBoxLayout();
    layout->setAlignment(Qt::AlignTop);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_viewerLabel);

    updateFields();
    connect(this, &Blocks::nodeIdChanged, this, &Blocks::updateFields);

    m_blockEditor->setContentLayout(layout);
}

void Blocks::initLibrary()
{
    auto layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);

    // search box
    auto *searchBox = new QLineEdit();
    searchBox->setPlaceholderText(QStringLiteral("Filter"));
    searchBox->setClearButtonEnabled(true);

    layout->addWidget(searchBox);

    // tree view
    auto treeView = new QTreeWidget();
    treeView->setHeaderHidden(true);

    layout->addWidget(treeView);

    // populate tree view from registry
    auto registry = BlockManager::getRegistry();
    for (auto const &category : registry->categories()) {
        auto item = new QTreeWidgetItem(treeView);
        item->setText(0, category);
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

    // setup filtering
    connect(searchBox, &QLineEdit::textChanged, [treeView](const QString &text) {
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

    m_library->setContentLayout(layout);
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