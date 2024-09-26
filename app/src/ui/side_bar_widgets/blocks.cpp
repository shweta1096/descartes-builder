#include "ui/side_bar_widgets/blocks.hpp"

#include <QComboBox>
#include <QFormLayout>
#include <QGraphicsItem>
#include <QHeaderView>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMenu>
#include <QSpinBox>
#include <QStackedWidget>
#include <QTreeWidget>
#include <QWidgetAction>

#include <QtNodes/DagGraphicsScene>
#include <QtNodes/GraphicsView>
#include <QtNodes/NodeDelegateModelRegistry>

#include <QtUtility/widgets/qcollapsible_widget.hpp>

#include "data/block_manager.hpp"
#include "data/constants.hpp"
#include "data/tab_manager.hpp"
#include "ui/models/fdf_block_model.hpp"
#include "ui/models/function_names.hpp"
#include "ui/models/trainer_models.hpp"

using QCollapsibleWidget = QtUtility::widgets::QCollapsibleWidget;

namespace {
constexpr uint ADD_BLOCK_SPACING = 20;
constexpr uint FUNCTION_ROW = 1;
constexpr uint INPUT_PORT_ROW = 3;
constexpr uint OUTPUT_PORT_ROW = 4;
constexpr uint TRAINER_INPUT_ROW = 5;
constexpr uint TRAINER_OUTPUT_ROW = 6;
constexpr uint PARAMETER_ROW = 7;
} // namespace

Blocks::Blocks(std::shared_ptr<BlockManager> blockManager,
               std::shared_ptr<TabManager> tabManager,
               QWidget *parent)
    : QWidget(parent)
    , m_blockManager(blockManager)
    , m_tabManager(tabManager)
    , m_nodeId(QtNodes::InvalidNodeId)
    , m_blockEditor(new QCollapsibleWidget("Editor"))
    , m_idEdit(new QLineEdit)
    , m_captionEdit(new QLineEdit)
    , m_functionNameEdit(new QLineEdit)
    , m_inputPortEdit(new QSpinBox)
    , m_outputPortEdit(new QSpinBox)
    , m_trainerInputEdit(new QSpinBox)
    , m_trainerOutputEdit(new QSpinBox)
    , m_parametersWidget(new QStackedWidget)
    , m_library(new QCollapsibleWidget("Library"))
{
    initUi();

    connect(m_blockManager.get(), &BlockManager::nodeSelected, this, &Blocks::setNodeId);
}

void Blocks::setNodeId(QtNodes::NodeId id)
{
    if (m_nodeId == id)
        return;
    m_nodeId = id;
    emit nodeIdChanged(id);
}

void Blocks::updateFields()
{
    blockEditorSignals(true);
    auto block = m_blockManager->getBlock(m_nodeId);
    enableEditorWidgets(block);
    handleTrainerBlock(block);
    if (auto widget = m_parametersWidget->currentWidget()) {
        m_parametersWidget->removeWidget(widget);
        widget->deleteLater();
    }
    if (!block) {
        m_idEdit->clear();
        m_captionEdit->clear();
        m_functionNameEdit->clear();
        m_inputPortEdit->clear();
        m_outputPortEdit->clear();
    } else {
        // fill the fields
        m_idEdit->setText(QString::number(m_nodeId));
        m_functionNameEdit->setText(block->functionName());
        m_captionEdit->setText(block->caption());
        m_inputPortEdit->setMinimum(
            block->minModifiablePorts(PortType::In, constants::DATA_PORT_ID));
        m_inputPortEdit->setValue(block->nPorts(PortType::In, constants::DATA_PORT_ID));
        m_inputPortEdit->setEnabled(block->portNumberModifiable(PortType::In));
        m_outputPortEdit->setMinimum(
            block->minModifiablePorts(PortType::Out, constants::DATA_PORT_ID));
        m_outputPortEdit->setValue(block->nPorts(PortType::Out, constants::DATA_PORT_ID));
        m_outputPortEdit->setEnabled(block->portNumberModifiable(PortType::Out));

        if (auto parameterWidget = generateParameterWidget(block))
            m_parametersWidget->addWidget(parameterWidget);
    }
    m_editorLayout->setRowVisible(FUNCTION_ROW, block && !block->functionName().isEmpty());
    m_editorLayout->setRowVisible(PARAMETER_ROW, m_parametersWidget->currentWidget());

    blockEditorSignals(false);
}

void Blocks::onLibraryItemClicked(QTreeWidgetItem *item)
{
    if (!(item->flags() & (Qt::ItemIsSelectable))) {
        return;
    }
    auto view = m_tabManager->getCurrentTab()->getView();
    auto pos = view->mapToScene(view->rect().center()).toPoint();
    auto scene = m_tabManager->getCurrentTab()->getScene();
    while (auto exisitingItem = scene->itemAt(pos, QTransform())) {
        // sometime will add to a previous location due to itemAt only return 1 item at the pos even though another item may also be there
        if (exisitingItem->pos() != pos)
            break;
        pos += QPoint(ADD_BLOCK_SPACING, ADD_BLOCK_SPACING);
    }
    scene->createNodeAt(item->text(0), pos);
}

void Blocks::onNodeUpdated(QtNodes::NodeId id)
{
    if (id != m_nodeId)
        return;
    updateFields();
}

void Blocks::initUi()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);
    layout->setContentsMargins(0, 0, 0, 0);

    initEditor();
    initLibrary();
    layout->addWidget(m_blockEditor);
    layout->addWidget(m_library);
}

void Blocks::initEditor()
{
    auto formWidget = new QWidget();
    m_editorLayout = new QFormLayout(formWidget);
    m_editorLayout->setContentsMargins(0, 0, 0, 0);
    m_editorLayout->addRow(new QLabel("Id:"), m_idEdit);
    m_idEdit->setDisabled(true);
    m_editorLayout->addRow(new QLabel("Function:"), m_functionNameEdit);
    m_editorLayout->setRowVisible(FUNCTION_ROW, false);
    m_functionNameEdit->setDisabled(true);
    m_editorLayout->addRow(new QLabel("Caption:"), m_captionEdit);

    m_inputPortEdit->setRange(0, constants::MAX_DATA_INPUT_PORTS);
    m_inputPortEdit->setMaximumWidth(constants::INT_SPIN_BOX_MAX_WIDTH);
    m_outputPortEdit->setRange(0, constants::MAX_DATA_OUTPUT_PORTS);
    m_outputPortEdit->setMaximumWidth(constants::INT_SPIN_BOX_MAX_WIDTH);
    m_editorLayout->addRow(new QLabel("Input Ports:"), m_inputPortEdit);
    m_editorLayout->addRow(new QLabel("Output Ports:"), m_outputPortEdit);

    m_trainerInputEdit->setRange(1, constants::MAX_DATA_INPUT_PORTS);
    m_trainerInputEdit->setMaximumWidth(constants::INT_SPIN_BOX_MAX_WIDTH);
    m_trainerOutputEdit->setRange(1, constants::MAX_DATA_OUTPUT_PORTS);
    m_trainerOutputEdit->setMaximumWidth(constants::INT_SPIN_BOX_MAX_WIDTH);
    m_editorLayout->addRow(new QLabel("Trainer Inputs:"), m_trainerInputEdit);
    m_editorLayout->addRow(new QLabel("Trainer Outputs:"), m_trainerOutputEdit);
    m_editorLayout->setRowVisible(TRAINER_INPUT_ROW, false);
    m_editorLayout->setRowVisible(TRAINER_OUTPUT_ROW, false);

    m_editorLayout->addRow(new QLabel("Parameters"));
    m_editorLayout->setRowVisible(PARAMETER_ROW, false);
    m_editorLayout->addRow(m_parametersWidget);
    m_blockEditor->setWidget(formWidget);

    // these will be blocked during updateFields()
    m_blockableEditorWidgets = {m_idEdit,
                                m_functionNameEdit,
                                m_captionEdit,
                                m_inputPortEdit,
                                m_outputPortEdit,
                                m_trainerInputEdit,
                                m_trainerOutputEdit};
    // these will be enabled/disabled after updateFields();
    m_editableEditorWidgets = {m_captionEdit, m_inputPortEdit, m_outputPortEdit};

    // init initial disabled state
    updateFields();

    connect(this, &Blocks::nodeIdChanged, this, &Blocks::updateFields);
    connect(m_blockManager.get(), &BlockManager::nodeUpdated, this, &Blocks::onNodeUpdated);

    connect(m_captionEdit, &QLineEdit::textChanged, this, [this](QString text) {
        m_blockManager->getBlock(m_nodeId)->setCaption(text);
    });
    connect(m_inputPortEdit, &QSpinBox::valueChanged, this, [this](int value) {
        m_blockManager->getBlock(m_nodeId)->setInputPortNumber(value);
    });
    connect(m_outputPortEdit, &QSpinBox::valueChanged, this, [this](int value) {
        m_blockManager->getBlock(m_nodeId)->setOutputPortNumber(value);
    });
    connect(m_trainerInputEdit, &QSpinBox::valueChanged, this, [this](int value) {
        if (auto trainer = dynamic_cast<TrainerModel *>(m_blockManager->getBlock(m_nodeId)))
            trainer->setTrainerInputNumber(value);
    });
    connect(m_trainerOutputEdit, &QSpinBox::valueChanged, this, [this](int value) {
        if (auto trainer = dynamic_cast<TrainerModel *>(m_blockManager->getBlock(m_nodeId)))
            trainer->setTrainerOutputNumber(value);
    });
}

void Blocks::initLibrary()
{
    auto widget = new QWidget();
    auto layout = new QVBoxLayout(widget);
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

    connect(treeView, &QTreeWidget::itemClicked, this, &Blocks::onLibraryItemClicked);

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

    m_library->setWidget(widget);
}

void Blocks::blockEditorSignals(bool value)
{
    for (auto widget : m_blockableEditorWidgets)
        widget->blockSignals(value);
}

void Blocks::enableEditorWidgets(bool value)
{
    for (auto widget : m_editableEditorWidgets)
        widget->setEnabled(value);
}

QWidget *Blocks::generateParameterWidget(FdfBlockModel *block)
{
    if (!block)
        return nullptr;
    auto values = block->getParameters();
    if (values.empty())
        return nullptr;
    auto widget = new QWidget;
    auto layout = new QFormLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    for (auto &pair : block->getParameterSchema()) {
        auto key = pair.first;
        // value is not found for now, we need to decide how to add optional params
        if (values.count(key) < 1)
            continue;
        auto value = values.at(key);
        if (pair.second == QMetaType::QString) {
            auto options = block->getParameterOptions(key);
            if (options.isEmpty()) {
                auto edit = new QLineEdit(value);
                layout->addRow(new QLabel(key), edit);
                connect(edit, &QLineEdit::textChanged, block, [block, key](const QString &text) {
                    block->setParameter(key, text);
                });
            } else {
                auto comboBox = new QComboBox;
                comboBox->addItems(options);
                comboBox->setCurrentText(value);
                layout->addRow(new QLabel(key), comboBox);
                connect(comboBox,
                        &QComboBox::currentTextChanged,
                        block,
                        [block, key](const QString &text) { block->setParameter(key, text); });
            }
        } else if (pair.second == QMetaType::Int) {
            auto spin = new QSpinBox;
            spin->setRange(std::numeric_limits<int>::lowest(), std::numeric_limits<int>::max());
            spin->setMaximumWidth(constants::INT_SPIN_BOX_MAX_WIDTH);
            spin->setValue(value.toInt());
            layout->addRow(new QLabel(key), spin);
            connect(spin, &QSpinBox::valueChanged, block, [block, key](const int &value) {
                block->setParameter(key, QString::number(value));
            });
        } else if (pair.second == QMetaType::QPoint) {
            auto pointLayout = new QHBoxLayout();
            {
                int xValue = value.mid(value.indexOf('[') + 1, value.indexOf(',') - 1).toInt();
                int yValue = value.mid(value.indexOf(' ') + 1, value.indexOf(']') - 1).toInt();
                pointLayout->setContentsMargins(0, 0, 0, 0);
                pointLayout->setSpacing(0);
                auto xSpin = new QSpinBox;
                xSpin->setRange(0, std::numeric_limits<int>::max());
                xSpin->setMaximumWidth(constants::INT_SPIN_BOX_MAX_WIDTH);
                xSpin->setValue(xValue);
                pointLayout->addWidget(xSpin);
                pointLayout->addWidget(new QLabel(", "));
                auto ySpin = new QSpinBox;
                ySpin->setRange(0, std::numeric_limits<int>::max());
                ySpin->setMaximumWidth(constants::INT_SPIN_BOX_MAX_WIDTH);
                ySpin->setValue(yValue);
                pointLayout->addWidget(ySpin);
                connect(xSpin, &QSpinBox::valueChanged, block, [block, key, ySpin](const int &value) {
                    block->setParameter(key,
                                        QString("[%1, %2]")
                                            .arg(QString::number(value),
                                                 QString::number(ySpin->value())));
                });
                connect(ySpin, &QSpinBox::valueChanged, block, [block, key, xSpin](const int &value) {
                    block->setParameter(key,
                                        QString("[%1, %2]")
                                            .arg(QString::number(xSpin->value()), value));
                });
            }
            layout->addRow(new QLabel(key), pointLayout);
        } else if (pair.second == QMetaType::QVector2D) {
            // UI for this can be improved
            auto edit = new QLineEdit(value);
            layout->addRow(new QLabel(key), edit);
            connect(edit, &QLineEdit::textChanged, block, [block, key](const QString &text) {
                block->setParameter(key, text);
            });
        } else {
            qCritical() << "Block parameter type is unhandled" << pair.second;
        }
    }
    return widget;
}

void Blocks::handleTrainerBlock(FdfBlockModel *block)
{
    bool isTrainer = block && block->functionName() == trainer_function::BASIC_TRAINER;
    m_editorLayout->setRowVisible(INPUT_PORT_ROW, !isTrainer);
    m_editorLayout->setRowVisible(OUTPUT_PORT_ROW, !isTrainer);
    m_editorLayout->setRowVisible(TRAINER_INPUT_ROW, isTrainer);
    m_editorLayout->setRowVisible(TRAINER_OUTPUT_ROW, isTrainer);
    if (auto trainer = dynamic_cast<TrainerModel *>(block)) {
        m_trainerInputEdit->setValue(trainer->getTrainerInputPortNum());
        m_trainerOutputEdit->setValue(trainer->getTrainerOutputPortNum());
    }
}
