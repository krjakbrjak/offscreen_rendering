import QtQuick 2
import QtQuick.Controls 2

Item {
    width: 400
    height: 300

    Column {
        anchors.fill: parent
        spacing: 10

        Rectangle {
            width: parent.width
            height: 50
            color: "lightblue"
            Text {
                anchors.centerIn: parent
                text: "Welcome!"
                font.pixelSize: 20
            }
        }

        Button {
            text: "Click Me"
            onClicked: {
                console.log("Button clicked!")
            }
        }

        CheckBox {
            text: "Check me"
            checked: true
            onCheckedChanged: {
                console.log("Checkbox checked:", checked)
            }
        }

        Slider {
            width: parent.width
            from: 0
            to: 100
            value: 50
            onValueChanged: {
                console.log("Slider value changed:", value)
            }
        }
    }
}
