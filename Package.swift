// swift-tools-version:5.2
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
    name: "libqrencode-swift",
    products: [
        // Products define the executables and libraries produced by a package, and make them visible to other packages.
    .library(name: "pngconvert", targets: ["pngconvert"]),
        .library(
            name: "libqrencode-swift",
            targets: ["libqrencode-swift"]),
    ],
    dependencies: [
    ],
    targets: [
        .systemLibrary(name: "libqrencode", pkgConfig: "libqrencode", providers: [.apt(["libqrencode-dev"]), .brew(["libqrencode"])]),
        .systemLibrary(name: "libpng", pkgConfig: "libpng", providers: [.apt(["libpng16-dev"]), .brew(["libpng"])]),
        .target(name: "pngconvert", dependencies:["libpng" /*, "libqrencode"*/]),
        .target(
            name: "libqrencode-swift",
            dependencies: ["libqrencode", "pngconvert"]),
        .testTarget(
            name: "libqrencode-swiftTests",
            dependencies: [.target(name: "libqrencode-swift")]
        ),
    ]
)
