import Foundation

internal struct BeatmapDifficultyModel: Decodable {

    enum LineIndex: Int, Decodable {
        case leftmost = 0
        case left = 1
        case right = 2
        case rightmost = 3

        var column: BeatmapColumn {
            switch self {
            case .leftmost:
                return .leftmost
            case .left:
                return .left
            case .right:
                return .right
            case .rightmost:
                return .rightmost
            }
        }
    }

    struct BeatmapNote: Decodable {

        enum CutDirection: Int, Decodable {
            case bottomToTop = 0
            case topToBottom = 1
            case rightToLeft = 2
            case leftToRight = 3
            case bottomRightToTopLeft = 4
            case bottomLeftToTopRight = 5
            case topRightToBottomLeft = 6
            case topLeftToBottomRight = 7
            case anyDirection = 8

            var direction: BeatmapDirection {
                switch self {
                case .bottomToTop:
                    return .bottomToTop
                case .topToBottom:
                    return .topToBottom
                case .rightToLeft:
                    return .rightToLeft
                case .leftToRight:
                    return .leftToRight
                case .bottomRightToTopLeft:
                    return .bottomRightToTopLeft
                case .bottomLeftToTopRight:
                    return .bottomLeftToTopRight
                case .topRightToBottomLeft:
                    return .topRightToBottomLeft
                case .topLeftToBottomRight:
                    return .topLeftToBottomRight
                case .anyDirection:
                    return .anyDirection
                }
            }
        }

        enum LineLayer: Int, Decodable {
            case bottom = 0
            case middle = 1
            case top = 2

            var row: BeatmapRow {
                switch self {
                case .bottom:
                    return .bottom
                case .middle:
                    return .middle
                case .top:
                    return .top
                }
            }
        }

        enum ObjectType: Int, Decodable {
            case redBlock = 0
            case blueBlock = 1
            case bomb = 3
        }

        let cutDirection: CutDirection
        let lineIndex: LineIndex
        let lineLayer: LineLayer
        let time: Double /* in beats */
        let type: ObjectType

        private enum CodingKeys: String, CodingKey {
            case cutDirection = "_cutDirection"
            case lineIndex = "_lineIndex"
            case lineLayer = "_lineLayer"
            case time = "_time"
            case type = "_type"
        }

        func asNoteEvent(with bpm: UInt, offset: Double) -> BeatmapNoteEvent {
            let secondsPerBeat = (bpm > 0) ? (60.0/Double(bpm)):0.0
            let seconds: TimeInterval = secondsPerBeat * time + offset/1000.0

            return .init(
                time: seconds,
                note: {
                    switch type {
                    case .bomb:
                        return .bomb
                    case .redBlock:
                        return .redBlock(cutDirection.direction)
                    case .blueBlock:
                        return .blueBlock(cutDirection.direction)
                    }
                }(),
                coordinates: .init(row: lineLayer.row, column: lineIndex.column)
            )
        }
    }

    struct BeatmapObstacle: Decodable {
        enum ObstacleType: Int, Decodable {
            case vertical = 0
            case horizontal = 1

            var direction: BeatmapObstacleEvent.Direction {
                switch self {
                case .vertical:
                    return .vertical
                case .horizontal:
                    return .horizontal
                }
            }
        }

        let duration: Double /* in beats */
        let lineIndex: LineIndex
        let time: Double /* in beats */
        let type: ObstacleType
        let width: Int

        private enum CodingKeys: String, CodingKey {
            case duration = "_duration"
            case lineIndex = "_lineIndex"
            case time = "_time"
            case type = "_type"
            case width = "_width"
        }

        func asObstacleEvent(with bpm: UInt, offset: Double) -> BeatmapObstacleEvent {
            let secondsPerBeat = (bpm > 0) ? (60.0/Double(bpm)):0.0
            let seconds: TimeInterval = secondsPerBeat * time + offset/1000.0

            return .init(
                time: seconds,
                duration: duration * secondsPerBeat,
                column: lineIndex.column,
                direction: type.direction,
                width: width
            )
        }
    }

    let version: SupportedSchemaVersion
    let notes: [BeatmapNote]
    let obstacles: [BeatmapObstacle]

    private enum CodingKeys: String, CodingKey {
        case version = "_version"
//        case events = "_events"
        case notes = "_notes"
        case obstacles = "_obstacles"
    }
}
