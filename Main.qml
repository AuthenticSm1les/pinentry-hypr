import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: window

    property var windowWidth: Math.round(fontMetrics.height * 32.2856)
    property var windowHeight: Math.round(fontMetrics.height * 15.9528) 
    property var heightSafeMargin: 15

    minimumWidth: Math.max(windowWidth, mainLayout.Layout.minimumWidth) + mainLayout.anchors.margins * 2
    minimumHeight: Math.max(windowHeight, mainLayout.Layout.minimumHeight) + mainLayout.anchors.margins * 2 + heightSafeMargin
    maximumWidth: minimumWidth
    maximumHeight: minimumHeight
    visible: true
    onClosing: {
        bridge.cancel();
    }

    FontMetrics {
        id: fontMetrics
    }

    SystemPalette {
        id: system

        colorGroup: SystemPalette.Active
    }

    Item {
        id: mainLayout

        anchors.fill: parent
        Keys.onEscapePressed: (e) => {
            bridge.cancel();
        }
        Keys.onReturnPressed: (e) => {
            bridge.submit(passwordField.text);
        }
        Keys.onEnterPressed: (e) => {
            bridge.submit(passwordField.text);
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 4

            Label {
                color: Qt.darker(system.windowText, 0.8)
                font.bold: true
                font.pointSize: Math.round(fontMetrics.height * 1.05)
                text: "Authenticating for GnuPG"
                Layout.alignment: Qt.AlignHCenter
                Layout.maximumWidth: parent.width
                elide: Text.ElideRight
                wrapMode: Text.WordWrap
            }

            HSeparator {
                Layout.topMargin: fontMetrics.height / 2
                Layout.bottomMargin: fontMetrics.height / 2
            }

            Label {
                color: system.windowText
                text: bridge.desc
                Layout.maximumWidth: parent.width
                elide: Text.ElideRight
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
            }

            // Password Field and Show Toggle on the same row
            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: fontMetrics.height / 2
                spacing: fontMetrics.height

                TextField {
                    id: passwordField
                    placeholderText: "Password"
                    hoverEnabled: true
                    persistentSelection: true
                    echoMode: showPassword.checked ? TextInput.Normal : TextInput.Password
                    focus: true
                    Layout.preferredWidth: Math.round(fontMetrics.height * 15)

                    Component.onCompleted: {
                        passwordField.forceActiveFocus();
                    }
                }

                CheckBox {
                    id: showPassword
                    text: "Show"
                    hoverEnabled: true
                    
                    contentItem: Text {
                        text: parent.text
                        font.pixelSize: Math.round(fontMetrics.height * 0.8)
                        color: system.windowText
                        leftPadding: parent.indicator.width + parent.spacing
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }

            // Remember Password below
            CheckBox {
                id: rememberCheck
                text: bridge.pwmngrLabel
                visible: bridge.allowCache
                hoverEnabled: true
                onCheckedChanged: bridge.remember = checked
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: fontMetrics.height / 4
                
                contentItem: Text {
                    text: parent.text
                    font.pixelSize: Math.round(fontMetrics.height * 0.8)
                    color: system.windowText
                    leftPadding: parent.indicator.width + parent.spacing
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Rectangle {
                color: "transparent"
                Layout.fillHeight: true
            }

            HSeparator {
                Layout.topMargin: fontMetrics.height / 2
                Layout.bottomMargin: fontMetrics.height / 2
            }

            RowLayout {
                Layout.alignment: Qt.AlignRight
                Layout.rightMargin: fontMetrics.height / 2
                spacing: 10

                Button {
                    text: "Cancel"
                    onClicked: (e) => {
                        bridge.cancel();
                    }
                }

                Button {
                    text: "Authenticate"
                    onClicked: (e) => {
                        bridge.submit(passwordField.text);
                    }
                }

            }

        }

    }

    component Separator: Rectangle {
        color: Qt.darker(window.palette.text, 1.5)
    }

    component HSeparator: Separator {
        implicitHeight: 1
        Layout.fillWidth: true
        Layout.leftMargin: fontMetrics.height * 8
        Layout.rightMargin: fontMetrics.height * 8
    }

}
