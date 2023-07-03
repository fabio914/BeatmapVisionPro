import Foundation
import UIKit

public struct BeatmapPreview {
    public let songName: String
    public let songSubName: String
    public let songAuthorName: String
    public let levelAuthorName: String
    public let beatsPerMinute: UInt
    public let songTimeOffset: TimeInterval
    public let coverImage: UIImage
    public let standardDifficulties: [BeatmapDifficulty]
}
