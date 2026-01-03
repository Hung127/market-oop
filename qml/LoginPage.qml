import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick. Layouts 1.12

Page {
    id: loginPage

    // Signals to emit when login/register is clicked
    signal loginClicked(string email, string password)
    signal registerClicked()
    signal loginSucceeded(string role)

    // Connect to AuthController signals
    Connections {
        target: authController
        
        function onLoginSuccess(role) {
            console.log("✓ Login success, role:", role)
            loginPage.loginSucceeded(role)
        }
        
        function onLoginFailed(error) {
            console.log("✗ Login failed:", error)
            errorLabel.text = error
        }
    }

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#2196F3" }
            GradientStop { position: 1.0; color: "#1976D2" }
        }

        // Login Card
        Rectangle {
            width: 400
            height: 500
            anchors.centerIn: parent
            color: "white"
            radius: 10
            border.color: "#e0e0e0"
            border.width: 1

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 40
                spacing: 20

                Label {
                    text: "Market App"
                    font.pixelSize: 32
                    font.bold: true
                    color: "#2196F3"
                    Layout.alignment: Qt.AlignHCenter
                }

                Label {
                    text: "Sign in to your account"
                    font. pixelSize: 14
                    color: "#666"
                    Layout.alignment: Qt.AlignHCenter
                }

                Item { Layout.preferredHeight: 20 }

                // Email Field
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    Label {
                        text: "Email"
                        font.pixelSize: 14
                        color: "#333"
                    }

                    TextField {
                        id: emailField
                        Layout.fillWidth: true
                        placeholderText: "Enter your email"
                        font.pixelSize: 14
                        selectByMouse: true
                        
                        background: Rectangle {
                            color: emailField.focus ? "#f0f8ff" : "#f5f5f5"
                            border.color: emailField.focus ? "#2196F3" : "#ddd"
                            border.width: 2
                            radius: 5
                        }
                        
                        padding: 12
                    }
                }

                // Password Field
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    Label {
                        text: "Password"
                        font.pixelSize: 14
                        color: "#333"
                    }

                    TextField {
                        id: passwordField
                        Layout.fillWidth: true
                        placeholderText: "Enter your password"
                        echoMode: TextInput.Password
                        font.pixelSize: 14
                        selectByMouse: true
                        
                        background: Rectangle {
                            color: passwordField.focus ? "#f0f8ff" : "#f5f5f5"
                            border.color: passwordField.focus ? "#2196F3" : "#ddd"
                            border.width: 2
                            radius: 5
                        }
                        
                        padding: 12

                        Keys.onReturnPressed: loginButton.clicked()
                    }
                }

                // Error message
                Label {
                    id:  errorLabel
                    text: ""
                    color: "#f44336"
                    font. pixelSize: 12
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    visible: text !== ""
                }

                Item { Layout.preferredHeight: 10 }

                // Login Button
                Button {
                    id:  loginButton
                    Layout.fillWidth: true
                    text: "Login"
                    font.pixelSize: 16
                    font.bold: true
                    
                    contentItem: Text {
                        text: loginButton.text
                        font: loginButton.font
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    
                    background: Rectangle {
                        color: loginButton.pressed ? "#1976D2" : (loginButton.hovered ? "#1E88E5" : "#2196F3")
                        radius: 5
                    }
                    
                    Layout.preferredHeight: 50
                    
                    onClicked: {
                        errorLabel.text = ""
                        
                        if (emailField.text === "") {
                            errorLabel.text = "Please enter your email"
                            return
                        }
                        
                        if (passwordField.text === "") {
                            errorLabel.text = "Please enter your password"
                            return
                        }
                        
                        // Call C++ controller
                        authController.login(emailField.text, passwordField. text)
                    }
                }

                // Divider
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 10

                    Rectangle {
                        Layout.fillWidth: true
                        height: 1
                        color: "#ddd"
                    }

                    Label {
                        text:  "OR"
                        color: "#999"
                        font.pixelSize: 12
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 1
                        color: "#ddd"
                    }
                }

                // Register Button
                Button {
                    Layout.fillWidth: true
                    text: "Create New Account"
                    font.pixelSize: 14
                    
                    contentItem: Text {
                        text:  parent.text
                        font: parent.font
                        color: "#2196F3"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    
                    background: Rectangle {
                        color: parent.pressed ? "#f0f0f0" : (parent.hovered ? "#f5f5f5" : "transparent")
                        border.color: "#2196F3"
                        border.width: 2
                        radius: 5
                    }
                    
                    Layout.preferredHeight: 45
                    
                    onClicked: {
                        loginPage.registerClicked()
                    }
                }

                Item { Layout.fillHeight: true }
            }
        }
    }

    function clearForm() {
        emailField.text = ""
        passwordField.text = ""
        errorLabel.text = ""
    }
}
