import Foundation
import UIKit

internal typealias Filename = String

// swiftlint:disable nesting

internal struct BeatmapInfoModel: Decodable {

    struct BeatmapSet: Decodable {

        enum SupportedCharacteristic: String, Decodable {
            case standard = "Standard"
            case oneSaber = "OneSaber"
            case noArrows = "NoArrows"

            // swiftlint:disable:next identifier_name
            case _90Degree = "360Degree"

            // swiftlint:disable:next identifier_name
            case _360Degree = "90Degree"

            case lightshow = "Lightshow"
            case lawless = "Lawless"
        }

        struct Beatmap: Decodable {

            enum DifficultyRank: Int, Comparable, Decodable {
                case easy = 1
                case normal = 3
                case hard = 5
                case expert = 7
                case expertPlus = 9

                var difficulty: BeatmapDifficulty {
                    switch self {
                    case .easy:
                        return .easy
                    case .normal:
                        return .normal
                    case .hard:
                        return .hard
                    case .expert:
                        return .expert
                    case .expertPlus:
                        return .expertPlus
                    }
                }

                static func < (lhs: DifficultyRank, rhs: DifficultyRank) -> Bool {
                    lhs.rawValue < rhs.rawValue
                }
            }

            struct CustomData: Decodable {

                struct Color: Decodable {
                    let red: Double
                    let green: Double
                    let blue: Double

                    private enum CodingKeys: String, CodingKey {
                        case red = "r"
                        case green = "g"
                        case blue = "b"
                    }

                    var uiColor: UIColor {
                        .init(red: CGFloat(red), green: CGFloat(green), blue: CGFloat(blue), alpha: 1.0)
                    }
                }

                let colorLeft: Color?
                let colorRight: Color?
                let obstacleColor: Color?

                private enum CodingKeys: String, CodingKey {
                    case colorLeft = "_colorLeft"
                    case colorRight = "_colorRight"
//                    case envColorLeft = "_envColorLeft"
//                    case envColorRight = "_envColorRight"
                    case obstacleColor = "_obstacleColor"
                }
            }

            let difficulty: String
            let difficultyRank: DifficultyRank
            let beatmapFilename: Filename
            let customData: CustomData?

            private enum CodingKeys: String, CodingKey {
                case difficulty = "_difficulty"
                case difficultyRank = "_difficultyRank"
                case beatmapFilename = "_beatmapFilename"
//                case noteJumpMovementSpeed = "_noteJumpMovementSpeed"
//                case noteJumpStartBeatOffset = "_noteJumpStartBeatOffset"
                case customData = "_customData"
            }
        }

        let beatmapCharacteristicName: SupportedCharacteristic
        let difficultyBeatmaps: [Beatmap]

        private enum CodingKeys: String, CodingKey {
            case beatmapCharacteristicName = "_beatmapCharacteristicName"
            case difficultyBeatmaps = "_difficultyBeatmaps"
        }
    }

    let version: SupportedSchemaVersion
    let songName: String
    let songSubName: String
    let songAuthorName: String
    let levelAuthorName: String
    let beatsPerMinute: UInt
    let songTimeOffset: Double // ms
    let songFilename: Filename
    let coverImageFilename: Filename
    let difficultyBeatmapSets: [BeatmapSet]

    private enum CodingKeys: String, CodingKey {
        case version = "_version"
        case songName = "_songName"
        case songSubName = "_songSubName"
        case songAuthorName = "_songAuthorName"
        case levelAuthorName = "_levelAuthorName"
        case beatsPerMinute = "_beatsPerMinute"
        case songTimeOffset = "_songTimeOffset"
//        case shuffle = "_shuffle"
//        case shufflePeriod = "_shufflePeriod"
//        case previewStartTime = "_previewStartTime"
//        case previewDuration = "_previewDuration"
        case songFilename = "_songFilename"
        case coverImageFilename = "_coverImageFilename"
//        case environmentName = "_environmentName"
//        case allDirectionsEnvironmentName = "_allDirectionsEnvironmentName"
        case difficultyBeatmapSets = "_difficultyBeatmapSets"
    }
}

// swiftlint:enable nesting
