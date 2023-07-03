import Foundation
import RealityKit
import RealityKitContent
import BeatmapLoader

final class SceneManager: ObservableObject {
    let anchor: Entity
    private let songRoot: Entity
    private let notes: Entity
    private let walls: Entity
    private let visibleSeconds = 1.5
    private let distancePerSecond: Double

    private let rootOriginPosition = SIMD3<Float>(x: -0.375, y: 1.0, z: -1.0)

    enum SceneManagerError: Error {
        case missingDifficulty
        case missingEntity
    }

    @MainActor
    init(url: URL, difficulty: BeatmapDifficulty) async throws {
        let dataSource = ZIPBeatmapLoaderDataSource(with: url)
        let loader = BeatmapLoader(dataSource: dataSource)
        let map = try loader.loadMap()

        guard let songDifficulty = map.standardDifficulties.first(where: { $0.difficulty == difficulty }) else {
            throw SceneManagerError.missingDifficulty
        }

        let sceneEntity = try await Entity(named: "Scene", in: realityKitContentBundle)

        guard let blueCube = sceneEntity.findEntity(named: "Blue"),
              let redCube = sceneEntity.findEntity(named: "Red"),
              let blueAnyCube = sceneEntity.findEntity(named: "Blue_Any_Direction"),
              let redAnyCube = sceneEntity.findEntity(named: "Red_Any_Direction"),
              let bomb = sceneEntity.findEntity(named: "Bomb"),
              let line = sceneEntity.findEntity(named: "Line"),
              let wall = sceneEntity.findEntity(named: "Wall")
        else {
            throw SceneManagerError.missingEntity
        }

        let beatsPerSecond = Double(map.preview.beatsPerMinute)/60.0
        let distancePerBeat = 4.0
        self.distancePerSecond = distancePerBeat * beatsPerSecond

        // FIXME!
        let anchor = Entity() //AnchorEntity(.plane(.horizontal, classification: .floor, minimumBounds: [0.5, 0.5]))

        let lineClone = line.clone(recursive: false)
        lineClone.position = .init(0, rootOriginPosition.y - 0.125, rootOriginPosition.z)
        anchor.addChild(lineClone)

        let songRoot = Entity()
        songRoot.position = rootOriginPosition

        let notes = Entity()
        let walls = Entity()

        songRoot.addChild(notes)
        songRoot.addChild(walls)

        for noteEvent in songDifficulty.notes {
            let baseEntity: Entity

            switch noteEvent.note {
            case .blueBlock(.anyDirection):
                baseEntity = blueAnyCube.clone(recursive: true)
            case .blueBlock:
                baseEntity = blueCube.clone(recursive: true)
            case .redBlock(.anyDirection):
                baseEntity = redAnyCube.clone(recursive: true)
            case .redBlock:
                baseEntity = redCube.clone(recursive: true)
            case .bomb:
                baseEntity = bomb.clone(recursive: true)
            }

            let noteEntity = NoteEntity(noteEvent: noteEvent, distancePerSecond: distancePerSecond, child: baseEntity)
            noteEntity.isEnabled = false

            notes.addChild(noteEntity)
        }

        for obstacleEvent in songDifficulty.obstacles {
            let obstacle = ObstacleEntity(
                obstacleEvent: obstacleEvent,
                distancePerSecond: distancePerSecond,
                child: wall.clone(recursive: true)
            )

            obstacle.isEnabled = false
            walls.addChild(obstacle)
        }

        anchor.addChild(songRoot)

        self.anchor = anchor
        self.songRoot = songRoot
        self.notes = notes
        self.walls = walls
    }

    @MainActor
    func update(elapsedTime: TimeInterval) {
        songRoot.position.z = Float(elapsedTime * distancePerSecond) + rootOriginPosition.z

        let visibleRange = (elapsedTime - visibleSeconds) ... (elapsedTime + visibleSeconds)

        notes.children
            .compactMap({ $0 as? NoteEntity })
            .forEach({ $0.isEnabled = $0.noteEvent.isContainedBy(visibleRange) })

        walls.children
            .compactMap({ $0 as? ObstacleEntity })
            .forEach({ $0.isEnabled = $0.obstacleEvent.isContainedBy(visibleRange) })
    }
}

final class NoteEntity: Entity {
    let noteEvent: BeatmapNoteEvent

    @MainActor
    init(noteEvent: BeatmapNoteEvent, distancePerSecond: Double, child: Entity) {
        self.noteEvent = noteEvent
        super.init()

        let direction: BeatmapDirection? = {
            switch noteEvent.note {
            case .blueBlock(let direction):
                return direction
            case .redBlock(let direction):
                return direction
            default:
                return nil
            }
        }()

        let coordinates = noteEvent.coordinates
        let noteTime = noteEvent.time

        self.transform.translation = .init(
            x: Float(coordinates.column.rawValue) * 0.25,
            y: Float(coordinates.row.rawValue) * 0.25,
            z: -1.0 * Float(noteTime * distancePerSecond)
        )

        let rotation = (direction?.angle ?? 0.0) * .pi/180.0
        self.transform.rotation = .init(angle: rotation, axis: .init(x: 0, y: 0, z: 1))

        child.transform.translation = .zero
        self.addChild(child)
    }

    @MainActor required init() {
        fatalError("init() has not been implemented")
    }
}

final class ObstacleEntity: Entity {
    let obstacleEvent: BeatmapObstacleEvent

    @MainActor
    init(obstacleEvent: BeatmapObstacleEvent, distancePerSecond: Double, child: Entity) {
        self.obstacleEvent = obstacleEvent
        super.init()

        let width = Float(obstacleEvent.width) * 0.25

        let row: Float = {
            switch obstacleEvent.direction {
            case .vertical:
                return 0.375
            case .horizontal:
                return 0.5
            }
        }()

        let length = Float(obstacleEvent.duration * distancePerSecond)

        let height: Float = {
            switch obstacleEvent.direction {
            case .vertical:
                return 1.0
            case .horizontal:
                return 0.5
            }
        }()

        self.position = .init(
            x: Float(obstacleEvent.column.rawValue) * 0.25 + (width - 0.25)/2.0,
            y: row,
            z: -Float(obstacleEvent.time * distancePerSecond) - length/2.0
        )

        child.transform.translation = .zero
        child.transform.scale = .init(x: width, y: height, z: length)
        self.addChild(child)

        let frontBottomLeft = SIMD3<Float>(-width/2.0, -height/2.0, length/2.0)
        let frontBottomRight = SIMD3<Float>(width/2.0, -height/2.0, length/2.0)
        let frontTopLeft = SIMD3<Float>(-width/2.0, height/2.0, length/2.0)
        let frontTopRight = SIMD3<Float>(width/2.0, height/2.0, length/2.0)

        let backBottomLeft = SIMD3<Float>(-width/2.0, -height/2.0, -length/2.0)
        let backBottomRight = SIMD3<Float>(width/2.0, -height/2.0, -length/2.0)
        let backTopLeft = SIMD3<Float>(-width/2.0, height/2.0, -length/2.0)
        let backTopRight = SIMD3<Float>(width/2.0, height/2.0, -length/2.0)

        let meshResource = MeshResource.generateBox(size: 1.0)
        let material = UnlitMaterial(color: .white)
        let boxEntity = ModelEntity(mesh: meshResource, materials: [material])

        self.addChild(makeLine(from: frontBottomLeft, to: frontBottomRight, reference: boxEntity))
        self.addChild(makeLine(from: frontBottomRight, to: frontTopRight, reference: boxEntity))
        self.addChild(makeLine(from: frontTopRight, to: frontTopLeft, reference: boxEntity))
        self.addChild(makeLine(from: frontTopLeft, to: frontBottomLeft, reference: boxEntity))

        self.addChild(makeLine(from: backBottomLeft, to: backBottomRight, reference: boxEntity))
        self.addChild(makeLine(from: backBottomRight, to: backTopRight, reference: boxEntity))
        self.addChild(makeLine(from: backTopRight, to: backTopLeft, reference: boxEntity))
        self.addChild(makeLine(from: backTopLeft, to: backBottomLeft, reference: boxEntity))

        self.addChild(makeLine(from: frontBottomLeft, to: backBottomLeft, reference: boxEntity))
        self.addChild(makeLine(from: frontBottomRight, to: backBottomRight, reference: boxEntity))
        self.addChild(makeLine(from: frontTopRight, to: backTopRight, reference: boxEntity))
        self.addChild(makeLine(from: frontTopLeft, to: backTopLeft, reference: boxEntity))
    }

    @MainActor required init() {
        fatalError("init() has not been implemented")
    }

    private func makeLine(from positionA: SIMD3<Float>, to positionB: SIMD3<Float>, reference: Entity) -> Entity {

        let diameter: Float = 0.01
        let vector = SIMD3<Float>(positionA.x - positionB.x, positionA.y - positionB.y, positionA.z - positionB.z)
        let distance = sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z)

        let midPosition = SIMD3<Float>(
            (positionA.x + positionB.x)/2.0,
            (positionA.y + positionB.y)/2.0,
            (positionA.z + positionB.z)/2.0
        )

        let entity = reference.clone(recursive: false)
        entity.position = midPosition
        entity.look(at: positionB, from: midPosition, relativeTo: nil)

        entity.scale = .init(x: diameter, y: diameter, z: (diameter + distance))
        return entity
    }
}
