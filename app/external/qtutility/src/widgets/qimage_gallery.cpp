#include "QtUtility/widgets/qimage_gallery.hpp"

#include <QLabel>
#include <QListView>
#include <QVBoxLayout>

#include <QtUtility/data/constexpr_qstring.hpp>

#include "QtUtility/widgets/image_list_model.hpp"

using ConstLatin1String = QtUtility::data::ConstLatin1String;

namespace {
constexpr uint ICON_SIZE = 100;
constexpr uint IMAGE_WIDTH = 450;
constexpr ConstLatin1String NO_SELECTION_TEXT = "No Image Selected.";
} // namespace

namespace QtUtility {
namespace widgets {

QImageGallery::QImageGallery(QWidget *parent)
    : QWidget(parent)
    , m_images(new ImageListModel)
{
    auto layout = new QVBoxLayout(this);

    m_list = new QListView;
    m_list->setModel(m_images);
    m_list->setViewMode(QListView::IconMode);
    m_list->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    m_list->setWrapping(false);
    m_list->setFlow(QListView::LeftToRight);
    m_list->setResizeMode(QListView::Adjust);
    m_list->setFixedHeight(ICON_SIZE);

    m_viewer = new QLabel(NO_SELECTION_TEXT);

    layout->addWidget(m_list);
    layout->addWidget(m_viewer, 1);

    connect(m_list->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &QImageGallery::onImageSelected,
            Qt::QueuedConnection);
}

size_t QImageGallery::count() const
{
    return m_images->rowCount();
}

QPixmap QImageGallery::at(const size_t &index) const
{
    return m_images->at(index);
}

void QImageGallery::add(const QPixmap &image)
{
    m_images->add(image);
}

void QImageGallery::add(const QString &path)
{
    QPixmap image(path);
    if (image.isNull())
        return;
    add(image);
}

void QImageGallery::remove(const size_t &index)
{
    m_images->remove(index);
}

void QImageGallery::clear()
{
    clearViewer();
    m_images->clear();
}

void QImageGallery::clearViewer()
{
    m_viewer->clear();
    m_viewer->setText(NO_SELECTION_TEXT);
}

void QImageGallery::onImageSelected()
{
    QItemSelectionModel *selectionModel = m_list->selectionModel();
    if (selectionModel->selectedIndexes().isEmpty()) {
        clearViewer();
    } else {
        QModelIndex index = selectionModel->currentIndex();
        m_viewer->setPixmap(
            m_images->at(index.row()).scaledToWidth(IMAGE_WIDTH, Qt::SmoothTransformation));
    }
}

} // namespace widgets
} // namespace QtUtility