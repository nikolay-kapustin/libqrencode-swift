//
//  QREncodeSwift.swift
//
//
//  Created by Nikolay Kapustin on 19.06.2020.
//

#if os(Linux)
import Glibc
#else
import Darwin
#endif

import Foundation
import libqrencode
import pngconvert

public class QREncodeSwift {
    public enum QRErrorLevel:UInt32 {case low=0, middle, high, max}
    public static func qr(from:String, level:QRErrorLevel = .middle, outputFileName:String = "qr.png", colorComponents:[Int]? = nil, caseSensivity:Bool = true) -> Bool {
        guard let cQRStr = from.cString(using: .ascii)
            else {return false}
        let output = FileManager.default.fileSystemRepresentation(withPath: outputFileName)

        let qrcode = QRcode_encodeString(cQRStr, 0, QRecLevel.init(level.rawValue), QRencodeMode.init(2), caseSensivity ? Int32(1):Int32(0))
        var result = false

        if let qr = qrcode {
            if let colors = colorComponents {
                if writePNG(qrcode, output, makeByte(from: colors.cColorComponents)) == 0 {
                    result = true
                }
            } else if writePNG(qr, output, nil) == 0 {
                result = true
            }
        }
        qrcode?.deallocate()
        return result
    }

    private static func makeByte(from array: [CUnsignedChar]?) -> UnsafeMutablePointer<UInt8>? {
        guard let arr = array else {return nil}
        let count = arr.count
        let result: UnsafeMutableBufferPointer<UInt8> = UnsafeMutableBufferPointer<UInt8>.allocate(capacity: count)
        _ = result.initialize(from: arr)
        return result.baseAddress!
    }
}

extension Collection where Element == Int{
    var cColorComponents:[CUnsignedChar]? {
        guard let red = self.first, let green = self.dropFirst().first, let blue = self.dropFirst(2).first
            else {return nil}
        guard (0...255).contains(red), (0...255).contains(green), (0...255).contains(blue)
            else {return nil}
        let r = CUnsignedChar(red)
        let g = CUnsignedChar(green)
        let b = CUnsignedChar(blue)
        return [r, g, b, CUnsignedChar(255)]
    }
}
