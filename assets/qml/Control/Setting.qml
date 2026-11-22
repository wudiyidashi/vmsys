import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic as Basic
import QtQuick.Layouts

Item {
    id: page
    objectName: "settingsPage"

    readonly property color ink: "#172b42"
    readonly property color secondary: "#526579"
    readonly property color accent: "#0099ff"
    readonly property color sectionInk: "#0066a6"
    readonly property color line: "#e2e8f0"
    readonly property color fieldFill: "#f7f9fc"
    readonly property real uiScale: Math.max(1, Math.min(2.25, width / 860, height / 680))
    readonly property int gutter: scaled(width < 480 ? 12 : 20)
    readonly property int cardPadding: scaled(width < 480 ? 14 : 18)
    readonly property int gap: scaled(14)
    readonly property font bodyFont: Qt.font({ family: "Microsoft YaHei UI", pixelSize: scaled(14) })

    readonly property var diagnostics: RobotStatus.diagnostics
    readonly property string savedNumber: String(RobokitManagerd.agv_num)
    readonly property string savedServer: RobokitManagerd.status_server_ip
    readonly property bool numberValid: numberField.acceptableInput && /^[0-9]+$/.test(numberField.text)
    readonly property bool serverValid: RobokitManagerd.isValidScheduleAddress(serverField.text.trim())
    readonly property bool changed: numberField.text !== savedNumber || serverField.text.trim() !== savedServer

    function scaled(size) {
        return Math.round(size * uiScale)
    }

    function saveSettings() {
        if (!saveButton.enabled)
            return
        RobokitManagerd.setAgv_num(Number(numberField.text))
        RobokitManagerd.setStatus_server_ip(serverField.text.trim())
        numberField.text = Qt.binding(function() { return page.savedNumber })
        serverField.text = Qt.binding(function() { return page.savedServer })
    }

    function obstacleStatus() {
        if (diagnostics.blocked === undefined)
            return "—"
        if (!diagnostics.blocked)
            return qsTr("未阻挡")
        const reasons = [qsTr("超声波"), qsTr("激光"), qsTr("防跌落传感器"), qsTr("碰撞传感器"),
                         qsTr("红外"), qsTr("锁车开关"), qsTr("动态障碍物"), qsTr("虚拟激光点"),
                         qsTr("3D 相机"), qsTr("数字输入")]
        return reasons[diagnostics.block_reason] || qsTr("已阻挡")
    }

    component SectionTitle: Label {
        Layout.fillWidth: true
        color: page.sectionInk
        font.family: page.bodyFont.family
        font.pixelSize: page.scaled(17)
        font.weight: Font.DemiBold
    }

    component InfoRow: RowLayout {
        id: row
        property string title
        property string value
        property color valueColor: page.ink

        Layout.fillWidth: true
        Layout.minimumHeight: page.scaled(33)
        spacing: page.gap

        Label {
            text: row.title
            color: page.secondary
            font: page.bodyFont
        }
        Label {
            Layout.fillWidth: true
            text: row.value
            color: row.valueColor
            font: page.bodyFont
            horizontalAlignment: Text.AlignRight
            elide: Text.ElideRight
        }
    }

    component ConnectionRow: InfoRow {
        property bool connected
        value: connected ? qsTr("已连接") : qsTr("未连接")
        valueColor: connected ? "#08765e" : page.secondary
    }

    component SettingField: Basic.TextField {
        id: field
        Layout.fillWidth: true
        Layout.preferredHeight: page.scaled(48)
        font: page.bodyFont
        color: page.ink
        placeholderTextColor: page.secondary
        leftPadding: page.scaled(12)
        rightPadding: leftPadding
        selectByMouse: true
        onAccepted: page.saveSettings()

        background: Rectangle {
            radius: page.scaled(7)
            color: page.fieldFill
            border.color: field.activeFocus ? page.accent : page.line
            border.width: field.activeFocus ? 2 : 1
        }
    }

    Flickable {
        id: scroll
        anchors.fill: parent
        anchors.margins: page.gutter
        contentWidth: width
        contentHeight: dashboard.y + dashboard.height + page.scaled(8)
        flickableDirection: Flickable.VerticalFlick
        boundsBehavior: Flickable.StopAtBounds
        clip: true
        ScrollBar.vertical: Basic.ScrollBar { policy: ScrollBar.AsNeeded }

        ColumnLayout {
            id: dashboard
            objectName: "settingsContent"
            width: Math.max(0, scroll.width - page.scaled(12))
            y: page.scaled(8)
            spacing: page.scaled(16)

            Label {
                text: qsTr("设置")
                color: page.ink
                font.family: page.bodyFont.family
                font.pixelSize: page.scaled(28)
                font.weight: Font.DemiBold
                Layout.bottomMargin: page.scaled(3)
            }

            GridLayout {
                id: statusGrid
                objectName: "settingsStatusGrid"
                Layout.fillWidth: true
                columns: dashboard.width >= page.scaled(530) ? 2 : 1
                columnSpacing: page.gap
                rowSpacing: page.gap

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1
                    implicitHeight: connections.implicitHeight + 2 * page.cardPadding
                    radius: page.scaled(10)
                    color: "white"
                    border.color: page.line

                    ColumnLayout {
                        id: connections
                        anchors.fill: parent
                        anchors.margins: page.cardPadding
                        spacing: page.scaled(4)

                        SectionTitle {
                            text: qsTr("连接状态")
                            Layout.bottomMargin: page.scaled(6)
                        }
                        ConnectionRow {
                            title: qsTr("机器人网络")
                            connected: RobokitManagerd.net_connection_statue
                        }
                        ConnectionRow { title: qsTr("调度服务"); connected: RobServer.connectionState }
                        ConnectionRow { title: qsTr("电池"); connected: Bat.connectionState }
                        ConnectionRow { title: qsTr("MCU"); connected: McuBoard.connectionState }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1
                    implicitHeight: telemetry.implicitHeight + 2 * page.cardPadding
                    radius: page.scaled(10)
                    color: "white"
                    border.color: page.line

                    ColumnLayout {
                        id: telemetry
                        anchors.fill: parent
                        anchors.margins: page.cardPadding
                        spacing: page.scaled(4)

                        SectionTitle {
                            text: qsTr("机器人信息")
                            Layout.bottomMargin: page.scaled(6)
                        }
                        InfoRow { title: qsTr("机器人 IP"); value: RobotStatus.robotAddress || "—" }
                        InfoRow { title: qsTr("当前地图"); value: RobotMap.mapName || "—" }
                        InfoRow {
                            title: qsTr("定位置信度")
                            value: typeof page.diagnostics.confidence === "number"
                                   ? (page.diagnostics.confidence * 100).toFixed(1) + "%" : "—"
                        }
                        InfoRow { title: qsTr("阻挡类型"); value: page.obstacleStatus() }
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                implicitHeight: configuration.implicitHeight + 2 * page.cardPadding
                radius: page.scaled(10)
                color: "white"
                border.color: page.line

                ColumnLayout {
                    id: configuration
                    anchors.fill: parent
                    anchors.margins: page.cardPadding
                    spacing: page.scaled(12)

                    SectionTitle { text: qsTr("调度配置") }

                    GridLayout {
                        id: fields
                        objectName: "settingsFieldGrid"
                        Layout.fillWidth: true
                        columns: dashboard.width - 2 * page.cardPadding >= page.scaled(490) ? 2 : 1
                        columnSpacing: page.gap
                        rowSpacing: page.scaled(12)

                        ColumnLayout {
                            Layout.fillWidth: true
                            Layout.preferredWidth: page.scaled(135)
                            spacing: page.scaled(6)

                            Label { text: qsTr("车号"); color: page.secondary; font: page.bodyFont }
                            SettingField {
                                id: numberField
                                objectName: "vehicleNumberField"
                                text: page.savedNumber
                                validator: IntValidator { bottom: 0 }
                                inputMethodHints: Qt.ImhDigitsOnly
                                Accessible.name: qsTr("车号")
                            }
                        }
                        ColumnLayout {
                            Layout.fillWidth: true
                            Layout.preferredWidth: page.scaled(270)
                            spacing: page.scaled(6)

                            Label { text: qsTr("调度 IP"); color: page.secondary; font: page.bodyFont }
                            SettingField {
                                id: serverField
                                objectName: "dispatchAddressField"
                                text: page.savedServer
                                placeholderText: qsTr("输入 IP 地址")
                                inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
                                Accessible.name: qsTr("调度 IP")
                            }
                        }
                    }

                    Rectangle { Layout.fillWidth: true; implicitHeight: 1; color: page.line }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: page.scaled(12)

                        Label {
                            Layout.fillWidth: true
                            color: "#a12d22"
                            font: page.bodyFont
                            wrapMode: Text.Wrap
                            text: !page.numberValid ? qsTr("请输入有效车号")
                                  : !page.serverValid ? qsTr("请输入有效的调度 IP 地址") : ""
                        }
                        Basic.Button {
                            id: saveButton
                            objectName: "saveSettingsButton"
                            Layout.preferredWidth: page.scaled(114)
                            Layout.preferredHeight: page.scaled(44)
                            text: qsTr("保存设置")
                            enabled: page.changed && page.numberValid && page.serverValid
                            onClicked: page.saveSettings()
                            font: page.bodyFont
                            contentItem: Label {
                                text: saveButton.text
                                font: saveButton.font
                                color: saveButton.enabled ? "white" : page.secondary
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            background: Rectangle {
                                radius: page.scaled(7)
                                color: !saveButton.enabled ? "#edf2f7"
                                     : saveButton.down ? "#005486"
                                     : saveButton.hovered ? "#0078bb" : "#006ba8"
                                border.color: saveButton.activeFocus ? page.accent : color
                                border.width: 2
                            }
                        }
                    }
                }
            }
        }
    }
}
