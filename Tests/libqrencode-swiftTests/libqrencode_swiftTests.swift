import XCTest
@testable import libqrencode_swift

final class libqrencode_swfitTests: XCTestCase {

    func testQR() {
        let str = "https://apple.com"
        let colors:[Int] = [120,200,45]
        let result = QREncodeSwift.qr(from: str, level: .middle, outputFileName: "./qr.png", colorComponents:colors)
        XCTAssert(result)
    }

    static var allTests = [
        ("testQR", testQR)
    ]
}
