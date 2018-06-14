#!/bin/sh

pod install

xcodebuild \
    -workspace RiksSamplesMQTTSwift.xcworkspace \
    -scheme RiksSamplesMQTTSwift \
    -derivedDataPath build
