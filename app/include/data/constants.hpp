#pragma once

#include <QColor>

#include <QtUtility/data/constexpr_qstring.hpp>

using ConstLatin1String = QtUtility::data::ConstLatin1String;

namespace constants {

// stylling constants
constexpr QColor COLOR_PRIMARY(0, 0, 0);
constexpr QColor COLOR_SECONDARY(255, 255, 255);
constexpr QColor COLOR_ACCENT(51, 153, 255);

constexpr QColor COLOR_CODER(185, 224, 165);
constexpr QColor COLOR_PROCESSOR(169, 196, 236);
constexpr QColor COLOR_TRAINER(205, 162, 190);

constexpr uint SIDE_BAR_MINIMUM_WIDTH = 250;

constexpr ConstLatin1String CONNECTION_STYLE =
    R"(
        {
            "ConnectionStyle": {
                "ConstructionColor": "gray",
                "NormalColor": "black",
                "SelectedColor": "gray",
                "SelectedHaloColor": "deepskyblue",
                "HoveredColor": "deepskyblue",

                "LineWidth": 2.0,
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
                "BackgroundColor": "white",
                "FineGridColor": "lightGray",
                "CoarseGridColor": "gray"
            }
        }
    )";
const ConstLatin1String NODE_STYLE =
    R"(
        {
            "NodeStyle": {
                "NormalBoundaryColor": "black",
                "SelectedBoundaryColor": [255, 165, 0],
                "GradientColor0": "white",
                "GradientColor1": "lightGray",
                "GradientColor2": [130, 130, 130],
                "GradientColor3": [90, 90, 90],
                "ShadowColor": [20, 20, 20],
                "FontColor" : "black",
                "FontColorFaded" : [100, 100, 100],
                "ConnectionPointColor": [169, 169, 169],
                "FilledConnectionPointColor": "yellow",
                "ErrorColor": "red",
                "WarningColor": [128, 128, 0],

                "PenWidth": 1.0,
                "HoveredPenWidth": 1.5,

                "ConnectionPointDiameter": 5.0,

                "Opacity": 0.8
            }
        }
    )";

} // namespace constants