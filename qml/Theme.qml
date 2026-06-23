import QtQuick

QtObject {
    readonly property color background: "#ffffff"
    readonly property color surface: "#f3f4f6"
    readonly property color primary: "#667eea"
    readonly property color primaryEmphasis: "#764ba2"
    readonly property color textPrimary: "#1f2937"
    readonly property color textSecondary: "#6b7280"
    readonly property color border: "#e5e7eb"

    readonly property int radiusSmall: 6
    readonly property int radiusMedium: 12
    readonly property int radiusLarge: 18

    readonly property int paddingSmall: 8
    readonly property int paddingMedium: 16
    readonly property int paddingLarge: 24

    readonly property int fontSmall: 12
    readonly property int fontBody: 14
    readonly property int fontTitle: 20

    readonly property color shadowColor: "#40000000"
    readonly property int shadowOffsetX: 0
    readonly property int shadowOffsetY: 4
    readonly property int shadowBlur: 12
}
