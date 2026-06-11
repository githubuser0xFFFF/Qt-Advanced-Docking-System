// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Particles
import fbitem

Rectangle {
    id: root
    property alias currentText: edit.text
    property alias multisample: fbitem.multisample
    property bool translucency: false

    gradient: Gradient {
        id: grad
        GradientStop { position: 0; color: "steelblue" }
        GradientStop { position: 1; color: "black" }
    }

    onTranslucencyChanged: {
        if (translucency) {
            root.color = "transparent";
            root.gradient = null;
        } else {
            root.color = "white";
            root.gradient = grad;
        }
    }

    ParticleSystem {
        anchors.fill: parent
        running: true

        ImageParticle {
            source: "qrc:///particleresources/glowdot.png"
            alpha: 0
            colorVariation: 1
        }

        Emitter {
            anchors.fill: parent
            lifeSpan: 3000
            emitRate: 30
            size: 50
            sizeVariation: 10
            velocity: PointDirection { xVariation: 10; yVariation: 10; }
            acceleration: PointDirection {
                y: -10
                xVariation: 5
                yVariation: 5
            }
        }
    }

    Rectangle {
        y: 10
        width: parent.width / 2
        height: edit.contentHeight + 4
        anchors.horizontalCenter: parent.horizontalCenter
        border.color: "gray"
        border.width: 2
        radius: 8
        color: "lightGray"
        clip: true
        TextInput {
            id: edit
            anchors.horizontalCenter: parent.horizontalCenter
            maximumLength: 30
            focus: true
            font.pointSize: 20
        }
    }

    FbItem {
        id: fbitem
        anchors.fill: parent
        SequentialAnimation on eye.y {
            loops: Animation.Infinite
            NumberAnimation {
                from: 0
                to: 0.15
                duration: 1000
            }
            NumberAnimation {
                from: 0.15
                to: 0
                duration: 2000
            }
        }
        SequentialAnimation on eye.x {
            loops: Animation.Infinite
            NumberAnimation {
                from: 0
                to: -0.5
                duration: 3000
            }
            NumberAnimation {
                from: -0.5
                to: 0.5
                duration: 3000
                easing.type: Easing.OutQuad
            }
            NumberAnimation {
                from: 0.5
                to: 0
                duration: 1000
            }
        }
        SequentialAnimation on rotation.y {
            loops: Animation.Infinite
            NumberAnimation {
                from: 0
                to: 360
                duration: 5000
            }
            NumberAnimation {
                from: 360
                to: 0
                duration: 2500
            }
        }
        SequentialAnimation on rotation.x {
            loops: Animation.Infinite
            NumberAnimation {
                from: 0
                to: 360
                duration: 6000
            }
            NumberAnimation {
                from: 360
                to: 0
                duration: 3000
            }
        }
    }

    Text {
        id: effText
        text: edit.text
        anchors.centerIn: parent
        font.pointSize: 60
        style: Text.Outline
        styleColor: "green"
    }

    ShaderEffectSource {
        id: effSource
        sourceItem: effText
        hideSource: true
    }

    ShaderEffect {
        SequentialAnimation on scale {
            loops: Animation.Infinite
            NumberAnimation { from: 1.0; to: 2.0; duration: 1000; easing.type: Easing.InCirc }
            PauseAnimation { duration: 1000 }
            NumberAnimation { from: 2.0; to: 0.5; duration: 1000; easing.type: Easing.OutExpo }
            NumberAnimation { from: 0.5; to: 1.0; duration: 500 }
            PauseAnimation { duration: 1000 }
        }
        width: effText.width
        height: effText.height
        anchors.centerIn: parent
        property variant source: effSource
        property real amplitude: 0.002
        property real frequency: 10
        property real time: 0
        NumberAnimation on time { loops: Animation.Infinite; from: 0; to: Math.PI * 2; duration: 1000 }
        fragmentShader: "wobble.frag.qsb"
    }
}
