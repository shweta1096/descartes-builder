#pragma once

#include <QColor>
#include <QString>

namespace constants
{
    const QColor COLOR_CODER(185, 224, 165);
    const QColor COLOR_PROCESSOR(169, 196, 236);
    const QColor COLOR_TRAINER(205, 162, 190);
    const QString CONNECTION_STYLE = R"(
        {
            "ConnectionStyle": {
            "ConstructionColor": "gray",
            "NormalColor": "black",
            "SelectedColor": "gray",
            "SelectedHaloColor": "deepskyblue",
            "HoveredColor": "deepskyblue",

            "LineWidth": 3.0,
            "ConstructionLineWidth": 2.0,
            "PointDiameter": 10.0,

            "UseDataDefinedColors": true
            }
        }
        )";
}