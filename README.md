# libqrencode-swfit

![Swift 5.2+](https://img.shields.io/badge/Swift-5.2%2B-orange.svg)
![Xcode 11.0+](https://img.shields.io/badge/Xcode-11%2B-blue.svg)
![Swift Package](https://img.shields.io/badge/Dependency%20Manager-Swift%20PM-orange)
[![License](https://img.shields.io/badge/license-MIT-green)](/LICENSE)

The libqrencode-swfit is tiny Swift library for quick and simple generating QR codes. The main goal of creating this wrapper is using lib on the server side. If you backed form iOS or MacOS developing, you probably know that QR code generation is trivial. Unfortunately, we can't using powerful Core Graphics frameworks on server side in Swift. So if you move on to server side code, that way is not applicable.

![Swift Package](https://swift.org/assets/images/swift.svg)

## Install
To integrate library into your code, use SPM

```swift
...
dependencies: [
.package(path: "https://github.com/digital-horizon/libqrencode-swfit")
]
...
```
The library works well in Mac OS and Linux – anywhere the Swift toolchain is available.

## Using

```swift
let str = "https://apple.com"
let colors:[Int] = [120,200,45] // you can optionally set color via components (or pass nil to black by default)

let result = QREncodeSwift.qr(from: str, level: .middle, outputFileName: "qr.png", colorComponents:nil)

if result == true {
    // check qr.png file here
}
```
And viola!

![QR](https://github.com/digital-horizon/libqrencode-swfit/blob/master/qr.png?raw=true)

## License
The libqrencode-swfit is released under the [MIT License](LICENSE).
