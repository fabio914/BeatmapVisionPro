import Foundation
import AVFoundation
import OggDecoder

struct AudioManager {

    static func load(oggSong: Data) async throws -> AVAudioPlayer {

        // TODO: Move this step to when we download the song, so we don't need to
        // convert it to WAV all the time...

        let manager = FileManager.default
        let temporaryDirectory = manager.temporaryDirectory

        if let temporaryContents = try? manager.contentsOfDirectory(at: temporaryDirectory, includingPropertiesForKeys: nil) {
            for itemUrl in temporaryContents {
                try? manager.removeItem(at: itemUrl)
            }
        }

        let uuidString = UUID().uuidString
        let temporaryDestination = temporaryDirectory.appendingPathComponent(uuidString)
        try oggSong.write(to: temporaryDestination)
        let decoder = OGGDecoder()

        let wavUUID = UUID().uuidString
        let wavTemporaryDestination = temporaryDirectory.appendingPathComponent(wavUUID)
        await decoder.decode(temporaryDestination, into: wavTemporaryDestination)

        let player = try AVAudioPlayer(contentsOf: wavTemporaryDestination, fileTypeHint: AVFileType.wav.rawValue)
        player.prepareToPlay()
        return player
    }
}
