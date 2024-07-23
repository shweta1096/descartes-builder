#pragma once

#include <QColor>

#include <QtUtility/data/constexpr_qstring>

using ConstLatin1String = QtUtility::data::ConstLatin1String;

namespace constants {

// stylling constants
constexpr QColor COLOR_PRIMARY(0, 0, 0);
constexpr QColor COLOR_SECONDARY(255, 255, 255);
constexpr QColor COLOR_ACCENT(51, 153, 255);

constexpr QColor COLOR_CODER(185, 224, 165);
constexpr QColor COLOR_PROCESSOR(169, 196, 236);
constexpr QColor COLOR_TRAINER(205, 162, 190);
constexpr ConstLatin1String CONNECTION_STYLE =
    R"(
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
const ConstLatin1String GRAPHICS_VIEW_STYLE =
    R"(
        {
            "GraphicsViewStyle": {
                "BackgroundColor": [53, 53, 53],
                "FineGridColor": [60, 60, 60],
                "CoarseGridColor": [25, 25, 25]
            }
        }
    )";
const ConstLatin1String NODE_STYLE =
    R"(
        {
            "NodeStyle": {
                "NormalBoundaryColor": [255, 255, 255],
                "SelectedBoundaryColor": [255, 165, 0],
                "GradientColor0": "gray",
                "GradientColor1": [80, 80, 80],
                "GradientColor2": [64, 64, 64],
                "GradientColor3": [58, 58, 58],
                "ShadowColor": [20, 20, 20],
                "FontColor" : "white",
                "FontColorFaded" : "gray",
                "ConnectionPointColor": [169, 169, 169],
                "FilledConnectionPointColor": "cyan",
                "ErrorColor": "red",
                "WarningColor": [128, 128, 0],

                "PenWidth": 1.0,
                "HoveredPenWidth": 1.5,

                "ConnectionPointDiameter": 8.0,

                "Opacity": 0.8
            }
        }
    )";

} // namespace constants