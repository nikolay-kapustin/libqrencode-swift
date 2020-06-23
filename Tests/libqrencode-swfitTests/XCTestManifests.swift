import XCTest

#if !canImport(ObjectiveC)
public func allTests() -> [XCTestCaseEntry] {
    return [
        testCase(libqrencode_swfitTests.allTests)
    ]
}
#endif
