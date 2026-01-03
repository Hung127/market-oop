import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Page {
    id: profilePage

    property string userName: "User"
    property string userEmail:  "user@test.com"
    property string userRole: "BUYER"
    property real userBalance: 1000.00
    
    signal backClicked()
    signal saveProfileClicked(string name, string email)
    signal changePasswordClicked(string oldPassword, string newPassword)
    signal logoutClicked()

    header: ToolBar {
        background: Rectangle {
            color: userRole === "BUYER" ? "#2196F3" : "#4CAF50"
        }

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 20
            anchors.rightMargin: 20
            spacing: 20

            Button {
                text: "← Back"
                font.pixelSize: 14
                flat: true
                
                contentItem: Text {
                    text:  parent.text
                    font: parent.font
                    color: "white"
                }
                
                onClicked:  profilePage.backClicked()
            }

            Label {
                text: "👤 My Profile"
                font.pixelSize: 24
                font.bold: true
                color: "white"
            }

            Item { Layout.fillWidth: true }
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "#f5f5f5"

        ScrollView {
            anchors.fill: parent
            anchors.margins: 30
            clip: true
            contentWidth: availableWidth

            ColumnLayout {
                width: Math.min(800, parent.width)
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 20

                // Profile Info Card
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 200
                    color: "white"
                    radius: 10
                    border.color: "#e0e0e0"
                    border.width: 1

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 30
                        spacing: 30

                        // Avatar
                        Rectangle {
                            Layout.preferredWidth: 120
                            Layout.preferredHeight: 120
                            color: userRole === "BUYER" ? "#2196F3" : "#4CAF50"
                            radius: 60

                            Label {
                                anchors.centerIn: parent
                                text: userName. charAt(0).toUpperCase()
                                font.pixelSize: 48
                                font.bold: true
                                color: "white"
                            }
                        }

                        // User Info
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 10

                            Label {
                                text: userName
                                font.pixelSize: 28
                                font.bold: true
                                color: "#333"
                            }

                            Label {
                                text: userEmail
                                font.pixelSize: 16
                                color: "#666"
                            }

                            Rectangle {
                                width: roleLabel.width + 20
                                height: 30
                                color: userRole === "BUYER" ? "#E3F2FD" : "#E8F5E9"
                                radius: 15

                                Label {
                                    id: roleLabel
                                    anchors.centerIn: parent
                                    text: userRole
                                    font.pixelSize: 14
                                    font.bold: true
                                    color:  userRole === "BUYER" ? "#2196F3" : "#4CAF50"
                                }
                            }

                            Label {
                                visible: userRole === "BUYER"
                                text: "Balance: $" + userBalance.toFixed(2)
                                font.pixelSize: 18
                                font.bold: true
                                color: "#4CAF50"
                            }
                        }
                    }
                }

                // Edit Profile Card
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: contentColumn1.height + 60
                    color: "white"
                    radius: 10
                    border.color: "#e0e0e0"
                    border.width: 1

                    ColumnLayout {
                        id: contentColumn1
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.margins: 30
                        spacing: 20

                        Label {
                            text: "Edit Profile"
                            font.pixelSize: 20
                            font.bold: true
                            color: "#333"
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            height: 1
                            color: "#e0e0e0"
                        }

                        // Name Field
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 8

                            Label {
                                text: "Full Name"
                                font.pixelSize: 14
                                font.bold: true
                                color: "#333"
                            }

                            TextField {
                                id: nameEditField
                                Layout.fillWidth: true
                                text: userName
                                font.pixelSize: 14
                                selectByMouse: true
                                
                                background: Rectangle {
                                    color: nameEditField.focus ? "#f0f8ff" : "#f5f5f5"
                                    border.color: nameEditField. focus ? "#2196F3" : "#ddd"
                                    border.width: 2
                                    radius: 5
                                }
                                
                                padding: 12
                            }
                        }

                        // Email Field
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing:  8

                            Label {
                                text: "Email"
                                font.pixelSize: 14
                                font.bold: true
                                color: "#333"
                            }

                            TextField {
                                id:  emailEditField
                                Layout. fillWidth: true
                                text: userEmail
                                font. pixelSize: 14
                                selectByMouse: true
                                
                                background: Rectangle {
                                    color: emailEditField.focus ? "#f0f8ff" : "#f5f5f5"
                                    border.color: emailEditField.focus ? "#2196F3" : "#ddd"
                                    border.width: 2
                                    radius: 5
                                }
                                
                                padding: 12
                            }
                        }

                        // Success/Error Message
                        Label {
                            id: profileMessage
                            text: ""
                            font.pixelSize: 14
                            Layout.fillWidth: true
                            visible: text !== ""
                        }

                        Button {
                            text: "Save Changes"
                            font.pixelSize: 14
                            font.bold: true
                            Layout.preferredWidth: 150
                            
                            contentItem: Text {
                                text: parent.text
                                font: parent.font
                                color: "white"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            
                            background: Rectangle {
                                color:  parent.pressed ? "#1976D2" : (parent.hovered ? "#1E88E5" : "#2196F3")
                                radius: 5
                            }
                            
                            Layout.preferredHeight: 45
                            
                            onClicked: {
                                profileMessage.color = "#4CAF50"
                                profileMessage.text = "Profile updated successfully!"
                                profilePage.saveProfileClicked(nameEditField.text, emailEditField.text)
                            }
                        }
                    }
                }

                // Change Password Card
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: contentColumn2.height + 60
                    color: "white"
                    radius: 10
                    border.color: "#e0e0e0"
                    border.width: 1

                    ColumnLayout {
                        id: contentColumn2
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors. top: parent.top
                        anchors.margins: 30
                        spacing: 20

                        Label {
                            text:  "Change Password"
                            font.pixelSize: 20
                            font.bold: true
                            color: "#333"
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            height: 1
                            color: "#e0e0e0"
                        }

                        // Old Password
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 8

                            Label {
                                text:  "Current Password"
                                font.pixelSize: 14
                                font.bold: true
                                color: "#333"
                            }

                            TextField {
                                id: oldPasswordField
                                Layout.fillWidth: true
                                placeholderText: "Enter current password"
                                echoMode: TextInput.Password
                                font.pixelSize: 14
                                selectByMouse: true
                                
                                background: Rectangle {
                                    color: oldPasswordField.focus ? "#f0f8ff" : "#f5f5f5"
                                    border.color: oldPasswordField.focus ? "#2196F3" : "#ddd"
                                    border. width: 2
                                    radius: 5
                                }
                                
                                padding:  12
                            }
                        }

                        // New Password
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 8

                            Label {
                                text: "New Password"
                                font.pixelSize: 14
                                font.bold: true
                                color: "#333"
                            }

                            TextField {
                                id: newPasswordField
                                Layout.fillWidth: true
                                placeholderText: "Enter new password"
                                echoMode: TextInput.Password
                                font.pixelSize: 14
                                selectByMouse: true
                                
                                background: Rectangle {
                                    color: newPasswordField.focus ? "#f0f8ff" : "#f5f5f5"
                                    border.color: newPasswordField. focus ? "#2196F3" : "#ddd"
                                    border.width: 2
                                    radius: 5
                                }
                                
                                padding: 12
                            }
                        }

                        // Confirm Password
                        ColumnLayout {
                            Layout. fillWidth: true
                            spacing: 8

                            Label {
                                text: "Confirm New Password"
                                font. pixelSize: 14
                                font.bold: true
                                color: "#333"
                            }

                            TextField {
                                id: confirmPasswordField
                                Layout.fillWidth: true
                                placeholderText:  "Confirm new password"
                                echoMode: TextInput. Password
                                font.pixelSize: 14
                                selectByMouse: true
                                
                                background: Rectangle {
                                    color: confirmPasswordField.focus ? "#f0f8ff" : "#f5f5f5"
                                    border.color: confirmPasswordField.focus ? "#2196F3" :  "#ddd"
                                    border.width: 2
                                    radius: 5
                                }
                                
                                padding: 12
                            }
                        }

                        // Password Message
                        Label {
                            id: passwordMessage
                            text: ""
                            font.pixelSize: 14
                            Layout.fillWidth: true
                            visible: text !== ""
                        }

                        Button {
                            text: "Change Password"
                            font.pixelSize: 14
                            font. bold: true
                            Layout. preferredWidth: 180
                            
                            contentItem: Text {
                                text:  parent.text
                                font: parent.font
                                color: "white"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            
                            background: Rectangle {
                                color:  parent.pressed ? "#388E3C" : (parent. hovered ? "#43A047" : "#4CAF50")
                                radius: 5
                            }
                            
                            Layout.preferredHeight: 45
                            
                            onClicked: {
                                passwordMessage.color = "#F44336"
                                
                                if (oldPasswordField.text === "") {
                                    passwordMessage.text = "Please enter current password"
                                    return
                                }
                                
                                if (newPasswordField. text === "") {
                                    passwordMessage.text = "Please enter new password"
                                    return
                                }
                                
                                if (newPasswordField.text. length < 6) {
                                    passwordMessage.text = "Password must be at least 6 characters"
                                    return
                                }
                                
                                if (newPasswordField.text !== confirmPasswordField.text) {
                                    passwordMessage.text = "Passwords do not match"
                                    return
                                }
                                
                                passwordMessage.color = "#4CAF50"
                                passwordMessage.text = "Password changed successfully!"
                                profilePage. changePasswordClicked(oldPasswordField.text, newPasswordField.text)
                                
                                // Clear fields
                                oldPasswordField.text = ""
                                newPasswordField.text = ""
                                confirmPasswordField. text = ""
                            }
                        }
                    }
                }

                // Logout Button
                Button {
                    Layout.fillWidth: true
                    text: "Logout"
                    font.pixelSize: 16
                    font.bold: true
                    
                    contentItem: Text {
                        text:  parent.text
                        font: parent.font
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    
                    background: Rectangle {
                        color:  parent.pressed ? "#D32F2F" : (parent.hovered ? "#E53935" : "#F44336")
                        radius: 5
                    }
                    
                    Layout.preferredHeight: 50
                    
                    onClicked: profilePage.logoutClicked()
                }
            }
        }
    }
}
