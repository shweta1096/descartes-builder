#include "ui/side_bar_widgets/settings.hpp"

#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>

Settings::Settings(QWidget *parent)
    : QWidget(parent)
{
    auto layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);
    layout->setContentsMargins(0, 20, 0, 0);

    // TODO: placeholder for now, not implemented yet
    auto formatBox = new QComboBox;
    formatBox->addItems({".dcb (Graph + data)", ".dag (Graph only)"});
    layout->addWidget(new QLabel("Default export format:"));
    layout->addWidget(formatBox);
}
