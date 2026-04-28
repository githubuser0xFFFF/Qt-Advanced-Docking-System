// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#version 440

layout(location = 0) in vec2 qt_TexCoord0;
layout(location = 0) out vec4 fragColor;

layout(binding = 1) uniform sampler2D source;

layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    float qt_Opacity;
    float amplitude;
    float frequency;
    float time;
};

void main()
{
    vec2 p = sin(time + frequency * qt_TexCoord0);
    fragColor = texture(source, qt_TexCoord0 + amplitude * vec2(p.y, -p.x)) * qt_Opacity;
}
