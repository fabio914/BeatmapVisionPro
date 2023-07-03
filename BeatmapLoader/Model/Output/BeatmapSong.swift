import UIKit

public enum BeatmapDifficulty: String, Codable, Hashable {
    case easy
    case normal
    case hard
    case expert
    case expertPlus

    public var displayName: String {
        switch self {
        case .easy:
            return "Easy"
        case .normal:
            return "Normal"
        case .hard:
            return "Hard"
        case .expert:
            return "Expert"
        case .expertPlus:
            return "Expert+"
        }
    }
}

public enum BeatmapColumn: Int {
    case leftmost = 0
    case left = 1
    case right = 2
    case rightmost = 3
}

public enum BeatmapRow: Int {
    case bottom = 0
    case middle = 1
    case top = 2
}

public struct BeatmapCoordinates {
    public let row: BeatmapRow
    public let column: BeatmapColumn
}

public enum BeatmapDirection {
    case bottomToTop
    case topToBottom
    case rightToLeft
    case leftToRight
    case bottomRightToTopLeft
    case bottomLeftToTopRight
    case topRightToBottomLeft
    case topLeftToBottomRight
    case anyDirection

    public var angle: Float {
        switch self {
        case .bottomToTop:
            return 180.0
        case .topToBottom:
            return 0.0
        case .rightToLeft:
            return -90.0
        case .leftToRight:
            return 90.0
        case .bottomRightToTopLeft:
            return -135.0
        case .bottomLeftToTopRight:
            return 135.0
        case .topRightToBottomLeft:
            return -45.0
        case .topLeftToBottomRight:
            return 45.0
        default:
            return 0.0
        }
    }
}

public enum BeatmapNote {
    case bomb
    case redBlock(BeatmapDirection)
    case blueBlock(BeatmapDirection)

    public var isBomb: Bool {
        if case .bomb = self { return true } else { return false }
    }
}

public struct BeatmapNoteEvent {
    public let time: TimeInterval
    public let note: BeatmapNote
    public let coordinates: BeatmapCoordinates

    public func isContainedBy(_ range: ClosedRange<TimeInterval>) -> Bool {
        range.contains(time)
    }
}

public struct BeatmapObstacleEvent {
    public enum Direction {
        case horizontal
        case vertical
    }

    public let time: TimeInterval
    public let duration: TimeInterval
    public let column: BeatmapColumn
    public let direction: Direction
    public let width: Int

    public func isContainedBy(_ range: ClosedRange<TimeInterval>) -> Bool {
        range.overlaps(time ... (time + max(0, duration)))
    }
}

public struct BeatmapColors {
    public let leftColor: UIColor?
    public let rightColor: UIColor?
    public let obstacleColor: UIColor?
}

public struct BeatmapSongDifficulty {
    public let name: String
    public let difficulty: BeatmapDifficulty
    public let notes: [BeatmapNoteEvent]
    public let obstacles: [BeatmapObstacleEvent]
    public let colors: BeatmapColors?

    public let noteCount: Int
    public let bombCount: Int
    public let wallCount: Int

    init(
        name: String,
        difficulty: BeatmapDifficulty,
        notes: [BeatmapNoteEvent],
        obstacles: [BeatmapObstacleEvent],
        colors: BeatmapColors?
    ) {
        self.name = name
        self.difficulty = difficulty
        self.notes = notes
        self.obstacles = obstacles
        self.colors = colors

        self.bombCount = notes.filter({ $0.note.isBomb }).count
        self.noteCount = notes.count - bombCount
        self.wallCount = obstacles.count
    }

    public struct Slice {
        public let notes: [BeatmapNoteEvent]
        public let obstacles: [BeatmapObstacleEvent]
    }

    public func slice(for range: ClosedRange<TimeInterval>) -> Slice {
        // TODO: Replace this with a Range Search Tree
        .init(notes: notes.filter({ $0.isContainedBy(range) }), obstacles: obstacles.filter({ $0.isContainedBy(range) }))
    }
}

public struct BeatmapSong {
    public let preview: BeatmapPreview
    public let song: Data
    public let standardDifficulties: [BeatmapSongDifficulty]
}
