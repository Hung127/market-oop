import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Page {
    id: registerPage

    signal registerClicked(string name, string email, string password, string role)
    signal backToLoginClicked()
    signal registerSucceeded()

    // Connect to AuthController signals
    Connections {
        target: authController
        
        function onRegisterSuccess() {
            console.log("✓ Registration successful")
            successLabel.visible = true
            successLabel.text = "Account created successfully!  Redirecting to login..."
            
            // Clear form
            clearForm()
            
            // Go back to login after 2 seconds
            redirectTimer.start()
        }
        
        function onRegisterFailed(error) {
            console.log("✗ Registration failed:", error)
            errorLabel.text = error
            errorLabel.visible = true
        }
    }

    // Timer to redirect to login page after successful registration
    Timer {
        id: redirectTimer
        interval: 2000
        repeat: false
        onTriggered: {
            registerPage.backToLoginClicked()
        }
    }

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#4CAF50" }
            GradientStop { position: 1.0; color: "#388E3C" }
        }

        // Register Card
        Rectangle {
            width: 450
            height: 680
            anchors.centerIn: parent
            color: "white"
            radius: 10
            border.color: "#e0e0e0"
            border.width: 1

            ColumnLayout {
                anchors. fill: parent
                anchors.margins: 40
                spacing: 15

                // Title
                Label {
                    text: "Create Account"
                    font. pixelSize: 28
                    font.bold: true
                    color: "#4CAF50"
                    Layout. alignment: Qt.AlignHCenter
                }

                Label {
                    text: "Join our marketplace today"
                    font.pixelSize: 14
                    color: "#666"
                    Layout.alignment: Qt.AlignHCenter
                }

                Item { Layout.preferredHeight: 10 }

                // Name Field
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    Label {
                        text: "Full Name"
                        font.pixelSize: 14
                        color: "#333"
                    }

                    TextField {
                        id: nameField
                        Layout.fillWidth: true
                        placeholderText: "Enter your full name"
                        font.pixelSize: 14
                        selectByMouse: true
                        
                        background: Rectangle {
                            color: nameField.focus ? "#f0f8ff" : "#f5f5f5"
                            border.color: nameField.focus ? "#4CAF50" : "#ddd"
                            border.width: 2
                            radius: 5
                        }
                        
                        padding: 12
                    }
                }

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
                        placeholderText:  "Enter your email"
                        font.pixelSize: 14
                        selectByMouse:  true
                        
                        background: Rectangle {
                            color:  emailField.focus ? "#f0f8ff" : "#f5f5f5"
                            border.color: emailField.focus ? "#4CAF50" :  "#ddd"
                            border.width: 2
                            radius: 5
                        }
                        
                        padding:  12
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
                        placeholderText: "Enter your password (min 6 characters)"
                        echoMode: TextInput.Password
                        font.pixelSize: 14
                        selectByMouse: true
                        
                        background: Rectangle {
                            color: passwordField.focus ? "#f0f8ff" : "#f5f5f5"
                            border.color: passwordField.focus ? "#4CAF50" : "#ddd"
                            border.width: 2
                            radius: 5
                        }
                        
                        padding: 12
                    }
                }

                // Confirm Password Field
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    Label {
                        text: "Confirm Password"
                        font.pixelSize: 14
                        color:  "#333"
                    }

                    TextField {
                        id: confirmPasswordField
                        Layout.fillWidth: true
                        placeholderText: "Confirm your password"
                        echoMode: TextInput.Password
                        font.pixelSize: 14
                        selectByMouse: true
                        
                        background: Rectangle {
                            color: confirmPasswordField.focus ? "#f0f8ff" : "#f5f5f5"
                            border.color: confirmPasswordField.focus ? "#4CAF50" : "#ddd"
                            border.width: 2
                            radius: 5
                        }
                        
                        padding: 12
                    }
                }

                // Role Selection
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    Label {
                        text:  "I want to:"
                        font.pixelSize: 14
                        color:  "#333"
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 20

                        RadioButton {
                            id: buyerRadio
                            text: "Buy Products"
                            checked: true
                            font.pixelSize: 14
                        }

                        RadioButton {
                            id: sellerRadio
                            text: "Sell Products"
                            font.pixelSize: 14
                        }
                    }
                }

                // Success message (hidden by default)
                Label {
                    id: successLabel
                    text: ""
                    color: "#4CAF50"
                    font.pixelSize: 14
                    font.bold: true
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    visible: false
                }

                // Error message
                Label {
                    id:  errorLabel
                    text: ""
                    color: "#f44336"
                    font.pixelSize: 12
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    visible: false
                }

                Item { Layout.preferredHeight: 5 }

                // Register Button
                Button {
                    id: registerButton
                    Layout.fillWidth: true
                    text: "Create Account"
                    font.pixelSize: 16
                    font.bold: true
                    
                    contentItem: Text {
                        text: registerButton.text
                        font: registerButton.font
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment:  Text.AlignVCenter
                    }
                    
                    background: Rectangle {
                        color:  registerButton.pressed ? "#388E3C" : (registerButton.hovered ? "#43A047" : "#4CAF50")
                        radius: 5
                    }
                    
                    Layout.preferredHeight: 50
                    
                    onClicked: {
                        errorLabel.visible = false
                        errorLabel.text = ""
                        successLabel.visible = false
                        successLabel.text = ""
                        
                        // Validation
                        if (nameField.text === "") {
                            errorLabel.text = "Please enter your name"
                            errorLabel.visible = true
                            return
                        }
                        
                        if (emailField.text === "") {
                            errorLabel.text = "Please enter your email"
                            errorLabel.visible = true
                            return
                        }
                        
                        if (passwordField.text === "") {
                            errorLabel.text = "Please enter your password"
                            errorLabel.visible = true
                            return
                        }
                        
                        if (passwordField.text. length < 6) {
                            errorLabel.text = "Password must be at least 6 characters"
                            errorLabel.visible = true
                            return
                        }
                        
                        if (passwordField.text !== confirmPasswordField. text) {
                            errorLabel.text = "Passwords do not match"
                            errorLabel. visible = true
                            return
                        }
                        
                        var role = buyerRadio.checked ? "buyer" : "seller"
                        
                        console.log("Calling authController.registerUser:", nameField.text, emailField.text, role)
                        
                        // Call C++ controller
                        authController.registerUser(
                            nameField.text,
                            emailField.text,
                            passwordField.text,
                            role
                        )
                    }
                }

                // Back to Login
                Button {
                    Layout.fillWidth: true
                    text: "Already have an account? Login"
                    font.pixelSize: 14
                    
                    contentItem: Text {
                        text: parent.text
                        font: parent.font
                        color: "#4CAF50"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment:  Text.AlignVCenter
                    }
                    
                    background: Rectangle {
                        color: "transparent"
                    }
                    
                    Layout.preferredHeight: 40
                    
                    onClicked: {
                        registerPage.backToLoginClicked()
                    }
                }

                Item { Layout.fillHeight: true }
            }
        }
    }

    function clearForm() {
        nameField.text = ""
        emailField.text = ""
        passwordField.text = ""
        confirmPasswordField.text = ""
        buyerRadio.checked = true
        errorLabel.text = ""
        errorLabel.visible = false
    }
}
